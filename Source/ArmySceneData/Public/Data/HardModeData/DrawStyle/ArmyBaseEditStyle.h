#pragma  once

#include "ArmyTypes.h"
#include "ArmyTextureAtlas.h"
#include "Runtime/Engine/Public/HitProxies.h"
#include "ArmyMath.h"
#include "SceneManagement.h"
#include "ArmyCommonTypes.h"
#include "FArmyConstructionItemInterface.h"
#include "ArmyConstructionManager.h"
struct HXRBaseAreaProxy :public HHitProxy
{
	DECLARE_HIT_PROXY(ARMYSCENEDATA_API);

	HXRBaseAreaProxy() :HHitProxy(HPP_World)
	{

	}

	virtual EMouseCursor::Type GetMouseCursor()override
	{
		return EMouseCursor::Crosshairs;
	}
	virtual bool AlwaysAllowsTranslucentPrimitives()const override
	{
		return true;
	}
	HXRBaseAreaProxy(TSharedPtr<class FArmyBaseArea> InBaseArea) :HHitProxy(HPP_World),
		BaseAreaObjectWeakPtr(InBaseArea)
	{

	}
	TWeakPtr<class FArmyBaseArea> BaseAreaObjectWeakPtr;

};
struct AreaCompare
{
	AreaCompare(const TArray<FVector>& inArea)
	{
		M_Area = inArea;
	}

	bool operator<(const AreaCompare& b) const
	{
		return FArmyMath::CalcPolyArea(M_Area) > FArmyMath::CalcPolyArea(b.M_Area);
	}


	bool  IsContainOther(TSharedPtr< AreaCompare> other)
	{
		if (FArmyMath::IsPolygonInPolygon(other->M_Area, M_Area))
		{
			InnearHoles.Push(AreaCompare(other->M_Area));
			return true;
		}
		return false;
	}
	TArray<AreaCompare> InnearHoles;
	TArray<FVector> M_Area;
};
enum EStyleType
{
	OT_StyleTypeNone = 0,   
	S_ContinueStyle  = 1,     // 连续直铺
	S_HerringBoneStyle = 2, // 人字铺法
	S_TrapeZoidStyle = 3, // 三六九铺法
	S_WhirlwindStyle = 4, // 旋风铺法
	S_WorkerStyle = 5, // 工字铺法
	S_CrossStyle = 6, // 交错铺法
	S_IntervalStyle = 7 ,  // 间隔铺法
	S_SeamlessStyle = 8 ,  // 墙漆、壁纸的无缝铺法
	S_BodaAreaStyle = 9,
	S_SlopeContinueStyle = 10, // @zengy 斜铺
};
enum AlignmentType
{
	LeftTop,
	MidTop,
	RightTop,
	LeftMid,
	MidMid,
	RightMid,
	LeftDown,
	MidDown,
	RightDown
};

class ARMYSCENEDATA_API FArmyBaseEditStyle
{
public:
	//@郭子阳
	//复制style的参数
	virtual void CopyFrom(TSharedPtr <FArmyBaseEditStyle >  OldStyle);

	virtual ~FArmyBaseEditStyle();

	/**
	* 获取铺法方式
	*/
	EStyleType GetEditType()const { return EdityType; }
	// 设置样式
	void SetStyleType(const EStyleType type);

	

	/**
	* 设置缝隙宽度
	*/
	void SetInternalDist(float dist);
	/**
	* 获取缝隙宽度
	*/
	float GetInternalDist()const { return InternalDist; }
	/**
	* 设置旋转角度
	*/
	void SetRotationAngle(float InAngle);
	/**
	* 获取旋转角度
	*/
	float GetRotationAngle()const { return RotationAngle; }
	/**
	* 设置离地高度
	*/
	void SetHeightToFloor(float InHeightToFloor) { HeightToFloor = InHeightToFloor; }
	/**
	* 获取离地高度
	*/
	float GetHeightToFloor()const { return HeightToFloor; }
	/**
	* 砖块的宽度
	*/
	int32 GetMainTexWidth()const { return MainTexWidth; }
	/**
	* 获取瓷砖铺贴的x方向偏移
	*/
	float GetXDirOffset()const { return XDirOffset; }
	/**
	* 获取瓷砖铺贴在y方向上的偏移
	*/
	float GetYDirOffset()const { return YDirOffset; }

	/**
	* 设置x方向偏移
	*/
	void SetXDirOffset(const float InXOffset);
	/**
	* 设置y方向上偏移
	*/
	void SetYDirOffset(const float InYOffset);
	/**
	* 设置对齐方式
	*/
	void SetAlignType(AlignmentType InAlignType);

	AlignmentType GetAlignType()const { return M_AlignmentType; }
	/**
	* 获取主纹理高度
	*/
	int32 GetMainTexHeight()const { return MainTexHeight; }
	/**
	* 设置主纹理
	*/
	void SetMainTexture(UTexture2D* InTexture);

	void SetMainTextureLocalPath(FString mainTexturePath);

	virtual void SetSecondTexturePath(const FString& InPath) {}

	virtual void  SetFirstAndSecondImage(UTexture2D* InTexture0, UTexture2D* InTexture1) {}
	/**
	* 设置主纹理和纹理采样的长度和宽度
	*/
	void SetMainTextureInfo(UTexture2D* InTexture, int32 InWidth, int32 InHeight);

	void SetMainMaterial(UMaterialInstanceDynamic * MID, int32 InWidth, int32 InLength);

	/**
	* 获取材质
	*/
	UMaterialInstanceDynamic* GetMaterial()const { return MI_FloorTextureMat; }

	virtual	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);

	virtual	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);

	virtual void Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View);
	/**
	* 瓷砖铺贴在施工图中的绘制放置
	*/
	virtual	void DrawWireFrame(class FPrimitiveDrawInterface* PDI, const FSceneView* View);

	/* @梁晓菲 无缝铺给边画线*/
	virtual	void DrawHoles(class FPrimitiveDrawInterface* PDI, const FSceneView* View);
	/**
	*  设置铺贴样式的外轮廓点和中间的空洞信息
	*/
	virtual	void SetDrawArea(const TArray<FVector>& outAreas, const TArray<TArray<FVector>>& Holes);
	/** 
	* 每种铺法对应的瓷砖铺贴方法
	*/
    virtual	void CalculateClipperBricks(const TArray<FVector>& OutAreaVertices, const TArray<TArray<FVector>>& InneraHoles) {}
    virtual	void CalculateClipperBricks() { CalculateClipperBricks(ClipperAreas, InnerHoles); }

	TSharedPtr<class FArmyBrickUnit> SelectedBrick(const FVector& pos, class UArmyEditorViewportClient* InViewportClient);

	void SetCategoryID(int32 _categoryID);

	int32 GetCategoryID();

	void SetGoodsID(int32 _GoodsID);

	void SetSecondGoodID(int32 _GoodsID);

	void SetWallPaintColor(const FLinearColor& InColor);

	FLinearColor& GetWallPaintColor() { return WallPaintColor; }

	int32 GetGoodsID();

	int32 GetSecondGoodID() { return SecondGoodsID; }
	/**
	* 设置裁切样式的平面 
	* @ planeXDir 表示平面的x方向
	* @ planeYDir 表示平面的y方向
	* @ PlaneCenter 表示平面的中心点坐标
	*/
	void SetPlaneInfo(const FVector& PlaneOrginPos, const FVector& xDir, const  FVector& yDir);
	/**
	* 设置砖块的长度和宽度
	*/
	void SetBrickWidthAndHeight(float inWidth, float inHeight) { MainTexWidth = inWidth; MainTexHeight = inHeight; }

	FString GetMainTextureUrl()const { return MainTextureUrl; }
	virtual void SetStylePlaneOffset(float InOffset);

	bool HasGoodID()const { return HasGoods; }
	/**
	* 瓷砖铺贴样式发生变化代理
	*/
	FSimpleDelegate StyleDirty;

	//当前铺法使用的商品信息
	void SetCurrentItem(const TSharedPtr< FContentItemSpace::FContentItem> item);
	TSharedPtr<FContentItemSpace::FContentItem> GetCurrentItem()const { return ContentItem; }
	// 获取裁切后砖块的个数
	int32 GetBrickNum()const;
	/**
	* 对应铺法在对应区域里面裁切后得到的多有顶点数据
	*/
	virtual void GetVertexInfo(TArray<FDynamicMeshVertex>& AllVertexs);
	/** 
	* 对应铺法在对应区域中裁切后缝隙的三角网格数据
	*/
	virtual void GetGapVertsInfo(TArray<FDynamicMeshVertex>& AllVerts);

	// 常远
	TMap <uint32, TArray<TSharedPtr<class FArmyBrickUnit>>> GetClipperBricks() {
		return ClipperBricks;
	};

    void SetLayingPointPos(const FVector& InPos) { LayingPointPos = InPos; }

	//是不是无铺法
	bool GetIsNonePaveMethod();

	// zhx  get 素材 片/块 个数
	int32  GetApplyBrickNum() { return ApplyBrickNum; }
	
	//@郭子阳
	//是否使用默认缝隙颜色，仅用于复制材质
	bool DefaultGapColor = true;
	//缝隙颜色，仅用于复制材质,并不是真正的缝隙颜色
	FLinearColor GapColor;
protected:

	FArmyBaseEditStyle();
	UMaterialInstanceDynamic* MI_FloorTextureMat=nullptr;

	UMaterialInstanceDynamic* SecondTextureMat = nullptr;
	/**
	* 计算瓷砖铺贴后的缝隙信息
	*/
	void CalculateGapVerts(const TArray<FVector>& OutAreaVertices, const TArray<TArray<FVector>>& InneraHoles);
	TArray<FVector2D> GapVerts;
	void UpdateBrickUVInfo();
	int32 CurrentTextureNum = 1;
	void UpdateBrickNumAndDirectionStartPosInfo(const TArray<FVector>& OutAreaVertices, FVector& InStartPos, FVector& InWidthDirection, FVector& InHeightDirection, int& xNum, int& yNum);

	TSharedPtr<FArmyTextureAtlas> M_TextureAtlas = nullptr;

	void RecaculateBrickLocation(const TArray<FVector>& OutAreaVertices, FVector& InStartPos, FVector&InWidthDirection, FVector& InHeightDirection, int& xNUm, int& yNum, TSharedPtr<FArmyBrickUnit> InBrick);
protected:
	virtual void UpdateAlignMentInfo(FVector& InPos, FVector& InWidthDirection, FVector& InHeight, int& xNum, int& ynum, const float boxWidth, const float boxHeight);
	TArray<FVector> ClipperAreas;
	TArray<TArray<FVector>> InnerHoles;
	AlignmentType M_AlignmentType = LeftTop;
	float InternalDist;
	EStyleType EdityType= EStyleType::OT_StyleTypeNone;

	float RotationAngle;
	float HeightToFloor = 0.0f;
	bool HasGoods = false;
	UTexture2D* MainTexture = nullptr;
	float MainTexWidth;
	float MainTexHeight;
	FColor InFillColor;
	float XDirOffset = 0.0f;
	float YDirOffset = 0.0f;
	float BodaAreaTextWidth = 0.0f;
	float BodaAreaTextHeight = 0.0f;
	FString MainTextureUrl;
	bool BNeedWallPaintColor;
	FLinearColor WallPaintColor;
	TMap < uint32, TArray<TSharedPtr<class FArmyBrickUnit>>> ClipperBricks;

	int32 CategoryID;
	int32 GoodsID=-1;
	int32 SecondGoodsID;
	FVector PlaneCenterPos = FVector::ZeroVector;
	FVector PlaneXDir = FVector(1, 0, 0);
	FVector PlaneYDir = FVector(0, 1, 0);

	//zhx  素材 片/块 个数
	int32	ApplyBrickNum;

    /** @欧石楠 起铺点位置 */
    FVector LayingPointPos;
public:

	TSharedPtr<FContentItemSpace::FContentItem>   GetContentItem() { return ContentItem; };
	//void  SetContentItem(TSharedPtr<FContentItemSpace::FContentItem> NewContentItem) {  ContentItem= NewContentItem; };
protected:
	TSharedPtr<FContentItemSpace::FContentItem> ContentItem = nullptr;

public:
	/**已废弃 施工项相关数据*/
	TSharedPtr<FArmyConstructionItemInterface> ConstructionItemData = nullptr;

	//Mat的施工项版本号
	enum class EConstructionVersion ConstructionVersion = CurrentConstructionDataVersion;
};
