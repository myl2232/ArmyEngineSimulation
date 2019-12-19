#include "ArmyLayerManager.h"

void FArmyObjLayer::AddObject(TSharedPtr<FArmyObject> InObj)
{
	Objects.AddUnique(InObj);
	FArmyLayerManager::GetInstance()->AddOrRemoveObj.ExecuteIfBound(ItemInfo(InObj->GetUniqueID(), FName(*(InObj->GetName())),LayerID),1);
}
void FArmyObjLayer::RemoveObject(TSharedPtr<FArmyObject> InObj)
{
	Objects.Remove(InObj);
	FArmyLayerManager::GetInstance()->AddOrRemoveObj.ExecuteIfBound(ItemInfo(InObj->GetUniqueID(), FName(*(InObj->GetName())), LayerID), 0);
}
void FArmyObjLayer::ClearObject()
{
	Objects.Empty();
	FArmyLayerManager::GetInstance()->AddOrRemoveObj.ExecuteIfBound(ItemInfo(LayerID,LayerName, LayerID),-1);
}

TArray<TSharedPtr<FArmyObject>> FArmyObjLayer::GetObjects(EObjectType InObjType) const
{
	TArray<TSharedPtr<FArmyObject>> ResultObjects;
	for (auto It : Objects)
	{
		if (It->GetType() == InObjType)
			ResultObjects.Push(It);
	}
	return ResultObjects;
}

TSharedPtr<FArmyLayerManager> FArmyLayerManager::Instance = NULL;
FArmyLayerManager::FArmyLayerManager()
{
}
void FArmyLayerManager::Destroy()
{
	Instance.Reset();
}
FArmyLayerManager* FArmyLayerManager::GetInstance()
{
	if (!Instance.IsValid())
	{
		Instance = MakeShareable(new FArmyLayerManager);
	}
	return Instance.Get();
}
FName FArmyLayerManager::ObjectLayerName(EObjectType InObjectType)
{
	FName LayerName(TEXT("其他"));
	switch (InObjectType)
	{
	case OT_Wall:
	case OT_ArcWall:
	case OT_BaseWall:
	case OT_ModifyWall:
	case OT_AddWall:
	case OT_BaseBoard:
	case OT_CrossDoorStone:
	case OT_Pass:
		LayerName = FName(TEXT("基础数据"));
		break;
	case OT_InternalRoom:
	case OT_OutRoom:
		LayerName = FName(TEXT("空间"));
		break;
	case OT_ArcWindow:
	case OT_Door:
	case OT_SlidingDoor:
	case OT_Window:
	case OT_FloorWindow:
	case OT_RectBayWindow:
	case OT_TrapeBayWindow:
	case OT_CornerBayWindow:
		LayerName = FName(TEXT("门窗"));
		break;
	case OT_ComponentBase:
		LayerName = FName(TEXT("构件"));
		break;
	default:
		break;
	}
	return LayerName;
}
FArmyLayerManager::~FArmyLayerManager()
{
}
