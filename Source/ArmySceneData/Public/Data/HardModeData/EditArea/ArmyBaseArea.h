#pragma once

#include "ArmyObject.h"
#include "ArmyConstructionQuantity.h"
#include "ArmyCommonTypes.h"
#include "DynamicMeshBuilder.h"
#include "FArmyAreaEdge.h"
#include "ArmyHttpModule.h"
#include "ArmyBaseboardExtruder.h"
#include "ArmyCrownMouldingExtruder.h"
#include "ArmyLampSlotExtruder.h"
#include "IArmyConstructionHelper.h"
enum AreaType
{
	M_BodaArea,
	M_InnearArea,
	M_OutArea,
};

/** @梁晓菲 放样线条类型*/
enum ExtrusionType
{
	FloorExtrusionLine,
	RoofExtrusionLine,
	WallExtrusionLine,
	Lamp
};

//绘制区域的类型
enum class EDrawAreaType :uint8
{
	NotDrawArea=0, //非绘制区域
	Rectangle=1,//矩形
	RegularPolygon=2, //正多边形
	CuastomPolygon=3, //自由绘制多边形
	Circle=4, //圆
};

class ARMYSCENEDATA_API FArmyBaseArea : public FArmyObject ,public IArmyConstructionHelper
{
public:
	enum RoomStatus
	{
		FLAG_DEFAULTSTYLE = 1,			//默认样式
		FLAG_HAVESKITLINE = 1 << 1,	//有没有放样线条
		FLAG_CLOSESKITLINE = 1 << 2,	//放样线条是否断开
		FLAG_HASLIGHT = 1 << 3
	};

	// 当前房间数据状态
	uint32 RoomDataStatus = 0;

    TSharedPtr<class FArmyPolygon> OutArea;

    /**
     * 此面片是根据房间或者构建生成的唯一id
     * 墙面此值是内房间的id，房梁、柱子、门、窗是对应构建的id
     */
    FString AttachRoomID;

public:
	FArmyBaseArea();
	friend class FArmyRoomSpaceArea;
    ~FArmyBaseArea() {}

	//@郭子阳
	//获取当前绘制区域的类型
	virtual EDrawAreaType GetDrawAreaType() { return EDrawAreaType::NotDrawArea; };
	//@郭子阳 
	//@RetVal 是否是绘制区域
	virtual bool IsPlotArea() { return false; }

	virtual	void SetStyle(TSharedPtr<class FArmyBaseEditStyle> InStyle);

	virtual  void IntervalDistChange(float InV) {}

	uint32 GetRoomStatus()const { return RoomDataStatus; }

	virtual const TSharedPtr<FArmyBaseEditStyle> GetStyle(AreaType m_defualtType = M_InnearArea);

	/** @梁晓菲 图纸模式下的绘制*/
	virtual void DrawConstructionMode(class FPrimitiveDrawInterface* PDI, const FSceneView* View);

	// @梁晓菲 用斜线绘制顶的下吊区域
	virtual void DrawRoof(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	virtual	bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

	virtual AreaType SelectAreaType(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

	virtual	bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* ViwePortClient, float& dist);

	virtual	TSharedPtr<FArmyBaseArea> ForceSelected(const FVector& Pos, class UArmyEditorViewportClient* ViwePortClient, float& dist);

	virtual bool IsSelected(FVector InStart, FVector InEnd, FVector& IntersectionPoint, FVector& IntersectionNormal);

    virtual void SetState(EObjectState InState) override;

	//@郭子阳
	//递归的设置状态
	void SetStateRecursively(EObjectState InState);

	void SetDrawOutLine(bool Enable);

	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);

	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);

	TSharedPtr<class FArmyPolygon> GetOutArea()const { return OutArea; }
	TSharedPtr<class FArmyPolygon> GetInnerArea() const { return InneraArea; }
	/**
	* 设置平面的 x 方向、y 方向和坐标原点 所有平面上的点均为二维坐标点，
	*/
	void SetPlaneInfo(const  FVector& XDir, const  FVector& Ydir, const  FVector& PlaneCenter);
	/**
	* 获取平面的x方向
	*/
	FVector GetXDir()const { return PlaneXDir; }
	/**
	* 获取平面的y 方向
	*/
	FVector GetYDir()const { return PlaneYDir; }
	/**
	*  获取平面的法线方向
	*/
	FVector GetPlaneNormal()const { return PlaneNormal; }
	/** 
	* 获取平面的中心点坐标
	*/
	FVector GetPlaneCenter()const { return PlaneOrignPos; }

	void PostModify(bool bTransaction/* =false */)override;

	FMultiVoidDelegate PositionChanged;

	FSimpleDelegate ExtursionHeightEvent;

	virtual void RefreshExtrusionPlane();
	/**
	* 计算平面挖洞后三角剖分结果，主要用于顶面视图下求半透明蒙版
	*/
	virtual void  CalculateBackGroundSurface(const TArray<FVector>& outLine, const TArray<TArray<FVector>>& AreaHoles);

	virtual void DrawOutLine(class FPrimitiveDrawInterface* PDI, const FSceneView* View, FLinearColor Color = FLinearColor::White, float thickness = 2.0f);
	//@郭子阳
	//递归的绘制轮廓线，轮廓线颜色用 FArmyBaseSurfaceController::DrawAreaOutlineColor 和 FArmyBaseSurfaceController::DrawAreaOutlineColorOnSelected
	virtual void DrawOutLineRecursion(class FPrimitiveDrawInterface* PDI, const FSceneView* View, FLinearColor NormalColor = FLinearColor::White, FLinearColor SelectedColor = FLinearColor::White, float thickness = 2.0f);

	virtual void GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs = false) override;

	void GetOutlineCenter(FVector&) const;

	/** @梁晓菲 获取放样线条数组*/
	void GetExtrusionLines(TMap<int32, TArray< TSharedPtr<FArmyLine> >>& ExtrusionLines, ExtrusionType TypeOfExtrusion) const;

	//获取底图GUID
	FGuid GetParentGuid()const { return ParentGUID; };
	void SetParentGuid(FGuid id) { ParentGUID = id; };

	void SetBodaTextureURL(FString _BodaTexture) { BodaTextureUrl = _BodaTexture; }
	FString GetBodaTextureURL() { return BodaTextureUrl; }
	void SetCornerTextureURL(FString _BodaTexture) { CornerTextureUrl = _BodaTexture; }
	FString GetCornerTextureURL() { return CornerTextureUrl; }

	uint32 SurfaceType = 0;// 0 表示地，1表示墙，2 表示顶

	uint32 SurportPlaceArea = 0;// 0 表示商品放置在地面，1 表示商品放置在墙面，2 表示商品放置在顶面

	virtual void SetExtrusionHeight(float InHeight);

	float GetExtrusionHeight()const { return M_ExtrusionHeight; }

	/** @梁晓菲 是否可以放置灯槽(是否与其他区域相交)*/
	bool bCanGenerateLampSlot(float lampSlotWidth);

    void CalculateLampSlotPaths(
        TArray<FArmyExtrusionPath>& OutPaths,
        TArray<FVector>& OutVertexes,
        TSharedPtr<FContentItemSpace::FContentItem> ContentItem,
        float InlampSlotWidth);

	/**
	 * 根据放样线条路径生成放样线条
	 */
    virtual void GenerateLampSlot(
        UWorld* InWorld,
        TSharedPtr<FContentItemSpace::FContentItem> ContentItem,
        float lampSlotWidth,
        bool bIsLoad = false)
    {}

    /** @欧石楠 分段生成器 */
    TSharedPtr<class FArmyBaseboardExtruder> GetBaseboardExtruder();
    TSharedPtr<class FArmyCrownMouldingExtruder> GetCrownMouldingExtruder();
    TSharedPtr<class FArmyLampSlotExtruder> GetLampSlotExtruder();
    TSharedPtr<FArmyBaseboardExtruder> BaseboardExtruder;
    TSharedPtr<FArmyCrownMouldingExtruder> CrownMouldingExtruder;
    TSharedPtr<FArmyLampSlotExtruder> LampSlotExtruder;

	/** @梁晓菲 根据ContentItem进行放样*/
	virtual	void GenerateExtrudeActor(UWorld* InWorld, TSharedPtr<FContentItemSpace::FContentItem> ContentItem);
	virtual void GenerateWallMoldingLine(UWorld* InWorld, TSharedPtr<FContentItemSpace::FContentItem> ContentItem);
	virtual void GenerateBuckleActorLine(UWorld* InWorld, TSharedPtr<FContentItemSpace::FContentItem> ContentItem);

	virtual bool CheckEdgeCouldPlaceBuckle(const FVector& startEdge, const FVector& endEdge);
	/**
	* 删除造型线条
	*/
	void RemoveWallModingLine();
	void RemoveBuckleLine(class AArmyExtrusionActor* BuckleActor);

    /** @欧石楠 删除踢脚线、顶角线 */
    virtual void RemoveSkitLineActor(AArmyExtrusionActor* skitActor);

	/** 删除灯槽 */
	virtual	void RemoveLampSlot(AArmyExtrusionActor* InLampSlotActor);

	//计算面积，考虑孔洞
	virtual float GetAreaDimension()const;

	////计算面积，不考虑孔洞
	float GetAreaDimensionWhithoutHoles() const;
	//获取面积，随绘制区域的显隐变化
	virtual float GetAreaDimensionWithVisibility();

	/** @梁晓菲 获取施工区域闭合周长*/
	virtual float GetAreaCircumference()const;

	virtual float GetAreaLength()const;

	virtual void Destroy()override;

	virtual void RecaculateRelatedArea();

	/** @郭子阳 获取施工区域*/
	virtual void GetConstructionAreas(TArray<FArmyGoods>& OutAreas);

	float GetDeltaHeight()const { return m_deltaHeight; }

	void SkitLineGoodsID(int32 InID) { SkitLineID = InID; }
	int32 GetSkitLineGoodsID()const { return SkitLineID; }
	/**
	* 设置铺贴缝隙颜色
	*/
	virtual	void SetGapColor(const FLinearColor& InColor);

	/**
	* 获取缝隙颜色
	*/
	FLinearColor& GetGapColor() { return GapColor; }

	virtual void SetPropertyFlag(PropertyType InType, bool InUse);

	virtual void SetActorVisible(bool InVisible);
	/**
	* 高亮区域三角剖分后的点位坐标
	*/
	TArray<FVector> TempHighLightVertices;

	class AArmyExtrusionActor* GetLampSlotActor() { return LampSlotActor; }
    TArray<FVector> GetLampLightDashLines();

	class AXRWallActor* GetWallActor() { return WallActor; }
	//计算放样线条
	virtual	void CaculateConstructionSkitLineInfo(const TArray<TArray<FVector>>& SkintLines, const TArray<float>& SkitLineDelta);

	TArray<struct FDynamicMeshVertex> GetBackGroundSurface() { return BackGroundSurface; }
	TSharedPtr<class FArmyBodaArea> GetBodaSurfaceArea() { return BodaSurfaceArea; }

	TArray<TSharedPtr<FContentItemSpace::FContentItem>> GetContentItemList() { return ContentItemList; }

	/* @梁晓菲 判断是否Hover了单条边并且返回边的端点信息*/
	virtual bool BHoverSingleEdge(const FVector2D& Pos, class UArmyEditorViewportClient* ViewPortClient, FVector& OutStart, FVector& OutEnd);
	/* @梁晓菲 设置扣条放置方式*/
	void SetbIsSingleBuckle(bool BIsSingle);
	// @马云龙 XRLightmass
	FGuid GetLightMapID() { return LightMapID; }
	FGuid GetExtrusionLightMapID() { return ExtrusionLightMapID; }
	
	//张洪星  硬装智能设计
	void SetVectorCode(int32 vectorCode);
	//张洪星  活得硬装智能设计
	int32 GetVectorCode() { return VectorCode; }
	TSharedPtr<FContentItemSpace::FContentItem> GetEdgeVectorCodeCloseToPos(TSharedPtr<FArmyBaseArea> FloorSpace,int32 & vectorCode);

	TSharedPtr<FContentItemSpace::FContentItem>  LampContentItem;
	// 张洪星 结束

	// 常远 获得样式
	TSharedPtr<class FArmyBaseEditStyle> GetMatStyle();
	// 常远 放样线条  
	TArray<TArray<TArray<FVector>>> GetConstructionSkitLineVerts() { return ConstructionSkitLineVerts; }
	// 平面坐标系转世界坐标系
	FVector PlaneToTranlate(const FVector& Plane);

	// 文海涛 通过世界坐标点获取相对当前平面的本地坐标
	FVector GetPlaneLocalFromWorld(const FVector& WorldPos);

    void UpdateWallActor();

    /** @欧石楠 设置起铺点 */
    void SetLayingPointPos(FVector InLayingPointPos);
    void OffsetLayingPointPos(const FVector& InDelta);

    const FVector& GetLayingPointPos();

	virtual void RefreshPolyVertices();

    virtual void UpdateWallArea(TArray<FVector> NewVertexes) {}

    /** @欧石楠 重新创建放样模型 */
    void OffsetExtrusionActors(const FVector& InOffset);

protected:
	//@郭子阳 获取所有面共有的铺贴类施工项
	virtual void GetCommonConstruction(TArray<FArmyGoods>&  ArtificialData);

	int32 ConvertStyleToPavingID(TSharedPtr<FArmyBaseEditStyle> InStyle);
	void DestroyAttachActor();
	void DestroyLampLight();/* @梁晓菲 销毁灯槽，清空路径，重新绘制区域，重新放样*/
	virtual	bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient, float& dist, const TArray<FVector>& PolygonVertices);
	TSharedPtr<class FArmyBodaArea> BodaSurfaceArea;
	bool HasCommonEdge = false;
	FVector PlaneNormal = FVector(0, 0, 1);
	FVector PlaneOrignPos = FVector(0, 0, 0);
	FVector PlaneXDir = FVector(1, 0, 0);
	FVector PlaneYDir = FVector(0, 1, 0);
	void ConvertRelativeToWorldPos(TArray<FVector>& InPoints);
	float M_ExtrusionHeight = 0.0f;
	float LampSlotWidth = 0.0f;
	TArray<TSharedPtr<struct AreaCompare>>  CombineOutAreaAndInnearHole(const TArray<FVector>& OutArea, const TArray<TArray<FVector>>& Holes);
	void TriangleArea(const TArray<FVector>& OutAreas, const TArray<TArray<FVector>>& Holes);
	TArray<struct FDynamicMeshVertex> BackGroundSurface;

    /** @欧石楠 起铺点位置 */
    FVector LayingPointPos;

	//@郭子阳
	//是不是临时面
	bool IsTempArea = false;

public:
	float GetLampSlotWidth() { return LampSlotWidth; }

protected:
	// 更具样式生成墙体
	virtual	void CalculateWallActor(TSharedPtr<FArmyBaseEditStyle> InStyle);
	
	// 墙体actor
	class AXRWallActor* WallActor;
	UMaterialInstanceDynamic* MI_BackGroundColor;
	FGuid ParentGUID;
	
	// 放样线条actor
	class AArmyExtrusionActor* ExturesionActor;
	// 灯槽actor
	class AArmyExtrusionActor* LampSlotActor;
	FString ExtrudedxfFilePath;
	FString ExturdedxfMaterialFilePath;
	int32 SkitLineID;
	FVector2D TranlateToPlane(const FVector& World);
	bool BeDrawOutLine = false;
	TSharedPtr<class FArmyBaseEditStyle> MatStyle;

	TSharedPtr<class FArmyPolygon> InneraArea;
	
	FVector2D minPos;
	FString BodaTextureUrl;//波打线纹理URL
	FString CornerTextureUrl;//角砖纹理URL

	float m_deltaHeight = 0.0f;
	TArray<FVector> LampLightDashLines;
	TArray<TArray<TArray<FVector>>> ConstructionSkitLineVerts;
	//缝隙颜色
	FLinearColor GapColor;

	TArray<TSharedPtr<FContentItemSpace::FContentItem>> ContentItemList;// @梁晓菲 一个区域可以有多个ContentItem信息

	// @马云龙 XRLightmass
	FGuid LightMapID;
	FGuid ExtrusionLightMapID;

public:
	TSharedPtr<FContentItemSpace::FContentItem>  TempContent;

	//void ReqConstructionData(int32 GoodsId);
	//void ResConstructionData(FArmyHttpResponse Response, int32 GoodsId);

protected:
	/* @梁晓菲 扣条相关内容*/
	bool bHasBuckle = false;/* 是否放置了扣条*/
	bool bIsSingleBuckle = false;/* 扣条是否为单边放置*/
	int32 VectorCode;/* 点的编号*/
	TMap<class FArmyAreaEdge, TSharedPtr<FContentItemSpace::FContentItem>> BuckleMap;
	TMap<class FArmyAreaEdge, AArmyExtrusionActor*> BuckleActorMap;
	TArray<class FArmyAreaEdge> BuckleEdges;
	TArray<class AArmyExtrusionActor*>  BuckleActors;/* 扣条Actor信息*/
	void CalculateBuckle();

    /** @欧石楠 生成施工图线条 */
    void GenerateConstructionInfo();

    /** @欧石楠 计算放样路径 */
    void CalculateBaseboardPaths(TArray<FArmyExtrusionPath>& OutPaths, TSharedPtr<FContentItemSpace::FContentItem> ContentItem);
    void CalculateCrownMouldingPaths(TArray<FArmyExtrusionPath>& OutPaths, TSharedPtr<FContentItemSpace::FContentItem> ContentItem);

    /** @欧石楠 清空放样类物体 */
    void ClearExtrusionActors();

public:
	/** 施工项相关数据*/
	TSharedPtr<FArmyConstructionItemInterface> ConstructionItemData;

	//@郭子阳 获取原始墙面的施工项
	void GetConstructionData(TArray<FArmyGoods>& ArtificialData);

	//@郭子阳 获取墙面上铺贴的材质的施工项参数
	virtual struct ConstructionPatameters  GetMatConstructionParameter() ;
	//@郭子阳 获取面所在房间
	virtual TSharedPtr<class FArmyRoom> GetRoom() override;
	//@郭子阳 当房间ID改变时发生
	virtual void OnRoomSpaceIDChanged(int32 NewSpaceID) override;
#pragma region 绘制区域相关


	/**
	*添加区域是否能否被放置在区域中
	*/
	// @InTestArea 绘制区域的顶点 
	// @OutParent 如果函数返回true ,则OutParent=可以包裹绘制区域的最小父面。否则OutParent=nullptr
	virtual bool TestAreaCanAddOrNot(const TArray<FVector>& InTestArea, FGuid ObjId,TSharedPtr<FArmyBaseArea> OutParent=nullptr);

	/**
	* 在墙面父空间中添加绘制的子空间区域，例如圆形区域、矩形区域、自由多边形区域
	*/
	virtual void AddArea(TSharedPtr<FArmyBaseArea>);

	//@郭子阳
	//把子面给自己的父面
	void MoveChildAreaToParent();

protected:

	//virtual void RemoveFromParents(FObjectPtr InParent = NULL);

	FDelegateHandle PositionChangedHandles;
public:


	/**
	* 删除墙面父区域中的子空间区域
	*/
	void RemoveArea(TSharedPtr<FArmyBaseArea> InArea);

	//@郭子阳
	//在这个面上的子区域 
	TArray<TSharedPtr<FArmyBaseArea>> RoomEditAreas;

	//获得绘制区域的父面
	//@ author 郭子阳
	//@ Ret 返回绘制区域的父面
	TSharedPtr<FArmyBaseArea> GetParentArea();

	//设置绘制区域的父面
	//@ author 郭子阳
	//@ NewParent 绘制区域的新父面
	void SetParentArea(TSharedPtr<FArmyBaseArea> NewParent);

	//判断一个点是不是在面内
	//@ author 郭子阳
	//@ Ret 返回判断结果
	//@ InPoint 点
	//@ ConsiderHole 要不要考虑面的内部空洞，若 ConsiderHole=true,当点在孔洞内部时返回false
	virtual bool IsPointIn(const FVector& InPoint,bool ConsiderHole);

	//递归寻找一个能包围点的最小子面
	//@ author 郭子阳
	//@ Ret 返回找到的面
	//@ InPoint 点
	virtual TSharedPtr<FArmyBaseArea> FindArea(const FVector& InPoint);
	
	//将面移动一段距离,主要用于绘制区域
	//@ author 郭子阳
	//@ Delta 位移
	virtual void Move(const FVector& Offset);

	/**
	* 获取此墙面区域中对应编辑的子区域
	*/
	TArray<TSharedPtr<FArmyBaseArea>> GetEditAreas() { return RoomEditAreas; }
#pragma endregion

	TArray<TArray<FVector>> SurfaceHoles;
	TArray<FVector> dropVerts;
	virtual void CalculateOutAndInnerHoles(TArray<FVector>& InOutArea, TArray<TArray<FVector>>& InnearHoles);
};

struct TempRoomInstersection
{
	TempRoomInstersection(TWeakPtr<FArmyBaseArea> room, float indist)
	{
		roomSpace = room;
		dist = indist;
	}
	TWeakPtr<FArmyBaseArea> roomSpace;
	float dist;

	bool operator <(const TempRoomInstersection& b)const
	{
		return dist < b.dist;
	}


	
};