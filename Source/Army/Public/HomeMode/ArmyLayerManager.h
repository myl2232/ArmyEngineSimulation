#pragma once
#include "CoreMinimal.h"
#include "ArmyObject.h"
#include "ArmyTypes.h"

class FArmyObjLayer
{
public:
	FArmyObjLayer(FName InName): Visible(true), LayerName(InName)
	{
		FPlatformMisc::CreateGuid(LayerID);
	}
	~FArmyObjLayer()
	{

	}

	FName GetName() { return LayerName; }
	void AddObject(TSharedPtr<FArmyObject> InObj);
	void RemoveObject(TSharedPtr<FArmyObject> InObj);
	//void DeleteObject(TSharedPtr<FArmyObject> InObj)
	//{
	//	Objects.Remove(InObj);
	//}
	void ClearObject();
	bool IsVisible()
	{
		return Visible;
	}
	void SetVisible(bool InVisible)
	{
		if (Visible == InVisible) return;
		
		for (auto ObjIt : Objects)
		{
			ObjIt->SetPropertyFlag(FArmyObject::FLAG_VISIBILITY, InVisible);
		}
		Visible = InVisible;
	}
	FObjectPtr SetVisible(const FGuid& InObjID,bool InVisible)
	{
		for (auto ObjIt : Objects)
		{
			if (ObjIt->GetUniqueID() == InObjID)
			{
				ObjIt->SetPropertyFlag(FArmyObject::FLAG_VISIBILITY, InVisible);
				return ObjIt;
			}
		}
		return NULL;
	}
	bool ContainObject(TSharedPtr<FArmyObject> InObj)
	{
		return Objects.Find(InObj) != INDEX_NONE;
	}
	const TArray<TSharedPtr<FArmyObject>>& GetObjects() const
	{
		return Objects;
	}
	TArray<TSharedPtr<FArmyObject>> GetObjects(EObjectType InObjType) const;
public:
	FGuid LayerID;
private:
	bool Visible;
	FName LayerName;
	TArray<TSharedPtr<FArmyObject>> Objects;
};

class FArmyLayerManager : public TSharedFromThis<FArmyLayerManager>
{
protected:
	FArmyLayerManager();
public:
	~FArmyLayerManager();

	void Destroy();

	static FArmyLayerManager* GetInstance();

	TSharedPtr<FArmyObjLayer> GetLayer(const FGuid& InLayerID)
	{
		TSharedPtr<FArmyObjLayer> ObjLayer = ObjLayers.FindRef(InLayerID);
		return ObjLayer;
	}

	TSharedPtr<FArmyObjLayer> CreateLayer(FName InName)
	{
		TSharedPtr<FArmyObjLayer> TempLayer = MakeShareable(new FArmyObjLayer(InName));
		ObjLayers.Add(TempLayer->LayerID, TempLayer);
		
		if (!InName.IsNone())
		{
			AddOrRemoveLayer.ExecuteIfBound(ItemInfo(TempLayer->LayerID, InName, TempLayer->LayerID), 1);
		}
		return TempLayer;
	}
	TSharedPtr<FArmyObjLayer> GetObjectLayer(TSharedPtr<FArmyObject> InObj)
	{
		for (auto LayerIt : ObjLayers)
		{
			if (LayerIt.Value->ContainObject(InObj))
			{
				return LayerIt.Value;
			}
		}
		return NULL;
	}
	void RemoveLayer(const FGuid& InID)
	{
		ItemInfo TempInfo(InID,FName(), InID);
		TSharedPtr<FArmyObjLayer> Layer = ObjLayers.FindRef(InID);
		if (Layer.IsValid())
		{
			TempInfo.ItemName = Layer->GetName();
			ObjLayers.Remove(InID);
		}
		AddOrRemoveLayer.ExecuteIfBound(TempInfo,0);
	}
	void RemoveFromLayer(TSharedPtr<FArmyObject> InObj)
	{
		for (auto layer : ObjLayers)
		{
			layer.Value->RemoveObject(InObj);
		}
	}
	void ClearAllLayer()
	{
		ClearAllObject();
		ObjLayers.Empty();
	}
	void ClearAllObject()
	{
		for (auto layer : ObjLayers)
		{
			layer.Value->ClearObject();
		}
		ObjLayers.Empty();
	}

	FName ObjectLayerName(EObjectType InObjectType);
public:
	SItemOperator AddOrRemoveLayer;
	SItemOperator AddOrRemoveObj;
private:
	TMap<FGuid, TSharedPtr<FArmyObjLayer>> ObjLayers;
	static TSharedPtr<FArmyLayerManager> Instance;
};

