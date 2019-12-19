#include "UIMixin/SArmyWHCExtrusionUI.h"
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

TSharedPtr<SWidget> FArmyWHCExtrusionUI::MakeWidget()
{
    int32 EType = ExtrusionType;

    TSharedPtr<SArmyWHCModeReplacingWidget> ExtrusionReplacing;
    SAssignNew(ExtrusionReplacing, SArmyWHCModeReplacingWidget)
        .ButtonImage(FArmyStyle::Get().GetBrush("WHCMode.Replace"))
        .OnWholeWidgetClicked_Raw(this, &FArmyWHCExtrusionUI::Callback_ReplacingExtrusionType)
        .OnButtonClicked_Raw(this, &FArmyWHCExtrusionUI::Callback_ReplacingExtrusionType);
    ExtrusionReplacing->SetReplacingItemThumbnail(TAttribute<const FSlateBrush*>::Create(
        [this, EType]() -> const FSlateBrush* {
            if (Container->IsWHCItemSelected<AXRShapeTableActor>())
            {
                switch (EType)
                {
                    case 2:
                    {
                        FTableInfo* pInfo = FArmyWHCabinetMgr::Get()->GetToeInfo();
                        if (pInfo != nullptr)
                            return FArmySlateModule::Get().WebImageCache->Download(pInfo->ThumbnailUrl)->GetBrush();
                        break;
                    }
                    case 3:
                    {
                        FTableInfo *pInfo = FArmyWHCabinetMgr::Get()->GetTopInfo();
                        if (pInfo != nullptr)
                            return FArmySlateModule::Get().WebImageCache->Download(pInfo->ThumbnailUrl)->GetBrush();
                        break;
                    }
                }             
            }
            return FCoreStyle::Get().GetDefaultBrush();
        }
    ));
    ExtrusionReplacing->SetReplacingItemText(TAttribute<FText>::Create(
        [this, EType]() -> FText {
            if (Container->IsWHCItemSelected<AXRShapeTableActor>())
            {
                switch (EType)
                {
                    case 2:
                    {
                        FTableInfo* pInfo = FArmyWHCabinetMgr::Get()->GetToeInfo();
                        if (pInfo != nullptr)
                            return FText::FromString(pInfo->Name);
                        break;
                    }
                    case 3:
                    {
                        FTableInfo* pInfo = FArmyWHCabinetMgr::Get()->GetTopInfo();
                        if (pInfo != nullptr)
                            return FText::FromString(pInfo->Name);
                        break;
                    }
                }            
            }
            return FText();
        }
    ));
    return ExtrusionReplacing;
}

void FArmyWHCExtrusionUI::LoadListData()
{
    if (!Container->IsWHCItemSelected<AXRShapeTableActor>())
        return;
    LoadExtrusionData();
}

void FArmyWHCExtrusionUI::Callback_ReplacingExtrusionType()
{
    if (!Container->IsWHCItemSelected<AXRShapeTableActor>())
        return;

    FString DesireState;
    switch (ExtrusionType)
    {
        case 2:
            DesireState = TEXT("踢脚板造型");
            break;
        case 3:
            DesireState = TEXT("上线条造型");
            break;
    }
    if (Container->IsCurrentState(DesireState))
        return;

    Container->ListPanel->ClearListController();
    if (Container->ReplacingSlot->GetWidget() == SNullWidget::NullWidget)
        Container->SetReplacingPanel(Container->ListPanel.ToSharedRef());
    
    Container->ListPanel->SetCurrentState(DesireState);

    Container->ClearReplacingList();
    LoadExtrusionData();
}

void FArmyWHCExtrusionUI::Callback_ReplacingExtrusionItem(int32 InItemCode)
{
    if (!Container->CheckIsValidReplacingIndex(InItemCode))
        return;
    if (!Container->IsWHCItemSelected<AXRShapeTableActor>())
        return;

    int32 Id = Container->GetReplacingIds()[InItemCode];
    FWHCModeGlobalData::TTableMap *TableMapPtr = nullptr;
    FWHCModeGlobalData::TMtlMap *TableMtlMapPtr = nullptr;
    switch (ExtrusionType)
    {
        case 2:
            TableMapPtr = &FWHCModeGlobalData::ToeSections;
            TableMtlMapPtr = &FWHCModeGlobalData::ToeMtlMap;
            break;
        case 3:
            TableMapPtr = &FWHCModeGlobalData::TopSections;
            TableMtlMapPtr = &FWHCModeGlobalData::TopMtlMap;
            break;
    }
    check(TableMapPtr != nullptr && TableMtlMapPtr != nullptr);

    const TSharedPtr<FTableInfo> * TableInfoPtr = TableMapPtr->Find(Id);
    check(TableInfoPtr != nullptr);
    const TSharedPtr<FTableInfo> & TableInfo = *TableInfoPtr;

    const TSharedPtr<FMtlInfo> * MtlInfoPtr = TableMtlMapPtr->Find(TableInfo->mtlId);
    check(MtlInfoPtr != nullptr);
    const TSharedPtr<FMtlInfo> & MtlInfo = *MtlInfoPtr;

    if (TableInfo->RcState == ECabinetResourceState::CabRc_Complete)
    {
        if (Container->IsClickedSameReplacingItem(InItemCode))
            return;

        FArmyWHCabinetMgr * RESTRICT CabMgr = FArmyWHCabinetMgr::Get();
            
        switch (ExtrusionType)
        {
            case 2:
            {
                CabMgr->SetToeTrim(TableInfo.Get());
                // 刷新
                CabMgr->GenTableBoard();
                // 更新柜子距离地面的高度
                CabMgr->OnUIAboveGroundChange(TableInfo->CurrentHeight);
                
                CabMgr->SetToeTrimMtl(MtlInfo.Get());

                break;
            }
            case 3:
            {
                CabMgr->SetTopTrim(TableInfo.Get());
		        CabMgr->GenHangGapBoard();
		        CabMgr->SetTopTrimMtl(MtlInfo.Get());
                break;
            }
        }

        Container->ListPanel->SetSelectedItem(InItemCode);
    }
    else if (TableInfo->RcState == ECabinetResourceState::CabRc_None)
    {
        const TSharedPtr<FMtlInfo> * MtlInfoPtr = TableMtlMapPtr->Find(TableInfo->mtlId);
        check(MtlInfoPtr != nullptr);
        const TSharedPtr<FMtlInfo> & MtlInfo = *MtlInfoPtr;
        int32 MtlId = MtlInfo->Id;
        TArray<FDownloadSpace::FDownloadFileInfo> FileInfos;
        WHC_ADD_DOWNLOAD_TASK(FileInfos, Id, TableInfo->GetCachedPath(), TableInfo->cadUrl, FString());
        WHC_ADD_DOWNLOAD_TASK(FileInfos, MtlId, MtlInfo->GetCachedPath(), MtlInfo->MtlUrl, MtlInfo->MtlMd5);

        SContentItem * ItemPtr = Container->GetListItem(InItemCode);
        if (FileInfos.Num() == 0)
        {
            SetDownloadExtrusionResState(false, ItemPtr, TableInfo.Get());
            return;
        }
        if (ItemPtr)
            ItemPtr->SetItemState(EDownloading);
        TableInfo->RcState = ECabinetResourceState::CabRc_FetchingRc;

        int32 EType = ExtrusionType;
        TSharedPtr<FDownloadFileSet> NewTask = FArmyDownloadModule::Get().GetDownloadManager()->AddMultiTask(TableInfo->cadUrl, FileInfos);
        NewTask->OnDownloadFinished.BindLambda(
            [this, Id, EType, TableMapPtr, TableMtlMapPtr](bool bSucceeded)
            {
                const TSharedPtr<FTableInfo> * TableInfoPtr = TableMapPtr->Find(Id);
                check(TableInfoPtr != nullptr);

                EWHCModeAttrType AttrType = EWHCModeAttrType::EWHCAttr_Unknown;
                switch (EType)
                {
                    case 2:
                        AttrType = EWHCModeAttrType::EWHCAttr_ToeType;
                        break;
                    case 3:
                        AttrType = EWHCModeAttrType::EWHCAttr_TopBlockerType;
                        break;
                }
                SContentItem * ItemPtr = Container->GetListItemMatchingType(Id, AttrType);

                if (bSucceeded)
                {
                    const TSharedPtr<FMtlInfo> * MtlInfoPtr = TableMtlMapPtr->Find((*TableInfoPtr)->mtlId);
                    check(MtlInfoPtr != nullptr);
                    const TSharedPtr<FMtlInfo> & MtlInfo = *MtlInfoPtr;
                    if (MtlInfo->Mtl == nullptr)
                    {
                        MtlInfo->Mtl = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(MtlInfo->GetCachedPath(), MtlInfo->MtlParam);
                    } 

                    if (MtlInfo->Mtl != nullptr)
                    {
                        MtlInfo->Mtl->AddToRoot();
                        SetDownloadExtrusionResState(true, ItemPtr, (*TableInfoPtr).Get());
                        return;
                    }
                }
                
                SetDownloadExtrusionResState(false, ItemPtr, (*TableInfoPtr).Get());
            }
        );
    }
}

void FArmyWHCExtrusionUI::LoadExtrusionData()
{
    if (Container->CheckNoMoreData())
        return;

    int32 EType = ExtrusionType;
    Container->RunHttpRequest(
        FString::Printf(TEXT("design/decoratingPlate?type=%d&current=%d&size=15"), EType, Container->GetPageIndexToLoad()),
        TBaseDelegate<void, const TSharedPtr<FJsonObject>&>::CreateLambda(
            [this, EType](const TSharedPtr<FJsonObject> &ResponseData)
            {
                const TSharedPtr<FJsonObject> &Data = ResponseData->GetObjectField(TEXT("data"));
                if (Data.IsValid())
                {
                    Container->SetReplacingTotalPage(Data->GetNumberField(TEXT("totalPage")));
                
                    const TArray<TSharedPtr<FJsonValue>>* ExtrusionArray = nullptr;
                    if (Data->TryGetArrayField(TEXT("list"), ExtrusionArray))
                    {
                        int32 nCount = (*ExtrusionArray).Num();
                        int32 IndexStart = Container->GetReplacingIds().Num();
                        for (int32 i = 0; i < nCount; ++i)
                        {
                            const TSharedPtr<FJsonValue> &Value = ExtrusionArray->operator[](i);
                            const TSharedPtr<FJsonObject> &ExtrusionJsonObject = Value->AsObject();

                            int32 Id = ExtrusionJsonObject->GetNumberField(TEXT("id"));
                            Container->AddReplacingId(Id);

                            FWHCModeGlobalData::TTableMap *TableMapPtr = nullptr;
                            FWHCModeGlobalData::TMtlMap *TableMtlMapPtr = nullptr;
                            switch (EType)
                            {
                                case 2:
                                    TableMapPtr = &FWHCModeGlobalData::ToeSections;
                                    TableMtlMapPtr = &FWHCModeGlobalData::ToeMtlMap;
                                    break;
                                case 3:
                                    TableMapPtr = &FWHCModeGlobalData::TopSections;
                                    TableMtlMapPtr = &FWHCModeGlobalData::TopMtlMap;
                                    break;
                            }

                            if (TableMapPtr && TableMapPtr->Find(Id) == nullptr)
                            {
                                FTableInfo * NewTableInfo = new FTableInfo;
                                NewTableInfo->Id = Id;
                                NewTableInfo->Name = ExtrusionJsonObject->GetStringField(TEXT("name"));
                                NewTableInfo->ThumbnailUrl = ExtrusionJsonObject->GetStringField(TEXT("thumbnailUrl"));
                                NewTableInfo->cadUrl = ExtrusionJsonObject->GetStringField(TEXT("cadUrl"));
                                const TSharedPtr<FJsonObject> & DefaultMtlJsonObj = ExtrusionJsonObject->GetObjectField(TEXT("defaultMaterial"));
                                NewTableInfo->mtlId = DefaultMtlJsonObj->GetIntegerField(TEXT("id"));
                                const TSharedPtr<FJsonObject> & SectionHeight = ExtrusionJsonObject->GetObjectField(TEXT("sectionHeight"));
                                NewTableInfo->CurrentHeight = SectionHeight->GetNumberField(TEXT("current"));
                                const TArray<TSharedPtr<FJsonValue>> & HeightArr = SectionHeight->GetArrayField(TEXT("value"));
                                for (const auto & Height : HeightArr)
                                    NewTableInfo->AvailableHeights.Emplace(Height->AsNumber());
                                
                                if (TableMtlMapPtr && TableMtlMapPtr->Find(NewTableInfo->mtlId) == nullptr)
                                {
                                    FMtlInfo * MtlInfo = new FMtlInfo;
                                    MtlInfo->Id = NewTableInfo->mtlId;
                                    MtlInfo->MtlName = DefaultMtlJsonObj->GetStringField(TEXT("name"));
                                    MtlInfo->MtlThumbnailUrl = DefaultMtlJsonObj->GetStringField(TEXT("thumbnailUrl"));
                                    MtlInfo->MtlUrl = DefaultMtlJsonObj->GetStringField(TEXT("pakUrl"));
                                    MtlInfo->MtlMd5 = DefaultMtlJsonObj->GetStringField(TEXT("pakMd5"));
                                    MtlInfo->MtlParam = DefaultMtlJsonObj->GetStringField(TEXT("optimizeParam"));
                                    TableMtlMapPtr->Emplace(MtlInfo->Id, MakeShareable(MtlInfo));
                                }

                                TableMapPtr->Emplace(Id, MakeShareable(NewTableInfo));
                            }
                        }
                        FillExtrusionList(IndexStart, nCount);
                    }
                }
            }
        )
    );
}

void FArmyWHCExtrusionUI::FillExtrusionList(int32 InStart, int32 InCount)
{
    if (!Container->IsWHCItemSelected<AXRShapeTableActor>())
        return;

    int32 TableId = 0;
    FWHCModeGlobalData::TTableMap *TableMapPtr = nullptr;
    EWHCModeAttrType AttrType = EWHCModeAttrType::EWHCAttr_Unknown;
    switch (ExtrusionType)
    {
        case 2:
        {
            TableMapPtr = &FWHCModeGlobalData::ToeSections;
            AttrType = EWHCModeAttrType::EWHCAttr_ToeType;
            FTableInfo * ToeInfo = FArmyWHCabinetMgr::Get()->GetToeInfo();
            TableId = ToeInfo != nullptr ? ToeInfo->Id : 0;
            break;
        }
        case 3:
            TableMapPtr = &FWHCModeGlobalData::TopSections;
            AttrType = EWHCModeAttrType::EWHCAttr_TopBlockerType;
            FTableInfo * TopInfo = FArmyWHCabinetMgr::Get()->GetTopInfo();
            TableId = TopInfo != nullptr ? TopInfo->Id : 0;
            break;
    }
    check(TableMapPtr != nullptr);

    int32 MatchingIndex = -1;
    TArray<int32> CabIds = Container->GetReplacingIds();
    for (int32 i = 0; i < InCount; ++i)
	{
        const TSharedPtr<FTableInfo> * TableInfoPtr = TableMapPtr->Find(CabIds[InStart + i]);
        check(TableInfoPtr != nullptr);
        const TSharedPtr<FTableInfo> & TableInfo = *TableInfoPtr;
		TSharedPtr<SContentItem> Item = SNew(SContentItem)
            .ItemCode(InStart + i)
            .ItemWidth(112.0f)
            .ItemHeight(132.0f)
			.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
			.TextColor(FArmyStyle::Get().GetColor("Color.FF969799"))
			.ThumbnailURL(TableInfo->ThumbnailUrl)
			.ThumbnailImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.DefaultImage_ContentBrowser")))//梁晓菲 默认图
			.bShowDownloadTips(TableInfo->RcState != ECabinetResourceState::CabRc_Complete)
			.OnClicked_Raw(this, &FArmyWHCExtrusionUI::Callback_ReplacingExtrusionItem)
			.DisplayText(FText::FromString(TableInfo->Name));
        Item->AddMetadata<FWHCModeAttrTypeInfo>(MakeShared<FWHCModeAttrTypeInfo>(AttrType, TableInfo->Id));
	
        switch (TableInfo->RcState)
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

        if (TableId == TableInfo->Id)
            MatchingIndex = InStart + i;
	}

    Container->ListPanel->SetSelectedItem(MatchingIndex);
	Container->ListPanel->Container->ResetScrollNotify();
}

void FArmyWHCExtrusionUI::SetDownloadExtrusionResState(bool bSucceeded, SContentItem *UIItem, FTableInfo *DataItem)
{
    if (UIItem)
        UIItem->SetItemState(bSucceeded ? EReady : ENotReady);
    if (DataItem)
        DataItem->RcState = bSucceeded ? ECabinetResourceState::CabRc_Complete : ECabinetResourceState::CabRc_None;
}