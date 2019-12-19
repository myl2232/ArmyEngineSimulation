#include "UIMixin/SArmyWHCSlidingwayUI.h"
#include "WHCMode/SArmyWHCModeAttrPanel.h"
#include "WHCMode/SArmyWHCModeListPanel.h"
#include "WHCMode/XRShapeFrameActor.h"
#include "WHCMode/XRWHCabinet.h"
#include "WHCMode/XRWHCabinetAutoMgr.h"
#include "WHCMode/XRWHCCabinetChangeList.h"

#include "ArmyStyle.h"
#include "Actor/XRActorConstant.h"
#include "SContentItem.h"
#include "SScrollWrapBox.h"
#include "ArmySlateModule.h"
#include "ArmyDownloadModule.h"
#include "Data/WHCModeData/XRWHCModeData.h"

#include "SCTShape.h"
#include "UtilityTools/SCTShapeUtilityTools.h"

#include "WebImageCache.h"

TSharedPtr<SWidget> FArmyWHCSlidingwayUI::MakeWidget()
{
    TSharedPtr<SArmyWHCModeReplacingWidget> SlidingwayReplacing;
    SAssignNew(SlidingwayReplacing, SArmyWHCModeReplacingWidget)
        .ButtonImage(FArmyStyle::Get().GetBrush("WHCMode.Replace"))
        .OnWholeWidgetClicked_Raw(this, &FArmyWHCSlidingwayUI::Callback_ReplacingSlidingway)
        .OnButtonClicked_Raw(this, &FArmyWHCSlidingwayUI::Callback_ReplacingSlidingway);
    SlidingwayReplacing->SetReplacingItemThumbnail(TAttribute<const FSlateBrush*>::Create(
        [this]() -> const FSlateBrush* {
            if (Container->IsWHCItemSelected<AXRShapeFrame>())
            { 
                AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
                check(ShapeFrame->ShapeInRoomRef != nullptr);

                TArray<FSCTShapeUtilityTool::FSlidwayInfo> SlidingwayInfos;
                FSCTShapeUtilityTool::GetCabinetSlidingDoorSlidways(ShapeFrame->ShapeInRoomRef->Shape.Get(), SlidingwayInfos);
                if (SlidingwayInfos.Num() > 0)
                {
                    for (const auto &SlidingwayInfo : SlidingwayInfos)
                    {
                        if (SlidingwayInfo.bIsUsed)
                            return FArmySlateModule::Get().WebImageCache->Download(SlidingwayInfo.Thumdurl)->GetBrush();
                    }
                }
            }
            return FCoreStyle::Get().GetDefaultBrush();
        }
    ));
    SlidingwayReplacing->SetReplacingItemText(TAttribute<FText>::Create(
        [this]() -> FText {
            if (Container->IsWHCItemSelected<AXRShapeFrame>())
            {
                AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
                check(ShapeFrame->ShapeInRoomRef != nullptr);

                TArray<FSCTShapeUtilityTool::FSlidwayInfo> SlidingwayInfos;
                FSCTShapeUtilityTool::GetCabinetSlidingDoorSlidways(ShapeFrame->ShapeInRoomRef->Shape.Get(), SlidingwayInfos);
                if (SlidingwayInfos.Num() > 0)
                {
                    for (const auto &SlidingwayInfo : SlidingwayInfos)
                    {
                        if (SlidingwayInfo.bIsUsed)
                            return FText::FromString(SlidingwayInfo.DisplayName);
                    }
                }
            }
            return FText();
        }
    ));
    return SlidingwayReplacing;
}

void FArmyWHCSlidingwayUI::LoadListData()
{
    if (!Container->IsWHCItemSelected<AXRShapeFrame>())
        return;
    LoadSlidingwayData();
}

void FArmyWHCSlidingwayUI::Callback_ReplacingSlidingway()
{
    if (!Container->IsWHCItemSelected<AXRShapeFrame>())
        return;

    FString DesireState(TEXT("导轨型号"));
    if (Container->IsCurrentState(DesireState))
        return;

    Container->ListPanel->ClearListController();
    if (Container->ReplacingSlot->GetWidget() == SNullWidget::NullWidget)
        Container->SetReplacingPanel(Container->ListPanel.ToSharedRef());
    
    Container->SetCurrentState(DesireState);
    
    Container->ClearReplacingList();
    LoadSlidingwayData();
}

void FArmyWHCSlidingwayUI::Callback_ReplacingSlidingwayItem(int32 InItemCode)
{
    if (!Container->CheckIsValidReplacingIndex(InItemCode))
        return;

    if (!Container->IsWHCItemSelected<AXRShapeFrame>())
        return;

    AXRShapeFrame * ShapeFrame = Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
    check(ShapeFrame->ShapeInRoomRef != nullptr);

    int32 Id = Container->GetReplacingIds()[InItemCode];
    const TSharedPtr<FCabinetWholeAccInfo> * AccInfoPtr = FWHCModeGlobalData::CabinetAccMap.Find(Id);
    check(AccInfoPtr != nullptr);
    const TSharedPtr<FCabinetWholeAccInfo> & AccInfo = *AccInfoPtr;
    if (AccInfo->CabRcState == ECabinetResourceState::CabRc_Complete)
    {
        FSCTShapeUtilityTool::ChangeCabinetSlidingDoorSlidways(ShapeFrame->ShapeInRoomRef->Shape.Get(), AccInfo->Self->Id);
        ShapeFrame->ShapeInRoomRef->MakeImmovable();
        FSCTShapeUtilityTool::SetActorTag(ShapeFrame, XRActorTag::WHCActor);
        Container->ListPanel->SetSelectedItem(InItemCode);

        FArmyWHCSlidingwayChange *SlidingwayChange = nullptr;
        for (const auto &Change : ShapeFrame->ShapeInRoomRef->ChangeList)
        {
            if (Change->IsSameType(SHAPE_CHANGE_SLIDINGWAY))
            {
                SlidingwayChange = static_cast<FArmyWHCSlidingwayChange*>(Change.Get());
                break;
            }
        }
        if (SlidingwayChange == nullptr)
        {
            SlidingwayChange = new FArmyWHCSlidingwayChange(ShapeFrame->ShapeInRoomRef->Shape.Get());
            ShapeFrame->ShapeInRoomRef->ChangeList.Emplace(MakeShareable(SlidingwayChange));
        }
        SlidingwayChange->SetSlidingwayId(AccInfo->Self->Id);
    }
}

void FArmyWHCSlidingwayUI::LoadSlidingwayData()
{
    if (Container->CheckNoMoreData())
        return;

    AXRShapeFrame * ShapeFrame = Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
    check(ShapeFrame->ShapeInRoomRef != nullptr);

    Container->RunHttpRequest(
        FString::Printf(TEXT("design/metals/search?metalsType=1&current=%d&size=15"), 
            Container->GetPageIndexToLoad()
        ),
        TBaseDelegate<void, const TSharedPtr<FJsonObject>&>::CreateLambda(
            [this, ShapeFrame](const TSharedPtr<FJsonObject> &ResponseData)
            {
                const TSharedPtr<FJsonObject> &Data = ResponseData->GetObjectField(TEXT("data"));
                if (Data.IsValid())
                {
                    Container->SetReplacingTotalPage(Data->GetNumberField(TEXT("totalPage")));

                    const TArray<TSharedPtr<FJsonValue>>* AccArray = nullptr;
                    if (Data->TryGetArrayField(TEXT("list"), AccArray))
                    {
                        int32 nCount = (*AccArray).Num();
                        int32 nRealCount = 0;
                        int32 IndexStart = Container->GetReplacingIds().Num();

                        TArray<FSCTShapeUtilityTool::FSlidwayInfo> SlidingwayInfos;
                        FSCTShapeUtilityTool::GetCabinetSlidingDoorSlidways(ShapeFrame->ShapeInRoomRef->Shape.Get(), SlidingwayInfos);

                        for (int32 i = 0; i < nCount; ++i)
                        {
                            const TSharedPtr<FJsonValue> &Value = AccArray->operator[](i);
                            const TSharedPtr<FJsonObject> &AccJsonObject = Value->AsObject();

                            int32 Id = AccJsonObject->GetNumberField(TEXT("id"));
                            if (SlidingwayInfos.FindByPredicate([Id](const FSCTShapeUtilityTool::FSlidwayInfo &InData) -> bool {
                                return InData.Id == Id;
                            }))
                            {
                                Container->AddReplacingId(Id);
                                ++nRealCount;

                                if (FWHCModeGlobalData::CabinetAccMap.Find(Id) == nullptr)
                                {
                                    FCabinetWholeAccInfo * NewAccInfo = new FCabinetWholeAccInfo;
                                    NewAccInfo->Deserialize(AccJsonObject);
                                    NewAccInfo->CabRcState = ECabinetResourceState::CabRc_Complete;
                                    FWHCModeGlobalData::CabinetAccMap.Emplace(Id, MakeShareable(NewAccInfo));
                                }
                            }
                            
                        }

                        FillSlidingwayList(IndexStart, nRealCount);
                    }
                }
            }
        )
    );
}

void FArmyWHCSlidingwayUI::FillSlidingwayList(int32 InStart, int32 InCount)
{
    if (!Container->IsWHCItemSelected<AXRShapeFrame>())
        return;

    AXRShapeFrame * ShapeFrame = Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
    check(ShapeFrame->ShapeInRoomRef != nullptr);

    int32 AccId = 0;
    TArray<FSCTShapeUtilityTool::FSlidwayInfo> SlidingwayInfos;
    FSCTShapeUtilityTool::GetCabinetSlidingDoorSlidways(ShapeFrame->ShapeInRoomRef->Shape.Get(), SlidingwayInfos);
    for (const auto &SlidingwayInfo : SlidingwayInfos)
    {
        if (SlidingwayInfo.bIsUsed)
        {
            AccId = SlidingwayInfo.Id;
            break;
        }
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
			.OnClicked_Raw(this, &FArmyWHCSlidingwayUI::Callback_ReplacingSlidingwayItem)
			.DisplayText(FText::FromString(AccInfo->Self->Name));
		// Item->AddMetadata<FWHCModeAttrTypeInfo>(MakeShared<FWHCModeAttrTypeInfo>(EWHCModeAttrType::EWHCAttr_Acc, AccInfo->Self->Id));
		Item->SetItemState(EReady);
		Container->ListPanel->AddItem(Item);

		if (AccId == AccInfo->Self->Id)
			MatchingIndex = InStart + i;
	}

	Container->ListPanel->SetSelectedItem(MatchingIndex);
	Container->ListPanel->Container->ResetScrollNotify();
}