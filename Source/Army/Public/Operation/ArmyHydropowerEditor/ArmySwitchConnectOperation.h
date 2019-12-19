#pragma once

#include "ArmyOperation.h"
#include "ArmyLine.h"
#include "SArmyInputBox.h"
#include "ArmyEditPoint.h"
#include "ArmyPipeline.h"
#include "ArmyResourceService.h"
#include "ArmyHydropowerSwitchConnect.h"

class FArmyHydropowerSwitchConnect;
class SArmySpaceTreeItem;
/**
* 开关控制图助手
*/
class FArmySwitchConnectOperation : public FArmyOperation
{
public:
	struct TreeItemInfo : TSharedFromThis<TreeItemInfo>
	{
		FObjectWeakPtr XRObjectItem;
		AActor* ActorItem;
	};
	/**
	* SwitchShowType
	*/
	enum SST
	{
	  	SST_None,
		SST_ENTER = 1,
		SST_ADD = 2,
		SST_EXIT_ADD = 3,
		SST_EXIT = 4,
		SST_Max
	};
public:
	FArmySwitchConnectOperation(EModelType InBelongModel);
	virtual ~FArmySwitchConnectOperation ();

	//~ Begin IArmyDrawHelper Interface
	virtual void Init();
	virtual void InitWidget(TSharedPtr<SOverlay> InParentWidget /* = NULL */);
	virtual void Draw (FPrimitiveDrawInterface* PDI,const FSceneView* View) override;
	virtual void BeginOperation (XRArgument InArg = XRArgument ()) override;
	virtual void EndOperation () override;
	virtual void Tick () override;
	virtual bool CanExit() override; //能否退出
    virtual void ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;
	virtual void MouseMove (UArmyEditorViewportClient* InViewPortClient,FViewport* ViewPort,int32 X,int32 Y) override;
	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient,class FViewport* InViewPort,FKey Key,EInputEvent Event) override;
	//~ End IArmyDrawHelper Interface
	/**
	* 刷新灯具
	*/
	void RefreshFurniture(TWeakPtr<FArmyObject>& InObject,TSharedPtr<SArmySpaceTreeItem>& LayerItem);
	/**
	* 刷新灯带
	*/
	void RefreshExtrusion(const FGuid& LightID, TSharedPtr<SArmySpaceTreeItem>& LayerItem);
	//void RefreshExtrusion(TWeakPtr<FArmyObject>& InObject,TSharedPtr<SArmySpaceTreeItem>& LayerItem);

	void SetDetailView(TSharedPtr<FArmyHydropowerSwitchConnect> InDetailView );//{ DetailView = InDetailView; }

	//启动添加灯具，或者添加完毕
	void CheckState(ECheckBoxState InCheck,SArmySpaceTreeItemPtr CurrentItem);

	//删除灯具
	void DeleteItem(SArmySpaceTreeItemPtr CurrentItem);

	//选择灯具
	void OnUIClicked(SArmySpaceTreeItemPtr Item);;
private:
	/**
	* 获取所有房间
	*/
	void InitRoom();
	/**
	* 获取所有房间内的开关
	*/
	void InitFurniture();

	/**
	* 设置是否可以点击 显示
	*/
	void UpdateCanClick(SST InShowType);
public:
	FBoolDelegate OnBenginOrEndOperationDelegate;
private:
	TArray<FObjectWeakPtr> RoomObjectes;
	TMultiMap<FGuid,TSharedPtr<TreeItemInfo> > RoomFurnitureItemMap;
	TMap<FGuid,TArray<AActor*>> RoomLampActorMap;
	SArmySpaceTreeItemPtr CurrentItem;
	TSharedPtr<FArmyHydropowerSwitchConnect> DetailView;

public:
	//是否在右侧UI中单击  郭子阳 1.14
	bool IsUIOperating;
	// 郭子阳 1.15
	//是否显示Actor坐标轴
	void SetCoordAxisShow(bool show);

	void OnSelectedSomething(UObject * obj);

private :
	//是否在为开关添加灯具
	bool IsAddingLight;

public:
	void	OpenSelectFilter();
	void	CloseSelectFilter();

	bool CanSelect(AActor * actor);

};