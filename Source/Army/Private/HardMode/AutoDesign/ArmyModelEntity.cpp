
#include "ArmyModelEntity.h"
#include "ArmyActorItem.h"
#include "ArmyDataTools.h"
#include "FArmyConstructionItemInterface.h"
#include "ArmyHardModeDetail.h"
#include "ArmyConstructionManager.h"
#include "ArmyBaseArea.h"


FArmyModelEntity::FArmyModelEntity()
{
	Location = FVector::ZeroVector;
	Rotation = FRotator::ZeroRotator;
	Scale3D = FVector::OneVector;
	Width = 0;
	Length = 0;
	Height = 0;
	IsMaker = false;
	ItemType = IT_None;
	HardModeType = HMT_None;
	IsDefault = true;
	LayoutType = ELT_None;
	ConstructionItemData = MakeShareable(new FArmyConstructionItemInterface());
	IsInEditArea = false;
}

FArmyModelEntity::FArmyModelEntity(int32 InItemID)
{
	ItemID = InItemID;
	Location = FVector::ZeroVector;
	Rotation = FRotator::ZeroRotator;
	Scale3D = FVector::OneVector;
	Width = 0;
	Length = 0;
	Height = 0;
	IsMaker = false;
	ItemType = IT_None;
	HardModeType = HMT_None;
	IsDefault = true;
	LayoutType = ELT_None;
	ConstructionItemData = MakeShareable(new FArmyConstructionItemInterface());
	IsInEditArea = false;
}
FArmyModelEntity::FArmyModelEntity(int32 InComponentId, int32 InItemID, FVector InLocation, FRotator InRotation, FVector InScale3D, int InWidth, int InLength, int InHeight, bool InIsMaker)
{
	ComponentId = InComponentId;
	ItemID = InItemID;
	Location = InLocation;
	Rotation = InRotation;
	Scale3D = InScale3D;
	Width = InWidth;
	Length = InLength;
	Height = InLength;
	IsMaker = InIsMaker;
	ItemType = IT_None;
	HardModeType = HMT_None;
	IsDefault = true;
	LayoutType = ELT_None;
	ConstructionItemData = MakeShareable(new FArmyConstructionItemInterface());
	IsInEditArea = false;
}

FArmyModelEntity::FArmyModelEntity(TSharedPtr<class FContentItemSpace::FContentItem> InContentItem)
{
	if (InContentItem.IsValid())
	{
		FurContentItem = InContentItem;
		ItemID = InContentItem->ID;
		ComponentId = InContentItem->AiCode;
		Location = FVector::ZeroVector;
		Rotation = FRotator::ZeroRotator;
		Scale3D = FVector::OneVector;
		FVector ModelInfo = FArmyDataTools::GetContextItemSize(FurContentItem);
		Width = ModelInfo.Y;
		Length = ModelInfo.X;
		Height = ModelInfo.Z;
		IsMaker = false;
		ItemType = IT_None;
		HardModeType = HMT_None;
		IsDefault = true;
		LayoutType = ELT_None;

	}
	ConstructionItemData = MakeShareable(new FArmyConstructionItemInterface());
	IsInEditArea = false;
}

FArmyModelEntity::FArmyModelEntity(TSharedPtr<class FArmyActorItem> InActorItem)
{
	if (InActorItem.IsValid())
	{
		FurContentItem = InActorItem->ContentItem;
		Actor = InActorItem->Actor;
		if (FurContentItem.IsValid())
		{
			ItemID = FurContentItem->ID;
			ComponentId = FurContentItem->AiCode;
			Location = InActorItem->Location;
			Rotation = InActorItem->Rotation;
			Scale3D = InActorItem->Scale;
			FVector ModelInfo = FArmyDataTools::GetContextItemSize(FurContentItem);
			Width = ModelInfo.Y;
			Length = ModelInfo.X;
			Height = ModelInfo.Z;
			IsMaker = false;
			ItemType = IT_None;
			HardModeType = HMT_None;
			IsDefault = true;
			LayoutType = ELT_None;
		}
	}
	ConstructionItemData = MakeShareable(new FArmyConstructionItemInterface());
	IsInEditArea = false;
}

// 序列化模型为josn数据 0:中台序列化 1:颜值包序列化 9：所有序列化
void FArmyModelEntity::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter,int32 JsonType)
{
	// 基本信息
	JsonWriter->WriteValue(TEXT("aiCode"), ComponentId);
	JsonWriter->WriteValue(TEXT("productId"), ItemID);
	JsonWriter->WriteValue(TEXT("type"), ItemType);
	JsonWriter->WriteValue(TEXT("hardType"), HardModeType);
	JsonWriter->WriteValue(TEXT("layoutType"), LayoutType);
	if (IsDefault)
	{
		JsonWriter->WriteValue(TEXT("isDefault"), 1);
	}
	else
	{
		JsonWriter->WriteValue(TEXT("isDefault"), 0);
	}
	

	if (FurContentItem.IsValid())
	{
		JsonWriter->WriteValue(TEXT("name"), FurContentItem->Name);
	}
	
	// 如果序列化颜值包数据或所有数据
	if (JsonType == 1 || JsonType == 9)
	{
		if (ConstructionItemData.IsValid() && ConstructionItemData->bHasSetted)
		{
			JsonWriter->WriteObjectStart(TEXT("params"));
			ConstructionItemData->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}

		// 如果序列化颜值包数据
		if (JsonType == 1)
		{
			return;
		}
	}

	// 自动布局信息
	JsonWriter->WriteValue(TEXT("no"), ItemNo);
	JsonWriter->WriteValue(TEXT("location"), Location.ToString());
	JsonWriter->WriteValue(TEXT("rotation"), Rotation.ToString());
	JsonWriter->WriteValue(TEXT("scale"),Scale3D.ToString());
	// 长宽数据交换
	JsonWriter->WriteValue(TEXT("width"), Length);
	JsonWriter->WriteValue(TEXT("length"), Width);
	JsonWriter->WriteValue(TEXT("height"), Height);
	
	if (IsMaker)
	{
		JsonWriter->WriteValue(TEXT("is_maker"), 1);
	}
	else
	{
		JsonWriter->WriteValue(TEXT("is_maker"), 0);
	}

}

// 反序列化模型json数据 0:中台序列化 1:颜值包序列化 9：所有序列化
void FArmyModelEntity::Deserialization(const TSharedPtr<FJsonObject>& InJsonData,int32 JsonType)
{
	// 初始化数据
	Location = FVector::ZeroVector;
	Scale3D = FVector(1, 1, 1);
	Rotation = FRotator::ZeroRotator;

	// 基本信息
	ComponentId  = InJsonData->GetIntegerField(TEXT("aiCode"));
	ItemID = InJsonData->GetIntegerField(TEXT("productId"));
	ItemType = EItemType(InJsonData->GetIntegerField(TEXT("type")));
	HardModeType = EHardModeType(InJsonData->GetIntegerField(TEXT("hardType")));
	//LayoutType = ELayoutType(InJsonData->GetIntegerField(TEXT("layoutType")));

	int32 InIsDefault = InJsonData->GetIntegerField(TEXT("IsDefault"));
	if (InIsDefault==1)
	{
		IsDefault = true;
	}
	else
	{
		IsDefault = false;
	}

	// 如果反序列化颜值包数据或所有数据
	if (JsonType == 1 || JsonType == 9)
	{
		if (ConstructionItemData.IsValid())
		{
			TSharedPtr<FJsonObject> ConstructionDataJson = InJsonData->GetObjectField(TEXT("params"));
			ConstructionItemData->Deserialization(ConstructionDataJson);
		}
		
		if (JsonType == 1)
		{
			return;
		}
	}

	// 布局信息
	ItemNo = InJsonData->GetStringField(TEXT("no"));
	Location.InitFromString(InJsonData->GetStringField(TEXT("location")));
	FVector Rotate = FVector::ZeroVector;
	Rotate.InitFromString(InJsonData->GetStringField(TEXT("rotation")));
	//Rotation
	//Rotation(Rotate.X, Rotate.Y, Rotate.Z);
	Scale3D = FVector(1,1,1);
	Rotate.Y -= 90.f;
	Rotation = FRotator(Rotate.X, Rotate.Y, Rotate.Z);
 

	// 长宽高信息
	Width = InJsonData->GetIntegerField(TEXT("width"));
	Length = InJsonData->GetIntegerField(TEXT("length"));
	Height = InJsonData->GetIntegerField(TEXT("height"));
	int32 InIsMaker = InJsonData->GetIntegerField(TEXT("is_maker"));
	if (InIsMaker == 1)
	{
		IsMaker = true;
	}
	else
	{
		IsMaker = false;
	}
}


void FArmyModelEntity::CopyTo(TSharedPtr<FArmyModelEntity>&Other)
{
	Other->ItemID = ItemID;
	Other->ComponentId = ComponentId;
	Other->Location = Location;
	Other->Rotation = Rotation;
	Other->Scale3D = Scale3D;
	Other->Width = Width;
	Other->Length = Length;
	Other->Height = Height;
	Other->IsMaker = IsMaker;
	Other->FurContentItem = FurContentItem;
	//
	Other->ItemNo = ItemNo;
	Other->ItemType = ItemType;
	Other->HardModeType = HardModeType;
	Other->Actor = Actor;
	Other->IsDefault = IsDefault;
	Other->LayoutType = LayoutType;
	Other->IsInEditArea = IsInEditArea;

	// 深度拷贝施工项数据
	Other->ConstructionItemData = MakeShareable(new FArmyConstructionItemInterface());
	Other->ConstructionItemData->bHasSetted = ConstructionItemData->bHasSetted;
	Other->ConstructionItemData ->CheckedId = ConstructionItemData->CheckedId;
}

// 初始化施工项
/*enum DetailType
{
	Index_None,
	// @梁晓菲 参数说明：0-2墙顶地 3软装 4铺贴类 5波打线 6放样类
	DT_RoughFloor = 0,
	DT_RoughWall = 1,
	DT_RoughRoof = 2,
	DT_Actor = 3,
	DT_Paste = 4,
	DT_Boda = 5,
	DT_Extrusion = 6,
};*/

// 设置铺贴类施工项
bool FArmyModelEntity::SetPasteConstructionItem(TSharedPtr<class FArmyBaseArea> InBaseArea)
{
	if (!InBaseArea.IsValid())
	{
		return false;
	}

	if (!InBaseArea->GetStyle().IsValid())
	{
		return false;
	}

	//这个面有没有下吊
	bool HasHung = false;
	if (InBaseArea->SurfaceType == 2 && InBaseArea->GetExtrusionHeight() != 0)
	{
		HasHung = true;
	}
	ConstructionPatameters Patameters;
	Patameters.SetPaveInfo(InBaseArea->GetStyle(), (EWallType)InBaseArea->SurfaceType, HasHung);
	ConstructionItemData = XRConstructionManager::Get()->GetSavedCheckedData(InBaseArea->GetUniqueID(), Patameters);
	

	return true;
}
// 放样类施工项
bool FArmyModelEntity::SetExtrusionConstructionItem(FString UniqueCodeString)
{
	ConstructionItemData = FArmySceneData::Get()->GetConstructionItemDataByExtrusionUniqueCode(UniqueCodeString);
	return true;
}

// 设置软装类施工项
bool FArmyModelEntity::SetActorConstructionItem(AActor* InActor)
{
	ConstructionItemData = FArmySceneData::Get()->GetConstructionItemDataByActorUniqueID(InActor->GetUniqueID());
	return true;
}
