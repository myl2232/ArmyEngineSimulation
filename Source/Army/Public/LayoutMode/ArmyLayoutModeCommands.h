/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRLayoutModeCommands.h
 * @Description 拆改户型操作命令
 *
 * @Author 欧石楠
 * @Date 2018年6月9日
 * @Version 1.0
 */

#pragma once

#include "Commands.h"
#include "ArmyStyle.h"

class FArmyLayoutModeCommands : public TCommands<FArmyLayoutModeCommands>
{
public:
    FArmyLayoutModeCommands()
        : TCommands<FArmyLayoutModeCommands>(
            TEXT("LayoutMode"),
            NSLOCTEXT("Contexts", "LayoutMode", "Layout Mode"),
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
	//TSharedPtr<FUICommandInfo> CommandUndo;			// 撤销
	//TSharedPtr<FUICommandInfo> CommandRedo;			// 重做
	
	TSharedPtr<FUICommandInfo> CommandEmpty;		// 清空
	TSharedPtr<FUICommandInfo> CommandDelete;		// 删除

    /** 拆改中户型状态 */
    TSharedPtr<FUICommandInfo> CommandDismantle;

    /** 拆改后户型状态 */
    TSharedPtr<FUICommandInfo> CommandAfterDismantle;

public:
	/** 拆改 */
	TSharedPtr<FUICommandInfo> CommandNewWall; // 新建墙体
	TSharedPtr<FUICommandInfo> CommandDismantleWall; // 拆除墙体
	TSharedPtr<FUICommandInfo> CommandNewPass;// 新开垭口
	TSharedPtr<FUICommandInfo> CommandPackPipe;// 包立管
	TSharedPtr<FUICommandInfo> CommandCurtainBox;// 窗帘盒子	

    /** 门窗 */
    TSharedPtr<FUICommandInfo> CommandDoor; // 标准门
    TSharedPtr<FUICommandInfo> CommandSlidingDoor; // 推拉门	

	/** @纪仁泽 区域分割 */
	TSharedPtr<FUICommandInfo> CommandSplitRegion;//进行区域分割

};