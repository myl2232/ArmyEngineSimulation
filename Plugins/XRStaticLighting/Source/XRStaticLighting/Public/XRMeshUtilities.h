#pragma once

#include "CoreMinimal.h"
#include "UnrealMathUtility.h"

/**
* Returns true if the specified points are about equal
*/
inline bool PointsEqual(const FVector& V1, const FVector& V2, bool bUseEpsilonCompare = true)
{
	const float Epsilon1 = bUseEpsilonCompare ? THRESH_POINTS_ARE_SAME : 0.0f;
	return FMath::Abs(V1.X - V2.X) <= Epsilon1 && FMath::Abs(V1.Y - V2.Y) <= Epsilon1 && FMath::Abs(V1.Z - V2.Z) <= Epsilon1;
}

/**
* Returns true if the specified normal vectors are about equal
*/
inline bool NormalsEqual(const FVector& V1, const FVector& V2)
{
	const float Epsilon2 = THRESH_NORMALS_ARE_SAME;
	return FMath::Abs(V1.X - V2.X) <= Epsilon2 && FMath::Abs(V1.Y - V2.Y) <= Epsilon2 && FMath::Abs(V1.Z - V2.Z) <= Epsilon2;
}

inline bool UVsEqual(const FVector2D& V1, const FVector2D& V2)
{
	const float Epsilon3 = 1.0f / 1024.0f;
	return FMath::Abs(V1.X - V2.X) <= Epsilon3 && FMath::Abs(V1.Y - V2.Y) <= Epsilon3;
}

enum class ELightmapUVVersion : int32
{
	BitByBit = 0,
	Segments = 1,
	SmallChartPacking = 2,
	Latest = SmallChartPacking
};

class XRSTATICLIGHTING_API FArmyMeshUtilities
{
public:
	FArmyMeshUtilities() {}
	~FArmyMeshUtilities() {}

	static FArmyMeshUtilities& Get();

	/** 挑选出重叠面片 */
	void FindOverlappingCorners(TMultiMap<int32, int32>& OutOverlappingCorners, FRawMesh const& RawMesh, float ComparisonThreshold);

	/** 为传入的RawMesh计算光照UV */
	bool BuildMeshLightMapUV(FRawMesh& OutRawMesh, int32 InRes, FMatrix InMat, bool bInWrapFlat = false);

	/** 根据源数据创建生成RenderData */
	bool GenerateRenderingMeshes(FRawMesh& InRawMesh, FStaticMeshRenderData& OutRenderData);

	void BuildStaticMeshVertexAndIndexBuffers(
		TArray<struct FStaticMeshBuildVertex>& OutVertices,
		TArray<TArray<uint32> >& OutPerSectionIndices,
		TArray<int32>& OutWedgeMap,
		const FRawMesh& RawMesh,
		const TMultiMap<int32, int32>& OverlappingCorners,
		const TMap<uint32, uint32>& MaterialToSectionMapping,
		float ComparisonThreshold,
		FVector BuildScale,
		int32 ImportVersion
	);


private:
	static TSharedPtr<FArmyMeshUtilities> Singleton;

	TMultiMap<int32, int32> OverlappingCorners;
};