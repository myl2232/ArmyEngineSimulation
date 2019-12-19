/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRPunch.h
* @Description 开洞
*
* @Author 欧石楠
* @Date 2018-09-20
* @Version 1.0
*/
#pragma once

#include "ArmyHardware.h"

class ARMYSCENEDATA_API FArmyPunch :public FArmyHardware
{
public:
	
	FArmyPunch();
	FArmyPunch(FArmyPunch* Copy);
	virtual ~FArmyPunch();
	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	virtual void SetState(EObjectState InState) override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)override;
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual bool Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;	
	virtual const FBox GetBounds() override;
	//~ End FObject2D Interface
	/**	设置属性标识*/
	virtual void SetPropertyFlag(PropertyType InType, bool InUse) override;
	//~ Begin FArmyHardware Interface
	virtual void Update() override;
	//~ End FArmyHardware Interface
	virtual void Generate(UWorld* InWorld) override;
	/** 获取洞的包围盒 */
	TArray<FVector> GetBoundingBox();

	/** bsp生成裁剪 */
	TArray<FVector> GetFirstClipingBox();	

	TArray<FVector> GetSecondClipingBox();

	TArray<FVector> GetClipingBox();

	FVector2D GetXDir();
	
	FVector2D GetYDir();
	
	void SetPunchDepth(float InDepth);

	float GetPunchDepth()const { return PunchDepth; }

	virtual void SetHeightToFloor(float InDist) override;

	//float GetHeightToFloor()const { return HeightToFloor; }

	void CheckIfOutLimited();

	// 获得绘制CAD的线段列表
	void GetDrawCadLines(TArray<TPair<FVector, FVector>>& OutLineList);
public:
	/**@欧石楠 深度扩展出来的点，默认隐藏*/
	TSharedPtr<FArmyEditPoint> DepthPoint;

protected:
	virtual void HideRulerLineInputBox(bool bHide) override;

	virtual void OnFirstLineLInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction) override;
	
	virtual void OnFirstLineRInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction) override;
	
	virtual void OnSecondLineLInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)override;
	
	virtual void OnSecondLineRInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)override;

	TSharedPtr<class FArmyRulerLine> DepthRulerLine;
	void OnDepthInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

protected:		
	//float HeightToFloor;
	float PunchDepth;		
	bool bInsideInteralWall = true;
	FLinearColor DrawColor;
	TSharedPtr<class FArmyRect> PunchDepthRect;
};
REGISTERCLASS(FArmyPunch)
