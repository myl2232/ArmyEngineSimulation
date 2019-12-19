/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SCTInsideShape.h
* @Description 型录Actor基类：用于显示
*
* @Author 赵志强
* @Date 2018年5月15日
* @Version 1.0
*/
#pragma once

#include "GameFramework/Actor.h"
#include "SCTShapeActor.generated.h"

class FSCTShape;

/**
 * Actor移动方向
 */
enum EActorMoveDir : uint8
{
    MD_None,
	MD_UpDown,
	MD_LeftRight,
	MD_FrontBack,
	MD_Max
};

//Actor基类
UCLASS()
class SCTSHAPEMODULE_API ASCTShapeActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASCTShapeActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

public:
	virtual void NotifyActorBeginCursorOver() override;
	virtual void NotifyActorEndCursorOver() override;
	virtual void NotifyActorOnClicked(FKey ButtonPressed /* = EKeys::LeftMouseButton */) override;
	virtual void NotifyActorOnReleased(FKey ButtonReleased /* = EKeys::LeftMouseButton */) override;

	/** 递归隐藏ShapeActor */
	virtual void SetShapeActorHiddenInGameRecursively(bool bHidden);

public:
	/** 获取碰撞包围盒 */
	virtual FBox GetCollisionBox() const { return FBox(ForceInit); };

public:
	//更新Actor尺寸
	virtual void UpdateActorDimension();
	virtual void UpdateActorPosition();
	virtual void UpdateActorRotation();

	/**
	* @brief 更新封边材质
	* @param[in] IsBoardMaterial 是否是同色的，如果为true，则使用板件的材质，否则使用封边对应的材质
	*/
	virtual void UpdateEdgeMaterial(const bool IsBoardMaterial = true) {}
	virtual void UpdateActorBoardMaterial() {}


	void SetActorPosition(const FVector& InPos);
	FVector GetActorPosition() const;

	void SetActorRotator(const FRotator& InRotator);
	FRotator GetActorRotator() const;

	void SetShapeMoveDir(EActorMoveDir InDir);
	EActorMoveDir GetShapeMoveDir();

	FVector GetBoxCenter() const;

	void UpdateActorMouseDragState();
	void SetCanDragByMouse(bool bDragByMouse) { bCanDragByMouse = bDragByMouse; }
	bool CanDragByMouse() const { return bCanDragByMouse; }

	void SetShape(FSCTShape* InShape);
	FSCTShape* GetShape();

	void AttachToActorOverride(ASCTShapeActor *ParentActor, 
		const FAttachmentTransformRules &AttachmentRules, FName SockerName = NAME_None);

	void DetachFromActorOverride(const FDetachmentTransformRules& DetachmentRules);

	ASCTShapeActor* GetActorAttachTo() { return ActorAttachedTo; }
	bool GetVisibleStateFromParent() const;

	void DestroyActorWithChildren();
protected:
	void DestroyActorWithChildrenRecursive(AActor *Actor);

protected:
	EActorMoveDir ActorMoveDir;
	bool bCanDragByMouse;

	// 对应的Shape型录
	FSCTShape* ShapeData;
	// 关联的父级型录
	ASCTShapeActor* ActorAttachedTo;
};

