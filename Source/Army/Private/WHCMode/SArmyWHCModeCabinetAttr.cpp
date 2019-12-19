#include "SArmyWHCModeCabinetAttr.h"
#include "ArmyWHCModeCabinetOperation.h"
#include "ArmyWHCabinetAutoMgr.h"
#include "ArmyShapeFrameActor.h"
#include "ArmyShapeRuler.h"
#include "common/XRShapeUtilities.h"

#include "UIMixin/SArmyWHCAboveGroundUI.h"
#include "UIMixin/SArmyWHCCabDimensionUI.h"
#include "UIMixin/SArmyWHCCabMtlUI.h"
#include "UIMixin/SArmyWHCCabUI.h"
#include "UIMixin/SArmyWHCDecorationBoardUI.h"

#include "Data/WHCModeData/XRWHCModeData.h"

#include "ArmyStyle.h"
#include "ArmyDetailBuilder.h"
#include "SContentItem.h"
#include "Actor/SCTShapeActor.h"
#include "Actor/XRActorConstant.h"
#include "Game/XRGameInstance.h"
#include "ArmySlateModule.h"
#include "ArmyDownloadModule.h"

#include "SCTShape.h"
#include "SCTAnimation.h"
#include "UtilityTools/SCTShapeUtilityTools.h"

FArmyWHCModeCabinetAttr::FArmyWHCModeCabinetAttr(TSharedRef<SArmyWHCModeAttrPanelContainer> InContainer)
: FArmyWHCModeAttrPanel(InContainer)
{}

void FArmyWHCModeCabinetAttr::TickPanel()
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
	TArray<int32> CabIds = ContainerPtr->GetReplacingIds();
    const FWHCModeGlobalData::TCabinetInfoArr &CabinetInfoArr = *FWHCModeGlobalData::CabinetInfoArrCollection.Find(FWHCModeGlobalData::CurrentWHCClassify);

    TSharedPtr<FDownloadFileSet> TheTask;
	for (int32 i = 0; i < CabIds.Num(); ++i)
	{
        int32 Id = CabIds[i];

		SContentItem *ContentItem = ContainerPtr->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_Cab);
		if (ContentItem && ContentItem->GetItemState() == EDownloading)
		{
            const FCabinetInfo *CabinetInfoPtr = CabinetInfoArr.FindByPredicate(FCabinetInfoFunctor(Id));
            check(CabinetInfoPtr != nullptr);
            TheTask = FArmyDownloadModule::Get().GetDownloadManager()->GetTask(CabinetInfoPtr->DownloadTaskFlag);
            if (TheTask.IsValid())
            {
                UpdateProgress(TheTask, ContentItem);
                continue;
            }
        }

        ContentItem = ContainerPtr->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_CabMtl);
        if (ContentItem && ContentItem->GetItemState() == EDownloading)
        {
            const TSharedPtr<FMtlInfo> * CabinetMtlPtr = FWHCModeGlobalData::CabinetMtlMap.Find(Id);
            check(CabinetMtlPtr != nullptr);
            TheTask = FArmyDownloadModule::Get().GetDownloadManager()->GetTask((*CabinetMtlPtr)->MtlUrl);
            if (TheTask.IsValid())
            {
                UpdateProgress(TheTask, ContentItem);
                continue;
            }
		}
	}
}

void FArmyWHCModeCabinetAttr::Initialize()
{
    SArmyWHCModeAttrPanelContainer * ContainerPtr = Container.Pin().Get();

    CabinetDetailPanel = MakeShareable(new FArmyDetailBuilder);

    FArmyDetailNode &BasicParameters = CabinetDetailPanel->AddCategory("BasicParameters", TEXT("基本参数"));

    FArmyWHCCabUI *CabUIComponent = MakeAttrUIComponent<FArmyWHCCabUI>(ContainerPtr, ComponentArr, (int32)ECabinetClassify::EClassify_Cabinet);
    FArmyDetailNode &CabTypeNode = BasicParameters.AddChildNode("CabinetType", TEXT("柜体型号"), 
        CabUIComponent->MakeWidget()
    );
    CabTypeNode.NameContentOverride().VAlign(VAlign_Center);
    CabTypeNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyWHCCabMtlUI *CabMtlComponent = MakeAttrUIComponent<FArmyWHCCabMtlUI>(ContainerPtr, ComponentArr, true);
    FArmyDetailNode &CabMtlNode = BasicParameters.AddChildNode("CabinetMtl", TEXT("柜体材质"),
        CabMtlComponent->MakeWidget()
    );
    CabMtlNode.NameContentOverride().VAlign(VAlign_Center);
    CabMtlNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    MakeAttrUIComponent<FArmyWHCCabWidthUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &CabWidthNode = BasicParameters.AddChildNode("CabWidth", TEXT("宽度(mm)"));
    CabWidthNode.NameContentOverride().VAlign(VAlign_Center);
    CabWidthNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
    MakeAttrUIComponent<FArmyWHCCabDepthUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &CabDepthNode = BasicParameters.AddChildNode("CabDepth", TEXT("深度(mm)"));
    CabDepthNode.NameContentOverride().VAlign(VAlign_Center);
    CabDepthNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
    MakeAttrUIComponent<FArmyWHCCabHeightUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &CabHeightNode = BasicParameters.AddChildNode("CabHeight", TEXT("高度(mm)"));
    CabHeightNode.NameContentOverride().VAlign(VAlign_Center);
    CabHeightNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyDetailNode &MovementParameters = CabinetDetailPanel->AddCategory("MovementParameters", TEXT("位置移动"));

    FArmyWHCAboveGroundUI *AboveGroundComponent = MakeAttrUIComponent<FArmyWHCAboveGroundUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &AboveGroundNode = MovementParameters.AddChildNode("AboveGround", TEXT("离地高度"),
        AboveGroundComponent->MakeWidget()
    );
    AboveGroundNode.NameContentOverride().VAlign(VAlign_Center);
    AboveGroundNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    LeftRightEditable = AddEditableTextNode(&MovementParameters, "LeftRightMove", TEXT("左右"), 
        FOnIsTypedCharValid::CreateRaw(this, &FArmyWHCModeCabinetAttr::Callback_IsNumericWithMinus),
        FOnTextCommitted::CreateRaw(this, &FArmyWHCModeCabinetAttr::Callback_RightValueCommitted));
    UpDownEditable = AddEditableTextNode(&MovementParameters, "UpDownMove", TEXT("上下"),
        FOnIsTypedCharValid::CreateRaw(this, &FArmyWHCModeCabinetAttr::Callback_IsNumericWithMinus),
        FOnTextCommitted::CreateRaw(this, &FArmyWHCModeCabinetAttr::Callback_UpValueCommitted));
    FrontBackEditable = AddEditableTextNode(&MovementParameters, "FrontBackMove", TEXT("前后"),
        FOnIsTypedCharValid::CreateRaw(this, &FArmyWHCModeCabinetAttr::Callback_IsNumericWithMinus),
        FOnTextCommitted::CreateRaw(this, &FArmyWHCModeCabinetAttr::Callback_FrontValueCommitted));

    

    MakeAttrUIComponent<FArmyWHCDecorationBoardUI>(ContainerPtr, ComponentArr, 0);
    MakeAttrUIComponent<FArmyWHCDecorationBoardUI>(ContainerPtr, ComponentArr, 1);
}

bool FArmyWHCModeCabinetAttr::TryDisplayAttrPanel(AActor *InActor)
{
    if (InActor->IsA<AXRShapeFrame>())
    {
        TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
        if (ContainerPtr->LastWHCItemSelected == InActor)
            return true;

        AXRShapeFrame * ShapeFrame = Cast<AXRShapeFrame>(InActor);
        FShapeInRoom * RESTRICT ShapeInRoom = ShapeFrame->ShapeInRoomRef;
        
        if (CheckCanShowCabinetAttr(ShapeInRoom))
        {
            ContainerPtr->LastWHCItemSelected = InActor;
            TryDisplayAttrPanelInternal(ShapeInRoom);
            ContainerPtr->SetAttrPanel(CabinetDetailPanel->BuildDetail().ToSharedRef());
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

void FArmyWHCModeCabinetAttr::LoadMoreReplacingData()
{
    const FString & CurrentState = Container.Pin()->GetCurrentState();
    if (CurrentState == TEXT("柜体型号"))
    {
        TArray<IArmyWHCAttrUIComponent*> CabUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_Cab, ComponentArr);
        FArmyWHCCabUI *CabUIComponent = (FArmyWHCCabUI *)CabUIComponents[0];
        CabUIComponent->LoadListData();
    }
    else if (CurrentState == TEXT("柜体材质"))
    {
        TArray<IArmyWHCAttrUIComponent*> CabMtlUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_CabMtl, ComponentArr);
        FArmyWHCCabMtlUI *CabMtlUIComponent = (FArmyWHCCabMtlUI *)CabMtlUIComponents[0];
        CabMtlUIComponent->LoadListData();
    }
    else if (CurrentState == TEXT("见光板材质"))
    {
        TArray<IArmyWHCAttrUIComponent*> DecBoardUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_DecorationBoard, ComponentArr);
        FArmyWHCDecorationBoardUI *DecBoardUIComponent = (FArmyWHCDecorationBoardUI *)DecBoardUIComponents[0];
        DecBoardUIComponent->LoadListData();
    }
}

TSharedPtr<SWidget> FArmyWHCModeCabinetAttr::RebuildAttrPanel()
{
    AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(Container.Pin()->LastWHCItemSelected);
    check(ShapeFrame->ShapeInRoomRef != nullptr);
    RebuildAttrPanelInternal(ShapeFrame->ShapeInRoomRef);
    return CabinetDetailPanel->BuildDetail();
}

void FArmyWHCModeCabinetAttr::TryDisplayAttrPanelInternal(FShapeInRoom *InShapeInRoom)
{
    CabinetDetailPanel->RemoveCategory("ConfigParameters");
    MakeDimensionUI();
    MakeDecorationBoardUI(InShapeInRoom);
}

void FArmyWHCModeCabinetAttr::RebuildAttrPanelInternal(FShapeInRoom *InShapeInRoom)
{
    CabinetDetailPanel->RemoveCategory("ConfigParameters");
    MakeDimensionUI();
    MakeDecorationBoardUI(InShapeInRoom);
}

bool FArmyWHCModeCabinetAttr::CheckCanShowCabinetAttr(FShapeInRoom *InShapeInRoom) const
{
    int32 ShapeCategory = InShapeInRoom->Shape->GetShapeCategory();
    return ShapeCategory == (int32)ECabinetType::EType_HangCab || 
            ShapeCategory == (int32)ECabinetType::EType_OnGroundCab || 
            ShapeCategory == (int32)ECabinetType::EType_TallCab || 
            ShapeCategory == (int32)ECabinetType::EType_OnCabCab;
}

void FArmyWHCModeCabinetAttr::MakeDimensionUI()
{
    TSharedPtr<FArmyDetailNode> BasicParameters = CabinetDetailPanel->EditCategory("BasicParameters");
    TSharedPtr<FArmyDetailNode> CabWidthNode = BasicParameters->FindChildNode("CabWidth");
    if (CabWidthNode.IsValid())
    {
        TArray<IArmyWHCAttrUIComponent*> CabWidthComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_WidthEditing, ComponentArr);
        CabWidthNode->ValueContentOverride()[CabWidthComponents[0]->MakeWidget().ToSharedRef()];
    }
    TSharedPtr<FArmyDetailNode> CabDepthNode = BasicParameters->FindChildNode("CabDepth");
    if (CabDepthNode.IsValid())
    {
        TArray<IArmyWHCAttrUIComponent*> CabDepthComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_DepthEditing, ComponentArr);
        CabDepthNode->ValueContentOverride()[CabDepthComponents[0]->MakeWidget().ToSharedRef()];
    }
    TSharedPtr<FArmyDetailNode> CabHeightNode = BasicParameters->FindChildNode("CabHeight");
    if (CabHeightNode.IsValid())
    {
        TArray<IArmyWHCAttrUIComponent*> CabHeightComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_HeightEditing, ComponentArr);
        CabHeightNode->ValueContentOverride()[CabHeightComponents[0]->MakeWidget().ToSharedRef()];
    }
}

void FArmyWHCModeCabinetAttr::MakeDecorationBoardUI(FShapeInRoom *InShapeInRoom)
{
    TSharedPtr<FArmyDetailNode> ConfigParameters;
    if (InShapeInRoom->DecBoardInfos.Num() > 0)
    {
        ConfigParameters = CabinetDetailPanel->EditCategory("ConfigParameters");
        if (!ConfigParameters.IsValid())
            ConfigParameters = CabinetDetailPanel->AddCategory("ConfigParameters", TEXT("柜体配置")).AsShared();
    }
    else
        return;

    TArray<IArmyWHCAttrUIComponent*> DecBoardUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_DecorationBoard, ComponentArr);

    FName Name;
    FString DisplayName;
    int32 Index = 0;
    TSharedPtr<FArmyDetailNode> DecBoardNode;

    for (const auto &BoardInfo : InShapeInRoom->DecBoardInfos)
    {
        switch ((FSCTShapeUtilityTool::EPosition)BoardInfo.Face)
        {
            case FSCTShapeUtilityTool::EPosition::E_Left:
            {
                Name = FName("LeftDecBoard");
                DisplayName = TEXT("左见光板");
                Index = 0;
                break;
            }
            case FSCTShapeUtilityTool::EPosition::E_Right:
            {
                Name = FName("RightDecBoard");
                DisplayName = TEXT("右见光板");
                Index = 1;
                break;
            }
        }

        FArmyWHCDecorationBoardUI *DecBoardUI = (FArmyWHCDecorationBoardUI *)(DecBoardUIComponents[Index]);
        DecBoardNode = ConfigParameters->AddChildNode(Name, DisplayName).AsShared();
        DecBoardNode->NameContentOverride().VAlign(VAlign_Center);
        if (BoardInfo.Spawned)
        {
            DecBoardUI->SetAddOrReplace(false);
            DecBoardNode->ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f))
            [DecBoardUI->MakeWidget().ToSharedRef()];
        }
        else
        {
            DecBoardUI->SetAddOrReplace(true);
            DecBoardNode->ValueContentOverride().HAlign(HAlign_Left).VAlign(VAlign_Fill)
            [DecBoardUI->MakeWidget().ToSharedRef()];
        }
    }
}


void FArmyWHCModeCabinetAttr::Callback_UpValueCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    if (!ContainerPtr->IsWHCItemSelected<AXRShapeFrame>())
        return;
	if (InTextAction == ETextCommit::OnEnter)
	{
        FString Str = InText.ToString();
        if (IsValidNumberText(Str))
        {
            float NewHeight = FCString::Atof(*Str);
            AXRShapeFrame * ShapeFrame = Cast<AXRShapeFrame>(ContainerPtr->LastWHCItemSelected);
            check(ShapeFrame->ShapeInRoomRef != nullptr);
            if (CheckIfChangeAllCabHeights(ShapeFrame->ShapeInRoomRef))
                gCabinMgr.OnUIAboveGroundChange(gCabinMgr.GetAboveGround((int32)ECabinetType::EType_OnGroundCab) * 10.0f + NewHeight);
            else
            {
                gCabinMgr.OnUIAboveGroundChangeSingle(ShapeFrame->ShapeInRoomRef->AboveGround * 10.0f + NewHeight, ShapeFrame->ShapeInRoomRef);
                ShapeFrame->ShapeInRoomRef->SpawnPlatform();
            }
            ContainerPtr->CabinetOperationRef->CalculateShapeDistanceFromWall(ShapeFrame->ShapeInRoomRef);
            ContainerPtr->CabinetOperationRef->TryModifySelectedCabinet();
        }
	}
}

void FArmyWHCModeCabinetAttr::Callback_FrontValueCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    if (!ContainerPtr->IsWHCItemSelected<AXRShapeFrame>())
        return;
	if (InTextAction == ETextCommit::OnEnter)
	{
        FString Str = InText.ToString();
        if (IsValidNumberText(Str))
        {
            float Delta = FCString::Atof(*Str) * 0.1f;
            if (!FMath::IsNearlyZero(Delta, KINDA_SMALL_NUMBER))
            {
                if (Delta < 0.0f)
                {
                    if (FMath::RoundToInt(FMath::Abs(Delta) * 10.0f) <= FMath::RoundToInt(ContainerPtr->CabinetOperationRef->GetSelectedShapeBackRange() * 10.0f))
                        ContainerPtr->CabinetOperationRef->MoveShapeBack(Delta);
                }
                else
                    ContainerPtr->CabinetOperationRef->MoveShapeBack(Delta);
            }
        }
	}
}

void FArmyWHCModeCabinetAttr::Callback_RightValueCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    if (!ContainerPtr->IsWHCItemSelected<AXRShapeFrame>())
        return;
	if (InTextAction == ETextCommit::OnEnter)
	{
        FString Str = InText.ToString();
        if (IsValidNumberText(Str))
        {
            float Delta = FCString::Atof(*Str) * 0.1f;
            float AbsDelta = FMath::Abs(Delta) * 10.0f;
            if (!FMath::IsNearlyZero(Delta, KINDA_SMALL_NUMBER))
            {
                if (Delta > 0.0f)
                {
                    if (FMath::RoundToInt(AbsDelta) <= FMath::RoundToInt(ContainerPtr->CabinetOperationRef->GetSelectedShapeRightMoveRange() * 10.0f))
                        ContainerPtr->CabinetOperationRef->MoveShapeRight(FMath::Abs(Delta));
                }
                else
                {
                    if (FMath::RoundToInt(AbsDelta) <= FMath::RoundToInt(ContainerPtr->CabinetOperationRef->GetSelectedShapeLeftMoveRange() * 10.0f))
                        ContainerPtr->CabinetOperationRef->MoveShapeLeft(FMath::Abs(Delta));
                }
            }
        }
	}
}

bool FArmyWHCModeCabinetAttr::CheckIfChangeAllCabHeights(FShapeInRoom *InShapeInRoom) const
{
    ECabinetType Type = (ECabinetType)InShapeInRoom->Shape->GetShapeCategory();
    switch (Type)
    {
        case ECabinetType::EType_OnGroundCab:
        case ECabinetType::EType_TallCab:
            return true;
        case ECabinetType::EType_HangCab:
        case ECabinetType::EType_OnCabCab:
            return false;
        default:
            return false;
    }
}

TSharedPtr<SEditableText> FArmyWHCModeCabinetAttr::AddEditableTextNode(FArmyDetailNode *ParentNode, FName Name, const FString &DisplayName, 
    FOnIsTypedCharValid IsTypedCharValid, 
    FOnTextCommitted TextCommitted)
{
    TSharedPtr<SEditableText> EditableText;
    FArmyDetailNode &Node = ParentNode->AddChildNode(Name, DisplayName,
        SNew(SBox)
        .HeightOverride(24.0f)
        [
            SNew(SBorder)
            .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF38393D"))
            .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
            .Padding(FMargin(8.0f, 0.0f, 0.0f, 0.0f))
            .VAlign(VAlign_Center)
            [
                SAssignNew(EditableText, SEditableText)
                .Text_Lambda([]() -> FText { return FText::FromString(TEXT("0")); })
                .Font(FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12").Font)
                .ColorAndOpacity(FLinearColor::White)
                .SelectAllTextWhenFocused(true)
                .OnIsTypedCharValid(IsTypedCharValid)
                .OnTextCommitted(TextCommitted)
            ]
        ]
    );
    Node.NameContentOverride().VAlign(VAlign_Center);
    Node.ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    return EditableText;
}

bool FArmyWHCModeCabinetAttr::Callback_IsNumericWithMinus(const TCHAR c)
{
    return Callback_IsNumericWithoutMinus(c) || (c == TEXT('-'));
}

bool FArmyWHCModeCabinetAttr::Callback_IsNumericWithoutMinus(const TCHAR c)
{
    return (c >= TEXT('0')) && (c <= TEXT('9'));
}

void FArmyWHCModeWithFrontBoardCabinetAttr::Initialize()
{
    FArmyWHCModeCabinetAttr::Initialize();

    SAssignNew(FrontBoardCabOpenDoorType, SArmyWHCModeNormalComboBox)
    .OnValueChanged_Raw(this, &FArmyWHCModeWithFrontBoardCabinetAttr::Callback_DoorOpenDirChanged);
    FrontBoardCabOpenDoorType->SetValueAttr(TAttribute<FText>::Create(
        [this]() -> FText {
            TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
            if (ContainerPtr->IsWHCItemSelected<AXRShapeFrame>())
            {
                AXRShapeFrame *ShapeFrame = static_cast<AXRShapeFrame*>(ContainerPtr->LastWHCItemSelected);
                check(ShapeFrame->ShapeInRoomRef != nullptr);

                bool bOutSingleDoor = false, bOutDoubleDoor = false, bOutCurIsSingleDoor = false, bOutCurIsLeft = false;
                float OutDoorSheetWidth = 0.0f;
                if (FSCTShapeUtilityTool::GetFrontBoardCabinetDoorModifyInfo(ShapeFrame->ShapeInRoomRef->Shape.Get(), 
                    bOutSingleDoor, bOutDoubleDoor, OutDoorSheetWidth, bOutCurIsSingleDoor, bOutCurIsLeft))
                {
                    return FText::FromString(bOutCurIsSingleDoor ? TEXT("单开") : TEXT("对开"));
                }
            }
            return FText();
        }
    ));
}

void FArmyWHCModeWithFrontBoardCabinetAttr::TryDisplayAttrPanelInternal(FShapeInRoom *InShapeInRoom)
{
    FArmyWHCModeCabinetAttr::TryDisplayAttrPanelInternal(InShapeInRoom);
    MakeFrontBoardCabUI(InShapeInRoom);
}

void FArmyWHCModeWithFrontBoardCabinetAttr::RebuildAttrPanelInternal(FShapeInRoom *InShapeInRoom)
{
    FArmyWHCModeCabinetAttr::RebuildAttrPanelInternal(InShapeInRoom);
    MakeFrontBoardCabUI(InShapeInRoom);
}

bool FArmyWHCModeWithFrontBoardCabinetAttr::CheckCanShowCabinetAttr(FShapeInRoom *InShapeInRoom) const
{
    bool bOutSingleDoor = false, bOutDoubleDoor = false, bOutCurIsSingleDoor = false, bOutCurIsLeft = false;
    float OutDoorSheetWidth = 0.0f;
    bool bFrontBoardCab = FSCTShapeUtilityTool::GetFrontBoardCabinetDoorModifyInfo(InShapeInRoom->Shape.Get(), bOutSingleDoor, bOutDoubleDoor, OutDoorSheetWidth, bOutCurIsSingleDoor, bOutCurIsLeft);
    return FArmyWHCModeCabinetAttr::CheckCanShowCabinetAttr(InShapeInRoom) && bFrontBoardCab;
}

void FArmyWHCModeWithFrontBoardCabinetAttr::Callback_DoorOpenDirChanged(const FString &InText)
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    if (!ContainerPtr->IsWHCItemSelected<AXRShapeFrame>())
        return;

    AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(ContainerPtr->LastWHCItemSelected);
    check(ShapeFrame->ShapeInRoomRef != nullptr);

    for (auto &Animation : ShapeFrame->ShapeInRoomRef->Animations)
        Animation->Stop();
    if (InText == TEXT("单开"))
        FSCTShapeUtilityTool::ModifyForntBoardCabinetDoorSheetNum(ShapeFrame->ShapeInRoomRef->Shape.Get(), 1);
    else if (InText == TEXT("对开"))
        FSCTShapeUtilityTool::ModifyForntBoardCabinetDoorSheetNum(ShapeFrame->ShapeInRoomRef->Shape.Get(), 2);
    ShapeFrame->ShapeInRoomRef->PrepareShapeAnimations();

    ShapeFrame->ShapeInRoomRef->MakeImmovable();
    FSCTShapeUtilityTool::SetActorTag(ShapeFrame->ShapeInRoomRef->Shape->GetShapeActor(), XRActorTag::WHCActor);
}

void FArmyWHCModeWithFrontBoardCabinetAttr::Callback_DoorSheetWidthValueCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    if (!ContainerPtr->IsWHCItemSelected<AXRShapeFrame>())
        return;

    if (InTextAction == ETextCommit::OnEnter)
    {
        FString TextStr = InText.ToString();
        if (IsValidNumberText(TextStr))
        {
            float Value = FCString::Atof(*TextStr);
            AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(ContainerPtr->LastWHCItemSelected);
            check(ShapeFrame->ShapeInRoomRef != nullptr);

            for (auto &Animation : ShapeFrame->ShapeInRoomRef->Animations)
                Animation->Stop();
            if (FSCTShapeUtilityTool::ModifyFrontBoardCabinetDoorWidth(ShapeFrame->ShapeInRoomRef->Shape.Get(), Value))
            {
                ShapeFrame->ShapeInRoomRef->PrepareShapeAnimations();
                ShapeFrame->ShapeInRoomRef->MakeImmovable();
                FSCTShapeUtilityTool::SetActorTag(ShapeFrame->ShapeInRoomRef->Shape->GetShapeActor(), XRActorTag::WHCActor);
                // 检查可能会发生的变化
                MakeFrontBoardCabUI(ShapeFrame->ShapeInRoomRef);
            }
            else
            {
                GGI->Window->ShowMessage(MT_Warning, TEXT("门板尺寸超出可变范围 请重新输入"));
            }
        }
    }
}

void FArmyWHCModeWithFrontBoardCabinetAttr::MakeFrontBoardCabUI(FShapeInRoom *InShapeInRoom)
{
    bool bOutSingleDoor = false, bOutDoubleDoor = false, bOutCurIsSingleDoor = false, bOutCurIsLeft = false;
    float OutDoorSheetWidth = 0.0f;
    if (FSCTShapeUtilityTool::GetFrontBoardCabinetDoorModifyInfo(InShapeInRoom->Shape.Get(), bOutSingleDoor, bOutDoubleDoor, OutDoorSheetWidth, bOutCurIsSingleDoor, bOutCurIsLeft))
    {
        TSharedPtr<FArmyDetailNode> ConfigParameters = CabinetDetailPanel->EditCategory("ConfigParameters");
        if (!ConfigParameters.IsValid())
            ConfigParameters = CabinetDetailPanel->AddCategory("ConfigParameters", TEXT("柜体配置")).AsShared();

        if (!ConfigParameters->FindChildNode("FrontBoardCabOpenDir").IsValid())
        {
            FArmyDetailNode &OpenDirNode = ConfigParameters->AddChildNode("FrontBoardCabOpenDir", TEXT("插脚开门"),
                FrontBoardCabOpenDoorType.ToSharedRef()
            );
            OpenDirNode.NameContentOverride().VAlign(VAlign_Center);
            OpenDirNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
        }
        TArray<TSharedPtr<FString>> OpenDirs;
        if (bOutSingleDoor)
            OpenDirs.Emplace(MakeShareable(new FString(TEXT("单开"))));
        if (bOutDoubleDoor)
            OpenDirs.Emplace(MakeShareable(new FString(TEXT("对开"))));
        FrontBoardCabOpenDoorType->SetOptionSource(OpenDirs);
            
        if (!ConfigParameters->FindChildNode("DoorSheetWidth").IsValid())
        {
            FrontBoardCabDoorWidthEditable = AddEditableTextNode(ConfigParameters.Get(), "DoorSheetWidth", TEXT("外露门板"), 
                FOnIsTypedCharValid::CreateRaw(this, &FArmyWHCModeWithFrontBoardCabinetAttr::Callback_IsNumericWithoutMinus),
                FOnTextCommitted::CreateRaw(this, &FArmyWHCModeWithFrontBoardCabinetAttr::Callback_DoorSheetWidthValueCommitted)
            );
            FrontBoardCabDoorWidthEditable->SetText(TAttribute<FText>::Create(
                [this]() -> FText {
                    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
                    if (ContainerPtr->IsWHCItemSelected<AXRShapeFrame>())
                    {
                        AXRShapeFrame *ShapeFrame = static_cast<AXRShapeFrame*>(ContainerPtr->LastWHCItemSelected);
                        check(ShapeFrame->ShapeInRoomRef != nullptr);

                        bool bOutSingleDoor = false, bOutDoubleDoor = false, bOutCurIsSingleDoor = false, bOutCurIsLeft = false;
                        float OutDoorSheetWidth = 0.0f;
                        if (FSCTShapeUtilityTool::GetFrontBoardCabinetDoorModifyInfo(ShapeFrame->ShapeInRoomRef->Shape.Get(), 
                            bOutSingleDoor, bOutDoubleDoor, OutDoorSheetWidth, bOutCurIsSingleDoor, bOutCurIsLeft))
                        {
                            return FText::FromString(FString::Printf(TEXT("%d"), FMath::RoundToInt(OutDoorSheetWidth)));
                        }
                    }
                    return FText::FromString(TEXT("0"));
                }
            ));
        }
    }
}