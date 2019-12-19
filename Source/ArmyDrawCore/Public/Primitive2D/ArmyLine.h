#pragma once
#include "ArmyEditPoint.h"
#include "ArmyPrimitive.h"

class ARMYDRAWCORE_API FArmyLine : public FArmyPrimitive
{
	friend class FArmyLine;
public:
	FArmyLine();
    FArmyLine(bool bInIsDashLine);
	FArmyLine(FArmyLine* Copy);
	FArmyLine(FVector InStart, FVector InEnd);
	FArmyLine(const TSharedPtr<FArmyEditPoint> InStart, const TSharedPtr<FArmyEditPoint> InEnd);
	~FArmyLine();

	virtual void SetStart(const FVector& InV);
	virtual void SetEnd(const FVector& InV);

	void SetStartPointer(const TSharedPtr<FArmyEditPoint> InV);
	void SetEndPointer(const TSharedPtr<FArmyEditPoint> InV);

	virtual FVector GetStart() const;
	virtual FVector GetEnd() const;

	float Size()const;

	const TSharedPtr<FArmyEditPoint> GetStartPointer() const;
	const TSharedPtr<FArmyEditPoint> GetEndPointer() const;

	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual bool IsSelected(const FBox2D& Box) override;
	virtual bool IsSelected(const FVector& Point, class UArmyEditorViewportClient* InViewportClient) override;
	virtual TSharedPtr<FArmyEditPoint> SelectEditPoint(FVector InPos, UArmyEditorViewportClient* InViewportClient) override;
	virtual void UpdateEditPoint()override;

	void SetLineColor(FLinearColor InColor);
	FLinearColor GetLineColor()const;	

	FVector GetDirectionNormal();

	void SetUseTrianlge();
	
	/**@欧石楠获取拖拽线的操作点*/
	TSharedPtr<class FArmyEditPoint> GetLineEditPoint();

	/**@欧石楠设置是否显示操作点*/
	void SetShowEditPoint(bool bShow) { ShowEditPoint = bShow; }

	/**@欧石楠 设置start和end点的Z轴数值*/
	void SetZForStartAndEnd(float InZ);

	/**@欧石楠 设置绘制时数据偏移*/
	void SetDrawDataOffset(FVector InOffset);

	/**@欧石楠 设置是否绘制*/
	virtual void SetShouldDraw(bool bDraw);

	/**@ 欧石楠 设置线宽*/
	void SetLineWidth(float Width) { LineWidth = Width; }
public:
	bool bIsDashLine;

	float Thickness;

	float DashSize;
protected:
	float LineWidth;
	
    FLinearColor LineColor;

    UMaterial*	Material;

	UMaterialInstanceDynamic* mDynamicMaterial;

	EOPState LineState;

	bool bUseTriangle = false;

	/**@欧石楠 设置是否绘制*/
	bool bShouldDraw = true;

	TSharedPtr<class FArmyEditPoint> StartPoint;
	TSharedPtr<class FArmyEditPoint> EndPoint;
	/**@欧石楠 拖动直线的操作点*/
	TSharedPtr<class FArmyEditPoint> LineEditPoint;
	//bool bShowEditPoint = true;

	/**@欧石楠 绘制数据偏移量*/
	FVector DrawOffset = FVector::ZeroVector;
};