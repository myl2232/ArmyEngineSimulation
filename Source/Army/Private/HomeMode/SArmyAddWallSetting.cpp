#include "SArmyAddWallSetting.h"
#include "SBox.h"
#include "SBoxPanel.h"
#include "STextBlock.h"
#include "ArmyStyle.h"
#include "SArmyEditableNumberBox.h"
#include "ArmyHomedata.h"

void SArmyAddWallSetting::Construct(const FArguments & InArgs)
{
	bNeedHeight = InArgs._bNeedHeight;
	CachedHeightValue = FArmySceneData::WallHeight * 10.f;
	int TempHeight = bNeedHeight ? 160 : 102;
	ChildSlot
		[
			SNew(SBox)
			.WidthOverride(420)
			.HeightOverride(TempHeight)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.FillHeight(1.f)
				.Padding(0, 20, 0, 16)
				[				
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.Padding(20, 0, 10, 0)
					.AutoWidth()
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Center)
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("墙体厚度:")))
						.TextStyle(FArmyStyle::Get(), "ArmyText_12")
						.Justification(ETextJustify::Center)
					]
					+ SHorizontalBox::Slot()
					.FillWidth(1.f)
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Fill)					
					[
						SAssignNew(ThicknessBox, SArmyEditableNumberBox)
						.IsIntegerOnly(true)
						.SelectAllTextWhenFocused(true)
						.Text(this, &SArmyAddWallSetting::GetThicknessValue)
						.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
						.OnTextCommitted_Raw(this, &SArmyAddWallSetting::OnThicknessValueChanged)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(10, 0, 20, 0)
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Center)
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("mm")))
						.TextStyle(FArmyStyle::Get(), "ArmyText_12")
						.Justification(ETextJustify::Center)
					]
				]
				+SVerticalBox::Slot()
				.FillHeight(1.f)
				.Padding(0, 0, 0, 16)
				[	
					SAssignNew(HeightContainBox, SBox)					
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.Padding(20, 0, 10, 0)
						.AutoWidth()
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("墙体高度:")))
							.TextStyle(FArmyStyle::Get(), "ArmyText_12")
							.Justification(ETextJustify::Center)
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1.f)
						.VAlign(VAlign_Fill)
						.HAlign(HAlign_Fill)					
						[
							SAssignNew(HeightBox, SArmyEditableNumberBox)
							.IsIntegerOnly(true)
							.SelectAllTextWhenFocused(true)
							.Text(this, &SArmyAddWallSetting::GetHeightValue)
							.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
							.OnTextCommitted_Raw(this, &SArmyAddWallSetting::OnHeightValueChanged)
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(10, 0, 20, 0)
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("mm")))
							.TextStyle(FArmyStyle::Get(), "ArmyText_12")
							.Justification(ETextJustify::Center)
						]
					]
				]
				+ SVerticalBox::Slot()
				.FillHeight(1.f)				
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.Padding(20, 0, 10, 0)
					.AutoWidth()
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Center)
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("逆时针")))
						.TextStyle(FArmyStyle::Get(), "ArmyText_12")
						.Justification(ETextJustify::Center)
					]
					+ SHorizontalBox::Slot()
					.FillWidth(1.f)
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Fill)
					.Padding(15, 0, 0, 0)
					[
						SAssignNew(CheckBox, SCheckBox)
						.Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("SingleCheckBox"))
						.IsChecked(ECheckBoxState::Checked)
					]
				]
			]
		];
		HeightContainBox->SetVisibility(bNeedHeight ? EVisibility::Visible : EVisibility::Collapsed);
}

void SArmyAddWallSetting::OnConfirmClicked()
{
	int InThincknessValue = FCString::Atoi(*(ThicknessBox->GetText().ToString()));
	int InHeightValue = FCString::Atoi(*(HeightBox->GetText().ToString()));
	if (InThincknessValue != CachedThicknessValue)
	{
		CachedThicknessValue = FMath::Clamp(InThincknessValue, MinValue, MaxValue);
	}
	if (InHeightValue != CachedHeightValue)
	{
		int maxHeight = FArmySceneData::WallHeight * 10.f;
		CachedHeightValue = FMath::Clamp(InHeightValue, 100, maxHeight);
	}
}

void SArmyAddWallSetting::OnThicknessValueChanged(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter || CommitType == ETextCommit::OnUserMovedFocus)
	{
		int InValue = FCString::Atoi(*InText.ToString());
		if (InValue != CachedThicknessValue)
		{
			CachedThicknessValue = FMath::Clamp(InValue, MinValue, MaxValue);
		}
	}
}

void SArmyAddWallSetting::OnHeightValueChanged(const FText& InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter || CommitType == ETextCommit::OnUserMovedFocus)
	{
		int InValue = FCString::Atoi(*InText.ToString());
		if (InValue != CachedHeightValue)
		{
			int maxHeight = FArmySceneData::WallHeight * 10.f;
			CachedHeightValue = FMath::Clamp(InValue, 100, maxHeight);
		}
	}
}
