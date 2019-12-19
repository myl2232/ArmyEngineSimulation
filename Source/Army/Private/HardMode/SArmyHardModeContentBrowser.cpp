#include "SArmyHardModeContentBrowser.h"
#include "ArmyCommonTypes.h"
#include "ArmyCommonTools.h"
#include "ArmyResourceModule.h"
#include "ArmyDownloadModule.h"
#include "ArmyHttpModule.h"
#include "ArmyUser.h"

void SArmyHardModeContentBrowser::ReqContentItems(const int32 Key, const int32 Value)
{
    if (!SelectPackges.IsValid())
    {
			return;
    }

		SArmyModelContentBrowser::ReqContentItems(Key, Value);
		
		FString path = "/api/bim_goods/menu_query?menuId=";
		if (eSubMode == ESubMode::SM_FloorMode || eSubMode == ESubMode::SM_WallMode || eSubMode == ESubMode::SM_CeilingMode)
		{
			path = "/api/bim_goods/category_query?categoryId=";
		}

		FString url = FString::Printf(TEXT("%s%d&setMealId=%d&page=%d&rows=20"), *path, Value, SelectPackges->Key, PageIndex);
		eModeIdent = EModeIdent::MI_SoftHardMode;
		IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(url, 
					FArmyHttpRequestCompleteDelegate::CreateRaw(this, &SArmyHardModeContentBrowser::ResContentItems));
		Request->ProcessRequest();
}

void SArmyHardModeContentBrowser::RequestSearchContentItems(bool isRequestBrank/* = false*/, bool isRequestSize/* = false*/)
{
		eModeIdent = EModeIdent::MI_SoftHardMode;
		SArmyModelContentBrowser::RequestSearchContentItems(isRequestBrank, isRequestSize);
}

void SArmyHardModeContentBrowser::ShowReplaceData()
{
		CategoryAndReplaceSwitcher->SetActiveWidgetIndex(1);
}

void SArmyHardModeContentBrowser::OnBeginMode()
{
		bClearContentItems = true;
		CurrentKey = -1;
		CurrentValue = -1;
}

//void SArmyHardModeContentBrowser::RequestCategory()
//{
//	
//
//	FString FullUrl = FString::Printf(TEXT("%s") , *CategoryUrl);
//	if (eSubMode == ESubMode::SM_FloorMode)
//	{
//		FullUrl = FString::Printf(TEXT("%s%d"), *CategoryUrl, 3);
//	}
//	else if (eSubMode == ESubMode::SM_WallMode) {
//		FullUrl = FString::Printf(TEXT("%s%d"), *CategoryUrl, 2);
//	}
//	else if (eSubMode == ESubMode::SM_CeilingMode)
//	{
//		FullUrl = FString::Printf(TEXT("%s%d"), *CategoryUrl, 1);
//
//	}
//	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(FullUrl, FArmyHttpRequestCompleteDelegate::CreateRaw(this, &SArmyHardModeContentBrowser::Callback_RequestCategory));
//	Request->ProcessRequest();
//}