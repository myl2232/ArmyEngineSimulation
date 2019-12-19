#include "SArmyHydropowerModeContentBrowser.h"
#include "ArmyCommonTypes.h"
#include "ArmyCommonTools.h"
#include "ArmyObject.h"
#include "ArmyUser.h"
#include "ArmyResourceModule.h"
#include "ArmyHttpModule.h"
using namespace FContentItemSpace;

void SArmyHydropowerModeContentBrowser::ReqContentItems (const int32 Key,const int32 Value)
{
	if (!SelectPackges.IsValid())
		return;
	SArmyModelContentBrowser::ReqContentItems (Key,Value);

	int32 TempPackId = 0;
	if (SelectPackges.IsValid())
		TempPackId = SelectPackges->Key;

	FString Url = "/api/bim_goods/hydropower/menu_query?menuId=";

	FString FullUrl = FString::Printf(TEXT("%s%d&setMealId=%d&page=%d&rows=18"), *Url, Value, TempPackId, PageIndex);
	eModeIdent = EModeIdent::MI_HydropowerMode;

	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(FullUrl, FArmyHttpRequestCompleteDelegate::CreateRaw(this, &SArmyHydropowerModeContentBrowser::ResContentItems));
	Request->ProcessRequest();
}

//void SArmyHydropowerModeContentBrowser::RequestCategory ()
//{
//	FString FullUrl = FString::Printf(TEXT("%s"), *CategoryUrl);
//	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(FullUrl, FArmyHttpRequestCompleteDelegate::CreateRaw(this, &SArmyHydropowerModeContentBrowser::Callback_RequestCategory));
//	Request->ProcessRequest();
//}

bool SArmyHydropowerModeContentBrowser::GetIsSupportDownload(FContentItemPtr _Item)
{
	TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = _Item->GetResObjNoComponent();
	if (resArr.Num() < 1)
		return false;
	TSharedPtr<FArmyPipeRes> ResObj = StaticCastSharedPtr<FArmyPipeRes>(resArr[0]);
	return ResObj->Categry0Type == 4 || ResObj->ComponentID == EC_Drain_Point;
}

void SArmyHydropowerModeContentBrowser::RequestSearchContentItems(bool isRequestBrank/* = false*/, bool isRequestSize/* = false*/)
{
	eModeIdent=EModeIdent::MI_HydropowerMode;
	SArmyModelContentBrowser::RequestSearchContentItems(isRequestBrank, isRequestSize);
}

void SArmyHydropowerModeContentBrowser::OnBeginMode()
{
	bClearContentItems = true;
	CurrentKey = -1;
	CurrentValue = -1;
}