#include "SCTShapeManager.h"
#include "SCTStaticMeshManager.h"
#include "HttpMgr.h"
#include "JsonObject.h"
#include "DelegateSignatureImpl.inl"
#include "SCTShape.h"
#include "SCTBoardShape.h"
#include "SCTModelShape.h"
#include "SCTBaseShape.h"
#include "SCTInsideShape.h"
#include "SCTUnitShape.h"
#include "SCTDoorGroup.h"
#include "SCTCabinetShape.h"
#include "SCTSpaceShape.h"
#include "SCTFrameShape.h"
#include "SCTDoorSheet.h"
#include "SCTSlidingDrawerShape.h"
#include "SCTEmbededElectricalGroup.h"
#include "SCTVeneeredSideBoardGroup.h"
#include "GameFramework/Actor.h"

const TSharedRef<FSCTShapeManager>& FSCTShapeManager::Get()
{
	static const TSharedRef<FSCTShapeManager> Instance = MakeShareable(new FSCTShapeManager);
	return Instance;
}

FSCTShapeManager::FSCTShapeManager()
{	
	StaticMeshManager = MakeShareable(new FMeshManager);
	ChildrenShapeLists.SetNum(EShapeType::ST_Max);
}

TSharedPtr<FSCTShape> FSCTShapeManager::CopyShapeToNew(const TSharedPtr<FSCTShape>& InShape)
{
	TSharedPtr<FSCTShape> NewShape = nullptr;
	int32 ShapeType = InShape->GetShapeType();
	switch (ShapeType)
	{
		case ST_Board://板件
		{
			NewShape = MakeShareable(new FBoardShape());
			break;
		}
		case ST_Decoration://饰品
		{
			NewShape = MakeShareable(new FDecorationShape());
		}
		case ST_Accessory://五金
		{
			NewShape = MakeShareable(new FAccessoryShape());
			break;
		}
		case ST_Base: //基础组件
		{
			NewShape = MakeShareable(new FBaseShape());
			break;
		}
		case ST_Inside: //内胆
		{
			NewShape = MakeShareable(new FInsideShape());
			break;
		}
		case ST_Unit: //单元柜
		{
			NewShape = MakeShareable(new FUnitShape());
			break;
		}
		case ST_Space: // 空间
		{
			NewShape = MakeShareable(new FSpaceShape());
			break;
		}
		case ST_Frame: // 轮廓
		{
			NewShape = MakeShareable(new FFrameShape());
			break;
		}
		case ST_SlidingDoor: // 移门
		{
			NewShape = MakeShareable(new FSlidingDoor());
			break;
		}
		case ST_SideHungDoor: // 掩门
		{
			NewShape = MakeShareable(new FSideHungDoor());
			break;
		}
		case ST_DrawerDoor: // 抽面 
		{
			NewShape = MakeShareable(new FDrawerDoorShape());
			break;
		}
		case ST_SpaceDividDoorSheet: // 空间划分式门板
		{
			NewShape = MakeShareable(new FSpaceDividDoorSheet());
			break;
		}
		case ST_VariableAreaDoorSheet://  可变区域划分式门板
		{
			NewShape = MakeShareable(new FVariableAreaDoorSheet());
			break;
		}
		case ST_VariableAreaDoorSheet_ForDrawerDoor:
		{
			NewShape = MakeShareable(new FDrawerDoorShape::FVariableAreaDoorSheetForDrawer());
			break;
		}
		case ST_VariableAreaDoorSheet_ForSideHungDoor:
		{
			NewShape = MakeShareable(new FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor());
			break;
		}
		case ST_Cabinet:
		{
			NewShape = MakeShareable(new FCabinetShape());
			break;
		}
		case ST_SlidingDrawer:
		{
			NewShape = MakeShareable(new FSlidingDrawerShape());
			break;
		}
		case ST_DrawerGroup:
		{
			NewShape = MakeShareable(new FDrawerGroupShape());
			break;
		}
		case ST_InsertionBoard:
		{
			NewShape = MakeShareable(new FInsertionBoard());
			break;
		}
		case ST_EmbeddedElectricalGroup:
		{
			NewShape = MakeShareable(new FEmbededElectricalGroup());
			break;
		}
		case ST_NoneModelVeneeredBoard:
		{
			NewShape = MakeShareable(new FNoneModelVeneeredSudeBoard);
			break;
		}
		case ST_VeneerdBoardGroup:
		{
			NewShape = MakeShareable(new FVeneerdSudeBoardGroup);
			break;
		}
		default:
			check(false);
			break;
	}
	InShape->CopyTo(NewShape.Get());
	return NewShape;
}

TSharedPtr<FSCTShape> FSCTShapeManager::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//根据型录类型创建新的型录
	TSharedPtr<FSCTShape> NewShape = nullptr;
	
	//型录类型
	int32 ShapeType = InJsonObject->GetNumberField(TEXT("type"));
	switch (ShapeType)
	{
		case ST_Board:
		{
			NewShape = MakeShareable(new FBoardShape());
			break;
		}
		case ST_Decoration:
		{
			NewShape = MakeShareable(new FDecorationShape());
			break;
		}
		case ST_Accessory:
		{
			NewShape = MakeShareable(new FAccessoryShape());
			break;
		}
		case ST_Base:
		{
			NewShape = MakeShareable(new FBaseShape());
			break;
		}
		case ST_Inside:
		{
			NewShape = MakeShareable(new FInsideShape());
			break;
		}
		case ST_Unit:
		{
			NewShape = MakeShareable(new FUnitShape());
			break;
		}
		case ST_Space:
		{
			NewShape = MakeShareable(new FSpaceShape());
			break;
		}
		case ST_Frame:
		{
			NewShape = MakeShareable(new FFrameShape());
			break;
		}
		case ST_SlidingDoor:
		{
			NewShape = MakeShareable(new FSlidingDoor());
			break;
		}
		case ST_SideHungDoor:
		{
			NewShape = MakeShareable(new FSideHungDoor());
			break;
		}
		case ST_DrawerDoor:
		{
			NewShape = MakeShareable(new FDrawerDoorShape());
			break;
		}
		case ST_SpaceDividDoorSheet:
		{
			NewShape = MakeShareable(new FSpaceDividDoorSheet());
			break;
		}
		case ST_VariableAreaDoorSheet:
		{
			NewShape = MakeShareable(new FVariableAreaDoorSheet());
			break;
		}
		case ST_VariableAreaDoorSheet_ForDrawerDoor:
		{
			NewShape = MakeShareable(new FDrawerDoorShape::FVariableAreaDoorSheetForDrawer());
			break;
		}
		case ST_VariableAreaDoorSheet_ForSideHungDoor:
		{
			NewShape = MakeShareable(new FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor());
			break;
		}
		case ST_SlidingDrawer:
		{
			NewShape = MakeShareable(new FSlidingDrawerShape());
			break;
		}
		case ST_DrawerGroup:
		{
			NewShape = MakeShareable(new FDrawerGroupShape());
			break;
		}
		case ST_Cabinet:
		{
			NewShape = MakeShareable(new FCabinetShape());
			break;
		}
		case ST_InsertionBoard:
		{
			NewShape = MakeShareable(new FInsertionBoard());
			break;
		}
		case ST_EmbeddedElectricalGroup:
		{
			NewShape = MakeShareable(new FEmbededElectricalGroup());
			break;
		}
		case ST_NoneModelVeneeredBoard:
		{
			NewShape = MakeShareable(new FNoneModelVeneeredSudeBoard);
			break;
		}
		case ST_VeneerdBoardGroup:
		{
			NewShape = MakeShareable(new FVeneerdSudeBoardGroup);
			break;
		}
		default:
			check(false);
			break;
	}
	
	//解析Json对象
	NewShape->ParseShapeFromJson(InJsonObject);

	return NewShape;
}

FString FSCTShapeManager::SaveShapeToJson(FSCTShape* InShape)
{
	//当前顶层型录保存为Json字符串
	FString JsonStr;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonStr);
	
	JsonWriter->WriteObjectStart();
	InShape->SaveToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();
	
	JsonWriter->Close();

	return JsonStr;
}

void FSCTShapeManager::ParseChildrenShapes(const TSharedPtr<FJsonObject>& InJsonObject)
{
	const TArray<TSharedPtr<FJsonValue>>* ShapeList = nullptr;	
	InJsonObject->TryGetArrayField(TEXT("shapes"), ShapeList);
	if (ShapeList != nullptr && (*ShapeList).Num() > 0)
	{
		for (auto& It : *ShapeList)
		{
			TSharedPtr<FJsonObject> ShapeObject = It->AsObject();
			TSharedPtr<FSCTShape> NewShape = FSCTShapeManager::Get()->ParseShapeFromJson(ShapeObject);
			int32 ShapeType = NewShape->GetShapeType();
			int64 ShapeId = NewShape->GetShapeId();
			ChildrenShapeLists[ShapeType].Add(ShapeId, NewShape);
		}
	}
}

void FSCTShapeManager::AddChildShape(TSharedPtr<FSCTShape> InShape)
{
	const int32 ShapeType = InShape->GetShapeType();
	const int64 ShapeID = InShape->GetShapeId();
	ChildrenShapeLists[ShapeType].Add(ShapeID, InShape);
}

void FSCTShapeManager::CountInheritChildShapes(FSCTShape* InShape)
{
	const TArray<TSharedPtr<FSCTShape>>& ChildShapes = InShape->GetChildrenShapes();
	for (int32 i=0; i<ChildShapes.Num(); ++i)
	{
		AddChildShape(ChildShapes[i]);
		CountInheritChildShapes(ChildShapes[i].Get());
	}
}

TMap<int64, TSharedPtr<FSCTShape>>& FSCTShapeManager::GetChildrenShapesByType(int32 InType)
{
	return ChildrenShapeLists[InType];
}

TSharedPtr<FSCTShape> FSCTShapeManager::GetChildShapeByTypeAndID(int32 InType, int64 InID)
{
	auto it = ChildrenShapeLists[InType].Find(InID);
	return *it;
}

bool FSCTShapeManager::FindChildShapeByTypeAndID(int32 InType, int64 InID)
{
	auto it = ChildrenShapeLists[InType].Find(InID);
	return it != nullptr;
}

void FSCTShapeManager::ClearAllChildrenShapes()
{
	for (auto& it : ChildrenShapeLists)
	{
		it.Empty();
	}
}


TSharedPtr<FMeshManager> FSCTShapeManager::GetStaticMeshManager()
{
	return StaticMeshManager;
}

