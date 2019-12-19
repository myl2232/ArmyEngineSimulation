#include "ArmyWHCModeGlobalParams.h"
#include "HttpMgr.h"
#include "ArmyWHCModeData.h"
#include "ArmyDownloadModule.h"
#include "SContentItem.h"
#include "ArmyHttpModule.h"

void FArmyWHCModeGlobalParams::Query()
{
    TSharedRef<IHttpRequest> HttpRequest = FHttpMgr::Get()->CreateHttpRequest(
		TEXT("design/crafts"), 
		TEXT("GET"));
	HttpRequest->SetHeader("Content-Type", "application/json; charset=utf-8");
	HttpRequest->SetHeader("xloginid", FArmyHttpModule::Get().GetLoginId());
    HttpRequest->SetHeader("xtoken", FArmyHttpModule::Get().GetToken());
    HttpRequest->OnProcessRequestComplete().BindLambda(
        [](FHttpRequestPtr RequestPtr, FHttpResponsePtr ResponsePtr, bool bSucceeded)
        {
            if (!FHttpMgr::Get()->CheckHttpResponseValid(ResponsePtr, bSucceeded))
			{
				return;
			}
			FString ErrorMsg;
			TSharedPtr<FJsonObject> ResponseData = FHttpMgr::Get()->GetContentAsJsonObject(ResponsePtr, &ErrorMsg);
			if (!ResponseData.IsValid())
			{
				return;
			}

            const TSharedPtr<FJsonObject> & GlobalParamsJsonObj = ResponseData->GetObjectField(TEXT("data"));

            const TSharedPtr<FJsonObject> & CabJsonObj = GlobalParamsJsonObj->GetObjectField(TEXT("4"));
            const TArray<TSharedPtr<FJsonValue>>* CabInfoArray = nullptr;
            if (CabJsonObj->TryGetArrayField(TEXT("craftCupboards"), CabInfoArray))
            {
                int32 nCount = (*CabInfoArray).Num();
                for (int32 i = 0; i < nCount; ++i)
                {
                    const TSharedPtr<FJsonValue> &Value = CabInfoArray->operator[](i);
					const TSharedPtr<FJsonObject> &CabInfoJsonObject = Value->AsObject();
                    ECabinetType CabType = (ECabinetType)CabInfoJsonObject->GetIntegerField(TEXT("type"));
                    FWHCGlobalParams::FWHCCabGlobalParams * CabGlobalParams = FWHCModeGlobalData::GlobalParams.CabGlobalParamsMap.Find(CabType);
                    if (CabGlobalParams != nullptr)
                    {
                        CabGlobalParams->AboveGround = CabInfoJsonObject->GetNumberField(TEXT("heightToGround")) * 0.1f;
                    }
                }
            }

            const TSharedPtr<FJsonObject> & PlatformJsonObj = GlobalParamsJsonObj->GetObjectField(TEXT("5"));
            FWHCModeGlobalData::GlobalParams.PlatformBulge = PlatformJsonObj->GetNumberField(TEXT("bulge")) * 0.1f;

            const TSharedPtr<FJsonObject> & AdjustmentJsonObj = GlobalParamsJsonObj->GetObjectField(TEXT("6"));
            {
                const TSharedPtr<FJsonObject> & VerticalJsonObj = AdjustmentJsonObj->GetObjectField(TEXT("verticalSize"));
                FWHCModeGlobalData::GlobalParams.AdjustmentVerticalRange.X = VerticalJsonObj->GetNumberField(TEXT("min")) * 0.1f;
                FWHCModeGlobalData::GlobalParams.AdjustmentVerticalRange.Y = VerticalJsonObj->GetNumberField(TEXT("max")) * 0.1f;

                const TSharedPtr<FJsonObject> & LateralJsonObj = AdjustmentJsonObj->GetObjectField(TEXT("lateralSize"));
                FWHCModeGlobalData::GlobalParams.AdjustmentLateralRange.X = LateralJsonObj->GetNumberField(TEXT("min")) * 0.1f;
                FWHCModeGlobalData::GlobalParams.AdjustmentLateralRange.Y = LateralJsonObj->GetNumberField(TEXT("max")) * 0.1f;

                FWHCModeGlobalData::GlobalParams.AdjustmentLateralDepth = AdjustmentJsonObj->GetNumberField(TEXT("adjustingPlateDeep")) * 0.1f;
                FWHCModeGlobalData::GlobalParams.AdjustmentLateralBlockerDepth = AdjustmentJsonObj->GetNumberField(TEXT("closurePlateDeep")) * 0.1f;

                FWHCGlobalParams::FWHCCabGlobalParams * CabGlobalParams = FWHCModeGlobalData::GlobalParams.CabGlobalParamsMap.Find(ECabinetType::EType_OnGroundCab);
                if (CabGlobalParams != nullptr)
                    CabGlobalParams->AdjType = (EAdjustmentType)AdjustmentJsonObj->GetIntegerField(TEXT("floorCabinet"));

                CabGlobalParams = FWHCModeGlobalData::GlobalParams.CabGlobalParamsMap.Find(ECabinetType::EType_TallCab);
                if (CabGlobalParams != nullptr)
                    CabGlobalParams->AdjType = (EAdjustmentType)AdjustmentJsonObj->GetIntegerField(TEXT("tallCabinet"));

                CabGlobalParams = FWHCModeGlobalData::GlobalParams.CabGlobalParamsMap.Find(ECabinetType::EType_HangCab);
                if (CabGlobalParams != nullptr)
                    CabGlobalParams->AdjType = (EAdjustmentType)AdjustmentJsonObj->GetIntegerField(TEXT("hangingCabinet"));

                // CabGlobalParams = FWHCModeGlobalData::GlobalParams.CabGlobalParamsMap.Find(ECabinetType::EType_OnCabCab);
                // if (CabGlobalParams != nullptr)
                //     CabGlobalParams->AdjType = (EAdjustmentType)AdjustmentJsonObj->GetIntegerField(TEXT("hangingCabinet"));
            }

            const TSharedPtr<FJsonObject> & ToeJsonObj = GlobalParamsJsonObj->GetObjectField(TEXT("8"));
            {
                FWHCModeGlobalData::GlobalParams.ToeFBShrink = ToeJsonObj->GetNumberField(TEXT("frontAndBackIndent")) * 0.1f;
                FWHCModeGlobalData::GlobalParams.ToeLRShrink = ToeJsonObj->GetNumberField(TEXT("leftAndRightIndent")) * 0.1f;
            }
        }
    );
    HttpRequest->ProcessRequest();
	//QueryTable();
	//QueryTopTrim();
}
// void FArmyWHCModeGlobalParams::QueryTable()
// {
// 	// ��ȡ��ǰҪ�����ҳ��
// 	int32 UrlCurrent = 1;
// 	TSharedRef<IHttpRequest> HttpRequest = FHttpMgr::Get()->CreateHttpRequest(
// 		FString::Printf(TEXT("design/worktops?current=%d&size=15"), UrlCurrent),
// 		TEXT("GET"));
// 	HttpRequest->SetHeader("Content-Type", "application/json; charset=utf-8");
// 	HttpRequest->SetHeader("xloginid", FArmyHttpModule::Get().GetLoginId());
//     HttpRequest->SetHeader("xtoken", FArmyHttpModule::Get().GetToken());
// 	HttpRequest->OnProcessRequestComplete().BindLambda(
// 		[this](FHttpRequestPtr RequestPtr, FHttpResponsePtr ResponsePtr, bool bSucceeded)
// 	{
// 		if (!FHttpMgr::Get()->CheckHttpResponseValid(ResponsePtr, bSucceeded))
// 			return;
// 		FString ErrorMsg;
// 		TSharedPtr<FJsonObject> ResponseData = FHttpMgr::Get()->GetContentAsJsonObject(ResponsePtr, &ErrorMsg);
// 		if (!ResponseData.IsValid())
// 			return;

// 		const TSharedPtr<FJsonObject> &Data = ResponseData->GetObjectField(TEXT("data"));

// 		const TArray<TSharedPtr<FJsonValue>>* PlatformArray = nullptr;
// 		if (Data->TryGetArrayField(TEXT("list"), PlatformArray))
// 		{
// 			int32 nCount = (*PlatformArray).Num();
// 			for (int32 i = 0; i < nCount; ++i)
// 			{
// 				const TSharedPtr<FJsonValue> &Value = PlatformArray->operator[](i);
// 				const TSharedPtr<FJsonObject> &PlatformJsonObject = Value->AsObject();

// 				int32 Id = PlatformJsonObject->GetNumberField(TEXT("id"));

// 				if (FWHCModeGlobalData::PlatformMap.Find(Id) == nullptr)
// 				{
// 					FMtlInfo * NewPlatformInfo = new FMtlInfo;
// 					const TSharedPtr<FJsonObject> & MtlJsonObj = PlatformJsonObject->GetObjectField(TEXT("material"));
// 					NewPlatformInfo->Id = Id;
// 					NewPlatformInfo->MtlName = MtlJsonObj->GetStringField(TEXT("name"));
// 					NewPlatformInfo->MtlThumbnailUrl = MtlJsonObj->GetStringField(TEXT("thumbnailUrl"));
// 					NewPlatformInfo->MtlUrl = MtlJsonObj->GetStringField(TEXT("pakUrl"));
// 					NewPlatformInfo->MtlMd5 = MtlJsonObj->GetStringField(TEXT("pakMd5"));
// 					NewPlatformInfo->MtlParam = MtlJsonObj->GetStringField(TEXT("optimizeParam"));
// 					FWHCModeGlobalData::PlatformMap.Emplace(Id, MakeShareable(NewPlatformInfo));
// 					//
// 					const TArray<TSharedPtr<FJsonValue>>* fontSections = nullptr;
// 					PlatformJsonObject->TryGetArrayField(TEXT("fontSections"), fontSections);
// 					for (auto& Section : *fontSections)
// 					{
// 						FTableInfo* pInfo = new FTableInfo;
// 						// pInfo->mCadState = ENotReady;
// 						pInfo->Id = Section->AsObject()->GetNumberField(TEXT("id"));
// 						pInfo->Name = Section->AsObject()->GetStringField(TEXT("name"));
// 						pInfo->ThumbnailUrl = Section->AsObject()->GetStringField(TEXT("thumbnailUrl"));
// 						pInfo->cadUrl = Section->AsObject()->GetStringField(TEXT("cadUrl"));
// 						NewPlatformInfo->fontSections.Add(pInfo);
// 						DownTableInfo(pInfo);
// 					}
// 					//
// 					const TArray<TSharedPtr<FJsonValue>>* backSections = nullptr;
// 					PlatformJsonObject->TryGetArrayField(TEXT("backSections"), backSections);
// 					for (auto& Section : *backSections)
// 					{
// 						FTableInfo* pInfo = new FTableInfo;
// 						// pInfo->mCadState = ENotReady;
// 						pInfo->Id = Section->AsObject()->GetNumberField(TEXT("id"));
// 						pInfo->Name = Section->AsObject()->GetStringField(TEXT("name"));
// 						pInfo->ThumbnailUrl = Section->AsObject()->GetStringField(TEXT("thumbnailUrl"));
// 						pInfo->cadUrl = Section->AsObject()->GetStringField(TEXT("cadUrl"));
// 						pInfo->mtlId = Id;
// 						NewPlatformInfo->backSections.Add(pInfo);
// 						DownTableInfo(pInfo);
// 					}
// 				}

// 			}
// 		}
// 	}
// 	);
// 	HttpRequest->ProcessRequest();
// }
// void FArmyWHCModeGlobalParams::DownTableInfo(FTableInfo* pInfo)
// {
// 	FString CachePath = pInfo->GetCachedPath();
// 	TArray<FDownloadSpace::FDownloadFileInfo> FileInfos{
// 		FDownloadSpace::FDownloadFileInfo(
// 			pInfo->Id,
// 			CachePath,
// 			pInfo->cadUrl,
// 			"",
// 			true
// 		)
// 	};
// 	TSharedPtr<FDownloadFileSet> NewTask = FArmyDownloadModule::Get().GetDownloadManager()->AddMultiTask(FileInfos);
// }
// void FArmyWHCModeGlobalParams::QueryTopTrim()
// {
// 	TSharedRef<IHttpRequest> HttpRequest = FHttpMgr::Get()->CreateHttpRequest(
// 		FString::Printf(TEXT("design/decoratingPlate?type=3") ),
// 		TEXT("GET"));
// 	HttpRequest->SetHeader("Content-Type", "application/json; charset=utf-8");
// 	HttpRequest->SetHeader("xloginid", FArmyHttpModule::Get().GetLoginId());
//     HttpRequest->SetHeader("xtoken", FArmyHttpModule::Get().GetToken());
// 	HttpRequest->OnProcessRequestComplete().BindLambda(
// 		[this](FHttpRequestPtr RequestPtr, FHttpResponsePtr ResponsePtr, bool bSucceeded)
// 	{
// 		if (!FHttpMgr::Get()->CheckHttpResponseValid(ResponsePtr, bSucceeded))
// 			return;
// 		FString ErrorMsg;
// 		TSharedPtr<FJsonObject> ResponseData = FHttpMgr::Get()->GetContentAsJsonObject(ResponsePtr, &ErrorMsg);
// 		if (!ResponseData.IsValid())
// 			return;

// 		const TSharedPtr<FJsonObject> &Data = ResponseData->GetObjectField(TEXT("data"));
// 		const TArray<TSharedPtr<FJsonValue>>* TopSections = nullptr;
// 		Data->TryGetArrayField(TEXT("list"), TopSections);
// 		for (auto& Section : *TopSections)
// 		{
// 			FTableInfo* pInfo = new FTableInfo;
// 			// pInfo->mCadState = ENotReady;
// 			pInfo->Id = Section->AsObject()->GetNumberField(TEXT("id"));
// 			pInfo->Name = Section->AsObject()->GetStringField(TEXT("name"));
// 			pInfo->ThumbnailUrl = Section->AsObject()->GetStringField(TEXT("thumbnailUrl"));
// 			pInfo->cadUrl = Section->AsObject()->GetStringField(TEXT("cadUrl"));
// 			FWHCModeGlobalData::TopSections.Emplace(pInfo->Id, MakeShareable(pInfo));
// 			DownTableInfo(pInfo);
// 		}
// 	}
// 	);
// 	HttpRequest->ProcessRequest();
// }