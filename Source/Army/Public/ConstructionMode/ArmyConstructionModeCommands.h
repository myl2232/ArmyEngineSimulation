/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRConstructionModeCommands.h
 * @Description 户型绘制操作命令
 *
 * @Author 欧石楠
 * @Date 2018年6月9日
 * @Version 1.0
 */

#pragma once

#include "ArmyStyle.h"

#include "Commands.h"

class FArmyConstructionCommands : public TCommands<FArmyConstructionCommands>
{
public:
    FArmyConstructionCommands()
        : TCommands<FArmyConstructionCommands>(
            TEXT("ConstructionMode"),
            NSLOCTEXT("Contexts", "ConstructionMode", "Construction Mode"),
            NAME_None,
            FArmyStyle::Get().GetStyleSetName()
            )
    {}

    //~ Begin TCommand<> interface
    virtual void RegisterCommands() override;
    //~ End TCommand<> interface

public:
    TSharedPtr<FUICommandInfo> CommandSave; // 保存
	TSharedPtr<FUICommandInfo> CommandDelete; // 删除

	TSharedPtr<FUICommandInfo> CommandUndo;	// 撤销
	TSharedPtr<FUICommandInfo> CommandRedo;	// 重做

	TSharedPtr<FUICommandInfo> CommandLoadFrame; // 图框
	TSharedPtr<FUICommandInfo> CommandSaveImage; // 生成图纸
	TSharedPtr<FUICommandInfo> CommandExportCAD; //导出CAD
	TSharedPtr<FUICommandInfo> CommandReset; // 重置标注
	TSharedPtr<FUICommandInfo> CommandUpdate; // 更新自动标注
	TSharedPtr<FUICommandInfo> CommandFacadeIndex; // 生成立面索引

	TSharedPtr<FUICommandInfo> CommandAutoRulerUp; // 自动标尺-上
	TSharedPtr<FUICommandInfo> CommandAutoRulerDown; // 自动标尺-下
	TSharedPtr<FUICommandInfo> CommandAutoRulerLeft; // 自动标尺-左
	TSharedPtr<FUICommandInfo> CommandAutoRulerRight; // 自动标尺-右

	TSharedPtr<FUICommandInfo> CommandDimension; // 尺寸标注
    TSharedPtr<FUICommandInfo> CommandDownLeadLabel_Size; // 规格标注
	TSharedPtr<FUICommandInfo> CommandDownLeadLabel_Material; // 材质标注
	TSharedPtr<FUICommandInfo> CommandDownLeadLabel_Text; // 文字标注
	TSharedPtr<FUICommandInfo> CommandDownLeadLabel_Craft; // 工艺标注
	TSharedPtr<FUICommandInfo> CommandDownLeadLabel_Height; // 高度标注
	TSharedPtr<FUICommandInfo> CommandDownLeadLabel_CustomHeight; // 手动标高

	TSharedPtr<FUICommandInfo> CommandCompass; // 指北针
	TSharedPtr<FUICommandInfo> CommandBoradSplitLine; // 台面分割线
};