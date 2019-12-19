#include "UIMixin/SArmyWHCCabDimensionUI.h"
#include "WHCMode/SArmyWHCModeAttrPanel.h"
#include "WHCMode/XRShapeFrameActor.h"
#include "WHCMode/XRShapeTableActor.h"
#include "WHCMode/XRWHCabinet.h"
#include "WHCMode/XRWHCModeCabinetOperation.h"
#include "WHCMode/XRWHCabinetAutoMgr.h"

#include "ArmyStyle.h"
#include "Actor/XRActorConstant.h"
#include "Game/XRGameInstance.h"
#include "SArmyEditableNumberBox.h"

#include "SCTCabinetShape.h"
#include "SCTSlidingDrawerShape.h"
#include "SCTDoorGroup.h"
#include "SCTAttribute.h"
#include "SCTAnimation.h"
#include "Actor/SCTShapeActor.h"
#include "UtilityTools/SCTShapeUtilityTools.h"

class SArmyCabDimensionComboBox : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyCabDimensionComboBox)
        {}
        SLATE_ARGUMENT(TArray<TSharedPtr<FString>>, OptionSource)
        SLATE_ARGUMENT(FString, Value)
        SLATE_EVENT(FStringDelegate, OnValueChanged)
    SLATE_END_ARGS()

    void Construct(const FArguments &InArgs)
    {
        OptionsSource = InArgs._OptionSource;
        OnValueChanged = InArgs._OnValueChanged;

        ChildSlot
        [
            SAssignNew(Combo, SComboBox<TSharedPtr<FString>>)
            .OptionsSource(&OptionsSource)
            .ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox"))
            .ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.PropertyCombobox"))
            .OnGenerateWidget(this, &SArmyCabDimensionComboBox::OnGenerateWidget)
            .OnSelectionChanged(this, &SArmyCabDimensionComboBox::OnSelectionChanged)
            .MaxListHeight(350)
            .ContentPadding(FMargin(0))
            .Content()
            [
                SNew(SBox)
                .HeightOverride(24.0f)
                .VAlign(VAlign_Center)
                [
                    SAssignNew(Value, SEditableText)
                    .Text(FText::FromString(InArgs._Value))
                    .Font(FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12").Font)
                    .ColorAndOpacity(FLinearColor::White)
                    .SelectAllTextWhenFocused(true)
                    .OnTextCommitted_Lambda(
                        [this](const FText &InText, ETextCommit::Type InType)
                        {
                            if (InType == ETextCommit::OnEnter)
                                OnValueChanged.ExecuteIfBound(InText.ToString());
                        }
                    )
                    .OnIsTypedCharValid_Lambda(
                        [](const TCHAR InChar) -> bool {
                            return (InChar >= 48 && InChar <= 57);
                        }
                    )
                ]
            ]
        ];
    }

    TSharedRef<SWidget> OnGenerateWidget(TSharedPtr<FString> InItem)
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

    void OnSelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
    {
        if (SelectInfo == ESelectInfo::OnMouseClick)
        {
            // FText NewText = FText::FromString(*NewSelection.Get());
            // Value->SetText(NewText);
            OnValueChanged.ExecuteIfBound(*NewSelection.Get());
        }
    }

    void SetValue(const FString &InText)
    {
        Value->SetText(FText::FromString(InText));
    }

    void SetOptions(const TArray<TSharedPtr<FString>> &InOptionSource)
    {
        OptionsSource.Empty();
        for (const auto &Option : InOptionSource)
            OptionsSource.Emplace(Option);
        Combo->RefreshOptions();
    }

    TSharedPtr<SEditableText> Value;
    TSharedPtr<SComboBox<TSharedPtr<FString>>> Combo;
    TArray<TSharedPtr<FString>> OptionsSource;
    FStringDelegate OnValueChanged;
};

int32 GetShapeClassify(int32 InShapeCategory)
{
    ECabinetType Type = (ECabinetType)InShapeCategory;
    switch (Type)
    {
        case ECabinetType::EType_HangCab:
        case ECabinetType::EType_OnGroundCab:
        case ECabinetType::EType_TallCab:
        case ECabinetType::EType_OnCabCab:
            return (int32)ECabinetClassify::EClassify_Cabinet;
        case ECabinetType::EType_CoverDoorWardrobe:
        case ECabinetType::EType_SlidingDoorWardrobe:
        case ECabinetType::EType_TopCab:
            return (int32)ECabinetClassify::EClassify_Wardrobe;
        case ECabinetType::EType_Other:
            return (int32)ECabinetClassify::EClassify_Other;
        case ECabinetType::EType_BathroomOnGroundCab:
        case ECabinetType::EType_BathroomHangCab:
        case ECabinetType::EType_BathroomStorageCab:
            return (int32)ECabinetClassify::EClassify_BathroomCab;
        default:
            return -1;
    }
}

TSharedPtr<SWidget> FArmyWHCCabWidthUI::MakeWidget()
{
    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    check(ShapeFrame != nullptr);
    check(ShapeFrame->ShapeInRoomRef != nullptr);
    FCabinetShape * CabShape = static_cast<FCabinetShape *>(ShapeFrame->ShapeInRoomRef->Shape.Get());

    FString DefaultSize(TEXT("0"));
    TArray<TSharedPtr<FString>> DefaultOptions { MakeShareable(new FString(TEXT("0"))) };

    if (!CabWidthCombo.IsValid())
    {
        SAssignNew(CabWidthCombo, SArmyCabDimensionComboBox)
        .Value(DefaultSize)
        .OptionSource(DefaultOptions)
        .OnValueChanged_Lambda(
            [this](const FString &InStr) {
                ModifyWidth(InStr);
            }
        );
        CabWidthCombo->Value->SetText(TAttribute<FText>::Create(
            [this]() -> FText {
                return GetWidth();
            }
        ));
    }

    if (!CabWidthEditContainer.IsValid())
    {
        SAssignNew(CabWidthEditContainer, SBox)
        .HeightOverride(24.0f)
        [
            SNew(SArmyEditableNumberBox)
            .Text_Lambda(
                [this]() -> FText {
                    return GetWidth();
                }
            )
            .Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox.Gray"))
            .SelectAllTextWhenFocused(true)
            .Padding(FMargin(8.0f, 0.0f, 0.0f, 0.0f))
            .ForegroundColor(FLinearColor::White)
            .OnTextCommitted_Lambda(
                [this](const FText &InText, ETextCommit::Type InType) {
                    if (InType == ETextCommit::OnEnter)
                    {
                        ModifyWidth(InText.ToString());
                    }
                }
            )
        ];
    }

    TArray<float> StandardWidths;
    if (ShapeFrame->ShapeInRoomRef->IsParamType< TArray<float> >(PARAM_NAME_CAB_WIDTH_INTERVALS))
    {
        StandardWidths = *ShapeFrame->ShapeInRoomRef->GetParam<TArray<float>>(PARAM_NAME_CAB_WIDTH_INTERVALS);
    }
    else
    {
        int32 Classify = GetShapeClassify(CabShape->GetShapeCategory());
        if (Classify >= 0)
        {
            const FWHCModeGlobalData::TCabinetInfoArr *CabinetInfoArrPtr = FWHCModeGlobalData::CabinetInfoArrCollection.Find(Classify);
            if (CabinetInfoArrPtr != nullptr)
            {
                int32 ShapeId = CabShape->GetShapeId();
                const FWHCModeGlobalData::TCabinetInfoArr &CabinetInfoArr = *CabinetInfoArrPtr;
                const FCabinetInfo *CabInfoPtr = CabinetInfoArr.FindByPredicate([ShapeId](const FCabinetInfo &Data) -> bool {
                    return Data.Id == ShapeId;
                });
                if (CabInfoPtr != nullptr)
                {
                    StandardWidths = CabInfoPtr->WidthInfo.Intervals;
                }
            }
        }
    }
    

    if (StandardWidths.Num() == 0)
        StandardWidths = CabShape->GetStandardWidths();
    // 有跳变值，显示的是编辑框和下拉框的混合控件
    if (StandardWidths.Num() > 0)
    {
        TArray<TSharedPtr<FString>> Options;
        for (const auto &Interval : StandardWidths)
            Options.Emplace(MakeShareable(new FString(FString::Printf(TEXT("%d"), FMath::RoundToInt(Interval)))));
        CabWidthCombo->SetOptions(Options);
        return CabWidthCombo;
    }
    // 无跳变值，显示的是普通的编辑框
    else
    {
        return CabWidthEditContainer;
    }
}

void FArmyWHCCabWidthUI::ModifyWidth(const FString &InStr)
{
    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    if (ShapeFrame == nullptr)
        return;

    check(ShapeFrame->ShapeInRoomRef != nullptr);

    FArmyWHCabinetMgr *CabMgr = FArmyWHCabinetMgr::Get();

    float NewWidth = FCString::Atof(*InStr);

    FCabinetShape * CabShape = static_cast<FCabinetShape *>(ShapeFrame->ShapeInRoomRef->Shape.Get());
    TSharedPtr<FNumberRangeAttri> RangeWidthAttri = StaticCastSharedPtr<FNumberRangeAttri>(CabShape->GetShapeWidthAttri());
    if (NewWidth < RangeWidthAttri->GetMinValue() || NewWidth > RangeWidthAttri->GetMaxValue())
    {
        GGI->Window->ShowMessage(MT_Warning, TEXT("尺寸超出限制 请重新输入"));
        return;
    }

    // Fix bug #1838
    for (auto & Animation : ShapeFrame->ShapeInRoomRef->Animations)
        Animation->Stop();

    bool bCanSet = CabMgr->AdjustCabinetNewWidth(ShapeFrame->ShapeInRoomRef, NewWidth);
    if (!bCanSet)
    {
        GGI->Window->ShowMessage(MT_Warning, TEXT("尺寸超出限制 请重新输入"));
        return;
    }
    Container->CabinetOperationRef->TryModifySelectedCabinet();
    Container->CabinetOperationRef->UpdateHighlight();
    
    // Fix bug #1838
    ShapeFrame->ShapeInRoomRef->PrepareShapeAnimations();

    // 重新计算距离标尺
    Container->CabinetOperationRef->CalculateShapeDistanceFromWall(ShapeFrame->ShapeInRoomRef);
    // 更新台面/踢脚板/上线条
    FArmyWHCabinetMgr::Get()->OnRefreshShape(CabShape->GetShapeCategory());
    // 更新调整脚
    ShapeFrame->ShapeInRoomRef->DestroyShapeLegs();
    ShapeFrame->ShapeInRoomRef->SpawnShapeLegs();
    // 更新独立台面
    ShapeFrame->ShapeInRoomRef->SpawnPlatform();
    // 检查对齐关系
    TArray<TSharedPtr<FShapeInRoom>> &PlacedShapes = CabMgr->GetPlaceShapes();
    if (FShapeInRoom *AlignedPassive = Container->CabinetOperationRef->GetAlignedPassive(ShapeFrame->ShapeInRoomRef))
    {
        Container->CabinetOperationRef->RemoveAlignedPairByActive(ShapeFrame->ShapeInRoomRef);
        Container->CabinetOperationRef->AddOverlappedPair(ShapeFrame->ShapeInRoomRef, AlignedPassive);
        if (!Container->CabinetOperationRef->CheckAligned(ShapeFrame->ShapeInRoomRef, PlacedShapes, KINDA_SMALL_NUMBER))
            Container->CabinetOperationRef->CheckOverlapped(ShapeFrame->ShapeInRoomRef, PlacedShapes);
    }
    else if (FShapeInRoom *AlignedActive = Container->CabinetOperationRef->GetAlignedActiveShapeByPassive(ShapeFrame->ShapeInRoomRef))
    {
        Container->CabinetOperationRef->RemoveAlignedPair(ShapeFrame->ShapeInRoomRef);
        Container->CabinetOperationRef->AddOverlappedPair(AlignedActive, ShapeFrame->ShapeInRoomRef);
        if (!Container->CabinetOperationRef->CheckAligned(AlignedActive, PlacedShapes, KINDA_SMALL_NUMBER))
            Container->CabinetOperationRef->CheckOverlapped(AlignedActive, PlacedShapes);
    }
    // 更新构件
    ShapeFrame->ShapeInRoomRef->UpdateComponents();
    ShapeFrame->ShapeInRoomRef->MakeImmovable();
    FSCTShapeUtilityTool::SetActorTag(ShapeFrame, XRActorTag::WHCActor);
}

FText FArmyWHCCabWidthUI::GetWidth() const
{
    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    if (ShapeFrame != nullptr)
    {
        check(ShapeFrame->ShapeInRoomRef != nullptr);
        float ShapeWidth = ShapeFrame->ShapeInRoomRef->Shape->GetShapeWidth();
        return FText::FromString(FString::Printf(TEXT("%d"), FMath::RoundToInt(ShapeWidth)));
    }
    else
        return FText::FromString(TEXT("0"));
}

TSharedPtr<SWidget> FArmyWHCCabDepthUI::MakeWidget()
{
    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    check(ShapeFrame != nullptr);
    check(ShapeFrame->ShapeInRoomRef != nullptr);
    FCabinetShape * CabShape = static_cast<FCabinetShape *>(ShapeFrame->ShapeInRoomRef->Shape.Get());

    FString DefaultSize(TEXT("0"));
    TArray<TSharedPtr<FString>> DefaultOptions { MakeShareable(new FString(TEXT("0"))) };

    if (!CabDepthCombo.IsValid())
    {
        SAssignNew(CabDepthCombo, SArmyCabDimensionComboBox)
        .Value(DefaultSize)
        .OptionSource(DefaultOptions)
        .OnValueChanged_Lambda(
            [this](const FString &InStr)
            {
                ModifyDepth(InStr);
            }
        );
        CabDepthCombo->Value->SetText(TAttribute<FText>::Create(
            [this]() -> FText {
                return GetDepth();
            }
        ));
    }

    if (!CabDepthEditContainer.IsValid())
    {
        SAssignNew(CabDepthEditContainer, SBox)
        .HeightOverride(24.0f)
        [
            SNew(SArmyEditableNumberBox)
            .Text_Lambda(
                [this]() -> FText {
                    return GetDepth();
                }
            )
            .Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox.Gray"))
            .SelectAllTextWhenFocused(true)
            .Padding(FMargin(8.0f, 0.0f, 0.0f, 0.0f))
            .ForegroundColor(FLinearColor::White)
            .OnTextCommitted_Lambda(
                [this](const FText &InText, ETextCommit::Type InType)
                {
                    if (InType == ETextCommit::OnEnter)
                    {
                        ModifyDepth(InText.ToString());
                    }
                }
            )
        ];
    }

    TArray<float> StandardDepths;
    if (ShapeFrame->ShapeInRoomRef->IsParamType< TArray<float> >(PARAM_NAME_CAB_DEPTH_INTERVALS))
    {
        StandardDepths = *ShapeFrame->ShapeInRoomRef->GetParam<TArray<float>>(PARAM_NAME_CAB_DEPTH_INTERVALS);
    }
    else
    {
        int32 Classify = GetShapeClassify(CabShape->GetShapeCategory());
        if (Classify >= 0)
        {
            const FWHCModeGlobalData::TCabinetInfoArr *CabinetInfoArrPtr = FWHCModeGlobalData::CabinetInfoArrCollection.Find(Classify);
            if (CabinetInfoArrPtr != nullptr)
            {
                int32 ShapeId = CabShape->GetShapeId();
                const FWHCModeGlobalData::TCabinetInfoArr &CabinetInfoArr = *CabinetInfoArrPtr;
                const FCabinetInfo *CabInfoPtr = CabinetInfoArr.FindByPredicate([ShapeId](const FCabinetInfo &Data) -> bool {
                    return Data.Id == ShapeId;
                });
                if (CabInfoPtr != nullptr)
                {
                    StandardDepths = CabInfoPtr->DepthInfo.Intervals;
                }
            }
        }
    }

    if (StandardDepths.Num() == 0)
        StandardDepths = CabShape->GetStandardDepths();
    // 有跳变值，显示的是编辑框和下拉框的混合控件
    if (StandardDepths.Num() > 0)
    {
        TArray<TSharedPtr<FString>> Options;
        for (const auto &Interval : StandardDepths)
            Options.Emplace(MakeShareable(new FString(FString::Printf(TEXT("%d"), FMath::RoundToInt(Interval)))));
        CabDepthCombo->SetOptions(Options);
        return CabDepthCombo;
    }
    // 无跳变值，显示的是普通的编辑框
    else
    {
        return CabDepthEditContainer;
    }
}

void FArmyWHCCabDepthUI::ModifyDepth(const FString &InStr)
{
    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    if (ShapeFrame == nullptr)
        return;

    check(ShapeFrame->ShapeInRoomRef != nullptr);

    float NewDepth = FCString::Atof(*InStr);

    FCabinetShape * CabShape = static_cast<FCabinetShape*>(ShapeFrame->ShapeInRoomRef->Shape.Get());
    TSharedPtr<FNumberRangeAttri> RangeDepthAttri = StaticCastSharedPtr<FNumberRangeAttri>(CabShape->GetShapeDepthAttri());
    if (NewDepth < RangeDepthAttri->GetMinValue() || NewDepth > RangeDepthAttri->GetMaxValue())
    {
        GGI->Window->ShowMessage(MT_Warning, TEXT("尺寸超出限制 请重新输入"));
        return;
    }

    for (auto & Animation : ShapeFrame->ShapeInRoomRef->Animations)
        Animation->Stop();

    if (CabShape->SetShapeDepth(NewDepth))
    {
        float OldWidth = CabShape->GetShapeWidth();
        float OldHeight = CabShape->GetShapeHeight();
        ShapeFrame->RefreshFrame(OldWidth * 0.1f, NewDepth * 0.1f, OldHeight * 0.1f);

        Container->CabinetOperationRef->TryModifySelectedCabinet();
        Container->CabinetOperationRef->UpdateHighlight();

        ShapeFrame->ShapeInRoomRef->PrepareShapeAnimations();

        // 重新计算距离标尺
        Container->CabinetOperationRef->CalculateShapeDistanceFromWall(ShapeFrame->ShapeInRoomRef);
        // 更新台面/踢脚板/上线条
        FArmyWHCabinetMgr::Get()->OnRefreshShape(CabShape->GetShapeCategory());
        // 更新单独生成的台面
        ShapeFrame->ShapeInRoomRef->SpawnPlatform();
        // 更新调整脚
        ShapeFrame->ShapeInRoomRef->DestroyShapeLegs();
        ShapeFrame->ShapeInRoomRef->SpawnShapeLegs();
        // 更新独立台面
        ShapeFrame->ShapeInRoomRef->SpawnPlatform();
        // 更新构件
        ShapeFrame->ShapeInRoomRef->UpdateComponents();
        ShapeFrame->ShapeInRoomRef->MakeImmovable();
        FSCTShapeUtilityTool::SetActorTag(ShapeFrame, XRActorTag::WHCActor);
    }
}

FText FArmyWHCCabDepthUI::GetDepth() const
{
    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    if (ShapeFrame != nullptr)
    {
        check(ShapeFrame->ShapeInRoomRef != nullptr);
        float ShapeDepth = ShapeFrame->ShapeInRoomRef->Shape->GetShapeDepth();
        return FText::FromString(FString::Printf(TEXT("%d"), FMath::RoundToInt(ShapeDepth)));
    }
    else
        return FText::FromString(TEXT("0"));
}

TSharedPtr<SWidget> FArmyWHCCabHeightUI::MakeWidget()
{
    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    check(ShapeFrame != nullptr);
    check(ShapeFrame->ShapeInRoomRef != nullptr);
    FCabinetShape * CabShape = static_cast<FCabinetShape *>(ShapeFrame->ShapeInRoomRef->Shape.Get());

    FString DefaultSize(TEXT("0"));
    TArray<TSharedPtr<FString>> DefaultOptions { MakeShareable(new FString(TEXT("0"))) };

    if (!CabHeightCombo.IsValid())
    {
        SAssignNew(CabHeightCombo, SArmyCabDimensionComboBox)
        .Value(DefaultSize)
        .OptionSource(DefaultOptions)
        .OnValueChanged_Lambda(
            [this](const FString &InStr)
            {
                ModifyHeight(InStr);
            }
        );
        CabHeightCombo->Value->SetText(TAttribute<FText>::Create(
            [this]() -> FText {
                return GetHeight();
            }
        ));
    }

    if (!CabHeightEditContainer.IsValid())
    {
        SAssignNew(CabHeightEditContainer, SBox)
        .HeightOverride(24.0f)
        [
            SNew(SArmyEditableNumberBox)
            .Text_Lambda(
                [this]() -> FText {
                    return GetHeight();
                }
            )
            .Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox.Gray"))
            .SelectAllTextWhenFocused(true)
            .Padding(FMargin(8.0f, 0.0f, 0.0f, 0.0f))
            .ForegroundColor(FLinearColor::White)
            .OnTextCommitted_Lambda(
                [this](const FText &InText, ETextCommit::Type InType)
                {
                    if (InType == ETextCommit::OnEnter)
                    {
                        ModifyHeight(InText.ToString());
                    }
                }
            )
        ];
    }

    TArray<float> StandardHeights;
    if (ShapeFrame->ShapeInRoomRef->IsParamType< TArray<float> >(PARAM_NAME_CAB_HEIGHT_INTERVALS))
    {
        StandardHeights = *ShapeFrame->ShapeInRoomRef->GetParam<TArray<float>>(PARAM_NAME_CAB_HEIGHT_INTERVALS);
    }
    else
    {
        int32 Classify = GetShapeClassify(CabShape->GetShapeCategory());
        if (Classify >= 0)
        {
            const FWHCModeGlobalData::TCabinetInfoArr *CabinetInfoArrPtr = FWHCModeGlobalData::CabinetInfoArrCollection.Find(Classify);
            if (CabinetInfoArrPtr != nullptr)
            {
                int32 ShapeId = CabShape->GetShapeId();
                const FWHCModeGlobalData::TCabinetInfoArr &CabinetInfoArr = *CabinetInfoArrPtr;
                const FCabinetInfo *CabInfoPtr = CabinetInfoArr.FindByPredicate([ShapeId](const FCabinetInfo &Data) -> bool {
                    return Data.Id == ShapeId;
                });
                if (CabInfoPtr != nullptr)
                {
                    StandardHeights = CabInfoPtr->HeightInfo.Intervals;
                }
            }
        }
    }

    if (StandardHeights.Num() == 0)
        StandardHeights = CabShape->GetStandardHeights();
    // 有跳变值，显示的是编辑框和下拉框的混合控件
    if (StandardHeights.Num() > 0)
    {
        TArray<TSharedPtr<FString>> Options;
        for (const auto &Interval : StandardHeights)
            Options.Emplace(MakeShareable(new FString(FString::Printf(TEXT("%d"), FMath::RoundToInt(Interval)))));
        CabHeightCombo->SetOptions(Options);
        return CabHeightCombo;
    }
    // 无跳变值，显示的是普通的编辑框
    else
    {
        return CabHeightEditContainer;
    }
}

void FArmyWHCCabHeightUI::ModifyHeight(const FString &InStr)
{
    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    if (ShapeFrame == nullptr)
        return;

    check(ShapeFrame->ShapeInRoomRef != nullptr);

    float NewHeight = FCString::Atof(*InStr);

    FCabinetShape * CabShape = static_cast<FCabinetShape*>(ShapeFrame->ShapeInRoomRef->Shape.Get());
    TSharedPtr<FNumberRangeAttri> RangeHeightAttri = StaticCastSharedPtr<FNumberRangeAttri>(CabShape->GetShapeHeightAttri());
    if (NewHeight < RangeHeightAttri->GetMinValue() || NewHeight > RangeHeightAttri->GetMaxValue())
    {
        GGI->Window->ShowMessage(MT_Warning, TEXT("尺寸超出限制 请重新输入"));
        return;
    }

    for (auto & Animation : ShapeFrame->ShapeInRoomRef->Animations)
        Animation->Stop();

    if (CabShape->SetShapeHeight(NewHeight))
    {
        float OldWidth = CabShape->GetShapeWidth();
        float OldDepth = CabShape->GetShapeDepth();
        ShapeFrame->RefreshFrame(OldWidth * 0.1f, OldDepth * 0.1f, NewHeight * 0.1f);

        Container->CabinetOperationRef->TryModifySelectedCabinet();
        Container->CabinetOperationRef->UpdateHighlight();

        ShapeFrame->ShapeInRoomRef->PrepareShapeAnimations();

        // 重新计算距离标尺
        Container->CabinetOperationRef->CalculateShapeDistanceFromWall(ShapeFrame->ShapeInRoomRef);
        // 更新台面/踢脚板/上线条
        FArmyWHCabinetMgr::Get()->OnRefreshShape(CabShape->GetShapeCategory());
        // 更新单独生成的台面
        ShapeFrame->ShapeInRoomRef->SpawnPlatform();
        // 更新调整脚
        ShapeFrame->ShapeInRoomRef->DestroyShapeLegs();
        ShapeFrame->ShapeInRoomRef->SpawnShapeLegs();
        // 更新独立台面
        ShapeFrame->ShapeInRoomRef->SpawnPlatform();
        // 更新构件
        ShapeFrame->ShapeInRoomRef->UpdateComponents();
        ShapeFrame->ShapeInRoomRef->MakeImmovable();
        FSCTShapeUtilityTool::SetActorTag(ShapeFrame, XRActorTag::WHCActor);
    }
}

FText FArmyWHCCabHeightUI::GetHeight() const
{
    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    if (ShapeFrame != nullptr)
    {
        check(ShapeFrame->ShapeInRoomRef != nullptr);
        float ShapeHeight = ShapeFrame->ShapeInRoomRef->Shape->GetShapeHeight();
        return FText::FromString(FString::Printf(TEXT("%d"), FMath::RoundToInt(ShapeHeight)));
    }
    else
        return FText::FromString(TEXT("0"));
}

TSharedPtr<SWidget> FArmyWHCCabVentilatorWidthUI::MakeWidget()
{
    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    check(ShapeFrame != nullptr);
    check(ShapeFrame->ShapeInRoomRef != nullptr);
    FCabinetShape * CabShape = static_cast<FCabinetShape *>(ShapeFrame->ShapeInRoomRef->Shape.Get());

    FString DefaultSize(TEXT("0"));
    TArray<TSharedPtr<FString>> DefaultOptions { MakeShareable(new FString(TEXT("0"))) };

    if (!VentilatorWidthCombo.IsValid())
    {
        SAssignNew(VentilatorWidthCombo, SArmyCabDimensionComboBox)
        .Value(DefaultSize)
        .OptionSource(DefaultOptions)
        .OnValueChanged_Lambda(
            [this](const FString &InStr)
            {
                ModifyVentilatorWidth(InStr);
            }
        );
        VentilatorWidthCombo->Value->SetText(TAttribute<FText>::Create(
            [this]() -> FText {
                return GetVentilatorWidth();
            }
        ));
    }

    if (!VentilatorWidthContainer.IsValid())
    {
        SAssignNew(VentilatorWidthContainer, SBox)
        .HeightOverride(24.0f)
        [
            SNew(SArmyEditableNumberBox)
            .Text_Lambda(
                [this]() -> FText {
                    return GetVentilatorWidth();
                }
            )
            .Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox.Gray"))
            .SelectAllTextWhenFocused(true)
            .Padding(FMargin(8.0f, 0.0f, 0.0f, 0.0f))
            .ForegroundColor(FLinearColor::White)
            .OnTextCommitted_Lambda(
                [this](const FText &InText, ETextCommit::Type InType)
                {
                    if (InType == ETextCommit::OnEnter)
                    {
                        ModifyVentilatorWidth(InText.ToString());
                    }
                }
            )
        ];
    }

    TArray<float> StandardWidths;
    if (ShapeFrame->ShapeInRoomRef->IsParamType< TArray<float> >(PARAM_NAME_CAB_VENTILATOR_WIDTH_INTERVALS))
        StandardWidths = *(ShapeFrame->ShapeInRoomRef->GetParam<TArray<float>>(PARAM_NAME_CAB_VENTILATOR_WIDTH_INTERVALS));

    // 有跳变值，显示的是编辑框和下拉框的混合控件
    if (StandardWidths.Num() > 0)
    {
        TArray<TSharedPtr<FString>> Options;
        for (const auto &Interval : StandardWidths)
            Options.Emplace(MakeShareable(new FString(FString::Printf(TEXT("%d"), FMath::RoundToInt(Interval)))));
        VentilatorWidthCombo->SetOptions(Options);
        return VentilatorWidthCombo;
    }
    // 无跳变值，显示的是普通的编辑框
    else
    {
        return VentilatorWidthContainer;
    }
}

void FArmyWHCCabVentilatorWidthUI::ModifyVentilatorWidth(const FString &InStr)
{
    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    if (ShapeFrame != nullptr)
    {
        check(ShapeFrame->ShapeInRoomRef != nullptr);
        float Width = FCString::Atof(*InStr);
        int32 Classify = GetShapeClassify(ShapeFrame->ShapeInRoomRef->Shape->GetShapeCategory());
        if (Classify >= 0)
        {
            const FWHCModeGlobalData::TCabinetInfoArr *CabinetInfoArrPtr = FWHCModeGlobalData::CabinetInfoArrCollection.Find(Classify);
            const FCabinetInfo *CabInfo = CabinetInfoArrPtr->FindByPredicate(FCabinetInfoFunctor(ShapeFrame->ShapeInRoomRef->Shape->GetShapeId()));
            if (CabInfo != nullptr)
            {
                const FDimInfo *DimInfo = CabInfo->OtherDimInfo.Find("VentilatorWidth");
                if (DimInfo == nullptr)
                    return;
                if (Width < DimInfo->Min || Width > DimInfo->Max)
                {
                    GGI->Window->ShowMessage(MT_Warning, TEXT("尺寸小于排风管道安全值 请重新输入"));
                    return;
                }
                TPair<TTuple<float,float,float>,TArray<float>> WidthInfo, DepthInfo, HeightInfo;
                if (FSCTShapeUtilityTool::GetSmokeCabinetSpaceRange(ShapeFrame->ShapeInRoomRef->Shape.Get(), 
                    WidthInfo, DepthInfo, HeightInfo))
                {
                    FSCTShapeUtilityTool::ResizeSmokeCabinetSpaceRangeWithExpectValue(ShapeFrame->ShapeInRoomRef->Shape.Get(), 
                        Width, DepthInfo.Key.Get<2>(), HeightInfo.Key.Get<2>());
                }
            }
        }
    }
}

FText FArmyWHCCabVentilatorWidthUI::GetVentilatorWidth() const
{
    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    if (ShapeFrame != nullptr)
    {
        check(ShapeFrame->ShapeInRoomRef != nullptr);
        TPair<TTuple<float,float,float>,TArray<float>> WidthInfo, DepthInfo, HeightInfo;
        if (FSCTShapeUtilityTool::GetSmokeCabinetSpaceRange(ShapeFrame->ShapeInRoomRef->Shape.Get(), 
            WidthInfo, DepthInfo, HeightInfo))
        {
            float Current = WidthInfo.Key.Get<2>();
            return FText::FromString(FString::Printf(TEXT("%d"), FMath::RoundToInt(Current)));
        }
    }
    
    return FText::FromString(TEXT("0"));
}

TSharedPtr<SWidget> FArmyWHCCabWidthStaticUIBase::MakeWidget()
{
    return SNew(SBox)
        .HeightOverride(24.0f)
        [
            SNew(SBorder)
            .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF38393D"))
            .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
            .Padding(FMargin(8.0f, 0.0f, 0.0f, 0.0f))
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text_Lambda(
                    [this]() -> FText {
                        if (IsValidSelection())
                        {
                            return FText::FromString(FString::Printf(TEXT("%d"), FMath::RoundToInt(GetWidthValue())));
                        }
                        else
                            return FText::FromString(TEXT("0"));
                    }
                )
                .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
                .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FF969799"))
            ]
        ];
}

bool FArmyWHCCabWidthStaticUI::IsValidSelection() const
{
    return Container->IsWHCItemSelected<ASCTShapeActor>();
}

float FArmyWHCCabWidthStaticUI::GetWidthValue()
{
    ASCTShapeActor *ShapeActor = static_cast<ASCTShapeActor*>(Container->LastWHCItemSelected);
    return ShapeActor->GetShape()->GetShapeWidth();
}

bool FArmyWHCAccWidthStaticUI::IsValidSelection() const
{
    return Container->IsWHCItemSelected<AXRElecDeviceActor>();
}

float FArmyWHCAccWidthStaticUI::GetWidthValue()
{
    AXRElecDeviceActor *ElecDevActor = Cast<AXRElecDeviceActor>(Container->LastWHCItemSelected);
    if (ElecDevActor->ShapeAccRef != nullptr)
    {
        const TSharedPtr<FCabinetAccInfo> &CabAccInfo = ElecDevActor->ShapeAccRef->CabAccInfo;
        return CabAccInfo->WidthInfo.Current;
    }
    else
        return 0.0f;
}

bool FArmyWHCDrawerWidthStaticUI::IsValidSelection() const
{
    return Container->IsWHCItemSelected<ASCTShapeActor>();
}

float FArmyWHCDrawerWidthStaticUI::GetWidthValue()
{
    ASCTShapeActor *ShapeActor = static_cast<ASCTShapeActor*>(Container->LastWHCItemSelected);
    FSlidingDrawerShape * SlidingDrawer = static_cast<FSlidingDrawerShape*>(ShapeActor->GetShape());
    const TArray<TSharedPtr<FDrawerDoorShape>> & DrawerDoors = SlidingDrawer->GetDrawerDoorShapes();
    check(DrawerDoors.Num() > 0);
    const TSharedPtr<FDrawerDoorShape> & DrawerDoor = DrawerDoors[0];
    return DrawerDoor->GetDrawDoorSheet()->GetShapeWidth();
}

TSharedPtr<SWidget> FArmyWHCCabDepthStaticUIBase::MakeWidget()
{
    return SNew(SBox)
        .HeightOverride(24.0f)
        [
            SNew(SBorder)
            .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF38393D"))
            .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
            .Padding(FMargin(8.0f, 0.0f, 0.0f, 0.0f))
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text_Lambda(
                    [this]() -> FText {
                        if (IsValidSelection())
                        {
                            return FText::FromString(FString::Printf(TEXT("%d"), FMath::RoundToInt(GetDepthValue())));
                        }
                        else
                            return FText::FromString(TEXT("0"));
                    }
                )
                .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
                .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FF969799"))
            ]
        ];
}

bool FArmyWHCCabDepthStaticUI::IsValidSelection() const
{
    return Container->IsWHCItemSelected<ASCTShapeActor>();
}

float FArmyWHCCabDepthStaticUI::GetDepthValue()
{
    ASCTShapeActor *ShapeActor = static_cast<ASCTShapeActor*>(Container->LastWHCItemSelected);
    return ShapeActor->GetShape()->GetShapeDepth();
}

bool FArmyWHCAccDepthStaticUI::IsValidSelection() const
{
    return Container->IsWHCItemSelected<AXRElecDeviceActor>();
}

float FArmyWHCAccDepthStaticUI::GetDepthValue()
{
    AXRElecDeviceActor *ElecDevActor = Cast<AXRElecDeviceActor>(Container->LastWHCItemSelected);
    if (ElecDevActor->ShapeAccRef != nullptr)
    {
        const TSharedPtr<FCabinetAccInfo> &CabAccInfo = ElecDevActor->ShapeAccRef->CabAccInfo;
        return CabAccInfo->DepthInfo.Current;
    }
    else
        return 0.0f;
}

bool FArmyWHCDrawerDepthStaticUI::IsValidSelection() const
{
    return Container->IsWHCItemSelected<ASCTShapeActor>();
}

float FArmyWHCDrawerDepthStaticUI::GetDepthValue()
{
    ASCTShapeActor *ShapeActor = static_cast<ASCTShapeActor*>(Container->LastWHCItemSelected);
    FSlidingDrawerShape * SlidingDrawer = static_cast<FSlidingDrawerShape*>(ShapeActor->GetShape());
    const TArray<TSharedPtr<FDrawerDoorShape>> & DrawerDoors = SlidingDrawer->GetDrawerDoorShapes();
    check(DrawerDoors.Num() > 0);
    const TSharedPtr<FDrawerDoorShape> & DrawerDoor = DrawerDoors[0];
    return DrawerDoor->GetDrawDoorSheet()->GetShapeDepth();   
}

TSharedPtr<SWidget> FArmyWHCCabHeightStaticUIBase::MakeWidget()
{
    return SNew(SBox)
        .HeightOverride(24.0f)
        [
            SNew(SBorder)
            .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF38393D"))
            .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
            .Padding(FMargin(8.0f, 0.0f, 0.0f, 0.0f))
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text_Lambda(
                    [this]() -> FText {
                        if (IsValidSelection())
                        {
                            return FText::FromString(FString::Printf(TEXT("%d"), FMath::RoundToInt(GetHeightValue())));
                        }
                        else
                            return FText::FromString(TEXT("0"));
                    }
                )
                .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
                .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FF969799"))
            ]
        ];
}

bool FArmyWHCCabHeightStaticUI::IsValidSelection() const
{
    return Container->IsWHCItemSelected<ASCTShapeActor>();
}

float FArmyWHCCabHeightStaticUI::GetHeightValue()
{
    ASCTShapeActor *ShapeActor = static_cast<ASCTShapeActor*>(Container->LastWHCItemSelected);
    return ShapeActor->GetShape()->GetShapeHeight();
}

bool FArmyWHCAccHeightStaticUI::IsValidSelection() const
{
    return Container->IsWHCItemSelected<AXRElecDeviceActor>();
}

float FArmyWHCAccHeightStaticUI::GetHeightValue()
{
    AXRElecDeviceActor *ElecDevActor = Cast<AXRElecDeviceActor>(Container->LastWHCItemSelected);
    if (ElecDevActor->ShapeAccRef != nullptr)
    {
        const TSharedPtr<FCabinetAccInfo> &CabAccInfo = ElecDevActor->ShapeAccRef->CabAccInfo;
        return CabAccInfo->HeightInfo.Current;
    }
    else
        return 0.0f;   
}

bool FArmyWHCDrawerHeightStaticUI::IsValidSelection() const
{
    return Container->IsWHCItemSelected<ASCTShapeActor>();
}

float FArmyWHCDrawerHeightStaticUI::GetHeightValue()
{
    ASCTShapeActor *ShapeActor = static_cast<ASCTShapeActor*>(Container->LastWHCItemSelected);
    FSlidingDrawerShape * SlidingDrawer = static_cast<FSlidingDrawerShape*>(ShapeActor->GetShape());
    const TArray<TSharedPtr<FDrawerDoorShape>> & DrawerDoors = SlidingDrawer->GetDrawerDoorShapes();
    check(DrawerDoors.Num() > 0);
    const TSharedPtr<FDrawerDoorShape> & DrawerDoor = DrawerDoors[0];
    return DrawerDoor->GetDrawDoorSheet()->GetShapeHeight();
}