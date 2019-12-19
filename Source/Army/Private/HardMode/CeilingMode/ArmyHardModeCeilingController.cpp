#include "ArmyHardModeCeilingController.h"
#include "SArmyHardModeContentBrowser.h"
#include "ArmyViewportClient.h"
#include "ArmyDesignEditor.h"
#include "ArmyReplaceTextureOperation.h"
#include "ArmyHardModeCommands.h"
#include "ArmyToolBarBuilder.h"
#include "ArmyFrameCommands.h"
#include "SArmyCategoryOrganization.h"
#include "SArmyCategoryOrganizationManage.h"

void FArmyHardModeCeilingController::Init()
{
	ReturnWidget = CreateToolBarRightArea();
	FArmyDesignModeController::Init();
}

TSharedPtr<SWidget> FArmyHardModeCeilingController::MakeLeftPanelWidget()
{
	CategoryOrgManageSurface = SNew(SArmyCategoryOrganizationManage);

	//企业素材
	LeftPanel = SNew(SArmyHardModeContentBrowser)
		//.CategoryUrl("/api/mode_menu/elevation")
		.SearchUrl("/api/bim_goods/query")
		//.OnDelegate_3DTo2DGanged(FArmyFComponentResDelegate::CreateRaw(this, &FArmyHardModeController::On3DTo2DGanged))
		.ParentId(1);
	LeftPanel->SetSubMode(ESubMode::SM_CeilingMode);
	//LeftPanel->RequestCategory();
	//FSimpleDelegate::CreateRaw(this, &FArmyBaseSurfaceController::ReplaceTextureOperation, nullptr)
	LeftPanel->ItemSelected.BindLambda([this]()
	{
		ReplaceTextureOperation();
	});
	//LeftPanel->ItemSelected.BindRaw(this, &FArmyBaseSurfaceController::ReplaceTextureOperation,nullptr);


	TSharedPtr<SArmyCategoryOrganization> CompanyDataWid = SNew(SArmyCategoryOrganization)
		.CategoryUrl("/api/mode_menu/mode/")
		.ButtonImage(FArmyStyle::Get().GetBrush("Icon.companyCategry"))
		.CurrentBrowser(LeftPanel.ToSharedRef())
		.ECurrentOrganization(SArmyCategoryOrganization::EOragnization::OZ_CompanyData);

	//请求目录数据
	CompanyDataWid->RequestHardModeCategory();
	CategoryOrgManageSurface->AddOrganization(CompanyDataWid);

	return CategoryOrgManageSurface;
}

TSharedPtr<SWidget> FArmyHardModeCeilingController::MakeRightPanelWidget()
{
	SufaceDetail = MakeShareable(new FArmyHardModeDetail);
	return SufaceDetail->CreateDetailWidget();
}

TSharedPtr<SWidget> FArmyHardModeCeilingController::MakeToolBarWidget()
{
#define LOCTEXT_NAMESPACE "ToolBarWidget"
	TSharedPtr<FArmyToolBarBuilder> ToolBarBuidler = MakeShareable(new FArmyToolBarBuilder);

	FCanExecuteAction CheckCanUndo = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckCanUndo);
	FCanExecuteAction CheckCanRedo = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckCanRedo);

	ToolBarBuidler->AddButton(FArmyHardModeCommands::Get().CommandSave, FExecuteAction::CreateRaw(this, &FArmyBaseSurfaceController::OnSaveCommand));
	ToolBarBuidler->AddButton(FArmyHardModeCommands::Get().CommandDelete, FExecuteAction::CreateRaw(this, &FArmyBaseSurfaceController::OnDeleteCommand), CheckHasSelectedAreaObj);
	ToolBarBuidler->AddButton(FArmyHardModeCommands::Get().CommandEmpty, FExecuteAction::CreateRaw(this, &FArmyBaseSurfaceController::OnEmptyCommand));
	ToolBarBuidler->AddButton(FArmyHardModeCommands::Get().CommandMaterialBrush, FExecuteAction::CreateRaw(this, &FArmyBaseSurfaceController::OnMaterialBrushCommand));
	ToolBarBuidler->AddComboButton(FOnGetContent::CreateRaw(this, &FArmyHardModeCeilingController::OnAreaCommand), LOCTEXT("Area", "绘制区域"), FSlateIcon("ArmyStyle", "HardMode.OnAreaCommand"));
	//ToolBarBuidler->AddButton(FArmyHardModeCommands::Get().CommandUndo, FExecuteAction::CreateRaw(this, &FArmyBaseSurfaceController::OnUndoCommand), CheckCanUndo);
	//ToolBarBuidler->AddButton(FArmyHardModeCommands::Get().CommandRedo, FExecuteAction::CreateRaw(this, &FArmyBaseSurfaceController::OnRedoCommand), CheckCanRedo);
	//ToolBarBuidler->AddButton(FArmyHardModeCommands::Get().CommandReplace, FExecuteAction::CreateRaw(this, &FArmyBaseSurfaceController::OnReplaceCommand));
	//ToolBarBuidler->AddButton(FArmyHardModeCommands::Get().CommandCopy, FExecuteAction::CreateRaw(this, &FArmyHardModeCeilingController::OnCopyCommand));
	//ToolBarBuidler->AddButton(FArmyHardModeCommands::Get().CommandReturnViewAngle, FExecuteAction::CreateRaw(this, &FArmyHardModeCeilingController::OnReturnViewAngleCommand));

#undef LOCTEXT_NAMESPACE
	return ToolBarBuidler->CreateToolBar(&FArmyStyle::Get(), "ToolBar");
}

TSharedPtr<SWidget> FArmyHardModeCeilingController::MakeSettingBarWidget()
{
	return nullptr;
}

TSharedRef<SWidget> FArmyHardModeCeilingController::OnAreaCommand()
{
	const TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());
	TSharedRef< FUICommandList > ActionList = MakeShareable(new FUICommandList);

	//
	ActionList->MapAction(FArmyHardModeCommands::Get().CommandRectArea, FExecuteAction::CreateRaw(this, &FArmyBaseSurfaceController::OnRectAreaCommand));
	ActionList->MapAction(FArmyHardModeCommands::Get().CommandCircleArea, FExecuteAction::CreateRaw(this, &FArmyBaseSurfaceController::OnCircleAreaCommand));
	ActionList->MapAction(FArmyHardModeCommands::Get().CommandRegularPolygonArea, FExecuteAction::CreateRaw(this, &FArmyBaseSurfaceController::OnRegularPolygonAreaCommand));
	ActionList->MapAction(FArmyHardModeCommands::Get().CommandArea, FExecuteAction::CreateRaw(this, &FArmyBaseSurfaceController::OnFreePolygonAreaCommand));

	FMenuBuilder MenuBuilder(true, ActionList, MenuExtender);

	MenuBuilder.AddMenuEntry(FArmyHardModeCommands::Get().CommandRectArea);
	MenuBuilder.AddMenuEntry(FArmyHardModeCommands::Get().CommandCircleArea);
	MenuBuilder.AddMenuEntry(FArmyHardModeCommands::Get().CommandRegularPolygonArea);
	MenuBuilder.AddMenuEntry(FArmyHardModeCommands::Get().CommandArea);

	MenuBuilder.SetStyle(&FArmyStyle::Get(), "Menu");

	return MenuBuilder.MakeWidget();
}
