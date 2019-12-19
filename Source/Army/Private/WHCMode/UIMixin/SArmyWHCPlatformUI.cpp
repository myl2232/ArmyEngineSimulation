#include "UIMixin/SArmyWHCPlatformUI.h"
#include "SArmyWHCModeAttrPanel.h"
#include "SArmyWHCModeListPanel.h"
#include "ArmyWHCabinet.h"

#include "ArmyStyle.h"
#include "SContentItem.h"
#include "SScrollWrapBox.h"
#include "ArmySlateModule.h"
#include "ArmyDownloadModule.h"
#include "Data/WHCModeData/XRWHCModeData.h"

#include "WebImageCache.h"

TSharedPtr<SWidget> FArmyWHCPlatformUI::MakeWidget()
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
            .OnClicked_Raw(this, &FArmyWHCPlatformUI::Callback_AddPlatform)
            [
                SNew(SImage)
                .Image(FArmyStyle::Get().GetBrush("Icon.Add"))
            ]
        ]; 
    }
    else
    {
        TSharedPtr<SArmyWHCModeReplacingWidget> ReplacingWidget;
        SAssignNew(ReplacingWidget, SArmyWHCModeReplacingWidget)
            .ButtonImage(FArmyStyle::Get().GetBrush("Icon.Delete"))
            .OnWholeWidgetClicked_Raw(this, &FArmyWHCPlatformUI::Callback_ReplacingPlatform)
            .OnButtonClicked_Raw(this, &FArmyWHCPlatformUI::Callback_DeletePlatform);
        ReplacingWidget->SetReplacingItemThumbnail(TAttribute<const FSlateBrush*>::Create(
            [this]() -> const FSlateBrush* {
                if (Container->IsWHCItemSelected<AXRShapeFrame>())
                {
                    AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
                    check(ShapeFrame->ShapeInRoomRef != nullptr);
					XRCabinetActorT<XRWHCabinetPlatformInfoComponent> PlatformInfoComponent = ShapeFrame->ShapeInRoomRef->CabinetActor;
					TSharedPtr<FPlatformInfo> PlatformInfo = PlatformInfoComponent->Get();
					if (PlatformInfo.IsValid())
					{
						if (PlatformInfo->PlatformMtl.IsValid())
							return FArmySlateModule::Get().WebImageCache->Download(PlatformInfo->PlatformMtl->MtlThumbnailUrl)->GetBrush();
					}
                }
                return FCoreStyle::Get().GetDefaultBrush();
            }
        ));
        ReplacingWidget->SetReplacingItemText(TAttribute<FText>::Create(
            [this]() -> FText {
                if (Container->IsWHCItemSelected<AXRShapeFrame>())
                {
                    AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
                    check(ShapeFrame->ShapeInRoomRef != nullptr);
					XRCabinetActorT<XRWHCabinetPlatformInfoComponent> PlatformInfoComponent = ShapeFrame->ShapeInRoomRef->CabinetActor;
					TSharedPtr<FPlatformInfo> PlatformInfo = PlatformInfoComponent->Get();
					if (PlatformInfo.IsValid())
					{
						if (PlatformInfo->PlatformMtl.IsValid())
							return FText::FromString(PlatformInfo->PlatformMtl->MtlName);
					}
                }
                return FText();
            }
        ));
        return MoveTemp(ReplacingWidget);
    }
}

void FArmyWHCPlatformUI::LoadListData()
{
    if (!Container->IsWHCItemSelected<AXRShapeFrame>())
        return;
    LoadPlatformData();
}

void FArmyWHCPlatformUI::Callback_ReplacingPlatform()
{
    if (!Container->IsWHCItemSelected<AXRShapeFrame>())
        return;

    FString DesireState(TEXT("台面材质"));
    if (Container->IsCurrentState(DesireState))
        return;   

    Container->ListPanel->ClearListController();
    if (Container->ReplacingSlot->GetWidget() == SNullWidget::NullWidget)
        Container->SetReplacingPanel(Container->ListPanel.ToSharedRef());

    Container->SetCurrentState(DesireState);

    Container->ClearReplacingList();
    LoadPlatformData();
}

void FArmyWHCPlatformUI::Callback_ReplacingPlatformItem(int32 InItemCode)
{
    if (!Container->CheckIsValidReplacingIndex(InItemCode))
        return;
    if (!Container->IsWHCItemSelected<AXRShapeFrame>())
        return;
    
    int32 Id = Container->GetReplacingIds()[InItemCode];
    const TSharedPtr<FPlatformInfo> *PlatformInfoPtr = FWHCModeGlobalData::PlatformMap.Find(Id);
    check(PlatformInfoPtr != nullptr);
    const TSharedPtr<FPlatformInfo> &PlatformInfo = *PlatformInfoPtr;
    check(PlatformInfo->PlatformMtl.IsValid());
    if (PlatformInfo->PlatformMtl->RcState == ECabinetResourceState::CabRc_Complete)
    {
        if (Container->IsClickedSameReplacingItem(InItemCode) && !IsAdd())
            return;

        AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
        check(ShapeFrame->ShapeInRoomRef != nullptr);
        bool bReplacingOperation = ShapeFrame->ShapeInRoomRef->IsPlatformSpawned();
        ShapeFrame->ShapeInRoomRef->InitializePlatform(PlatformInfo);
        ShapeFrame->ShapeInRoomRef->SpawnPlatform();
        Container->ListPanel->SetSelectedItem(InItemCode);

        if (!bReplacingOperation)
        {
            SetAddOrReplace(false);
            Container->NotifyAttrPanelNeedRebuild();
        }
    }
    else if (PlatformInfo->PlatformMtl->RcState == ECabinetResourceState::CabRc_None)
    {
        TArray<FDownloadSpace::FDownloadFileInfo> FileInfos;
        WHC_ADD_DOWNLOAD_TASK(FileInfos, Id, PlatformInfo->PlatformMtl->GetCachedPath(), PlatformInfo->PlatformMtl->MtlUrl, PlatformInfo->PlatformMtl->MtlMd5);

        if (PlatformInfo->FrontSections.Num() > 0)
        {
            const TSharedPtr<FTableInfo> &TableInfo = PlatformInfo->FrontSections[0];
            WHC_ADD_DOWNLOAD_TASK(FileInfos, 
                TableInfo->Id,
                TableInfo->GetCachedPath(),
                TableInfo->cadUrl,
                FString());
        }

        if (PlatformInfo->BackSections.Num() > 0)
        {
            const TSharedPtr<FTableInfo> &TableInfo = PlatformInfo->BackSections[0];
            WHC_ADD_DOWNLOAD_TASK(FileInfos, 
                TableInfo->Id,
                TableInfo->GetCachedPath(),
                TableInfo->cadUrl,
                FString());
        }

        SContentItem * ItemPtr = Container->GetListItem(InItemCode);
        if (FileInfos.Num() == 0)
        {
            SetDownloadPlatformResState(false, ItemPtr, PlatformInfo->PlatformMtl.Get());
            return;
        }
        if (ItemPtr)
            ItemPtr->SetItemState(EDownloading);
        PlatformInfo->PlatformMtl->RcState = ECabinetResourceState::CabRc_FetchingRc;

        TSharedPtr<FDownloadFileSet> NewTask = FArmyDownloadModule::Get().GetDownloadManager()->AddMultiTask(PlatformInfo->PlatformMtl->MtlUrl, FileInfos);
        NewTask->OnDownloadFinished.BindLambda(
            [this, Id](bool bSucceeded)
            {
                SContentItem * ItemPtr = Container->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_PlatformMtl);

                const TSharedPtr<FPlatformInfo> * PlatformInfoPtr = FWHCModeGlobalData::PlatformMap.Find(Id);
                check(PlatformInfoPtr != nullptr);
                const TSharedPtr<FPlatformInfo> & PlatformInfo = *PlatformInfoPtr;

                if (bSucceeded)
                {
                    if (PlatformInfo->PlatformMtl->Mtl == nullptr)
                        PlatformInfo->PlatformMtl->Mtl = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(PlatformInfo->PlatformMtl->GetCachedPath(), PlatformInfo->PlatformMtl->MtlParam);

                    if (PlatformInfo->PlatformMtl->Mtl != nullptr)
                    {
                        PlatformInfo->PlatformMtl->Mtl->AddToRoot();
                        SetDownloadPlatformResState(true, ItemPtr, PlatformInfo->PlatformMtl.Get());
                        return;
                    }
                }

                SetDownloadPlatformResState(false, ItemPtr, PlatformInfo->PlatformMtl.Get());
            }
        );
    }
}

FReply FArmyWHCPlatformUI::Callback_AddPlatform()
{
    Callback_ReplacingPlatform();
    return FReply::Handled();
}

void FArmyWHCPlatformUI::Callback_DeletePlatform()
{
    if (Container->IsWHCItemSelected<AXRShapeFrame>())
    {
        AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
        check(ShapeFrame->ShapeInRoomRef != nullptr);
        ShapeFrame->ShapeInRoomRef->DestroyPlatform();
        SetAddOrReplace(true);
        Container->NotifyAttrPanelNeedRebuild();
    }
}

void FArmyWHCPlatformUI::LoadPlatformData()
{
    if (Container->CheckNoMoreData())
		return;

	Container->RunHttpRequest(
		FString::Printf(TEXT("design/worktops?current=%d&size=15"), Container->GetPageIndexToLoad()),
		TBaseDelegate<void, const TSharedPtr<FJsonObject>&>::CreateLambda(
			[this](const TSharedPtr<FJsonObject> &ResponseData)
			{
				const TSharedPtr<FJsonObject> &Data = ResponseData->GetObjectField(TEXT("data"));
				Container->SetReplacingTotalPage(Data->GetNumberField(TEXT("totalPage")));

				const TArray<TSharedPtr<FJsonValue>>* PlatformArray = nullptr;
				if (Data->TryGetArrayField(TEXT("list"), PlatformArray))
				{
					int32 nCount = (*PlatformArray).Num();
					int32 IndexStart = Container->GetReplacingIds().Num();
					for (int32 i = 0; i < nCount; ++i)
					{
						const TSharedPtr<FJsonValue> &Value = PlatformArray->operator[](i);
						const TSharedPtr<FJsonObject> &PlatformJsonObject = Value->AsObject();

						int32 Id = PlatformJsonObject->GetNumberField(TEXT("id"));
						Container->AddReplacingId(Id);

						if (FWHCModeGlobalData::PlatformMap.Find(Id) == nullptr)
						{
							FPlatformInfo * NewPlatformInfo = new FPlatformInfo;
                            NewPlatformInfo->Deserialize(PlatformJsonObject);
							FWHCModeGlobalData::PlatformMap.Emplace(Id, MakeShareable(NewPlatformInfo));
						}

					}

					FillPlatformList(IndexStart, nCount);
				}
			}
		)
	);
}

void FArmyWHCPlatformUI::FillPlatformList(int32 InStart, int32 InCount)
{
    if (!Container->IsWHCItemSelected<AXRShapeFrame>())
        return;

    AXRShapeFrame * ShapeFrame = Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
    check(ShapeFrame->ShapeInRoomRef != nullptr);
	XRCabinetActorT<XRWHCabinetPlatformInfoComponent> PlatformInfoComponent = ShapeFrame->ShapeInRoomRef->CabinetActor;
	TSharedPtr<FPlatformInfo> PlatformInfo = PlatformInfoComponent->Get();
	if (!PlatformInfo.IsValid())
		return;
    int32 PlatformId = PlatformInfo->Id;

    int32 MatchingIndex = -1;
    TArray<int32> CabIds = Container->GetReplacingIds();
    for (int32 i = 0; i < InCount; ++i)
	{
        const TSharedPtr<FPlatformInfo> * PlatformInfoPtr = FWHCModeGlobalData::PlatformMap.Find(CabIds[InStart + i]);
        check(PlatformInfoPtr != nullptr);
        const TSharedPtr<FPlatformInfo> & PlatformInfo = *PlatformInfoPtr;
        bool bHasValidMtl = PlatformInfo->PlatformMtl.IsValid();
        TSharedPtr<SContentItem> Item = SNew(SContentItem)
        .ItemCode(InStart + i)
        .ItemWidth(112.0f)
        .ItemHeight(132.0f)
        .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
        .TextColor(FArmyStyle::Get().GetColor("Color.FF969799"))
        .ThumbnailURL(bHasValidMtl ? PlatformInfo->PlatformMtl->MtlThumbnailUrl : TEXT(""))
        .ThumbnailImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.DefaultImage_ContentBrowser")))//梁晓菲 默认图
        .bShowDownloadTips(bHasValidMtl ? PlatformInfo->PlatformMtl->Mtl == nullptr : false)
        .OnClicked_Raw(this, &FArmyWHCPlatformUI::Callback_ReplacingPlatformItem)
        .DisplayText(bHasValidMtl ? FText::FromString(PlatformInfo->PlatformMtl->MtlName) : FText());
        if (bHasValidMtl)
        {
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
        }
        else
            Item->SetItemState(ENotReady);
    
        Container->ListPanel->AddItem(Item);

        if (PlatformInfo->Id == PlatformId)
            MatchingIndex = InStart + i;
	}

    Container->ListPanel->SetSelectedItem(MatchingIndex);
	Container->ListPanel->Container->ResetScrollNotify();
}

void FArmyWHCPlatformUI::SetDownloadPlatformResState(bool bSucceeded, SContentItem *UIItem, FMtlInfo *DataItem)
{
    if (UIItem)
        UIItem->SetItemState(bSucceeded ? EReady : ENotReady);
    if (DataItem)
        DataItem->RcState = bSucceeded ? ECabinetResourceState::CabRc_Complete : ECabinetResourceState::CabRc_None;
}