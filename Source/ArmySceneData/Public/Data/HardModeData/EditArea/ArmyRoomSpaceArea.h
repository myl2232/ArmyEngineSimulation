#pragma once

#include "ArmyObject.h"
#include "ArmyBaseArea.h"
#include "ArmyPolygon.h"
#include "SceneManagement.h"
#include "ArmySceneData.h"
#include "DynamicMeshBuilder.h"
#include "FArmyConstructionItemInterface.h"
#include "ArmyExtruder.h"

/**
 * 所有二维户型生成的三维墙面数据类
 */
class ARMYSCENEDATA_API FArmyRoomSpaceArea : public FArmyBaseArea
{
public:
	struct SurfaceHoleInfo
	{
		EObjectType surfaceHoleType;
		TArray<FVector> surfaceVertices;
	};

public:
	FArmyRoomSpaceArea();

	// FObject2D Interface Begin
	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;

	virtual	bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* ViwePortClient, float& dist) override;

	virtual bool IsSelected(FVector InStart, FVector InEnd, FVector& IntersectionPoint, FVector& IntersectionNormal)override;

	virtual bool IsPointOneSurface(const FVector& InPos);

	bool IsSelectEditPlane(const FVector& Pos, class UArmyEditorViewportClient* ViwePortClient, float& dist, bool IncludeInnearAreas = false);

	/**
	* 设置面的外轮廓点和面上的洞位置
	*/
	void SetVerticesAndHoles(const TArray<FVector>& InVertices, const TArray<TArray<FVector>>& InHoles);
	/**
	* 生成原始墙体
	*/
	void GenerateOrignalActor(const TArray<FVector>& InVertices, const TArray<TArray<FVector>>& InHoles, const FVector& xDir, const FVector& yDir, const FVector& center);
	/**  
	* 掩藏原始墙体
	*/
	void SetOriginalActorVisible(bool bVisible);
	/**
	* 掩藏或者显示完成墙面
	*/
	void SetOrignalFinishWallVisible(bool bVisible);

	/**
	* 获取墙、顶、地面的外轮廓
	*/
	const  TArray<FVector>& GetVertices()const { return InneraArea->Vertices; }

	virtual const FBox GetBounds();
	/**
	* 获取区域中的捕捉信息，主要是给六轴标尺和捕捉功能使用
	*/
	virtual void GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs = false) override;
	/**
	* 在墙面父空间中添加绘制的子空间区域，例如圆形区域、矩形区域、自由多边形区域
	*/
	//void AddArea(TSharedPtr<FArmyBaseArea> InArea);

	//获取面积，随绘制区域的显隐变化
	virtual	float GetAreaDimensionWithVisibility() override;

	/**
	* 删除墙面父区域中的子空间区域
	*/
	//void RemoveArea(TSharedPtr<FArmyBaseArea> InArea);
	/** 
	* 设置此编辑面的瓷砖、地板铺贴样式
	*/
	void SetStyle(TSharedPtr<class FArmyBaseEditStyle> InStyle)override;

	/**
	* 获取此区域中门洞和窗洞的数据
	*/
	const TArray<TArray<FVector>>& GetSurfaceHoles()const { return SurfaceHoles; }
	/**
	* 将此区域设置成对应的毛坯状态
	*/
	void ResetSurfaceStyle(bool cleanEditAreas = false);
	/**
	* 获取此区域的面积
	*/
	//float GetAreaDimension()const override;

	/* @郭子阳 重载，设置施工项区域*/
	void GetConstructionAreas(TArray<FArmyGoods>& ArtificialData) override;

	virtual void GetCommonConstruction(TArray<FArmyGoods>&  ArtificialData) override;

	// @梁晓菲 绘制过门石区域
	void DrawBridgeStoneArea(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	/**
	* 将房顶或者地面下掉或者抬高一定的高度
	*/
	virtual void SetExtrusionHeight(float InHeight)override;
	/**
	* 将顶面下掉后计算对应房间内相邻的几面墙体，然后在每个墙体上扣掉对应顶面下掉区域的面积
	*/
	virtual void SetRoofDropDownHeight(float InHeight);

	virtual void SetPropertyFlag(PropertyType InType, bool InUse)override;

	virtual void SetActorVisible(bool InVisible)override;

	virtual void PostModify(bool bTransaction)override;

	virtual void Destroy()override;

	/**
	* 获取原始面
	*/
	AXRWallActor* GetOriginalSurfaceActor() { return OrignalSurfaceActor; }

	/**
	* 此id 是唯一确定三维墙面唯一对应二维户型中墙线的id
	*/
	FString AttachBspID;
	/**
	* 此面上所有门、窗、哑口构建的名称，主要用于双击进入墙面后带出相应墙面上3d模型功能
	*/
	TArray<FString> CurrentAttachWindowName;
	/**
	 *面片是由什么二维面片生成的
	 */
	EObjectType GenerateFromObjectType;

	TArray<TArray<FVector>> GetOrginalSurfaceHoles() { return OrginalSurfaceHoles; }

	//@郭子阳 扣条施工算量
	void GetBuckleConstructionItems(TArray<FArmyGoods>& OutAre, const TSharedPtr<FArmyRoomSpaceArea> RelateRoom);

	//@郭子阳 获得面上的扣条是否与另一个面相连
	//@Buckle 扣条 
	//@AnotherArea 另一个面
	//@Ret 如果扣条处于this与另一个面之间，就返回true,否则false;
	bool IsBuckleConnected(AArmyExtrusionActor * Buckle, const TSharedPtr<FArmyRoomSpaceArea> AnotherArea);

	/**
	* 检测过门石区域那些边能放置扣条
	*/
	bool CheckEdgeCouldPlaceBuckle(const FVector& startEdge, const FVector& endEdge)override;

	//@马云龙 XRLightmass
	FGuid GetExtrusionLightMapID(class AArmyExtrusionActor* InActor);
	// 获得墙面片对应的墙线
	TSharedPtr<class FArmyWallLine> GetAttachLine();
	// 获得绘制立面图的线数据  0 表示地，1表示墙，2 表示顶
	TArray<struct FLinesInfo> GetFacadeBox(int32 InSurfaceType = 1);

    /** @欧石楠 获得区域平面的Z轴坐标 */
    const float GetZ() const;

protected:
	TArray<TSharedPtr<AreaCompare>> CombineOutAreaAndInnearHole(const TArray<FVector>& OutArea, const TArray<TArray<FVector>>& Holes);

	//@马云龙 XRLightmass 记录踢脚线光照图LightmapGUID
	TArray<FGuid> SkirtLineGUIDs;

	// 原始墙面外轮廓点
	TArray<FVector> OrginalSurfaceVerts;
	// 原始墙面的洞顶点数据
	TArray<TArray<FVector>> OrginalSurfaceHoles;
	// 原始墙面中心点数据
	FVector OrignalSurfaceCenter;

	/* @梁晓菲 图纸模式绘制*/
	virtual void DrawConstructionMode(class FPrimitiveDrawInterface* PDI, const FSceneView* View)override;

private:
	class AXRWallActor* OrignalSurfaceActor = NULL;

public:
	//@郭子阳 获取面所在房间
	virtual TSharedPtr<class FArmyRoom> GetRoom() override;
};

REGISTERCLASS(FArmyRoomSpaceArea)