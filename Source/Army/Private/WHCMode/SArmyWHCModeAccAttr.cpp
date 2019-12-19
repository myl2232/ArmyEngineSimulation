#include "SArmyWHCModeAccAttr.h"
#include "SArmyWHCModeListPanel.h"
#include "ArmyShapeTableActor.h"
#include "ArmyWHCModeCabinetOperation.h"
#include "ArmyShapeFrameActor.h"
#include "ArmyWHCabinet.h"
#include "ArmyWHCabinetAutoMgr.h"

#include "UIMixin/SArmyWHCCabAccUI.h"
#include "UIMixin/SArmyWHCCabDimensionUI.h"

#include "ArmyGameInstance.h"
#include "ArmyStyle.h"
#include "ArmyDetailBuilder.h"
#include "SContentItem.h"
#include "ArmySlateModule.h"
#include "ArmyDownloadModule.h"
#include "SScrollWrapBox.h"
#include "WebImageCache.h"
#include "Actor/XRActorConstant.h"
#include "Data/WHCModeData/XRWHCModeData.h"

FArmyWHCModeAccAttr::FArmyWHCModeAccAttr(TSharedRef<SArmyWHCModeAttrPanelContainer> InContainer)
: FArmyWHCModeAttrPanel(InContainer)
{}

void FArmyWHCModeAccAttr::TickPanel()
{   
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    if (!ContainerPtr->IsWHCItemSelected<AXRElecDeviceActor>())
        return;

    TArray<int32> CabIds = ContainerPtr->GetReplacingIds();
    TSharedPtr<FDownloadFileSet> TheTask;
	for (int32 i = 0; i < CabIds.Num(); ++i)
    {
        int32 Id = CabIds[i];

        SContentItem *ContentItem = ContainerPtr->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_Acc);
        if (ContentItem && ContentItem->GetItemState() == EDownloading)
        {
            const TSharedPtr<FCabinetWholeAccInfo> * pAccInfo = FWHCModeGlobalData::CabinetAccMap.Find(Id);
            check(pAccInfo != nullptr);
            TheTask = FArmyDownloadModule::Get().GetDownloadManager()->GetTask((*pAccInfo)->Self->PakUrl);
            if (TheTask.IsValid())
            {
                // 设置下载进度
                UpdateProgress(TheTask, ContentItem);
                continue;
            }
        }
    }
}

void FArmyWHCModeAccAttr::Initialize()
{
    SArmyWHCModeAttrPanelContainer * ContainerPtr = Container.Pin().Get();

    AccPanel = MakeShareable(new FArmyDetailBuilder);

    FArmyDetailNode &BasicParameters = AccPanel->AddCategory("BasicParameters", TEXT("基本参数"));

    FArmyDetailNode &TypeNode = BasicParameters.AddChildNode("Type", TEXT("型号"));
    TypeNode.NameContentOverride().VAlign(VAlign_Center);
    TypeNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyWHCAccWidthStaticUI *WidthUIComponent = MakeAttrUIComponent<FArmyWHCAccWidthStaticUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &AccWidthNode = BasicParameters.AddChildNode("AccWidth", TEXT("宽度(mm)"), 
        WidthUIComponent->MakeWidget()
    );
    AccWidthNode.NameContentOverride().VAlign(VAlign_Center);
    AccWidthNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyWHCAccDepthStaticUI *DepthUIComponent = MakeAttrUIComponent<FArmyWHCAccDepthStaticUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &AccDepthNode = BasicParameters.AddChildNode("AccDepth", TEXT("厚度(mm)"), 
        DepthUIComponent->MakeWidget()
    );
    AccDepthNode.NameContentOverride().VAlign(VAlign_Center);
    AccDepthNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyWHCAccHeightStaticUI *HeightUIComponent = MakeAttrUIComponent<FArmyWHCAccHeightStaticUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &AccHeightNode = BasicParameters.AddChildNode("AccHeight", TEXT("高度(mm)"), 
        HeightUIComponent->MakeWidget()
    );
    AccHeightNode.NameContentOverride().VAlign(VAlign_Center);
    AccHeightNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    // 顶层附属件
    MakeAttrUIComponent<FArmyWHCAccUI>(ContainerPtr, ComponentArr, true, false);
    // 子附属件，可以删除
    MakeAttrUIComponent<FArmyWHCAccUI>(ContainerPtr, ComponentArr, false, true);
    // 子附属件，不可以删除
    MakeAttrUIComponent<FArmyWHCAccUI>(ContainerPtr, ComponentArr, false, false);
}

bool FArmyWHCModeAccAttr::TryDisplayAttrPanel(AActor *InActor)
{
    if (InActor->IsA<AXRElecDeviceActor>())
    {
        TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
        if (ContainerPtr->LastWHCItemSelected == InActor)
            return true;

        AXRElecDeviceActor * ElecDevActor = Cast<AXRElecDeviceActor>(InActor);
        if (CheckCanShowAccAttr(ElecDevActor))
        {
            ContainerPtr->LastWHCItemSelected = ElecDevActor;
            TryDisplayAttrPanelInternal(ElecDevActor);
            ContainerPtr->SetAttrPanel(AccPanel->BuildDetail().ToSharedRef());
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

void FArmyWHCModeAccAttr::LoadMoreReplacingData()
{
    TArray<IArmyWHCAttrUIComponent*> UIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_CabAcc, ComponentArr);

    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    const FString &CurrentState = ContainerPtr->GetCurrentState();
    if (CurrentState == TEXT("附属件型号"))
    {
        FArmyWHCAccUI *AccUIComponent = (FArmyWHCAccUI *)UIComponents[0];
        AccUIComponent->LoadListData();
    }
    else if (CurrentState == TEXT("子附属件型号"))
    {
        if (ContainerPtr->IsWHCItemSelected<AXRElecDeviceActor>())
        {
            AXRElecDeviceActor *ElecDevActor = Cast<AXRElecDeviceActor>(ContainerPtr->LastWHCItemSelected);
            if (IsTopCategory(ElecDevActor->GetType()))
            {
                FArmyWHCAccUI *AccUIComponent = (FArmyWHCAccUI *)UIComponents[1];
                AccUIComponent->LoadListData();
            }
            else
            {
                FArmyWHCAccUI *AccUIComponent = (FArmyWHCAccUI *)UIComponents[2];
                AccUIComponent->LoadListData();
            }
        }
    }
}

TSharedPtr<SWidget> FArmyWHCModeAccAttr::RebuildAttrPanel()
{
    TArray<IArmyWHCAttrUIComponent*> AccUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_CabAcc, ComponentArr);    
    FArmyWHCAccUIBase *SubAccUIComponent = static_cast<FArmyWHCAccUIBase *>(AccUIComponents[1]);

    TSharedPtr<FArmyDetailNode> ConfigParameters = AccPanel->EditCategory("ConfigParameters");
    if (ConfigParameters.IsValid())
    {
        TSharedPtr<FArmyDetailNode> SubAccNode = ConfigParameters->FindChildNode("SubType");
        check(SubAccNode.IsValid());
        SubAccNode->ValueContentOverride().HAlign(SubAccUIComponent->IsAdd() ? HAlign_Left : HAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f))[
            SubAccUIComponent->MakeWidget().ToSharedRef()
        ];
    }

    return AccPanel->BuildDetail();
}

void FArmyWHCModeAccAttr::TryDisplayAttrPanelInternal(AXRElecDeviceActor *InActor)
{
    TArray<IArmyWHCAttrUIComponent*> UIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_CabAcc, ComponentArr);

    TMap<FName, TSharedPtr<FArmyDetailNode>> & ChildNodes = AccPanel->GetChildrenNodes();
    const TSharedPtr<FArmyDetailNode> & BasicParameters = *ChildNodes.Find("BasicParameters");
    TSharedPtr<FArmyDetailNode> TypeNode = BasicParameters->FindChildNode("Type");

    FArmyDetailNode *SubTypeNode = nullptr;

    TSharedPtr<SEditableText> LeftRightEditable; 
    TSharedPtr<SEditableText> AboveTableEditable;

    EMetalsType CurrentAccType = InActor->GetType();
    switch (CurrentAccType)
    {
        case EMetalsType::MT_KITCHEN_RANGE: // 灶具
        {
            TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();

            TypeNode->SetNodeDisplayName(FText::FromString(TEXT("灶具型号")));
            TypeNode->ValueContentOverride().SetWidget(UIComponents[0]->MakeWidget());

            AXRShapeFrame *ShapeFrame = ContainerPtr->CabinetOperationRef->GetShapeFrameFromSelected();
            check(ShapeFrame != nullptr && ShapeFrame->ShapeInRoomRef != nullptr);
            // 只有灶具柜不在任何重叠组或对齐组中，才会允许添加烟机
            if ( (!ContainerPtr->CabinetOperationRef->IsOverlappedPairByPassive(ShapeFrame->ShapeInRoomRef)) && 
                (!ContainerPtr->CabinetOperationRef->IsAlignedPairByPassive(ShapeFrame->ShapeInRoomRef)) )
            {
                FArmyDetailNode *ConfigParameters = AddDetailNode("ConfigParameters", TEXT("灶具配置"));
                SubTypeNode = &ConfigParameters->AddChildNode("SubType", TEXT("烟机型号"));    
            }
            else
            {
                RemoveDetailNode("ConfigParameters");
            }
            
            
            FArmyDetailNode *MovementParameters = AddDetailNode("MovementParameters", TEXT("位置移动"));
            LeftRightEditable = AddEditableTextNode(MovementParameters, "LeftRight", TEXT("左右偏移"), 
                FOnIsTypedCharValid::CreateRaw(this, &FArmyWHCModeAccAttr::Callback_IsNumericWithMinus),
                FOnTextCommitted::CreateRaw(this, &FArmyWHCModeAccAttr::Callback_LeftRightTextCommitted));
            break;
        }
        case EMetalsType::MT_WATER_CHANNE: // 水槽
        {
            TypeNode->SetNodeDisplayName(FText::FromString(TEXT("水槽型号")));
            TypeNode->ValueContentOverride().SetWidget(UIComponents[0]->MakeWidget());
            FArmyDetailNode *ConfigParameters = AddDetailNode("ConfigParameters", TEXT("水槽配置"));
            SubTypeNode = &ConfigParameters->AddChildNode("SubType", TEXT("龙头型号"));
            FArmyDetailNode *MovementParameters = AddDetailNode("MovementParameters", TEXT("位置移动"));
            LeftRightEditable = AddEditableTextNode(MovementParameters, "LeftRight", TEXT("左右偏移"), 
                FOnIsTypedCharValid::CreateRaw(this, &FArmyWHCModeAccAttr::Callback_IsNumericWithMinus),
                FOnTextCommitted::CreateRaw(this, &FArmyWHCModeAccAttr::Callback_LeftRightTextCommitted));
            break;
        }
        case EMetalsType::MT_BIBCOCK: // 龙头
        {
            TypeNode->SetNodeDisplayName(FText::FromString(TEXT("龙头型号")));
            TypeNode->ValueContentOverride().SetWidget(UIComponents[2]->MakeWidget());
            RemoveDetailNode("ConfigParameters");
            RemoveDetailNode("MovementParameters");
            break;
        }
        case EMetalsType::MT_FLUE_GAS_TURBINE: // 烟机
        {
            TypeNode->SetNodeDisplayName(FText::FromString(TEXT("烟机型号")));
            TypeNode->ValueContentOverride().SetWidget(UIComponents[2]->MakeWidget());
            RemoveDetailNode("ConfigParameters");
            FArmyDetailNode *MovementParameters = AddDetailNode("MovementParameters", TEXT("位置移动"));
            LeftRightEditable = AddEditableTextNode(MovementParameters, "LeftRight", TEXT("左右偏移"), 
                FOnIsTypedCharValid::CreateRaw(this, &FArmyWHCModeAccAttr::Callback_IsNumericWithMinus),
                FOnTextCommitted::CreateRaw(this, &FArmyWHCModeAccAttr::Callback_LeftRightTextCommitted));
            AboveTableEditable = AddEditableTextNode(MovementParameters, "AboveTable", TEXT("距离台面"),
                FOnIsTypedCharValid::CreateRaw(this, &FArmyWHCModeAccAttr::Callback_IsNumericWithoutMinus),
                FOnTextCommitted::CreateRaw(this, &FArmyWHCModeAccAttr::Callback_AboveTableCommitted));
            break;
        }
        default:
        {
            TypeNode->SetNodeDisplayName(FText::FromString(TEXT("未知型号")));
            TypeNode->ValueContentOverride().SetWidget(UIComponents[0]->MakeWidget());
            RemoveDetailNode("ConfigParameters");
            RemoveDetailNode("MovementParameters");
            break;
        }
    }

    if (LeftRightEditable.IsValid())
    {
        LeftRightEditable->SetText(TAttribute<FText>::Create(
            [this]() -> FText {
                TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
                if (ContainerPtr->IsWHCItemSelected<AXRElecDeviceActor>())
                {
                    AXRElecDeviceActor * ElecDevActor = Cast<AXRElecDeviceActor>(ContainerPtr->LastWHCItemSelected);
                    FVector RelativeLocation = ElecDevActor->GetStaticMeshComponent()->GetRelativeTransform().GetLocation();
                    return FText::FromString(FString::Printf(TEXT("%d"), FMath::RoundToInt(RelativeLocation.X * 10.0f)));
                }
                else
                {
                    return FText::FromString(TEXT("0"));
                }
                
            }
        ));
    }

    if (AboveTableEditable.IsValid())
    {
        AboveTableEditable->SetText(TAttribute<FText>::Create(
            [this]() -> FText {
                TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
                if (ContainerPtr->IsWHCItemSelected<AXRElecDeviceActor>())
                {
                    AXRElecDeviceActor * ElecDevActor = Cast<AXRElecDeviceActor>(ContainerPtr->LastWHCItemSelected);
                    FVector RelativeLocation = ElecDevActor->GetStaticMeshComponent()->GetRelativeTransform().GetLocation();
                    return FText::FromString(FString::Printf(TEXT("%d"), FMath::RoundToInt(RelativeLocation.Z * 10.0f)));
                }
                else
                {
                    return FText::FromString(TEXT("0"));
                }
                
            }
        ));
    }

    if (InActor->ShapeAccRef != nullptr)
    {
        // 只有顶层附属件才会有子部件替换
        if (IsTopCategory(CurrentAccType))
        {
            FArmyWHCAccUIBase *AccUIComponent = (FArmyWHCAccUIBase *)(UIComponents[1]);
            if (InActor->ShapeAccRef->AttachedSubAccessories.Num() > 0)
            {
                check(SubTypeNode != nullptr);

                AccUIComponent->SetAddOrReplace(false);
                SubTypeNode->ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f)).SetWidget(AccUIComponent->MakeWidget());
            }
            else if (SubTypeNode != nullptr)
            {
                AccUIComponent->SetAddOrReplace(true);
                SubTypeNode->ValueContentOverride().HAlign(HAlign_Left).SetWidget(AccUIComponent->MakeWidget());
            }
        }
    }
}

FArmyDetailNode* FArmyWHCModeAccAttr::AddDetailNode(FName Name, const FString &DisplayName)
{
    return &AccPanel->AddCategory(Name, DisplayName);
}

void FArmyWHCModeAccAttr::RemoveDetailNode(FName Name)
{
    AccPanel->GetChildrenNodes().Remove(Name);
}

TSharedPtr<SEditableText> FArmyWHCModeAccAttr::AddEditableTextNode(FArmyDetailNode *ParentNode, FName Name, const FString &DisplayName, 
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
                .Text(FText::FromString(TEXT("0")))
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

bool FArmyWHCModeAccAttr::Callback_IsNumericWithMinus(const TCHAR c)
{
    return Callback_IsNumericWithoutMinus(c) || (c == TEXT('-'));
}

bool FArmyWHCModeAccAttr::Callback_IsNumericWithoutMinus(const TCHAR c)
{
    return (c >= TEXT('0')) && (c <= TEXT('9'));
}

void FArmyWHCModeAccAttr::Callback_LeftRightTextCommitted(const FText &InText, ETextCommit::Type InType)
{
    if (InType == ETextCommit::OnEnter)
    {
        TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
        if (!ContainerPtr->IsWHCItemSelected<AXRElecDeviceActor>())
            return;

        AXRElecDeviceActor * ElecDevActor = Cast<AXRElecDeviceActor>(ContainerPtr->LastWHCItemSelected);
        FVector RelativeLocation = ElecDevActor->GetStaticMeshComponent()->GetRelativeTransform().GetLocation();

        FString Str = InText.ToString();
        if (IsValidNumberText(Str))
        {
            EMetalsType AccType = ElecDevActor->GetType();
            AXRShapeFrame * ShapeFrame = ContainerPtr->CabinetOperationRef->GetShapeFrameFromSelected();
            check(ShapeFrame != nullptr);
            check(ShapeFrame->ShapeInRoomRef != nullptr);
            int32 Offset = FCString::Atoi(*InText.ToString());
            float CabWidth = ShapeFrame->ShapeInRoomRef->Shape->GetShapeWidth();
            int32 AbsOffset = FMath::Abs(Offset);
            bool bCanMove = false;
            if ((AccType == EMetalsType::MT_WATER_CHANNE || AccType == EMetalsType::MT_BATHROOM_BASIN) && (AbsOffset < (CabWidth - ElecDevActor->ShapeAccRef->CabAccInfo->WidthInfo.Current) * 0.5f))
                bCanMove = true;
            else if ((AccType == EMetalsType::MT_KITCHEN_RANGE) && (AbsOffset < CabWidth * 0.5f))
                bCanMove = true;
            else if (AccType == EMetalsType::MT_FLUE_GAS_TURBINE)
                bCanMove = true;
            if (bCanMove)
            {
                RelativeLocation.X = Offset * 0.1f;
                bool bCanReallyMove = true;
                // 如果修改的是灶具，则同步修改可能对齐的烟机柜位置
                if (AccType == EMetalsType::MT_KITCHEN_RANGE)
                {
                    if (FShapeInRoom *AlignedActiveShape = ContainerPtr->CabinetOperationRef->GetAlignedActiveShapeByPassive(ShapeFrame->ShapeInRoomRef))
                    {
                        FVector PassiveLocation = ShapeFrame->GetActorLocation();
                        FVector ActiveLocation = AlignedActiveShape->ShapeFrame->GetActorLocation();
                        FRotator Rotation = ShapeFrame->GetActorRotation();
                        FVector Dir = Rotation.RotateVector(FVector::ForwardVector);
                        FVector Nor = Rotation.RotateVector(FVector::RightVector);
                        FVector AlignedLocation = PassiveLocation + Dir * RelativeLocation.X + Nor * ((ActiveLocation - PassiveLocation) | Nor);
                        AlignedLocation.Z = ActiveLocation.Z;
                        // 使用希望移动到的位置检测烟机柜是否和其它柜子发生碰撞
                        FArmyWHCRect DesiredRect = AlignedActiveShape->GetRect(&AlignedLocation);
                        bCanReallyMove = !FArmyWHCabinetMgr::Get()->Collision(AlignedActiveShape, &DesiredRect, -0.001f, false);
                        if (bCanReallyMove)
                        {
                            // 如果未发生碰撞则可以移动
                            AlignedActiveShape->ShapeFrame->SetActorLocation(AlignedLocation);
                            // 移动后需更新上线条
                            FArmyWHCabinetMgr::Get()->OnRefreshShape((int32)ECabinetType::EType_HangCab);
                        }
                    }
                }
                if (bCanReallyMove)
                {
                    ElecDevActor->SetActorRelativeLocation(RelativeLocation);
                    ElecDevActor->ShapeAccRef->CabAccInfo->RelativeLocation = RelativeLocation;
                    // 更新构件位置
					XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = ShapeFrame->ShapeInRoomRef->CabinetActor;
					TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
                    Accessory->UpdateComponent();
                    // 在修改完位置后需要重新生成台面
                    if (AccType == EMetalsType::MT_WATER_CHANNE || AccType == EMetalsType::MT_KITCHEN_RANGE)
                        FArmyWHCabinetMgr::Get()->OnRefreshShape((int32)ECabinetType::EType_OnGroundCab);
                }
            }
            else
                GGI->Window->ShowMessage(MT_Warning, TEXT("尺寸超出限制 请重新输入"));
        }
        else
            GGI->Window->ShowMessage(MT_Warning, TEXT("输入不是一个有效数值 请重新输入"));
    }
}

void FArmyWHCModeAccAttr::Callback_AboveTableCommitted(const FText &InText, ETextCommit::Type InType)
{
    if (InType == ETextCommit::OnEnter)
    {
        TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
        if (!ContainerPtr->IsWHCItemSelected<AXRElecDeviceActor>())
            return;

        int32 Distance = FCString::Atoi(*InText.ToString());
        AXRElecDeviceActor * ElecDevActor = Cast<AXRElecDeviceActor>(ContainerPtr->LastWHCItemSelected);
        float AboveGround = ElecDevActor->GetActorLocation().Z;
        float WallHeight = FArmySceneData::Get()->WallHeight;
        if (WallHeight * 10.0f > (AboveGround * 10.0f + Distance))
        {
            FVector RelativeLocation = ElecDevActor->GetStaticMeshComponent()->GetRelativeTransform().GetLocation();
            RelativeLocation.Z = Distance * 0.1f;
            ElecDevActor->SetActorRelativeLocation(RelativeLocation);
            ElecDevActor->ShapeAccRef->CabAccInfo->RelativeLocation = RelativeLocation;
        } 
        else
        {
            GGI->Window->ShowMessage(MT_Warning, TEXT("高度超过了房间高度，请重新输入"));
        }
    }
}

bool FArmyWHCModeAccAttr::IsTopCategory(EMetalsType InMetalsType) const
{
    return InMetalsType == EMetalsType::MT_KITCHEN_RANGE || InMetalsType == EMetalsType::MT_WATER_CHANNE;
}