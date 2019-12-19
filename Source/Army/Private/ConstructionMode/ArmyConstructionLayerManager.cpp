#include "ArmyConstructionLayerManager.h"
#include "ArmyEditorViewportClient.h"

FArmyClass::FArmyClass()
{

}
FArmyClass::~FArmyClass()
{

}
void FArmyClass::DrawHUD(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, const FSceneView* View, FCanvas* Canvas)
{
	if (!IsVisibility) return;
	for (auto Obj : InnerObjects)
	{
		Obj->DrawHUD(InViewPortClient,ViewPort,View,Canvas);
	}
}
void FArmyClass::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (!IsVisibility) return;
	for (auto Obj : InnerObjects)
	{
		Obj->Draw(PDI, View);
	}
}
void FArmyClass::AddObject(TSharedPtr<FArmyObject> InObj)
{
	InnerObjects.AddUnique(InObj);
	switch (InObj->GetType())
	{
	case OT_Dimensions:
	case OT_ObjectRuler:
	case OT_InSideWallDimensions:
	case OT_OutSideWallDimensions:
		ForceVisibleList.AddUnique(InObj.ToSharedRef());
		break;
	default:
		break;
	}
}
void FArmyClass::AppendObject(const TArray<TSharedPtr<FArmyObject>>& InObjs)
{
	InnerObjects.Append(InObjs);
	for (auto ObjIt : InObjs)
	{
		switch (ObjIt->GetType())
		{
		case OT_Dimensions:
		case OT_ObjectRuler:
			ForceVisibleList.AddUnique(ObjIt.ToSharedRef());
			break;
		default:
			break;
		}
	}
}
FObjectPtr FArmyClass::GetObjectByID(const FGuid& InObjID)
{
	for (auto& ObjIt : InnerObjects)
	{
		if (ObjIt->GetUniqueID() == InObjID)
		{
			return ObjIt;
		}
	}
	return NULL;
}
bool FArmyClass::RemoveObject(TSharedPtr<FArmyObject> InObj)
{
	ForceVisibleList.Remove(InObj.ToSharedRef());
	return InnerObjects.Remove(InObj) != 0;
}
void FArmyClass::ClearObjects()
{
	InnerObjects.Empty();
	ForceVisibleList.Empty();
}
FArmyLayer::FArmyLayer(bool IsStatic):bStaticLayer(IsStatic)
{

}
FArmyLayer::~FArmyLayer()
{

}
void FArmyLayer::DrawHUD(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, const FSceneView* View, FCanvas* Canvas)
{
	if (!IsVisibility) return;
	for (auto ClassIt : InnerClasses)
	{
		ClassIt.Value->DrawHUD(InViewPortClient, ViewPort,View,Canvas);
	}
}
void FArmyLayer::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (!IsVisibility) return;
	for (auto ClassIt : InnerClasses)
	{
		ClassIt.Value->Draw(PDI, View);
	}
}
TSharedPtr<FArmyClass> FArmyLayer::GetOrCreateClass(ObjectClassType InClassType,bool InCreate)
{
	TSharedPtr<FArmyClass> ObjClass = InnerClasses.FindRef(InClassType);

	if (!ObjClass.IsValid() && InCreate)
	{
		ObjClass = MakeShareable(new FArmyClass);
		ObjClass->SetVisible(false);
		InnerClasses.Add(InClassType, ObjClass);
	}
	return ObjClass;
}
bool FArmyLayer::RemoveClass(ObjectClassType InClassType)
{
	return InnerClasses.Remove(InClassType) != 0;
}
FObjectPtr FArmyLayer::GetObjectByID(const FGuid& InObjID)
{
	for (auto& ClassIt : InnerClasses)
	{
		FObjectPtr TempObj = ClassIt.Value->GetObjectByID(InObjID);
		if (TempObj.IsValid())
		{
			return TempObj;
		}
	}
	return NULL;
}
FArmyConstructionLayerManager::FArmyConstructionLayerManager()
{
}
FArmyConstructionLayerManager::~FArmyConstructionLayerManager()
{

}
void FArmyConstructionLayerManager::DrawHUD(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, const FSceneView* View, FCanvas* Canvas)
{
	if (CurrentLayer.IsValid())
	{
		CurrentLayer.Pin()->DrawHUD(InViewPortClient, ViewPort, View, Canvas);
	}
}
void FArmyConstructionLayerManager::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (CurrentLayer.IsValid())
	{
		CurrentLayer.Pin()->Draw(PDI, View);
	}
}
void FArmyConstructionLayerManager::ModifyLayer(const FName& InLayerName, const FName& InNewLayerName)
{
	TSharedPtr<FArmyLayer> layer = InnerLayers.FindRef(InLayerName);
	if (layer.IsValid())
	{
		layer->SetName(InNewLayerName);
		InnerLayers.Remove(InLayerName);
		InnerLayers.Add(InNewLayerName, layer);
	}
}
TWeakPtr<FArmyLayer> FArmyConstructionLayerManager::CreateLayer(const FName& InLayerName, bool IsStaticLayer)
{
	TSharedPtr<FArmyLayer> layer = InnerLayers.FindRef(InLayerName);
	if (!layer.IsValid())
	{
		layer = MakeShareable(new FArmyLayer(IsStaticLayer));
		layer->SetName(InLayerName);
		InnerLayers.Add(InLayerName, layer);
	}
	return layer;
}
TWeakPtr<FArmyLayer> FArmyConstructionLayerManager::GetLayer(const FName& InLayerName)
{
	return InnerLayers.FindRef(InLayerName);
}

/**
* @brief 根据层级名设置显示Layer
* @param 层级名
*/
void FArmyConstructionLayerManager::SetCurrentLayer(const FName& InLayerName)
{
	if (CurrentLayer.IsValid())
	{
		CurrentLayer.Pin()->SetForceVisible(false);
	}
	CurrentLayer = GetLayer(InLayerName);
	if (CurrentLayer == nullptr)
	{
		return;
	}
	CurrentLayer.Pin()->SetForceVisible(true);
}
TWeakPtr<FArmyLayer> FArmyConstructionLayerManager::GetDeaultLayer()
{
	if (InnerLayers.Num() > 0)
	{
		return InnerLayers.CreateIterator().Value().ToSharedRef();
	}
	return NULL;
}

TArray<TWeakPtr<FArmyLayer>> FArmyConstructionLayerManager::GetDynamicLayers()
{
	TArray<TWeakPtr<FArmyLayer>> DynamicLayers;
	for (auto& It : InnerLayers)
	{
		if (It.Value.IsValid())
		{
			if (!It.Value->IsStaticLayer())
			{
				DynamicLayers.Add(It.Value);
			}
		}
	}

	return DynamicLayers;
}

bool FArmyConstructionLayerManager::RemoveLayer(const FName& InLayerName)
{
	return InnerLayers.Remove(InLayerName) != 0;
}

void FArmyConstructionLayerManager::ClearLayer()
{
	for (auto &It : InnerLayers)
	{
		if (It.Value.IsValid())
		{
			for (auto &ClassIt : It.Value->GetAllClass())
			{
				if (ClassIt.Value.IsValid())
				{
					ClassIt.Value->ClearObjects();
				}
			}
		}
	}
}