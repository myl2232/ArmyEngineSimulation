#pragma once

#include "CoreMinimal.h"
#include "ArmyObject.h"
#include "ArmyTypes.h"

class ObjClassTypeName
{
public:
	ObjClassTypeName(ObjectClassType InType, FText InName) :Type(InType), Name(InName) {}

	ObjectClassType Type;
	FText Name;
};
class FArmyClass : public TSharedFromThis<FArmyClass>
{
public:
	FArmyClass();
	~FArmyClass();

	void DrawHUD(class UArmyEditorViewportClient* InViewPortClient, class FViewport* ViewPort, const class FSceneView* View, class FCanvas* Canvas);

	void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);
	void SetForceVisible(bool InV)
	{
		if (IsVisibility || !InV)
		{
			for (auto ObjIt : ForceVisibleList)
			{
				ObjIt->SetForceVisible(InV);
			}
		}
	}
	void SetVisible(bool InV) 
	{ 
		IsVisibility = InV;
		SetForceVisible(InV);
	};
	bool IsVisible() {return IsVisibility; };

	void SetName(FName& InName) { ClassName = InName; };
	const FName GetName() { return ClassName; };

	void AddObject(TSharedPtr<FArmyObject> InObj);

	void AppendObject(const TArray<TSharedPtr<FArmyObject>>& InObjs);

	bool RemoveObject(TSharedPtr<FArmyObject> InObj);

	FObjectPtr GetObjectByID(const FGuid& InObjID);

	const TArray<TSharedPtr<FArmyObject>>& GetAllObjects()const { return InnerObjects; }

	void ClearObjects();
private:
	ObjectClassType ClassType;

	bool IsVisibility;

	FName ClassName;

	TArray<TSharedPtr<FArmyObject>> InnerObjects;
	TArray<TSharedRef<FArmyObject>> ForceVisibleList;
};
class FArmyLayer : public TSharedFromThis<FArmyLayer>
{
public:
	enum FLayerType
	{
		LT_NONE = -1,
		LT_S_ORIGINAL = 1,
		LT_S_MODIFY = 2,
		LT_D_CUPBOARDTABLE = 20,//橱柜台面动态图层，前面的数留给静态图层以后的扩展用
		LT_D_WALLFACADE = 30, //墙体立面动态图层
		LT_D_WALLINDEXEDITOR = 40,//立面索引图的编辑状态（该类型图层和最终的立面索引图不是一个图层，因为户型数据一个是复制出来的一个是实时更新的）
	};

	FArmyLayer(bool IsStatic = true);
	~FArmyLayer();

	void DrawHUD(class UArmyEditorViewportClient* InViewPortClient, class FViewport* ViewPort, const class FSceneView* View, class FCanvas* Canvas);

	void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	void SetForceVisible(bool InV)
	{
		if (IsVisibility || !InV)
		{
			for (auto ClassIt : InnerClasses)
			{
				ClassIt.Value->SetForceVisible(InV);
			}
		}
	}
	void SetVisible(bool InV) 
	{ 
		IsVisibility = InV;
		SetForceVisible(InV);
	};

	void SetName(const FName& InName) { LayerName = InName; };
	const FName GetName() { return LayerName; };

	TSharedPtr<FArmyClass> GetOrCreateClass(ObjectClassType InClassType, bool InCreate = true);

	const TMap<ObjectClassType, TSharedPtr<FArmyClass>>& GetAllClass() { return InnerClasses; };

	bool RemoveClass(ObjectClassType InClassType);

	FObjectPtr GetObjectByID(const FGuid& InObjID);

	void SetFlag(int32 InFlag) { Flag = InFlag; }; 

	int32 GetFlag() { return Flag; }

	void SetLayerType(FLayerType InType) { LayerType = InType; };

	FLayerType GetLayerType() { return LayerType; }

	/* 是否为后台配置的静态图层*/
	bool IsStaticLayer() const { return bStaticLayer; }
private:
	int32 Flag = 0;//拆改墙绘制状态，用来区分户型拆改后的真实（拆改墙没有彩色及填充）
	FLayerType LayerType = LT_NONE;//用来区分原始勘探图 1 代表是原始图层 2 代表是拆改中的户型数据图层
	bool IsVisibility;
	bool bStaticLayer = true;//该图层时后台配置的图层则为true，为前端动态生成的图层则为false
	FName LayerName;
	TMap<ObjectClassType,TSharedPtr<FArmyClass>> InnerClasses;
};
class FArmyConstructionLayerManager : public TSharedFromThis<FArmyConstructionLayerManager>
{
public:
	FArmyConstructionLayerManager();
	~FArmyConstructionLayerManager();

	void DrawHUD(class UArmyEditorViewportClient* InViewPortClient, class FViewport* ViewPort, const class FSceneView* View, class FCanvas* Canvas);

	void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	void ModifyLayer(const FName& InLayerName, const FName& InNewLayerName);

	TWeakPtr<FArmyLayer> CreateLayer(const FName& InLayerName,bool IsStaticLayer = true);

	TWeakPtr<FArmyLayer> GetLayer(const FName& InLayerName);

	TWeakPtr<FArmyLayer> GetCurrentLayer() { return CurrentLayer; };

	TWeakPtr<FArmyLayer> GetDeaultLayer();

	void SetCurrentLayer(const FName& InLayerName);

	const TMap<FName, TSharedPtr<FArmyLayer>>& GetLayerMap() const { return InnerLayers; };

	TArray<TWeakPtr<FArmyLayer>> GetDynamicLayers();

	bool RemoveLayer(const FName& InLayerName);

	/* @ 梁晓菲 把每个layer下的Class清空*/
	void ClearLayer();

	void ClearAllLayer() 
	{
		CurrentLayer.Reset();
		InnerLayers.Empty(); 
	};
private:
	TWeakPtr<FArmyLayer> CurrentLayer;
	TMap<FName,TSharedPtr<FArmyLayer>> InnerLayers;
};

