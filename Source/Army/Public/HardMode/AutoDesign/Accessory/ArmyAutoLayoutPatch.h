/**
* Copyright 2019 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRAutoLayoutPatch.h
* @Description 智能设计-配饰布局
*
* @Author 程泽
* @Date 2019年3月27日
* @Version 1.0
*/

/*

8种房间的配饰（或卫生间软装）汇总如下：

一、卧室（主卧、次卧、儿童房、客卧）

1、梳妆凳【依赖于梳妆台】
2、书椅【依赖于书桌】

二、卫生间（卫生间、主卫生间、客卫生间）

1、马桶【依赖于马桶下水、最近墙垂直方向】
2、浴室柜【依赖于离门最近墙角、墙角柱子（柱子、包立管、风道）、避开正对马桶】（有脚和无脚两种）
3、淋浴房【依赖于离门最远墙角、墙角柱子（柱子、包立管、风道）】
4、花洒【依赖于离门最远墙角、窗户所在墙、墙角柱子（柱子、包立管、风道）】
5、洗衣机【依赖于墙角、墙角柱子（柱子、包立管、风道）】

三、书房

1、书椅【依赖于书桌】

四、客厅

1、边几【依赖于主位沙发】

五、餐厅

1、餐椅【依赖于餐桌】

六、阳台（阳台、后阳台、卧室阳台）

1、边几
2、休闲椅

七、玄关

八、厨房

|| CurrentComponentId == AI_Closestool \
|| CurrentComponentId == AI_Sprinkler \
|| CurrentComponentId == AI_ShowerRoom \
|| CurrentComponentId == AI_BathroomArk \
|| CurrentComponentId == AI_BathroomArk_Wall \
|| CurrentComponentId == AI_WashingMachine \
|| CurrentComponentId == AI_SideTable\
|| CurrentComponentId == AI_LeisureChair\
|| CurrentComponentId == AI_BookChair\
|| CurrentComponentId == AI_DiningChair\
|| CurrentComponentId == AI_DressingStool

*/

#pragma once

#include "ArmyAutoDesignSettings.h"
#include "ArmyModelEntity.h"
#include "ArmyRoomEntity.h"
#include "ArmyDataTools.h"
#include "ArmySceneData.h"
#include "ArmyActorItem.h"
#include "ArmyCommonTypes.h"
#include "ArmyResourceModule.h"
#include "ArmyGameInstance.h"

/** 构建配饰空间基类 */
class FArmyAutoLayoutPatch
{
public:
	//FArmyAutoLayoutPatch() {};
	//virtual ~FArmyAutoLayoutPatch() {};

public:
	/** 分发函数 */
	virtual void CalcLayoutPatch(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray) = 0;

protected:
	/* 检测某个模型与柱子、包立管、风道的碰撞，碰撞了返回true */
	bool CheckWallCornerCollision(AActor* ActorItr, TSharedPtr<FArmyRoomEntity> InRoom);
	/* 检测某个模型与顶面的碰撞，碰撞了返回true */
	bool CheckCeilingCollision(AActor* ActorItr, TSharedPtr<FArmyRoomEntity> InRoom);
	/* 检测某个模型与墙体的碰撞，碰撞了返回true */
	bool CheckWallCollision(AActor* ActorItr, TSharedPtr<FArmyRoomEntity> InRoom);
	/* 检测某个模型与墙上物件的碰撞，碰撞了返回true */
	bool CheckWallObjectCollision(AActor* ActorItr, TSharedPtr<FArmyRoomEntity> InRoom);
	/* 检测某个模型与其他模型的碰撞，碰撞了返回true */
	bool CheckOtherActorCollision(AActor* ActorItr, TSharedPtr<FArmyRoomEntity> InRoom, const TSet<AActor*>& IgnoreActorArr);

private:

};

class FArmyLayoutPatchFactory
{
public:
	FArmyLayoutPatchFactory() {};
	~FArmyLayoutPatchFactory() {};

	static const TSharedRef<FArmyLayoutPatchFactory>& Get();

	TSharedRef<FArmyAutoLayoutPatch>& GetLayoutPatchRoom(TSharedPtr<FArmyRoomEntity>& InRoom);

private:

};

/** 房间 */
class FArmyAutoLayoutPatchRoom : public FArmyAutoLayoutPatch
{
public:
	/** 分发函数 */
	virtual void CalcLayoutPatch(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray) override;

private:
};

/** 卧室 */
class FArmyAutoLayoutPatchBedroom :public FArmyAutoLayoutPatch
{
public:
	/** 分发函数 */
	virtual void CalcLayoutPatch(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray) override;

private:

	/** 卧室 梳妆台椅子 */
	void CalcChairLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);
	/** 卧室 书桌椅子 */
	void CalcDeskChairLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

};

/** 书房 */
class FArmyAutoLayoutPatchStudyroom :public FArmyAutoLayoutPatch
{
public:
	/** 分发函数 */
	virtual void CalcLayoutPatch(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray) override;

private:

	/** 书房 椅子 */
	void CalcChairLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

};

/** 客厅 */
class FArmyAutoLayoutPatchLivingroom :public FArmyAutoLayoutPatch
{
public:
	/** 分发函数 */
	virtual void CalcLayoutPatch(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray) override;


private:

	/** 客厅 边几（软装） */
	void CalcSideTableLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

};

/** 卫生间 */
class FArmyAutoLayoutPatchBathRoom :public FArmyAutoLayoutPatch
{
public:
	/** 分发函数 */
	virtual void CalcLayoutPatch(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray) override;

private:
	
	//马桶、花洒、淋浴房、浴室柜、洗衣机，作为配饰来做
	/** 卫生间 马桶 */
	void CalcToilet(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);
	/** 卫生间 花洒 */
	void CalcShowerSprinkler(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);
	/** 卫生间 淋浴房 */
	void CalcShower(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);
	/** 卫生间 浴室柜 */
	void CalcCabinet(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);
	/** 卫生间 洗衣机 */
	void CalcWasher(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);
};


/** 阳台 */
class FArmyAutoLayoutPatchBalcony :public FArmyAutoLayoutPatch
{
public:
	/** 分发函数 */
	virtual void CalcLayoutPatch(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray) override;

private:
	
	/** 阳台 边几（软装） */
	void CalcSideTableLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 阳台 休闲椅（软装） */
	void CalcLeisureChairLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);
};

/** 餐厅 */
class FArmyAutoLayoutPatchDiningroom :public FArmyAutoLayoutPatch
{
public:
	/** 分发函数 */
	virtual void CalcLayoutPatch(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray) override;

private:

	/** 餐厅 椅子 */
	void CalcChairLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

};



