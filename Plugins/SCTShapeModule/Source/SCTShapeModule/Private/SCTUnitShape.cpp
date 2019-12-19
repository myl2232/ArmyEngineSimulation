#include "SCTUnitShape.h"
#include "SCTInsideShape.h"
#include "SCTShapeBoxActor.h"
#include "SCTAttribute.h"
#include "SharedPointer.h"
#include "JsonObject.h"
#include "Math/UnrealMathUtility.h"
#include "SCTShapeActor.h"
#include "SCTBoardShape.h"
#include "Engine/CollisionProfile.h"
#include "SCTShapeManager.h"
#include "SCTShape.h"

FUnitShape::FUnitShape()
	:FSCTShape()
{
	SetShapeType(ST_Unit);
	SetShapeName(TEXT("新内胆"));
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

FUnitShape::~FUnitShape()
{
	
}

void FUnitShape::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//解析型录基本信息
	FSCTShape::ParseFromJson(InJsonObject);
	//修改位置最小值>=0的情况
	TSharedPtr<FNumberRangeAttri> UnitPosXAttri = StaticCastSharedPtr<FNumberRangeAttri>(PositionX);
	if (UnitPosXAttri->GetMinValue() >= 0.0)
	{
		UnitPosXAttri->SetMinValue(-10000.0);
	}
	TSharedPtr<FNumberRangeAttri> UnitPosYAttri = StaticCastSharedPtr<FNumberRangeAttri>(PositionY);
	if (UnitPosYAttri->GetMinValue() >= 0.0)
	{
		UnitPosYAttri->SetMinValue(-10000.0);
	}
	TSharedPtr<FNumberRangeAttri> UnitPosZAttri = StaticCastSharedPtr<FNumberRangeAttri>(PositionZ);
	if (UnitPosZAttri->GetMinValue() >= 0.0)
	{
		UnitPosZAttri->SetMinValue(-10000.0);
	}

	//解析子级型录(顶层内胆)
	const TArray<TSharedPtr<FJsonValue>>* ChildList = nullptr;
	InJsonObject->TryGetArrayField(TEXT("children"), ChildList);
	if (ChildList != nullptr && (*ChildList).Num() > 0)
	{
		check((*ChildList).Num() == 1);
		for (auto& It : *ChildList)
		{
			TSharedPtr<FJsonObject> ChildObject = It->AsObject();
			int32 ShapeType = ChildObject->GetNumberField(TEXT("type"));
			int64 ShapeId = ChildObject->GetNumberField(TEXT("id"));
			TSharedPtr<FSCTShape> NewShape = FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ShapeType, ShapeId);
			TSharedPtr<FSCTShape> CopyShape = FSCTShapeManager::Get()->CopyShapeToNew(NewShape);
			TSharedPtr<FInsideShape> TopInside = StaticCastSharedPtr<FInsideShape>(CopyShape);
			SetTopInsideShape(TopInside);
			CopyShape->ParseAttributesFromJson(ChildObject);
		}
	}
}

void FUnitShape::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
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
		check((*ChildList).Num() == 1);
		for (auto& It : *ChildList)
		{
			TSharedPtr<FJsonObject> ChildObject = It->AsObject();
			int32 ShapeType = ChildObject->GetNumberField(TEXT("type"));
			int64 ShapeId = ChildObject->GetNumberField(TEXT("id"));
			TSharedPtr<FSCTShape> NewShape = FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ShapeType, ShapeId);
			TSharedPtr<FSCTShape> CopyShape = FSCTShapeManager::Get()->CopyShapeToNew(NewShape);
			TSharedPtr<FInsideShape> TopInside = StaticCastSharedPtr<FInsideShape>(CopyShape);
			SetTopInsideShape(TopInside);
			CopyShape->ParseAttributesFromJson(ChildObject);
		}
	}
}

void FUnitShape::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseShapeFromJson(InJsonObject);
}

void FUnitShape::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//基类信息
	FSCTShape::ParseContentFromJson(InJsonObject);
}

void FUnitShape::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//保存型录基本信息
	FSCTShape::SaveToJson(JsonWriter);

	//保存Children信息(顶层内胆)
	JsonWriter->WriteArrayStart(TEXT("children"));
	JsonWriter->WriteObjectStart();
	TopInsideShape->SaveAttriToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();
	JsonWriter->WriteArrayEnd();

	//保存Shapes信息
	//统计当前型录所有层级的子型录
	FSCTShapeManager::Get()->CountInheritChildShapes(this);
	JsonWriter->WriteArrayStart(TEXT("shapes"));
	for (int32 i=ST_None; i<ST_Max; ++i)
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

void FUnitShape::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//1、型录基本信息
	FSCTShape::SaveAttriToJson(JsonWriter);

	//2、子型录信息
	TArray<TSharedPtr<FSCTShape>> ChildrenShapes = GetChildrenShapes();
	JsonWriter->WriteArrayStart(TEXT("children"));
	//TopInsideShape
	if (TopInsideShape.IsValid())
	{
		JsonWriter->WriteObjectStart();
		TopInsideShape->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//End children
	JsonWriter->WriteArrayEnd();
}

void FUnitShape::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveShapeToJson(JsonWriter);
}

void FUnitShape::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//基类信息
	FSCTShape::SaveContentToJson(JsonWriter);
}

void FUnitShape::CopyTo(FSCTShape* OutShape)
{
	//基类信息拷贝
	FSCTShape::CopyTo(OutShape);

	//单元柜信息拷贝
	FUnitShape* OutUnitShape = StaticCast<FUnitShape*>(OutShape);

	//顶层内胆
	if (TopInsideShape.IsValid())
	{
		TSharedPtr<FSCTShape> NewCopyShape = FSCTShapeManager::Get()->CopyShapeToNew(TopInsideShape);
		TSharedPtr<FInsideShape> NewTopInside = StaticCastSharedPtr<FInsideShape>(NewCopyShape);
		OutUnitShape->SetTopInsideShape(NewTopInside);
	}
}

ASCTShapeActor* FUnitShape::SpawnShapeActor()
{
	//创建单元柜型录Actor
	ASCTShapeActor* NewUnitActor = FSCTShape::SpawnShapeActor();

	//创建单元柜顶层内胆型录Actor
	if (TopInsideShape.IsValid())
	{
		ASCTShapeActor* TopInsideActor = TopInsideShape->SpawnShapeActor();
		TopInsideActor->AttachToActorOverride(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	
	return NewUnitActor;
}

void FUnitShape::SpawnActorsForSelected(FName InSelectProfileName, FName InHoverProfileName)
{
	SelfInsideProfileName = InSelectProfileName;
	NullInsideProfileName = InHoverProfileName;
	if (TopInsideShape.IsValid())
	{
		TopInsideShape->SpawnActorsForSelected(InSelectProfileName, InHoverProfileName);
	}
}

FName FUnitShape::GetSelfInsideProfileName()
{
	return SelfInsideProfileName;
}
FName FUnitShape::GetNullInsideProfileName()
{
	return NullInsideProfileName;
}

void FUnitShape::SetCollisionProfileName(FName InProfileName)
{
	if (TopInsideShape.IsValid())
	{
		TopInsideShape->SetCollisionProfileName(InProfileName);
	}
}

void FUnitShape::GetResourceUrls(TArray<FString> &OutResourceUrls)
{
	if (TopInsideShape.IsValid())
	{
		TopInsideShape->GetResourceUrls(OutResourceUrls);
	}
}

void FUnitShape::GetFileCachePaths(TArray<FString> &OutFileCachePaths)
{
	if (TopInsideShape.IsValid())
	{
		TopInsideShape->GetFileCachePaths(OutFileCachePaths);
	}
}

void FUnitShape::HiddenUnitShapeActors(bool bHidden)
{
	if (TopInsideShape.IsValid())
	{
		TopInsideShape->HiddenInsideShapeActors(bHidden);
	}
}

bool FUnitShape::SetShapeWidth(float InValue)
{
	bool bInside = true;
	if (TopInsideShape.IsValid())
	{
		float MinVlaue = 0.0;
		float MaxValue = 0.0;
		float CurValue = TopInsideShape->GetInsideWidthRange(MinVlaue, MaxValue);
		if (InValue < MinVlaue || InValue > MaxValue)
		{
			return false;
		}
		bInside = TopInsideShape->SetShapeWidth(InValue);
	}
	bool bResult = FSCTShape::SetShapeWidth(InValue);
	return bInside && bResult;
}
bool FUnitShape::SetShapeDepth(float InValue)
{
	bool bInside = true;
	if (TopInsideShape.IsValid())
	{
		float MinVlaue = 0.0;
		float MaxValue = 0.0;
		float CurValue = TopInsideShape->GetInsideDepthRange(MinVlaue, MaxValue);
		if (InValue < MinVlaue || InValue > MaxValue)
		{
			return false;
		}
		bInside = TopInsideShape->SetShapeDepth(InValue);
	}
	bool bResult = FSCTShape::SetShapeDepth(InValue);
	return bInside && bResult;
}
bool FUnitShape::SetShapeHeight(float InValue)
{
	bool bInside = true;
	if (TopInsideShape.IsValid())
	{
		float MinVlaue = 0.0;
		float MaxValue = 0.0;
		float CurValue = TopInsideShape->GetInsideHeightRange(MinVlaue, MaxValue);
		if (InValue < MinVlaue || InValue > MaxValue)
		{
			return false;
		}
		bInside = TopInsideShape->SetShapeHeight(InValue);
	}
	bool bResult = FSCTShape::SetShapeHeight(InValue);
	return bInside && bResult;
}

float FUnitShape::GetUnitWidthRange(float& MinValue, float& MaxValue)
{
	MinValue = 0.0;
	MaxValue = 10000.0;
	float CurValue = GetShapeWidth();
	if (TopInsideShape.IsValid())
	{
		CurValue = TopInsideShape->GetInsideHeightRange(MinValue, MaxValue);
	}
	return CurValue;
}
float FUnitShape::GetUnitDepthRange(float& MinValue, float& MaxValue)
{
	MinValue = 0.0;
	MaxValue = 10000.0;
	float CurValue = GetShapeDepth();
	if (TopInsideShape.IsValid())
	{
		CurValue = TopInsideShape->GetInsideHeightRange(MinValue, MaxValue);
	}
	return CurValue;
}
float FUnitShape::GetUnitHeightRange(float& MinValue, float& MaxValue)
{
	MinValue = 0.0;
	MaxValue = 10000.0;
	float CurValue = GetShapeHeight();
	if (TopInsideShape.IsValid())
	{
		CurValue = TopInsideShape->GetInsideHeightRange(MinValue, MaxValue);
	}
	return CurValue;
}

// void FUnitShape::SetUnitShapeType(int32 InType)
// {
// 	UnitShapeType = (EUnitShapeType)InType;
// }
// int32 FUnitShape::GetUnitShapeType()
// {
// 	return (int32)UnitShapeType;
// }

void FUnitShape::SetTopInsideShape(const TSharedPtr<FInsideShape> &InShape)
{
	//首先移除原有的顶层内胆
	if (TopInsideShape.IsValid())
	{
		RemoveChildShape(TopInsideShape);
		TopInsideShape = nullptr;
	}
	//创建内胆，并添加到子型录集合
	if (InShape.IsValid())
	{
		TopInsideShape = InShape;
		AddChildShape(TopInsideShape);
		TopInsideShape->SetParentShape(this);
		TopInsideShape->SetUnitShapeData(this);
	}
}
const TSharedPtr<FInsideShape>& FUnitShape::GetTopInsideShape() const
{
	return TopInsideShape;
}
void FUnitShape::DeleteTopTinsideShape()
{
	if (TopInsideShape.IsValid())
	{
		TopInsideShape->DestroyBoundingBoxActors();
		RemoveChildShape(TopInsideShape);
		TopInsideShape = nullptr;
	}
}

void FUnitShape::SetActiveRetractValue(float InValue)
{
	ActiveRetract = InValue;
	if (TopInsideShape.IsValid())
	{
		TopInsideShape->UpdateActiveBoardRetract();
	}
}

FInsideShape* FUnitShape::SplitSpecialInsideShape(const TSharedPtr<FSCTShape> &InChild, EInsideDirectType DirectType, FInsideShape* InInside)
{
	//1、获取当前内胆的方向类型
 	EInsideDirectType CurDirectType = InInside->GetDirectType();

	//2、分割当前内胆为三个子内胆
	TArray<FInsideShape*> ChildInsideShapes;
	if (DirectType == IDT_Horizontal)
	{
		SplitInsideShapeByHorizontal(ChildInsideShapes, InChild, InInside);
	} 
	else
	{
		SplitInsideShapeByVertical(ChildInsideShapes, InChild, InInside);
	}
	FInsideShape* SelectInside = ChildInsideShapes[1];

	//3、根据父级空间的方向类型添加三个子空间
	//当前内胆即为顶层内胆
	if (InInside == TopInsideShape.Get())
	{
		TopInsideShape->SetDirectType(DirectType == IDT_Horizontal ? IDT_Vertical : IDT_Horizontal);

		ASCTShapeActor* TopInsideActor = TopInsideShape->GetShapeActor();
		ASCTShapeActor* ChildInsideActor0 = ChildInsideShapes[0]->SpawnShapeActor();
		ChildInsideActor0->AttachToActorOverride(TopInsideActor, FAttachmentTransformRules::KeepRelativeTransform);
		TopInsideShape->InsertChildInsideShape(nullptr, MakeShareable(ChildInsideShapes[0]));
		ChildInsideShapes[0]->SpawnActorsForSelected(SelfInsideProfileName, NullInsideProfileName);
		
		ASCTShapeActor* ChildInsideActor1 = ChildInsideShapes[1]->SpawnShapeActor();
		ChildInsideActor1->AttachToActorOverride(TopInsideActor, FAttachmentTransformRules::KeepRelativeTransform);
		TopInsideShape->InsertChildInsideShape(nullptr, MakeShareable(ChildInsideShapes[1]));
		ChildInsideShapes[1]->SetSelfShape(InChild);
		ChildInsideShapes[1]->SpawnActorsForSelected(SelfInsideProfileName, NullInsideProfileName);
		ChildInsideShapes[1]->UpdateActiveBoardRetract();

		ASCTShapeActor* ChildInsideActor2 = ChildInsideShapes[2]->SpawnShapeActor();
		ChildInsideActor2->AttachToActorOverride(TopInsideActor, FAttachmentTransformRules::KeepRelativeTransform);
		TopInsideShape->InsertChildInsideShape(nullptr, MakeShareable(ChildInsideShapes[2]));
		ChildInsideShapes[2]->SpawnActorsForSelected(SelfInsideProfileName, NullInsideProfileName);

		//删除父级型录的BoundingBoxActor
		TopInsideShape->DestroyBoundingBoxActors();
	}
	else  //当前内胆不是顶层内胆
	{
		FInsideShape* CurParent = InInside->GetParentInsideShape();
		//if 方向一致：顺序插入三个新子内胆，然后删除原来的当前空内胆（注意插入顺序）
		//else 方向不一致：新建一个父级内胆结点，将新生成的三个子内胆置为其子节点，
		//同时新建两个尺寸为0的新内胆，作为新建父级内胆的线性相邻节点，
		//后续操作如同方向一致时的操作步骤（注意插入顺序）
		if (CurDirectType == DirectType)
		{
			ASCTShapeActor* CurParentActor = CurParent->GetShapeActor();
			//将新建子对象插入，并移除原对象
			ASCTShapeActor* ChildInsideActor0 = ChildInsideShapes[0]->SpawnShapeActor();
			ChildInsideActor0->AttachToActorOverride(CurParentActor, FAttachmentTransformRules::KeepRelativeTransform);
			CurParent->InsertChildInsideShape(InInside, MakeShareable(ChildInsideShapes[0]));
			ChildInsideShapes[0]->SpawnActorsForSelected(SelfInsideProfileName, NullInsideProfileName);

			ASCTShapeActor* ChildInsideActor1 = ChildInsideShapes[1]->SpawnShapeActor();
			ChildInsideActor1->AttachToActorOverride(CurParentActor, FAttachmentTransformRules::KeepRelativeTransform);
			CurParent->InsertChildInsideShape(InInside, MakeShareable(ChildInsideShapes[1]));
			ChildInsideShapes[1]->SetSelfShape(InChild);
			ChildInsideShapes[1]->SpawnActorsForSelected(SelfInsideProfileName, NullInsideProfileName);
			ChildInsideShapes[1]->UpdateActiveBoardRetract();

			ASCTShapeActor* ChildInsideActor2 = ChildInsideShapes[2]->SpawnShapeActor();
			ChildInsideActor2->AttachToActorOverride(CurParentActor, FAttachmentTransformRules::KeepRelativeTransform);
			CurParent->InsertChildInsideShape(InInside, MakeShareable(ChildInsideShapes[2]));
			ChildInsideShapes[2]->SpawnActorsForSelected(SelfInsideProfileName, NullInsideProfileName);

			//删除Hover到的原来的子内胆结点
			CurParent->DeleteChildInsideShape(InInside);
			InInside = nullptr;
		}
		else
		{
			TArray<FInsideShape*> PrtChildInsideShapes;
			SplitInsideShapeByIncongruous(PrtChildInsideShapes, CurDirectType, InInside);

			ASCTShapeActor* CurParentActor = CurParent->GetShapeActor();
			//将新建父级子对象插入，并移除原对象
			ASCTShapeActor* PrtChildInsideActor0 = PrtChildInsideShapes[0]->SpawnShapeActor();
			PrtChildInsideActor0->AttachToActorOverride(CurParentActor, FAttachmentTransformRules::KeepRelativeTransform);
			CurParent->InsertChildInsideShape(InInside, MakeShareable(PrtChildInsideShapes[0]));
			PrtChildInsideShapes[0]->SpawnActorsForSelected(SelfInsideProfileName, NullInsideProfileName);

			ASCTShapeActor* PrtChildInsideActor1 = PrtChildInsideShapes[1]->SpawnShapeActor();
			PrtChildInsideActor1->AttachToActorOverride(CurParentActor, FAttachmentTransformRules::KeepRelativeTransform);
			CurParent->InsertChildInsideShape(InInside, MakeShareable(PrtChildInsideShapes[1]));
			PrtChildInsideShapes[1]->SpawnActorsForSelected(SelfInsideProfileName, NullInsideProfileName);

			ASCTShapeActor* PrtChildInsideActor2 = PrtChildInsideShapes[2]->SpawnShapeActor();
			PrtChildInsideActor2->AttachToActorOverride(CurParentActor, FAttachmentTransformRules::KeepRelativeTransform);
			CurParent->InsertChildInsideShape(InInside, MakeShareable(PrtChildInsideShapes[2]));
			PrtChildInsideShapes[2]->SpawnActorsForSelected(SelfInsideProfileName, NullInsideProfileName);

			//删除Hover到的原来的子内胆结点
			CurParent->DeleteChildInsideShape(InInside);

			//修改子级对象的位置
			if (IDT_Horizontal == CurDirectType)
			{
				ChildInsideShapes[0]->SetShapePosZ(0.0);
				ChildInsideShapes[1]->SetShapePosZ(0.0);
				ChildInsideShapes[2]->SetShapePosZ(0.0);
			}
			else
			{
				ChildInsideShapes[0]->SetShapePosX(0.0);
				ChildInsideShapes[1]->SetShapePosX(0.0);
				ChildInsideShapes[2]->SetShapePosX(0.0);
			}

			ASCTShapeActor* PrtChildInsideActor = PrtChildInsideShapes[1]->GetShapeActor();
			//将新建子对象插入，并移除原对象
			ASCTShapeActor* ChildInsideActor0 = ChildInsideShapes[0]->SpawnShapeActor();
			ChildInsideActor0->AttachToActorOverride(PrtChildInsideActor, FAttachmentTransformRules::KeepRelativeTransform);
			PrtChildInsideShapes[1]->InsertChildInsideShape(nullptr, MakeShareable(ChildInsideShapes[0]));
			ChildInsideShapes[0]->SpawnActorsForSelected(SelfInsideProfileName, NullInsideProfileName);

			ASCTShapeActor* ChildInsideActor1 = ChildInsideShapes[1]->SpawnShapeActor();
			ChildInsideActor1->AttachToActorOverride(PrtChildInsideActor, FAttachmentTransformRules::KeepRelativeTransform);
			PrtChildInsideShapes[1]->InsertChildInsideShape(nullptr, MakeShareable(ChildInsideShapes[1]));
			ChildInsideShapes[1]->SetSelfShape(InChild);
			ChildInsideShapes[1]->SpawnActorsForSelected(SelfInsideProfileName, NullInsideProfileName);
			ChildInsideShapes[1]->UpdateActiveBoardRetract();

			ASCTShapeActor* ChildInsideActor2 = ChildInsideShapes[2]->SpawnShapeActor();
			ChildInsideActor2->AttachToActorOverride(PrtChildInsideActor, FAttachmentTransformRules::KeepRelativeTransform);
			PrtChildInsideShapes[1]->InsertChildInsideShape(nullptr, MakeShareable(ChildInsideShapes[2]));
			ChildInsideShapes[2]->SpawnActorsForSelected(SelfInsideProfileName, NullInsideProfileName);

			//删除父级型录的BoundingBoxActor
			PrtChildInsideShapes[1]->DestroyBoundingBoxActors();
		}
	}

	return SelectInside;
}

bool FUnitShape::FitChildShapeToInsideShape(const TSharedPtr<FSCTShape> &InChild, EInsideDirectType DirectType, FInsideShape* InInside)
{
	//1、判断当前空间是否可以插入子型录
	float InsideWidth = InInside->GetShapeWidth();
	float InsideDepth = InInside->GetShapeDepth();
	float InsideHeight = InInside->GetShapeHeight();
	if (InsideWidth < KINDA_SMALL_NUMBER || 
		InsideDepth < KINDA_SMALL_NUMBER || 
		InsideHeight < KINDA_SMALL_NUMBER)
	{
		return false;
	}

	bool bResult = true;
	if (DirectType == IDT_Vertical)
	{
		bResult = InChild->GetShapeHeight() <= InsideWidth;
		if (bResult)
		{
			bResult = InChild->SetShapeWidth(InsideHeight);
		}
		if (bResult)
		{
			bResult = InChild->SetShapeDepth(InsideDepth);
		}
	}
	else
	{
		bResult = InChild->GetShapeHeight() <= InsideHeight;
		if (bResult)
		{
			bResult = InChild->SetShapeWidth(InsideWidth);
		}
		if (bResult)
		{
			bResult = InChild->SetShapeDepth(InsideDepth);
		}
	}
	return bResult;
}

bool FUnitShape::ModifyInsidePosition(FInsideShape* InInside, const FVector& InDelta, bool bIgnoreZero, bool bContinuedOperator)
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

bool FUnitShape::ModifInsideDimention(FInsideShape* InInside, const FVector &InDelta, bool bPositive, bool bIgnoreZero, bool bContinuedOperator)
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

void FUnitShape::ClearModifyCacheStatus()
{
	SurplusDelta.Set(0.0, 0.0, 0.0);
	LoBIndex = -1;
	RoUIndex = -1;
}

bool FUnitShape::SetInsideWidth(FInsideShape* InInside, float InValue)
{
	//水平内胆无法设置W尺寸
	if (IDT_Horizontal == InInside->GetDirectType())
	{
		return false;
	}

	//0、父级内胆为空则不允许调整位置
	FInsideShape* ParentInside = InInside->GetParentInsideShape();
	if (!ParentInside)
	{
		return false;
	}

	//1、判断当前内胆的尺寸是否允许当前设置
	float CurMin = 0.0;  //获取相邻内胆的变化范围
	float CurMax = 0.0;
	float CurCur = InInside->GetInsideWidthRange(CurMin, CurMax);
	if (InValue < CurMin || InValue > CurMax)
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

	//获取左侧第一个尺寸不为0的线性邻接内胆
	int32 LoBIndex = CurIndex - 1;
	while (LoBIndex >= 0)
	{
		LoBInside = ChildInsides[LoBIndex];
		if (/*bIgnoreZero && */LoBInside->GetShapeWidth() < KINDA_SMALL_NUMBER)
		{
			LoBIndex -= 1;
			LoBInside = nullptr;
		}
		else
		{
			break;
		}
	}
	//获取相邻内胆的变化范围
	float LoBMin = 0.0;
	float LoBMax = 0.0;
	float LoBCur = 0.0;
	if (LoBInside.IsValid())
	{
		LoBCur = LoBInside->GetInsideWidthRange(LoBMin, LoBMax);
	}
	

	//获取右侧第一个尺寸不为0的线性邻接内胆
	int32 RoUIndex = CurIndex + 1;
	while (RoUIndex < ChildCount)
	{
		RoUInside = ChildInsides[RoUIndex];
		if (/*bIgnoreZero && */RoUInside->GetShapeWidth() < KINDA_SMALL_NUMBER)
		{
			RoUIndex += 1;
			RoUInside = nullptr;
		}
		else
		{
			break;
		}
	}
	//获取相邻内胆的变化范围
	float RoUMin = 0.0;
	float RoUMax = 0.0;
	float RoUCur = 0.0;
	if (!RoUInside.IsValid())
	{
		RoUCur = RoUInside->GetInsideWidthRange(RoUMin, RoUMax);
	}
	

	//宽度总偏移量及两侧偏移量
	float CurDelta = InValue - InInside->GetShapeWidth();
	float LoBDelta = CurDelta / 2.0;
	float RoUDelta = CurDelta / 2.0;

	//判断左右相邻内胆是否允许当前尺寸设置
	if (CurDelta > 0)  //尺寸增加
	{
		float LoBValid = LoBCur - LoBMin;
		float RoUValid = RoUCur - RoUMin;
		
		if (LoBValid + RoUValid < CurDelta)
		{
			return false;
		}
		else  //计算相邻两内胆分别的偏移量
		{
			if (LoBValid < LoBDelta)
			{
				LoBDelta = LoBValid;
				RoUDelta = CurDelta - LoBValid;
			}
			else if (RoUValid < RoUDelta)
			{
				RoUDelta = RoUValid;
				LoBDelta = CurDelta - RoUValid;
			}
		}
	} 
	else  //尺寸减小
	{
		float LoBValid = LoBCur - LoBMax;
		float RoUValid = RoUCur - RoUMax;

		if (LoBValid + RoUValid > CurDelta)
		{
			return false;
		}
		else  //计算相邻两内胆分别的偏移量
		{
			if (LoBValid > LoBDelta)
			{
				LoBDelta = LoBValid;
				RoUDelta = CurDelta - LoBValid;
			}
			else if (RoUValid > RoUDelta)
			{
				RoUDelta = RoUValid;
				LoBDelta = CurDelta - RoUValid;
			}
		}
	}

	//处理当前内胆的尺寸和位置
	CurInside->SetShapeWidth(InValue);
	CurInside->SetShapePosX(CurInside->GetShapePosX() - LoBDelta);
	//处理左侧相邻内胆的尺寸和位置
	for (int32 i = CurIndex - 1; i > LoBIndex; --i)
	{
		ChildInsides[i]->SetShapePosX(ChildInsides[i]->GetShapePosX() - LoBDelta);
	}
	if (LoBInside.IsValid())
	{
		LoBInside->SetShapeWidth(LoBInside->GetShapeWidth() - LoBDelta);
	}
	//处理右侧相邻内胆的尺寸和位置
	for (int32 i = CurIndex + 1; i < RoUIndex; ++i)
	{
		ChildInsides[i]->SetShapePosX(ChildInsides[i]->GetShapePosX() + RoUDelta);
	}
	if (RoUInside.IsValid())
	{
		RoUInside->SetShapePosX(RoUInside->GetShapePosX() + RoUDelta);
		RoUInside->SetShapeWidth(RoUInside->GetShapeWidth() - RoUDelta);
	}

	return true;
}
bool FUnitShape::SetInsideHeight(FInsideShape* InInside, float InValue)
{
	//垂直内胆无法设置H尺寸
	if (IDT_Vertical == InInside->GetDirectType())
	{
		return false;
	}

	//0、父级内胆为空则不允许调整位置
	FInsideShape* ParentInside = InInside->GetParentInsideShape();
	if (!ParentInside)
	{
		return false;
	}

	//1、判断当前内胆尺寸是否允许当前设置
	float CurMin = 0.0; //获取当前内胆的变化范围
	float CurMax = 0.0;
	float CurCur = InInside->GetInsideHeightRange(CurMin, CurMax);
	if (InValue > CurMax || InValue < CurMin)
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

	//获取下侧第一个尺寸不为0的线性邻接内胆
	int32 LoBIndex = CurIndex - 1;
	while (LoBIndex >= 0)
	{
		LoBInside = ChildInsides[LoBIndex];
		if (/*bIgnoreZero && */LoBInside->GetShapeHeight() < KINDA_SMALL_NUMBER)
		{
			LoBIndex -= 1;
			LoBInside = nullptr;
		}
		else
		{
			break;
		}
	}
	//获取相邻内胆的变化范围
	float LoBMin = 0.0;
	float LoBMax = 0.0;
	float LoBCur = 0.0;
	if (LoBInside.IsValid())
	{
		LoBCur = LoBInside->GetInsideHeightRange(LoBMin, LoBMax);
	}

	//获取上侧第一个尺寸不为0的线性邻接内胆
	int32 RoUIndex = CurIndex + 1;
	while (RoUIndex < ChildCount)
	{
		RoUInside = ChildInsides[RoUIndex];
		if (/*bIgnoreZero && */RoUInside->GetShapeHeight() < KINDA_SMALL_NUMBER)
		{
			RoUIndex += 1;
			RoUInside = nullptr;
		}
		else
		{
			break;
		}
	}
	//获取相邻内胆的变化范围
	float RoUMin = 0.0;
	float RoUMax = 0.0;
	float RoUCur = 0.0;
	if (RoUInside.IsValid())
	{
		RoUCur = RoUInside->GetInsideHeightRange(RoUMin, RoUMax);
	}

	//厚度总偏移量及两侧偏移量
	float CurDelta = InValue - InInside->GetShapeHeight();
	float LoBDelta = CurDelta / 2.0;
	float RoUDelta = CurDelta / 2.0;

	//判断左右相邻内胆是否允许当前尺寸设置
	if (CurDelta > 0)  //尺寸增加
	{
		float LoBValid = LoBCur - LoBMin;
		float RoUValid = RoUCur - RoUMin;

		if (LoBValid + RoUValid < CurDelta)
		{
			return false;
		}
		else  //计算相邻两内胆分别的偏移量
		{
			if (LoBValid < LoBDelta)
			{
				LoBDelta = LoBValid;
				RoUDelta = CurDelta - LoBValid;
			}
			else if (RoUValid < RoUDelta)
			{
				RoUDelta = RoUValid;
				LoBDelta = CurDelta - RoUValid;
			}
		}
	}
	else  //尺寸减小
	{
		float LoBValid = LoBCur - LoBMax;
		float RoUValid = RoUCur - RoUMax;

		if (LoBValid + RoUValid > CurDelta)
		{
			return false;
		}
		else  //计算相邻两内胆分别的偏移量
		{
			if (LoBValid > LoBDelta)
			{
				LoBDelta = LoBValid;
				RoUDelta = CurDelta - LoBValid;
			}
			else if (RoUValid > RoUDelta)
			{
				RoUDelta = RoUValid;
				LoBDelta = CurDelta - RoUValid;
			}
		}
	}

	//处理当前内胆的尺寸和位置
	CurInside->SetShapeHeight(InValue);
	CurInside->SetShapePosZ(CurInside->GetShapePosZ() - LoBDelta);
	//处理下侧相邻内胆的位置和尺寸
	for (int32 i = CurIndex - 1; i > LoBIndex; --i)
	{
		ChildInsides[i]->SetShapePosZ(ChildInsides[i]->GetShapePosZ() - LoBDelta);
	}
	if (LoBInside.IsValid())
	{
		LoBInside->SetShapeHeight(LoBInside->GetShapeHeight() - LoBDelta);
	}
	//处理上侧相邻内胆的位置和尺寸
	for (int32 i = CurIndex + 1; i < RoUIndex; ++i)
	{
		ChildInsides[i]->SetShapePosZ(ChildInsides[i]->GetShapePosZ() + RoUDelta);
	}
	if (RoUInside.IsValid())
	{
		RoUInside->SetShapePosZ(RoUInside->GetShapePosZ() + RoUDelta);
		RoUInside->SetShapeHeight(RoUInside->GetShapeHeight() - RoUDelta);
	}

	return true;
}

bool FUnitShape::SetInsidePosX(FInsideShape* InInside, float Invalue)
{
	//水平内胆无法设置X方向位置
	if (IDT_Horizontal == InInside->GetDirectType())
	{
		return false;
	}

	//内胆当前位置
	float CurPosX = InInside->GetShapePosX();
	
	//计算偏移量
	float DeltaValue = Invalue - CurPosX;
	if (DeltaValue < KINDA_SMALL_NUMBER)
	{
		return true;
	}
	FVector CurDelta(DeltaValue, 0.0, 0.0);

	//修改内胆位置
	return ModifyInsidePosition(InInside, CurDelta, true, false);
}
bool FUnitShape::SetInsidePosZ(FInsideShape* InInside, float Invalue)
{
	//垂直内胆无法设置Z方向位置
	if (IDT_Vertical == InInside->GetDirectType())
	{
		return false;
	}

	//内胆当前位置
	float CurPosZ = InInside->GetShapePosZ();

	//计算偏移量
	float DeltaValue = Invalue - CurPosZ;
	if (FMath::Abs(DeltaValue) < KINDA_SMALL_NUMBER)
	{
		return true;
	}
	FVector CurDelta(0.0, 0.0, DeltaValue);

	//修改内胆位置
	return ModifyInsidePosition(InInside, CurDelta, true, false);
}

bool FUnitShape::SetInsideCopyCount(FInsideShape* InInside, int32 InCount)
{
	//注意：输入的复制数量包含自身，即InCount = CopyShapes.Num() + 1
	//0、垂直内胆无法设置H尺寸
	if (IDT_Vertical == InInside->GetDirectType())
	{
		return false;
	}

	//1、当前只处理基础组件的复制
	TSharedPtr<FSCTShape> SelfShape = InInside->GetSelfShape();
	if (!SelfShape.IsValid() || ST_Base != SelfShape->GetShapeType())
	{
		return false;
	}

	//0、父级内胆为空则不允许调整位置
	FInsideShape* ParentInside = InInside->GetParentInsideShape();
	if (!ParentInside)
	{
		return false;
	}

	//2、复制数量是否改变
	int32 OldCopyCount = InInside->GetCurrentCopyCount();
	if (OldCopyCount == InCount)
	{
		return true;
	}

	//3、获取基础组件的高度变化范围和当前值
	TSharedPtr<FShapeAttribute> HeightAttri = SelfShape->GetShapeHeightAttri();
	TSharedPtr<FNumberRangeAttri> NumberAttri = StaticCastSharedPtr<FNumberRangeAttri>(HeightAttri);
	float MinHeight = NumberAttri->GetMinValue();
	float MaxHeight = NumberAttri->GetMaxValue();
	
	//4、计算单个组件的尺寸
	float OldInsideHeight = InInside->GetShapeHeight();
	float SingleHeight = OldInsideHeight / (InCount/* + 1*/);

	//5、需要修改当前内胆尺寸，以及相邻内胆的尺寸和位置
	if (MaxHeight < SingleHeight || MinHeight > SingleHeight)
	{
		//5.1、计算内胆的尺寸
		float NewInsideHeight = InCount < OldCopyCount ? MaxHeight * InCount : MinHeight * InCount;

		//5.2、获取当前内胆的第一个尺寸不为0的线性邻接内胆
		int32 CurIndex = ParentInside->GetChildInsideShapeIndex(InInside);
		TArray<TSharedPtr<FInsideShape>>& ChildInsides = ParentInside->GetChildInsideShapes();
		int32 ChildCount = ChildInsides.Num();
		TSharedPtr<FInsideShape> CurInside = ChildInsides[CurIndex];
		TSharedPtr<FInsideShape> LoBInside = nullptr;
		TSharedPtr<FInsideShape> RoUInside = nullptr;

		//获取下侧第一个尺寸不为0的线性邻接内胆
		int32 LoBIndex = CurIndex - 1;
		while (LoBIndex >= 0)
		{
			LoBInside = ChildInsides[LoBIndex];
			if (/*bIgnoreZero && */LoBInside->GetShapeHeight() < KINDA_SMALL_NUMBER)
			{
				LoBIndex -= 1;
				LoBInside = nullptr;
			}
			else
			{
				break;
			}
		}
		//获取相邻内胆的变化范围
		float LoBMin = 0.0;
		float LoBMax = 0.0;
		float LoBCur = 0.0; 
		if (LoBInside.IsValid())
		{
			LoBCur = LoBInside->GetInsideHeightRange(LoBMin, LoBMax);
		}

		//获取上侧第一个尺寸不为0的线性邻接内胆
		int32 RoUIndex = CurIndex + 1;
		while (RoUIndex < ChildCount)
		{
			RoUInside = ChildInsides[RoUIndex];
			if (/*bIgnoreZero && */RoUInside->GetShapeHeight() < KINDA_SMALL_NUMBER)
			{
				RoUIndex += 1;
				RoUInside = nullptr;
			}
			else
			{
				break;
			}
		}
		//获取相邻内胆的变化范围
		float RoUMin = 0.0;
		float RoUMax = 0.0;
		float RoUCur = 0.0;
		if (RoUInside.IsValid())
		{
			RoUCur = RoUInside->GetInsideHeightRange(RoUMin, RoUMax);
		}

		//厚度总偏移量及两侧偏移量
		float CurDelta = NewInsideHeight - InInside->GetShapeHeight();
		float LoBDelta = CurDelta / 2.0;
		float RoUDelta = CurDelta / 2.0;

		//判断左右相邻内胆是否允许当前尺寸设置
		if (CurDelta > 0)  //尺寸增加
		{
			float LoBValid = LoBCur - LoBMin;
			float RoUValid = RoUCur - RoUMin;

			if (LoBValid + RoUValid < CurDelta)
			{
				return false;
			}
			else  //计算相邻两内胆分别的偏移量
			{
				if (LoBValid < LoBDelta)
				{
					LoBDelta = LoBValid;
					RoUDelta = CurDelta - LoBValid;
				}
				else if (RoUValid < RoUDelta)
				{
					RoUDelta = RoUValid;
					LoBDelta = CurDelta - RoUValid;
				}
			}
		}
		else  //尺寸减小
		{
			float LoBValid = LoBCur - LoBMax;
			float RoUValid = RoUCur - RoUMax;

			if (LoBValid + RoUValid > CurDelta)
			{
				return false;
			}
			else  //计算相邻两内胆分别的偏移量
			{
				if (LoBValid > LoBDelta)
				{
					LoBDelta = LoBValid;
					RoUDelta = CurDelta - LoBValid;
				}
				else if (RoUValid > RoUDelta)
				{
					RoUDelta = RoUValid;
					LoBDelta = CurDelta - RoUValid;
				}
			}
		}

		//处理当前内胆的尺寸和位置
		CurInside->SetShapeHeight(NewInsideHeight);
		CurInside->SetShapePosZ(CurInside->GetShapePosZ() - LoBDelta);
		//处理下侧相邻内胆的位置和尺寸
		for (int32 i = CurIndex - 1; i > LoBIndex; --i)
		{
			ChildInsides[i]->SetShapePosZ(ChildInsides[i]->GetShapePosZ() - LoBDelta);
		}
		if (LoBInside.IsValid())
		{
			LoBInside->SetShapeHeight(LoBInside->GetShapeHeight() - LoBDelta);
		}
		//处理上侧相邻内胆的位置和尺寸
		for (int32 i = CurIndex + 1; i < RoUIndex; ++i)
		{
			ChildInsides[i]->SetShapePosZ(ChildInsides[i]->GetShapePosZ() + RoUDelta);
		}
		if (RoUInside.IsValid())
		{
			RoUInside->SetShapePosZ(RoUInside->GetShapePosZ() + RoUDelta);
			RoUInside->SetShapeHeight(RoUInside->GetShapeHeight() - RoUDelta);
		}
	}

	//设置子对象复制数量
	return InInside->SetCopyCount(InCount);
}

void FUnitShape::DeleteSpecialInsideShape(FInsideShape* InShape)
{
	//1、顶层内胆不允许删除
	if (InShape == TopInsideShape.Get())
	{
		return;
	}

	//2、内胆为空不允许删除
	if (InShape->IsInsideShapeEmpty())
	{
		return;
	}

	//3、设置当前内胆为空
	InShape->EmptyInsideShape();

	//4、合并相邻的线性空内胆
	MergeAdjacentEmptyInsides(InShape);
}

bool FUnitShape::IsTopInsideShapeEmpty()
{
	if (TopInsideShape.IsValid())
	{
		return TopInsideShape->IsInsideShapeEmpty();
	}
	return true;
}
void FUnitShape::EmptyTopInsideShape()
{
	if (TopInsideShape.IsValid())
	{
		TopInsideShape->EmptyInsideShape();
	}
}

void FUnitShape::SplitInsideShapeByHorizontal(TArray<FInsideShape*>& ChildInsideShapes, TSharedPtr<FSCTShape> InChild, FInsideShape* InInside)
{
	//1、当前内胆尺寸和位置
	float InsideWidth = InInside->GetShapeWidth();
	float InsideDepth = InInside->GetShapeDepth();
	float InsideHeight = InInside->GetShapeHeight();
	float InsidePosX = InInside->GetShapePosX();
	float InsidePosY = InInside->GetShapePosY();
	float InsidePosZ = InInside->GetShapePosZ();
	float AbsoluteZ = InInside->GetAbsolutePosZ();
	float DeltaValue = AbsoluteZ - InsidePosZ;

	//2、子型录的尺寸和位置
	float ChildWidth = InChild->GetShapeWidth();
	float ChildDepth = InChild->GetShapeDepth();
	float ChildHeight = InChild->GetShapeHeight();
	float ChildPosX = InChild->GetShapePosX();
	float ChildPosY = InChild->GetShapePosY();
	float ChildPosZ = InChild->GetShapePosZ() - DeltaValue;
	//基础组件尺寸+位置 > 当前内胆的尺寸+位置
	if (ChildPosZ + ChildHeight > InsidePosZ + InsideHeight)
	{
		ChildPosZ = InsidePosZ + (InsideHeight - ChildHeight) / 2.0;
	}

	//子型录的相对位置
	InChild->SetShapePosX(0.0);
	InChild->SetShapePosY(0.0);
	InChild->SetShapePosZ(0.0);

	//2、创建三个子内胆
	//沿Z轴正方向顺序编码0、1、2
	FInsideShape* NewInside0 = new FInsideShape(this);
	NewInside0->SetDirectType(IDT_Horizontal);
	NewInside0->SetShapePosX(InsidePosX);
	NewInside0->SetShapePosY(InsidePosY);
	NewInside0->SetShapePosZ(InsidePosZ);
	NewInside0->SetShapeWidth(InsideWidth);
	NewInside0->SetShapeDepth(InsideDepth);
	NewInside0->SetShapeHeight(ChildPosZ - InsidePosZ);

	FInsideShape* NewInside1 = new FInsideShape(this);
	NewInside1->SetDirectType(IDT_Horizontal);
	NewInside1->SetShapePosX(InsidePosX);
	NewInside1->SetShapePosY(InsidePosY);
	NewInside1->SetShapePosZ(ChildPosZ);
	NewInside1->SetShapeWidth(ChildWidth);
	NewInside1->SetShapeDepth(ChildDepth);
	NewInside1->SetShapeHeight(ChildHeight);

	FInsideShape* NewInside2 = new FInsideShape(this);
	NewInside2->SetDirectType(IDT_Horizontal);
	NewInside2->SetShapePosX(InsidePosX);
	NewInside2->SetShapePosY(InsidePosY);
	NewInside2->SetShapePosZ(ChildPosZ + ChildHeight);
	NewInside2->SetShapeWidth(InsideWidth);
	NewInside2->SetShapeDepth(InsideDepth);
	NewInside2->SetShapeHeight(InsidePosZ + InsideHeight - ChildHeight - ChildPosZ);

	//3、将当前空间分割为三个子空间
	ChildInsideShapes.Add(NewInside0);
	ChildInsideShapes.Add(NewInside1);
	ChildInsideShapes.Add(NewInside2);
}
void FUnitShape::SplitInsideShapeByVertical(TArray<FInsideShape*>& ChildInsideShapes, TSharedPtr<FSCTShape> InChild, FInsideShape* InInside)
{
	//1、当前内胆尺寸和位置
	float InsideWidth = InInside->GetShapeWidth();
	float InsideDepth = InInside->GetShapeDepth();
	float InsideHeight = InInside->GetShapeHeight();
	float InsidePosX = InInside->GetShapePosX();
	float InsidePosY = InInside->GetShapePosY();
	float InsidePosZ = InInside->GetShapePosZ();
	float AbsoluteX = InInside->GetAbsolutePosX();
	float DeltaValue = AbsoluteX - InsidePosX;

	//2、子型录的尺寸和位置
	float ChildWidth = InChild->GetShapeWidth();
	float ChildDepth = InChild->GetShapeDepth();
	float ChildHeight = InChild->GetShapeHeight();
	float ChildPosX = InChild->GetShapePosX() - DeltaValue;
	float ChildPosY = InChild->GetShapePosY();
	float ChildPosZ = InChild->GetShapePosZ();
	//基础组件尺寸+位置 > 当前内胆的尺寸+位置
	if (ST_Board == InChild->GetShapeType())
	{
		if (ChildPosX + ChildHeight > InsidePosX + InsideWidth)
		{
			ChildPosX = InsidePosX + (InsideWidth - ChildHeight) / 2.0;
		}
	} 
	else //基础组件
	{
		if (ChildPosX + ChildWidth > InsidePosX + InsideWidth)
		{
			ChildPosX = InsidePosX + (InsideWidth - ChildWidth) / 2.0;
		}
	}

	//子型录的相对位置
	InChild->SetShapePosX(ChildHeight);
	InChild->SetShapePosY(0.0);
	InChild->SetShapePosZ(0.0);

	//2、创建三个子内胆
	//沿X轴正方向顺序编码0、1、2
	FInsideShape* NewInside0 = new FInsideShape(this);
	NewInside0->SetDirectType(IDT_Vertical);
	NewInside0->SetShapePosX(InsidePosX);
	NewInside0->SetShapePosY(InsidePosY);
	NewInside0->SetShapePosZ(InsidePosZ);
	NewInside0->SetShapeWidth(ChildPosX - InsidePosX - ChildHeight);
	NewInside0->SetShapeDepth(InsideDepth);
	NewInside0->SetShapeHeight(InsideHeight);

	FInsideShape* NewInside1 = new FInsideShape(this);
	NewInside1->SetDirectType(IDT_Vertical);
	NewInside1->SetShapePosX(ChildPosX - ChildHeight);
	NewInside1->SetShapePosY(InsidePosY);
	NewInside1->SetShapePosZ(InsidePosZ);
	NewInside1->SetShapeWidth(ChildHeight);
	NewInside1->SetShapeDepth(ChildDepth);
	NewInside1->SetShapeHeight(ChildWidth);

	FInsideShape* NewInside2 = new FInsideShape(this);
	NewInside2->SetDirectType(IDT_Vertical);
	NewInside2->SetShapePosX(ChildPosX);
	NewInside2->SetShapePosY(InsidePosY);
	NewInside2->SetShapePosZ(InsidePosZ);
	NewInside2->SetShapeWidth(InsideWidth - ChildPosX + InsidePosX);
	NewInside2->SetShapeDepth(InsideDepth);
	NewInside2->SetShapeHeight(InsideHeight);

	//3、输出子内胆
	ChildInsideShapes.Add(NewInside0);
	ChildInsideShapes.Add(NewInside1);
	ChildInsideShapes.Add(NewInside2);
}

void FUnitShape::SplitInsideShapeByIncongruous(TArray<FInsideShape*>& ChildInsideShapes, int32 DirectType, FInsideShape* InInside)
{
	//1、当前内胆尺寸和位置
	float InsideWidth = InInside->GetShapeWidth();
	float InsideDepth = InInside->GetShapeDepth();
	float InsideHeight = InInside->GetShapeHeight();
	float InsidePosX = InInside->GetShapePosX();
	float InsidePosY = InInside->GetShapePosY();
	float InsidePosZ = InInside->GetShapePosZ();

	//2、创建新子级内胆
	FInsideShape* EmptyInside0 = new FInsideShape(this);//1、新建尺寸为0内胆
	FInsideShape* NewPrtInside = new FInsideShape(this);//2、新建父级内胆
	FInsideShape* EmptyInside1 = new FInsideShape(this);//3、新建尺寸为0内胆

	//3、设置内胆尺寸和位置
	if (IDT_Horizontal == DirectType)  //父级为水平
	{
		EmptyInside0->SetDirectType(IDT_Horizontal);
		EmptyInside0->SetShapePosX(InsidePosX);
		EmptyInside0->SetShapePosY(InsidePosY);
		EmptyInside0->SetShapePosZ(InsidePosZ);
		EmptyInside0->SetShapeWidth(InsideWidth);
		EmptyInside0->SetShapeDepth(InsideDepth);
		EmptyInside0->SetShapeHeight(0.0);

		NewPrtInside->SetDirectType(IDT_Horizontal);
		NewPrtInside->SetShapePosX(InsidePosX);
		NewPrtInside->SetShapePosY(InsidePosY);
		NewPrtInside->SetShapePosZ(InsidePosZ);
		NewPrtInside->SetShapeWidth(InsideWidth);
		NewPrtInside->SetShapeDepth(InsideDepth);
		NewPrtInside->SetShapeHeight(InsideHeight);

		EmptyInside1->SetDirectType(IDT_Horizontal);
		EmptyInside1->SetShapePosX(InsidePosX);
		EmptyInside1->SetShapePosY(InsidePosY);
		EmptyInside1->SetShapePosZ(InsidePosZ + InsideHeight);
		EmptyInside1->SetShapeWidth(InsideWidth);
		EmptyInside1->SetShapeDepth(InsideDepth);
		EmptyInside1->SetShapeHeight(0.0);
	} 
	else  //父级为垂直
	{
		EmptyInside0->SetDirectType(IDT_Vertical);
		EmptyInside0->SetShapePosX(InsidePosX);
		EmptyInside0->SetShapePosY(InsidePosY);
		EmptyInside0->SetShapePosZ(InsidePosZ);
		EmptyInside0->SetShapeWidth(0.0);
		EmptyInside0->SetShapeDepth(InsideDepth);
		EmptyInside0->SetShapeHeight(InsideHeight);

		NewPrtInside->SetDirectType(IDT_Vertical);
		NewPrtInside->SetShapePosX(InsidePosX);
		NewPrtInside->SetShapePosY(InsidePosY);
		NewPrtInside->SetShapePosZ(InsidePosZ);
		NewPrtInside->SetShapeWidth(InsideWidth);
		NewPrtInside->SetShapeDepth(InsideDepth);
		NewPrtInside->SetShapeHeight(InsideHeight);

		EmptyInside1->SetDirectType(IDT_Vertical);
		EmptyInside1->SetShapePosX(InsidePosX + InsideWidth);
		EmptyInside1->SetShapePosY(InsidePosY);
		EmptyInside1->SetShapePosZ(InsidePosZ);
		EmptyInside1->SetShapeWidth(0.0);
		EmptyInside1->SetShapeDepth(InsideDepth);
		EmptyInside1->SetShapeHeight(InsideHeight);
	}

	//4、输出子内胆
	ChildInsideShapes.Add(EmptyInside0);
	ChildInsideShapes.Add(NewPrtInside);
	ChildInsideShapes.Add(EmptyInside1);
}

void FUnitShape::MergeAdjacentEmptyInsides(FInsideShape* InInside)
{
	//1、父级内胆为空则无需进行合并
	FInsideShape* ParentInside = InInside->GetParentInsideShape();
	if (!ParentInside)
	{
		return;
	}

	//2、获取当前空间的邻接线性子内胆
	int32 CurIndex = ParentInside->GetChildInsideShapeIndex(InInside);
	TArray<TSharedPtr<FInsideShape>>& ChildInsides = ParentInside->GetChildInsideShapes();
	TSharedPtr<FInsideShape> CurInside = ChildInsides[CurIndex];
	TSharedPtr<FInsideShape> LoBInside = CurIndex == 0 ? nullptr : ChildInsides[CurIndex - 1];
	TSharedPtr<FInsideShape> RoUInside = CurIndex == ChildInsides.Num() -1 ? nullptr : ChildInsides[CurIndex + 1];

	//3、判断是否需要合并相邻线性空间
	int32 DirectType = CurInside->GetDirectType();
	if (DirectType == IDT_Horizontal)
	{
		if (LoBInside.IsValid() && LoBInside->IsInsideShapeEmpty())
		{
			LoBInside->SetShapeHeight(CurInside->GetShapeHeight() + LoBInside->GetShapeHeight());
			ParentInside->DeleteChildInsideShape(CurInside.Get());
			CurInside = LoBInside;
		}
		if (RoUInside.IsValid() && RoUInside->IsInsideShapeEmpty())
		{
			CurInside->SetShapeHeight(CurInside->GetShapeHeight() + RoUInside->GetShapeHeight());
			ParentInside->DeleteChildInsideShape(RoUInside.Get());
		}
	}
	else if (DirectType == IDT_Vertical)
	{
		if (LoBInside.IsValid() && LoBInside->IsInsideShapeEmpty())
		{
			LoBInside->SetShapeWidth(CurInside->GetShapeWidth() + LoBInside->GetShapeWidth());
			ParentInside->DeleteChildInsideShape(CurInside.Get());
			CurInside = LoBInside;
		}
		if (RoUInside.IsValid() && RoUInside->IsInsideShapeEmpty())
		{
			CurInside->SetShapeWidth(CurInside->GetShapeWidth() + RoUInside->GetShapeWidth());
			ParentInside->DeleteChildInsideShape(RoUInside.Get());
		}
	}
	else
	{
		check(false);
	}

	//4、判断是否需要递归合并父级线性空间
	if (ParentInside->GetChildInsideShapes().Num() == 1)
	{
		ParentInside->EmptyInsideShape();
		ParentInside->SpawnBoundingBoxActors(GetNullInsideProfileName());
		//ParentInside->UpdateBondingBoxProfileName();
		FInsideShape* PParentInside = ParentInside->GetParentInsideShape();
		if (PParentInside)
		{
			MergeAdjacentEmptyInsides(ParentInside);
		}
	}
}

