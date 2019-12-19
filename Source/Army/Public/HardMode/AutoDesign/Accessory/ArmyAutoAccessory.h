/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRAutoAccessory.h
* @Description 智能设计-配饰布局
*
* @Author 纪仁泽 程泽
* @Date 2019年1月16日
* @Version 1.0
*/

/*

8种房间的配饰（或卫生间软装）汇总如下：

一、卧室（主卧、次卧、儿童房、客卧）
1、吊灯、吸顶灯、主灯【依赖于墙高】
2、普通窗帘【依赖于窗户、墙高、吊顶、顶角线】
3、挂画【依赖于床（双人床、单人床）】
4、地毯【依赖于床（双人床、单人床）】
5、台灯【依赖于左床头柜】【依赖于书桌】
6、挂壁电视【依赖于床、电视柜】
7、立式电视【依赖于电视柜】
8、挂壁空调【依赖于最大进光墙、床头】
9、壁灯【依赖于床头柜】

卧室软装（梳妆凳）
1、梳妆凳（依赖于梳妆台）
2、书椅（依赖于书桌）

二、卫生间（卫生间、主卫生间、客卫生间）
1、百叶帘【依赖于窗户】
2、集成LED灯、主灯【依赖于墙高、最长墙方向】
3、浴巾架【依赖于马桶，墙角柱子（柱子、包立管、风道）】
4、毛巾架【依赖于浴室柜或马桶，墙角柱子（柱子、包立管、风道）】
5、厕纸架【依赖于马桶，墙角柱子（柱子、包立管、风道）】

卫生间软装（马桶、浴室柜、花洒、淋浴房、洗衣机）
1、马桶【依赖于马桶下水、最近墙垂直方向】
2、浴室柜【依赖于离门最近墙角、墙角柱子（柱子、包立管、风道）、避开正对马桶】
3、花洒【依赖于离门最远墙角、窗户所在墙、墙角柱子（柱子、包立管、风道）】
4、淋浴房【依赖于离门最远墙角、墙角柱子（柱子、包立管、风道）】
5、洗衣机【依赖于墙角、墙角柱子（柱子、包立管、风道）】

三、书房
1、吊灯、吸顶灯、主灯【依赖于墙高】
2、普通窗帘【依赖于窗户、墙高、吊顶、顶角线】
3、挂画【依赖于书桌】
4、地毯【依赖于书桌】
5、台灯【依赖于书桌】
6、挂壁空调【依赖于最大进光墙】

书房软装（书椅）
1、书椅（依赖于书桌）

四、客厅
1、吊灯、吸顶灯、主灯【依赖于墙高】
2、普通窗帘【依赖于窗户、墙高、吊顶、顶角线】
3、挂画【依赖于主位沙发】
4、地毯【依赖于主位沙发】
5、台灯【依赖于边几】
6、落地灯【依赖于主位沙发、边几】
7、绿植【依赖于主位沙发、边几|电视柜】
8、立式空调【依赖于主位沙发、最大进光墙】
9、挂壁空调【依赖于主位沙发、最大进光墙】
10、立式电视【依赖于电视柜】
11、挂壁电视【依赖于电视柜、主位沙发】
12、壁灯【依赖于硬装背景墙】

客厅软装（边几）
1、边几（垂直和水平偏移50mm）

五、餐厅
1、吊灯、吸顶灯、主灯【依赖于墙高、餐桌】
2、普通窗帘【依赖于窗户、墙高、吊顶、顶角线】
3、挂画【依赖于餐桌、餐边柜】
4、地毯【依赖于餐桌】
5、餐具【依赖于餐桌和餐椅】

餐厅软装（餐椅）
1、餐椅（依赖于餐桌）

六、阳台（阳台、后阳台、卧室阳台）
1、吊灯、吸顶灯、主灯【依赖于墙高】
2、绿植【依赖于避开门（门、门洞、推拉门、垭口灯）的墙角】
3、晾衣杆【依赖于墙高、平行于最大窗户】

阳台软装（边几和休闲椅）
1、边几（休闲椅）(边几在阳台中心点，休闲椅紧靠边几方向平行于阳台长边)
2、休闲椅

七、玄关
1、玄关小地毯【依赖于入户门】
2、吊灯、吸顶灯、主灯【依赖于墙高】

八、厨房
1、集成LED灯、主灯【依赖于墙高、最长墙方向】

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


DECLARE_LOG_CATEGORY_EXTERN(AutoAccessory, Log, All);

/** 构建配饰空间基类 @纪仁泽 */
class FArmyAutoAccessory
{
public:
	//FArmyAutoAccessory() {};
	//virtual ~FArmyAutoAccessory() {};

public:
	/** 分发函数 */
	virtual void CalcAccessoryLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray) = 0;

protected:
	/* 获取进光墙 */
	TSharedPtr<FArmyWallLine> LightAreaMaxWallLine(const TSharedPtr<FArmyRoom> Room);
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

	/**主灯： 吊灯 吸顶灯 集成LED灯*/
	virtual void CalcLampMain(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 标准窗帘 */
	virtual void CalcCurtainStd(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

private:

};

class FArmyAccessoryFactory
{
public:
	FArmyAccessoryFactory() {};
	~FArmyAccessoryFactory() {};

	static const TSharedRef<FArmyAccessoryFactory>& Get();

	TSharedRef<FArmyAutoAccessory>& GetAccessoryRoom(TSharedPtr<FArmyRoomEntity>& InRoom);

private:

};

/** 卧室 */
class FArmyAutoAccessoryRoom : public FArmyAutoAccessory
{
public:
	/** 分发函数 */
	virtual void CalcAccessoryLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray) override;

private:
};

/** 卧室 */
class FArmyAutoAccessoryBedroom :public FArmyAutoAccessory
{
public:
	/** 分发函数 */
	virtual void CalcAccessoryLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray) override;

private:

	/** 卧室 地毯 */
	void CalcCarpetLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);
	
	/** 卧室 台灯 */
	void CalcLampTable(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);
	
	/** 卧室 壁灯 */
	void CalcLampWall(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 卧室 挂画 */
	void CalcTitanic(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 卧室 壁挂空调 */
	void CalcAirConditionedHang(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 卧室 壁挂电视 */
	void CalcTVWall(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 卧室 立式电视 */
	void CalcTVVert(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);
};

/** 书房 */
class FArmyAutoAccessoryStudyroom :public FArmyAutoAccessory
{
public:
	/** 分发函数 */
	virtual void CalcAccessoryLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray) override;

private:
	/** 书房 地毯 */
	void CalcCarpetLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 书房 台灯 */
	void CalcLampTable(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 书房 挂画 */
	void CalcTitanic(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 书房 壁挂空调 */
	void CalcAirConditionedHang(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);
};

/** 客厅 */
class FArmyAutoAccessoryLivingroom :public FArmyAutoAccessory
{
public:
	/** 分发函数 */
	virtual void CalcAccessoryLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray) override;


private:
	/** 客厅 地毯 */
	void CalcCarpetLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 客厅 台灯 */
	void CalcLampTable(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 客厅壁灯 */
	void CalcLampWall(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 客厅 落地灯 */
	void CalcLampFloorDown(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 客厅 绿植 */
	void CalcGreenPlants(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 客厅 挂画 */
	void CalcTitanic(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 客厅 立式空调 */
	void CalcAirConditionedVert(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 客厅 壁挂空调 */
	void CalcAirConditionedHang(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 客厅 壁挂电视 */
	void CalcTVWall(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 客厅 立式电视 */
	void CalcTVVert(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

};

/** 卫生间 */
class FArmyAutoAccessoryBathRoom :public FArmyAutoAccessory
{
public:
	/** 分发函数 */
	virtual void CalcAccessoryLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray) override;

private:

	/** 卫生间 百叶窗 */
	void CalcShutters(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);
	
	/** 卫生间 浴巾架 */
	void CalcBathTowelHolder(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 卫生间 毛巾架 */
	void CalcFaceclothHolder(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 卫生间 厕纸架 */
	void CalcToiletPaperHolder(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);
};


/** 阳台 */
class FArmyAutoAccessoryBalcony :public FArmyAutoAccessory
{
public:
	/** 分发函数 */
	virtual void CalcAccessoryLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray) override;

private:
	/** 阳台 绿植 */
	void CalcGreenPlants(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 阳台 晾衣杆 */
	void CalcClotheslinePole(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

};


/** 厨房 */
class FArmyAutoAccessoryKitchen :public FArmyAutoAccessory
{
public:
	/** 分发函数 */
	virtual void CalcAccessoryLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray) override;

private:
};

/** 餐厅 */
class FArmyAutoAccessoryDiningroom :public FArmyAutoAccessory
{
public:
	/** 分发函数 */
	virtual void CalcAccessoryLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray) override;

private:
	/** 吊灯（餐桌上方） */
	virtual void CalcLampMain(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray) override;

	/** 餐厅 地毯 */
	void CalcCarpetLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 餐厅 挂画 */
	void CalcTitanic(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);

	/** 餐厅 餐具 */
	void CalcDinnerware(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);
};

/** 玄关 */
class FArmyAutoAccessoryHallway :public FArmyAutoAccessory
{
public:
	/** 分发函数 */
	virtual void CalcAccessoryLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray) override;

private:
	/** 玄关 地毯 */
	void CalcCarpetLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray);
};



