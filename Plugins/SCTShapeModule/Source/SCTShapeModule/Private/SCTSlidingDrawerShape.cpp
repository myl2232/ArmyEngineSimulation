#include "SCTSlidingDrawerShape.h"
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

FSlidingDrawerShape::FSlidingDrawerShape()
	:FSCTShape()
{
	SetShapeType(ST_SlidingDrawer);
	SetShapeName(TEXT("新建抽屉"));
	SetShapeCategory(BST_None);
}

FSlidingDrawerShape::~FSlidingDrawerShape()
{

}

void FSlidingDrawerShape::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
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

	//抽类动画设置
	bCanAnimation = InJsonObject->GetBoolField(TEXT("drawerAnimation"));

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

void FSlidingDrawerShape::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//型录类型和Id
	int32 CurrentType = InJsonObject->GetNumberField(TEXT("type"));
	int32 CurrentId = InJsonObject->GetNumberField(TEXT("id"));
	check(CurrentType == ShapeType && CurrentId == ShapeId);

	//更新型录基本信息
	FSCTShape::ParseAttributesFromJson(InJsonObject);

	//抽类动画设置
	bCanAnimation = InJsonObject->GetBoolField(TEXT("drawerAnimation"));

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

void FSlidingDrawerShape::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseShapeFromJson(InJsonObject);
}

void FSlidingDrawerShape::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//基类信息
	FSCTShape::ParseContentFromJson(InJsonObject);
}

void FSlidingDrawerShape::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//保存型录基本信息
	FSCTShape::SaveToJson(JsonWriter);

	//抽类动画设置
	JsonWriter->WriteValue(TEXT("drawerAnimation"), bCanAnimation);

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

void FSlidingDrawerShape::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//型录基本信息
	FSCTShape::SaveAttriToJson(JsonWriter);

	//抽类动画设置
	JsonWriter->WriteValue(TEXT("drawerAnimation"), bCanAnimation);

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

void FSlidingDrawerShape::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//型录基本信息
	FSCTShape::SaveShapeToJson(JsonWriter);
}

void FSlidingDrawerShape::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//基类信息
	FSCTShape::SaveContentToJson(JsonWriter);
}

void FSlidingDrawerShape::CopyTo(FSCTShape* OutShape)
{
	//基类操作
	FSCTShape::CopyTo(OutShape);

	//基础组件类型
	FSlidingDrawerShape* OutBaseShape = StaticCast<FSlidingDrawerShape*>(OutShape);
	//OutBaseShape->BaseShapeType = BaseShapeType;

	//抽类动画设置
	OutBaseShape->bCanAnimation = bCanAnimation;

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

ASCTShapeActor* FSlidingDrawerShape::SpawnShapeActor()
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

void FSlidingDrawerShape::SpawnActorsForSelected(FName InProfileName)
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

void FSlidingDrawerShape::SetCollisionProfileName(FName InProfileName)
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

void FSlidingDrawerShape::GetResourceUrls(TArray<FString> &OutResourceUrls)
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

void FSlidingDrawerShape::GetFileCachePaths(TArray<FString> &OutFileCachePaths)
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

void FSlidingDrawerShape::HiddenDrawerShapeActors(bool bHidden)
{
	for (auto it = BoardShapes.CreateConstIterator(); it; ++it)
	{
		ASCTShapeActor* CurBoardActor = (*it)->GetShapeActor();
		if (CurBoardActor)
		{
			CurBoardActor->SetActorHiddenInGame(bHidden);
		}
	}
	for (auto it = AccessoryShapes.CreateConstIterator(); it; ++it)
	{
		ASCTShapeActor* CurBoardActor = (*it)->GetShapeActor();
		if (CurBoardActor)
		{
			CurBoardActor->SetActorHiddenInGame(bHidden);
		}
	}
	for (auto it = DrawerDoorShapes.CreateConstIterator(); it; ++it)
	{
		(*it)->HiddenDoorGroupActors(bHidden);
	}
}

void FSlidingDrawerShape::ShowDrawer()
{
	for (auto & Ref : DrawerDoorShapes)
	{
		Ref->HiddenDoorGroupActors(false);
	}
}

void FSlidingDrawerShape::HideDrawer()
{
	for (auto & Ref : DrawerDoorShapes)
	{
		Ref->HiddenDoorGroupActors(true);
	}
}

bool FSlidingDrawerShape::SetShapeWidth(float InIntValue)
{
	return SetShapeWidth(FString::Printf(TEXT("%f"), InIntValue));
}

bool FSlidingDrawerShape::SetShapeWidth(const FString &InStrValue)
{
	if (IsValidForWidth(FCString::Atof(*InStrValue)) == false) return false;
	bool bResult = FSCTShape::SetShapeWidth(InStrValue);
	if (!bResult) return false;
	for (auto it : BoardShapes)
	{
		ASCTShapeActor* CurBoardActor = it->GetShapeActor();
		if (CurBoardActor)
		{
			CurBoardActor->UpdateActorPosition();
			CurBoardActor->UpdateActorDimension();
		}
	}
	for (auto it : AccessoryShapes)
	{
		ASCTShapeActor* CurAccessoryActor = it->GetShapeActor();
		if (CurAccessoryActor)
		{
			CurAccessoryActor->UpdateActorPosition();
			CurAccessoryActor->UpdateActorDimension();
		}
	}
	for (auto it : DrawerDoorShapes)
	{
		ASCTShapeActor* CurAccessoryActor = it->GetShapeActor();
		if (!CurAccessoryActor) continue;
		CurAccessoryActor->UpdateActorPosition();
		it->UpdateDoorGroup();
	}
	return true;
}

bool FSlidingDrawerShape::SetShapeDepth(float InIntValue)
{
	return SetShapeDepth(FString::Printf(TEXT("%f"), InIntValue));
}

bool FSlidingDrawerShape::SetShapeDepth(const FString &InStrValue)
{
	if (IsValidForDepth(FCString::Atof(*InStrValue)) == false) return false;
	bool bResult = FSCTShape::SetShapeDepth(InStrValue);  
	if (!bResult) return false;
	for (auto it : BoardShapes)
	{
		ASCTShapeActor* CurBoardActor = it->GetShapeActor();
		if (CurBoardActor)
		{
			CurBoardActor->UpdateActorPosition();
			CurBoardActor->UpdateActorDimension();
		}
	}
	for (auto it : AccessoryShapes)
	{
		ASCTShapeActor* CurAccessoryActor = it->GetShapeActor();
		if (CurAccessoryActor)
		{
			CurAccessoryActor->UpdateActorPosition();
			CurAccessoryActor->UpdateActorDimension();
		}
	}
	for (auto it : DrawerDoorShapes)
	{
		ASCTShapeActor* CurAccessoryActor = it->GetShapeActor();
		if (!CurAccessoryActor) continue;
		CurAccessoryActor->UpdateActorPosition();
		it->UpdateDoorGroup();
	}
	return true;
}

bool FSlidingDrawerShape::SetShapeHeight(float InIntValue)
{
	return SetShapeHeight(FString::Printf(TEXT("%f"), InIntValue));
}

bool FSlidingDrawerShape::SetShapeHeight(const FString &InStrValue)
{
	if (IsValidForHeight(FCString::Atof(*InStrValue)) == false) return false;	
	bool bResult = FSCTShape::SetShapeHeight(InStrValue);
	if (!bResult) return false;
	for (auto it : BoardShapes)
	{
		ASCTShapeActor* CurBoardActor = it->GetShapeActor();
		if (CurBoardActor)
		{
			CurBoardActor->UpdateActorPosition();
			CurBoardActor->UpdateActorDimension();
		}
	}
	for (auto it : AccessoryShapes)
	{
		ASCTShapeActor* CurAccessoryActor = it->GetShapeActor();
		if (CurAccessoryActor)
		{
			CurAccessoryActor->UpdateActorPosition();
			CurAccessoryActor->UpdateActorDimension();
		}
	}
	for (auto it : DrawerDoorShapes)
	{
		ASCTShapeActor* CurAccessoryActor = it->GetShapeActor();
		if (!CurAccessoryActor) continue;
		CurAccessoryActor->UpdateActorPosition();
		it->UpdateDoorGroup();
	}
	return true;
}

bool FSlidingDrawerShape::IsValidForWidth(float InValue)
{
	float WidthMinValue = 0.0;
	float WidthMaxValue = 0.0;
	TSharedPtr<FShapeAttribute> DrawerWidthAttri = GetShapeWidthAttri();
	if (SAT_NumberRange == DrawerWidthAttri->GetAttributeType())
	{
		TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(DrawerWidthAttri);
		WidthMinValue = NumberRangeAttri->GetMinValue();
		WidthMaxValue = NumberRangeAttri->GetMaxValue();
	}
	else
	{
		WidthMinValue = GetShapeWidth();
		WidthMaxValue = GetShapeWidth();
	}
	if (InValue < WidthMinValue || InValue > WidthMaxValue)
	{
		return false;
	}
	return true;
}

bool FSlidingDrawerShape::IsValidForDepth(float InValue)
{
	float DepthMinValue = 0.0;
	float DepthMaxValue = 0.0;
	float DepthCurValue = 0.0;
	TSharedPtr<FShapeAttribute> DrawerDepthAttri = GetShapeDepthAttri();
	if (SAT_NumberRange == DrawerDepthAttri->GetAttributeType())
	{
		TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(DrawerDepthAttri);
		DepthMinValue = NumberRangeAttri->GetMinValue();
		DepthMaxValue = NumberRangeAttri->GetMaxValue();
		DepthCurValue = NumberRangeAttri->GetNumberValue();
	}
	else
	{
		DepthMinValue = GetShapeDepth();
		DepthMaxValue = GetShapeDepth();
	}
	if (InValue < DepthMinValue || InValue > DepthMaxValue)
	{
		return false;
	}
	return true;
}

bool FSlidingDrawerShape::IsValidForHeight(float InValue)
{
	float HeightMinValue = 0.0;
	float HeightMaxValue = 0.0;
	TSharedPtr<FShapeAttribute> DrawerHeightAttri = GetShapeHeightAttri();
	if (SAT_NumberRange == DrawerHeightAttri->GetAttributeType())
	{
		TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(DrawerHeightAttri);
		HeightMinValue = NumberRangeAttri->GetMinValue();
		HeightMaxValue = NumberRangeAttri->GetMaxValue();
	}
	else
	{
		HeightMinValue = GetShapeHeight();
		HeightMaxValue = GetShapeHeight();
	}
	if (InValue < HeightMinValue || InValue > HeightMaxValue)
	{
		return false;
	}
	return true;
}

float FSlidingDrawerShape::GetDrawerDoorTotalDepth()
{
	if (DrawerDoorShapes.Num() > 0)
	{
		return DrawerDoorShapes[0]->GetDoorTotalDepth();
	}
	return 0.0;
}

void FSlidingDrawerShape::AddBaseChildShape(const TSharedPtr<FSCTShape> &InShape)
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

void FSlidingDrawerShape::DeleteBaseChildShape(FSCTShape* InShape)
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

const TArray<TSharedPtr<FBoardShape>>& FSlidingDrawerShape::GetBoardShapes() const
{
	return BoardShapes;
}

const TArray<TSharedPtr<FAccessoryShape>>& FSlidingDrawerShape::GetAccessoryShapes() const
{
	return AccessoryShapes;
}

const TArray<TSharedPtr<FDrawerDoorShape>> & FSlidingDrawerShape::GetDrawerDoorShapes() const
{
	return DrawerDoorShapes;
}

void FSlidingDrawerShape::SetUpDoorExtensionValue(float InValue)
{
	for (auto it : DrawerDoorShapes)
	{
		it->SetUpExtensionValue(InValue);
	}
}

void FSlidingDrawerShape::SetDownDoorExtensionValue(float InValue)
{
	for (auto it : DrawerDoorShapes)
	{
		it->SetDownExtensionValue(InValue);
	}
}

void FSlidingDrawerShape::SetLeftDoorExtensionValue(float InValue)
{
	for (auto it : DrawerDoorShapes)
	{
		it->SetLeftExtensionValue(InValue);
	}
}

void FSlidingDrawerShape::SetRightDoorExtensionValue(float InValue)
{
	for (auto it : DrawerDoorShapes)
	{
		it->SetRightExtensionValue(InValue);
	}
}



FDrawerGroupShape::FDrawerGroupShape()
{
	SetShapeType(ST_DrawerGroup);
	SetShapeName(TEXT("外盖抽屉组"));
}

FDrawerGroupShape::~FDrawerGroupShape()
{

}

void FDrawerGroupShape::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//解析型录基本信息
	FSCTShape::ParseFromJson(InJsonObject);

	//抽屉组四边掩盖方式
	UpCorverType = InJsonObject->GetIntegerField(TEXT("upCoverType"));
	DownCorverType = InJsonObject->GetIntegerField(TEXT("downCoverType"));
	LeftCorverType = InJsonObject->GetIntegerField(TEXT("leftCoverType"));
	RightCorverType = InJsonObject->GetIntegerField(TEXT("rightCoverType"));

	//抽屉组抽面四方向外延值
	UpExtension = InJsonObject->GetNumberField(TEXT("upExtension"));
	DownExtension = InJsonObject->GetNumberField(TEXT("downExtension"));
	LeftExtension = InJsonObject->GetNumberField(TEXT("leftExtension"));
	RightExtension = InJsonObject->GetNumberField(TEXT("rightExtension"));
	//抽屉组上下外延值
	GroupUpExtension = InJsonObject->GetNumberField(TEXT("groupUpExtension"));
	GroupDownExtension = InJsonObject->GetNumberField(TEXT("groupDownExtension"));

	//解析子级型录
	const TArray<TSharedPtr<FJsonValue>>* ChildList = nullptr;
	InJsonObject->TryGetArrayField(TEXT("children"), ChildList);
	if (ChildList != nullptr && (*ChildList).Num() > 0)
	{
		for (int32 i = 0; i < (*ChildList).Num(); ++i)
		{
			TSharedPtr<FJsonObject> ChildObject = (*ChildList)[i]->AsObject();
			int32 ShapeType = ChildObject->GetNumberField(TEXT("type"));
			int64 ShapeId = ChildObject->GetNumberField(TEXT("id"));
			TSharedPtr<FSCTShape> NewShape = FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ShapeType, ShapeId);
			TSharedPtr<FSCTShape> CopyShape = FSCTShapeManager::Get()->CopyShapeToNew(NewShape);
			check(NewShape->GetShapeType() == ST_SlidingDrawer);
			TSharedPtr<FSlidingDrawerShape> NewDrawerShape = StaticCastSharedPtr<FSlidingDrawerShape>(CopyShape);
			if (0 == i)
			{
				SetPrimitiveDrawer(NewDrawerShape);
			}
			else
			{
				AddCopyDrawer(NewDrawerShape);
			}
			NewDrawerShape->ParseAttributesFromJson(ChildObject);
		}
	}
}

void FDrawerGroupShape::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//型录类型和Id
	int32 CurrentType = InJsonObject->GetNumberField(TEXT("type"));
	int32 CurrentId = InJsonObject->GetNumberField(TEXT("id"));
	check(CurrentType == ShapeType && CurrentId == ShapeId);

	//更新型录基本信息
	FSCTShape::ParseAttributesFromJson(InJsonObject);

	//抽屉组四边掩盖方式
	UpCorverType = InJsonObject->GetIntegerField(TEXT("upCoverType"));
	DownCorverType = InJsonObject->GetIntegerField(TEXT("downCoverType"));
	LeftCorverType = InJsonObject->GetIntegerField(TEXT("leftCoverType"));
	RightCorverType = InJsonObject->GetIntegerField(TEXT("rightCoverType"));

	//抽屉组抽面四方向外延值
	UpExtension = InJsonObject->GetNumberField(TEXT("upExtension"));
	DownExtension = InJsonObject->GetNumberField(TEXT("downExtension"));
	LeftExtension = InJsonObject->GetNumberField(TEXT("leftExtension"));
	RightExtension = InJsonObject->GetNumberField(TEXT("rightExtension"));
	//抽屉组上下外延值
	GroupUpExtension = InJsonObject->GetNumberField(TEXT("groupUpExtension"));
	GroupDownExtension = InJsonObject->GetNumberField(TEXT("groupDownExtension"));

	//更新子级型录信息
	const TArray<TSharedPtr<FJsonValue>>* ChildList = nullptr;
	InJsonObject->TryGetArrayField(TEXT("children"), ChildList);
	if (ChildList != nullptr && (*ChildList).Num() > 0)
	{
		for (int32 i = 0; i < (*ChildList).Num(); ++i)
		{
			TSharedPtr<FJsonObject> ChildObject = (*ChildList)[i]->AsObject();
			int32 ShapeType = ChildObject->GetNumberField(TEXT("type"));
			int64 ShapeId = ChildObject->GetNumberField(TEXT("id"));
			TSharedPtr<FSCTShape> NewShape = FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ShapeType, ShapeId);
			TSharedPtr<FSCTShape> CopyShape = FSCTShapeManager::Get()->CopyShapeToNew(NewShape);
			check(ShapeType == ST_SlidingDrawer);
			TSharedPtr<FSlidingDrawerShape> NewDrawerShape = StaticCastSharedPtr<FSlidingDrawerShape>(CopyShape);
			if (0 == i)
			{
				SetPrimitiveDrawer(NewDrawerShape);
			}
			else
			{
				AddCopyDrawer(NewDrawerShape);
			}
			NewDrawerShape->ParseAttributesFromJson(ChildObject);
		}
	}
}

void FDrawerGroupShape::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseShapeFromJson(InJsonObject);
}

void FDrawerGroupShape::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseContentFromJson(InJsonObject);
}

void FDrawerGroupShape::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//1、保存型录基本信息
	FSCTShape::SaveToJson(JsonWriter);

	//抽屉组四边掩盖方式
	JsonWriter->WriteValue(TEXT("upCoverType"), UpCorverType);
	JsonWriter->WriteValue(TEXT("downCoverType"), DownCorverType);
	JsonWriter->WriteValue(TEXT("leftCoverType"), LeftCorverType);
	JsonWriter->WriteValue(TEXT("rightCoverType"), RightCorverType);
	//抽屉组抽面四边外延值
	JsonWriter->WriteValue(TEXT("upExtension"), UpExtension);
	JsonWriter->WriteValue(TEXT("downExtension"), DownExtension);
	JsonWriter->WriteValue(TEXT("leftExtension"), LeftExtension);
	JsonWriter->WriteValue(TEXT("rightExtension"), RightExtension);
	//抽屉组上下外延值
	JsonWriter->WriteValue(TEXT("groupUpExtension"), GroupUpExtension);
	JsonWriter->WriteValue(TEXT("groupDownExtension"), GroupDownExtension);

	//2、保存Children信息
	JsonWriter->WriteArrayStart(TEXT("children"));
	//2.1 SelfShape
	if (PrimitiveDrawerShape.IsValid())
	{
		JsonWriter->WriteObjectStart();
		PrimitiveDrawerShape->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//2.2 CopyShapes
	for (int32 i = 0; i < CopyDrawerShapes.Num(); ++i)
	{
		JsonWriter->WriteObjectStart();
		CopyDrawerShapes[i]->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//End children
	JsonWriter->WriteArrayEnd();

	//3、保存Shapes信息
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

void FDrawerGroupShape::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//1、型录基本信息
	FSCTShape::SaveAttriToJson(JsonWriter);

	//抽屉组四边掩盖方式
	JsonWriter->WriteValue(TEXT("upCoverType"), UpCorverType);
	JsonWriter->WriteValue(TEXT("downCoverType"), DownCorverType);
	JsonWriter->WriteValue(TEXT("leftCoverType"), LeftCorverType);
	JsonWriter->WriteValue(TEXT("rightCoverType"), RightCorverType);
	//抽屉组四边外延值
	JsonWriter->WriteValue(TEXT("upExtension"), UpExtension);
	JsonWriter->WriteValue(TEXT("downExtension"), DownExtension);
	JsonWriter->WriteValue(TEXT("leftExtension"), LeftExtension);
	JsonWriter->WriteValue(TEXT("rightExtension"), RightExtension);
	//抽屉组上下外延值
	JsonWriter->WriteValue(TEXT("groupUpExtension"), GroupUpExtension);
	JsonWriter->WriteValue(TEXT("groupDownExtension"), GroupDownExtension);

	//2、子型录信息
	TArray<TSharedPtr<FSCTShape>> ChildrenShapes = GetChildrenShapes();
	JsonWriter->WriteArrayStart(TEXT("children"));
	//2.1、SelfShape
	if (PrimitiveDrawerShape.IsValid())
	{
		JsonWriter->WriteObjectStart();
		PrimitiveDrawerShape->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//2.2、CopyShapes
	for (auto it : CopyDrawerShapes)
	{
		JsonWriter->WriteObjectStart();
		it->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//End children
	JsonWriter->WriteArrayEnd();
}

void FDrawerGroupShape::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveShapeToJson(JsonWriter);
}

void FDrawerGroupShape::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveContentToJson(JsonWriter);
}

void FDrawerGroupShape::CopyTo(FSCTShape* OutShape)
{
	//基类信息拷贝
	FSCTShape::CopyTo(OutShape);

	//抽屉组信息拷贝
	FDrawerGroupShape* OutDrawerGroupShape = StaticCast<FDrawerGroupShape*>(OutShape);

	//四边掩盖方式:0-未定义，1-全盖，2-半盖，3-内嵌
	OutDrawerGroupShape->UpCorverType = UpCorverType;
	OutDrawerGroupShape->DownCorverType = DownCorverType;
	OutDrawerGroupShape->LeftCorverType = LeftCorverType;
	OutDrawerGroupShape->RightCorverType = RightCorverType;
	//抽屉组四方向外延值
	OutDrawerGroupShape->UpExtension = UpExtension;
	OutDrawerGroupShape->DownExtension = DownExtension;
	OutDrawerGroupShape->LeftExtension = LeftExtension;
	OutDrawerGroupShape->RightExtension = RightExtension;
	//抽屉组上下外延值
	OutDrawerGroupShape->GroupUpExtension = GroupUpExtension;
	OutDrawerGroupShape->GroupDownExtension = GroupDownExtension;

	//关联的子型录
	if (PrimitiveDrawerShape.IsValid())
	{
		FSlidingDrawerShape* NewDrawerShape = new FSlidingDrawerShape;
		PrimitiveDrawerShape->CopyTo(NewDrawerShape);
		OutDrawerGroupShape->SetPrimitiveDrawer(MakeShareable(NewDrawerShape));
	}

	//复制出的子型录集合
	for (int32 i = 0; i < CopyDrawerShapes.Num(); ++i)
	{
		FSlidingDrawerShape* NewDrawerShape = new FSlidingDrawerShape;
		CopyDrawerShapes[i]->CopyTo(NewDrawerShape);
		OutDrawerGroupShape->AddCopyDrawer(MakeShareable(NewDrawerShape));
	}
}

ASCTShapeActor* FDrawerGroupShape::SpawnShapeActor()
{
	//创建内胆自身的Actor
	ASCTShapeActor* NewDrawerGroupActor = FSCTShape::SpawnShapeActor();
	//创建子级型录的Actor
	if (PrimitiveDrawerShape.IsValid())
	{
		ASCTShapeActor* SelfActor = PrimitiveDrawerShape->GetShapeActor();
		if (!SelfActor)
		{
			SelfActor = PrimitiveDrawerShape->SpawnShapeActor();
		}
		SelfActor->AttachToActorOverride(NewDrawerGroupActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	for (int32 i = 0; i < CopyDrawerShapes.Num(); ++i)
	{
		ASCTShapeActor* CopyActor = CopyDrawerShapes[i]->GetShapeActor();
		if (!CopyActor)
		{
			CopyActor = CopyDrawerShapes[i]->SpawnShapeActor();
		}
		CopyActor->AttachToActorOverride(NewDrawerGroupActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	UpdateDrawerGroup();
	return NewDrawerGroupActor;
}

void FDrawerGroupShape::SpawnActorsForSelected(FName InProfileName)
{
	//当前内胆自身的Actor
	ASCTShapeActor* CurGroupActor = FSCTShape::GetShapeActor();
	if (CurGroupActor)
	{
		//创建型录外包框
		ASCTWireframeActor* WireFrameActor = SpawnWireFrameActor();
		WireFrameActor->UpdateActorDimension();
		WireFrameActor->SetActorHiddenInGame(true);
		WireFrameActor->AttachToActor(CurGroupActor, FAttachmentTransformRules::KeepRelativeTransform);
		//创建顶层型录半透明包围盒
		ASCTBoundingBoxActor* BoundingBoxActor = SpawnBoundingBoxActor();
		BoundingBoxActor->SetCollisionProfileName(InProfileName);
		BoundingBoxActor->SetRegionAlpha(0.0f);
		BoundingBoxActor->UpdateActorDimension();
		BoundingBoxActor->AttachToActor(CurGroupActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void FDrawerGroupShape::SetCollisionProfileName(FName InProfileName)
{
	if (PrimitiveDrawerShape.IsValid())
	{
		PrimitiveDrawerShape->SetCollisionProfileName(InProfileName);
	}
	for (auto it : CopyDrawerShapes)
	{
		it->SetCollisionProfileName(InProfileName);
	}
}

void FDrawerGroupShape::GetResourceUrls(TArray<FString> &OutResourceUrls)
{
	if (PrimitiveDrawerShape.IsValid())
	{
		PrimitiveDrawerShape->GetResourceUrls(OutResourceUrls);
	}
	for (auto it : CopyDrawerShapes)
	{
		it->GetResourceUrls(OutResourceUrls);
	}
}

void FDrawerGroupShape::GetFileCachePaths(TArray<FString> &OutFileCachePaths)
{
	if (PrimitiveDrawerShape.IsValid())
	{
		PrimitiveDrawerShape->GetFileCachePaths(OutFileCachePaths);
	}
	for (auto it : CopyDrawerShapes)
	{
		it->GetFileCachePaths(OutFileCachePaths);
	}
}

void FDrawerGroupShape::HiddenDrawerGroupShapeActors(bool bHidden)
{
	if (PrimitiveDrawerShape.IsValid())
	{
		PrimitiveDrawerShape->HiddenDrawerShapeActors(bHidden);
	}
	for (auto it : CopyDrawerShapes)
	{
		it->HiddenDrawerShapeActors(bHidden);
	}
}

bool FDrawerGroupShape::SetShapePosX(float InValue)
{
	bool bResult = true;
	FString StrValue = FString::Printf(TEXT("%f"), InValue);
	if (PositionX->GetAttributeStr() != StrValue)
	{
		bResult = PositionX->SetAttributeValue(StrValue);
		//若Actor已生成，同时更新Actor
		if (bResult && ShapeActor)
		{
			ShapeActor->UpdateActorPosition();
		}
	}
	return bResult;
}

bool FDrawerGroupShape::SetShapePosY(float InValue)
{
	bool bResult = true;
	FString StrValue = FString::Printf(TEXT("%f"), InValue);
	if (PositionY->GetAttributeStr() != StrValue)
	{
		bResult = PositionY->SetAttributeValue(StrValue);
		//若Actor已生成，同时更新Actor
		if (bResult && ShapeActor)
		{
			ShapeActor->UpdateActorPosition();
		}
	}
	return bResult;
}

bool FDrawerGroupShape::SetShapePosZ(float InValue)
{
	bool bResult = true;
	FString StrValue = FString::Printf(TEXT("%f"), InValue);
	if (PositionZ->GetAttributeStr() != StrValue)
	{
		bResult = PositionZ->SetAttributeValue(StrValue);
		//若Actor已生成，同时更新Actor
		if (bResult && ShapeActor)
		{
			ShapeActor->UpdateActorPosition();
		}
	}
	return bResult;
}

bool FDrawerGroupShape::SetShapeWidth(float InValue)
{
	//处理基类尺寸
	FSCTShape::SetShapeWidth(InValue);
	//
	UpdateDrawerGroup();
	return true;
}

bool FDrawerGroupShape::SetShapeDepth(float InValue)
{
	//处理基类尺寸
	FSCTShape::SetShapeDepth(InValue);
	//
	UpdateDrawerGroup();
	return true;
}

bool FDrawerGroupShape::SetShapeHeight(float InValue)
{
	//首先基类尺寸
	bool bRes = FSCTShape::SetShapeHeight(InValue);
	if (!bRes)
	{
		return false;
	}
	bool bRebuild = RebuildDrawerCopies();
	if (!bRebuild)
	{
		return false;
	}
	UpdateDrawerGroup();
	return true;
}

bool FDrawerGroupShape::SetGroupDimensionExtension(float InWidth, float InDepth, float InHeight,
	float InUp, float InDown, float InLeft, float InRight)
{
	bool bResult = false;
	//设置尺寸
	bResult = FSCTShape::SetShapeWidth(InWidth);
	if (!bResult)
	{
		return false;
	}
	bResult = FSCTShape::SetShapeDepth(InDepth);
	if (!bResult)
	{
		return false;
	}
	bResult = FSCTShape::SetShapeHeight(InHeight);
	if (!bResult)
	{
		return false;
	}
	//重新计算抽屉数量
	bResult = RebuildDrawerCopies();
	if (!bResult)
	{
		return false;
	}
	//设置外延值
	UpExtension = InUp;
	DownExtension = InDown;
	LeftExtension = InLeft;
	RightExtension = InRight;
	//更新抽屉组内部子对象的位置和尺寸
	UpdateDrawerGroup();
	return true;
}

float FDrawerGroupShape::GetDrawerDoorTotalDepth()
{
	if (PrimitiveDrawerShape.IsValid())
	{
		return PrimitiveDrawerShape->GetDrawerDoorTotalDepth();
	}
	return 0.0;
}

float FDrawerGroupShape::GetShapeWidthRange(float& MinValue, float& MaxValue)
{
	float CurWidth = GetShapeWidth();
	//原始抽屉尺寸
	TSharedPtr<FShapeAttribute> ShapeAttri = PrimitiveDrawerShape->GetShapeWidthAttri();
	if (SAT_NumberRange == ShapeAttri->GetAttributeType())
	{
		TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(ShapeAttri);
		MinValue = NumberRangeAttri->GetMinValue();
		MaxValue = NumberRangeAttri->GetMaxValue();
	}
	else if (SAT_NumberSelect == ShapeAttri->GetAttributeType())
	{
		TSharedPtr<FNumberSelectAttri> NumberSelectAttri = StaticCastSharedPtr<FNumberSelectAttri>(ShapeAttri);
		MinValue = NumberSelectAttri->GetNumberValue();
		MaxValue = NumberSelectAttri->GetNumberValue();
	}
	else
	{
		check(false);
	}

	return CurWidth;
}

float FDrawerGroupShape::GetShapeDepthRange(float& MinValue, float& MaxValue)
{
	float CurDepth = GetShapeDepth();
	//原始抽屉尺寸
	TSharedPtr<FShapeAttribute> DepthAttri = PrimitiveDrawerShape->GetShapeDepthAttri();
	if (SAT_NumberRange == DepthAttri->GetAttributeType())
	{
		TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(DepthAttri);
		MinValue = NumberRangeAttri->GetMinValue();
		MaxValue = NumberRangeAttri->GetMaxValue();
	}
	else if (SAT_NumberSelect == DepthAttri->GetAttributeType())
	{
		TSharedPtr<FNumberSelectAttri> NumberSelectAttri = StaticCastSharedPtr<FNumberSelectAttri>(DepthAttri);
		MinValue = NumberSelectAttri->GetNumberValue();
		MaxValue = NumberSelectAttri->GetNumberValue();
	}
	else
	{
		MinValue = CurDepth;
		MaxValue = CurDepth;
	}
	return CurDepth;
}

float FDrawerGroupShape::GetShapeHeightRange(float& MinValue, float& MaxValue)
{
	float CurHeight = GetShapeHeight();
	//原始抽出尺寸
	TSharedPtr<FShapeAttribute> ShapeAttri = PrimitiveDrawerShape->GetShapeHeightAttri();
	if (SAT_NumberRange == ShapeAttri->GetAttributeType())
	{
		TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(ShapeAttri);
		MinValue = NumberRangeAttri->GetMinValue();
		MaxValue = NumberRangeAttri->GetMaxValue();
	}
	else if (SAT_NumberSelect == ShapeAttri->GetAttributeType())
	{
		TSharedPtr<FNumberSelectAttri> NumberSelectAttri = StaticCastSharedPtr<FNumberSelectAttri>(ShapeAttri);
		MinValue = NumberSelectAttri->GetNumberValue();
		MaxValue = NumberSelectAttri->GetNumberValue();
	}
	else
	{
		check(false);
	}

	//可复制方向最大/最小值调整
	//MaxValue = 10000.0;
	int32 CopyCount = CopyDrawerShapes.Num();
	MinValue += MinValue * CopyCount;
	MaxValue += MaxValue * CopyCount;

	return CurHeight;
}

bool FDrawerGroupShape::IsValidForWidth(float InValue)
{
	if (!PrimitiveDrawerShape.IsValid())
	{
		return true;
	}
	if (!PrimitiveDrawerShape->IsValidForWidth(InValue))
	{
		return false;
	}
	//TODO: 验证抽面尺寸（外延值）
	return true;
}

bool FDrawerGroupShape::IsValidForDepth(float InValue)
{
	if (!PrimitiveDrawerShape.IsValid())
	{
		return true;
	}
	if (!PrimitiveDrawerShape->IsValidForDepth(InValue))
	{
		return false;
	}
	return true;
}

bool FDrawerGroupShape::IsValidForHeight(float InValue)
{
	if (!PrimitiveDrawerShape.IsValid())
	{
		return true;
	}
	for (int32 i = 1; i < 20; ++i)
	{
		float CurSingleHeight = InValue / i;
		if (PrimitiveDrawerShape->IsValidForHeight(CurSingleHeight))
		{
			return true;
		}
	}
	return false;
}

bool FDrawerGroupShape::RebuildDrawerCopies()
{
	if (!PrimitiveDrawerShape.IsValid())
	{
		return true;
	}

	//1、当前抽屉组的高度
	float CurGroupHeight = GetShapeHeight();

	//2、获取基础组件的高度变化范围和当前值
	TSharedPtr<FShapeAttribute> HeightAttri = PrimitiveDrawerShape->GetShapeHeightAttri();
	TSharedPtr<FNumberRangeAttri> NumberAttri = StaticCastSharedPtr<FNumberRangeAttri>(HeightAttri);
	float MinHeight = NumberAttri->GetMinValue();
	float MaxHeight = NumberAttri->GetMaxValue();
	float BaseHeight = NumberAttri->GetNumberValue();

	//4、根据当前内胆尺寸和组件总体厚度增、删复制组件
	if (CurGroupHeight > MaxHeight * (CopyDrawerShapes.Num() + 1))
	{
		while (MinHeight  * (CopyDrawerShapes.Num() + 1 + 1) <= CurGroupHeight)
		{
			FSlidingDrawerShape* NewDrawerShape = new FSlidingDrawerShape;
			PrimitiveDrawerShape->CopyTo(NewDrawerShape);
			AddCopyDrawer(MakeShareable(NewDrawerShape));
			if (CurGroupHeight <= MaxHeight * (CopyDrawerShapes.Num() + 1))
			{
				break;
			}
		}
	}
	else if (CurGroupHeight < MinHeight * (CopyDrawerShapes.Num() + 1))
	{
		while (MinHeight  * (CopyDrawerShapes.Num() + 1) > CurGroupHeight)
		{
			if (CopyDrawerShapes.Num() == 0)
			{
				return false;
			}
			RemoveCopyDrawerAt(CopyDrawerShapes.Num() - 1);
			if (CurGroupHeight >= MinHeight * (CopyDrawerShapes.Num() + 1))
			{
				break;
			}
		}
	}
	return true;
}

void FDrawerGroupShape::UpdateDrawerGroup()
{
	//1、当前抽屉组的尺寸
	float CurGroupWidth = GetShapeWidth();
	float CurGroupDepth = GetShapeDepth();
	float CurGroupHeight = GetShapeHeight();

	//2、计算单个抽屉的高度
	float SingleHeight = CurGroupHeight / (CopyDrawerShapes.Num() + 1);

	//3、计算单个抽面外延值
	float CurUpExtension = (UpExtension + GroupUpExtension) / (CopyDrawerShapes.Num() + 1);
	float CurDownExtension = (DownExtension + GroupDownExtension) / (CopyDrawerShapes.Num() + 1);
	float CurLeftExtension = LeftExtension;
	float CurRightExtension = RightExtension;

	//4、设置原始抽屉的尺寸、位置和外延值
	float CurDrawerPosZ = -CurDownExtension * CopyDrawerShapes.Num();
	if (PrimitiveDrawerShape.IsValid())
	{
		PrimitiveDrawerShape->SetShapeWidth(CurGroupWidth);
		PrimitiveDrawerShape->SetShapeDepth(CurGroupDepth);
		PrimitiveDrawerShape->SetShapeHeight(SingleHeight);
		PrimitiveDrawerShape->SetShapePosX(0.0);
		PrimitiveDrawerShape->SetShapePosY(0.0);
		PrimitiveDrawerShape->SetShapePosZ(CurDrawerPosZ);
		PrimitiveDrawerShape->SetUpDoorExtensionValue(CurUpExtension);
		PrimitiveDrawerShape->SetDownDoorExtensionValue(CurDownExtension);
		PrimitiveDrawerShape->SetLeftDoorExtensionValue(CurLeftExtension);
		PrimitiveDrawerShape->SetRightDoorExtensionValue(CurRightExtension);
	}

	//5、设置复制抽屉的尺寸、位置和外延值
	for (int32 i = 0; i < CopyDrawerShapes.Num(); ++i)
	{
		CurDrawerPosZ += SingleHeight + CurDownExtension + CurUpExtension;
		CopyDrawerShapes[i]->SetShapeWidth(CurGroupWidth);
		CopyDrawerShapes[i]->SetShapeDepth(CurGroupDepth);
		CopyDrawerShapes[i]->SetShapeHeight(SingleHeight);
		CopyDrawerShapes[i]->SetShapePosX(0.0);
		CopyDrawerShapes[i]->SetShapePosY(0.0);
		CopyDrawerShapes[i]->SetShapePosZ(CurDrawerPosZ);
		CopyDrawerShapes[i]->SetUpDoorExtensionValue(CurUpExtension);
		CopyDrawerShapes[i]->SetDownDoorExtensionValue(CurDownExtension);
		CopyDrawerShapes[i]->SetLeftDoorExtensionValue(CurLeftExtension);
		CopyDrawerShapes[i]->SetRightDoorExtensionValue(CurRightExtension);
	}
}

void FDrawerGroupShape::ShowDrawer()
{
	if(PrimitiveDrawerShape.IsValid())
	{	
		PrimitiveDrawerShape->ShowDrawer();
	}	
	for (int32 i = 0; i < CopyDrawerShapes.Num(); ++i)
	{
		CopyDrawerShapes[i]->ShowDrawer();
	}
}

void FDrawerGroupShape::HideDrawer()
{
	if (PrimitiveDrawerShape.IsValid())
	{
		PrimitiveDrawerShape->HideDrawer();
	}
	for (int32 i = 0; i < CopyDrawerShapes.Num(); ++i)
	{
		CopyDrawerShapes[i]->HideDrawer();
	}
}

void FDrawerGroupShape::SetUpCorverType(int32 InType)
{
	UpCorverType = InType;
}

void FDrawerGroupShape::SetDownCorverType(int32 InType)
{
	DownCorverType = InType;
}

void FDrawerGroupShape::SetLeftCorverType(int32 InType)
{
	LeftCorverType = InType;
}

void FDrawerGroupShape::SetRightCorverType(int32 InType)
{
	RightCorverType = InType;
}

void FDrawerGroupShape::SetDrawerUpExtensionValue(float InValue)
{
	UpExtension = InValue;
	UpdateDrawerGroup();
}

void FDrawerGroupShape::SetDrawerDownExtensionValue(float InValue)
{
	DownExtension = InValue;
	UpdateDrawerGroup();
}

void FDrawerGroupShape::SetDrawerLeftExtensionValue(float InValue)
{
	LeftExtension = InValue;
	UpdateDrawerGroup();
}

void FDrawerGroupShape::SetDrawerRightExtensionValue(float InValue)
{
	RightExtension = InValue;
	UpdateDrawerGroup();
}

void FDrawerGroupShape::SetGroupUpExtensionValue(float InValue)
{
	GroupUpExtension = InValue;
	UpdateDrawerGroup();
}

float FDrawerGroupShape::GetGroupUpExtensionValue()
{
	return GroupUpExtension;
}

void FDrawerGroupShape::SetGroupDownExtensionValue(float InValue)
{
	GroupDownExtension = InValue;
	UpdateDrawerGroup();
}

float FDrawerGroupShape::GetGroupDownExtensionValue()
{
	return GroupDownExtension;
}

void FDrawerGroupShape::SetPrimitiveDrawer(TSharedPtr<FSlidingDrawerShape> InShape)
{
	PrimitiveDrawerShape = InShape;
	PrimitiveDrawerShape->SetParentShape(this);
	AddChildShape(PrimitiveDrawerShape);
	//修改原始抽屉抽屉的位置
	PrimitiveDrawerShape->SetShapePosX(0.0);
	PrimitiveDrawerShape->SetShapePosY(0.0);
	PrimitiveDrawerShape->SetShapePosZ(0.0);
	//根据抽屉的尺寸初始化抽屉组的尺寸
	SetShapeWidth(PrimitiveDrawerShape->GetShapeWidth());
	SetShapeDepth(PrimitiveDrawerShape->GetShapeDepth());
	SetShapeHeight(PrimitiveDrawerShape->GetShapeHeight());
}

TSharedPtr<FSlidingDrawerShape> FDrawerGroupShape::GetPrimitiveDrawer() const
{
	return PrimitiveDrawerShape;
}

TArray<TSharedPtr<FSlidingDrawerShape>> FDrawerGroupShape::GetCopyDrawerShapes() const
{
	return CopyDrawerShapes;
}

void FDrawerGroupShape::AddCopyDrawer(const TSharedPtr<FSlidingDrawerShape> &InShape)
{
	InShape->SetParentShape(this);
	CopyDrawerShapes.Add(InShape);
	AddChildShape(InShape);
	if (ShapeActor)
	{
		//将子型录Actor挂接到内胆的Actor上
		ASCTShapeActor* CopyActor = InShape->GetShapeActor();
		if (!CopyActor)
		{
			CopyActor = InShape->SpawnShapeActor();
		}
		CopyActor->AttachToActorOverride(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void FDrawerGroupShape::RemoveCopyDrawerAt(int32 InIndex)
{
	TSharedPtr<FSCTShape> CurCopyShape = CopyDrawerShapes[InIndex];
	RemoveChildShape(CurCopyShape);
	CopyDrawerShapes.RemoveAt(InIndex);
}

void FDrawerGroupShape::ClearAllCopyDrawers()
{
	for (int32 i = 0; i < CopyDrawerShapes.Num(); ++i)
	{
		RemoveChildShape(CopyDrawerShapes[i]);
	}
	CopyDrawerShapes.Empty();
}

bool FDrawerGroupShape::SetCopyDrawerCount(int32 InCount)
{
	//注意：输入的复制数量包含自身，即InCount = CopyShapes.Num() + 1
	if (CopyDrawerShapes.Num() + 1 == InCount)
	{
		return true;
	}

	//2、获取基础组件的高度变化范围和当前值
	TSharedPtr<FShapeAttribute> HeightAttri = PrimitiveDrawerShape->GetShapeHeightAttri();
	TSharedPtr<FNumberRangeAttri> NumberAttri = StaticCastSharedPtr<FNumberRangeAttri>(HeightAttri);
	float MinHeight = NumberAttri->GetMinValue();
	float MaxHeight = NumberAttri->GetMaxValue();
	float BaseHeight = NumberAttri->GetNumberValue();

	//3、当前内胆尺寸
	float InsideValue = GetShapeHeight();

	//4、计算单个组件的尺寸
	float SingleHeight = InsideValue / (InCount/* + 1*/);

	//5、判断当前复制数量是否有效
	if (MaxHeight < SingleHeight || MinHeight > SingleHeight)
	{
		return false;
	}

	//根据当前内胆尺寸和组件总体厚度增、删复制组件
	int32 CopyCount = CopyDrawerShapes.Num();
	if (CopyCount < InCount - 1)
	{
		for (int32 i = CopyCount; i < InCount - 1; ++i)
		{
			FSlidingDrawerShape* NewDrawerShape = new FSlidingDrawerShape;
			PrimitiveDrawerShape->CopyTo(NewDrawerShape);
			AddCopyDrawer(MakeShareable(NewDrawerShape));
		}
	}
	else if (CopyCount > InCount - 1)
	{
		for (int32 i = CopyCount; i > InCount - 1; --i)
		{
			RemoveCopyDrawerAt(i - 1);
		}
	}

	//更新计算抽屉的尺寸和位置
	UpdateDrawerGroup();

	return true;
}

TArray<int32> FDrawerGroupShape::GetValidCopyDrawerCounts()
{
	TSharedPtr<FShapeAttribute> HeightAttri = PrimitiveDrawerShape->GetShapeHeightAttri();
	TSharedPtr<FNumberRangeAttri> NumberRange = StaticCastSharedPtr<FNumberRangeAttri>(HeightAttri);
	float MinValue = NumberRange->GetMinValue();
	float MaxValue = NumberRange->GetMaxValue();
	float CurValue = NumberRange->GetNumberValue();

	float InsideHeight = GetShapeHeight();
	int32 MinCount = InsideHeight / MaxValue;
	int32 MaxCount = InsideHeight / MinValue;

	TArray<int32> ValidCounts;
	for (int32 i = MinCount; i <= MaxCount; ++i)
	{
		if (MinValue * i <= InsideHeight && InsideHeight <= MaxValue * i)
		{
			ValidCounts.Add(i);
		}
	}
	return MoveTemp(ValidCounts);
}

int32 FDrawerGroupShape::GetCurrentCopyCount()
{
	if (PrimitiveDrawerShape.IsValid())
	{
		return CopyDrawerShapes.Num() + 1;
	}
	return 0;
}

