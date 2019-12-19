#include "ArmyFrameCommands.h"
#include "SlateApplication.h"
#include "ArmyEditorEngine.h"
#include "ArmyHttpModule.h"
#include "ArmyViewportClient.h"
#include "ArmyPlayerController.h"
#include "ArmyModalManager.h"
#include "ArmyUser.h"
#include "CoreGlobals.h"
#include "ConfigCacheIni.h"
#include "ArmyGameInstance.h"
#include "ArmyDesignEditor.h"
#include "ArmyGlobalActionCallBack.h"
#include "UICommandList.h"

#define LOCTEXT_NAMESPACE "ArmyFrameCommands"

bool FArmyFrameCommands::CanUseEditMenu = true;

void FArmyFrameCommands::RegisterCommands()
{
	UI_COMMAND(CommandMenuNew, "新建方案", "New", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::N));
	UI_COMMAND(CommandMenuOpen, "打开方案", "Open", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::O));
	UI_COMMAND(CommandMenuSave, "保存方案", "Save", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::S));
	UI_COMMAND(CommandMenuSaveAs, "另存方案", "SaveAs", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandMenuMasterSaveAs, "另存为大师方案", "MasterSaveAs", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandMenuExportList, "导出清单", "ExportList", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::E));

	UI_COMMAND(CommandMenuRotate, "旋转", "Rotate", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandMenuScale, "缩放", "Scale", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandMenuMirrorX, "镜像X", "MirrorX", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandMenuMirrorY, "镜像Y", "MirrorY", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandMenuCopy, "复制", "Copy", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::C));
	UI_COMMAND(CommandMenuPaste, "粘贴", "Paste", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::V));
	UI_COMMAND(CommandMenuDelete, "删除", "Delete", EUserInterfaceActionType::Button, FInputChord(EKeys::Delete));
	UI_COMMAND(CommandMenuUndo, "撤销", "Undo", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::Z));
	UI_COMMAND(CommandMenuRedo, "重做", "Redo", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::Y));
	UI_COMMAND(CommandMenuGroup, "成组", "Group", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::G));
	UI_COMMAND(CommandMenuUnGroup, "解组", "UnGroup", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandMenuSaveGroup, "存组", "SaveGroup", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(CommandMenuOverallView, "3D图", "OverallView", EUserInterfaceActionType::Check, FInputChord(EKeys::P));
	UI_COMMAND(CommandMenuTopView, "地面视图", "Plane View", EUserInterfaceActionType::Check, FInputChord(EKeys::T));
	UI_COMMAND(CommandMenuCeilingView, "顶面视图", "Ceiling View", EUserInterfaceActionType::Check, FInputChord());
	UI_COMMAND(CommandMenuSurroundView, "环绕图", "SurroundView", EUserInterfaceActionType::Check, FInputChord());

	UI_COMMAND(CommandMenuDesignSketch, "效果图", "DesignSketch", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandMenuSinglePanorama, "单张全景图", "SinglePanorama", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandMenuMultiplePanoram, "多房间行走全景图", "MultiplePanoram", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(CommandMenuHighQuality, "高清", "HighQuality", EUserInterfaceActionType::Check, FInputChord());
	UI_COMMAND(CommandMenuNormalQuality, "流畅", "NormalQuality", EUserInterfaceActionType::Check, FInputChord());

	UI_COMMAND(CommandMenuVRMode, "VR模式", "VRMode", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(CommandMenuOnlineService, "在线客服", "OnlineService", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandMenuVideoTutorial, "视频教程", "CompanyWeb", EUserInterfaceActionType::Button, FInputChord());

	BindGlobalCommands();
}

void FArmyFrameCommands::BindGlobalCommands()
{
	GlobalCommandsList = MakeShareable(new FUICommandList);

	//是否可用的判定方法
	FCanExecuteAction DefaultExecuteAction = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::DefaultCanExecuteAction);
	FCanExecuteAction DefaultCanNotExecute = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::DefaultCanNotExecuteAction);

	FCanExecuteAction CheckHasSelectedActor = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckHasSelectedActor);
	FCanExecuteAction CheckHasSelectedAnyObj = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckHasSelectedAnyObj);
	FCanExecuteAction CheckCanEditActor = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckCanEditActor);
	FCanExecuteAction CanEditAnyObj = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CanEditAnyObj);
	
	
	
	FCanExecuteAction CheckCanUndo = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckCanUndo);
	FCanExecuteAction CheckCanRedo = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckCanRedo);

	FCanExecuteAction View3DCanExecute = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::View3DCanExecuteAction);
	FCanExecuteAction CeilingViewCanExecute = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CeilingViewCanExecuteAction);
	FCanExecuteAction PanoramaCanExecute = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::PanoramaCanExecuteAction);

	//成组  解组控件的显隐
	FCanExecuteAction CheckHasGroupActor = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckHasGroupActor);
	FCanExecuteAction CheckHasUpGroupActor = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckHasUpGroupActor);
	FCanExecuteAction CheckCanGroupActor = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckCanGroupActor);
	FCanExecuteAction CheckCanUnGroupActor = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckCanUnGroupActor);

	//是否被勾选的判定
	FIsActionChecked IsHighQualityChecked = FIsActionChecked::CreateStatic(&FArmyFrameCommands::IsHighQualityChecked);
	FIsActionChecked IsNormalQualityChecked = FIsActionChecked::CreateStatic(&FArmyFrameCommands::IsNormalQualityChecked);

	FIsActionChecked IsOverallViewChecked = FIsActionChecked::CreateStatic(&FArmyFrameCommands::IsOverallViewChecked);
	FIsActionChecked IsTopViewChecked = FIsActionChecked::CreateStatic(&FArmyFrameCommands::IsTopViewChecked);
	FIsActionChecked IsCeilingViewChecked = FIsActionChecked::CreateStatic(&FArmyFrameCommands::IsCeilingViewChecked);
	FIsActionChecked IsSurroundViewChecked = FIsActionChecked::CreateStatic(&FArmyFrameCommands::IsSurroundViewChecked);


	GlobalCommandsList->MapAction(CommandMenuNew, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuNew), DefaultExecuteAction);
	GlobalCommandsList->MapAction(CommandMenuOpen, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuOpen), DefaultExecuteAction);	
	GlobalCommandsList->MapAction(CommandMenuExportList, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuExportList), DefaultExecuteAction);
	GlobalCommandsList->MapAction(CommandMenuSave, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuSave), DefaultExecuteAction);
	GlobalCommandsList->MapAction(CommandMenuSaveAs, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuSaveAs), DefaultExecuteAction);
	GlobalCommandsList->MapAction(CommandMenuMasterSaveAs, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuMasterSaveAs), DefaultExecuteAction);

	GlobalCommandsList->MapAction(CommandMenuRotate, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuRotate), CheckCanEditActor);
	GlobalCommandsList->MapAction(CommandMenuScale, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuScale), CheckCanEditActor);
	GlobalCommandsList->MapAction(CommandMenuMirrorX, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuMirrorX), CheckCanEditActor);
	GlobalCommandsList->MapAction(CommandMenuMirrorY, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuMirrorY), CheckCanEditActor);
	GlobalCommandsList->MapAction(CommandMenuCopy, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuCopy), CheckCanEditActor);
	GlobalCommandsList->MapAction(CommandMenuPaste, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuPaste), CheckCanEditActor);
	GlobalCommandsList->MapAction(CommandMenuDelete, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuDelete), CanEditAnyObj);
	GlobalCommandsList->MapAction(CommandMenuUndo, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuUndo), CheckCanUndo);
	GlobalCommandsList->MapAction(CommandMenuRedo, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuRedo), CheckCanRedo);
	GlobalCommandsList->MapAction(CommandMenuGroup, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuGroup), CheckCanGroupActor);
	GlobalCommandsList->MapAction(CommandMenuUnGroup, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuUnGroup), CheckCanUnGroupActor);
	GlobalCommandsList->MapAction(CommandMenuSaveGroup, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuSaveGroup), DefaultCanNotExecute);

	GlobalCommandsList->MapAction(CommandMenuOverallView, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuOverallView), View3DCanExecute, IsOverallViewChecked);
	GlobalCommandsList->MapAction(CommandMenuTopView, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuTopView), View3DCanExecute, IsTopViewChecked);
	GlobalCommandsList->MapAction(CommandMenuCeilingView, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuCeilingView), CeilingViewCanExecute, IsCeilingViewChecked);
	//GlobalCommandsList->MapAction(CommandMenuSurroundView, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuSurroundView), DefaultCanNotExecute, IsSurroundViewChecked);

	//@梁晓菲 全景图
	GlobalCommandsList->MapAction(CommandMenuDesignSketch, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuDesignSketch), PanoramaCanExecute);
	GlobalCommandsList->MapAction(CommandMenuSinglePanorama, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuSinglePanorama), PanoramaCanExecute);
	GlobalCommandsList->MapAction(CommandMenuMultiplePanoram, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuMultiplePanorama), PanoramaCanExecute);

	GlobalCommandsList->MapAction(CommandMenuHighQuality, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuHighQuality), View3DCanExecute, IsHighQualityChecked);
	GlobalCommandsList->MapAction(CommandMenuNormalQuality, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuNormalQuality), View3DCanExecute, IsNormalQualityChecked);

	GlobalCommandsList->MapAction(CommandMenuVRMode, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuVRMode), View3DCanExecute);
	
	GlobalCommandsList->MapAction(CommandMenuOnlineService, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuOnlineService), DefaultExecuteAction);
	GlobalCommandsList->MapAction(CommandMenuVideoTutorial, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuVideoTutorial), DefaultExecuteAction);

	/**@欧石楠设置全局可用的command list*/
	FArmyGlobalActionCallBack::Get().SetGlobalCommandsList(GlobalCommandsList);
}
bool FArmyFrameCommands::DefaultCanExecuteAction()
{
	return FSlateApplication::Get().IsNormalExecution();
}

bool FArmyFrameCommands::DefaultCanNotExecuteAction()
{
	return false;
}

bool FArmyFrameCommands::CheckCanEdit()
{
	return CanUseEditMenu;
}

bool FArmyFrameCommands::CanEditAnyObj()
{
	return CheckCanEdit()&& CheckHasSelectedAnyObj();
}

bool FArmyFrameCommands::CheckCanEditActor()
{
	return CheckCanEdit()&& CheckHasSelectedActor();
}

bool FArmyFrameCommands::CheckCanGroupActor()
{
	return CheckCanEdit() && CheckHasGroupActor();
}

bool FArmyFrameCommands::CheckCanUnGroupActor()
{
	return CheckCanEdit() && CheckHasUpGroupActor();
}

bool FArmyFrameCommands::CheckHasSelectedActor()
{
	if (GXREditor->GetSelectedActorCount() > 0) 
	{
		AActor* SeletedActor = GXREditor->GetSingleSelectedActor();
		if(SeletedActor)
		{
			if (SeletedActor->Tags.Contains(TEXT("AreaHighLight")) ||
                SeletedActor->ActorHasTag(XRActorTag::SecurityDoor) ||
                SeletedActor->ActorHasTag(XRActorTag::Door) || 
                SeletedActor->ActorHasTag(XRActorTag::Pass) ||
                SeletedActor->ActorHasTag(XRActorTag::Window))
			{
				return false;
			}
			else
				return true;
		}
	
		return true;
	}	
	return false;
}

bool FArmyFrameCommands::CheckHasSelectedXRObj()
{
	/* @梁晓菲 非手动标高和室内净高的OT_TextLabel或者OT_ComponentBase，不可删除*/
	//在多选的情况下，如果存在不可删除的，则不能删除
	bool CanDelete = false;

	if (FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects().Num() > 0)
    {
		CanDelete = true;

		if (FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects().Num() > 0    &&
			FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects()[0].IsValid() &&
			FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects()[0].Pin()->GetPropertyFlag(FArmyObject::FLAG_MODIFY))
		{
			CanDelete = false;
		}

		for (auto& It : FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects())
		{
			if (!It.IsValid()) continue;
			switch (It.Pin()->GetType())
			{
			case EObjectType::OT_TextLabel:
			{
				TSharedPtr<FArmyTextLabel> Label = StaticCastSharedPtr<FArmyTextLabel>(It.Pin());
				if (Label->GetLabelType() != FArmyTextLabel::LT_CustomHeight &&
					Label->GetLabelType() != FArmyTextLabel::LT_SpaceHeight)
				{
					CanDelete = false;
				}
			}
			break;
			case EObjectType::OT_Entrance:
				CanDelete = false;
				break;
			case EObjectType::OT_ComponentBase:
				CanDelete = true;
				break;
			case EObjectType::OT_InternalRoom:
				CanDelete = FArmySceneData::Get()->GetHasOutWall() ? false : true;
				break;
			default:
				break;
			}
		}
	}
    else if (FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedPrimitives().Num() > 0 && !FArmySceneData::Get()->GetHasOutWall())
    {
        CanDelete = true;
    }

	return CanDelete;
}

bool FArmyFrameCommands::CheckHasSelectedAnyObj()
{
	return (CheckHasSelectedActor() || CheckHasSelectedXRObj());
}

bool FArmyFrameCommands::CheckCanUndo()
{
	if (GGI->Window->GetActiveController()->TransMgr.IsValid())
	{
		return GGI->Window->GetActiveController()->TransMgr->CanUndo();
	}
	else
		return false;
}

bool FArmyFrameCommands::CheckCanRedo()
{
	if (GGI->Window->GetActiveController()->TransMgr.IsValid())
		return GGI->Window->GetActiveController()->TransMgr->CanRedo();
	else
		return false;
}

bool FArmyFrameCommands::CheckCanGroup()
{
	return false;
}

bool FArmyFrameCommands::View3DCanExecuteAction()
{
	if (!GVC->IsLockViewPortClient() && GGI->DesignEditor.IsValid()) {
		if (GGI->DesignEditor->GetCurrentDesignMode() != DesignMode::HomeMode
		&& GGI->DesignEditor->GetCurrentDesignMode() != DesignMode::ConstrctionMode
		&& GGI->DesignEditor->GetCurrentDesignMode() != DesignMode::LayoutMode) {
			return true;
		}
	}
	return false;
}

bool FArmyFrameCommands::CeilingViewCanExecuteAction()
{
	if (!GVC->IsLockViewPortClient() && GGI->DesignEditor.IsValid()) {
		if (GGI->DesignEditor->GetCurrentDesignMode() != DesignMode::HomeMode
			&& GGI->DesignEditor->GetCurrentDesignMode() != DesignMode::ConstrctionMode
			&& GGI->DesignEditor->GetCurrentDesignMode() != DesignMode::HydropowerMode
			&& GGI->DesignEditor->GetCurrentDesignMode() != DesignMode::LayoutMode) {
			return true;
		}
	}
	return false;
}

bool FArmyFrameCommands::PanoramaCanExecuteAction()
{
	if (!GVC->IsLockViewPortClient() && GGI->DesignEditor.IsValid()) {
		if (GGI->DesignEditor->GetCurrentDesignMode() == DesignMode::HardMode)
		{
			return true;
		}
	}
	return false;
}

bool FArmyFrameCommands::IsHighQualityChecked()
{
	if (GXRPC) {
		return (GXRPC->GetQualityLevels() == 3) ? true : false;
	}
	return false;
}

bool FArmyFrameCommands::IsNormalQualityChecked()
{
	if (GXRPC) {
		return (GXRPC->GetQualityLevels() == 3) ? false : true;
	}
	return false;
}

bool FArmyFrameCommands::IsOverallViewChecked()
{
	if (GXRPC) {
		return (GXRPC->GetXRViewMode() == EXRView_FPS) ? true : false;
	}
	return false;
}

bool FArmyFrameCommands::IsTopViewChecked()
{
	if (GXRPC) {
		return (GXRPC->GetXRViewMode() == EXRView_TOP) ? true : false;
	}
	return false;
}
bool FArmyFrameCommands::IsCeilingViewChecked()
{
	if (GXRPC) {
		return (GXRPC->GetXRViewMode() == EXRView_CEILING) ? true : false;
	}
	return false;
}
bool FArmyFrameCommands::IsSurroundViewChecked()
{
	if (GXRPC) {
		return (GXRPC->GetXRViewMode() == EXRView_TPS) ? true : false;
	}
	return false;
}

bool FArmyFrameCommands::CheckHasGroupActor()
{
	if (GXREditor->GetSelectedActorCount() > 1) {
		return true;
	}
	return false;
}

bool FArmyFrameCommands::CheckHasUpGroupActor()
{
	if (GXREditor->GetSelectedActorCount() > 1) {
		return true;
	}
	return false;
}

void FArmyFrameCommands::OnMenuNew()
{
	FArmyGlobalActionCallBack::Get().OnNewPlan();
}

void FArmyFrameCommands::OnMenuOpen()
{
	FArmyGlobalActionCallBack::Get().OnOpenPlan();
}

void FArmyFrameCommands::OnMenuExportList()
{
	/**@欧石楠 当保存之前取消所有选中*/
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	FArmyGlobalActionCallBack::Get().OnExportList();
}

void FArmyFrameCommands::OnMenuSave()
{
	/**@欧石楠 当保存之前取消所有选中*/
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	FArmyGlobalActionCallBack::Get().OnSavePlan();
}

void FArmyFrameCommands::OnMenuSaveAs()
{
	FArmyGlobalActionCallBack::Get().OnSaveAsPlan();
}

void FArmyFrameCommands::OnMenuMasterSaveAs()
{
	FArmyGlobalActionCallBack::Get().OnCheckSaveAsMasterPlan();
}

void FArmyFrameCommands::OnMenuRotate()
{
}

void FArmyFrameCommands::OnMenuScale()
{
}

void FArmyFrameCommands::OnMenuMirrorX()
{
	GXREditor->MirrorXSelectedActor();
}

void FArmyFrameCommands::OnMenuMirrorY()
{
	GXREditor->MirrorYSelectedActor();
}

void FArmyFrameCommands::OnMenuCopy()
{
	GXREditor->edactCopySelected(GVC->GetWorld());
}

void FArmyFrameCommands::OnMenuPaste()
{
	GXREditor->edactPasteSelected(GVC->GetWorld(), false, false, false);
}

void FArmyFrameCommands::OnMenuDelete()
{
	GGI->DesignEditor->CurrentController->Delete();	
}

void FArmyFrameCommands::OnMenuUndo()
{
    //GGI->Window->GetActiveController()->TransMgr->Undo();
}

void FArmyFrameCommands::OnMenuRedo()
{
    //GGI->Window->GetActiveController()->TransMgr->Redo();
}

void FArmyFrameCommands::OnMenuGroup()
{
	GXREditor->edactRegroupFromSelected();
}

void FArmyFrameCommands::OnMenuUnGroup()
{
	GXREditor->edactUngroupFromSelected();
}

void FArmyFrameCommands::OnMenuSaveGroup()
{
}

void FArmyFrameCommands::OnMenuOverallView()
{
	if (GXRPC) {
		GXRPC->SetXRViewMode(EXRView_FPS);
	}
}

void FArmyFrameCommands::OnMenuTopView()
{
	if (GXRPC) {
		GXRPC->SetXRViewMode(EXRView_TOP);
	}
}
void FArmyFrameCommands::OnMenuCeilingView()
{
	if (GXRPC) {
		GXRPC->SetXRViewMode(EXRView_CEILING);
	}
}
void FArmyFrameCommands::OnMenuSurroundView()
{
	if (GXRPC) {
		GXRPC->SetXRViewMode(EXRView_TPS);
	}
}

void FArmyFrameCommands::OnMenuDesignSketch()
{
	FArmyGlobalActionCallBack::Get().OnGenerateRenderingImage();
}

void FArmyFrameCommands::OnMenuSinglePanorama()
{
	FArmyGlobalActionCallBack::Get().OnSinglePanorama();
}

void FArmyFrameCommands::OnMenuMultiplePanorama()
{
	FArmyGlobalActionCallBack::Get().OnMultiplePanorama();
}

void FArmyFrameCommands::OnMenuHighQuality()
{
	if (GXRPC) {
		GXRPC->SetQualityLevels(3);
	}
}

void FArmyFrameCommands::OnMenuNormalQuality()
{
	if (GXRPC) {
		GXRPC->SetQualityLevels(0);
	}
}

void FArmyFrameCommands::OnMenuVRMode()
{
	if (GXRPC) {
		if (!GXRPC->EnableVRMode(true)) {
			GGI->Window->ShowMessage(MT_Warning, TEXT("未检测到VR设备！"));
		}		
	}	
}

void FArmyFrameCommands::OnMenuOnlineService()
{
	FString URL = "/html/dbj_us_contact.html";
	FPlatformProcess::LaunchURL(*URL, NULL, NULL);
}

void FArmyFrameCommands::OnMenuVideoTutorial()
{
	FString URL = "/html/dbj_yy_video.html";
	FPlatformProcess::LaunchURL(*URL, NULL, NULL);
}

void FArmyFrameCommands::OnAccoutChanged(const FString & InString)
{
	if (InString == TEXT("后台管理")) {
		FString Url = FString::Printf(TEXT("%s?token=%s"), *FArmyHttpModule::Get().GetBackendUrl(), *FArmyHttpModule::Get().GetToken());
		FPlatformProcess::LaunchURL(*Url, NULL, NULL);
		FArmyGlobalActionCallBack::Get().OnBackGroundManager();
	}
	else if(InString == TEXT("注销")){
        FArmyGlobalActionCallBack::Get().OnLogout();
	}
	else if (InString == TEXT("退出")) {
        FArmyGlobalActionCallBack::Get().OnExit();
	}
}

void FArmyFrameCommands::SetCanUseEditMenu(bool CanUse)
{
	CanUseEditMenu = CanUse;
}

#undef  LOCTEXT_NAMESPACE