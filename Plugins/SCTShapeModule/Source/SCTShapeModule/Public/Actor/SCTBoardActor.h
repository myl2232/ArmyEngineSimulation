/**
 * Copyright 2018 ����������(���)�Ƽ����޹�˾.
 * All Rights Reserved.
 * 
 *
 * @File BoardActor.h
 * @Description ���Actor
 *
 * @Author ��Ԯ
 * @Date 2018��5��4��
 * @Version 1.0
 */
#pragma once

#include "SCTShapeActor.h"
#include "TypedPrimitive.h"
#include "SCTBoardActor.generated.h"

class FSCTShape;
class FBoardShape;
class UPNTUVPrimitive;

UCLASS()
class SCTSHAPEMODULE_API ASCTBoardActor : public ASCTShapeActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASCTBoardActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual FBox GetCollisionBox() const override;
	virtual void UpdateActorDimension() override;
 	virtual void UpdateActorPosition() override;

	/** 更新板件材质 */
	virtual void UpdateActorBoardMaterial() override;


	/** 更新包边材质 */
	void UpdateActorCoveredEdgeMaterial();


	/** 更新铝箔 */
	void UpdateAluminumFildMaterial();


	/**
	 * @brief 更新封边材质
	 * @param[in] IsBoardMaterial 是否是同色的，如果为true，则使用板件的材质，否则使用封边对应的材质
	 */
	virtual void UpdateEdgeMaterial(const bool IsBoardMaterial = true) override;

	void SetBoardPointsAndThickness(float InThickness);
	//void SetBoardPoints(const TArray<FVector> &InCWPoints);
	void SetBoardThickness(float InThickness);
	void SetBoardMaterial(class UMaterialInterface *InOverrideMaterial);

	/**
	* 设置板件边材质
	*
	* @param[in]	InOverrideMaterial	材质实例
	*/
	void SetBoardEdgeMaterial(UMaterialInterface *InOverrideMaterial);

	/**
	* 设置板件边材质	
	* @param[in]	InOverrideMaterial	材质实例
	* @param[in]   InSpecialEdgeIndex 指定边
	*/
	void SetBoardEdgeMaterial(UMaterialInterface *InOverrideMaterial,const TArray<int32> & InSpecialEdgeIndex);


	/**
	 * 设置顶边材质
	 * @param[in] InOverrideMaterial 材质实例
	 */
	void SetBoardTopEdgeMaterial(UMaterialInterface *InOverrideMaterial);


	/**
	* 恢复到初始默认材质
	*/
	void RestoreEdgeMaterialToDefault() {}

	/** 
	 * @description 临时设置到板件Actor上面的材质,该材质并不会存储到Shape里面的，但是如果设置了，则优先使用
	 * @param 	MaterialInterface 材质实例
	 */
	void SetTemporaryMaterial(UMaterialInterface * MaterialInterface) { TemporaryMaterial = MaterialInterface; }
	void ResetTemporaryMaterial() { TemporaryMaterial = nullptr; }

	void SetCollisionProfileName(FName InCollisionName);


	void SetRotateTopBottomTexture(bool bInRotate);
	void SetRotateSideTexture(bool bInRotate);
	void SetDonotRepeatTexture(bool bInDonotRepeat); 

private:
	void UpdateBoardPrimitiveInfo();

	void CreateOrUpdatePrimitiveCompoent(const TArray<TArray<FPUVVertex>> & InTopAndBottomPUVArray,const TArray< TArray<uint16> > & InTopAndBottomIndexArray,
		const TArray<TArray<FPUVVertex>> & InEdgesPUVArray, const TArray< TArray<uint16> > & InEdgesIndexArray);

private:
	TArray<FVector> BoardCWPoints;   //顺时针方向
	TArray<TArray<FVector>> BoardHoleCWPoint;
	TArray< TArray<FVector> > BoardCWPointsArray;	

	float BoardThickness;
	uint8 bRotateTopBottomTexture : 1;
	uint8 bRotateSideTexture : 1;
	uint8 bDonotRepeatTexture : 1;


	/// 目前为止会存储底面与顶面
	/// 索引 0 -- 底面
	/// 索引 1 -- 顶面
	TArray<UPNTUVPrimitive*> BoardPrimitiveArray;
	
	/// 用来存储边 -- 从左下里为起点，顺时针绕来记录边
	TArray<UPNTUVPrimitive*> BoardEdgePrimitiveArray;

	bool bIsFirstCreateCompoent ;
	/** 用于临时改变Actor的材质，改材质的信息，并不会存储到Shape里面，如果存在,则优先使用*/
	UMaterialInterface * TemporaryMaterial = nullptr;
};