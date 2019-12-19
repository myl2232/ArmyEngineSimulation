#include "XRMeshUtilities.h"
#include "RawMesh.h"
#include "StaticMeshResources.h"

/** Helper struct for building acceleration structures. */
struct FIndexAndZ
{
	float Z;
	int32 Index;

	/** Default constructor. */
	FIndexAndZ() {}

	/** Initialization constructor. */
	FIndexAndZ(int32 InIndex, FVector V)
	{
		Z = 0.30f * V.X + 0.33f * V.Y + 0.37f * V.Z;
		Index = InIndex;
	}
};

/** Sorting function for vertex Z/index pairs. */
struct FCompareIndexAndZ
{
	FORCEINLINE bool operator()(FIndexAndZ const& A, FIndexAndZ const& B) const { return A.Z < B.Z; }
};

inline bool PointsEqual(const FVector& V1, const FVector& V2, float ComparisonThreshold)
{
	if (FMath::Abs(V1.X - V2.X) > ComparisonThreshold
		|| FMath::Abs(V1.Y - V2.Y) > ComparisonThreshold
		|| FMath::Abs(V1.Z - V2.Z) > ComparisonThreshold)
	{
		return false;
	}
	return true;
}

static inline FVector GetPositionForWedge(FRawMesh const& Mesh, int32 WedgeIndex)
{
	int32 VertexIndex = Mesh.WedgeIndices[WedgeIndex];
	return Mesh.VertexPositions[VertexIndex];
}

static bool AreVerticesEqual(
	FStaticMeshBuildVertex const& A,
	FStaticMeshBuildVertex const& B,
	float ComparisonThreshold
)
{
	if (!PointsEqual(A.Position, B.Position, ComparisonThreshold)
		|| !NormalsEqual(A.TangentX, B.TangentX)
		|| !NormalsEqual(A.TangentY, B.TangentY)
		|| !NormalsEqual(A.TangentZ, B.TangentZ)
		|| A.Color != B.Color)
	{
		return false;
	}

	// UVs
	for (int32 UVIndex = 0; UVIndex < MAX_STATIC_TEXCOORDS; UVIndex++)
	{
		if (!UVsEqual(A.UVs[UVIndex], B.UVs[UVIndex]))
		{
			return false;
		}
	}

	return true;
}

static void BuildDepthOnlyIndexBuffer(
	TArray<uint32>& OutDepthIndices,
	const TArray<FStaticMeshBuildVertex>& InVertices,
	const TArray<uint32>& InIndices,
	const TArray<FStaticMeshSection>& InSections
)
{
	int32 NumVertices = InVertices.Num();
	if (InIndices.Num() <= 0 || NumVertices <= 0)
	{
		OutDepthIndices.Empty();
		return;
	}

	// Create a mapping of index -> first overlapping index to accelerate the construction of the shadow index buffer.
	TArray<FIndexAndZ> VertIndexAndZ;
	VertIndexAndZ.Empty(NumVertices);
	for (int32 VertIndex = 0; VertIndex < NumVertices; VertIndex++)
	{
		new(VertIndexAndZ)FIndexAndZ(VertIndex, InVertices[VertIndex].Position);
	}
	VertIndexAndZ.Sort(FCompareIndexAndZ());

	// Setup the index map. 0xFFFFFFFF == not set.
	TArray<uint32> IndexMap;
	IndexMap.AddUninitialized(NumVertices);
	FMemory::Memset(IndexMap.GetData(), 0xFF, NumVertices * sizeof(uint32));

	// Search for duplicates, quickly!
	for (int32 i = 0; i < VertIndexAndZ.Num(); i++)
	{
		uint32 SrcIndex = VertIndexAndZ[i].Index;
		float Z = VertIndexAndZ[i].Z;
		IndexMap[SrcIndex] = FMath::Min(IndexMap[SrcIndex], SrcIndex);

		// Search forward since we add pairs both ways.
		for (int32 j = i + 1; j < VertIndexAndZ.Num(); j++)
		{
			if (FMath::Abs(VertIndexAndZ[j].Z - Z) > THRESH_POINTS_ARE_SAME * 4.01f)
				break; // can't be any more dups

			uint32 OtherIndex = VertIndexAndZ[j].Index;
			if (PointsEqual(InVertices[SrcIndex].Position, InVertices[OtherIndex].Position,/*bUseEpsilonCompare=*/ false))
			{
				IndexMap[SrcIndex] = FMath::Min(IndexMap[SrcIndex], OtherIndex);
				IndexMap[OtherIndex] = FMath::Min(IndexMap[OtherIndex], SrcIndex);
			}
		}
	}

	// Build the depth-only index buffer by remapping all indices to the first overlapping
	// vertex in the vertex buffer.
	OutDepthIndices.Empty();
	for (int32 SectionIndex = 0; SectionIndex < InSections.Num(); ++SectionIndex)
	{
		const FStaticMeshSection& Section = InSections[SectionIndex];
		int32 FirstIndex = Section.FirstIndex;
		int32 LastIndex = FirstIndex + Section.NumTriangles * 3;
		for (int32 SrcIndex = FirstIndex; SrcIndex < LastIndex; ++SrcIndex)
		{
			uint32 VertIndex = InIndices[SrcIndex];
			OutDepthIndices.Add(IndexMap[VertIndex]);
		}
	}
}

bool FArmyMeshUtilities::GenerateRenderingMeshes(FRawMesh& InRawMesh, FStaticMeshRenderData& OutRenderData)
{
	int32 ImportVersion = 2;

	// Generate per-LOD rendering data.
	OutRenderData.AllocateLODResources(1);
	for (int32 LODIndex = 0; LODIndex < 1; ++LODIndex)
	{
		FStaticMeshLODResources& LODModel = OutRenderData.LODResources[LODIndex];
		//FRawMesh& RawMesh = LODMeshes[LODIndex];
		LODModel.MaxDeviation = 0;// LODMaxDeviation[LODIndex];

		TArray<FStaticMeshBuildVertex> Vertices;
		TArray<TArray<uint32> > PerSectionIndices;

		TMap<uint32, uint32> MaterialToSectionMapping;

		// Find out how many sections are in the mesh.
		TArray<int32> MaterialIndices;
		for (const int32 MaterialIndex : InRawMesh.FaceMaterialIndices)
		{
			// Find all unique material indices
			MaterialIndices.AddUnique(MaterialIndex);
		}

		// Need X number of sections for X number of material indices
		//for (const int32 MaterialIndex : MaterialIndices)
		for (int32 Index = 0; Index < MaterialIndices.Num(); ++Index)
		{
			const int32 MaterialIndex = MaterialIndices[Index];
			FStaticMeshSection* Section = new(LODModel.Sections) FStaticMeshSection();
			Section->MaterialIndex = MaterialIndex;
			if (ImportVersion < RemoveStaticMeshSkinxxWorkflow)
			{
				MaterialToSectionMapping.Add(MaterialIndex, MaterialIndex);
			}
			else
			{
				MaterialToSectionMapping.Add(MaterialIndex, Index);
			}
			new(PerSectionIndices)TArray<uint32>;
		}

		// Build and cache optimize vertex and index buffers.
		{
			// TODO_STATICMESH: The wedge map is only valid for LODIndex 0 if no reduction has been performed.
			// We can compute an approximate one instead for other LODs.
			TArray<int32> TempWedgeMap;
			TArray<int32>& WedgeMap = (LODIndex == 0) ? OutRenderData.WedgeMap : TempWedgeMap;
			float ComparisonThreshold = THRESH_POINTS_ARE_SAME;// GetComparisonThreshold(LODBuildSettings[LODIndex]);
			BuildStaticMeshVertexAndIndexBuffers(Vertices, PerSectionIndices, WedgeMap, InRawMesh, OverlappingCorners, MaterialToSectionMapping, ComparisonThreshold, FVector(1, 1, 1), ImportVersion);
			check(WedgeMap.Num() == InRawMesh.WedgeIndices.Num());

			// 			if (InRawMesh.WedgeIndices.Num() < 100000 * 3)
			// 			{
			// 				MeshUtilities.CacheOptimizeVertexAndIndexBuffer(Vertices, PerSectionIndices, WedgeMap);
			// 				check(WedgeMap.Num() == InRawMesh.WedgeIndices.Num());
			// 			}
		}

		//verifyf(Vertices.Num() != 0, TEXT("No valid vertices found for the mesh."));
		if (Vertices.Num() == 0)
		{
			return false;
		}

		// Initialize the vertex buffer.
		int32 NumTexCoords = 2;// ComputeNumTexCoords(InRawMesh, MAX_STATIC_TEXCOORDS);
		LODModel.VertexBuffer.SetUseHighPrecisionTangentBasis(false);
		LODModel.VertexBuffer.SetUseFullPrecisionUVs(false);
		LODModel.VertexBuffer.Init(Vertices, NumTexCoords);
		LODModel.PositionVertexBuffer.Init(Vertices);
		LODModel.ColorVertexBuffer.Init(Vertices);

		// Concatenate the per-section index buffers.
		TArray<uint32> CombinedIndices;
		bool bNeeds32BitIndices = false;
		for (int32 SectionIndex = 0; SectionIndex < LODModel.Sections.Num(); SectionIndex++)
		{
			FStaticMeshSection& Section = LODModel.Sections[SectionIndex];
			TArray<uint32> const& SectionIndices = PerSectionIndices[SectionIndex];
			Section.FirstIndex = 0;
			Section.NumTriangles = 0;
			Section.MinVertexIndex = 0;
			Section.MaxVertexIndex = 0;

			if (SectionIndices.Num())
			{
				Section.FirstIndex = CombinedIndices.Num();
				Section.NumTriangles = SectionIndices.Num() / 3;

				CombinedIndices.AddUninitialized(SectionIndices.Num());
				uint32* DestPtr = &CombinedIndices[Section.FirstIndex];
				uint32 const* SrcPtr = SectionIndices.GetData();

				Section.MinVertexIndex = *SrcPtr;
				Section.MaxVertexIndex = *SrcPtr;

				for (int32 Index = 0; Index < SectionIndices.Num(); Index++)
				{
					uint32 VertIndex = *SrcPtr++;

					bNeeds32BitIndices |= (VertIndex > MAX_uint16);
					Section.MinVertexIndex = FMath::Min<uint32>(VertIndex, Section.MinVertexIndex);
					Section.MaxVertexIndex = FMath::Max<uint32>(VertIndex, Section.MaxVertexIndex);
					*DestPtr++ = VertIndex;
				}
			}
		}
		LODModel.IndexBuffer.SetIndices(CombinedIndices, bNeeds32BitIndices ? EIndexBufferStride::Force32Bit : EIndexBufferStride::Force16Bit);

		// Build the reversed index buffer.
		//if (MeshUtilities.bEnableReversedIndexBuffer)
		{
			TArray<uint32> InversedIndices;
			const int32 IndexCount = CombinedIndices.Num();
			InversedIndices.AddUninitialized(IndexCount);

			for (int32 SectionIndex = 0; SectionIndex < LODModel.Sections.Num(); ++SectionIndex)
			{
				const FStaticMeshSection& SectionInfo = LODModel.Sections[SectionIndex];
				const int32 SectionIndexCount = SectionInfo.NumTriangles * 3;

				for (int32 i = 0; i < SectionIndexCount; ++i)
				{
					InversedIndices[SectionInfo.FirstIndex + i] = CombinedIndices[SectionInfo.FirstIndex + SectionIndexCount - 1 - i];
				}
			}
			LODModel.ReversedIndexBuffer.SetIndices(InversedIndices, bNeeds32BitIndices ? EIndexBufferStride::Force32Bit : EIndexBufferStride::Force16Bit);
		}

		// Build the depth-only index buffer.
		TArray<uint32> DepthOnlyIndices;
		//if (MeshUtilities.bEnableDepthOnlyIndexBuffer)
		{
			BuildDepthOnlyIndexBuffer(
				DepthOnlyIndices,
				Vertices,
				CombinedIndices,
				LODModel.Sections
			);

			if (DepthOnlyIndices.Num() < 50000 * 3)
			{
				//MeshUtilities.CacheOptimizeIndexBuffer(DepthOnlyIndices);
			}

			LODModel.DepthOnlyIndexBuffer.SetIndices(DepthOnlyIndices, bNeeds32BitIndices ? EIndexBufferStride::Force32Bit : EIndexBufferStride::Force16Bit);
		}

		// Build the inversed depth only index buffer.
		//if (InOutModels[0].BuildSettings.bBuildReversedIndexBuffer && MeshUtilities.bEnableDepthOnlyIndexBuffer && MeshUtilities.bEnableReversedIndexBuffer)

		TArray<uint32> ReversedDepthOnlyIndices;
		const int32 IndexCount = DepthOnlyIndices.Num();
		ReversedDepthOnlyIndices.AddUninitialized(IndexCount);
		for (int32 i = 0; i < IndexCount; ++i)
		{
			ReversedDepthOnlyIndices[i] = DepthOnlyIndices[IndexCount - 1 - i];
		}
		LODModel.ReversedDepthOnlyIndexBuffer.SetIndices(ReversedDepthOnlyIndices, bNeeds32BitIndices ? EIndexBufferStride::Force32Bit : EIndexBufferStride::Force16Bit);
		//}

		// Build a list of wireframe edges in the static mesh.
		/*{
		TArray<FMeshEdge> Edges;
		TArray<uint32> WireframeIndices;

		FStaticMeshEdgeBuilder(CombinedIndices, Vertices, Edges).FindEdges();
		WireframeIndices.Empty(2 * Edges.Num());
		for (int32 EdgeIndex = 0; EdgeIndex < Edges.Num(); EdgeIndex++)
		{
		FMeshEdge&	Edge = Edges[EdgeIndex];
		WireframeIndices.Add(Edge.Vertices[0]);
		WireframeIndices.Add(Edge.Vertices[1]);
		}
		LODModel.WireframeIndexBuffer.SetIndices(WireframeIndices, bNeeds32BitIndices ? EIndexBufferStride::Force32Bit : EIndexBufferStride::Force16Bit);
		}*/

		// Build the adjacency index buffer used for tessellation.
		/*if (InOutModels[0].BuildSettings.bBuildAdjacencyBuffer)
		{
		TArray<uint32> AdjacencyIndices;

		BuildStaticAdjacencyIndexBuffer(
		LODModel.PositionVertexBuffer,
		LODModel.VertexBuffer,
		CombinedIndices,
		AdjacencyIndices
		);
		LODModel.AdjacencyIndexBuffer.SetIndices(AdjacencyIndices, bNeeds32BitIndices ? EIndexBufferStride::Force32Bit : EIndexBufferStride::Force16Bit);
		}*/
	}

	// Copy the original material indices to fixup meshes before compacting of materials was done.
	//if (NumValidLODs > 0)
	//{
	//OutRenderData.MaterialIndexToImportIndex = LODMeshes[0].MaterialIndexToImportIndex;
	//}

	// Calculate the bounding box.
	FBox BoundingBox(ForceInit);
	FPositionVertexBuffer& BasePositionVertexBuffer = OutRenderData.LODResources[0].PositionVertexBuffer;
	for (uint32 VertexIndex = 0; VertexIndex < BasePositionVertexBuffer.GetNumVertices(); VertexIndex++)
	{
		BoundingBox += BasePositionVertexBuffer.VertexPosition(VertexIndex);
	}
	BoundingBox.GetCenterAndExtents(OutRenderData.Bounds.Origin, OutRenderData.Bounds.BoxExtent);

	// Calculate the bounding sphere, using the center of the bounding box as the origin.
	OutRenderData.Bounds.SphereRadius = 0.0f;
	for (uint32 VertexIndex = 0; VertexIndex < BasePositionVertexBuffer.GetNumVertices(); VertexIndex++)
	{
		OutRenderData.Bounds.SphereRadius = FMath::Max(
			(BasePositionVertexBuffer.VertexPosition(VertexIndex) - OutRenderData.Bounds.Origin).Size(),
			OutRenderData.Bounds.SphereRadius
		);
	}

	//Stage = EStage::GenerateRendering;
	return true;
}

struct FMeshEdge
{
	int32	Vertices[2];
	int32	Faces[2];
};

static FStaticMeshBuildVertex BuildStaticMeshVertex(FRawMesh const& RawMesh, int32 WedgeIndex, FVector BuildScale)
{
	FStaticMeshBuildVertex Vertex;
	Vertex.Position = GetPositionForWedge(RawMesh, WedgeIndex) * BuildScale;

	const FMatrix ScaleMatrix = FScaleMatrix(BuildScale).Inverse().GetTransposed();
	Vertex.TangentX = ScaleMatrix.TransformVector(RawMesh.WedgeTangentX[WedgeIndex]).GetSafeNormal();
	Vertex.TangentY = ScaleMatrix.TransformVector(RawMesh.WedgeTangentY[WedgeIndex]).GetSafeNormal();
	Vertex.TangentZ = ScaleMatrix.TransformVector(RawMesh.WedgeTangentZ[WedgeIndex]).GetSafeNormal();

	if (RawMesh.WedgeColors.IsValidIndex(WedgeIndex))
	{
		Vertex.Color = RawMesh.WedgeColors[WedgeIndex];
	}
	else
	{
		Vertex.Color = FColor::White;
	}

	int32 NumTexCoords = FMath::Min<int32>(MAX_MESH_TEXTURE_COORDS, MAX_STATIC_TEXCOORDS);
	for (int32 i = 0; i < NumTexCoords; ++i)
	{
		if (RawMesh.WedgeTexCoords[i].IsValidIndex(WedgeIndex))
		{
			Vertex.UVs[i] = RawMesh.WedgeTexCoords[i][WedgeIndex];
		}
		else
		{
			Vertex.UVs[i] = FVector2D(0.0f, 0.0f);
		}
	}
	return Vertex;
}

TSharedPtr<FArmyMeshUtilities> FArmyMeshUtilities::Singleton = nullptr;

FArmyMeshUtilities& FArmyMeshUtilities::Get()
{
	if (!Singleton.IsValid())
	{
		Singleton = MakeShareable(new FArmyMeshUtilities());
	}
	return *Singleton;
}

static int32 ComputeNumTexCoords(FRawMesh const& RawMesh, int32 MaxSupportedTexCoords)
{
	int32 NumWedges = RawMesh.WedgeIndices.Num();
	int32 NumTexCoords = 0;
	for (int32 TexCoordIndex = 0; TexCoordIndex < MAX_MESH_TEXTURE_COORDS; ++TexCoordIndex)
	{
		if (RawMesh.WedgeTexCoords[TexCoordIndex].Num() != NumWedges)
		{
			break;
		}
		NumTexCoords++;
	}
	return FMath::Min(NumTexCoords, MaxSupportedTexCoords);
}

void FArmyMeshUtilities::FindOverlappingCorners(TMultiMap<int32, int32>& OutOverlappingCorners, FRawMesh const& RawMesh, float ComparisonThreshold)
{
	const int32 NumWedges = RawMesh.WedgeIndices.Num();

	// Create a list of vertex Z/index pairs
	TArray<FIndexAndZ> VertIndexAndZ;
	VertIndexAndZ.Reserve(NumWedges);
	for (int32 WedgeIndex = 0; WedgeIndex < NumWedges; WedgeIndex++)
	{
		new(VertIndexAndZ)FIndexAndZ(WedgeIndex, RawMesh.VertexPositions[RawMesh.WedgeIndices[WedgeIndex]]);
	}

	// Sort the vertices by z value
	VertIndexAndZ.Sort(FCompareIndexAndZ());

	// Search for duplicates, quickly!
	for (int32 i = 0; i < VertIndexAndZ.Num(); i++)
	{
		// only need to search forward, since we add pairs both ways
		for (int32 j = i + 1; j < VertIndexAndZ.Num(); j++)
		{
			if (FMath::Abs(VertIndexAndZ[j].Z - VertIndexAndZ[i].Z) > ComparisonThreshold)
				break; // can't be any more dups

			const FVector& PositionA = RawMesh.VertexPositions[RawMesh.WedgeIndices[VertIndexAndZ[i].Index]];
			const FVector& PositionB = RawMesh.VertexPositions[RawMesh.WedgeIndices[VertIndexAndZ[j].Index]];

			if (PointsEqual(PositionA, PositionB, ComparisonThreshold))
			{
				OutOverlappingCorners.Add(VertIndexAndZ[i].Index, VertIndexAndZ[j].Index);
				OutOverlappingCorners.Add(VertIndexAndZ[j].Index, VertIndexAndZ[i].Index);
			}
		}
	}
}

bool FArmyMeshUtilities::BuildMeshLightMapUV(FRawMesh& OutRawMesh, int32 InRes, FMatrix InMat, bool bInWrapFlat)
 {
	OverlappingCorners.Reset();
	FArmyMeshUtilities::Get().FindOverlappingCorners(OverlappingCorners, OutRawMesh, 0.00002f);

	bool bPackSuccess = false;

	if (bInWrapFlat)
	{
		TArray<FVector> TransformedPosition;

		for (FVector& It : OutRawMesh.VertexPositions)
		{
			FVector NewPosition = InMat.TransformPosition(It);
			TransformedPosition.Add(NewPosition);
		}
		FVector MaxPoint = FVector::ZeroVector;
		FVector MinPoint = FVector::ZeroVector;
		for (auto& It : TransformedPosition)
		{
			if (It.X > MaxPoint.X)
				MaxPoint.X = It.X;
			if (It.Y > MaxPoint.Y)
				MaxPoint.Y = It.Y;

			if (It.X < MinPoint.X)
				MinPoint.X = It.X;
			if (It.Y < MinPoint.Y)
				MinPoint.Y = It.Y;
		}

		FVector2D Size = FVector2D(MaxPoint.X - MinPoint.X, MaxPoint.Y - MinPoint.Y);
		
		OutRawMesh.WedgeTexCoords[1].SetNumZeroed(OutRawMesh.WedgeTexCoords[0].Num());
		for (int32 i = 0 ; i< TransformedPosition.Num(); i++)
		{
			OutRawMesh.WedgeTexCoords[1][i].X = (TransformedPosition[i].X - MinPoint.X) / Size.X * 0.98f + 0.01f;
			OutRawMesh.WedgeTexCoords[1][i].Y = (TransformedPosition[i].Y - MinPoint.Y) / Size.Y * 0.98f + 0.01;
		}
		
		bPackSuccess = true;
	}
	else
	{
		FLayoutUV Packer(&OutRawMesh, 0, 1, InRes);
		Packer.SetVersion(ELightmapUVVersion::SmallChartPacking);
		Packer.FindCharts(OverlappingCorners);
		bPackSuccess = Packer.FindBestPacking();
		if (bPackSuccess)
		{
			Packer.CommitPackedUVs();
		}
	}

	return bPackSuccess;
}

void FArmyMeshUtilities::BuildStaticMeshVertexAndIndexBuffers(TArray<FStaticMeshBuildVertex>& OutVertices, TArray<TArray<uint32> >& OutPerSectionIndices, TArray<int32>& OutWedgeMap, const FRawMesh& RawMesh, const TMultiMap<int32, int32>& OverlappingCorners, const TMap<uint32, uint32>& MaterialToSectionMapping, float ComparisonThreshold, FVector BuildScale, int32 ImportVersion)
{
	TMap<int32, int32> FinalVerts;
	TArray<int32> DupVerts;
	int32 NumFaces = RawMesh.WedgeIndices.Num() / 3;

	// Process each face, build vertex buffer and per-section index buffers.
	for (int32 FaceIndex = 0; FaceIndex < NumFaces; FaceIndex++)
	{
		int32 VertexIndices[3];
		FVector CornerPositions[3];

		for (int32 CornerIndex = 0; CornerIndex < 3; CornerIndex++)
		{
			CornerPositions[CornerIndex] = GetPositionForWedge(RawMesh, FaceIndex * 3 + CornerIndex);
		}

		// Don't process degenerate triangles.
		if (PointsEqual(CornerPositions[0], CornerPositions[1], ComparisonThreshold)
			|| PointsEqual(CornerPositions[0], CornerPositions[2], ComparisonThreshold)
			|| PointsEqual(CornerPositions[1], CornerPositions[2], ComparisonThreshold))
		{
			for (int32 CornerIndex = 0; CornerIndex < 3; CornerIndex++)
			{
				OutWedgeMap.Add(INDEX_NONE);
			}
			continue;
		}

		for (int32 CornerIndex = 0; CornerIndex < 3; CornerIndex++)
		{
			int32 WedgeIndex = FaceIndex * 3 + CornerIndex;
			FStaticMeshBuildVertex ThisVertex = BuildStaticMeshVertex(RawMesh, WedgeIndex, BuildScale);

			DupVerts.Reset();
			OverlappingCorners.MultiFind(WedgeIndex, DupVerts);
			DupVerts.Sort();

			int32 Index = INDEX_NONE;
			for (int32 k = 0; k < DupVerts.Num(); k++)
			{
				if (DupVerts[k] >= WedgeIndex)
				{
					// the verts beyond me haven't been placed yet, so these duplicates are not relevant
					break;
				}

				int32 *Location = FinalVerts.Find(DupVerts[k]);
				if (Location != NULL
					&& AreVerticesEqual(ThisVertex, OutVertices[*Location], ComparisonThreshold))
				{
					Index = *Location;
					break;
				}
			}
			if (Index == INDEX_NONE)
			{
				Index = OutVertices.Add(ThisVertex);
				FinalVerts.Add(WedgeIndex, Index);
			}
			VertexIndices[CornerIndex] = Index;
		}

		// Reject degenerate triangles.
		if (VertexIndices[0] == VertexIndices[1]
			|| VertexIndices[1] == VertexIndices[2]
			|| VertexIndices[0] == VertexIndices[2])
		{
			for (int32 CornerIndex = 0; CornerIndex < 3; CornerIndex++)
			{
				OutWedgeMap.Add(INDEX_NONE);
			}
			continue;
		}

		// Put the indices in the material index buffer.
		uint32 SectionIndex = 0;
		if (ImportVersion < RemoveStaticMeshSkinxxWorkflow)
		{
			SectionIndex = FMath::Clamp(RawMesh.FaceMaterialIndices[FaceIndex], 0, OutPerSectionIndices.Num() - 1);
		}
		else
		{
			SectionIndex = MaterialToSectionMapping.FindChecked(RawMesh.FaceMaterialIndices[FaceIndex]);
		}
		TArray<uint32>& SectionIndices = OutPerSectionIndices[SectionIndex];
		for (int32 CornerIndex = 0; CornerIndex < 3; CornerIndex++)
		{
			SectionIndices.Add(VertexIndices[CornerIndex]);
			OutWedgeMap.Add(VertexIndices[CornerIndex]);
		}
	}
}
