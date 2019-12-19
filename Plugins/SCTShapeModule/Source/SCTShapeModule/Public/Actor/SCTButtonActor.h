/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SCTButtonActor.h
* @Description ButtonActor
*
* @Author 赵志强
* @Date 2018年6月22日
* @Version 1.0
*/
#pragma once

#include "GameFramework/Actor.h"
#include "SCTButtonActor.generated.h"

class FSCTShape;

UCLASS()
class SCTSHAPEMODULE_API ASCTButtonActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASCTButtonActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

public:
	void SetCollisionProfileName(FName InCollisionProfileName);

	void SetShapeData(FSCTShape* InShape);
	void SetRefName(const FString& InName);
	const FString& GetRefName() const;

	void SetDimension(float InWidth, float InDepth, float InHeight);

	void SetLineColor(const FLinearColor &InColor);
	void SetRegionColor(const FLinearColor &InColor);

private:
	void UpdateBBox();

private:
	class UPNTPrimitive *BoundingPrimitive;
	class UPNTPrimitive *WireframePrimitive;

	float fWidth, fDepth, fHeight;

	FSCTShape* ShapeData;
	FString RefName;
};