#include "SArmyWHCModeEmbbedElecDevAttr.h"
#include "SArmyWHCModeListPanel.h"
#include "ArmyWHCCabinetChangeList.h"
#include "UIMixin/SArmyWHCCabDimensionUI.h"

#include "Actor/XRActorConstant.h"
#include "ArmyStyle.h"
#include "ArmyDetailBuilder.h"
#include "SContentItem.h"
#include "ArmySlateModule.h"
#include "ArmyDownloadModule.h"
#include "ArmyHttpModule.h"
#include "SScrollWrapBox.h"
#include "WebImageCache.h"
#include "Data/WHCModeData/XRWHCModeData.h"

#include "SCTEmbededElectricalGroup.h"
#include "UtilityTools/SCTAccessShapeForBim.h"

FArmyWHCModeEmbbedElecDevAttr::FArmyWHCModeEmbbedElecDevAttr(TSharedRef<SArmyWHCModeAttrPanelContainer> InContainer)
: FArmyWHCModeAttrPanel(InContainer)
{}

void FArmyWHCModeEmbbedElecDevAttr::TickPanel()
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    TArray<int32> CabIds = ContainerPtr->GetReplacingIds();
    TSharedPtr<FDownloadFileSet> TheTask;
	for (int32 i = 0; i < CabIds.Num(); ++i)
    {
        int32 Id = CabIds[i];

        SContentItem *ContentItem = ContainerPtr->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_CabAcc);
        if (ContentItem && ContentItem->GetItemState() == EDownloading)
        {
            const TSharedPtr<FStandaloneCabAccInfo> * pAccInfo = FWHCModeGlobalData::StandaloneAccessoryMap.Find(Id);
            check(pAccInfo != nullptr);
            TheTask = FArmyDownloadModule::Get().GetDownloadManager()->GetTask((*pAccInfo)->AccShape->GetFileUrl());
            if (TheTask.IsValid())
            {
                // 设置下载进度
                UpdateProgress(TheTask, ContentItem);
                continue;
            }
        }
    }
}

void FArmyWHCModeEmbbedElecDevAttr::Initialize()
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();

    EmbbedElecDevDetailPanel = MakeShareable(new FArmyDetailBuilder);

    FArmyDetailNode &BasicParameters = EmbbedElecDevDetailPanel->AddCategory("BasicParameters", TEXT("基本参数"));

    FArmyDetailNode &TypeNode = BasicParameters.AddChildNode("Type", TEXT("电器型号"),
        SAssignNew(EmbbedElecDevReplacingPanel, SArmyWHCModeReplacingWidget)
        .ButtonImage(FArmyStyle::Get().GetBrush("WHCMode.Replace"))
        .OnWholeWidgetClicked_Raw(this, &FArmyWHCModeEmbbedElecDevAttr::Callback_ReplacingEmbbedElecDev)
        .OnButtonClicked_Raw(this, &FArmyWHCModeEmbbedElecDevAttr::Callback_ReplacingEmbbedElecDev)
    );
    TypeNode.NameContentOverride().VAlign(VAlign_Center);
    TypeNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    EmbbedElecDevReplacingPanel->SetReplacingItemThumbnail(TAttribute<const FSlateBrush*>::Create(
        [this]() -> const FSlateBrush* {
            TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
            if (ContainerPtr->IsWHCItemSelected<ASCTShapeActor>())
            {
                ASCTShapeActor *ShapeActor = static_cast<ASCTShapeActor*>(ContainerPtr->LastWHCItemSelected);
                return FArmySlateModule::Get().WebImageCache->Download(ShapeActor->GetShape()->GetThumbnailUrl())->GetBrush();
            }
            else
                return FCoreStyle::Get().GetDefaultBrush();
        }
    ));

    EmbbedElecDevReplacingPanel->SetReplacingItemText(TAttribute<FText>::Create(
        [this]() -> FText {
            TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
            if (ContainerPtr->IsWHCItemSelected<ASCTShapeActor>())
            {
                ASCTShapeActor *ShapeActor = static_cast<ASCTShapeActor*>(ContainerPtr->LastWHCItemSelected);
                return FText::FromString(ShapeActor->GetShape()->GetShapeName());
            }
            else
                return FText();
        }
    ));

    FArmyWHCCabWidthStaticUI *WidthStaticUIComponent = TXRWHCAttrUIComponent<FArmyWHCCabWidthStaticUI>::MakeComponentInstance(ContainerPtr.Get());
    FArmyDetailNode &EmbbedElecDevWidthNode = BasicParameters.AddChildNode("EmbbedElecDevWidth", TEXT("宽度(mm)"), 
        WidthStaticUIComponent->MakeWidget()
    );
    EmbbedElecDevWidthNode.NameContentOverride().VAlign(VAlign_Center);
    EmbbedElecDevWidthNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyWHCCabDepthStaticUI *DepthStaticUIComponent = TXRWHCAttrUIComponent<FArmyWHCCabDepthStaticUI>::MakeComponentInstance(ContainerPtr.Get());
    FArmyDetailNode &EmbbedElecDevDepthNode = BasicParameters.AddChildNode("EmbbedElecDevDepth", TEXT("深度(mm)"), 
        DepthStaticUIComponent->MakeWidget()
    );
    EmbbedElecDevDepthNode.NameContentOverride().VAlign(VAlign_Center);
    EmbbedElecDevDepthNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyWHCCabHeightStaticUI *HeightStaticUIComponent = TXRWHCAttrUIComponent<FArmyWHCCabHeightStaticUI>::MakeComponentInstance(ContainerPtr.Get());
    FArmyDetailNode &EmbbedElecDevHeightNode = BasicParameters.AddChildNode("EmbbedElecDevHeight", TEXT("高度(mm)"), 
        HeightStaticUIComponent->MakeWidget()
    );
    EmbbedElecDevHeightNode.NameContentOverride().VAlign(VAlign_Center);
    EmbbedElecDevHeightNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyDetailNode &ConfigParameters = EmbbedElecDevDetailPanel->AddCategory("ConfigParameters", TEXT("配置"));
    FArmyDetailNode &InsertionBoardNode = ConfigParameters.AddChildNode("InsertionBoardHeight", TEXT("饰条高度"), 
        SNew(SBox)
        .HeightOverride(24.0f)
        [
            SNew(SBorder)
            .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF38393D"))
            .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
            .Padding(FMargin(8.0f, 0.0f, 0.0f, 0.0f))
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text_Lambda(
                    [this]() -> FText {
                        TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
                        if (ContainerPtr->IsWHCItemSelected<ASCTShapeActor>())
                        {
                            ASCTShapeActor *ShapeActor = static_cast<ASCTShapeActor*>(ContainerPtr->LastWHCItemSelected);
                            FSCTShape *ParentShape = ShapeActor->GetShape()->GetParentShape();
                            check(ParentShape && ParentShape->GetShapeType() == ST_EmbeddedElectricalGroup);
                            FEmbededElectricalGroup *ElecGroup = static_cast<FEmbededElectricalGroup*>(ParentShape);
                            TSharedPtr<FInsertionBoard> InsertionBoard = ElecGroup->GetInsertionBoardShape();
                            return FText::FromString(FString::Printf(TEXT("%d"), InsertionBoard.IsValid() ? FMath::RoundToInt(InsertionBoard->GetShapeHeight()) : 0));
                        }
                        return FText::FromString(TEXT("0"));
                    }
                )
                .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
                .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FF969799"))
            ]
        ]
    );
    InsertionBoardNode.NameContentOverride().VAlign(VAlign_Center);
    InsertionBoardNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
}

bool FArmyWHCModeEmbbedElecDevAttr::TryDisplayAttrPanel(AActor *InActor)
{
    if (InActor->IsA<ASCTShapeActor>())
    {
        TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
        if (ContainerPtr->LastWHCItemSelected == InActor)
            return true;
        
        ASCTShapeActor *ShapeActor = Cast<ASCTShapeActor>(InActor);
        FSCTShape *Shape = ShapeActor->GetShape();
        if (Shape->GetShapeCategory() != (int32)EMetalsType::MT_EMBEDDED_APPLIANCE)
            return false;

        ContainerPtr->LastWHCItemSelected = InActor;

        ContainerPtr->SetAttrPanel(EmbbedElecDevDetailPanel->BuildDetail().ToSharedRef());

        return true;
    }
    else
        return false;
}

void FArmyWHCModeEmbbedElecDevAttr::LoadMoreReplacingData()
{
    const FString &CurrentState = Container.Pin()->GetCurrentState();
    if (CurrentState == TEXT("电器型号"))
        LoadEmbbedElecDevData();
}

void FArmyWHCModeEmbbedElecDevAttr::Callback_ItemClicked(int32 InItemCode)
{
    const FString &CurrentState = Container.Pin()->GetCurrentState();
    if (CurrentState == TEXT("电器型号"))
        ReplacingEmbbedElecDevItemClicked(InItemCode);
}

void FArmyWHCModeEmbbedElecDevAttr::Callback_ReplacingEmbbedElecDev()
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    if (!ContainerPtr->IsWHCItemSelected<ASCTShapeActor>())
        return;

    FString DesireState(TEXT("电器型号"));
    if (ContainerPtr->IsCurrentState(DesireState))
        return;

    ContainerPtr->ListPanel->ClearListController();
    if (ContainerPtr->ReplacingSlot->GetWidget() == SNullWidget::NullWidget)
        ContainerPtr->SetReplacingPanel(ContainerPtr->ListPanel.ToSharedRef());

    ContainerPtr->SetCurrentState(DesireState);

    ContainerPtr->ClearReplacingList();
    LoadEmbbedElecDevData();
}

void FArmyWHCModeEmbbedElecDevAttr::LoadEmbbedElecDevData()
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    if (ContainerPtr->CheckNoMoreData())
        return;

    ContainerPtr->RunHttpRequest(
        FString::Printf(
            TEXT("design/metals?metalsType=21&current=%d&size=15"), 
            ContainerPtr->GetPageIndexToLoad()
        ),
        TBaseDelegate<void, const TSharedPtr<FJsonObject>&>::CreateLambda(
            [this, ContainerPtr](const TSharedPtr<FJsonObject> &ResponseData)
            {
                const TSharedPtr<FJsonObject> &Data = ResponseData->GetObjectField(TEXT("data"));
                if (Data.IsValid())
                {
                    ContainerPtr->SetReplacingTotalPage(Data->GetNumberField(TEXT("totalPage")));
                    const TArray<TSharedPtr<FJsonValue>>* ElecDevArray = nullptr;
                    if (Data->TryGetArrayField(TEXT("list"), ElecDevArray))
                    {
                        int32 nCount = (*ElecDevArray).Num();
                        int32 IndexStart = ContainerPtr->GetReplacingIds().Num();

                        for (int32 i = 0; i < nCount; ++i)
                        {
                            const TSharedPtr<FJsonObject> &ElecDevJsonObj = (ElecDevArray->operator[] (i))->AsObject();
                            int32 Id = ElecDevJsonObj->GetNumberField(TEXT("id"));
                            ContainerPtr->AddReplacingId(Id);
                            if (FWHCModeGlobalData::StandaloneAccessoryMap.Find(Id) == nullptr)
                            {
                                FAccessShapeForBim *AccShapeForBim = new FAccessShapeForBim;
                                AccShapeForBim->ParseFromJson(ElecDevJsonObj);
                                FStandaloneCabAccInfo *NewAccInfo = new FStandaloneCabAccInfo;
                                NewAccInfo->AccShape = MakeShareable(AccShapeForBim);
                                FWHCModeGlobalData::StandaloneAccessoryMap.Emplace(Id, MakeShareable(NewAccInfo));
                            }
                        }

                        FillEmbbedElecDevList(IndexStart, nCount);
                    }
                }
            }
        )
    );
}

void FArmyWHCModeEmbbedElecDevAttr::FillEmbbedElecDevList(int32 InStart, int32 InCount)
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    if (!ContainerPtr->IsWHCItemSelected<ASCTShapeActor>())
        return;

    ASCTShapeActor *ShapeActor = Cast<ASCTShapeActor>(ContainerPtr->LastWHCItemSelected);
    int32 Id = ShapeActor->GetShape()->GetShapeId();

    int32 MatchingIndex = -1;
    TArray<int32> CabIds = ContainerPtr->GetReplacingIds();
    for (int32 i = 0; i < InCount; ++i)
	{
        const TSharedPtr<FStandaloneCabAccInfo> * AccessoryInfoPtr = FWHCModeGlobalData::StandaloneAccessoryMap.Find(CabIds[InStart + i]);
        check(AccessoryInfoPtr != nullptr);
        const TSharedPtr<FStandaloneCabAccInfo> & AccessoryInfo = *AccessoryInfoPtr;
		TSharedPtr<SContentItem> Item = SNew(SContentItem)
            .ItemCode(InStart + i)
            .ItemWidth(112.0f)
            .ItemHeight(132.0f)
			.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
			.TextColor(FArmyStyle::Get().GetColor("Color.FF969799"))
			.ThumbnailURL(AccessoryInfo->AccShape->GetThumbnailUrl())
			.ThumbnailImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.DefaultImage_ContentBrowser")))//梁晓菲 默认图
			.bShowDownloadTips(AccessoryInfo->CabRcState != ECabinetResourceState::CabRc_Complete)
			.OnClicked_Raw(this, &FArmyWHCModeEmbbedElecDevAttr::Callback_ItemClicked)
			.DisplayText(FText::FromString(AccessoryInfo->AccShape->GetShapeName()));
        Item->AddMetadata<FWHCModeAttrTypeInfo>(MakeShared<FWHCModeAttrTypeInfo>(EWHCModeAttrType::EWHCAttr_CabAcc, AccessoryInfo->AccShape->GetShapeId()));

        switch (AccessoryInfo->CabRcState)
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

        if (Id == AccessoryInfo->AccShape->GetShapeId())
            MatchingIndex = InStart + i;
    }

    ContainerPtr->ListPanel->SetSelectedItem(MatchingIndex);
    ContainerPtr->ListPanel->Container->ResetScrollNotify();
}

void FArmyWHCModeEmbbedElecDevAttr::Callback_DeleteEmbbedElecDev()
{

}

void FArmyWHCModeEmbbedElecDevAttr::ReplacingEmbbedElecDevItemClicked(int32 InItemCode)
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    if (!ContainerPtr->CheckIsValidReplacingIndex(InItemCode))
        return;

    if (!ContainerPtr->IsWHCItemSelected<ASCTShapeActor>())
        return;

    int32 Id = ContainerPtr->GetReplacingIds()[InItemCode];
    const TSharedPtr<FStandaloneCabAccInfo> *AccInfoPtr = FWHCModeGlobalData::StandaloneAccessoryMap.Find(Id);
    check(AccInfoPtr != nullptr);
    const TSharedPtr<FStandaloneCabAccInfo> &AccInfo = *AccInfoPtr;
    if (AccInfo->CabRcState == ECabinetResourceState::CabRc_None)
    {
        SContentItem *Item = ContainerPtr->GetListItem(InItemCode);
        if (Item)
            Item->SetItemState(EDownloading);
        AccInfo->CabRcState = ECabinetResourceState::CabRc_FetchingRc;

        TArray<FString> FileCachePaths;
        AccInfo->AccShape->GetFileCachePaths(FileCachePaths);
        check(FileCachePaths.Num() > 0);
        TArray<FDownloadSpace::FDownloadFileInfo> FileInfos;
        WHC_ADD_DOWNLOAD_TASK(FileInfos, Id, FileCachePaths[0], AccInfo->AccShape->GetFileUrl(), AccInfo->AccShape->GetFileMd5());
        if (FileInfos.Num() == 0)
        {
            SetDownloadAccResState(false, Item, AccInfo.Get());
            return;
        }
        TSharedPtr<FDownloadFileSet> NewTask = FArmyDownloadModule::Get().GetDownloadManager()->AddMultiTask(AccInfo->AccShape->GetFileUrl(), FileInfos);
        NewTask->OnDownloadFinished.BindLambda(
            [this, Id, ContainerPtr](bool bSucceeded) {
                const TSharedPtr<FStandaloneCabAccInfo> * AccInfoPtr = FWHCModeGlobalData::StandaloneAccessoryMap.Find(Id);
                check(AccInfoPtr != nullptr);

                SContentItem * ItemPtr = ContainerPtr->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_CabAcc);
                SetDownloadAccResState(bSucceeded, ItemPtr, (*AccInfoPtr).Get());
            }
        );
    }
    else if (AccInfo->CabRcState == ECabinetResourceState::CabRc_Complete)
    {
        if (ContainerPtr->IsClickedSameReplacingItem(InItemCode))
            return;

        ASCTShapeActor *EmbbedElecActor = Cast<ASCTShapeActor>(ContainerPtr->LastWHCItemSelected);
        FSCTShape *ParentShape = EmbbedElecActor->GetShape()->GetParentShape();
        check(ParentShape != nullptr);
        if (FSCTShapeUtilityTool::ReplaceEmbedElectricalGroup(ParentShape, AccInfo->AccShape.Get()))
        {
            FEmbededElectricalGroup *EmbbedElecGroup = static_cast<FEmbededElectricalGroup*>(ParentShape);
            ContainerPtr->LastWHCItemSelected = EmbbedElecGroup->GetEmbededEletricalShape()->GetShapeActor();
            ContainerPtr->CabinetOperationRef->UpdateLastSelected(ContainerPtr->LastWHCItemSelected);
            ContainerPtr->CabinetOperationRef->UpdateHighlight();

            AXRShapeFrame *ShapeFrame = ContainerPtr->CabinetOperationRef->GetShapeFrameFromSelected();
            check(ShapeFrame != nullptr);
            ShapeFrame->ShapeInRoomRef->MakeImmovableInternal(ParentShape->GetShapeActor());

            FArmyWHCEmbbedElecDevChange *Change = GetOrCreateEmbbedElecDevChange(ShapeFrame->ShapeInRoomRef);
            Change->SetElecDevId(AccInfo->AccShape->GetShapeId());
            Change->SetElecDevGroupGuid(ParentShape->GetShapeCopyFromGuid());
            FSCTShapeUtilityTool::SetActorTag(ParentShape->GetShapeActor(), XRActorTag::WHCActor);

            ContainerPtr->ListPanel->SetSelectedItem(InItemCode);
        }
    }
}

FArmyWHCEmbbedElecDevChange* FArmyWHCModeEmbbedElecDevAttr::GetOrCreateEmbbedElecDevChange(FShapeInRoom *InShapeInRoom) const
{
    for (const auto &Change : InShapeInRoom->ChangeList)
    {
        if (Change->IsSameType(SHAPE_CHANGE_EMBBED_ELEC_DEV))
        {
            return static_cast<FArmyWHCEmbbedElecDevChange*>(Change.Get());
        }
    }

    FArmyWHCEmbbedElecDevChange *Change = new FArmyWHCEmbbedElecDevChange(InShapeInRoom->Shape.Get());
    InShapeInRoom->ChangeList.Emplace(MakeShareable(Change));
    return Change;
}

void FArmyWHCModeEmbbedElecDevAttr::SetDownloadAccResState(bool bSucceeded, SContentItem *UIItem, FStandaloneCabAccInfo *DataItem)
{
    if (UIItem != nullptr)
        UIItem->SetItemState(bSucceeded ? EReady : ENotReady);
    if (DataItem != nullptr)
        DataItem->CabRcState = bSucceeded ? ECabinetResourceState::CabRc_Complete : ECabinetResourceState::CabRc_None;
}