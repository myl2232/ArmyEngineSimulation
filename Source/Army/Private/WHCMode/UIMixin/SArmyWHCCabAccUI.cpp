#include "UIMixin/SArmyWHCCabAccUI.h"
#include "WHCMode/SArmyWHCModeAttrPanel.h"
#include "WHCMode/SArmyWHCModeListPanel.h"
#include "WHCMode/XRShapeFrameActor.h"
#include "WHCMode/XRWHCabinet.h"
#include "WHCMode/XRWHCModeCabinetOperation.h"
#include "WHCMode/XRWHCabinetAutoMgr.h"

#include "ArmyStyle.h"
#include "SContentItem.h"
#include "SScrollWrapBox.h"
#include "ArmySlateModule.h"
#include "ArmyDownloadModule.h"
#include "Game/XRGameInstance.h"
#include "Data/WHCModeData/XRWHCModeData.h"

#include "WebImageCache.h"

TSharedPtr<SWidget> FArmyWHCAccUIBase::MakeWidget()
{
    if (bAddOrReplace)
    {
        return SNew(SBox)
        .WidthOverride(32.0f)
        .HeightOverride(32.0f)
        [
            SNew(SButton)
            .ContentPadding(FMargin(8.0f))
            .ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Gray.FF353638"))
            .OnClicked_Raw(this, &FArmyWHCCabAccUI::Callback_AddAcc)
            [
                SNew(SImage)
                .Image(FArmyStyle::Get().GetBrush("Icon.Add"))
            ]
        ];
    }
    else
    {
        TSharedPtr<SArmyWHCModeReplacingWidget> CabAccReplacing;

        SAssignNew(CabAccReplacing, SArmyWHCModeReplacingWidget)
        .ButtonImage(bCanDelete ? FArmyStyle::Get().GetBrush("Icon.Delete") : FArmyStyle::Get().GetBrush("WHCMode.Replace"))
        .OnWholeWidgetClicked_Raw(this, &FArmyWHCCabAccUI::Callback_ReplacingAcc)
        .OnButtonClicked_Raw(this, bCanDelete ? (&FArmyWHCCabAccUI::Callback_DeleteAcc) : (&FArmyWHCCabAccUI::Callback_ReplacingAcc));

        CabAccReplacing->SetReplacingItemThumbnail(TAttribute<const FSlateBrush*>::Create(
            [this]() -> const FSlateBrush* {
                if (IsValidSelection())
                {
                    AXRShapeFrame *ShapeFrame = GetShapeFrame();
                    if (ShapeFrame != nullptr)
                    {
                        check(ShapeFrame->ShapeInRoomRef);
						if (ShapeFrame->ShapeInRoomRef->CabinetActor.IsValid())
						{
							XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = ShapeFrame->ShapeInRoomRef->CabinetActor;
							TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
							if (Accessory.IsValid())
							{
								TSharedPtr<FShapeAccessory> &AccessoryRef = Accessory;
								if (bTopCategory)
									return FArmySlateModule::Get().WebImageCache->Download(AccessoryRef->CabAccInfo->ThumbnailUrl)->GetBrush();
								else if (AccessoryRef->AttachedSubAccessories.Num() > 0)
									return FArmySlateModule::Get().WebImageCache->Download(AccessoryRef->AttachedSubAccessories[0]->CabAccInfo->ThumbnailUrl)->GetBrush();
							}
						}
                    }
                    
                }
                return FCoreStyle::Get().GetDefaultBrush();
            }
        ));
        CabAccReplacing->SetReplacingItemText(TAttribute<FText>::Create(
            [this]() -> FText {
                if (IsValidSelection())
                {
                    AXRShapeFrame *ShapeFrame = GetShapeFrame();
                    if (ShapeFrame != nullptr)
                    {
                        check(ShapeFrame->ShapeInRoomRef);
						if (ShapeFrame->ShapeInRoomRef->CabinetActor.IsValid())
						{
							XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = ShapeFrame->ShapeInRoomRef->CabinetActor;
							TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
							if (Accessory.IsValid())
							{
								TSharedPtr<FShapeAccessory> &AccessoryRef = Accessory;
								if (bTopCategory)
									return FText::FromString(AccessoryRef->CabAccInfo->Name);
								else if (AccessoryRef->AttachedSubAccessories.Num() > 0)
									return FText::FromString(AccessoryRef->AttachedSubAccessories[0]->CabAccInfo->Name);
							}
						}
                    }
                    
                }
                return FText();      
            }
        ));

        return MoveTemp(CabAccReplacing);
    }
}

void FArmyWHCAccUIBase::LoadListData()
{
    if (!IsValidSelection())
        return;

    LoadAccData();
}

void FArmyWHCAccUIBase::ReplacingAccItemInternal(int32 InItemCode, FShapeInRoom *InShapeInRoom, FCabinetWholeAccInfo *InAccInfo)
{
    if (bTopCategory)
    {
        InShapeInRoom->DestroyAccessories();
        TSharedPtr<FCabinetWholeComponent> WholeComponent = FWHCModeGlobalData::CabinetComponentMap.FindRef(InShapeInRoom->Shape->GetShapeId());
        if (WholeComponent.IsValid())
        {
            WholeComponent->Accessorys.Append(InAccInfo->Self->ComponentArray);
            for (auto SubComponent : InAccInfo->SubAccInfos)
            {
                WholeComponent->Accessorys.Append(SubComponent->ComponentArray);
            }
        }
        InShapeInRoom->InitializeAccessories(InAccInfo, InAccInfo->Self->ComponentArray);
        InShapeInRoom->SpawnAccessories();
        InShapeInRoom->UpdateComponents(); 
    }
    else
    {
        bool bUseSocket = (InAccInfo->Self->Type == (uint8)EMetalsType::MT_BIBCOCK || InAccInfo->Self->Type == (uint8)EMetalsType::MT_BATHROOM_TAP);

        FVector RelLocation(EForceInit::ForceInitToZero);
        if (InAccInfo->Self->Type == (uint8)EMetalsType::MT_FLUE_GAS_TURBINE)
            RelLocation.Set(0.0f, -InShapeInRoom->Shape->GetShapeDepth() * 0.05f, InAccInfo->Self->AboveGround * 0.1f);
		XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = InShapeInRoom->CabinetActor;
		TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
        if (Accessory->AttachedSubAccessories.Num() > 0)
        {
			/*   FShapeAccessory * SubAccessory = InShapeInRoom->ReplaceSubAccessory(InShapeInRoom->Accessory->AttachedSubAccessories[0]->CabAccInfo.Get(),
				   InAccInfo->Self.Get(), (EMetalsType)InAccInfo->Self->Type, bUseSocket);
			   InShapeInRoom->SpawnSubAccessory(SubAccessory, RelLocation);*/
			InShapeInRoom->ReplaceAndSpawnSubAccessory(Accessory->AttachedSubAccessories[0]->CabAccInfo.Get(),
				InAccInfo->Self.Get(), (EMetalsType)InAccInfo->Self->Type, bUseSocket, RelLocation);
        }
        else
        {
            //FShapeAccessory * SubAccessory = InShapeInRoom->AddSubAccessory(InAccInfo->Self.Get(), (EMetalsType)InAccInfo->Self->Type, bUseSocket);
            //InShapeInRoom->SpawnSubAccessory(SubAccessory, RelLocation);
			InShapeInRoom->AddAndSpawnSubAccessory(InAccInfo->Self.Get(), (EMetalsType)InAccInfo->Self->Type, bUseSocket, RelLocation);
            SetAddOrReplace(false);
            Container->NotifyAttrPanelNeedRebuild();
        }
    }

    AfterAccReplaced(InShapeInRoom, InAccInfo);

    // 更新选中状态
    Container->ListPanel->SetSelectedItem(InItemCode);
}

void FArmyWHCAccUIBase::Callback_ReplacingAcc()
{
    if (!IsValidSelection())
        return;

    FString DesireState(bTopCategory ? TEXT("附属件型号") : TEXT("子附属件型号"));
    if (Container->IsCurrentState(DesireState))
        return;   

    Container->ListPanel->ClearListController();
    if (Container->ReplacingSlot->GetWidget() == SNullWidget::NullWidget)
        Container->SetReplacingPanel(Container->ListPanel.ToSharedRef());

    Container->SetCurrentState(DesireState);

    Container->ClearReplacingList();
    LoadAccData();
}

void FArmyWHCAccUIBase::Callback_ReplacingAccItem(int32 InItemCode)
{
    if (!Container->CheckIsValidReplacingIndex(InItemCode))
        return;

    if (!IsValidSelection())
        return;

    AXRShapeFrame *ShapeFrame = GetShapeFrame();
    check(ShapeFrame->ShapeInRoomRef != nullptr);

    int32 Id = Container->GetReplacingIds()[InItemCode];
    const TSharedPtr<FCabinetWholeAccInfo> * AccInfoPtr = FWHCModeGlobalData::CabinetAccMap.Find(Id);
    check(AccInfoPtr != nullptr);
    const TSharedPtr<FCabinetWholeAccInfo> & AccInfo = *AccInfoPtr;
    if (AccInfo->CabRcState == ECabinetResourceState::CabRc_Complete)
    {
        if (Container->IsClickedSameReplacingItem(InItemCode) && !IsAdd())
            return;

        ReplacingAccItemInternal(InItemCode, ShapeFrame->ShapeInRoomRef, AccInfo.Get());
        
    }
    else if (AccInfo->CabRcState == ECabinetResourceState::CabRc_None)
    {
        SContentItem *Item = Container->GetListItem(InItemCode);
        if (Item)
            Item->SetItemState(EDownloading);
        AccInfo->CabRcState = ECabinetResourceState::CabRc_FetchingRc;
        TArray<FDownloadSpace::FDownloadFileInfo> FileInfos;
        WHC_ADD_DOWNLOAD_TASK(FileInfos, Id, AccInfo->Self->GetCachePath(), AccInfo->Self->PakUrl, AccInfo->Self->PakMd5);
        for (const auto & SubAccInfo : AccInfo->SubAccInfos)
        {
            WHC_ADD_DOWNLOAD_TASK(FileInfos, SubAccInfo->Id, SubAccInfo->GetCachePath(), SubAccInfo->PakUrl, SubAccInfo->PakMd5);
        }
        for (const auto & Component : AccInfo->Self->ComponentArray)
        {
            WHC_ADD_DOWNLOAD_TASK(FileInfos, Component->Id, Component->GetCachePath(1), Component->TopResourceUrl, Component->TopResourceMd5);
            WHC_ADD_DOWNLOAD_TASK(FileInfos, Component->Id, Component->GetCachePath(2), Component->FrontResourceUrl, Component->FrontResourceMd5);
            WHC_ADD_DOWNLOAD_TASK(FileInfos, Component->Id, Component->GetCachePath(3), Component->SideResourceUrl, Component->SideResourceMd5);
        }
        // AddMultiTask当FileInfo的数量为0时会产生一个断言
        if (FileInfos.Num() == 0)
        {
            SetDownloadAccResState(false, Item, AccInfo.Get());
            return;
        }

        TSharedPtr<FDownloadFileSet> NewTask = FArmyDownloadModule::Get().GetDownloadManager()->AddMultiTask(AccInfo->Self->PakUrl, FileInfos);
        NewTask->OnDownloadFinished.BindLambda(
            [this, Id](bool bSucceeded) {
                const TSharedPtr<FCabinetWholeAccInfo> * AccInfoPtr = FWHCModeGlobalData::CabinetAccMap.Find(Id);
                check(AccInfoPtr != nullptr);
                SContentItem * ItemPtr = Container->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_Acc);
                SetDownloadAccResState(bSucceeded, ItemPtr, (*AccInfoPtr).Get());
            }
        );
    }
}

FReply FArmyWHCAccUIBase::Callback_AddAcc()
{
    if (!IsValidSelection())
        return FReply::Handled();

    FString DesireState(TEXT("子附属件型号"));
    if (Container->IsCurrentState(DesireState))
        return FReply::Handled();
    
    Container->ListPanel->ClearListController();
    if (Container->ReplacingSlot->GetWidget() == SNullWidget::NullWidget)
        Container->SetReplacingPanel(Container->ListPanel.ToSharedRef());
    
    Container->ListPanel->SetCurrentState(DesireState);

    Container->ClearReplacingList();
    LoadAccData();

    return FReply::Handled();
}

void FArmyWHCAccUIBase::Callback_DeleteAcc()
{
    if (!IsValidSelection())
        return;

    AXRShapeFrame *ShapeFrame = GetShapeFrame();
    check(ShapeFrame->ShapeInRoomRef != nullptr);
	if (ShapeFrame->ShapeInRoomRef->CabinetActor.IsValid())
	{
		XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = ShapeFrame->ShapeInRoomRef->CabinetActor;
		TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
		if (!Accessory.IsValid())
			return;
		if (Accessory->AttachedSubAccessories.Num() == 0)
			return;

		// 目前附属件下只有一个子附属件
		ShapeFrame->ShapeInRoomRef->DeleteSubAccessory(0);

		SetAddOrReplace(true);
		Container->NotifyAttrPanelNeedRebuild();
	}
}

void FArmyWHCAccUIBase::LoadAccData()
{
    if (Container->CheckNoMoreData())
        return;

    AXRShapeFrame * ShapeFrame = GetShapeFrame();
    check(ShapeFrame->ShapeInRoomRef != nullptr);
	if (!ShapeFrame->ShapeInRoomRef->CabinetActor.IsValid())
		return;
	XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = ShapeFrame->ShapeInRoomRef->CabinetActor;
	TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
    if (!Accessory.IsValid())
        return;
    
    Container->RunHttpRequest(
        MakeQueryUrl(ShapeFrame->ShapeInRoomRef),
        TBaseDelegate<void, const TSharedPtr<FJsonObject>&>::CreateLambda(
            [this](const TSharedPtr<FJsonObject> &ResponseData)
            {
                const TSharedPtr<FJsonObject> &Data = ResponseData->GetObjectField(TEXT("data"));
                if (Data.IsValid())
                {
                    Container->SetReplacingTotalPage(Data->GetNumberField(TEXT("totalPage")));

                    const TArray<TSharedPtr<FJsonValue>>* AccArray = nullptr;
                    if (Data->TryGetArrayField(TEXT("list"), AccArray))
                    {
                        int32 nCount = (*AccArray).Num();
                        int32 IndexStart = Container->GetReplacingIds().Num();
                        for (int32 i = 0; i < nCount; ++i)
                        {
                            const TSharedPtr<FJsonValue> &Value = AccArray->operator[](i);
                            const TSharedPtr<FJsonObject> &AccJsonObject = Value->AsObject();

                            int32 Id = AccJsonObject->GetNumberField(TEXT("id"));
                            Container->AddReplacingId(Id);

                            if (FWHCModeGlobalData::CabinetAccMap.Find(Id) == nullptr)
                            {
                                FCabinetWholeAccInfo * NewAccInfo = new FCabinetWholeAccInfo;
                                NewAccInfo->Deserialize(AccJsonObject);
                                FWHCModeGlobalData::CabinetAccMap.Emplace(Id, MakeShareable(NewAccInfo));
                            }
                        }

                        FillAccList(IndexStart, nCount);
                    }
                }
            }
        )
    );
}

void FArmyWHCAccUIBase::FillAccList(int32 InStart, int32 InCount)
{
    if (!IsValidSelection())
        return;

    AXRShapeFrame * ShapeFrame = GetShapeFrame();
    check(ShapeFrame->ShapeInRoomRef != nullptr);
	if (!ShapeFrame->ShapeInRoomRef->CabinetActor.IsValid())
		return;
	XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = ShapeFrame->ShapeInRoomRef->CabinetActor;
	TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
    if (!Accessory.IsValid())
        return;

    int32 AccId = 0;
    TSharedPtr<FShapeAccessory> &AccessoryRef = Accessory;
    if (bTopCategory)
    {
        AccId = AccessoryRef->CabAccInfo->Id;
    }
    else
    {
        if (AccessoryRef->AttachedSubAccessories.Num() > 0)
            AccId = AccessoryRef->AttachedSubAccessories[0]->CabAccInfo->Id;
    }
    
    int32 MatchingIndex = -1;

    TArray<int32> CabIds = Container->GetReplacingIds();
	for (int32 i = 0; i < InCount; ++i)
	{
        int32 Id = CabIds[InStart + i];
		const TSharedPtr<FCabinetWholeAccInfo> * pAccInfo = FWHCModeGlobalData::CabinetAccMap.Find(Id);
		check(pAccInfo != nullptr);
		const TSharedPtr<FCabinetWholeAccInfo> & AccInfo = *pAccInfo;
		TSharedPtr<SContentItem> Item = SNew(SContentItem)
			.ItemCode(InStart + i)
			.ItemWidth(112.0f)
			.ItemHeight(132.0f)
			.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
			.TextColor(FArmyStyle::Get().GetColor("Color.FF969799"))
			.ThumbnailURL(AccInfo->Self->ThumbnailUrl)
			.ThumbnailImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.DefaultImage_ContentBrowser")))//梁晓菲 默认图
			.bShowDownloadTips(AccInfo->CabRcState != ECabinetResourceState::CabRc_Complete)
			.OnClicked_Raw(this, &FArmyWHCCabAccUI::Callback_ReplacingAccItem)
			.DisplayText(FText::FromString(AccInfo->Self->Name));
		Item->AddMetadata<FWHCModeAttrTypeInfo>(MakeShared<FWHCModeAttrTypeInfo>(EWHCModeAttrType::EWHCAttr_Acc, AccInfo->Self->Id));

		switch (AccInfo->CabRcState)
		{
			case ECabinetResourceState::CabRc_None:
				Item->SetItemState(ENotReady);
				break;
			case ECabinetResourceState::CabRc_FetchingJson:
				Item->SetItemState(EPending);
				break;
			case ECabinetResourceState::CabRc_FetchingRc:
				Item->SetItemState(EDownloading);
				break;
			case ECabinetResourceState::CabRc_Complete:
				Item->SetItemState(EReady);
				break;
		}
		Container->ListPanel->AddItem(Item);

		if (AccId == AccInfo->Self->Id)
			MatchingIndex = InStart + i;
	}

	Container->ListPanel->SetSelectedItem(MatchingIndex);
	Container->ListPanel->Container->ResetScrollNotify();
}

FString FArmyWHCAccUIBase::MakeQueryUrl(FShapeInRoom *InShapeInRoom) const
{
	XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = InShapeInRoom->CabinetActor;
    TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
	TSharedPtr<FShapeAccessory> &AccessoryRef = Accessory;
    int32 CurrentAccType = 0;
    int32 CurrentTopAccId = 0;
    if (bTopCategory)
    {
        CurrentAccType = AccessoryRef->CabAccInfo->Type;
        if (CurrentAccType == (uint8)EMetalsType::MT_WATER_CHANNE || CurrentAccType == (uint8)EMetalsType::MT_BATHROOM_BASIN)
        {
            return FString::Printf(TEXT("design/metals?metalsType=%d&width=%d&current=%d&size=15"), 
                CurrentAccType, 
                FMath::RoundToInt(InShapeInRoom->Shape->GetShapeWidth()),
                Container->GetPageIndexToLoad()
            );
        }
        else if (CurrentAccType == (uint8)EMetalsType::MT_FLUE_GAS_TURBINE)
        {
            if (InShapeInRoom->HasValidShape())
            {
                TPair<TTuple<float,float,float>,TArray<float>> WidthInfo, DepthInfo, HeightInfo;
                if (FSCTShapeUtilityTool::GetSmokeCabinetSpaceRange(InShapeInRoom->Shape.Get(), WidthInfo, DepthInfo, HeightInfo))
                {
                    return FString::Printf(TEXT("design/metals?metalsType=%d&hollowDepth=%d&id=%d&current=%d&size=15"), 
                        CurrentAccType, 
                        FMath::RoundToInt(DepthInfo.Key.Get<2>()),
                        InShapeInRoom->Shape->GetShapeId(),
                        Container->GetPageIndexToLoad()
                    );
                }
                else
                    return FString();    
            }
            else
            {
                return FString::Printf(TEXT("design/metals?metalsType=%d&current=%d&size=15"), CurrentAccType, Container->GetPageIndexToLoad());
            }
        }
        else
        {
            return FString::Printf(TEXT("design/metals?metalsType=%d&current=%d&size=15"), 
                CurrentAccType, 
                Container->GetPageIndexToLoad()
            );
        }
    }
    else
    {
        if (AccessoryRef->AttachedSubAccessories.Num() > 0)
            CurrentAccType = AccessoryRef->AttachedSubAccessories[0]->CabAccInfo->Type;
        else
        {
            switch ((EMetalsType)AccessoryRef->CabAccInfo->Type)
            {
                case EMetalsType::MT_WATER_CHANNE:
                    CurrentAccType = (int32)EMetalsType::MT_BIBCOCK;
                    break;
                case EMetalsType::MT_BATHROOM_BASIN:
                    CurrentAccType = (int32)EMetalsType::MT_BATHROOM_TAP;
                    break;
                case EMetalsType::MT_KITCHEN_RANGE:
                    CurrentAccType = (int32)EMetalsType::MT_FLUE_GAS_TURBINE;
                    break;
            }
        }

        switch ((EMetalsType)AccessoryRef->CabAccInfo->Type)
        {
            case EMetalsType::MT_WATER_CHANNE:
                CurrentTopAccId = AccessoryRef->CabAccInfo->Id;
                break;
            case EMetalsType::MT_BATHROOM_BASIN:
                CurrentTopAccId = AccessoryRef->CabAccInfo->Id;
                break;
        }

        return FString::Printf(TEXT("design/metals?metalsType=%d&id=%d&current=%d&size=15"), 
            CurrentAccType, 
            CurrentTopAccId, 
            Container->GetPageIndexToLoad()
        );
    }
}

void FArmyWHCAccUIBase::SetDownloadAccResState(bool bSucceeded, SContentItem *UIItem, FCabinetWholeAccInfo *DataItem)
{
    if (UIItem != nullptr)
        UIItem->SetItemState(bSucceeded ? EReady : ENotReady);
    if (DataItem != nullptr)
        DataItem->CabRcState = bSucceeded ? ECabinetResourceState::CabRc_Complete : ECabinetResourceState::CabRc_None;
}

bool FArmyWHCCabAccUI::IsValidSelection() const
{
    return Container->IsWHCItemSelected<AXRShapeFrame>();
}

AXRShapeFrame* FArmyWHCCabAccUI::GetShapeFrame() const
{
    return Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
}

bool FArmyWHCAccUI::IsValidSelection() const
{
    return Container->IsWHCItemSelected<AXRElecDeviceActor>();
}

AXRShapeFrame* FArmyWHCAccUI::GetShapeFrame() const
{
    return Container->CabinetOperationRef->GetShapeFrameFromSelected();
}

void FArmyWHCAccUI::AfterAccReplaced(FShapeInRoom *InShapeInRoom, FCabinetWholeAccInfo *InAccInfo)
{
	if (InShapeInRoom->CabinetActor.IsValid())
	{
		XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = InShapeInRoom->CabinetActor;
		TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
		if (Accessory.IsValid()) {
			if (IsTopCategory())
			{
				FArmyWHCabinetMgr::Get()->OnRefreshShape((int32)ECabinetType::EType_OnGroundCab);

				AActor * Selected = Accessory->Actor;
				Container->LastWHCItemSelected = Selected;
				Container->CabinetOperationRef->UpdateLastSelected(Selected);
			}
			else
			{
				if (!IsCanDelete())
				{
					AActor * Selected = Accessory->AttachedSubAccessories[0]->Actor;
					Container->LastWHCItemSelected = Selected;
					Container->CabinetOperationRef->UpdateLastSelected(Selected);
				}
			}
		}
	}
}

void FArmyWHCVentilatorUI::ReplacingAccItemInternal(int32 InItemCode, FShapeInRoom *InShapeInRoom, FCabinetWholeAccInfo *InAccInfo)
{
    Container->RunHttpRequest(
        FString::Printf(TEXT("design/cabinets/%d/hollowValue?metalsId=%d&width=%d&depth=%d"), 
            InShapeInRoom->Shape->GetShapeId(), InAccInfo->Self->Id,
            FMath::RoundToInt(InShapeInRoom->Shape->GetShapeWidth()), 
            FMath::RoundToInt(InShapeInRoom->Shape->GetShapeDepth())),
        TBaseDelegate<void, const TSharedPtr<FJsonObject>&>::CreateLambda(
            [this, InItemCode, InShapeInRoom, InAccInfo](const TSharedPtr<FJsonObject> &ResponseData)
            {
                const TSharedPtr<FJsonObject> &Data = ResponseData->GetObjectField(TEXT("data"));
                if (Data.IsValid())
                {
                    const TArray<TSharedPtr<FJsonValue>> &WidthIntervals = Data->GetArrayField(TEXT("enableCabinetsWidth"));
                    TArray<float> fWidthIntervals;
                    for (const auto &WidthInterval : WidthIntervals)
                        fWidthIntervals.Emplace(WidthInterval->AsNumber());
                    InShapeInRoom->AddOrSetParam(PARAM_NAME_CAB_WIDTH_INTERVALS, MoveTemp(fWidthIntervals));

                    const TArray<TSharedPtr<FJsonValue>> &DepthIntervals = Data->GetArrayField(TEXT("enableCabinetsDepth"));
                    TArray<float> fDepthIntervals;
                    for (const auto &DepthInterval : DepthIntervals)
                        fDepthIntervals.Emplace(DepthInterval->AsNumber());
                    InShapeInRoom->AddOrSetParam(PARAM_NAME_CAB_DEPTH_INTERVALS, MoveTemp(fDepthIntervals));

                    const TArray<TSharedPtr<FJsonValue>> &VentilatorWidthIntervals = Data->GetArrayField(TEXT("hollowStandardWidth"));
                    TArray<float> fVentilatorWidthIntervals;
                    for (const auto &VentilatorWidthInterval : VentilatorWidthIntervals)
                        fVentilatorWidthIntervals.Emplace(VentilatorWidthInterval->AsNumber());
                    InShapeInRoom->AddOrSetParam(PARAM_NAME_CAB_VENTILATOR_WIDTH_INTERVALS, MoveTemp(fVentilatorWidthIntervals));

                    InShapeInRoom->AddOrSetParam(PARAM_NAME_CAB_MAX_VENTILATOR_WIDTH, static_cast<float>(Data->GetNumberField(TEXT("maxHollowWidth"))));
                    InShapeInRoom->AddOrSetParam(PARAM_NAME_CAB_MIN_VENTILATOR_WIDTH, static_cast<float>(Data->GetNumberField(TEXT("minHollowWidth"))));

                    FArmyWHCAccUIBase::ReplacingAccItemInternal(InItemCode, InShapeInRoom, InAccInfo);
                }
            }
        ),
        TBaseDelegate<void, EHttpErrorReason>::CreateLambda(
            [](EHttpErrorReason ErrorReason)
            {
                if (ErrorReason == EHttpErrorReason::EReason_HttpCodeError)
                    GGI->Window->ShowMessage(MT_Warning, TEXT("网络错误"));
                else if (ErrorReason == EHttpErrorReason::EReason_NoValidData)
                    GGI->Window->ShowMessage(MT_Warning, TEXT("未获取到内空数据"));
            }
        )
    );
}