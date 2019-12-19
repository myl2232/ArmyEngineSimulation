#include "XRStaticMeshStaticLightingMesh.h"
#include "Lightmass.h"
#include "Components/LightComponent.h"

FArmyStaticMeshStaticLightingMesh::FArmyStaticMeshStaticLightingMesh(const UStaticMeshComponent* InPrimitive, int32 InLODIndex, const TArray<ULightComponent*>& InRelevantLights)
	: FStaticMeshStaticLightingMesh(InPrimitive, InLODIndex, InRelevantLights)
{
	bCustomMesh = true;
}

void FArmyStaticMeshStaticLightingMesh::ExportMeshInstance(class FLightmassExporter* Exporter) const
{
	Exporter->StaticMeshLightingMeshes.AddUnique(this);

	for (int32 LightIdx = 0; LightIdx < RelevantLights.Num(); LightIdx++)
	{
		ULightComponent* Light = RelevantLights[LightIdx];
		if (Light)
		{
			Exporter->AddLight(Light);
		}
	}

	// Add the UStaticMesh and materials to the exporter...
	if (StaticMesh && StaticMesh->RenderData)
	{
		Exporter->StaticMeshes.AddUnique(StaticMesh);
		if (Primitive)
		{
			for (int32 ResourceIndex = 0; ResourceIndex < StaticMesh->RenderData->LODResources.Num(); ++ResourceIndex)
			{
				const FStaticMeshLODResources& LODResourceData = StaticMesh->RenderData->LODResources[ResourceIndex];
				for (int32 SectionIndex = 0; SectionIndex < LODResourceData.Sections.Num(); ++SectionIndex)
				{
					const FStaticMeshSection& Section = LODResourceData.Sections[SectionIndex];
					UMaterialInterface* Material = Primitive->GetMaterial(Section.MaterialIndex);
					Exporter->AddMaterial(Material);
				}
			}
		}
	}
}

FArmyStaticMeshStaticLightingTextureMapping::FArmyStaticMeshStaticLightingTextureMapping(UStaticMeshComponent* InPrimitive, int32 InLODIndex, FStaticLightingMesh* InMesh, int32 InSizeX, int32 InSizeY, int32 InTextureCoordinateIndex, bool bPerformFullQualityRebuild)
	: FStaticMeshStaticLightingTextureMapping(InPrimitive, InLODIndex, InMesh, InSizeX, InSizeY, InTextureCoordinateIndex, bPerformFullQualityRebuild)
{
	bCustomMapping = true;
}

void FArmyStaticMeshStaticLightingTextureMapping::ExportMapping(class FLightmassExporter* Exporter)
{
	Exporter->StaticMeshTextureMappings.AddUnique(this);
}

#if WITH_EDITOR
bool FArmyStaticMeshStaticLightingTextureMapping::DebugThisMapping() const
{
	return false;
}
#endif
