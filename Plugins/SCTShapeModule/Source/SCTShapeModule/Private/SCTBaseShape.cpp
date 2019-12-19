#include "SCTBaseShape.h"
#include "SCTShapeManager.h"
#include "SCTBoardShape.h"
#include "SCTModelShape.h"
#include "JsonObject.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/EngineTypes.h"
#include "SCTShapeActor.h"
#include "SCTAttribute.h"
#include "SCTShapeBoxActor.h"
#include "Components/MeshComponent.h"
#include "SCTDoorGroup.h"
#include "SCTModelActor.h"
#include "SCTBoardActor.h"

FBaseShape::FBaseShape()
	:FSCTShape()
{
	SetShapeType(ST_Base);
	SetShapeName(TEXT("新基础组件"));
	SetShapeCategory(BST_None);
	// 宽
	StaticCastSharedPtr<FNumberRangeAttri>(Width)->SetMinValue(0.0f);
	StaticCastSharedPtr<FNumberRangeAttri>(Width)->SetMaxValue(3000.0f);

	// 深
	StaticCastSharedPtr<FNumberRangeAttri>(Depth)->SetMinValue(0.0f);
	StaticCastSharedPtr<FNumberRangeAttri>(Depth)->SetMaxValue(3000.0f);

	// 高
	StaticCastSharedPtr<FNumberRangeAttri>(Height)->SetMinValue(0.0f);
	StaticCastSharedPtr<FNumberRangeAttri>(Height)->SetMaxValue(3000.0f);
}

FBaseShape::~FBaseShape()
{

}

void FBaseShape::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//解析型录基本信息
	FSCTShape::ParseFromJson(InJsonObject);
	//修改位置最小值>=0的情况
	TSharedPtr<FNumberRangeAttri> BasePosXAttri = StaticCastSharedPtr<FNumberRangeAttri>(PositionX);
	if (BasePosXAttri->GetMinValue() >= 0.0)
	{
		BasePosXAttri->SetMinValue(-10000.0);
	}
	TSharedPtr<FNumberRangeAttri> BasePosYAttri = StaticCastSharedPtr<FNumberRangeAttri>(PositionY);
	if (BasePosYAttri->GetMinValue() >= 0.0)
	{
		BasePosYAttri->SetMinValue(-10000.0);
	}
	TSharedPtr<FNumberRangeAttri> BasePosZAttri = StaticCastSharedPtr<FNumberRangeAttri>(PositionZ);
	if (BasePosZAttri->GetMinValue() >= 0.0)
	{
		BasePosZAttri->SetMinValue(-10000.0);
	}


	//解析基础组件类型
// 	int32 BaseShapeType = InJsonObject->GetIntegerField(TEXT("subType"));
// 	SetShapeTag(BaseShapeType);

	//解析子级型录
	const TArray<TSharedPtr<FJsonValue>>* ChildList = nullptr;
	InJsonObject->TryGetArrayField(TEXT("children"), ChildList);
	if (ChildList != nullptr && (*ChildList).Num() > 0)
	{
		for (auto& It : *ChildList)
		{
			TSharedPtr<FJsonObject> ChildObject = It->AsObject();
			int32 ShapeType = ChildObject->GetNumberField(TEXT("type"));
			int64 ShapeId = ChildObject->GetNumberField(TEXT("id"));
			TSharedPtr<FSCTShape> NewShape = FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ShapeType, ShapeId);
			TSharedPtr<FSCTShape> CopyShape = FSCTShapeManager::Get()->CopyShapeToNew(NewShape);
			AddBaseChildShape(CopyShape);
			CopyShape->ParseAttributesFromJson(ChildObject);
		}
	}
}

void FBaseShape::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//型录类型和Id
	int32 CurrentType = InJsonObject->GetNumberField(TEXT("type"));
	int32 CurrentId = InJsonObject->GetNumberField(TEXT("id"));
	check(CurrentType == ShapeType && CurrentId == ShapeId);

	//更新型录基本信息
	FSCTShape::ParseAttributesFromJson(InJsonObject);

	//更新子级型录信息
	const TArray<TSharedPtr<FJsonValue>>* ChildList = nullptr;
	InJsonObject->TryGetArrayField(TEXT("children"), ChildList);
	if (ChildList != nullptr && (*ChildList).Num() > 0)
	{
		for (auto& It : *ChildList)
		{
			TSharedPtr<FJsonObject> ChildObject = It->AsObject();
			int32 ShapeType = ChildObject->GetNumberField(TEXT("type"));
			int64 ShapeId = ChildObject->GetNumberField(TEXT("id"));
			TSharedPtr<FSCTShape> NewShape = FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ShapeType, ShapeId);
			TSharedPtr<FSCTShape> CopyShape = FSCTShapeManager::Get()->CopyShapeToNew(NewShape);
			AddBaseChildShape(CopyShape);
			CopyShape->ParseAttributesFromJson(ChildObject);
		}
	}
}

void FBaseShape::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseShapeFromJson(InJsonObject);
}

void FBaseShape::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//基类信息
	FSCTShape::ParseContentFromJson(InJsonObject);
}

void FBaseShape::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//保存型录基本信息
	FSCTShape::SaveToJson(JsonWriter);

	//保存Children信息
	JsonWriter->WriteArrayStart(TEXT("children"));
	for (auto it = ChildrenShapes.CreateConstIterator(); it; ++it)
	{
		JsonWriter->WriteObjectStart();
		(*it)->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//保存Shapes信息
	//统计当前型录所有层级的子型录
	FSCTShapeManager::Get()->CountInheritChildShapes(this);
	JsonWriter->WriteArrayStart(TEXT("shapes"));
	for (int32 i = ST_None; i < ST_Max; ++i)
	{
		TMap<int64, TSharedPtr<FSCTShape>>& CurTypeChildShapes = FSCTShapeManager::Get()->GetChildrenShapesByType(i);
		for (auto it = CurTypeChildShapes.CreateIterator(); it; ++it)
		{
			TSharedPtr<FSCTShape> CurShape = it->Value;
			JsonWriter->WriteObjectStart();
			CurShape->SaveShapeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
	}
	//End Shapes
	JsonWriter->WriteArrayEnd();
	//清空当前型录所有层级的子型录缓存
	FSCTShapeManager::Get()->ClearAllChildrenShapes();
}

void FBaseShape::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//型录基本信息
	FSCTShape::SaveAttriToJson(JsonWriter);

	//子型录信息
	TArray<TSharedPtr<FSCTShape>> ChildrenShapes = GetChildrenShapes();
	JsonWriter->WriteArrayStart(TEXT("children"));
	for (auto it = ChildrenShapes.CreateConstIterator(); it; ++it)
	{
		JsonWriter->WriteObjectStart();
		(*it)->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
}

void FBaseShape::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//型录基本信息
	FSCTShape::SaveShapeToJson(JsonWriter);
}

void FBaseShape::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//基类信息
	FSCTShape::SaveContentToJson(JsonWriter);
}

void FBaseShape::CopyTo(FSCTShape* OutShape)
{
	//基类操作
	FSCTShape::CopyTo(OutShape);

	//基础组件类型
	FBaseShape* OutBaseShape = StaticCast<FBaseShape*>(OutShape);
	//OutBaseShape->BaseShapeType = BaseShapeType;

	//子级型录
	for (int32 i=0; i<BoardShapes.Num(); ++i)
	{
		TSharedPtr<FSCTShape> BoardShape = FSCTShapeManager::Get()->CopyShapeToNew(BoardShapes[i]);
		OutBaseShape->AddBaseChildShape(BoardShape);
	}
	for (int32 i=0; i<AccessoryShapes.Num(); ++i)
	{
		TSharedPtr<FSCTShape> AccessoryShape = FSCTShapeManager::Get()->CopyShapeToNew(AccessoryShapes[i]);
		OutBaseShape->AddBaseChildShape(AccessoryShape);
	}		
	for (int32 i = 0; i < DrawerDoorShapes.Num(); ++i)
	{
		TSharedPtr<FSCTShape> DrawerDoorShape = FSCTShapeManager::Get()->CopyShapeToNew(DrawerDoorShapes[i]);
		OutBaseShape->AddBaseChildShape(DrawerDoorShape);
	}	
}

ASCTShapeActor* FBaseShape::SpawnShapeActor()
{
	//创建基础组件型录
	ASCTShapeActor* NewBaseActor = FSCTShape::SpawnShapeActor();

	//创建组件子型录
	for (auto it = BoardShapes.CreateConstIterator(); it; ++it)
	{
		ASCTShapeActor* NewBoardActor = (*it)->SpawnShapeActor();
		NewBoardActor->AttachToActorOverride(NewBaseActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	for (auto it = AccessoryShapes.CreateConstIterator(); it; ++it)
	{
		ASCTShapeActor* NewAccessoryActor = (*it)->SpawnShapeActor();
		NewAccessoryActor->AttachToActorOverride(NewBaseActor, FAttachmentTransformRules::KeepRelativeTransform);
	}	
	// 放置所有的抽面
	for (auto it = DrawerDoorShapes.CreateConstIterator(); it; ++it)
	{
		ASCTShapeActor* NewDoorActor = (*it)->SpawnShapeActor();
		NewDoorActor->AttachToActorOverride(NewBaseActor, FAttachmentTransformRules::KeepRelativeTransform);
	}	
 	return NewBaseActor;
}

void FBaseShape::SpawnActorsForSelected(FName InProfileName)
{
	//基础组件型录
	ASCTShapeActor* CurBaseActor = GetShapeActor();
	if (!CurBaseActor)
	{
		return;
	}

	//创建组件子型录
	for (auto it = BoardShapes.CreateConstIterator(); it; ++it)
	{
		(*it)->SpawnActorsForSelected(InProfileName);
	}
	for (auto it = AccessoryShapes.CreateConstIterator(); it; ++it)
	{
		(*it)->SpawnActorsForSelected(InProfileName);
	}
	// 放置所有的抽面
	for (auto it = DrawerDoorShapes.CreateConstIterator(); it; ++it)
	{
		(*it)->SpawnActorsForSelected(InProfileName);
	}
}

void FBaseShape::SetCollisionProfileName(FName InProfileName)
{
	for (auto it = BoardShapes.CreateConstIterator(); it; ++it)
	{
		(*it)->SetCollisionProfileName(InProfileName);
	}
	for (auto it = AccessoryShapes.CreateConstIterator(); it; ++it)
	{
		(*it)->SetCollisionProfileName(InProfileName);
	}	
	for (auto it = DrawerDoorShapes.CreateConstIterator(); it; ++it)
	{
		(*it)->SetCollisionProfileName(InProfileName);
	}
}

void FBaseShape::GetResourceUrls(TArray<FString> &OutResourceUrls)
{
	for (auto it = BoardShapes.CreateConstIterator(); it; ++it)
	{
		(*it)->GetResourceUrls(OutResourceUrls);
	}
	for (auto it = AccessoryShapes.CreateConstIterator(); it; ++it)
	{
		(*it)->GetResourceUrls(OutResourceUrls);
	}
	for (auto it = DrawerDoorShapes.CreateConstIterator(); it; ++it)
	{
		(*it)->GetResourceUrls(OutResourceUrls);
	}
}

void FBaseShape::GetFileCachePaths(TArray<FString> &OutFileCachePaths)
{
	for (auto it = BoardShapes.CreateConstIterator(); it; ++it)
	{
		(*it)->GetFileCachePaths(OutFileCachePaths);
	}
	for (auto it = AccessoryShapes.CreateConstIterator(); it; ++it)
	{
		(*it)->GetFileCachePaths(OutFileCachePaths);
	}
	for (auto it = DrawerDoorShapes.CreateConstIterator(); it; ++it)
	{
		(*it)->GetFileCachePaths(OutFileCachePaths);
	}
}

void FBaseShape::HiddenBaseShapeActors(bool bHidden)
{
	for (auto it = BoardShapes.CreateConstIterator(); it; ++it)
	{
		ASCTShapeActor* CurShapeActor = (*it)->GetShapeActor();
		if (CurShapeActor)
		{
			CurShapeActor->SetActorHiddenInGame(bHidden);
		}
	}
	for (auto it = AccessoryShapes.CreateConstIterator(); it; ++it)
	{

		ASCTShapeActor* CurShapeActor = (*it)->GetShapeActor();
		if (CurShapeActor)
		{
			CurShapeActor->SetActorHiddenInGame(bHidden);
		}
	}
	for (auto it = DrawerDoorShapes.CreateConstIterator(); it; ++it)
	{
		(*it)->HiddenDoorGroupActors(bHidden);
	}
}

bool FBaseShape::SetShapeWidth(const FString & InStrValue)
{
	bool bResult = true;
	if (Width->GetAttributeStr() != InStrValue)
	{
		bResult = Width->SetAttributeValue(InStrValue);
		//若Actor已生成，同时更新Actor
		if (bResult)
		{
			UpdateAssociatedActors(1);
			UpdateChildrenDimensions();
		}
	}
	return bResult;
}

bool FBaseShape::SetShapeDepth(const FString & InStrValue)
{
	bool bResult = true;
	if (Depth->GetAttributeStr() != InStrValue)
	{
		bResult = Depth->SetAttributeValue(InStrValue);
		//若Actor已生成，同时更新Actor
		if (bResult)
		{
			UpdateAssociatedActors(1);
			UpdateChildrenDimensions();
		}
	}
	return bResult;
}

bool FBaseShape::SetShapeHeight(const FString & InStrValue)
{
	bool bResult = true;
	if (Height->GetAttributeStr() != InStrValue)
	{
		bResult = Height->SetAttributeValue(InStrValue);
		//若Actor已生成，同时更新Actor
		if (bResult)
		{
			UpdateAssociatedActors(1);
			UpdateChildrenDimensions();
		}
	}
	return bResult;
}

void FBaseShape::UpdateChildrenDimensions()
{
	for (const auto & Ref : ChildrenShapes)
	{
		ASCTShapeActor* CurChildActor = Ref->GetShapeActor();
		if (CurChildActor)
		{
			CurChildActor->UpdateActorDimension();
			CurChildActor->UpdateActorPosition();
		}
	}
}

// void FBaseShape::HiddenAllBoundingBoxActors()
// {
// 	for (auto it = AccessoryShapes.CreateConstIterator(); it; ++it)
// 	{
// 		ASCTBoundingBoxActor* CurShapeActor = (*it)->GetBoundingBoxActor();
// 		CurShapeActor->SetActorHiddenInGame(true);
// 	}
// 	for (auto it = DrawerDoorShapes.CreateConstIterator(); it; ++it)
// 	{
// 		ASCTBoundingBoxActor* CurShapeActor = (*it)->GetBoundingBoxActor();
// 		CurShapeActor->SetActorHiddenInGame(true);
// 	}
// }

// void FBaseShape::SetBaseShapeType(int32 InType)
// {
// 	BaseShapeType = (EBaseShapeType)InType;
// }
// 
// int32 FBaseShape::GetBaseShapeType()
// {
// 	return (int32)BaseShapeType;
// }

void FBaseShape::AddBaseChildShape(const TSharedPtr<FSCTShape> &InShape)
{
	//添加到子对象中，并设置父级型录
	ChildrenShapes.Add(InShape);
	InShape->SetParentShape(this);

	switch (InShape->GetShapeType())
	{
	case ST_Board:
	{
		TSharedPtr<FBoardShape> CurrentBoard = StaticCastSharedPtr<FBoardShape>(InShape);
		BoardShapes.Add(CurrentBoard);
		break;
	}
	case ST_Accessory:
	{
		TSharedPtr<FAccessoryShape> CurrentAccessory = StaticCastSharedPtr<FAccessoryShape>(InShape);
		AccessoryShapes.Add(CurrentAccessory);
		break;
	}	
	case ST_DrawerDoor:
	{
		TSharedPtr<FDrawerDoorShape> TempDrawableDoor = StaticCastSharedPtr<FDrawerDoorShape>(InShape);
		DrawerDoorShapes.Add(TempDrawableDoor);
		break;
	}	
	default:
		check(false);
		break;
	}
}

void FBaseShape::DeleteBaseChildShape(FSCTShape* InShape)
{
	if (!InShape)
	{
		return;
	}

	switch (InShape->GetShapeType())
	{
	case ST_Board:
	{
		//DeleteBoardShape(InShape);
		int32 BoardCount = BoardShapes.Num();
		for (int32 i = 0; i < BoardCount; ++i)
		{
			if (BoardShapes[i].Get() == InShape)
			{
				FSCTShape::RemoveChildShape(BoardShapes[i]);
				BoardShapes.RemoveAt(i);
				break;
			}
		}
		break;
	}
	case ST_Accessory:
	{
		//DeleteAccessoryShape(InShape);
		int32 AccessoryCount = AccessoryShapes.Num();
		for (int32 i = 0; i < AccessoryCount; ++i)
		{
			if (AccessoryShapes[i].Get() == InShape)
			{
				FSCTShape::RemoveChildShape(AccessoryShapes[i]);
				AccessoryShapes.RemoveAt(i);
				break;
			}
		}
		break;
	}		
	case ST_DrawerDoor:
	{
		int32 DoorNumber = DrawerDoorShapes.Num();
		for (int32 i = 0; i < DoorNumber; ++i)
		{
			if (DrawerDoorShapes[i].Get() == InShape)
			{
				FSCTShape::RemoveChildShape(DrawerDoorShapes[i]);
				DrawerDoorShapes.RemoveAt(i);
				break;
			}
		}
		break;
	}
	default:
		check(false);
		break;
	}
}

const TArray<TSharedPtr<FBoardShape>>& FBaseShape::GetBoardShapes() const
{
	return BoardShapes;
}

const TArray<TSharedPtr<FAccessoryShape>>& FBaseShape::GetAccessoryShapes() const
{
	return AccessoryShapes;
}