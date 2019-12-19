/**
 * Copyright 2019 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRNewPass.h
 * @Description 开垭口
 *
 * @Author 欧石楠
 * @Date 2019年1月9日
 * @Version 1.0
 */
#pragma once
#include "ArmyHardware.h"
#include "FArmyConstructionItemInterface.h"
#include "ArmyPolyline.h"
#include "IArmyReplace.h"
#include "ArmyCommonTypes.h"
#include "IArmyConstructionHelper.h"
 /**
 * 开垭口
 */
class ARMYSCENEDATA_API FArmyNewPass : public FArmyHardware, public IArmyReplace, public IArmyConstructionHelper
{
public:
	FArmyNewPass();
	FArmyNewPass(FArmyNewPass* Copy);
	virtual ~FArmyNewPass();

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

	/**	设置属性标识*/
	virtual void SetPropertyFlag(PropertyType InType, bool InUse) override;

	//~ Begin FArmyHardware Interface
	virtual void Update() override;
	//~ End FArmyHardware Interface
	virtual void Generate(UWorld* InWorld) override;

	virtual void ReplaceGoods(FContentItemPtr InContentItem, UWorld* InWorld) override;

	/** 生成立面投影 */
	TArray<struct FLinesInfo> GetFacadeBox();

	/** bsp生成裁剪 */
	TArray<FVector> GetClipingBox();

	FORCEINLINE float GetThickness() { return Thickness; }
	FORCEINLINE float GetOuterWidth() { return OuterWidth; }
	FORCEINLINE float GetTotalWidth() { return Width + OuterWidth * 2.f; }

	void SetThickness(const float InThickness);
	void SetOuterWidth(const float InOuterWidth);

	const void GetAlonePoints(TArray< TSharedPtr<FArmyPoint> >& OutPoints) override;

	void SetIfGeneratePassModel(bool bGenerate);
	bool GetIfGeneratePassModel() { return bIfGenerateModel; }		

	void SetModifyDoorHoleMatType(FText InStr);
	FText GetModifyDoorHoleMatType() { return ModifyDoorHoleMatType; }
	
	void ReqModifyMaterialSaleID();
	void ResModifyMaterialSaleID(TSharedPtr<FJsonObject> Data);

	/**@欧石楠 计算周长和面积*/	
	void GetModifyWallAreaAndPerimeter(float& OutArea, float& OutPerimeter);

	void GetOriginPassArea(float &OutArea);

	/**@欧石楠 获取材质商品ID*/
	int GetModifyMaterialSaleID() { return ModifyMaterialSaleID; }	

	/**@欧石楠 获取选中的材质ID*/
	int GetSelectedModifyMaterialID() { return SelectedModifyMaterialID; }		

	int32 GetSaleID() const { return SaleID; }
    FString GetThumbnailUrl() const { return ThumbnailUrl; }

    void SetThumbnailUrl(FString InThumbnailUrl) { ThumbnailUrl = InThumbnailUrl; }

	// 常远获得垭口的填充矩形
	TSharedPtr<class FArmyRect> GetModifyRect() { return ModifyRect; }
	// 获取左垭口套
	TSharedPtr<FArmyPolyline>	GetStartPass() { return StartPass; }
	//获取右垭口套
	TSharedPtr<FArmyPolyline> GetEndPass() { return EndPass; }

	/**@欧石楠 获取选中的材质ID*/
	int GetSelectedMaterialID() { return SelectedMaterialID; }
	void SetMatType(FString InType);
	FString GetMatType() { return MatType; }
public:
	/**@欧石楠 施工项相关数据*/
	TSharedPtr<FArmyConstructionItemInterface> ConstructionItemData;

	TSharedPtr<class FArmyPolyline> StartPass;
	TSharedPtr<class FArmyPolyline> EndPass;
	TSharedPtr<class FArmyRect> Spacing;

	class AArmyExtrusionActor* ExtrusionActor;		
	
	bool bModifyType = false;

	FContentItemPtr PassContentItem;//商品信息（默认是空）

	//@郭子阳
	//有没有赋予商品
	bool HasGoods() { return SaleID > 0; };
protected:
	FString MatType = TEXT("轻体砖");
	int SelectedMaterialID = 2;//轻体砖

private:
	/** 垭口厚度 */
	float Thickness;

	/** 垭口外边宽 */
	float OuterWidth;

	/**@欧石楠 是否生成包边*/
	bool bIfGenerateModel = true;	

	/**@欧石楠 顶视图下显示的额外数据*/
	TSharedPtr<class FArmyRect> TopViewRectImagePanel;
	TSharedPtr<class FArmyPolyline> TopViewStartPass;
	TSharedPtr<class FArmyPolyline> TopViewEndPass;
	TSharedPtr<class FArmyRect> TopViewSpacing;
	
	UMaterialInstanceDynamic* MI_ModifyWall;	
	
	/** 新开垭口表示拆改的填充*/
	TSharedPtr<class FArmyRect> ModifyRect;

	FText ModifyDoorHoleMatType = FText::FromString(TEXT("轻体砖"));
	
	int ModifyMaterialSaleID;
	int SelectedModifyMaterialID = 2;//轻体砖

	/** 垭口的商品ID*/
	int32 SaleID = -1;

    /** 缩略图url */
    FString ThumbnailUrl;

	//@郭子阳
	//*********3D模式施工项相关**********
public:
	TSharedPtr<struct ConstructionPatameters> GetConstructionParameter();
	//获取施工项查询Key
	TSharedPtr<struct ObjectConstructionKey> GetConstructionKey();
	//输出窗台石施工项
	void ConstructionData(TArray<struct FArmyGoods>& ArtificialData);
	//获取房间
	TSharedPtr<FArmyRoom> GetRoom() override;

	void OnRoomSpaceIDChanged(int32 NewSpaceID);
	 
};
REGISTERCLASS(FArmyNewPass)
