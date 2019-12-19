#include "UIMixin/SArmyWHCCabMtlUI.h"
#include "WHCMode/SArmyWHCModeAttrPanel.h"
#include "WHCMode/SArmyWHCModeListPanel.h"
#include "WHCMode/XRShapeFrameActor.h"
#include "WHCMode/XRWHCabinet.h"
#include "WHCMode/XRWHCabinetAutoMgr.h"
#include "WHCMode/XRWHCModeCabinetOperation.h"
#include "WHCMode/XRWHCCabinetChangeList.h"
#include "../common/XRShapeUtilities.h"
#include "UIMixin/SArmyWHCModificationRangeUI.h"

#include "ArmyStyle.h"
#include "SContentItem.h"
#include "SScrollWrapBox.h"
#include "ArmySlateModule.h"
#include "ArmyDownloadModule.h"
#include "Data/WHCModeData/XRWHCModeData.h"

#include "SCTCabinetShape.h"
#include "SCTResManager.h"
#include "MaterialManager.h"
#include "UtilityTools/SCTShapeUtilityTools.h"

#include "WebImageCache.h"

TSharedPtr<SWidget> FArmyWHCCabMtlUI::MakeWidget()
{
    TSharedPtr<SArmyWHCModeReplacingWidget> CabMtlReplacing;
    SAssignNew(CabMtlReplacing, SArmyWHCModeReplacingWidget)
        .ButtonImage(FArmyStyle::Get().GetBrush("WHCMode.Replace"))
        .OnWholeWidgetClicked_Raw(this, &FArmyWHCCabMtlUI::Callback_ReplacingCabinetMtl)
        .OnButtonClicked_Raw(this, &FArmyWHCCabMtlUI::Callback_ReplacingCabinetMtl);
    CabMtlReplacing->SetReplacingItemThumbnail(TAttribute<const FSlateBrush*>::Create(
        [this]() -> const FSlateBrush* {
            AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
            if (ShapeFrame != nullptr)
            {
                check(ShapeFrame->ShapeInRoomRef != nullptr);

                int32 Id;
                FString Thumbnail, Name;
                UpdateCabMtlUI(ShapeFrame->ShapeInRoomRef, Id, Thumbnail, Name);
                return FArmySlateModule::Get().WebImageCache->Download(Thumbnail)->GetBrush();
            }
            else
                return FCoreStyle::Get().GetDefaultBrush();
        }
    ));
    CabMtlReplacing->SetReplacingItemText(TAttribute<FText>::Create(
        [this]() -> FText {
            AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
            if (ShapeFrame != nullptr)
            {
                check(ShapeFrame->ShapeInRoomRef != nullptr);

                int32 Id;
                FString Thumbnail, Name;
                UpdateCabMtlUI(ShapeFrame->ShapeInRoomRef, Id, Thumbnail, Name);
                return FText::FromString(Name);
            }
            else
            {
                return FText();
            }
        }
    ));
    SAssignNew(CabMtlCategoryComboBox, SArmyWHCModeNormalComboBox)
        .OptionSource(TArray<TSharedPtr<FString>> { MakeShareable(new FString(TEXT("全部"))) })
        .Value(TEXT("全部"))
        .OnValueChanged_Raw(this, &FArmyWHCCabMtlUI::Callback_CabinetMtlCategoryChanged);
    if (bNeedRangeReplacing)
    {
        MRangeUI = MakeShareable(TXRWHCAttrUIComponent<FArmyWHCModificationRangeUI>::MakeComponentInstance(Container, false));
        MRangeUI->MakeWidget();
        MRangeUI->SetMRange(SArmyWHCModeModificationRange::MR_All);
    }
    return MoveTemp(CabMtlReplacing);
}

void FArmyWHCCabMtlUI::LoadListData()
{
    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    if (ShapeFrame == nullptr)
        return;
    LoadCabinetMtlData();
}

void FArmyWHCCabMtlUI::Callback_ReplacingCabinetMtl()
{
    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    if (ShapeFrame == nullptr)
        return;

    FString DesireState(TEXT("柜体材质"));
    if (Container->IsCurrentState(DesireState))
        return;

    Container->ListPanel->ClearListController();
    TSharedPtr<SVerticalBox> ListController;
    Container->ListPanel->SetListController(
        SAssignNew(ListController, SVerticalBox)
    );
    if (bNeedRangeReplacing)
    {
        ListController->AddSlot().AutoHeight()
        [
            MRangeUI->MakeWidget().ToSharedRef()
        ];
    }
    ListController->AddSlot()
    .AutoHeight()
    .HAlign(HAlign_Left)
    .Padding(16.0f, 8.0f, 16.0f, 0.0f)
    [
        SNew(SBox)
        .WidthOverride(96.0f)
        .HeightOverride(32.0f)
        [
            CabMtlCategoryComboBox.ToSharedRef()
        ]
    ];
    
    if (Container->ReplacingSlot->GetWidget() == SNullWidget::NullWidget)
        Container->SetReplacingPanel(Container->ListPanel.ToSharedRef());
    
    Container->SetCurrentState(DesireState);

    CurrentMtlCategory = 0;
    Container->ClearReplacingList();
    LoadCabinetMtlCategoryData(1); // 1代表板材
    LoadCabinetMtlData();
}

void FArmyWHCCabMtlUI::Callback_ReplacingCabinetMtlItem(int32 InItemCode)
{
    if (!Container->CheckIsValidReplacingIndex(InItemCode))
        return;

    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    if (ShapeFrame == nullptr)
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
            SetDownloadCabMtlResState(false, Item, CabinetMtl.Get());
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
                        SetDownloadCabMtlResState(true, Item, CabinetMtl.Get());
                        return;
                    }
                }

                SetDownloadCabMtlResState(false, Item, CabinetMtl.Get());
            },
            CachePath,
            Id
        );
    }
    else if (CabinetMtl->RcState == ECabinetResourceState::CabRc_Complete)
    {
        check(ShapeFrame->ShapeInRoomRef != nullptr);

        if (bNeedRangeReplacing)
        {
            if (Container->IsClickedSameReplacingItem(InItemCode) && !MRangeUI->IsMRangeChanged())
                return;
            MRangeUI->SyncMRange();
            switch ((SArmyWHCModeModificationRange::EModificationRange)MRangeUI->GetMRange())
            {
                case SArmyWHCModeModificationRange::MR_SingleCab:
                {
                    ReplaceCabMtlInternal(ShapeFrame->ShapeInRoomRef, CabinetMtl.Get());
                    break;
                }
                case SArmyWHCModeModificationRange::MR_SameType:
                {
                    int32 ShapeCategory = ShapeFrame->ShapeInRoomRef->Shape->GetShapeCategory();
                    TArray<TSharedPtr<FShapeInRoom>> & PlacedShapes = FArmyWHCabinetMgr::Get()->GetPlaceShapes();
                    for (const auto & PlacedShape : PlacedShapes)
                    {
                        if (PlacedShape->Shape->GetShapeCategory() == ShapeCategory)
                            ReplaceCabMtlInternal(PlacedShape.Get(), CabinetMtl.Get());
                    }
                    break;
                }
                case SArmyWHCModeModificationRange::MR_All:
                {
                    TArray<TSharedPtr<FShapeInRoom>> & PlacedShapes = FArmyWHCabinetMgr::Get()->GetPlaceShapes();
                    for (const auto & PlacedShape : PlacedShapes)
                    {
                        ReplaceCabMtlInternal(PlacedShape.Get(), CabinetMtl.Get());
                    }
                    break;
                }
            }
        }
        else
        {
            if (Container->IsClickedSameReplacingItem(InItemCode))
                return;
            ReplaceCabMtlInternal(ShapeFrame->ShapeInRoomRef, CabinetMtl.Get());
        }

        Container->ListPanel->SetSelectedItem(InItemCode);
    }
}

void FArmyWHCCabMtlUI::Callback_CabinetMtlCategoryChanged(const FString &InNewCategory)
{
    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    if (ShapeFrame == nullptr)
        return;

    TArray<TPair<int32, FString>> * CabinetMtlCategory = CabMtlCategoryMap.Find(CurrentMtlType);
    check(CabinetMtlCategory != nullptr);
    for (auto & Category : *CabinetMtlCategory)
    {
        if (Category.Value == InNewCategory)
        {
            CurrentMtlCategory = Category.Key;
            CabMtlCategoryComboBox->SetValue(Category.Value);
            Container->ClearReplacingList();
            LoadCabinetMtlData();
            break;
        }
    }
}

void FArmyWHCCabMtlUI::LoadCabinetMtlData()
{
    if (Container->CheckNoMoreData())
        return;

    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    check(ShapeFrame->ShapeInRoomRef != nullptr);

    Container->RunHttpRequest(
        FString::Printf(TEXT("design/materials/cabinetsProduct/%d/materials?materialType=%d&categoryId=%d&current=%d&size=15"), 
            ShapeFrame->ShapeInRoomRef->Shape->GetShapeId(),
            CurrentMtlType, 
            CurrentMtlCategory, 
            Container->GetPageIndexToLoad()
        ),
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
                            const TSharedPtr<FJsonObject> &CabMtlJsonObject = Value->AsObject();

                            int32 Id = CabMtlJsonObject->GetNumberField(TEXT("id"));
                            Container->AddReplacingId(Id);

                            if (FWHCModeGlobalData::CabinetMtlMap.Find(Id) == nullptr)
                            {
                                FMtlInfo * NewCabMtlInfo = new FMtlInfo;
                                NewCabMtlInfo->Id = Id;
                                NewCabMtlInfo->MtlName = CabMtlJsonObject->GetStringField(TEXT("name"));
                                NewCabMtlInfo->MtlThumbnailUrl = CabMtlJsonObject->GetStringField(TEXT("thumbnailUrl"));
                                NewCabMtlInfo->MtlUrl = CabMtlJsonObject->GetStringField(TEXT("pakUrl"));
                                NewCabMtlInfo->MtlMd5 = CabMtlJsonObject->GetStringField(TEXT("pakMd5"));
                                NewCabMtlInfo->MtlParam = CabMtlJsonObject->GetStringField(TEXT("optimizeParam"));
                                FWHCModeGlobalData::CabinetMtlMap.Emplace(Id, MakeShareable(NewCabMtlInfo));
                            }
                        }
                        FillCabinetMtlList(IndexStart, nCount);
                    }
                }
            }
        )
    );
}

void FArmyWHCCabMtlUI::FillCabinetMtlList(int32 InStart, int32 InCount)
{
    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    if (ShapeFrame == nullptr)
        return;

    check(ShapeFrame->ShapeInRoomRef != nullptr);

    int32 MtlId = -1;
    FString Thumbnail, Name;
    UpdateCabMtlUI(ShapeFrame->ShapeInRoomRef, MtlId, Thumbnail, Name);

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
			.OnClicked_Raw(this, &FArmyWHCCabMtlUI::Callback_ReplacingCabinetMtlItem)
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

void FArmyWHCCabMtlUI::LoadCabinetMtlCategoryData(int32 InMtlType)
{
    CurrentMtlType = InMtlType;

    TArray<TPair<int32, FString>> * CabMtlCategory = CabMtlCategoryMap.Find(InMtlType);
    if (CabMtlCategory == nullptr)
    {
        Container->RunHttpRequest(FString::Printf(TEXT("design/materials/category/%d"), InMtlType),
            TBaseDelegate<void, const TSharedPtr<FJsonObject>&>::CreateLambda(
                [this](const TSharedPtr<FJsonObject> &ResponseData) 
                {
                    const TArray<TSharedPtr<FJsonValue>> & Categories = ResponseData->GetArrayField(TEXT("data"));
                    TArray<TPair<int32, FString>> CategoryStrings;
                    CategoryStrings.Emplace(TPairInitializer<int32, FString>(0, TEXT("全部")));
                    for (const auto & Category : Categories)
                    {
                        const TSharedPtr<FJsonObject> & CategoryJsonObj = Category->AsObject();
                        CategoryStrings.Emplace(TPairInitializer<int32, FString>(CategoryJsonObj->GetIntegerField(TEXT("id")), CategoryJsonObj->GetStringField(TEXT("name"))));
                    }
                    CabMtlCategoryMap.Emplace(CurrentMtlType, CategoryStrings);
                    FillCabinetMtlCategoryComboBox(CategoryStrings);
                }
            )
        );
    }
    else
        FillCabinetMtlCategoryComboBox(*CabMtlCategory);
}

void FArmyWHCCabMtlUI::FillCabinetMtlCategoryComboBox(const TArray<TPair<int32, FString>> &InCabMtlCategory)
{
    TArray<TSharedPtr<FString>> OptionSource;
    for (const auto & OneCategory : InCabMtlCategory)
        OptionSource.Emplace(MakeShareable(new FString(OneCategory.Value)));
    CabMtlCategoryComboBox->SetOptionSource(OptionSource);
}

void FArmyWHCCabMtlUI::ReplaceCabMtlInternal(FShapeInRoom *InShapeInRoom, FMtlInfo *InMtlInfo)
{
    FSCTShapeUtilityTool::ReplaceCabinetMaterial(InMtlInfo->Mtl, InShapeInRoom->Shape.Get());
    FArmyWHCCabinetMtlChange *CabMtlChange = GetOrCreateCabinetMtlChange(InShapeInRoom);
    CabMtlChange->SetCabinetMtlId(InMtlInfo->Id);
}

void FArmyWHCCabMtlUI::UpdateCabMtlUI(FShapeInRoom *InShapeInRoom, int32 &OutId, FString &OutThumbnailUrl, FString &OutName)
{
    OutId = 0;

    for (const auto &Change : InShapeInRoom->ChangeList)
    {
        if (Change->IsSameType(SHAPE_CHANGE_CABINET_MTL))
        {
            FArmyWHCCabinetMtlChange *CabMtlChange = static_cast<FArmyWHCCabinetMtlChange*>(Change.Get());
            OutId = CabMtlChange->GetCabinetMtlId();
            break;
        }
    }

    FText Text;
    if (OutId > 0)
    {
        const TSharedPtr<FMtlInfo> *MtlInfoPtr = FWHCModeGlobalData::CabinetMtlMap.Find(OutId);
        if (MtlInfoPtr)
        {
            const TSharedPtr<FMtlInfo> &MtlInfo = *MtlInfoPtr;
            OutThumbnailUrl = MtlInfo->MtlThumbnailUrl;
            OutName = MtlInfo->MtlName;
            return;
        }
    }

    FCabinetShape *CabShape = static_cast<FCabinetShape*>(InShapeInRoom->Shape.Get());

    TSharedPtr<FBoardShape> BoardShape = XRShapeUtilities::GetCabinetFrameBoard(CabShape, 2);
    if (!BoardShape.IsValid())
        BoardShape = XRShapeUtilities::GetCabinetFrameBoard(CabShape, 3);
    
    if (BoardShape.IsValid())
    {
        const FPakMetaData & MtlData = BoardShape->GetMaterialData();
        OutId = MtlData.ID;
        OutThumbnailUrl = MtlData.ThumbnailUrl;
        OutName = MtlData.Name;
    }
}

FArmyWHCCabinetMtlChange* FArmyWHCCabMtlUI::GetOrCreateCabinetMtlChange(FShapeInRoom *InShapeInRoom) const
{
    FArmyWHCCabinetMtlChange * CabinetMtlChange = nullptr;
    for (const auto & Change : InShapeInRoom->ChangeList)
    {
        if (Change->IsSameType(SHAPE_CHANGE_CABINET_MTL))
        {
            CabinetMtlChange = static_cast<FArmyWHCCabinetMtlChange*>(Change.Get());
            break;
        }
    }
    if (CabinetMtlChange == nullptr)
    {
        CabinetMtlChange = new FArmyWHCCabinetMtlChange(InShapeInRoom->Shape.Get());
        InShapeInRoom->ChangeList.Emplace(MakeShareable(CabinetMtlChange));
    }
    return CabinetMtlChange;
}

void FArmyWHCCabMtlUI::SetDownloadCabMtlResState(bool bSucceeded, SContentItem *UIItem, FMtlInfo *DataItem)
{
    if (UIItem != nullptr)
        UIItem->SetItemState(bSucceeded ? EReady : ENotReady);
    if (DataItem != nullptr)
        DataItem->RcState = bSucceeded ? ECabinetResourceState::CabRc_Complete : ECabinetResourceState::CabRc_None;
}