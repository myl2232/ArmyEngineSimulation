#include "UIMixin/SArmyWHCDecorationBoardUI.h"
#include "WHCMode/SArmyWHCModeAttrPanel.h"
#include "WHCMode/SArmyWHCModeListPanel.h"
#include "WHCMode/XRShapeFrameActor.h"
#include "WHCMode/XRWHCabinet.h"
#include "WHCMode/XRWHCModeCabinetOperation.h"

#include "ArmyStyle.h"
#include "Actor/XRActorConstant.h"
#include "Game/XRGameInstance.h"
#include "SContentItem.h"
#include "SScrollWrapBox.h"
#include "ArmySlateModule.h"
#include "ArmyDownloadModule.h"
#include "Data/WHCModeData/XRWHCModeData.h"

#include "SCTResManager.h"
#include "MaterialManager.h"
#include "UtilityTools/SCTShapeUtilityTools.h"

#include "WebImageCache.h"

TSharedPtr<SWidget> FArmyWHCDecorationBoardUI::MakeWidget()
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
            .OnClicked_Raw(this, &FArmyWHCDecorationBoardUI::Callback_AddDecorationBoard)
            [
                SNew(SImage)
                .Image(FArmyStyle::Get().GetBrush("Icon.Add"))
            ]
        ]; 
    }
    else
    {
        TSharedPtr<SArmyWHCModeReplacingWidget> DecorationBoardReplacing;
        SAssignNew(DecorationBoardReplacing, SArmyWHCModeReplacingWidget)
            .ButtonImage(FArmyStyle::Get().GetBrush("Icon.Delete"))
            .OnWholeWidgetClicked_Raw(this, &FArmyWHCDecorationBoardUI::Callback_ReplacingDecorationBoardMtl)
            .OnButtonClicked_Raw(this, &FArmyWHCDecorationBoardUI::Callback_DeleteDecorationBoard);
        DecorationBoardReplacing->SetReplacingItemThumbnail(TAttribute<const FSlateBrush*>::Create(
            [this]() -> const FSlateBrush* {
                AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
                if (ShapeFrame != nullptr)
                {
                    // AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
                    check(ShapeFrame->ShapeInRoomRef != nullptr);
                    for (const auto &BoardInfo : ShapeFrame->ShapeInRoomRef->DecBoardInfos)
                    {
                        if (Direction == BoardInfo.Face)
                        {
                            const TSharedPtr<FMtlInfo> *MtlInfoPtr = FWHCModeGlobalData::CabinetMtlMap.Find(BoardInfo.MtlId);
                            if (MtlInfoPtr != nullptr)
                                return FArmySlateModule::Get().WebImageCache->Download((*MtlInfoPtr)->MtlThumbnailUrl)->GetBrush();
                        }
                    }
                }
                return FCoreStyle::Get().GetDefaultBrush();
            }
        ));
        DecorationBoardReplacing->SetReplacingItemText(TAttribute<FText>::Create(
            [this]() -> FText {
                AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
                if (ShapeFrame != nullptr)
                {
                    // AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
                    check(ShapeFrame->ShapeInRoomRef != nullptr);
                    for (const auto &BoardInfo : ShapeFrame->ShapeInRoomRef->DecBoardInfos)
                    {
                        if (Direction == BoardInfo.Face)
                        {
                            const TSharedPtr<FMtlInfo> *MtlInfoPtr = FWHCModeGlobalData::CabinetMtlMap.Find(BoardInfo.MtlId);
                            if (MtlInfoPtr != nullptr)
                                return FText::FromString((*MtlInfoPtr)->MtlName);
                        }
                    }
                }
                return FText();
            }
        ));
        return MoveTemp(DecorationBoardReplacing);
    }
}

void FArmyWHCDecorationBoardUI::LoadListData()
{
    if (!Container->IsWHCItemSelected<AXRShapeFrame>())
        return;
    LoadMtlData();
}

void FArmyWHCDecorationBoardUI::Callback_ReplacingDecorationBoardMtl()
{
    if (!Container->IsWHCItemSelected<AXRShapeFrame>())
        return;

    FString DesireState(TEXT("见光板材质"));
    if (Container->IsCurrentState(DesireState))
        return;   

    Container->ListPanel->ClearListController();
    if (Container->ReplacingSlot->GetWidget() == SNullWidget::NullWidget)
        Container->SetReplacingPanel(Container->ListPanel.ToSharedRef());

    Container->SetCurrentState(DesireState);

    Container->ClearReplacingList();
    LoadMtlData();
}

void FArmyWHCDecorationBoardUI::Callback_ReplacingDecorationBoardMtlItem(int32 InItemCode)
{
    if (!Container->CheckIsValidReplacingIndex(InItemCode))
        return;

    if (!Container->IsWHCItemSelected<AXRShapeFrame>())
        return;

    int32 Id = Container->GetReplacingIds()[InItemCode];
    const TSharedPtr<FMtlInfo> * CabinetMtlPtr = FWHCModeGlobalData::CabinetMtlMap.Find(Id);
    check(CabinetMtlPtr != nullptr);
    const TSharedPtr<FMtlInfo> & CabinetMtl = *CabinetMtlPtr;
    if (CabinetMtl->RcState == ECabinetResourceState::CabRc_None)
    {
        FString CachePath = FSCTShapeUtilityTool::GetFullCachePathByMaterialTypeAndID(EMaterialType::Board_Material, Id);

        SContentItem * Item = Container->GetListItem(InItemCode);
        if (Item)
            Item->SetItemState(EDownloading);
        CabinetMtl->RcState = ECabinetResourceState::CabRc_FetchingRc;

        TArray<FDownloadSpace::FDownloadFileInfo> FileInfos;
        WHC_ADD_DOWNLOAD_TASK(FileInfos, Id, CachePath, CabinetMtl->MtlUrl, CabinetMtl->MtlMd5);

        if (FileInfos.Num() == 0)
        {
            SetDownloadMtlResState(false, Item, CabinetMtl.Get());
            return;
        }

        TSharedPtr<FDownloadFileSet> NewTask = FArmyDownloadModule::Get().GetDownloadManager()->AddMultiTask(CabinetMtl->MtlUrl, FileInfos);
        NewTask->OnDownloadFinished.BindLambda(
            [this](bool bSucceeded, FString InCachePath, int32 Id)
            {
                SContentItem * Item = Container->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_CabMtl);
                const TSharedPtr<FMtlInfo> * CabinetMtlPtr = FWHCModeGlobalData::CabinetMtlMap.Find(Id);
                check(CabinetMtlPtr != nullptr);
                const TSharedPtr<FMtlInfo> & CabinetMtl = *CabinetMtlPtr;

                if (bSucceeded)
                {
                    CabinetMtl->Mtl = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(InCachePath, CabinetMtl->MtlParam);
                    if (CabinetMtl->Mtl != nullptr)
                    {
                        CabinetMtl->Mtl->AddToRoot();
                        SetDownloadMtlResState(true, Item, CabinetMtl.Get());
                        return;
                    }
                }

                SetDownloadMtlResState(false, Item, CabinetMtl.Get());
            },
            CachePath,
            Id
        );
    }
    else if (CabinetMtl->RcState == ECabinetResourceState::CabRc_Complete)
    {
        AXRShapeFrame * ShapeFrame = Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
        check(ShapeFrame->ShapeInRoomRef != nullptr);

        if (Container->IsClickedSameReplacingItem(InItemCode))
            return;
        ShapeFrame->ShapeInRoomRef->ChangeSunBoardMtl(CabinetMtl.Get());
        Container->ListPanel->SetSelectedItem(InItemCode);
    }
    
}

FReply FArmyWHCDecorationBoardUI::Callback_AddDecorationBoard()
{
    if (!Container->IsWHCItemSelected<AXRShapeFrame>())
        return FReply::Unhandled();
    AXRShapeFrame * ShapeFrame = Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
    check(ShapeFrame->ShapeInRoomRef != nullptr);
    if (ShapeFrame->ShapeInRoomRef->SetSunBoard(Direction))
    {
        // 替换成功后需要更新见光板
        ShapeFrame->ShapeInRoomRef->SpawnPlatform();
        ShapeFrame->ShapeInRoomRef->MakeImmovable();
        FSCTShapeUtilityTool::SetActorTag(ShapeFrame, XRActorTag::WHCActor);
        Container->NotifyAttrPanelNeedRebuild();
    }
    else
    {
        GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("见光板和物件重合不能添加见光板"));
    }
    
    return FReply::Handled();
}

void FArmyWHCDecorationBoardUI::Callback_DeleteDecorationBoard()
{
    if (!Container->IsWHCItemSelected<AXRShapeFrame>())
        return;
    AXRShapeFrame * ShapeFrame = Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
    check(ShapeFrame->ShapeInRoomRef != nullptr);
    ShapeFrame->ShapeInRoomRef->UnsetSunBoard(Direction);
    // 替换成功后需要更新见光板
    ShapeFrame->ShapeInRoomRef->SpawnPlatform();
    Container->NotifyAttrPanelNeedRebuild();
}

void FArmyWHCDecorationBoardUI::LoadMtlData()
{
    if (Container->CheckNoMoreData())
        return;

    AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
    check(ShapeFrame->ShapeInRoomRef != nullptr);

    Container->RunHttpRequest(
        FString::Printf(TEXT("design/materials/list?materialType=1&current=%d&size=15"), Container->GetPageIndexToLoad()),
        TBaseDelegate<void, const TSharedPtr<FJsonObject>&>::CreateLambda(
            [this](const TSharedPtr<FJsonObject> &ResponseData)
            {
                const TSharedPtr<FJsonObject> &Data = ResponseData->GetObjectField(TEXT("data"));
                if (Data.IsValid())
                {
                    Container->SetReplacingTotalPage(Data->GetNumberField(TEXT("totalPage")));
                
                    const TArray<TSharedPtr<FJsonValue>>* MtlArray = nullptr;
                    if (Data->TryGetArrayField(TEXT("list"), MtlArray))
                    {
                        int32 nCount = (*MtlArray).Num();
                        int32 IndexStart = Container->GetReplacingIds().Num();
                        for (int32 i = 0; i < nCount; ++i)
                        {
                            const TSharedPtr<FJsonValue> &Value = MtlArray->operator[](i);
                            const TSharedPtr<FJsonObject> &MtlJsonObject = Value->AsObject();

                            int32 Id = MtlJsonObject->GetNumberField(TEXT("id"));
                            Container->AddReplacingId(Id);

                            if (FWHCModeGlobalData::CabinetMtlMap.Find(Id) == nullptr)
                            {
                                FMtlInfo * NewMtlInfo = new FMtlInfo;
                                NewMtlInfo->Id = Id;
                                NewMtlInfo->MtlName = MtlJsonObject->GetStringField(TEXT("name"));
                                NewMtlInfo->MtlThumbnailUrl = MtlJsonObject->GetStringField(TEXT("thumbnailUrl"));
                                NewMtlInfo->MtlUrl = MtlJsonObject->GetStringField(TEXT("pakUrl"));
                                NewMtlInfo->MtlMd5 = MtlJsonObject->GetStringField(TEXT("pakMd5"));
                                NewMtlInfo->MtlParam = MtlJsonObject->GetStringField(TEXT("optimizeParam"));
                                FWHCModeGlobalData::CabinetMtlMap.Emplace(Id, MakeShareable(NewMtlInfo));
                            }
                        }
                        FillMtlList(IndexStart, nCount);
                    }
                }
            }
        )
    );
}

void FArmyWHCDecorationBoardUI::FillMtlList(int32 InStart, int32 InCount)
{
    if (!Container->IsWHCItemSelected<AXRShapeFrame>())
        return;

    AXRShapeFrame * ShapeFrame = Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
    check(ShapeFrame->ShapeInRoomRef != nullptr);

    int32 MtlId = -1;
    for (const auto &BoardInfo : ShapeFrame->ShapeInRoomRef->DecBoardInfos)
    {
        if (Direction == BoardInfo.Face)
        {
            MtlId = BoardInfo.MtlId;
            break;
        }
    }

    int32 MatchingIndex = -1;
    TArray<int32> CabIds = Container->GetReplacingIds();
    for (int32 i = 0; i < InCount; ++i)
	{
        const TSharedPtr<FMtlInfo> * CabMtlInfoPtr = FWHCModeGlobalData::CabinetMtlMap.Find(CabIds[InStart + i]);
        check(CabMtlInfoPtr != nullptr);
        const TSharedPtr<FMtlInfo> & CabMtlInfo = *CabMtlInfoPtr;
		TSharedPtr<SContentItem> Item = SNew(SContentItem)
            .ItemCode(InStart + i)
            .ItemWidth(112.0f)
            .ItemHeight(132.0f)
			.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
			.TextColor(FArmyStyle::Get().GetColor("Color.FF969799"))
			.ThumbnailURL(CabMtlInfo->MtlThumbnailUrl)
			.ThumbnailImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.DefaultImage_ContentBrowser")))//梁晓菲 默认图
			.bShowDownloadTips(CabMtlInfo->Mtl == nullptr)
			.OnClicked_Raw(this, &FArmyWHCDecorationBoardUI::Callback_ReplacingDecorationBoardMtlItem)
			.DisplayText(FText::FromString(CabMtlInfo->MtlName));
        Item->AddMetadata<FWHCModeAttrTypeInfo>(MakeShared<FWHCModeAttrTypeInfo>(EWHCModeAttrType::EWHCAttr_CabMtl, CabMtlInfo->Id));
	
        switch (CabMtlInfo->RcState)
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

        if (CabMtlInfo->Id == MtlId)
            MatchingIndex = InStart + i;
	}

    Container->ListPanel->SetSelectedItem(MatchingIndex);
	Container->ListPanel->Container->ResetScrollNotify();
}

void FArmyWHCDecorationBoardUI::SetDownloadMtlResState(bool bSucceeded, SContentItem *UIItem, FMtlInfo *DataItem)
{
    if (UIItem != nullptr)
        UIItem->SetItemState(bSucceeded ? EReady : ENotReady);
    if (DataItem != nullptr)
        DataItem->RcState = bSucceeded ? ECabinetResourceState::CabRc_Complete : ECabinetResourceState::CabRc_None;
}