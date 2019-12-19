#include "SArmyStaticLightingSettings.h"
#include "SCheckBox.h"
#include "STextBlock.h"
#include "SBoxPanel.h"
#include "SButton.h"
#include "SBox.h"
#include "ArmyStyle.h"
#include "ArmyGameInstance.h"
#include "ArmyCameraParamModule.h"
#include "ArmyHttpModule.h"
#include "ArmyEngineTools.h"
#include "ArmyPlayerController.h"
#include "ArmyStaticLighting/Public/StaticLightingPrivate.h"
#include "ArmyWorldManager.h"

void SArmyStaticLightingSettings::Construct(const FArguments & InArgs)
{
	OnBuildFinishedDelegate = InArgs._OnBuildFinishedDelegate;
	OnGenerateClickedDelegate = InArgs._OnGenerateClickedDelegate;
	SelectedQualityIndex = 1;
	BuildingState = EBuildingState::ENotStart;

	ChildSlot
	[
		CreateTempletWidget()
	];

	OnQualityClicked(1);
}

void SArmyStaticLightingSettings::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if (BuildingState == EBuildingState::ENotStart)
	{
	}
	else if (BuildingState == EBuildingState::EProcessing)
	{
		//构建进程中，则更新进度信息
		if (FStaticLightingManager::Get()->GetBuildStage() == 1)
		{
			FString BuildQualityStr = SelectedQualityIndex == 0 ? TEXT("预览") : (SelectedQualityIndex == 1 ? TEXT("标准") : TEXT("高清"));
			int32 BuildTime = SelectedQualityIndex == 0 ? 2 : (SelectedQualityIndex == 1 ? 5 : 15);
			float BuildPercent = FStaticLightingManager::Get()->GetBuildPercent();

			FString BuildMeshStr = FArmyWorldManager::Get().bBuildStaticMesh ? TEXT("（家具参与渲染时导出时间较长）") : TEXT("");
			FString Str = FString::Printf(TEXT("导出场景信息进度%s %.2f%%"), *BuildMeshStr,  BuildPercent);
			GGI->Window->UpdateThrobberMessage(FText::FromString(Str));
		}
		else if (FStaticLightingManager::Get()->GetBuildStage() == 2)
		{
			FString BuildQualityStr = SelectedQualityIndex == 0 ? TEXT("预览") : (SelectedQualityIndex == 1 ? TEXT("标准") : TEXT("高清"));
			int32 BuildTime = SelectedQualityIndex == 0 ? 2 : (SelectedQualityIndex == 1 ? 5 : 15);
			if (FArmyWorldManager::Get().bBuildStaticMesh)
				BuildTime *= 3;
			float BuildPercent = FStaticLightingManager::Get()->GetBuildPercent();

			FString Str = FString::Printf(TEXT("渲染%s效果图进度 %.2f%% (预计需要时间%d分钟)"), *BuildQualityStr, BuildPercent, BuildTime);
			GGI->Window->UpdateThrobberMessage(FText::FromString(Str));
		}
		else if (FStaticLightingManager::Get()->GetBuildStage() == 3)
		{
			BuildingState = EBuildingState::ESuccess;
			GGI->Window->HideThrobber();
			GGI->Window->DismissModalDialog();
			GGI->Window->ShowMessage(MT_Success, TEXT("渲染成功!"));
			OnBuildFinishedDelegate.ExecuteIfBound(true);
		}
		else if (FStaticLightingManager::Get()->GetBuildStage() == 4)
		{
			BuildingState = EBuildingState::EFailed;
			GGI->Window->HideThrobber();
			GGI->Window->DismissModalDialog();
			GGI->Window->ShowMessage(MT_Warning, TEXT("渲染失败！"));
			OnBuildFinishedDelegate.ExecuteIfBound(false);
		}

	}
	else if (BuildingState == EBuildingState::ESuccess)
	{

	}
	else if (BuildingState == EBuildingState::EFailed)
	{

	}
}

void SArmyStaticLightingSettings::InitializeSettings()
{
	CachedBuildStaticMesh = FArmyWorldManager::Get().bBuildStaticMesh ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

TSharedRef<SWidget> SArmyStaticLightingSettings::CreateTempletWidget()
{
	return 
		SNew(SBox)
		.WidthOverride(540)
		.HeightOverride(360)
		.VAlign(EVerticalAlignment::VAlign_Fill)
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(FMargin(0, 32, 0, 0))
			.AutoHeight()
			.VAlign(EVerticalAlignment::VAlign_Fill)
			.HAlign(EHorizontalAlignment::HAlign_Center)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(STextBlock)
					.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
					.Text(FText::FromString(TEXT("请选择渲染质量")))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0, 24, 0, 0))
				.HAlign(EHorizontalAlignment::HAlign_Center)
				[
					SNew(SHorizontalBox)
					/*+ SHorizontalBox::Slot()
					[
						SNew(SButton)
						.ContentPadding(FMargin(0))
						.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.None"))
						.OnClicked(this, &SArmyStaticLightingSettings::OnQualityClicked, 0)
						.OnHovered(this, &SArmyStaticLightingSettings::OnQualityHovered, 0)
						.OnUnhovered(this, &SArmyStaticLightingSettings::OnQualityUnHoverd, 0)
						[
							SAssignNew(QualityBorder0, SBorder)
							.Padding(FMargin(2))
							.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
							.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
							[
								SNew(SImage)
								.Image(FArmyStyle::Get().GetBrush("Icon.StaticLightingQuality_Preview"))
							]
						]
					]*/
					+ SHorizontalBox::Slot()
					.Padding(FMargin(16,0,0,0))
					[
						SNew(SButton)
						.ContentPadding(FMargin(0))
						.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.None"))
						.OnClicked(this, &SArmyStaticLightingSettings::OnQualityClicked, 1)
						.OnHovered(this, &SArmyStaticLightingSettings::OnQualityHovered, 1)
						.OnUnhovered(this, &SArmyStaticLightingSettings::OnQualityUnHoverd, 1)
						[
							SAssignNew(QualityBorder1, SBorder)
							.Padding(FMargin(2))
							.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
							.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
							[
								SNew(SImage)
								.Image(FArmyStyle::Get().GetBrush("Icon.StaticLightingQuality_Standard"))
							]
						]
					]
					+ SHorizontalBox::Slot()
					.Padding(FMargin(16, 0, 0, 0))
					[
						SNew(SButton)
						.ContentPadding(FMargin(0))
						.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.None"))
						.OnClicked(this, &SArmyStaticLightingSettings::OnQualityClicked, 2)
						.OnHovered(this, &SArmyStaticLightingSettings::OnQualityHovered, 2)
						.OnUnhovered(this, &SArmyStaticLightingSettings::OnQualityUnHoverd, 2)
						[
							SAssignNew(QualityBorder2, SBorder)
							.Padding(FMargin(2))
							.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
							.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
							[
								SNew(SImage)
								.Image(FArmyStyle::Get().GetBrush("Icon.StaticLightingQuality_HD"))
							]
						]
					]
				]
			]

			+ SVerticalBox::Slot()
			.Padding(FMargin(80, 10, 0, 0))
			.AutoHeight()
			.VAlign(EVerticalAlignment::VAlign_Center)
			.HAlign(EHorizontalAlignment::HAlign_Left)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SCheckBox)
					.Style(FArmyStyle::Get(), "SingleCheckBox")
					.IsChecked(this, &SArmyStaticLightingSettings::GetBuildStaticMesh)
					.OnCheckStateChanged(this, &SArmyStaticLightingSettings::OnBuildStaticMeshChanged)
				]

				+ SHorizontalBox::Slot()
				.Padding(FMargin(10, 0, 0, 0))
				.AutoWidth()
				[
					SNew(STextBlock)
					.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
					.ColorAndOpacity(FLinearColor::White)
					.Text(FText::FromString(TEXT("家具参与渲染（渲染时间较长，但整体光影效果会更佳）")))
				]
			]

			+ SVerticalBox::Slot()
			.Padding(FMargin(0, 32, 20, 20))
			.VAlign(EVerticalAlignment::VAlign_Center)
			.HAlign(EHorizontalAlignment::HAlign_Right)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				[
					SNew(SBox)
					.HeightOverride(30)
					.WidthOverride(80)
					[
						SNew(SButton)
						.ButtonStyle(FCoreStyle::Get(), "Box")
						.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.FFFF9800"))
						.HAlign(EHorizontalAlignment::HAlign_Center)
						.VAlign(EVerticalAlignment::VAlign_Center)
						.OnClicked(this, &SArmyStaticLightingSettings::OnGenerateClicked)
						.Content()
						[
							SNew(STextBlock)
							.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
							.ColorAndOpacity(FLinearColor::White)
							.Text(FText::FromString(TEXT("开始渲染")))
						]
					]
				]

				+ SHorizontalBox::Slot()
				.Padding(20,0,0,0)
				[
					SNew(SBox)
					.HeightOverride(30)
					.WidthOverride(80)
					[
						SNew(SButton)
						.ButtonStyle(FCoreStyle::Get(), "Box")
						.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Gray.FF353638"))
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						.OnClicked(this, &SArmyStaticLightingSettings::CancelClicked)
						[
							SNew(STextBlock)
							.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
							.ColorAndOpacity(FLinearColor::White)
							.Text(FText::FromString(TEXT("取消")))
						]
					]
				]
			]
		];

		CachedBuildStaticMesh = FArmyWorldManager::Get().bBuildStaticMesh ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

TSharedRef<SWidget> SArmyStaticLightingSettings::CreateCustomalizeWidget()
{
	return
		SNew(SBox)
		.WidthOverride(420)
		.HeightOverride(168);
}

FReply SArmyStaticLightingSettings::OnGenerateClicked()
{
	FStaticLightingManager::Get()->ClearStaticLighting();

	GGI->Window->ShowThrobber(FText::FromString(TEXT("准备开始渲染...")), FSimpleDelegate::CreateSP(this, &SArmyStaticLightingSettings::CancelProcessingBuild));
	FSlateApplication::Get().Tick();

	if (SelectedQualityIndex >= 0 && SelectedQualityIndex <= 2)
	{
		if (SelectedQualityIndex == 2)
			SelectedQualityIndex = 3;

		OnGenerateClickedDelegate.ExecuteIfBound(SelectedQualityIndex);
	}

	BuildingState = EBuildingState::EProcessing;

	return FReply::Handled();
}

FReply SArmyStaticLightingSettings::CancelClicked()
{
	GGI->Window->DismissModalDialog();
	return FReply::Handled();
}


void SArmyStaticLightingSettings::OnBuildStaticMeshChanged(ECheckBoxState InNewState)
{
	CachedBuildStaticMesh = InNewState;
	FArmyWorldManager::Get().bBuildStaticMesh = CachedBuildStaticMesh == ECheckBoxState::Checked;
}

FReply SArmyStaticLightingSettings::OnQualityClicked(int32 QualityIndex)
{
	UnHoverAll();
// 	if (QualityIndex == 0)
// 	{
// 		QualityBorder0->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FFFD9800"));
// 		SelectedQualityIndex = 0;
// 	}
	/*else*/ if (QualityIndex == 1)
	{
		QualityBorder1->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FFFD9800"));
		SelectedQualityIndex = 1;
	}
	else if (QualityIndex == 2)
	{
		QualityBorder2->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FFFD9800"));
		SelectedQualityIndex = 2;
	}
	return FReply::Handled();
}

void SArmyStaticLightingSettings::OnQualityHovered(int32 QualityIndex)
{
	//if (QualityIndex == 0)
	//	QualityBorder0->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FFFD9800"));
	/*else*/ if (QualityIndex == 1)
		QualityBorder1->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FFFD9800"));
	else if (QualityIndex == 2)
		QualityBorder2->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FFFD9800"));
}

void SArmyStaticLightingSettings::OnQualityUnHoverd(int32 QualityIndex)
{
	//if (QualityIndex == 0 && SelectedQualityIndex != QualityIndex)
	//	QualityBorder0->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"));
	/*else*/ if (QualityIndex == 1 && SelectedQualityIndex != QualityIndex)
		QualityBorder1->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"));
	else if (QualityIndex == 2 && SelectedQualityIndex != QualityIndex)
		QualityBorder2->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"));
}

void SArmyStaticLightingSettings::UnHoverAll()
{
	//QualityBorder0->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"));
	QualityBorder1->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"));
	QualityBorder2->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"));

}

void SArmyStaticLightingSettings::CancelProcessingBuild()
{
	FStaticLightingManager::Get()->SetMapBuildCancelled(true);
}
