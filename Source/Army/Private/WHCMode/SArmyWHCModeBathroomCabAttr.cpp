#include "SArmyWHCModeBathroomCabAttr.h"
#include "ArmyWHCabinet.h"
#include "ArmyShapeFrameActor.h"

#include "UIMixin/SArmyWHCAboveGroundUI.h"
#include "UIMixin/SArmyWHCCabDimensionUI.h"
#include "UIMixin/SArmyWHCCabMtlUI.h"
#include "UIMixin/SArmyWHCCabUI.h"
#include "UIMixin/SArmyWHCDecorationBoardUI.h"

#include "UIMixin/SArmyWHCCabDoorUI.h"
#include "UIMixin/SArmyWHCCabDoorMtlUI.h"
#include "UIMixin/SArmyWHCCabDoorHandleUI.h"

#include "Data/WHCModeData/XRWHCModeData.h"

#include "ArmyDetailBuilder.h"
#include "SContentItem.h"
#include "ArmyWHTextRenderActor.h"
#include "SCTShapeActor.h"
#include "SCTDoorGroup.h"
#include "SArmyWHCDoorVisibilityUI.h"


FArmyWHCModeBathroomCabAttr::FArmyWHCModeBathroomCabAttr(TSharedRef<SArmyWHCModeAttrPanelContainer> InContainer)
: FArmyWHCModeAttrPanel(InContainer)
{}

void FArmyWHCModeBathroomCabAttr::TickPanel()
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    if (!ContainerPtr->IsWHCItemSelected<AXRShapeFrame>())
        return;

    const FWHCModeGlobalData::TCabinetInfoArr &CabinetInfoArr = *FWHCModeGlobalData::CabinetInfoArrCollection.Find((int32)ECabinetClassify::EClassify_BathroomCab);
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
    }
}

void FArmyWHCModeBathroomCabAttr::Initialize()
{
    SArmyWHCModeAttrPanelContainer * ContainerPtr = Container.Pin().Get();

    BathroomCabDetailPanel = MakeShareable(new FArmyDetailBuilder);

    FArmyDetailNode &BasicParameters = BathroomCabDetailPanel->AddCategory("BasicParameters", TEXT("基本参数"));

    FArmyWHCBathroomCabUI *CabUIComponent = MakeAttrUIComponent<FArmyWHCBathroomCabUI>(ContainerPtr, ComponentArr, (int32)ECabinetClassify::EClassify_BathroomCab);
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

    MakeAttrUIComponent<FArmyWHCDecorationBoardUI>(ContainerPtr, ComponentArr, 0);
    MakeAttrUIComponent<FArmyWHCDecorationBoardUI>(ContainerPtr, ComponentArr, 1);
}

bool FArmyWHCModeBathroomCabAttr::TryDisplayAttrPanel(AActor *InActor)
{
    if (InActor->IsA<AXRShapeFrame>())
    {
        TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
        if (ContainerPtr->LastWHCItemSelected == InActor)
            return true;

        AXRShapeFrame * ShapeFrame = Cast<AXRShapeFrame>(InActor);
        FShapeInRoom * RESTRICT ShapeInRoom = ShapeFrame->ShapeInRoomRef;
        
        int32 ShapeCategory = ShapeInRoom->Shape->GetShapeCategory();
        if (ShapeCategory == (int32)ECabinetType::EType_BathroomOnGroundCab || 
            ShapeCategory == (int32)ECabinetType::EType_BathroomHangCab ||
            ShapeCategory == (int32)ECabinetType::EType_BathroomStorageCab)
        {
            ContainerPtr->LastWHCItemSelected = InActor;

            MakeDimensionUI();
            MakeDecorationBoardUI();
			MakeDoorPlankAttributesUI(ShapeInRoom);
			MakeDrawerFaceModel(ShapeInRoom);
            ContainerPtr->SetAttrPanel(BathroomCabDetailPanel->BuildDetail().ToSharedRef());

            return true;
        }
        else
            return false;
    }
    else
        return false;
}

void FArmyWHCModeBathroomCabAttr::LoadMoreReplacingData()
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
    else if (CurrentState == TEXT("见光板材质"))
    {
        TArray<IArmyWHCAttrUIComponent*> DecBoardUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_DecorationBoard, ComponentArr);
        FArmyWHCDecorationBoardUI *DecBoardUIComponent = (FArmyWHCDecorationBoardUI *)DecBoardUIComponents[0];
        DecBoardUIComponent->LoadListData();
    }
}

void FArmyWHCModeBathroomCabAttr::MakeDimensionUI()
{
    TSharedPtr<FArmyDetailNode> BasicParameters = BathroomCabDetailPanel->EditCategory("BasicParameters");
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

void FArmyWHCModeBathroomCabAttr::MakeDecorationBoardUI()
{
    AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(Container.Pin()->LastWHCItemSelected);
    check(ShapeFrame->ShapeInRoomRef != nullptr);
    BathroomCabDetailPanel->RemoveCategory("FuncBoardParameters");
    if (ShapeFrame->ShapeInRoomRef->DecBoardInfos.Num() > 0)
    {
        FArmyDetailNode &FuncBoardParameters = BathroomCabDetailPanel->AddCategory("FuncBoardParameters", TEXT("功能板件"));

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
            DecBoardNode = FuncBoardParameters.AddChildNode(Name, DisplayName).AsShared();
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
}


void FArmyWHCModeBathroomCabAttr::MakeDoorPlankAttributesUI(FShapeInRoom *InShapeInRoom) {
	if (nullptr == InShapeInRoom)
		return;

	FCabinetShape *CabShape = static_cast<FCabinetShape*>(InShapeInRoom->Shape.Get());
	if (nullptr != CabShape)
	{
		TArray<FDoorGroup*> DoorGroups;
		EShapeType CurrentDoorType = CabShape->GetShapeType();
		if (CurrentDoorType == ST_Cabinet) // 移门替换
		{
			XRShapeUtilities::GetCabinetDoors(CabShape, DoorGroups, FLAG_ALLDOORS);
		}
		else {
			return;
		}

		BathroomCabDetailPanel->RemoveCategory("ConfigParameters");

		if (DoorGroups.Num() < 1)
			return;

		TSharedPtr<FArmyDetailNode> ConfigParameters;
		ConfigParameters = BathroomCabDetailPanel->AddCategory("ConfigParameters", TEXT("门板属性")).AsShared();
		if (!ConfigParameters.IsValid())
			return;
		SArmyWHCModeAttrPanelContainer * ContainerPtr = Container.Pin().Get();
		TSharedPtr<FArmyDetailNode> DecDoorNode;

		FName Name = FName("");
		FString DisplayName = TEXT("门板");
		int32 Index = 0;
		int32 doorNum = DoorGroups.Num();
		for (const auto &DoorGroup : DoorGroups)
		{
			Index++;
			FString finalName = (doorNum == 1 ? "" : FString::FromInt(Index));
			Name = FName(*("Door" + finalName));
			DisplayName = TEXT("门板") + finalName;

			DecDoorNode = ConfigParameters->AddChildNode(Name, DisplayName).AsShared();
			DecDoorNode->NameContentOverride().VAlign(VAlign_Center);

			//item
			FArmyWHCCabDoorUI *CabDoorUIComponent = MakeAttrUIComponent<FArmyWHCCabDoorUI>(ContainerPtr, ComponentArr, 2); // 掩门门板类型
			FArmyDetailNode &DoorTypeNode = DecDoorNode.Get()->AddChildNode("CabinetDoorType", TEXT("门板型号"),
				CabDoorUIComponent->MakeWidget()
			);
			DoorTypeNode.NameContentOverride().VAlign(VAlign_Center);
			DoorTypeNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

			FArmyWHCCabDoorMtlUI *CabDoorMtlUIComponent = MakeAttrUIComponent<FArmyWHCCabDoorMtlUI>(ContainerPtr, ComponentArr);
			FArmyDetailNode &DoorMtlNode = DecDoorNode.Get()->AddChildNode("CabinetDoorMtl", TEXT("门板材质"),
				CabDoorMtlUIComponent->MakeWidget()
			);
			DoorMtlNode.NameContentOverride().VAlign(VAlign_Center);
			DoorMtlNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

			FArmyWHCCabDoorHandleUI *CabDoorHandleUIComponent = MakeAttrUIComponent<FArmyWHCCabDoorHandleUI>(ContainerPtr, ComponentArr);
			FArmyDetailNode &DoorHandleNode = DecDoorNode.Get()->AddChildNode("CabinetDoorHandle", TEXT("拉手型号"),
				CabDoorHandleUIComponent->MakeWidget()
			);

			//check box
			FArmyWHCDoorVisibilityUI *DoorVisibilityUIComponent = MakeAttrUIComponent<FArmyWHCDoorVisibilityUI>(ContainerPtr, ComponentArr);
			FArmyDetailNode &DoorVisibilityNode = DecDoorNode.Get()->AddChildNode("DoorVisibility", TEXT("隐藏门板"),
				DoorVisibilityUIComponent->MakeWidget()
			);

			//render text
			SpawnModeBathRoomSlideDoorText(DoorGroup, Index);
		}
	}
}

//抽面造型
void FArmyWHCModeBathroomCabAttr::MakeDrawerFaceModel(FShapeInRoom *InShapeInRoom) {
	if (nullptr == InShapeInRoom)
		return;

	FCabinetShape *CabShape = static_cast<FCabinetShape*>(InShapeInRoom->Shape.Get());
	if (nullptr != CabShape) {
		TArray<FDoorGroup*> DrawerDoorGroups;
		EShapeType CurrentDoorType = CabShape->GetShapeType();
		if (CurrentDoorType == ST_Cabinet)
		{
			XRShapeUtilities::GetCabinetDrawerDoors(CabShape, DrawerDoorGroups);
		}
		else {
			return;
		}
		BathroomCabDetailPanel->RemoveCategory("ConfigDrawerParameters");

		if (DrawerDoorGroups.Num() < 1)
			return;

		TSharedPtr<FArmyDetailNode> ConfigParameters;
		ConfigParameters = BathroomCabDetailPanel->AddCategory("ConfigDrawerParameters", TEXT("抽面属性")).AsShared();
		if (!ConfigParameters.IsValid())
			return;

		SArmyWHCModeAttrPanelContainer * ContainerPtr = Container.Pin().Get();
		TSharedPtr<FArmyDetailNode> DecFaceNode;

		FName Name = FName("");
		FString DisplayName = "";
		int32 Index = 0;
		int32 faceNum = DrawerDoorGroups.Num();
		for (const auto &DoorGroup : DrawerDoorGroups) {
			Index++;
			FString finalName = (faceNum == 1 ? "" : FString::FromInt(Index));
			Name = FName(*("DrawerDoor" + finalName));
			DisplayName = TEXT("抽面") + finalName;

			DecFaceNode = ConfigParameters->AddChildNode(Name, DisplayName).AsShared();
			DecFaceNode->NameContentOverride().VAlign(VAlign_Center);

			//item
			FArmyWHCCabDoorUI *DoorUIAttrComponent = MakeAttrUIComponent<FArmyWHCCabDoorUI>(ContainerPtr, ComponentArr, 3);
			FArmyDetailNode &DrawerTypeNode = DecFaceNode.Get()->AddChildNode("DrawerType", TEXT("抽面型号"),
				DoorUIAttrComponent->MakeWidget()
			);

			DrawerTypeNode.NameContentOverride().VAlign(VAlign_Center);
			DrawerTypeNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

			FArmyWHCCabDoorMtlUI *DoorMtlUIAttrComponent = MakeAttrUIComponent<FArmyWHCCabDoorMtlUI>(ContainerPtr, ComponentArr);
			FArmyDetailNode &DrawerMtlNode = DecFaceNode.Get()->AddChildNode("DrawerMtl", TEXT("抽面材质"),
				DoorMtlUIAttrComponent->MakeWidget()
			);

			FArmyWHCCabDoorHandleUI *DoorHandleUIComponent = MakeAttrUIComponent<FArmyWHCCabDoorHandleUI>(ContainerPtr, ComponentArr);
			FArmyDetailNode &HandleNode = DecFaceNode.Get()->AddChildNode("DoorHandle", TEXT("拉手型号"),
				DoorHandleUIComponent->MakeWidget()
			);
			//check box
			
			FArmyWHCabDrawerDoorVisibilityUI *DrawerDoorVisibilityUIComponent = MakeAttrUIComponent<FArmyWHCabDrawerDoorVisibilityUI>(ContainerPtr, ComponentArr);
			FArmyDetailNode &DrawerDoorVisibilityNode = DecFaceNode.Get()->AddChildNode("DrawerDoorVisibility", TEXT("隐藏抽面"),
				DrawerDoorVisibilityUIComponent->MakeWidget()
			);
			DrawerDoorVisibilityUIComponent->GrabLocation(Index);
			//render text
			SpawnModeBathRoomDrawerDoorText(DoorGroup, Index);
		}
	}
}

TSharedPtr<SWidget> FArmyWHCModeBathroomCabAttr::RebuildAttrPanel()
{
    MakeDimensionUI();
    MakeDecorationBoardUI();
    return BathroomCabDetailPanel->BuildDetail();
}

void FArmyWHCModeBathroomCabAttr::SpawnModeBathRoomSlideDoorText(FDoorGroup* pDoorGroup, int32 groupIndex)
{
	FSideHungDoor* CurHungDoor = StaticCast<FSideHungDoor*>(pDoorGroup);
	if (CurHungDoor) {
		AXRWHTextRenderActor* ModeTextActor = GVC->GetWorld()->SpawnActor<AXRWHTextRenderActor>(AXRWHTextRenderActor::StaticClass());
		if (ModeTextActor && CurHungDoor->GetShapeActor())
		{
			ModeTextActor->AttachToActor(static_cast<AActor*>(CurHungDoor->GetShapeActor()),
				FAttachmentTransformRules::KeepRelativeTransform);
			FString modeText = FString::FromInt(groupIndex);

			float with = CurHungDoor->GetShapeWidth() * 0.05f;
			float height = CurHungDoor->GetShapeHeight() * 0.035f;
			float depth = CurHungDoor->GetShapeDepth() * 0.125f;
			FVector relativeLoc(with, depth, height);
			ModeTextActor->SetTextRelativeLocation(relativeLoc);
			ModeTextActor->SetColor(FColor::Yellow);
			ModeTextActor->SetText(modeText);
		}
	}
}

void FArmyWHCModeBathroomCabAttr::SpawnModeBathRoomDrawerDoorText(FDoorGroup* pDoorGroup, int32 groupIndex)
{
	//抽面编号
	FDrawerDoorShape* CurDrawerDoor = StaticCast<FDrawerDoorShape*>(pDoorGroup);

	TSharedPtr<FDrawerDoorShape::FVariableAreaDoorSheetForDrawer> drawerDoorAreaDoorSheet = CurDrawerDoor->GetDrawDoorSheet();
	if (drawerDoorAreaDoorSheet.IsValid()) {
		AXRWHTextRenderActor* ModeTextActor = GVC->GetWorld()->SpawnActor<AXRWHTextRenderActor>(AXRWHTextRenderActor::StaticClass());
		if (ModeTextActor && drawerDoorAreaDoorSheet->GetShapeActor())
		{
			ModeTextActor->AttachToActor(static_cast<AActor*>(drawerDoorAreaDoorSheet->GetShapeActor()),
				FAttachmentTransformRules::KeepRelativeTransform);
			
			float with = drawerDoorAreaDoorSheet->GetShapeWidth() * 0.05f;
			float height = drawerDoorAreaDoorSheet->GetShapeHeight() * 0.015f;
			float depth = drawerDoorAreaDoorSheet->GetShapeDepth() * 0.125f;
			FVector relativeLoc(with, depth, height);
			ModeTextActor->SetTextRelativeLocation(relativeLoc);
			ModeTextActor->SetColor(FColor::Orange);
			FString modeText =/* TEXT("抽面 ") +*/ FString::FromInt(groupIndex);
			ModeTextActor->SetText(modeText);
		}
	}
}
