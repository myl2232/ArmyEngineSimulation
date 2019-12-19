#include "SArmyWHCModeOtherCabAttr.h"
#include "ArmyWHCabinet.h"
#include "ArmyShapeFrameActor.h"

#include "UIMixin/SArmyWHCAboveGroundUI.h"
#include "UIMixin/SArmyWHCCabDimensionUI.h"
#include "UIMixin/SArmyWHCCabMtlUI.h"
#include "UIMixin/SArmyWHCCabDoorUI.h"
#include "UIMixin/SArmyWHCCabDoorMtlUI.h"
#include "UIMixin/SArmyWHCCabDoorHandleUI.h"
#include "UIMixin/SArmyWHCCabAccUI.h"
#include "UIMixin/SArmyWHCDoorVisibilityUI.h"
#include "UIMixin/SArmyWHCDecorationBoardUI.h"
#include "UIMixin/SArmyWHCPlatformUI.h"

#include "Data/WHCModeData/XRWHCModeData.h"

#include "ArmyDetailBuilder.h"
#include "SContentItem.h"
#include "ArmyDownloadModule.h"

#include "SCTShape.h"
#include "UtilityTools/SCTShapeUtilityTools.h"

FArmyWHCModeOtherCabAttr::FArmyWHCModeOtherCabAttr(TSharedRef<SArmyWHCModeAttrPanelContainer> InContainer)
: FArmyWHCModeAttrPanel(InContainer)
{}

void FArmyWHCModeOtherCabAttr::TickPanel()
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
            const TSharedPtr<FCoverDoorTypeInfo> * DoorTypeInfoPtr = FWHCModeGlobalData::CoverDoorTypeMap.Find(Id);
            check(DoorTypeInfoPtr != nullptr);
            TheTask = FArmyDownloadModule::Get().GetDownloadManager()->GetTask((*DoorTypeInfoPtr)->DownloadTaskFlag);

            if (TheTask.IsValid())
            {
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

        ContentItem = ContainerPtr->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_DoorHandle);
        if (ContentItem && ContentItem->GetItemState() == EDownloading)
        {
            TheTask = FArmyDownloadModule::Get().GetDownloadManager()->GetTask(FString::Printf(TEXT("CoverDoorHandle_%d"), Id));

            if (TheTask.IsValid())
            {
                UpdateProgress(TheTask, ContentItem);
                continue;
            }
        }

        ContentItem = ContainerPtr->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_Acc);
        if (ContentItem && ContentItem->GetItemState() == EDownloading)
        {
            const TSharedPtr<FCabinetWholeAccInfo> * pAccInfo = FWHCModeGlobalData::CabinetAccMap.Find(Id);
            check(pAccInfo != nullptr);
            TheTask = FArmyDownloadModule::Get().GetDownloadManager()->GetTask((*pAccInfo)->Self->PakUrl);
            if (TheTask.IsValid())
            {
                // 设置下载进度
                UpdateProgress(TheTask, ContentItem);
                continue;
            }
        }

        ContentItem = ContainerPtr->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_PlatformMtl);
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
	}
}

void FArmyWHCModeOtherCabAttr::Initialize()
{
    SArmyWHCModeAttrPanelContainer * ContainerPtr = Container.Pin().Get();

    OtherCabDetailPanel = MakeShareable(new FArmyDetailBuilder);

    FArmyDetailNode &BasicParameters = OtherCabDetailPanel->AddCategory("BasicParameters", TEXT("基本参数"));

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

    FArmyDetailNode &DoorParameters = OtherCabDetailPanel->AddCategory("DoorParameters", TEXT("柜体门板"));

    FArmyWHCCabDoorUI *CabDoorUIComponent = MakeAttrUIComponent<FArmyWHCCabDoorUI>(ContainerPtr, ComponentArr, 2); // 掩门门板类型
    FArmyDetailNode &DoorTypeNode = DoorParameters.AddChildNode("CabinetDoorType", TEXT("门板型号"), 
        CabDoorUIComponent->MakeWidget()
    );
    DoorTypeNode.NameContentOverride().VAlign(VAlign_Center);
    DoorTypeNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyWHCCabDoorMtlUI *CabDoorMtlUIComponent = MakeAttrUIComponent<FArmyWHCCabDoorMtlUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &DoorMtlNode = DoorParameters.AddChildNode("CabinetDoorMtl", TEXT("门板材质"), 
        CabDoorMtlUIComponent->MakeWidget()
    );
    DoorMtlNode.NameContentOverride().VAlign(VAlign_Center);
    DoorMtlNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyWHCCabDoorHandleUI *CabDoorHandleUIComponent = MakeAttrUIComponent<FArmyWHCCabDoorHandleUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &DoorHandleNode = DoorParameters.AddChildNode("CabinetDoorHandle", TEXT("拉手型号"), 
        CabDoorHandleUIComponent->MakeWidget()
    );
    DoorHandleNode.NameContentOverride().VAlign(VAlign_Center);
    DoorHandleNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyWHCDoorVisibilityUI *DoorVisibilityUIComponent = MakeAttrUIComponent<FArmyWHCDoorVisibilityUI>(ContainerPtr, ComponentArr);
    FArmyDetailNode &DoorVisibilityNode = DoorParameters.AddChildNode("DoorVisibility", TEXT("隐藏门板"), 
        DoorVisibilityUIComponent->MakeWidget()
    );
    DoorVisibilityNode.NameContentOverride().VAlign(VAlign_Center);
    DoorVisibilityNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    // FArmyDetailNode &FuncBoardParameters = OtherCabDetailPanel->AddCategory("FuncBoardParameters", TEXT("功能板件"));

    MakeAttrUIComponent<FArmyWHCCabAccUI>(ContainerPtr, ComponentArr, true, false);
    MakeAttrUIComponent<FArmyWHCCabAccUI>(ContainerPtr, ComponentArr, false, true);

    MakeAttrUIComponent<FArmyWHCDecorationBoardUI>(ContainerPtr, ComponentArr, 0);
    MakeAttrUIComponent<FArmyWHCDecorationBoardUI>(ContainerPtr, ComponentArr, 1);

    MakeAttrUIComponent<FArmyWHCPlatformUI>(ContainerPtr, ComponentArr);
}

bool FArmyWHCModeOtherCabAttr::TryDisplayAttrPanel(AActor *InActor)
{
    if (InActor->IsA<AXRShapeFrame>())
    {
        TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
        if (ContainerPtr->LastWHCItemSelected == InActor)
            return true;

        AXRShapeFrame * ShapeFrame = Cast<AXRShapeFrame>(InActor);
        FShapeInRoom * RESTRICT ShapeInRoom = ShapeFrame->ShapeInRoomRef;
        
        int32 ShapeCategory = ShapeInRoom->Shape->GetShapeCategory();
        if (ShapeCategory == (int32)ECabinetType::EType_Other)
        {
            ContainerPtr->LastWHCItemSelected = InActor;

            OtherCabDetailPanel->RemoveCategory("FuncBoardParameters");

            MakeDimensionUI();
            MakeDecorationBoardUI(ShapeInRoom);

            TArray<IArmyWHCAttrUIComponent*> PlatformUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_Platform, ComponentArr);
            FArmyWHCPlatformUI *PlatformUIComponent = (FArmyWHCPlatformUI *)PlatformUIComponents[0];
            PlatformUIComponent->SetAddOrReplace(!ShapeInRoom->IsPlatformSpawned());
            MakePlatformUI(ShapeInRoom);
			TSharedPtr<FArmyDetailNode> CabAccParameters = OtherCabDetailPanel->EditCategory("CabAccParameters");
			if (ShapeInRoom->CabinetActor.IsValid())
			{
				XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = ShapeInRoom->CabinetActor;
				TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
				if (Accessory.IsValid())
				{
					if (Accessory->CabAccInfo->Type == (uint8)EMetalsType::MT_BATHROOM_BASIN)
					{
						if (!CabAccParameters.IsValid())
							CabAccParameters = OtherCabDetailPanel->AddCategory("CabAccParameters", TEXT("柜体附件")).AsShared();

						TSharedPtr<FArmyDetailNode> CabAccNode = CabAccParameters->FindChildNode("CabinetAcc");
						if (!CabAccNode.IsValid())
						{
							CabAccNode = CabAccParameters->AddChildNode("CabinetAcc", TEXT("未知附件")).AsShared();
							CabAccNode->NameContentOverride().VAlign(VAlign_Center);
							CabAccNode->ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
						}
						TSharedPtr<FArmyDetailNode> CabSubAccNode = CabAccParameters->FindChildNode("CabinetSubAcc");
						if (!CabSubAccNode.IsValid())
						{
							CabSubAccNode = CabAccParameters->AddChildNode("CabinetSubAcc", TEXT("未知子附件")).AsShared();
							CabSubAccNode->NameContentOverride().VAlign(VAlign_Center);
							CabSubAccNode->ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
						}

						TArray<IArmyWHCAttrUIComponent*> CabAccUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_CabAcc, ComponentArr);
						FArmyWHCCabAccUI *TopAccUIComponent = static_cast<FArmyWHCCabAccUI *>(CabAccUIComponents[0]);
						FArmyWHCCabAccUI *SubAccUIComponent = static_cast<FArmyWHCCabAccUI *>(CabAccUIComponents[1]);

						CabAccNode->SetNodeDisplayName(FText::FromString(TEXT("水盆(含台面)")));
						CabAccNode->ValueContentOverride()[TopAccUIComponent->MakeWidget().ToSharedRef()];
						if (Accessory->AttachedSubAccessories.Num() > 0)
						{
							CabSubAccNode->SetNodeDisplayName(FText::FromString(TEXT("水盆龙头")));
							SubAccUIComponent->SetAddOrReplace(false);
							CabSubAccNode->ValueContentOverride()[SubAccUIComponent->MakeWidget().ToSharedRef()];
						}
						else
						{
							SubAccUIComponent->SetAddOrReplace(true);
							CabSubAccNode->ValueContentOverride()[SubAccUIComponent->MakeWidget().ToSharedRef()];
						}
					}
					else
					{
						CabAccParameters->RemoveChildNode("CabinetAcc");
						CabAccParameters->RemoveChildNode("CabinetSubAcc");
					}
				}
			}
            else
            {
                if (CabAccParameters.IsValid())
                    OtherCabDetailPanel->RemoveCategory("CabAccParameters");
            }

            ContainerPtr->SetAttrPanel(OtherCabDetailPanel->BuildDetail().ToSharedRef());

            return true;
        }
        else
            return false;
    }
    else
        return false;
}

void FArmyWHCModeOtherCabAttr::LoadMoreReplacingData()
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    const FString & CurrentState = ContainerPtr->GetCurrentState();
    if (CurrentState == TEXT("柜体材质"))
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
    else if (CurrentState == TEXT("门板材质"))
    {
        TArray<IArmyWHCAttrUIComponent*> CabDoorMtlUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_DoorMtl, ComponentArr);
        FArmyWHCCabDoorMtlUI *CabDoorMtlUIComponent = (FArmyWHCCabDoorMtlUI *)CabDoorMtlUIComponents[0];
        CabDoorMtlUIComponent->LoadListData();
    }
    else if (CurrentState == TEXT("拉手型号"))
    {
        TArray<IArmyWHCAttrUIComponent*> CabDoorHandleUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_DoorHandle, ComponentArr);
        FArmyWHCCabDoorHandleUI *CabDoorHandleUIComponent = (FArmyWHCCabDoorHandleUI *)CabDoorHandleUIComponents[0];
        CabDoorHandleUIComponent->LoadListData();
    }
    else if (CurrentState == TEXT("附属件型号"))
    {
        TArray<IArmyWHCAttrUIComponent*> CabAccUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_CabAcc, ComponentArr);
        FArmyWHCCabAccUI *CabAccUIComponent = (FArmyWHCCabAccUI *)CabAccUIComponents[0];
        CabAccUIComponent->LoadListData();
    }
    else if (CurrentState == TEXT("子附属件型号"))
    {
        TArray<IArmyWHCAttrUIComponent*> CabAccUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_CabAcc, ComponentArr);
        FArmyWHCCabAccUI *CabAccUIComponent = (FArmyWHCCabAccUI *)CabAccUIComponents[1];
        CabAccUIComponent->LoadListData();
    }
    else if (CurrentState == TEXT("见光板材质"))
    {
        TArray<IArmyWHCAttrUIComponent*> DecBoardUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_DecorationBoard, ComponentArr);
        FArmyWHCDecorationBoardUI *DecBoardUIComponent = (FArmyWHCDecorationBoardUI *)DecBoardUIComponents[0];
        DecBoardUIComponent->LoadListData();
    }
    else if (CurrentState == TEXT("台面材质"))
    {
        TArray<IArmyWHCAttrUIComponent*> PlatformUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_Platform, ComponentArr);
        FArmyWHCPlatformUI *PlatformUIComponent = (FArmyWHCPlatformUI *)PlatformUIComponents[0];
        PlatformUIComponent->LoadListData();
    }
}

void FArmyWHCModeOtherCabAttr::MakeDimensionUI()
{
    TSharedPtr<FArmyDetailNode> BasicParameters = OtherCabDetailPanel->EditCategory("BasicParameters");
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

void FArmyWHCModeOtherCabAttr::MakeDecorationBoardUI(FShapeInRoom *InShapeInRoom)
{
    TSharedPtr<FArmyDetailNode> FuncBoardParameters;
    if (InShapeInRoom->DecBoardInfos.Num() > 0)
    {
        FuncBoardParameters = OtherCabDetailPanel->EditCategory("FuncBoardParameters");
        if (!FuncBoardParameters.IsValid())
            FuncBoardParameters = OtherCabDetailPanel->AddCategory("FuncBoardParameters", TEXT("功能板件")).AsShared();
    }
    else
        return;

    TArray<IArmyWHCAttrUIComponent*> DecBoardUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_DecorationBoard, ComponentArr);

    FName Name;
    FString DisplayName;
    int32 Index = 0;
    TSharedPtr<FArmyDetailNode> DecBoardNode;

    for (const auto &BoardInfo : InShapeInRoom->DecBoardInfos)
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

void FArmyWHCModeOtherCabAttr::MakePlatformUI(FShapeInRoom *InShapeInRoom)
{
	XRCabinetActorT<XRWHCabinetPlatformInfoComponent> PlatformInfoComponent = InShapeInRoom->CabinetActor;
	TSharedPtr<FPlatformInfo> PlatformInfo = PlatformInfoComponent->Get();
    if (PlatformInfo.IsValid() && PlatformInfo->PlatformMtl.IsValid())
    {
        TSharedPtr<FArmyDetailNode> FuncBoardParameters = OtherCabDetailPanel->EditCategory("FuncBoardParameters");
        if (!FuncBoardParameters.IsValid())
            FuncBoardParameters = OtherCabDetailPanel->AddCategory("FuncBoardParameters", TEXT("功能板件")).AsShared();

        TArray<IArmyWHCAttrUIComponent*> PlatformUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_Platform, ComponentArr);
        FArmyWHCPlatformUI *PlatformUIComponent = (FArmyWHCPlatformUI *)PlatformUIComponents[0];
        FArmyDetailNode &PlatformNode = FuncBoardParameters->AddChildNode("Platform", TEXT("上台面板"), PlatformUIComponent->MakeWidget());
        PlatformNode.NameContentOverride().VAlign(VAlign_Center);
        if (PlatformUIComponent->IsAdd())
            PlatformNode.ValueContentOverride().HAlign(HAlign_Left).VAlign(VAlign_Fill);
        else
            PlatformNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
    }
}

TSharedPtr<SWidget> FArmyWHCModeOtherCabAttr::RebuildAttrPanel()
{
    TArray<IArmyWHCAttrUIComponent*> CabAccUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_CabAcc, ComponentArr);    
    FArmyWHCCabAccUI *SubAccUIComponent = static_cast<FArmyWHCCabAccUI *>(CabAccUIComponents[1]);

    TSharedPtr<FArmyDetailNode> CabAccParameters = OtherCabDetailPanel->EditCategory("CabAccParameters");
    if (CabAccParameters.IsValid())
    {
        TSharedPtr<FArmyDetailNode> CabSubAccNode = CabAccParameters->FindChildNode("CabinetSubAcc");
        check(CabSubAccNode.IsValid());
        CabSubAccNode->ValueContentOverride().HAlign(SubAccUIComponent->IsAdd() ? HAlign_Left : HAlign_Fill)[
            SubAccUIComponent->MakeWidget().ToSharedRef()
        ];
    }

    OtherCabDetailPanel->RemoveCategory("FuncBoardParameters");
    AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(Container.Pin()->LastWHCItemSelected);
    check(ShapeFrame->ShapeInRoomRef != nullptr);
    // 其它类型柜子不需要重新构建尺寸部分的UI，因为不允许柜子替换
    MakeDecorationBoardUI(ShapeFrame->ShapeInRoomRef);
    MakePlatformUI(ShapeFrame->ShapeInRoomRef);

    return OtherCabDetailPanel->BuildDetail();
}