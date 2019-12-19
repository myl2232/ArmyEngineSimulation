#pragma once
#include "CoreMinimal.h"
#include "StaticMeshLight.h"

class FArmyStaticMeshStaticLightingMesh : public FStaticMeshStaticLightingMesh
{
public:
	XRSTATICLIGHTING_API FArmyStaticMeshStaticLightingMesh(const UStaticMeshComponent* InPrimitive, int32 InLODIndex, const TArray<ULightComponent*>& InRelevantLights);
	
	XRSTATICLIGHTING_API virtual void ExportMeshInstance(class FLightmassExporter* Exporter) const override;
};

class FArmyStaticMeshStaticLightingTextureMapping : public FStaticMeshStaticLightingTextureMapping
{
public:
	XRSTATICLIGHTING_API FArmyStaticMeshStaticLightingTextureMapping(UStaticMeshComponent* InPrimitive, int32 InLODIndex, FStaticLightingMesh* InMesh, int32 InSizeX, int32 InSizeY, int32 InTextureCoordinateIndex, bool bPerformFullQualityRebuild);

	XRSTATICLIGHTING_API virtual void ExportMapping(class FLightmassExporter* Exporter) override;

	//virtual FArmyStaticMeshStaticLightingTextureMapping* GetTextureMapping()
	//{
	//	return this;
	//}

#if WITH_EDITOR
	XRSTATICLIGHTING_API virtual bool DebugThisMapping() const override;
#endif

	int32 a;
};