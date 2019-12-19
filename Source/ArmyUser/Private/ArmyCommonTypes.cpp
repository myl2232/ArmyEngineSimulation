#include "ArmyCommonTypes.h"
#include "ArmyFurniture.h"

FFurnitureProperty::FFurnitureProperty()
{
	FurContentItem = MakeShareable(new FContentItemSpace::FContentItem());
	CalContentItemID();
}

FFurnitureProperty::FFurnitureProperty(EResourceType _Type)
{
	FurContentItem = MakeShareable(new FContentItemSpace::FContentItem());
	CalContentItemID();
	if (_Type == EResourceType::Hydropower)
	{
		FurContentItem->ResObjArr.Add(MakeShareable(new FContentItemSpace::FComponentRes("", "", "", "", EResourceType::ComponentDXF, 0, 0)));
		FurContentItem->ResObjArr.Add(MakeShareable(new FContentItemSpace::FArmyPipeRes("", "", "", "", EResourceType::HardPAK, "", "", "", "")));
		FurContentItem->ProObj = MakeShareable(new FContentItemSpace::FHydropowerProductObj());
	}
	SetType(_Type);

}

FFurnitureProperty::FFurnitureProperty(TSharedPtr<FFurnitureProperty> Copy)
{
	if (Copy.IsValid())
	{
		FurContentItem = MakeShareable(new FContentItemSpace::FContentItem());
		CalContentItemID();
		if (Copy->GetType() == EResourceType::Hydropower)
		{
			FurContentItem->ResObjArr.Add(MakeShareable(new FContentItemSpace::FComponentRes("", "", "", "", EResourceType::ComponentDXF, 0, 0)));
			FurContentItem->ResObjArr.Add(MakeShareable(new FContentItemSpace::FArmyPipeRes("", "", "", "", EResourceType::HardPAK, "", "", "", "")));
			FurContentItem->ProObj = MakeShareable(new FContentItemSpace::FHydropowerProductObj());
		}

		if (FurContentItem->ResObjArr.Num() > 0 && FurContentItem->ProObj.IsValid())
		{
			SetType(Copy->GetType());

			SetRadius(Copy->GetRadius());
			SetLength(Copy->GetLength());
			SetWidth(Copy->GetWidth());
			SetHeight(Copy->GetHeight());
			SetAltitude(Copy->GetAltitude());
			SetComponentPath(Copy->GetComponentPath());
			SetModelPath(Copy->GetModelPath());
			SetBelongClass(Copy->GetBelongClass());
			SetbIsPakModel(Copy->GetbIsPakModel());
			SetObjectType(Copy->GetObjectType());
			SetAltitude(Copy->GetAltitude());
			SetOptionsAltitude(Copy->GetOptionsAltitude());
			SetOptimizeParam(FPaths::SetExtension(Copy->GetModelPath(), TEXT(".json")));
			SetModeType(Copy->GetModelType());

		}
		// 复制其他构件
		for (auto It : Copy->GetLegendMap())
		{
			LegendMap.Add(It.Key, It.Value);
		}
		
	}
}

bool FFurnitureProperty::SetRadius(float _Radius)
{
	if (FurContentItem->ProObj.IsValid() && FurContentItem->ResourceType == EResourceType::Hydropower)
	{
		for (auto arr : FurContentItem->ResObjArr)
		{
			if (arr->ResourceType == HardPAK)
			{
				TSharedPtr<FContentItemSpace::FArmyPipeRes> obj = StaticCastSharedPtr<FContentItemSpace::FArmyPipeRes>(arr);
				if (obj.IsValid())
				{
					obj->Raduis = _Radius *10.f;
					return true;
				}
			}
		}
	}
	return false;
}

float FFurnitureProperty::GetRadius()
{
	if (FurContentItem->ProObj.IsValid() && FurContentItem->ResourceType == EResourceType::Hydropower)
	{
		for (auto arr : FurContentItem->ResObjArr)
		{
			if (arr->ResourceType == HardPAK)
			{
				TSharedPtr<FContentItemSpace::FArmyPipeRes> obj = StaticCastSharedPtr<FContentItemSpace::FArmyPipeRes>(arr);
				if (obj.IsValid())
				{
					return obj->Raduis / 10.f;
				}
			}
		}
	}
	return -1;
}

bool FFurnitureProperty::SetLength(float _Length)
{

	if (FurContentItem->ProObj.IsValid() && FurContentItem->ResourceType == EResourceType::Hydropower)
	{
		for (auto arr : FurContentItem->ResObjArr)
		{
			if (arr->ResourceType == HardPAK)
			{
				TSharedPtr<FContentItemSpace::FArmyPipeRes> obj = StaticCastSharedPtr<FContentItemSpace::FArmyPipeRes>(arr);
				if (obj.IsValid())
				{
					obj->Length = _Length * 10.f;
					return true;
				}
			}
		}
	}
	return false;
}

float FFurnitureProperty::GetLength()
{
	if (FurContentItem->ProObj.IsValid() && FurContentItem->ResourceType == EResourceType::Hydropower)
	{
		for (auto arr : FurContentItem->ResObjArr)
		{
			if (arr->ResourceType == HardPAK)
			{
				TSharedPtr<FContentItemSpace::FArmyPipeRes> obj = StaticCastSharedPtr<FContentItemSpace::FArmyPipeRes>(arr);
				if (obj.IsValid())
				{
					return obj->Length / 10.f;
				}
			}
		}
	}
	return -1;
}

bool FFurnitureProperty::SetWidth(float _Width)
{
	if (FurContentItem->ProObj.IsValid() && FurContentItem->ResourceType == EResourceType::Hydropower)
	{
		for (auto arr : FurContentItem->ResObjArr)
		{
			if (arr->ResourceType == HardPAK)
			{
				TSharedPtr<FContentItemSpace::FArmyPipeRes> obj = StaticCastSharedPtr<FContentItemSpace::FArmyPipeRes>(arr);
				if (obj.IsValid())
				{
					obj->Width = _Width * 10.f;
					return true;
				}
			}
		}
	}
	return false;
}

float FFurnitureProperty::GetWidth()
{
	if (FurContentItem->ProObj.IsValid() && FurContentItem->ResourceType == EResourceType::Hydropower)
	{
		for (auto arr : FurContentItem->ResObjArr)
		{
			if (arr->ResourceType == HardPAK)
			{
				TSharedPtr<FContentItemSpace::FArmyPipeRes> obj = StaticCastSharedPtr<FContentItemSpace::FArmyPipeRes>(arr);
				if (obj.IsValid())
				{
					return obj->Width / 10.f;
				}
			}
		}
	}
	return -1;
}

bool FFurnitureProperty::SetHeight(float _Height)
{
	if (FurContentItem->ProObj.IsValid() && FurContentItem->ResourceType == EResourceType::Hydropower)
	{
		for (auto arr : FurContentItem->ResObjArr)
		{
			if (arr->ResourceType == HardPAK)
			{
				TSharedPtr<FContentItemSpace::FArmyPipeRes> obj = StaticCastSharedPtr<FContentItemSpace::FArmyPipeRes>(arr);
				if (obj.IsValid())
				{
					obj->Height = _Height * 10.f;
					return true;
				}
			}
		}
	}
	return false;

}

float FFurnitureProperty::GetHeight()
{
	if (FurContentItem->ProObj.IsValid() && FurContentItem->ResourceType == EResourceType::Hydropower)
	{
		for (auto arr : FurContentItem->ResObjArr)
		{
			if (arr->ResourceType == HardPAK)
			{
				TSharedPtr<FContentItemSpace::FArmyPipeRes> obj = StaticCastSharedPtr<FContentItemSpace::FArmyPipeRes>(arr);
				if (obj.IsValid())
				{
					return obj->Height / 10.f;
				}
			}
		}
	}
	return -1;
}

bool FFurnitureProperty::SetOptionsAltitude(const TArray<float> _Altitude)
{
	if (FurContentItem->ResObjArr.Num()>0 && FurContentItem->ResourceType == EResourceType::Hydropower)
	{
		for (auto arr : FurContentItem->ResObjArr)
		{
			if (arr->ResourceType == HardPAK)
			{
				TSharedPtr<FContentItemSpace::FArmyPipeRes> obj = StaticCastSharedPtr<FContentItemSpace::FArmyPipeRes>(arr);
				if (obj.IsValid())
				{
					TArray<float> TempA;
					for (auto & A : _Altitude)
					{
						TempA.Add(A * 10.f);
					}
					obj->Altitudes  = TempA;
					return true;
				}
			}
		}
	}
	return false;
}

TArray<float> FFurnitureProperty::GetOptionsAltitude()
{
	if (FurContentItem->ResObjArr.Num() > 0 && FurContentItem->ResourceType == EResourceType::Hydropower)
	{
		for (auto arr : FurContentItem->ResObjArr)
		{
			if (arr->ResourceType == HardPAK)
			{
				TSharedPtr<FContentItemSpace::FArmyPipeRes> obj = StaticCastSharedPtr<FContentItemSpace::FArmyPipeRes>(arr);
				if (obj.IsValid() && obj->Altitudes.Num() > 0)
				{
					TArray<float> TempA;
					for (auto & A : obj->Altitudes)
					{
						TempA.Add(A / 10.f);
					}
					return TempA;
				}
			}
		}
	}
	TArray<float> NullArr;
	return NullArr;
}

void FFurnitureProperty::SetAltitude(float _Altitude)
{
	Altitude = _Altitude;
}

float FFurnitureProperty::GetAltitude()
{
	return Altitude;
}

bool FFurnitureProperty::SetComponentPath(FString _ComponentPath)
{
	if (FurContentItem->ResObjArr.Num() > 0 && FurContentItem->ResourceType == EResourceType::Hydropower)
	{
		for (auto arr : FurContentItem->ResObjArr)
		{
			if (arr->ResourceType == ComponentDXF)
			{
				TSharedPtr<FContentItemSpace::FComponentRes> obj = StaticCastSharedPtr<FContentItemSpace::FComponentRes>(arr);
				if (obj.IsValid())
				{
					obj->FilePath = _ComponentPath;
					return true;
				}
			}
		}
	}
	return false;
}

FString FFurnitureProperty::GetComponentPath()
{
	if (FurContentItem->ResObjArr.Num() > 0 && FurContentItem->ResourceType == EResourceType::Hydropower)
	{
		for (auto arr : FurContentItem->ResObjArr)
		{
			if (arr->ResourceType == ComponentDXF)
			{
				TSharedPtr<FContentItemSpace::FComponentRes> obj = StaticCastSharedPtr<FContentItemSpace::FComponentRes>(arr);
				if (obj.IsValid())
				{
					return obj->FilePath;
				}
			}
		}
	}
	return "";
}

bool FFurnitureProperty::SetModelPath(FString _ModelPath)
{
	if (FurContentItem->ResObjArr.Num() > 0 && FurContentItem->ResourceType == EResourceType::Hydropower)
	{
		for (auto arr : FurContentItem->ResObjArr)
		{
			if (arr->ResourceType == HardPAK)
			{
				TSharedPtr<FContentItemSpace::FArmyPipeRes> obj = StaticCastSharedPtr<FContentItemSpace::FArmyPipeRes>(arr);
				if (obj.IsValid())
				{
					obj->FilePath = _ModelPath;
					return true;
				}
			}
		}
	}
	return false;
}

FString FFurnitureProperty::GetModelPath()
{
	if (FurContentItem->ResObjArr.Num() > 0 && FurContentItem->ResourceType == EResourceType::Hydropower)
	{
		for (auto arr : FurContentItem->ResObjArr)
		{
			if (arr->ResourceType == HardPAK)
			{
				TSharedPtr<FContentItemSpace::FArmyPipeRes> obj = StaticCastSharedPtr<FContentItemSpace::FArmyPipeRes>(arr);
				if (obj.IsValid())
				{
					return obj->FilePath;
				}
			}
		}
	}
	return "";
}

bool FFurnitureProperty::SetType(EResourceType _Type)
{
	FurContentItem->ResourceType = _Type;
	return true;
}

EResourceType FFurnitureProperty::GetType()
{
	return FurContentItem->ResourceType;
}

bool FFurnitureProperty::SetBelongClass(int32 _Type)
{
	if (FurContentItem->ResObjArr.Num() > 0 && FurContentItem->ResourceType == EResourceType::Hydropower)
	{
		for (auto arr : FurContentItem->ResObjArr)
		{
			if (arr->ResourceType == ComponentDXF)
			{
				TSharedPtr<FContentItemSpace::FComponentRes> obj = StaticCastSharedPtr<FContentItemSpace::FComponentRes>(arr);
				if (obj.IsValid())
				{
					obj->TypeID = _Type;
					return true;
				}
			}
		}
	}
	return false;
}

int32 FFurnitureProperty::GetBelongClass()
{
	if (FurContentItem->ResObjArr.Num() > 0 && FurContentItem->ResourceType == EResourceType::Hydropower)
	{
		for (auto arr : FurContentItem->ResObjArr)
		{
			if (arr->ResourceType == ComponentDXF)
			{
				TSharedPtr<FContentItemSpace::FComponentRes> obj = StaticCastSharedPtr<FContentItemSpace::FComponentRes>(arr);
				if (obj.IsValid())
				{
					return obj->TypeID;
				}
			}
		}
	}
	return 0;
}

bool FFurnitureProperty::SetObjectType(int32 _Type)
{
	if (FurContentItem->ResObjArr.Num() > 0 && FurContentItem->ResourceType == EResourceType::Hydropower)
	{
		for (auto arr : FurContentItem->ResObjArr)
		{
			if (arr->ResourceType == HardPAK)
			{
				TSharedPtr<FContentItemSpace::FArmyPipeRes> obj = StaticCastSharedPtr<FContentItemSpace::FArmyPipeRes>(arr);
				if (obj.IsValid())
				{
					obj->ObjectType = _Type;
					return true;
				}
			}
		}
	}
	return false;
}

int32 FFurnitureProperty::GetObjectType()
{
	if (FurContentItem->ResObjArr.Num() > 0 && FurContentItem->ResourceType == EResourceType::Hydropower)
	{
		for (auto arr : FurContentItem->ResObjArr)
		{
			if (arr->ResourceType == HardPAK)
			{
				TSharedPtr<FContentItemSpace::FArmyPipeRes> obj = StaticCastSharedPtr<FContentItemSpace::FArmyPipeRes>(arr);
				if (obj.IsValid())
				{
					return obj->ObjectType;
				}
			}
		}
	}
	return -1;
}

bool FFurnitureProperty::SetOptimizeParam(FString _ParamFile)
{
	if (FPaths::FileExists(_ParamFile))
	{
		//读取材质参数
		FString FileContent;
		FString MaterialParameter = "";
		FString LightParameter = "";
		if (FFileHelper::LoadFileToString(FileContent, *_ParamFile))
		{
			TSharedPtr<FJsonObject> JOptimizeParam;
			if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(FileContent), JOptimizeParam))
			{
				MaterialParameter = JOptimizeParam->GetStringField("materialParameter");
				LightParameter = JOptimizeParam->GetStringField("lightParameter");
			}
		}

		//设置材质参数
		if (FurContentItem->ResObjArr.Num() > 0 && FurContentItem->ResourceType == EResourceType::Hydropower)
		{
			for (auto arr : FurContentItem->ResObjArr)
			{
				if (arr->ResourceType == HardPAK)
				{
					TSharedPtr<FContentItemSpace::FArmyPipeRes> obj = StaticCastSharedPtr<FContentItemSpace::FArmyPipeRes>(arr);
					if (obj.IsValid())
					{
						if (!MaterialParameter.IsEmpty())
						{
							obj->MaterialParameter = MaterialParameter;
						}
						if (!LightParameter.IsEmpty())
						{
							obj->LightParameter = LightParameter;
						}
						return true;
					}
				}
			}
		}
	}
	return false;
}

// @刘克祥 获得模型参数 0:材质参数 1：灯光参数
FString FFurnitureProperty::GetOptimizeParam(int32 ParamType)
{
	//设置材质参数
	if (FurContentItem->ResObjArr.Num() > 0 && FurContentItem->ResourceType == EResourceType::Hydropower)
	{
		for (auto arr : FurContentItem->ResObjArr)
		{
			if (arr->ResourceType == HardPAK)
			{
				TSharedPtr<FContentItemSpace::FArmyPipeRes> obj = StaticCastSharedPtr<FContentItemSpace::FArmyPipeRes>(arr);
				if (obj.IsValid())
				{
					if (ParamType == 0)
					{
						return obj->MaterialParameter;
					}
					else if (ParamType == 1)
					{
						return obj->LightParameter;
					}
				}
			}
		}
	}

	return "";
}

void FFurnitureProperty::SetModeType(EModeIdent _type )
{
	FurContentItem->ModeIdent = _type;
}

EModeIdent FFurnitureProperty::GetModelType()
{
	return FurContentItem->ModeIdent;
}

void FFurnitureProperty::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FurContentItem->SerializeToJson(JsonWriter);
	JsonWriter->WriteValue("bIsPakModel", bIsPakModel);
	JsonWriter->WriteValue("Altitude", Altitude);

	// 刘克祥 图例序列化
	JsonWriter->WriteArrayStart("LegendList");
	for (auto It : LegendMap)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("LegendType",It.Key);
		JsonWriter->WriteObjectStart("LegendInfo");
		It.Value->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
	//FurContentItem->SerializeToJson(JsonWriter);
}

void FFurnitureProperty::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	//if (!FurContentItem.IsValid())
	{
		FurContentItem = MakeShareable(new FContentItemSpace::FContentItem());
	}
	FurContentItem->Deserialization(InJsonData);
	bIsPakModel = InJsonData->GetBoolField("bIsPakModel");
	Altitude = InJsonData->GetIntegerField("Altitude");
	/*if (FurContentItem.IsValid())
	{
		FurContentItem->Deserialization(InJsonData);
	}*/
	// 刘克祥 图例反序列化
	const TArray<TSharedPtr<FJsonValue>> LegendList = InJsonData->GetArrayField(TEXT("LegendList"));
	for (auto& It : LegendList)
	{
		TSharedPtr<FJsonObject> LegendObj = It->AsObject();
		TSharedPtr<FArmyFurniture> Legend = MakeShareable(new FArmyFurniture());
		Legend->Deserialization(LegendObj->GetObjectField("LegendInfo"));
		EItemDxfType LegendType = (EItemDxfType)LegendObj->GetIntegerField("LegendType");
		LegendMap.Add(LegendType, Legend);
	}
}

void FFurnitureProperty::CalContentItemID()
{
	int64 a = FDateTime::Now().GetTicks();
	int64 b = FDateTime(1970, 1, 1, 8).GetTicks();
	FurContentItem->ID = FMath::Abs((a - b) / 1000);
}

// 添加构件的图例
void FFurnitureProperty::AddItemLegend(EItemDxfType InType, TSharedPtr<class FArmyFurniture> InFurniture)
{
	if (LegendMap.Contains(InType))
	{
		return;
	}
	else
	{
		LegendMap.Add(InType, InFurniture);
	}
}
// 获得构件的图例
TSharedPtr<class FArmyFurniture>  FFurnitureProperty::GetItemLegend(EItemDxfType InType)
{
	if (LegendMap.Contains(InType))
	{
		return LegendMap.FindRef(InType);
	}
	else
	{
		return nullptr;
	}
}

// 是否存在图例
bool FFurnitureProperty::IsExitItemLegend(EItemDxfType InType)
{
	if (LegendMap.Contains(InType))
	{
		return true;
	}
	else
	{
		return false;
	}
}