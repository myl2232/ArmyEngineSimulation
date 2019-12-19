#include "SArmyWHCModeTopBlockerAttr.h"
#include "ArmyShapeTableActor.h"

#include "UIMixin/SArmyWHCExtrusionUI.h"
#include "UIMixin/SArmyWHCExtrusionMtlUI.h"

#include "ArmyDetailBuilder.h"
#include "SContentItem.h"
#include "ArmyDownloadModule.h"
#include "Actor/XRActorConstant.h"
#include "Data/WHCModeData/XRWHCModeData.h"

FArmyWHCModeTopBlockerAttr::FArmyWHCModeTopBlockerAttr(TSharedRef<class SArmyWHCModeAttrPanelContainer> InContainer)
: FArmyWHCModeAttrPanel(InContainer)
{}

void FArmyWHCModeTopBlockerAttr::TickPanel()
{
	TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    TArray<int32> CabIds = ContainerPtr->GetReplacingIds();
    TSharedPtr<FDownloadFileSet> TheTask;
	for (int32 i = 0; i < CabIds.Num(); ++i)
	{
        int32 Id = CabIds[i];

		SContentItem *ContentItem = ContainerPtr->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_TopBlockerType);
		if (ContentItem && ContentItem->GetItemState() == EDownloading)
		{
			const TSharedPtr<FTableInfo> * TableInfoPtr = FWHCModeGlobalData::TopSections.Find(Id);
            check(TableInfoPtr != nullptr);
			TheTask = FArmyDownloadModule::Get().GetDownloadManager()->GetTask((*TableInfoPtr)->cadUrl);
			if (TheTask.IsValid())
			{
				// 设置下载进度
				UpdateProgress(TheTask, ContentItem);
				continue;
			}
        }
		ContentItem = ContainerPtr->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_TopBlockerMtl);
        if (ContentItem && ContentItem->GetItemState() == EDownloading)
        {	
            const TSharedPtr<FMtlInfo> * TopMtlPtr = FWHCModeGlobalData::TopMtlMap.Find(Id);
            check(TopMtlPtr != nullptr);
			TheTask = FArmyDownloadModule::Get().GetDownloadManager()->GetTask((*TopMtlPtr)->MtlUrl);
			if (TheTask.IsValid())
			{
				UpdateProgress(TheTask, ContentItem);
				continue;
			}
		}
	}
}

void FArmyWHCModeTopBlockerAttr::Initialize()
{
	SArmyWHCModeAttrPanelContainer *ContainerPtr = Container.Pin().Get();

    TopBlockerPanel = MakeShareable(new FArmyDetailBuilder);

    FArmyDetailNode &BasicParameters = TopBlockerPanel->AddCategory("BasicParameters", TEXT("基本参数"));

	FArmyWHCExtrusionUI *ExtrusionUIComponent = MakeAttrUIComponent<FArmyWHCExtrusionUI>(ContainerPtr, ComponentArr, 3);
    FArmyDetailNode &TopBlockerTypeNode = BasicParameters.AddChildNode("TopBlockerType", TEXT("上线条型号"), 
        ExtrusionUIComponent->MakeWidget()
    );
    TopBlockerTypeNode.NameContentOverride().VAlign(VAlign_Center);
    TopBlockerTypeNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

	FArmyWHCExtrusionMtlUI *ExtrusionMtlUIComponent = MakeAttrUIComponent<FArmyWHCExtrusionMtlUI>(ContainerPtr, ComponentArr, 3);
    FArmyDetailNode &TopBlockerMtlNode = BasicParameters.AddChildNode("TopBlockerMtl", TEXT("上线条材质"), 
        ExtrusionMtlUIComponent->MakeWidget()
    );
    TopBlockerMtlNode.NameContentOverride().VAlign(VAlign_Center);
    TopBlockerMtlNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
}

bool FArmyWHCModeTopBlockerAttr::TryDisplayAttrPanel(AActor *InActor)
{
    if (InActor->ActorHasTag(XRActorTag::WHCHang) && InActor->IsA<AXRShapeTableActor>())
    {
        TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
        if (ContainerPtr->LastWHCItemSelected == InActor)
            return true;

        ContainerPtr->LastWHCItemSelected = InActor;
        ContainerPtr->SetAttrPanel(TopBlockerPanel->BuildDetail().ToSharedRef());
        return true;
    }
    else
        return false;
}

void FArmyWHCModeTopBlockerAttr::LoadMoreReplacingData()
{
    const FString & CurrentState = Container.Pin()->ListPanel->GetCurrentState();
    if (CurrentState == TEXT("上线条造型"))
	{
		TArray<IArmyWHCAttrUIComponent*> ExtrusionUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_Extrusion, ComponentArr);
        FArmyWHCExtrusionUI *ExtrusionUIComponent = (FArmyWHCExtrusionUI *)ExtrusionUIComponents[0];
        ExtrusionUIComponent->LoadListData();
	}
	else if (CurrentState == TEXT("上线条材质"))
	{
		TArray<IArmyWHCAttrUIComponent*> ExtrusionMtlUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_ExtrusionMtl, ComponentArr);
        FArmyWHCExtrusionMtlUI *ExtrusionMtlUIComponent = (FArmyWHCExtrusionMtlUI *)ExtrusionMtlUIComponents[0];
        ExtrusionMtlUIComponent->LoadListData();	
	}
}