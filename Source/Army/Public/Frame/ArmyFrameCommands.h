/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRFrameCommands.h
* @Description 全局菜单栏命令
*
* @Author 欧石楠
* @Date 2018年6月8日
* @Version 1.0
*/

#pragma once

#include "CoreMinimal.h"
#include "Commands.h"
#include "ArmyStyle.h"
#include "Reply.h"

class FArmyFrameCommands : public TCommands<FArmyFrameCommands> {

public:
	FArmyFrameCommands() : TCommands<FArmyFrameCommands>(
		TEXT("ArmyFrame"),
		NSLOCTEXT("Contexts", "ArmyFrame", "ArmyFrame"),
		NAME_None,
		FArmyStyle::Get().GetStyleSetName()
		){}

	virtual void RegisterCommands() override;
public:
	TSharedPtr<class FUICommandList> GlobalCommandsList;

	//文件菜单下拉子项命令
	TSharedPtr<FUICommandInfo> CommandMenuNew;		//新建方案
	TSharedPtr<FUICommandInfo> CommandMenuOpen;		//打开方案	
	TSharedPtr<FUICommandInfo> CommandMenuSave;		//保存方案
	TSharedPtr<FUICommandInfo> CommandMenuSaveAs;	//另存方案
	TSharedPtr<FUICommandInfo> CommandMenuMasterSaveAs; //另存为大师方案
	TSharedPtr<FUICommandInfo> CommandMenuExportList;//导出清单

	//编辑菜单下拉子项命令
	TSharedPtr<FUICommandInfo> CommandMenuRotate;	//旋转
	TSharedPtr<FUICommandInfo> CommandMenuScale;	//缩放
	TSharedPtr<FUICommandInfo> CommandMenuMirrorX;	//镜像X
	TSharedPtr<FUICommandInfo> CommandMenuMirrorY;	//镜像Y
	TSharedPtr<FUICommandInfo> CommandMenuCopy;		//复制
	TSharedPtr<FUICommandInfo> CommandMenuPaste;	//粘贴
	TSharedPtr<FUICommandInfo> CommandMenuDelete;	//删除
	TSharedPtr<FUICommandInfo> CommandMenuUndo;		//撤销
	TSharedPtr<FUICommandInfo> CommandMenuRedo;		//重做
	TSharedPtr<FUICommandInfo> CommandMenuGroup;	//成组
	TSharedPtr<FUICommandInfo> CommandMenuUnGroup;	//解组
	TSharedPtr<FUICommandInfo> CommandMenuSaveGroup;//存组

	//视图菜单下拉子项命令
	TSharedPtr<FUICommandInfo> CommandMenuOverallView;	//透视图
	TSharedPtr<FUICommandInfo> CommandMenuTopView;		//平面视图
	TSharedPtr<FUICommandInfo> CommandMenuCeilingView;	//吊顶视图
	TSharedPtr<FUICommandInfo> CommandMenuSurroundView;	//环绕图

	//渲染菜单下拉子项命令
	TSharedPtr<FUICommandInfo> CommandMenuDesignSketch;	//效果图
	TSharedPtr<FUICommandInfo> CommandMenuSinglePanorama; //单张全景图
	TSharedPtr<FUICommandInfo> CommandMenuMultiplePanoram; //多房间行走全景图

	//画质菜单下拉子项命令
	TSharedPtr<FUICommandInfo> CommandMenuHighQuality;	//高清
	TSharedPtr<FUICommandInfo> CommandMenuNormalQuality;//流畅

	//VR菜单下拉子项命令
	TSharedPtr<FUICommandInfo> CommandMenuVRMode;		//VR模式

	//帮助菜单下拉子项命令
	TSharedPtr<FUICommandInfo> CommandMenuOnlineService;//在线客服
	TSharedPtr<FUICommandInfo> CommandMenuVideoTutorial;	//视频教程
private:
	void BindGlobalCommands();
public:
	static bool DefaultCanExecuteAction();
	static bool DefaultCanNotExecuteAction();

	//@郭子阳
	//检查编辑菜单是否可用
	static bool CheckCanEdit();
	static bool CanEditAnyObj();
	static bool CheckCanEditActor();
	static bool CheckCanGroupActor();
	static bool CheckCanUnGroupActor();

	static bool CheckHasSelectedActor();
	static bool CheckHasSelectedXRObj();
	static bool CheckHasSelectedAnyObj();

    static bool CheckCanUndo();
    static bool CheckCanRedo();
	static bool CheckCanGroup();

	static bool View3DCanExecuteAction();
	static bool CeilingViewCanExecuteAction();
	static bool PanoramaCanExecuteAction();

	static bool IsHighQualityChecked();
	static bool IsNormalQualityChecked();

	static bool IsOverallViewChecked();
	static bool IsTopViewChecked();
	static bool IsCeilingViewChecked();
	static bool IsSurroundViewChecked();
	
	static bool CheckHasGroupActor();
	static bool CheckHasUpGroupActor();

public:
	static void OnMenuNew();
	static void OnMenuOpen();	
	static void OnMenuExportList();
	static void OnMenuSave();
	static void OnMenuSaveAs();
	static void OnMenuMasterSaveAs();

	static void OnMenuRotate();
	static void OnMenuScale();
	static void OnMenuMirrorX();
	static void OnMenuMirrorY();
	static void OnMenuCopy();
	static void OnMenuPaste();
	static void OnMenuDelete();
	static void OnMenuUndo();
	static void OnMenuRedo();
	static void OnMenuGroup();
	static void OnMenuUnGroup();
	static void OnMenuSaveGroup();

	static void OnMenuOverallView();
	static void OnMenuTopView();
	static void OnMenuCeilingView();
	static void OnMenuSurroundView();

	static void OnMenuDesignSketch();
	static void OnMenuSinglePanorama();
	static void OnMenuMultiplePanorama();

	static void OnMenuHighQuality();
	static void OnMenuNormalQuality();

	static void OnMenuVRMode();

	static void OnMenuOnlineService();
	static void OnMenuVideoTutorial();

public:
	static void OnAccoutChanged(const FString& InString);

	//@郭子阳
	//设置编辑菜单是否可用
	static void SetCanUseEditMenu(bool);
	static bool GetCanUseEditMenu() { return CanUseEditMenu; };
private:
	//@郭子阳
	//编辑菜单是否可用
	static bool CanUseEditMenu;
};