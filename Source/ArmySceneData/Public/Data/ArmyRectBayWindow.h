#pragma once

#include "ArmyHardware.h"
#include "IArmyConstructionHelper.h"
class ARMYSCENEDATA_API FArmyRectBayWindow :public FArmyHardware , public IArmyConstructionHelper
{
public:
	enum BayWindowType
	{
		AntiBoard,
		LeftBoard,
		RightBoard,
		DoubleSideBoard
	};
	FArmyRectBayWindow();
	FArmyRectBayWindow(FArmyRectBayWindow* Copy);
	virtual ~FArmyRectBayWindow();
	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	virtual void SetState(EObjectState InState) override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)override;
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual bool Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
	virtual void ApplyTransform(const FTransform& Trans) override;
	virtual const FBox GetBounds() override;
	virtual void GetCadLineArray(TArray<TPair<FVector, FVector>>& OutLineList) const override;
    virtual void Destroy() override;
	//~ End FObject2D Interface
	virtual void SetDirection(const FVector& InDirection) override;
	//~ Begin FArmyHardware Interface
	virtual void Update() override;
	//~ End FArmyHardware Interface
	virtual void Generate(UWorld* InWorld) override;


	/** 获取门的包围盒 */
	TArray<FVector> GetBoundingBox();
	/** @纪仁泽 获得立面投影 */
	TArray<struct FLinesInfo> GetFacadeBox();

	/** bsp生成裁剪 */
	TArray<FVector> GetClipingBox();

	void SetWindowHoleLength(float HoleLength);

	float GetWindowHoleLength()const { return BayWindowHoleLength; }

	void SetWindowHoleDepth(float Depth);

	float GetWindowHoleDepth()const { return BayWindowHoleDepth + Width; }

	void SetWindowLeftDist(float leftDist);

	float GetWidowLeftDist() const { return LeftOffset; }

	void SetWindowRightDist(float rightDist);

	float GetWindowRightDist()const { return RightOffset; }

	void SetWindowType(int mType);

	BayWindowType GetWindowType()const { return WindowType; }

	//void SetHeightToFloor(float dist) { HeightToFloor = dist; }

	//float GetHeightToFloor()const { return HeightToFloor; }

	virtual float GetWindowDeep()override { return BayWindowHoleDepth; }

	//void SetHeight(float InHeight) { Height = InHeight; }

	//float GetHeight()const { return Height; }

	virtual void SetOutWindowLength(float InOutLength);

	float GetOutWindowLength() { return OutWindowLength; }

	void SetWindowBottomBoardThickness(float InBottomHeight) { WindowBottomHeight = InBottomHeight; }
	float GetWindowBottomBoardTickness()const { return WindowBottomHeight; }

	void SetWindowBottomBoardExtrudeLength(float InExtrude) { WindowBottomExtrudeLength = InExtrude; }
	float GetWindowBottomBoardExtrudeLength() { return WindowBottomExtrudeLength; }

	class AXRShapeActor* WindowWall;
	class AXRWindowActor* HoleWindow;
	virtual const void GetAlonePoints(TArray< TSharedPtr<FArmyPoint> >& OutPoints) override;

	virtual void GetWindowWallVertices(TArray<struct FBSPWallInfo>& OutWallVertices);

	virtual void GetAttachHoleWallInfo(TArray<struct FBSPWallInfo>& OutAttachWallInfo)override;
	
	// @马云龙 XRLightmass 每一个面对应一个LightMapID
	FGuid GetLightMapID() { return LightMapID; }

	/**@欧石楠 获取是否生成窗台石*/
	bool GetIfGenerateWindowStone() { return bIfGenerateWindowStone; }

	/**@欧石楠 设置是否生成窗台石*/
	void SetIfGenerateWindowStone(bool bValue);

protected:
	/**@欧石楠 更新绘制数据*/
	virtual void UpdateWindowData();

	/**@欧石楠 绘制窗户数据*/
	void DrawWindowData(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	void CombineWindowPillar();
	void CombineWindowWall();
	void DrawWindow(FPrimitiveDrawInterface*PDI, const FSceneView* View);
	void DrawWithVertice(FPrimitiveDrawInterface* PDI, const FSceneView* View, TArray<FVector>& InVertices, FLinearColor LineColor, float InThickness = 0.f);
	virtual void UpdateArcAngle();
	void UpdateAntiBoardWindow();
	void UpdateLeftBoardWindow();
	void UpdateRightBoardWindow();
	void UpdateDoubleBoardWindow();
	void CaculateVertice(TArray<FVector>& Results, float leftDist, float height, float upLength);
	void CaculateCenterAndLeftAndRightDirection(FVector& centerPoint, FVector& leftDirection, FVector& rightDirection);
	void CaculateLeftBoardMiddleAndInnearVerticeList(TArray<FVector>& vertices, FVector startPoint, FVector leftDirectin, FVector rightDirection, float height, float upLenght);
	void CaculateBottomVertices(TArray<FVector>& Results);
	BayWindowType WindowType;
	//float WindowHeight;
	//float HeightToFloor;
	float BayWindowHoleLength;
	float LeftOffset;
	float RightOffset;
	float BayWindowHoleDepth;
	float ArcAngle;

	/**@欧石楠外窗长度*/
	float OutWindowLength;

	/**@欧石楠 绘制墙线*/
	TSharedPtr<FArmyLine> MaskedLine;
	TSharedPtr<FArmyLine> LeftLine_1;
	TSharedPtr<FArmyLine> LeftLine_2;
	TSharedPtr<FArmyLine> LeftLine_3;
	TSharedPtr<FArmyLine> LeftLine_4;

	TSharedPtr<FArmyLine> RightLine_1;
	TSharedPtr<FArmyLine> RightLine_2;
	TSharedPtr<FArmyLine> RightLine_3;
	TSharedPtr<FArmyLine> RightLine_4;

	TSharedPtr<FArmyLine> TopLine_1;
	TSharedPtr<FArmyLine> TopLine_2;
	TSharedPtr<FArmyLine> TopLine_3;
	TSharedPtr<FArmyLine> TopLine_4;

	/**@欧石楠 左右挡板线*/
	TSharedPtr<FArmyLine> LeftBoardLine_1;
	TSharedPtr<FArmyLine> LeftBoardLine_2;
	TSharedPtr<FArmyLine> LeftBoardLine_3;
	TSharedPtr<FArmyLine> LeftBoardLine_4;

	TSharedPtr<FArmyLine> RightBoardLine_1;
	TSharedPtr<FArmyLine> RightBoardLine_2;
	TSharedPtr<FArmyLine> RightBoardLine_3;
	TSharedPtr<FArmyLine> RightBoardLine_4;

	/**@欧石楠 两侧断墙线*/
	TSharedPtr<FArmyLine> LeftBreakLine;
	TSharedPtr<FArmyLine> RightBreakLine;

	TSharedPtr<class FArmyPolygon> MaskedPolygon;

	/**@欧石楠 其他模式绘制墙线*/	
	TSharedPtr<FArmyLine> OtherModeLeftLine_1;
	TSharedPtr<FArmyLine> OtherModeLeftLine_2;
	TSharedPtr<FArmyLine> OtherModeLeftLine_3;
	TSharedPtr<FArmyLine> OtherModeLeftLine_4;

	TSharedPtr<FArmyLine> OtherModeRightLine_1;
	TSharedPtr<FArmyLine> OtherModeRightLine_2;
	TSharedPtr<FArmyLine> OtherModeRightLine_3;
	TSharedPtr<FArmyLine> OtherModeRightLine_4;

	TSharedPtr<FArmyLine> OtherModeTopLine_1;
	TSharedPtr<FArmyLine> OtherModeTopLine_2;
	TSharedPtr<FArmyLine> OtherModeTopLine_3;
	TSharedPtr<FArmyLine> OtherModeTopLine_4;

	/**@欧石楠 其他模式左右挡板线*/
	TSharedPtr<FArmyLine> OtherModeLeftBoardLine_1;
	TSharedPtr<FArmyLine> OtherModeLeftBoardLine_2;
	TSharedPtr<FArmyLine> OtherModeLeftBoardLine_3;
	TSharedPtr<FArmyLine> OtherModeLeftBoardLine_4;

	TSharedPtr<FArmyLine> OtherModeRightBoardLine_1;
	TSharedPtr<FArmyLine> OtherModeRightBoardLine_2;
	TSharedPtr<FArmyLine> OtherModeRightBoardLine_3;
	TSharedPtr<FArmyLine> OtherModeRightBoardLine_4;

	/**@欧石楠 其他模式下半边墙线和两侧断墙线*/
	TSharedPtr<FArmyLine> OtherModeLeftBreakLine;
	TSharedPtr<FArmyLine> OtherModeRightBreakLine;
	TSharedPtr<FArmyLine> OtherModeHalfLine;

	float WindowBottomHeight;
	float WindowBottomExtrudeLength;
	//无板子飘窗
	TArray<FVector> OutWindowVerticeList;
	TArray<FVector> MiddleWindowVerticeList;
	TArray<FVector> InnearWindowVerticeList;
	TArray<FVector> BottomWindowVerticeList;

	// 右侧挡板飘窗
	TArray<FVector> RightBoardMiddleList;
	TArray<FVector> RightBoardInnearList;
	TArray<FVector> RightBoardRightList;

	// 左侧挡板飘窗
	TArray<FVector> LeftBoardLeftList;
	TArray<FVector> LeftBoardMiddleList;
	TArray<FVector> LeftBoardInnearList;

	// 双侧无挡板飘窗
	TArray<FVector> DoubleBoardLeftList;
	TArray<FVector> DoubleBoardMiddleList;
	TArray<FVector> DoubleBoardInnearList;
	TArray<FVector> DoubleBoardRightList;

	FLinearColor DrawColor;
	//TSharedPtr<class FArmyRect> RectImagePanel;

	//@欧石楠 临时用于在3D顶视图下绘制显示的数据
	void DrawTopViewWindow(FPrimitiveDrawInterface*PDI, const FSceneView* View);
	
	/**@欧石楠 是否生产窗台石*/
	bool bIfGenerateWindowStone = true;
	
protected:
	//是否使用默认材质
	bool bDefaultMaterial = true;
	//窗台石材质
	UMaterialInterface * WindowStoneMat;

	//@郭子阳 获取窗台石长度 单位 cm
	 float GetStoneLenth();

	//@郭子阳 获取窗台石面积 单位 cm2
	 float GetStoneArea();
	 //窗台石面积 单位 cm2
	 float StoneArea = 0;
public:
	//@郭子阳 设置窗台石商品
	void SetContentItem(TSharedPtr<FContentItemSpace::FContentItem> Goods);
	//@郭子阳 设置窗台石商品
	TSharedPtr<FContentItemSpace::FContentItem>  GetContentItem() { return ContentItem; };

	//输出窗台石施工项
	void ConstructionData(TArray<struct FArmyGoods>& ArtificialData);
	//@郭子阳
	//创建与商品的关联
	int32 SaleID = -1;
	//@郭子阳
	//窗台石材质对应的商品
	TSharedPtr<FContentItemSpace::FContentItem> ContentItem = nullptr;
	//@郭子阳
	//获取房间
	TSharedPtr<FArmyRoom> GetRoom() override;

	void OnRoomSpaceIDChanged(int32 NewSpaceID);
};
REGISTERCLASS(FArmyRectBayWindow)
