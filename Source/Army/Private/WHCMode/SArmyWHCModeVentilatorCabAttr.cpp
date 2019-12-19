#include "SArmyWHCModeVentilatorCabAttr.h"
#include "ArmyShapeFrameActor.h"
#include "ArmyShapeTableActor.h"
#include "ArmyWHCabinet.h"
#include "ArmyWHCModeCabinetOperation.h"
#include "ArmyWHCabinetAutoMgr.h"

#include "UIMixin/SArmyWHCAboveGroundUI.h"
#include "UIMixin/SArmyWHCCabDimensionUI.h"
#include "UIMixin/SArmyWHCCabMtlUI.h"
#include "UIMixin/SArmyWHCCabUI.h"
#include "UIMixin/SArmyWHCDecorationBoardUI.h"
#include "UIMixin/SArmyWHCCabAccUI.h"

#include "ArmyStyle.h"
#include "ArmyDetailBuilder.h"
#include "SContentItem.h"
#include "Actor/XRActorConstant.h"
#include "ArmyGameInstance.h"
#include "Data/WHCModeData/XRWHCModeData.h"

#include "SCTCabinetShape.h"
#include "SCTShapeManager.h"
#include "Actor/SCTShapeActor.h"

#define VIS_FLAG_CAB 0
#define VIS_FLAG_VENTILATOR 1

void FArmyWHCModeVentilatorCabinetAttr::TickPanel()
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

        ContentItem = ContainerPtr->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_Acc);
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

void FArmyWHCModeVentilatorCabinetAttr::Initialize()
{
    SArmyWHCModeAttrPanelContainer * ContainerPtr = Container.Pin().Get();

    CabinetDetailPanel = MakeShareable(new FArmyDetailBuilder);

    FArmyDetailNode &BasicParameters = CabinetDetailPanel->AddCategory("BasicParameters", TEXT("基本参数"));

    FArmyDetailNode &VisModeNode = BasicParameters.AddChildNode("VisMode", TEXT("结构显示"), 
        SAssignNew(VisModeCombo, SArmyWHCModeNormalComboBox)
        .OptionSource(
            TArray<TSharedPtr<FString>> {
                MakeShareable(new FString(TEXT("烟机柜+烟机"))),
                MakeShareable(new FString(TEXT("仅烟机柜"))),
                MakeShareable(new FString(TEXT("仅烟机")))
            }
        )
        .Value(TEXT("烟机柜+烟机"))
        .OnValueChanged_Raw(this, &FArmyWHCModeVentilatorCabinetAttr::Callback_VisModeChanged)
    );
    VisModeNode.NameContentOverride().VAlign(VAlign_Center);
    VisModeNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    MakeAttrUIComponent<FArmyWHCVentilatorCabUI>(ContainerPtr, ComponentArr, (int32)ECabinetClassify::EClassify_Cabinet);
    MakeAttrUIComponent<FArmyWHCCabMtlUI>(ContainerPtr, ComponentArr, false);

    MakeAttrUIComponent<FArmyWHCCabWidthUI>(ContainerPtr, ComponentArr);
    MakeAttrUIComponent<FArmyWHCCabDepthUI>(ContainerPtr, ComponentArr);
    MakeAttrUIComponent<FArmyWHCCabHeightUI>(ContainerPtr, ComponentArr);
    MakeAttrUIComponent<FArmyWHCCabVentilatorWidthUI>(ContainerPtr, ComponentArr);

    MakeAttrUIComponent<FArmyWHCAccWidthStaticUI>(ContainerPtr, ComponentArr);
    MakeAttrUIComponent<FArmyWHCAccDepthStaticUI>(ContainerPtr, ComponentArr);
    MakeAttrUIComponent<FArmyWHCAccHeightStaticUI>(ContainerPtr, ComponentArr);

    FArmyDetailNode &MovementParameters = CabinetDetailPanel->AddCategory("MovementParameters", TEXT("位置移动"));

    FArmyWHCAboveGroundUI *AboveGroundComponent = MakeAttrUIComponent<FArmyWHCAboveGroundUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &AboveGroundNode = MovementParameters.AddChildNode("AboveGround", TEXT("离地高度"),
        AboveGroundComponent->MakeWidget()
    );
    AboveGroundNode.NameContentOverride().VAlign(VAlign_Center);
    AboveGroundNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    LeftRightEditable = AddEditableTextNode(&MovementParameters, "LeftRightMove", TEXT("左右"), 
        FOnIsTypedCharValid::CreateRaw(this, &FArmyWHCModeVentilatorCabinetAttr::Callback_IsNumericWithMinus),
        FOnTextCommitted::CreateRaw(this, &FArmyWHCModeVentilatorCabinetAttr::Callback_VentilatorCabRightValueCommitted));
    UpDownEditable = AddEditableTextNode(&MovementParameters, "UpDownMove", TEXT("上下"),
        FOnIsTypedCharValid::CreateRaw(this, &FArmyWHCModeVentilatorCabinetAttr::Callback_IsNumericWithMinus),
        FOnTextCommitted::CreateRaw(this, &FArmyWHCModeVentilatorCabinetAttr::Callback_VentilatorCabUpValueCommitted));
    FrontBackEditable = AddEditableTextNode(&MovementParameters, "FrontBackMove", TEXT("前后"),
        FOnIsTypedCharValid::CreateRaw(this, &FArmyWHCModeVentilatorCabinetAttr::Callback_IsNumericWithMinus),
        FOnTextCommitted::CreateRaw(this, &FArmyWHCModeVentilatorCabinetAttr::Callback_VentilatorCabFrontValueCommitted));

    MakeAttrUIComponent<FArmyWHCDecorationBoardUI>(ContainerPtr, ComponentArr, 0);
    MakeAttrUIComponent<FArmyWHCDecorationBoardUI>(ContainerPtr, ComponentArr, 1);
    MakeAttrUIComponent<FArmyWHCVentilatorUI>(ContainerPtr, ComponentArr, true, false);
}

bool FArmyWHCModeVentilatorCabinetAttr::TryDisplayAttrPanel(AActor *InActor)
{
    if (InActor->IsA<AXRShapeFrame>() || InActor->IsA<AXRElecDeviceActor>())
    {
        TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
        if (ContainerPtr->LastWHCItemSelected == InActor)
            return true;

        FString VisValue;
        AXRShapeFrame *ShapeFrame = nullptr;
        int32 Flag = -1;
        if (InActor->IsA<AXRShapeFrame>())
        {
            ShapeFrame = Cast<AXRShapeFrame>(InActor);
            check(ShapeFrame->ShapeInRoomRef != nullptr);
            if (!ShapeFrame->ShapeInRoomRef->Labels.Contains(ECabinetLabelType::ELabelType_Ventilator))
                return false;
			if (ShapeFrame->ShapeInRoomRef->CabinetActor.IsValid())
			{
				XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = ShapeFrame->ShapeInRoomRef->CabinetActor;
				TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
				ContainerPtr->LastWHCItemSelected = ShapeFrame;
				if (Accessory.IsValid() && Accessory->Actor != nullptr)
				{
					VisValue = ShapeFrame->ShapeInRoomRef->HasValidAccessory() ? TEXT("烟机柜+烟机") : TEXT("仅烟机柜");
				}
			}
            else
            {
                VisValue = TEXT("仅烟机柜");
            }
            Flag = VIS_FLAG_CAB;
        }
        else if (InActor->IsA<AXRElecDeviceActor>())
        {
            AXRElecDeviceActor *ElecDevActor = Cast<AXRElecDeviceActor>(InActor);
            if (ElecDevActor->GetType() != EMetalsType::MT_FLUE_GAS_TURBINE)
                return false;
            
            AActor *Actor = ElecDevActor;
            do 
            {
                Actor = Actor->GetAttachParentActor();
                if (Actor->IsA<AXRShapeFrame>())
                    break;
            }
            while (Actor != nullptr);
            check(Actor != nullptr);
            ShapeFrame = Cast<AXRShapeFrame>(Actor);
            check(ShapeFrame->ShapeInRoomRef != nullptr);
            if (!ShapeFrame->ShapeInRoomRef->Labels.Contains(ECabinetLabelType::ELabelType_Ventilator))
                return false;
            ContainerPtr->LastWHCItemSelected = ElecDevActor;
            if (ShapeFrame->ShapeInRoomRef->HasValidShape())
            {
                VisValue = TEXT("烟机柜+烟机");
            }
            else
            {
                VisValue = TEXT("仅烟机");    
            }
            Flag = VIS_FLAG_VENTILATOR;
        }
        VisModeCombo->SetValue(VisValue);
        RebuildVentilatorAttrPanel(ShapeFrame->ShapeInRoomRef, Flag);
        ContainerPtr->SetAttrPanel(CabinetDetailPanel->BuildDetail().ToSharedRef());
        return true;
    }
    else
        return false;
}

void FArmyWHCModeVentilatorCabinetAttr::RebuildVentilatorAttrPanel(FShapeInRoom *InShapeInRoom, int32 InFlag)
{
    TSharedPtr<FArmyDetailNode> BasicParameters = CabinetDetailPanel->EditCategory("BasicParameters");
    CabinetDetailPanel->RemoveCategory("ConfigParameters");

    if (InFlag == VIS_FLAG_VENTILATOR)
    {
        BasicParameters->RemoveChildNode("CabinetType");
        BasicParameters->RemoveChildNode("CabinetMtl");
        BasicParameters->RemoveChildNode("CabWidth");
        BasicParameters->RemoveChildNode("CabDepth");
        BasicParameters->RemoveChildNode("CabHeight");
        BasicParameters->RemoveChildNode("VentilatorWidth");

        TArray<IArmyWHCAttrUIComponent*> AccUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_CabAcc, ComponentArr);
        FArmyDetailNode &AccNode = BasicParameters->AddChildNode("AccType", TEXT("烟机型号"), 
            AccUIComponents[0]->MakeWidget());
        AccNode.NameContentOverride().VAlign(VAlign_Center);
        AccNode.ValueContentOverride().VAlign(VAlign_Center).HAlign(HAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

        TArray<IArmyWHCAttrUIComponent*> AccWidthUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_Width, ComponentArr);
        FArmyDetailNode &AccWidthNode = BasicParameters->AddChildNode("AccWidth", TEXT("宽度(mm)"), 
            AccWidthUIComponents[0]->MakeWidget()
        );
        AccWidthNode.NameContentOverride().VAlign(VAlign_Center);
        AccWidthNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

        TArray<IArmyWHCAttrUIComponent*> AccDepthUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_Depth, ComponentArr);
        FArmyDetailNode &AccDepthNode = BasicParameters->AddChildNode("AccDepth", TEXT("厚度(mm)"), 
            AccDepthUIComponents[0]->MakeWidget()
        );
        AccDepthNode.NameContentOverride().VAlign(VAlign_Center);
        AccDepthNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

        TArray<IArmyWHCAttrUIComponent*> AccHeightUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_Height, ComponentArr);
        FArmyDetailNode &AccHeightNode = BasicParameters->AddChildNode("AccHeight", TEXT("高度(mm)"), 
            AccHeightUIComponents[0]->MakeWidget()
        );
        AccHeightNode.NameContentOverride().VAlign(VAlign_Center);
        AccHeightNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
    }
    else if (InFlag == VIS_FLAG_CAB)
    {
        BasicParameters->RemoveChildNode("AccType");
        BasicParameters->RemoveChildNode("AccWidth");
        BasicParameters->RemoveChildNode("AccDepth");
        BasicParameters->RemoveChildNode("AccHeight");

        TArray<IArmyWHCAttrUIComponent*> CabUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_Cab, ComponentArr);
        FArmyDetailNode &CabNode = BasicParameters->AddChildNode("CabinetType", TEXT("柜体型号"), 
            CabUIComponents[0]->MakeWidget());
        CabNode.NameContentOverride().VAlign(VAlign_Center);
        CabNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

        TArray<IArmyWHCAttrUIComponent*> CabMtlUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_CabMtl, ComponentArr);
        FArmyDetailNode &CabMtlNode = BasicParameters->AddChildNode("CabinetMtl", TEXT("柜体材质"), 
            CabMtlUIComponents[0]->MakeWidget());
        CabMtlNode.NameContentOverride().VAlign(VAlign_Center);
        CabMtlNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

        FArmyDetailNode &CabWidthNode = BasicParameters->AddChildNode("CabWidth", TEXT("宽度(mm)"));
        CabWidthNode.NameContentOverride().VAlign(VAlign_Center);
        CabWidthNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
        FArmyDetailNode &CabDepthNode = BasicParameters->AddChildNode("CabDepth", TEXT("深度(mm)"));
        CabDepthNode.NameContentOverride().VAlign(VAlign_Center);
        CabDepthNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
        FArmyDetailNode &CabHeightNode = BasicParameters->AddChildNode("CabHeight", TEXT("高度(mm)"));
        CabHeightNode.NameContentOverride().VAlign(VAlign_Center);
        CabHeightNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
        MakeDimensionUI();

        TArray<IArmyWHCAttrUIComponent*> VentilatorWidthComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_VentilatorWidthEditing, ComponentArr);
        FArmyDetailNode &VentilatorWidthNode = BasicParameters->AddChildNode("VentilatorWidth", TEXT("内空宽(mm)"), 
            VentilatorWidthComponents[0]->MakeWidget());
        VentilatorWidthNode.NameContentOverride().VAlign(VAlign_Center);
        VentilatorWidthNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

        MakeDecorationBoardUI(InShapeInRoom);
    }
}

void FArmyWHCModeVentilatorCabinetAttr::Callback_VisModeChanged(const FString &InNewStr)
{
    if (VisModeCombo->GetValue() == InNewStr)
        return;

    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    if (ContainerPtr->IsWHCItemSelected<AXRShapeFrame>() || ContainerPtr->IsWHCItemSelected<AXRElecDeviceActor>())
    {
        AActor *Actor = ContainerPtr->LastWHCItemSelected;
        do
        {
            if (Actor->IsA<AXRShapeFrame>())
                break;
            Actor = Actor->GetAttachParentActor();
        } while (Actor != nullptr);
        check(Actor != nullptr);
        AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(Actor);
        check(ShapeFrame->ShapeInRoomRef != nullptr);
        VisModeCombo->SetValue(InNewStr);
        if (InNewStr == TEXT("烟机柜+烟机"))
        {
            RebuildVentilatorAttrPanel(ShapeFrame->ShapeInRoomRef, VIS_FLAG_CAB);
            VisMode_BothVentilatorAndCab(ContainerPtr.Get(), ShapeFrame->ShapeInRoomRef);
        }
        else if (InNewStr == TEXT("仅烟机柜"))
        {
            RebuildVentilatorAttrPanel(ShapeFrame->ShapeInRoomRef, VIS_FLAG_CAB);
            VisMode_OnlyVentilatorCab(ContainerPtr.Get(), ShapeFrame->ShapeInRoomRef);
        }
        else if (InNewStr == TEXT("仅烟机"))
        {
            RebuildVentilatorAttrPanel(ShapeFrame->ShapeInRoomRef, VIS_FLAG_VENTILATOR);
            VisMode_OnlyVentilator(ContainerPtr.Get(), ShapeFrame->ShapeInRoomRef);
        }
            
        ContainerPtr->SetAttrPanel(CabinetDetailPanel->BuildDetail().ToSharedRef());
        ContainerPtr->CloseReplacingPanel();
    }
}

void FArmyWHCModeVentilatorCabinetAttr::VisMode_BothVentilatorAndCab(SArmyWHCModeAttrPanelContainer *InContainer, FShapeInRoom *InShapeInRoom)
{
	if (InShapeInRoom->CabinetActor.IsValid())
	{
		XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = InShapeInRoom->CabinetActor;
		TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
		// 需要检测烟机和烟机柜是否能够匹配
		if (Accessory.IsValid())
		{
			InContainer->RunHttpRequest(
				FString::Printf(TEXT("design/cabinets/%d/hollowValue?metalsId=%d&width=%d&depth=%d"), 
					InShapeInRoom->Shape->GetShapeId(), Accessory->CabAccInfo->Id,
					FMath::RoundToInt(InShapeInRoom->Shape->GetShapeWidth()),
					FMath::RoundToInt(InShapeInRoom->Shape->GetShapeDepth())),
				TBaseDelegate<void, const TSharedPtr<FJsonObject>&>::CreateLambda(
					[this, InContainer, InShapeInRoom](const TSharedPtr<FJsonObject> &ResponseData)
					{
						const TSharedPtr<FJsonObject> &Data = ResponseData->GetObjectField(TEXT("data"));
						if (Data.IsValid())
						{
							if (!Data->GetBoolField(TEXT("fitted")))
							{
								GGI->Window->ShowMessage(MT_Warning, TEXT("烟机和烟机柜不匹配"));
							}

							// 更新柜子可用宽高跳变值
							// const FWHCModeGlobalData::TCabinetInfoArr *CabinetInfoArr = FWHCModeGlobalData::CabinetInfoArrCollection.Find((int32)ECabinetClassify::EClassify_Cabinet);
							// check(CabinetInfoArr != nullptr);
							// const FCabinetInfo *CabInfoCPtr = CabinetInfoArr->FindByPredicate(FCabinetInfoFunctor(InShapeInRoom->Shape->GetShapeId()));
							// check(CabInfoCPtr != nullptr);
							// FCabinetInfo *CabInfo = const_cast<FCabinetInfo*>(CabInfoCPtr);
							// CabInfo->WidthInfo.Intervals.Empty();
							bool bWidthInRange = false, bDepthInRange = false, bVentilatorWidthInRange = false;
							if (TArray<float> *WidthIntervals = InShapeInRoom->GetParam<TArray<float>>(PARAM_NAME_CAB_WIDTH_INTERVALS))
							{
								bWidthInRange = WidthIntervals->Contains(InShapeInRoom->Shape->GetShapeWidth());
							}
							if (TArray<float> *DepthIntervals = InShapeInRoom->GetParam<TArray<float>>(PARAM_NAME_CAB_DEPTH_INTERVALS))
							{
								bDepthInRange = DepthIntervals->Contains(InShapeInRoom->Shape->GetShapeDepth());
							}
							if (TArray<float> *VentilatorWidthIntervals = InShapeInRoom->GetParam<TArray<float>>(PARAM_NAME_CAB_VENTILATOR_WIDTH_INTERVALS))
							{
								TPair<TTuple<float, float, float>, TArray<float>> WidthInfo, DepthInfo, HeightInfo;
								FSCTShapeUtilityTool::GetSmokeCabinetSpaceRange(InShapeInRoom->Shape.Get(), WidthInfo, DepthInfo, HeightInfo);
								bVentilatorWidthInRange = VentilatorWidthIntervals->Contains(WidthInfo.Key.Get<2>());
							}

							if ((!bWidthInRange) || (!bDepthInRange) || (!bVentilatorWidthInRange))
								GGI->Window->ShowMessage(MT_Warning, TEXT("烟机柜当前尺寸不适配烟机 请手动检查"));
						}
						else
							GGI->Window->ShowMessage(MT_Warning, TEXT("烟机和烟机柜校验失败 可能会不匹配 请手动检查"));

						VisMode_BothVentilatorAndCabInternal(InContainer, InShapeInRoom);
						InContainer->NotifyAttrPanelNeedRebuild();
					}
				),
				TBaseDelegate<void, EHttpErrorReason>::CreateLambda(
					[this, InContainer, InShapeInRoom](EHttpErrorReason ErrorReason)
					{
						GGI->Window->ShowMessage(MT_Warning, TEXT("烟机和烟机柜校验失败 可能会不匹配 请手动检查"));
						VisMode_BothVentilatorAndCabInternal(InContainer, InShapeInRoom);
					}
				)
			);
		}
	}
}

void FArmyWHCModeVentilatorCabinetAttr::Callback_VentilatorCabUpValueCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    if (ContainerPtr->IsWHCItemSelected<AXRShapeFrame>() || ContainerPtr->IsWHCItemSelected<AXRElecDeviceActor>())
    {
        if (InTextAction == ETextCommit::OnEnter)
        {
            FString Str = InText.ToString();
            if (IsValidNumberText(Str))
            {
                FArmyWHCabinetMgr *CabMgr = FArmyWHCabinetMgr::Get();

                float NewHeight = FCString::Atof(*Str);
                AXRShapeFrame * ShapeFrame = ContainerPtr->CabinetOperationRef->GetShapeFrameFromSelected();
                check(ShapeFrame->ShapeInRoomRef != nullptr);
                if (CheckIfChangeAllCabHeights(ShapeFrame->ShapeInRoomRef))
                    CabMgr->OnUIAboveGroundChange(gCabinMgr.GetAboveGround((int32)ECabinetType::EType_OnGroundCab) * 10.0f + NewHeight);
                else
                    CabMgr->OnUIAboveGroundChangeSingle(ShapeFrame->ShapeInRoomRef->AboveGround * 10.0f + NewHeight, ShapeFrame->ShapeInRoomRef);
                ContainerPtr->CabinetOperationRef->CalculateShapeDistanceFromWall(ShapeFrame->ShapeInRoomRef);
                ContainerPtr->CabinetOperationRef->TryModifySelectedCabinet();
            }
        }
    }
}

void FArmyWHCModeVentilatorCabinetAttr::Callback_VentilatorCabFrontValueCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    if (ContainerPtr->IsWHCItemSelected<AXRShapeFrame>() || ContainerPtr->IsWHCItemSelected<AXRElecDeviceActor>())
    {
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
}

void FArmyWHCModeVentilatorCabinetAttr::Callback_VentilatorCabRightValueCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    if (ContainerPtr->IsWHCItemSelected<AXRShapeFrame>() || ContainerPtr->IsWHCItemSelected<AXRElecDeviceActor>())
    {
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
}

void FArmyWHCModeVentilatorCabinetAttr::VisMode_BothVentilatorAndCabInternal(SArmyWHCModeAttrPanelContainer *InContainer, FShapeInRoom *InShapeInRoom)
{
	if (InShapeInRoom->CabinetActor.IsValid())
	{
		XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = InShapeInRoom->CabinetActor;
		TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
		if (Accessory.IsValid() && Accessory->Actor != nullptr)
		{
			Accessory->Actor->SetIsSelected(false);
			Accessory->Actor->SetActorHiddenInGame(false);
			Accessory->Actor->Tags.Remove(XRActorTag::WHCAlwaysHidden);
		}
	}

    TSharedPtr<FCabinetShape> CabShape = StaticCastSharedPtr<FCabinetShape>(InShapeInRoom->Shape);
    CabShape->HiddenCabinetShapeActors(false);
    FSCTShapeUtilityTool::RemoveActorTag(InShapeInRoom->Shape->GetShapeActor(), XRActorTag::WHCAlwaysHidden);

    InContainer->LastWHCItemSelected = InShapeInRoom->ShapeFrame;
    InContainer->CabinetOperationRef->UpdateLastSelected(InShapeInRoom->ShapeFrame);
    InContainer->CabinetOperationRef->UpdateHighlight();

    // 更新上线条
    FArmyWHCabinetMgr::Get()->OnRefreshShape(InShapeInRoom->Shape->GetShapeCategory());
}

void FArmyWHCModeVentilatorCabinetAttr::VisMode_OnlyVentilatorCab(SArmyWHCModeAttrPanelContainer *InContainer, FShapeInRoom *InShapeInRoom)
{
	if (InShapeInRoom->CabinetActor.IsValid())
	{
		XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = InShapeInRoom->CabinetActor;
		TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
		if (Accessory.IsValid() && Accessory->Actor != nullptr)
		{
			Accessory->Actor->SetIsSelected(false);
			Accessory->Actor->SetActorHiddenInGame(true);
			Accessory->Actor->Tags.Emplace(XRActorTag::WHCAlwaysHidden);
		}
	}

    TSharedPtr<FCabinetShape> CabShape = StaticCastSharedPtr<FCabinetShape>(InShapeInRoom->Shape);
    CabShape->HiddenCabinetShapeActors(false);
    FSCTShapeUtilityTool::RemoveActorTag(CabShape->GetShapeActor(), XRActorTag::WHCAlwaysHidden);

    InContainer->LastWHCItemSelected = InShapeInRoom->ShapeFrame;
    InContainer->CabinetOperationRef->UpdateLastSelected(InShapeInRoom->ShapeFrame);
    InContainer->CabinetOperationRef->UpdateHighlight();

    // 更新上线条
    FArmyWHCabinetMgr::Get()->OnRefreshShape(InShapeInRoom->Shape->GetShapeCategory());
}

void FArmyWHCModeVentilatorCabinetAttr::VisMode_OnlyVentilator(SArmyWHCModeAttrPanelContainer *InContainer, FShapeInRoom *InShapeInRoom)
{  
    TSharedPtr<FCabinetShape> CabShape = StaticCastSharedPtr<FCabinetShape>(InShapeInRoom->Shape);
    CabShape->HiddenCabinetShapeActors(true);
    FSCTShapeUtilityTool::SetActorTag(InShapeInRoom->Shape->GetShapeActor(), XRActorTag::WHCAlwaysHidden);
    InContainer->CabinetOperationRef->ClearSelection();
	if (InShapeInRoom->CabinetActor.IsValid())
	{
		XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = InShapeInRoom->CabinetActor;
		TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
		if (Accessory.IsValid() && Accessory->Actor != nullptr)
		{
			Accessory->Actor->SetIsSelected(true);
			Accessory->Actor->SetActorHiddenInGame(false);
			Accessory->Actor->Tags.Remove(XRActorTag::WHCAlwaysHidden);
			InContainer->LastWHCItemSelected = Accessory->Actor;
			InContainer->CabinetOperationRef->UpdateLastSelected(Accessory->Actor);
		}
	}

    // 更新上线条
    FArmyWHCabinetMgr::Get()->OnRefreshShape(InShapeInRoom->Shape->GetShapeCategory());
}