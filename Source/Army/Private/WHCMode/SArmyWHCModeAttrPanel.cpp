#include "SArmyWHCModeAttrPanel.h"
#include "SArmyWHCModeListPanel.h"
#include "SArmyWHCModeCabinetAttr.h"
#include "SArmyWHCModeDoorAttr.h"
#include "SArmyWHCModeDrawerAttr.h"
#include "SArmyWHCModePlatformAttr.h"
#include "SArmyWHCModeSideBoardAttr.h"
#include "SArmyWHCModeToeAttr.h"
#include "SArmyWHCModeTopBlockerAttr.h"
#include "SArmyWHCModeAccAttr.h"
#include "SArmyWHCModeEmbbedElecDevAttr.h"
#include "SArmyWHCModeWardrobeAttr.h"
#include "SArmyWHCModeCoverDoorWardrobeAttr.h"
#include "SArmyWHCModeOtherCabAttr.h"
#include "SArmyWHCModeVentilatorCabAttr.h"
#include "SArmyWHCModeBathroomCabAttr.h"
#include "ArmyWHCModeCabinetOperation.h"

#include "ArmyStyle.h"
#include "Frame/SArmyWindow.h"
#include "Game/XRViewportClient.h"
#include "Game/XRGameInstance.h"
#include "ArmyHttpModule.h"
#include "ArmyDownloadModule.h"
#include "SScrollWrapBox.h"
#include "SContentItem.h"
#include "Data/WHCModeData/XRWHCModeData.h"

#include "HttpMgr.h"

#include "Widgets/Input/SComboBox.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SToolTip.h"

void SArmyWHCModeNormalComboBox::Construct(const FArguments &InArgs)
{
    OptionsSource = InArgs._OptionSource;
    OnValueChanged = InArgs._OnValueChanged;

    ChildSlot
        [
            SAssignNew(ComboBox, SComboBox<TSharedPtr<FString>>)
            .OptionsSource(&OptionsSource)
            .ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox"))
            .ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.PropertyCombobox"))
            // .ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("ComboBox.Button.FF212224"))
            .OnGenerateWidget(this, &SArmyWHCModeNormalComboBox::OnGenerateWidget)
            .OnSelectionChanged(this, &SArmyWHCModeNormalComboBox::OnSelectionChanged)
            .MaxListHeight(350)
            .ContentPadding(FMargin(0.0f))
            .Content()
            [
                // SNew(SHorizontalBox)
                // + SHorizontalBox::Slot()
                // .FillWidth(1.0f)
                // .VAlign(VAlign_Center)
                // .Padding(FMargin(8.0f, 0.0f, 0.0f, 0.0f))
                // [
                //     SAssignNew(Value, STextBlock)
                //     .Text(FText::FromString(InArgs._Value))
                //     .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
                //     .ColorAndOpacity(FLinearColor::White)
                // ]
                // + SHorizontalBox::Slot()
                // .AutoWidth()
                // .VAlign(VAlign_Center)
                // .HAlign(HAlign_Right)
                // .Padding(0, 0, 4, 0)
                // [
                //     SNew(SImage)
                //     .Image(FArmyStyle::Get().GetBrush("Icon.DownArrow_White"))
                // ]

                SAssignNew(Value, STextBlock)
                .Text(FText::FromString(InArgs._Value))
                .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
                .ColorAndOpacity(FLinearColor::White)
            ]
        ];
}

void SArmyWHCModeNormalComboBox::SetOptionSource(const TArray<TSharedPtr<FString>> &InOptionSource)
{
    OptionsSource.Empty();
    for (const auto &NewSource : InOptionSource)
        OptionsSource.Emplace(NewSource);
    ComboBox->RefreshOptions();
}

TSharedRef<SWidget> SArmyWHCModeNormalComboBox::OnGenerateWidget(TSharedPtr<FString> InItem)
{
    return SNew(SBox)
            .HeightOverride(32.0f)
            .VAlign(VAlign_Center)
            .Padding(FMargin(8.0f, 0.0f, 0.0f, 0.0f))
            [
                SNew(STextBlock)
                .Text(FText::FromString(*InItem.Get()))
                .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
                .ColorAndOpacity(FLinearColor::White)
            ];
}

void SArmyWHCModeNormalComboBox::OnSelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
    if (SelectInfo == ESelectInfo::OnMouseClick)
    {
        // FText NewText = FText::FromString(*NewSelection.Get());
        // Value->SetText(NewText);
        OnValueChanged.ExecuteIfBound(*NewSelection.Get());
    }
}

void SArmyWHCModeNormalComboBox::SetValueAttr(const TAttribute<FText> &InText)
{
    Value->SetText(InText);
}

void SArmyWHCModeNormalComboBox::SetValue(const FString &InText)
{
    Value->SetText(FText::FromString(InText));
}

FString SArmyWHCModeNormalComboBox::GetValue() const
{
    return Value->GetText().ToString();
}

void SArmyWHCModeModificationRange::Construct(const FArguments &InArgs)
{
    TSharedPtr<SHorizontalBox> RangeContainer;
    ChildSlot
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SBox)
                .HeightOverride(40.0f)
                [
                    SNew(SBorder)
                    .Padding(FMargin(16.0f, 0.0f, 16.0f, 0.0f))
                    .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
                    .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
                    [
                        SAssignNew(RangeContainer, SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        .Padding(0.0f, 0.0f, 4.0f, 0.0f)
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("应用到: ")))
                            .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
                            .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FF7D7E80"))
                        ]                                            
                    ]
                ]
            ]
        ];

    if (InArgs._bHasSingleRadio)
    {
        RangeContainer->AddSlot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        [
            SAssignNew(SingleRadio, SCheckBox)
            .Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("Menu.CheckBox"))
            .IsChecked(ECheckBoxState::Checked)
            .Padding(FMargin(8.0f, 0.0f, 8.0f, 0.0f))
            .ForegroundColor(FArmyStyle::Get().GetColor("Color.FFC8C9CC"))
            .OnCheckStateChanged_Lambda(
                [this](ECheckBoxState InState)
                {
                    if (InState == ECheckBoxState::Checked)
                        SwitchRange(MR_Single);
                    else if (InState == ECheckBoxState::Unchecked)
                        SingleRadio->SetIsChecked(ECheckBoxState::Checked);
                }
            )
            [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("当前")))
                .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
            ]
        ];
    }

    if (InArgs._bHasCabRadio)
    {
        RangeContainer->AddSlot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        [
            SAssignNew(SingleCabRadio, SCheckBox)
            .Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("Menu.CheckBox"))
            .IsChecked(ECheckBoxState::Unchecked)
            .Padding(FMargin(8.0f, 0.0f, 8.0f, 0.0f))
            .ForegroundColor(FArmyStyle::Get().GetColor("Color.FFC8C9CC"))
            .OnCheckStateChanged_Lambda(
                [this](ECheckBoxState InState)
                {
                    if (InState == ECheckBoxState::Checked)
                        SwitchRange(MR_SingleCab);
                    else if (InState == ECheckBoxState::Unchecked)
                        SingleCabRadio->SetIsChecked(ECheckBoxState::Checked);
                }
            )
            [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("整柜")))
                .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
            ]
        ];
    }

    if (InArgs._bHasSameTypeRadio)
    {
        RangeContainer->AddSlot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        [
            SAssignNew(SameTypeRadio, SCheckBox)
            .Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("Menu.CheckBox"))
            .IsChecked(ECheckBoxState::Unchecked)
            .Padding(FMargin(8.0f, 0.0f, 8.0f, 0.0f))
            .ForegroundColor(FArmyStyle::Get().GetColor("Color.FFC8C9CC"))
            .OnCheckStateChanged_Lambda(
                [this](ECheckBoxState InState)
                {
                    if (InState == ECheckBoxState::Checked)
                        SwitchRange(MR_SameType);
                    else if (InState == ECheckBoxState::Unchecked)
                        SameTypeRadio->SetIsChecked(ECheckBoxState::Checked);
                }
            )
            [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("同类")))
                .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
            ]
        ];
    }

    if (InArgs._bHasAllRadio)
    {
        RangeContainer->AddSlot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        [
            SAssignNew(AllRadio, SCheckBox)
            .Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("Menu.CheckBox"))
            .IsChecked(ECheckBoxState::Unchecked)
            .Padding(FMargin(8.0f, 0.0f, 8.0f, 0.0f))
            .ForegroundColor(FArmyStyle::Get().GetColor("Color.FFC8C9CC"))
            .OnCheckStateChanged_Lambda(
                [this](ECheckBoxState InState)
                {
                    if (InState == ECheckBoxState::Checked)
                        SwitchRange(MR_All);
                    else if (InState == ECheckBoxState::Unchecked)
                        AllRadio->SetIsChecked(ECheckBoxState::Checked);
                }
            )
            [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("全部")))
                .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
            ]
        ];
    }
}

void SArmyWHCModeModificationRange::Reset(EModificationRange InRange)
{
    SwitchRange(InRange);
}

void SArmyWHCModeModificationRange::SwitchRange(EModificationRange InNewRange)
{
    switch (InNewRange)
    {
        case MR_Single:
        {
            if (SingleRadio.IsValid())
                SingleRadio->SetIsChecked(ECheckBoxState::Checked);
            if (SingleCabRadio.IsValid())
                SingleCabRadio->SetIsChecked(ECheckBoxState::Unchecked);
            if (SameTypeRadio.IsValid())
                SameTypeRadio->SetIsChecked(ECheckBoxState::Unchecked);
            if (AllRadio.IsValid())
                AllRadio->SetIsChecked(ECheckBoxState::Unchecked);
            break;
        }
        case MR_SingleCab:
        {
            if (SingleRadio.IsValid())   
                SingleRadio->SetIsChecked(ECheckBoxState::Unchecked);
            if (SingleCabRadio.IsValid())
                SingleCabRadio->SetIsChecked(ECheckBoxState::Checked);
            if (SameTypeRadio.IsValid())
                SameTypeRadio->SetIsChecked(ECheckBoxState::Unchecked);
            if (AllRadio.IsValid())
                AllRadio->SetIsChecked(ECheckBoxState::Unchecked);
            break;
        }
        case MR_SameType:
        {
            if (SingleRadio.IsValid())
                SingleRadio->SetIsChecked(ECheckBoxState::Unchecked);
            if (SingleCabRadio.IsValid())
                SingleCabRadio->SetIsChecked(ECheckBoxState::Unchecked);
            if (SameTypeRadio.IsValid())
                SameTypeRadio->SetIsChecked(ECheckBoxState::Checked);
            if (AllRadio.IsValid())
                AllRadio->SetIsChecked(ECheckBoxState::Unchecked);
            break;
        }
        case MR_All:
        {
            if (SingleRadio.IsValid())
                SingleRadio->SetIsChecked(ECheckBoxState::Unchecked);
            if (SingleCabRadio.IsValid())
                SingleCabRadio->SetIsChecked(ECheckBoxState::Unchecked);
            if (SameTypeRadio.IsValid())
                SameTypeRadio->SetIsChecked(ECheckBoxState::Unchecked);
            if (AllRadio.IsValid())
                AllRadio->SetIsChecked(ECheckBoxState::Checked);
            break;
        }
    }
    MRange = InNewRange;
}

void SArmyWHCModeReplacingWidget::Construct(const FArguments &InArgs)
{
    OnWholeWidgetClicked = InArgs._OnWholeWidgetClicked;
    OnButtonClicked = InArgs._OnButtonClicked;

    ChildSlot
        [
            SAssignNew(Container, SBox)
            .HeightOverride(32.0f)
            [
                SNew(SBorder)
                .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF38393D"))
                .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
                .Padding(FMargin(0.0f))
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SNew(SBox)
                        .WidthOverride(32.0f)
                        [
                            SAssignNew(ThumbnailImage, SImage)
                        ]
                    ]
                    + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    .VAlign(VAlign_Center)
                    .Padding(8.0f, 0.0f, 8.0f, 0.0f)
                    [
                        SAssignNew(Text, STextBlock)
                        .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
                        .ColorAndOpacity(FLinearColor::White)
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(0.0f, 4.0f, 2.0f, 4.0f)
                    [
                        SAssignNew(ButtonImage, SImage)
                        .Image(InArgs._ButtonImage)
                        // .Image(FArmyStyle::Get().GetBrush("WHCMode.Replace"))
                    ]
                ]
            ]
        ];

    Container->SetToolTip(
        SNew(SToolTip)
        .Text_Lambda(
            [this]() -> FText {
                return Text->GetText();
            }
        )
        .Font(FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12").Font)
        .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
        .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
    );

    Container->SetOnMouseButtonUp(FPointerEventHandler::CreateLambda(
        [this](const FGeometry&, const FPointerEvent&) -> FReply {
            OnWholeWidgetClicked.ExecuteIfBound();
            return FReply::Handled();
        }
    ));
    ButtonImage->SetOnMouseButtonUp(FPointerEventHandler::CreateLambda(
        [this](const FGeometry&, const FPointerEvent&) -> FReply {
            OnButtonClicked.ExecuteIfBound();
            return FReply::Handled();
        }
    ));
}

void SArmyWHCModeReplacingWidget::SetReplacingItemThumbnail(TAttribute<const FSlateBrush *> InThumbnail)
{
    ThumbnailImage->SetImage(InThumbnail);
}

void SArmyWHCModeReplacingWidget::SetReplacingItemText(const TAttribute<FText> &InText)
{
    Text->SetText(InText);
}

void SArmyWHCModeReplacingWidget::SetReplacingItemText(const FText &InText)
{
    Text->SetText(InText);
}

void SArmyWHCModeReplacingWidget::SetReplacingItemToolTip(const FText &InText)
{
    if (InText.IsEmpty())
        Container->SetToolTip(TSharedPtr<SToolTip>());
    else
        Container->SetToolTip(
            SNew(SToolTip)
            .Text(InText)
            .Font(FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12").Font)
            .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
            .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
        );
}

void SArmyWHCModeReplacingWidget::SetButtonImage(const FSlateBrush *InButtonImage)
{
    ButtonImage->SetImage(InButtonImage);
}

FSimpleDelegate& SArmyWHCModeReplacingWidget::GetWholeWidgetClicked()
{
    if (OnWholeWidgetClicked.IsBound())
        OnWholeWidgetClicked.Unbind();
    return OnWholeWidgetClicked;
}

FSimpleDelegate& SArmyWHCModeReplacingWidget::GetButtonClicked()
{
    if (OnButtonClicked.IsBound())
        OnButtonClicked.Unbind();
    return OnButtonClicked;
}

void SArmyWHCModeReplacingWidgetWithOverlayMenu::Construct(const FArguments &InArgs)
{
    TSharedPtr<SImage> MenuImage;

    ChildSlot
        [
            SAssignNew(Container, SBox)
            .HeightOverride(32.0f)
            [
                SNew(SBorder)
                .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF38393D"))
                .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
                .Padding(FMargin(0.0f))
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SNew(SBox)
                        .WidthOverride(32.0f)
                        [
                            SAssignNew(ThumbnailImage, SImage)
                        ]
                    ]
                    + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    .VAlign(VAlign_Center)
                    .Padding(8.0f, 0.0f, 8.0f, 0.0f)
                    [
                        SAssignNew(Text, STextBlock)
                        .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
                        .ColorAndOpacity(FLinearColor::White)
                    ]
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(0.0f, 4.0f, 2.0f, 4.0f)
                    [
                        SAssignNew(MenuImage, SImage)
                        .Image(FArmyStyle::Get().GetBrush("WHCMode.Menu"))
                    ]
                ]
            ]
        ];

    if (InArgs._OverlayWidgetForMenu.IsValid())
    {
        MenuSlot = &InArgs._OverlayWidgetForMenu->AddSlot();
        OverlayRef = InArgs._OverlayWidgetForMenu;
    }

    OnMenuItemSelectionChanged = InArgs._OnMenuItemSelectionChanged;

    MenuOptions.Emplace(MakeShareable(new FString(TEXT("替换"))));
    MenuOptions.Emplace(MakeShareable(new FString(TEXT("删除"))));
    SAssignNew(Menu, SBox)
    .WidthOverride(68.0f)
    [
        SNew(SBorder)
        .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF383A3D"))
        .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
        .Padding(FMargin(0.0f))
        [
            SNew(SListView<TSharedPtr<FString>>)
            .ItemHeight(32.0f)
            .SelectionMode(ESelectionMode::Single)
            .ListItemsSource(&MenuOptions)
            .OnGenerateRow_Lambda(
                [](TSharedPtr<FString> Item, const TSharedRef<STableViewBase> &TableView) -> TSharedRef<ITableRow>
                {
                    return SNew(STableRow<TSharedPtr<FString>>, TableView)
                        .Padding(FMargin(22.0f, 10.0f, 0.0f, 10.0f))
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(*Item.Get()))
                            .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
                        ];
                }
            )
            .OnSelectionChanged_Lambda(
                [this](TSharedPtr<FString> Item, ESelectInfo::Type InType)
                {
                    if (InType == ESelectInfo::OnMouseClick)
                    {
                        OnMenuItemSelectionChanged.ExecuteIfBound(*Item);
                    }
                }
            )
        ]
    ];
    
    MenuImage->SetOnMouseButtonUp(
        FPointerEventHandler::CreateLambda(
            [this](const FGeometry &InGeometry, const FPointerEvent &InEvent) -> FReply
            {
                const FGeometry & Geometry = OverlayRef.Pin()->GetCachedGeometry();
                float DeltaY = InGeometry.AbsolutePosition.Y - Geometry.AbsolutePosition.Y;
                MenuSlot->HAlign(HAlign_Right).VAlign(VAlign_Top).Padding(0.0f, DeltaY + 32.0f, 16.0f, 0.0f)
                .operator[](Menu.ToSharedRef());
                return FReply::Handled();
            }
        )
    );
}

void SArmyWHCModeReplacingWidgetWithOverlayMenu::SetReplacingItemThumbnail(TAttribute<const FSlateBrush *> InThumbnail)
{
    ThumbnailImage->SetImage(InThumbnail);
}

void SArmyWHCModeReplacingWidgetWithOverlayMenu::SetReplacingItemText(const FText &InText)
{
    Text->SetText(InText);
}

void SArmyWHCModeReplacingWidgetWithOverlayMenu::SetReplacingItemToolTip(const FText &InText)
{
    if (InText.IsEmpty())
        Container->SetToolTip(TSharedPtr<SToolTip>());
    else
        Container->SetToolTip(
            SNew(SToolTip)
            .Text(InText)
            .Font(FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12").Font)
            .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
            .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
        );
}

void SArmyWHCModeAddDelWidget::Construct(const FArguments &InArgs)
{
    OnAdd = InArgs._OnAdd;
    OnDel = InArgs._OnDel;

    SAssignNew(AddButtonContainer, SBox)
    .WidthOverride(32.0f)
    .HeightOverride(32.0f)
    [
        SNew(SButton)
        .ContentPadding(FMargin(8.0f))
        .ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Gray.FF353638"))
        .OnClicked(this, &SArmyWHCModeAddDelWidget::Callback_Add)
        [
            SNew(SImage)
            .Image(FArmyStyle::Get().GetBrush("Icon.Add"))
        ]
    ];

    SAssignNew(DelButtonContainer, SBox)
    .WidthOverride(114.0f)
    .HeightOverride(32.0f)
    [
        SNew(SBorder)
        .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF38393D"))
        .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
        .Padding(FMargin(0.0f))
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            .VAlign(VAlign_Center)
            .Padding(8.0f, 0.0f, 8.0f, 0.0f)
            [
                SAssignNew(DisplayText, STextBlock)
                .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(0.0f, 4.0f, 0.0f, 4.0f)
            [
                SNew(SButton)
                .ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Gray.FF353638"))
                .OnClicked(this, &SArmyWHCModeAddDelWidget::Callback_Del)
                [
                    SNew(SImage)
                    .Image(FArmyStyle::Get().GetBrush("Icon.delete"))
                ]
            ]
        ]
    ];

    DelButtonContainer->SetToolTip(
        SNew(SToolTip)
        .Text_Lambda(
            [this]() -> FText {
                return DisplayText->GetText();
            }
        )
        .Font(FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12").Font)
        .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FF2A2B2E"))
        .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
    );

    ChildSlot[AddButtonContainer.ToSharedRef()];
}

void SArmyWHCModeAddDelWidget::SetAddOrDel(bool IsAdded)
{
    if (IsAdded)
        ChildSlot[DelButtonContainer.ToSharedRef()];
    else
        ChildSlot[AddButtonContainer.ToSharedRef()];
}

void SArmyWHCModeAddDelWidget::SetDisplayTextAttr(const TAttribute<FText> &InText)
{
    DisplayText->SetText(InText);
}

void SArmyWHCModeAddDelWidget::SetDisplayText(const FText &InText)
{
    DisplayText->SetText(InText);
}

void SArmyWHCModeAddDelWidget::SetDisplayToolTip(const FText &InText)
{
    if (InText.IsEmpty())
        DelButtonContainer->SetToolTip(TSharedPtr<SToolTip>());
    else
        DelButtonContainer->SetToolTip(
            SNew(SToolTip)
            .Text(InText)
            .Font(FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12").Font)
            .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FF2A2B2E"))
            .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
        );
}

FReply SArmyWHCModeAddDelWidget::Callback_Add()
{
    if (OnAdd.IsBound())
    {
        if (OnAdd.Execute())
            ChildSlot[DelButtonContainer.ToSharedRef()];
    }
    return FReply::Handled();
}

FReply SArmyWHCModeAddDelWidget::Callback_Del()
{
    ChildSlot[AddButtonContainer.ToSharedRef()];
    OnDel.ExecuteIfBound();
    return FReply::Handled();
}

FArmyWHCModeAttrPanel::FArmyWHCModeAttrPanel(TSharedRef<SArmyWHCModeAttrPanelContainer> InContainer)
: Container(InContainer)
{}

void FArmyWHCModeAttrPanel::ShowLoadingThrobber(const FString &InStr)
{
    GGI->Window->ShowThrobber(FText::FromString(InStr));
}

void FArmyWHCModeAttrPanel::HideLoadingThrobber()
{
    GGI->Window->HideThrobber();
}

bool FArmyWHCModeAttrPanel::IsValidNumberText(const FString &InStr) const
{
    int32 Index = INDEX_NONE, IndexLast = INDEX_NONE;
    if (InStr.FindChar(TEXT('-'), Index) && InStr.FindLastChar(TEXT('-'), IndexLast))
    {
        if ((Index == 0) && (Index == IndexLast) && InStr.Len() > 1) // 只有一个负号，且在字符串开头
            return true;
        else 
            return false;
    }
    else
        return true;
}

void FArmyWHCModeAttrPanel::UpdateProgress(const TSharedPtr<FDownloadFileSet> &InTask, SContentItem *InContentItem)
{
    // 设置下载进度
    float Progress = InTask->GetProgress();
    InContentItem->SetDownloadText(FText::FromString(FString::Printf(TEXT("%.2f%%"), 20.0f + Progress * 0.8f)));
    InContentItem->SetPercent(Progress);
}

void SArmyWHCModeAttrPanelContainer::Construct(const FArguments &InArgs)
{
    CabinetOperationRef = InArgs._CabinetOperation;

    ChildSlot
        [
            SNew(SVerticalBox)
			+ SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(HAlign_Fill)
            [
                SNew(SBox)
                .HeightOverride(32.0f)
                [
                    SNew(SBorder)
                    .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF212226"))
                    .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
                    .Padding(FMargin(16.0f, 0.0f, 0.0f, 0.0f))
                    .VAlign(VAlign_Center)
                    .Content()
		            [
			            SNew(STextBlock)
			            .Text(FText::FromString(TEXT("属性")))
		                .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		            ]
                ]
            ]
            + SVerticalBox::Slot()
            .FillHeight(1.0f)
            [
                SAssignNew(ContentOverlay, SOverlay)
                + SOverlay::Slot()
                [
                    SAssignNew(ContentSlot, SBorder)
                    .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
                    .Padding(FMargin(0.0f, 8.0f, 0.0f, 0.0f))
                ]
            ]
        ];

    ReplacingSlot = &(GVC->ViewportOverlayWidget->AddSlot()
                        .VAlign(VAlign_Fill)
                        .HAlign(HAlign_Right)
                        .Padding(FMargin(0.0f, 2.0f, 2.0f, 120.0f)));

    SAssignNew(ListPanel, SArmyWHCModeListPanel).OnPanelClosed(this, &SArmyWHCModeAttrPanelContainer::CloseReplacingPanel);
    ListPanel->Container->Delegate_ScrollToEnd.BindSP(this, &SArmyWHCModeAttrPanelContainer::Callback_LoadMoreReplacingData);

    LastWHCItemSelected = nullptr;
}

void SArmyWHCModeAttrPanelContainer::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

    if (CurrentAttrPanel.IsValid())
        CurrentAttrPanel.Pin()->TickPanel();
}

void SArmyWHCModeAttrPanelContainer::Initialize()
{
    TSharedRef<SArmyWHCModeAttrPanelContainer> ContainerRef = StaticCastSharedRef<SArmyWHCModeAttrPanelContainer>(AsShared());

    TSharedPtr<FArmyWHCModeVentilatorCabinetAttr> VentilatorCabinetAttr = MakeShareable(new FArmyWHCModeVentilatorCabinetAttr(ContainerRef));
    VentilatorCabinetAttr->Initialize();
    AttrPanels.Emplace(VentilatorCabinetAttr);

    TSharedPtr<FArmyWHCModeWithFrontBoardCabinetAttr> WithFrontBoardCabinetAttr = MakeShareable(new FArmyWHCModeWithFrontBoardCabinetAttr(ContainerRef));
    WithFrontBoardCabinetAttr->Initialize();
    AttrPanels.Emplace(WithFrontBoardCabinetAttr);

    TSharedPtr<FArmyWHCModeCabinetAttr> CabinetAttr = MakeShareable(new FArmyWHCModeCabinetAttr(ContainerRef));
    CabinetAttr->Initialize();
    AttrPanels.Emplace(CabinetAttr);

    TSharedPtr<FArmyWHCModeDoorAttr> DoorAttr = MakeShareable(new FArmyWHCModeDoorAttr(ContainerRef));
    DoorAttr->Initialize();
    AttrPanels.Emplace(DoorAttr);

    TSharedPtr<FArmyWHCModeDrawerAttr> DrawerAttr = MakeShareable(new FArmyWHCModeDrawerAttr(ContainerRef));
    DrawerAttr->Initialize();
    AttrPanels.Emplace(DrawerAttr);

    TSharedPtr<FArmyWHCModePlatformAttr> PlatformAttr = MakeShareable(new FArmyWHCModePlatformAttr(ContainerRef));
    PlatformAttr->Initialize();
    AttrPanels.Emplace(PlatformAttr);

    TSharedPtr<FArmyWHCModeSideBoardAttr> SideBoardAttr = MakeShareable(new FArmyWHCModeSideBoardAttr(ContainerRef));
    SideBoardAttr->Initialize();
    AttrPanels.Emplace(SideBoardAttr);

    TSharedPtr<FArmyWHCModeToeAttr> ToeAttr = MakeShareable(new FArmyWHCModeToeAttr(ContainerRef));
    ToeAttr->Initialize();
    AttrPanels.Emplace(ToeAttr);

    TSharedPtr<FArmyWHCModeTopBlockerAttr> TopBlockerAttr = MakeShareable(new FArmyWHCModeTopBlockerAttr(ContainerRef));
    TopBlockerAttr->Initialize();
    AttrPanels.Emplace(TopBlockerAttr);

    TSharedPtr<FArmyWHCModeAccAttr> AccAttr = MakeShareable(new FArmyWHCModeAccAttr(ContainerRef));
    AccAttr->Initialize();
    AttrPanels.Emplace(AccAttr);

    TSharedPtr<FArmyWHCModeEmbbedElecDevAttr> EmbbedElecDevAttr = MakeShareable(new FArmyWHCModeEmbbedElecDevAttr(ContainerRef));
    EmbbedElecDevAttr->Initialize();
    AttrPanels.Emplace(EmbbedElecDevAttr);

    TSharedPtr<FArmyWHCModeWardrobeAttr> WardrobeAttr = MakeShareable(new FArmyWHCModeWardrobeAttr(ContainerRef));
    WardrobeAttr->Initialize();
    AttrPanels.Emplace(WardrobeAttr);

    TSharedPtr<FArmyWHCModeCoverDoorWardrobeAttr> CoverDoorWardrobeAttr = MakeShareable(new FArmyWHCModeCoverDoorWardrobeAttr(ContainerRef));
    CoverDoorWardrobeAttr->Initialize();
    AttrPanels.Emplace(CoverDoorWardrobeAttr);

    TSharedPtr<FArmyWHCModeOtherCabAttr> OtherCabAttr = MakeShareable(new FArmyWHCModeOtherCabAttr(ContainerRef));
    OtherCabAttr->Initialize();
    AttrPanels.Emplace(OtherCabAttr);

    TSharedPtr<FArmyWHCModeBathroomCabAttr> BathroomCabAttr = MakeShareable(new FArmyWHCModeBathroomCabAttr(ContainerRef));
    BathroomCabAttr->Initialize();
    AttrPanels.Emplace(BathroomCabAttr);
}

void SArmyWHCModeAttrPanelContainer::BeginMode()
{
    CloseAttrPanel();
}

void SArmyWHCModeAttrPanelContainer::EndMode()
{
    CloseReplacingPanel();
    ShowInteractiveMessage(false);
    LastWHCItemSelected = nullptr;
}

void SArmyWHCModeAttrPanelContainer::Callback_WHCItemSelected(AActor *InActor)
{
    Callback_WHCItemDeleted();
    if (InActor == nullptr)
    {
        FWHCModeGlobalData::SelectionState = 0;
        return;
    }

    for (auto & AttrPanel : AttrPanels)
    {
        if (AttrPanel->TryDisplayAttrPanel(InActor))
        {
            CurrentAttrPanel = AttrPanel;
            FWHCModeGlobalData::SelectionState = 1;
            break;
        }
    }
}

void SArmyWHCModeAttrPanelContainer::Callback_WHCItemDeleted()
{
    CloseAttrPanel();
    CloseReplacingPanel();
    ShowInteractiveMessage(false);
    LastWHCItemSelected = nullptr;
}

void SArmyWHCModeAttrPanelContainer::SetAttrPanel(TSharedRef<SWidget> InPanel)
{
    ContentSlot->SetContent(InPanel);
}

void SArmyWHCModeAttrPanelContainer::CloseAttrPanel()
{
    ContentSlot->ClearContent();
    CurrentAttrPanel.Reset();
}

void SArmyWHCModeAttrPanelContainer::SetReplacingPanel(TSharedRef<SWidget> InPanel)
{
    ReplacingSlot->operator[] (InPanel);
}

FReply SArmyWHCModeAttrPanelContainer::CloseReplacingPanel()
{
    ReplacingSlot->DetachWidget();
    ListPanel->SetCurrentState(FString());
    return FReply::Handled();
}

bool SArmyWHCModeAttrPanelContainer::IsClickedSameReplacingItem(int32 InIndex) const
{
    return InIndex == ListPanel->GetSelectedItemIndex();
}

void SArmyWHCModeAttrPanelContainer::Callback_LoadMoreReplacingData()
{
    if (CurrentAttrPanel.IsValid())
        CurrentAttrPanel.Pin()->LoadMoreReplacingData();
}

void SArmyWHCModeAttrPanelContainer::ShowInteractiveMessage(bool bShow)
{
    GGI->Window->ShowInteractiveMessage(bShow);
}

void SArmyWHCModeAttrPanelContainer::NotifyAttrPanelNeedRebuild()
{
    if (CurrentAttrPanel.IsValid())
    {
        TSharedPtr<SWidget> Widget = CurrentAttrPanel.Pin()->RebuildAttrPanel();
        if (Widget.IsValid())
            SetAttrPanel(Widget.ToSharedRef());
    }
}

void SArmyWHCModeAttrPanelContainer::ClearReplacingList()
{
    ReplacingInfo.TotalDataPage = 0;
    ReplacingInfo.ReplacingIds.Empty();
    ListPanel->ClearItems();
}

bool SArmyWHCModeAttrPanelContainer::CheckIsValidReplacingIndex(int32 InIndex) const
{
    return InIndex >= 0 && InIndex < ReplacingInfo.ReplacingIds.Num();
}

int32 SArmyWHCModeAttrPanelContainer::GetPageIndexToLoad() const
{
    // 获取当前要请求的页数
    return ReplacingInfo.ReplacingIds.Num() / 15 + 1; 
} 

bool SArmyWHCModeAttrPanelContainer::CheckNoMoreData() const
{
    // 如果超过了总页数，则不再请求
    return ReplacingInfo.TotalDataPage != 0 && (ReplacingInfo.ReplacingIds.Num() % 15 != 0);
}

SContentItem* SArmyWHCModeAttrPanelContainer::GetListItem(int32 Index) const
{
    const TArray<TSharedPtr<SContentItem>> & Items = ListPanel->GetItems();
    if (Items.IsValidIndex(Index))
        return const_cast<SContentItem*>(Items[Index].Get());
    else
        return nullptr;
}

SContentItem* SArmyWHCModeAttrPanelContainer::GetListItemMatchingType(int32 InId, EWHCModeAttrType InMatchingType) const
{
    const TArray<TSharedPtr<SContentItem>> & Items = ListPanel->GetItems();
    for (const auto & Item : Items)
    {
        TSharedPtr<FWHCModeAttrTypeInfo> AttrTypeInfo = Item->GetMetaData<FWHCModeAttrTypeInfo>();
        if (AttrTypeInfo.IsValid() && AttrTypeInfo->Id == InId && AttrTypeInfo->Type == InMatchingType)
            return Item.Get();
    }
    return nullptr;
}

void SArmyWHCModeAttrPanelContainer::SetCurrentState(const FString &InState)
{
    ListPanel->SetCurrentState(InState);
}

const FString& SArmyWHCModeAttrPanelContainer::GetCurrentState() const
{
    return ListPanel->GetCurrentState();
}

bool SArmyWHCModeAttrPanelContainer::IsCurrentState(const FString &InTestState) const
{
    return ListPanel->GetCurrentState() == InTestState;
} 

void SArmyWHCModeAttrPanelContainer::RunHttpRequest(const FString &InUrl, TBaseDelegate<void, const TSharedPtr<FJsonObject>&> &&ResponseCallback, TBaseDelegate<void, EHttpErrorReason> &&ErrorCallback)
{
    // if (CurrentAttrPanel.IsValid())
    // {
    //     CurrentAttrPanel.Pin()->RunHttpRequest(InUrl, 
    //         Forward<TBaseDelegate<void, const TSharedPtr<FJsonObject>&>>(ResponseCallback),
    //         Forward<TBaseDelegate<void, EHttpErrorReason>>(ErrorCallback));
    // }

    TSharedRef<IHttpRequest> HttpRequest = FHttpMgr::Get()->CreateHttpRequest(InUrl, TEXT("GET"));
	HttpRequest->SetHeader("Content-Type", "application/json; charset=utf-8");
    HttpRequest->SetHeader("xloginid", FArmyHttpModule::Get().GetLoginId());
    HttpRequest->SetHeader("xtoken", FArmyHttpModule::Get().GetToken());
    HttpRequest->OnProcessRequestComplete().BindLambda(
        [Response = ResponseCallback, Error = ErrorCallback](FHttpRequestPtr RequestPtr, FHttpResponsePtr ResponsePtr, bool bSucceeded)
        {
            const TSharedRef<FHttpMgr> &HttpMgr = FHttpMgr::Get();
            if (!HttpMgr->CheckHttpResponseValid(ResponsePtr, bSucceeded))
            {
                Error.ExecuteIfBound(EHttpErrorReason::EReason_HttpCodeError);
				return;
            }
			FString ErrorMsg;
			TSharedPtr<FJsonObject> ResponseData = HttpMgr->GetContentAsJsonObject(ResponsePtr, &ErrorMsg);
			if (ResponseData.IsValid())
				Response.ExecuteIfBound(ResponseData);
            else
                Error.ExecuteIfBound(EHttpErrorReason::EReason_NoValidData);
        }
    );
    HttpRequest->ProcessRequest();
}

void SArmyWHCModeAttrPanelContainer::RunHttpRequest(const FString &InUrl, const TBaseDelegate<void, const TSharedPtr<FJsonObject>&> &ResponseCallback, const TBaseDelegate<void, EHttpErrorReason> &ErrorCallback)
{
    RunHttpRequest(InUrl, 
        MoveTemp(const_cast<TBaseDelegate<void, const TSharedPtr<FJsonObject>&> &>(ResponseCallback)), 
        MoveTemp(const_cast<TBaseDelegate<void, EHttpErrorReason> &>(ErrorCallback)));
}

void SArmyWHCModeAttrPanelContainer::SetReplacingTotalPage(int32 InTotalPage)
{
    ReplacingInfo.TotalDataPage = InTotalPage;
}

void SArmyWHCModeAttrPanelContainer::AddReplacingId(int32 InId)
{
    ReplacingInfo.ReplacingIds.Emplace(InId);
}

const TArray<int32>& SArmyWHCModeAttrPanelContainer::GetReplacingIds() const
{
    return ReplacingInfo.ReplacingIds;
}