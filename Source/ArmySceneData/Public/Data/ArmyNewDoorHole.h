/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRNewDoorHole.h
 * @Description 新建门洞
 *
 * @Author 欧石楠
 * @Date 2018年12月6日
 * @Version 1.0
 */
#pragma once

#include "ArmyHardware.h"

 /**
 * 新建门洞
 */
class ARMYSCENEDATA_API FArmyNewDoorHole : public FArmyHardware
{
public:
	FArmyNewDoorHole();
	FArmyNewDoorHole(FArmyNewDoorHole* Copy);
	virtual ~FArmyNewDoorHole();

	//~ Begin FArmyObject Interface
	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	virtual void SetState(EObjectState InState) override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual bool IsSelected(const FVector& Pos, UArmyEditorViewportClient* InViewportClient) override;
	virtual bool Hover(const FVector& Pos, UArmyEditorViewportClient* InViewportClient) override;
	virtual const FBox GetBounds() override;
	virtual void Destroy() override;
	//~ End FArmyObject Interface

	//~ Begin FArmyHardware Interface
	virtual void Update() override;
	//~ End FArmyHardware Interface
	virtual void Generate(UWorld* InWorld) override;

	/** bsp生成裁剪 */
	TArray<FVector> GetClipingBox();

	void UpdatePostWall();

	FORCEINLINE float GetThickness() { return Thickness; }
	FORCEINLINE float GetOuterWidth() { return OuterWidth; }
	FORCEINLINE float GetTotalWidth() { return Width + OuterWidth * 2.f; }

	void SetThickness(const float InThickness);
	void SetOuterWidth(const float InOuterWidth);

	const void GetAlonePoints(TArray< TSharedPtr<FArmyPoint> >& OutPoints) override;

	void SetIfGeneratePassModel(bool bGenerate);
	bool GetIfGeneratePassModel() { return bIfGenerateModel; }

public:
	TSharedPtr<class FArmyPolyline> StartPass;
	TSharedPtr<class FArmyPolyline> EndPass;
	TSharedPtr<class FArmyRect> Spacing;

	class AArmyExtrusionActor* ExtrusionActor;

	TSharedPtr<FArmyHardware> ConnectHardwareObj;

private:
	/** 垭口厚度 */
	float Thickness;

	/** 垭口外边宽 */
	float OuterWidth;

	/**@欧石楠 是否生成包边*/
	bool bIfGenerateModel = false;

	/**@欧石楠 顶视图下显示的额外数据*/
	TSharedPtr<class FArmyRect> TopViewRectImagePanel;
	TSharedPtr<class FArmyPolyline> TopViewStartPass;
	TSharedPtr<class FArmyPolyline> TopViewEndPass;
	TSharedPtr<class FArmyRect> TopViewSpacing;
};
REGISTERCLASS(FArmyNewDoorHole)