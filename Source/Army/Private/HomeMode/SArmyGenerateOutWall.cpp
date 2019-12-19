#include "SArmyGenerateOutWall.h"
#include "SBox.h"
#include "SBoxPanel.h"
#include "STextBlock.h"
#include "ArmyStyle.h"
#include "SArmyEditableNumberBox.h"

void SArmyGenerateOutWall::Construct(const FArguments & InArgs)
{
	ChildSlot
	[
		SNew(SBox)
		.WidthOverride(420)
		.HeightOverride(58)
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
			.Padding(0, 10, 0, 10)
			[
				SAssignNew(EditableNumberBox, SArmyEditableNumberBox)
				.IsIntegerOnly(true)
				.SelectAllTextWhenFocused(true)
				.Text(this, &SArmyGenerateOutWall::GetValue)
				.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
				.OnTextCommitted_Raw(this, &SArmyGenerateOutWall::OnValueChanged)				
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
	];
}

void SArmyGenerateOutWall::OnConfirmClicked()
{	
	int InValue = FCString::Atoi(*(EditableNumberBox->GetText().ToString()));
	if (InValue != CachedValue)
	{
		if (InValue < MinValue)
		{
			InValue = MinValue;
		}
		if (InValue > MaxValue)
		{
			InValue = MaxValue;
		}
		CachedValue = InValue;
	}
}

void SArmyGenerateOutWall::OnValueChanged(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter || CommitType == ETextCommit::OnUserMovedFocus)
	{
		int InValue = FCString::Atoi(*InText.ToString());
		if (InValue != CachedValue)
		{
			if (InValue < MinValue)
			{
				InValue = MinValue;
			}
			if (InValue > MaxValue)
			{
				InValue = MaxValue;
			}
			CachedValue = InValue;
		}
	}
}
