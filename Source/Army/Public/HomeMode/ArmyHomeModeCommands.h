/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRHomeModeCommands.h
 * @Description 户型绘制操作命令
 *
 * @Author 欧石楠
 * @Date 2018年6月9日
 * @Version 1.0
 */

#pragma once

#include "ArmyStyle.h"

#include "Commands.h"

class FArmyHomeModeCommands : public TCommands<FArmyHomeModeCommands>
{
public:
    FArmyHomeModeCommands()
        : TCommands<FArmyHomeModeCommands>(
            TEXT("HomeMode"),
            NSLOCTEXT("Contexts", "HomeMode", "Home Mode"),
            NAME_None,
            FArmyStyle::Get().GetStyleSetName()
            )
    {}

    //~ Begin TCommand<> interface
    virtual void RegisterCommands() override;
    //~ End TCommand<> interface
public:
	/** @欧石楠 ToolBar Commands*/	
	TSharedPtr<FUICommandInfo> CommandSave;			// 保存
	TSharedPtr<FUICommandInfo> CommandUndo;			// 撤销
	TSharedPtr<FUICommandInfo> CommandRedo;			// 重做
	
	TSharedPtr<FUICommandInfo> CommandHouseType;	// 户型
	TSharedPtr<FUICommandInfo> CommandEmpty;		// 清空
	TSharedPtr<FUICommandInfo> CommandDelete;		// 删除
    TSharedPtr<FUICommandInfo> CommandImportFacsimile;      // 导入底图
    TSharedPtr<FUICommandInfo> CommandSetFacsimileScale;    // 设置比例尺
	TSharedPtr<FUICommandInfo> CommandOffset;		// 偏移
	TSharedPtr<FUICommandInfo> CommandCreateOutWall;		// 生成外墙
	TSharedPtr<FUICommandInfo> CommandDeleteOutWall;		// 删除外墙

public:
    /** 建筑 */
	TSharedPtr<FUICommandInfo> CommandInternalWall; // 原始内墙	
	TSharedPtr<FUICommandInfo> CommandBalconyPunch; // 开阳台
	TSharedPtr<FUICommandInfo> CommandPass; // 垭口/门洞
	TSharedPtr<FUICommandInfo> CommandSecurityDoor; //防盗门

	/** 拆改 */
	TSharedPtr<FUICommandInfo> CommandNewWall; // 新建墙体
	TSharedPtr<FUICommandInfo> CommandDismantleWall; // 拆除墙体

	TSharedPtr<FUICommandInfo> CommandOriginalWall; // 原始墙体	
	TSharedPtr<FUICommandInfo> CommandBearingWall; // 承重墙
	TSharedPtr<FUICommandInfo> CommandSpaceLine; // 空间线

    /** 门窗 */
    TSharedPtr<FUICommandInfo> CommandDoor; // 标准门
    TSharedPtr<FUICommandInfo> CommandSlidingDoor; // 推拉门
    
	
    TSharedPtr<FUICommandInfo> CommandWindow; // 标准窗
	TSharedPtr<FUICommandInfo> CommandFloorWindow; // 落地窗
    TSharedPtr<FUICommandInfo> CommandBayWindow; // 标准飘窗
	TSharedPtr<FUICommandInfo> CommandCornerBayWindow; // 转角飘窗
	TSharedPtr<FUICommandInfo> CommandTrapezoidBayWindow; // 梯形飘窗
	TSharedPtr<FUICommandInfo> CommandArcWindow; // 弧形窗

	TSharedPtr<FUICommandInfo> CommandPillar;// 柱子		
	TSharedPtr<FUICommandInfo> CommandAirFlue;// 风道	
	TSharedPtr<FUICommandInfo> CommandBeam;// 梁		
	TSharedPtr<FUICommandInfo> CommandAirLouver;// 空调孔
	/**点位*/
	TSharedPtr<FUICommandInfo> CommandDiversityWaterDevice; // 分集水器
	//TSharedPtr<FUICommandInfo> CommandMainBlowOffPipe; // 排污管主管	
	TSharedPtr<FUICommandInfo> CommandDewateringPipe; // 下水管
	TSharedPtr<FUICommandInfo> CommandRainPipe; // 雨水管
	TSharedPtr<FUICommandInfo> CommandClosestool;// 坐便下水	
	TSharedPtr<FUICommandInfo> CommandGasMeter; // 燃气表
	TSharedPtr<FUICommandInfo> CommandGasMainPipe; // 燃气主管道
	TSharedPtr<FUICommandInfo> CommandHighElvBox; // 强电箱
	TSharedPtr<FUICommandInfo> CommandLowElvBox; // 弱电箱
	TSharedPtr<FUICommandInfo> CommandDrainPoint; // 下水主管道
	TSharedPtr<FUICommandInfo> CommandWitch1O2Point; // 单联双控开关
	TSharedPtr<FUICommandInfo> CommandWaterChilledPoint; // 冷水点位
	TSharedPtr<FUICommandInfo> CommandWaterHotPoint; // 热水点位
	TSharedPtr<FUICommandInfo> CommandWaterSeparatorPoint; // 分集水器
	TSharedPtr<FUICommandInfo> CommandWaterNormalPoint; // 中水点位
	TSharedPtr<FUICommandInfo> CommandBasin; // 地漏下水
	TSharedPtr<FUICommandInfo> CommandEnterRoomWater; // 入户水点位
};