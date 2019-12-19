#pragma once

#include "ArmyObject.h"
#include "ArmyEditPoint.h"
#include "ArmyHardware.h"
#include "IArmyConstructionHelper.h"
#define OutPillarWidth 6
#define OutPillarThiness 6
#define MiddlePillarThiness 4
#define MiddlePillarHeight 10
#define BottomOrTopThickness 5

#define  MiddlePillarRectLength 4
#define MiddleHorizontalWidth 5

class AXRBrush;
class FArmyRect;
class AStaticMeshActor;
/**
 * 窗户
 */
class ARMYSCENEDATA_API FArmyWindow : public FArmyHardware, public IArmyConstructionHelper
{
public:
	FArmyWindow();
	FArmyWindow(FArmyWindow* Copy);
	virtual ~FArmyWindow();

	//~ Begin FArmyObject Interface
	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	virtual void SetState(EObjectState InState) override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual bool Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual const FBox GetBounds() override;
	virtual void GetCadLineArray(TArray<TPair<FVector, FVector>>& OutLineList) const override;
    virtual void Destroy() override;
	//~ End FArmyObject Interface

	//~ Begin FArmyHardware Interface
	virtual void Update() override;
	//~ End FArmyHardware Interface
	virtual void Generate(UWorld* InWorld) override;

	//float GetHeight() { return Height; }
	//float GetWindowLength() { return Length; }

	//float GetHeightToFloor() { return HeightToFloor; }
	virtual void SetHeightToFloor(float InValue) override;

	TArray<FVector> GetBoundingBox();

	/** @纪仁泽 获得立面投影 */
	TArray<struct FLinesInfo> GetFacadeBox();

	/** bsp生成裁剪 */
	TArray<FVector> GetClipingBox();

	class AXRWindowActor* HoleWindow;
	void SetWindowType(uint32 type);
	uint32 GetWindowType()const { return WindowType; }
	float GetWindowSillThickness() { return SillThickness; }
	virtual void GetAttachHoleWallInfo(TArray<struct FBSPWallInfo>& OutAttachWallInfo) override;

    FString GetThumbnailUrl() const { return ThumbnailUrl; }
    void SetThumbnailUrl(FString InThumbnailUrl) { ThumbnailUrl = InThumbnailUrl; }

	/**@欧石楠 获取是否生成窗台石*/
	bool GetIfGenerateWindowStone() { return bIfGenerateWindowStone; }

	/**@欧石楠 设置是否生成窗台石*/
	void SetIfGenerateWindowStone(bool bValue);

	// @马云龙 XRLightmass 每一个面对应一个LightMapID
	FGuid GetLightMapID() { return LightMapID; }

protected:
	virtual void GenerateWindowSill(UWorld* World);
	virtual void GenerateWindowGlass(UWorld* World);

	virtual void GenerateWindowPillars(UWorld* World);

	void GenerateOutPillars(UWorld* World);

	float SillThickness;
	uint32 WindowType;

	float WindowBottomExtrudeLength = 3.0f;

	// @马云龙 XRLightmass 每一个面对应一个LightMapID
	FGuid LightMapID;

	//@郭子阳 获取窗台石长度 单位 cm
	 float GetStoneLenth();

	//@郭子阳 获取窗台石面积 单位 cm2
	 float GetStoneArea();
public:
	//@郭子阳 设置窗台石商品
	void SetContentItem(TSharedPtr<FContentItemSpace::FContentItem> Goods);
	//@郭子阳 设置窗台石商品
	TSharedPtr<FContentItemSpace::FContentItem>  GetContentItem() { return ContentItem; };

	//输出窗台石施工项
	void ConstructionData(TArray<struct FArmyGoods>& ArtificialData);
	//获取房间
	TSharedPtr<FArmyRoom> GetRoom() override ;
	virtual void OnRoomSpaceIDChanged(int32 NewSpaceID) ;
	/** 窗框 */
	//TSharedPtr<FArmyRect> WindowFrame;

	/** 窗玻璃 */
	TSharedPtr<FArmyLine> WindowGlass1;
	TSharedPtr<FArmyLine> WindowGlass2;
	TSharedPtr<FArmyLine> CenterLine;

	/** @欧石楠顶视图显示 */
	TSharedPtr<FArmyRect> TopViewWindowFrame;
	TSharedPtr<FArmyLine> TopViewWindowGlass1;
	TSharedPtr<FArmyLine> TopViewWindowGlass2;
	TSharedPtr<FArmyLine> TopCenterLine;

	/** 离地高度 */
	//float HeightToFloor;
private:
	//是否使用默认材质
	bool bDefaultMaterial = true;
	//窗台石材质
	UMaterialInterface * WindowStoneMat;
	
	//@郭子阳
	//创建与商品的关联
	int32 SaleID = -1;
	//@郭子阳
	//窗台石材质对应的商品
	TSharedPtr<FContentItemSpace::FContentItem> ContentItem=nullptr;



	/**@欧石楠 是否生产窗台石*/
	bool bIfGenerateWindowStone = true;

    /** 缩略图url */
    FString ThumbnailUrl;


};
REGISTERCLASS(FArmyWindow)