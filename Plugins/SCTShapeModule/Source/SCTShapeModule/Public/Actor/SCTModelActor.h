/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SCTInsideShape.h
* @Description 模型Actor
*
* @Author 赵志强
* @Date 2018年5月15日
* @Version 1.0
*/
#pragma once

#include "SCTShapeActor.h"
#include "SCTModelActor.generated.h"

class FAccessoryShape;

enum class ModelSacleFilter
{
	SCALE_X = 0x1,
	SCALE_Y = 0x2,
	SCALE_Z = 0x4,
	SCALE_XZ = SCALE_X | SCALE_Z,
	SCALE_XYZ = SCALE_X | SCALE_Y | SCALE_Z
};
inline bool operator &= (const ModelSacleFilter & a, const ModelSacleFilter & b)
{
	return (static_cast<int>(a) & static_cast<int>(b)) == static_cast<int>(b);
}

UCLASS()
class SCTSHAPEMODULE_API ASCTModelActor : public ASCTShapeActor
{
	GENERATED_BODY()
public:
	void SetScaleFilter(const ModelSacleFilter InScaleFilter) { ScaleFilter = InScaleFilter; }
	ModelSacleFilter GetScaleFilter() const { return ScaleFilter; }
private:
	ModelSacleFilter ScaleFilter = ModelSacleFilter::SCALE_XYZ;

public:
	ASCTModelActor();

	//void InitActor(bool InbHasChildren = false);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

public:
	virtual FBox GetCollisionBox() const override;

public:
	virtual void UpdateActorDimension() override;
 	//virtual void UpdateActorPosition() override;
 	//virtual void UpdateActorRotation() override;

	virtual void ChangeMaterial(UMaterialInterface* InMaterial);

	FVector GetModelMeshSize();
	const FVector & GetModelCenter() const { return MeshCenter; }

	/** 对于带动画的骨骼来说，获取的包围盒尺寸不一定是正确的，所以可以通过外部修正*/
	void SetFixedModelMeshCenterSize(const FVector & InCenter,const FVector & InSize);

	void SetStaticMesh(UStaticMesh* InStaticMesh, const TArray<TPair<FString, UMaterialInstanceDynamic*>> & InMaterialList);
	void SetSkeletalMesh(USkeletalMesh* InSkeletalMesh, UAnimationAsset *InAnimationAsset, const TArray<TPair<FString, UMaterialInstanceDynamic*>> & InMaterialList);

	void PlayAnimation(bool bForward);
	bool IsAnimationPlaying() const;
	float GetAnimationLength() const;

	//int32 GetNumChild() const;
	//class UStaticMeshComponent* GetChildSMC(int32 Index) const;
	UMaterialInterface* GetSingleMaterial();

	void SetSingleMaterialOffsetAndRepeatParameter(const float InOffsetU, const float InOffsetV, const float InRepeatU, const float InRepeatV);

	/** 旋转材质 */
	void SetSingleMaterialRotateParameter(const float InDegree);

	bool GetIsDelayReBuildBoundBox() const { return bDelayReBuildBoundBox; }
	void SetIsDelayReBuildBoundBox(const bool bInIsDelayRebuldBoundBox) { bDelayReBuildBoundBox = bInIsDelayRebuldBoundBox;}

	bool IsSkeletalMesh() const { return SkeletalModelMesh != nullptr; }
private:
	void SetMaterialList(const TArray<TPair<FString, UMaterialInstanceDynamic*>> & InMaterialList,const bool InIsSkeletalMesh);
	
private:
	class UStaticMeshComponent *ModelMesh;
	class USkeletalMeshComponent *SkeletalModelMesh;

	bool bDelayReBuildBoundBox = false;	/*!< 是否需要在Tick之后，重新计算一次包围盒尺寸信息 */

	//模型网格尺寸
	float MeshWidth;
	float MeshHeight;
	float MeshDepth;
	FVector MeshCenter;
};