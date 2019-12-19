#include "SArmyDesignTitleBar.h"
#include "ArmyDesignEditor.h"
#include "UICommandList.h"
#include "SImage.h"
#include "ArmyStyle.h"
#include "SArmyCheckBoxGroup.h"
#include "ArmyUser.h"
#include "SArmyComboboxItem.h"

#define LOCTEXT_NAMESPACE "SArmyDesignTitleBar"

void SArmyDesignTitleBar::Construct(const FArguments& InArgs)
{
    OwnerEditorPtr = InArgs._OwnerEditorPtr;

	ChildSlot
    .HAlign(HAlign_Center)
    .VAlign(VAlign_Top)
    [
        SAssignNew(Box_DesignModeList, SBox)
    ];
}

SArmyDesignTitleBar::~SArmyDesignTitleBar()
{
    OwnerEditorPtr = nullptr;
}

void SArmyDesignTitleBar::InitDesignModeList()
{
    CB_DesignModeList = MakeDesignModeComboBox();
    Box_DesignModeList->SetContent(CB_DesignModeList.ToSharedRef());
}

TSharedRef<SWidget> SArmyDesignTitleBar::OnGenerateWidget(TSharedPtr<FString> InItem)
{
	/**@梁晓菲 存储ComboboxItem*/
	TSharedPtr<SArmyComboboxItem> ComboboxItem;

	SAssignNew(ComboboxItem, SArmyComboboxItem)
        .Height(45)
		.NormalImage(FArmyStyle::Get().GetBrush(FName(**InItem)))
		.SelectedImage(FArmyStyle::Get().GetBrush(FName(*(*InItem + "_Active"))))
		.TextNormalColor(FArmyStyle::Get().GetColor("Color.Gray.FFC8C9CC"))
		.TextSelectedColor(FArmyStyle::Get().GetColor("Color.FFFF9800"))
		.Text(FText::FromString(*InItem))
        .TextStyle(FArmyStyle::Get(), "ArmyText_14");

	ComboboxUIList.Add(ComboboxItem);

	return ComboboxItem.ToSharedRef();
}

void SArmyDesignTitleBar::OnDesignModeChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
    if (!NewSelection.IsValid())
    {
        return;
    }

    // 如果模式EndMode返回true，则不允许切换模式，相应的UI选中态也不能变
    bool bNeedClearSelection = false;

	FString TempSelectDesignMode = SelectedDesignMode;
    if (SelectInfo == ESelectInfo::OnMouseClick)
    {    						
		if (!OwnerEditorPtr.Pin()->SetCurrentDesignMode(*NewSelection.Get()))
		{
            bNeedClearSelection = true;
		}
		else
		{
			SelectedDesignMode = *NewSelection.Get();
			TempSelectDesignMode = SelectedDesignMode;
		}
    }
    else if (SelectInfo == ESelectInfo::Direct) // 如果通过代码设置选中项，不会真正切换模式，只是切换UI选中态
    {
        SelectedDesignMode = *NewSelection.Get();
        TempSelectDesignMode = SelectedDesignMode;
    }

	/**@梁晓菲 遍历ComboboxList，将非选中项设为正常*/
	for (int i = 0;i<ComboboxUIList.Num();i++)
	{
		if (ComboboxUIList[i]->GetText().Equals(TempSelectDesignMode))
		{
            ComboboxUIList[i]->SetTextColorAndImageBrushSelected();
		}
        else
        {
            ComboboxUIList[i]->SetTextColorAndImageBrushNormal();
        }
	}

    if (bNeedClearSelection)
    {
        CB_DesignModeList->ClearSelection();
    }
}

FText SArmyDesignTitleBar::GetSelectedDesignMode() const
{
    return FText::FromString(SelectedDesignMode);
}

const FSlateBrush* SArmyDesignTitleBar::GetSelectedDesignModeBrush() const
{
    FString DisignModeActiveBrushName = GetSelectedDesignMode().ToString() + TEXT("_Active");

    return FArmyStyle::Get().GetBrush(FName(*DisignModeActiveBrushName));
}

TSharedPtr< SComboBox< TSharedPtr<FString> > > SArmyDesignTitleBar::MakeDesignModeComboBox()
{
    ModeNameArray = OwnerEditorPtr.Pin()->GetModeNameArray();
    SelectedDesignMode = *ModeNameArray[0].Get();
    OwnerEditorPtr.Pin()->SetCurrentDesignMode(SelectedDesignMode);

    return
        SNew(SComboBox< TSharedPtr<FString> >)
        .ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.None"))
        .ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.None"))
        .ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.ModeChange"))
        .OptionsSource(&ModeNameArray)
        .InitiallySelectedItem(ModeNameArray[0])
        .OnGenerateWidget(this, &SArmyDesignTitleBar::OnGenerateWidget)
        .OnSelectionChanged(this, &SArmyDesignTitleBar::OnDesignModeChanged)
        .Content()
        [
            SNew(SBox)
            .WidthOverride(148)
            .HeightOverride(40)
            .VAlign(VAlign_Center)
            [
                SNew(SHorizontalBox)

                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(16, 0, 8, 0)
                .VAlign(VAlign_Center)
                [
                    SNew(SImage)
                    .Image(this, &SArmyDesignTitleBar::GetSelectedDesignModeBrush)
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(this, &SArmyDesignTitleBar::GetSelectedDesignMode)
                    .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_14"))
                    .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFFF9800"))
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    SNew(SImage)
                    .Image(FArmyStyle::Get().GetBrush("Icon.DownArrow_Orange"))
                ]
            ]
        ];
}

#undef  LOCTEXT_NAMESPACE