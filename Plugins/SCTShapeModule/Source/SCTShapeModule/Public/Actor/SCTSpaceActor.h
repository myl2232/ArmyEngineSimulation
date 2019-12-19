/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SCTInsideShape.h
* @Description 空间Actor
*
* @Author 赵志强
* @Date 2018年9月21日
* @Version 1.0
*/
#pragma once

#include "GameFramework/Actor.h"
#include "VertexType.h"
#include "SCTSpaceActor.generated.h"

class FSCTShape;
class FSCTOutline;

UCLASS()
class SCTSHAPEMODULE_API ASCTSpaceActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASCTSpaceActor();

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

	/** 设置材质 */
	void SetLineMaterial(UMaterialInterface* InMaterial);
	void SetRegionMaterial(UMaterialInterface* InMaterial);

	/** 设置线颜色 */
	void SetLineColor(const FLinearColor &InColor);
	void SetLineAlpha(float InAlpha);
	/** 设置面颜色和透明度 */
	void SetRegionColor(const FLinearColor &InColor);
	void SetRegionAlpha(float InAlpha);

	void SetCollisionProfileName(FName InProfileName);

	void SetIsEnableCollision(bool bEnable);

private:
	void UpdatePrimitive();
	void UpdatePrimitiveForNoneOutline(TSharedPtr<FSCTOutline> InOutline);
	void UpdatePrimitiveForLeftGirder(TSharedPtr<FSCTOutline> InOutline);
	void UpdatePrimitiveForRightGirder(TSharedPtr<FSCTOutline> InOutline);
	void UpdatePrimitiveForBackGirder(TSharedPtr<FSCTOutline> InOutline);
	void UpdatePrimitiveForLeftPiller(TSharedPtr<FSCTOutline> InOutline);
	void UpdatePrimitiveForRightPiller(TSharedPtr<FSCTOutline> InOutline);
	void UpdatePrimitiveForMiddlePiller(TSharedPtr<FSCTOutline> InOutline);
	void UpdatePrimitiveForCornerPiller(TSharedPtr<FSCTOutline> InOutline);

private:
	// 对应的Shape型录
	FSCTShape* ShapeData;

	// 包围盒
	class UPNTPrimitive *BoundingBoxPrimitive;
	// 外包框
	class UPNTPrimitive *WireframePrimitive;

	float fWidth, fDepth, fHeight;
};

