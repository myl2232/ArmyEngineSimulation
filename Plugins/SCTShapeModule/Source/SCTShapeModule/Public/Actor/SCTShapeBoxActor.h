/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SCTInsideShape.h
* @Description 型录外包框
*
* @Author 赵志强
* @Date 2018年6月11日
* @Version 1.0
*/
#pragma once

#include "GameFramework/Actor.h"
#include "VertexType.h"
#include "SCTShapeBoxActor.generated.h"

class FSCTShape;

UCLASS()
class SCTSHAPEMODULE_API ASCTBoundingBoxActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASCTBoundingBoxActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

public:
	/** 设置关联的ShapeData */
	void SetShape(FSCTShape* InShape);
	FSCTShape* GetShape();

	/** 更新外包框尺寸 */
	void UpdateActorDimension();
	/** 设置外包框尺寸 */
	void SetActorDimension(float InWidth, float InDepth, float InHeight);
	void SetActorDimension(const FVector & InCenter, const FVector & InMinPoint, const FVector & InMaxPoint);

	/** 设置材质 */
	void SetLineMaterial(UMaterialInterface* InMaterial);
	void SetRegionMaterial(UMaterialInterface* InMaterial);

	/** 设置线颜色 */
	void SetLineColor(const FLinearColor &InColor);
	void SetLineAlpha(float InAlpha);
	/** 设置面颜色和透明度 */
	void SetRegionColor(const FLinearColor &InColor);
	void SetRegionAlpha(float InAlpha);

	void SetIsEnableCollision(bool bEnable);

	void SetCollisionProfileName(FName InProfileName);

	/** 当Shape的尺寸信息发生改变之后，是否需要进一步获取Shape的尺寸来更细包围盒*/
	bool GetIsNeedGetShapeSize() const { return bNeedGetShapeSize; }
	void SetIsNeedGetShapeSize(const bool bInIsNeed) { bNeedGetShapeSize = bInIsNeed; }

private:
	void UpdatePrimitive();

private:
	// 对应的Shape型录
	FSCTShape* ShapeData;

	//
	class UPNTPrimitive *BoundingBoxPrimitive;
	class UPNTPrimitive *WireframePrimitive;

	float fWidth, fDepth, fHeight;
	FVector BoundBoxMinPoint = FVector::ZeroVector;
	FVector BoundBoxMaxPoint = FVector::ZeroVector;
	FVector BoundBoxCenter = FVector::ZeroVector;
	// 是否需要获取Shape的尺寸,对于五金而言，在更新尺寸的时候，不需要再进一步获取shape的尺寸了，因为五金对应的ShapeActor会应用缩放
	bool bNeedGetShapeSize = true;	
};

UCLASS()
class SCTSHAPEMODULE_API ASCTWireframeActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASCTWireframeActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

public:
	// 更新外包框尺寸
	void UpdateActorDimension();
	// 设置外包框尺寸
	void SetActorDimension(float InWidth, float InDepth, float InHeight);
	void SetActorDimension(const FVector & InCenter,const FVector & InMinPoint,const FVector & InMaxPoint);

	/** 设置材质 */
	void SetLineMaterial(UMaterialInterface* InMaterial);

	/** 设置外包框颜色 */
	void SetLineColor(const FLinearColor &InColor);

	//设置外包框线宽
	void SetLineWidth(float InValue);

	/** 设置可碰撞性 */
	void SetIsEnableCollision(bool bEnable);

	void SetCollisionProfileName(FName InProfileName);

	/** 设置关联的ShapeData */
	void SetShape(FSCTShape* InShape);
	FSCTShape* GetShape();

	/** 设置自定义深度模板值 */
	//void SetCustomDepthStencilValue(int32 InValue);

	/** 当Shape的尺寸信息发生改变之后，是否需要进一步获取Shape的尺寸来更细包围盒*/
	bool GetIsNeedGetShapeSize() const { return bNeedGetShapeSize; }
	void SetIsNeedGetShapeSize(const bool bInIsNeed) { bNeedGetShapeSize = bInIsNeed; }

	bool IsDynamicScaleLineSizeFollowCamera() const { return bDynamicScaleLineSizeFollowCamera; }
	void SetIsIsDynamicScaleLineSizeFollowCamera(const bool InbIsDynamicScaleLineSize) { bDynamicScaleLineSizeFollowCamera = InbIsDynamicScaleLineSize; }

private:
	void UpdateBBoxEx();
	void UpdateBBox();
	void GenerateLineBox(TArray<FPVertex>& Vertices, TArray<uint16>& LineIndices,
		const FVector& InLineStart, const FVector& InLineEnd);
	void GenerateLineBox10(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices);
	void GenerateLineBox12(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices);
	void GenerateLineBox03(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices);
	void GenerateLineBox23(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices);
	void GenerateLineBox54(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices);
	void GenerateLineBox56(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices);
	void GenerateLineBox47(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices);
	void GenerateLineBox67(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices);
	void GenerateLineBox04(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices);
	void GenerateLineBox15(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices);
	void GenerateLineBox26(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices);
	void GenerateLineBox37(TArray<FPVertex>& OutVertices, TArray<uint16>& OutIndices);
	void AddIndices(TArray<uint16>& OutIndices, int32 PntCount);

private:
	//
	class UPNTPrimitive *WireframePrimitive;

	// 对应的Shape型录
	FSCTShape* ShapeData;

	//Actor尺寸
	float fWidth, fDepth, fHeight;
	FVector BoundBoxMinPoint = FVector::ZeroVector;
	FVector BoundBoxMaxPoint = FVector::ZeroVector;
	FVector BoundBoxCenter = FVector::ZeroVector;
	//线宽
	float LineWidth;
	float LineScale = 1.0f;

	//临时变量
	TArray<FVector> Positions;
	TArray<uint16> Indices;

	// 是否需要获取Shape的尺寸,对于五金而言，在更新尺寸的时候，不需要再进一步获取shape的尺寸了，因为五金对应的ShapeActor会应用缩放
	bool bNeedGetShapeSize = true;

	bool bDynamicScaleLineSizeFollowCamera = true;   /*!< 线条宽度是否跟随这相机进行缩放 */
};
