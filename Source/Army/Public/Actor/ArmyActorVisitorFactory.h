/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRActorVisitorFactory.h
 * @Description Actor访问管理器创建工厂
 *
 * @Author 欧石楠
 * @Date 2018年12月7日
 * @Version 1.0
 */

#pragma once

#include "ArmyActorConstant.h"
#include "IArmyActorVisitor.h"

enum EXRActorVisitor
{
    /** 立面模式 */
    AV_HardMode,

    /** 立面子模式 */
    AV_HardSubMode,

    /** 水电模式 */
    AV_HydropowerMode,

    /** 木作模式 */
    AV_WHCMode,

    /** 立面模式平面视图 */
    AV_TopHardMode,

    /** 水电模式平面视图 */
    AV_TopHydropowerMode,

	/** 立面模式顶面视图 */
	AV_HardModeCeilingMode,

    /** 模型 */
    AV_Model,

    /** 原始点位 */
    AV_OriginalPoint,

	/** 外墙 */
	AV_OuterWall,

	/** 原始墙 */
	AV_OriginalWall,

	/** 木作物体 */
	AV_WHCActor,

	/** 半透明吊顶 */
	AV_TranslucentRoofActor,

    /** 门窗 */
    AV_HardwareActor,

    /** 光源图标 */
    AV_LightIcon,
};

class FArmyActorVisitorFactory
{
public:
    static FArmyActorVisitorFactory& Get();

    /** 创建访问管理器 */
    TSharedPtr<IArmyActorVisitor> CreateVisitor(EXRActorVisitor ActorVisitor);
	
private:
	static TSharedPtr<FArmyActorVisitorFactory> Instance;
};