#include "SArmyWHCModeDoorAttr.h"
#include "SArmyWHCModeListPanel.h"
#include "ArmyWHCModeCabinetOperation.h"
#include "ArmyShapeFrameActor.h"
#include "ArmyWHCabinet.h"
#include "WHCMode/XRWHCShapeDoorChangeList.h"

#include "UIMixin/SArmyWHCCabDoorUI.h"
#include "UIMixin/SArmyWHCCabDoorMtlUI.h"
#include "UIMixin/SArmyWHCCabDoorHandleUI.h"
#include "UIMixin/SArmyWHCCabDimensionUI.h"

#include "Data/WHCModeData/XRWHCModeData.h"

#include "ArmyStyle.h"
#include "ArmyDetailBuilder.h"
#include "SContentItem.h"

#include "SCTDoorGroup.h"
#include "SCTDoorSheet.h"
#include "SCTAnimation.h"
#include "Actor/SCTShapeActor.h"


FArmyWHCModeDoorAttr::FArmyWHCModeDoorAttr(TSharedRef<class SArmyWHCModeAttrPanelContainer> InContainer)
: FArmyWHCModeAttrPanel(InContainer)
{}

void FArmyWHCModeDoorAttr::TickPanel()
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();

    const TArray<int32> &CabIds = ContainerPtr->GetReplacingIds();
    TSharedPtr<FDownloadFileSet> TheTask;
	for (int32 i = 0; i < CabIds.Num(); ++i)
	{
        int32 Id = CabIds[i];

		SContentItem *ContentItem = ContainerPtr->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_DoorType);
		if (ContentItem && ContentItem->GetItemState() == EDownloading)
		{
			const TSharedPtr<FCoverDoorTypeInfo> * CoverDoorTypeInfo = FWHCModeGlobalData::CoverDoorTypeMap.Find(Id);
            check(CoverDoorTypeInfo != nullptr);
            TheTask = FArmyDownloadModule::Get().GetDownloadManager()->GetTask((*CoverDoorTypeInfo)->DownloadTaskFlag);
            if (TheTask.IsValid())
            {
                // 设置下载进度
                UpdateProgress(TheTask, ContentItem);
                continue;
            }
        }

        ContentItem = ContainerPtr->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_DoorMtl);
		if (ContentItem && ContentItem->GetItemState() == EDownloading)
        {	
            const TSharedPtr<FMtlInfo> * DoorMtlPtr = FWHCModeGlobalData::CabinetMtlMap.Find(Id);
            check(DoorMtlPtr != nullptr);
            TheTask = FArmyDownloadModule::Get().GetDownloadManager()->GetTask((*DoorMtlPtr)->MtlUrl);
            if (TheTask.IsValid())
            {
                UpdateProgress(TheTask, ContentItem);
                continue;
            }
		}
	}
}

void FArmyWHCModeDoorAttr::Initialize()
{
    SArmyWHCModeAttrPanelContainer * ContainerPtr = Container.Pin().Get();

    DoorDetailPanel = MakeShareable(new FArmyDetailBuilder);

    FArmyDetailNode &BasicParameters = DoorDetailPanel->AddCategory("BasicParameters", TEXT("基本参数"));

    FArmyWHCDoorUI *DoorUIAttrComponent = MakeAttrUIComponent<FArmyWHCDoorUI>(ContainerPtr, ComponentArr, 2);
    FArmyDetailNode &DoorTypeNode = BasicParameters.AddChildNode("DoorType", TEXT("门板型号"), 
        DoorUIAttrComponent->MakeWidget()
    );
    DoorTypeNode.NameContentOverride().VAlign(VAlign_Center);
    DoorTypeNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyWHCDoorMtlUI *DoorMtlUIAttrComponent = MakeAttrUIComponent<FArmyWHCDoorMtlUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &DoorMtlNode = BasicParameters.AddChildNode("DoorMtl", TEXT("门板材质"), 
        DoorMtlUIAttrComponent->MakeWidget()
    );
    DoorMtlNode.NameContentOverride().VAlign(VAlign_Center);
    DoorMtlNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyWHCCabWidthStaticUI *StaticWidthUIAttrComponent = MakeAttrUIComponent<FArmyWHCCabWidthStaticUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &DoorWidthNode = BasicParameters.AddChildNode("DoorWidth", TEXT("宽度(mm)"), 
        StaticWidthUIAttrComponent->MakeWidget()
    );
    DoorWidthNode.NameContentOverride().VAlign(VAlign_Center);
    DoorWidthNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyWHCCabDepthStaticUI *StaticDepthUIAttrComponent = MakeAttrUIComponent<FArmyWHCCabDepthStaticUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &DoorDepthNode = BasicParameters.AddChildNode("DoorDepth", TEXT("厚度(mm)"), 
        StaticDepthUIAttrComponent->MakeWidget()
    );
    DoorDepthNode.NameContentOverride().VAlign(VAlign_Center);
    DoorDepthNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyWHCCabHeightStaticUI *StaticHeightUIAttrComponent = MakeAttrUIComponent<FArmyWHCCabHeightStaticUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &DoorHeightNode = BasicParameters.AddChildNode("DoorHeight", TEXT("高度(mm)"), 
        StaticHeightUIAttrComponent->MakeWidget()
    );
    DoorHeightNode.NameContentOverride().VAlign(VAlign_Center);
    DoorHeightNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyDetailNode &ConfigParameters = DoorDetailPanel->AddCategory("ConfigParameters", TEXT("门板配置"));

    FArmyWHCDoorHandleUI *DoorHandleUIAttrComponent = MakeAttrUIComponent<FArmyWHCDoorHandleUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &HandleNode = ConfigParameters.AddChildNode("DoorHandle", TEXT("拉手型号"), 
        DoorHandleUIAttrComponent->MakeWidget()
    );
    HandleNode.NameContentOverride().VAlign(VAlign_Center);
    HandleNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    SAssignNew(DoorOpenDirComboBox, SArmyWHCModeNormalComboBox)
    // .OptionSource(TArray<TSharedPtr<FString>>{ MakeShareable(new FString(TEXT("左开"))), MakeShareable(new FString(TEXT("右开"))) })
    // .Value(TEXT("左开"))
    .OnValueChanged_Raw(this, &FArmyWHCModeDoorAttr::Callback_DoorOpenDirValueChanged);	
    DoorOpenDirComboBox->SetValueAttr(
        TAttribute<FText>::Create(
            [this]() -> FText {
                TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
                if (ContainerPtr->IsWHCItemSelected<ASCTShapeActor>())
                {
                    ASCTShapeActor *ShapeActor = Cast<ASCTShapeActor>(ContainerPtr->LastWHCItemSelected);
                    FVariableAreaDoorSheet *DoorSheet = static_cast<FVariableAreaDoorSheet*>(ShapeActor->GetShape());
                    FVariableAreaDoorSheet::EDoorOpenDirection CurrentOpenDir = DoorSheet->GetOpenDoorDirection();
                    switch (CurrentOpenDir)
                    {
                        case FVariableAreaDoorSheet::EDoorOpenDirection::E_None:
                            DoorOpenDirComboBox->SetEnabled(true);
                            return FText::FromString(TEXT("不开"));
                        case FVariableAreaDoorSheet::EDoorOpenDirection::E_Left:
                            DoorOpenDirComboBox->SetEnabled(true);
                            return FText::FromString(TEXT("左开"));
                        case FVariableAreaDoorSheet::EDoorOpenDirection::E_Right:
                            DoorOpenDirComboBox->SetEnabled(true);
                            return FText::FromString(TEXT("右开"));
                        case FVariableAreaDoorSheet::EDoorOpenDirection::E_Top:
                            DoorOpenDirComboBox->SetEnabled(false);
                            return FText::FromString(TEXT("上翻"));
                    }
                }
                return FText::FromString(TEXT("无"));
            }
        )
    );
}

bool FArmyWHCModeDoorAttr::TryDisplayAttrPanel(AActor *InActor)
{   
    if (InActor->IsA<ASCTShapeActor>())
    {
        TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
        if (ContainerPtr->LastWHCItemSelected == InActor)
            return true;

        ASCTShapeActor * ShapeActor = Cast<ASCTShapeActor>(InActor);
        FSCTShape * Shape = ShapeActor->GetShape();
        if (Shape && Shape->GetShapeType() != ST_VariableAreaDoorSheet_ForSideHungDoor)
            return false;
    
        ContainerPtr->LastWHCItemSelected = InActor;
        
        AXRShapeFrame *ShapeFrame = ContainerPtr->CabinetOperationRef->GetShapeFrameFromSelected();
        int32 ShapeCategory = ShapeFrame->ShapeInRoomRef->Shape->GetShapeCategory();
        // 衣柜、系统柜、浴室柜不使用单独的门替换界面
        if (ShapeCategory == (int32)ECabinetType::EType_CoverDoorWardrobe || 
            ShapeCategory == (int32)ECabinetType::EType_SlidingDoorWardrobe || 
            ShapeCategory == (int32)ECabinetType::EType_TopCab || 
            ShapeCategory == (int32)ECabinetType::EType_Other || 
            ShapeCategory == (int32)ECabinetType::EType_BathroomOnGroundCab || 
            ShapeCategory == (int32)ECabinetType::EType_BathroomHangCab || 
            ShapeCategory == (int32)ECabinetType::EType_BathroomStorageCab || 
            ShapeCategory == (int32)ECabinetType::EType_Unknown)
            return false;

        FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor * DoorSheet = static_cast<FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor*>(Shape);
        if (DoorSheet->GetOpenDoorDirection() != FVariableAreaDoorSheet::EDoorOpenDirection::E_None)
            ContainerPtr->ShowInteractiveMessage(true);

        ContainerPtr->SetAttrPanel(RebuildAttrPanel().ToSharedRef());
            
        return true;
    }
    else
        return false;
}

void FArmyWHCModeDoorAttr::LoadMoreReplacingData()
{
    const FString & CurrentState = Container.Pin()->GetCurrentState();
    if (CurrentState == TEXT("门板型号"))
    {
        TArray<IArmyWHCAttrUIComponent*> DoorUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_Door, ComponentArr);
        FArmyWHCDoorUI *DoorUIComponent = (FArmyWHCDoorUI *)DoorUIComponents[0];
        DoorUIComponent->LoadListData();
    }
    else if (CurrentState == TEXT("门板材质"))
    {
        TArray<IArmyWHCAttrUIComponent*> DoorMtlUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_DoorMtl, ComponentArr);
        FArmyWHCDoorMtlUI *DoorMtlUIComponent = (FArmyWHCDoorMtlUI *)DoorMtlUIComponents[0];
        DoorMtlUIComponent->LoadListData();
    }
    else if (CurrentState == TEXT("拉手型号"))
    {
        TArray<IArmyWHCAttrUIComponent*> DoorHandleUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_DoorHandle, ComponentArr);
        FArmyWHCDoorHandleUI *DoorHandleUIComponent = (FArmyWHCDoorHandleUI *)DoorHandleUIComponents[0];
        DoorHandleUIComponent->LoadListData();
    }
}

TSharedPtr<SWidget> FArmyWHCModeDoorAttr::RebuildAttrPanel()
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    ASCTShapeActor *DoorSheetActor = Cast<ASCTShapeActor>(ContainerPtr->LastWHCItemSelected);
    FVariableAreaDoorSheet *DoorSheet = static_cast<FVariableAreaDoorSheet*>(DoorSheetActor->GetShape());
    FSCTShape *DoorGroup = DoorSheet->GetParentShape();
    FSideHungDoor *SideHungDoor = static_cast<FSideHungDoor*>(DoorGroup);

    TSharedPtr<FArmyDetailNode> * ConfigParamNode = DoorDetailPanel->GetChildrenNodes().Find("ConfigParameters");
    TSharedPtr<FArmyDetailNode> DoorOpenDirNode = (*ConfigParamNode)->FindChildNode("DoorOpenDir");

    // TArray<TSharedPtr<FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor>> & DoorSheets = SideHungDoor->GetDoorSheets();
    int32 OpenType = SideHungDoor->GetDoorSheetNum();
    if (OpenType == 2)
    {
        if (DoorOpenDirNode.IsValid())
            DoorOpenDirNode->SetNodeVisibility(EVisibility::Collapsed);
    }
    if (OpenType == 1)
    {
        if (DoorOpenDirNode.IsValid())
            DoorOpenDirNode->SetNodeVisibility(EVisibility::Visible);
        else
        {
            FArmyDetailNode &DoorOpenDirNode = (*ConfigParamNode)->AddChildNode("DoorOpenDir", TEXT("开门方向"),
                SNew(SBox)
                .HeightOverride(24.0f)
                [
                    DoorOpenDirComboBox.ToSharedRef()
                ]
            );
            DoorOpenDirNode.NameContentOverride().VAlign(VAlign_Center);
            DoorOpenDirNode.ValueContentOverride().VAlign(VAlign_Fill).HAlign(HAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
        }

        int32 DoorGroupId = SideHungDoor->GetShapeId();
        const TSharedPtr<FCoverDoorTypeInfo> *DoorTypeInfoPtr = FWHCModeGlobalData::CoverDoorTypeMap.Find(DoorGroupId);
        TArray<int32> OpenDirArr;
        if (DoorTypeInfoPtr != nullptr)
            OpenDirArr = (*DoorTypeInfoPtr)->SKUOpenDoorDirections;
        if (OpenDirArr.Num() == 0)
        {
            for (const auto &OpenDir : DoorSheet->GetAvailableOpenDirections())
                OpenDirArr.Emplace((int32)OpenDir);
        }
        TArray<TSharedPtr<FString>> OpenDirOptionSources;
        for (const auto & OpenDir : OpenDirArr)
        {
            switch (OpenDir)
            {
                case 0:
                    OpenDirOptionSources.Emplace(MakeShareable(new FString(TEXT("不开"))));
                    break;
                case 1:
                    OpenDirOptionSources.Emplace(MakeShareable(new FString(TEXT("左开"))));
                    break;
                case 2:
                    OpenDirOptionSources.Emplace(MakeShareable(new FString(TEXT("右开"))));
                    break;
                case 3:
                    OpenDirOptionSources.Emplace(MakeShareable(new FString(TEXT("上翻"))));
                    break;
            }
        }
        DoorOpenDirComboBox->SetOptionSource(OpenDirOptionSources);
    }

    return DoorDetailPanel->BuildDetail();
}

void FArmyWHCModeDoorAttr::Callback_DoorOpenDirValueChanged(const FString &InNewValue)
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    if (!ContainerPtr->IsWHCItemSelected<ASCTShapeActor>())
        return;

    ASCTShapeActor * DoorSheetActor = Cast<ASCTShapeActor>(ContainerPtr->LastWHCItemSelected);
    check(DoorSheetActor->GetShape()->GetShapeType() == ST_VariableAreaDoorSheet_ForSideHungDoor);

    FVariableAreaDoorSheet * DoorSheet = static_cast<FVariableAreaDoorSheet*>(DoorSheetActor->GetShape());
    FVariableAreaDoorSheet::EDoorOpenDirection DoorOpenDir = FVariableAreaDoorSheet::EDoorOpenDirection::E_None;
    if (InNewValue == TEXT("左开"))
        DoorOpenDir = FVariableAreaDoorSheet::EDoorOpenDirection::E_Left;
    else if (InNewValue == TEXT("右开"))
        DoorOpenDir = FVariableAreaDoorSheet::EDoorOpenDirection::E_Right;
    else if (InNewValue == TEXT("上翻"))
        DoorOpenDir = FVariableAreaDoorSheet::EDoorOpenDirection::E_Top;

    AXRShapeFrame * ShapeFrame = ContainerPtr->CabinetOperationRef->GetShapeFrameFromSelected();
    check(ShapeFrame != nullptr);
    for (auto & Animation : ShapeFrame->ShapeInRoomRef->Animations)
        Animation->Stop();
    DoorSheet->SetOpenDoorDirection((FVariableAreaDoorSheet::EDoorOpenDirection)DoorOpenDir);
    ShapeFrame->ShapeInRoomRef->PrepareShapeAnimations();

    FGuid Guid = DoorSheet->GetShapeCopyFromGuid();

    FArmyWHCDoorOpenDirChange * OpenDirChange = nullptr;
    for (const auto & Change : ShapeFrame->ShapeInRoomRef->ChangeList)
    {
        if (Change->IsSameType(SHAPE_CHANGE_DOOR_OPEN_DIR))
        {
            FArmyWHCDoorOpenDirChange * Temp = static_cast<FArmyWHCDoorOpenDirChange*>(Change.Get());
            if (Temp->GetDoorSheetGuid() == Guid)
            {
                OpenDirChange = Temp;
                break;
            }
        }
    }
    if (OpenDirChange == nullptr)
    {
        OpenDirChange = new FArmyWHCDoorOpenDirChange(ShapeFrame->ShapeInRoomRef->Shape.Get());
        OpenDirChange->SetDoorSheetGuid(Guid);
        ShapeFrame->ShapeInRoomRef->ChangeList.Emplace(MakeShareable(OpenDirChange));
    }
    OpenDirChange->SetDoorOpenDir((int32)DoorOpenDir);
}