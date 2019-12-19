#include "SArmyWHCModeWardrobeAttr.h"
#include "ArmyWHCabinet.h"
#include "ArmyShapeFrameActor.h"

#include "UIMixin/SArmyWHCAboveGroundUI.h"
#include "UIMixin/SArmyWHCCabDimensionUI.h"
#include "UIMixin/SArmyWHCCabMtlUI.h"
#include "UIMixin/SArmyWHCCabUI.h"
#include "UIMixin/SArmyWHCCabDoorUI.h"
#include "UIMixin/SArmyWHCSlidingDoorWardrobeBBoardUI.h"
#include "UIMixin/SArmyWHCDoorVisibilityUI.h"
#include "UIMixin/SArmyWHCSlidingwayUI.h"
#include "UIMixin/SArmyWHCDecorationBoardUI.h"

#include "Data/WHCModeData/XRWHCModeData.h"

#include "ArmyDetailBuilder.h"
#include "SContentItem.h"
#include "ArmyDownloadModule.h"

#include "SCTShape.h"
#include "UtilityTools/SCTShapeUtilityTools.h"

FArmyWHCModeWardrobeAttr::FArmyWHCModeWardrobeAttr(TSharedRef<SArmyWHCModeAttrPanelContainer> InContainer)
: FArmyWHCModeAttrPanel(InContainer)
{}

void FArmyWHCModeWardrobeAttr::TickPanel()
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    if (!ContainerPtr->IsWHCItemSelected<AXRShapeFrame>())
        return;

    const FWHCModeGlobalData::TCabinetInfoArr &CabinetInfoArr = *FWHCModeGlobalData::CabinetInfoArrCollection.Find((int32)ECabinetClassify::EClassify_Wardrobe);
	TArray<int32> CabIds = ContainerPtr->GetReplacingIds();
    TSharedPtr<FDownloadFileSet> TheTask;
	for (int32 i = 0; i < CabIds.Num(); ++i)
	{
        int32 Id = CabIds[i];

		SContentItem *ContentItem = ContainerPtr->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_Cab);
		if (ContentItem && ContentItem->GetItemState() == EDownloading)
		{
            const FCabinetInfo *CabinetInfoPtr = CabinetInfoArr.FindByPredicate(FCabinetInfoFunctor(Id));
            check(CabinetInfoPtr != nullptr);
            TheTask = FArmyDownloadModule::Get().GetDownloadManager()->GetTask(CabinetInfoPtr->DownloadTaskFlag);
            if (TheTask.IsValid())
            {
                UpdateProgress(TheTask, ContentItem);
                continue;
            }
        }

        ContentItem = ContainerPtr->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_CabMtl);
        if (ContentItem && ContentItem->GetItemState() == EDownloading)
        {
            const TSharedPtr<FMtlInfo> * CabinetMtlPtr = FWHCModeGlobalData::CabinetMtlMap.Find(Id);
            check(CabinetMtlPtr != nullptr);
            TheTask = FArmyDownloadModule::Get().GetDownloadManager()->GetTask((*CabinetMtlPtr)->MtlUrl);
            if (TheTask.IsValid())
            {
                UpdateProgress(TheTask, ContentItem);
                continue;
            }
		}

        ContentItem = ContainerPtr->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_DoorType);
        if (ContentItem && ContentItem->GetItemState() == EDownloading)
        {
            const TSharedPtr<FCoverDoorTypeInfo> * DoorTypeInfoPtr = FWHCModeGlobalData::SlidingDoorTypeMap.Find(Id);
            check(DoorTypeInfoPtr != nullptr);
            TheTask = FArmyDownloadModule::Get().GetDownloadManager()->GetTask((*DoorTypeInfoPtr)->DownloadTaskFlag);

            if (TheTask.IsValid())
            {
                UpdateProgress(TheTask, ContentItem);
                continue;
            }
        }
	}
}

void FArmyWHCModeWardrobeAttr::Initialize()
{
    SArmyWHCModeAttrPanelContainer * ContainerPtr = Container.Pin().Get();

    WardrobeDetailPanel = MakeShareable(new FArmyDetailBuilder);

    FArmyDetailNode &BasicParameters = WardrobeDetailPanel->AddCategory("BasicParameters", TEXT("基本参数"));

    FArmyWHCCabUI *CabUIComponent = MakeAttrUIComponent<FArmyWHCCabUI>(ContainerPtr, ComponentArr, (int32)ECabinetClassify::EClassify_Wardrobe);
    FArmyDetailNode &CabTypeNode = BasicParameters.AddChildNode("CabinetType", TEXT("柜体型号"), 
        CabUIComponent->MakeWidget()
    );
    CabTypeNode.NameContentOverride().VAlign(VAlign_Center);
    CabTypeNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyWHCCabMtlUI *CabMtlComponent = MakeAttrUIComponent<FArmyWHCCabMtlUI>(ContainerPtr, ComponentArr, false);
    FArmyDetailNode &CabMtlNode = BasicParameters.AddChildNode("CabinetMtl", TEXT("柜体材质"),
        CabMtlComponent->MakeWidget()
    );
    CabMtlNode.NameContentOverride().VAlign(VAlign_Center);
    CabMtlNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    MakeAttrUIComponent<FArmyWHCCabWidthUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &CabWidthNode = BasicParameters.AddChildNode("CabWidth", TEXT("宽度(mm)"));
    CabWidthNode.NameContentOverride().VAlign(VAlign_Center);
    CabWidthNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
    MakeAttrUIComponent<FArmyWHCCabDepthUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &CabDepthNode = BasicParameters.AddChildNode("CabDepth", TEXT("深度(mm)"));
    CabDepthNode.NameContentOverride().VAlign(VAlign_Center);
    CabDepthNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
    MakeAttrUIComponent<FArmyWHCCabHeightUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &CabHeightNode = BasicParameters.AddChildNode("CabHeight", TEXT("高度(mm)"));
    CabHeightNode.NameContentOverride().VAlign(VAlign_Center);
    CabHeightNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyWHCAboveGroundUI *AboveGroundComponent = MakeAttrUIComponent<FArmyWHCAboveGroundUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &AboveGroundNode = BasicParameters.AddChildNode("AboveGround", TEXT("离地高度"),
        AboveGroundComponent->MakeWidget()
    );
    AboveGroundNode.NameContentOverride().VAlign(VAlign_Center);
    AboveGroundNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyDetailNode &DoorParameters = WardrobeDetailPanel->AddCategory("DoorParameters", TEXT("柜体门板"));

    FArmyWHCCabDoorUI *CabDoorUIComponent = MakeAttrUIComponent<FArmyWHCCabDoorUI>(ContainerPtr, ComponentArr, 1); // 移门门板类型
    FArmyDetailNode &DoorTypeNode = DoorParameters.AddChildNode("CabinetDoorType", TEXT("门板型号"), 
        CabDoorUIComponent->MakeWidget()
    );
    DoorTypeNode.NameContentOverride().VAlign(VAlign_Center);
    DoorTypeNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyWHCSlidingwayUI *SlidingwayUIComponent = MakeAttrUIComponent<FArmyWHCSlidingwayUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &SlidingwayNode = DoorParameters.AddChildNode("Slidingway", TEXT("移门导轨"), 
        SlidingwayUIComponent->MakeWidget()
    );
    SlidingwayNode.NameContentOverride().VAlign(VAlign_Center);
    SlidingwayNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyWHCDoorVisibilityUI *DoorVisibilityUIComponent = MakeAttrUIComponent<FArmyWHCDoorVisibilityUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &DoorVisibilityNode = DoorParameters.AddChildNode("DoorVisibility", TEXT("隐藏门板"), 
        DoorVisibilityUIComponent->MakeWidget()
    );
    DoorVisibilityNode.NameContentOverride().VAlign(VAlign_Center);
    DoorVisibilityNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyDetailNode &FuncBoardParameters = WardrobeDetailPanel->AddCategory("FuncBoardParameters", TEXT("功能板件"));

    FArmyWHCSlidingDoorWardrobeBBoardUI *BBoardUIComponent = MakeAttrUIComponent<FArmyWHCSlidingDoorWardrobeBBoardUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &BottomBoardNode = FuncBoardParameters.AddChildNode("BottomBoard", TEXT("下垫板"), 
        BBoardUIComponent->MakeWidget()
    );
    BottomBoardNode.NameContentOverride().VAlign(VAlign_Center);
    BottomBoardNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    MakeAttrUIComponent<FArmyWHCDecorationBoardUI>(ContainerPtr, ComponentArr, 0);
    MakeAttrUIComponent<FArmyWHCDecorationBoardUI>(ContainerPtr, ComponentArr, 1);
}

bool FArmyWHCModeWardrobeAttr::TryDisplayAttrPanel(AActor *InActor)
{
    if (InActor->IsA<AXRShapeFrame>())
    {
        TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
        if (ContainerPtr->LastWHCItemSelected == InActor)
            return true;

        AXRShapeFrame * ShapeFrame = Cast<AXRShapeFrame>(InActor);
        FShapeInRoom * RESTRICT ShapeInRoom = ShapeFrame->ShapeInRoomRef;
        
        int32 ShapeCategory = ShapeInRoom->Shape->GetShapeCategory();
        if (ShapeCategory == (int32)ECabinetType::EType_SlidingDoorWardrobe)
        {
            ContainerPtr->LastWHCItemSelected = InActor;

            MakeDimensionUI();
            MakeDecorationBoardUI();

            ContainerPtr->SetAttrPanel(WardrobeDetailPanel->BuildDetail().ToSharedRef());

            return true;
        }
        else
            return false;
    }
    else
        return false;
}

void FArmyWHCModeWardrobeAttr::LoadMoreReplacingData()
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    const FString & CurrentState = ContainerPtr->GetCurrentState();
    if (CurrentState == TEXT("柜体型号"))
    {
        TArray<IArmyWHCAttrUIComponent*> CabUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_Cab, ComponentArr);
        FArmyWHCCabUI *CabUIComponent = (FArmyWHCCabUI *)CabUIComponents[0];
        CabUIComponent->LoadListData();
    }
    else if (CurrentState == TEXT("柜体材质"))
    {
        TArray<IArmyWHCAttrUIComponent*> CabMtlUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_CabMtl, ComponentArr);
        FArmyWHCCabMtlUI *CabMtlUIComponent = (FArmyWHCCabMtlUI *)CabMtlUIComponents[0];
        CabMtlUIComponent->LoadListData();
    }
    else if (CurrentState == TEXT("门板型号"))
    {
        TArray<IArmyWHCAttrUIComponent*> CabDoorUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_Door, ComponentArr);
        FArmyWHCCabDoorUI *CabDoorUIComponent = (FArmyWHCCabDoorUI *)CabDoorUIComponents[0];
        CabDoorUIComponent->LoadListData();
    }
    else if (CurrentState == TEXT("导轨型号"))
    {
        TArray<IArmyWHCAttrUIComponent*> SlidingwayUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_Slidingway, ComponentArr);
        FArmyWHCSlidingwayUI *SlidingwayUIComponent = (FArmyWHCSlidingwayUI *)SlidingwayUIComponents[0];
        SlidingwayUIComponent->LoadListData();
    }
    else if (CurrentState == TEXT("见光板材质"))
    {
        TArray<IArmyWHCAttrUIComponent*> DecBoardUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_DecorationBoard, ComponentArr);
        FArmyWHCDecorationBoardUI *DecBoardUIComponent = (FArmyWHCDecorationBoardUI *)DecBoardUIComponents[0];
        DecBoardUIComponent->LoadListData();
    }
}

void FArmyWHCModeWardrobeAttr::MakeDimensionUI()
{
    TSharedPtr<FArmyDetailNode> BasicParameters = WardrobeDetailPanel->EditCategory("BasicParameters");
    TSharedPtr<FArmyDetailNode> CabWidthNode = BasicParameters->FindChildNode("CabWidth");
    if (CabWidthNode.IsValid())
    {
        TArray<IArmyWHCAttrUIComponent*> CabWidthComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_WidthEditing, ComponentArr);
        CabWidthNode->ValueContentOverride()[CabWidthComponents[0]->MakeWidget().ToSharedRef()];
    }
    TSharedPtr<FArmyDetailNode> CabDepthNode = BasicParameters->FindChildNode("CabDepth");
    if (CabDepthNode.IsValid())
    {
        TArray<IArmyWHCAttrUIComponent*> CabDepthComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_DepthEditing, ComponentArr);
        CabDepthNode->ValueContentOverride()[CabDepthComponents[0]->MakeWidget().ToSharedRef()];
    }
    TSharedPtr<FArmyDetailNode> CabHeightNode = BasicParameters->FindChildNode("CabHeight");
    if (CabHeightNode.IsValid())
    {
        TArray<IArmyWHCAttrUIComponent*> CabHeightComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_HeightEditing, ComponentArr);
        CabHeightNode->ValueContentOverride()[CabHeightComponents[0]->MakeWidget().ToSharedRef()];
    }
}

void FArmyWHCModeWardrobeAttr::MakeDecorationBoardUI()
{
    AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(Container.Pin()->LastWHCItemSelected);
    check(ShapeFrame->ShapeInRoomRef != nullptr);
    TSharedPtr<FArmyDetailNode> FuncBoardParameters = WardrobeDetailPanel->EditCategory("FuncBoardParameters");
    check(FuncBoardParameters.IsValid());
    FuncBoardParameters->RemoveChildNode("LeftDecBoard");
    FuncBoardParameters->RemoveChildNode("RightDecBoard");

    TArray<IArmyWHCAttrUIComponent*> DecBoardUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_DecorationBoard, ComponentArr);

    FName Name;
    FString DisplayName;
    int32 Index = 0;
    TSharedPtr<FArmyDetailNode> DecBoardNode;

    for (const auto &BoardInfo : ShapeFrame->ShapeInRoomRef->DecBoardInfos)
    {
        switch ((FSCTShapeUtilityTool::EPosition)BoardInfo.Face)
        {
            case FSCTShapeUtilityTool::EPosition::E_Left:
            {
                Name = FName("LeftDecBoard");
                DisplayName = TEXT("左见光板");
                Index = 0;
                break;
            }
            case FSCTShapeUtilityTool::EPosition::E_Right:
            {
                Name = FName("RightDecBoard");
                DisplayName = TEXT("右见光板");
                Index = 1;
                break;
            }
        }

        FArmyWHCDecorationBoardUI *DecBoardUI = (FArmyWHCDecorationBoardUI *)(DecBoardUIComponents[Index]);
        DecBoardNode = FuncBoardParameters->AddChildNode(Name, DisplayName).AsShared();
        DecBoardNode->NameContentOverride().VAlign(VAlign_Center);
        if (BoardInfo.Spawned)
        {
            DecBoardUI->SetAddOrReplace(false);
            DecBoardNode->ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f))
            [DecBoardUI->MakeWidget().ToSharedRef()];
        }
        else
        {
            DecBoardUI->SetAddOrReplace(true);
            DecBoardNode->ValueContentOverride().HAlign(HAlign_Left).VAlign(VAlign_Fill)
            [DecBoardUI->MakeWidget().ToSharedRef()];
        }
    }
}

TSharedPtr<SWidget> FArmyWHCModeWardrobeAttr::RebuildAttrPanel()
{
    MakeDimensionUI();
    MakeDecorationBoardUI();
    return WardrobeDetailPanel->BuildDetail();
}