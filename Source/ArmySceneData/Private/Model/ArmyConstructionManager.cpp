#include "../../Public/Model/XRConstructionManager.h"
#include "ArmyHttpModule.h"
#include "IArmyHttp.h"
#include "SArmyMulitCategory.h"
#include "ArmyCommonTypes.h"
#include "ArmyBaseEditStyle.h"
#include "ArmySceneData.h"
#include "IArmyConstructionHelper.h"
#include "ArmyRoom.h"
#include "ArmyViewportClient.h"
#include "ArmyExtrusionActor.h"
//#include ""

void XRConstructionManager::Init()
{

	//绑定房间ID变化代理
	TArray<TWeakPtr<FArmyObject>> RoomObjs;
	FArmySceneData::Get()->GetObjects(EModelType::E_LayoutModel, EObjectType::OT_InternalRoom,  RoomObjs);
	
	for (auto & RoomObj : RoomObjs)
	{
		auto Room = StaticCastSharedPtr<FArmyRoom>(RoomObj.Pin());
		OnRoomAdded(Room);
	}
	FArmySceneData::Get()->OnRoomAdded.BindRaw(this, &XRConstructionManager::OnRoomAdded);
}

//TSharedPtr <XRConstructionManager>  XRConstructionManager::Me= MakeShared<XRConstructionManager>();
TSharedPtr<XRConstructionManager> XRConstructionManager::Get()
{
	static TSharedPtr <XRConstructionManager> Me = MakeShared<XRConstructionManager>();
	return Me;
}

void XRConstructionManager::TryToFindConstructionData(FGuid ObjectID, ConstructionPatameters Parameter, FConstructionDataDelegate CallBack)
{
	FGuid ObjId(ObjectID);
	ObjectConstructionKey Key(ObjId, Parameter);
	//auto Key = MakeShared<ObjectConstructionKey>();
	 
	if (!OriginalConstructionDataMap.Contains(Key.Parameters) /*如果没有数据就查询*/)
	{
		QueryQueue.AddUnique(Key);

		if (CallBack.IsBound())
		{
			auto&  Delegates = CallBacks.FindOrAdd(Key);
			Delegates.Add(CallBack);
		}
		TryStartAQuery();
		return;
	}

	//没有有现成的勾选数据
	if (!CheckedDatas.Contains(Key))
	{
		auto data = MakeShared<FArmyConstructionItemInterface>();
		data->SetConstructionItemCheckedId((*OriginalConstructionDataMap.Find(Key.Parameters)).DefaultChecked->CheckedId);
		CheckedDatas.Add(Key, data);
	}

	if (CallBack.IsBound())
	{
		(CallBack).ExecuteIfBound(EConstructionRequeryResult::Succeed, (*OriginalConstructionDataMap.Find(Key.Parameters)).OriginalData, *CheckedDatas.Find(Key), Key);
	}
}

OriginalData XRConstructionManager::GetJsonData(const ConstructionPatameters & Parameter)
{
	auto data = OriginalConstructionDataMap.Find(Parameter);
	if (data)
	{
		return data->OriginalData;
	}
	return nullptr;
}

void XRConstructionManager::SaveConstructionData(FGuid  ObjectID, ConstructionPatameters  Parameter, CheckedData NewCheckData, bool Copy/*=true*/)
{
	if (Copy)
	{
		CheckedData data = MakeShared<FArmyConstructionItemInterface>();
		data->bHasSetted = true;
		data->SetConstructionItemCheckedId(NewCheckData->CheckedId);
		AddCheckedConstructionData(ObjectConstructionKey(ObjectID, Parameter), data);
	}
	else
	{
		NewCheckData->bHasSetted = true;
		AddCheckedConstructionData(ObjectConstructionKey(ObjectID, Parameter), NewCheckData);
	}
}

CheckedData XRConstructionManager::GetSavedCheckedData(const FGuid & ObjectID, EConstructionType Type /*= ConstructionType::All*/)
{
	CheckedData data = MakeShared<FArmyConstructionItemInterface>();
	for (const auto & Data : CheckedDatas)
	{
		if (Data.Key.ObjectID != ObjectID)
		{
			continue;
		}
		if (Type != EConstructionType::All)
		{
			if (Data.Key.Parameters.Type != Type)
			{
				continue;
			}
		}
		data->CheckedId.Append(Data.Value->CheckedId);
	}
	return data;
}

CheckedData XRConstructionManager::GetSavedCheckedData(const FGuid & ObjectID,const ConstructionPatameters & Parameter)
{

	FGuid ObjId(ObjectID);
	ObjectConstructionKey Key(ObjId, Parameter);

	if (!OriginalConstructionDataMap.Contains(Key.Parameters) /*如果没有数据就查询*/)
	{
		QueryQueue.AddUnique(Key);
		//auto&  Delegates = CallBacks.FindOrAdd(*Key);
		TryStartAQuery();

		auto data = MakeShared<FArmyConstructionItemInterface>();
		//data->SetConstructionItemCheckedId((*OriginalConstructionDataMap.Find(Key->Parameters)).DefaultChecked->CheckedId);
		CheckedDatas.Add(Key, data);
		return  data;
	}

	//没有有现成的勾选数据
	if (!CheckedDatas.Contains(Key))
	{
		auto data = MakeShared<FArmyConstructionItemInterface>();
		data->SetConstructionItemCheckedId((*OriginalConstructionDataMap.Find(Key.Parameters)).DefaultChecked->CheckedId);
		CheckedDatas.Add(Key, data);
		return data;
	}

	auto data = CheckedDatas.Find(Key);
	return *data;
}

CheckedData XRConstructionManager::ParseJson(TSharedPtr<FJsonObject> inOrignalData, TSharedPtr<SArmyMulitCategory>* outUI)
{
	auto DefaultChecked = MakeShared<FArmyConstructionItemInterface>();

	auto UI = MakeShared<SArmyMulitCategory>();
	//解析json获得默认施工项
	auto data = UI->ParseData(inOrignalData);
	DefaultChecked->SetConstructionItemCheckedId(data);
	if (outUI)
	{
		*outUI = UI;
	}
	return DefaultChecked;
}

void XRConstructionManager::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//存储所有的key和所有的勾选信息
	JsonWriter->WriteObjectStart("AllConstructionData");

	//存储所有的key
	TArray<ObjectConstructionKey> Keys;
	CheckedDatas.GenerateKeyArray(Keys);

	for (auto& Query : QueryQueue)
	{
		Keys.AddUnique(Query);
	}

	for (auto& Query : CurrentQuery)
	{
		Keys.AddUnique(Query);
	}

	for (int32 i = 0; i < Keys.Num();)
	{
		if (!FArmySceneData::Get()->GetObjectByGuidAlone(Keys[i].ObjectID).IsValid())
		{
			Keys.RemoveAt(i);
		}
		else
		{
			++i;
		}
	}

	if (Keys.Num() > 0)
	{
		JsonWriter->WriteArrayStart("QueryKeys");
		for (const auto Key : Keys)
		{

			JsonWriter->WriteObjectStart();
			Key.SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();
	}

	//存储所有的勾选信息

	if (CheckedDatas.Num() > 0)
	{
		JsonWriter->WriteArrayStart("CheckedDatas");

		for (const auto CheckedData : CheckedDatas)
		{
			JsonWriter->WriteObjectStart();
			CheckedData.Key.SerializeToJson(JsonWriter);
			CheckedData.Value->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();
	}


	JsonWriter->WriteObjectEnd();
}

void XRConstructionManager::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{

	//暂存一部分原始数据
	TMap<ConstructionPatameters /*施工项参数*/, OriginalConstructionData /*与参数对应的原始施工项以及默认勾选数据*/> TempOrignalData;
	for (auto DataMap : OriginalConstructionDataMap)
	{
		if (ProtectedConstructionParameter.Contains(DataMap.Key))
		{
			TempOrignalData.Add(DataMap.Key, DataMap.Value);
		}
	}
	OriginalConstructionDataMap.Empty();
	OriginalConstructionDataMap.Append(TempOrignalData);

	CheckedDatas.Empty();
	CallBacks.Empty();
	CurrentQuery.Empty();
	QueryQueue.Empty();

	//所有Key
	auto ObjData = InJsonData->GetObjectField("AllConstructionData");
	auto KeysData = ObjData->GetArrayField("QueryKeys");

	for (const auto& KeyData : KeysData)
	{
		ObjectConstructionKey Key;// = MakeShared<ObjectConstructionKey>();
		Key.Deserialization(KeyData->AsObject());

		QueryQueue.Add(Key);
	}

	//所有的勾选信息
	auto CheckedDatasObj = ObjData->GetArrayField("CheckedDatas");

	for (const auto& CheckedDataObj : CheckedDatasObj)
	{
		auto Key = MakeShared<ObjectConstructionKey>();
		Key->Deserialization(CheckedDataObj->AsObject());
		auto ConstructionCheckedData = MakeShared<FArmyConstructionItemInterface>();
		ConstructionCheckedData->Deserialization(CheckedDataObj->AsObject());
		CheckedDatas.Add(*Key, ConstructionCheckedData);
	}

	TryStartAQuery();
}



void XRConstructionManager::AddOriginalConstructionData(ConstructionPatameters Param, OriginalData Data)
{
	OriginalConstructionData ConstructionData;
	ConstructionData.OriginalData = Data;
	ConstructionData.DefaultChecked = ParseJson(Data, nullptr);
	OriginalConstructionDataMap.Add(Param, ConstructionData);
}

void XRConstructionManager::AddCheckedConstructionData(ObjectConstructionKey Key, CheckedData Value)
{
	CheckedDatas.Add(Key, Value);
}

//尝试开始一个请求
void XRConstructionManager::TryStartAQuery()
{
	if (QueryQueue.Num() <= 0)
	{
		return;
	}

	for (auto& Query : QueryQueue)
	{
		if (!CurrentQuery.Contains(Query))
		{
			if (!OriginalConstructionDataMap.Contains(Query.Parameters))
			{
				CurrentQuery.Add(Query);
				FString Url = Query.Parameters.GetQueryUrl();
				IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(Url, FArmyHttpRequestCompleteDelegate::CreateRaw(this, &XRConstructionManager::OnGetResponseForKeyQuery, Query));
				Request->ProcessRequest();
			}

		}
	}
	QueryQueue.Empty();

}

void XRConstructionManager::OnGetResponseForKeyQuery(FArmyHttpResponse Response, ObjectConstructionKey  Query)
{

	if (Response.bWasSuccessful)
	{
		AddOriginalConstructionData(Query.Parameters, Response.Data);

		//执行回调
		if (!CheckedDatas.Contains(Query) ||
			(CheckedDatas.Contains(Query) && !(*CheckedDatas.Find(Query))->bHasSetted)
			)
		{
			AddCheckedConstructionData(Query, ParseJson(Response.Data, nullptr));
		}

		auto  CallBacksShouldCall = CallBacks.Find(Query);
		if (CallBacksShouldCall)
		{
			for (const auto & CallBack : (*CallBacksShouldCall))
			{
				CallBack.ExecuteIfBound(EConstructionRequeryResult(Response.bWasSuccessful), Response.Data, *CheckedDatas.Find(Query), Query);
			}
		}
		CallBacks.Remove(Query);
	}
	else
	{
		
		UE_LOG(LogTemp, Warning, TEXT("函数XRConstructionManager::OnGetResponse：  请求失败"))
	
		auto  CallBacksShouldCall = CallBacks.Find(Query);
		if (CallBacksShouldCall)
		{
			for (const auto & CallBack : (*CallBacksShouldCall))
			{
				CallBack.ExecuteIfBound(EConstructionRequeryResult(Response.bWasSuccessful),nullptr, nullptr, Query);
			}
		}
	}

	CurrentQuery.Remove(Query);
	 //开始查询下一个
	TryStartAQuery();
}

//void XRConstructionManager::OnGetResponseForParameterQuery(FArmyHttpResponse Response, ConstructionPatameters Parameter)
//{
//	if (Response.bWasSuccessful)
//	{
//		AddOriginalConstructionData(Parameter, Response.Data);
//	}
//	else
//	{
//		//暂不考虑失败的情况
//		UE_LOG(LogTemp, Warning, TEXT("函数XRConstructionManager::OnGetResponseForParameterQuery：  请求失败"))
//	}
//
//}

void XRConstructionManager::OnRoomAdded(TSharedPtr<FArmyRoom> Room)
{
	if (!Room->SpaceIDChanged.IsBound())
	{
		Room->SpaceIDChanged.BindRaw(this, &XRConstructionManager::OnRoomSpaceIDChanged);
	}
}

void XRConstructionManager::OnRoomSpaceIDChanged(TSharedPtr<FArmyRoom> Room)
{
	FArmySceneData::Get()->ChangeConstructionHelpersSpaceID(Room);
}

//纯移施工项数据
//@ InterfaceID 向后台查询数据时需要的参数

void ConstructionPatameters::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) const
{
	static const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EConstructionType"), true);
	if (!EnumPtr)
	{
		//出现错误没有找到枚举对象
		return;
	}
	JsonWriter->WriteObjectStart("Parameters");
	JsonWriter->WriteValue<FString>("Type", EnumPtr->GetNameByValue((int64)this->Type).ToString());
	JsonWriter->WriteValue<int32>("Ver", this->Version);
	JsonWriter->WriteValue<int32>("P1", this->IntParam1);
	JsonWriter->WriteValue<int32>("P2", this->IntParam2);
	JsonWriter->WriteValue<int32>("P3", this->IntParam3);
	JsonWriter->WriteValue<int32>("P4", this->IntParam4);
	JsonWriter->WriteValue<int32>("P5", this->SpaceIDParam);
	JsonWriter->WriteValue<int32>("P6", this->IntParam6);
	JsonWriter->WriteObjectEnd();
}

void ConstructionPatameters::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	static const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EConstructionType"), true);
	if (!EnumPtr)
	{
		//出现错误没有找到枚举对象
		return;
	}
	auto ParameterObj = InJsonData->GetObjectField("Parameters");

	FString EnumNameStr = ParameterObj->GetStringField("Type");
	Type = (EConstructionType)EnumPtr->GetValueByNameString(EnumNameStr);
	Version = ParameterObj->GetIntegerField("Ver");
	if (Version == 0)
	{
		Version = (int32)CurrentConstructionDataVersion;
	}
	IntParam1 = ParameterObj->GetIntegerField("P1");
	IntParam2 = ParameterObj->GetIntegerField("P2");
	IntParam3 = ParameterObj->GetIntegerField("P3");
	IntParam4 = ParameterObj->GetIntegerField("P4");
	ParameterObj->TryGetNumberField("P5", SpaceIDParam);
	ParameterObj->TryGetNumberField("P6", IntParam6);
}

//根据参数生成查询用的url
FString ConstructionPatameters::GetQueryUrl()
{
	switch (Type)
	{
	case EConstructionType::NormalGoods:
		return FString::Printf(TEXT("/api/quota/goods/%d"), IntParam1);
	case EConstructionType::PureOrignalOffset:
	{
		FString OffsetParameter;

		switch (IntParam1)
		{
		case 17:
			//弱电箱
			OffsetParameter = TEXT("weak_electric_box_move");
			break;
		case 16:
			//强电箱
			OffsetParameter = TEXT("strong_electric_box_move");
			break;
		case 18:
			//马桶下水
			OffsetParameter = TEXT("toilet_water_move");
			break;
		case 19:
			//水盆下水
			OffsetParameter = TEXT("basin_water_move");
			break;
		case 20:
			//地漏下水
			OffsetParameter = TEXT("floor_drain_water_move");
			break;
		case 21:
			//燃气表
			OffsetParameter = TEXT("gas_meter_move");
			break;
		case 22:
			//冷水点位
			OffsetParameter = TEXT("cold_water_move");
			break;
		case 23:
			//热水点位
			OffsetParameter = TEXT("hot_water_move");
			break;
		case 24:
			//中水点位
			OffsetParameter = TEXT("middle_water_move");
			break;
		case 25:
			//冷热水点位
			OffsetParameter = TEXT("cold_and_hot_water_move");
			break;
		case 26:
			//分集水器
			OffsetParameter = TEXT("divider_move");
			break;

		default:
			//不允许运行到这里
			OffsetParameter = FString();

		}
		return FString(TEXT("/api/quota/construction_object/")) + OffsetParameter;
	}
	break;
	case EConstructionType::Pave:
	{
		//传入Parameter3，获得对应的铺法参数名
		auto GetPaveParameterName = [](int32 Parameter3)->FString
		{
			switch (Parameter3)
			{
			case 1:
				return FString(TEXT("straight_spread"));
			case 2:
				return FString(TEXT("I-shape_spread"));
			case 3:
				return FString(TEXT("cyclone_spread"));
			case 4:
				return FString(TEXT("herringbone_spread"));
			case 5:
				return FString(TEXT("369_spread"));
			case 6:
				return FString(TEXT("wave_line_spread"));
			case 7:
				return FString(TEXT("S_SlopeContinueStyle"));
			default:
				return FString();
			}
		};

		//传入Parameter2(墙顶地)和Parameter4(是否下吊)，获得对应的铺法参数名
		auto GetSurfaceParameterName = [](int32 Parameter2, int32 Parameter4)->int32
		{
			switch (Parameter2)
			{
			case 3:
				if (Parameter4>0)
				{
					return 4;
				}
			case 1:
			case 2:
				return Parameter2;
			default:
				//错误的调用了本函数
				return -1;
			}
		};

		if (Version == (int32)EConstructionVersion::V140)
		{
			return  FString::Printf(TEXT("/api/quota/goods/%d"), IntParam1);
		}

		if (Version == (int32)EConstructionVersion::V150)
		{
			//return IntParam3 == -1 ?
			FString::Printf(TEXT("/api/quota/goods/%d?constructionSurface=%d"), IntParam1, GetSurfaceParameterName(IntParam2, IntParam4));
			//:FString::Printf(TEXT("/api/quota/goods/%d?constructionSurface=%d&&paveId=%d"), IntParam1, IntParam2, IntParam3);
		}

		if (Version >= (int32)EConstructionVersion::V160)
		{

			FString RetVal = FString::Printf(TEXT("/api/quota/goods/%d"), IntParam1);

			TArray<FString> Parameters;

 			if (IntParam2 != -1)
			{
				Parameters.Add(FString::Printf(TEXT("constructionSurface=%d"), GetSurfaceParameterName(IntParam2, IntParam4)));
			}

			if (IntParam3 != -1)
			{
				Parameters.Add(FString::Printf(TEXT("pave=%s"), *GetPaveParameterName(IntParam3)));
			}


			for (int32 i = 0; i < Parameters.Num(); i++)
			{
				if (i == 0)
				{
					RetVal += FString(TEXT("?"));
				}
				else
				{
					RetVal += FString(TEXT("&&"));
				}
				RetVal += Parameters[i];
			}
			return RetVal;
		}
	}
	//不允许运行到此处
	return FString();
	break;
	case EConstructionType::OriginalSurface:
		if (IntParam2 != -1)
		{	
			//有顶面下吊
			return FString(TEXT("/api/quota/construction_object/lower_hanging_area"));
		}
		else
		{
			EWallType WallType = EWallType::Wall;
			switch (IntParam1)
			{
			case 1:
				WallType = EWallType::Wall;
				break;
			case 2:
				WallType = EWallType::Roof;
				break;
			case 3:
				WallType = EWallType::Floor;
				break;
			}
			switch (WallType)
			{
			case EWallType::Wall:
				return FString(TEXT("/api/quota/construction_object/original_wall_surface"));
			case EWallType::Floor:
				return FString(TEXT("/api/quota/construction_object/original_ground_surface"));
			case EWallType::Roof:
				return FString(TEXT("/api/quota/construction_object/original_top_surface"));
			}
			//不允许运行到此处
			return FString();
		}
		break;
	default:
		return FString();
	}

}

void ConstructionPatameters::SetNormalGoodsInfo(int32 GoodsID, int32 SpaceID )
{
	Type = EConstructionType::NormalGoods;
	IntParam1 = GoodsID;
	SpaceIDParam = SpaceID;
	//其他参数恢复默认
	IntParam2 = -1;
	IntParam3 = -1;
	IntParam4 = -1;

	IntParam6 = -1;
}

void ConstructionPatameters::SetPaveInfo(int32 GoodsID, EWallType WallType, EStyleType PaveMethod, bool HasHung /*= false*/, EConstructionVersion NewVersion/*== CurrentConstructionDataVersion*/)
{
	Type = EConstructionType::Pave;
	IntParam1 = GoodsID;
	Version = (int32)NewVersion;
	switch (WallType)
	{
	case EWallType::Roof:
		IntParam2 = 2;
		break;
	case EWallType::Floor:
		IntParam2 = 3;
		break;

	case EWallType::Wall:
		IntParam2 = 1;
		break;
	default:
		IntParam2 = -1;
	}


	switch (PaveMethod)
	{
	case EStyleType::S_WorkerStyle:
		IntParam3 = 2;
		break;
	case EStyleType::S_WhirlwindStyle:
		IntParam3 = 3;
		break;
	case EStyleType::S_HerringBoneStyle:
		IntParam3 = 4;
		break;
	case EStyleType::S_TrapeZoidStyle:
		IntParam3 = 5;
		break;
	case EStyleType::S_BodaAreaStyle:
		IntParam3 = 6;
		break;

	case EStyleType::S_SlopeContinueStyle:
		IntParam3 = 7;
		break;

	case EStyleType::S_ContinueStyle:
		IntParam3 = 1;
		break;
	default:
		IntParam3 = -1;
	}

	if (EWallType::Roof == WallType)
	{
		IntParam4 = HasHung;
		return;
	}
	//其他参数恢复默认
	IntParam4 = -1;

}

void ConstructionPatameters::SetPaveInfo(TSharedPtr<FArmyBaseEditStyle> Style, EWallType WallType, bool HasHung /* = false*/)
{
check(Style.IsValid())
		Type = EConstructionType::Pave;
	IntParam1 = Style->GetGoodsID();
	Version = (int32)Style->ConstructionVersion;

	//用IntParam2代表墙顶地
	switch (WallType)
	{
	case EWallType::Roof:
		IntParam2 = 2;
		break;
	case EWallType::Floor:
		IntParam2 = 3;
		break;

	case EWallType::Wall:
		IntParam2 = 1;
		break;
	default:
		IntParam2 = -1;
	}

	//用IntParam3代表铺法
	if (!Style->GetIsNonePaveMethod())
	{
		switch ((EStyleType)Style->GetEditType())
		{
		case EStyleType::S_WorkerStyle:
			IntParam3 = 2;
			break;
		case EStyleType::S_WhirlwindStyle:
			IntParam3 = 3;
			break;
		case EStyleType::S_HerringBoneStyle:
			IntParam3 = 4;
			break;
		case EStyleType::S_TrapeZoidStyle:
			IntParam3 = 5;
			break;
		case EStyleType::S_BodaAreaStyle:
			IntParam3 = 6;
			break;

		case EStyleType::S_SlopeContinueStyle:
			IntParam3 = 7;
			break;

		case EStyleType::S_ContinueStyle:
			IntParam3 = 1;
			break;
		default:
			IntParam3 = -1;
		}
	}
	else
	{
		IntParam3 = -1;
	}

	//用IntParam4代表铺法是不是下吊面
	if (EWallType::Roof == WallType &&HasHung)
	{
		IntParam4 = 1;
		
	}
	else
	{
		IntParam4 = -1;
	}
	
	//其他参数恢复默认
	IntParam6 = -1;
	SpaceIDParam = -1;

}

void ConstructionPatameters::SetPaveInfo2(TSharedPtr<class FArmyBaseEditStyle> Style, EWallType WallType, int32 SpaceID)
{
	check(Style.IsValid())
		Type = EConstructionType::Pave;
	IntParam1 = Style->GetGoodsID();
	Version = (int32)Style->ConstructionVersion;

	//用IntParam2代表墙顶地
	switch (WallType)
	{
	case EWallType::Roof:
		IntParam2 = 2;
		break;
	case EWallType::Floor:
		IntParam2 = 3;
		break;

	case EWallType::Wall:
		IntParam2 = 1;
		break;
	default:
		IntParam2 = -1;
	}

	//用IntParam3代表铺法
	if (!Style->GetIsNonePaveMethod())
	{
		switch ((EStyleType)Style->GetEditType())
		{
		case EStyleType::S_WorkerStyle:
			IntParam3 = 2;
			break;
		case EStyleType::S_WhirlwindStyle:
			IntParam3 = 3;
			break;
		case EStyleType::S_HerringBoneStyle:
			IntParam3 = 4;
			break;
		case EStyleType::S_TrapeZoidStyle:
			IntParam3 = 5;
			break;
		case EStyleType::S_BodaAreaStyle:
			IntParam3 = 6;
			break;

		case EStyleType::S_SlopeContinueStyle:
			IntParam3 = 7;
			break;

		case EStyleType::S_ContinueStyle:
			IntParam3 = 1;
			break;
		default:
			IntParam3 = -1;
		}
	}
	else
	{
		IntParam3 = -1;
	}

	//用IntParam4代表面有没有凸出
	float ExtrusionHeight = Style->GetHeightToFloor();
	if (ExtrusionHeight)
	{
		IntParam4 = 1;
	}
	else
	{
		IntParam4 = -1;
	}

	//用SpaceIDParam代表空间ID
	SpaceIDParam = SpaceID;

	//其他参数恢复默认
	IntParam6 = -1;
}

void ConstructionPatameters::SetOriginalSurface(EWallType WallType, bool Hung, int32 SpaceID)
{
	Type = EConstructionType::OriginalSurface;
	switch (WallType)
	{
	case EWallType::Roof:
		IntParam1 = 2;
		break;
	case EWallType::Floor:
		IntParam1 = 3;
		break;

	case EWallType::Wall:
	default:
		IntParam1 = 1;
	}

	//用参数2代表有没有下吊
	if (WallType == EWallType::Roof &&Hung)
	{
		IntParam2 = 1;
	}

	SpaceIDParam = SpaceID;

	//其他参数恢复默认
	IntParam3 = -1;
	IntParam4 = -1;
	IntParam6 = -1;
}

bool ConstructionPatameters::VersionLessThan(EConstructionVersion VersionToCompare)
{
	return  Version < (int32)VersionToCompare;
}

void ConstructionPatameters::SetPureOrignalOffset(EComponentID ComponentID, int32 SpaceID )
{
	Type = EConstructionType::PureOrignalOffset;
	int32 TypeID = -1;
	switch (ComponentID)
	{
	case  EComponentID::EC_EleBoxL_Point:
		TypeID = 17;
		break;
	case  EComponentID::EC_EleBoxH_Point:
		TypeID = 16;
		break;

	case  EComponentID::EC_Water_Hot_Point:
		TypeID = 23;
		break;
	case EComponentID::EC_Gas_Meter:// = 1201 //燃气表
		TypeID = 21;
		break;
	case	 EComponentID::EC_Water_Normal_Point: //中水点位
		TypeID = 24;
		break;
	case	 EComponentID::EC_Water_Separator_Point://分集水器
		TypeID = 26;
		break;
	case	 EComponentID::EC_Water_Chilled_Point: //= 17,//冷水点位
		TypeID = 22;
		break;
	case	 EComponentID::EC_Water_Hot_Chilled_Point: //= 18,//冷热水
		TypeID = 25;
		break;
	case	 EComponentID::EC_Basin://= 60,//地漏下水
		TypeID = 20;
		break;
	case	 EComponentID::EC_Water_Basin:// = 59,//水盆下水
		TypeID = 19;
		break;
	case	 EComponentID::EC_Closestool:// = 31,//马桶下水
		TypeID = 18;

	default:
		TypeID = -1;
	}

	IntParam1 = TypeID;
	SpaceIDParam = SpaceID;
	//其他参数恢复默认
	IntParam2 = -1;
	IntParam3 = -1;
	IntParam4 = -1;
	IntParam6 = -1;
}

void ObjectConstructionKey::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) const
{

	JsonWriter->WriteValue<FString>("ID", this->ObjectID.ToString());
	Parameters.SerializeToJson(JsonWriter);
}

void ObjectConstructionKey::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FGuid::Parse(InJsonData->GetStringField("ID"), ObjectID);
	Parameters.Deserialization(InJsonData);

	//兼容V1.9之前的旧方案，添加空间ID
	if(Parameters.VersionLessThan(EConstructionVersion::V190))
	{ 
		//获得对应的object
		auto Object = FArmySceneData::Get()->GetObjectByGuid(EModelType::E_LayoutModel, this->ObjectID);
		auto  IConstruction =  dynamic_cast<IArmyConstructionHelper*>(Object.Pin().Get());
		if(IConstruction)
		{ 
			this->Parameters.SetSpaceID(IConstruction->GetRoomSpaceID());
		}
	}

}
