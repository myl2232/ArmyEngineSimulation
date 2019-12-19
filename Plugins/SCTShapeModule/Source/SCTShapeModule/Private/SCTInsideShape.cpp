#include "SCTInsideShape.h"
#include "SCTShapeBoxActor.h"
#include "SCTAttribute.h"
#include "SharedPointer.h"
#include "JsonObject.h"
#include "Math/UnrealMathUtility.h"
#include "SCTShapeActor.h"
#include "SCTShapeManager.h"
#include "SCTShape.h"
#include "SCTBoardShape.h"
#include "SCTUnitShape.h"
#include "Engine/CollisionProfile.h"
#include "SCTBaseShape.h"
#include "SCTSlidingDrawerShape.h"

FInsideShape::FInsideShape()
	:FSCTShape()
{
	UnitShapeData = nullptr;
	SetShapeType(ST_Inside);
	SetShapeName(TEXT("新内胆"));
}

FInsideShape::FInsideShape(FUnitShape* InUnitShape)
	:FSCTShape()
{
	UnitShapeData = InUnitShape;
	SetShapeType(ST_Inside);
	SetShapeName(TEXT("新内胆"));
	//修正尺寸最小值为0.0；
	TSharedPtr<FNumberRangeAttri> WidthRange = StaticCastSharedPtr<FNumberRangeAttri>(Width);
	WidthRange->SetMinValue(0.0);
	TSharedPtr<FNumberRangeAttri> DepthRange = StaticCastSharedPtr<FNumberRangeAttri>(Depth);
	DepthRange->SetMinValue(0.0);
	TSharedPtr<FNumberRangeAttri> HeightRange = StaticCastSharedPtr<FNumberRangeAttri>(Height);
	HeightRange->SetMinValue(0.0);
}

FInsideShape::~FInsideShape()
{
	UnitShapeData = nullptr;
}

void FInsideShape::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//解析型录基本信息
	FSCTShape::ParseFromJson(InJsonObject);

	//内胆方向
	int32 DirectInt = InJsonObject->GetIntegerField(TEXT("directType"));
	SetDirectType((EInsideDirectType)DirectInt);

	//板件复制相关信息
	bBoardCopyValid = InJsonObject->GetBoolField(TEXT("boardCopyValid"));
	//ShowThreshold = InJsonObject->GetNumberField(TEXT("showThreshold"));
	CopyThreshold = InJsonObject->GetNumberField(TEXT("copyThreshold"));

	//解析子级型录(顶层内胆)
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
			switch (ShapeType)
			{
			case ST_Board:
			case ST_Base:
			{
				if (0 == i)
				{
					SetSelfShape(CopyShape);
				}
				else
				{
					AddCopyShape(CopyShape);
				}
				break;
				break;
			}
			case ST_Inside:
			{
				TSharedPtr<FInsideShape> ChildInside = StaticCastSharedPtr<FInsideShape>(CopyShape);
				InsertChildInsideShape(nullptr, ChildInside);
				break;
			}
			default:
				break;
			}
			CopyShape->ParseAttributesFromJson(ChildObject);
		}
	}
}

void FInsideShape::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//型录类型和Id
	int32 CurrentType = InJsonObject->GetNumberField(TEXT("type"));
	int32 CurrentId = InJsonObject->GetNumberField(TEXT("id"));
	check(CurrentType == ShapeType && CurrentId == ShapeId);

	//更新型录基本信息
	FSCTShape::ParseAttributesFromJson(InJsonObject);

	//内胆方向
	int32 DirectInt = InJsonObject->GetIntegerField(TEXT("directType"));
	SetDirectType((EInsideDirectType)DirectInt);

	//更新子级型录信息
	const TArray<TSharedPtr<FJsonValue>>* ChildList = nullptr;
	InJsonObject->TryGetArrayField(TEXT("children"), ChildList);
	if (ChildList != nullptr && (*ChildList).Num() > 0)
	{
		for (int32 i=0; i<(*ChildList).Num(); ++i)
		{
			TSharedPtr<FJsonObject> ChildObject = (*ChildList)[i]->AsObject();
			int32 ShapeType = ChildObject->GetNumberField(TEXT("type"));
			int64 ShapeId = ChildObject->GetNumberField(TEXT("id"));
			TSharedPtr<FSCTShape> NewShape = FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ShapeType, ShapeId);
			TSharedPtr<FSCTShape> CopyShape = FSCTShapeManager::Get()->CopyShapeToNew(NewShape);
			switch (ShapeType)
			{
				case ST_Board:  //板件
				case ST_Base:  //基础组件
				case ST_SlidingDrawer:
				{
					if (0 == i)
					{
						SetSelfShape(CopyShape);
					}
					else
					{
						AddCopyShape(CopyShape);
					}
					break;
				}
				case ST_Inside:  //内胆
				{
					TSharedPtr<FInsideShape> ChildInside = StaticCastSharedPtr<FInsideShape>(CopyShape);
					InsertChildInsideShape(nullptr, ChildInside);
					break;
				}
				default:
					break;
			}
			CopyShape->ParseAttributesFromJson(ChildObject);
		}
	}

	//板件复制相关信息
	if (SelfShape.IsValid() && ST_Board == SelfShape->GetShapeType())
	{
		bBoardCopyValid = InJsonObject->GetBoolField(TEXT("boardCopyValid"));
		//ShowThreshold = InJsonObject->GetNumberField(TEXT("showThreshold"));
		CopyThreshold = InJsonObject->GetNumberField(TEXT("copyThreshold"));
	}
}

void FInsideShape::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseShapeFromJson(InJsonObject);
}

void FInsideShape::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseContentFromJson(InJsonObject);
}

void FInsideShape::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//1、保存型录基本信息
	FSCTShape::SaveToJson(JsonWriter);

	//内胆方向
	JsonWriter->WriteValue(TEXT("directType"), GetDirectType());

	//板件复制相关信息
	JsonWriter->WriteValue(TEXT("boardCopyValid"), bBoardCopyValid);
	//JsonWriter->WriteValue(TEXT("showThreshold"), ShowThreshold);
	JsonWriter->WriteValue(TEXT("copyThreshold"), CopyThreshold);

	//2、保存Children信息
	JsonWriter->WriteArrayStart(TEXT("children"));
	//2.1 SelfShape
	if (SelfShape.IsValid())
	{
		JsonWriter->WriteObjectStart();
		SelfShape->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//2.2 CopyShapes
	for (int32 i=0; i<CopyShapes.Num(); ++i)
	{
		JsonWriter->WriteObjectStart();
		CopyShapes[i]->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//2.3 ChildShpapes
	for (int32 i=0; i<ChildInsideShapes.Num(); ++i)
	{
		JsonWriter->WriteObjectStart();
		ChildInsideShapes[i]->SaveAttriToJson(JsonWriter);
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

void FInsideShape::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//1、型录基本信息
	FSCTShape::SaveAttriToJson(JsonWriter);

	//内胆方向
	JsonWriter->WriteValue(TEXT("directType"), GetDirectType());

	//板件复制相关信息
	if (SelfShape.IsValid() && ST_Board == SelfShape->GetShapeType())
	{
		JsonWriter->WriteValue(TEXT("boardCopyValid"), bBoardCopyValid);
		//JsonWriter->WriteValue(TEXT("showThreshold"), ShowThreshold);
		JsonWriter->WriteValue(TEXT("copyThreshold"), CopyThreshold);
	}

	//2、子型录信息
	TArray<TSharedPtr<FSCTShape>> ChildrenShapes = GetChildrenShapes();
	JsonWriter->WriteArrayStart(TEXT("children"));
	//2.1、SelfShape
	if (SelfShape.IsValid())
	{
		JsonWriter->WriteObjectStart();
		SelfShape->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//2.2、CopyShapes
	for (auto it = CopyShapes.CreateConstIterator(); it; ++it)
	{
		JsonWriter->WriteObjectStart();
		(*it)->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//2.3、ChildShapes
	for (auto it = ChildInsideShapes.CreateConstIterator(); it; ++it)
	{
		JsonWriter->WriteObjectStart();
		(*it)->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//End children
	JsonWriter->WriteArrayEnd();
}

void FInsideShape::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//基类
	FSCTShape::SaveShapeToJson(JsonWriter);
}

void FInsideShape::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveContentToJson(JsonWriter);
}

void FInsideShape::CopyTo(FSCTShape* OutShape)
{
	//基类信息拷贝
	FSCTShape::CopyTo(OutShape);

	//内胆信息拷贝
	FInsideShape* OutInsideShape = StaticCast<FInsideShape*>(OutShape);
	
	//内胆方向
	OutInsideShape->DirectType = DirectType;

	//板件复制相关信息
	OutInsideShape->bBoardCopyValid = bBoardCopyValid;
	//OutInsideShape->ShowThreshold = ShowThreshold;
	OutInsideShape->CopyThreshold = CopyThreshold;

	//关联的子型录
	if (SelfShape.IsValid())
	{
		TSharedPtr<FSCTShape> NewSelfShape = FSCTShapeManager::Get()->CopyShapeToNew(SelfShape);
		OutInsideShape->SetSelfShape(NewSelfShape);
	}
	
	//复制出的子型录集合
	for (int32 i=0; i<CopyShapes.Num(); ++i)
	{
		TSharedPtr<FSCTShape> NewCopyShape = FSCTShapeManager::Get()->CopyShapeToNew(CopyShapes[i]);
		OutInsideShape->AddCopyShape(NewCopyShape);
	}

	//父级对象
	OutInsideShape->ParentInsideShape = ParentInsideShape;
	//分割出的子对象
	for (int32 i=0; i<ChildInsideShapes.Num(); ++i)
	{
		TSharedPtr<FSCTShape> NewChildShape = FSCTShapeManager::Get()->CopyShapeToNew(ChildInsideShapes[i]);
		TSharedPtr<FInsideShape> NewChildInside = StaticCastSharedPtr<FInsideShape>(NewChildShape);
		OutInsideShape->InsertChildInsideShape(nullptr, NewChildInside);
	}
}

ASCTShapeActor* FInsideShape::SpawnShapeActor()
{
	//创建内胆自身的Actor
	ASCTShapeActor* NewInsideActor = FSCTShape::SpawnShapeActor();

	//创建子级型录的Actor
	if (SelfShape.IsValid())
	{
		ASCTShapeActor* SelfActor = SelfShape->SpawnShapeActor();
		SelfActor->AttachToActorOverride(NewInsideActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	for (int32 i = 0; i < CopyShapes.Num(); ++i)
	{
		ASCTShapeActor* CopyActor = CopyShapes[i]->SpawnShapeActor();
		CopyActor->AttachToActorOverride(NewInsideActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	for (int32 i = 0; i<ChildInsideShapes.Num(); ++i)
	{
		ASCTShapeActor* ChildActor = ChildInsideShapes[i]->SpawnShapeActor();
		ChildActor->AttachToActorOverride(NewInsideActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	if (SelfShape.IsValid())
	{
		ProcessBoardShapeCopies();
		ProcessBaseShapeCopies();
	}
	return NewInsideActor;
}

void FInsideShape::SpawnActorsForSelected(FName InSelectProfileName, FName InHoverProfileName)
{
	//当前内胆自身的Actor
	ASCTShapeActor* CurInsideActor = FSCTShape::GetShapeActor();
	//创建型录外包框
	if (CurInsideActor)
	{
		ASCTWireframeActor* WireFrameActor = SpawnWireFrameActor();
		WireFrameActor->UpdateActorDimension();
		WireFrameActor->SetActorHiddenInGame(true);
		WireFrameActor->AttachToActor(CurInsideActor, FAttachmentTransformRules::KeepRelativeTransform);
	}

	//创建顶层型录半透明包围盒（注意：只有叶子内胆才需要创建包围盒）
	if (ChildInsideShapes.Num() == 0 && CurInsideActor)
	{
		//创建顶层型录半透明包围盒
		ASCTBoundingBoxActor* BoundingBoxActor = SpawnBoundingBoxActor();
		if (SelfShape.IsValid() && !InSelectProfileName.IsNone())
		{
			BoundingBoxActor->SetCollisionProfileName(InSelectProfileName);
		}
		else if (!SelfShape.IsValid() && !InHoverProfileName.IsNone())
		{
			BoundingBoxActor->SetCollisionProfileName(InHoverProfileName);
		}
		BoundingBoxActor->SetRegionAlpha(0.0f);
		BoundingBoxActor->UpdateActorDimension();
		BoundingBoxActor->AttachToActor(CurInsideActor, FAttachmentTransformRules::KeepRelativeTransform);
	}

	//处理子级内胆
	for (int32 i = 0; i < ChildInsideShapes.Num(); ++i)
	{
		ChildInsideShapes[i]->SpawnActorsForSelected(InSelectProfileName, InHoverProfileName);
	}
}

void FInsideShape::SetCollisionProfileName(FName InProfileName)
{
	if (SelfShape.IsValid())
	{
		SelfShape->SetCollisionProfileName(InProfileName);
	}
	for (int32 i=0; i<CopyShapes.Num(); ++i)
	{
		CopyShapes[i]->SetCollisionProfileName(InProfileName);
	}
	for (int32 i=0; i<ChildInsideShapes.Num(); ++i)
	{
		ChildInsideShapes[i]->SetCollisionProfileName(InProfileName);
	}
}

void FInsideShape::GetResourceUrls(TArray<FString> &OutResourceUrls)
{
	//Self
	if (SelfShape.IsValid())
	{
		SelfShape->GetResourceUrls(OutResourceUrls);
	}
	//Children
	for (int32 i = 0; i < ChildInsideShapes.Num(); ++i)
	{
		ChildInsideShapes[i]->GetResourceUrls(OutResourceUrls);
	}
}

void FInsideShape::GetFileCachePaths(TArray<FString> &OutFileCachePaths)
{
	//Self
	if (SelfShape.IsValid())
	{
		SelfShape->GetFileCachePaths(OutFileCachePaths);
	}
	//Children
	for (int32 i = 0; i < ChildInsideShapes.Num(); ++i)
	{
		ChildInsideShapes[i]->GetFileCachePaths(OutFileCachePaths);
	}
}

void FInsideShape::HiddenInsideShapeActors(bool bHidden)
{
	if (SelfShape.IsValid())
	{
		if (SelfShape->GetShapeType() == ST_Board)
		{
			ASCTShapeActor* CurBoardActor = SelfShape->GetShapeActor();
			if (CurBoardActor)
			{
				CurBoardActor->SetActorHiddenInGame(bHidden);
			}
		} 
		else if (SelfShape->GetShapeType() == ST_SlidingDrawer)
		{
			TSharedPtr<FSlidingDrawerShape> CurDrawerShape = StaticCastSharedPtr<FSlidingDrawerShape>(SelfShape);
			CurDrawerShape->HiddenDrawerShapeActors(bHidden);
		}
		else if (SelfShape->GetShapeType() == ST_Base)
		{
			TSharedPtr<FBaseShape> CurBaseShape = StaticCastSharedPtr<FBaseShape>(SelfShape);
			CurBaseShape->HiddenBaseShapeActors(bHidden);
		}
	}
	for (int32 i=0; i<CopyShapes.Num(); ++i)
	{
		if (CopyShapes[i]->GetShapeType() == ST_Board)
		{
			ASCTShapeActor* CurBoardActor = CopyShapes[i]->GetShapeActor();
			if (CurBoardActor)
			{
				CurBoardActor->SetActorHiddenInGame(bHidden);
			}
		}
		else if (CopyShapes[i]->GetShapeType() == ST_SlidingDrawer)
		{
			TSharedPtr<FSlidingDrawerShape> CurDrawerShape = StaticCastSharedPtr<FSlidingDrawerShape>(CopyShapes[i]);
			CurDrawerShape->HiddenDrawerShapeActors(bHidden);
		}
		else if (SelfShape->GetShapeType() == ST_Base)
		{
			TSharedPtr<FBaseShape> CurBaseShape = StaticCastSharedPtr<FBaseShape>(CopyShapes[i]);
			CurBaseShape->HiddenBaseShapeActors(bHidden);
		}
	}
	for (int32 i=0; i<ChildInsideShapes.Num(); ++i)
	{
		ChildInsideShapes[i]->HiddenInsideShapeActors(bHidden);
	}
}

bool FInsideShape::SetShapePosX(float InValue)
{
	bool bResult = true;
	FString StrValue = FString::Printf(TEXT("%f"), InValue);
	if (PositionX->GetAttributeStr() != StrValue)
	{
		bResult = PositionX->SetAttributeValue(StrValue);
		//若Actor已生成，同时更新Actor
		if (bResult)
		{
			UpdateAssociatedActors(2);
			//UpdateChileShapes(0);
		}
	}
	return bResult;
}

bool FInsideShape::SetShapePosY(float InValue)
{
	bool bResult = true;
	FString StrValue = FString::Printf(TEXT("%f"), InValue);
	if (PositionY->GetAttributeStr() != StrValue)
	{
		bResult = PositionY->SetAttributeValue(StrValue);
		//若Actor已生成，同时更新Actor
		if (bResult)
		{
			UpdateAssociatedActors(2);
			//UpdateChileShapes(0);
		}
	}
	return bResult;
}

bool FInsideShape::SetShapePosZ(float InValue)
{
	bool bResult = true;
	FString StrValue = FString::Printf(TEXT("%f"), InValue);
	if (PositionZ->GetAttributeStr() != StrValue)
	{
		bResult = PositionZ->SetAttributeValue(StrValue);
		//若Actor已生成，同时更新Actor
		if (bResult)
		{
			UpdateAssociatedActors(2);
			//UpdateChileShapes(0);
		}
	}
	return bResult;
}

bool FInsideShape::SetShapeWidth(float InValue)
{
	if (IsInsideShapeEmpty())  //空内胆
	{
		return FSCTShape::SetShapeWidth(InValue);
	}
	else if (SelfShape.IsValid())
	{
		int32 ShapeType = SelfShape->GetShapeType();
		if (ST_Board == ShapeType)  //板件内胆
		{
			return SetShapeWidthWithBoard(InValue);
		}
		else if (ST_Base == ShapeType)  //组件内胆
		{
			return SetShapeWidthWithBase(InValue);
		}
	}
	else  //嵌套子内胆
	{
		return SetShapeWidthWithChildren(InValue);
	}
	return false;
}

bool FInsideShape::SetShapeDepth(float InValue)
{
	if (IsInsideShapeEmpty())  //空内胆
	{
		return FSCTShape::SetShapeDepth(InValue);
	}
	else if (SelfShape.IsValid())
	{
		int32 ShapeType = SelfShape->GetShapeType();
		if (ST_Board == ShapeType)  //板件内胆
		{
			return SetShapeDepthWithBoard(InValue);
		}
		else if (ST_Base == ShapeType)  //组件内胆
		{
			return SetShapeDepthWithBase(InValue);
		}
	}
	else  //嵌套子内胆
	{
		return SetShapeDepthWithChildren(InValue);
	}
	return false;
}

bool FInsideShape::SetShapeHeight(float InValue)
{
	if (IsInsideShapeEmpty())  //空内胆
	{
		return FSCTShape::SetShapeHeight(InValue);
	}
	else if (SelfShape.IsValid())
	{
		int32 ShapeType = SelfShape->GetShapeType();
		if (ST_Board == ShapeType)  //板件内胆
		{
			return SetShapeHeightWithBoard(InValue);
		}
		else if (ST_Base == ShapeType)  //组件内胆
		{
			return SetShapeHeightWithBase(InValue);
		}
	}
	else  //嵌套子内胆
	{
		return SetShapeHeightWithChildren(InValue);
	}
	return false;
}

float FInsideShape::GetInsideWidthRange(float& MinValue, float& MaxValue)
{
	float CurWidth = GetShapeWidth();
	if (IsInsideShapeEmpty())  //空内胆
	{
		TSharedPtr<FShapeAttribute> WidthAttri = GetShapeWidthAttri();
		if (SAT_NumberRange == WidthAttri->GetAttributeType())
		{
			TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(WidthAttri);
			MinValue = NumberRangeAttri->GetMinValue();
			MaxValue = NumberRangeAttri->GetMaxValue();
		}
		else
		{
			MinValue = CurWidth;
			MaxValue = CurWidth;
		}
	}
	else if (SelfShape.IsValid()) //包含板件或组件
	{
		TSharedPtr<FShapeAttribute> ShapeAttri = nullptr;
		if (IDT_Horizontal == DirectType)
		{
			ShapeAttri = SelfShape->GetShapeWidthAttri();
		}
		else if (IDT_Vertical == DirectType)
		{
			ShapeAttri = SelfShape->GetShapeHeightAttri();
		}

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
		if (IDT_Vertical == DirectType)
		{
			if (ST_Board == SelfShape->GetShapeType())
			{
				if (bBoardCopyValid) //竖板且开启复制
				{
					TSharedPtr<FNumberSelectAttri> NumberSelectAttri = StaticCastSharedPtr<FNumberSelectAttri>(ShapeAttri);
					MinValue = NumberSelectAttri->GetNumberValue();;  //板件厚度
					MaxValue = 10000.0;
				}
			}
		}
	}
	else  //嵌套子内胆
	{
		if (IDT_Horizontal == DirectType)
		{
			MinValue = 0.0;
			MaxValue = 0.0;
		}
		else
		{
			MinValue = 0.0;
			MaxValue = 10000.0;
		}
		
		for (int32 i=0; i<ChildInsideShapes.Num(); ++i)
		{
			float ChildMin = 0.0;
			float ChildMax = 10000.0;
			ChildInsideShapes[i]->GetInsideWidthRange(ChildMin, ChildMax);
			//当前内胆为横向，则取并集
			if (IDT_Horizontal == DirectType)
			{
				MinValue += ChildMin;
				MaxValue += ChildMax;
			} 
			else //当前内胆为纵向，则取交集
			{
				if (MinValue < ChildMin)
				{
					MinValue = ChildMin;
				}
				if (MaxValue > ChildMax)
				{
					MaxValue = ChildMax;
				}
			}
		}
	}
	return CurWidth;
}

float FInsideShape::GetInsideDepthRange(float& MinValue, float& MaxValue)
{
	float CurDepth = GetShapeDepth();
	if (IsInsideShapeEmpty())  //空内胆
	{
		TSharedPtr<FShapeAttribute> DepthAttri = GetShapeDepthAttri();
		if (SAT_NumberRange == DepthAttri->GetAttributeType())
		{
			TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(DepthAttri);
			MinValue = NumberRangeAttri->GetMinValue();
			MaxValue = NumberRangeAttri->GetMaxValue();
		}
		else
		{
			MinValue = CurDepth;
			MaxValue = CurDepth;
		}
	}
	else if (SelfShape.IsValid()) //包含板件或组件
	{
		TSharedPtr<FShapeAttribute> DepthAttri = SelfShape->GetShapeDepthAttri();
		if (SAT_NumberRange == DepthAttri->GetAttributeType())
		{
			TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(DepthAttri);
			MinValue = NumberRangeAttri->GetMinValue();
			MaxValue = NumberRangeAttri->GetMaxValue();
		}
		else
		{
			MinValue = CurDepth;
			MaxValue = CurDepth;
		}
	}
	else  //嵌套子内胆
	{
		//取交集
		MinValue = 0.0;
		MaxValue = 10000.0;
		for (int32 i = 0; i < ChildInsideShapes.Num(); ++i)
		{
			float ChildMin = 0.0;
			float ChildMax = 10000.0;
			ChildInsideShapes[i]->GetInsideDepthRange(ChildMin, ChildMax);
			//取子内胆深度的交集
			if (MinValue < ChildMin)
			{
				MinValue = ChildMin;
			}
			if (MaxValue > ChildMax)
			{
				MaxValue = ChildMax;
			}
		}
	}
	return CurDepth;
}

float FInsideShape::GetInsideHeightRange(float& MinValue, float& MaxValue)
{
	float CurHeight = GetShapeHeight();
	if (IsInsideShapeEmpty())  //空内胆
	{
		TSharedPtr<FShapeAttribute> HeightAttri = GetShapeHeightAttri();
		if (SAT_NumberRange == HeightAttri->GetAttributeType())
		{
			TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(HeightAttri);
			MinValue = NumberRangeAttri->GetMinValue();
			MaxValue = NumberRangeAttri->GetMaxValue();
		}
		else
		{
			MinValue = CurHeight;
			MaxValue = CurHeight;
		}
	}
	else if (SelfShape.IsValid()) //包含板件或组件
	{
		TSharedPtr<FShapeAttribute> ShapeAttri = nullptr;
		if (IDT_Horizontal == DirectType)
		{
			ShapeAttri = SelfShape->GetShapeHeightAttri();
		}
		else if (IDT_Vertical == DirectType)
		{
			ShapeAttri = SelfShape->GetShapeWidthAttri();
		}

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
		if (IDT_Horizontal == DirectType)
		{
			if (ST_Board == SelfShape->GetShapeType())
			{
				if (bBoardCopyValid) //横板且开启复制
				{
					TSharedPtr<FNumberSelectAttri> NumberSelectAttri = StaticCastSharedPtr<FNumberSelectAttri>(ShapeAttri);
					MinValue = NumberSelectAttri->GetNumberValue();;  //板件厚度
					MaxValue = 10000.0;
				}
			}
			else //基础组件
			{
				//MaxValue = 10000.0;
				int32 CopyCount = CopyShapes.Num();
				MinValue += MinValue * CopyCount;
				MaxValue += MaxValue * CopyCount;
			}
		}
	}
	else  //嵌套子内胆
	{
		MinValue = 0.0;
		MaxValue = 10000.0;
		for (int32 i = 0; i < ChildInsideShapes.Num(); ++i)
		{
			float ChildMin = 0.0;
			float ChildMax = 10000.0;
			ChildInsideShapes[i]->GetInsideHeightRange(ChildMin, ChildMax);
			//当前内胆为纵向，则需要分别处理固定值和范围变化值
			if (IDT_Vertical == DirectType)
			{
				//处理固定值情况：去并集
				MinValue += ChildMin;
				MaxValue += ChildMax;
			} 
			else //当前内胆为横向，则取交集
			{
				if (MinValue < ChildMin)
				{
					MinValue = ChildMin;
				}
				if (MaxValue > ChildMax)
				{
					MaxValue = ChildMax;
				}
			}
		}
	}
	return CurHeight;
}

float FInsideShape::GetAbsolutePosX()
{
	float ParentPosX = 0.0;
	if (ParentInsideShape)
	{
		ParentPosX = ParentInsideShape->GetAbsolutePosX();
	}
	return FSCTShape::GetShapePosX() + ParentPosX;
}

float FInsideShape::GetAbsolutePosY()
{
	float ParentPosY = 0.0;
	if (ParentInsideShape)
	{
		ParentPosY = ParentInsideShape->GetAbsolutePosY();
	}
	return FSCTShape::GetShapePosY() + ParentPosY;
}

float FInsideShape::GetAbsolutePosZ()
{
	float ParentPosZ = 0.0;
	if (ParentInsideShape)
	{
		ParentPosZ = ParentInsideShape->GetAbsolutePosZ();
	}
	return FSCTShape::GetShapePosZ() + ParentPosZ;
}

void FInsideShape::SpawnBoundingBoxActors(FName InProfileName)
{
	//创建顶层型录半透明包围盒
	if (!BoundingBoxActor)
	{
		ASCTBoundingBoxActor* BoundingBoxActor = SpawnBoundingBoxActor();
		BoundingBoxActor->SetCollisionProfileName(InProfileName);
		BoundingBoxActor->SetRegionAlpha(0.0f);
		BoundingBoxActor->UpdateActorDimension();
		BoundingBoxActor->AttachToActor(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void FInsideShape::DestroyBoundingBoxActors()
{
	if (BoundingBoxActor)
	{
		BoundingBoxActor->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		BoundingBoxActor->Destroy();
		BoundingBoxActor = nullptr;
	}
}

bool FInsideShape::SetBoardActiveRetract(bool bActive)
{
	float ActiveRetractValue = 0.0;
	if (bActive)
	{
		ActiveRetractValue = UnitShapeData->GetActiveRetractValue();
	}

	if (SelfShape.IsValid())
	{
		if (SelfShape->GetShapeType() == ST_Board)
		{
			TSharedPtr<FBoardShape> CurBoard = StaticCastSharedPtr<FBoardShape>(SelfShape);
			CurBoard->SetBoardActive(bActive, ActiveRetractValue);
		}
	}
	for (auto CopyShape : CopyShapes)
	{
		if (CopyShape->GetShapeType() == ST_Board)
		{
			TSharedPtr<FBoardShape> CurBoard = StaticCastSharedPtr<FBoardShape>(CopyShape);
			CurBoard->SetBoardActive(bActive, ActiveRetractValue);
		}
	}
	return true;
}

void FInsideShape::UpdateActiveBoardRetract()
{
	float ActiveRetractValue = UnitShapeData->GetActiveRetractValue();
	if (SelfShape.IsValid())
	{
		if (SelfShape->GetShapeType() == ST_Board)
		{
			TSharedPtr<FBoardShape> CurBoard = StaticCastSharedPtr<FBoardShape>(SelfShape);
			if (CurBoard->IsBoardActive())
			{
				CurBoard->SetActiveRetractValue(ActiveRetractValue);
			}
		}
	}
	for (auto CopyShape : CopyShapes)
	{
		if (CopyShape->GetShapeType() == ST_Board)
		{
			TSharedPtr<FBoardShape> CurBoard = StaticCastSharedPtr<FBoardShape>(CopyShape);
			if (CurBoard->IsBoardActive())
			{
				CurBoard->SetActiveRetractValue(ActiveRetractValue);
			}
		}
	}
	for (auto ChildInside : ChildInsideShapes)
	{
		ChildInside->UpdateActiveBoardRetract();
	}
}

FUnitShape* FInsideShape::GetUnitShapeData()
{
	return UnitShapeData;
}
void FInsideShape::SetUnitShapeData(FUnitShape* InUnitShape)
{
	UnitShapeData = InUnitShape;
	for (int32 i = 0; i < ChildInsideShapes.Num(); ++i)
	{
		ChildInsideShapes[i]->SetUnitShapeData(UnitShapeData);
	}
}

bool FInsideShape::IsInsideShapeEmpty() const
{
	return (!SelfShape.IsValid() && ChildInsideShapes.Num() == 0);
}

void FInsideShape::EmptyInsideShape()
{
	if (SelfShape.IsValid())
	{
		RemoveChildShape(SelfShape);
		SelfShape = nullptr;
		//设置BoundingBox的profileName
		BoundingBoxActor->SetCollisionProfileName("BlockBoundingBox");
	}
	for (int32 i=0; i<CopyShapes.Num(); ++i)
	{
		RemoveChildShape(CopyShapes[i]);
	}
	CopyShapes.Empty();
	for (int32 i =0; i<ChildInsideShapes.Num(); ++i)
	{
		RemoveChildShape(ChildInsideShapes[i]);
	}
	ChildInsideShapes.Empty();

}

const TSharedPtr<FSCTShape>& FInsideShape::GetSelfShape() const
{
	return SelfShape;
}

void FInsideShape::SetSelfShape(const TSharedPtr<FSCTShape> &InShape)
{
	//设置父子关系
	SelfShape = InShape;
	SelfShape->SetParentShape(this);
	AddChildShape(SelfShape);
	//设置内胆名称
	SetShapeName(InShape->GetShapeName());
	//将子型录Actor挂接到内胆的Actor上
	ASCTShapeActor* SelfActor = SelfShape->GetShapeActor();
	if (SelfActor)
	{
		SelfActor->AttachToActorOverride(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

bool FInsideShape::UpdateSelfShape()
{
	float InsideWidth = GetShapeWidth();
	float InsideDepth = GetShapeDepth();
	float InsideHeight = GetShapeHeight();

	bool bResult = true;
	if (bResult)
	{
		bResult = SelfShape->SetShapeWidth(InsideWidth);
	}
	if (bResult)
	{
		bResult = SelfShape->SetShapeDepth(InsideDepth);
	}
	if (bResult)
	{
		bResult = SelfShape->SetShapeHeight(InsideHeight);
	}

	return bResult;
}

void FInsideShape::AddCopyShape(const TSharedPtr<FSCTShape> &InShape)
{
	InShape->SetParentShape(this);
	CopyShapes.Add(InShape);
	AddChildShape(InShape);
	//将子型录Actor挂接到内胆的Actor上
	ASCTShapeActor* CopyActor = InShape->GetShapeActor();
	if (CopyActor)
	{
		CopyActor->AttachToActorOverride(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void FInsideShape::RemoveCopyShapeAt(int32 InIndex)
{
	TSharedPtr<FSCTShape> CurCopyShape = CopyShapes[InIndex];
	RemoveChildShape(CurCopyShape);
	CopyShapes.RemoveAt(InIndex);
}

void FInsideShape::ClearAllCopyShapes()
{
	for (int32 i = 0; i<CopyShapes.Num(); ++i)
	{
		RemoveChildShape(CopyShapes[i]);
	}
	CopyShapes.Empty();
}

bool FInsideShape::SetCopyCount(int32 InCount)
{
	//注意：输入的复制数量包含自身，即InCount = CopyShapes.Num() + 1
	if (CopyShapes.Num() + 1 == InCount)
	{
		return true;
	}

	//1、当前只处理基础组件的复制
	if (ST_Base != SelfShape->GetShapeType())
	{
		return false;
	}

	//2、获取基础组件的高度变化范围和当前值
	TSharedPtr<FShapeAttribute> HeightAttri = SelfShape->GetShapeHeightAttri();
	TSharedPtr<FNumberRangeAttri> NumberAttri = StaticCastSharedPtr<FNumberRangeAttri>(HeightAttri);
	float MinHeight = NumberAttri->GetMinValue();
	float MaxHeight = NumberAttri->GetMaxValue();
	float BaseHeight = NumberAttri->GetNumberValue();

	//3、当前内胆尺寸
	float InsideValue = GetShapeHeight();

	//4、计算单个组件的尺寸
	float SingleHeight = InsideValue / (InCount/* + 1*/);

	//5、判断当前复制数量是否有效
	if (MaxHeight >= SingleHeight && MinHeight <= SingleHeight)
	{
		//根据当前内胆尺寸和组件总体厚度增、删复制组件
		int32 CopyCount = CopyShapes.Num();
		if (CopyCount < InCount - 1)
		{
			for (int32 i = CopyCount; i < InCount - 1; ++i)
			{
				TSharedPtr<FSCTShape> NewBase = FSCTShapeManager::Get()->CopyShapeToNew(SelfShape);
				ASCTShapeActor* NewActor = NewBase->SpawnShapeActor();
				NewActor->AttachToActorOverride(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
				AddCopyShape(NewBase);
			}
		}
		else if (CopyCount > InCount - 1)
		{
			for (int32 i = CopyCount; i > InCount - 1; --i)
			{
				RemoveCopyShapeAt(i - 1);
			}
		}

		//处理所有基础组件(SelfShape和CopyShapes)的尺寸和位置
		SelfShape->SetShapeHeight(SingleHeight);
		for (int32 i = 0; i < CopyShapes.Num(); ++i)
		{
			CopyShapes[i]->SetShapeHeight(SingleHeight);
			CopyShapes[i]->SetShapePosZ(SingleHeight * (i + 1));
		}
	}
	else
	{
		return false;
	}

	return true;
}

TArray<int32> FInsideShape::GetValidCopyCounts()
{
	TSharedPtr<FShapeAttribute> HeightAttri = SelfShape->GetShapeHeightAttri();
	TSharedPtr<FNumberRangeAttri> NumberRange = StaticCastSharedPtr<FNumberRangeAttri>(HeightAttri);
	float MinValue = NumberRange->GetMinValue();
	float MaxValue = NumberRange->GetMaxValue();
	float CurValue = NumberRange->GetNumberValue();

	float InsideHeight = GetShapeHeight();
	int32 MinCount = InsideHeight / MaxValue;
	int32 MaxCount = InsideHeight / MinValue;

	TArray<int32> ValidCounts;
	for (int32 i = MinCount; i<=MaxCount; ++i)
	{
		if (MinValue * i <= InsideHeight && InsideHeight <= MaxValue * i)
		{
			ValidCounts.Add(i);
		}
	}
	return MoveTemp(ValidCounts);
}

bool FInsideShape::SetBoardCopyThreshold(float InValue)
{
	if (0/*SelfShape->GetShapeHeight()*/ > InValue)
	{
		return false;
	}
	CopyThreshold = InValue;
	return ProcessBoardShapeCopies();
}

bool FInsideShape::SetBoardCopyValid(bool bValid)
{
	if (bBoardCopyValid == bValid)
	{
		return true;
	}
	bBoardCopyValid = bValid;
	
	if (bBoardCopyValid) //从板件不可复制状态转换为可复制状态
	{
		if (IDT_Horizontal == DirectType) //横板
		{
			float CurInsideHeight = GetShapeHeight();
			return SetShapeHeight(CurInsideHeight);
		} 
		else //竖板
		{
			float CurInsideWidth = GetShapeWidth();
			return SetShapeWidth(CurInsideWidth);
		}
	}
	else  //从板件复制状态转换未不可复制状态
	{
		int32 CurIndex = ParentInsideShape->GetChildInsideShapeIndex(this);
		TArray<TSharedPtr<FInsideShape>>& ChildInsides = ParentInsideShape->GetChildInsideShapes();
		int32 ChildCount = ChildInsides.Num();
		if (CurIndex < 1 || CurIndex > ChildCount - 2)
		{
			check(false);
			return false;
		}
		TSharedPtr<FInsideShape> CurInside = ChildInsides[CurIndex];
		TSharedPtr<FInsideShape> LoBInside = ChildInsides[CurIndex - 1];
		TSharedPtr<FInsideShape> RoUInside = ChildInsides[CurIndex + 1];

		check(ST_Board == SelfShape->GetShapeType());
		float BoardHeight = SelfShape->GetShapeHeight();
		if (IDT_Horizontal == DirectType)  //横板
		{
			//保留SelfShape，移除所有复制Shapes
			ClearAllCopyShapes();
			ASCTShapeActor* SelfActor = SelfShape->GetShapeActor();
			if (!SelfActor)//生成SelfShape的Actor
			{
				SelfActor = SelfShape->SpawnShapeActor();
				SelfShape->SetCollisionProfileName("BlockAllExceptAxis");
				SelfActor->AttachToActorOverride(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
			}
			SelfShape->SetShapePosZ(0.0);

			//更新当前内胆的尺寸和位置，以及上下相邻内胆的尺寸和位置
			float InsidePosZ = GetShapePosZ();
			float InsideHeight = GetShapeHeight();
			float DeltaWidth = (InsideHeight - BoardHeight) / 2.0;
			CurInside->SetShapePosZ(InsidePosZ + DeltaWidth);
			CurInside->SetShapeHeight(BoardHeight);
			LoBInside->SetShapeHeight(LoBInside->GetShapeHeight() + DeltaWidth);
			RoUInside->SetShapePosZ(InsidePosZ + DeltaWidth + BoardHeight);
			RoUInside->SetShapeHeight(RoUInside->GetShapeHeight() + DeltaWidth);

		} 
		else //竖板
		{
			//保留SelfShape，移除所有复制Shapes
			ClearAllCopyShapes();
			ASCTShapeActor* SelfActor = SelfShape->GetShapeActor();
			if (!SelfActor)//生成SelfShape的Actor
			{
				ASCTShapeActor* SelfActor = SelfShape->SpawnShapeActor();
				SelfShape->SetCollisionProfileName("BlockAllExceptAxis");
				SelfActor->AttachToActorOverride(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
			}
			SelfShape->SetShapePosX(BoardHeight);

			//更新当前内胆的尺寸和位置，以及上下相邻内胆的尺寸和位置
			float InsideWidth = GetShapeWidth();
			float InsidePosX = GetShapePosX();
			float DeltaWidth = (InsideWidth - BoardHeight) / 2.0;
			CurInside->SetShapePosX(InsidePosX + DeltaWidth);
			CurInside->SetShapeWidth(BoardHeight);
			LoBInside->SetShapeWidth(LoBInside->GetShapeWidth() + DeltaWidth);
			RoUInside->SetShapePosX(InsidePosX + DeltaWidth + BoardHeight);
			RoUInside->SetShapeWidth(RoUInside->GetShapeWidth() + DeltaWidth);
		}

		return true;
	}
}

bool FInsideShape::SetBoardHeight(const FString& InValue)
{
	//0、只处理板件内胆
	if (!SelfShape.IsValid())
	{
		return false;
	}
	if (ST_Board != SelfShape->GetShapeType())
	{
		return false;
	}

	//1、获取板件相关信息
	TSharedPtr<FBoardShape> BoardShape = StaticCastSharedPtr<FBoardShape>(SelfShape);
	float OldHeightValue = BoardShape->GetShapeHeight();
	FString OldHeightStr = BoardShape->GetShapeHeightAttri()->GetAttributeStr();
	if (OldHeightStr == InValue)
	{
		return false;
	}

	//2、设置板件厚度
	bool bResult = BoardShape->SetShapeHeight(InValue);
	if (!bResult)
	{
		return false;
	}
	float BoardHeight = BoardShape->GetShapeHeight();

	//3、板件内胆已开启复制
	if (bBoardCopyValid)
	{
		float InsideHeight = GetShapeHeight();
		//3.1、当前内胆尺寸小于输入板件厚度
		if (InsideHeight < BoardHeight)
		{
			//获取所属的单元柜
			FSCTShape* ParentShape = ParentInsideShape->GetParentShape();
			while (ParentShape && ST_Unit != ParentShape->GetShapeType())
			{
				ParentShape = ParentShape->GetParentShape();
			}
			//通过单元柜设置内胆尺寸
			FUnitShape* UnitShape = StaticCast<FUnitShape*>(ParentShape);
			if (IDT_Horizontal == DirectType)
			{
				bResult = UnitShape->SetInsideHeight(this, BoardHeight);
			}
			else
			{
				bResult = UnitShape->SetInsideWidth(this, BoardHeight);
			}
			//内胆厚度设置失败，重置板件厚度未原来的值
			if (!bResult)
			{
				SelfShape->SetShapeHeight(OldHeightStr);
			}
		}

		//3.2、处理复制板件的厚度
		for (int32 i = 0; i < CopyShapes.Num(); ++i)
		{
			bResult = CopyShapes[i]->SetShapeHeight(InValue);
			if (!bResult)
			{
				return false;
			}
		}

		//3.3、重新处理复制板件
		bResult = ProcessBoardShapeCopies();
	}
	//4、板件内胆未开启复制
	else
	{
		//4.1、获取所属的单元柜
		FSCTShape* ParentShape = ParentInsideShape->GetParentShape();
		while (ParentShape && ST_Unit != ParentShape->GetShapeType())
		{
			ParentShape = ParentShape->GetParentShape();
		}
		//4.2、通过单元柜设置内胆尺寸
		FUnitShape* UnitShape = StaticCast<FUnitShape*>(ParentShape);
		if (IDT_Horizontal == DirectType)
		{
			bResult = UnitShape->SetInsideHeight(this, BoardHeight);
		}
		else
		{
			bResult = UnitShape->SetInsideWidth(this, BoardHeight);
			//调整板件位置
			if (bResult)
			{
				bResult = SelfShape->SetShapePosX(BoardHeight);
			}
		}
		//4.3、内胆厚度设置失败，重置板件厚度未原来的值
		if (!bResult)
		{
			SelfShape->SetShapeHeight(OldHeightStr);
		}
	}

	return bResult;
}

int32 FInsideShape::GetChildInsideShapeIndex(FInsideShape* InShape) const
{
	for (int32 i=0; i<ChildInsideShapes.Num(); ++i)
	{
		if (ChildInsideShapes[i].Get() == InShape)
		{
			return i;
		}
	}
	return -1;
}

bool FInsideShape::InsertChildInsideShape(FInsideShape* InCurShape, TSharedPtr<FInsideShape> InNewShape)
{
	InNewShape->SetUnitShapeData(GetUnitShapeData());
	InNewShape->SetParentShape(this);
	InsertChildShape(InCurShape, InNewShape);

	InNewShape->SetParentInsideShape(this);
	if (!InCurShape)
	{
		ChildInsideShapes.Add(InNewShape);
	} 
	else
	{
		int32 IndexFind = GetChildInsideShapeIndex(InCurShape);
		ChildInsideShapes.Insert(InNewShape, IndexFind);
	}
	return true;
}

void FInsideShape::DeleteChildInsideShape(FInsideShape* InShape)
{
	for (int32 i = 0; i < ChildInsideShapes.Num(); ++i)
	{
		if (ChildInsideShapes[i].Get() == InShape)
		{
			RemoveChildShape(ChildInsideShapes[i]);
			ChildInsideShapes.RemoveAt(i);
			break;
		}
	}
}

bool FInsideShape::SetShapeWidthWithBoard(float InValue)
{
	//处理内胆尺寸
	FSCTShape::SetShapeWidth(InValue);
	//处理板件尺寸
	if (IDT_Horizontal == DirectType)
	{
		SelfShape->SetShapeWidth(InValue);
		for (int32 i=0; i<CopyShapes.Num(); ++i)
		{
			CopyShapes[i]->SetShapeWidth(InValue);
		}
	}
	else if (IDT_Vertical == DirectType)
	{
		if (bBoardCopyValid)
		{
			ProcessBoardShapeCopies();
		}
		else
		{
			//板件厚度不能这样修改
			//SelfShape->SetShapeHeight(InValue);
		}
	}
	else
	{
		check(false);
	}
	return true;
}

bool FInsideShape::SetShapeWidthWithBase(float InValue)
{
	//处理内胆尺寸
	FSCTShape::SetShapeWidth(InValue);
	//处理基础组件尺寸
	SelfShape->SetShapeWidth(InValue);
	//TODO：处理复制组件型录的尺寸
	for (int32 i=0; i<CopyShapes.Num(); ++i)
	{
		CopyShapes[i]->SetShapeWidth(InValue);
	}

	return true;
}

bool FInsideShape::SetShapeWidthWithChildren(float InValue)
{
	//处理子内胆尺寸
	//当前内胆为横向，分别按比例处理每个子内胆（包括尺寸和位置）
	if (IDT_Horizontal == DirectType)
	{
		bool bScale = ScaleChildrenWidth(InValue);
	} 
	else //当前内胆为纵向，分别独立处理每个子内胆
	{
		for (int32 i = 0; i < ChildInsideShapes.Num(); ++i)
		{
			ChildInsideShapes[i]->SetShapeWidth(InValue);
		}
	}

	//处理内胆尺寸
	FSCTShape::SetShapeWidth(InValue);
	return true;
}

bool FInsideShape::SetShapeDepthWithBoard(float InValue)
{
	//处理内胆尺寸
	FSCTShape::SetShapeDepth(InValue);
	//处理板件尺寸
	if (IDT_Horizontal == DirectType)
	{
		SelfShape->SetShapeDepth(InValue);
		for (int32 i=0; i<CopyShapes.Num(); ++i)
		{
			CopyShapes[i]->SetShapeDepth(InValue);
		}
	}
	else if (IDT_Vertical == DirectType)
	{
		SelfShape->SetShapeDepth(InValue);
		for (int32 i = 0; i < CopyShapes.Num(); ++i)
		{
			CopyShapes[i]->SetShapeDepth(InValue);
		}
	}
	else
	{
		check(false);
	}
	return true;
}

bool FInsideShape::SetShapeDepthWithBase(float InValue)
{
	//处理内胆尺寸
	FSCTShape::SetShapeDepth(InValue);
	//处理基础组件尺寸
	SelfShape->SetShapeDepth(InValue);
	//处理复制组件型录的尺寸
	for (int32 i = 0; i < CopyShapes.Num(); ++i)
	{
		CopyShapes[i]->SetShapeDepth(InValue);
	}
	return true;
}

bool FInsideShape::SetShapeDepthWithChildren(float InValue)
{
	//处理子内胆尺寸
	for (int32 i = 0; i < ChildInsideShapes.Num(); ++i)
	{
		ChildInsideShapes[i]->SetShapeDepth(InValue);
	}
	//处理内胆尺寸
	FSCTShape::SetShapeDepth(InValue);
	return true;
}

bool FInsideShape::SetShapeHeightWithBoard(float InValue)
{
	//处理内胆尺寸
	FSCTShape::SetShapeHeight(InValue);
	//处理板件尺寸
	if (IDT_Horizontal == DirectType)  //复制方向
	{
		if (bBoardCopyValid)
		{
			ProcessBoardShapeCopies();
		} 
		else
		{
			//板件厚度不能这样修改
			//SelfShape->SetShapeHeight(InValue);
		}
	} 
	else if (IDT_Vertical == DirectType) //非复制方向
	{
		SelfShape->SetShapeWidth(InValue);
		for (int32 i=0; i<CopyShapes.Num(); ++i)
		{
			CopyShapes[i]->SetShapeWidth(InValue);
		}
	} 
	else
	{
		check(false);
	}
	return true;
}

bool FInsideShape::SetShapeHeightWithBase(float InValue)
{
	//首先设置内胆尺寸
	bool bRes = FSCTShape::SetShapeHeight(InValue);
	if (!bRes)
	{
		return false;
	}
	
	//处理复制组件型录的尺寸
	bool bBase = ProcessBaseShapeCopies();
	
	//修正内胆尺寸
	float InsideHeight = SelfShape->GetShapeHeight() * (CopyShapes.Num() + 1);
	FSCTShape::SetShapeHeight(InsideHeight);

	return bBase;
}

bool FInsideShape::SetShapeHeightWithChildren(float InValue)
{
	//处理子内胆尺寸
	//当前内胆为纵向，分别按比例处理每个子内胆（包括尺寸和位置）
	if (IDT_Vertical == DirectType)
	{
		bool bScale = ScaleChildrenHeihgt(InValue);
	}
	else //当前内胆为横向，分别独立处理每个子内胆
	{
		for (int32 i = 0; i < ChildInsideShapes.Num(); ++i)
		{
			ChildInsideShapes[i]->SetShapeHeight(InValue);
		}
	}

	//处理内胆尺寸
	FSCTShape::SetShapeHeight(InValue);
	return true;
}

bool FInsideShape::ScaleChildrenWidth(float InValue)
{
	//当前内胆为横向时，才需要下列处理
	check(IDT_Horizontal == DirectType);
	//当前内胆的宽度变化范围
	float MinWidth = 0.0;
	float MaxWidth = 0.0;
	float CurWidth = GetInsideWidthRange(MinWidth, MaxWidth);
	if (InValue < MinWidth || InValue > MaxWidth)
	{
		return false;
	}

	//当前尺寸变化为扩大操作
	bool bEnlarge = InValue > CurWidth;
	//当前操作变化量
	float CurDelta = InValue - CurWidth;
	
	//获取子内胆的尺寸信息
	float TotalRange = 0.0;
	float Totalfixed = 0.0;
	TArray<float> extremumValues; //可变子内胆尺寸极值
	TArray<float> CurrentValues;  //可变子内胆尺寸当前量
	for (int32 i=0; i<ChildInsideShapes.Num(); ++i)
	{
		float ChildMin = 10000.0;
		float ChildMax = 0.0;
		float ChildCur = ChildInsideShapes[i]->GetInsideWidthRange(ChildMin, ChildMax);
		//处理当前不可变子内胆
		if (((ChildMax - ChildMin) < KINDA_SMALL_NUMBER) ||              //子内胆尺寸固定
			(bEnlarge && (ChildMax - ChildCur) < KINDA_SMALL_NUMBER) ||  //子内胆尺寸达到极大值
			(!bEnlarge && (ChildCur - ChildMin) < KINDA_SMALL_NUMBER))   //子内胆尺寸达到极小值
		{
			Totalfixed += ChildCur;
			extremumValues.Add(-1.0);
			CurrentValues.Add(ChildCur);
		}
		else  //处理当前可变子内胆
		{
			TotalRange += ChildCur;
			CurrentValues.Add(ChildCur);
			if (bEnlarge)
			{
				extremumValues.Add(ChildMax);
			}
			else
			{
				extremumValues.Add(ChildMin);
			}
		}

	}

	//判断子内胆尺寸是否都在合理变化范围内
	bool bSucceed = true;
	float NewLocation = 0.0;
	float NewRange = TotalRange + CurDelta;
	for (int32 i = 0; i<ChildInsideShapes.Num(); ++i)
	{
		//当前子内胆尺寸不可变，只调整位置
		if (extremumValues[i] < 0)
		{
			ChildInsideShapes[i]->SetShapePosX(NewLocation);
			NewLocation += ChildInsideShapes[i]->GetShapeWidth();
		} 
		//当前子内胆尺寸可变，进行尺寸和位置调整
		else
		{
			float DesiredValue = CurrentValues[i] * NewRange / TotalRange;
			//当前变化操作可执行，修改尺寸至期望值，并同步位置
			if ((bEnlarge && DesiredValue <= extremumValues[i]) || 
				(!bEnlarge && DesiredValue >= extremumValues[i]))
			{
				ChildInsideShapes[i]->SetShapeWidth(DesiredValue);
				ChildInsideShapes[i]->SetShapePosX(NewLocation);
				NewLocation += DesiredValue;
			} 
			//当前变化操作不可执行，暂时修改尺寸至极大/小值，并同步位置
			else  
			{
				ChildInsideShapes[i]->SetShapeWidth(extremumValues[i]);
				ChildInsideShapes[i]->SetShapePosX(NewLocation);
				NewLocation += extremumValues[i];
				bSucceed = false;
			}
		}
	}

	//处理剩余变化量
	if (!bSucceed)
	{
		ScaleChildrenWidth(InValue);
	}

	return true;
}
bool FInsideShape::ScaleChildrenHeihgt(float InValue)
{
	//当前内胆为纵向时，才需要下列处理
	check(IDT_Vertical == DirectType);
	//当前内胆的厚度变化范围
	float MinHeight = 0.0;
	float MaxHeight = 0.0;
	float CurHeight = GetInsideHeightRange(MinHeight, MaxHeight);
	if (InValue < MinHeight || InValue > MaxHeight)
	{
		return false;
	}

	//当前尺寸变化为扩大操作
	bool bEnlarge = InValue > CurHeight;
	//当前操作变化量
	float CurDelta = InValue - CurHeight;

	//获取子内胆的尺寸信息
	float TotalRange = 0.0;
	float Totalfixed = 0.0;
	TArray<float> extremumValues; //可变子内胆尺寸极值
	TArray<float> CurrentValues;  //可变子内胆尺寸当前量
	for (int32 i = 0; i < ChildInsideShapes.Num(); ++i)
	{
		float ChildMin = 10000.0;
		float ChildMax = 0.0;
		float ChildCur = ChildInsideShapes[i]->GetInsideHeightRange(ChildMin, ChildMax);
		//处理当前不可变子内胆
		if (((ChildMax - ChildMin) < KINDA_SMALL_NUMBER) ||              //子内胆尺寸固定
			(bEnlarge && (ChildMax - ChildCur) < KINDA_SMALL_NUMBER) ||  //子内胆尺寸达到极大值
			(!bEnlarge && (ChildCur - ChildMin) < KINDA_SMALL_NUMBER))   //子内胆尺寸达到极小值
		{
			Totalfixed += ChildCur;
			extremumValues.Add(-1.0);
			CurrentValues.Add(ChildCur);
		}
		else  //处理当前可变子内胆
		{
			TotalRange += ChildCur;
			CurrentValues.Add(ChildCur);
			if (bEnlarge)
			{
				extremumValues.Add(ChildMax);
			}
			else
			{
				extremumValues.Add(ChildMin);
			}
		}

	}

	//判断子内胆尺寸是否都在合理变化范围内
	bool bSucceed = true;
	float NewLocation = 0.0;
	float NewRange = TotalRange + CurDelta;
	for (int32 i = 0; i < ChildInsideShapes.Num(); ++i)
	{
		//当前子内胆尺寸不可变，只调整位置
		if (extremumValues[i] < 0)
		{
			ChildInsideShapes[i]->SetShapePosZ(NewLocation);
			NewLocation += ChildInsideShapes[i]->GetShapeHeight();
		}
		//当前子内胆尺寸可变，进行尺寸和位置调整
		else
		{
			float DesiredValue = CurrentValues[i] * NewRange / TotalRange;
			//当前变化操作可执行，修改尺寸至期望值，并同步位置
			if ((bEnlarge && DesiredValue <= extremumValues[i]) ||
				(!bEnlarge && DesiredValue >= extremumValues[i]))
			{
				ChildInsideShapes[i]->SetShapeHeight(DesiredValue);
				ChildInsideShapes[i]->SetShapePosZ(NewLocation);
				NewLocation += DesiredValue;
			}
			//当前变化操作不可执行，暂时修改尺寸至极大/小值，并同步位置
			else
			{
				ChildInsideShapes[i]->SetShapeHeight(extremumValues[i]);
				ChildInsideShapes[i]->SetShapePosZ(NewLocation);
				NewLocation += extremumValues[i];
				bSucceed = false;
			}
		}
	}

	//处理剩余变化量
	if (!bSucceed)
	{
		ScaleChildrenHeihgt(InValue);
	}

	return true;
}

bool FInsideShape::ProcessBaseShapeCopies()
{
	//1、仅处理基础组件
	if (ST_Base != SelfShape->GetShapeType())
	{
		return true;
	}

	//2、获取基础组件的高度变化范围和当前值
	TSharedPtr<FShapeAttribute> HeightAttri = SelfShape->GetShapeHeightAttri();
	TSharedPtr<FNumberRangeAttri> NumberAttri = StaticCastSharedPtr<FNumberRangeAttri>(HeightAttri);
	float MinHeight = NumberAttri->GetMinValue();
	float MaxHeight = NumberAttri->GetMaxValue();
	float BaseHeight = NumberAttri->GetNumberValue();

	//3、获取内胆当前尺寸
	float InsideValue = GetShapeHeight();

	//4、根据当前内胆尺寸和组件总体厚度增、删复制组件
	if (InsideValue > MaxHeight * (CopyShapes.Num() + 1))
	{
		while (MinHeight  * (CopyShapes.Num() + 1 + 1) <= InsideValue)
		{
			TSharedPtr<FSCTShape> NewBase = FSCTShapeManager::Get()->CopyShapeToNew(SelfShape);
			ASCTShapeActor* NewActor = NewBase->SpawnShapeActor();
			NewBase->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName/*"BlockAllExceptAxis"*/);
			AddCopyShape(NewBase);
			if (InsideValue <= MaxHeight * (CopyShapes.Num() + 1))
			{
				break;
			}
		}
	}
	else if (InsideValue < MinHeight * (CopyShapes.Num() + 1))
	{
		while (MinHeight  * (CopyShapes.Num() + 1) > InsideValue)
		{
			if (CopyShapes.Num() == 0)
			{
				return false;
			}
			RemoveCopyShapeAt(CopyShapes.Num() - 1);
			if (InsideValue >= MinHeight * (CopyShapes.Num() + 1))
			{
				break;
			}
		}
	}

	//5、处理所有基础组件(SelfShape和CopyShapes)的尺寸和位置
	float SingleHeight = 0.0;
	if (InsideValue >= MinHeight * (CopyShapes.Num() + 1) && InsideValue <= MaxHeight * (CopyShapes.Num() + 1))
	{
		SingleHeight = InsideValue / (CopyShapes.Num() + 1);
	} 
	else
	{
		SingleHeight = MaxHeight;
	}
	SelfShape->SetShapeHeight(SingleHeight);
	for (int32 i = 0; i < CopyShapes.Num(); ++i)
	{
		CopyShapes[i]->SetShapeHeight(SingleHeight);
		CopyShapes[i]->SetShapePosZ(SingleHeight * (i + 1));
	}

	return true;
}

bool FInsideShape::ProcessBoardShapeCopies()
{
	//1、是否启动板件复制
	if (!bBoardCopyValid || ST_Board != SelfShape->GetShapeType())
	{
		return true;
	}

	//2、获取当前内胆的尺寸和板件厚度
	float InsideValue = 0.0;
	if (IDT_Horizontal == DirectType)
	{
		InsideValue = GetShapeHeight();
	}
	else
	{
		InsideValue = GetShapeWidth();
	}
	float BoardHeight = SelfShape->GetShapeHeight();

	//3、计算当前内胆尺寸允许的板件数量
	int32 BoardCount = FMath::FloorToInt((InsideValue - CopyThreshold) / (BoardHeight + CopyThreshold));

	//4、处理板件自身
	ASCTShapeActor* SelfActor = SelfShape->GetShapeActor();
	if (BoardCount > 0)
	{
		//生成SelfShape的Actor
		if (!SelfActor && ShapeActor)
		{
			SelfActor = SelfShape->SpawnShapeActor();
			SelfShape->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName/*"BlockAllExceptAxis"*/);
			SelfActor->AttachToActorOverride(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
	else
	{
		if (ShapeActor)
		{
			SelfShape->DestroyShapeActor();
		}
	}

	//5、处理板件复制
	int32 NewCopyCount = BoardCount < 2 ? 0 : BoardCount - 1;
	int32 CopyCount = CopyShapes.Num();
	if (NewCopyCount > CopyCount)
	{
		for (int32 i = CopyCount; i < NewCopyCount; ++i)
		{
			TSharedPtr<FSCTShape> NewBoard = FSCTShapeManager::Get()->CopyShapeToNew(SelfShape);
			ASCTShapeActor* NewActor = NewBoard->SpawnShapeActor();
			NewBoard->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName/*"BlockAllExceptAxis"*/);
			AddCopyShape(NewBoard);
		}
	}
	else if (NewCopyCount < CopyCount)
	{
		for (int32 i = CopyCount; i > NewCopyCount; --i)
		{
			RemoveCopyShapeAt(i - 1);
		}
	}

	//6、板件数量为0，无需重新计算板件位置
	if (BoardCount <= 0)
	{
		return true;
	}

	//7、重新计算所有板件位置
	float SingleHeight = InsideValue / (BoardCount + 1);
	if (IDT_Horizontal == DirectType)
	{
		SelfShape->SetShapePosZ(SingleHeight - BoardHeight / 2.0);
		for (int32 i = 0; i < NewCopyCount; ++i)
		{
			CopyShapes[i]->SetShapePosZ(SingleHeight * (i + 2) - BoardHeight / 2.0);
		}
	}
	else
	{
		SelfShape->SetShapePosX(SingleHeight + BoardHeight / 2.0);
		for (int32 i = 0; i < NewCopyCount; ++i)
		{
			CopyShapes[i]->SetShapePosX(SingleHeight * (i + 2) + BoardHeight / 2.0);
		}
	}

	return true;
}


