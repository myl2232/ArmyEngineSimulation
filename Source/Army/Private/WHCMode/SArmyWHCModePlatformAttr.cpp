#include "SArmyWHCModePlatformAttr.h"
#include "SArmyWHCModeListPanel.h"
#include "ArmyShapeTableActor.h"
#include "ArmyWHCabinetAutoMgr.h"

#include "ArmyStyle.h"
#include "ArmyDetailBuilder.h"
#include "SContentItem.h"
#include "ArmySlateModule.h"
#include "ArmyDownloadModule.h"
#include "WebImageCache.h"
#include "Actor/XRActorConstant.h"
#include "Data/WHCModeData/XRWHCModeData.h"
#include "SScrollWrapBox.h"
#include "Game/XRGameInstance.h"

#include "TypedPrimitive.h"
#include "SCTResManager.h"

#include "Materials/MaterialInterface.h"

FArmyWHCModePlatformAttr::FArmyWHCModePlatformAttr(TSharedRef<SArmyWHCModeAttrPanelContainer> InContainer)
: FArmyWHCModeAttrPanel(InContainer)
{}

void FArmyWHCModePlatformAttr::TickPanel()
{
	TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
	TArray<int32> CabIds = ContainerPtr->GetReplacingIds();
    TSharedPtr<FDownloadFileSet> TheTask;
	for (int32 i = 0; i < CabIds.Num(); ++i)
	{
		int32 Id = CabIds[i];

		SContentItem *ContentItem = ContainerPtr->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_PlatformMtl);
		if (ContentItem && ContentItem->GetItemState() == EDownloading)
		{
			const TSharedPtr<FPlatformInfo> * PlatformInfo = FWHCModeGlobalData::PlatformMap.Find(Id);
			check(PlatformInfo != nullptr);
			TheTask = FArmyDownloadModule::Get().GetDownloadManager()->GetTask((*PlatformInfo)->PlatformMtl->MtlUrl);
			if (TheTask.IsValid())
			{
				// 设置下载进度
				UpdateProgress(TheTask, ContentItem);
				continue;
			}
		}

		ContentItem = ContainerPtr->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_PlatformFront);
		if (ContentItem && ContentItem->GetItemState() == EDownloading)
		{
			// 获取当前正在使用的台面
			const TSharedPtr<FPlatformInfo> * PlatformInfo = FWHCModeGlobalData::PlatformMap.Find(CurrentPlatformId);
			check(PlatformInfo != nullptr);
			// 从当前台面中查找特定的前挡造型
			const TSharedPtr<FTableInfo> * TableInfo = (*PlatformInfo)->FrontSections.FindByPredicate(
				[Id](const TSharedPtr<FTableInfo> & TableInfo)
				{ return Id == TableInfo->Id; }
			);
			check(TableInfo != nullptr);
			TheTask = FArmyDownloadModule::Get().GetDownloadManager()->GetTask((*TableInfo)->cadUrl);
			if (TheTask.IsValid())
			{
				// 设置下载进度
				UpdateProgress(TheTask, ContentItem);
				continue;
			}
		}
	}
}

void FArmyWHCModePlatformAttr::Initialize()
{
    PlatformPanel = MakeShareable(new FArmyDetailBuilder);

    FArmyDetailNode &BasicParameters = PlatformPanel->AddCategory("BasicParameters", TEXT("基本参数"));

	TSharedPtr<SArmyWHCModeReplacingWidget> PlatformMtlReplacing;
    FArmyDetailNode &PlatformMtlNode = BasicParameters.AddChildNode("PlatformMtl", TEXT("台面材质"), 
        SAssignNew(PlatformMtlReplacing, SArmyWHCModeReplacingWidget)
		.ButtonImage(FArmyStyle::Get().GetBrush("WHCMode.Replace"))
        .OnWholeWidgetClicked_Raw(this, &FArmyWHCModePlatformAttr::Callback_ReplacingPlatformMtl)
		.OnButtonClicked_Raw(this, &FArmyWHCModePlatformAttr::Callback_ReplacingPlatformMtl)
    );
	PlatformMtlReplacing->SetReplacingItemThumbnail(TAttribute<const FSlateBrush*>::Create(
		[this]() -> const FSlateBrush* {
			FPlatformInfo * PlatformInfo = FArmyWHCabinetMgr::Get()->GetPlatformInfo();
			return (PlatformInfo != nullptr) ? 
				FArmySlateModule::Get().WebImageCache->Download(PlatformInfo->PlatformMtl->MtlThumbnailUrl)->GetBrush() : 
				FCoreStyle::Get().GetDefaultBrush();
		}
	));
	PlatformMtlReplacing->SetReplacingItemText(TAttribute<FText>::Create(
		[this]() -> FText {
			FPlatformInfo * PlatformInfo = FArmyWHCabinetMgr::Get()->GetPlatformInfo();
			return FText::FromString((PlatformInfo != nullptr) ? PlatformInfo->PlatformMtl->MtlName : TEXT(""));
		}
	));
    PlatformMtlNode.NameContentOverride().VAlign(VAlign_Center);
    PlatformMtlNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

	TSharedPtr<SArmyWHCModeReplacingWidget> PlatformFrontReplacing;
    FArmyDetailNode &PlatformFrontNode = BasicParameters.AddChildNode("PlatformFront", TEXT("前沿型号"),
        SAssignNew(PlatformFrontReplacing, SArmyWHCModeReplacingWidget)
		.ButtonImage(FArmyStyle::Get().GetBrush("WHCMode.Replace"))
        .OnWholeWidgetClicked_Raw(this, &FArmyWHCModePlatformAttr::Callback_ReplacingPlatformFront)
		.OnButtonClicked_Raw(this, &FArmyWHCModePlatformAttr::Callback_ReplacingPlatformFront)
    );
	PlatformFrontReplacing->SetReplacingItemThumbnail(TAttribute<const FSlateBrush*>::Create(
		[this]() -> const FSlateBrush* {
			FTableInfo* pTableInfo = FArmyWHCabinetMgr::Get()->GetFrontInfo();
			return (pTableInfo != nullptr) ? 
				FArmySlateModule::Get().WebImageCache->Download(pTableInfo->ThumbnailUrl)->GetBrush() : 
				FCoreStyle::Get().GetDefaultBrush();
		}
	));
	PlatformFrontReplacing->SetReplacingItemText(TAttribute<FText>::Create(
		[this]() -> FText {
			FTableInfo* pTableInfo = FArmyWHCabinetMgr::Get()->GetFrontInfo();
			return FText::FromString((pTableInfo != nullptr) ? pTableInfo->Name : TEXT(""));
		}
	));
    PlatformFrontNode.NameContentOverride().VAlign(VAlign_Center);
    PlatformFrontNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

	// FArmyDetailNode &PlatformBackNode = BasicParameters.AddChildNode("PlatformBack", TEXT("后挡型号"),
    //     SAssignNew(PlatformBackReplacing, SArmyWHCModeReplacingWidgetWithOverlayMenu)
	// 	.OverlayWidgetForMenu(Container.Pin()->ContentOverlay)
	// 	.OnMenuItemSelectionChanged_Raw(this, &FArmyWHCModePlatformAttr::Callback_PlatformBackMenuItemSelectionChanged)
    // );
    // PlatformBackNode.NameContentOverride().VAlign(VAlign_Center);
    // PlatformBackNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
}

bool FArmyWHCModePlatformAttr::TryDisplayAttrPanel(AActor *InActor)
{
    if (InActor->ActorHasTag(XRActorTag::WHCTable) && InActor->IsA<AXRShapeTableActor>())
    {
        TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
        if (ContainerPtr->LastWHCItemSelected == InActor)
            return true;

        ContainerPtr->LastWHCItemSelected = InActor; 
        ContainerPtr->SetAttrPanel(PlatformPanel->BuildDetail().ToSharedRef());
        return true;
    }
    else
        return false;
}

void FArmyWHCModePlatformAttr::LoadMoreReplacingData()
{
    const FString & CurrentState = Container.Pin()->GetCurrentState();
    if (CurrentState == TEXT("台面材质"))
        LoadPlatformData();
}

void FArmyWHCModePlatformAttr::Callback_ItemClicked(int32 InItemCode)
{
    const FString & CurrentState = Container.Pin()->GetCurrentState();
    if (CurrentState == TEXT("台面材质"))
        ReplacingPlatformMtlItemClicked(InItemCode);
    else if (CurrentState == TEXT("前沿型号"))
        ReplacingPlatformFrontItemClicked(InItemCode);
}

void FArmyWHCModePlatformAttr::Callback_ReplacingPlatformMtl()
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
	if (!ContainerPtr->IsWHCItemSelected<AXRShapeTableActor>())
		return;

	FString DesiredState(TEXT("台面材质"));
	if (ContainerPtr->IsCurrentState(DesiredState))
		return;

	ContainerPtr->ListPanel->ClearListController();
    if (ContainerPtr->ReplacingSlot->GetWidget() == SNullWidget::NullWidget)
        ContainerPtr->SetReplacingPanel(ContainerPtr->ListPanel.ToSharedRef());
    
    ContainerPtr->SetCurrentState(DesiredState);

    ContainerPtr->ClearReplacingList();
    LoadPlatformData();
}

void FArmyWHCModePlatformAttr::LoadPlatformData()
{
	TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    if (ContainerPtr->CheckNoMoreData())
		return;

	ContainerPtr->RunHttpRequest(
		FString::Printf(TEXT("design/worktops?current=%d&size=15"), ContainerPtr->GetPageIndexToLoad()),
		TBaseDelegate<void, const TSharedPtr<FJsonObject>&>::CreateLambda(
			[this](const TSharedPtr<FJsonObject> &ResponseData)
			{
				TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
				const TSharedPtr<FJsonObject> &Data = ResponseData->GetObjectField(TEXT("data"));
				ContainerPtr->SetReplacingTotalPage(Data->GetNumberField(TEXT("totalPage")));

				const TArray<TSharedPtr<FJsonValue>>* PlatformArray = nullptr;
				if (Data->TryGetArrayField(TEXT("list"), PlatformArray))
				{
					int32 nCount = (*PlatformArray).Num();
					int32 IndexStart = ContainerPtr->GetReplacingIds().Num();
					for (int32 i = 0; i < nCount; ++i)
					{
						const TSharedPtr<FJsonValue> &Value = PlatformArray->operator[](i);
						const TSharedPtr<FJsonObject> &PlatformJsonObject = Value->AsObject();

						int32 Id = PlatformJsonObject->GetNumberField(TEXT("id"));
						ContainerPtr->AddReplacingId(Id);

						if (FWHCModeGlobalData::PlatformMap.Find(Id) == nullptr)
						{
							FPlatformInfo * NewPlatformInfo = new FPlatformInfo;
							NewPlatformInfo->Id = Id;

							FMtlInfo * NewMtlInfo = new FMtlInfo;
							const TSharedPtr<FJsonObject> & MtlJsonObj = PlatformJsonObject->GetObjectField(TEXT("material"));
							NewMtlInfo->Id = MtlJsonObj->GetIntegerField(TEXT("id"));
							NewMtlInfo->MtlName = MtlJsonObj->GetStringField(TEXT("name"));
							NewMtlInfo->MtlThumbnailUrl = MtlJsonObj->GetStringField(TEXT("thumbnailUrl"));
							NewMtlInfo->MtlUrl = MtlJsonObj->GetStringField(TEXT("pakUrl"));
							NewMtlInfo->MtlMd5 = MtlJsonObj->GetStringField(TEXT("pakMd5"));
							NewMtlInfo->MtlParam = MtlJsonObj->GetStringField(TEXT("optimizeParam"));
							NewPlatformInfo->PlatformMtl = MakeShareable(NewMtlInfo);

							// 前沿
							const TArray<TSharedPtr<FJsonValue>>* fontSections = nullptr;
							if (PlatformJsonObject->TryGetArrayField(TEXT("fontSections"), fontSections))
							{
								for (auto& Section : *fontSections)
								{
									const TSharedPtr<FJsonObject> & FrontJsonObj = Section->AsObject();

									FTableInfo* pInfo = new FTableInfo;
									pInfo->Id = FrontJsonObj->GetNumberField(TEXT("id"));
									pInfo->Name = FrontJsonObj->GetStringField(TEXT("name"));
									pInfo->ThumbnailUrl = FrontJsonObj->GetStringField(TEXT("thumbnailUrl"));
									pInfo->cadUrl = FrontJsonObj->GetStringField(TEXT("cadUrl"));
									NewPlatformInfo->FrontSections.Emplace(MakeShareable(pInfo));
								}
							}
							
							// 后档
							const TArray<TSharedPtr<FJsonValue>>* backSections = nullptr;
							if (PlatformJsonObject->TryGetArrayField(TEXT("backSections"), backSections))
							{
								for (auto& Section : *backSections)
								{
									const TSharedPtr<FJsonObject> & BackJsonObj = Section->AsObject();

									FTableInfo* pInfo = new FTableInfo;
									pInfo->Id = BackJsonObj->GetNumberField(TEXT("id"));
									pInfo->Name = BackJsonObj->GetStringField(TEXT("name"));
									pInfo->ThumbnailUrl = BackJsonObj->GetStringField(TEXT("thumbnailUrl"));
									pInfo->cadUrl = BackJsonObj->GetStringField(TEXT("cadUrl"));
									NewPlatformInfo->BackSections.Emplace(MakeShareable(pInfo));
								}
							}	
							
							FWHCModeGlobalData::PlatformMap.Emplace(Id, MakeShareable(NewPlatformInfo));
						}

					}

					FillPlatformMtlList(IndexStart, nCount);
				}
			}
		)
	);
}

void FArmyWHCModePlatformAttr::FillPlatformMtlList(int32 InStart, int32 InCount)
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
	if (!ContainerPtr->IsWHCItemSelected<AXRShapeTableActor>())
		return;

	FPlatformInfo * PlatformInfo = FArmyWHCabinetMgr::Get()->GetPlatformInfo();
	int32 Id = PlatformInfo ? PlatformInfo->Id : 0;

	int32 MatchingIndex = -1;
    TArray<int32> CabIds = ContainerPtr->GetReplacingIds();
    for (int32 i = 0; i < InCount; ++i)
	{
        const TSharedPtr<FPlatformInfo> * PlatformInfoPtr = FWHCModeGlobalData::PlatformMap.Find(CabIds[InStart + i]);
		check(PlatformInfoPtr != nullptr);
		const TSharedPtr<FPlatformInfo> & PlatformInfo = *PlatformInfoPtr;
		TSharedPtr<SContentItem> Item = SNew(SContentItem)
            .ItemCode(InStart + i)
            .ItemWidth(112.0f)
            .ItemHeight(132.0f)
			.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
			.TextColor(FArmyStyle::Get().GetColor("Color.FF969799"))
			.ThumbnailURL(PlatformInfo->PlatformMtl->MtlThumbnailUrl)
			.ThumbnailImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.DefaultImage_ContentBrowser")))//梁晓菲 默认图
			.bShowDownloadTips(PlatformInfo->PlatformMtl->RcState != ECabinetResourceState::CabRc_Complete)
			.OnClicked_Raw(this, &FArmyWHCModePlatformAttr::Callback_ItemClicked)
			.DisplayText(FText::FromString(PlatformInfo->PlatformMtl->MtlName));
		Item->AddMetadata<FWHCModeAttrTypeInfo>(MakeShared<FWHCModeAttrTypeInfo>(EWHCModeAttrType::EWHCAttr_PlatformMtl, PlatformInfo->Id));

		switch (PlatformInfo->PlatformMtl->RcState)
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
		ContainerPtr->ListPanel->AddItem(Item); 

		if (Id == PlatformInfo->Id)
			MatchingIndex = InStart + i;
	}

	ContainerPtr->ListPanel->SetSelectedItem(MatchingIndex);
	ContainerPtr->ListPanel->Container->ResetScrollNotify();
}

void FArmyWHCModePlatformAttr::ReplacingPlatformMtlItemClicked(int32 InItemCode)
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
	if (!ContainerPtr->IsWHCItemSelected<AXRShapeTableActor>())
		return;

	if (!ContainerPtr->CheckIsValidReplacingIndex(InItemCode))
		return;
	

    int32 Id = ContainerPtr->GetReplacingIds()[InItemCode];
    const TSharedPtr<FPlatformInfo> * PlatformInfoPtr = FWHCModeGlobalData::PlatformMap.Find(Id);
	check(PlatformInfoPtr != nullptr);
	const TSharedPtr<FPlatformInfo> & PlatformInfo = *PlatformInfoPtr;
    if (PlatformInfo->PlatformMtl->RcState == ECabinetResourceState::CabRc_None)
    {
        FString CachePath = PlatformInfo->PlatformMtl->GetCachedPath();
        TArray<FDownloadSpace::FDownloadFileInfo> FileInfos;
		WHC_ADD_DOWNLOAD_TASK(FileInfos, Id, CachePath, PlatformInfo->PlatformMtl->MtlUrl, PlatformInfo->PlatformMtl->MtlMd5);
		if (PlatformInfo->FrontSections.Num() > 0)
		{
			// 随台面材质一起替换的是第一个可用的前挡造型
			WHC_ADD_DOWNLOAD_TASK(FileInfos, 
				PlatformInfo->FrontSections[0]->Id,
				PlatformInfo->FrontSections[0]->GetCachedPath(),
				PlatformInfo->FrontSections[0]->cadUrl,
				FString());
		}

		SContentItem * Item = ContainerPtr->GetListItem(InItemCode);
		if (FileInfos.Num() == 0)
		{
			SetDownloadPlatformMtlResState(false, Item, PlatformInfo->PlatformMtl.Get());
			return;
		}
		if (Item)
        	Item->SetItemState(EDownloading);
		PlatformInfo->PlatformMtl->RcState = ECabinetResourceState::CabRc_FetchingRc;

        TSharedPtr<FDownloadFileSet> NewTask = FArmyDownloadModule::Get().GetDownloadManager()->AddMultiTask(PlatformInfo->PlatformMtl->MtlUrl, FileInfos);
        NewTask->OnDownloadFinished.BindLambda(
            [this](bool bSucceeded, FString InCachePath, int32 Id)
            {
                SContentItem * ContentItem = Container.Pin()->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_PlatformMtl);

				const TSharedPtr<FPlatformInfo> * PlatformInfoPtr = FWHCModeGlobalData::PlatformMap.Find(Id);
				check(PlatformInfoPtr != nullptr);
				const TSharedPtr<FPlatformInfo> & PlatformInfo = *PlatformInfoPtr;

                if (bSucceeded)
                {
					if (PlatformInfo->PlatformMtl->Mtl == nullptr)
					{
						PlatformInfo->PlatformMtl->Mtl = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(InCachePath, PlatformInfo->PlatformMtl->MtlParam);
					}
                    // 只有台面材质能够正确加载才认为资源下载成功
                    if (PlatformInfo->PlatformMtl->Mtl != nullptr)
                    {
                        PlatformInfo->PlatformMtl->Mtl->AddToRoot();
						SetDownloadPlatformMtlResState(true, ContentItem, PlatformInfo->PlatformMtl.Get());
                        return;
                    }
                }

				SetDownloadPlatformMtlResState(false, ContentItem, PlatformInfo->PlatformMtl.Get());
            },
            CachePath,
            Id
        );
    }
    else if (PlatformInfo->PlatformMtl->RcState == ECabinetResourceState::CabRc_Complete)
    {
		if (ContainerPtr->IsClickedSameReplacingItem(InItemCode))
        	return;

		// 更新当前使用的台面ID
		CurrentPlatformId = PlatformInfo->Id;
			
		FArmyWHCabinetMgr * RESTRICT CabMgr = FArmyWHCabinetMgr::Get(); 
        CabMgr->SetPlatformInfo(PlatformInfo.Get());
        CabMgr->RefreshPlatformMtl();

		ContainerPtr->ListPanel->SetSelectedItem(InItemCode);

		if (PlatformInfo->FrontSections.Num() > 0)
		{
			// 同步更新材质绑定的第一个造型
			const TSharedPtr<FTableInfo> & TableInfo = PlatformInfo->FrontSections[0];
			CabMgr->SetFrontTrim(PlatformInfo->FrontSections[0].Get());
			// 刷新台面
			CabMgr->GenTableBoard();
			// 根据前沿的高度来更新台面上物体(比如水槽、灶具等)的高度
			CabMgr->UpdateShapeAccessoryLocation();
		}
    }
}

void FArmyWHCModePlatformAttr::Callback_ReplacingPlatformFront()
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
	if (!ContainerPtr->IsWHCItemSelected<AXRShapeTableActor>())
		return;

	FString DesiredState(TEXT("前沿型号"));
	if (ContainerPtr->IsCurrentState(DesiredState))
		return;
	ContainerPtr->ListPanel->ClearListController();
    if (ContainerPtr->ReplacingSlot->GetWidget() == SNullWidget::NullWidget)
        ContainerPtr->SetReplacingPanel(ContainerPtr->ListPanel.ToSharedRef());
    
    ContainerPtr->SetCurrentState(DesiredState);
	ContainerPtr->ClearReplacingList();
	LoadPlatformTrimData();
}

void FArmyWHCModePlatformAttr::LoadPlatformTrimData()
{
	FillPlatformFrontList();
}

void FArmyWHCModePlatformAttr::FillPlatformFrontList()
{
	TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();

	FPlatformInfo * PlatformInfo = FArmyWHCabinetMgr::Get()->GetPlatformInfo();
	if (PlatformInfo != nullptr)
	{
		FTableInfo * FrontInfo = FArmyWHCabinetMgr::Get()->GetFrontInfo();
		int32 FrontId = FrontInfo != nullptr ? FrontInfo->Id : 0;

		int32 MatchingIndex = -1;
		for (int32 i = 0; i < PlatformInfo->FrontSections.Num(); ++i)
		{
			const TSharedPtr<FTableInfo> & pTableInfo = PlatformInfo->FrontSections[i];
			ContainerPtr->AddReplacingId(pTableInfo->Id);
			TSharedPtr<SContentItem> Item = SNew(SContentItem)
				.ItemCode(i)
				.ItemWidth(112.0f)
				.ItemHeight(132.0f)
				.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
				.TextColor(FArmyStyle::Get().GetColor("Color.FFB5B7BE"))
				.ThumbnailURL(pTableInfo->ThumbnailUrl)
				.ThumbnailImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.DefaultImage_ContentBrowser")))//梁晓菲 默认图
				.bShowDownloadTips(pTableInfo->RcState != ECabinetResourceState::CabRc_Complete)
				.OnClicked_Raw(this, &FArmyWHCModePlatformAttr::Callback_ItemClicked)
				.DisplayText(FText::FromString(pTableInfo->Name));
			Item->AddMetadata<FWHCModeAttrTypeInfo>(MakeShared<FWHCModeAttrTypeInfo>(EWHCModeAttrType::EWHCAttr_PlatformFront, pTableInfo->Id));

			switch (pTableInfo->RcState)
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

			ContainerPtr->ListPanel->AddItem(Item);

			if (FrontId == pTableInfo->Id)
				MatchingIndex = i;
		}

		ContainerPtr->ListPanel->SetSelectedItem(MatchingIndex);
	}
}

void FArmyWHCModePlatformAttr::ReplacingPlatformFrontItemClicked(int32 InItemCode)
{
	TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
	
	if (!ContainerPtr->IsWHCItemSelected<AXRShapeTableActor>())
		return;

	if (!ContainerPtr->CheckIsValidReplacingIndex(InItemCode))
		return;

	FArmyWHCabinetMgr * RESTRICT CabMgr = FArmyWHCabinetMgr::Get();

	FPlatformInfo * PlatformInfo = CabMgr->GetPlatformInfo();
	check(PlatformInfo != nullptr);
	const TSharedPtr<FTableInfo> & TableInfo = PlatformInfo->FrontSections[InItemCode];

	if (TableInfo->RcState == ECabinetResourceState::CabRc_None)
	{
		FString CachePath = TableInfo->GetCachedPath();
		TArray<FDownloadSpace::FDownloadFileInfo> FileInfos;
		WHC_ADD_DOWNLOAD_TASK(FileInfos, TableInfo->Id, CachePath, TableInfo->cadUrl, FString());

		SContentItem * Item = ContainerPtr->GetListItem(InItemCode);
		if (FileInfos.Num() == 0)
		{
			SetDownloadPlatformFrontTrimResState(false, Item, TableInfo.Get());
			return;
		}
		if (Item)
			Item->SetItemState(EDownloading);
		TableInfo->RcState = ECabinetResourceState::CabRc_FetchingRc;

		TSharedPtr<FDownloadFileSet> NewTask = FArmyDownloadModule::Get().GetDownloadManager()->AddMultiTask(TableInfo->cadUrl, FileInfos);
		NewTask->OnDownloadFinished.BindLambda(
			[this](bool bSucceeded, FString InCachePath, int32 Id)
			{
				SContentItem * Item = Container.Pin()->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_PlatformFront);

				FPlatformInfo * PlatformInfo = FArmyWHCabinetMgr::Get()->GetPlatformInfo();
				check(PlatformInfo != nullptr);
				const TSharedPtr<FTableInfo> * TableInfoPtr = PlatformInfo->FrontSections.FindByPredicate([Id](const TSharedPtr<FTableInfo> & TableInfo) { return TableInfo->Id == Id; } );
				check(TableInfoPtr != nullptr);
				SetDownloadPlatformFrontTrimResState(bSucceeded, Item, (*TableInfoPtr).Get());
			},
			CachePath,
			TableInfo->Id
		);
	}
	else if (TableInfo->RcState == ECabinetResourceState::CabRc_Complete)
	{
		if (ContainerPtr->IsClickedSameReplacingItem(InItemCode))
        	return;

		CabMgr->SetFrontTrim(TableInfo.Get());
		// 刷新台面
		CabMgr->GenTableBoard();
		// 更新台面上物体的高度
		CabMgr->UpdateShapeAccessoryLocation();
		
		ContainerPtr->ListPanel->SetSelectedItem(InItemCode);
	}
}

void FArmyWHCModePlatformAttr::Callback_PlatformBackMenuItemSelectionChanged(const FString &InMenuItem)
{

}

void FArmyWHCModePlatformAttr::SetDownloadPlatformMtlResState(bool bSucceeded, SContentItem *UIItem, FMtlInfo *DataItem)
{
	if (UIItem)
        UIItem->SetItemState(bSucceeded ? EReady : ENotReady);
    if (DataItem)
        DataItem->RcState = bSucceeded ? ECabinetResourceState::CabRc_Complete : ECabinetResourceState::CabRc_None;
}

void FArmyWHCModePlatformAttr::SetDownloadPlatformFrontTrimResState(bool bSucceeded, SContentItem *UIItem, FTableInfo *DataItem)
{
	if (UIItem)
        UIItem->SetItemState(bSucceeded ? EReady : ENotReady);
    if (DataItem)
        DataItem->RcState = bSucceeded ? ECabinetResourceState::CabRc_Complete : ECabinetResourceState::CabRc_None;
}