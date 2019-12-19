#include "ArmyTextCheckBoxSlate.h"
#include "ArmyStyle.h"

#include "SToolTip.h"

void SArmyTextCheckBox::Construct(const FArguments& InArgs)
{
	CheckStateChanged = InArgs._OnCheckStateChanged;
	IsCheckboxChecked = InArgs._IsChecked;
	NormalText = InArgs._NormalText;
	CheckText = InArgs._CheckText;
    ChildSlot
    [
        SNew(SBox)
        .WidthOverride(78)
        .HeightOverride(102)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SBorder)
                [
                    SAssignNew(CheckBox,SCheckBox)
					.OnCheckStateChanged(this,&SArmyTextCheckBox::OnCheckStateChanged)
					.IsChecked(this,&SArmyTextCheckBox::IsChecked)
                    .Style(InArgs._CheckBoxStyle)
					.Content()
					[
						SNew(STextBlock)
						.Text(this,&SArmyTextCheckBox::Text)
						.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
					]
                ]
            ]
        ]
    ];
}

void SArmyTextCheckBox::SetCheckBoxStyle(const FCheckBoxStyle& InCheckBoxStyle)
{
	CheckBox->SetStyle(&InCheckBoxStyle);
}

void SArmyTextCheckBox::SetBorderColor(const FSlateColor& InBorderBackgroundColor)
{
	Border->SetBorderBackgroundColor(InBorderBackgroundColor);
}

void SArmyTextCheckBox::SetTextColor(const FLinearColor& TextColor)
{
	TB_Text->SetColorAndOpacity(TextColor);
}

ECheckBoxState SArmyTextCheckBox::IsChecked() const
{
	return IsCheckboxChecked.Get();
}

void SArmyTextCheckBox::OnCheckStateChanged(ECheckBoxState InCheck)
{
	if (!IsCheckboxChecked.IsBound())
	{
		IsCheckboxChecked.Set(InCheck);
	}
	CheckStateChanged.ExecuteIfBound(InCheck);
}

FText SArmyTextCheckBox::Text()const
{
	if (IsCheckboxChecked.Get() == ECheckBoxState::Checked)
		return CheckText;
	else if (IsCheckboxChecked.Get() == ECheckBoxState::Unchecked)
		return NormalText;
	return NormalText;
}
