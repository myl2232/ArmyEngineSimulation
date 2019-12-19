#include "SArmyWHCModeDrawerAttr.h"
#include "SArmyWHCModeListPanel.h"
#include "ArmyShapeFrameActor.h"
#include "ArmyWHCabinet.h"

#include "UIMixin/SArmyWHCCabDoorUI.h"
#include "UIMixin/SArmyWHCCabDoorMtlUI.h"
#include "UIMixin/SArmyWHCCabDoorHandleUI.h"
#include "UIMixin/SArmyWHCCabDimensionUI.h"

#include "Data/WHCModeData/XRWHCModeData.h"

#include "ArmyStyle.h"
#include "ArmyDetailBuilder.h"
#include "SContentItem.h"

#include "Actor/SCTShapeActor.h"
#include "SCTShape.h"

FArmyWHCModeDrawerAttr::FArmyWHCModeDrawerAttr(TSharedRef<class SArmyWHCModeAttrPanelContainer> InContainer)
: FArmyWHCModeAttrPanel(InContainer)
{}

void FArmyWHCModeDrawerAttr::TickPanel()
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
			const TSharedPtr<FCoverDoorTypeInfo> * DrawerTypeInfo = FWHCModeGlobalData::DrawerTypeMap.Find(Id);
            check(DrawerTypeInfo != nullptr);
            TheTask = FArmyDownloadModule::Get().GetDownloadManager()->GetTask((*DrawerTypeInfo)->DownloadTaskFlag);
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
            const TSharedPtr<FMtlInfo> * DrawerMtlPtr = FWHCModeGlobalData::CabinetMtlMap.Find(Id);
            check(DrawerMtlPtr != nullptr);
            TheTask = FArmyDownloadModule::Get().GetDownloadManager()->GetTask((*DrawerMtlPtr)->MtlUrl);
            if (TheTask.IsValid())
            {
                UpdateProgress(TheTask, ContentItem);
                continue;
            }
		}
	}
}

void FArmyWHCModeDrawerAttr::Initialize()
{
    SArmyWHCModeAttrPanelContainer * ContainerPtr = Container.Pin().Get();

    DrawerDetailPanel = MakeShareable(new FArmyDetailBuilder);

    FArmyDetailNode &BasicParameters = DrawerDetailPanel->AddCategory("BasicParameters", TEXT("基本参数"));

    FArmyWHCDoorUI *DoorUIAttrComponent = MakeAttrUIComponent<FArmyWHCDoorUI>(ContainerPtr, ComponentArr, 3);
    FArmyDetailNode &DrawerTypeNode = BasicParameters.AddChildNode("DrawerType", TEXT("抽面型号"), 
        DoorUIAttrComponent->MakeWidget()
    );
    DrawerTypeNode.NameContentOverride().VAlign(VAlign_Center);
    DrawerTypeNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyWHCDoorMtlUI *DoorMtlUIAttrComponent = MakeAttrUIComponent<FArmyWHCDoorMtlUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &DrawerMtlNode = BasicParameters.AddChildNode("DrawerMtl", TEXT("抽面材质"), 
        DoorMtlUIAttrComponent->MakeWidget()
    );
    DrawerMtlNode.NameContentOverride().VAlign(VAlign_Center);
    DrawerMtlNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyWHCDrawerWidthStaticUI *WidthUIComponent = MakeAttrUIComponent<FArmyWHCDrawerWidthStaticUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &DrawerWidthNode = BasicParameters.AddChildNode("DrawerWidth", TEXT("宽度(mm)"), 
        WidthUIComponent->MakeWidget()
    );
    DrawerWidthNode.NameContentOverride().VAlign(VAlign_Center);
    DrawerWidthNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyWHCDrawerDepthStaticUI *DepthUIComponent = MakeAttrUIComponent<FArmyWHCDrawerDepthStaticUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &DrawerDepthNode = BasicParameters.AddChildNode("DrawerDepth", TEXT("厚度(mm)"), 
        DepthUIComponent->MakeWidget()
    );
    DrawerDepthNode.NameContentOverride().VAlign(VAlign_Center);
    DrawerDepthNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyWHCDrawerHeightStaticUI *HeightUIComponent = MakeAttrUIComponent<FArmyWHCDrawerHeightStaticUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &DrawerHeightNode = BasicParameters.AddChildNode("DrawerHeight", TEXT("高度(mm)"), 
        HeightUIComponent->MakeWidget()
    );
    DrawerHeightNode.NameContentOverride().VAlign(VAlign_Center);
    DrawerHeightNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyDetailNode &ConfigParameters = DrawerDetailPanel->AddCategory("ConfigParameters", TEXT("抽面配置"));

    FArmyWHCDoorHandleUI *DoorHandleUIComponent = MakeAttrUIComponent<FArmyWHCDoorHandleUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &HandleNode = ConfigParameters.AddChildNode("DoorHandle", TEXT("拉手型号"), 
        DoorHandleUIComponent->MakeWidget()
    );
    HandleNode.NameContentOverride().VAlign(VAlign_Center);
    HandleNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
}

bool FArmyWHCModeDrawerAttr::TryDisplayAttrPanel(AActor *InActor)
{
    if (InActor->IsA<ASCTShapeActor>())
    {
        TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
        if (ContainerPtr->LastWHCItemSelected == InActor)
            return true;

        ASCTShapeActor * ShapeActor = Cast<ASCTShapeActor>(InActor);
        FSCTShape * Shape = ShapeActor->GetShape();
        if (Shape && Shape->GetShapeType() != ST_SlidingDrawer)
            return false;

        ContainerPtr->LastWHCItemSelected = InActor;

        AXRShapeFrame *ShapeFrame = ContainerPtr->CabinetOperationRef->GetShapeFrameFromSelected();
        int32 ShapeCategory = ShapeFrame->ShapeInRoomRef->Shape->GetShapeCategory();
        // 衣柜和系统柜不使用单独的门替换界面
        if (ShapeCategory == (int32)ECabinetType::EType_CoverDoorWardrobe || 
            ShapeCategory == (int32)ECabinetType::EType_SlidingDoorWardrobe || 
            ShapeCategory == (int32)ECabinetType::EType_TopCab || 
            ShapeCategory == (int32)ECabinetType::EType_Other || 
            ShapeCategory == (int32)ECabinetType::EType_BathroomOnGroundCab || 
            ShapeCategory == (int32)ECabinetType::EType_BathroomHangCab || 
            ShapeCategory == (int32)ECabinetType::EType_BathroomStorageCab || 
            ShapeCategory == (int32)ECabinetType::EType_Unknown)
            return false;
            
        ContainerPtr->ShowInteractiveMessage(true);

        ContainerPtr->SetAttrPanel(DrawerDetailPanel->BuildDetail().ToSharedRef());

        return true;
    }   
    else
        return false;
}

void FArmyWHCModeDrawerAttr::LoadMoreReplacingData()
{
    const FString & CurrentState = Container.Pin()->GetCurrentState();
    if (CurrentState == TEXT("抽面型号"))
    {
        TArray<IArmyWHCAttrUIComponent*> UIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_Door, ComponentArr);
        FArmyWHCDoorUI *DoorUIComponent = (FArmyWHCDoorUI *)UIComponents[0];
        DoorUIComponent->LoadListData();
    }
    else if (CurrentState == TEXT("抽面材质"))
    {
        TArray<IArmyWHCAttrUIComponent*> UIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_DoorMtl, ComponentArr);
        FArmyWHCDoorMtlUI *DoorMtlUIComponent = (FArmyWHCDoorMtlUI *)UIComponents[0];
        DoorMtlUIComponent->LoadListData();
    }
    else if (CurrentState == TEXT("拉手型号"))
    {
        TArray<IArmyWHCAttrUIComponent*> UIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_DoorHandle, ComponentArr);
        FArmyWHCDoorHandleUI *DoorHandleUIComponent = (FArmyWHCDoorHandleUI *)UIComponents[0];
        DoorHandleUIComponent->LoadListData();
    }
}