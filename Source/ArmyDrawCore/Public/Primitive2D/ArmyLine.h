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
	
	/**@ŷʯ骻�ȡ��ק�ߵĲ�����*/
	TSharedPtr<class FArmyEditPoint> GetLineEditPoint();

	/**@ŷʯ������Ƿ���ʾ������*/
	void SetShowEditPoint(bool bShow) { ShowEditPoint = bShow; }

	/**@ŷʯ� ����start��end���Z����ֵ*/
	void SetZForStartAndEnd(float InZ);

	/**@ŷʯ� ���û���ʱ����ƫ��*/
	void SetDrawDataOffset(FVector InOffset);

	/**@ŷʯ� �����Ƿ����*/
	virtual void SetShouldDraw(bool bDraw);

	/**@ ŷʯ� �����߿�*/
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

	/**@ŷʯ� �����Ƿ����*/
	bool bShouldDraw = true;

	TSharedPtr<class FArmyEditPoint> StartPoint;
	TSharedPtr<class FArmyEditPoint> EndPoint;
	/**@ŷʯ� �϶�ֱ�ߵĲ�����*/
	TSharedPtr<class FArmyEditPoint> LineEditPoint;
	//bool bShowEditPoint = true;

	/**@ŷʯ� ��������ƫ����*/
	FVector DrawOffset = FVector::ZeroVector;
};