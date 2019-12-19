#include "ArmyProceduralMeshRender.h"
#include "ArmyProceduralMeshComponent.h"
#include "Engine/MapBuildDataRegistry.h"

FPrimitiveSceneProxy* UXRProceduralMeshComponent::CreateSceneProxy()
{
	FPrimitiveSceneProxy* Proxy = nullptr;
	if (ProcMeshSections.Num() > 0)
	{
		Proxy = new FArmyProceduralMeshSceneProxy(this);
	}
	return Proxy;
}

FArmyProceduralMeshSceneProxy::FArmyProceduralMeshSceneProxy(UXRProceduralMeshComponent* InComponent)
	:FPrimitiveSceneProxy(InComponent)
	, BodySetup(InComponent->GetBodySetup())
	, MaterialRelevance(InComponent->GetMaterialRelevance(GetScene().GetFeatureLevel()))
	, LightMapResolution(InComponent->GetStaticLightMapResolution())
	, RenderData(InComponent->TempConvertedMeshComponentForBuildingLight ? InComponent->TempConvertedMeshComponentForBuildingLight->GetStaticMesh()->RenderData.Get() : nullptr)
{
	if (RenderData)
	{
		const auto FeatureLevel = GetScene().GetFeatureLevel();
		bool bAnySectionCastsShadows = false;
		for (int32 LODIndex = 0; LODIndex < RenderData->LODResources.Num(); LODIndex++)
		{
			FLODInfo* NewLODInfo = new(LODs) FLODInfo(InComponent, LODIndex, /*bLODsShareStaticLighting*/true);

			// Under certain error conditions an LOD's material will be set to 
			// DefaultMaterial. Ensure our material view relevance is set properly.
			const int32 NumSections = NewLODInfo->Sections.Num();
			for (int32 SectionIndex = 0; SectionIndex < NumSections; ++SectionIndex)
			{
				const FLODInfo::FSectionInfo& SectionInfo = NewLODInfo->Sections[SectionIndex];
				bAnySectionCastsShadows |= RenderData->LODResources[LODIndex].Sections[SectionIndex].bCastShadow;
				if (SectionInfo.Material == UMaterial::GetDefaultMaterial(MD_Surface))
				{
					MaterialRelevance |= UMaterial::GetDefaultMaterial(MD_Surface)->GetRelevance(FeatureLevel);
				}
			}
		}
	}

	// Copy each section
	const int32 NumSections = InComponent->ProcMeshSections.Num();
	Sections.AddZeroed(NumSections);
	for (int SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
	{
		FArmyProcMeshSection& SrcSection = InComponent->ProcMeshSections[SectionIdx];
		if (SrcSection.ProcIndexBuffer.Num() > 0 && SrcSection.ProcVertexBuffer.Num() > 0)
		{
			FArmyProcMeshProxySection* NewSection = new FArmyProcMeshProxySection();

			// Copy data from vertex buffer
			const int32 NumVerts = SrcSection.ProcVertexBuffer.Num();

			// Allocate verts
			NewSection->VertexBuffer.Vertices.SetNumUninitialized(NumVerts);

			// Copy verts
			for (int VertIdx = 0; VertIdx < NumVerts; VertIdx++)
			{
				const FArmyProcMeshVertex& ProcVert = SrcSection.ProcVertexBuffer[VertIdx];
				FDynamicMeshVertex& Vert = NewSection->VertexBuffer.Vertices[VertIdx];
				ConvertProcMeshToDynMeshVertex(Vert, ProcVert);
			}

			// Copy index buffer
			NewSection->IndexBuffer.Indices = SrcSection.ProcIndexBuffer;

			// Init vertex factory
			NewSection->VertexFactory.Init(&NewSection->VertexBuffer);

			// Enqueue initialization of render resource
			BeginInitResource(&NewSection->VertexBuffer);
			BeginInitResource(&NewSection->IndexBuffer);
			BeginInitResource(&NewSection->VertexFactory);

			// Grab material
			NewSection->Material = InComponent->GetMaterial(SectionIdx);
			if (NewSection->Material == NULL)
			{
				NewSection->Material = UMaterial::GetDefaultMaterial(MD_Surface);
			}

			// Copy visibility info
			NewSection->bSectionVisible = SrcSection.bSectionVisible;

			// Save ref to new section
			Sections[SectionIdx] = NewSection;
		}
	}
}

FArmyProceduralMeshSceneProxy::~FArmyProceduralMeshSceneProxy()
{
	for (FArmyProcMeshProxySection* Section : Sections)
	{
		if (Section != nullptr)
		{
			Section->VertexBuffer.ReleaseResource();
			Section->IndexBuffer.ReleaseResource();
			Section->VertexFactory.ReleaseResource();
			delete Section;
		}
	}
}

void FArmyProceduralMeshSceneProxy::UpdateSection_RenderThread(FArmyProcMeshSectionUpdateData* SectionData)
{
	//SCOPE_CYCLE_COUNTER(STAT_XRProcMesh_UpdateSectionRT);

	check(IsInRenderingThread());

	// Check we have data 
	if (SectionData != nullptr)
	{
		// Check it references a valid section
		if (SectionData->TargetSection < Sections.Num() &&
			Sections[SectionData->TargetSection] != nullptr)
		{
			FArmyProcMeshProxySection* Section = Sections[SectionData->TargetSection];

			// Lock vertex buffer
			const int32 NumVerts = SectionData->NewVertexBuffer.Num();
			FDynamicMeshVertex* VertexBufferData = (FDynamicMeshVertex*)RHILockVertexBuffer(Section->VertexBuffer.VertexBufferRHI, 0, NumVerts * sizeof(FDynamicMeshVertex), RLM_WriteOnly);

			// Iterate through vertex data, copying in new info
			for (int32 VertIdx = 0; VertIdx < NumVerts; VertIdx++)
			{
				const FArmyProcMeshVertex& ProcVert = SectionData->NewVertexBuffer[VertIdx];
				FDynamicMeshVertex& Vert = VertexBufferData[VertIdx];
				ConvertProcMeshToDynMeshVertex(Vert, ProcVert);
			}

			// Unlock vertex buffer
			RHIUnlockVertexBuffer(Section->VertexBuffer.VertexBufferRHI);
		}

		// Free data sent from game thread
		delete SectionData;
	}
}

void FArmyProceduralMeshSceneProxy::SetSectionVisibility_RenderThread(int32 SectionIndex, bool bNewVisibility)
{
	check(IsInRenderingThread());

	if (SectionIndex < Sections.Num() &&
		Sections[SectionIndex] != nullptr)
	{
		Sections[SectionIndex]->bSectionVisible = bNewVisibility;
	}
}

void FArmyProceduralMeshSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const
{
	//SCOPE_CYCLE_COUNTER (STAT_XRGenMesh_GetMeshElements);

	QUICK_SCOPE_CYCLE_COUNTER(STAT_FGenMeshSceneProxy_GetDynamicMeshElements);
	checkSlow(IsInRenderingThread());

	const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;

	auto WireframeMaterialInstance = new FColoredMaterialRenderProxy(
		GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy(IsSelected()) : nullptr,
		FLinearColor(0, 0.5f, 1.f)
	);
	Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);

	// Iterate over sections
	for (const FArmyProcMeshProxySection* Section : Sections)
	{
		if (Section != nullptr && Section->bSectionVisible)
		{
			FMaterialRenderProxy *MaterialProxy = bWireframe ? WireframeMaterialInstance : Section->Material->GetRenderProxy(IsSelected());

			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ++ViewIndex)
			{
				const FSceneView* View = Views[ViewIndex];
				if (IsShown(View) && (VisibilityMap & (1 << ViewIndex)))
				{
					FMeshBatch &Mesh = Collector.AllocateMesh();
					if (LODs.IsValidIndex(0))
						Mesh.LCI = &LODs[0];
					FMeshBatchElement &BatchElement = Mesh.Elements[0];
					BatchElement.IndexBuffer = &Section->IndexBuffer;
					Mesh.bWireframe = bWireframe;
					Mesh.VertexFactory = &Section->VertexFactory;
					Mesh.MaterialRenderProxy = MaterialProxy;
					BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, UseEditorDepthTest());
					BatchElement.FirstIndex = 0;
					BatchElement.NumPrimitives = Section->IndexBuffer.Indices.Num() / 3;
					BatchElement.MinVertexIndex = 0;
					BatchElement.MaxVertexIndex = Section->VertexBuffer.Vertices.Num() - 1;
					Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
					Mesh.bDisableBackfaceCulling = 1;
					Mesh.Type = PT_TriangleList;
					Mesh.DepthPriorityGroup = GetStaticDepthPriorityGroup();
					Mesh.bCanApplyViewModeOverrides = false;
					Collector.AddMesh(ViewIndex, Mesh);
				}
			}
		}
	}
}

FPrimitiveViewRelevance FArmyProceduralMeshSceneProxy::GetViewRelevance(const FSceneView* View) const
{
	FPrimitiveViewRelevance Result;
	Result.bDrawRelevance = IsShown(View);
	Result.bShadowRelevance = IsShadowCast(View);
	Result.bDynamicRelevance = true;
	Result.bRenderInMainPass = ShouldRenderInMainPass();
	Result.bUsesLightingChannels = GetLightingChannelMask() != GetDefaultLightingChannelMask();
	Result.bRenderCustomDepth = ShouldRenderCustomDepth();
	MaterialRelevance.SetPrimitiveViewRelevance(Result);
	return Result;
}

void FArmyProceduralMeshSceneProxy::GetLCIs(FLCIArray& LCIs)
{
	for (int32 LODIndex = 0; LODIndex < LODs.Num(); ++LODIndex)
	{
		FLightCacheInterface* LCI = &LODs[LODIndex];
		LCIs.Push(LCI);
	}
}

void FArmyProceduralMeshSceneProxy::GetLightRelevance(const FLightSceneProxy* LightSceneProxy, bool& bDynamic, bool& bRelevant, bool& bLightMapped, bool& bShadowMapped) const
{
	// Attach the light to the primitive's static meshes.
	bDynamic = true;
	bRelevant = false;
	bLightMapped = true;
	bShadowMapped = true;

	if (LODs.Num() > 0)
	{
		for (int32 LODIndex = 0; LODIndex < LODs.Num(); LODIndex++)
		{
			const FLODInfo& LCI = LODs[LODIndex];

			ELightInteractionType InteractionType = LCI.GetInteraction(LightSceneProxy).GetType();

			if (InteractionType != LIT_CachedIrrelevant)
			{
				bRelevant = true;
			}

			if (InteractionType != LIT_CachedLightMap && InteractionType != LIT_CachedIrrelevant)
			{
				bLightMapped = false;
			}

			if (InteractionType != LIT_Dynamic)
			{
				bDynamic = false;
			}

			if (InteractionType != LIT_CachedSignedDistanceFieldShadowMap2D)
			{
				bShadowMapped = false;
			}
		}
	}
	else
	{
		bRelevant = true;
		bLightMapped = false;
		bShadowMapped = false;
	}
}

FArmyProceduralMeshSceneProxy::FLODInfo::FLODInfo( UXRProceduralMeshComponent* InComponent, int32 InLODIndex, bool bLODsShareStaticLighting)
	: FLightCacheInterface(nullptr, nullptr)
	, OverrideColorVertexBuffer(0)
	, PreCulledIndexBuffer(NULL)
	, bUsesMeshModifyingMaterials(false)
{
	const auto FeatureLevel = InComponent->GetWorld()->FeatureLevel;
	UStaticMeshComponent* SMC = InComponent->TempConvertedMeshComponentForBuildingLight;

	FStaticMeshRenderData* MeshRenderData = SMC->GetStaticMesh()->RenderData.Get();
	FStaticMeshLODResources& LODModel = MeshRenderData->LODResources[InLODIndex];
	if (InLODIndex < SMC->LODData.Num())
	{
		const FStaticMeshComponentLODInfo& ComponentLODInfo = SMC->LODData[InLODIndex];

		UMapBuildDataRegistry* MDBR = SMC->GetOwner()->GetLevel()->MapBuildData;
		if (MDBR)
		{
			FMeshMapBuildData* MeshMapBuildData = MDBR->GetMeshBuildData(ComponentLODInfo.MapBuildDataId);
			if (MeshMapBuildData)
			{
				SetLightMap(MeshMapBuildData->LightMap);
				SetShadowMap(MeshMapBuildData->ShadowMap);
				IrrelevantLights = MeshMapBuildData->IrrelevantLights;
			}

			PreCulledIndexBuffer = &ComponentLODInfo.PreCulledIndexBuffer;

			// Initialize this LOD's overridden vertex colors, if it has any
			if (ComponentLODInfo.OverrideVertexColors)
			{
				bool bBroken = false;
				for (int32 SectionIndex = 0; SectionIndex < LODModel.Sections.Num(); SectionIndex++)
				{
					const FStaticMeshSection& Section = LODModel.Sections[SectionIndex];
					if (Section.MaxVertexIndex >= ComponentLODInfo.OverrideVertexColors->GetNumVertices())
					{
						bBroken = true;
						break;
					}
				}
				if (!bBroken)
				{
					// the instance should point to the loaded data to avoid copy and memory waste
					OverrideColorVertexBuffer = ComponentLODInfo.OverrideVertexColors;
					check(OverrideColorVertexBuffer->GetStride() == sizeof(FColor)); //assumed when we set up the stream
				}
			}
		}
	}
	else
	{
		FGuid LightMapID = InComponent->GetLightMapIDFromOwner();
		UMapBuildDataRegistry* MDBR = SMC->GetOwner()->GetLevel()->MapBuildData;
		if (MDBR)
		{
			FMeshMapBuildData* MeshMapBuildData = MDBR->GetMeshBuildData(LightMapID);
			if (MeshMapBuildData)
			{
				SetLightMap(MeshMapBuildData->LightMap);
				SetShadowMap(MeshMapBuildData->ShadowMap);
				IrrelevantLights = MeshMapBuildData->IrrelevantLights;
			}
		}
		else
		{
			//InComponent->SetMobility(EComponentMobility::Movable);
		}
	}

	if (InLODIndex > 0 && bLODsShareStaticLighting && SMC->LODData.IsValidIndex(0))
	{
		const FStaticMeshComponentLODInfo& ComponentLODInfo = SMC->LODData[0];
		const FMeshMapBuildData* MeshMapBuildData = SMC->GetMeshMapBuildData(ComponentLODInfo);

		if (MeshMapBuildData)
		{
			SetLightMap(MeshMapBuildData->LightMap);
			SetShadowMap(MeshMapBuildData->ShadowMap);
			IrrelevantLights = MeshMapBuildData->IrrelevantLights;
		}
	}

	bool bHasStaticLighting = GetLightMap() != NULL || GetShadowMap() != NULL;

	// Gather the materials applied to the LOD.
	Sections.Empty(MeshRenderData->LODResources[InLODIndex].Sections.Num());
	for (int32 SectionIndex = 0; SectionIndex < LODModel.Sections.Num(); SectionIndex++)
	{
		const FStaticMeshSection& Section = LODModel.Sections[SectionIndex];
		FSectionInfo SectionInfo;

		// Determine the material applied to this element of the LOD.
		SectionInfo.Material = SMC->GetMaterial(Section.MaterialIndex);
		SectionInfo.MaterialIndex = Section.MaterialIndex;

		/*if (GForceDefaultMaterial && SectionInfo.Material && !IsTranslucentBlendMode(SectionInfo.Material->GetBlendMode()))
		{
			SectionInfo.Material = UMaterial::GetDefaultMaterial(MD_Surface);
		}*/

		// If there isn't an applied material, or if we need static lighting and it doesn't support it, fall back to the default material.
		if (!SectionInfo.Material || (bHasStaticLighting && !SectionInfo.Material->CheckMaterialUsage_Concurrent(MATUSAGE_StaticLighting)))
		{
			SectionInfo.Material = UMaterial::GetDefaultMaterial(MD_Surface);
		}

		const bool bRequiresAdjacencyInformation = false;// RequiresAdjacencyInformation(SectionInfo.Material, LODModel.VertexFactory.GetType(), FeatureLevel);
		if (bRequiresAdjacencyInformation && !LODModel.bHasAdjacencyInfo)
		{
			UE_LOG(LogStaticMesh, Warning, TEXT("Adjacency information not built for static mesh with a material that requires it. Using default material instead.\n\tMaterial: %s\n\tStaticMesh: %s"),
				*SectionInfo.Material->GetPathName(),
				*SMC->GetStaticMesh()->GetPathName());
			SectionInfo.Material = UMaterial::GetDefaultMaterial(MD_Surface);
		}

		// Per-section selection for the editor.
#if WITH_EDITORONLY_DATA
		if (GIsEditor)
		{
			if (SMC->SelectedEditorMaterial >= 0)
			{
				SectionInfo.bSelected = (SMC->SelectedEditorMaterial == Section.MaterialIndex);
			}
			else
			{
				SectionInfo.bSelected = (SMC->SelectedEditorSection == SectionIndex);
			}
		}
#endif

		if (InLODIndex < SMC->LODData.Num())
		{
			const FStaticMeshComponentLODInfo& ComponentLODInfo = SMC->LODData[InLODIndex];

			if (SectionIndex < ComponentLODInfo.PreCulledSections.Num())
			{
				SectionInfo.FirstPreCulledIndex = ComponentLODInfo.PreCulledSections[SectionIndex].FirstIndex;
				SectionInfo.NumPreCulledTriangles = ComponentLODInfo.PreCulledSections[SectionIndex].NumTriangles;
			}
		}

		// Store the element info.
		Sections.Add(SectionInfo);

		// Flag the entire LOD if any material modifies its mesh
		UMaterialInterface::TMicRecursionGuard RecursionGuard;
		FMaterialResource const* MaterialResource = const_cast<UMaterialInterface const*>(SectionInfo.Material)->GetMaterial_Concurrent(RecursionGuard)->GetMaterialResource(FeatureLevel);
		if (MaterialResource)
		{
			if (IsInGameThread())
			{
				if (MaterialResource->MaterialModifiesMeshPosition_GameThread())
				{
					bUsesMeshModifyingMaterials = true;
				}
			}
			else
			{
				if (MaterialResource->MaterialModifiesMeshPosition_RenderThread())
				{
					bUsesMeshModifyingMaterials = true;
				}
			}
		}
	}
}

FLightInteraction FArmyProceduralMeshSceneProxy::FLODInfo::GetInteraction(const FLightSceneProxy* LightSceneProxy) const
{
	// ask base class
	ELightInteractionType LightInteraction = GetStaticInteraction(LightSceneProxy, IrrelevantLights);

	if (LightInteraction != LIT_MAX)
	{
		return FLightInteraction(LightInteraction);
	}

	// Use dynamic lighting if the light doesn't have static lighting.
	return FLightInteraction::Dynamic();
}