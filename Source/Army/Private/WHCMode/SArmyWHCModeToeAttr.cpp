#include "SArmyWHCModeToeAttr.h"
#include "ArmyShapeTableActor.h"

#include "UIMixin/SArmyWHCExtrusionUI.h"
#include "UIMixin/SArmyWHCExtrusionMtlUI.h"

#include "ArmyDetailBuilder.h"
#include "SContentItem.h"
#include "ArmyDownloadModule.h"
#include "Actor/XRActorConstant.h"
#include "Data/WHCModeData/XRWHCModeData.h"

FArmyWHCModeToeAttr::FArmyWHCModeToeAttr(TSharedRef<SArmyWHCModeAttrPanelContainer> InContainer)
: FArmyWHCModeAttrPanel(InContainer)
{}

void FArmyWHCModeToeAttr::TickPanel()
{
    TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
    TArray<int32> CabIds = ContainerPtr->GetReplacingIds();
    TSharedPtr<FDownloadFileSet> TheTask;
	for (int32 i = 0; i < CabIds.Num(); ++i)
	{
        int32 Id = CabIds[i];

		SContentItem *ContentItem = ContainerPtr->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_ToeType);
		if (ContentItem && ContentItem->GetItemState() == EDownloading)
		{
			const TSharedPtr<FTableInfo> * TableInfoPtr = FWHCModeGlobalData::ToeSections.Find(Id);
            check(TableInfoPtr != nullptr);
            TheTask = FArmyDownloadModule::Get().GetDownloadManager()->GetTask((*TableInfoPtr)->cadUrl);
            if (TheTask.IsValid())
            {
                // 设置下载进度
                UpdateProgress(TheTask, ContentItem);
                continue;
            }
        }
		ContentItem = ContainerPtr->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_ToeMtl);
        if (ContentItem && ContentItem->GetItemState() == EDownloading)
        {	
            const TSharedPtr<FMtlInfo> * ToeMtlPtr = FWHCModeGlobalData::ToeMtlMap.Find(Id);
            check(ToeMtlPtr != nullptr);
            TheTask = FArmyDownloadModule::Get().GetDownloadManager()->GetTask((*ToeMtlPtr)->MtlUrl);
            if (TheTask.IsValid())
            {
                UpdateProgress(TheTask, ContentItem);
                continue;
            }
		}
	}
}

void FArmyWHCModeToeAttr::Initialize()
{
    SArmyWHCModeAttrPanelContainer *ContainerPtr = Container.Pin().Get();

    ToePanel = MakeShareable(new FArmyDetailBuilder);

    FArmyDetailNode &BasicParameters = ToePanel->AddCategory("BasicParameters", TEXT("基本参数"));

    FArmyWHCExtrusionUI *ExtrusionUIComponent = MakeAttrUIComponent<FArmyWHCExtrusionUI>(ContainerPtr, ComponentArr, 2);
    FArmyDetailNode &ToeNode = BasicParameters.AddChildNode("Toe", TEXT("踢脚板造型"), 
        ExtrusionUIComponent->MakeWidget()
    );
    ToeNode.NameContentOverride().VAlign(VAlign_Center);
    ToeNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));

    FArmyWHCExtrusionMtlUI *ExtrusionMtlUIComponent = MakeAttrUIComponent<FArmyWHCExtrusionMtlUI>(ContainerPtr, ComponentArr, 2);
    FArmyDetailNode &ToeMtlNode = BasicParameters.AddChildNode("ToeMtl", TEXT("踢脚板材质"), 
        ExtrusionMtlUIComponent->MakeWidget()
    );
    ToeMtlNode.NameContentOverride().VAlign(VAlign_Center);
    ToeMtlNode.ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Fill).Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f));
}

bool FArmyWHCModeToeAttr::TryDisplayAttrPanel(AActor *InActor)
{
    if (InActor->ActorHasTag(XRActorTag::WHCFloor) && InActor->IsA<AXRShapeTableActor>())
    {
        TSharedPtr<SArmyWHCModeAttrPanelContainer> ContainerPtr = Container.Pin();
        if (ContainerPtr->LastWHCItemSelected == InActor)
            return true;

        ContainerPtr->LastWHCItemSelected = InActor;   
        
        ContainerPtr->SetAttrPanel(ToePanel->BuildDetail().ToSharedRef());
        return true;
    }
    else
        return false;
}

void FArmyWHCModeToeAttr::LoadMoreReplacingData()
{
    const FString & CurrentState = Container.Pin()->GetCurrentState();
    if (CurrentState == TEXT("踢脚板造型"))
    {
        TArray<IArmyWHCAttrUIComponent*> ExtrusionUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_Extrusion, ComponentArr);
        FArmyWHCExtrusionUI *ExtrusionUIComponent = (FArmyWHCExtrusionUI *)ExtrusionUIComponents[0];
        ExtrusionUIComponent->LoadListData();
    }
    else if (CurrentState == TEXT("踢脚板材质"))
    {
        TArray<IArmyWHCAttrUIComponent*> ExtrusionMtlUIComponents = FindAttrUIComponents(EWHCAttrUIType::EWAUT_ExtrusionMtl, ComponentArr);
        FArmyWHCExtrusionMtlUI *ExtrusionMtlUIComponent = (FArmyWHCExtrusionMtlUI *)ExtrusionMtlUIComponents[0];
        ExtrusionMtlUIComponent->LoadListData();
    }
}