#pragma once

#include "SceneManagement.h"

/**
 * 图元基础类
 */
class ARMYDRAWCORE_API FArmyPrimitive : public TSharedFromThis<FArmyPrimitive>
{
public:
	enum EOPState
	{
		OPS_Default,
		OPS_Normal,
		OPS_Hovered,
		OPS_Selected,
		OPS_Disable
	};

	FArmyPrimitive();
	virtual	~FArmyPrimitive();

	/** 是否被点选 */
	virtual bool IsSelected(const FVector& pos,class UArmyEditorViewportClient* InViewportClient) = 0;

    /** 是否被框选 */
	virtual bool IsSelected(const FBox2D& Box) = 0;
	
    /** 图元绘制方法 */
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	/**	更新编辑点*/
	virtual void UpdateEditPoint() {};

	/**	获取编辑点索引*/
	virtual int32 GetEditPointIndex(TSharedPtr<class FArmyEditPoint> InEditPoint);

	virtual TSharedPtr<class FArmyEditPoint> SelectEditPoint(FVector InPos, UArmyEditorViewportClient* InViewportClient);

	virtual void Transform(const FVector& PostAdd){}
	virtual void Rotate(const FRotator& Rotation){}
	virtual void Scale(const FVector& Scale){}
	virtual void SetBasePoint(const FVector& InBasePoint){}

	virtual void SetState(EOPState InState);
	virtual EOPState GetState() const { return PriState; };

	void SetBaseColor(const FLinearColor& InColor);
	/**@欧石楠 可修改hover color的接口*/
	void SetHoverColor(const FLinearColor& InColor);
	/**@欧石楠 可修改NormalColor的接口*/
	void SetNormalColor(const FLinearColor& InColor);

	const FLinearColor& GetBaseColor() const;
	const FLinearColor& GetHoverColor() const;
	const FLinearColor& GetNormalColor() const;

public:
	EOPState PriState;

	uint8 DepthPriority;

	float DepthBias;

	bool ShowEditPoint;

	TMap<int32, TSharedPtr<class FArmyEditPoint>> EditPointMap;

protected:
	FLinearColor BaseColor;
	/**@欧石楠 hover color用于当被hover或者selected时变化的颜色*/
	FLinearColor HoverColor;
	/**@欧石楠 normal color用于当hover或者selected结束后重置到normal的颜色，*/
	FLinearColor NormalColor;

	FLinearColor DefaultColor;
};

