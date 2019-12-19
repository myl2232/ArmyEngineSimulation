#include "ArmyAutoDesignDataManager.h"
#include "ArmyDownloadModule.h"
#include "DownloadFileSet.h"
#include "ArmyHttpModule.h"
#include "ArmyResourceModule.h"
#include "ArmyCommonTypes.h"
#include "SArmyModelContentBrowser.h"
#include "ArmyActorConstant.h"
#include "ArmyHydropowerModeController.h"
#include "ArmyGameInstance.h"
#include "ArmyDesignEditor.h"
#include "ArmyCommonTools.h"

FArmyAutoDesignDataManager::FArmyAutoDesignDataManager()
{
	Init();
}

FArmyAutoDesignDataManager::~FArmyAutoDesignDataManager()
{

}

void FArmyAutoDesignDataManager::Tick(float DeltaTime)
{
	UpdateDownloadProgress();
}

bool FArmyAutoDesignDataManager::IsTickable() const
{
	return true;
}

TStatId FArmyAutoDesignDataManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FArmyAutoDesignDataManager, STATGROUP_Tickables);
}

void FArmyAutoDesignDataManager::Init()
{
	////开关
	//ReqDataFileList.AddUnique(ADM_Switch1O1_Point);
	//ReqDataFileList.AddUnique(ADM_Switch2O1_Point);
	//ReqDataFileList.AddUnique(ADM_Switch3O1_Point);
	//ReqDataFileList.AddUnique(ADM_Switch4O1_Point);
	//ReqDataFileList.AddUnique(ADM_Switch1O2_Point);
	//ReqDataFileList.AddUnique(ADM_Switch2O2_Point);
	//ReqDataFileList.AddUnique(ADM_Switch3O2_Point);
	//ReqDataFileList.AddUnique(ADM_Switch4O2_Point);
	//ReqDataFileList.AddUnique(ADM_Switch_BathHeater);

	////插座
	//ReqDataFileList.AddUnique(ADM_Socket_Three_On_Point_3A);
	//ReqDataFileList.AddUnique(ADM_Socket_Three_3A);
	//ReqDataFileList.AddUnique(ADM_Socket_Five_2A);
	//ReqDataFileList.AddUnique(ADM_Socket_Three_Spatter);
	//ReqDataFileList.AddUnique(ADM_Socket_Five_Spatter);
	//ReqDataFileList.AddUnique(ADM_Socket_IT_TV);
	//ReqDataFileList.AddUnique(ADM_Socket_TV_Couplet);
	//ReqDataFileList.AddUnique(ADM_Socket_TP);

	////水位
	//ReqDataFileList.AddUnique(ADM_Water_Chilled_Point);
	//ReqDataFileList.AddUnique(ADM_Water_Hot_Point);
	//ReqDataFileList.AddUnique(ADM_Closestool);
	//ReqDataFileList.AddUnique(ADM_Tank_Basin);
	//ReqDataFileList.AddUnique(ADM_Bath_Brain);
	//ReqDataFileList.AddUnique(ADM_WashingMachine_Brain);
	//ReqDataFileList.AddUnique(ADM_tap);

}

bool FArmyAutoDesignDataManager::RequestData()
{
	ResquestGoodsIDList();
	//ReqData();
	return true;
}

AActor * FArmyAutoDesignDataManager::CreateActor(EAutoDesignModelID TypeID)
{
	int32 ModelId = AutoDesignPointMap.FindRef(TypeID);
	for (FContentItemPtr & Item : AutoDesignContentItems)
	{
		if (Item->ID == ModelId)
		{
			AActor * NewActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), Item);
			if (NewActor)
			{
				NewActor->Tags.Add(XRActorTag::AutoDesignPoint);

				NewActor->SetActorLabel(Item->Name);
				NewActor->SetFolderPath(FName(*(SArmyModelContentBrowser::FindObjectGroup(Item->CategryID))));

				//添加是否是商品的标记
				if (Item->bIsSale)
				{
					NewActor->Tags.Add(FName("IsSale"));
				}

				NewActor->Tags.Add(FName(TEXT("HydropowerActor")));

				if (Item->GetComponent().IsValid()&& GGI->DesignEditor->GetCurrentDesignMode().Equals(DesignMode::HydropowerMode))
				{
					TSharedPtr<FArmyHydropowerModeController> Hyd = StaticCastSharedPtr<FArmyHydropowerModeController>(GGI->DesignEditor->CurrentController);
					Hyd->ThreeTo2DGanged(Item->GetComponent(), NewActor);
				}

				return NewActor;
			}
		}
	} 
	return nullptr;
}

void FArmyAutoDesignDataManager::ResquestGoodsIDList()
{
	//请求数据
	FArmyHttpRequestCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindRaw(this, &FArmyAutoDesignDataManager::ResquestGoodsIDListResponse);
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest("/api/ai-wiring", CompleteDelegate);
	Request->ProcessRequest();
}

void FArmyAutoDesignDataManager::ResquestGoodsIDListResponse(struct FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful)
	{
		//AutoDesignContentItems.Reset();
		AutoDesignContentItems.Reset();
		ReqDataFileList.Reset();
		ReqDataDxfFileList.Reset();
		AutoDesignPointMap.Reset();
		// 根据同步的结果，创建的下载文件列表
		TArray<FDownloadSpace::FDownloadFileInfo> NeedToUpdateFiles;

		const TArray<TSharedPtr<FJsonValue>> JArray = Response.Data->GetArrayField("data");

		// 解析返回的列表
		for (auto& It : JArray)
		{
			TSharedPtr<FJsonObject> JObject = It->AsObject();
			int32 itemId = JObject->GetIntegerField("itemId");
			int32 itemType = JObject->GetIntegerField("itemType");// 单联单控开关
			int32 itemIdType = JObject->GetIntegerField("itemIdType");// itemId类型： 1 商品 2 构件 

			if (itemIdType == 1)
			{
				ReqDataFileList.Add(itemId);
			}
			else if(itemIdType == 2)
			{
				ReqDataDxfFileList.Add(itemType);
			}
			
			if (itemIdType == 1 /*|| itemIdType == 2*/)
			{
				EAutoDesignModelID TempModeID = GetModelIdByType(itemType);
				AutoDesignPointMap.Add(TempModeID, itemId);
				//AutoDesignPointMap.Reset();
			}

		}

		ReqData();

		//不再同步构件获取白模
		//ResquestDXFIDList();
	
	}
	else
	{
		GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("网络错误"));
	}
}

void FArmyAutoDesignDataManager::ResquestDXFIDList()
{
	//请求数据
	FString Url = TEXT("/api/ai-wiring/members?memberIds=");

	for (int32 i = 0;i<ReqDataDxfFileList.Num();i++)
	{
		if (i != 0)
		{
			Url.Append(TEXT(","));
		}
		Url.Append(FString::FromInt(ReqDataDxfFileList[i]));
	}

	FArmyHttpRequestCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindRaw(this, &FArmyAutoDesignDataManager::ResquestDxfIDListResponse);
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(Url, CompleteDelegate);
	Request->ProcessRequest();
}

void FArmyAutoDesignDataManager::ResquestDxfIDListResponse(struct FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful)
	{
		const TArray<TSharedPtr<FJsonValue>> JArray2 = Response.Data->GetArrayField("data");
		for (auto& It2 : JArray2)
		{
			const TSharedPtr<FJsonObject> JObject = It2->AsObject();
			FContentItemPtr Item = FArmyCommonTools::ParsePakFromDxfJson(JObject);
			if (Item.IsValid())
			{
				AutoDesignContentItems.Add(Item);
			}
		}

		ReqData();
	}
}

bool FArmyAutoDesignDataManager::ReqData()
{
	//构造下载列表
	if (ReqDataFileList.Num() < 1)
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("请联系管理员在后台配置自动布点数据！"));
		return false;
	}

	IdsArray.Reset();
	IdsArray.AddDefaulted(ReqDataFileList.Num() / 200 + 1);
	int32 Index = 0;
	for (int32 i = 0; i < ReqDataFileList.Num(); ++i)
	{
		IdsArray[Index].Append(FString::FromInt(ReqDataFileList[i]));
		if (i == 0 || i % 200 != 0)
		{
			if (i < ReqDataFileList.Num() - 1)
			{
				IdsArray[Index].Append(",");
			}
		}
		else
		{
			++Index;
		}
	}

	for (int32 i = 0; i < IdsArray.Num(); ++i)
	{
		FString Ids = IdsArray[i];

		if (!Ids.IsEmpty())
		{
			if (i == 0)
			{
				//显示下载进度
				TAttribute<FText> SyncProgressTextAttr = TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateRaw(this, &FArmyAutoDesignDataManager::GetDownloadProgressText));
				GGI->Window->ShowThrobber(SyncProgressTextAttr, FSimpleDelegate::CreateRaw(this, &FArmyAutoDesignDataManager::CancleDataDownloadCallBack));
			}

			FArmyHttpRequestCompleteDelegate CompleteDelegate;
			CompleteDelegate.BindRaw(this, &FArmyAutoDesignDataManager::ResSyncAutoDesignData,Ids);
			FString Url = FString::Printf(TEXT("/api/plans/load?ids=%s"), *Ids);
			IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(Url, CompleteDelegate);
			Request->ProcessRequest();
		}
	}

	return true;
}

void FArmyAutoDesignDataManager::ResSyncAutoDesignData(FArmyHttpResponse Response,FString InIds)
{
	IdsArray.Remove(InIds);
	if (Response.bWasSuccessful)
	{
		//AutoDesignContentItems.Reset();
		// 根据同步的结果，创建的下载文件列表
		TArray<FDownloadSpace::FDownloadFileInfo> NeedToUpdateFiles;

		const TArray< TSharedPtr<FJsonValue> > JArray = Response.Data->GetArrayField("data");
		// 解析返回的列表
		for (auto& It : JArray)
		{
			const TSharedPtr<FJsonObject> JObject2 = It->AsObject()->GetObjectField("data");

			bool IsHydropower = false;
			const TArray<TSharedPtr<FJsonValue>>* VArray = nullptr;
			if (JObject2->TryGetArrayField("goodsClassify", VArray))
			{
				for (int32 i = 0; i < VArray->Num(); ++i)
				{
					if ((*VArray)[i]->AsNumber() == 2)
					{
						IsHydropower = true;
						break;
					}
				}
			}

			if (IsHydropower)
			{
				FContentItemPtr Item = FArmyCommonTools::ParseHydropowerContentItemFromJson(JObject2);
				if (Item.IsValid())
				{
					Item->ModeIdent = MI_HydropowerMode;
					AutoDesignContentItems.Add(Item);
				}
			}
			else
			{
				FContentItemPtr Item = FArmyCommonTools::ParseHardModeContemItemFromJson(JObject2);
				if (Item.IsValid())
				{
					Item->ModeIdent = MI_SoftHardMode;
					AutoDesignContentItems.Add(Item);
				}
			}
		}

		for (auto&It : AutoDesignContentItems)
		{
			if (!It.IsValid())
				continue;
			TArray< TSharedPtr<FResObj> >resArr = It->GetResObjNoComponent();
			for (auto FileIt : resArr)
			{
				// 如果NetMD5或者URL为空，有可能是下架产品，不再作为下载对象
				if (FileIt->FileMD5 == TEXT("") || FileIt->FileURL == TEXT(""))
				{
					continue;
				}

				EFileExistenceState FileState = FArmyResourceModule::Get().GetResourceManager()->CheckFileExistState(FileIt->FilePath, FileIt->FileMD5);
				if (FileState == EFileExistenceState::Complete || FileState == EFileExistenceState::PendingDownload)
				{
					continue;
				}

				NeedToUpdateFiles.Add(FDownloadSpace::FDownloadFileInfo(It->ID, FileIt->FilePath, FileIt->FileURL, FileIt->FileMD5));
			}

			//// 下载构件
			//if (It->GetComponent().IsValid() && !(It->GetComponent()->FileState == EFileExistenceState::Exist || It->GetComponent()->FileState == EFileExistenceState::Complete))
			//{
			//	NeedToUpdateFiles.Add(FDownloadSpace::FDownloadFileInfo(It->ID, It->GetComponent()->FilePath, It->GetComponent()->FileURL, It->GetComponent()->FileMD5));
			//}

			// 下载构件
			if (It->GetComponent().IsValid())
			{
				// 下载俯视图图例
				if (!(It->GetComponent()->FileState == EFileExistenceState::Exist || It->GetComponent()->FileState == EFileExistenceState::Complete))
				{
					NeedToUpdateFiles.Add(FDownloadSpace::FDownloadFileInfo(
						It->ID, It->GetComponent()->FilePath, It->GetComponent()->FileURL, It->GetComponent()->FileMD5));
				}

				// 下载其他图例
				for (auto ResObj : It->GetComponent()->MapResObj)
				{
					TSharedPtr<FResObj> CurrentRes = ResObj.Value;
					if (CurrentRes.IsValid() && !(CurrentRes->FileState == EFileExistenceState::Exist || CurrentRes->FileState == EFileExistenceState::Complete))
					{
						NeedToUpdateFiles.Add(FDownloadSpace::FDownloadFileInfo(
							It->ID, CurrentRes->FilePath, CurrentRes->FileURL, CurrentRes->FileMD5));
					}
				}
			}


		}

		// 创建下载任务
		if (NeedToUpdateFiles.Num() > 0)
		{
			DownLoadTasks = FArmyDownloadModule::Get().GetDownloadManager()->AddMultiTask(NeedToUpdateFiles);
			DownLoadTasks->OnDownloadFinished = OnDownLoadFinished;
		}
		else
		{
			OnDownLoadFinished.ExecuteIfBound(true);
		}
	}
}
void FArmyAutoDesignDataManager::DownloadModelData()
{
}
void FArmyAutoDesignDataManager::CancleDataDownloadCallBack()
{
	GGI->Window->HideThrobber();
	OnDownLoadFinished.Unbind();
}
void FArmyAutoDesignDataManager::UpdateDownloadProgress()
{
	if (DownLoadTasks.IsValid())
	{
		DownloadProgress = DownLoadTasks->GetProgress();
	}
}

EAutoDesignModelID FArmyAutoDesignDataManager::GetModelIdByType(int32 inType)
{
	switch (inType)
	{
	case 1:
		return EAutoDesignModelID::ADM_Switch1O1_Point;
	case 2:
		return EAutoDesignModelID::ADM_Switch2O1_Point;
	case 3:
		return EAutoDesignModelID::ADM_Switch3O1_Point;
	case 4:
		return EAutoDesignModelID::ADM_Switch4O1_Point;
	case 5:
		return EAutoDesignModelID::ADM_Switch1O2_Point;
	case 6:
		return EAutoDesignModelID::ADM_Switch2O2_Point;
	case 7:
		return EAutoDesignModelID::ADM_Switch3O2_Point;
	case 8:
		return EAutoDesignModelID::ADM_Switch4O2_Point;
	case 9:
		return EAutoDesignModelID::ADM_Switch_BathHeater;
	case 10:
		return EAutoDesignModelID::ADM_Socket_Three_Point_16A;
	case 11:
		return EAutoDesignModelID::ADM_Socket_Five_Point_16A;
	case 12:
		return EAutoDesignModelID::ADM_Socket_Three_Point_10A;
	case 13:
		return EAutoDesignModelID::ADM_Socket_Five_Point_10A;
	case 14:
		return EAutoDesignModelID::ADM_Socket_Three_Spatter_Point_16A;
	case 15:
		return EAutoDesignModelID::ADM_Socket_Five_Spatter_Point_10A;
	case 16:
		return EAutoDesignModelID::ADM_Socket_TP;
	case 17:
		return EAutoDesignModelID::ADM_Socket_Living_Couplet;
	case 18:
		return EAutoDesignModelID::ADM_Socket_Net_TV_Couplet;
	case 19:
		return EAutoDesignModelID::ADM_Socket_Computer_Tel_Couplet;
	case 20:
		return EAutoDesignModelID::ADM_Socket_TV_Tel_Couplet;
	case 21:
		return EAutoDesignModelID::ADM_Socket_Computer;
	case 22:
		return EAutoDesignModelID::ADM_Socket_TV;
	case 23:
		return EAutoDesignModelID::ADM_Socket_Tel;
	case 24:
		return EAutoDesignModelID::ADM_Socket_Audio;
	case 25:
		return EAutoDesignModelID::ADM_Socket_Five_Point_USB_10A;
	case 26:
		return EAutoDesignModelID::ADM_Water_Hot_Water_OutLet;
	case 27:
		return EAutoDesignModelID::ADM_Water_Cold_Water_OutLet;
	case 28:
		return EAutoDesignModelID::ADM_WashingMachine_Brain;
	case 29:
		return EAutoDesignModelID::ADM_Water_The_Floor_Drain;
	case 30:
		return EAutoDesignModelID::ADM_Water_Launching_Of_A_Basin;
	case 31:
		return EAutoDesignModelID::ADM_Water_Floor_Drain;
	case 32:
		return EAutoDesignModelID::ADM_Water_Closestool_Drain;
	default:
		break;
	}
	return EAutoDesignModelID::ADM_None;
}

FString FArmyAutoDesignDataManager::GetModelNameByModelID(EAutoDesignModelID InID)
{
	switch (InID)
	{
	case ADM_Switch1O1_Point:
		return TEXT("单联单控开关");
	case ADM_Switch2O1_Point:
		return TEXT("双联单控开关");
	case ADM_Switch3O1_Point:
		return TEXT("三联单控开关");
	case ADM_Switch4O1_Point:
		return TEXT("四联单控开关");
	case ADM_Switch1O2_Point:
		return TEXT("单联双控开关");
	case ADM_Switch2O2_Point:
		return TEXT("双联双控开关");
	case ADM_Switch3O2_Point:
		return TEXT("三联双控开关");
	case ADM_Switch4O2_Point:
		return TEXT("四联双控");
	case ADM_Switch_BathHeater:
		return TEXT("浴霸开关");
	case ADM_Socket_Three_Point_16A:
		return TEXT("16A三孔插座");
	case ADM_Socket_Five_Point_16A:
		return TEXT("16A五孔插座");
	case ADM_Socket_Three_Point_10A:
		return TEXT("10A三孔插座");
	case ADM_Socket_Five_Point_10A:
		return TEXT("10A五孔插座");
	case ADM_Socket_Three_Spatter_Point_16A:
		return TEXT("带防溅盒16A三孔插座");
	case ADM_Socket_Five_Spatter_Point_10A:
		return TEXT("带防溅盒10A五孔插座");
	case ADM_Socket_TP:
		return TEXT("厨房台面联排插座");
	case ADM_Socket_Living_Couplet:
		return TEXT("客厅联排插座");
	case ADM_Socket_Net_TV_Couplet:
		return TEXT("网络+电视插座");
	case ADM_Socket_Computer_Tel_Couplet:
		return TEXT("电脑+电话插座");
	case ADM_Socket_TV_Tel_Couplet:
		return TEXT("电视+电话插座");
	case ADM_Socket_Computer:
		return TEXT("电脑插座");
	case ADM_Socket_TV:
		return TEXT("电视插座");
	case ADM_Socket_Tel:
		return TEXT("电话插座");
	case ADM_Socket_Audio:
		return TEXT("音频插座");
	case ADM_Socket_Five_Point_USB_10A:
		return TEXT("10A五孔插座带USB");
	case ADM_Water_Hot_Water_OutLet:
		return TEXT("热水口");
	case ADM_Water_Cold_Water_OutLet:
		return TEXT("冷水口");
	case ADM_WashingMachine_Brain:
		return TEXT("洗衣机地漏");
	case ADM_Water_The_Floor_Drain:
		return TEXT("普通地漏");
	case ADM_Water_Launching_Of_A_Basin:
		return TEXT("水盆下水");
	case ADM_Water_Floor_Drain:
		return TEXT("地漏下水");
	case ADM_Water_Closestool_Drain:
		return TEXT("马桶下水");
	default:
		break;
	}
	return TEXT("");
}

FString FArmyAutoDesignDataManager::GetRelyModelNameByModelID(int32 InID)
{
	switch (InID)
	{
		/*水位依赖的模型*/
	case AI_WashingMachine:
		return TEXT("洗衣机");
	case AI_Electric_Heater:
		return TEXT("电热水器");
	case AI_Gas_Heater:
		return TEXT("燃气热水器");
	case AI_Closestool:
		return TEXT("马桶");
	case AI_Sprinkler:
		return TEXT("花洒");
	case AI_BathroomArk:
		return TEXT("浴室柜");
	case AI_BathroomArk_Wall:
		return TEXT("壁挂浴室柜");

		/*开关依赖的模型*/
	case OT_Door:
		return TEXT("标准门");
	case OT_SecurityDoor:
		return TEXT("防盗门");
	case OT_SlidingDoor:
		return TEXT("推拉门");
	case AI_SingleBed:
		return TEXT("单人床");
	case AI_DoubleBed:
		return TEXT("双人床");

		/*插座依赖的模型*/
	case AI_Desk:
		return TEXT("书桌");
	case AI_DiningTable:
		return TEXT("餐桌");
	case AI_Hood:
		return TEXT("油烟机");
	case AI_Dresser:
		return TEXT("梳妆台");
	case AI_TVWall:
		return TEXT("壁挂电视");
	case AI_TVBench:
		return TEXT("电视柜");
	case AI_AirConditionedHang:
		return TEXT("壁挂空调");
	case AI_IntelClosestool:
		return TEXT("智能马桶");
	case AI_Sofa_Master:
		return TEXT("主位沙发");
	case AI_Refrigerator:
		return TEXT("冰箱");
	case AI_AirConditionedVert:
		return TEXT("立式空调");
		
	default:
		break;
	}
	return TEXT("");
}

void FArmyAutoDesignDataManager::Clear()
{
	//AutoDesignContentItems.Reset();
}

bool FArmyAutoDesignDataManager::IsExistAutoDesignModel(EAutoDesignModelID AutoDesignModelID)
{
	return AutoDesignPointMap.Contains(AutoDesignModelID);
}
