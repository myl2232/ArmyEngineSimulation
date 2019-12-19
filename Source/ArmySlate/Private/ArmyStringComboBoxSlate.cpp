#include "ArmyStringComboBoxSlate.h"
#include "ArmyStyle.h"
#include "Widgets/SToolTip.h"


void SArmyStringComboBox::Construct(const FArguments& InArgs)
{
	OptionsSource = InArgs._OptionsSource;

	SelectionChangeEvent = InArgs._OnSelected;

	ChildSlot
		[
			SAssignNew(ComboBoxWidget, SComboBox< TSharedPtr<FString> >)
			.OptionsSource(&OptionsSource)
			.ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.Black"))
			.ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.PropertyCombobox"))
			.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("ComboBox.Button.FF212224"))
			.HasDownArrow(true)
			.ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
			.OnGenerateWidget(this, &SArmyStringComboBox::GenerateComboItem)
			.OnSelectionChanged(this, &SArmyStringComboBox::OnSelectionChanged)
			.MaxListHeight(350)
			.ContentPadding(FMargin(0))
			.Content()
			[
				SNew(SBox)
				.WidthOverride(InArgs._Width)
				.HeightOverride(InArgs._Height)
				.VAlign(VAlign_Center)
				[
					SAssignNew(ValueTextBlock, STextBlock)
					.Text(this, &SArmyStringComboBox::GetSelectedComboText)
					.TextStyle(FArmyStyle::Get(), "ArmyText_12")
				]
			]
		];

	// 初始化选中项
	if (OptionsSource.Num() > 0)
	{
		if (InArgs._DefaultItem != "")
		{			
			OnSelectionChanged(MakeShareable(new FString(InArgs._DefaultItem)), ESelectInfo::Direct);
		}
		else
		{
			OnSelectionChanged(OptionsSource[0], ESelectInfo::Direct);
		}	

		// @欧石楠 增加ToolTip，以便文字显示不全时能够在ToolTip中看到全部内容
		ComboBoxWidget->SetToolTip(
			SNew(SToolTip)
			.Text(this, &SArmyStringComboBox::GetSelectedComboText)
			.Font(FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12").Font)
			.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
			.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
		);	
	}
}

FString SArmyStringComboBox::GetSelectedItem()
{
	return SelectedString;
}

FText SArmyStringComboBox::GetSelectedComboText() const
{
	return FText::FromString(SelectedString);
}

void SArmyStringComboBox::SetSelectedItem(FString InItem)
{
		SelectedString = InItem;
}

TSharedRef<SWidget> SArmyStringComboBox::GenerateComboItem(TSharedPtr<FString> InItem)
{
	return
		SNew(STextBlock)
		.Text(FText::FromString(*InItem.Get()))
		.ColorAndOpacity(FLinearColor::White)
		.TextStyle(FCoreStyle::Get(), "VRSText_12");
}

void SArmyStringComboBox::OnSelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
	if (NewSelection.IsValid() && (SelectInfo == ESelectInfo::OnMouseClick || SelectInfo == ESelectInfo::Direct))
	{
		SelectedString = *NewSelection;
		if (SelectInfo == ESelectInfo::OnMouseClick)//如果是程序直接赋值则不出发代理
		{
			SelectionChangeEvent.ExecuteIfBound();
		}		
	}
}
