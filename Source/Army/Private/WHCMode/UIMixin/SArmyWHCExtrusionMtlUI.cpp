#include "UIMixin/SArmyWHCExtrusionMtlUI.h"
#include "SArmyWHCModeAttrPanel.h"
#include "SArmyWHCModeListPanel.h"
#include "ArmyWHCabinet.h"
#include "ArmyWHCabinetAutoMgr.h"
#include "ArmyShapeFrameActor.h"

#include "ArmyStyle.h"
#include "SContentItem.h"
#include "SScrollWrapBox.h"
#include "ArmySlateModule.h"
#include "ArmyDownloadModule.h"
#include "Data/WHCModeData/XRWHCModeData.h"

#include "SCTResManager.h"

#include "WebImageCache.h"

TSharedPtr<SWidget> FArmyWHCExtrusionMtlUI::MakeWidget()
{
    int32 EType = ExtrusionType;

    TSharedPtr<SArmyWHCModeReplacingWidget> ExtrusionMtlReplacing;
    SAssignNew(ExtrusionMtlReplacing, SArmyWHCModeReplacingWidget)
        .ButtonImage(FArmyStyle::Get().GetBrush("WHCMode.Replace"))
        .OnWholeWidgetClicked_Raw(this, &FArmyWHCExtrusionMtlUI::Callback_ReplacingExtrusionMtl)
        .OnButtonClicked_Raw(this, &FArmyWHCExtrusionMtlUI::Callback_ReplacingExtrusionMtl);
    ExtrusionMtlReplacing->SetReplacingItemThumbnail(TAttribute<const FSlateBrush*>::Create(
        [this, EType]() -> const FSlateBrush* {
            if (Container->IsWHCItemSelected<AXRShapeTableActor>())
            {
                switch (EType)
                {
                    case 2:
                    {
                        FTableInfo* pInfo = FArmyWHCabinetMgr::Get()->GetToeInfo();
                        if (pInfo != nullptr)
                        {
                            const TSharedPtr<FMtlInfo> * MtlInfoPtr = FWHCModeGlobalData::ToeMtlMap.Find(pInfo->mtlId);
                            if (MtlInfoPtr != nullptr)
                            {
                                const TSharedPtr<FMtlInfo> & MtlInfo = *MtlInfoPtr;
                                return FArmySlateModule::Get().WebImageCache->Download(MtlInfo->MtlThumbnailUrl)->GetBrush();
                            }
                        }
                        break;
                    }
                    case 3:
                    {
                        FTableInfo* pInfo = FArmyWHCabinetMgr::Get()->GetTopInfo();
                        if (pInfo != nullptr)
                        {
                            const TSharedPtr<FMtlInfo> * MtlInfoPtr = FWHCModeGlobalData::TopMtlMap.Find(pInfo->mtlId);
                            if (MtlInfoPtr != nullptr)
                            {
                                const TSharedPtr<FMtlInfo> & MtlInfo = *MtlInfoPtr;
                                return FArmySlateModule::Get().WebImageCache->Download(MtlInfo->MtlThumbnailUrl)->GetBrush();
                            }
                        }
                        break;
                    }
                }             
            }
            return FCoreStyle::Get().GetDefaultBrush();
        }
    ));
    ExtrusionMtlReplacing->SetReplacingItemText(TAttribute<FText>::Create(
        [this, EType]() -> FText {
            if (Container->IsWHCItemSelected<AXRShapeTableActor>())
            {
                switch (EType)
                {
                    case 2:
                    {
                        FTableInfo* pInfo = FArmyWHCabinetMgr::Get()->GetToeInfo();
                        if (pInfo != nullptr)
                        {
                            const TSharedPtr<FMtlInfo> * MtlInfoPtr = FWHCModeGlobalData::ToeMtlMap.Find(pInfo->mtlId);
                            if (MtlInfoPtr != nullptr)
                            {
                                const TSharedPtr<FMtlInfo> & MtlInfo = *MtlInfoPtr;
                                return FText::FromString(MtlInfo->MtlName);
                            }
                        }
                        break;
                    }
                    case 3:
                    {
                        FTableInfo* pInfo = FArmyWHCabinetMgr::Get()->GetTopInfo();
                        if (pInfo != nullptr)
                        {
                            const TSharedPtr<FMtlInfo> * MtlInfoPtr = FWHCModeGlobalData::TopMtlMap.Find(pInfo->mtlId);
                            if (MtlInfoPtr != nullptr)
                            {
                                const TSharedPtr<FMtlInfo> & MtlInfo = *MtlInfoPtr;
                                return FText::FromString(MtlInfo->MtlName);
                            }
                        }
                        break;
                    }
                }            
            }
            return FText();
        }
    ));
    return ExtrusionMtlReplacing;
}

void FArmyWHCExtrusionMtlUI::LoadListData()
{
    if (!Container->IsWHCItemSelected<AXRShapeTableActor>())
        return;
    LoadExtrusionMtlData();
}

void FArmyWHCExtrusionMtlUI::Callback_ReplacingExtrusionMtl()
{
    if (!Container->IsWHCItemSelected<AXRShapeTableActor>())
        return;

    FString DesireState;
    switch (ExtrusionType)
    {
        case 2:
            DesireState = TEXT("踢脚板材质");
            break;
        case 3:
            DesireState = TEXT("上线条材质");
            break;
    }
    if (Container->IsCurrentState(DesireState))
        return;

    Container->ListPanel->ClearListController();
    if (Container->ReplacingSlot->GetWidget() == SNullWidget::NullWidget)
        Container->SetReplacingPanel(Container->ListPanel.ToSharedRef());
    
    Container->ListPanel->SetCurrentState(DesireState);

    Container->ClearReplacingList();
    LoadExtrusionMtlData();
}

void FArmyWHCExtrusionMtlUI::Callback_ReplacingExtrusionMtlItem(int32 InItemCode)
{
    if (!Container->CheckIsValidReplacingIndex(InItemCode))
        return;
    if (!Container->IsWHCItemSelected<AXRShapeTableActor>())
        return;

    int32 Id = Container->GetReplacingIds()[InItemCode];
    FWHCModeGlobalData::TMtlMap *TableMtlMapPtr = nullptr;
    switch (ExtrusionType)
    {
        case 2:
            TableMtlMapPtr = &FWHCModeGlobalData::ToeMtlMap;
            break;
        case 3:
            TableMtlMapPtr = &FWHCModeGlobalData::TopMtlMap;
            break;
    }
    check(TableMtlMapPtr != nullptr);

    const TSharedPtr<FMtlInfo> * MtlInfoPtr = TableMtlMapPtr->Find(Id);
    check(MtlInfoPtr != nullptr);
    const TSharedPtr<FMtlInfo> & MtlInfo = *MtlInfoPtr;

    if (MtlInfo->RcState == ECabinetResourceState::CabRc_Complete)
    {
        if (Container->IsClickedSameReplacingItem(InItemCode))
            return;

        switch (ExtrusionType)
        {
            case 2:
                FArmyWHCabinetMgr::Get()->SetToeTrimMtl(MtlInfo.Get());
                break;
            case 3:
                FArmyWHCabinetMgr::Get()->SetTopTrimMtl(MtlInfo.Get());
                break;
        }
        Container->ListPanel->SetSelectedItem(InItemCode);
    }
    else if (MtlInfo->RcState == ECabinetResourceState::CabRc_None)
    {
        TArray<FDownloadSpace::FDownloadFileInfo> FileInfos;
        WHC_ADD_DOWNLOAD_TASK(FileInfos, Id, MtlInfo->GetCachedPath(), MtlInfo->MtlUrl, MtlInfo->MtlMd5);

        SContentItem * ItemPtr = Container->GetListItem(InItemCode);
        if (FileInfos.Num() == 0)
        {
            SetDownloadExtrusionMtlResState(false, ItemPtr, MtlInfo.Get());
            return;
        }
        if (ItemPtr)
            ItemPtr->SetItemState(EDownloading);
        MtlInfo->RcState = ECabinetResourceState::CabRc_FetchingRc;

        int32 EType = ExtrusionType;
        TSharedPtr<FDownloadFileSet> NewTask = FArmyDownloadModule::Get().GetDownloadManager()->AddMultiTask(MtlInfo->MtlUrl, FileInfos);
        NewTask->OnDownloadFinished.BindLambda(
            [this, Id, EType, TableMtlMapPtr](bool bSucceeded)
            {
                EWHCModeAttrType AttrType = EWHCModeAttrType::EWHCAttr_Unknown;
                switch (EType)
                {
                    case 2:
                        AttrType = EWHCModeAttrType::EWHCAttr_ToeMtl;
                        break;
                    case 3:
                        AttrType = EWHCModeAttrType::EWHCAttr_TopBlockerMtl;
                        break;
                }
                SContentItem * ItemPtr = Container->GetListItemMatchingType(Id, AttrType);

                const TSharedPtr<FMtlInfo> * MtlInfoPtr = TableMtlMapPtr->Find(Id);
                check(MtlInfoPtr != nullptr);
                const TSharedPtr<FMtlInfo> & MtlInfo = *MtlInfoPtr;

                if (bSucceeded)
                {
                    if (MtlInfo->Mtl == nullptr)
                    {
                        MtlInfo->Mtl = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(MtlInfo->GetCachedPath(), MtlInfo->MtlParam);
                    } 

                    if (MtlInfo->Mtl != nullptr)
                    {
                        MtlInfo->Mtl->AddToRoot();
                        SetDownloadExtrusionMtlResState(true, ItemPtr, MtlInfo.Get());
                        return;
                    }
                }
                
                SetDownloadExtrusionMtlResState(false, ItemPtr, MtlInfo.Get());
            }
        );
    }
}

void FArmyWHCExtrusionMtlUI::LoadExtrusionMtlData()
{
    if (Container->CheckNoMoreData())
        return;

    int32 Id = 0;
    switch (ExtrusionType)
    {
        case 2:
        {
            FTableInfo * ToeInfo = FArmyWHCabinetMgr::Get()->GetToeInfo();
            Id = ToeInfo ? ToeInfo->Id : 0;
            break;
        }
        case 3:
        {
            FTableInfo * TopInfo = FArmyWHCabinetMgr::Get()->GetTopInfo();
            Id = TopInfo ? TopInfo->Id : 0;
            break;
        }
    }

    int32 EType = ExtrusionType;
    Container->RunHttpRequest(
        FString::Printf(TEXT("design/decoratingPlate/materials?decoratingId=%d&categoryId=0"), Id), 
        TBaseDelegate<void, const TSharedPtr<FJsonObject>&>::CreateLambda(
            [this, EType](const TSharedPtr<FJsonObject> &ResponseData)
            {
                const TSharedPtr<FJsonObject> &Data = ResponseData->GetObjectField(TEXT("data"));
                if (Data.IsValid())
                {
                    Container->SetReplacingTotalPage(Data->GetNumberField(TEXT("totalPage")));
                
                    const TArray<TSharedPtr<FJsonValue>>* ExtrusionMtlArray = nullptr;
                    if (Data->TryGetArrayField(TEXT("list"), ExtrusionMtlArray))
                    {
                        int32 nCount = (*ExtrusionMtlArray).Num();
                        int32 IndexStart = Container->GetReplacingIds().Num();
                        for (int32 i = 0; i < nCount; ++i)
                        {
                            const TSharedPtr<FJsonValue> &Value = ExtrusionMtlArray->operator[](i);
                            const TSharedPtr<FJsonObject> &ExtrusionMtlJsonObject = Value->AsObject();

                            int32 Id = ExtrusionMtlJsonObject->GetNumberField(TEXT("id"));
                            Container->AddReplacingId(Id);

                            FWHCModeGlobalData::TMtlMap *TableMtlMapPtr = nullptr;
                            switch (EType)
                            {
                                case 2:
                                    TableMtlMapPtr = &FWHCModeGlobalData::ToeMtlMap;
                                    break;
                                case 3:
                                    TableMtlMapPtr = &FWHCModeGlobalData::TopMtlMap;
                                    break;
                            }
                            if (TableMtlMapPtr->Find(Id) == nullptr)
                            {
                                FMtlInfo * NewTableMtlInfo = new FMtlInfo;
                                NewTableMtlInfo->Id = Id;
                                NewTableMtlInfo->MtlName = ExtrusionMtlJsonObject->GetStringField(TEXT("name"));
                                NewTableMtlInfo->MtlThumbnailUrl = ExtrusionMtlJsonObject->GetStringField(TEXT("thumbnailUrl"));
                                NewTableMtlInfo->MtlUrl = ExtrusionMtlJsonObject->GetStringField(TEXT("pakUrl"));
                                NewTableMtlInfo->MtlMd5 = ExtrusionMtlJsonObject->GetStringField(TEXT("pakMd5"));
                                NewTableMtlInfo->MtlParam = ExtrusionMtlJsonObject->GetStringField(TEXT("optimizeParam"));
					        	TableMtlMapPtr->Emplace(Id, MakeShareable(NewTableMtlInfo));
                            }
                        }
                        FillExtrusionMtlList(IndexStart, nCount);
                    }
                }
            }
        )
    );
}

void FArmyWHCExtrusionMtlUI::FillExtrusionMtlList(int32 InStart, int32 InCount)
{
    if (!Container->IsWHCItemSelected<AXRShapeTableActor>())
        return;

    int32 TableMtlId = 0;
    FWHCModeGlobalData::TMtlMap *TableMtlMapPtr = nullptr;
    EWHCModeAttrType AttrType = EWHCModeAttrType::EWHCAttr_Unknown;
    switch (ExtrusionType)
    {
        case 2:
        {
            TableMtlMapPtr = &FWHCModeGlobalData::ToeMtlMap;
            AttrType = EWHCModeAttrType::EWHCAttr_ToeMtl;
            FTableInfo * ToeInfo = FArmyWHCabinetMgr::Get()->GetToeInfo();
            TableMtlId = ToeInfo != nullptr ? ToeInfo->mtlId : 0;
            break;
        }
        case 3:
            TableMtlMapPtr = &FWHCModeGlobalData::TopMtlMap;
            AttrType = EWHCModeAttrType::EWHCAttr_TopBlockerMtl;
            FTableInfo * TopInfo = FArmyWHCabinetMgr::Get()->GetTopInfo();
            TableMtlId = TopInfo != nullptr ? TopInfo->mtlId : 0;
            break;
    }
    check(TableMtlMapPtr != nullptr);

    int32 MatchingIndex = -1;
    TArray<int32> CabIds = Container->GetReplacingIds();
    for (int32 i = 0; i < InCount; ++i)
    {
        const TSharedPtr<FMtlInfo> * TableMtlInfoPtr = TableMtlMapPtr->Find(CabIds[InStart + i]);
        check(TableMtlInfoPtr != nullptr);
        const TSharedPtr<FMtlInfo> & TableMtlInfo = *TableMtlInfoPtr;
		TSharedPtr<SContentItem> Item = SNew(SContentItem)
            .ItemCode(InStart + i)
            .ItemWidth(112.0f)
            .ItemHeight(132.0f)
			.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
			.TextColor(FArmyStyle::Get().GetColor("Color.FF969799"))
			.ThumbnailURL(TableMtlInfo->MtlThumbnailUrl)
			.ThumbnailImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.DefaultImage_ContentBrowser")))//梁晓菲 默认图
			.bShowDownloadTips(TableMtlInfo->RcState != ECabinetResourceState::CabRc_Complete)
			.OnClicked_Raw(this, &FArmyWHCExtrusionMtlUI::Callback_ReplacingExtrusionMtlItem)
			.DisplayText(FText::FromString(TableMtlInfo->MtlName));
        Item->AddMetadata<FWHCModeAttrTypeInfo>(MakeShared<FWHCModeAttrTypeInfo>(AttrType, TableMtlInfo->Id));
	
        switch (TableMtlInfo->RcState)
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

        if (TableMtlId == TableMtlInfo->Id)
            MatchingIndex = InStart + i;
    }

    Container->ListPanel->SetSelectedItem(MatchingIndex);
	Container->ListPanel->Container->ResetScrollNotify();
}

void FArmyWHCExtrusionMtlUI::SetDownloadExtrusionMtlResState(bool bSucceeded, SContentItem *UIItem, FMtlInfo *DataItem)
{
    if (UIItem)
        UIItem->SetItemState(bSucceeded ? EReady : ENotReady);
    if (DataItem)
        DataItem->RcState = bSucceeded ? ECabinetResourceState::CabRc_Complete : ECabinetResourceState::CabRc_None;
}