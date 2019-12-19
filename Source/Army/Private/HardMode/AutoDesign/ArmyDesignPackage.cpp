
#include "ArmyDesignPackage.h"
#include "ArmyModelEntity.h"
#include "ArmyDataTools.h"
#include "ArmyHardModeData.h"
#include "ArmyFreePolygonArea.h"


FArmyDesignPackage::FArmyDesignPackage()
{
	SpaceId = 0;
	LayoutModelArray.Empty();
	OrnamentModelArray.Empty();
	GlobalModelArray.Empty();
	No = FGuid::NewGuid().ToString();
	RoomArea = 0;
}

FArmyDesignPackage::FArmyDesignPackage(FString InNo, FString InRoomName, int32 InSpaceId)
{
	No = InNo;
	RoomName = InRoomName;
	InSpaceId = SpaceId;
	RoomArea = 0;
}

int32 FArmyDesignPackage::GetSpaceId()
{
	return this->SpaceId;
}
void FArmyDesignPackage::SetSpaceId(int32 InSpaceId)
{
	this->SpaceId = InSpaceId;
}


FString FArmyDesignPackage::GetRoomName()
{
	return RoomName;
}
void FArmyDesignPackage::SetRoomName(FString InRoomName)
{
	this->RoomName = InRoomName;
}

FString FArmyDesignPackage::GetNo()
{
	return No;
}
void FArmyDesignPackage::SetNo(FString InNo)
{
	this->No = InNo;
}

float FArmyDesignPackage::GetRoomArea()
{
	return RoomArea;
}
void FArmyDesignPackage::SetRoomArea(float InRoomArea)
{
	this->RoomArea = InRoomArea;
}

// 设置软装模型
void FArmyDesignPackage::AddLayoutModelList(TSharedPtr<class FArmyModelEntity> InLayoutModel)
{
	for (auto It : LayoutModelArray)
	{
		if (It->ItemID == InLayoutModel->ItemID)
		{
			return;
		}

		if (It->ComponentId == InLayoutModel->ComponentId)
		{
			InLayoutModel->IsDefault = false;
		}
	}

	InLayoutModel->ItemType = IT_Layout;
	LayoutModelArray.Add(InLayoutModel);
}

// 设置配饰模型
void FArmyDesignPackage::AddOrnamentModelList(TSharedPtr<class FArmyModelEntity> InOrnamentModel)
{
	for (auto It : OrnamentModelArray)
	{
		if (It->ItemID == InOrnamentModel->ItemID)
		{
			return;
		}
		if (It->ComponentId == InOrnamentModel->ComponentId)
		{
			InOrnamentModel->IsDefault = false;
		}
	}
	InOrnamentModel->ItemType = IT_Ornament;
	OrnamentModelArray.Add(InOrnamentModel);
}

// 设置全局模型
void FArmyDesignPackage::AddGlobalModelList(TSharedPtr<class FArmyModelEntity> InGlobalModel)
{
	GlobalModelArray.Add(InGlobalModel);
}

// 序列化json数据
void FArmyDesignPackage::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	
	// 基本信息
	JsonWriter->WriteValue(TEXT("no"), No);
	JsonWriter->WriteValue(TEXT("roomName"), RoomName);
	JsonWriter->WriteValue("spaceId", SpaceId);
	JsonWriter->WriteValue("areaValue", RoomArea);

	// 所有模型信息
	JsonWriter->WriteArrayStart(TEXT("goods"));
	for (auto It : GetAllModelList(false))
	{
		if (!It.IsValid())
		{
			continue;
		}

		// 壁灯处理
		if (SpaceId== RT_LivingRoom&&It->ComponentId == AI_LampWall && !(It->IsInEditArea))
		{
			continue;
		}

		JsonWriter->WriteObjectStart();
		It->SerializeToJson(JsonWriter,1);
		JsonWriter->WriteObjectEnd();
	}

	JsonWriter->WriteArrayEnd();
	// 硬装信息
	if (HardModeData.IsValid())
	{
		HardModeData->SerializeToJson(JsonWriter);
	}
	else
	{
		JsonWriter->WriteObjectStart(TEXT("params"));
		JsonWriter->WriteObjectEnd();
	}
}
// 反序列化颜值包
void FArmyDesignPackage::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	if (!InJsonData.IsValid())
	{
		return;
	}

	LayoutModelArray.Empty();
	OrnamentModelArray.Empty();

	// 基本信息
	No = InJsonData->GetStringField(TEXT("no"));
	RoomName = InJsonData->GetStringField(TEXT("roomName"));
	SpaceId = InJsonData->GetIntegerField(TEXT("spaceId"));
	double TmpArea = 0.0;
	InJsonData->TryGetNumberField("floorArea", TmpArea);
	RoomArea = TmpArea;

	// 读取模型list
	const TArray<TSharedPtr<FJsonValue>>  GoodListData = InJsonData->GetArrayField(TEXT("goods"));
	TArray<TSharedPtr<FArmyModelEntity>> HardArray;
	for (int i = 0; i < GoodListData.Num(); i++)
	{
		TSharedPtr<FArmyModelEntity> Model = MakeShareable(new FArmyModelEntity());
		Model->Deserialization(GoodListData[i]->AsObject(),1);
		if (!Model->IsDefault)
		{
			continue;
		}

		// 如果是其他空间，处理窗台石和过门石
		if (SpaceId == RT_Other)
		{
			GlobalModelArray.Add(Model);
			continue;
		}

		if (Model->ItemType == IT_Layout)
		{
			// 处理床头柜
			if (Model->ComponentId == AI_BedsideTable_L)
			{
				TSharedPtr<FArmyModelEntity> Model2 = MakeShareable(new FArmyModelEntity());
				Model->CopyTo(Model2);
				Model2->ComponentId = AI_BedsideTable_R;
				LayoutModelArray.Add(Model2);
			}
			// 处理左辅位沙发
			if (Model->ComponentId == AI_SofaAuxiliary_L)
			{
				TSharedPtr<FArmyModelEntity> Model2 = MakeShareable(new FArmyModelEntity());
				Model->CopyTo(Model2);
				Model2->ComponentId = AI_SofaAuxiliary_R;
				LayoutModelArray.Add(Model2);
			}
			LayoutModelArray.Add(Model);
		}
		else if (Model->ItemType == IT_Ornament)
		{
			OrnamentModelArray.Add(Model);
		}
		else if (Model->ItemType == IT_HardMode)
		{
			HardArray.Add(Model);
		}
	}

	// 读取硬装数据
	TSharedPtr<FJsonObject>  HardModeDataJson = InJsonData->GetObjectField(TEXT("params"));
	HardModeData = MakeShareable(new FArmyHardModeData);
	HardModeData->Deserialization(HardModeDataJson, HardArray);
	
}
// 获得需中台布置的模型列表
TArray<TSharedPtr<FArmyModelEntity>> FArmyDesignPackage::GetLayoutModelList()
{
	return LayoutModelArray;
}
// 获得软装配饰类的模型列表
TArray<TSharedPtr<FArmyModelEntity>> FArmyDesignPackage::GetOrnamentModelList()
{
	return OrnamentModelArray;
}

// 获得全局模型
TArray<TSharedPtr<class FArmyModelEntity>> FArmyDesignPackage::GetGlobalModelList()
{
	return GlobalModelArray;
}

// 设置全局模型
void FArmyDesignPackage::SetGlobalModelList(TArray<TSharedPtr<class FArmyModelEntity>> InGlobalModelArray)
{
	GlobalModelArray.Empty();
	GlobalModelArray = InGlobalModelArray;
}
// 获得硬装的模型列表
TArray<TSharedPtr<class FArmyModelEntity>> FArmyDesignPackage::GetHardModelList(bool IsAll)
{
	TArray<TSharedPtr<FArmyModelEntity>> ModelArray;
	if (HardModeData.IsValid())
	{
		if (IsAll)
		{
			ModelArray = HardModeData->GetHardModelList();
		}
		else
		{
			ModelArray = HardModeData->GetHardSimpleModelList();
		}
	}

	return ModelArray;
}

// 获得不同布局方式的模型
TArray<TSharedPtr<class FArmyModelEntity>> FArmyDesignPackage::GetModelArrayByType(ELayoutType InLayoutType)
{
	TArray<TSharedPtr<FArmyModelEntity>> ModelArray;
	for (auto It : GetAllModelList(true))
	{
		if (GetMoldeLayoutType(It) == InLayoutType)
		{
			ModelArray.Add(It);
		}
	}

	return ModelArray;
}

// 获得硬装对象
TSharedPtr<class FArmyHardModeData> FArmyDesignPackage::GetHardModeData()
{
	return HardModeData;
}

// 设置硬装对象
void  FArmyDesignPackage::SetHardModeData(TSharedPtr<class FArmyHardModeData> InHardModeData)
{
	HardModeData = InHardModeData;
}

// 获得全局硬装对象
TSharedPtr<class FArmyHardwareData> FArmyDesignPackage::GetHardwareData()
{
	return HardwareData;
}
// 设置全局硬装对象
void FArmyDesignPackage::SetHardwareData(TSharedPtr<class FArmyHardwareData> InHardwareData)
{
	HardwareData = InHardwareData;
}

FVector FArmyDesignPackage::GetCeilingLampPos(TSharedPtr<class FArmyRoomEntity> RoomEntity)
{
	if (HardModeData.IsValid())
	{
		TSharedPtr<FModellingRoof> ModellingRoof = HardModeData->GetModellingRoof();
		if (ModellingRoof.IsValid())
		{
			TSharedPtr<FArmyRoom> InRoom = RoomEntity->GetRoom();
			FBox MaxRangeBox;
			FVector WorldMaxRangeCenter;
			TArray<FVector> Points = InRoom->GetWorldPoints(true);
			FArmyHardModeData::ExtractMaximumRectangleInPolygonSimply(Points, WorldMaxRangeCenter, MaxRangeBox);
			if (ModellingRoof->SpaceAreaList.Num() == 0 || RoomEntity->GetAiRoomType() == RT_LivingRoom)
			{
				return FVector(WorldMaxRangeCenter.X, WorldMaxRangeCenter.Y, ModellingRoof->ExtrusionHeight);
				//FArmySceneData::WallHeight - 
			}
			else
			{
				TArray<TSharedPtr<class FArmyRoomSpaceArea>> CurrentSpaceList = FArmySceneData::Get()->
					GetRoomAttachedSurfacesWidthRoomID(InRoom->GetUniqueID().ToString());
				for (auto ItSpace : CurrentSpaceList)
				{
					if (ItSpace->SurfaceType == 2)
					{
						FVector MaxRangeCenter;
						FArmyHardModeData::ExtractMaximumRectangleInPolygonSimply(ItSpace->GetVertices()
							, MaxRangeCenter, MaxRangeBox);

						TArray<TSharedPtr<FArmyBaseArea>> EditAreaList = ItSpace->GetEditAreas();
						for (auto ItArea : EditAreaList)
						{  
							if (ItArea->GetType() == OT_FreePolygonArea
								|| ItArea->GetType() == OT_PolygonArea)
							{
								TSharedPtr<FArmyRegularPolygonArea> FreeArea = StaticCastSharedPtr<FArmyRegularPolygonArea>(ItArea);
								TArray<FVector> Vertices = FreeArea->GetOutVertices();
								if (FArmyMath::IsPointInPolygon2D(MaxRangeCenter, Vertices))
									return FVector(WorldMaxRangeCenter.X, WorldMaxRangeCenter.Y,
										FArmySceneData::WallHeight - ItArea->GetExtrusionHeight());
							}
							else//其他统一用包围盒计算
							{
								//FVector min = ItArea->GetBounds().Min.X * ItArea->GetXDir() + ItArea->GetBounds().Min.Y * ItArea->GetYDir() + ItSpace->GetPlaneCenter();
								//FVector max = ItArea->GetBounds().Max.X * ItArea->GetXDir() + ItArea->GetBounds().Max.Y * ItArea->GetYDir() + ItSpace->GetPlaneCenter();

								//FBox AreaBounds(min, max);
								//if (AreaBounds.IsInsideXY(MaxRangeCenter))
								if (ItArea->GetBounds().IsInsideXY(MaxRangeCenter))
								{
									return FVector(WorldMaxRangeCenter.X, WorldMaxRangeCenter.Y,
										FArmySceneData::WallHeight - ItArea->GetExtrusionHeight());
								}
							}
						}

						return FVector(WorldMaxRangeCenter.X, WorldMaxRangeCenter.Y,
							FArmySceneData::WallHeight - ItSpace->GetExtrusionHeight());
					}
				}
			}
		}
	}

	return FVector::ZeroVector;
}

TArray<FVector> FArmyDesignPackage::GetWallLampPos(TSharedPtr<FArmyRoom> InRoom, FVector& OutDirection)
{
	TArray<FVector> OutPoints;
	TArray<TSharedPtr<FModellingWall>> WallList = HardModeData->GetModellingWallList();
	for (auto It : WallList)
	{
		if (It->ModellingWallType == 3 && It->SpaceAreaList.Num() > 0)
		{
			for (auto ItSapce : It->SpaceAreaList)
			{
				if (ItSapce.bHasLampWall)
				{
					OutPoints.Push(ItSapce.LampWallWorldPos);
					OutDirection = ItSapce.LampWallNormal;
				}
			}
			break;
		}
	}

	return OutPoints;
}

// 所有的模型列表
TArray<TSharedPtr<class FArmyModelEntity>> FArmyDesignPackage::GetAllModelList(bool IsAll)
{
	TArray<TSharedPtr<FArmyModelEntity>> ModelArray;;
	TArray<TSharedPtr<FArmyModelEntity>> HardsModels = GetHardModelList(IsAll);
	ModelArray.Append(LayoutModelArray);
	ModelArray.Append(OrnamentModelArray);
	ModelArray.Append(HardsModels);

	// 处理其他空间的过门石和窗台石的信息
	if (SpaceId == RT_Other)
	{
		for (auto It : GlobalModelArray)
		{
			ModelArray.Add(It);
		}
	}
	return ModelArray;
}

// 是否软装模型走配饰规则
bool FArmyDesignPackage::CheckLayoutToOrnament(TSharedPtr<class FArmyModelEntity> InModel)
{
	// 0:硬装  1 : 软装 2：配饰  3：全局硬装  9其他 - 1 无效
	if (InModel->ItemType == IT_Layout)
	{
		int32 CurrentComponentId = InModel->ComponentId;
		if (CurrentComponentId == AI_Closestool \
			|| CurrentComponentId == AI_Sprinkler \
			|| CurrentComponentId == AI_ShowerRoom \
			|| CurrentComponentId == AI_BathroomArk \
			|| CurrentComponentId == AI_BathroomArk_Wall \
			|| CurrentComponentId == AI_WashingMachine \
			|| CurrentComponentId == AI_SideTable\
			|| CurrentComponentId == AI_LeisureChair\
			|| CurrentComponentId == AI_BookChair\
			|| CurrentComponentId == AI_DiningChair\
			|| CurrentComponentId == AI_DressingStool)
		{
			//马桶、花洒、淋浴房、浴室柜、洗衣机，作为配饰来做，边几、休闲椅也是作为配饰来做
			return true;
		}

	}
	
	return false;
}

// 获得模型的布局类型
ELayoutType FArmyDesignPackage::GetMoldeLayoutType(TSharedPtr<class FArmyModelEntity> InModel)
{
	ELayoutType ResultType = ELT_None;
	if (!InModel.IsValid())
	{
		return ResultType;
	}
	// 配饰走配饰布局
	if (InModel->ItemType == IT_Ornament)
	{
		ResultType = ELT_Bim_Ornament;
	}

	// 硬装走硬装布局
	if (InModel->ItemType == IT_HardMode)
	{
		ResultType = ELT_Bim_HardMode;
	}

	// 软装部分走软装，部分走配饰
	if (InModel->ItemType == IT_Layout)
	{
		ResultType = ELT_Mid_Layout;
		int32 CurrentComponentId = InModel->ComponentId;
		if (CurrentComponentId == AI_Closestool \
			|| CurrentComponentId == AI_Sprinkler \
			|| CurrentComponentId == AI_ShowerRoom \
			|| CurrentComponentId == AI_BathroomArk \
			|| CurrentComponentId == AI_BathroomArk_Wall \
			|| CurrentComponentId == AI_WashingMachine \
			|| CurrentComponentId == AI_SideTable\
			|| CurrentComponentId == AI_LeisureChair\
			|| CurrentComponentId == AI_BookChair\
			|| CurrentComponentId == AI_DiningChair\
			|| CurrentComponentId == AI_DressingStool)
		{
			//马桶、花洒、淋浴房、浴室柜、洗衣机，作为配饰来做，边几、休闲椅也是作为配饰来做
			ResultType = ELT_Bim_Layout;
		}
	}

	return ResultType;
}