#include "SArmyDynamicFontSize.h"
#include "ArmyStyle.h"
#include "ArmyDownLeadLabel.h"
#include "ArmyDimensions.h"
#include "ArmyGameInstance.h"

void SArmyDynamicFontSize::Construct(const FArguments& InArgs)
{
	for (int32 i = 6; i < 54; i += 2)
	{
		TextSizeList.AddUnique(MakeShared<int32>(i));
	}

	ChildSlot
		[
			SNew(SBox)
			.WidthOverride(218)
			.HeightOverride(64)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Padding(0)
			[
				SNew(SBorder)
				.Padding(FMargin(0))
				.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
				.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Top)
					.HAlign(HAlign_Left)
					.Padding(20,24,0,23)
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("字体大小")))
					]
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Top)
					.HAlign(HAlign_Right)
					.Padding(0,20,20,20)
					[
						SNew(SBox)
						.WidthOverride(114)
						.HeightOverride(24)
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SComboBox<TSharedPtr<int32>>)
							.ContentPadding(FMargin(0, 0, 0, 0))
							.ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.Black"))
							.ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.PropertyCombobox"))
							.HasDownArrow(true)
							.MaxListHeight(100)
							//.CustomScrollbar(SNew(SScrollBar))
							.OptionsSource(&TextSizeList)
							.OnGenerateWidget(this, &SArmyDynamicFontSize::GenerateFontSizeComboItem)
							.OnSelectionChanged(this, &SArmyDynamicFontSize::HandleFontSizeComboChanged)
							.ForegroundColor(FLinearColor::White)
							[
								SNew(STextBlock)
								.TextStyle(&FArmyStyle::GetWidgetStyle<FTextBlockStyle>("ArmyText_10"))
								.ColorAndOpacity(FLinearColor::White)
								.Text(this, &SArmyDynamicFontSize::GetFontSizeComboText)
							]
						]
					]	
				]
			]
		];
}
void SArmyDynamicFontSize::SetBoundObj(FObjectWeakPtr InObj)
{
	BoundObj = InObj;
}
TSharedRef<SWidget> SArmyDynamicFontSize::GenerateFontSizeComboItem(TSharedPtr<int32> InItem)
{
	return 	SNew(STextBlock).Text(FText::AsNumber(*InItem.Get())).ColorAndOpacity(FLinearColor::White);
}
void SArmyDynamicFontSize::HandleFontSizeComboChanged(TSharedPtr<int32> NewSelection, ESelectInfo::Type SelectInfo)
{
	if (BoundObj.IsValid())
	{
		switch (BoundObj.Pin()->GetType())
		{
		case OT_DownLeadLabel:
		{
			FArmyDownLeadLabel* TempLabel = BoundObj.Pin()->AsassignObj<FArmyDownLeadLabel>();
			if (TempLabel)
			{
				SCOPE_TRANSACTION(TEXT("修改字体大小"));
				TempLabel->SetTextSize(*NewSelection.Get());
				TempLabel->Modify();
			}
		}
		break;
		case OT_Dimensions:
		case OT_InSideWallDimensions:
		case OT_OutSideWallDimensions:
		{
			FArmyDimensions* TempDimension = BoundObj.Pin()->AsassignObj<FArmyDimensions>();
			if (TempDimension)
			{
				SCOPE_TRANSACTION(TEXT("修改字体大小"));
				TempDimension->SetTextSize(*NewSelection.Get());
				TempDimension->UpdateText();
				TempDimension->Modify();
			}
		}
		break;
		default:
			break;
		}

	}
}
FText SArmyDynamicFontSize::GetFontSizeComboText() const
{
	if (BoundObj.IsValid())
	{
		switch (BoundObj.Pin()->GetType())
		{
		case OT_DownLeadLabel:
		{
			FArmyDownLeadLabel* TempLabel = BoundObj.Pin()->AsassignObj<FArmyDownLeadLabel>();
			if (TempLabel)
			{
				return FText::AsNumber(TempLabel->GetTextSize());
			}
		}
		break;
		case OT_Dimensions:
		case OT_InSideWallDimensions:
		case OT_OutSideWallDimensions:
		{
			FArmyDimensions* TempDimension = BoundObj.Pin()->AsassignObj<FArmyDimensions>();
			if (TempDimension)
			{
				return FText::AsNumber(TempDimension->GetTextSize());
			}
		}
		break;
		default:
			break;
		}
	}
	return FText::AsNumber(12);
}