#include "ArmyComboBoxSlate.h"
#include "ThumbnailRendering/SArmyThumbnailItem.h"
#include "ArmyStyle.h"
#include "Widgets/SToolTip.h"

void SArmyPropertyComboBox::Construct(const FArguments& InArgs)
{
    OptionsSource = InArgs._OptionsSource;
    OnSelectionChanged = InArgs._OnSelectionChanged;
	BoxItemHorizontalAlignment = InArgs._BoxItemHorizontalAlignment.Get();
	check(OptionsSource.IsValid());
	BoxImagePadding = InArgs._BoxImagePadding;
    ChildSlot
    [
        SAssignNew(ComboBoxWidget, SComboBox< TSharedPtr<FArmyKeyValue> >)
        .OptionsSource(&OptionsSource->Array)
        .ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.None"))
        .ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.Property"))
        .ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("ComboBox.Button.Property"))
        .OnGenerateWidget(this, &SArmyPropertyComboBox::OnGenerateWidget)
        .OnSelectionChanged(this, &SArmyPropertyComboBox::OnSelectionChanged_Internal)
        .OnComboBoxOpening(this, &SArmyPropertyComboBox::OnComboBoxOpen)
        .OnComboBoxCloseing(this, &SArmyPropertyComboBox::OnComboBoxClose)
        .MaxListHeight(350)
        .ContentPadding(FMargin(0))
        .Content()
        [
            SNew(SBox)
            .WidthOverride(InArgs._Width)
            .HeightOverride(InArgs._Height)
            .VAlign(VAlign_Center)
            [
                SNew(SHorizontalBox)

                + SHorizontalBox::Slot()
				// .AutoWidth()
                .Padding(FMargin(8, 0, 0, 0))
                .VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
                [
                    SAssignNew(ValueTextBlock, STextBlock)
                    .Text(InArgs._Value)
                    .TextStyle(FArmyStyle::Get(), "ArmyText_12")
                ]

                + SHorizontalBox::Slot()
                // .FillWidth(1)
				.AutoWidth()
                .VAlign(VAlign_Center)
                .HAlign(HAlign_Right)
                [
                    SAssignNew(ArrowImage, SImage)
                    .Image(FArmyStyle::Get().GetBrush("Icon.DownArrow_White"))
                ]
            ]
        ]
    ];

    // 初始化选中项
    if (OptionsSource->Array.Num() > 0)
    {
        OnSelectionChanged_Internal(OptionsSource->Array[0], ESelectInfo::Direct);

		
    }

	ComboBoxWidget->SetToolTip(
		SNew(SToolTip)
		.Text(InArgs._Value)
		.Font(FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12").Font)
		.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
		.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
	);
}

void SArmyPropertyComboBox::SetSelectedItem(TSharedPtr<FArmyKeyValue> InItem)
{
    ComboBoxWidget->ClearSelection();
    OnSelectionChanged_Internal(InItem, ESelectInfo::Direct);
}

void SArmyPropertyComboBox::RefreshOptions()
{
    ComboBoxWidget->RefreshOptions();
}

TSharedRef<SWidget> SArmyPropertyComboBox::OnGenerateWidget(TSharedPtr<FArmyKeyValue> InItem)
{
    TSharedPtr<SArmyComboboxItem> ComboboxItem;

    SAssignNew(ComboboxItem, SArmyComboboxItem)
		.ImagePadding(BoxImagePadding)
		.ItemHorizontalAlignment(BoxItemHorizontalAlignment)
        .TextNormalColor(FLinearColor::White)
        .TextSelectedColor(FArmyStyle::Get().GetColor("Color.FFFF9800"))
        .Text(FText::FromString(InItem->Value));

    ComboboxUIList.Add(ComboboxItem);

    // 设置默认选中项的样式
    if (InItem == SelectedItem)
    {
        ComboboxItem->SetTextColorAndImageBrushSelected();
    }

    return ComboboxItem.ToSharedRef();
}

void SArmyPropertyComboBox::OnSelectionChanged_Internal(TSharedPtr<FArmyKeyValue> NewSelection, ESelectInfo::Type SelectInfo)
{
    if (NewSelection.IsValid() && (SelectInfo == ESelectInfo::OnMouseClick || SelectInfo == ESelectInfo::Direct))
    {			
        SelectedItem = NewSelection;

        /** @欧石楠 遍历ComboboxList，将非选中项设为正常 */
        for (int i = 0; i < ComboboxUIList.Num(); i++)
        {
            if (ComboboxUIList[i]->GetText() != *(NewSelection->Value))
            {
                ComboboxUIList[i]->SetTextColorAndImageBrushNormal();
            }
            else
            {
                ComboboxUIList[i]->SetTextColorAndImageBrushSelected();
            }
        }

		if (SelectInfo != ESelectInfo::Direct)
		{
			OnSelectionChanged.ExecuteIfBound(SelectedItem->Key, SelectedItem->Value);
		}        
    }
}

void SArmyPropertyComboBox::OnComboBoxOpen()
{
    ArrowImage->SetImage(FArmyStyle::Get().GetBrush("Icon.UpArrow_White"));
	/**@欧石楠 当打开下拉列表时，根据value去更新下拉列表中需要变色的选项*/
	for (auto &It : OptionsSource->Array)
	{
		if (It->Value == ValueTextBlock->GetText().ToString())
		{
			SetSelectedItem(It);
		}
	}	
}

void SArmyPropertyComboBox::OnComboBoxClose()
{
    ArrowImage->SetImage(FArmyStyle::Get().GetBrush("Icon.DownArrow_White"));
}

void SArmyTextPulldownBox::Construct(const FArguments& InArgs)
{
	ComboItemList = InArgs._ComboItemList;
	DisplayString = InArgs._DisplayString;
	OnComboBoxSelectionChanged = InArgs._OnComboBoxSelectionChanged;
	ChildSlot
	[
		SNew(SBox)
		.HeightOverride(30)
		[
			SAssignNew(MyComboBox, SComboBox< TSharedPtr<FString> >)
            .ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.None"))
			.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.None"))
            .ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.Modechange"))//@   欧石楠，用户详情下拉页Item样式
			.OptionsSource(&ComboItemList)
			.OnGenerateWidget(this, &SArmyTextPulldownBox::OnComboBoxGenerateWidgetEvent)
			.OnSelectionChanged(this, &SArmyTextPulldownBox::OnComboBoxSelectionChangedEvent)
			.HasDownArrow(true)
			.OnComboBoxOpening(this, &SArmyTextPulldownBox::OnComboboxOpen)
			.OnComboBoxCloseing(this, &SArmyTextPulldownBox::OnComboboxClose)
			.Content()
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SAssignNew(NumText, STextBlock)
					.Text(this, &SArmyTextPulldownBox::GetDisplayContent)
					.TextStyle(FArmyStyle::Get(), "ArmyText_12")
					.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.Gray.FFC8C9CC"))
				]

                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    SAssignNew(ArrowImage, SImage)
                   .Image(FArmyStyle::Get().GetBrush("Icon.DownArrow_Gray"))
                ]
			]
		]
	];
}

void SArmyTextPulldownBox::SetDisplayContent(FString content)
{
	DisplayString = content;
}

void SArmyTextPulldownBox::OnComboboxOpen()
{
	NumText->SetColorAndOpacity(FLinearColor::White);
	ArrowImage->SetImage(FArmyStyle::Get().GetBrush("Icon.UpArrow_White"));
}

void SArmyTextPulldownBox::OnComboboxClose()
{
	NumText->SetColorAndOpacity(FArmyStyle::Get().GetColor("Color.Gray.FFC8C9CC"));
	ArrowImage->SetImage(FArmyStyle::Get().GetBrush("Icon.DownArrow_Gray"));
}

FText SArmyTextPulldownBox::GetDisplayContent() const
{
	return FText::FromString(DisplayString);
}

void SArmyTextPulldownBox::OnComboBoxSelectionChangedEvent(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
	NumText->SetColorAndOpacity(FArmyStyle::Get().GetColor("Color.Gray.FFC8C9CC"));
	ArrowImage->SetImage(FArmyStyle::Get().GetBrush("Icon.DownArrow_Gray"));//欧石楠 下拉箭头

	if (NewSelection.IsValid()) {
		OnComboBoxSelectionChanged.ExecuteIfBound(*NewSelection);
	}	
	MyComboBox->ClearSelection();	
}

TSharedRef<SWidget> SArmyTextPulldownBox::OnComboBoxGenerateWidgetEvent(TSharedPtr<FString> InItem)
{
	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(FMargin(12,9,8,9))//@   欧石楠，用户详情下拉页间距
		[
			SNew(SBox)
			.WidthOverride(16)
			.HeightOverride(16)
			[
				SNew(SImage)
				.Image(FArmyStyle::Get().GetBrush(FName(**InItem)))
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(FMargin(0,0,16,0))//@   欧石楠，用户详情下拉页间距
		[
			SNew(STextBlock)
			.Text(FText::FromString(*InItem))
			.TextStyle(FArmyStyle::Get(), "ArmyText_12")
			.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.Gray.FFC8C9CC"))//@   欧石楠，用户详情下拉页字体颜色
		];
}