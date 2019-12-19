#include "SCTSpaceShape.h"
#include "SCTShapeActor.h"
#include "SCTShapeBoxActor.h"
#include "SCTFrameShape.h"
#include "SCTUnitShape.h"
#include "SCTShape.h"
#include "SCTBoardShape.h"
#include "SCTOutline.h"
#include "SCTShapeData.h"
#include "SCTSpaceActor.h"
#include "Engine/Engine.h"
#include "Engine/CollisionProfile.h"
#include "SCTDoorGroup.h"
#include "SCTCabinetShape.h"
#include "SCTShapeManager.h"
#include "SCTAttribute.h"
#include "MaterialManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "SCTSlidingDrawerShape.h"
#include "SCTEmbededElectricalGroup.h"
#include "SCTModelShape.h"


FSpaceShape::FSpaceShape()
{
	CabinetShapeData = nullptr;
	SetShapeType(ST_Space);
	SetShapeName(TEXT("柜体空间"));
	//空间轮廓
	SpaceOutline = MakeShareable(new FSCTOutline);
	//修正尺寸最小值为0.0；
	TSharedPtr<FNumberRangeAttri> WidthRange = StaticCastSharedPtr<FNumberRangeAttri>(Width);
	WidthRange->SetMinValue(0.0);
	TSharedPtr<FNumberRangeAttri> DepthRange = StaticCastSharedPtr<FNumberRangeAttri>(Depth);
	DepthRange->SetMinValue(0.0);
	TSharedPtr<FNumberRangeAttri> HeightRange = StaticCastSharedPtr<FNumberRangeAttri>(Height);
	HeightRange->SetMinValue(0.0);
}
FSpaceShape::FSpaceShape(FCabinetShape* InCabinetShape)
{
	CabinetShapeData = InCabinetShape;
	SetShapeType(ST_Space);
	SetShapeName(TEXT("柜体空间"));
	//空间轮廓
	SpaceOutline = MakeShareable(new FSCTOutline);
}

FSpaceShape::~FSpaceShape()
{
	DestroySpaceShapeActor();
}

void FSpaceShape::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//解析型录基本信息
	FSCTShape::ParseFromJson(InJsonObject);

	//空间类型
	SpaceSubType = (ESpaceSubType)InJsonObject->GetIntegerField(TEXT("spaceSubType"));
	//空间方向
	int32 DirectInt = InJsonObject->GetIntegerField(TEXT("spaceDirect"));
	SetDirectType((ESpaceDirectType)DirectInt);
	//分割数值
	ScaleValue = InJsonObject->GetNumberField(TEXT("scaleValue"));
	//空间轮廓
	check(SpaceOutline.IsValid());
	SpaceOutline->ParseFromJson(InJsonObject);

	//分割板件前向外延计数
	FrontExtensionCount = InJsonObject->GetIntegerField(TEXT("frontExtensionCount"));
	//顶层内胆内缩方向
	RetractType = InJsonObject->GetIntegerField(TEXT("retractType"));
	//显示/隐藏门组Actor
	bShowDoorGroup = InJsonObject->GetBoolField(TEXT("ShowDoorGroup"));
	//子级空间分割
	ChildrenSplitMode = InJsonObject->GetBoolField(TEXT("childrenSplitMode"));
	int32 SplitDirectInt = InJsonObject->GetIntegerField(TEXT("childrenSplitDirect"));
	SetChildrenSpaceSplitDirect((ESpaceDirectType)SplitDirectInt);

	if (InJsonObject->HasTypedField<EJson::Object>(TEXT("exportSizeInfo")))
	{
		auto ParseExportValue = [](const TSharedPtr<FJsonObject> & InObjRef,FRangeAndStandValue & OutValue)
		{
			OutValue.MinValue = InObjRef->GetNumberField(TEXT("minValue"));
			OutValue.MaxValue = InObjRef->GetNumberField(TEXT("maxValue"));
			const TArray<TSharedPtr<FJsonValue>> & ArrayValueRef = InObjRef->GetArrayField(TEXT("standValues"));
			for(const auto & Ref : ArrayValueRef)
			{
				OutValue.StandValueList.Emplace(Ref->AsNumber());
			}
		};

		const TSharedPtr<FJsonObject> & ExportSizeInfoBojRef = InJsonObject->GetObjectField(TEXT("exportSizeInfo"));
		if (ExportSizeInfoBojRef->HasTypedField<EJson::Object>(TEXT("width")))
		{
			const TSharedPtr<FJsonObject> & WidthExportObjRef = ExportSizeInfoBojRef->GetObjectField(TEXT("width"));
			{
				ParseExportValue(WidthExportObjRef,WidthExportValue);
			}			
		}
		if (ExportSizeInfoBojRef->HasTypedField<EJson::Object>(TEXT("height")))
		{
			const TSharedPtr<FJsonObject> & HeightExportObjRef = ExportSizeInfoBojRef->GetObjectField(TEXT("height"));
			{
				ParseExportValue(HeightExportObjRef, HeightExportValue);
			}
		}
		if (ExportSizeInfoBojRef->HasTypedField<EJson::Object>(TEXT("depth")))
		{
			const TSharedPtr<FJsonObject> & DepthExportObjRef = ExportSizeInfoBojRef->GetObjectField(TEXT("depth"));
			{
				ParseExportValue(DepthExportObjRef, DepthExportValue);
			}
		}
	}


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
			//解析子级对象
			CopyShape->ParseAttributesFromJson(ChildObject);
			//根据子对象类型分别处理
			switch (ShapeType)
			{
			case ST_Base:  //板件
			{
				TSharedPtr<FBoardShape> NewBoardShape = StaticCastSharedPtr<FBoardShape>(CopyShape);
				SetSplitBoard(NewBoardShape);
				break;
			}
			case ST_Frame:   //柜体框
			{
				TSharedPtr<FFrameShape> NewFrameShape = StaticCastSharedPtr<FFrameShape>(CopyShape);
				NewFrameShape->SetSpaceShapeData(this);
				SetFrameShape(NewFrameShape);
				break;
			}
			case ST_DrawerGroup:  //外盖抽屉组
			{
				TSharedPtr<FDrawerGroupShape> NewDrawerGroup = StaticCastSharedPtr<FDrawerGroupShape>(CopyShape);
				//设置外盖抽屉组
				SetCoveredDrawerGroup(NewDrawerGroup);
				break;
			}
			case ST_SlidingDoor:  //移门
			{
				TSharedPtr<FSlidingDoor> NewSlidingDoor = StaticCastSharedPtr<FSlidingDoor>(CopyShape);
				//设置（摆放）移门
				SetDoorShape(NewSlidingDoor);
				break;
			}	
			case ST_EmbeddedElectricalGroup:
			{
				TSharedPtr<FEmbededElectricalGroup> NewEmbededElectricalGroup = StaticCastSharedPtr<FEmbededElectricalGroup>(CopyShape);
				//设置五金电器组
				SetEmbededElectricalGroup(NewEmbededElectricalGroup);
				break;
			}
			case ST_SideHungDoor:  //掩门
			{
				TSharedPtr<FSideHungDoor> NewSideHungDoor = StaticCastSharedPtr<FSideHungDoor>(CopyShape);
				//设置（摆放）掩门
				SetDoorShape(NewSideHungDoor);
				break;
			}
			case ST_Unit:  //顶层内胆
			{
				TSharedPtr<FUnitShape> NewUnitShape = StaticCastSharedPtr<FUnitShape>(CopyShape);
				SetTopInsideShape(NewUnitShape);
				break;
			}
			case ST_Space:
			{
				TSharedPtr<FSpaceShape> NewSpaceShape = StaticCastSharedPtr<FSpaceShape>(CopyShape); 
				NewSpaceShape->SetCabinetShapeData(GetCabinetShapeData());
				//内部空间
				if (NewSpaceShape->IsInsideSpace())
				{
					SetInsideSpace(NewSpaceShape);
				}
				//底部空间
				else if (NewSpaceShape->IsBottomSpace())
				{
					SetBottomSpace(NewSpaceShape);
				}
				//框体空间(子级空间)
				else if (NewSpaceShape->IsFrameShapeSpace())
				{
					bool bInsert = InsertChildSpaceShape(nullptr, NewSpaceShape);
					check(bInsert);
				}
				else
				{
					check(false);
				}
				break;
			}
			default:
				break;
			}
		}
	}
}

void FSpaceShape::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//型录类型和Id
	int32 CurrentType = InJsonObject->GetNumberField(TEXT("type"));
	int32 CurrentId = InJsonObject->GetNumberField(TEXT("id"));
	check(CurrentType == ShapeType && CurrentId == ShapeId);

	//解析型录基本信息
	FSCTShape::ParseAttributesFromJson(InJsonObject);

	//空间类型
	SpaceSubType = (ESpaceSubType)InJsonObject->GetIntegerField(TEXT("spaceSubType"));
	//空间方向
	int32 DirectInt = InJsonObject->GetIntegerField(TEXT("spaceDirect"));
	SetDirectType((ESpaceDirectType)DirectInt);
	//分割数值
	ScaleValue = InJsonObject->GetNumberField(TEXT("scaleValue"));
	//空间轮廓
	check(SpaceOutline.IsValid());
	SpaceOutline->ParseAttributesFromJson(InJsonObject);

	//分割板件前向外延计数
	FrontExtensionCount = InJsonObject->GetIntegerField(TEXT("frontExtensionCount"));
	//顶层内胆内缩方向
	RetractType = InJsonObject->GetIntegerField(TEXT("retractType"));
	//显示/隐藏门组Actor
	bShowDoorGroup = InJsonObject->GetBoolField(TEXT("ShowDoorGroup"));
	//子级空间分割
	ChildrenSplitMode = InJsonObject->GetBoolField(TEXT("childrenSplitMode"));
	int32 SplitDirectInt = InJsonObject->GetIntegerField(TEXT("childrenSplitDirect"));
	SetChildrenSpaceSplitDirect((ESpaceDirectType)SplitDirectInt);

	if (InJsonObject->HasTypedField<EJson::Object>(TEXT("exportSizeInfo")))
	{
		auto ParseExportValue = [](const TSharedPtr<FJsonObject> & InObjRef, FRangeAndStandValue & OutValue)
		{
			OutValue.MinValue = InObjRef->GetNumberField(TEXT("minValue"));
			OutValue.MaxValue = InObjRef->GetNumberField(TEXT("maxValue"));
			const TArray<TSharedPtr<FJsonValue>> & ArrayValueRef = InObjRef->GetArrayField(TEXT("standValues"));
			for (const auto & Ref : ArrayValueRef)
			{
				OutValue.StandValueList.Emplace(Ref->AsNumber());
			}
		};

		const TSharedPtr<FJsonObject> & ExportSizeInfoBojRef = InJsonObject->GetObjectField(TEXT("exportSizeInfo"));
		if (ExportSizeInfoBojRef->HasTypedField<EJson::Object>(TEXT("width")))
		{
			const TSharedPtr<FJsonObject> & WidthExportObjRef = ExportSizeInfoBojRef->GetObjectField(TEXT("width"));
			{
				ParseExportValue(WidthExportObjRef, WidthExportValue);
			}
		}
		if (ExportSizeInfoBojRef->HasTypedField<EJson::Object>(TEXT("height")))
		{
			const TSharedPtr<FJsonObject> & HeightExportObjRef = ExportSizeInfoBojRef->GetObjectField(TEXT("height"));
			{
				ParseExportValue(HeightExportObjRef, HeightExportValue);
			}
		}
		if (ExportSizeInfoBojRef->HasTypedField<EJson::Object>(TEXT("depth")))
		{
			const TSharedPtr<FJsonObject> & DepthExportObjRef = ExportSizeInfoBojRef->GetObjectField(TEXT("depth"));
			{
				ParseExportValue(DepthExportObjRef, DepthExportValue);
			}
		}
	}

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
			
			//根据子对象类型分别处理
			switch (ShapeType)
			{
				case ST_Board:  //板件
				{
					TSharedPtr<FBoardShape> NewBoardShape = StaticCastSharedPtr<FBoardShape>(CopyShape);
					SetSplitBoard(NewBoardShape);
					//解析子级对象
					NewBoardShape->ParseAttributesFromJson(ChildObject);
					break;
				}
				case ST_Frame:   //柜体框
				{
					TSharedPtr<FFrameShape> NewFrameShape = StaticCastSharedPtr<FFrameShape>(CopyShape);
					SetFrameShape(NewFrameShape);
					//解析子级对象
					NewFrameShape->ParseAttributesFromJson(ChildObject);
					break;
				}
				case ST_DrawerGroup:  //外盖抽屉组
				{
					TSharedPtr<FDrawerGroupShape> NewDrawerGroup = StaticCastSharedPtr<FDrawerGroupShape>(CopyShape);
					//解析子级对象
					NewDrawerGroup->ParseAttributesFromJson(ChildObject);
					//设置外盖抽屉组
					SetCoveredDrawerGroup(NewDrawerGroup);
					break;
				}
				case ST_EmbeddedElectricalGroup: // 电器饰条组
				{
					TSharedPtr<FEmbededElectricalGroup> NewGroup = StaticCastSharedPtr<FEmbededElectricalGroup>(CopyShape);
					//解析子级对象
					NewGroup->ParseAttributesFromJson(ChildObject);
					//设置电器饰条组
					SetEmbededElectricalGroup(NewGroup);
					break;
				}
				case ST_SlidingDoor:  //移门
				{
					TSharedPtr<FSlidingDoor> NewSlidingDoor = StaticCastSharedPtr<FSlidingDoor>(CopyShape); 
					//解析子级对象
					NewSlidingDoor->ParseAttributesFromJson(ChildObject);
					//设置（摆放）移门
					SetDoorShape(NewSlidingDoor);					
					break;
				}
				case ST_SideHungDoor:  //掩门
				{
					TSharedPtr<FSideHungDoor> NewSideHungDoor = StaticCastSharedPtr<FSideHungDoor>(CopyShape);
					//解析子级对象
					NewSideHungDoor->ParseAttributesFromJson(ChildObject);
					//设置（摆放）掩门
					SetDoorShape(NewSideHungDoor);
					break;
				}
				case ST_Unit:  //顶层内胆
				{
					TSharedPtr<FUnitShape> NewUnitShape = StaticCastSharedPtr<FUnitShape>(CopyShape);
					SetTopInsideShape(NewUnitShape);
					//解析子级对象
					NewUnitShape->ParseAttributesFromJson(ChildObject);
					break;
				}				
				case ST_Space:
				{
					TSharedPtr<FSpaceShape> NewSpaceShape = StaticCastSharedPtr<FSpaceShape>(CopyShape);
					//空间类型
					int32 CurSpaceSubType = ChildObject->GetIntegerField(TEXT("spaceSubType"));
					NewSpaceShape->SetSpaceSubType((ESpaceSubType)CurSpaceSubType);
					//分割板件空间
					if (NewSpaceShape->IsSplitBoardSpace())
					{
						bool bInsert = InsertChildSpaceShape(nullptr, NewSpaceShape);
						check(bInsert);
					}
					//外盖抽屉组空间
					else if (NewSpaceShape->IsDrawerGroupSpace())
					{
						bool bInsert = InsertChildSpaceShape(nullptr, NewSpaceShape);
						check(bInsert);
					}  
					// 电器饰条组空间
					else if (NewSpaceShape->IsEmbededElectricalGroup())
					{
						bool bInsert = InsertChildSpaceShape(nullptr, NewSpaceShape);
						check(bInsert);
					}
					//内部空间
					else if (NewSpaceShape->IsInsideSpace())
					{
						SetInsideSpace(NewSpaceShape);
					}
					//底部空间
					else if (NewSpaceShape->IsBottomSpace())
					{
						SetBottomSpace(NewSpaceShape);
					}
					//框体空间(子级空间)
					else if (NewSpaceShape->IsFrameShapeSpace())
					{
						bool bInsert = InsertChildSpaceShape(nullptr, NewSpaceShape);
						check(bInsert);
					}
					else
					{
						check(false);
					}
					//解析子级对象
					//NewSpaceShape->SetCabinetShapeData(GetCabinetShapeData());
					NewSpaceShape->ParseAttributesFromJson(ChildObject);
					break;
				}
				default:
					break;
			}
		}
	}
}

void FSpaceShape::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseShapeFromJson(InJsonObject);
}

void FSpaceShape::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseContentFromJson(InJsonObject);
}

void FSpaceShape::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//1、保存型录基本信息
	FSCTShape::SaveToJson(JsonWriter);

	//空间类型
	JsonWriter->WriteValue(TEXT("spaceSubType"), (int32)SpaceSubType);
	//空间方向和尺寸值
	JsonWriter->WriteValue(TEXT("spaceDirect"), GetDirectType());
	JsonWriter->WriteValue(TEXT("scaleValue"), ScaleValue);
	//空间轮廓
	check(SpaceOutline.IsValid());
	SpaceOutline->SaveToJson(JsonWriter);

	//分割板件前向外延计数
	JsonWriter->WriteValue(TEXT("frontExtensionCount"), FrontExtensionCount);
	//顶层内胆内缩方向
	JsonWriter->WriteValue(TEXT("retractType"), RetractType);
	//显示/隐藏门组Actor
	JsonWriter->WriteValue(TEXT("ShowDoorGroup"), bShowDoorGroup);
	//子级空间分割
	JsonWriter->WriteValue(TEXT("childrenSplitMode"), ChildrenSplitMode);
	JsonWriter->WriteValue(TEXT("childrenSplitDirect"), (int32)ChildrenSplitDirect);

	JsonWriter->WriteObjectStart(TEXT("exportSizeInfo"));
	{
		auto WriteExportValue = [&JsonWriter](const FString & InObjName, const FRangeAndStandValue & InValue)
		{
			JsonWriter->WriteObjectStart(InObjName);
			JsonWriter->WriteValue(TEXT("minValue"), InValue.MinValue);
			JsonWriter->WriteValue(TEXT("maxValue"), InValue.MaxValue);

			JsonWriter->WriteArrayStart(TEXT("standValues"));
			for (const auto & Ref : InValue.StandValueList)
			{
				JsonWriter->WriteValue(Ref);
			}
			JsonWriter->WriteArrayEnd();
			JsonWriter->WriteObjectEnd();
		};
		switch (DirectType)
		{
			case SDT_XDirect:
			{
				ScaleValue = GetSpaceModifyScale();
				if (ScaleValue > 0.0f)
				{
					WriteExportValue(TEXT("width"), WidthExportValue);
				}
				break;
			}
			case SDT_YDirect:
			{
				ScaleValue = GetSpaceModifyScale();
				if (ScaleValue > 0.0f)
				{
					WriteExportValue(TEXT("depth"), DepthExportValue);
				}
				break;
			}
			case SDT_ZDirect:
			{
				ScaleValue = GetSpaceModifyScale();
				if (ScaleValue > 0.0f)
				{
					WriteExportValue(TEXT("height"), HeightExportValue);
				}
				break;
			}
		}
	}
	JsonWriter->WriteObjectEnd();

	//2、保存Children信息
	JsonWriter->WriteArrayStart(TEXT("children"));
	//2.0 分割板件
	if (SplitBoardShape.IsValid())
	{
		JsonWriter->WriteObjectStart();
		SplitBoardShape->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//2.1 柜体框
	if (FrameShape.IsValid())
	{
		JsonWriter->WriteObjectStart();
		FrameShape->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//2.2 外盖抽屉组
	if (CoveredDrawerGroup.IsValid())
	{
		JsonWriter->WriteObjectStart();
		CoveredDrawerGroup->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//2.2 门组
	if (DoorShape.IsValid())
	{
		JsonWriter->WriteObjectStart();
		DoorShape->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//2.3 内部空间
	if (InsideSpace.IsValid())
	{
		JsonWriter->WriteObjectStart();
		InsideSpace->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//2.4 顶层内胆
	if (TopInsideShape.IsValid())
	{
		JsonWriter->WriteObjectStart();
		TopInsideShape->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//2.5 底部空间
	if (BottomSpace.IsValid())
	{
		JsonWriter->WriteObjectStart();
		BottomSpace->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//2.6 子级空间
	for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
	{
		JsonWriter->WriteObjectStart();
		ChildSpaceShapes[i]->SaveAttriToJson(JsonWriter);
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

void FSpaceShape::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//1、保存型录基本信息
	FSCTShape::SaveAttriToJson(JsonWriter);

	//空间类型
	JsonWriter->WriteValue(TEXT("spaceSubType"), (int32)SpaceSubType);
	//空间方向和尺寸值
	JsonWriter->WriteValue(TEXT("spaceDirect"), GetDirectType());
	JsonWriter->WriteValue(TEXT("scaleValue"), ScaleValue);
	//空间轮廓
	check(SpaceOutline.IsValid());
	SpaceOutline->SaveAttriToJson(JsonWriter);

	//分割板件前向外延计数
	JsonWriter->WriteValue(TEXT("frontExtensionCount"), FrontExtensionCount);
	//顶层内胆内缩方向
	JsonWriter->WriteValue(TEXT("retractType"), RetractType);
	//显示/隐藏门组Actor
	JsonWriter->WriteValue(TEXT("ShowDoorGroup"), bShowDoorGroup);
	//子级空间分割
	JsonWriter->WriteValue(TEXT("childrenSplitMode"), ChildrenSplitMode);
	JsonWriter->WriteValue(TEXT("childrenSplitDirect"), (int32)ChildrenSplitDirect);

	JsonWriter->WriteObjectStart(TEXT("exportSizeInfo"));
	{
		auto WriteExportValue = [&JsonWriter](const FString & InObjName, const FRangeAndStandValue & InValue)
		{
			JsonWriter->WriteObjectStart(InObjName);
			JsonWriter->WriteValue(TEXT("minValue"), InValue.MinValue);
			JsonWriter->WriteValue(TEXT("maxValue"), InValue.MaxValue);

			JsonWriter->WriteArrayStart(TEXT("standValues"));
			for (const auto & Ref : InValue.StandValueList)
			{
				JsonWriter->WriteValue(Ref);
			}
			JsonWriter->WriteArrayEnd();
			JsonWriter->WriteObjectEnd();
		};
		switch (DirectType)
		{
			case SDT_XDirect:
			{
				ScaleValue = GetSpaceModifyScale();
				if (ScaleValue > 0.0f)
				{
					WriteExportValue(TEXT("width"), WidthExportValue);
				}
				break;
			}
			case SDT_YDirect:
			{
				ScaleValue = GetSpaceModifyScale();
				if (ScaleValue > 0.0f)
				{
					WriteExportValue(TEXT("depth"), DepthExportValue);
				}
				break;
			}
			case SDT_ZDirect:
			{
				ScaleValue = GetSpaceModifyScale();
				if (ScaleValue > 0.0f)
				{
					WriteExportValue(TEXT("height"), HeightExportValue);
				}
				break;
			}	
		}		
	}
	JsonWriter->WriteObjectEnd();

	//2、保存Children信息
	JsonWriter->WriteArrayStart(TEXT("children"));
	//2.0 分割板件
	if (SplitBoardShape.IsValid())
	{
		JsonWriter->WriteObjectStart();
		SplitBoardShape->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//2.1 柜体框
	if (FrameShape.IsValid())
	{
		JsonWriter->WriteObjectStart();
		FrameShape->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//2.2 外盖抽屉组
	if (CoveredDrawerGroup.IsValid())
	{
		JsonWriter->WriteObjectStart();
		CoveredDrawerGroup->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	// 电器饰条组
	if(EmbededElecticalGroup.IsValid())
	{
		JsonWriter->WriteObjectStart();
		EmbededElecticalGroup->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//2.2 门组
	if (DoorShape.IsValid())
	{
		JsonWriter->WriteObjectStart();
		DoorShape->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//2.3 内部空间
	if (InsideSpace.IsValid())
	{
		JsonWriter->WriteObjectStart();
		InsideSpace->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//2.4 顶层内胆
	if (TopInsideShape.IsValid())
	{
		JsonWriter->WriteObjectStart();
		TopInsideShape->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//2.5 底部空间
	if (BottomSpace.IsValid())
	{
		JsonWriter->WriteObjectStart();
		BottomSpace->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//2.6 子级空间
	for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
	{
		JsonWriter->WriteObjectStart();
		ChildSpaceShapes[i]->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	//End children
	JsonWriter->WriteArrayEnd();
}

void FSpaceShape::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveShapeToJson(JsonWriter);
}

void FSpaceShape::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveContentToJson(JsonWriter);
}

void FSpaceShape::CopyTo(FSCTShape* OutShape)
{
	//基类信息拷贝
	FSCTShape::CopyTo(OutShape);

	//空间信息拷贝
	FSpaceShape* OutSpaceShape = StaticCast<FSpaceShape*>(OutShape);

	//是否为底部空间
	OutSpaceShape->SpaceSubType = SpaceSubType;

	//空间方向和比例值
	OutSpaceShape->DirectType = DirectType;
	OutSpaceShape->ScaleValue = ScaleValue;
	//空间轮廓
	FSCTOutline* NewSpaceOutline = new FSCTOutline;
	SpaceOutline->CopyTo(NewSpaceOutline);
	OutSpaceShape->SetSpaceOutline(MakeShareable(NewSpaceOutline));

	//分割板件前向外延计数
	OutSpaceShape->FrontExtensionCount = FrontExtensionCount;

	//顶层内胆内缩方向
	OutSpaceShape->RetractType = RetractType;

	//显示/隐藏门组Actor
	OutSpaceShape->bShowDoorGroup = bShowDoorGroup;

	//子级空间分割方式
	OutSpaceShape->ChildrenSplitMode = ChildrenSplitMode;
	//子级空间分割方向
	OutSpaceShape->ChildrenSplitDirect = ChildrenSplitDirect;
	OutSpaceShape->WidthExportValue = WidthExportValue;
	OutSpaceShape->HeightExportValue = HeightExportValue;
	OutSpaceShape->DepthExportValue = DepthExportValue;

	//分割板件
	if (SplitBoardShape.IsValid())
	{
		FBoardShape* NewSplitBoard = new FBoardShape;
		OutSpaceShape->SetSplitBoard(MakeShareable(NewSplitBoard));
		SplitBoardShape->CopyTo(NewSplitBoard);
	}

	//柜体框
	if (FrameShape.IsValid())
	{
		FFrameShape* NewFrameShape = new FFrameShape;
		OutSpaceShape->SetFrameShape(MakeShareable(NewFrameShape));
		FrameShape->CopyTo(NewFrameShape);
	}
	//外盖抽屉组
	if (CoveredDrawerGroup.IsValid())
	{
		FDrawerGroupShape* NewDrawerGroupShape = new FDrawerGroupShape;
		CoveredDrawerGroup->CopyTo(NewDrawerGroupShape);
		OutSpaceShape->SetCoveredDrawerGroup(MakeShareable(NewDrawerGroupShape));
	}

	//门组
	if (DoorShape.IsValid())
	{
		if (DoorShape->GetShapeType() == ST_SlidingDoor)
		{
			FSlidingDoor* NewDoorShape = new FSlidingDoor();
			OutSpaceShape->SetDoorShape(MakeShareable(NewDoorShape));
			DoorShape->CopyTo(NewDoorShape);
		}
		else if (DoorShape->GetShapeType() == ST_SideHungDoor)
		{
			FSideHungDoor* NewDoorShape = new FSideHungDoor();
			OutSpaceShape->SetDoorShape(MakeShareable(NewDoorShape));
			DoorShape->CopyTo(NewDoorShape);
		}
		else
		{
			check(false);
		}
	}
	// 电器组
	if (EmbededElecticalGroup.IsValid())
	{
		TSharedPtr<FEmbededElectricalGroup> NewEmbededElectricalGroup = MakeShareable(new FEmbededElectricalGroup);
		EmbededElecticalGroup->CopyTo(NewEmbededElectricalGroup.Get());
		OutSpaceShape->SetEmbededElectricalGroup(NewEmbededElectricalGroup);
	}

	//内部空间
	if (InsideSpace.IsValid())
	{
		FSpaceShape* NewInsideSpace = new FSpaceShape;
		OutSpaceShape->SetInsideSpace(MakeShareable(NewInsideSpace));
		InsideSpace->CopyTo(NewInsideSpace);
	}

	//顶层内胆
	if (TopInsideShape.IsValid())
	{
		FUnitShape* NewUnitShape = new FUnitShape;
		OutSpaceShape->SetTopInsideShape(MakeShareable(NewUnitShape));
		TopInsideShape->CopyTo(NewUnitShape);
	}

	//底部空间
	if (BottomSpace.IsValid())
	{
		FSpaceShape* NewBottomSpace = new FSpaceShape;
		OutSpaceShape->SetBottomSpace(MakeShareable(NewBottomSpace));
		BottomSpace->CopyTo(NewBottomSpace);
	}

	//不能在此处进行父空间指针的拷贝，因为父空间的指针也是动态生成的
	//父级对象
	//OutSpaceShape->ParentSpaceShape = ParentSpaceShape;

	//分割出的子对象
	for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
	{
		FSpaceShape* NewChildSpace = new FSpaceShape;
		OutSpaceShape->InsertChildSpaceShape(nullptr, MakeShareable(NewChildSpace));
		ChildSpaceShapes[i]->CopyTo(NewChildSpace);
		ChildSpaceShapes[i]->SetParentSpaceShape(this);
	}
}

ASCTShapeActor* FSpaceShape::SpawnShapeActor()
{
	//创建空间自身的Actor
	ASCTShapeActor* NewSpaceActor = FSCTShape::SpawnShapeActor();

	//分割板件Actor
	if (SplitBoardShape.IsValid())
	{
		ASCTShapeActor* SplitBoardActor = SplitBoardShape->SpawnShapeActor();
		SplitBoardActor->AttachToActorOverride(NewSpaceActor, FAttachmentTransformRules::KeepRelativeTransform);
	}

	//创建框体Actor
	if (FrameShape.IsValid())
	{
		ASCTShapeActor* FrameActor = FrameShape->SpawnShapeActor();
		FrameActor->AttachToActorOverride(NewSpaceActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	//创建外盖抽屉Actor
	if (CoveredDrawerGroup.IsValid())
	{
		ASCTShapeActor* DrawerGroupActor = CoveredDrawerGroup->SpawnShapeActor();
		DrawerGroupActor->AttachToActorOverride(NewSpaceActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	// 五金电器饰条组
	if (EmbededElecticalGroup.IsValid())
	{
		ASCTShapeActor* EmbededElectricalGroupActor = EmbededElecticalGroup->SpawnShapeActor();
		EmbededElectricalGroupActor->AttachToActorOverride(NewSpaceActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	//门Actor
	if (DoorShape.IsValid())
	{	
		if (DoorShape->GetShapeType() == ST_SlidingDoor)
		{
			StaticCastSharedPtr<FSlidingDoor>(DoorShape)->UpdateDoorGroup();
		}
		ASCTShapeActor* DoorShapeActor = DoorShape->SpawnShapeActor();
		DoorShapeActor->AttachToActorOverride(NewSpaceActor, FAttachmentTransformRules::KeepRelativeTransform);
		//设置门组的显隐
		if (bShowDoorGroup)
		{
			DoorShape->ShowDoorSheetsActor();
		} 
		else
		{
			DoorShape->HideDoorSheetsActor();
		}
	}
	//底部空间Actor
	if (BottomSpace.IsValid())
	{
		ASCTShapeActor* BottomSpaceActor = BottomSpace->SpawnShapeActor();
		BottomSpaceActor->AttachToActorOverride(NewSpaceActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	//内部空间Actor
	if (InsideSpace.IsValid())
	{
		ASCTShapeActor* InsideSpaceActor = InsideSpace->SpawnShapeActor();
		InsideSpaceActor->AttachToActorOverride(NewSpaceActor, FAttachmentTransformRules::KeepRelativeTransform);
	}

	//顶层内胆Actor
	if (TopInsideShape.IsValid())
	{
		ASCTShapeActor* TopInsideActor = TopInsideShape->SpawnShapeActor();
		TopInsideActor->AttachToActorOverride(NewSpaceActor, FAttachmentTransformRules::KeepRelativeTransform);
	}
	//创建子级型录的Actor
	for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
	{
		ASCTShapeActor* ChildActor = ChildSpaceShapes[i]->SpawnShapeActor();
		ChildActor->AttachToActorOverride(NewSpaceActor, FAttachmentTransformRules::KeepRelativeTransform);
	}

	return NewSpaceActor;
}

void FSpaceShape::SpawnActorsForSelected(FName InSelectProfileName, FName InHoverProfileName)
{
	//创建型录外包框（用于显示空间状态）
	if (IsSplitBoardSpace() || IsFrameShapeSpace())
	{
		ASCTWireframeActor* WireFrameActor = SpawnWireFrameActor();
		WireFrameActor->UpdateActorDimension();
		WireFrameActor->SetActorHiddenInGame(true);
		WireFrameActor->AttachToActor(GetShapeActor(), FAttachmentTransformRules::KeepRelativeTransform);
	}

	//分割板件
	if (SplitBoardShape.IsValid())
	{
		SplitBoardShape->SpawnActorsForSelected(InSelectProfileName);
	}
	//框体板件
	if (FrameShape.IsValid())
	{
		FrameShape->SpawnActorsForSelected(InSelectProfileName);
	}
	//外盖抽屉组
	if (CoveredDrawerGroup.IsValid())
	{
		CoveredDrawerGroup->SpawnActorsForSelected(InSelectProfileName);
	}
	//门组
	if (DoorShape.IsValid())
	{
		DoorShape->SpawnActorsForSelected(InSelectProfileName);
	}
	// 五金电器饰条组
	if (EmbededElecticalGroup.IsValid())
	{
		EmbededElecticalGroup->SpawnActorsForSelected(InSelectProfileName);		
	}
	//叶空间
	if (IsFrameShapeSpace() && !InsideSpace->IsChildrenSpace())
	{
		TSharedPtr<FUnitShape> CurUnitShape = InsideSpace->GetTopInsideShape();
		TSharedPtr<FInsideShape> CurTopInsideShape = CurUnitShape->GetTopInsideShape();
		if (!CurTopInsideShape.IsValid())
		{
			//创建轮廓空间包围盒（用于点选）
			ASCTSpaceActor* SpaceActor = SpawnSpaceShapeActor(InHoverProfileName);
			SpaceActor->UpdateActorDimension();
			SpaceActor->AttachToActor(GetShapeActor(), FAttachmentTransformRules::KeepRelativeTransform);
		}
	}

	//顶层内档
	if (TopInsideShape.IsValid())
	{
		TopInsideShape->SpawnActorsForSelected(InSelectProfileName, InHoverProfileName);
	}
	//内部空间
	if (InsideSpace.IsValid())
	{
		InsideSpace->SpawnActorsForSelected(InSelectProfileName, InHoverProfileName);
	}
	//底部空间
	if (BottomSpace.IsValid())
	{
		BottomSpace->SpawnActorsForSelected(InSelectProfileName, InHoverProfileName);
	}
	//子级空间
	for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
	{
		ChildSpaceShapes[i]->SpawnActorsForSelected(InSelectProfileName, InHoverProfileName);
	}
}

void FSpaceShape::SetCollisionProfileName(FName InProfileName)
{
	//分割板件
	if (SplitBoardShape.IsValid())
	{
		SplitBoardShape->SetCollisionProfileName(InProfileName);
	}
	//框体
	if (FrameShape.IsValid())
	{
		FrameShape->SetCollisionProfileName(InProfileName);
	}
	//外盖抽屉组
	if (CoveredDrawerGroup.IsValid())
	{
		CoveredDrawerGroup->SetCollisionProfileName(InProfileName);
	}
	//门组
	if (DoorShape.IsValid())
	{
		DoorShape->SetCollisionProfileName(InProfileName);
	}
	// 五金电器组
	if (EmbededElecticalGroup.IsValid())
	{
		EmbededElecticalGroup->SetCollisionProfileName(InProfileName);
	}
	//内部空间
	if (InsideSpace.IsValid())
	{
		InsideSpace->SetCollisionProfileName(InProfileName);
	}
	//底部空间
	if (BottomSpace.IsValid())
	{
		BottomSpace->SetCollisionProfileName(InProfileName);
	}
	//顶层内胆
	if (TopInsideShape.IsValid())
	{
		TopInsideShape->SetCollisionProfileName(InProfileName);
	}
	//子级空间
	for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
	{
		ChildSpaceShapes[i]->SetCollisionProfileName(InProfileName);
	}
}

void FSpaceShape::GetResourceUrls(TArray<FString> &OutResourceUrls)
{
	//分割板件
	if (SplitBoardShape.IsValid())
	{
		SplitBoardShape->GetResourceUrls(OutResourceUrls);
	}
	//框体
	if (FrameShape.IsValid())
	{
		FrameShape->GetResourceUrls(OutResourceUrls);
	}
	//外盖抽屉组
	if (CoveredDrawerGroup.IsValid())
	{
		CoveredDrawerGroup->GetResourceUrls(OutResourceUrls);
	}
	//门组
	if (DoorShape.IsValid())
	{
		DoorShape->GetResourceUrls(OutResourceUrls);
	}
	// 五金电器组
	if (EmbededElecticalGroup.IsValid())
	{
		EmbededElecticalGroup->GetResourceUrls(OutResourceUrls);
	}
	//内部空间
	if (InsideSpace.IsValid())
	{
		InsideSpace->GetResourceUrls(OutResourceUrls);
	}
	//底部空间
	if (BottomSpace.IsValid())
	{
		BottomSpace->GetResourceUrls(OutResourceUrls);
	}
	//顶层内胆
	if (TopInsideShape.IsValid())
	{
		TopInsideShape->GetResourceUrls(OutResourceUrls);
	}
	//子级空间
	for (int32 i=0; i<ChildSpaceShapes.Num(); ++i)
	{
		ChildSpaceShapes[i]->GetResourceUrls(OutResourceUrls);
	}
}

void FSpaceShape::GetFileCachePaths(TArray<FString> &OutFileCachePaths)
{
	//分割板件
	if (SplitBoardShape.IsValid())
	{
		SplitBoardShape->GetFileCachePaths(OutFileCachePaths);
	}
	//框体
	if (FrameShape.IsValid())
	{
		FrameShape->GetFileCachePaths(OutFileCachePaths);
	}
	//外盖抽屉组
	if (CoveredDrawerGroup.IsValid())
	{
		CoveredDrawerGroup->GetFileCachePaths(OutFileCachePaths);
	}
	//门组
	if (DoorShape.IsValid())
	{
		DoorShape->GetFileCachePaths(OutFileCachePaths);
	}
	// 五金电器组
	if (EmbededElecticalGroup.IsValid())
	{
		EmbededElecticalGroup->GetFileCachePaths(OutFileCachePaths);
	}
	//内部空间
	if (InsideSpace.IsValid())
	{
		InsideSpace->GetFileCachePaths(OutFileCachePaths);
	}
	//底部空间
	if (BottomSpace.IsValid())
	{
		BottomSpace->GetFileCachePaths(OutFileCachePaths);
	}
	//顶层内胆
	if (TopInsideShape.IsValid())
	{
		TopInsideShape->GetFileCachePaths(OutFileCachePaths);
	}
	//子级空间
	for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
	{
		ChildSpaceShapes[i]->GetFileCachePaths(OutFileCachePaths);
	}
}

bool FSpaceShape::SetShapeWidth(float InValue)
{
	float OldValue = GetShapeWidth();
	//1、处理空间自身信息
	//处理基类型录
	bool bResult = FSCTShape::SetShapeWidth(InValue);
	//处理轮廓信息
	bResult = SpaceOutline->SetSpaceWidth(InValue);
	if (!bResult)
	{
		FSCTShape::SetShapeWidth(OldValue);
		return false;
	}

	//2、处理空间内部信息
	//处理分割板件空间
	if (IsSplitBoardSpace())
	{
		switch (DirectType)
		{
			case SDT_XDirect:
			{
				//SplitBoardShape->SetShapeHeight(InValue);
				bResult = true;
				break;
			}
			case SDT_YDirect:
			{
				bResult = SplitBoardShape->SetShapeDepth(InValue);
				break;
			}
			case SDT_ZDirect:
			{
				bResult = SplitBoardShape->SetShapeWidth(InValue);
				break;
			}
			default:
				check(false);
				break;
		}
	}

	//处理外盖抽屉空间
	if (IsDrawerGroupSpace())
	{
		//处理框体信息
		if (FrameShape.IsValid())
		{
			bResult = FrameShape->UpdateAllFrameShapes();
		}
		if (CoveredDrawerGroup.IsValid())
		{
			UpdateCoveredDrawerGroup();
		}
	}

	// 电器饰条组
	if (IsEmbededElectricalGroup())
	{
		//处理框体信息
		if (FrameShape.IsValid())
		{
			bResult = FrameShape->UpdateAllFrameShapes();
		}
		if (EmbededElecticalGroup.IsValid())
		{
			UpdateEmbededElectricalGroup();
		}
	}

	//处理框体空间
	if (IsFrameShapeSpace())
	{
		//处理框体信息
		FVector InsidePosition;
		FVector InsideDimention;
		if (FrameShape.IsValid())
		{
			bResult = FrameShape->UpdateAllFrameShapes();
			FrameShape->CalInsideSpacePosDim(InsidePosition, InsideDimention);
		}
		//TODO：处理门信息
		if (DoorShape.IsValid())
		{
			bResult = UpdateDoorGroup();
		}
		//处理内部空间
		if (InsideSpace.IsValid())
		{
			//注意内胆的尺寸需要通过框体重新计算出来
			bResult = InsideSpace->SetShapeWidth(InsideDimention.X/*InValue*/);
		}
	}

	//处理底部空间
	if (BottomSpace.IsValid())
	{
		bResult = BottomSpace->SetShapeWidth(InValue);
		check(ChildSpaceShapes.Num() == 1);
		bResult = ChildSpaceShapes[0]->SetShapeWidth(InValue);
	}

	//处理子级空间信息
	else if (IsChildrenSpace())
	{
		//子空间分割方向为X轴方向
		if (SDT_XDirect == ChildrenSplitDirect)
		{
			bResult = ScaleChildrenWidth(InValue);
		}
		else //当前内胆为纵向，分别独立处理每个子内胆
		{
			for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
			{
				bResult = ChildSpaceShapes[i]->SetShapeWidth(InValue);
				check(bResult);  //需要处理，增加获取空间尺寸变化范围的接口，避免出现断言情况
			}
		}
	}
	//处理顶层内胆信息
	if (TopInsideShape.IsValid())
	{
		//处理顶层内胆的位置和尺寸
		FVector UnitShapePosition;
		FVector UnitShapeDimention;
		FSpaceShape* FrameSpaceShape = StaticCast<FSpaceShape*>(ParentShape);
		FrameSpaceShape->CalUnitShapePosDim(UnitShapePosition, UnitShapeDimention);
		bResult = TopInsideShape->SetShapeWidth(UnitShapeDimention.X);
	}
	
	//3、更新Actors
	if (bResult)
	{
		//处理框体位置和尺寸
		if (SpaceShapeActor)
		{
			SpaceShapeActor->UpdateActorDimension();
		}
	}
	
	return bResult;
}

bool FSpaceShape::SetShapeDepth(float InValue)
{
	float OldValue = GetShapeDepth();
	//1、处理空间自身信息
	//处理基类信息
	bool bResult = FSCTShape::SetShapeDepth(InValue);
	//处理轮廓信息
	bResult = SpaceOutline->SetSpaceDepth(InValue);
	if (!bResult)
	{
		FSCTShape::SetShapeDepth(OldValue);
		return false;
	}

	//2、处理内部信息
	//处理分割板件空间
	if (IsSplitBoardSpace())
	{
		switch (DirectType)
		{
			case SDT_XDirect:
			{
				SplitBoardShape->SetShapeDepth(InValue);
				break;
			}
			case SDT_YDirect:
			{
				//SplitBoardShape->SetShapeHeight(InValue);
				break;
			}
			case SDT_ZDirect:
			{
				SplitBoardShape->SetShapeDepth(InValue);
				break;
			}
			default:
				check(false);
				break;
		}
	}

	//处理外盖抽屉空间
	if (IsDrawerGroupSpace())
	{
		//处理框体信息
		if (FrameShape.IsValid())
		{
			bResult = FrameShape->UpdateAllFrameShapes();
		}
		if (CoveredDrawerGroup.IsValid())
		{
			UpdateCoveredDrawerGroup();
		}
	}

	//处理电器饰条组空间
	if (IsEmbededElectricalGroup())
	{
		//处理框体信息
		if (FrameShape.IsValid())
		{
			bResult = FrameShape->UpdateAllFrameShapes();
		}
		if (EmbededElecticalGroup.IsValid())
		{
			UpdateEmbededElectricalGroup();
		}
	}

	//处理框体空间
	if (IsFrameShapeSpace())
	{
		//TODO：处理框体信息
		FVector InsidePosition;
		FVector InsideDimention;
		if (FrameShape.IsValid())
		{
			bResult = FrameShape->UpdateAllFrameShapes();
			FrameShape->CalInsideSpacePosDim(InsidePosition, InsideDimention);
		}
		//TODO：处理门信息
		if (DoorShape.IsValid())
		{
			bResult = UpdateDoorGroup();
		}
		//处理内部空间
		if (InsideSpace.IsValid())
		{
			//注意内胆的尺寸需要通过框体重新计算出来
			bResult = InsideSpace->SetShapeDepth(InsideDimention.Y/*InValue*/);
		}
	}

	//处理底部空间
	if (BottomSpace.IsValid())
	{
		float DoorRetract = BottomSpace->GetDoorGroupRetractValue();
		bResult = BottomSpace->SetShapeDepth(InValue - DoorRetract);
		check(ChildSpaceShapes.Num() == 1);
		bResult = ChildSpaceShapes[0]->SetShapeDepth(InValue);
	}
	//处理子级空间
	else if (IsChildrenSpace())
	{
		//子空间分割方向为X轴方向
		if (SDT_YDirect == ChildrenSplitDirect)
		{
			bResult = ScaleChildrenDepth(InValue);
		}
		else //当前内胆为纵向，分别独立处理每个子内胆
		{
			for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
			{
				bResult = ChildSpaceShapes[i]->SetShapeDepth(InValue);
				check(bResult);  //需要处理，增加获取空间尺寸变化范围的接口，避免出现断言情况
			}
		}
	}
	//处理顶层内胆
	if (TopInsideShape.IsValid())
	{
		//处理顶层内胆的位置和尺寸
		FVector UnitShapePosition;
		FVector UnitShapeDimention;
		FSpaceShape* FrameSpaceShape = StaticCast<FSpaceShape*>(ParentShape);
		FrameSpaceShape->CalUnitShapePosDim(UnitShapePosition, UnitShapeDimention);	
		// 避免内胆出现小于0的深度 -- 划分一个小的深度空间专门放置门板，在进行门板替换时候可能会出现内胆深度计算值为0的情况
		bResult = TopInsideShape->SetShapeDepth(UnitShapeDimention.Y < 0.0f ? 0.0f : UnitShapeDimention.Y);
	}
	
	//3、更新Actors
	if (bResult)
	{
		//处理框体位置和尺寸
		if (SpaceShapeActor)
		{
			SpaceShapeActor->UpdateActorDimension();
		}
	}

	return bResult;
}

bool FSpaceShape::SetShapeHeight(float InValue)
{
	float OldValue = GetShapeHeight();
	//1、处理空间自身信息
	//处理基类信息
	bool bResult = FSCTShape::SetShapeHeight(InValue);
	//处理轮廓信息
	bResult = SpaceOutline->SetSpaceHeight(InValue);
	if (!bResult)
	{
		FSCTShape::SetShapeHeight(OldValue);
		return false;
	}

	//2、处理内部信息
	//处理分割板件空间
	if (IsSplitBoardSpace())
	{
		switch (DirectType)
		{
			case SDT_XDirect:
			{
				bResult = SplitBoardShape->SetShapeWidth(InValue);
				break;
			}
			case SDT_YDirect:
			{
				bResult = SplitBoardShape->SetShapeWidth(InValue);
				break;
			}
			case SDT_ZDirect:
			{
				//SplitBoardShape->SetShapeHeight(InValue);
				bResult = true;
				break;
			}
			default:
				check(false);
				break;
		}
	}

	//处理外盖抽屉空间
	if (IsDrawerGroupSpace())
	{
		//处理框体信息
		if (FrameShape.IsValid())
		{
			bResult = FrameShape->UpdateAllFrameShapes();
		}
		if (CoveredDrawerGroup.IsValid())
		{
			UpdateCoveredDrawerGroup();
		}
	}

	//处理电器饰条组空间
	if (IsEmbededElectricalGroup())
	{
		//处理框体信息
		if (FrameShape.IsValid())
		{
			bResult = FrameShape->UpdateAllFrameShapes();
		}
		if (EmbededElecticalGroup.IsValid())
		{
			UpdateEmbededElectricalGroup();
		}
	}

	//处理框体空间信息
	if (IsFrameShapeSpace())
	{
		//TODO：处理框体信息
		FVector InsidePosition;
		FVector InsideDimention;
		if (FrameShape.IsValid())
		{
			bResult = FrameShape->UpdateAllFrameShapes();
			FrameShape->CalInsideSpacePosDim(InsidePosition, InsideDimention);
		}
		//TODO：处理门信息
		if (DoorShape.IsValid())
		{
			bResult = UpdateDoorGroup();
		}
		//处理内部空间
		if (InsideSpace.IsValid())
		{
			//注意内胆的尺寸需要通过框体重新计算出来
			bResult = InsideSpace->SetShapeHeight(InsideDimention.Z/*InValue*/);
		}
	}

	//处理底部空间
	if (BottomSpace.IsValid())
	{
		float BottomHeight = BottomSpace->GetShapeHeight();
		check(ChildSpaceShapes.Num() == 1);
		bResult = ChildSpaceShapes[0]->SetShapeHeight(InValue - BottomHeight);
	}

	//处理子级空间
	else if (IsChildrenSpace())
	{
		//子空间分割方向为X轴方向
		if (SDT_ZDirect == ChildrenSplitDirect)
		{
			bResult = ScaleChildrenHeight(InValue);
		}
		else //当前内胆为纵向，分别独立处理每个子内胆
		{
			for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
			{
				bResult = ChildSpaceShapes[i]->SetShapeHeight(InValue);
				check(bResult);  //需要处理，增加获取空间尺寸变化范围的接口，避免出现断言情况
			}
		}
	}
	//处理顶层内胆信息
	if (TopInsideShape.IsValid())
	{
		//处理顶层内胆的尺寸
		FVector UnitShapePosition;
		FVector UnitShapeDimention;
		FSpaceShape* FrameSpaceShape = StaticCast<FSpaceShape*>(ParentShape);
		FrameSpaceShape->CalUnitShapePosDim(UnitShapePosition, UnitShapeDimention);
		bResult = TopInsideShape->SetShapeHeight(UnitShapeDimention.Z);
	}
	
	//3、更新Actors
	if (bResult)
	{
		//处理框体位置和尺寸
		if (SpaceShapeActor)
		{
			SpaceShapeActor->UpdateActorDimension();
		}
	}

	return bResult;
}

bool FSpaceShape::RebuildSpaceWidth()
{
	bool RetResult = false;
	do
	{
		RetResult = IsChildrenSpace();
		if (!RetResult) break;
		RetResult = SDT_XDirect == ChildrenSplitDirect;
		if (!RetResult) break;
		RetResult = ScaleChildrenWidth(GetShapeWidth());		
	} while (false);
	return RetResult;
}

bool FSpaceShape::RebuildSpaceHeight()
{
	bool RetResult = false;
	do
	{
		RetResult = IsChildrenSpace();
		if (!RetResult) break;
		RetResult = SDT_ZDirect == ChildrenSplitDirect;
		if (!RetResult) break;
		RetResult = ScaleChildrenHeight(GetShapeHeight());
	} while (false);
	return RetResult;
}

bool FSpaceShape::RebuildSpaceDepth()
{
	bool RetResult = false;
	do
	{
		RetResult = IsChildrenSpace();
		if (!RetResult) break;
		RetResult = SDT_YDirect == ChildrenSplitDirect;
		if (!RetResult) break;
		RetResult = ScaleChildrenDepth(GetShapeDepth());
	} while (false);
	return RetResult;
}

float FSpaceShape::GetShapeWidthRange(float& OutMinValue, float& OutMaxValue)
{
	check(false);
	if (IsSplitBoardSpace())
	{

	}
	return 0.0;
}

float FSpaceShape::GetShapeDepthRange(float& OutMinValue, float& OutMaxValue)
{
	check(false);
	return 0.0;
}

float FSpaceShape::GetShapeHeightRange(float& OutMinValue, float& OutMaxValue)
{
	check(false);
	return 0.0;
}

bool FSpaceShape::IsValidForWidth(float InValue)
{
	//1、验证自身限制
	//处理轮廓信息
	bool bResult = SpaceOutline->IsValidForWidth(InValue);
	if (!bResult)
	{
		return false;
	}

	//2、验证内部限制
	//验证分割板件空间
	if (IsSplitBoardSpace())
	{
		//TODO：验证板件有效性
	}
	//验证外盖抽屉组
	if (IsDrawerGroupSpace())
	{
		bool bResult = CoveredDrawerGroup->IsValidForWidth(InValue);
		if (!bResult)
		{
			return false;
		}
	}
	//TODO: 验证电器饰条组

	//验证框体空间
	if (IsFrameShapeSpace())
	{
		//TODO：验证框体
		float FrameWidth = 0.0;
		if (FrameShape.IsValid())
		{
			//TODO：验证框体有效性
			//根据左板右板情况计算FrameWidth
		}
		//TODO：验证门
		if (DoorShape.IsValid())
		{
			float MinDoorValue = 0.0;
			float MaxDoorValue = 0.0;
			float CurDoorWidth = DoorShape->GetShapeWidthRange(MinDoorValue, MaxDoorValue);	
			if (InValue < MinDoorValue || InValue > MaxDoorValue)
			{
				return false;
			}
		}
		//验证内部空间
		if (InsideSpace.IsValid())
		{
			bResult = InsideSpace->IsValidForWidth(InValue - FrameWidth);
			if (!bResult)
			{
				return false;
			}
		}
	}

	//验证子级空间
	if (IsChildrenSpace())
	{
		//验证底部空间
		if (BottomSpace.IsValid())
		{
			bResult = BottomSpace->IsValidForWidth(InValue);
			if (!bResult)
			{
				return false;
			}
			check(ChildSpaceShapes.Num() == 1);
			bResult = ChildSpaceShapes[0]->IsValidForWidth(InValue);
			if (!bResult)
			{
				return false;
			}
		}
		else
		{
			if (SDT_XDirect == ChildrenSplitDirect)
			{
				//TODO: 
			}
			else
			{
				for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
				{
					bResult = ChildSpaceShapes[i]->IsValidForWidth(InValue);
					if (!bResult)
					{
						return false;
					}
				}
			}
		}
	}

	//验证顶层内胆
	if (TopInsideShape.IsValid())
	{
		//TODO：验证顶层内胆有效性
		float MinValue = 0.0;
		float MaxValue = 10000.0;
		float CurValue = TopInsideShape->GetUnitWidthRange(MinValue, MaxValue);
		if (InValue < MinValue || InValue > MaxValue)
		{
			return false;
		}
	}

	return bResult;
}

bool FSpaceShape::IsValidForDepth(float InValue)
{
	//1、验证自身限制
	//处理轮廓信息
	bool bResult = SpaceOutline->IsValidForDepth(InValue);
	if (!bResult)
	{
		return false;
	}

	//2、验证内部限制
	//验证分割板件空间
	if (IsSplitBoardSpace())
	{
		//TODO：验证板件有效性
	}
	//验证外盖抽屉组
	if (IsDrawerGroupSpace())
	{
		bool bResult = CoveredDrawerGroup->IsValidForDepth(InValue);
		if (!bResult)
		{
			return false;
		}
	}
	//TODO: 验证电器饰条组

	//验证框体空间
	else if (IsFrameShapeSpace())
	{
		//TODO：验证门
		float FrameDepth = 0.0;
		if (DoorShape.IsValid())
		{
			//TODO：验证门有效性
			//根据背板前板情况计算FrameDepth
		}
		//TODO：验证框体
		if (FrameShape.IsValid())
		{
			//TODO：验证框体有效性
		}
		//验证内部空间
		if (InsideSpace.IsValid())
		{
			bResult = InsideSpace->IsValidForDepth(InValue - FrameDepth);
			if (!bResult)
			{
				return false;
			}
		}
	}

	//验证子级空间
	if (IsChildrenSpace())
	{
		//验证底部空间
		if (BottomSpace.IsValid())
		{
			bResult = BottomSpace->IsValidForDepth(InValue);
			if (!bResult)
			{
				return false;
			}
			check(ChildSpaceShapes.Num() == 1);
			bResult = ChildSpaceShapes[0]->IsValidForDepth(InValue);
			if (!bResult)
			{
				return false;
			}
		}
		else
		{
			if (SDT_YDirect == ChildrenSplitDirect)
			{
				//TODO: 
			}
			else
			{
				for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
				{
					bResult = ChildSpaceShapes[i]->IsValidForDepth(InValue);
					if (!bResult)
					{
						return false;
					}
				}
			}
		}
	}

	//验证顶层内胆
	if (TopInsideShape.IsValid())
	{
		//TODO：验证顶层内胆有效性
		check(TopInsideShape.IsValid());
		float MinValue = 0.0;
		float MaxValue = 10000.0;
		float CurValue = TopInsideShape->GetUnitDepthRange(MinValue, MaxValue);
		if (InValue < MinValue || InValue > MaxValue)
		{
			return false;
		}
	}
	
	return bResult;
}

bool FSpaceShape::IsValidForHeight(float InValue)
{
	//1、验证自身限制
	//处理轮廓信息
	bool bResult = SpaceOutline->IsValidForHeight(InValue);
	if (!bResult)
	{
		return false;
	}

	//2、验证内部限制
	//验证分割板件空间
	if (IsSplitBoardSpace())
	{
		//TODO：验证板件有效性
	}
	//验证外盖抽屉组
	if (IsDrawerGroupSpace())
	{
		bool bResult = CoveredDrawerGroup->IsValidForHeight(InValue);
		if (!bResult)
		{
			return false;
		}
	}
	//TODO: 验证电器饰条组

	//验证框体空间
	if (IsFrameShapeSpace())
	{
		//TODO：验证门
		if (DoorShape.IsValid())
		{
			float MinDoorValue = 0.0;
			float MaxDoorValue = 0.0;
			float CurDoorWidth = DoorShape->GetShapeHeightRange(MinDoorValue, MaxDoorValue);
			if (InValue < MinDoorValue || InValue > MaxDoorValue)
			{
				return false;
			}
		}
		//TODO：验证框体
		float FrameHeight = 0.0;
		if (FrameShape.IsValid())
		{
			//TODO：验证框体有效性
			//TODO：根据顶板底板的情况计算FrameHeight
		}
		//验证内部空间
		if (InsideSpace.IsValid())
		{
			bResult = InsideSpace->IsValidForHeight(InValue - FrameHeight);
			if (!bResult)
			{
				return false;
			}
		}
	}

	//验证子级空间
	if (IsChildrenSpace())
	{
		//验证底部空间
		if (BottomSpace.IsValid())
		{
			float BottomHeight = BottomSpace->GetShapeHeight();
			check(ChildSpaceShapes.Num() == 1);
			bResult = ChildSpaceShapes[0]->IsValidForHeight(InValue - BottomHeight);
			if (!bResult)
			{
				return false;
			}
		}
		else
		{
			if (SDT_ZDirect == ChildrenSplitDirect)
			{
				//TODO: 
			}
			else
			{
				for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
				{
					bResult = ChildSpaceShapes[i]->IsValidForHeight(InValue);
					if (!bResult)
					{
						return false;
					}
				}
			}
		}
	}
	//验证顶层内胆
	if (TopInsideShape.IsValid())
	{
		//TODO：验证顶层内胆有效性
		check(TopInsideShape.IsValid());
		float MinValue = 0.0;
		float MaxValue = 10000.0;
		float CurValue = TopInsideShape->GetUnitHeightRange(MinValue, MaxValue);
		if (InValue < MinValue || InValue > MaxValue)
		{
			return false;
		}
	}

	return bResult;
}

float FSpaceShape::GetSpaceAbsolutePosX()
{
	float ParentPosX = 0.0;
	if (IsInsideSpace())
	{
		FSCTShape* ParentShape = GetParentShape();
		FSpaceShape* CurParentSpace = StaticCast<FSpaceShape*>(ParentShape);
		ParentPosX = CurParentSpace->GetSpaceAbsolutePosX();
	}
	else if (ParentSpaceShape)
	{
		ParentPosX = ParentSpaceShape->GetSpaceAbsolutePosX();
	}
	return FSCTShape::GetShapePosX() + ParentPosX;
}

float FSpaceShape::GetSpaceAbsolutePosY()
{
	float ParentPosY = 0.0;
	if (IsInsideSpace())
	{
		FSCTShape* ParentShape = GetParentShape();
		FSpaceShape* CurParentSpace = StaticCast<FSpaceShape*>(ParentShape);
		ParentPosY = CurParentSpace->GetSpaceAbsolutePosY();
	}
	else if (ParentSpaceShape)
	{
		ParentPosY = ParentSpaceShape->GetSpaceAbsolutePosY();
	}
	return FSCTShape::GetShapePosY() + ParentPosY;
}

float FSpaceShape::GetSpaceAbsolutePosZ()
{
	float ParentPosZ = 0.0;
	if (IsInsideSpace())
	{
		FSCTShape* ParentShape = GetParentShape();
		FSpaceShape* CurParentSpace = StaticCast<FSpaceShape*>(ParentShape);
		ParentPosZ = CurParentSpace->GetSpaceAbsolutePosZ();
	}
	else if (ParentSpaceShape)
	{
		ParentPosZ = ParentSpaceShape->GetSpaceAbsolutePosZ();
	}
	return FSCTShape::GetShapePosZ() + ParentPosZ;
}

ASCTSpaceActor* FSpaceShape::GetSpaceShapeActor()
{
	return SpaceShapeActor;
}

ASCTSpaceActor* FSpaceShape::SpawnSpaceShapeActor(FName InProfileName)
{
	check(!SpaceShapeActor);
	SpaceShapeActor = GEngine->GetWorldContextFromGameViewport(
		GEngine->GameViewport)->World()->SpawnActor<ASCTSpaceActor>(
			ASCTSpaceActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	SpaceShapeActor->SetShape(this);
	UMaterialInterface* DefaultMaterial = FMaterialManagerInstatnce::GetIns().GetMaterialManagerPtr()->GetColorUnlitTMaterial();
	UMaterialInstanceDynamic* LineDynMaterial = UMaterialInstanceDynamic::Create(DefaultMaterial, nullptr);
	LineDynMaterial->SetVectorParameterValue(TEXT("Emissive"), FLinearColor::Black);
	LineDynMaterial->SetScalarParameterValue(TEXT("Alpha"), 0.8f);
	SpaceShapeActor->SetLineMaterial(LineDynMaterial);
	UMaterialInstanceDynamic* RegionDynMaterial = UMaterialInstanceDynamic::Create(DefaultMaterial, nullptr);
	RegionDynMaterial->SetVectorParameterValue(TEXT("Emissive"), FLinearColor::White);
	RegionDynMaterial->SetScalarParameterValue(TEXT("Alpha"), 0.0f);
	SpaceShapeActor->SetRegionMaterial(RegionDynMaterial);
	SpaceShapeActor->SetCollisionProfileName(InProfileName);
	return SpaceShapeActor;
}

void FSpaceShape::DestroySpaceShapeActor()
{
	if (SpaceShapeActor)
	{
		SpaceShapeActor->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		SpaceShapeActor->Destroy();
		SpaceShapeActor = nullptr;
	}
}

bool FSpaceShape::SetSpaceOutline(TSharedPtr<FSCTOutline> InOutline)
{
	SpaceOutline = InOutline;

	//处理内部空间的轮廓
	if (InsideSpace.IsValid())
	{
		FSCTOutline* NewOutline = new FSCTOutline;
		SpaceOutline->CopyTo(NewOutline);
		InsideSpace->SetSpaceOutline(MakeShareable(NewOutline));
	}

	//处理顶层内胆
	if (TopInsideShape.IsValid())
	{
		//TODO：设置顶层内胆的轮廓信息
	}

	//处理空间轮廓Actor
	if (SpaceShapeActor)
	{
		SpaceShapeActor->UpdateActorDimension();
	}

	return true;
}

TSharedPtr<FSCTOutline> FSpaceShape::GetSpaceOutline() const
{
	return SpaceOutline;
}

bool FSpaceShape::SetOutlineParamValueByRefName(const FString &InRefName, float InValue)
{
	//处理自身轮廓参数
	bool bResult = SpaceOutline->SetOutlineParamValueByRefName(InRefName, InValue);
	if (!bResult)
	{
		return false;
	}

	//处理分割板件轮廓参数
	if (bResult && IsSplitBoardSpace())
	{
		TSharedPtr<FSCTOutline> BoardOutline = SplitBoardShape->GetBoardOutline();
		bResult = BoardOutline->SetOutlineParamValueByRefName(InRefName, InValue);
		if (bResult)
		{
			ASCTShapeActor* BoardActor = SplitBoardShape->GetShapeActor();
			if (BoardActor)
			{
				BoardActor->UpdateActorDimension();
			}
		}
	}

	//处理框体空间
	if (bResult && IsFrameShapeSpace())
	{
		//处理框体板件
		if (FrameShape.IsValid())
		{
			bResult = FrameShape->UpdateAllFrameShapes();
		}
		//处理内部空间
		if (InsideSpace.IsValid())
		{
			bResult = FrameShape->CalInsideOutlineParm(InsideSpace);
		}
	}

	//处理子级空间轮廓参数
	if (bResult && IsChildrenSpace())
	{
		//处理底部空间
		if (BottomSpace.IsValid())
		{
			bResult = BottomSpace->SetOutlineParamValueByRefName(InRefName, InValue);
		}

		for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
		{
			TSharedPtr<FSCTOutline> CurrentOutline = ChildSpaceShapes[i]->GetSpaceOutline();
			if (CurrentOutline->GetOutlineType() != OLT_None)
			{
				bResult = ChildSpaceShapes[i]->SetOutlineParamValueByRefName(InRefName, InValue);
				if (!bResult)
				{
					break;
				}
			}
		}
	}
	
	//处理顶层内胆
	if (bResult && TopInsideShape.IsValid())
	{
		//TODO：更新顶层内胆的轮廓参数
	}

	//处理框体包围盒
	if (bResult && SpaceShapeActor)
	{
		SpaceShapeActor->UpdateActorDimension();
	}

	//若设置失败，还原原来的数值
	if (!bResult)
	{
		//TODO：恢复原来的轮廓参数
	}

	return bResult;
}

TArray<int32> FSpaceShape::CalculateFrameBoardCount()
{
	TArray<int32> resultCount;
	EOutlineType OutlineType = SpaceOutline->GetOutlineType();
	switch (OutlineType)
	{
	case OLT_None:
	{
		resultCount.Add(1); //顶
		resultCount.Add(1); //底
		resultCount.Add(1); //左
		resultCount.Add(1); //右
		resultCount.Add(1); //背
		resultCount.Add(1); //前
		break;
	}
	case OLT_LeftGirder:
	{
		resultCount.Add(2); //顶
		resultCount.Add(1); //底
		resultCount.Add(2); //左
		resultCount.Add(1); //右
		resultCount.Add(1); //背
		resultCount.Add(1); //前
		break;
	}
	case OLT_RightGirder:
	{
		resultCount.Add(2); //顶
		resultCount.Add(1); //底
		resultCount.Add(1); //左
		resultCount.Add(2); //右
		resultCount.Add(1); //背
		resultCount.Add(1); //前
		break;
	}
	case OLT_BackGirder:
	{
		resultCount.Add(2); //顶
		resultCount.Add(1); //底
		resultCount.Add(1); //左
		resultCount.Add(1); //右
		resultCount.Add(2); //背
		resultCount.Add(1); //前
		break;
	}
	case OLT_LeftPiller:
	{
		resultCount.Add(1); //顶
		resultCount.Add(1); //底
		resultCount.Add(2); //左
		resultCount.Add(1); //右
		resultCount.Add(2); //背
		resultCount.Add(1); //前
		break;
	}
	case OLT_RightPiller:
	{
		resultCount.Add(1); //顶
		resultCount.Add(1); //底
		resultCount.Add(1); //左
		resultCount.Add(2); //右
		resultCount.Add(2); //背
		resultCount.Add(1); //前
		break;
	}
	case OLT_MiddlePiller:
	{
		resultCount.Add(1); //顶
		resultCount.Add(1); //底
		resultCount.Add(2); //左
		resultCount.Add(2); //右
		resultCount.Add(3); //背
		resultCount.Add(1); //前
		break;
	}
	default:
		check(false);
		break;
	}

	return resultCount;
}

float FSpaceShape::GetSpaceModifyScale()
{
	if (ScaleValue < 0.0)
	{
		return ScaleValue;
	}

	switch (DirectType)
	{
	case SDT_XDirect:
	{
		ScaleValue = GetShapeWidth();
		break;
	}
	case SDT_YDirect:
	{
		ScaleValue = GetShapeDepth();
		break;
	}
	case SDT_ZDirect:
	{
		ScaleValue = GetShapeHeight();
		break;
	}
	default:
		check(false);
		break;
	}
	return ScaleValue;
};

void FSpaceShape::HiddenSpaceShapeActors(bool bHidden)
{
	//内部空间
	if (InsideSpace.IsValid())
	{
		InsideSpace->HiddenSpaceShapeActors(bHidden);
	}
	//底部空间
	if (BottomSpace.IsValid())
	{
		BottomSpace->HiddenSpaceShapeActors(bHidden);
	}
	//子级空间
	for (int32 i=0; i<ChildSpaceShapes.Num(); ++i)
	{
		ChildSpaceShapes[i]->HiddenSpaceShapeActors(bHidden);
	}
	//分割板件
	if (SplitBoardShape.IsValid())
	{
		ASCTShapeActor* CurSplitBoardActor = SplitBoardShape->GetShapeActor();
		if (CurSplitBoardActor)
		{
			CurSplitBoardActor->SetActorHiddenInGame(bHidden);
		}
	}
	//空间按柜体外框
	if (FrameShape.IsValid())
	{
		FrameShape->HiddenFrameShapeActors(bHidden);
	}
	//抽屉组
	if (CoveredDrawerGroup.IsValid())
	{
		CoveredDrawerGroup->HiddenDrawerGroupShapeActors(bHidden);
	}
	//门组
	if (DoorShape.IsValid())
	{
		DoorShape->HiddenDoorGroupActors(bHidden);
	}
	//顶层内胆
	if (TopInsideShape.IsValid())
	{
		TopInsideShape->HiddenUnitShapeActors(bHidden);
	}
}

FCabinetShape* FSpaceShape::GetCabinetShapeData()
{
	return CabinetShapeData;
}
void FSpaceShape::SetCabinetShapeData(FCabinetShape* InCabinetShape)
{
	CabinetShapeData = InCabinetShape;
	if (InsideSpace.IsValid())
	{
		InsideSpace->SetCabinetShapeData(CabinetShapeData);
	}
	if (BottomSpace.IsValid())
	{
		BottomSpace->SetCabinetShapeData(CabinetShapeData);
	}
	for (int32 i=0; i<ChildSpaceShapes.Num(); ++i)
	{
		ChildSpaceShapes[i]->SetCabinetShapeData(CabinetShapeData);
	}
}

bool FSpaceShape::MakeDefaultSpace()
{
	//默认空间为 框体空间 （包含框体和内部空间）
	SetSpaceSubType(SST_FrameSpace);  //框体空间
	//顶层空间设置内部空间
	FSpaceShape* InsideSpace = new FSpaceShape();
	InsideSpace->SetShapeName(TEXT("内部空间"));
	InsideSpace->SetSpaceSubType(SST_UnitSpace);  //内部空间
	SetInsideSpace(MakeShareable(InsideSpace));

	//顶层空间设置框体和内部空间
	FFrameShape* FrameShape = new FFrameShape();
	FrameShape->SetShapeName(TEXT("框体板件"));
	FrameShape->SetSpaceShapeData(this);
	SetFrameShape(MakeShareable(FrameShape));

	//计算顶层内胆的尺寸
	FVector UnitPosition;
	FVector UnitDimention;
	bool bCalUnit = CalUnitShapePosDim(UnitPosition, UnitDimention);

	//默认内部空间的顶层内胆空间
	TSharedPtr<FUnitShape> NewTopInsideShape = MakeShareable(new FUnitShape);
	NewTopInsideShape->SetShapeName(TEXT("顶层内胆"));
	NewTopInsideShape->SetShapeWidth(UnitDimention.X);
	NewTopInsideShape->SetShapeDepth(UnitDimention.Y);
	NewTopInsideShape->SetShapeHeight(UnitDimention.Z);
	InsideSpace->SetTopInsideShape(NewTopInsideShape);

	return true;
}

void FSpaceShape::EmptySpaceShape()
{
	if (IsSplitBoardSpace())
	{
 		DeleteSplitBoard();
	} 
	else if (IsDrawerGroupSpace())
	{
		DeleteCoveredDrawerGroup();
	}
	else
	{
		//TODO：清空框体板件
		if (FrameShape.IsValid())
		{
			FrameShape->EmptyFrameBoard();
		}
		//TODO：清空门
		if (DoorShape.IsValid())
		{
			DeleteDoorShape();
		}
		//清空内部空间
		if (InsideSpace.IsValid())
		{
			if (InsideSpace->IsChildrenSpace())
			{
				//清空底部空间
				if (InsideSpace->BottomSpace.IsValid())
				{
					InsideSpace->RemoveChildShape(InsideSpace->BottomSpace);
					InsideSpace->BottomSpace = nullptr;
				}
				//清空子级空间
				InsideSpace->EmptyChildrenSpace();

				//计算顶层内胆的尺寸
				FVector UnitPosition;
				FVector UnitDimention;
				bool bCalUnit = CalUnitShapePosDim(UnitPosition, UnitDimention);

				//创建顶层内胆
				TSharedPtr<FUnitShape> NewTopInsideShape = MakeShareable(new FUnitShape);
				NewTopInsideShape->SetShapeName(TEXT("顶层内胆"));
				NewTopInsideShape->SetShapeWidth(UnitDimention.X);
				NewTopInsideShape->SetShapeDepth(UnitDimention.Y);
				NewTopInsideShape->SetShapeHeight(UnitDimention.Z);
				ASCTShapeActor* TopInsideActor = NewTopInsideShape->SpawnShapeActor();
				TopInsideActor->AttachToActorOverride(InsideSpace->GetShapeActor(), FAttachmentTransformRules::KeepRelativeTransform);
				InsideSpace->SetTopInsideShape(NewTopInsideShape);
				//创建轮廓空间包围盒
				SpawnSpaceShapeActor(GetCabinetShapeData()->GetBoundSpaceProfileName());
			}
			else
			{
				InsideSpace->TopInsideShape->EmptyTopInsideShape();
			}
		}
	}
	//更新当前空间
	UpdateSpaceShape();
}

bool FSpaceShape::IsSpaceShapeEmpty()
{
	if (IsSplitBoardSpace())
	{
		return false;
	}
	else if (IsDrawerGroupSpace())
	{
		return false;
	}
	else if (IsEmbededElectricalGroup())
	{
		return false;
	}
	else
	{
		//TODO：判断框体板件是否为空
		if (FrameShape.IsValid())
		{
			bool bFrame = FrameShape->IsFrameBoardEmpty();
			if (!bFrame)
			{
				return false;
			}
		}
		//TODO：判断门是否为空
		if (DoorShape.IsValid())
		{
			return false;
		}
		
		//判断内部空间
		check(InsideSpace.IsValid());
		if (InsideSpace->IsChildrenSpace())
		{
			return false;
			//判断底部空间
// 			if (BottomSpace.IsValid())
// 			{
// 				bool bEmpty = BottomSpace->IsSpaceShapeEmpty();
// 				if (!bEmpty)
// 				{
// 					return false;
// 				}
// 			}
		}
		else
		{
			//TODO：判断顶层内胆是否为空
			bool bEmpty = InsideSpace->TopInsideShape->IsTopInsideShapeEmpty();
			if (!bEmpty)
			{
				return false;
			}
		}
	}
	return true;
}

bool FSpaceShape::IsSplitBoardSpace()
{
	return SpaceSubType == SST_SplitSpace;
	//
	bool bResult = SplitBoardShape.IsValid();
	if (bResult)
	{
		check(!FrameShape.IsValid() && !InsideSpace.IsValid());
	}
	return bResult;
}

bool FSpaceShape::IsFrameShapeSpace()
{
	return SpaceSubType == SST_FrameSpace;
	//
	bool bResult = FrameShape.IsValid() && InsideSpace.IsValid();
	if (bResult)
	{
		check(!SplitBoardShape.IsValid());
	}
	return bResult;
}

bool FSpaceShape::IsDrawerGroupSpace()
{
	return SpaceSubType == SST_DrawerSpace;
	//
	bool bResult = FrameShape.IsValid() && !InsideSpace.IsValid();
	if (bResult)
	{
		check(!SplitBoardShape.IsValid());
	}
	return bResult;
}

bool FSpaceShape::IsEmbededElectricalGroup() const
{
	return SpaceSubType == SST_EmbededElectrical;
}

bool FSpaceShape::IsBottomSpace()
{
	return SpaceSubType == SST_BottomSpace;
}
bool FSpaceShape::IsInsideContainBottomSpace()
{
	if (InsideSpace.IsValid())
	{
		if (InsideSpace->GetBottomSpace().IsValid())
		{
			return true;
		}
	}
	return false;
}

bool FSpaceShape::IsInsideSpace()
{
	return (SpaceSubType == SST_InsideSpace) || 
		(SpaceSubType == SST_ChildrenSpace) || 
		(SpaceSubType == SST_UnitSpace);
	//
	bool bResult = IsChildrenSpace() || IsUnitSpace();
	return bResult;
}

bool FSpaceShape::IsUnitSpace()
{
	return SpaceSubType == SST_UnitSpace;
	//注：归属于内部空间
	return TopInsideShape.IsValid();
}

bool FSpaceShape::IsChildrenSpace()
{
	return SpaceSubType == SST_ChildrenSpace;
	//注：归属于内部空间
	bool bResult = ChildSpaceShapes.Num() > 0;
	if (bResult)
	{
		check(!TopInsideShape.IsValid());
	}
	return bResult;
}

void FSpaceShape::EmptyChildrenSpace()
{
	//清除子级空间
	for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
	{
		RemoveChildShape(ChildSpaceShapes[i]);
	}
	ChildSpaceShapes.Empty();
}

bool FSpaceShape::IsInsideSpaceEmpty()
{
	if (!IsFrameShapeSpace())
	{
		//check(false);
		return false;
	}

	bool bChildrenSpace = InsideSpace->IsChildrenSpace();
	if (bChildrenSpace)
	{
		return false;
	}

	TSharedPtr<FUnitShape> CurUnitShape = InsideSpace->GetTopInsideShape();
	bool bUnitShape = CurUnitShape->IsTopInsideShapeEmpty();
	return bUnitShape;
}

void FSpaceShape::EmptyInsideSpace()
{
	if (!IsFrameShapeSpace())
	{
		check(false);
		return;
	}

	bool bChildrenSpace = InsideSpace->IsChildrenSpace();
	if (bChildrenSpace)
	{
		InsideSpace->EmptyChildrenSpace();
		InsideSpace->SetSpaceSubType(SST_UnitSpace);  //内部空间

		//计算顶层内胆的尺寸
		FVector UnitPosition;
		FVector UnitDimention;
		bool bCalUnit = CalUnitShapePosDim(UnitPosition, UnitDimention);

		//默认内部空间的顶层内胆空间
		TSharedPtr<FUnitShape> NewTopInsideShape = MakeShareable(new FUnitShape);
		NewTopInsideShape->SetShapeName(TEXT("顶层内胆"));
		NewTopInsideShape->SetShapeWidth(UnitDimention.X);
		NewTopInsideShape->SetShapeDepth(UnitDimention.Y);
		NewTopInsideShape->SetShapeHeight(UnitDimention.Z);
		InsideSpace->SetTopInsideShape(NewTopInsideShape);
		return;
	}

	TSharedPtr<FUnitShape> CurUnitShape = InsideSpace->GetTopInsideShape();
	bool bUnitShapeEmpty = CurUnitShape->IsTopInsideShapeEmpty();
	if (!bUnitShapeEmpty)
	{
		CurUnitShape->DeleteTopTinsideShape();
	}
}

bool FSpaceShape::UpdateSpaceShape()
{
	//空间尺寸修改需要调用该接口
	//1、分割板件空间
	if (IsSplitBoardSpace())
	{
		return true;
	}
	//2、外盖抽屉空间
	else if (IsDrawerGroupSpace())
	{
		//1、更新框体
		bool bFrame = UpdateFrameBoard();
		//2、更新外盖抽屉组
		bool CoverDrawer = UpdateCoveredDrawerGroup();
	}
	// 3、 电器组空间
	else if (IsEmbededElectricalGroup())
	{
		//1、更新框体
		bool bFrame = UpdateFrameBoard();
		//2、更新电器抽屉组
		bool RetResult = UpdateEmbededElectricalGroup();		
		
	}
	//4、框体空间
	else
	{
		//2、更新门组
		bool bDoor = UpdateDoorGroup();

		//1、更新框体
		bool bFrame = UpdateFrameBoard();
		
		//3、更新内部空间
		bool bInside = UpdateInsideSpace();
	}
	return true;
}

void FSpaceShape::SetTopInsideShape(TSharedPtr<FUnitShape> InShape)
{
	TopInsideShape = InShape;
	//设置父子关系
	TopInsideShape->SetParentShape(this);
	AddChildShape(TopInsideShape);
	//设置空间类型
	SetSpaceSubType(SST_UnitSpace);

	FCabinetShape* CurCabinetShape = GetCabinetShapeData();
	//设置顶层内胆中活层内缩值
	float ActiveBoardRetract = CurCabinetShape->GetActiveRetractValue();
	TopInsideShape->SetActiveRetractValue(ActiveBoardRetract);
	//设置顶层内胆的ProfileName
	FName CurSpaceProfile = CurCabinetShape->GetShapeSpaceProfileName();
	FName CurBoundProfile = CurCabinetShape->GetBoundSpaceProfileName();
	TopInsideShape->SpawnActorsForSelected(CurSpaceProfile, CurBoundProfile);
}

void FSpaceShape::DeleteTopInsideShape()
{
	RemoveChildShape(TopInsideShape);
	TopInsideShape = nullptr;
}

float FSpaceShape::GetUnitShapeRetractValue()
{
	//return RetractValue;
	check(CabinetShapeData);
	return CabinetShapeData->GetInsideRetractValue();
}

bool FSpaceShape::SetUnitShapeRetractValue(float InValue)
{
	//RetractValue = InValue;
	//递归设置子空间的内胆内缩值
	if (InsideSpace.IsValid())
	{
		InsideSpace->SetUnitShapeRetractValue(InValue);
	}
	if (BottomSpace.IsValid())
	{
		BottomSpace->SetUnitShapeRetractValue(InValue);
	}
	for (int32 i=0; i<ChildSpaceShapes.Num(); ++i)
	{
		ChildSpaceShapes[i]->SetUnitShapeRetractValue(InValue);
	}
	//更新当前空间
	UpdateSpaceShape();
	return true;
}

int32 FSpaceShape::GetUnitShapeRetractType()
{
	return RetractType;
}

void FSpaceShape::SetUnitShapeRetractType(int32 InType)
{
	RetractType = InType;
	//更新当前空间
	UpdateSpaceShape();
}

bool FSpaceShape::CalUnitShapePosDim(FVector& UnitPosition, FVector& UnitDimention)
{
	//框体空间调用该函数
	check(IsFrameShapeSpace() || IsBottomSpace());
	check(FrameShape.IsValid());
	check(InsideSpace.IsValid());
	
	bool bHasFrontBoard = false;
	FSpaceShape * TempPtr = this;
	do
	{
		if (TempPtr->GetFrameBoard(5, 0).IsValid())
		{
			bHasFrontBoard = true;
			break;
		}
		else
		{
			TempPtr = TempPtr->GetParentSpaceShape();
		}

	} while (TempPtr);

	//首先计算内部空间的位置和尺寸
	bool bInsideSpace = FrameShape->CalInsideSpacePosDim(UnitPosition, UnitDimention);

	//门组或外盖抽屉引起的顶层内胆内缩值
	float DoorGroupRetractValue = GetDoorGroupRetractValue();	
	//柜体工艺引起的顶层内胆内缩值
	float InsideRetractValue = CabinetShapeData->GetInsideRetractValue();
	//顶层内胆内缩值
	float RetractValue = DoorGroupRetractValue + InsideRetractValue;
	//根据内胆内缩方向在内部空间的位置和尺寸基础上修改
	switch (RetractType)
	{
		case 0:  //顶
		{
			UnitDimention.Z -= RetractValue;
			break;
		}
		case 1:  //底
		{
			UnitPosition.Z += RetractValue;
			UnitDimention.Z -= RetractValue;
			break;
		}
		case 2:  //左
		{
			UnitPosition.X += RetractValue;
			UnitDimention.X -= RetractValue;
			break;
		}
		case 3:  //右
		{
			UnitDimention.X -= RetractValue;
			break;
		}
		case 4:  //后
		{
			UnitPosition.Y += RetractValue;
			UnitDimention.Y -= RetractValue;
			break;
		}
		case 5:  //前
		{
			
			UnitDimention.Y -= bHasFrontBoard ? 0 : RetractValue;
			break;
		}
		default:
			break;
	}
	return true;
}

bool FSpaceShape::SetUnitShapeActiveRetract(float InValue)
{
	//设置给顶层内胆
	if (TopInsideShape.IsValid())
	{
		TopInsideShape->SetActiveRetractValue(InValue);
	}
	//递归设置子空间的顶层内胆
	if (InsideSpace.IsValid())
	{
		InsideSpace->SetUnitShapeActiveRetract(InValue);
	}
	if (BottomSpace.IsValid())
	{
		BottomSpace->SetUnitShapeActiveRetract(InValue);
	}
	for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
	{
		ChildSpaceShapes[i]->SetUnitShapeActiveRetract(InValue);
	}
	return true;
}

bool FSpaceShape::AddDoorCoverRetractValue(float InValue)
{
	if (IsSplitBoardSpace())
	{
		AddSplitBoardFrontExtension(-InValue);
	}
	else
	{
		//设置当前空间的内胆内缩值
		if (DoorCoverRetractValue < InValue)
		{
			DoorCoverRetractValue = InValue;
		}
		++DoorCoverRetractCount;
	}

	//递归设置子级空间的内胆内缩值
	if (InsideSpace.IsValid() && InsideSpace->ChildSpaceShapes.Num() > 0)
	{
		TArray<TSharedPtr<FSpaceShape>>& CurChildSpaceShapes = InsideSpace->GetChildSpaceShapes();
		for (auto it : CurChildSpaceShapes)
		{
			it->AddDoorCoverRetractValue(InValue);
		}
		TSharedPtr<FSpaceShape> CurBottomSpace = InsideSpace->GetBottomSpace();
		if (CurBottomSpace.IsValid())
		{
			CurBottomSpace->AddDoorCoverRetractValue(InValue);
		}
	}
	return true;
}

void FSpaceShape::RemoveDoorCoverRetractValue()
{
	if (IsSplitBoardSpace())
	{
		RemoveSplitBoardFrontExtension();
	}
	else
	{
		//移除当前空间的内胆内缩值
		--DoorCoverRetractCount;
		if (DoorCoverRetractCount == 0)
		{
			DoorCoverRetractValue = 0.0;
		}
	}
	
	//递归移除子级空间的内胆内缩值
	if (InsideSpace.IsValid() && InsideSpace->ChildSpaceShapes.Num() > 0)
	{
		TArray<TSharedPtr<FSpaceShape>>& CurChildSpaceShapes = InsideSpace->GetChildSpaceShapes();
		for (auto it : CurChildSpaceShapes)
		{
			it->RemoveDoorCoverRetractValue();
		}
		TSharedPtr<FSpaceShape> CurBottomSpace = InsideSpace->GetBottomSpace();
		if (CurBottomSpace.IsValid())
		{
			CurBottomSpace->RemoveDoorCoverRetractValue();
		}
	}
}

bool FSpaceShape::AddDoorNestedRetractValue(float InValue)
{
	if (IsSplitBoardSpace())
	{
		AddSplitBoardFrontExtension(-InValue);
	} 
	else
	{
		//设置当前空间的内胆内缩值
		if (DoorNestedRetractValue < InValue)
		{
			DoorNestedRetractValue = InValue;
		}
		++DoorNestedRetractCount;
	}

	//递归设置子级空间的内胆内缩值
	if (InsideSpace.IsValid() && InsideSpace->ChildSpaceShapes.Num() > 0)
	{
		TArray<TSharedPtr<FSpaceShape>>& CurChildSpaceShapes = InsideSpace->GetChildSpaceShapes();
		for (auto it : CurChildSpaceShapes)
		{
			it->AddDoorNestedRetractValue(InValue);
		}
		TSharedPtr<FSpaceShape> CurBottomSpace = InsideSpace->GetBottomSpace();
		if (CurBottomSpace.IsValid())
		{
			CurBottomSpace->AddDoorNestedRetractValue(InValue);
		}
	}
	return true;
}

void FSpaceShape::RemoveDoorNestedRetractValue()
{
	if (IsSplitBoardSpace())
	{
		RemoveSplitBoardFrontExtension();
	} 
	else
	{
		//设置当前空间的内胆内缩值
		--DoorNestedRetractCount;
		if (DoorNestedRetractCount == 0)
		{
			DoorNestedRetractValue = 0.0;
		}
	}
	
	//递归移除子级空间的内胆内缩值
	if (InsideSpace.IsValid() && InsideSpace->ChildSpaceShapes.Num() > 0)
	{
		TArray<TSharedPtr<FSpaceShape>>& CurChildSpaceShapes = InsideSpace->GetChildSpaceShapes();
		for (auto it : CurChildSpaceShapes)
		{
			it->RemoveDoorNestedRetractValue();
		}
		TSharedPtr<FSpaceShape> CurBottomSpace = InsideSpace->GetBottomSpace();
		if (CurBottomSpace.IsValid())
		{
			CurBottomSpace->RemoveDoorNestedRetractValue();
		}
	}
}

void FSpaceShape::SetSplitBoard(TSharedPtr<FBoardShape> InShape)
{
	SplitBoardShape = InShape;
	//设置父子关系
	SplitBoardShape->SetParentShape(this);
	AddChildShape(SplitBoardShape);
	
	//设置空间类型
	SetSpaceSubType(SST_SplitSpace);
	SetShapeName(TEXT("分割板件"));

	//设置板件轮廓信息
	FSCTOutline* NewOutline = new FSCTOutline;
	SpaceOutline->CopyTo(NewOutline);
	SplitBoardShape->SetBoardOutline(MakeShareable(NewOutline));

	//清除框体和内部空间
	if (FrameShape.IsValid())
	{
		DeleteFrameShape();
	}
	if (InsideSpace.IsValid())
	{
		DeleteInsideSpace();
	}
}

void FSpaceShape::DeleteSplitBoard()
{
	RemoveChildShape(SplitBoardShape);
	SplitBoardShape = nullptr;
	//恢复为默认的框体空间
	MakeDefaultSpace();
}

bool FSpaceShape::AddSplitBoardFrontExtension(float InValue)
{
	if (!SplitBoardShape.IsValid())
	{
		return false;
	}
	//设置板件外延值
	if (FrontExtensionCount == 0)
	{
		SplitBoardShape->SetShapeFrontExpand(InValue);
	}
	//外延计数加一
	++FrontExtensionCount;
	return true;
}
void FSpaceShape::RemoveSplitBoardFrontExtension()
{
	if (!SplitBoardShape.IsValid())
	{
		return;
	}
	//外延计数减一
	--FrontExtensionCount;
	//设置板件外延值
	if (FrontExtensionCount == 0)
	{
		SplitBoardShape->SetShapeFrontExpand(0.0);
	}
}

TSharedPtr<FDrawerGroupShape> FSpaceShape::GetCoveredDrawerGroup()
{
	return CoveredDrawerGroup;
}

void FSpaceShape::SetCoveredDrawerGroup(TSharedPtr<FDrawerGroupShape> InDrawerGroup)
{
	CoveredDrawerGroup = InDrawerGroup;
	//设置父子关系
	CoveredDrawerGroup->SetParentShape(this);
	AddChildShape(CoveredDrawerGroup);
	//设置空间类型
	SetSpaceSubType(SST_DrawerSpace);

	//清楚内部空间
	if (InsideSpace.IsValid())
	{
		DeleteInsideSpace();
	}
}

void FSpaceShape::DeleteCoveredDrawerGroup()
{
	//取消抽屉组添加对框体板件和空间的影响
	CancelInsetedBoardInfluences();
	CancelCoveredBoardInfluences();
	CancelCoveredSpaceInfluences();
	ClearDoorGroupInfluences();

	//删除外盖抽屉组
	RemoveChildShape(CoveredDrawerGroup);
	CoveredDrawerGroup = nullptr;

	//恢复为默认的框体空间
	SetSpaceSubType(SST_FrameSpace);
	//顶层空间设置内部空间
	FSpaceShape* InsideSpace = new FSpaceShape();
	InsideSpace->SetShapeName(TEXT("内部空间"));
	InsideSpace->SetSpaceSubType(SST_UnitSpace);  //内部空间
	SetInsideSpace(MakeShareable(InsideSpace));

	//计算顶层内胆的尺寸
	FVector UnitPosition;
	FVector UnitDimention;
	bool bCalUnit = CalUnitShapePosDim(UnitPosition, UnitDimention);

	//默认内部空间的顶层内胆空间
	TSharedPtr<FUnitShape> NewTopInsideShape = MakeShareable(new FUnitShape);
	NewTopInsideShape->SetShapeName(TEXT("顶层内胆"));
	NewTopInsideShape->SetShapeWidth(UnitDimention.X);
	NewTopInsideShape->SetShapeDepth(UnitDimention.Y);
	NewTopInsideShape->SetShapeHeight(UnitDimention.Z);
	InsideSpace->SetTopInsideShape(NewTopInsideShape);
}

bool FSpaceShape::AddCoveredDrawerGroup(TSharedPtr<FDrawerGroupShape> InDrawerGroup)
{
	//1、框体空间中添加掩门门组
	check(IsFrameShapeSpace());
	//2、清楚所有门组影响的框体板件和空间缓存
	ClearDoorGroupInfluences();
	//3、添加门组适配当前空间
	bool bFitDrawerGroup = FitCoveredDrawerGroupToCurSpaceShape(InDrawerGroup.Get());
	if (!bFitDrawerGroup)
	{
		return false;
	}

	//4、设置门组
	SetCoveredDrawerGroup(InDrawerGroup);
	//5、创建外盖抽屉组Actor
	ASCTShapeActor* NewDrawerShapeActor = InDrawerGroup->SpawnShapeActor();
	NewDrawerShapeActor->AttachToActorOverride(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
	//创建型录外包框和包围盒
	FName ShapeSpaceProfileName = CabinetShapeData->GetShapeSpaceProfileName();
	InDrawerGroup->SpawnActorsForSelected(ShapeSpaceProfileName);

	return true;
}

bool FSpaceShape::UpdateEmbededElectricalGroup()
{
	if (!EmbededElecticalGroup.IsValid())
	{
		return true;
	}
	check(IsEmbededElectricalGroup());
	EmbededElecticalGroup->UpdateEmbededEletricalShape();

	//取消之前门组影响的所有框体板件和空间
	CancelInsetedBoardInfluences();
	CancelCoveredBoardInfluences();
	CancelCoveredSpaceInfluences();
	ClearDoorGroupInfluences();

	bool bResult = false;
	//根据抽屉组类型以及掩盖类型，更新抽屉组尺寸和位置
	bResult = FitEmbededElectricalGroupToCurSpaceShape(EmbededElecticalGroup.Get());
	return bResult;
}

bool FSpaceShape::UpdateCoveredDrawerGroup()
{
	if (!CoveredDrawerGroup.IsValid())
	{
		return true;
	}
	check(IsDrawerGroupSpace());

	//取消之前门组影响的所有框体板件和空间
	CancelInsetedBoardInfluences();
	CancelCoveredBoardInfluences();
	CancelCoveredSpaceInfluences();
	ClearDoorGroupInfluences();

	bool bResult = false;
	//根据抽屉组类型以及掩盖类型，更新抽屉组尺寸和位置
	bResult = FitCoveredDrawerGroupToCurSpaceShape(CoveredDrawerGroup.Get());
	return bResult;
}

void FSpaceShape::SetEmbededElectricalGroup(TSharedPtr<FEmbededElectricalGroup> InEmbededElectricalGroup)
{
	EmbededElecticalGroup = InEmbededElectricalGroup;
	//设置父子关系
	EmbededElecticalGroup->SetParentShape(this);
	AddChildShape(EmbededElecticalGroup);
	//设置空间类型
	SetSpaceSubType(SST_EmbededElectrical);

	//清除内部空间
	if (InsideSpace.IsValid())
	{
		DeleteInsideSpace();
	}
}

bool FSpaceShape::AddEmbededElectricalGroup(TSharedPtr<FEmbededElectricalGroup> InEmbededElectricalGroup)
{
	//1、框体空间中添加掩门门组
	check(IsFrameShapeSpace());

	// 清除已有的电器组影响的框体板件和空间缓存
	CancelInsetedBoardInfluences();
	CancelCoveredBoardInfluences();
	CancelCoveredSpaceInfluences();
	ClearDoorGroupInfluences();
	//3、添加电器组适配当前空间
	bool bFitDrawerGroup = FitEmbededElectricalGroupToCurSpaceShape(InEmbededElectricalGroup.Get());
	if (!bFitDrawerGroup)
	{
		return false;
	}

	//4、设置电器组
	SetEmbededElectricalGroup(InEmbededElectricalGroup);
	//5、创建电子组Actor
	ASCTShapeActor* NewShapeActor = InEmbededElectricalGroup->SpawnShapeActor();
	NewShapeActor->AttachToActorOverride(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
	//创建型录外包框和包围盒
	FName ShapeSpaceProfileName = CabinetShapeData->GetShapeSpaceProfileName();
	InEmbededElectricalGroup->SpawnActorsForSelected(ShapeSpaceProfileName);	
	return true;
}

bool FSpaceShape::RecalEmbedElectricalGroupInfluences()
{
	check(EmbededElecticalGroup.IsValid());
	//取消电器组添加对框体板件和空间的影响	
	CancelInsetedBoardInfluences();
	CancelCoveredBoardInfluences();
	CancelCoveredSpaceInfluences();
	ClearDoorGroupInfluences();
	bool RetResult = true;
	RetResult = FitEmbededElectricalGroupToCurSpaceShape(EmbededElecticalGroup.Get());
	if (RetResult)
	{
		EmbededElecticalGroup->UpdateEmbededEletricalShape();
	}
	return RetResult;
}

void FSpaceShape::DeleteEmbededElectricalGroup()
{
	//取消电器组添加对框体板件和空间的影响	
	CancelInsetedBoardInfluences();
	CancelCoveredBoardInfluences();
	CancelCoveredSpaceInfluences();
	ClearDoorGroupInfluences();
	//删除外盖抽屉组
	RemoveChildShape(EmbededElecticalGroup);
	EmbededElecticalGroup = nullptr;

	//恢复为默认的框体空间
	SetSpaceSubType(SST_FrameSpace);
	//顶层空间设置内部空间
	FSpaceShape* InsideSpace = new FSpaceShape();
	InsideSpace->SetShapeName(TEXT("内部空间"));
	InsideSpace->SetSpaceSubType(SST_UnitSpace);  //内部空间
	SetInsideSpace(MakeShareable(InsideSpace));

	//计算顶层内胆的尺寸
	FVector UnitPosition;
	FVector UnitDimention;
	bool bCalUnit = CalUnitShapePosDim(UnitPosition, UnitDimention);

	//默认内部空间的顶层内胆空间
	TSharedPtr<FUnitShape> NewTopInsideShape = MakeShareable(new FUnitShape);
	NewTopInsideShape->SetShapeName(TEXT("顶层内胆"));
	NewTopInsideShape->SetShapeWidth(UnitDimention.X);
	NewTopInsideShape->SetShapeDepth(UnitDimention.Y);
	NewTopInsideShape->SetShapeHeight(UnitDimention.Z);
	InsideSpace->SetTopInsideShape(NewTopInsideShape);
}

bool FSpaceShape::IsSlidingDrawerToCurSpaceMatching(FSlidingDrawerShape* InDrawerShape)
{
	//1、框体空间中添加掩门门组
	check(IsFrameShapeSpace());

	//2、内部空间的尺寸和位置
	FVector InsideSpacePos;
	FVector InsideSpaceDim;
	bool bInsidePosDim = FrameShape->CalInsideSpacePosDim(InsideSpacePos, InsideSpaceDim);
	check(bInsidePosDim);

	//4、计算抽屉尺寸变化范围，并验证当前空间是否有效
	//宽度
	float WidthMinValue = 0.0;
	float WidthMaxValue = 0.0;
	TSharedPtr<FShapeAttribute> DrawerWidthAttri = InDrawerShape->GetShapeWidthAttri();
	if (SAT_NumberRange == DrawerWidthAttri->GetAttributeType())
	{
		TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(DrawerWidthAttri);
		WidthMinValue = NumberRangeAttri->GetMinValue();
		WidthMaxValue = NumberRangeAttri->GetMaxValue();
	}
	if (InsideSpaceDim.X < WidthMinValue || InsideSpaceDim.X > WidthMaxValue)
	{
		return false;
	}
	//深度
	float DepthMinValue = 0.0;
	float DepthMaxValue = 0.0;
	float CurDoorFaceDepth = InDrawerShape->GetDrawerDoorTotalDepth();
	TSharedPtr<FShapeAttribute> DrawerDepthAttri = InDrawerShape->GetShapeDepthAttri();
	if (SAT_NumberRange == DrawerDepthAttri->GetAttributeType())
	{
		TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(DrawerDepthAttri);
		DepthMinValue = NumberRangeAttri->GetMinValue();
		DepthMaxValue = NumberRangeAttri->GetMaxValue();
	}
	if (InsideSpaceDim.Y - CurDoorFaceDepth < DepthMinValue || InsideSpaceDim.Y > DepthMaxValue)
	{
		return false;
	}
	//厚度
	bool bHeightValid = false;
	float HeightMinValue = 0.0;
	float HeightMaxValue = 0.0;
	TSharedPtr<FShapeAttribute> DrawerHeightAttri = InDrawerShape->GetShapeHeightAttri();
	if (SAT_NumberRange == DrawerHeightAttri->GetAttributeType())
	{
		TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(DrawerHeightAttri);
		HeightMinValue = NumberRangeAttri->GetMinValue();
		HeightMaxValue = NumberRangeAttri->GetMaxValue();
	}
	for (int32 i = 0; i < 10; ++i)
	{
		if (InsideSpaceDim.Z >= HeightMinValue*i && InsideSpaceDim.Z <= HeightMaxValue*i)
		{
			bHeightValid = true;
			break;
		}
	}

	//5、设置抽屉的相对位置和尺寸
	if (bHeightValid)
	{
		InDrawerShape->SetShapeWidth(InsideSpaceDim.X);
		InDrawerShape->SetShapeDepth(InsideSpaceDim.Y);
		InDrawerShape->SetShapePosX(InsideSpacePos.X);
		InDrawerShape->SetShapePosY(InsideSpacePos.Y);
	}
	return bHeightValid;
}

bool FSpaceShape::IsEmbededEletricalGroupSpaceMatching(FAccessoryShape * InEmbededEletricalShape)
{
	//1、框体空间中添加掩门门组
	check(IsFrameShapeSpace());

	//2、内部空间的尺寸和位置
	FVector InsideSpacePos;
	FVector InsideSpaceDim;
	bool bInsidePosDim = FrameShape->CalInsideSpacePosDim(InsideSpacePos, InsideSpaceDim);
	check(bInsidePosDim);		
	//4、计算抽屉尺寸变化范围，并验证当前空间是否有效
	auto CheckRangeValid = [](const float InCurValue,TSharedPtr<FShapeAttribute> InAttri)->bool
	{		
		bool RetValue = true;
		float MinValue = 0.0f;
		float MaxValue = 0.0f;
		check(SAT_NumberRange == InAttri->GetAttributeType());
		{
			TSharedPtr<FNumberRangeAttri> NumberRangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(InAttri);
			MinValue = NumberRangeAttri->GetMinValue();
			MaxValue = NumberRangeAttri->GetMaxValue();
		}
		if (InCurValue < MinValue)
		{
			RetValue =  false;
		}
		return  RetValue;
	};
	bool RetResult = false;
	do
	{
		// 宽度方向暂时不做判断
		//RetResult = CheckRangeValid(InsideSpaceDim.X, InEmbededEletricalShape->GetShapeWidthAttri());
		//if (!RetResult) break;
		RetResult = CheckRangeValid(InsideSpaceDim.Z, InEmbededEletricalShape->GetShapeHeightAttri());
		if (!RetResult) break;
		RetResult = CheckRangeValid(InsideSpaceDim.Y, InEmbededEletricalShape->GetShapeDepthAttri());
		if (!RetResult) break;

		// 修正坐标与尺寸
		{
			// 宽度方向暂时不做尺寸修正		
			//if (InEmbededEletricalShape->GetShapeWidth() > InsideSpaceDim.X)
			//{
			//	InEmbededEletricalShape->SetShapeWidth(InsideSpaceDim.X);
			//}
			if (InEmbededEletricalShape->GetShapeHeight() > InsideSpaceDim.Z)
			{
				InEmbededEletricalShape->SetShapeHeight(InsideSpaceDim.Z);
			}
			if (InEmbededEletricalShape->GetShapeDepth() > InsideSpaceDim.Y)
			{
				InEmbededEletricalShape->SetShapeDepth(InsideSpaceDim.Y);
			}			
			InEmbededEletricalShape->SetShapePosX(InsideSpacePos.X);
			InEmbededEletricalShape->SetShapePosY(InsideSpacePos.Y);
		}

	} while (false);
	return RetResult;
}

void FSpaceShape::SetFrameShape(TSharedPtr<FFrameShape> InShape)
{
	FrameShape = InShape;
	//设置父子关系
	FrameShape->SetParentShape(this);
	AddChildShape(FrameShape);
	//设置框体归属的空间
	FrameShape->SetSpaceShapeData(this);
}

void FSpaceShape::DeleteFrameShape()
{
	RemoveChildShape(FrameShape);
	FrameShape = nullptr;
}

void FSpaceShape::AddFrameBoard(TSharedPtr<FBoardShape> InShape, int32 InType, int32 InIndex/* = 0*/)
{
	if (FrameShape.IsValid())
	{
		FBoardShape* AddedBoard = FrameShape->AddFrameBoardShape(InShape, InType, InIndex);
		if (AddedBoard)
		{
			FCabinetShape* CurCabinetShape = GetCabinetShapeData();
			FName BoardProfileName = CurCabinetShape->GetShapeSpaceProfileName();
			//创建板件的Actor
			ASCTShapeActor* NewShapeActor = AddedBoard->SpawnShapeActor();
			AddedBoard->SpawnActorsForSelected(BoardProfileName);
			NewShapeActor->AttachToActorOverride(FrameShape->GetShapeActor(), FAttachmentTransformRules::KeepRelativeTransform);

			//更新内部空间
			UpdateInsideSpace();
		}
	}
}

void FSpaceShape::ChangeFrameBoard(TSharedPtr<FBoardShape> InShape, int32 InType, int32 InIndex/* = 0*/)
{
	if (FrameShape.IsValid())
	{
		bool bChange = FrameShape->ChangeFrameBoardShape(InShape, InType, InIndex);
		if (bChange)
		{
			bChange = UpdateInsideSpace();
		}
	}
}

void FSpaceShape::DeleteFrameBoardByTypeIndex(int32 InType, int32 InIndex/* = 0*/)
{
	if (FrameShape.IsValid())
	{
		TSharedPtr<FBoardShape> CurBoardShape = FrameShape->GetFrameBoardShape(InType, InIndex);
		DeleteFrameBoard(CurBoardShape.Get());
// 		bool bDelete = FrameShape->RemoveFrameBoardShapeByTypeIndex(InType, InIndex);
// 		if (bDelete)
// 		{
// 			bDelete = UpdateInsideSpace();
// 		}
	}
}

void FSpaceShape::DeleteFrameBoard(FBoardShape* InBoard)
{
	if (FrameShape.IsValid())
	{
		//先移除柜体中缓存的受影响的板件
		CabinetShapeData->RemoveInfluenceBoardInSpace(InBoard);
		//再移除柜体框中的板件
		bool bDelete = FrameShape->RemoveFrameBoardShape(InBoard);
		if (bDelete)
		{
			bDelete = UpdateInsideSpace();
		}
	}
}

TSharedPtr<FBoardShape> FSpaceShape::GetFrameBoard(int32 InType, int32 InIndex/* = 0*/)
{
	if (FrameShape.IsValid())
	{
		return FrameShape->GetFrameBoardShape(InType, InIndex);
	}
	return nullptr;
}

bool FSpaceShape::ModifyFrameBoardRetractByTypeIndex(int32 InType, int32 InIndex, float InValue)
{
	bool bModify = false;
	if (FrameShape.IsValid())
	{
		bModify = FrameShape->ModifyFrameBoardRetrack(InType, InIndex, InValue);
		if (bModify)
		{
			bModify = UpdateSpaceShape();
		}
	}
	return bModify;
}

bool FSpaceShape::ModifyFrameBoardHeightByTypeIndex(int32 InType, int32 InIndex, FString InValue)
{
	bool bModify = false;
	if (FrameShape.IsValid())
	{
		bModify = FrameShape->ModifyFrameBoardHeight(InType, InIndex, InValue);
		if (bModify)
		{
			bModify = UpdateSpaceShape();
		}
	}
	return bModify;
}

bool FSpaceShape::ModifyFrameBoardRetract(FBoardShape* InBoard, float InValue)
{
	bool bModify = false;
	if (FrameShape.IsValid())
	{
		int32 BoardType = -1;
		int32 BoardIndex = -1;
		FrameShape->GetFrameBoardTypeIndex(BoardType, BoardIndex, InBoard);
		bModify = ModifyFrameBoardRetractByTypeIndex(BoardType, BoardIndex, InValue);
	}
	return bModify;
}

bool FSpaceShape::ModifyFrameBoardHeight(FBoardShape* InBoard, FString InValue)
{
	bool bModify = false;
	if (FrameShape.IsValid())
	{
		int32 BoardType = -1;
		int32 BoardIndex = -1;
		FrameShape->GetFrameBoardTypeIndex(BoardType, BoardIndex, InBoard);
		bModify = ModifyFrameBoardHeightByTypeIndex(BoardType, BoardIndex, InValue);
	}
	return bModify;
}

bool FSpaceShape::UpdateFrameBoard()
{
	if (FrameShape.IsValid())
	{
		return FrameShape->UpdateAllFrameShapes();
	}
	return false;
}

bool FSpaceShape::AddSideHungDoor(TSharedPtr<FSideHungDoor> InDoorShape, const bool InCreateBoundBox /*= true */)
{
	//1、框体空间中添加掩门门组
	check(IsFrameShapeSpace());
	//2、清楚所有门组影响的框体板件和空间缓存
	ClearDoorGroupInfluences();
	//3、添加门组适配当前空间
	bool bFitDoorGroup = FitSideHungDoorGroupToCurSpaceShape(InDoorShape.Get());
	if (!bFitDoorGroup)
	{
		return false;
	}

	//4、设置门组
	SetDoorShape(InDoorShape);
	//5、创建门组Actor
	ASCTShapeActor* NewDoorShapeActor = InDoorShape->SpawnShapeActor();
	NewDoorShapeActor->AttachToActorOverride(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);

	//创建型录外包框和包围盒
	if (InCreateBoundBox)
	{
		FName ShapeSpaceProfileName = CabinetShapeData->GetShapeSpaceProfileName();
		InDoorShape->SpawnActorsForSelected(ShapeSpaceProfileName);
	}
	return true;
}

bool FSpaceShape::AddSlidingDoor(TSharedPtr<FSlidingDoor> InDoorShape,const bool InCreateBoundBox /*= true */)
{
	//1、框体空间中添加移门门组
	check(IsFrameShapeSpace());
	//2、清楚所有门组影响的框体板件和空间缓存
	ClearDoorGroupInfluences();
	//3、添加门组适配当前空间
	bool bFitDoorGroup = FitSlidingDoorGroupToCurSpaceShape(InDoorShape.Get());
	if (!bFitDoorGroup)
	{
		return false;
	}
	//4、设置门组
	SetDoorShape(InDoorShape);
	//5、创建门组Actor
	ASCTShapeActor* NewDoorShapeActor = InDoorShape->SpawnShapeActor();
	NewDoorShapeActor->AttachToActorOverride(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
	if (InCreateBoundBox)
	{
		//创建型录外包框和包围盒
		FName ShapeSpaceProfileName = CabinetShapeData->GetShapeSpaceProfileName();
		InDoorShape->SpawnActorsForSelected(ShapeSpaceProfileName);
	}
	return true;
}

void FSpaceShape::RemoveDoorGroup()
{
	if (!DoorShape.IsValid())
	{
		return;
	}

	//取消门组添加对框体板件和空间的影响
	CancelInsetedBoardInfluences();
	CancelCoveredBoardInfluences();
	CancelCoveredSpaceInfluences();
	ClearDoorGroupInfluences();
	
	bShowDoorGroup = true;
	//删除门组数据
	DeleteDoorShape();
	//更新相关对象的位置和尺寸
	UpdateSpaceShape();
}

void FSpaceShape::ShowDoorGropActor()
{
	//处理当前空间
	if (IsFrameShapeSpace() && DoorShape.IsValid())
	{
		bShowDoorGroup = true;
		DoorShape->ShowDoorSheetsActor();
	}
	//递归处理子级空间
	if (InsideSpace.IsValid() && InsideSpace->IsChildrenSpace())
	{
		for (int32 i = 0; i < InsideSpace->ChildSpaceShapes.Num(); ++i)
		{
			InsideSpace->ChildSpaceShapes[i]->ShowDoorGropActor();
		}
	}
}

void FSpaceShape::HideDoorGropActor()
{
	//处理当前空间
	if (IsFrameShapeSpace() && DoorShape.IsValid())
	{
		bShowDoorGroup = false;
		DoorShape->HideDoorSheetsActor();
	}
	//递归处理子级空间
	if (InsideSpace.IsValid() && InsideSpace->IsChildrenSpace())
	{
		for (int32 i = 0; i < InsideSpace->ChildSpaceShapes.Num(); ++i)
		{
			InsideSpace->ChildSpaceShapes[i]->HideDoorGropActor();
		}
	}
}

bool FSpaceShape::IsDoorGroupShow()
{
	return bShowDoorGroup;
}

bool FSpaceShape::IsContainDoorGroup()
{
	//当前空间包含门组
	if (IsFrameShapeSpace() && DoorShape.IsValid())
	{
		return true;
	}
	//递归处理子级空间
	bool bContain = false;
	if (InsideSpace.IsValid() && InsideSpace->IsChildrenSpace())
	{
		for (int32 i = 0; i < InsideSpace->ChildSpaceShapes.Num(); ++i)
		{
			bContain = InsideSpace->ChildSpaceShapes[i]->IsContainDoorGroup();
			if (bContain)
			{
				break;
			}
		}
	}
	return bContain;
}

bool FSpaceShape::UpdateDoorGroup()
{
	if (!DoorShape.IsValid())
	{
		return true;
	}
	check(IsFrameShapeSpace());

	bool bResult = false;
	//取消之前门组影响的所有框体板件和空间
	CancelInsetedBoardInfluences();
	CancelCoveredBoardInfluences();
	CancelCoveredSpaceInfluences();
	ClearDoorGroupInfluences();

	//根据门组类型以及掩盖类型，更新门组尺寸和位置
	if (DoorShape->GetShapeType() == ST_SlidingDoor)
	{
		TSharedPtr<FSlidingDoor> CurDoorGroup = StaticCastSharedPtr<FSlidingDoor>(DoorShape);
		bResult = FitSlidingDoorGroupToCurSpaceShape(CurDoorGroup.Get());
	}
	else if (DoorShape->GetShapeType() == ST_SideHungDoor)
	{
		TSharedPtr<FSideHungDoor> CurDoorGroup = StaticCastSharedPtr<FSideHungDoor>(DoorShape);
		bResult = FitSideHungDoorGroupToCurSpaceShape(CurDoorGroup.Get());
	}
	else
	{
		check(false);
	}
	return bResult;
}

void FSpaceShape::ShowDrawerActor()
{
	//处理当前空间
	if (CoveredDrawerGroup.IsValid())
	{
		bShowDrawerGroup = true;
		CoveredDrawerGroup->ShowDrawer();
	}
	//递归处理子级空间
	if (InsideSpace.IsValid())
	{
		for (int32 i = 0; i < InsideSpace->ChildSpaceShapes.Num(); ++i)
		{
			InsideSpace->ChildSpaceShapes[i]->ShowDrawerActor();
		}
	}
}

void FSpaceShape::HideDrawerActor()
{
	//处理当前空间
	if (CoveredDrawerGroup.IsValid())
	{
		bShowDrawerGroup = false;
		CoveredDrawerGroup->HideDrawer();
	}
	//递归处理子级空间
	if (InsideSpace.IsValid())
	{
		for (int32 i = 0; i < InsideSpace->ChildSpaceShapes.Num(); ++i)
		{
			InsideSpace->ChildSpaceShapes[i]->HideDrawerActor();
		}
	}

}

void FSpaceShape::SetDoorShape(TSharedPtr<FDoorGroup> InShape)
{
	DoorShape = InShape;
	//设置父子关系
	DoorShape->SetParentShape(this);
	AddChildShape(DoorShape);
}

void FSpaceShape::DeleteDoorShape()
{
	RemoveChildShape(DoorShape);
	DoorShape = nullptr;
}

TSharedPtr<FSpaceShape> FSpaceShape::GetInsideSpace()
{
	return InsideSpace;
}

bool FSpaceShape::SetInsideSpace(TSharedPtr<FSpaceShape> InSpace)
{
	InsideSpace = InSpace;
	//设置父子关系
	InsideSpace->SetParentSpaceShape(this);
	InsideSpace->SetParentShape(this);
	AddChildShape(InsideSpace);
	InsideSpace->SetShapeName(TEXT("内部空间"));
	//设置内部空间归属的柜体
	InsideSpace->SetCabinetShapeData(GetCabinetShapeData());

	//设置内部空间的尺寸和位置
	InsideSpace->SetShapeWidth(GetShapeWidth());
	InsideSpace->SetShapeDepth(GetShapeDepth());
	InsideSpace->SetShapeHeight(GetShapeHeight());
	//设置内部空间的轮廓信息
	FSCTOutline* NewOutline = new FSCTOutline;
	SpaceOutline->CopyTo(NewOutline);
	InsideSpace->SetSpaceOutline(MakeShareable(NewOutline));
	return true;
}

void FSpaceShape::DeleteInsideSpace()
{
	RemoveChildShape(InsideSpace);
	InsideSpace = nullptr;
}

bool FSpaceShape::UpdateInsideSpace()
{
	//1、框体空间没有内部空间
	if (!InsideSpace.IsValid())
	{
		return true;
	}

	if (BottomSpace.IsValid())
	{
		BottomSpace->UpdateSpaceShape();
	}

	//2、根据框体板件的添加情况修正轮廓参数
	bool bOutline = FrameShape->CalInsideOutlineParm(InsideSpace);

	//3、根据框体情况修正内部空间的位置、尺寸
	FVector InsidePosition;
	FVector InsideDimention;
	bool bPosdim = FrameShape->CalInsideSpacePosDim(InsidePosition, InsideDimention);
	check(bPosdim);

	//4、计算内胆的位置和尺寸
	InsideSpace->SetShapePosX(InsidePosition.X);
	InsideSpace->SetShapePosY(InsidePosition.Y);
	InsideSpace->SetShapePosZ(InsidePosition.Z);
	InsideSpace->SetShapeWidth(InsideDimention.X);
	InsideSpace->SetShapeDepth(InsideDimention.Y);
	InsideSpace->SetShapeHeight(InsideDimention.Z);

	return true;
}

TSharedPtr<FSpaceShape> FSpaceShape::GetBottomSpace()
{
	return BottomSpace;
}

void FSpaceShape::SetBottomSpace(TSharedPtr<FSpaceShape> InSpace)
{
	if (BottomSpace.IsValid())
	{
		return;
	}
	BottomSpace = InSpace;
	BottomSpace->SetSpaceSubType(SST_BottomSpace);  //底部空间
	BottomSpace->SetParentShape(this);
	AddChildShape(BottomSpace);
	BottomSpace->SetShapeName(TEXT("底部空间"));
	//设置底部空间归属的柜体
	BottomSpace->SetCabinetShapeData(GetCabinetShapeData());
}

bool FSpaceShape::CreateBottomSpace(bool bForce/* = false*/)
{
	//1、判断当前空间是否允许
	if (IsSplitBoardSpace())
	{
		return false;
	}
	else
	{
		check(InsideSpace.IsValid());
		//柜体空间不为空时，先清空子空间
		if (InsideSpace->IsChildrenSpace())
		{
			if (bForce)
			{
				InsideSpace->EmptyChildrenSpace();
			}
			else
			{
				return false;
			}
		}
	}

	//空间归属的柜体
	FCabinetShape* CurCabinetShapeData = GetCabinetShapeData();

	//当前空间的尺寸
	float SpaceWidth = InsideSpace->GetShapeWidth();
	float SpaceDepth = InsideSpace->GetShapeDepth();
	float SpaceHeight = InsideSpace->GetShapeHeight();
	//当前空间的位置
	float SpacePosX = InsideSpace->GetShapePosX();
	float SpacePosY = InsideSpace->GetShapePosY();
	float SpacePosZ = InsideSpace->GetShapePosZ();

	//2、分割尺寸
	float BottomHeight = CurCabinetShapeData->GetBottomSpaceHeight();
	float childHeight = SpaceHeight - BottomHeight;
	if (childHeight <= 0 || !IsValidForHeight(childHeight))
	{
		return false;
	}

	//保存分割模式和分割方向到当前空间
	InsideSpace->SetChildrenSpaceSplitMode(false);          //相对于虚拟分割
	InsideSpace->SetChildrenSpaceSplitDirect(SDT_ZDirect);  //相对于垂直方向 SDT_ZDirect = 3,

	//新建底部空间
	FSpaceShape* NewBottomSpace = new FSpaceShape(CurCabinetShapeData);
	NewBottomSpace->SetShapeName(TEXT("底部空间"));
	NewBottomSpace->SetDirectType(SDT_ZDirect);
	NewBottomSpace->SetSpaceModifyScale(BottomHeight);
	NewBottomSpace->SetShapePosX(0.0);
	NewBottomSpace->SetShapePosY(0.0);
	NewBottomSpace->SetShapePosZ(0.0);
	NewBottomSpace->SetShapeWidth(SpaceWidth);
	NewBottomSpace->SetShapeDepth(SpaceDepth);
	NewBottomSpace->SetShapeHeight(BottomHeight);
	NewBottomSpace->MakeDefaultSpace();
	//NewBottomSpace->SetSpaceSubType(SST_BottomSpace);  //底部空间

	//新建子空间
	FSpaceShape* NewChildSpace = new FSpaceShape(CurCabinetShapeData);
	NewChildSpace->SetShapeName(TEXT("上部空间"));
	NewChildSpace->SetDirectType(SDT_ZDirect);
	NewChildSpace->SetSpaceModifyScale(-1);
	NewChildSpace->SetShapePosX(0.0);
	NewChildSpace->SetShapePosY(0.0);
	NewChildSpace->SetShapePosZ(BottomHeight);
	NewChildSpace->SetShapeWidth(SpaceWidth);
	NewChildSpace->SetShapeDepth(SpaceDepth);
	NewChildSpace->SetShapeHeight(childHeight);
	NewChildSpace->MakeDefaultSpace();
	//NewChildSpace->SetSpaceSubType(SST_FrameSpace);  //框体空间

	//空间轮廓的类型和方向
	TSharedPtr<FSCTOutline> CurrentOutline = InsideSpace->GetSpaceOutline();
	EOutlineType OutlineType = CurrentOutline->GetOutlineType();
	int32 OutlineDirect = CurrentOutline->GetOutlineDirect();

	//处理子空间的轮廓
	switch (OutlineType)
	{
	case OLT_LeftGirder:
	{
		float GHeight = CurrentOutline->GetOutlineParamValueByRefName(TEXT("GH"));
		float ChildSpaceHeight = NewChildSpace->GetShapeDepth();
		if (GHeight >= ChildSpaceHeight)
		{
			return false;
		}
		FSCTOutline* NewOutline = new FSCTOutline;
		CurrentOutline->CopyTo(NewOutline);
		NewOutline->SetSpaceHeight(ChildSpaceHeight);
		NewChildSpace->SetSpaceOutline(MakeShareable(NewOutline));
		break;
	}
	case OLT_RightGirder:
	{
		float GHeight = CurrentOutline->GetOutlineParamValueByRefName(TEXT("GH"));
		float ChildSpaceHeight = NewChildSpace->GetShapeDepth();
		if (GHeight >= ChildSpaceHeight)
		{
			return false;
		}
		FSCTOutline* NewOutline = new FSCTOutline;
		CurrentOutline->CopyTo(NewOutline);
		NewOutline->SetSpaceHeight(ChildSpaceHeight);
		NewChildSpace->SetSpaceOutline(MakeShareable(NewOutline));
		break;
	}
	case OLT_BackGirder:
	{
		float GHeight = CurrentOutline->GetOutlineParamValueByRefName(TEXT("GH"));
		float ChildSpaceHeight = NewChildSpace->GetShapeDepth();
		if (GHeight >= ChildSpaceHeight)
		{
			return false;
		}
		FSCTOutline* NewOutline = new FSCTOutline;
		CurrentOutline->CopyTo(NewOutline);
		NewOutline->SetSpaceHeight(ChildSpaceHeight);
		NewChildSpace->SetSpaceOutline(MakeShareable(NewOutline));
		break;
	}
	case OLT_LeftPiller:
	{
		{
			float ChildSpaceHeight = NewChildSpace->GetShapeDepth();
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceHeight(ChildSpaceHeight);
			NewChildSpace->SetSpaceOutline(MakeShareable(NewOutline));
		}
		{
			float ChildSpaceHeight = NewBottomSpace->GetShapeDepth();
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceHeight(ChildSpaceHeight);
			NewBottomSpace->SetSpaceOutline(MakeShareable(NewOutline));
		}
		break;
	}
	case OLT_RightPiller:
	{
		{
			float ChildSpaceHeight = NewChildSpace->GetShapeDepth();
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceHeight(ChildSpaceHeight);
			NewChildSpace->SetSpaceOutline(MakeShareable(NewOutline));
		}
		{
			float ChildSpaceHeight = NewBottomSpace->GetShapeDepth();
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceHeight(ChildSpaceHeight);
			NewBottomSpace->SetSpaceOutline(MakeShareable(NewOutline));
		}
		break;
	}
	case OLT_MiddlePiller:
	{
		{
			float ChildSpaceHeight = NewChildSpace->GetShapeDepth();
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceHeight(ChildSpaceHeight);
			NewChildSpace->SetSpaceOutline(MakeShareable(NewOutline));
		}
		{
			float ChildSpaceHeight = NewBottomSpace->GetShapeDepth();
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceHeight(ChildSpaceHeight);
			NewBottomSpace->SetSpaceOutline(MakeShareable(NewOutline));
		}
		break;
	}
	case OLT_CornerPiller:
	default:
		break;
	}

	//将新建子对象作为子空间插入当前空间
	InsideSpace->DeleteTopInsideShape();
	ASCTShapeActor* CurrentActor = InsideSpace->GetShapeActor();
	//获取ProfileName
	FName SelectProfileName = CurCabinetShapeData->GetShapeSpaceProfileName();
	FName HoverProfileName = CurCabinetShapeData->GetBoundSpaceProfileName();
	//生成子级空间Actor，关联父子关系
	{
		ASCTShapeActor* ChildSpaceActor = NewChildSpace->SpawnShapeActor();
		NewChildSpace->SpawnActorsForSelected(SelectProfileName, HoverProfileName);
		ChildSpaceActor->AttachToActorOverride(CurrentActor, FAttachmentTransformRules::KeepRelativeTransform);
		InsideSpace->InsertChildSpaceShape(nullptr, MakeShareable(NewChildSpace));
	}
	//生成底部空间Actor，关联父子关系
	{
		ASCTShapeActor* ChildSpaceActor = NewBottomSpace->SpawnShapeActor();
		NewBottomSpace->SpawnActorsForSelected(SelectProfileName, HoverProfileName);
		ChildSpaceActor->AttachToActorOverride(CurrentActor, FAttachmentTransformRules::KeepRelativeTransform);
		InsideSpace->SetBottomSpace(MakeShareable(NewBottomSpace));
	}
	DestroySpaceShapeActor();

	return true;
}

void FSpaceShape::DeleteBottomSpace()
{
	if (!InsideSpace.IsValid())
	{
		return;
	}
	if (!InsideSpace->BottomSpace.IsValid())
	{
		return;
	}

	//删除底部空间直接清空当前框体空间
	EmptySpaceShape();
}

bool FSpaceShape::SetBottomSpaceHeight(float InValue)
{
	if (InsideSpace.IsValid())
	{
		InsideSpace->SetBottomSpaceHeight(InValue);
	}

	if (BottomSpace.IsValid())
	{
		check(ChildSpaceShapes.Num() == 1);
		BottomSpace->SetShapeHeight(InValue);

		float ParentHeight = ParentShape->GetShapeHeight();
		ChildSpaceShapes[0]->SetShapePosZ(InValue);
		ChildSpaceShapes[0]->SetShapeHeight(ParentHeight - InValue);

		ChildSpaceShapes[0]->SetBottomSpaceHeight(InValue);
	}
	else
	{
		for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
		{
			ChildSpaceShapes[i]->SetBottomSpaceHeight(InValue);
		}
	}

	return true;
}

float FSpaceShape::GetBottomSpaceHeight()
{
	float BottomHeight = GetCabinetShapeData()->GetBottomSpaceHeight();
	return BottomHeight;
}

bool FSpaceShape::IsValidForBottomHeight(float InValue)
{
	bool bResult = false;
	if (InsideSpace.IsValid())
	{
		bResult = InsideSpace->IsValidForBottomHeight(InValue);
		if (!bResult)
		{
			return false;
		}
	}

	if (BottomSpace.IsValid())
	{
		bResult = BottomSpace->IsValidForHeight(InValue);
		if (!bResult)
		{
			return false;
		}

		float OldBottomHeight = BottomSpace->GetShapeHeight();
		float ParentHeight = ParentShape->GetShapeHeight();
		check(ChildSpaceShapes.Num() == 1);
		bResult = ChildSpaceShapes[0]->IsValidForHeight(ParentHeight - InValue);
		if (!bResult)
		{
			return false;
		}

		bResult = ChildSpaceShapes[0]->IsValidForBottomHeight(InValue);
		if (!bResult)
		{
			return false;
		}
	}
	else
	{
		for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
		{
			bResult = ChildSpaceShapes[i]->IsValidForBottomHeight(InValue);
			if (!bResult)
			{
				return false;
			}
		}
	}
	
	return true;
}

bool FSpaceShape::SplitSpaceByVirtual(ESpaceDirectType InDirect, TArray<float> InPartValues, bool bForce/* = false*/)
{
	if (IsSplitBoardSpace())
	{
		return false;
	} 
	else
	{
		check(InsideSpace.IsValid());
		//内部空间不为空时，先清空内部空间
		if (!IsInsideSpaceEmpty())
		{
			if (bForce)
			{
				EmptyInsideSpace();
			} 
			else
			{
				return false;
			}
		} 
	}

	//保存分割模式和分割方向到当前空间
	InsideSpace->SetChildrenSpaceSplitMode(false);
	InsideSpace->SetChildrenSpaceSplitDirect(InDirect);

	//当前空间的尺寸
	float SpaceWidth = InsideSpace->GetShapeWidth();
	float SpaceDepth = InsideSpace->GetShapeDepth();
	float SpaceHeight = InsideSpace->GetShapeHeight();
	//当前空间的位置
	float SpacePosX = InsideSpace->GetShapePosX();
	float SpacePosY = InsideSpace->GetShapePosY();
	float SpacePosZ = InsideSpace->GetShapePosZ();

	//统计子空间尺寸信息
	float TotalFixed = 0.0;
	float TotalScaled = 0.0;
	int32 PartCount = InPartValues.Num();
	for (int32 i = 0; i < PartCount; ++i)
	{
		if (InPartValues[i] < 0.0)
		{
			TotalScaled += InPartValues[i];
		}
		else
		{
			TotalFixed += InPartValues[i];
		}
	}

	//空间归属的柜体
	FCabinetShape* CurCabinetShapeData = GetCabinetShapeData();
	//ProfileName
	FName SelectProfileName = CurCabinetShapeData->GetShapeSpaceProfileName();
	FName HoverProfileName = CurCabinetShapeData->GetBoundSpaceProfileName();

	//空间轮廓的类型和方向
	TSharedPtr<FSCTOutline> CurrentOutline = InsideSpace->GetSpaceOutline();
	EOutlineType OutlineType = CurrentOutline->GetOutlineType();
	int32 OutlineDirect = CurrentOutline->GetOutlineDirect();

	//根据分割方向分割当前空间
	switch (InDirect)
	{
	case SDT_XDirect:
	{
		//判断输入分割数据是否有误
		if (TotalFixed > SpaceWidth ||
			(-TotalScaled > KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed - SpaceWidth) < KINDA_SMALL_NUMBER) ||
			(-TotalScaled < KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed - SpaceWidth) > KINDA_SMALL_NUMBER))
		{
			return false;
		}

		//计算比例空间的单位尺寸
		float ScaleSize = 0.0;
		if (TotalScaled < 0.0)
		{
			ScaleSize = (SpaceWidth - TotalFixed) / TotalScaled;
		}

		//新建子空间
		TArray<TSharedPtr<FSpaceShape>> NewChildSpaces;
		float CurrentPosX = 0.0;
		for (int32 i = 0; i < PartCount; ++i)
		{
			TSharedPtr<FSpaceShape> NewSpace = MakeShareable(new FSpaceShape(CurCabinetShapeData));
			//NewSpace->SetSpaceSubType(SST_FrameSpace); 
			NewSpace->SetDirectType(InDirect);
			NewSpace->SetSpaceModifyScale(InPartValues[i]);
			NewSpace->SetShapePosX(CurrentPosX);
			NewSpace->SetShapePosY(0.0);
			NewSpace->SetShapePosZ(0.0);
			NewSpace->SetShapeDepth(SpaceDepth);
			NewSpace->SetShapeHeight(SpaceHeight);
			if (InPartValues[i] < 0.0) //比例值
			{
				NewSpace->SetShapeWidth(InPartValues[i] * ScaleSize);
			}
			else   //固定值
			{
				NewSpace->SetShapeWidth(InPartValues[i]);
			}
			NewSpace->MakeDefaultSpace(); //框体空间
			NewChildSpaces.Add(NewSpace);
			CurrentPosX += NewSpace->GetShapeWidth();
		}

		//处理子空间的轮廓
		switch (OutlineType)
		{
		case OLT_LeftGirder:
		{
			float GWidth = CurrentOutline->GetOutlineParamValueByRefName(TEXT("GW"));
			float ChildSpaceWidth = NewChildSpaces[0]->GetShapeWidth();
			if (GWidth >= ChildSpaceWidth)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceWidth(ChildSpaceWidth);
			NewChildSpaces[0]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_RightGirder:
		{
			float GWidth = CurrentOutline->GetOutlineParamValueByRefName(TEXT("GW"));
			float ChildSpaceWidth = NewChildSpaces[PartCount-1]->GetShapeWidth();
			if (GWidth >= ChildSpaceWidth)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceWidth(ChildSpaceWidth);
			NewChildSpaces[PartCount - 1]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_BackGirder:
		{
			for (int32 i=0; i<PartCount; ++i)
			{
				float ChildSpaceWidth = NewChildSpaces[i]->GetShapeWidth();
				FSCTOutline* NewOutline = new FSCTOutline;
				CurrentOutline->CopyTo(NewOutline);
				NewOutline->SetSpaceWidth(ChildSpaceWidth);
				NewChildSpaces[i]->SetSpaceOutline(MakeShareable(NewOutline));
			}
			break;
		}
		case OLT_LeftPiller:
		{
			float PWidth = CurrentOutline->GetOutlineParamValueByRefName(TEXT("PW"));
			float ChildSpaceWidth = NewChildSpaces[0]->GetShapeWidth();
			if (PWidth >= ChildSpaceWidth)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceWidth(ChildSpaceWidth);
			NewChildSpaces[0]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_RightPiller:
		{
			float PWidth = CurrentOutline->GetOutlineParamValueByRefName(TEXT("PW"));
			float ChildSpaceWidth = NewChildSpaces[PartCount - 1]->GetShapeWidth();
			if (PWidth >= ChildSpaceWidth)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceWidth(ChildSpaceWidth);
			NewChildSpaces[PartCount - 1]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_MiddlePiller:
		{
			//TODO: 需要首先判断柱所在的子空间位置，然后再进一步判断是否合理
			float PPosX = CurrentOutline->GetOutlineParamValueByRefName(TEXT("PX"));
			float PWidth = CurrentOutline->GetOutlineParamValueByRefName(TEXT("PW"));
			int32 i = 0;
			for (; i<PartCount; ++i)
			{
				float CurWidth = NewChildSpaces[i]->GetShapeWidth();
				if (CurWidth > PPosX)
				{
					break;
				}
				PPosX -= CurWidth;
			}
			float ChildSpaceWidth = NewChildSpaces[i]->GetShapeWidth();
			if (PWidth >= ChildSpaceWidth)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetOutlineParamValueByRefName(TEXT("PX"), PPosX);
			NewOutline->SetSpaceWidth(ChildSpaceWidth);
			NewChildSpaces[i]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_CornerPiller:
		{
			return false;
		}
		default:
			break;
		}

		//将新建子对象作为子空间插入当前空间
		InsideSpace->DeleteTopInsideShape();
		ASCTShapeActor* CurrentActor = InsideSpace->GetShapeActor();
		for (int32 i = 0; i < PartCount; ++i)
		{
			//生成Actor，关联父子关系
			ASCTShapeActor* ChildSpaceActor = NewChildSpaces[i]->SpawnShapeActor();
			NewChildSpaces[i]->SpawnActorsForSelected(SelectProfileName, HoverProfileName);
			ChildSpaceActor->AttachToActorOverride(CurrentActor, FAttachmentTransformRules::KeepRelativeTransform);
			InsideSpace->InsertChildSpaceShape(nullptr, NewChildSpaces[i]);
		}
		DestroySpaceShapeActor();

		break;
	}
	case SDT_YDirect:
	{
		//判断输入分割数据是否有误
		if (TotalFixed > SpaceDepth ||
			(-TotalScaled > KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed - SpaceDepth) < KINDA_SMALL_NUMBER) ||
			(-TotalScaled < KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed - SpaceDepth) > KINDA_SMALL_NUMBER))
		{
			return false;
		}

		//计算比例空间的单位尺寸
		float ScaleSize = 0.0;
		if (TotalScaled < 0.0)
		{
			ScaleSize = (SpaceDepth - TotalFixed) / TotalScaled;
		}

		//新建子空间
		TArray<FSpaceShape*> NewChildSpaces;
		float CurrentPosY = 0.0;
		for (int32 i = 0; i < PartCount; ++i)
		{
			FSpaceShape* NewSpace = new FSpaceShape(CurCabinetShapeData);
			NewSpace->SetDirectType(InDirect);
			NewSpace->SetSpaceModifyScale(InPartValues[i]);
			NewSpace->SetShapePosX(0.0);
			NewSpace->SetShapePosY(CurrentPosY);
			NewSpace->SetShapePosZ(0.0);
			NewSpace->SetShapeWidth(SpaceWidth);
			NewSpace->SetShapeHeight(SpaceHeight);
			if (InPartValues[i] < 0.0) //比例值
			{
				NewSpace->SetShapeDepth(InPartValues[i] * ScaleSize);
			}
			else  //固定值
			{
				NewSpace->SetShapeDepth(InPartValues[i]);
			}
			NewSpace->MakeDefaultSpace();
			NewChildSpaces.Add(NewSpace);
			CurrentPosY += NewSpace->GetShapeDepth();
		}

		//处理子空间的轮廓
		switch (OutlineType)
		{
		case OLT_LeftGirder:
		{
			for (int32 i=0; i<PartCount; ++i)
			{
				float ChildSpaceDepth = NewChildSpaces[i]->GetShapeDepth();
				FSCTOutline* NewOutline = new FSCTOutline;
				CurrentOutline->CopyTo(NewOutline);
				NewOutline->SetSpaceDepth(ChildSpaceDepth);
				NewChildSpaces[i]->SetSpaceOutline(MakeShareable(NewOutline));
			}
			break;
		}
		case OLT_RightGirder:
		{
			for (int32 i = 0; i < PartCount; ++i)
			{
				float ChildSpaceDepth = NewChildSpaces[i]->GetShapeDepth();
				FSCTOutline* NewOutline = new FSCTOutline;
				CurrentOutline->CopyTo(NewOutline);
				NewOutline->SetSpaceDepth(ChildSpaceDepth);
				NewChildSpaces[i]->SetSpaceOutline(MakeShareable(NewOutline));
			}
			break;
		}
		case OLT_BackGirder:
		{
			float GDepth = CurrentOutline->GetOutlineParamValueByRefName(TEXT("GD"));
			float ChildSpaceDepth = NewChildSpaces[0]->GetShapeDepth();
			if (GDepth >= ChildSpaceDepth)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceDepth(ChildSpaceDepth);
			NewChildSpaces[0]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_LeftPiller:
		{
			float PDepth = CurrentOutline->GetOutlineParamValueByRefName(TEXT("PD"));
			float ChildSpaceDepth = NewChildSpaces[0]->GetShapeWidth();
			if (PDepth >= ChildSpaceDepth)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceDepth(ChildSpaceDepth);
			NewChildSpaces[0]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_RightPiller:
		{
			float PDepth = CurrentOutline->GetOutlineParamValueByRefName(TEXT("PD"));
			float ChildSpaceDepth = NewChildSpaces[0]->GetShapeWidth();
			if (PDepth >= ChildSpaceDepth)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceDepth(ChildSpaceDepth);
			NewChildSpaces[0]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_MiddlePiller:
		{
			float PDepth = CurrentOutline->GetOutlineParamValueByRefName(TEXT("PD"));
			float ChildSpaceDepth = NewChildSpaces[0]->GetShapeWidth();
			if (PDepth >= ChildSpaceDepth)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceDepth(ChildSpaceDepth);
			NewChildSpaces[0]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_CornerPiller:
		{
			return false;
		}
		default:
			break;
		}

		//将新建子对象作为子空间插入当前空间
		InsideSpace->DeleteTopInsideShape();
		ASCTShapeActor* CurrentActor = InsideSpace->GetShapeActor();
		for (int32 i = 0; i < PartCount; ++i)
		{
			//生成Actor，关联父子关系
			ASCTShapeActor* ChildSpaceActor = NewChildSpaces[i]->SpawnShapeActor();
			NewChildSpaces[i]->SpawnActorsForSelected(SelectProfileName, HoverProfileName);
			ChildSpaceActor->AttachToActorOverride(CurrentActor, FAttachmentTransformRules::KeepRelativeTransform);
			InsideSpace->InsertChildSpaceShape(nullptr, MakeShareable(NewChildSpaces[i]));
		}
		DestroySpaceShapeActor();

		break;
	}
	case SDT_ZDirect:
	{
		//判断输入分割数据是否有误
		if (TotalFixed > SpaceHeight ||
			(-TotalScaled > KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed - SpaceHeight) < KINDA_SMALL_NUMBER) ||
			(-TotalScaled < KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed - SpaceHeight) > KINDA_SMALL_NUMBER))
		{
			return false;
		}

		//计算比例空间的单位尺寸
		float ScaleSize = 0.0;
		if (TotalScaled < 0.0)
		{
			ScaleSize = (SpaceHeight - TotalFixed) / TotalScaled;
		}

		//新建子空间
		TArray<FSpaceShape*> NewChildSpaces;
		float CurrentPosZ = 0.0;
		for (int32 i = 0; i < PartCount; ++i)
		{
			FSpaceShape* NewSpace = new FSpaceShape(CurCabinetShapeData);
			NewSpace->SetDirectType(InDirect);
			NewSpace->SetSpaceModifyScale(InPartValues[i]);
			NewSpace->SetShapePosX(0.0);
			NewSpace->SetShapePosY(0.0);
			NewSpace->SetShapePosZ(CurrentPosZ);
			NewSpace->SetShapeWidth(SpaceWidth);
			NewSpace->SetShapeDepth(SpaceDepth);
			if (InPartValues[i] < 0.0) //比例值
			{
				NewSpace->SetShapeHeight(InPartValues[i] * ScaleSize);
			}
			else  //固定值
			{
				NewSpace->SetShapeHeight(InPartValues[i]);
			}
			NewSpace->MakeDefaultSpace();
			NewChildSpaces.Add(NewSpace);
			CurrentPosZ += NewSpace->GetShapeHeight();
		}

		//处理子空间的轮廓
		switch (OutlineType)
		{
		case OLT_LeftGirder:
		{
			float GHeight = CurrentOutline->GetOutlineParamValueByRefName(TEXT("GH"));
			float ChildSpaceHeight = NewChildSpaces[PartCount-1]->GetShapeDepth();
			if (GHeight >= ChildSpaceHeight)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceHeight(ChildSpaceHeight);
			NewChildSpaces[PartCount - 1]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_RightGirder:
		{
			float GHeight = CurrentOutline->GetOutlineParamValueByRefName(TEXT("GH"));
			float ChildSpaceHeight = NewChildSpaces[PartCount - 1]->GetShapeDepth();
			if (GHeight >= ChildSpaceHeight)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceHeight(ChildSpaceHeight);
			NewChildSpaces[PartCount - 1]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_BackGirder:
		{
			float GHeight = CurrentOutline->GetOutlineParamValueByRefName(TEXT("GH"));
			float ChildSpaceHeight = NewChildSpaces[PartCount - 1]->GetShapeDepth();
			if (GHeight >= ChildSpaceHeight)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceHeight(ChildSpaceHeight);
			NewChildSpaces[PartCount - 1]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_LeftPiller:
		{
			for (int32 i = 0; i < PartCount; ++i)
			{
				float ChildSpaceHeight = NewChildSpaces[i]->GetShapeDepth();
				FSCTOutline* NewOutline = new FSCTOutline;
				CurrentOutline->CopyTo(NewOutline);
				NewOutline->SetSpaceHeight(ChildSpaceHeight);
				NewChildSpaces[i]->SetSpaceOutline(MakeShareable(NewOutline));
			}
			break;
		}
		case OLT_RightPiller:
		{
			for (int32 i = 0; i < PartCount; ++i)
			{
				float ChildSpaceHeight = NewChildSpaces[i]->GetShapeDepth();
				FSCTOutline* NewOutline = new FSCTOutline;
				CurrentOutline->CopyTo(NewOutline);
				NewOutline->SetSpaceHeight(ChildSpaceHeight);
				NewChildSpaces[i]->SetSpaceOutline(MakeShareable(NewOutline));
			}
			break;
		}
		case OLT_MiddlePiller:
		{
			for (int32 i = 0; i < PartCount; ++i)
			{
				float ChildSpaceHeight = NewChildSpaces[i]->GetShapeDepth();
				FSCTOutline* NewOutline = new FSCTOutline;
				CurrentOutline->CopyTo(NewOutline);
				NewOutline->SetSpaceHeight(ChildSpaceHeight);
				NewChildSpaces[i]->SetSpaceOutline(MakeShareable(NewOutline));
			}
			break;
		}
		case OLT_CornerPiller:
		{
			for (int32 i=0; i<PartCount; ++i)
			{
				float ChildSpaceHeight = NewChildSpaces[i]->GetShapeDepth();
				FSCTOutline* NewOutline = new FSCTOutline;
				CurrentOutline->CopyTo(NewOutline);
				NewOutline->SetSpaceHeight(ChildSpaceHeight);
				NewChildSpaces[i]->SetSpaceOutline(MakeShareable(NewOutline));
			}
			break;
		}
		default:
			break;
		}

		//将新建子对象作为子空间插入当前空间
		InsideSpace->DeleteTopInsideShape();
		ASCTShapeActor* CurrentActor = InsideSpace->GetShapeActor();
		for (int32 i = 0; i < PartCount; ++i)
		{
			//生成Actor，关联父子关系
			ASCTShapeActor* ChildSpaceActor = NewChildSpaces[i]->SpawnShapeActor();
			NewChildSpaces[i]->SpawnActorsForSelected(SelectProfileName, HoverProfileName);
			ChildSpaceActor->AttachToActorOverride(CurrentActor, FAttachmentTransformRules::KeepRelativeTransform);
			InsideSpace->InsertChildSpaceShape(nullptr, MakeShareable(NewChildSpaces[i]));
		}
		DestroySpaceShapeActor();

		break;
	}
	default:
		check(false);
		break;
	}

	return true;
}

bool FSpaceShape::SplitSpaceByBoard(FBoardShape* SplitBoard, ESpaceDirectType InDirect, TArray<float> InPartValues, bool bForce/* = false*/)
{
	if (IsSplitBoardSpace())
	{
		return false;
	}
	else
	{
		check(InsideSpace.IsValid());
		//内部空间不为空时，先清空内部空间
		if (!IsInsideSpaceEmpty())
		{
			if (bForce)
			{
				EmptyInsideSpace();
			}
			else
			{
				return false;
			}
		}
	}

	//空间归属的柜体
	FCabinetShape* CurCabinetShapeData = GetCabinetShapeData();
	//ProfileName
	FName SelectProfileName = CurCabinetShapeData->GetShapeSpaceProfileName();
	FName HoverProfileName = CurCabinetShapeData->GetBoundSpaceProfileName();

	//保存分割模式和分割方向到当前空间
	InsideSpace->SetChildrenSpaceSplitMode(true);
	InsideSpace->SetChildrenSpaceSplitDirect(InDirect);

	//当前板件厚度和旋转方向
	EBoardUseType BoardUseType = (EBoardUseType)SplitBoard->GetShapeCategory();
// 	if ((InDirect == SDT_XDirect && BoardUseType != BUT_VerticalBoard) ||
// 		(InDirect == SDT_YDirect && BoardUseType != BUT_BackBoard) ||
// 		(InDirect == SDT_ZDirect && BoardUseType != BUT_HorizontalBoard))
// 	{
// 		return false;
// 	}
	float BoardHeight = SplitBoard->GetShapeHeight();

	//当前空间的尺寸
	float SpaceWidth = InsideSpace->GetShapeWidth();
	float SpaceDepth = InsideSpace->GetShapeDepth();
	float SpaceHeight = InsideSpace->GetShapeHeight();
	//当前空间的位置
	float SpacePosX = InsideSpace->GetShapePosX();
	float SpacePosY = InsideSpace->GetShapePosY();
	float SpacePosZ = InsideSpace->GetShapePosZ();

	//统计新子空间尺寸
	float TotalFixed = 0.0;
	float TotalScaled = 0.0;
	int32 PartCount = InPartValues.Num();
	if (PartCount < 2)
	{
		return false;
	}
	for (int32 i = 0; i < PartCount; ++i)
	{
		if (InPartValues[i] < 0.0)
		{
			TotalScaled += InPartValues[i];
		}
		else
		{
			TotalFixed += InPartValues[i];
		}
	}
	float TotalBoard = BoardHeight * (PartCount - 1);

	//空间轮廓的类型和方向
	TSharedPtr<FSCTOutline> CurrentOutline = InsideSpace->GetSpaceOutline();
	EOutlineType OutlineType = CurrentOutline->GetOutlineType();
	int32 OutlineDirect = CurrentOutline->GetOutlineDirect();

	//根据分割方向分割当前空间
	switch (InDirect)
	{
	case SDT_XDirect:
	{
		//判断输入分割数据是否有误
		if ((TotalFixed + TotalBoard > SpaceWidth) ||
			(-TotalScaled > KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed + TotalBoard - SpaceWidth) < KINDA_SMALL_NUMBER) ||
			(-TotalScaled < KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed + TotalBoard - SpaceWidth) > KINDA_SMALL_NUMBER))
		{
			return false;
		}

		//计算比例空间的单位尺寸
		float ScaleSize = 0.0;
		if (TotalScaled < 0.0)
		{
			ScaleSize = (SpaceWidth - TotalFixed - TotalBoard) / TotalScaled;
		}

		//新建子空间
		TArray<TSharedPtr<FSpaceShape>> NewChildSpaces;
		float CurrentPosX = 0.0;
		for (int32 i = 0; i < PartCount; ++i)
		{
			//创建新的子空间
			TSharedPtr<FSpaceShape> NewSpace = MakeShareable(new FSpaceShape(CurCabinetShapeData));
			//NewSpace->SetSpaceSubType(SST_FrameSpace);  //框体空间
			NewSpace->SetDirectType(InDirect);
			NewSpace->SetSpaceModifyScale(InPartValues[i]);
			NewSpace->SetShapePosX(CurrentPosX);
			NewSpace->SetShapePosY(0.0);
			NewSpace->SetShapePosZ(0.0);
			NewSpace->SetShapeDepth(SpaceDepth);
			NewSpace->SetShapeHeight(SpaceHeight);
			if (InPartValues[i] < 0.0) //比例值
			{
				NewSpace->SetShapeWidth(InPartValues[i] * ScaleSize);
			}
			else  //固定值
			{
				NewSpace->SetShapeWidth(InPartValues[i]);
			}
			NewSpace->MakeDefaultSpace();
			NewChildSpaces.Add(NewSpace);
			CurrentPosX += NewSpace->GetShapeWidth();

			//创建分割板件子空间
			if (i < PartCount - 1)
			{
				//设置分割板件
				FBoardShape* NewBoard = new FBoardShape();
				SplitBoard->CopyTo(NewBoard);
				NewBoard->SetShapePosX(BoardHeight);
				NewBoard->SetShapePosY(0.0);
				NewBoard->SetShapePosZ(0.0);
				NewBoard->SetShapeWidth(SpaceHeight);
				NewBoard->SetShapeDepth(SpaceDepth);
				//创建新的子空间
				TSharedPtr<FSpaceShape> BoardSpace = MakeShareable(new FSpaceShape(CurCabinetShapeData));
				BoardSpace->SetDirectType(InDirect);
				BoardSpace->SetSplitBoard(MakeShareable(NewBoard));
				BoardSpace->SetShapeName(TEXT("分割板件"));
				BoardSpace->SetSpaceSubType(SST_SplitSpace);   //分割空间
				BoardSpace->SetShapePosX(CurrentPosX);
				BoardSpace->SetShapePosY(0.0);
				BoardSpace->SetShapePosZ(0.0);
				BoardSpace->SetShapeWidth(BoardHeight);
				BoardSpace->SetShapeDepth(SpaceDepth);
				BoardSpace->SetShapeHeight(SpaceHeight);
				NewChildSpaces.Add(BoardSpace);
				CurrentPosX += BoardHeight;
			}
		}

		//处理子空间的轮廓
		switch (OutlineType)
		{
		case OLT_LeftGirder:
		{
			float GWidth = CurrentOutline->GetOutlineParamValueByRefName(TEXT("GW"));
			float ChildSpaceWidth = NewChildSpaces[0]->GetShapeWidth();
			if (GWidth >= ChildSpaceWidth)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceWidth(ChildSpaceWidth);
			NewChildSpaces[0]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_RightGirder:
		{
			float GWidth = CurrentOutline->GetOutlineParamValueByRefName(TEXT("GW"));
			float ChildSpaceWidth = NewChildSpaces[PartCount * 2 - 2]->GetShapeWidth();
			if (GWidth >= ChildSpaceWidth)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceWidth(ChildSpaceWidth);
			NewChildSpaces[PartCount * 2 - 2]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_BackGirder:
		{
			for (int32 i = 0; i < PartCount * 2 - 1; ++i)
			{
				float ChildSpaceWidth = NewChildSpaces[i]->GetShapeWidth();
				FSCTOutline* NewOutline = new FSCTOutline;
				CurrentOutline->CopyTo(NewOutline);
				NewOutline->SetSpaceWidth(ChildSpaceWidth);
				NewChildSpaces[i]->SetSpaceOutline(MakeShareable(NewOutline));
			}
			break;
		}
		case OLT_LeftPiller:
		{
			float PWidth = CurrentOutline->GetOutlineParamValueByRefName(TEXT("PW"));
			float ChildSpaceWidth = NewChildSpaces[0]->GetShapeWidth();
			if (PWidth >= ChildSpaceWidth)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceWidth(ChildSpaceWidth);
			NewChildSpaces[0]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_RightPiller:
		{
			float PWidth = CurrentOutline->GetOutlineParamValueByRefName(TEXT("PW"));
			float ChildSpaceWidth = NewChildSpaces[PartCount * 2 - 2]->GetShapeWidth();
			if (PWidth >= ChildSpaceWidth)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceWidth(ChildSpaceWidth);
			NewChildSpaces[PartCount * 2 - 2]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_MiddlePiller:
		{
			//TODO: 需要首先判断柱所在的子空间位置，然后再进一步判断是否合理
			float PPosX = CurrentOutline->GetOutlineParamValueByRefName(TEXT("PX"));
			float PWidth = CurrentOutline->GetOutlineParamValueByRefName(TEXT("PW"));
			int32 i = 0;
			for (; i < PartCount * 2 - 1; ++i)
			{
				float CurWidth = NewChildSpaces[i]->GetShapeWidth();
				if (CurWidth > PPosX)
				{
					break;
				}
				PPosX -= CurWidth;
			}
			float ChildSpaceWidth = NewChildSpaces[i]->GetShapeWidth();
			if (PWidth >= ChildSpaceWidth)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetOutlineParamValueByRefName(TEXT("PX"), PPosX);
			NewOutline->SetSpaceWidth(ChildSpaceWidth);
			NewChildSpaces[i]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_CornerPiller:
		{
			return false;
		}
		default:
			break;
		}

		//将新建子对象作为子空间插入当前空间
		InsideSpace->DeleteTopInsideShape();
		ASCTShapeActor* CurrentActor = InsideSpace->GetShapeActor();
		for (int32 i = 0; i < PartCount * 2 - 1; ++i)
		{
			//设置板件的轮廓信息
			if (i % 2 == 1)
			{
				TSharedPtr<FBoardShape> BoardShape = NewChildSpaces[i]->GetSplitBoard();
				TSharedPtr<FSCTOutline> ChildOutline = NewChildSpaces[i]->GetSpaceOutline();
				FSCTOutline* NewOutline = new FSCTOutline;
				ChildOutline->CopyTo(NewOutline);
				BoardShape->SetBoardOutline(MakeShareable(NewOutline));
			}
			//生成Actor，关联父子关系
			ASCTShapeActor* ChildSpaceActor = NewChildSpaces[i]->SpawnShapeActor();
			NewChildSpaces[i]->SpawnActorsForSelected(SelectProfileName, HoverProfileName);
			ChildSpaceActor->AttachToActorOverride(CurrentActor, FAttachmentTransformRules::KeepRelativeTransform);
			InsideSpace->InsertChildSpaceShape(nullptr, NewChildSpaces[i]);
		}
		DestroySpaceShapeActor();

		break;
	}
	case SDT_YDirect:
	{
		//判断输入分割数据是否有误
		if ((TotalFixed + TotalBoard > SpaceDepth) ||
			(-TotalScaled > KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed + TotalBoard - SpaceDepth) < KINDA_SMALL_NUMBER) ||
			(-TotalScaled < KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed + TotalBoard - SpaceDepth) > KINDA_SMALL_NUMBER))
		{
			return false;
		}

		//计算比例空间的单位尺寸
		float ScaleSize = 0.0;
		if (TotalScaled < 0.0)
		{
			ScaleSize = (SpaceDepth - TotalFixed - TotalBoard) / TotalScaled;
		}

		//新建子空间
		TArray<TSharedPtr<FSpaceShape>> NewChildSpaces;
		float CurrentPosY = 0.0;
		for (int32 i = 0; i < PartCount; ++i)
		{
			//创建新的子空间
			TSharedPtr<FSpaceShape> NewSpace = MakeShareable(new FSpaceShape(CurCabinetShapeData));
			NewSpace->SetDirectType(InDirect);
			NewSpace->SetSpaceModifyScale(InPartValues[i]);
			NewSpace->SetShapePosX(0.0);
			NewSpace->SetShapePosY(CurrentPosY);
			NewSpace->SetShapePosZ(0.0);
			NewSpace->SetShapeWidth(SpaceWidth);
			NewSpace->SetShapeHeight(SpaceHeight);
			if (InPartValues[i] < 0.0) //比例值
			{
				NewSpace->SetShapeDepth(InPartValues[i] * ScaleSize);
			}
			else  //固定值
			{
				NewSpace->SetShapeDepth(InPartValues[i]);
			}
			NewSpace->MakeDefaultSpace();
			NewChildSpaces.Add(NewSpace);
			CurrentPosY += NewSpace->GetShapeDepth();

			//创建分割板件子空间
			if (i < PartCount - 1)
			{
				//设置分割板件
				FBoardShape* NewBoard = new FBoardShape();
				SplitBoard->CopyTo(NewBoard);
				NewBoard->SetShapePosX(0.0);
				NewBoard->SetShapePosY(0.0);
				NewBoard->SetShapePosZ(0.0);
				NewBoard->SetShapeWidth(SpaceHeight);
				NewBoard->SetShapeDepth(SpaceWidth);
				//创建新的子空间
				TSharedPtr<FSpaceShape> BoardSpace = MakeShareable(new FSpaceShape(CurCabinetShapeData));
				BoardSpace->SetDirectType(InDirect);
				BoardSpace->SetSplitBoard(MakeShareable(NewBoard));
				BoardSpace->SetShapeName(TEXT("分割板件"));
				BoardSpace->SetShapePosX(0.0);
				BoardSpace->SetShapePosY(CurrentPosY);
				BoardSpace->SetShapePosZ(0.0);
				BoardSpace->SetShapeWidth(SpaceWidth);
				BoardSpace->SetShapeDepth(BoardHeight);
				BoardSpace->SetShapeHeight(SpaceHeight);
				NewChildSpaces.Add(BoardSpace);
				CurrentPosY += BoardHeight;
			}
		}

		//处理子空间的轮廓
		switch (OutlineType)
		{
		case OLT_LeftGirder:
		{
			for (int32 i = 0; i < PartCount * 2 - 1; ++i)
			{
				float ChildSpaceDepth = NewChildSpaces[i]->GetShapeDepth();
				FSCTOutline* NewOutline = new FSCTOutline;
				CurrentOutline->CopyTo(NewOutline);
				NewOutline->SetSpaceDepth(ChildSpaceDepth);
				NewChildSpaces[i]->SetSpaceOutline(MakeShareable(NewOutline));
			}
			break;
		}
		case OLT_RightGirder:
		{
			for (int32 i = 0; i < PartCount * 2 - 1; ++i)
			{
				float ChildSpaceDepth = NewChildSpaces[i]->GetShapeDepth();
				FSCTOutline* NewOutline = new FSCTOutline;
				CurrentOutline->CopyTo(NewOutline);
				NewOutline->SetSpaceDepth(ChildSpaceDepth);
				NewChildSpaces[i]->SetSpaceOutline(MakeShareable(NewOutline));
			}
			break;
		}
		case OLT_BackGirder:
		{
			float GDepth = CurrentOutline->GetOutlineParamValueByRefName(TEXT("GD"));
			float ChildSpaceDepth = NewChildSpaces[0]->GetShapeDepth();
			if (GDepth >= ChildSpaceDepth)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceDepth(ChildSpaceDepth);
			NewChildSpaces[0]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_LeftPiller:
		{
			float PDepth = CurrentOutline->GetOutlineParamValueByRefName(TEXT("PD"));
			float ChildSpaceDepth = NewChildSpaces[0]->GetShapeDepth();
			if (PDepth >= ChildSpaceDepth)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceDepth(ChildSpaceDepth);
			NewChildSpaces[0]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_RightPiller:
		{
			float PDepth = CurrentOutline->GetOutlineParamValueByRefName(TEXT("PD"));
			float ChildSpaceDepth = NewChildSpaces[0]->GetShapeDepth();
			if (PDepth >= ChildSpaceDepth)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceDepth(ChildSpaceDepth);
			NewChildSpaces[0]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_MiddlePiller:
		{
			float PDepth = CurrentOutline->GetOutlineParamValueByRefName(TEXT("PD"));
			float ChildSpaceDepth = NewChildSpaces[0]->GetShapeDepth();
			if (PDepth >= ChildSpaceDepth)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceDepth(ChildSpaceDepth);
			NewChildSpaces[0]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_CornerPiller:
		{
			return false;
		}
		default:
			break;
		}

		//将新建子对象作为子空间插入当前空间
		InsideSpace->DeleteTopInsideShape();
		ASCTShapeActor* CurrentActor = InsideSpace->GetShapeActor();
		for (int32 i = 0; i < PartCount * 2 - 1; ++i)
		{
			//设置板件轮廓信息
			if (i % 2 == 1)
			{
				TSharedPtr<FBoardShape> BoardShape = NewChildSpaces[i]->GetSplitBoard();
				TSharedPtr<FSCTOutline> ChildOutline = NewChildSpaces[i]->GetSpaceOutline();
				FSCTOutline* NewOutline = new FSCTOutline;
				ChildOutline->CopyTo(NewOutline);
				BoardShape->SetBoardOutline(MakeShareable(NewOutline));
			}
			//生成Actor，关联父子关系
			ASCTShapeActor* ChildSpaceActor = NewChildSpaces[i]->SpawnShapeActor();
			NewChildSpaces[i]->SpawnActorsForSelected(SelectProfileName, HoverProfileName);
			ChildSpaceActor->AttachToActorOverride(CurrentActor, FAttachmentTransformRules::KeepRelativeTransform);
			InsideSpace->InsertChildSpaceShape(nullptr, NewChildSpaces[i]);
		}
		DestroySpaceShapeActor();

		break;
	}
	case SDT_ZDirect:
	{
		//判断输入分割数据是否有误
		if ((TotalFixed + TotalBoard > SpaceHeight) ||
			(-TotalScaled > KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed + TotalBoard - SpaceHeight) < KINDA_SMALL_NUMBER) ||
			(-TotalScaled < KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed + TotalBoard - SpaceHeight) > KINDA_SMALL_NUMBER))
		{
			return false;
		}

		//计算比例空间的单位尺寸
		float ScaleSize = 0.0;
		if (TotalScaled < 0.0)
		{
			ScaleSize = (SpaceHeight - TotalFixed - TotalBoard) / TotalScaled;
		}

		//新建子空间
		TArray<TSharedPtr<FSpaceShape>> NewChildSpaces;
		float CurrentPosZ = 0.0;
		for (int32 i = 0; i < PartCount; ++i)
		{
			//创建新的子空间
			TSharedPtr<FSpaceShape> NewSpace = MakeShareable(new FSpaceShape(CurCabinetShapeData));
			NewSpace->SetDirectType(InDirect);
			NewSpace->SetSpaceModifyScale(InPartValues[i]);
			NewSpace->SetShapePosX(0.0);
			NewSpace->SetShapePosY(0.0);
			NewSpace->SetShapePosZ(CurrentPosZ);
			NewSpace->SetShapeWidth(SpaceWidth);
			NewSpace->SetShapeDepth(SpaceDepth);
			if (InPartValues[i] < 0.0) //比例值
			{
				NewSpace->SetShapeHeight(InPartValues[i] * ScaleSize);
			}
			else  //固定值
			{
				NewSpace->SetShapeHeight(InPartValues[i]);
			}
			NewSpace->MakeDefaultSpace();
			NewChildSpaces.Add(NewSpace);
			CurrentPosZ += NewSpace->GetShapeHeight();

			//创建分割板件子空间
			if (i < PartCount - 1)
			{
				//设置分割板件
				FBoardShape* NewBoard = new FBoardShape();
				SplitBoard->CopyTo(NewBoard);
				NewBoard->SetShapePosX(0.0);
				NewBoard->SetShapePosY(0.0);
				NewBoard->SetShapePosZ(0.0);
				NewBoard->SetShapeWidth(SpaceWidth);
				NewBoard->SetShapeDepth(SpaceDepth);
				//创建新的子空间
				TSharedPtr<FSpaceShape> BoardSpace = MakeShareable(new FSpaceShape(CurCabinetShapeData));
				BoardSpace->SetDirectType(InDirect);
				BoardSpace->SetSplitBoard(MakeShareable(NewBoard));
				BoardSpace->SetShapeName(TEXT("分割板件"));
				BoardSpace->SetShapePosX(0.0);
				BoardSpace->SetShapePosY(0.0);
				BoardSpace->SetShapePosZ(CurrentPosZ);
				BoardSpace->SetShapeWidth(SpaceWidth);
				BoardSpace->SetShapeDepth(SpaceDepth);
				BoardSpace->SetShapeHeight(BoardHeight);
				NewChildSpaces.Add(BoardSpace);
				CurrentPosZ += BoardHeight;
			}
		}

		//处理子空间的轮廓
		switch (OutlineType)
		{
		case OLT_LeftGirder:
		{
			float GHeight = CurrentOutline->GetOutlineParamValueByRefName(TEXT("GH"));
			float ChildSpaceHeight = NewChildSpaces[PartCount * 2 - 2]->GetShapeHeight();
			if (GHeight >= ChildSpaceHeight)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceHeight(ChildSpaceHeight);
			NewChildSpaces[PartCount * 2 - 2]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_RightGirder:
		{
			float GHeight = CurrentOutline->GetOutlineParamValueByRefName(TEXT("GH"));
			float ChildSpaceHeight = NewChildSpaces[PartCount * 2 - 2]->GetShapeHeight();
			if (GHeight >= ChildSpaceHeight)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceHeight(ChildSpaceHeight);
			NewChildSpaces[PartCount * 2 - 2]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_BackGirder:
		{
			float GHeight = CurrentOutline->GetOutlineParamValueByRefName(TEXT("GH"));
			float ChildSpaceHeight = NewChildSpaces[PartCount * 2 - 2]->GetShapeHeight();
			if (GHeight >= ChildSpaceHeight)
			{
				return false;
			}
			FSCTOutline* NewOutline = new FSCTOutline;
			CurrentOutline->CopyTo(NewOutline);
			NewOutline->SetSpaceHeight(ChildSpaceHeight);
			NewChildSpaces[PartCount * 2 - 2]->SetSpaceOutline(MakeShareable(NewOutline));
			break;
		}
		case OLT_LeftPiller:
		{
			for (int32 i = 0; i < PartCount * 2 - 1; ++i)
			{
				float ChildSpaceHeight = NewChildSpaces[i]->GetShapeHeight();
				FSCTOutline* NewOutline = new FSCTOutline;
				CurrentOutline->CopyTo(NewOutline);
				NewOutline->SetSpaceHeight(ChildSpaceHeight);
				NewChildSpaces[i]->SetSpaceOutline(MakeShareable(NewOutline));
			}
			break;
		}
		case OLT_RightPiller:
		{
			for (int32 i = 0; i < PartCount * 2 - 1; ++i)
			{
				float ChildSpaceHeight = NewChildSpaces[i]->GetShapeHeight();
				FSCTOutline* NewOutline = new FSCTOutline;
				CurrentOutline->CopyTo(NewOutline);
				NewOutline->SetSpaceHeight(ChildSpaceHeight);
				NewChildSpaces[i]->SetSpaceOutline(MakeShareable(NewOutline));
			}
			break;
		}
		case OLT_MiddlePiller:
		{
			for (int32 i = 0; i < PartCount * 2 - 1; ++i)
			{
				float ChildSpaceHeight = NewChildSpaces[i]->GetShapeHeight();
				FSCTOutline* NewOutline = new FSCTOutline;
				CurrentOutline->CopyTo(NewOutline);
				NewOutline->SetSpaceHeight(ChildSpaceHeight);
				NewChildSpaces[i]->SetSpaceOutline(MakeShareable(NewOutline));
			}
			break;
		}
		case OLT_CornerPiller:
		{
			for (int32 i = 0; i < PartCount * 2 - 1; ++i)
			{
				float ChildSpaceHeight = NewChildSpaces[i]->GetShapeHeight();
				FSCTOutline* NewOutline = new FSCTOutline;
				CurrentOutline->CopyTo(NewOutline);
				NewOutline->SetSpaceHeight(ChildSpaceHeight);
				NewChildSpaces[i]->SetSpaceOutline(MakeShareable(NewOutline));
			}
			break;
		}
		default:
			break;
		}

		//将新建子对象作为子空间插入当前空间
		InsideSpace->DeleteTopInsideShape();
		ASCTShapeActor* CurrentActor = InsideSpace->GetShapeActor();
		for (int32 i = 0; i < PartCount * 2 - 1; ++i)
		{
			//设置板件轮廓信息
			if (i % 2 == 1)
			{
				TSharedPtr<FBoardShape> BoardShape = NewChildSpaces[i]->GetSplitBoard();
				TSharedPtr<FSCTOutline> ChildOutline = NewChildSpaces[i]->GetSpaceOutline();
				FSCTOutline* NewOutline = new FSCTOutline;
				ChildOutline->CopyTo(NewOutline);
				BoardShape->SetBoardOutline(MakeShareable(NewOutline));
			}
			//生成Actor，关联父子关系
			ASCTShapeActor* ChildSpaceActor = NewChildSpaces[i]->SpawnShapeActor();
			NewChildSpaces[i]->SpawnActorsForSelected(SelectProfileName, HoverProfileName);
			ChildSpaceActor->AttachToActorOverride(CurrentActor, FAttachmentTransformRules::KeepRelativeTransform);
			InsideSpace->InsertChildSpaceShape(nullptr, NewChildSpaces[i]);
		}
		DestroySpaceShapeActor();

		break;
	}
	default:
		check(false);
		break;
	}

	return true;
}

int32 FSpaceShape::GetChildSpaceShapeIndex(FSpaceShape* InShape) const
{
	for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
	{
		if (ChildSpaceShapes[i].Get() == InShape)
		{
			return i;
		}
	}
	return -1;
}

bool FSpaceShape::InsertChildSpaceShape(FSpaceShape* InCurShape, TSharedPtr<FSpaceShape> InNewShape)
{
	InNewShape->SetParentSpaceShape(this);
	InNewShape->SetParentShape(this);
	AddChildShape(InNewShape);
	SetSpaceSubType(SST_ChildrenSpace);
	InNewShape->SetShapeName(TEXT("框体空间"));
	//设置子空间归属的柜体
	InNewShape->SetCabinetShapeData(GetCabinetShapeData());

	if (!InCurShape)
	{
		ChildSpaceShapes.Add(InNewShape);
	}
	else
	{
		int32 IndexFind = GetChildSpaceShapeIndex(InCurShape);
		ChildSpaceShapes.Insert(InNewShape, IndexFind);
	}
	return true;
}

void FSpaceShape::DeleteChildSpaceShape(FSpaceShape* InShape)
{
	for (int32 i = 0; i < ChildSpaceShapes.Num(); ++i)
	{
		if (ChildSpaceShapes[i].Get() == InShape)
		{
			RemoveChildShape(ChildSpaceShapes[i]);
			ChildSpaceShapes.RemoveSingle(ChildSpaceShapes[i]);
			break;
		}
	}
}

void FSpaceShape::RemoveInfluenceBoard(FBoardShape* InBoard)
{
	//处理当前空间
	InsetInfluencedBoards.Remove(InBoard);
	CoverInfluencedBoards.Remove(InBoard);
	//处理子级空间
	if (InsideSpace.IsValid())
	{
		TArray<TSharedPtr<FSpaceShape>>& CurChildSpaces = InsideSpace->GetChildSpaceShapes();
		for (auto it : CurChildSpaces)
		{
			it->RemoveInfluenceBoard(InBoard);
		}
	}
}

bool FSpaceShape::GetUpSplitBoardShape(FBoardShape*& OutSplitBoard, FSpaceShape*& OutSpaceShape)
{
	if (!IsFrameShapeSpace() && !IsDrawerGroupSpace() && !IsEmbededElectricalGroup())
	{
		return false;
	}
	//上侧的分割板件
	if (ParentSpaceShape &&
		ParentSpaceShape->ChildrenSplitDirect == SDT_ZDirect)
	{
		int32 CurIndex = ParentSpaceShape->GetChildSpaceShapeIndex(this);
		TArray<TSharedPtr<FSpaceShape>>& CurChildrenSpaces = ParentSpaceShape->GetChildSpaceShapes();
		int32 ChildCount = CurChildrenSpaces.Num();
		if (ParentSpaceShape->ChildrenSplitMode && CurIndex < ChildCount - 1)
		{
			OutSplitBoard = CurChildrenSpaces[CurIndex + 1]->GetSplitBoard().Get();
			OutSpaceShape = CurChildrenSpaces[CurIndex + 2].Get();
			return true;
		}
		else if (CurIndex == ChildCount -1) //查找父级的上侧分割板件
		{
			FSpaceShape* ParentFrameSpace = ParentSpaceShape->GetParentSpaceShape();
			return ParentFrameSpace->GetUpSplitBoardShape(OutSplitBoard, OutSpaceShape);
		}
	}
	
	return false;
}

bool FSpaceShape::GetDownSplitBoardShape(FBoardShape*& OutSplitBoard, FSpaceShape*& OutSpaceShape)
{
	if (!IsFrameShapeSpace() && !IsDrawerGroupSpace() && !IsEmbededElectricalGroup())
	{
		return false;
	}
	//下侧的分割板件
	if (ParentSpaceShape &&
		ParentSpaceShape->ChildrenSplitDirect == SDT_ZDirect)
	{
		int32 CurIndex = ParentSpaceShape->GetChildSpaceShapeIndex(this);
		TArray<TSharedPtr<FSpaceShape>>& CurChildrenSpaces = ParentSpaceShape->GetChildSpaceShapes();
		int32 ChildCount = CurChildrenSpaces.Num();
		if (ParentSpaceShape->ChildrenSplitMode && CurIndex > 0)
		{
			OutSplitBoard = CurChildrenSpaces[CurIndex - 1]->GetSplitBoard().Get();
			OutSpaceShape = CurChildrenSpaces[CurIndex - 2].Get();
			return true;
		}
		else if (CurIndex == 0)
		{
			FSpaceShape* ParentFrameSpace = ParentSpaceShape->GetParentSpaceShape();
			return ParentFrameSpace->GetDownSplitBoardShape(OutSplitBoard, OutSpaceShape);
		}
	}
	return false;
}

bool FSpaceShape::GetLeftSplitBoardShape(FBoardShape*& OutSplitBoard, FSpaceShape*& OutSpaceShape)
{
	if (!IsFrameShapeSpace() && !IsDrawerGroupSpace() && !IsEmbededElectricalGroup())
	{
		return false;
	}
	//左侧的分割板件
	if (ParentSpaceShape &&
		ParentSpaceShape->ChildrenSplitDirect == SDT_XDirect)
	{
		int32 CurIndex = ParentSpaceShape->GetChildSpaceShapeIndex(this);
		TArray<TSharedPtr<FSpaceShape>>& CurChildrenSpaces = ParentSpaceShape->GetChildSpaceShapes();
		int32 ChildCount = CurChildrenSpaces.Num();
		if (ParentSpaceShape->ChildrenSplitMode && CurIndex > 0)
		{
			OutSplitBoard = CurChildrenSpaces[CurIndex - 1]->GetSplitBoard().Get();
			OutSpaceShape = CurChildrenSpaces[CurIndex - 2].Get();
			return true;
		}
		else if (CurIndex == 0)
		{
			FSpaceShape* ParentFrameSpace = ParentSpaceShape->GetParentSpaceShape();
			return ParentFrameSpace->GetLeftSplitBoardShape(OutSplitBoard, OutSpaceShape);
		}
	}
	return false;
}

bool FSpaceShape::GetRightSplitBoardShape(FBoardShape*& OutSplitBoard, FSpaceShape*& OutSpaceShape)
{
	if (!IsFrameShapeSpace() && !IsDrawerGroupSpace() && !IsEmbededElectricalGroup())
	{
		return false;
	}
	//右侧的分割板件
	if (ParentSpaceShape &&
		ParentSpaceShape->ChildrenSplitDirect == SDT_XDirect)
	{
		int32 CurIndex = ParentSpaceShape->GetChildSpaceShapeIndex(this);
		TArray<TSharedPtr<FSpaceShape>>& CurChildrenSpaces = ParentSpaceShape->GetChildSpaceShapes();
		int32 ChildCount = CurChildrenSpaces.Num();
		if (ParentSpaceShape->ChildrenSplitMode && CurIndex < ChildCount - 1)
		{
			OutSplitBoard = CurChildrenSpaces[CurIndex + 1]->GetSplitBoard().Get();
			OutSpaceShape = CurChildrenSpaces[CurIndex + 2].Get();
			return true;
		}
		else if (CurIndex == ChildCount - 1) //查找父级的上侧分割板件
		{
			FSpaceShape* ParentFrameSpace = ParentSpaceShape->GetParentSpaceShape();
			return ParentFrameSpace->GetUpSplitBoardShape(OutSplitBoard, OutSpaceShape);
		}
	}
	return false;
}

bool FSpaceShape::GetUpBoardInParentSpace(FBoardShape*& OutFrameBoard, TArray<FSpaceShape*>& OutSpaceShapes)
{
	if (!IsFrameShapeSpace() && !IsDrawerGroupSpace() && !IsEmbededElectricalGroup())
	{
		return false;
	}
	//父级空间不能为空
	if (!ParentSpaceShape)
	{
		return false;
	}

	//X轴方向分割子空间，则所有子级空间全部受影响
	if (ParentSpaceShape->ChildrenSplitDirect == SDT_XDirect)
	{
		FSpaceShape* ParentFrameSpace = ParentSpaceShape->GetParentSpaceShape();
		if (ParentSpaceShape->IsInsideSpace() && ParentFrameSpace->IsFrameShapeSpace())
		{
			TSharedPtr<FBoardShape> UpFrameBoard = ParentFrameSpace->GetFrameBoard(0, 0);
			//当前父级空间没有上侧板，继续向上查找
			if (!UpFrameBoard.IsValid())
			{
				return ParentFrameSpace->GetUpBoardInParentSpace(OutFrameBoard, OutSpaceShapes);
			}
			//当前空间存在上侧板
			OutFrameBoard = UpFrameBoard.Get();
			TArray<TSharedPtr<FSpaceShape>>& CurChildrenSpaces = ParentSpaceShape->GetChildSpaceShapes();
			for (auto it : CurChildrenSpaces)
			{
				OutSpaceShapes.Add(it.Get());
			}
			return true;
		}
	}
	//Z方向分割子空间，则只有相邻子空间受影响
	else if (ParentSpaceShape->ChildrenSplitDirect == SDT_ZDirect)
	{
		FSpaceShape* ParentFrameSpace = ParentSpaceShape->GetParentSpaceShape();
		if (ParentSpaceShape->IsInsideSpace() && ParentFrameSpace->IsFrameShapeSpace())
		{
			int32 ChildCount = ParentSpaceShape->GetChildSpaceShapes().Num();
			int32 CurIndex = ParentSpaceShape->GetChildSpaceShapeIndex(this);
			if (ChildCount - 1 == CurIndex)
			{
				TSharedPtr<FBoardShape> UpFrameBoard = ParentFrameSpace->GetFrameBoard(0, 0);
				//当前父级空间没有上侧板，继续向上查找
				if (!UpFrameBoard.IsValid())
				{
					return ParentFrameSpace->GetUpBoardInParentSpace(OutFrameBoard, OutSpaceShapes);
				}
				//当前父级空间存在上侧板
				OutFrameBoard = UpFrameBoard.Get();
				OutSpaceShapes.Add(this);
				return true;
			}
		}
	}
	else
	{
		return false;
	}
	return false;
}

bool FSpaceShape::GetDownBoardInParentSpace(FBoardShape*& OutFrameBoard, TArray<FSpaceShape*>& OutSpaceShapes)
{
	if (!IsFrameShapeSpace() && !IsDrawerGroupSpace() && !IsEmbededElectricalGroup())
	{
		return false;
	}
	//父级空间不能为空
	if (!ParentSpaceShape)
	{
		return false;
	}

	//X轴方向分割子空间，则所有子级空间全部受影响
	if (ParentSpaceShape->ChildrenSplitDirect == SDT_XDirect)
	{
		FSpaceShape* ParentFrameSpace = ParentSpaceShape->GetParentSpaceShape();
		if (ParentSpaceShape->IsInsideSpace() && ParentFrameSpace->IsFrameShapeSpace())
		{
			TSharedPtr<FBoardShape> BottomFrameBoard = ParentFrameSpace->GetFrameBoard(1, 0);
			//当前父级空间没有下侧板，继续向上查找
			if (!BottomFrameBoard.IsValid())
			{
				return ParentFrameSpace->GetDownBoardInParentSpace(OutFrameBoard, OutSpaceShapes);
			}
			//当前空间存在下侧板
			OutFrameBoard = BottomFrameBoard.Get();
			TArray<TSharedPtr<FSpaceShape>>& CurChildrenSpaces = ParentSpaceShape->GetChildSpaceShapes();
			for (auto it : CurChildrenSpaces)
			{
				OutSpaceShapes.Add(it.Get());
			}
			return true;
		}
	}
	//Z方向分割子空间，则只有相邻子空间受影响
	else if (ParentSpaceShape->ChildrenSplitDirect == SDT_ZDirect)
	{
		FSpaceShape* ParentFrameSpace = ParentSpaceShape->GetParentSpaceShape();
		if (ParentSpaceShape->IsInsideSpace() && ParentFrameSpace->IsFrameShapeSpace())
		{
			int32 CurIndex = ParentSpaceShape->GetChildSpaceShapeIndex(this);
			if (0 == CurIndex)
			{
				TSharedPtr<FBoardShape> BottomFrameBoard = ParentFrameSpace->GetFrameBoard(1, 0);
				//当前父级空间没有下侧板，继续向上查找
				if (!BottomFrameBoard.IsValid())
				{
					return ParentFrameSpace->GetDownBoardInParentSpace(OutFrameBoard, OutSpaceShapes);
				}
				//当前父级空间存在下侧板
				OutFrameBoard = BottomFrameBoard.Get();
				OutSpaceShapes.Add(this);
				return true;
			}
		}
	}
	else
	{
		return false;
	}
	return false;
}

bool FSpaceShape::GetLeftBoardInParentSpace(FBoardShape*& OutFrameBoard, TArray<FSpaceShape*>& OutSpaceShapes)
{
	if (!IsFrameShapeSpace() && !IsDrawerGroupSpace() && !IsEmbededElectricalGroup())
	{
		return false;
	}
	//父级空间不能为空
	if (!ParentSpaceShape)
	{
		return false;
	}
	
	//Z轴方向分割子空间，则所有子级空间全部受影响
	if (ParentSpaceShape->ChildrenSplitDirect == SDT_ZDirect)
	{
		FSpaceShape* ParentFrameSpace = ParentSpaceShape->GetParentSpaceShape();
		if (ParentSpaceShape->IsInsideSpace() && ParentFrameSpace->IsFrameShapeSpace())
		{
			TSharedPtr<FBoardShape> LeftFrameBoard = ParentFrameSpace->GetFrameBoard(2, 0);
			//当前父级空间没有左侧板，继续向上查找
			if (!LeftFrameBoard.IsValid())
			{
				return ParentFrameSpace->GetLeftBoardInParentSpace(OutFrameBoard, OutSpaceShapes);
			}
			//当前空间存在左侧板
			OutFrameBoard = LeftFrameBoard.Get();
			TArray<TSharedPtr<FSpaceShape>>& CurChildrenSpaces = ParentSpaceShape->GetChildSpaceShapes();
			if (CurChildrenSpaces.Num() == 1)
			{
				OutSpaceShapes.Add(CurChildrenSpaces[0].Get());
				TSharedPtr<FSpaceShape> CurBottomSpace = ParentSpaceShape->GetBottomSpace();
				check(CurBottomSpace.IsValid());
				OutSpaceShapes.Add(CurBottomSpace.Get());
			}
			else
			{
				for (auto it : CurChildrenSpaces)
				{
					OutSpaceShapes.Add(it.Get());
				}
			}
			return true;
		}
	}
	//X方向分割子空间，则只有相邻子空间受影响
	else if (ParentSpaceShape->ChildrenSplitDirect == SDT_XDirect)
	{
		FSpaceShape* ParentFrameSpace = ParentSpaceShape->GetParentSpaceShape();
		if (ParentSpaceShape->IsInsideSpace() && ParentFrameSpace->IsFrameShapeSpace())
		{
			int32 CurIndex = ParentSpaceShape->GetChildSpaceShapeIndex(this);
			if (0 == CurIndex)
			{
				TSharedPtr<FBoardShape> LeftFrameBoard = ParentFrameSpace->GetFrameBoard(2, 0);
				//当前父级空间没有左侧板，继续向上查找
				if (!LeftFrameBoard.IsValid())
				{
					return ParentFrameSpace->GetLeftBoardInParentSpace(OutFrameBoard, OutSpaceShapes);
				} 
				//当前父级空间存在左侧板
				OutFrameBoard = LeftFrameBoard.Get();
				OutSpaceShapes.Add(this);
				return true;
			}
		}
	} 
	else
	{
		return false;
	}
	return false;
}

bool FSpaceShape::GetRightBoardInParentSpace(FBoardShape*& OutFrameBoard, TArray<FSpaceShape*>& OutSpaceShapes)
{
	if (!IsFrameShapeSpace() && !IsDrawerGroupSpace() && !IsEmbededElectricalGroup())
	{
		return false;
	}
	//父级空间不能为空
	if (!ParentSpaceShape)
	{
		return false;
	}

	//Z轴方向分割子空间，则所有子级空间全部受影响
	if (ParentSpaceShape->ChildrenSplitDirect == SDT_ZDirect)
	{
		FSpaceShape* ParentFrameSpace = ParentSpaceShape->GetParentSpaceShape();
		if (ParentSpaceShape->IsInsideSpace() && ParentFrameSpace->IsFrameShapeSpace())
		{
			TSharedPtr<FBoardShape> RightFrameBoard = ParentFrameSpace->GetFrameBoard(3, 0);
			//当前父级空间没有右侧板，继续向上查找
			if (!RightFrameBoard.IsValid())
			{
				return ParentFrameSpace->GetRightBoardInParentSpace(OutFrameBoard, OutSpaceShapes);
			}
			//当前空间存在右侧板
			OutFrameBoard = RightFrameBoard.Get();
			TArray<TSharedPtr<FSpaceShape>>& CurChildrenSpaces = ParentSpaceShape->GetChildSpaceShapes();
			if (CurChildrenSpaces.Num() == 1)
			{
				OutSpaceShapes.Add(CurChildrenSpaces[0].Get());
				TSharedPtr<FSpaceShape> CurBottomSpace = ParentSpaceShape->GetBottomSpace();
				check(CurBottomSpace.IsValid());
				OutSpaceShapes.Add(CurBottomSpace.Get());
			}
			else
			{
				for (auto it : CurChildrenSpaces)
				{
					OutSpaceShapes.Add(it.Get());
				}
			}
			return true;
		}
	}
	//X方向分割子空间，则只有相邻子空间受影响
	else if (ParentSpaceShape->ChildrenSplitDirect == SDT_XDirect)
	{
		FSpaceShape* ParentFrameSpace = ParentSpaceShape->GetParentSpaceShape();
		if (ParentSpaceShape->IsInsideSpace() && ParentFrameSpace->IsFrameShapeSpace())
		{
			int32 ChildCount = ParentSpaceShape->GetChildSpaceShapes().Num();
			int32 CurIndex = ParentSpaceShape->GetChildSpaceShapeIndex(this);
			if (ChildCount - 1 == CurIndex)
			{
				TSharedPtr<FBoardShape> RightFrameBoard = ParentFrameSpace->GetFrameBoard(3, 0);
				//当前父级空间没有右侧板，继续向上查找
				if (!RightFrameBoard.IsValid())
				{
					return ParentFrameSpace->GetRightBoardInParentSpace(OutFrameBoard, OutSpaceShapes);
				}
				//当前父级空间存在右侧板
				OutFrameBoard = RightFrameBoard.Get();
				OutSpaceShapes.Add(this);
				return true;
			}
		}
	}
	else
	{
		return false;
	}
	return false;
}

bool FSpaceShape::GetLeftBoardInChildrenSpace(TArray<FBoardShape*>& OutFrameBoards, FBoardShape*& OutTopFrameBoard)
{
	if (!IsFrameShapeSpace() && !IsDrawerGroupSpace() && !IsEmbededElectricalGroup())
	{
		return false;
	}

	//子级空间的左板
	if (InsideSpace.IsValid() &&
		!InsideSpace->ChildrenSplitMode &&
		InsideSpace->ChildrenSplitDirect == SDT_ZDirect)
	{
		TArray<TSharedPtr<FSpaceShape>>& CurChildrenSpaces = InsideSpace->GetChildSpaceShapes();
		if (CurChildrenSpaces.Num() > 1)
		{
			int32 ChildCount = CurChildrenSpaces.Num();
			for (int32 i=0; i<ChildCount; ++i)
			{
				TSharedPtr<FBoardShape> LeftFrameBoard = CurChildrenSpaces[i]->GetFrameBoard(2, 0);
				if (LeftFrameBoard.IsValid())
				{
					OutFrameBoards.Add(LeftFrameBoard.Get());
				}
			}
			if (OutFrameBoards.Num() != ChildCount)
			{
				OutFrameBoards.Empty();
			}
			else
			{
				OutTopFrameBoard = OutFrameBoards[OutFrameBoards.Num() - 1];
			}
		}
	}
	return OutFrameBoards.Num() > 0;
}

bool FSpaceShape::GetRightBoardInChildrenSpace(TArray<FBoardShape*>& OutFrameBoards, FBoardShape*& OutTopFrameBoard)
{
	if (!IsFrameShapeSpace() && !IsDrawerGroupSpace() && !IsEmbededElectricalGroup())
	{
		return false;
	}
	//子级空间的左板
	if (InsideSpace.IsValid() &&
		!InsideSpace->ChildrenSplitMode &&
		InsideSpace->ChildrenSplitDirect == SDT_ZDirect)
	{
		TArray<TSharedPtr<FSpaceShape>>& CurChildrenSpaces = InsideSpace->GetChildSpaceShapes();
		if (CurChildrenSpaces.Num() > 1)
		{
			int32 ChildCount = CurChildrenSpaces.Num();
			for (int32 i = 0; i < ChildCount; ++i)
			{
				TSharedPtr<FBoardShape> LeftFrameBoard = CurChildrenSpaces[i]->GetFrameBoard(3, 0);
				if (LeftFrameBoard.IsValid())
				{
					OutFrameBoards.Add(LeftFrameBoard.Get());
				}
			}
			if (OutFrameBoards.Num() != ChildCount)
			{
				OutFrameBoards.Empty();
			}
			else
			{
				OutTopFrameBoard = OutFrameBoards[OutFrameBoards.Num() - 1];
			}
		}
	}
	return OutFrameBoards.Num() > 0;
}

float FSpaceShape::GetFrameBoardHeight(TArray<TSharedPtr<FBoardShape>>& OutResult)
{
	float OutBoardHeight = -99.9;
	for (auto CurBoard : OutResult)
	{
		float CurBoardHeight = CurBoard->GetShapeHeight();
		if (CurBoardHeight > OutBoardHeight)
		{
			OutBoardHeight = CurBoardHeight;
		}
	}
	return OutBoardHeight;
}

bool FSpaceShape::SetFrameBoardFrontExtension(TArray<TSharedPtr<FBoardShape>>& OutResult, float InValue)
{
	for (auto CurBoard : OutResult)
	{
		FSCTShape* ParentShape = CurBoard->GetParentShape();
		if (ParentShape->GetShapeType() == ST_Space)
		{
			FSpaceShape* ParentSpace = StaticCast<FSpaceShape*>(ParentShape);
			ParentSpace->AddSplitBoardFrontExtension(InValue);
		}
		else if (ParentShape->GetShapeType() == ST_Frame)
		{
			FFrameShape* ParentFrame = StaticCast<FFrameShape*>(ParentShape);
			ParentFrame->AddBoardFrontExtension(CurBoard.Get(), InValue);
		} 
		else
		{
			check(false);
		}
	}
	return true;
}

void FSpaceShape::GetAllBoardsInSpaceShape(TArray<FBoardShape*>& OutFrameBoards)
{
	if (!IsFrameShapeSpace())
	{
		return;
	}
	if (FrameShape.IsValid())
	{
		FrameShape->GetUpDownLeftRightFrameBoardShapes(OutFrameBoards);
	}
	if (InsideSpace.IsValid())
	{
		TArray<TSharedPtr<FSpaceShape>> CurChildrenSpaces = InsideSpace->GetChildSpaceShapes();
		for (auto it : CurChildrenSpaces)
		{
			it->GetAllBoardsInSpaceShape(OutFrameBoards);
		}
	}
}

void FSpaceShape::ExecuteCoveredSpaceInfluences(float InValue)
{
	for (auto it : CoverInfluencedSpaces)
	{
		//设置内缩值
		if (it->IsSplitBoardSpace())
		{
			it->AddSplitBoardFrontExtension(-InValue);
		}
		else if (bNestedAdded)
		{
			it->AddDoorNestedRetractValue(InValue);
		}
		else
		{
			it->AddDoorCoverRetractValue(InValue);
		}
	}
}

void FSpaceShape::CancelCoveredSpaceInfluences()
{
	for (auto it : CoverInfluencedSpaces)
	{
		//恢复内缩值
		if (it->IsSplitBoardSpace())
		{
			it->RemoveSplitBoardFrontExtension();
		}
		else if (bNestedAdded)
		{
			it->RemoveDoorNestedRetractValue();
		}
		else
		{
			it->RemoveDoorCoverRetractValue();
		}
	}
}

void FSpaceShape::ExecuteCoveredBoardInfluences(float InValue)
{
	for (auto it : CoverInfluencedBoards)
	{
		FSCTShape* CurParentShape = it->GetParentShape();
		if (CurParentShape->GetShapeType() == ST_Space)
		{
			FSpaceShape* CurParentSpace = StaticCast<FSpaceShape*>(CurParentShape);
			CurParentSpace->AddSplitBoardFrontExtension(-InValue);
		}
		else
		{
			FFrameShape* CurParentFrameShape = StaticCast<FFrameShape*>(CurParentShape);
			CurParentFrameShape->AddBoardFrontExtension(it, -InValue);
		}
	}
}

void FSpaceShape::CancelCoveredBoardInfluences()
{
	for (auto it : CoverInfluencedBoards)
	{
		FSCTShape* CurParentShape = it->GetParentShape();
		if (CurParentShape->GetShapeType() == ST_Space)
		{
			FSpaceShape* CurParentSpace = StaticCast<FSpaceShape*>(CurParentShape);
			CurParentSpace->RemoveSplitBoardFrontExtension();
		}
		else
		{
			FFrameShape* CurParentFrameShape = StaticCast<FFrameShape*>(CurParentShape);
			CurParentFrameShape->RemoveBoardFrontExtension(it);
		}
	}
}

void FSpaceShape::ExecuteInsetedBoardInfluences(float InValue)
{
	for (auto it : InsetInfluencedBoards)
	{
		FSCTShape* CurParentShape = it->GetParentShape();
		if (CurParentShape->GetShapeType() == ST_Frame)
		{
			FFrameShape* CurParentFrameShape = StaticCast<FFrameShape*>(CurParentShape);
			CurParentFrameShape->AddBoardUpExtension(it, InValue);
		}
		else
		{
			check(false);
		}
	}
}

void FSpaceShape::CancelInsetedBoardInfluences()
{
	for (auto it : InsetInfluencedBoards)
	{
		FSCTShape* CurParentShape = it->GetParentShape();
		if (CurParentShape->GetShapeType() == ST_Frame)
		{
			FFrameShape* CurParentFrameShape = StaticCast<FFrameShape*>(CurParentShape);
			CurParentFrameShape->RemoveBoardUpExtension(it);
		}
		else
		{
			check(false);
		}
	}
}

void FSpaceShape::ClearDoorGroupInfluences()
{
	InsetInfluencedBoards.Empty();
	CoverInfluencedBoards.Empty();
	CoverInfluencedSpaces.Empty();
}

float FSpaceShape::GetDoorGroupRetractValue()
{
	if (DoorCoverRetractValue < SMALL_NUMBER && DoorNestedRetractValue < SMALL_NUMBER)
	{
		return 0.0;
	}
	else if (DoorCoverRetractValue > SMALL_NUMBER && DoorNestedRetractValue < SMALL_NUMBER)
	{
		return DoorCoverRetractValue;		
	}
	else if (DoorCoverRetractValue < SMALL_NUMBER && DoorNestedRetractValue > SMALL_NUMBER)
	{
		return DoorNestedRetractValue;
	}
	else
	{
		if (IsDrawerGroupSpace())
		{
			return bNestedAdded ? DoorNestedRetractValue + DoorCoverRetractValue : DoorCoverRetractValue;
		}
		else if (IsFrameShapeSpace())
		{
			if (!DoorShape.IsValid())
			{
				return DoorCoverRetractValue;
			}
			else
			{
				return bNestedAdded ? DoorNestedRetractValue + DoorCoverRetractValue : DoorCoverRetractValue;
			}
		}
	}
	return 0.0;
}

bool FSpaceShape::FitSideHungDoorGroupToCurSpaceShape(FSideHungDoor* InDoorGroup)
{
	//1、框体空间中添加掩门门组
	check(IsFrameShapeSpace());

	//2、内部空间的尺寸和位置
	FVector InsideSpacePos;
	FVector InsideSpaceDim;
	bool bInsidePosDim = FrameShape->CalInsideSpacePosDim(InsideSpacePos, InsideSpaceDim);
	check(bInsidePosDim);

	bool bHasFrontBoard = false;
	FSpaceShape * TempPtr = this;
	do
	{
		if (TempPtr->GetFrameBoard(5,0).IsValid())
		{
			bHasFrontBoard = true;
			break;
		}
		else
		{
			TempPtr = TempPtr->GetParentSpaceShape();
		}

	} while (TempPtr);




	//3、掩门门组四边的掩盖方式(0-未定义，1-全盖，2-半盖，3-内嵌，4-平盖)
	//4、计算门组的位置和尺寸，同时处理框体板件的外延值和受影响的空间内缩值
	TArray<FSpaceShape*> CurCoverInfluencedSpaces;
	FBoardShape* TopBoardShape = nullptr;
	FBoardShape* BottomBoardShape = nullptr;
	FBoardShape* LeftBoardShape = nullptr;
	FBoardShape* RightBoardShape = nullptr;
	int32 UpCorverType = InDoorGroup->GetUpCorverType();
	int32 DownCorverType = InDoorGroup->GetDownCorverType();
	int32 LeftCorverType = InDoorGroup->GetLeftCorverType();
	int32 RightCorverType = InDoorGroup->GetRightCorverType();

	//5、处理上侧框体板
	if (UpCorverType == 1 || UpCorverType == 2)
	{
		float TopBoardHeight = 0.0;
		//当前空间的上侧框体板
		TSharedPtr<FBoardShape> CurTopBoardShape = FrameShape->GetFrameBoardShape(0, 0);
		if (CurTopBoardShape.IsValid())
		{
			TopBoardHeight = CurTopBoardShape->GetShapeHeight();
			TopBoardShape = CurTopBoardShape.Get();
			CurCoverInfluencedSpaces.Add(this);
		}
		else if(bHasFrontBoard == false)
		{
			//上侧的分割板件
			FSpaceShape* CurInfluencedSpace = nullptr;
			bool bUpSplitBoard = GetUpSplitBoardShape(TopBoardShape, CurInfluencedSpace);
			if (bUpSplitBoard)
			{
				TopBoardHeight = TopBoardShape->GetShapeHeight();
				CurCoverInfluencedSpaces.Add(this);
				CurCoverInfluencedSpaces.Add(CurInfluencedSpace);
			}
			else
			{
				//父级上侧框体板
				bool bParentBoard = GetUpBoardInParentSpace(TopBoardShape, CurCoverInfluencedSpaces);
				if (bParentBoard)
				{
					TopBoardHeight = TopBoardShape->GetShapeHeight();
				}
			}
		}
		//根据框体板件厚度修正门组的尺寸和位置
		if (UpCorverType == 1)
		{
			InsideSpaceDim.Z += TopBoardHeight;
		}
		else if (UpCorverType == 2)
		{
			InsideSpaceDim.Z += TopBoardHeight / 2.0;
		}
	}
	else if(bHasFrontBoard == false)
	{
		CurCoverInfluencedSpaces.Add(this);
	}

	//6、处理下侧框体板
	if (DownCorverType == 1 || DownCorverType == 2)
	{
		float BottomBoardHeight = 0.0;
		//当前空间的下侧框体板
		TSharedPtr<FBoardShape> CurBottomBoardShape = FrameShape->GetFrameBoardShape(1, 0);
		if (CurBottomBoardShape.IsValid())
		{
			BottomBoardHeight = CurBottomBoardShape->GetShapeHeight();
			BottomBoardShape = CurBottomBoardShape.Get();
			CurCoverInfluencedSpaces.Add(this);
		}
		else if(bHasFrontBoard == false)
		{
			//下侧的分割板件
			FSpaceShape* CurInfluencedSpace = nullptr;
			bool bDownSplitBoard = GetDownSplitBoardShape(BottomBoardShape, CurInfluencedSpace);
			if (bDownSplitBoard)
			{
				BottomBoardHeight = BottomBoardShape->GetShapeHeight();
				CurCoverInfluencedSpaces.Add(this);
				CurCoverInfluencedSpaces.Add(CurInfluencedSpace);
			}
			else
			{
				//父级下侧框体板
				bool bParentBoard = GetDownBoardInParentSpace(BottomBoardShape, CurCoverInfluencedSpaces);
				if (bParentBoard)
				{
					BottomBoardHeight = BottomBoardShape->GetShapeHeight();
				}
			}
		}
		//根据框体板件厚度修正门组的尺寸和位置
		if (DownCorverType == 1)
		{
			InsideSpaceDim.Z += BottomBoardHeight;
			InsideSpacePos.Z -= BottomBoardHeight;
		}
		else if (DownCorverType == 2)
		{
			InsideSpaceDim.Z += BottomBoardHeight / 2.0;
			InsideSpacePos.Z -= BottomBoardHeight / 2.0;
		}
	}
	else if(bHasFrontBoard == false)
	{
		CurCoverInfluencedSpaces.Add(this);
	}

	//7、处理左侧框体板
	if (LeftCorverType == 1 || LeftCorverType == 2)
	{
		float LeftBoardHeight = 0.0;
		//当前空间的左侧框体板
		TSharedPtr<FBoardShape> CurLeftBoardShape = FrameShape->GetFrameBoardShape(2, 0);
		if (CurLeftBoardShape.IsValid())
		{
			LeftBoardHeight = CurLeftBoardShape->GetShapeHeight();
			LeftBoardShape = CurLeftBoardShape.Get();
			CurCoverInfluencedSpaces.Add(this);
		}
		else if(bHasFrontBoard == false)
		{
			//左侧的分割板件
			FSpaceShape* CurInfluencedSpace = nullptr;
			bool bLeftSplitBoard = GetLeftSplitBoardShape(LeftBoardShape, CurInfluencedSpace);
			if (bLeftSplitBoard)
			{
				LeftBoardHeight = LeftBoardShape->GetShapeHeight();
				CurCoverInfluencedSpaces.Add(this);
				CurCoverInfluencedSpaces.Add(CurInfluencedSpace);
			}
			else
			{
				//父级左侧框体板
				bool bParentBoard = GetLeftBoardInParentSpace(LeftBoardShape, CurCoverInfluencedSpaces);
				if (bParentBoard)
				{
					LeftBoardHeight = LeftBoardShape->GetShapeHeight();
				}
			}
		}
		//根据框体板件厚度修正门组的尺寸和位置
		if (LeftCorverType == 1)
		{
			InsideSpaceDim.X += LeftBoardHeight;
			InsideSpacePos.X -= LeftBoardHeight;
		}
		else if (LeftCorverType == 2)
		{
			InsideSpaceDim.X += LeftBoardHeight / 2.0;
			InsideSpacePos.X -= LeftBoardHeight / 2.0;
		}
	}
	else if(bHasFrontBoard == false)
	{
		CurCoverInfluencedSpaces.Add(this);
	}

	//8、处理右侧框体板
	if (RightCorverType == 1 || RightCorverType == 2)
	{
		float RightBoardHeight = 0.0;
		//当前空间的右侧框体板
		TSharedPtr<FBoardShape> CurRightBoardShape = FrameShape->GetFrameBoardShape(3, 0);
		if (CurRightBoardShape.IsValid())
		{
			RightBoardHeight = CurRightBoardShape->GetShapeHeight();
			RightBoardShape = CurRightBoardShape.Get();
			CurCoverInfluencedSpaces.Add(this);
		}
		else if(bHasFrontBoard == false)
		{
			//右侧的分割板件
			FSpaceShape* CurInfluencedSpace = nullptr;
			bool bRightSplitBoard = GetRightSplitBoardShape(RightBoardShape, CurInfluencedSpace);
			if (bRightSplitBoard)
			{
				RightBoardHeight = RightBoardShape->GetShapeHeight();
				CurCoverInfluencedSpaces.Add(this);
				CurCoverInfluencedSpaces.Add(CurInfluencedSpace);
			}
			else
			{
				//父级右侧框体板
				bool bParentBoard = GetRightBoardInParentSpace(RightBoardShape, CurCoverInfluencedSpaces);
				if (bParentBoard)
				{
					RightBoardHeight = RightBoardShape->GetShapeHeight();
				}
			}
		}
		//根据框体板件厚度修正门组的尺寸和位置
		if (RightCorverType == 1)
		{
			InsideSpaceDim.X += RightBoardHeight;
		}
		else if (RightCorverType == 2)
		{
			InsideSpaceDim.X += RightBoardHeight / 2.0;
		}
	}
	else if (bHasFrontBoard == false)
	{
		CurCoverInfluencedSpaces.Add(this);
	}

	//四边同时内嵌时，抽屉组认为是内嵌方式添加
	bNestedAdded = (UpCorverType == 3 && DownCorverType == 3 && LeftCorverType == 3 && RightCorverType == 3);

	//9、验证门组尺寸是否有效
	if ((!InDoorGroup->IsValidForWidth(InsideSpaceDim.X)) ||
		(!InDoorGroup->IsValidForHeight(InsideSpaceDim.Z)))
	{
		return false;
	}

	//11、调整框体板件的外延值，实现外盖
	if (TopBoardShape)
	{
		CoverInfluencedBoards.Add(TopBoardShape);
	}
	if (BottomBoardShape)
	{
		CoverInfluencedBoards.Add(BottomBoardShape);
	}
	if (LeftBoardShape)
	{
		CoverInfluencedBoards.Add(LeftBoardShape);
	}
	if (RightBoardShape)
	{
		CoverInfluencedBoards.Add(RightBoardShape);
	}

	if (InsideSpace.IsValid())
	{
		for (const auto & Ref : InsideSpace->GetChildSpaceShapes())
		{
			CurCoverInfluencedSpaces.Add(Ref.Get());
		}
	}
	if (bHasFrontBoard == false)
	{
		// 当前空间一定会受到影响
		CurCoverInfluencedSpaces.Add(this);
	}
	//12、设置受影响的内部空间的内缩值
	for (auto it : CurCoverInfluencedSpaces)
	{
		CoverInfluencedSpaces.Add(it);
		if (it != this)
		{
			it->GetAllBoardsInSpaceShape(CoverInfluencedBoards);
		}
	}

	//10、掩门门组的厚度+门后缝
	float DoorTotalDepth = InDoorGroup->GetDoorTotalDepth();
	ExecuteCoveredBoardInfluences(DoorTotalDepth);
	ExecuteCoveredSpaceInfluences(DoorTotalDepth);

	if (FrameShape.IsValid())
	{
		TSharedPtr<FBoardShape> FrontBoard = FrameShape->GetFrameBoardShape(5, 0);
		if (FrontBoard.IsValid())
		{
			InsideSpaceDim.Y += FrontBoard->GetShapeHeight() + FrontBoard->GetShapeContraction();
		}
	}


	//13、设置掩门门组的位置和尺寸
	const float CurDoorGroupDelta = GetDoorGroupRetractValue();
	InDoorGroup->SetShapePosX(InsideSpacePos.X);
	InDoorGroup->SetShapePosY(InsideSpacePos.Y + InsideSpaceDim.Y - CurDoorGroupDelta);
	InDoorGroup->SetShapePosZ(InsideSpacePos.Z);
	InDoorGroup->SetShapeWidth(InsideSpaceDim.X);
	InDoorGroup->SetShapeHeight(InsideSpaceDim.Z);

	return true;
}

bool FSpaceShape::FitSideHungDoorGroupToCurSpaceShapeWithoutconfluence(FSideHungDoor * InDoorGroup)
{
	//1、框体空间中添加掩门门组
	check(IsFrameShapeSpace());

	//2、内部空间的尺寸和位置
	FVector InsideSpacePos;
	FVector InsideSpaceDim;
	bool bInsidePosDim = FrameShape->CalInsideSpacePosDim(InsideSpacePos, InsideSpaceDim);
	check(bInsidePosDim);

	//3、掩门门组四边的掩盖方式(0-未定义，1-全盖，2-半盖，3-内嵌，4-平盖)
	//4、计算门组的位置和尺寸，同时处理框体板件的外延值和受影响的空间内缩值
	TArray<FSpaceShape*> CurCoverInfluencedSpaces;
	FBoardShape* TopBoardShape = nullptr;
	FBoardShape* BottomBoardShape = nullptr;
	FBoardShape* LeftBoardShape = nullptr;
	FBoardShape* RightBoardShape = nullptr;
	int32 UpCorverType = InDoorGroup->GetUpCorverType();
	int32 DownCorverType = InDoorGroup->GetDownCorverType();
	int32 LeftCorverType = InDoorGroup->GetLeftCorverType();
	int32 RightCorverType = InDoorGroup->GetRightCorverType();

	//5、处理上侧框体板
	if (UpCorverType == 1 || UpCorverType == 2)
	{
		float TopBoardHeight = 0.0;
		//当前空间的上侧框体板
		TSharedPtr<FBoardShape> CurTopBoardShape = FrameShape->GetFrameBoardShape(0, 0);
		if (CurTopBoardShape.IsValid())
		{
			TopBoardHeight = CurTopBoardShape->GetShapeHeight();
			TopBoardShape = CurTopBoardShape.Get();
			CurCoverInfluencedSpaces.Add(this);
		}
		else
		{
			//上侧的分割板件
			FSpaceShape* CurInfluencedSpace = nullptr;
			bool bUpSplitBoard = GetUpSplitBoardShape(TopBoardShape, CurInfluencedSpace);
			if (bUpSplitBoard)
			{
				TopBoardHeight = TopBoardShape->GetShapeHeight();
				CurCoverInfluencedSpaces.Add(this);
				CurCoverInfluencedSpaces.Add(CurInfluencedSpace);
			}
			else
			{
				//父级上侧框体板
				bool bParentBoard = GetUpBoardInParentSpace(TopBoardShape, CurCoverInfluencedSpaces);
				if (bParentBoard)
				{
					TopBoardHeight = TopBoardShape->GetShapeHeight();
				}
			}
		}
		//根据框体板件厚度修正门组的尺寸和位置
		if (UpCorverType == 1)
		{
			InsideSpaceDim.Z += TopBoardHeight;
		}
		else if (UpCorverType == 2)
		{
			InsideSpaceDim.Z += TopBoardHeight / 2.0;
		}
	}
	else
	{
		CurCoverInfluencedSpaces.Add(this);
	}

	//6、处理下侧框体板
	if (DownCorverType == 1 || DownCorverType == 2)
	{
		float BottomBoardHeight = 0.0;
		//当前空间的下侧框体板
		TSharedPtr<FBoardShape> CurBottomBoardShape = FrameShape->GetFrameBoardShape(1, 0);
		if (CurBottomBoardShape.IsValid())
		{
			BottomBoardHeight = CurBottomBoardShape->GetShapeHeight();
			BottomBoardShape = CurBottomBoardShape.Get();
			CurCoverInfluencedSpaces.Add(this);
		}
		else
		{
			//下侧的分割板件
			FSpaceShape* CurInfluencedSpace = nullptr;
			bool bDownSplitBoard = GetDownSplitBoardShape(BottomBoardShape, CurInfluencedSpace);
			if (bDownSplitBoard)
			{
				BottomBoardHeight = BottomBoardShape->GetShapeHeight();
				CurCoverInfluencedSpaces.Add(this);
				CurCoverInfluencedSpaces.Add(CurInfluencedSpace);
			}
			else
			{
				//父级下侧框体板
				bool bParentBoard = GetDownBoardInParentSpace(BottomBoardShape, CurCoverInfluencedSpaces);
				if (bParentBoard)
				{
					BottomBoardHeight = BottomBoardShape->GetShapeHeight();
				}
			}
		}
		//根据框体板件厚度修正门组的尺寸和位置
		if (DownCorverType == 1)
		{
			InsideSpaceDim.Z += BottomBoardHeight;
			InsideSpacePos.Z -= BottomBoardHeight;
		}
		else if (DownCorverType == 2)
		{
			InsideSpaceDim.Z += BottomBoardHeight / 2.0;
			InsideSpacePos.Z -= BottomBoardHeight / 2.0;
		}
	}
	else
	{
		CurCoverInfluencedSpaces.Add(this);
	}

	//7、处理左侧框体板
	if (LeftCorverType == 1 || LeftCorverType == 2)
	{
		float LeftBoardHeight = 0.0;
		//当前空间的左侧框体板
		TSharedPtr<FBoardShape> CurLeftBoardShape = FrameShape->GetFrameBoardShape(2, 0);
		if (CurLeftBoardShape.IsValid())
		{
			LeftBoardHeight = CurLeftBoardShape->GetShapeHeight();
			LeftBoardShape = CurLeftBoardShape.Get();
			CurCoverInfluencedSpaces.Add(this);
		}
		else
		{
			//左侧的分割板件
			FSpaceShape* CurInfluencedSpace = nullptr;
			bool bLeftSplitBoard = GetLeftSplitBoardShape(LeftBoardShape, CurInfluencedSpace);
			if (bLeftSplitBoard)
			{
				LeftBoardHeight = LeftBoardShape->GetShapeHeight();
				CurCoverInfluencedSpaces.Add(this);
				CurCoverInfluencedSpaces.Add(CurInfluencedSpace);
			}
			else
			{
				//父级左侧框体板
				bool bParentBoard = GetLeftBoardInParentSpace(LeftBoardShape, CurCoverInfluencedSpaces);
				if (bParentBoard)
				{
					LeftBoardHeight = LeftBoardShape->GetShapeHeight();
				}
			}
		}
		//根据框体板件厚度修正门组的尺寸和位置
		if (LeftCorverType == 1)
		{
			InsideSpaceDim.X += LeftBoardHeight;
			InsideSpacePos.X -= LeftBoardHeight;
		}
		else if (LeftCorverType == 2)
		{
			InsideSpaceDim.X += LeftBoardHeight / 2.0;
			InsideSpacePos.X -= LeftBoardHeight / 2.0;
		}
	}
	else
	{
		CurCoverInfluencedSpaces.Add(this);
	}

	//8、处理右侧框体板
	if (RightCorverType == 1 || RightCorverType == 2)
	{
		float RightBoardHeight = 0.0;
		//当前空间的右侧框体板
		TSharedPtr<FBoardShape> CurRightBoardShape = FrameShape->GetFrameBoardShape(3, 0);
		if (CurRightBoardShape.IsValid())
		{
			RightBoardHeight = CurRightBoardShape->GetShapeHeight();
			RightBoardShape = CurRightBoardShape.Get();
			CurCoverInfluencedSpaces.Add(this);
		}
		else
		{
			//右侧的分割板件
			FSpaceShape* CurInfluencedSpace = nullptr;
			bool bRightSplitBoard = GetRightSplitBoardShape(RightBoardShape, CurInfluencedSpace);
			if (bRightSplitBoard)
			{
				RightBoardHeight = RightBoardShape->GetShapeHeight();
				CurCoverInfluencedSpaces.Add(this);
				CurCoverInfluencedSpaces.Add(CurInfluencedSpace);
			}
			else
			{
				//父级右侧框体板
				bool bParentBoard = GetRightBoardInParentSpace(RightBoardShape, CurCoverInfluencedSpaces);
				if (bParentBoard)
				{
					RightBoardHeight = RightBoardShape->GetShapeHeight();
				}
			}
		}
		//根据框体板件厚度修正门组的尺寸和位置
		if (RightCorverType == 1)
		{
			InsideSpaceDim.X += RightBoardHeight;
		}
		else if (RightCorverType == 2)
		{
			InsideSpaceDim.X += RightBoardHeight / 2.0;
		}
	}
	else
	{
		CurCoverInfluencedSpaces.Add(this);
	}
	//9、验证门组尺寸是否有效
	if ((!InDoorGroup->IsValidForWidth(InsideSpaceDim.X)) ||
		(!InDoorGroup->IsValidForHeight(InsideSpaceDim.Z)))
	{
		return false;
	}	
	return true;
}

bool FSpaceShape::FitSlidingDoorGroupToCurSpaceShape(FSlidingDoor* InDoorGroup)
{
	//1、框体空间中添加移门门组
	check(IsFrameShapeSpace());

	//2、内部空间的尺寸和位置
	FVector InsideSpacePos;
	FVector InsideSpaceDim;
	bool bInsidePosDim = FrameShape->CalInsideSpacePosDim(InsideSpacePos, InsideSpaceDim);
	check(bInsidePosDim);


	bool bHasFrontBoard = false;
	FSpaceShape * TempPtr = this;
	do
	{
		if (TempPtr->GetFrameBoard(5, 0).IsValid())
		{
			bHasFrontBoard = true;
			break;
		}
		else
		{
			TempPtr = TempPtr->GetParentSpaceShape();
		}

	} while (TempPtr);

	//3、判断五金类型
	//TODO：双轨移门、吊轨移门、悬挂移门、单轨移门
	//暂时只支持双轨移门

	//判断移门掩盖方式
	int32 CorverType = InDoorGroup->GetCorverType();
	switch (CorverType)
	{
		//4、内嵌方式添加移门门组
		case 1:  //内嵌
		{
			bNestedAdded = true;
			//4.2、判断左右侧板的情况，修正门组的位置和尺寸
			FBoardShape* CurLeftBoardShape = nullptr;
			FBoardShape* CurRightBoardShape = nullptr;
			TArray<FBoardShape*> CurLeftChildBoards;
			TArray<FBoardShape*> CurRightChildBoards;
			//判断左侧板情况
			TSharedPtr<FBoardShape> LeftBoardShape = FrameShape->GetFrameBoardShape(2, 0);
			if (LeftBoardShape.IsValid())
			{
				CurLeftBoardShape = LeftBoardShape.Get();
				CurLeftChildBoards.Add(LeftBoardShape.Get());
			}
			else if(bHasFrontBoard == false)//查找子级空间的左侧板
			{
				bool bChildrenBoard = GetLeftBoardInChildrenSpace(CurLeftChildBoards, CurLeftBoardShape);
				if (bChildrenBoard)
				{
					float LeftBoardHeight = CurLeftChildBoards[0]->GetShapeHeight();
					InsideSpacePos.X += LeftBoardHeight;
					InsideSpaceDim.X -= LeftBoardHeight;
				}
			}
			//判断右侧板情况
			TSharedPtr<FBoardShape> RightBoardShape = FrameShape->GetFrameBoardShape(3, 0);
			if (RightBoardShape.IsValid())
			{
				CurRightBoardShape = RightBoardShape.Get();
				CurRightChildBoards.Add(RightBoardShape.Get());
			}
			else if (bHasFrontBoard == false) //查找子级空间的右侧板
			{
				bool bChildrenBoard = GetRightBoardInChildrenSpace(CurRightChildBoards, CurRightBoardShape);
				if (bChildrenBoard)
				{
					float RightBoardHeight = CurRightChildBoards[0]->GetShapeHeight();
					InsideSpaceDim.X -= RightBoardHeight;
				}
			}

			//4.3、验证门组尺寸是否有效
			if ((!InDoorGroup->IsValidForWidth(InsideSpaceDim.X)) ||
				(!InDoorGroup->IsValidForHeight(InsideSpaceDim.Z)))
			{
				return false;
			}

			//4.4、判断是否存在顶板和底板，决定是否存在上垫板和下垫板
			TSharedPtr<FBoardShape> TopBoardShape = FrameShape->GetFrameBoardShape(0, 0);
			InDoorGroup->SetUpPlateBoardValid(!TopBoardShape.IsValid());
			TSharedPtr<FBoardShape> BottomBoardShape = FrameShape->GetFrameBoardShape(1, 0);
			InDoorGroup->SetDownPlateBoardValid(!BottomBoardShape.IsValid());

			//4.5统计门组添加影响的框体板件和空间
			if (!TopBoardShape.IsValid())
			{
				if (CurLeftBoardShape)
				{
					InsetInfluencedBoards.Add(CurLeftBoardShape);
				}
				if (CurRightBoardShape)
				{
					InsetInfluencedBoards.Add(CurRightBoardShape);
				}
			}
			//当前空间受影响
			CoverInfluencedSpaces.Add(this);
			//查找当前空间的所有框体板件
			GetAllBoardsInSpaceShape(CoverInfluencedBoards);
			//移除门组四边的框体板件
			for (auto it : CurLeftChildBoards)
			{
				CoverInfluencedBoards.Remove(it);
			}
			for (auto it : CurRightChildBoards)
			{
				CoverInfluencedBoards.Remove(it);
			}
			if (TopBoardShape.IsValid())
			{
				CoverInfluencedBoards.Remove(TopBoardShape.Get());
			}
			if (BottomBoardShape.IsValid())
			{
				CoverInfluencedBoards.Remove(BottomBoardShape.Get());
			}
		
			//4.6、执行门组添加对框体板件和空间的影响
			float SlidewayPresetValue = InDoorGroup->GetSlidewayPresetValue();
			ExecuteCoveredBoardInfluences(SlidewayPresetValue);
			ExecuteCoveredSpaceInfluences(SlidewayPresetValue);
			TSharedPtr<FBoardShape> UpPlateBoard = InDoorGroup->GetUpPlateBoard();
			float UpPlateBoardHeight = UpPlateBoard->GetShapeHeight();
			ExecuteInsetedBoardInfluences(UpPlateBoardHeight);

			//4.7、设置门组的尺寸和位置
			float CurDoorGroupDelta = GetDoorGroupRetractValue();
			InDoorGroup->SetShapeWidth(InsideSpaceDim.X);
			InDoorGroup->SetShapeHeight(InsideSpaceDim.Z);
			InDoorGroup->SetShapePosX(InsideSpacePos.X);
			InDoorGroup->SetShapePosY(InsideSpacePos.Y + InsideSpaceDim.Y - CurDoorGroupDelta);
			InDoorGroup->SetShapePosZ(InsideSpacePos.Z);

			break;
		}
		//5、外盖方式添加移门门组
		case 2:  //外盖
		{
			bNestedAdded = false;
			//5.2、判断是否存在顶、底、左、右板，计算板件厚度
			float TopBoardHeight = 0.0;
			float BottomBoardHeight = 0.0;
			float LeftBoardHeight = 0.0;
			float RightBoardHeight = 0.0;
			TSharedPtr<FBoardShape> TopBoardShape = FrameShape->GetFrameBoardShape(0, 0);
			if (TopBoardShape.IsValid())
			{
				TopBoardHeight = TopBoardShape->GetShapeHeight();
			}
			TSharedPtr<FBoardShape> BottomBoardShape = FrameShape->GetFrameBoardShape(1, 0);
			if (BottomBoardShape.IsValid())
			{
				BottomBoardHeight = BottomBoardShape->GetShapeHeight();
			}
			TSharedPtr<FBoardShape> LeftBoardShape = FrameShape->GetFrameBoardShape(2, 0);
			if (LeftBoardShape.IsValid())
			{
				LeftBoardHeight = LeftBoardShape->GetShapeHeight();
			}
			TSharedPtr<FBoardShape> RightBoardShape = FrameShape->GetFrameBoardShape(3, 0);
			if (RightBoardShape.IsValid())
			{
				RightBoardHeight = RightBoardShape->GetShapeHeight();
			}

			//5.3、计算门组尺寸
			float DoorGroupWidth = InsideSpaceDim.X + LeftBoardHeight + RightBoardHeight;
			float DoorGroupHeight = InsideSpaceDim.Z + TopBoardHeight + BottomBoardHeight;

			//5.4、验证门组尺寸是否有效
			if ((!InDoorGroup->IsValidForWidth(DoorGroupWidth)) ||
				(!InDoorGroup->IsValidForHeight(DoorGroupHeight)))
			{
				return false;
			}

			//5.5、统计受影响的框体板件和空间
			CoverInfluencedSpaces.Add(this);
			GetAllBoardsInSpaceShape(CoverInfluencedBoards);

			//5.6、执行门组添加对框体板件和空间的影响
			float SlidewayPresetValue = InDoorGroup->GetSlidewayPresetValue();//移门滑轨预设值
			ExecuteCoveredBoardInfluences(SlidewayPresetValue);
			ExecuteCoveredSpaceInfluences(SlidewayPresetValue);

			if (FrameShape.IsValid())
			{
				TSharedPtr<FBoardShape> FrontBoard = FrameShape->GetFrameBoardShape(5, 0);
				if (FrontBoard.IsValid())
				{
					InsideSpaceDim.Y += FrontBoard->GetShapeHeight() + FrontBoard->GetShapeContraction();
				}
			}

			//5.7、设置门组的尺寸和位置
			float CurDoorGroupDelta = GetDoorGroupRetractValue();
			InDoorGroup->SetShapeWidth(DoorGroupWidth);
			InDoorGroup->SetShapeHeight(DoorGroupHeight);
			InDoorGroup->SetShapePosX(InsideSpacePos.X - LeftBoardHeight);
			InDoorGroup->SetShapePosY(InsideSpacePos.Y + InsideSpaceDim.Y - CurDoorGroupDelta);
			InDoorGroup->SetShapePosZ(InsideSpacePos.Z - BottomBoardHeight);

			break;
		}
		default:
			check(false);
			break;
	}

	return true;
}

bool FSpaceShape::FitCoveredDrawerGroupToCurSpaceShape(FDrawerGroupShape* InDrawerGroup)
{
	//1、框体空间中添加外盖抽屉组和抽屉空间中更新
	if (!IsFrameShapeSpace() && !IsDrawerGroupSpace())
	{
		check(false);
		return false;
	}

	//2、内部空间的尺寸和位置
	FVector InsideSpacePos;
	FVector InsideSpaceDim;
	bool bInsidePosDim = FrameShape->CalInsideSpacePosDim(InsideSpacePos, InsideSpaceDim);
	check(bInsidePosDim);

	//3、掩门门组四边的掩盖方式(0-未定义，1-全盖，2-半盖，3-内嵌)
	int32 UpCorverType = InDrawerGroup->GetUpCorverType();
	int32 DownCorverType = InDrawerGroup->GetDownCorverType();
	int32 LeftCorverType = InDrawerGroup->GetLeftCorverType();
	int32 RightCorverType = InDrawerGroup->GetRightCorverType();
	//4、计算门组的位置和尺寸，同时处理框体板件的外延值和受影响的空间内缩值
	TArray<FSpaceShape*> CurCoverInfluencedSpaces;
	FBoardShape* TopBoardShape = nullptr;
	FBoardShape* BottomBoardShape = nullptr;
	FBoardShape* LeftBoardShape = nullptr;
	FBoardShape* RightBoardShape = nullptr;
	float TopBoardHeight = 0.0;
	float BottomBoardHeight = 0.0;
	float LeftBoardHeight = 0.0;
	float RightBoardHeight = 0.0;

	//5、处理上侧框体板
	if (UpCorverType == 1 || UpCorverType == 2)
	{
		//当前空间的上侧框体板
		TSharedPtr<FBoardShape> CurTopBoardShape = FrameShape->GetFrameBoardShape(0, 0);
		if (CurTopBoardShape.IsValid())
		{
			TopBoardHeight = CurTopBoardShape->GetShapeHeight();
			TopBoardShape = CurTopBoardShape.Get();
			CurCoverInfluencedSpaces.Add(this);
		}
		else
		{
			//上侧的分割板件
			FSpaceShape* CurInfluencedSpace = nullptr;
			bool bUpSplitBoard = GetUpSplitBoardShape(TopBoardShape, CurInfluencedSpace);
			if (bUpSplitBoard)
			{
				TopBoardHeight = TopBoardShape->GetShapeHeight();
				CurCoverInfluencedSpaces.Add(this);
				CurCoverInfluencedSpaces.Add(CurInfluencedSpace);
			}
			else
			{
				//父级上侧框体板
				bool bParentBoard = GetUpBoardInParentSpace(TopBoardShape, CurCoverInfluencedSpaces);
				if (bParentBoard)
				{
					TopBoardHeight = TopBoardShape->GetShapeHeight();
				}
				else
				{
					CurCoverInfluencedSpaces.Add(this);
				}
			}
		}
	}
	else
	{
		CurCoverInfluencedSpaces.Add(this);
	}

	//6、处理下侧框体板
	if (DownCorverType == 1 || DownCorverType == 2)
	{
		//当前空间的下侧框体板
		TSharedPtr<FBoardShape> CurBottomBoardShape = FrameShape->GetFrameBoardShape(1, 0);
		if (CurBottomBoardShape.IsValid())
		{
			BottomBoardHeight = CurBottomBoardShape->GetShapeHeight();
			BottomBoardShape = CurBottomBoardShape.Get();
			CurCoverInfluencedSpaces.Add(this);
		}
		else
		{
			//下侧的分割板件
			FSpaceShape* CurInfluencedSpace = nullptr;
			bool bDownSplitBoard = GetDownSplitBoardShape(BottomBoardShape, CurInfluencedSpace);
			if (bDownSplitBoard)
			{
				BottomBoardHeight = BottomBoardShape->GetShapeHeight();
				CurCoverInfluencedSpaces.Add(this);
				CurCoverInfluencedSpaces.Add(CurInfluencedSpace);
			}
			else
			{
				//父级下侧框体板
				bool bParentBoard = GetDownBoardInParentSpace(BottomBoardShape, CurCoverInfluencedSpaces);
				if (bParentBoard)
				{
					BottomBoardHeight = BottomBoardShape->GetShapeHeight();
				}
				else
				{
					CurCoverInfluencedSpaces.Add(this);
				}
			}
		}
	}
	else
	{
		CurCoverInfluencedSpaces.Add(this);
	}

	//7、处理左侧框体板
	if (LeftCorverType == 1 || LeftCorverType == 2)
	{
		//当前空间的左侧框体板
		TSharedPtr<FBoardShape> CurLeftBoardShape = FrameShape->GetFrameBoardShape(2, 0);
		if (CurLeftBoardShape.IsValid())
		{
			LeftBoardHeight = CurLeftBoardShape->GetShapeHeight();
			LeftBoardShape = CurLeftBoardShape.Get();
			CurCoverInfluencedSpaces.Add(this);
		}
		else
		{
			//左侧的分割板件
			FSpaceShape* CurInfluencedSpace = nullptr;
			bool bLeftSplitBoard = GetLeftSplitBoardShape(LeftBoardShape, CurInfluencedSpace);
			if (bLeftSplitBoard)
			{
				LeftBoardHeight = LeftBoardShape->GetShapeHeight();
				CurCoverInfluencedSpaces.Add(this);
				CurCoverInfluencedSpaces.Add(CurInfluencedSpace);
			}
			else
			{
				//父级左侧框体板
				bool bParentBoard = GetLeftBoardInParentSpace(LeftBoardShape, CurCoverInfluencedSpaces);
				if (bParentBoard)
				{
					LeftBoardHeight = LeftBoardShape->GetShapeHeight();
				}
				else
				{
					CurCoverInfluencedSpaces.Add(this);
				}
			}
		}
	}
	else
	{
		CurCoverInfluencedSpaces.Add(this);
	}

	//8、处理右侧框体板
	if (RightCorverType == 1 || RightCorverType == 2)
	{
		//当前空间的右侧框体板
		TSharedPtr<FBoardShape> CurRightBoardShape = FrameShape->GetFrameBoardShape(3, 0);
		if (CurRightBoardShape.IsValid())
		{
			RightBoardHeight = CurRightBoardShape->GetShapeHeight();
			RightBoardShape = CurRightBoardShape.Get();
			CurCoverInfluencedSpaces.Add(this);
		}
		else
		{
			//右侧的分割板件
			FSpaceShape* CurInfluencedSpace = nullptr;
			bool bRightSplitBoard = GetRightSplitBoardShape(RightBoardShape, CurInfluencedSpace);
			if (bRightSplitBoard)
			{
				RightBoardHeight = RightBoardShape->GetShapeHeight();
				CurCoverInfluencedSpaces.Add(this);
				CurCoverInfluencedSpaces.Add(CurInfluencedSpace);
			}
			else
			{
				//父级右侧框体板
				bool bParentBoard = GetRightBoardInParentSpace(RightBoardShape, CurCoverInfluencedSpaces);
				if (bParentBoard)
				{
					RightBoardHeight = RightBoardShape->GetShapeHeight();
				}
				else
				{
					CurCoverInfluencedSpaces.Add(this);
				}
			}
		}
	}
	else
	{
		CurCoverInfluencedSpaces.Add(this);
	}

	//四边同时内嵌时，抽屉组认为是内嵌方式添加
	bNestedAdded = (UpCorverType == 3 && DownCorverType == 3 && LeftCorverType == 3 && RightCorverType == 3);
	//9、根据内部空间尺寸及掩盖方式等计算抽屉组的深度
	//掩门门组的厚度+门后缝
	float CurDrawerDoorDepth = InDrawerGroup->GetDrawerDoorTotalDepth();
	//门组引起的内胆内缩值及计数
	float CurDoorCoverRetractValue = GetDoorCoverRetractValue();
	int32 CurDoorCoverRetractCount = GetDoorCoverRetractCount();
	if (bNestedAdded && CurDoorCoverRetractCount > 0)
	{
		//计算外盖抽屉的深度
		if (IsDrawerGroupSpace())
		{
			InsideSpaceDim.Y -= CurDoorCoverRetractValue;
		}
		else  //第一次添加外盖抽屉组
		{
			InsideSpaceDim.Y -= CurDrawerDoorDepth;
		}
	}

	//9、验证门组尺寸是否有效
	if ((!InDrawerGroup->IsValidForWidth(InsideSpaceDim.X)) ||
		(!InDrawerGroup->IsValidForDepth(InsideSpaceDim.Y)) ||
		(!InDrawerGroup->IsValidForHeight(InsideSpaceDim.Z)))
	{
		return false;
	}

	//10、调整框体板件的外延值，实现外盖
	if (TopBoardShape)
	{
		CoverInfluencedBoards.Add(TopBoardShape);
	}
	if (BottomBoardShape)
	{
		CoverInfluencedBoards.Add(BottomBoardShape);
	}
	if (LeftBoardShape)
	{
		CoverInfluencedBoards.Add(LeftBoardShape);
	}
	if (RightBoardShape)
	{
		CoverInfluencedBoards.Add(RightBoardShape);
	}
	//11、设置受影响的内部空间的内缩值
	for (auto it : CurCoverInfluencedSpaces)
	{
		CoverInfluencedSpaces.Add(it);
		if (it != this)
		{
			it->GetAllBoardsInSpaceShape(CoverInfluencedBoards);
		}
	}
	ExecuteCoveredBoardInfluences(CurDrawerDoorDepth);
	ExecuteCoveredSpaceInfluences(CurDrawerDoorDepth);

	//13、根据框体板件厚度计算抽面组外延值
	float UpExtension = 0.0;
	float DownExtension = 0.0;
	float LeftExtension = 0.0;
	float RightExtension = 0.0;
	if (UpCorverType == 1)
	{
		UpExtension = TopBoardHeight;
	}
	else if (UpCorverType == 2)
	{
		UpExtension = TopBoardHeight / 2.0;
	}
	if (DownCorverType == 1)
	{
		DownExtension = BottomBoardHeight;
	}
	else if (DownCorverType == 2)
	{
		DownExtension = BottomBoardHeight / 2.0;
	}
	if (LeftCorverType == 1)
	{
		LeftExtension = LeftBoardHeight;
	}
	else if (LeftCorverType == 2)
	{
		LeftExtension = LeftBoardHeight / 2.0;
	}
	if (RightCorverType == 1)
	{
		RightExtension = RightBoardHeight;
	}
	else if (RightCorverType == 2)
	{
		RightExtension = RightBoardHeight / 2.0;
	}

	//14、设置掩门门组的位置和尺寸
	InDrawerGroup->SetShapePosX(InsideSpacePos.X);
	InDrawerGroup->SetShapePosY(InsideSpacePos.Y);
	InDrawerGroup->SetShapePosZ(InsideSpacePos.Z);
	//设置抽屉组的尺寸和外延值
	InDrawerGroup->SetGroupDimensionExtension(
		InsideSpaceDim.X, InsideSpaceDim.Y, InsideSpaceDim.Z,
		UpExtension, DownExtension, LeftExtension, RightExtension);

	return true;
}

bool FSpaceShape::FitEmbededElectricalGroupToCurSpaceShape(FEmbededElectricalGroup * InEmbededElectricalGroup)
{
	//2、内部空间的尺寸和位置
	FVector InsideSpacePos;
	FVector InsideSpaceDim;
	bool bInsidePosDim = FrameShape->CalInsideSpacePosDim(InsideSpacePos, InsideSpaceDim);
	check(bInsidePosDim);

	float UpExtension = 0.0;
	float DownExtension = 0.0;
	float LeftExtension = 0.0;
	float RightExtension = 0.0;
	if (InEmbededElectricalGroup->GetInsertionBoardShape().IsValid())
	{
		//3、饰条的掩盖方式(0-未定义，1-全盖，2-半盖，3-内嵌),暂时只支持外盖
		const int32 UpCorverType = 1;
		const int32 DownCorverType = 1;
		const int32 LeftCorverType = 1;
		const int32 RightCorverType = 1;

		//4、计算饰条尺寸，同时处理框体板件的外延值和受影响的空间内缩值
		TArray<FSpaceShape*> CurCoverInfluencedSpaces;
		FBoardShape* TopBoardShape = nullptr;
		FBoardShape* BottomBoardShape = nullptr;
		FBoardShape* LeftBoardShape = nullptr;
		FBoardShape* RightBoardShape = nullptr;
		float TopBoardHeight = 0.0;
		float BottomBoardHeight = 0.0;
		float LeftBoardHeight = 0.0;
		float RightBoardHeight = 0.0;

		//5、处理上侧框体板
		if (UpCorverType == 1 || UpCorverType == 2)
		{
			//当前空间的上侧框体板
			TSharedPtr<FBoardShape> CurTopBoardShape = FrameShape->GetFrameBoardShape(0, 0);
			if (CurTopBoardShape.IsValid())
			{
				TopBoardHeight = CurTopBoardShape->GetShapeHeight();
				TopBoardShape = CurTopBoardShape.Get();
				CurCoverInfluencedSpaces.Add(this);
			}
			else
			{
				//上侧的分割板件
				FSpaceShape* CurInfluencedSpace = nullptr;
				bool bUpSplitBoard = GetUpSplitBoardShape(TopBoardShape, CurInfluencedSpace);
				if (bUpSplitBoard)
				{
					TopBoardHeight = TopBoardShape->GetShapeHeight();
					CurCoverInfluencedSpaces.Add(this);
					CurCoverInfluencedSpaces.Add(CurInfluencedSpace);
				}
				else
				{
					//父级上侧框体板
					bool bParentBoard = GetUpBoardInParentSpace(TopBoardShape, CurCoverInfluencedSpaces);
					if (bParentBoard)
					{
						TopBoardHeight = TopBoardShape->GetShapeHeight();
					}
					else
					{
						CurCoverInfluencedSpaces.Add(this);
					}
				}
			}
		}
		else
		{
			CurCoverInfluencedSpaces.Add(this);
		}

		//6、处理下侧框体板
		if (DownCorverType == 1 || DownCorverType == 2)
		{
			//当前空间的下侧框体板
			TSharedPtr<FBoardShape> CurBottomBoardShape = FrameShape->GetFrameBoardShape(1, 0);
			if (CurBottomBoardShape.IsValid())
			{
				BottomBoardHeight = CurBottomBoardShape->GetShapeHeight();
				BottomBoardShape = CurBottomBoardShape.Get();
				CurCoverInfluencedSpaces.Add(this);
			}
			else
			{
				//下侧的分割板件
				FSpaceShape* CurInfluencedSpace = nullptr;
				bool bDownSplitBoard = GetDownSplitBoardShape(BottomBoardShape, CurInfluencedSpace);
				if (bDownSplitBoard)
				{
					BottomBoardHeight = BottomBoardShape->GetShapeHeight();
					CurCoverInfluencedSpaces.Add(this);
					CurCoverInfluencedSpaces.Add(CurInfluencedSpace);
				}
				else
				{
					//父级下侧框体板
					bool bParentBoard = GetDownBoardInParentSpace(BottomBoardShape, CurCoverInfluencedSpaces);
					if (bParentBoard)
					{
						BottomBoardHeight = BottomBoardShape->GetShapeHeight();
					}
					else
					{
						CurCoverInfluencedSpaces.Add(this);
					}
				}
			}
		}
		else
		{
			CurCoverInfluencedSpaces.Add(this);
		}

		//7、处理左侧框体板
		if (LeftCorverType == 1 || LeftCorverType == 2)
		{
			//当前空间的左侧框体板
			TSharedPtr<FBoardShape> CurLeftBoardShape = FrameShape->GetFrameBoardShape(2, 0);
			if (CurLeftBoardShape.IsValid())
			{				
				LeftBoardHeight = CurLeftBoardShape->GetShapeHeight();
				LeftBoardShape = CurLeftBoardShape.Get();
				CurCoverInfluencedSpaces.Add(this);
			}
			else
			{
				//左侧的分割板件
				FSpaceShape* CurInfluencedSpace = nullptr;
				bool bLeftSplitBoard = GetLeftSplitBoardShape(LeftBoardShape, CurInfluencedSpace);
				if (bLeftSplitBoard)
				{
					LeftBoardHeight = LeftBoardShape->GetShapeHeight();
					CurCoverInfluencedSpaces.Add(this);
					CurCoverInfluencedSpaces.Add(CurInfluencedSpace);
				}
				else
				{
					//父级左侧框体板
					bool bParentBoard = GetLeftBoardInParentSpace(LeftBoardShape, CurCoverInfluencedSpaces);
					if (bParentBoard)
					{
						LeftBoardHeight = LeftBoardShape->GetShapeHeight();
					}
					else
					{
						CurCoverInfluencedSpaces.Add(this);
					}
				}
			}
		}
		else
		{
			CurCoverInfluencedSpaces.Add(this);
		}

		//8、处理右侧框体板
		if (RightCorverType == 1 || RightCorverType == 2)
		{
			//当前空间的右侧框体板
			TSharedPtr<FBoardShape> CurRightBoardShape = FrameShape->GetFrameBoardShape(3, 0);
			if (CurRightBoardShape.IsValid())
			{				
				RightBoardHeight = CurRightBoardShape->GetShapeHeight();
				RightBoardShape = CurRightBoardShape.Get();
				CurCoverInfluencedSpaces.Add(this);
			}
			else
			{
				//右侧的分割板件
				FSpaceShape* CurInfluencedSpace = nullptr;
				bool bRightSplitBoard = GetRightSplitBoardShape(RightBoardShape, CurInfluencedSpace);
				if (bRightSplitBoard)
				{
					RightBoardHeight = RightBoardShape->GetShapeHeight();
					CurCoverInfluencedSpaces.Add(this);
					CurCoverInfluencedSpaces.Add(CurInfluencedSpace);
				}
				else
				{
					//父级右侧框体板
					bool bParentBoard = GetRightBoardInParentSpace(RightBoardShape, CurCoverInfluencedSpaces);
					if (bParentBoard)
					{
						RightBoardHeight = RightBoardShape->GetShapeHeight();
					}
					else
					{
						CurCoverInfluencedSpaces.Add(this);
					}
				}
			}
		}
		else
		{
			CurCoverInfluencedSpaces.Add(this);
		}

		//四边同时内嵌时，电器饰条组认为是内嵌方式添加
		bNestedAdded = (UpCorverType == 3 && DownCorverType == 3 && LeftCorverType == 3 && RightCorverType == 3);
		//9、根据内部空间尺寸及掩盖方式等计算抽屉组的深度
		//饰条的厚度+门后缝
		const float InsertionBoardDepth = InEmbededElectricalGroup->GetInsertionBoardShape()->GetShapeDepth();
		//引起的内胆内缩值及计数
		

		//9、验证饰条尺寸是否有效
	
		//10、调整框体板件的外延值，实现外盖
		if (TopBoardShape)
		{
			CoverInfluencedBoards.Add(TopBoardShape);
		}
		if (BottomBoardShape)
		{
			CoverInfluencedBoards.Add(BottomBoardShape);
		}
		if (LeftBoardShape)
		{
			CoverInfluencedBoards.Add(LeftBoardShape);
		}
		if (RightBoardShape)
		{
			CoverInfluencedBoards.Add(RightBoardShape);
		}
		//11、设置受影响的内部空间的内缩值
		for (auto it : CurCoverInfluencedSpaces)
		{
			CoverInfluencedSpaces.Add(it);
			if (it != this)
			{
				it->GetAllBoardsInSpaceShape(CoverInfluencedBoards);
			}
		}
		ExecuteCoveredBoardInfluences(InsertionBoardDepth);
		ExecuteCoveredSpaceInfluences(InsertionBoardDepth);

		//13、根据框体板件厚度计算抽面组外延值
		if (UpCorverType == 1)
		{
			UpExtension = TopBoardHeight;
		}
		else if (UpCorverType == 2)
		{
			UpExtension = TopBoardHeight / 2.0;
		}
		if (DownCorverType == 1)
		{
			DownExtension = BottomBoardHeight;
		}
		else if (DownCorverType == 2)
		{
			DownExtension = BottomBoardHeight / 2.0;
		}
		if (LeftCorverType == 1)
		{
			LeftExtension = LeftBoardHeight;
		}
		else if (LeftCorverType == 2)
		{
			LeftExtension = LeftBoardHeight / 2.0;
		}
		if (RightCorverType == 1)
		{
			RightExtension = RightBoardHeight;
		}
		else if (RightCorverType == 2)
		{
			RightExtension = RightBoardHeight / 2.0;
		}		
		if (InEmbededElectricalGroup->GetInsertionBoardShape()->GetInserBoardPositionType() == FInsertionBoard::EInserBoardPositionType::E_Up)
		{
			InEmbededElectricalGroup->GetInsertionBoardShape()->SetUpExtension(UpExtension);
			InEmbededElectricalGroup->GetInsertionBoardShape()->SetDownExtension(0.0f);
		}
		else if(InEmbededElectricalGroup->GetInsertionBoardShape()->GetInserBoardPositionType() == FInsertionBoard::EInserBoardPositionType::E_Bottom)
		{
			InEmbededElectricalGroup->GetInsertionBoardShape()->SetUpExtension(0.0f);
			InEmbededElectricalGroup->GetInsertionBoardShape()->SetDownExtension(DownExtension);
		}
		InEmbededElectricalGroup->GetInsertionBoardShape()->SetLeftExtension(LeftExtension);
		InEmbededElectricalGroup->GetInsertionBoardShape()->SetRightExtension(RightExtension);
	}
		
	InEmbededElectricalGroup->SetShapePosX(InsideSpacePos.X);
	InEmbededElectricalGroup->SetShapePosY(InsideSpacePos.Y);
	InEmbededElectricalGroup->SetShapePosZ(InsideSpacePos.Z);
	InEmbededElectricalGroup->SetShapeWidth(InsideSpaceDim.X);
	InEmbededElectricalGroup->SetShapeHeight(InsideSpaceDim.Z);
	InEmbededElectricalGroup->SetShapeDepth(InsideSpaceDim.Y);
	const bool  HasLeftBoard = FrameShape->GetFrameBoardShape(2, 0).IsValid();
	const bool HasRightBoard = FrameShape->GetFrameBoardShape(3, 0).IsValid();
	if (HasLeftBoard)
	{
		InEmbededElectricalGroup->SetLeftRelativeBoardWidth(FrameShape->GetFrameBoardShape(2, 0)->GetShapeHeight());
		
	}
	if (HasRightBoard)
	{
		InEmbededElectricalGroup->SetRightRelativeBoardWidth(FrameShape->GetFrameBoardShape(3, 0)->GetShapeHeight());
	}			
	InEmbededElectricalGroup->UpdateEmbededEletricalShape();
	return true;
}

bool FSpaceShape::ScaleChildrenWidth(float InValue)
{
	if (SDT_XDirect != ChildrenSplitDirect)
	{
		return false;
	}
	if (ChildSpaceShapes.Num() == 0)
	{
		return false;
	}

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

	//判断输入分割数据是否有误
	if ((TotalFixed + TotalBoard > InValue) ||
		(-TotalScaled > KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed + TotalBoard - InValue) < KINDA_SMALL_NUMBER) ||
		(-TotalScaled < KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed + TotalBoard - InValue) > KINDA_SMALL_NUMBER))
	{
		return false;
	}

	//计算比例空间的单位尺寸
	float ScaleSize = 0.0;
	if (TotalScaled < 0.0)
	{
		ScaleSize = (InValue - TotalFixed - TotalBoard) / TotalScaled;
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

bool FSpaceShape::ScaleChildrenDepth(float InValue)
{
	if (SDT_YDirect != ChildrenSplitDirect)
	{
		return false;
	}
	if (ChildSpaceShapes.Num() == 0)
	{
		return false;
	}

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

	//判断输入分割数据是否有误
	if ((TotalFixed + TotalBoard > InValue) ||
		(-TotalScaled > KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed + TotalBoard - InValue) < KINDA_SMALL_NUMBER) ||
		(-TotalScaled < KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed + TotalBoard - InValue) > KINDA_SMALL_NUMBER))
	{
		return false;
	}

	//计算比例空间的单位尺寸
	float ScaleSize = 0.0;
	if (TotalScaled < 0.0)
	{
		ScaleSize = (InValue - TotalFixed - TotalBoard) / TotalScaled;
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

		ChildSpaceShapes[i]->SetShapeDepth(CurDepth);
		ChildSpaceShapes[i]->SetShapePosY(CurrentPosY);
		CurrentPosY += CurDepth;
	}

	return true;
}

bool FSpaceShape::ScaleChildrenHeight(float InValue)
{
	if (SDT_ZDirect != ChildrenSplitDirect)
	{
		return false;
	}
	if (ChildSpaceShapes.Num() == 0)
	{
		return false;
	}

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

	//判断输入分割数据是否有误
	if ((TotalFixed + TotalBoard > InValue) ||
		(-TotalScaled > KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed + TotalBoard - InValue) < KINDA_SMALL_NUMBER) ||
		(-TotalScaled < KINDA_SMALL_NUMBER && FMath::Abs(TotalFixed + TotalBoard - InValue) > KINDA_SMALL_NUMBER))
	{
		return false;
	}

	//计算比例空间的单位尺寸
	float ScaleSize = 0.0;
	if (TotalScaled < 0.0)
	{
		ScaleSize = (InValue - TotalFixed - TotalBoard) / TotalScaled;
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