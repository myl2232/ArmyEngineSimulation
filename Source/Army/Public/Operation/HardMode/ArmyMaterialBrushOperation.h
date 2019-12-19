/**
* Copyright 2019 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRMaterialBrushOperation.h
* @Description 材质刷
*
* @Author 郭子阳
* @Date 2019年4月23日
* @Version 1.0
*/
#pragma once
#include "ArmyOperation.h"
#include "ArmyCommonTypes.h"
class FArmyMaterialBrushOperation :public FArmyOperation
{
public:
	FArmyMaterialBrushOperation(EModelType InBelongModel);
	virtual void BeginOperation(XRArgument InArg = XRArgument()) override;
	virtual void EndOperation() override;

	bool IsActive = false;

	//将引用计数器设为友元
	friend class SharedPointerInternals::TIntrusiveReferenceController<FArmyMaterialBrushOperation>;

	/** 鼠标单击事件，所有的鼠标左键/右键/中键单击事件都在该方法中实现 */
	virtual void ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;

	//鼠标单击事件，用于立面子模式
	virtual void ProcessClickInViewPortClient(FViewport* InViewPort, FKey Key, EInputEvent Event) override;
	

	//是否在切换模式时进入该操作，用于立面和子模式之间的切换
	void SetRebeginAtStart()
	{
		bRebeginAtStart = true;
	};

	bool GetRebeginAtStart()
	{
		return bRebeginAtStart;
	};

private:
	static TSharedPtr<SImage> CursorWidget;

	//下次进入执行进入该operation时是否使用上次吸取的材质
	static bool bRebeginAtStart ;

	//吸取的材质商品
	 TSharedPtr<FContentItemSpace::FContentItem> AbsorbededContentItem;
	//吸取的材质样式
	TSharedPtr<class FArmyBaseEditStyle> MatData;

	//刷子的状态
	enum class BrushState
	{
		Idle, //闲置状态
		Absorb, //吸取
		Place, //放置
	};

	//当前的刷子状态
	BrushState CurrentBrushState= BrushState::Idle;

	void  SetBrushState(BrushState NewState);

	//在使用材质刷之前是否可用编辑按钮
	bool FormerCanUseEditMenu = false;
	bool IsFormerCanUseEditMenuSet = false;

	 TSharedPtr<FArmyBaseArea> Source = nullptr; //被吸取材质的面
	 TSharedPtr<FArmyBaseArea> Destination = nullptr; //放置材质的面

     //被鼠标选中的面，可以和Source或Destination相同
	 TSharedPtr<FArmyBaseArea> SelectedArea = nullptr;

	 //被选中的面是否已经利用了
	 //bool SelectedAreaConsumed = true;
};
