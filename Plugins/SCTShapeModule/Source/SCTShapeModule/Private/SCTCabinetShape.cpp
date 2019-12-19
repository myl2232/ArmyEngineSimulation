#include "SCTCabinetShape.h"
#include "SCTSpaceShape.h"
#include "SCTBoardShape.h"
#include "SCTShapeActor.h"
#include "SCTShape.h"
#include "SCTInsideShape.h"
#include "SCTShapeBoxActor.h"
#include "JsonValue.h"
#include "JsonObject.h"
#include "SCTShapeManager.h"
#include "SCTAttribute.h"
#include "SCTVeneeredSideBoardGroup.h"

FCabinetShape::FCabinetShape()
{
	SetShapeType(ST_Cabinet);
	SetShapeName(TEXT("新建柜子"));
	
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

FCabinetShape::~FCabinetShape()
{
	
}

void FCabinetShape::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//解析型录基本信息
	FSCTShape::ParseFromJson(InJsonObject);

	//顶层内胆内缩值
	RetractValue = InJsonObject->GetNumberField(TEXT("retract"));
	//活层内缩值
	ActiveRetract = InJsonObject->GetNumberField(TEXT("activeRetravt"));
	//底部空间的高度
	BottomHeight = InJsonObject->GetNumberField(TEXT("bottromHeight"));
	//柜体中所有门组的显隐
	bShowDoorGroup = InJsonObject->GetBoolField(TEXT("showDoorGroup"));

	//解析子级型录(顶层内胆)
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
			switch (StaticCast<EShapeType>(ShapeType))
			{
				case ST_Space:
				{
					TSharedPtr<FSpaceShape> TopSpaceShape = StaticCastSharedPtr<FSpaceShape>(CopyShape);
					SetTopSpaceShape(TopSpaceShape);
					TopSpaceShape->ParseAttributesFromJson(ChildObject);
					//更新顶层空间
					TopSpaceShape->UpdateSpaceShape();
				} break;
				case ST_VeneerdBoardGroup:
				{
					TSharedPtr<FVeneerdSudeBoardGroup> VeneerdSudeBoardGroup = StaticCastSharedPtr<FVeneerdSudeBoardGroup>(CopyShape);
					SetVeneerdSudeBoardGroup(VeneerdSudeBoardGroup);
					VeneerdSudeBoardGroup->ParseAttributesFromJson(ChildObject);					
					VeneerdSudeBoardGroup->UpdateGroup();
				}break;
			default:
				check(false);
				break;
			}			
		}
	} 

	//尺寸标准值
	const TArray<TSharedPtr<FJsonValue>>* WidthObjects = nullptr;
	InJsonObject->TryGetArrayField(TEXT("standardWidths"), WidthObjects);
	if (WidthObjects != nullptr && (*WidthObjects).Num() > 0)
	{
		for (int32 i = 0; i < (*WidthObjects).Num(); ++i)
		{
			float CurValue = (*WidthObjects)[i]->AsNumber();
			StandardWidths.Add(CurValue);
		}
	}
	const TArray<TSharedPtr<FJsonValue>>* DepthObjects = nullptr;
	InJsonObject->TryGetArrayField(TEXT("standardDepths"), DepthObjects);
	if (DepthObjects != nullptr && (*DepthObjects).Num() > 0)
	{
		for (int32 i = 0; i < (*DepthObjects).Num(); ++i)
		{
			float CurValue = (*DepthObjects)[i]->AsNumber();
			StandardDepths.Add(CurValue);
		}
	}
	const TArray<TSharedPtr<FJsonValue>>* HeightObjects = nullptr;
	InJsonObject->TryGetArrayField(TEXT("standardHeights"), HeightObjects);
	if (HeightObjects != nullptr && (*HeightObjects).Num() > 0)
	{
		for (int32 i = 0; i < (*HeightObjects).Num(); ++i)
		{
			float CurValue = (*HeightObjects)[i]->AsNumber();
			StandardHeights.Add(CurValue);
		}
	}

	// 门缝值
	if(InJsonObject->HasField(TEXT("doorGaps")))
	{
		const TSharedPtr<FJsonObject> & DoorGapsRef = InJsonObject->GetObjectField(TEXT("doorGaps"));
		DoorBackGap = DoorGapsRef->GetNumberField(TEXT("backGap"));
		DoorUpAndDownGap = DoorGapsRef->GetNumberField(TEXT("upAndDownGap"));
		DoorLeftAndRightGap = DoorGapsRef->GetNumberField(TEXT("leftAndRightGap"));
	}

}

void FCabinetShape::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//型录类型和Id
	int32 CurrentType = InJsonObject->GetIntegerField(TEXT("type"));
	int32 CurrentId = InJsonObject->GetIntegerField(TEXT("id"));
	check(CurrentType == ShapeType && CurrentId == ShapeId);

	//更新型录基本信息
	FSCTShape::ParseAttributesFromJson(InJsonObject);

	//顶层内胆内缩值
	RetractValue = InJsonObject->GetNumberField(TEXT("retract"));
	//活层内缩值
	ActiveRetract = InJsonObject->GetNumberField(TEXT("activeRetravt"));
	//底部空间的高度
	BottomHeight = InJsonObject->GetNumberField(TEXT("bottromHeight"));
	//柜体中所有门组的显隐
	bShowDoorGroup = InJsonObject->GetBoolField(TEXT("showDoorGroup"));

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
			switch (StaticCast<EShapeType>(ShapeType))
			{
				case ST_Space:
				{
					TSharedPtr<FSpaceShape> TopSpaceShape = StaticCastSharedPtr<FSpaceShape>(CopyShape);
					SetTopSpaceShape(TopSpaceShape);
					TopSpaceShape->ParseAttributesFromJson(ChildObject);
					//更新顶层空间
					TopSpaceShape->UpdateSpaceShape();
				} break;
				case ST_VeneerdBoardGroup:
				{
					TSharedPtr<FVeneerdSudeBoardGroup> VeneerdSudeBoardGroup = StaticCastSharedPtr<FVeneerdSudeBoardGroup>(CopyShape);
					SetVeneerdSudeBoardGroup(VeneerdSudeBoardGroup);
					VeneerdSudeBoardGroup->ParseAttributesFromJson(ChildObject);
					VeneerdSudeBoardGroup->UpdateGroup();
				}break;
				default:
					check(false);
					break;
			}
		}
	}

	//尺寸标准值
	const TArray<TSharedPtr<FJsonValue>>* WidthObjects = nullptr;
	InJsonObject->TryGetArrayField(TEXT("standardWidths"), WidthObjects);
	if (WidthObjects != nullptr && (*WidthObjects).Num() > 0)
	{
		for (int32 i = 0; i < (*WidthObjects).Num(); ++i)
		{
			float CurValue = (*WidthObjects)[i]->AsNumber();
			StandardWidths.Add(CurValue);
		}
	}
	const TArray<TSharedPtr<FJsonValue>>* DepthObjects = nullptr;
	InJsonObject->TryGetArrayField(TEXT("standardDepths"), DepthObjects);
	if (DepthObjects != nullptr && (*DepthObjects).Num() > 0)
	{
		for (int32 i = 0; i < (*DepthObjects).Num(); ++i)
		{
			float CurValue = (*DepthObjects)[i]->AsNumber();
			StandardDepths.Add(CurValue);
		}
	}
	const TArray<TSharedPtr<FJsonValue>>* HeightObjects = nullptr;
	InJsonObject->TryGetArrayField(TEXT("standardHeights"), HeightObjects);
	if (HeightObjects != nullptr && (*HeightObjects).Num() > 0)
	{
		for (int32 i = 0; i < (*HeightObjects).Num(); ++i)
		{
			float CurValue = (*HeightObjects)[i]->AsNumber();
			StandardHeights.Add(CurValue);
		}
	}
	if (InJsonObject->HasField(TEXT("doorGaps")))
	{
		const TSharedPtr<FJsonObject> & DoorGapsRef = InJsonObject->GetObjectField(TEXT("doorGaps"));
		DoorBackGap = DoorGapsRef->GetNumberField(TEXT("backGap"));
		DoorUpAndDownGap = DoorGapsRef->GetNumberField(TEXT("upAndDownGap"));
		DoorLeftAndRightGap = DoorGapsRef->GetNumberField(TEXT("leftAndRightGap"));
	}
}

void FCabinetShape::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseShapeFromJson(InJsonObject);
}

void FCabinetShape::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseContentFromJson(InJsonObject);
}

void FCabinetShape::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//保存型录基本信息
	FSCTShape::SaveToJson(JsonWriter);

	//顶层内胆内缩值
	JsonWriter->WriteValue(TEXT("retract"), RetractValue);
	//活层内缩值
	JsonWriter->WriteValue(TEXT("activeRetravt"), ActiveRetract);
	//底部空间的高度
	JsonWriter->WriteValue(TEXT("bottromHeight"), BottomHeight);
	//柜体中所有门组的显隐
	JsonWriter->WriteValue(TEXT("showDoorGroup"), bShowDoorGroup);

	//尺寸标准值
	JsonWriter->WriteArrayStart(TEXT("standardWidths"));
	for (int32 i = 0; i < StandardWidths.Num(); ++i)
	{
		JsonWriter->WriteValue(StandardWidths[i]);
	}
	JsonWriter->WriteArrayEnd();
	JsonWriter->WriteArrayStart(TEXT("standardDepths"));
	for (int32 i = 0; i < StandardDepths.Num(); ++i)
	{
		JsonWriter->WriteValue(StandardDepths[i]);
	}
	JsonWriter->WriteArrayEnd();
	JsonWriter->WriteArrayStart(TEXT("standardHeights"));
	for (int32 i = 0; i < StandardHeights.Num(); ++i)
	{
		JsonWriter->WriteValue(StandardHeights[i]);
	}
	JsonWriter->WriteArrayEnd();

	// 门缝值
	{
		JsonWriter->WriteObjectStart(TEXT("doorGaps"));
		JsonWriter->WriteValue(TEXT("backGap"), DoorBackGap);
		JsonWriter->WriteValue(TEXT("upAndDownGap"), DoorUpAndDownGap);
		JsonWriter->WriteValue(TEXT("leftAndRightGap"), DoorLeftAndRightGap);
		JsonWriter->WriteObjectEnd();
	}

	//保存Children信息(顶层空间)
	JsonWriter->WriteArrayStart(TEXT("children"));
	JsonWriter->WriteObjectStart();
	TopSpaceShape->SaveAttriToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();
	if (VeneerdSudeBoardGroup.IsValid())
	{
		JsonWriter->WriteObjectStart();
		VeneerdSudeBoardGroup->SaveAttriToJson(JsonWriter);
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
	JsonWriter->WriteArrayEnd();
	//End Shapes
	
	//清空当前型录所有层级的子型录缓存
	FSCTShapeManager::Get()->ClearAllChildrenShapes();	
	
}

void FCabinetShape::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//1、型录基本信息
	FSCTShape::SaveAttriToJson(JsonWriter);

	//顶层内胆内缩值
	JsonWriter->WriteValue(TEXT("retract"), RetractValue);
	//活层内缩值
	JsonWriter->WriteValue(TEXT("activeRetravt"), ActiveRetract);
	//底部空间的高度
	JsonWriter->WriteValue(TEXT("bottromHeight"), BottomHeight);
	//柜体中所有门组的显隐
	JsonWriter->WriteValue(TEXT("showDoorGroup"), bShowDoorGroup);

	//2、子型录信息
	TArray<TSharedPtr<FSCTShape>> ChildrenShapes = GetChildrenShapes();
	JsonWriter->WriteArrayStart(TEXT("children"));
	//TopSpaceShape
	if (TopSpaceShape.IsValid())
	{
		JsonWriter->WriteObjectStart();
		TopSpaceShape->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	if(VeneerdSudeBoardGroup.IsValid())
	{
		JsonWriter->WriteObjectStart();
		VeneerdSudeBoardGroup->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//End children
	JsonWriter->WriteArrayEnd();

	//尺寸标准值
	JsonWriter->WriteArrayStart(TEXT("standardWidths"));
	for (int32 i = 0; i < StandardWidths.Num(); ++i)
	{
		JsonWriter->WriteValue(StandardWidths[i]);
	}
	JsonWriter->WriteArrayEnd();
	JsonWriter->WriteArrayStart(TEXT("standardDepths"));
	for (int32 i = 0; i < StandardDepths.Num(); ++i)
	{
		JsonWriter->WriteValue(StandardDepths[i]);
	}
	JsonWriter->WriteArrayEnd();
	JsonWriter->WriteArrayStart(TEXT("standardHeights"));
	for (int32 i = 0; i < StandardHeights.Num(); ++i)
	{
		JsonWriter->WriteValue(StandardHeights[i]);
	}
	JsonWriter->WriteArrayEnd();

	// 门缝值
	{
		JsonWriter->WriteObjectStart(TEXT("doorGaps"));
		JsonWriter->WriteValue(TEXT("backGap"), DoorBackGap);
		JsonWriter->WriteValue(TEXT("upAndDownGap"), DoorUpAndDownGap);
		JsonWriter->WriteValue(TEXT("leftAndRightGap"), DoorLeftAndRightGap);
		JsonWriter->WriteObjectEnd();
	}

}

void FCabinetShape::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveShapeToJson(JsonWriter);
}

void FCabinetShape::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveContentToJson(JsonWriter);
	//无门缩略图Url
	JsonWriter->WriteValue(TEXT("noDoorThumbnailUrl"), GetNoDoorThumbnailUrl());
	//尺寸标准值
	JsonWriter->WriteArrayStart(TEXT("standardWidths"));
	for (int32 i = 0; i < StandardWidths.Num(); ++i)
	{
		JsonWriter->WriteValue(StandardWidths[i]);
	}
	JsonWriter->WriteArrayEnd();
	JsonWriter->WriteArrayStart(TEXT("standardDepths"));
	for (int32 i = 0; i < StandardDepths.Num(); ++i)
	{
		JsonWriter->WriteValue(StandardDepths[i]);
	}
	JsonWriter->WriteArrayEnd();
	JsonWriter->WriteArrayStart(TEXT("standardHeights"));
	for (int32 i = 0; i < StandardHeights.Num(); ++i)
	{
		JsonWriter->WriteValue(StandardHeights[i]);
	}
	JsonWriter->WriteArrayEnd();
}

void FCabinetShape::CopyTo(FSCTShape* OutShape)
{
	//基类信息拷贝
	FSCTShape::CopyTo(OutShape);

	//柜体信息拷贝
	FCabinetShape* OutCabinetShape = StaticCast<FCabinetShape*>(OutShape);

	//顶层内胆内缩值
	OutCabinetShape->RetractValue = RetractValue;
	OutCabinetShape->ActiveRetract = ActiveRetract;
	//底部空间的高度
	OutCabinetShape->BottomHeight = BottomHeight;
	//柜体中所有门组的显隐
	OutCabinetShape->bShowDoorGroup = bShowDoorGroup;

	//柜体缩略图（隐藏门板）URL
	OutCabinetShape->NoDoorThumbnailUrl = NoDoorThumbnailUrl;

	//柜体尺寸标准值
	OutCabinetShape->StandardWidths = StandardWidths;
	OutCabinetShape->StandardDepths = StandardDepths;
	OutCabinetShape->StandardHeights = StandardHeights;

	// 门缝值
	OutCabinetShape->DoorBackGap = DoorBackGap;
	OutCabinetShape->DoorUpAndDownGap = DoorUpAndDownGap;
	OutCabinetShape->DoorLeftAndRightGap = DoorLeftAndRightGap;

	//空间Actor的ProfileName
	OutCabinetShape->ShapeSpaceProfileName = ShapeSpaceProfileName;
	OutCabinetShape->BoundSpaceProfileName = ShapeSpaceProfileName;

	//顶层空间
	FSpaceShape* NewTopSpaceShape = new FSpaceShape;
	OutCabinetShape->SetTopSpaceShape(MakeShareable(NewTopSpaceShape));
	TopSpaceShape->CopyTo(NewTopSpaceShape);
	OutCabinetShape->GetTopSpaceShape()->UpdateSpaceShape();

	if (VeneerdSudeBoardGroup.IsValid())
	{
		TSharedPtr<FVeneerdSudeBoardGroup> Group = MakeShareable(new FVeneerdSudeBoardGroup);
		VeneerdSudeBoardGroup->CopyTo(Group.Get());
		OutCabinetShape->SetVeneerdSudeBoardGroup(Group);
		Group->UpdateGroup();
	}	

}

ASCTShapeActor* FCabinetShape::SpawnShapeActor()
{
	//创建单元柜型录Actor
	ASCTShapeActor* NewCabinetActor = FSCTShape::SpawnShapeActor();
	//创建单元柜顶层内胆型录Actor
	if (TopSpaceShape.IsValid())
	{
		ASCTShapeActor* TopInsideActor = TopSpaceShape->SpawnShapeActor();
		TopInsideActor->AttachToActorOverride(NewCabinetActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	if (VeneerdSudeBoardGroup.IsValid())
	{
		ASCTShapeActor* Actor = VeneerdSudeBoardGroup->SpawnShapeActor();
		Actor->AttachToActorOverride(NewCabinetActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	return NewCabinetActor;
}

void FCabinetShape::SpawnActorsForSelected(FName InSelectProfileName, FName InHoverProfileName)
{
	ShapeSpaceProfileName = InSelectProfileName;
	BoundSpaceProfileName = InHoverProfileName;
	if (TopSpaceShape.IsValid())
	{
		TopSpaceShape->SpawnActorsForSelected(InSelectProfileName, InHoverProfileName);
	}
	if (VeneerdSudeBoardGroup.IsValid())
	{
		VeneerdSudeBoardGroup->SpawnActorsForSelected(InSelectProfileName);
	}
}

FName FCabinetShape::GetShapeSpaceProfileName()
{
	return ShapeSpaceProfileName;
}

FName FCabinetShape::GetBoundSpaceProfileName()
{
	return BoundSpaceProfileName;
}

void FCabinetShape::SetCollisionProfileName(FName InProfileName)
{
	if (TopSpaceShape.IsValid())
	{
		TopSpaceShape->SetCollisionProfileName(InProfileName);
	}
	if (VeneerdSudeBoardGroup.IsValid())
	{
		VeneerdSudeBoardGroup->SetCollisionProfileName(InProfileName);
	}
}

void FCabinetShape::GetResourceUrls(TArray<FString> &OutResourceUrls)
{
	if (TopSpaceShape.IsValid())
	{
		TopSpaceShape->GetResourceUrls(OutResourceUrls);
	}
	if (VeneerdSudeBoardGroup.IsValid())
	{
		VeneerdSudeBoardGroup->GetResourceUrls(OutResourceUrls);
	}
}

void FCabinetShape::GetFileCachePaths(TArray<FString> &OutFileCachePaths)
{
	if (TopSpaceShape.IsValid())
	{
		TopSpaceShape->GetFileCachePaths(OutFileCachePaths);
	}
	if (VeneerdSudeBoardGroup.IsValid())
	{
		VeneerdSudeBoardGroup->GetFileCachePaths(OutFileCachePaths);
	}
}

bool FCabinetShape::SetShapeWidth(float InValue)
{
	bool bResult = false;
	const float OldValue = GetShapeWidth();
	do
	{	
		if (FMath::IsNearlyEqual(OldValue, InValue)) { bResult = true; break; }
		bResult = FSCTShape::SetShapeWidth(InValue);
		if (!bResult) break;
		if (TopSpaceShape.IsValid() == false) { bResult = true; break; }
		bResult = TopSpaceShape->IsValidForWidth(InValue);
		if (!bResult) break;
		bResult = TopSpaceShape->SetShapeWidth(InValue);
		if (!bResult) break;
		if (VeneerdSudeBoardGroup.IsValid())
		{
			VeneerdSudeBoardGroup->SetShapeWidth(InValue);
			VeneerdSudeBoardGroup->UpdateGroup();
		}

	} while (false);	
	if (!bResult)
	{
		SetShapeWidth(OldValue);	
	}
	return bResult;
}

bool FCabinetShape::SetShapeDepth(float InValue)
{
	bool bResult = FSCTShape::SetShapeDepth(InValue);
	if (TopSpaceShape.IsValid())
	{
		bResult = TopSpaceShape->IsValidForDepth(InValue);
		if (bResult)
		{
			bResult = TopSpaceShape->SetShapeDepth(InValue);
			if (VeneerdSudeBoardGroup.IsValid())
			{
				VeneerdSudeBoardGroup->SetShapeDepth(InValue);
				VeneerdSudeBoardGroup->UpdateGroup();
			}
		}
	}
	return bResult;
}

bool FCabinetShape::SetShapeHeight(float InValue)
{
	bool bResult = FSCTShape::SetShapeHeight(InValue);;
	if (TopSpaceShape.IsValid())
	{
		bResult = TopSpaceShape->IsValidForHeight(InValue);
		if (bResult)
		{
			bResult = TopSpaceShape->SetShapeHeight(InValue);
			if (VeneerdSudeBoardGroup.IsValid())
			{
				VeneerdSudeBoardGroup->SetShapeHeight(InValue);
				VeneerdSudeBoardGroup->UpdateGroup();
			}
		}
	}
	return bResult;
}

float FCabinetShape::GetShapeWidthRange(float& OutMinValue, float& OutMaxValue)
{
	if (TopSpaceShape.IsValid())
	{
		return TopSpaceShape->GetShapeWidthRange(OutMinValue, OutMaxValue);
	}
	return GetShapeWidth();
}

float FCabinetShape::GetShapeDepthRange(float& OutMinValue, float& OutMaxValue)
{
	if (TopSpaceShape.IsValid())
	{
		return TopSpaceShape->GetShapeDepthRange(OutMinValue, OutMaxValue);
	}
	return GetShapeDepth();
}

float FCabinetShape::GetShapeHeightRange(float& OutMinValue, float& OutMaxValue)
{
	if (TopSpaceShape.IsValid())
	{
		return TopSpaceShape->GetShapeHeightRange(OutMinValue, OutMaxValue);
	}
	return GetShapeHeight();
}

void FCabinetShape::SetTopSpaceShape(const TSharedPtr<FSpaceShape> &InShape)
{
	//首先移除原有的顶层空间
	if (TopSpaceShape.IsValid())
	{
		RemoveChildShape(TopSpaceShape);
		TopSpaceShape = nullptr;
	}
	TopSpaceShape = InShape;
	AddChildShape(TopSpaceShape);
	TopSpaceShape->SetParentShape(this);
	TopSpaceShape->SetCabinetShapeData(this);
	//设置空间基本信息
	TopSpaceShape->SetShapeName(TEXT("顶层空间"));
	TopSpaceShape->SetSpaceSubType(SST_FrameSpace); //框体空间
	//设置顶层空间的尺寸
	TopSpaceShape->SetShapeWidth(GetShapeWidth());
	TopSpaceShape->SetShapeDepth(GetShapeDepth());
	TopSpaceShape->SetShapeHeight(GetShapeHeight());
}

const TSharedPtr<FSpaceShape>& FCabinetShape::GetTopSpaceShape() const
{
	return TopSpaceShape;
}

void FCabinetShape::SetVeneerdSudeBoardGroup(const TSharedPtr<FVeneerdSudeBoardGroup>& InShape)
{
	//首先移除原有的顶层空间
	if (VeneerdSudeBoardGroup.IsValid())
	{
		RemoveChildShape(VeneerdSudeBoardGroup);
		VeneerdSudeBoardGroup = nullptr;
	}
	VeneerdSudeBoardGroup = InShape;
	AddChildShape(VeneerdSudeBoardGroup);
	VeneerdSudeBoardGroup->SetParentShape(this);

	//设置见光板组
	VeneerdSudeBoardGroup->SetShapeName(TEXT("见光板组"));
	VeneerdSudeBoardGroup->SetShapePosX(0.0f);
	VeneerdSudeBoardGroup->SetShapePosY(0.0f);
	VeneerdSudeBoardGroup->SetShapePosZ(0.0f);
	VeneerdSudeBoardGroup->SetShapeWidth(GetShapeWidth());
	VeneerdSudeBoardGroup->SetShapeDepth(GetShapeDepth());
	VeneerdSudeBoardGroup->SetShapeHeight(GetShapeHeight());
}

bool FCabinetShape::SetSpaceShapeWidth(FSpaceShape* InSpace, float InValue)
{
	ESpaceDirectType CurDirect = InSpace->GetDirectType();
	float ScaleValue = InSpace->GetSpaceModifyScale();

	if (SDT_XDirect != CurDirect)
	{
		return false;
	}
	if (ScaleValue < 0.0)
	{
		return false;
	}

	//父级空间
	FSpaceShape* ParentSpace = InSpace->GetParentSpaceShape();
	float ParentWidth = ParentSpace->GetShapeWidth();
	//当前内胆的索引值
	TArray<TSharedPtr<FSpaceShape>>& ChildSpaceShapes = ParentSpace->GetChildSpaceShapes();
	int32 CurIndex = ParentSpace->GetChildSpaceShapeIndex(InSpace);

	//统计父级空间的所有子空间的修改比例值
	TArray<float> ChildScaleValues;
	float TotalFixed = 0.0;
	float TotalScaled = 0.0;
	float TotalBoard = 0.0;
	for (int32 i=0; i<ChildSpaceShapes.Num(); ++i)
	{
		TSharedPtr<FBoardShape> SplitBoard = ChildSpaceShapes[i]->GetSplitBoard();
		if (SplitBoard.IsValid())
		{
			TotalBoard += SplitBoard->GetShapeHeight();
			ChildScaleValues.Add(SplitBoard->GetShapeHeight());
		}
		else
		{
			if (i == CurIndex)
			{
				TotalFixed += InValue;
				ChildScaleValues.Add(InValue);
			}
			else
			{
				float CurScaleValue = ChildSpaceShapes[i]->GetSpaceModifyScale();
				if (CurScaleValue < 0.0)
				{
					TotalScaled += CurScaleValue;
				}
				else
				{
					TotalFixed += CurScaleValue;
				}
				ChildScaleValues.Add(CurScaleValue);
			}
		}
	}

	//判断输入分割数据是否有误
	if ((TotalFixed + TotalBoard > ParentWidth) ||
		(-TotalScaled > KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed + TotalBoard - ParentWidth) < KINDA_SMALL_NUMBER) ||
		(-TotalScaled < KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed + TotalBoard - ParentWidth) > KINDA_SMALL_NUMBER))
	{
		return false;
	}

	//计算比例空间的单位尺寸
	float ScaleSize = 0.0;
	if (TotalScaled < 0.0)
	{
		ScaleSize = (ParentWidth - TotalFixed - TotalBoard) / TotalScaled;
	}

	//更新所有子空间的尺寸和位置
	float CurrentPosX = 0.0;
	for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
	{
		float CurWidth = ChildScaleValues[i];
		if (CurWidth < 0.0)
		{
			CurWidth = CurWidth * ScaleSize;
		}

		ChildSpaceShapes[i]->SetShapeWidth(CurWidth);
		ChildSpaceShapes[i]->SetShapePosX(CurrentPosX);
		CurrentPosX += CurWidth;
	}	
	return true;
}

bool FCabinetShape::SetSpaceShapeDepth(FSpaceShape* InSpace, float InValue)
{
	ESpaceDirectType CurDirect = InSpace->GetDirectType();
	float ScaleValue = InSpace->GetSpaceModifyScale();

	if (SDT_YDirect != CurDirect)
	{
		return false;
	}
	if (ScaleValue < 0.0)
	{
		return false;
	}

	float OldWidth = InSpace->GetShapeWidth();

	//父级空间
	FSpaceShape* ParentSpace = InSpace->GetParentSpaceShape();
	float ParentDepth = ParentSpace->GetShapeDepth();
	//当前内胆的索引值
	TArray<TSharedPtr<FSpaceShape>>& ChildSpaceShapes = ParentSpace->GetChildSpaceShapes();
	int32 CurIndex = ParentSpace->GetChildSpaceShapeIndex(InSpace);

	//统计父级空间的所有子空间的修改比例值
	TArray<float> ChildScaleValues;
	float TotalFixed = 0.0;
	float TotalScaled = 0.0;
	float TotalBoard = 0.0;
	for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
	{
		TSharedPtr<FBoardShape> SplitBoard = ChildSpaceShapes[i]->GetSplitBoard();
		if (SplitBoard.IsValid())
		{
			TotalBoard += SplitBoard->GetShapeHeight();
			ChildScaleValues.Add(SplitBoard->GetShapeHeight());
		}
		else
		{
			if (i == CurIndex)
			{
				TotalFixed += InValue;
				ChildScaleValues.Add(InValue);
			}
			else
			{
				float CurScaleValue = ChildSpaceShapes[i]->GetSpaceModifyScale();
				if (CurScaleValue < 0.0)
				{
					TotalScaled += CurScaleValue;
				}
				else
				{
					TotalFixed += CurScaleValue;
				}
				ChildScaleValues.Add(CurScaleValue);
			}
		}
	}

	//判断输入分割数据是否有误
	if ((TotalFixed + TotalBoard > ParentDepth) ||
		(-TotalScaled > KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed + TotalBoard - ParentDepth) < KINDA_SMALL_NUMBER) ||
		(-TotalScaled < KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed + TotalBoard - ParentDepth) > KINDA_SMALL_NUMBER))
	{
		return false;
	}

	//计算比例空间的单位尺寸
	float ScaleSize = 0.0;
	if (TotalScaled < 0)
	{
		ScaleSize = (ParentDepth - TotalFixed - TotalBoard) / TotalScaled;
	}

	//更新所有子空间的尺寸和位置
	float CurrentPosY = 0.0;
	for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
	{
		float CurDepth = ChildScaleValues[i];
		if (CurDepth < 0.0)
		{
			CurDepth = CurDepth * ScaleSize;
		}

		ChildSpaceShapes[i]->SetShapeWidth(CurDepth);
		ChildSpaceShapes[i]->SetShapePosX(CurrentPosY);
		CurrentPosY += CurDepth;
	}	
	return true;
}

bool FCabinetShape::SetSpaceShapeHeight(FSpaceShape* InSpace, float InValue)
{
	ESpaceDirectType CurDirect = InSpace->GetDirectType();
	float ScaleValue = InSpace->GetSpaceModifyScale();

	if (SDT_ZDirect != CurDirect)
	{
		return false;
	}
	if (ScaleValue < 0.0)
	{
		return false;
	}

	//父级空间
	FSpaceShape* ParentSpace = InSpace->GetParentSpaceShape();
	float ParentHeight = ParentSpace->GetShapeHeight();
	//当前内胆的索引值
	TArray<TSharedPtr<FSpaceShape>>& ChildSpaceShapes = ParentSpace->GetChildSpaceShapes();
	int32 CurIndex = ParentSpace->GetChildSpaceShapeIndex(InSpace);

	//统计父级空间的所有子空间的修改比例值
	TArray<float> ChildScaleValues;
	float TotalFixed = 0.0;
	float TotalScaled = 0.0;
	float TotalBoard = 0.0;
	for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
	{		
		TSharedPtr<FBoardShape> SplitBoard = ChildSpaceShapes[i]->GetSplitBoard();
		if (SplitBoard.IsValid())
		{
			TotalBoard += SplitBoard->GetShapeHeight();
			ChildScaleValues.Add(SplitBoard->GetShapeHeight());
		}
		else
		{
			if (i == CurIndex)
			{
				TotalFixed += InValue;
				ChildScaleValues.Add(InValue);
			}
			else
			{
				float CurScaleValue = ChildSpaceShapes[i]->GetSpaceModifyScale();
				if (CurScaleValue < 0.0)
				{
					TotalScaled += CurScaleValue;
				}
				else
				{
					TotalFixed += CurScaleValue;
				}
				ChildScaleValues.Add(CurScaleValue);
			}
		}
	}

	//判断输入分割数据是否有误
	if ((TotalFixed + TotalBoard > ParentHeight) ||
		(-TotalScaled > KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed + TotalBoard - ParentHeight) < KINDA_SMALL_NUMBER) ||
		(-TotalScaled < KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed + TotalBoard - ParentHeight) > KINDA_SMALL_NUMBER))
	{
		return false;
	}

	//计算比例空间的单位尺寸
	float ScaleSize = 0.0;
	if (TotalScaled < 0.0)
	{
		ScaleSize = (ParentHeight - TotalFixed - TotalBoard) / TotalScaled;
	}

	//更新所有子空间的尺寸和位置
	float CurrentPosZ = 0.0;
	for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
	{
		float CurHeight = ChildScaleValues[i];
		if (CurHeight < 0.0)
		{
			CurHeight = CurHeight * ScaleSize;
		}

		ChildSpaceShapes[i]->SetShapeHeight(CurHeight);
		ChildSpaceShapes[i]->SetShapePosZ(CurrentPosZ);
		CurrentPosZ += CurHeight;
	}	
	return true;
}

bool FCabinetShape::SetInsideRetractValue(float InValue)
{
	RetractValue = InValue;
	//递归设置给柜体空间
	if (TopSpaceShape.IsValid())
	{
		return TopSpaceShape->SetUnitShapeRetractValue(RetractValue);
	}
	return true;
}

bool FCabinetShape::SetActiveRetractValue(float InValue)
{
	ActiveRetract = InValue;
	//递归设置给柜体空间
	if (TopSpaceShape.IsValid())
	{
		return TopSpaceShape->SetUnitShapeActiveRetract(ActiveRetract - RetractValue);
	}
	return true;
}

float FCabinetShape::GetBottomSpaceHeight()
{
	return BottomHeight;
}

bool FCabinetShape::SetBottomSpaceHeight(float InValue)
{
	BottomHeight = InValue;
	if (TopSpaceShape.IsValid())
	{
		bool bResult = TopSpaceShape->IsValidForBottomHeight(InValue);
		if (bResult)
		{
			return TopSpaceShape->SetBottomSpaceHeight(BottomHeight);
		}
		return false;
	}
	return true;
}

void FCabinetShape::ShowDoorGroup(bool bShow)
{
	bShowDoorGroup = bShow;
	if (TopSpaceShape.IsValid())
	{
		if (bShowDoorGroup)
		{
			TopSpaceShape->ShowDoorGropActor();
		}
		else
		{
			TopSpaceShape->HideDoorGropActor();
		}
	}
}

bool FCabinetShape::IsDoorGroupShow()
{
	return bShowDoorGroup;
}

bool FCabinetShape::IsContainDoorGroup()
{
	if (TopSpaceShape.IsValid())
	{
		return TopSpaceShape->IsContainDoorGroup();
	}
	return false;
}

void FCabinetShape::ShowDrawer(const bool bInShow)
{	
	bShowDrawer = bInShow;
	if (TopSpaceShape.IsValid())
	{
		if (bShowDrawer)
		{
			TopSpaceShape->ShowDrawerActor();
		}
		else
		{
			TopSpaceShape->HideDrawerActor();
		}
	}

}

void FCabinetShape::HiddenCabinetShapeActors(bool bHidden)
{
	bHiddenCabinetActor = bHidden;
	if (TopSpaceShape.IsValid())
	{
		TopSpaceShape->HiddenSpaceShapeActors(bHidden);
	}
	if (VeneerdSudeBoardGroup.IsValid())
	{
		VeneerdSudeBoardGroup->HiddenVeneerdGroup(bHidden);
	}
}

void FCabinetShape::UpdateCabinet()
{
	if (TopSpaceShape.IsValid())
	{
		TopSpaceShape->UpdateSpaceShape();
	}
}

bool FCabinetShape::SetTopSpaceOutline(TSharedPtr<FSCTOutline> InOutline, bool bForce/* = false*/)
{
	check(TopSpaceShape.IsValid());
	if (!TopSpaceShape->IsSpaceShapeEmpty())
	{
		if (bForce)
		{
			TopSpaceShape->EmptySpaceShape();
		}
		else
		{
			return false;
		}
	}

	return TopSpaceShape->SetSpaceOutline(InOutline);
}

TSharedPtr<FSCTOutline> FCabinetShape::GetTopSpaceOutline()
{
	return TopSpaceShape->GetSpaceOutline();
}

bool FCabinetShape::SetOutlineParamValueByRefName(const FString &InRefName, float InValue)
{
	return TopSpaceShape->SetOutlineParamValueByRefName(InRefName, InValue);
}

void FCabinetShape::RemoveInfluenceBoardInSpace(FBoardShape* InBoard)
{
	if (TopSpaceShape.IsValid())
	{
		TopSpaceShape->RemoveInfluenceBoard(InBoard);
	}
}

bool FCabinetShape::ModifyInsidePosition(FInsideShape* InInside, const FVector &InDelta, bool bIgnoreZero, bool bContinuedOperator)
{
	FVector FinalDelta(InDelta + SurplusDelta);
	SurplusDelta.Set(0.0, 0.0, 0.0);
	//1、父级内胆为空则不允许调整位置
	FInsideShape* ParentInside = InInside->GetParentInsideShape();
	if (!ParentInside)
	{
		return false;
	}

	//2、获取当前内胆的第一个尺寸不为0的线性邻接内胆
	int32 CurIndex = ParentInside->GetChildInsideShapeIndex(InInside);
	TArray<TSharedPtr<FInsideShape>>& ChildInsides = ParentInside->GetChildInsideShapes();
	int32 ChildCount = ChildInsides.Num();
	TSharedPtr<FInsideShape> CurInside = ChildInsides[CurIndex];
	TSharedPtr<FInsideShape> LoBInside = nullptr;
	TSharedPtr<FInsideShape> RoUInside = nullptr;

	//3、区分当前内胆的方向类型分别处理
	int32 DirectType = CurInside->GetDirectType();
	if (IDT_Horizontal == DirectType)
	{
		//0、当前位置偏移量
		float CurDelta = FinalDelta.Z;

		//1、获取受影响的相邻内胆
		if (bContinuedOperator)
		{
			if (0 <= LoBIndex && LoBIndex < ChildCount)
			{
				LoBInside = ChildInsides[LoBIndex];
			}
			if (0 <= RoUIndex && RoUIndex < ChildCount)
			{
				RoUInside = ChildInsides[RoUIndex];
			}
		}
		else
		{
			//获取下侧第一个尺寸不为0的线性邻接内胆
			LoBIndex = CurIndex - 1;
			while (LoBIndex >= 0)
			{
				LoBInside = ChildInsides[LoBIndex];
				if (bIgnoreZero && LoBInside->GetShapeHeight() < KINDA_SMALL_NUMBER)
				{
					LoBIndex -= 1;
					LoBInside = nullptr;
				}
				else
				{
					break;
				}
			}
			//获取上侧第一个尺寸不为0的线性邻接内胆
			RoUIndex = CurIndex + 1;
			while (RoUIndex < ChildCount)
			{
				RoUInside = ChildInsides[RoUIndex];
				if (bIgnoreZero && RoUInside->GetShapeHeight() < KINDA_SMALL_NUMBER)
				{
					RoUIndex += 1;
					RoUInside = nullptr;
				}
				else
				{
					break;
				}
			}
		}
		if (!LoBInside.IsValid())
		{
			return false;
		}
		if (!RoUInside.IsValid())
		{
			return false;
		}

		//2、下侧相邻内胆的变化范围
		float LoBMin = 0.0;
		float LoBMax = 0.0;
		float LoBCur = LoBInside->GetInsideHeightRange(LoBMin, LoBMax);
		bool bLoBValid = true;
		if (LoBCur + CurDelta < LoBMin || LoBCur + CurDelta > LoBMax)
		{
			//相邻内胆中子对象可复制，特殊处理
			TSharedPtr<FSCTShape> LoBSelf = LoBInside->GetSelfShape();
			if (LoBSelf.IsValid() && ST_Base == LoBSelf->GetShapeType())
			{
				LoBMin = LoBSelf->GetShapeHeight();
				LoBMax = 10000.0;
				bLoBValid = false;
				if (LoBCur + CurDelta < LoBMin)
				{
					return false;
				}
			}
		}
		//当前偏移量超出变化极值，修改偏移量
		if (CurDelta > 0)
		{
			if (LoBCur >= LoBMax)
			{
				return false;
			}
			if (LoBCur + CurDelta > LoBMax)
			{
				CurDelta = LoBMax - LoBCur;
			}
		}
		else if (CurDelta < 0)
		{
			if (LoBCur <= LoBMin)
			{
				return false;
			}
			if (LoBCur + CurDelta < LoBMin)
			{
				CurDelta = LoBMin - LoBCur;
			}
		}

		//3、上侧相邻内胆的变化范围
		float RoUMin = 0.0;
		float RoUMax = 0.0;
		float RoUCur = RoUInside->GetInsideHeightRange(RoUMin, RoUMax);
		bool bRoUValid = true;
		if (RoUCur - CurDelta < RoUMin || RoUCur - CurDelta > RoUMax)
		{
			//相邻内胆中子对象可复制，特殊处理
			TSharedPtr<FSCTShape> RoUSelf = RoUInside->GetSelfShape();
			if (RoUSelf.IsValid() && ST_Base == RoUSelf->GetShapeType())
			{
				RoUMin = RoUSelf->GetShapeHeight();
				RoUMax = 10000.0;
				bRoUValid = false;
				if (RoUCur - CurDelta < RoUMin)
				{
					return false;
				}
			}
		}
		//当前偏移量超出变化极值，修改偏移量
		if (CurDelta < 0)
		{
			if (RoUCur >= RoUMax)
			{
				return false;
			}
			if (RoUCur - CurDelta > RoUMax)
			{
				CurDelta = RoUCur - RoUMax;
			}
		}
		else if (CurDelta > 0)
		{
			if (RoUCur <= RoUMin)
			{
				return false;
			}
			if (RoUCur - CurDelta < RoUMin)
			{
				CurDelta = RoUCur - RoUMin;
			}
		}

		//4、两个相邻内胆均为不连续变化时尺寸调整失败
		if (!bLoBValid && !bRoUValid)
		{
			return false;
		}

		//5、首先处理不连续变化的内胆尺寸，并修改调整值
		if (!bLoBValid)
		{
			float OldLoBHeight = LoBInside->GetShapeHeight();
			LoBInside->SetShapeHeight(OldLoBHeight + CurDelta);
			float NewLoBHeight = LoBInside->GetShapeHeight();
			float SuplusValue = CurDelta - (NewLoBHeight - OldLoBHeight);
			if (FMath::Abs(SuplusValue) > KINDA_SMALL_NUMBER)
			{
				CurDelta = NewLoBHeight - OldLoBHeight;
			}
			SurplusDelta.Set(0.0, 0.0, SuplusValue);
		}
		if (!bRoUValid)
		{
			float OldRoUHeight = RoUInside->GetShapeHeight();
			RoUInside->SetShapeHeight(OldRoUHeight - CurDelta);
			float NewRoUHeight = RoUInside->GetShapeHeight();
			float SuplusValue = CurDelta - (OldRoUHeight - NewRoUHeight);
			if (FMath::Abs(SuplusValue) > KINDA_SMALL_NUMBER)
			{
				CurDelta = OldRoUHeight - NewRoUHeight;
			}
			SurplusDelta.Set(0.0, 0.0, SuplusValue);
		}

		//6、根据调整至，处理当前内胆和相邻内胆的尺寸和位置
		//处理当前内胆的位置
		CurInside->SetShapePosZ(CurInside->GetShapePosZ() + CurDelta);
		//处理下侧相邻内胆的尺寸和位置
		for (int32 i = CurIndex - 1; i > LoBIndex; --i)
		{
			ChildInsides[i]->SetShapePosZ(ChildInsides[i]->GetShapePosZ() + CurDelta);
		}
		if (bLoBValid)
		{
			LoBInside->SetShapeHeight(LoBInside->GetShapeHeight() + CurDelta);
		}
		//处理上侧相邻内胆的尺寸和位置
		for (int32 i = CurIndex + 1; i < RoUIndex; ++i)
		{
			ChildInsides[i]->SetShapePosZ(ChildInsides[i]->GetShapePosZ() + CurDelta);
		}
		if (bRoUValid)
		{
			RoUInside->SetShapeHeight(RoUInside->GetShapeHeight() - CurDelta);
		}
		RoUInside->SetShapePosZ(RoUInside->GetShapePosZ() + CurDelta);

		return true;
	}
	else if (IDT_Vertical == DirectType)
	{
		//0、当前位置偏移量
		float CurDelta = FinalDelta.X;

		//1、获取受影响的相邻内胆
		if (bContinuedOperator)
		{
			if (0 <= LoBIndex && LoBIndex < ChildCount)
			{
				LoBInside = ChildInsides[LoBIndex];
			}
			if (0 <= RoUIndex && RoUIndex < ChildCount)
			{
				RoUInside = ChildInsides[RoUIndex];
			}
		}
		else
		{
			//获取左侧第一个尺寸不为0的线性邻接内胆
			LoBIndex = CurIndex - 1;
			while (LoBIndex >= 0)
			{
				LoBInside = ChildInsides[LoBIndex];
				if (bIgnoreZero && LoBInside->GetShapeWidth() < KINDA_SMALL_NUMBER)
				{
					LoBIndex -= 1;
					LoBInside = nullptr;
				}
				else
				{
					break;
				}
			}
			//获取右侧第一个尺寸不为0的线性邻接内胆
			RoUIndex = CurIndex + 1;
			while (RoUIndex < ChildCount)
			{
				RoUInside = ChildInsides[RoUIndex];
				if (bIgnoreZero && RoUInside->GetShapeWidth() < KINDA_SMALL_NUMBER)
				{
					RoUIndex += 1;
					RoUInside = nullptr;
				}
				else
				{
					break;
				}
			}
		}
		if (!LoBInside.IsValid())
		{
			return false;
		}
		if (!RoUInside.IsValid())
		{
			return false;
		}

		//2、左侧相邻内胆的变化范围
		float LoBMin = 0.0;
		float LoBMax = 0.0;
		float LoBCur = LoBInside->GetInsideWidthRange(LoBMin, LoBMax);
		//当前偏移量超出变化极值，修改偏移量
		if (CurDelta > 0)
		{
			if (LoBCur >= LoBMax)
			{
				return false;
			}
			if (LoBCur + CurDelta > LoBMax)
			{
				CurDelta = LoBMax - LoBCur;
			}
		}
		else if (CurDelta < 0)
		{
			if (LoBCur <= LoBMin)
			{
				return false;
			}
			if (LoBCur + CurDelta < LoBMin)
			{
				CurDelta = LoBMin - LoBCur;
			}
		}

		//3、右侧相邻内胆的变化范围
		float RoUMin = 0.0;
		float RoUMax = 0.0;
		float RoUCur = RoUInside->GetInsideWidthRange(RoUMin, RoUMax);
		//当前偏移量超出变化极值，修改偏移量
		if (CurDelta < 0)
		{
			if (RoUCur >= RoUMax)
			{
				return false;
			}
			if (RoUCur - CurDelta > RoUMax)
			{
				CurDelta = RoUCur - RoUMax;
			}
		}
		else if (CurDelta > 0)
		{
			if (RoUCur <= RoUMin)
			{
				return false;
			}
			if (RoUCur - CurDelta < RoUMin)
			{
				CurDelta = RoUCur - RoUMin;
			}
		}

		//4、当前操作允许，修改当前位置，同时修改上下相邻内胆的尺寸
		CurInside->SetShapePosX(CurInside->GetShapePosX() + CurDelta);
		for (int32 i = CurIndex - 1; i > LoBIndex; --i)
		{
			ChildInsides[i]->SetShapePosX(ChildInsides[i]->GetShapePosX() + CurDelta);
		}
		LoBInside->SetShapeWidth(LoBInside->GetShapeWidth() + CurDelta);
		for (int32 i = CurIndex + 1; i < RoUIndex; ++i)
		{
			ChildInsides[i]->SetShapePosX(ChildInsides[i]->GetShapePosX() + CurDelta);
		}
		RoUInside->SetShapePosX(RoUInside->GetShapePosX() + CurDelta);
		RoUInside->SetShapeWidth(RoUInside->GetShapeWidth() - CurDelta);

		return true;
	}
	else
	{
		check(false);
	}
	return false;
}

bool FCabinetShape::ModifInsideDimention(FInsideShape* InInside, const FVector &InDelta, bool bPositive, bool bIgnoreZero, bool bContinuedOperator)
{
	FVector FinalDelta(InDelta + SurplusDelta);
	SurplusDelta.Set(0.0, 0.0, 0.0);
	//1、父级内胆为空则不允许调整位置
	FInsideShape* ParentInside = InInside->GetParentInsideShape();
	if (!ParentInside)
	{
		return false;
	}

	//2、获取当前内胆的第一个尺寸不为0的线性邻接内胆
	int32 CurIndex = ParentInside->GetChildInsideShapeIndex(InInside);
	TArray<TSharedPtr<FInsideShape>>& ChildInsides = ParentInside->GetChildInsideShapes();
	int32 ChildCount = ChildInsides.Num();
	TSharedPtr<FInsideShape> CurInside = ChildInsides[CurIndex];
	TSharedPtr<FInsideShape> NextInside = nullptr;

	//3、根据调节方向和当前内胆方向修改尺寸
	int32 DirectType = CurInside->GetDirectType();
	if (IDT_Horizontal == DirectType && bPositive)  //水平内胆正向尺寸变化
	{
		//0、当前变化量
		float CurDelta = FinalDelta.Z;

		//1、获取受影响的相邻内胆
		if (bContinuedOperator)
		{
			if (0 <= RoUIndex && RoUIndex < ChildCount)
			{
				NextInside = ChildInsides[RoUIndex];
			}
		}
		else
		{
			//获取上侧第一个尺寸不为0的线性邻接内胆
			RoUIndex = CurIndex + 1;
			while (RoUIndex < ChildCount)
			{
				NextInside = ChildInsides[RoUIndex];
				if (bIgnoreZero && NextInside->GetShapeHeight() < KINDA_SMALL_NUMBER)
				{
					RoUIndex += 1;
					NextInside = nullptr;
				}
				else
				{
					break;
				}
			}
		}
		if (!NextInside.IsValid())
		{
			return false;
		}

		//2、当前内胆的尺寸变化范围
		float CurMin = 0.0;
		float CurMax = 0.0;
		float CurCur = CurInside->GetInsideHeightRange(CurMin, CurMax);
		bool bCurValid = true;
		if (CurCur + CurDelta < CurMin || CurCur + CurDelta > CurMax)
		{
			//当前内胆中子对象可复制，特殊处理
			TSharedPtr<FSCTShape> CurSelf = CurInside->GetSelfShape();
			if (CurSelf.IsValid() && ST_Base == CurSelf->GetShapeType())
			{
				CurMin = CurSelf->GetShapeHeight();
				CurMax = 10000.0;
				bCurValid = false;
				if (CurCur + CurDelta < CurMin)
				{
					return false;
				}
			}
			else //当前内胆尺寸不满足尺寸调整
			{
				return false;
			}
		}

		//3、上侧相邻内胆的尺寸变化范围
		float NextMin = 0.0;
		float NextMax = 0.0;
		float NextCur = NextInside->GetInsideHeightRange(NextMin, NextMax);
		bool bNextValid = true;
		if (NextCur - CurDelta < NextMin || NextCur - CurDelta > NextMax)
		{
			//相邻内胆中子对象可复制，特殊处理
			TSharedPtr<FSCTShape> NextSelf = NextInside->GetSelfShape();
			if (NextSelf.IsValid() && ST_Base == NextSelf->GetShapeType())
			{
				NextMin = NextSelf->GetShapeHeight();
				NextMax = 10000.0;
				bNextValid = false;
				if (NextCur - CurDelta < NextMin)
				{
					return false;
				}
			}
		}
		//相邻内胆尺寸变化范围小于当前调整值，需要修正调整值
		if (CurDelta < 0)
		{
			if (NextCur >= NextMax)
			{
				return false;
			}
			if (NextCur - CurDelta > NextMax)
			{
				CurDelta = NextCur - NextMax;
			}
		}
		else if (CurDelta > 0)
		{
			if (NextCur <= NextMin)
			{
				return false;
			}
			if (NextCur - CurDelta < NextMin)
			{
				CurDelta = NextCur - NextMin;
			}
		}

		//4、当前内胆和相邻内胆均为不连续变化时尺寸调整失败
		if (!bCurValid && !bNextValid)
		{
			return false;
		}

		//5、首先处理不连续变化的内胆尺寸，并修改调整值
		if (!bCurValid)
		{
			float OldCurHeight = CurInside->GetShapeHeight();
			CurInside->SetShapeHeight(OldCurHeight + CurDelta);
			float NewCurHeight = CurInside->GetShapeHeight();
			float SuplusValue = CurDelta - (NewCurHeight - OldCurHeight);
			if (FMath::Abs(SuplusValue) > KINDA_SMALL_NUMBER)
			{
				CurDelta = NewCurHeight - OldCurHeight;
			}
			SurplusDelta.Set(0.0, 0.0, SuplusValue);
		}
		if (!bNextValid)
		{
			float OldNextHeight = NextInside->GetShapeHeight();
			NextInside->SetShapeHeight(OldNextHeight - CurDelta);
			float NewNextHeight = NextInside->GetShapeHeight();
			float SuplusValue = CurDelta - (OldNextHeight - NewNextHeight);
			if (FMath::Abs(SuplusValue) > KINDA_SMALL_NUMBER)
			{
				CurDelta = OldNextHeight - NewNextHeight;
			}
			SurplusDelta.Set(0.0, 0.0, SuplusValue);
		}

		//6、根据调整至，处理当前内胆和相邻内胆的尺寸和位置
		//处理当前内胆的尺寸
		if (bCurValid)
		{
			CurInside->SetShapeHeight(CurInside->GetShapeHeight() + CurDelta);
		}
		//处理中间的尺寸为0的空内胆位置
		for (int32 i = CurIndex + 1; i < RoUIndex; ++i)
		{
			ChildInsides[i]->SetShapePosZ(ChildInsides[i]->GetShapePosZ() + CurDelta);
		}
		//处理相邻内胆的位置和尺寸
		if (bNextValid)
		{
			NextInside->SetShapeHeight(NextInside->GetShapeHeight() - CurDelta);
		}
		NextInside->SetShapePosZ(NextInside->GetShapePosZ() + CurDelta);

		return true;
	}
	else if (IDT_Vertical == DirectType && bPositive)  //垂直内胆正向尺寸变化
	{
		//0、当前变化量
		float CurDelta = FinalDelta.X;

		//1、获取受影响的相邻内胆
		if (bContinuedOperator)
		{
			if (0 <= RoUIndex && RoUIndex < ChildCount)
			{
				NextInside = ChildInsides[RoUIndex];
			}
		}
		else
		{
			//获取右侧第一个尺寸不为0的线性邻接内胆
			RoUIndex = CurIndex + 1;
			while (RoUIndex < ChildCount)
			{
				NextInside = ChildInsides[RoUIndex];
				if (bIgnoreZero && NextInside->GetShapeWidth() < KINDA_SMALL_NUMBER)
				{
					RoUIndex += 1;
					NextInside = nullptr;
				}
				else
				{
					break;
				}
			}
		}
		if (!NextInside.IsValid())
		{
			return false;
		}

		//2、判断当前内胆的尺寸变化范围是否允许当前操作
		float CurMin = 0.0;
		float CurMax = 0.0;
		float CurCur = InInside->GetInsideWidthRange(CurMin, CurMax);
		if (CurCur + CurDelta < CurMin || CurCur + CurDelta > CurMax)
		{
			return false; //当前内胆的尺寸范围不允许当前操作
		}


		//3、判断右侧相邻内胆是否允许当前操作
		float NextMin = 0.0;  //获取相邻内胆的变化范围
		float NextMax = 0.0;
		float NextCur = NextInside->GetInsideWidthRange(NextMin, NextMax);
		//当前偏移量超出变化极值，修改偏移量
		if (CurDelta < 0)
		{
			if (NextCur >= NextMax)
			{
				return false;
			}
			if (NextCur - CurDelta > NextMax)
			{
				CurDelta = NextCur - NextMax;
			}
		}
		else if (CurDelta > 0)
		{
			if (NextCur <= NextMin)
			{
				return false;
			}
			if (NextCur - CurDelta < NextMin)
			{
				CurDelta = NextCur - NextMin;
			}
		}

		//4、当前操作允许，修改当前位置，同时修改相邻内胆的尺寸
		CurInside->SetShapeWidth(CurInside->GetShapeWidth() + CurDelta);
		for (int32 i = CurIndex + 1; i < RoUIndex; ++i)
		{
			ChildInsides[i]->SetShapePosX(ChildInsides[i]->GetShapePosX() + CurDelta);
		}
		NextInside->SetShapePosX(NextInside->GetShapePosX() + CurDelta);
		NextInside->SetShapeWidth(NextInside->GetShapeWidth() - CurDelta);

		return true;
	}
	else if (IDT_Horizontal == DirectType && !bPositive) //水平内胆负向尺寸变化
	{
		//0、当前变化量
		float CurDelta = FinalDelta.Z;

		//1、获取受影响的相邻内胆
		if (bContinuedOperator)
		{
			if (0 <= LoBIndex && LoBIndex < ChildCount)
			{
				NextInside = ChildInsides[LoBIndex];
			}
		}
		else
		{
			//获取下侧第一个尺寸不为0的线性邻接内胆
			LoBIndex = CurIndex - 1;
			while (LoBIndex >= 0)
			{
				NextInside = ChildInsides[LoBIndex];
				if (bIgnoreZero && NextInside->GetShapeHeight() < KINDA_SMALL_NUMBER)
				{
					LoBIndex -= 1;
					NextInside = nullptr;
				}
				else
				{
					break;
				}
			}
		}
		if (!NextInside.IsValid())
		{
			return false;
		}

		//2、当前内胆的尺寸变化范围
		float CurMin = 0.0;
		float CurMax = 0.0;
		float CurCur = InInside->GetInsideHeightRange(CurMin, CurMax);
		bool bCurValid = true;
		if (CurCur - CurDelta < CurMin || CurCur - CurDelta > CurMax)
		{
			//当前内胆中子对象可复制，特殊处理
			TSharedPtr<FSCTShape> CurSelf = CurInside->GetSelfShape();
			if (CurSelf.IsValid() && ST_Base == CurSelf->GetShapeType())
			{
				CurMin = CurSelf->GetShapeHeight();
				CurMax = 10000.0;
				bCurValid = false;
				if (CurCur + CurDelta < CurMin)
				{
					return false;
				}
			}
			else //当前内胆尺寸不满足尺寸调整
			{
				return false;
			}
		}

		//3、下侧相邻内胆的变化范围
		float NextMin = 0.0;
		float NextMax = 0.0;
		float NextCur = NextInside->GetInsideHeightRange(NextMin, NextMax);
		bool bNextValid = true;
		if (NextCur + CurDelta < NextMin || NextCur + CurDelta > NextMax)
		{
			//相邻内胆中子对象可复制，特殊处理
			TSharedPtr<FSCTShape> NextSelf = NextInside->GetSelfShape();
			if (NextSelf.IsValid() && ST_Base == NextSelf->GetShapeType())
			{
				NextMin = NextSelf->GetShapeHeight();
				NextMax = 10000.0;
				bNextValid = false;
				if (NextCur - CurDelta < NextMin)
				{
					return false;
				}
			}
		}
		//当前偏移量超出变化极值，修改偏移量
		if (CurDelta > 0)
		{
			if (NextCur >= NextMax)
			{
				return false;
			}
			if (NextCur + CurDelta > NextMax)
			{
				CurDelta = NextMax - NextCur;
			}
		}
		else if (CurDelta < 0)
		{
			if (NextCur <= NextMin)
			{
				return false;
			}
			if (NextCur + CurDelta < NextMin)
			{
				CurDelta = NextMin - NextCur;
			}
		}

		//4、当前内胆和相邻内胆均为不连续变化时尺寸调整失败
		if (!bCurValid && !bNextValid)
		{
			return false;
		}

		//5、首先处理不连续变化的内胆尺寸，并修改调整值
		if (!bCurValid)
		{
			float OldCurHeight = CurInside->GetShapeHeight();
			CurInside->SetShapeHeight(OldCurHeight - CurDelta);
			float NewCurHeight = CurInside->GetShapeHeight();
			float SuplusValue = CurDelta - (OldCurHeight - NewCurHeight);
			if (FMath::Abs(SuplusValue) > KINDA_SMALL_NUMBER)
			{
				CurDelta = OldCurHeight - NewCurHeight;
			}
			SurplusDelta.Set(0.0, 0.0, SuplusValue);
		}
		if (!bNextValid)
		{
			float OldNextHeight = NextInside->GetShapeHeight();
			NextInside->SetShapeHeight(OldNextHeight - CurDelta);
			float NewNextHeight = NextInside->GetShapeHeight();
			float SuplusValue = CurDelta - (NewNextHeight - OldNextHeight);
			if (FMath::Abs(SuplusValue) > KINDA_SMALL_NUMBER)
			{
				CurDelta = NewNextHeight - OldNextHeight;
			}
			SurplusDelta.Set(0.0, 0.0, SuplusValue);
		}

		//6、根据调整至，处理当前内胆和相邻内胆的尺寸和位置
		//处理当前内胆的尺寸
		if (bCurValid)
		{
			CurInside->SetShapeHeight(CurInside->GetShapeHeight() - CurDelta);
		}
		//处理当前内胆的位置
		CurInside->SetShapePosZ(CurInside->GetShapePosZ() + CurDelta);
		//处理中间的尺寸为0的空内胆位置
		for (int32 i = CurIndex - 1; i > LoBIndex; --i)
		{
			ChildInsides[i]->SetShapePosZ(ChildInsides[i]->GetShapePosZ() + CurDelta);
		}
		//处理相邻内胆的尺寸
		if (bNextValid)
		{
			NextInside->SetShapeHeight(NextInside->GetShapeHeight() + CurDelta);
		}

		return true;
	}
	else if (IDT_Vertical == DirectType && !bPositive) //垂直内胆负向尺寸变化
	{
		//0、当前变化量
		float CurDelta = FinalDelta.X;

		//1、获取受影响的相邻内胆
		if (bContinuedOperator)
		{
			if (0 <= LoBIndex && LoBIndex < ChildCount)
			{
				NextInside = ChildInsides[LoBIndex];
			}
		}
		else
		{
			//获取左侧第一个尺寸不为0的线性邻接内胆
			LoBIndex = CurIndex - 1;
			while (LoBIndex >= 0)
			{
				NextInside = ChildInsides[LoBIndex];
				if (bIgnoreZero && NextInside->GetShapeWidth() < KINDA_SMALL_NUMBER)
				{
					LoBIndex -= 1;
					NextInside = nullptr;
				}
				else
				{
					break;
				}
			}
		}
		if (!NextInside.IsValid())
		{
			return false;
		}

		//2、当前内胆的尺寸变化范围是否允许当前操作
		float CurMin = 0.0;
		float CurMax = 0.0;
		float CurCur = InInside->GetInsideWidthRange(CurMin, CurMax);
		if (CurCur - CurDelta < CurMin || CurCur - CurDelta > CurMax)
		{
			return false; //当前内胆的尺寸范围不允许当前操作
		}

		//3、判断左侧相邻内胆是否允许当前操作
		float NextMin = 0.0;  //获取相邻内胆的变化范围
		float NextMax = 0.0;
		float NextCur = NextInside->GetInsideWidthRange(NextMin, NextMax);
		//当前偏移量超出变化极值，修改偏移量
		if (CurDelta > 0)
		{
			if (NextCur >= NextMax)
			{
				return false;
			}
			if (NextCur + CurDelta > NextMax)
			{
				CurDelta = NextMax - NextCur;
			}
		}
		else if (CurDelta < 0)
		{
			if (NextCur <= NextMin)
			{
				return false;
			}
			if (NextCur + CurDelta < NextMin)
			{
				CurDelta = NextMin - NextCur;
			}
		}

		//4、当前操作允许，修改当前位置，同时修改相邻内胆的尺寸
		CurInside->SetShapePosX(CurInside->GetShapePosX() + CurDelta);
		CurInside->SetShapeWidth(CurInside->GetShapeWidth() - CurDelta);
		for (int32 i = CurIndex - 1; i > LoBIndex; --i)
		{
			ChildInsides[i]->SetShapePosX(ChildInsides[i]->GetShapePosX() + CurDelta);
		}
		NextInside->SetShapeWidth(NextInside->GetShapeWidth() + CurDelta);

		return true;
	}
	else
	{
		check(false);
	}

	return false;
}
