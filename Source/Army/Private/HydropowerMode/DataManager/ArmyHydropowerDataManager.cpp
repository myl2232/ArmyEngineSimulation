// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ArmyHydropowerDataManager.h"
#include "ArmyHttpModule.h"
#include "ArmyCommonTools.h"
#include "ArmyCommonTypes.h"
#include "ArmyObjectMinimal.h"
#include "ArmyGameInstance.h"

#define RequestPowerSystemProductURL "/api/hydropower/power-distribution-box/"
#define RequestPowerSystemLoopTypeURL "/api/hydropower/power-distribution-box-type/"
#define RequestPowerSystemLoopSampleURL "/api/hydropower/power-distribution-box/default/"//{setMealId}"


using namespace FContentItemSpace; 

FArmyHydropowerDataManager::FArmyHydropowerDataManager():CurrentOpacity(100)
{
}
 
FArmyHydropowerDataManager::~FArmyHydropowerDataManager()
{
}

TSharedPtr<FArmyHydropowerDataManager>& FArmyHydropowerDataManager::Get()
{
	static TSharedPtr<FArmyHydropowerDataManager> Instance  = nullptr;

	if (!Instance.IsValid())
	{
		Instance = MakeShareable(new FArmyHydropowerDataManager);
	}
	return Instance;
}

void FArmyHydropowerDataManager::GetProductInfo(int32 ProductType,TArray<TSharedPtr<class FContentItemSpace::FContentItem>>& OutArray)
{
	PowerSystemProductMap.MultiFind(ProductType,OutArray);
}


TSharedPtr<FPowerSystemLoop> FArmyHydropowerDataManager::GetPowerSystemLoop(int32 InID)
{
	for (auto& Item : PowerSystemLoopes)
	{
		if (Item->id == InID)
		{
			return Item;
		}
	}
	return nullptr;
}

void FArmyHydropowerDataManager::GetProductKeys(TArray<int32>& OutArray)
{
	TArray<int32> TemOutArray;
	PowerSystemProductMap.GenerateKeyArray(TemOutArray);
	for (int32 i = 0 ;i< TemOutArray.Num();i++)
		OutArray.AddUnique(TemOutArray[i]);
}


TSharedPtr<class FArmyPipePoint> FArmyHydropowerDataManager::MakeShareablePoint(FVector InLocation,EObjectType InType,FColor InColor,FColor ReformColor,bool bTrans /*= false*/)
{
	EObjectType LineTypeObj = InType;
	EObjectType LinerType =FArmyObject::GetPipeLinkerType(InType);
		
	//	(LineTypeObj==OT_Drain_Point ? OT_Drain_Linker : EObjectType((int32)(LineTypeObj-OT_Pipe_Begin)+OT_Linker_Begin));
	//if (LineTypeObj == OT_StrongElectricity_6)
	//{
	//	LinerType = OT_StrongElectricity_6_Linker;
	//}

	TArray<FObjectWeakPtr> Objects;
	FArmySceneData::Get()->GetObjects(E_HydropowerModel,LinerType,Objects);
	for (int32 i = 0;i<Objects.Num ();i++)
	{
		TSharedPtr<FArmyPipePoint> PipePoint = StaticCastSharedPtr<FArmyPipePoint> (Objects[i].Pin());
		float DistRaduis = (PipePoint->GetRadius ()*2)/10.f;
		if (PipePoint->GetLocation ().Equals (InLocation,DistRaduis))
		{
			if (!PipePoint->IsElectirc())
			{
				if (PipePoint->GetPPType ()==EPPT_Tee)
					break;
			}
			else if (PipePoint->IsElectirc())
			{
				if (PipePoint->PipeLines.Num ()>=2)
					break;
			}
			return PipePoint;
		}
	}
	TSharedPtr<FArmyPipePoint> New = MakeShareable(new FArmyPipePoint());
	New->SetLineType(LineTypeObj);
	New->SetLocation(InLocation);
	New->SetColor(InColor);
	New->SetReformColor(ReformColor);
	New->SetType(LinerType);
	FArmySceneData::Get()->Add(New,XRArgument(1).ArgUint32(E_HydropowerModel),nullptr,bTrans);
	return New;
}


TSharedPtr<class FArmyPipeline> FArmyHydropowerDataManager::CreatePipeline(TSharedPtr<FArmyPipePoint> StartPoint,TSharedPtr<FArmyPipePoint> EndPoint,TSharedPtr<class FArmyPipeRes>& Res,FObjectPtr ParentObject)
{
	if (StartPoint->GetLocation().Equals(EndPoint->GetLocation()))
	{
		TArray<TWeakPtr<FArmyPipeline>> OutLines;
		StartPoint->GetPipeLines(OutLines);
		if (OutLines.Num() == 0 )
		{
			FArmySceneData::Get()->Delete(StartPoint,true);
		}
		OutLines.Empty();
		EndPoint->GetPipeLines(OutLines);
		if (OutLines.Num()==0)
		{
			FArmySceneData::Get()->Delete(EndPoint,true);
		}
		return nullptr;
	}
	SCOPE_TRANSACTION(TEXT("绘制管线"));
	EObjectType LineType = (EObjectType)Res->ObjectType;
	TSharedPtr<FArmyPipeline> Result = MakeShareable (new FArmyPipeline(StartPoint,EndPoint,LineType));
	Result->SetType (LineType);
	Result->ProductID = Res->ID;
	Result->SetColor (Res->Color);
	Result->SetReformColor(Res->RefromColor);
	Result->SetPointColor(Res->PointColor);
	Result->SetPointRefromColor(Res->PointReformColor);
	Result->SetRadius(Res->Raduis);
	Result->SetName(Res->Name);
	StartPoint->AddPipeLine(Result);
	EndPoint->AddPipeLine(Result);
	FArmySceneData::Get()->Add(Result,XRArgument(1).ArgUint32(E_HydropowerModel).ArgFName(FName (*Res->ClassName)).ArgBoolean(true),ParentObject);
	//PipeGraph.Pin()->AddEdge(Result);
	return Result;
}


void FArmyHydropowerDataManager::Initialize()
{
	PowerSystemProductMap.Reset();
	PowerSystemLoopes.Empty();
	GetPowerSystemLoopType(1);
	GetPowerSystemLoopType();
	int32 i = 0;
}

void FArmyHydropowerDataManager::GetPowerSystemProductInfo(int32 ProductType)
{
    FString Url = RequestPowerSystemProductURL + FString::FromInt(ProductType);
    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &FArmyHydropowerDataManager::Callback_GetPowerSystemProductInfo, ProductType);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest(Url, CompleteDelegate);
    Request->ProcessRequest();
}

void FArmyHydropowerDataManager::Callback_GetPowerSystemProductInfo(FArmyHttpResponse Response, int32 productType)
{
    if (Response.bWasSuccessful)
    {
        const TArray<TSharedPtr<FJsonValue>> JArray = Response.Data->GetArrayField("data");

        for (auto& ArrayIt : JArray)
        {
            const TSharedPtr<FJsonObject> JDataIt = ArrayIt->AsObject();
            TSharedPtr<FContentItem> ParsedContentItem = FArmyCommonTools::ParseHydropowerContentItemFromJson(JDataIt);
            ParsedContentItem->ModeIdent = MI_HydropowerMode;
            PowerSystemProductMap.Add(productType, ParsedContentItem);
        }
    }
}

void FArmyHydropowerDataManager::GetPowerSystemLoopType(int32 LoopType /*= 2*/)
{
    FString Url = RequestPowerSystemLoopTypeURL + FString::FromInt(LoopType);
    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &FArmyHydropowerDataManager::Callback_GetPowerSystemLoopType, LoopType);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest(Url, CompleteDelegate);
    Request->ProcessRequest();
}

void FArmyHydropowerDataManager::Callback_GetPowerSystemLoopType(FArmyHttpResponse Response, int32 LoopType)
{
    if (Response.bWasSuccessful)
    {
        const TArray<TSharedPtr<FJsonValue>> JArray = Response.Data->GetArrayField("data");
        for (auto& ArrayIt : JArray)
        {
            TSharedPtr<FJsonObject> PowerSystemLoopJsonObject = ArrayIt->AsObject();
            TSharedPtr<FPowerSystemLoop> ParsedContentItem = MakeShareable(new FPowerSystemLoop);
            ParsedContentItem->Deserialization(PowerSystemLoopJsonObject);
            if (LoopType != 1)
            {
                PowerSystemLoopes.AddUnique(ParsedContentItem);
            }
            GetPowerSystemProductInfo(ParsedContentItem->id);
        }
    }
}

void FPowerSystemLoop::Deserialization(const TSharedPtr<FJsonObject>& InJsonObject)
{
	id = InJsonObject->GetIntegerField("id");
	Name = InJsonObject->GetStringField("name");
	 LoopType = InJsonObject->GetIntegerField("id");
	 spaceRepeat = InJsonObject->GetIntegerField("spaceRepeat");
	 spaceMultiSelect = InJsonObject->GetIntegerField("spaceMultiSelect");
}

// 获取样本回路数据开始
void FArmyHydropowerDataManager::InitPowerSystemLoopSamples()
{
	//从服务器获取样本回路数据
	int32 curMealId = FArmyUser::Get().GetCurrentMealID();
	//curMealId = 84; //84:测试用套餐id，套餐id需要从项目种获取
	if (curMealId != -1 && (curMealId != lastMealId || !(PowerSystemLoopSample.IsValid())))
	{
		GetPowerSystemLoopSamples(curMealId);
		lastMealId = curMealId;
	}
}

void FArmyHydropowerDataManager::GetPowerSystemLoopSamples(int32 MealId)
{
	GGI->Window->ShowThrobber(MAKE_TEXT("正在加载数据，请耐心等待..."));
	FString Url = RequestPowerSystemLoopSampleURL + FString::FromInt(MealId);
	FArmyHttpRequestCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindRaw(this, &FArmyHydropowerDataManager::Callback_GetPowerSystemLoopSamples, MealId);
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest(Url, CompleteDelegate);
	Request->ProcessRequest();
}

void FArmyHydropowerDataManager::Callback_GetPowerSystemLoopSamples(FArmyHttpResponse Response, int32 MealId)
{
	if (Response.bWasSuccessful)
	{
		const int32 code = Response.Data->GetIntegerField("code");
		const FString message = Response.Data->GetStringField("message");
		const TSharedPtr<FJsonObject> dataObj = Response.Data->GetObjectField("data");
		//
		if (code == 1 && dataObj.IsValid())
		{
			PowerSystemLoopSample = MakeShareable(new FPowerSystemLoopSample());
			PowerSystemLoopSample->Deserialization(dataObj);
		}
	}
	GGI->Window->HideThrobber();
}

void FPowerSystemLoopSample::Deserialization(const TSharedPtr<FJsonObject>& dataObj)
{
	//基本全局配置信息
	otcode = dataObj->GetStringField("code");
	otid = dataObj->GetIntegerField("id");
	otname = dataObj->GetStringField("name");
	otremarks = dataObj->GetStringField("remarks");
	otmealid = dataObj->GetIntegerField("setMealId");
	otmealname = dataObj->GetStringField("setMealName");
	otwiretypename = dataObj->GetStringField("wireTypeName");
	otwiretypeid = dataObj->GetIntegerField("wrieTypeId");
	//入户电线配置
	const TSharedPtr<FJsonObject> joinHouseWireObj = dataObj->GetObjectField("joinHouseWire");//
	{
		joinHouseWire = MakeShareable(new FPowerSystemJoinHouseWire);
		joinHouseWire->jhwBreakerGoodId = joinHouseWireObj->GetIntegerField("breakerGoodId");
		joinHouseWire->jhwBreakerGoodName = joinHouseWireObj->GetStringField("breakerGoodName");
		joinHouseWire->jhwCompanyId = joinHouseWireObj->GetIntegerField("companyId");
		joinHouseWire->jhwId = joinHouseWireObj->GetIntegerField("id");
		joinHouseWire->jhwIncomingCableType = joinHouseWireObj->GetIntegerField("incomingCableType");// 入户电缆类型 1："WDZBYJ-3X10-PC32-WC,CC"，2："WDZBYJ-5X10-PC32-WC,CC"，
		joinHouseWire->jhwIncomingCableTypeName = joinHouseWireObj->GetStringField("incomingCableTypeName");
		joinHouseWire->jhwPowerType = joinHouseWireObj->GetIntegerField("powerType");// 功率类型，1低功率，2高功率
		joinHouseWire->jhwSettingId = joinHouseWireObj->GetIntegerField("settingId");
	}
	//回路样本数据数组
	const TArray<TSharedPtr<FJsonValue>> loopSettingList = dataObj->GetArrayField("loopSettingList");//
	for (int32 i = 0; i < loopSettingList.Num(); ++i)
	{
		//回路样本对象
		const TSharedPtr<FJsonObject> loopObj = loopSettingList[i]->AsObject();//
		TSharedPtr<FPowerSystemLoopData> loopSetting = MakeShareable(new FPowerSystemLoopData);
		loopSetting->lBreakerGoodsId = loopObj->GetIntegerField("breakerGoodsId");
		loopSetting->lbreakerGoodsName = loopObj->GetStringField("breakerGoodsName");
		loopSetting->lid = loopObj->GetIntegerField("id");
		loopSetting->lloopType = loopObj->GetIntegerField("loopType");
		loopSetting->lloopTypeCode = loopObj->GetStringField("loopTypeCode");
		loopSetting->lloopTypeName = loopObj->GetStringField("loopTypeName");
		loopSetting->lsettingId = loopObj->GetIntegerField("settingId");
		loopSetting->lspaceAllSelect = loopObj->GetBoolField("spaceAllSelect");
		loopSetting->lwireGoodsId = loopObj->GetIntegerField("wireGoodsId");
		loopSetting->lwireGoodsName = loopObj->GetStringField("wireGoodsName");
		const TArray<TSharedPtr<FJsonValue>> lspaceIdList = loopObj->GetArrayField("spaceIdList");//
		for (int32 n = 0; n < lspaceIdList.Num(); ++n)
		{
			int32 spaceid = (int32)lspaceIdList[n]->AsNumber();//
			loopSetting->lspaceIdArr.Add(spaceid);
		}
		const TArray<TSharedPtr<FJsonValue>> lspaceList = loopObj->GetArrayField("spaceList");//
		for (int32 n = 0; n < lspaceList.Num(); ++n)
		{
			const TSharedPtr<FJsonObject> spaceObj = lspaceList[n]->AsObject();//
			TSharedPtr<FPowerSystemLoopSpace> space = MakeShareable(new FPowerSystemLoopSpace);
			space->sid = spaceObj->GetIntegerField("id");
			space->sname = spaceObj->GetStringField("name");
			loopSetting->lspaceArr.Add(space);
		}
		loopSettingArr.Add(loopSetting);
	}
}

TSharedPtr<FPowerSystemLoopSample> FArmyHydropowerDataManager::GetPowerSystemLoopSample()
{
	if (PowerSystemLoopSample.IsValid())
	{
		return PowerSystemLoopSample;
	}
	return nullptr;
}
// 获取样本回路数据结束
