/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File ArmyAutoCad.h
* @Description 与cad文件交互的工具
*
* @Author 欧石楠
* @Date 2019年2月13日
* @Version 1.0
*/


#pragma once
#include "CoreMinimal.h"
#include "IBlock.h"
#include "IFactory.h"
//#include "ArmyDownloadCad.h"
#include "AllowWindowsPlatformTypes.h"
#include "Windows.h"
#include <string>
#include "HideWindowsPlatformTypes.h"
#include "ArmyPolygon.h"
#include  "ArmyLine.h"
#include "ArmyPoint.h"
#include "ArmyCircle.h"
#include "ArmyRect.h"
#include "ArmyPolyline.h"
#include "ArmyArcLine.h"
#include "ArmyObject.h"
#include "math.h"
#include "ArmyWHCTableObject.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

enum EIncrementBlockType
{
	IBT_Window = 1,
	IBT_Beam = 2,
	IBT_Pillar =3,
	IBT_AirFlue =4,
	IBT_Door = 5,
	IBT_Lable = 6,
	IBT_DimLable = 7,
	IBT_BoardSpliteline =8,
	IBT_CustomHatch =9,
	IBT_PackPipe =10,
	IBT_PassEdge =11,
	IBT_NewPassEdge = 12,
	IBT_Punch = 13,
	IBT_Symbol=14,
	IBT_Cabinet = 15,
	IBT_CustomDefine = 16,
	IBT_WHCComponent = 17,
};

enum EDefineBlockType
{
	DBT_Compass = 1,
	DBT_Entrance =2,
};
 
enum ERealTimeBlockType
{
	ERT_Window,
	ERT_FloorWindow,
	ERT_RectBayWindow,
	ERT_TrapeBayWindow,
	ERT_CornerBayWindow,
	ERT_SingleDoor,
	ERT_SlidingDoor,
	ERT_SecurityDoor,
};

#pragma region block辅助类

class ARMYTOOLS_API "FArmyCadBundle
{
public:
	"FArmyCadBundle();
	~"FArmyCadBundle() = default;
	friend class "FArmyAutoCad;
	/**
	 * @添加点
	 * @param Point - const "FArmyPoint & - 
	 * @return void - 
	 */
	void AddPoint(const TSharedPtr<"FArmyPoint> &Point,const FString &LinesType=L"CONTINUOUS",LineWeight Weight= LineWeight::Weight000);
	
	/**
	 * @添加线
	 * @param Line - const "FArmyLine & - 
	 * @return void - 
	 */
	void AddLine(const TSharedPtr<"FArmyLine>&Line, const FString &LinesType = L"CONTINUOUS", LineWeight Weight = LineWeight::Weight000);
	
	/**
	 * @添加多边形
	 * @param Polygon - const "FArmyPolygon & - 
	 * @return void - 
	 */
	void AddPolygon(const TSharedPtr<"FArmyPolygon>&Polygon, const FString &LinesType = L"CONTINUOUS", LineWeight Weight = LineWeight::Weight000);
	
	/**
	 * 添加圆
	 * @param Circle - const "FArmyCircle & - 
	 * @return void - 
	 */
	void AddCircle(const TSharedPtr<"FArmyCircle>&Circle, const FString &LinesType = L"CONTINUOUS", LineWeight Weight = LineWeight::Weight000);
	
	/**
	 * 添加半圆
	 * @param ArcLine - const "FArmyArcLine & - 
	 * @return void - 
	 */
	void AddArcLine(const TSharedPtr<"FArmyArcLine>&ArcLine, const FString &LinesType = L"CONTINUOUS", LineWeight Weight = LineWeight::Weight000);
	

	/**
	 * 添加多线段
	 * @param Polyline - const "FArmyPolyline & - 
	 * @return void - 
	 */
	void AddPolyline(const TSharedPtr<"FArmyPolyline>&Polyline, const FString &LinesType = L"CONTINUOUS", LineWeight Weight = LineWeight::Weight000);

	/**
	* 添加文字
	* @param Polyline - const "FArmyPolyline & -
	* @return void -
	*/
	void AddCanvasText(const FScaleCanvasText CanvasText, const FString &LinesType = L"CONTINUOUS", LineWeight Weight = LineWeight::Weight000);


	/**
	 * @添加键值
	 * @param Key - int32 - 
	 * @return void - 
	 */
	void AddKey(int32 InKey);

 
private:
 
	TArray<TSharedPtr<"FArmyPoint>>		Points;
	TArray<LineWeight>	PointsWeight;
	TArray<FString>			PointsLineName;
		
	TArray<TSharedPtr<"FArmyLine>>		Lines;
	TArray<LineWeight>	LinesWeight;
	TArray<FString>			LinesLineName;

	TArray<TSharedPtr<"FArmyPolygon>> Polygons;
	TArray<LineWeight>	PolygonsWeight;
	TArray<FString>			PolygonsLineName;

	TArray<TSharedPtr<"FArmyCircle>>		Circles;
	TArray<LineWeight>	CirclesWeight;
	TArray<FString>			CirclesLineName;

	TArray<TSharedPtr<"FArmyArcLine>>	ArcLines;
	TArray<LineWeight>	ArcLinesWeight;
	TArray<FString>			ArcLinesLineName;

	TArray<TSharedPtr<"FArmyPolyline>>	Polylines;
	TArray<LineWeight>	PolylinesWeight;
	TArray<FString>			PolylinesLineName;

	// 文字
	TArray<FScaleCanvasText>	CanvasTexts;
	TArray<LineWeight>	CanvasTextsWeight;
	TArray<FString>			CanvasTextsLineName;

	int32 Key;
	FString BlockName;
	FVector BasePoint;
	bool IsDefaultDesc;
	AttributeDesc Desc;
};

#pragma endregion block辅助类

class ARMYTOOLS_API "FArmyAutoCad
{
public:
	static const TSharedRef<"FArmyAutoCad>& Get();
	"FArmyAutoCad();
	~"FArmyAutoCad();
private:
	/** 工具类 */
	class VectorHelper
	{
	public:
		VectorHelper(FVector param)
		{
			vector = param;
		}
	    Double4	asDouble2()
		{
			return Double4(vector.X, vector.Y);
		}
		Double4 asDouble3()
		{
			return Double4(vector.X, vector.Y, vector.Z);
		}
		Double4 cadCoord()
		{
			return Double4(vector.X, -vector.Y, vector.Z);
		}
		FVector vector;
	};

	// 块扩展类
	class BlockExt
	{
	public:
		BlockExt(IBlock* pblock, FVector  poffset,const FString &Name,float InLength = 0) :
			block(pblock), offset(poffset), blockName(Name),FrameLength(InLength)
		{}
		BlockExt() = default;
		~BlockExt() = default;
	public:
		IBlock*					block;
		FString					blockName;
		FVector  				offset;
		float FrameLength;
	};
public:	
 
	// 清空画布
	void ClearCanvas();
 
	// 创建图层
	bool CreateLayer(TMap<int32, TSharedPtr<LayerInfo>> LayerMap);

	/**
	 * 添加构件,来自文件
	 * @param Key - int32 - 构件key值
	 * @param Path - FString - 构件路径
	 * @return bool true表示添加成功
	 */
	bool AddComponent(int32 Key,FString Path);
	
	/**
	* 添加构件,来自bim
	* @param Key - int32 - 构件key值
	* @param Furniture - TSharedPtr<class "FArmyFurniture> - 构件
	* @return bool true表示添加成功
	*/
	bool AddComponent(int32 Key, TSharedPtr<class "FArmyFurniture> Furniture);
	/**
	 * 添加家具,来自文件
	 * @param key - int32 - 
	 * @param Path - FString - 
	 * @return bool - true表示添加成功
	 */
	bool AddFurniture(int32 Key, FString Path);
	/**
	* @添加家具,来自bim
	* @param key - int32 -
	* @param Furniture - TSharedPtr<class "FArmyFurniture> -
	* @return bool - true 表示添加成功
	*/
	bool AddFurniture(int32 Key, TSharedPtr<class "FArmyFurniture> Furniture);
 
	
	/**
	* 在指定的图层上绘制图框
	* @param LayerName - const FVector & - 图层名称
	* @param index - int - 图框名称索引
	* @param InFrame - const TSharedPtr<class "FArmyConstructionFrame>& - 图框数据
	* @return bool - true 表示绘制成功
	*/
	bool DrawFrame(const FString LayerName,int index, const TSharedPtr<class "FArmyConstructionFrame>& InFrame);
	
	/**
	 * 在指定的图框指定的图层上绘制构件
	 * @param FrameIndex - const int32 -  图框名称
	 * @param LayerName - const FString - 图层名称
	 * @param Key - int32 - 构件id
	 * @param InFurniture - const TSharedPtr<class "FArmyFurniture> - 构件
	 * @param Scale - const FVector & - 缩放
	 * @param Angle - const float - 旋转
	 * @return bool - 成功返回true
	 */
	bool DrawComponent(const int32 FrameIndex, const FString LayerName, int32 Key, const TSharedPtr<class "FArmyFurniture> InFurniture);
	
	/**
	* 在指定的图框指定的图层上绘制家具
	* @param FrameIndex - const int32 -  图层名字
	* @param LayerName - const FString - 图层名称
	* @param Key - int32 -				构件id
	* @param Postion - const FVector & -  世界坐标
	* @param Scale - const FVector & -		 缩放
	* @param Angle - const float - 旋转
	* @return bool - 成功返回true
	*/
	bool DrawFurniture(const int32 FrameIndex, const FString LayerName, int32 Key, const FVector&Postion, const FVector &Scale, const float Angle);
	
	/**
	* 在指定的图框指定的图层上绘制线段
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const int32 - 图层名称
	* @param Beg - const FVector & - 开始点
	* @param End - const FVector & - 结束点
	* @return bool - true 表示绘制成功
	*/
	bool DrawLine(const int32 FrameIndex, const FString LayerName, const FVector &Beg, const FVector &End);
	
	/**
	* 在指定的图框指定的图层上绘制线段
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const int32 - 图层名称
	* @param WallLine - const TSharedPtr<class "FArmyLine> & - 墙线
	* @return bool - true 表示绘制成功
	*/
	bool DrawLine(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyLine>& WallLine);

	/**
	* 在指定的图框指定的图层上绘制线段
	* @param FrameIndex - const int32 - 图框名称
	* @param InLayer - const TSharedPtr<LayerInfo> - 图层信息
	* @param WallLine - const TSharedPtr<class "FArmyLine> & - 墙线
	* @return bool - true 表示绘制成功
	*/
	bool DrawLine(const int32 FrameIndex, TSharedPtr<LayerInfo> InLayer, const TSharedPtr<class "FArmyLine>& WallLine);

	
	/**
	* 在指定的图框指定的图层上绘制多边形线
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const int32 - 图层名称
	* @param InPolygon - const TSharedPtr<class "FArmyPolygon>&  - 多边形
	* @return bool - true 表示绘制成功
	*/
	bool DrawPolygon(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyPolygon>& InPolygon);

	/**
	* 在指定的图框指定的图层上绘制标准窗户
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const int32 - 图层名称
	* @param InWindow - const const TSharedPtr<class "FArmyWindow> - 标准窗户
	* @return bool - true 表示绘制成功
	*/
	bool DrawWindow(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyWindow> InWindow);
	bool DrawWindow2(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyWindow> InWindow);

	/**
	* 在指定的图框指定的图层上绘制落地窗
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const int32 - 图层名称
	* @param InFloorWindow - const const TSharedPtr<class "FArmyFloorWindow> - 落地窗
	* @return bool - true 表示绘制成功
	*/
	bool DrawFloorWindow(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyFloorWindow> InFloorWindow);
	bool DrawFloorWindow2(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyFloorWindow> InFloorWindow);
	/**
	* 在指定的图框指定的图层上绘矩形飘窗
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const int32 - 图层名称
	* @param InRectBayWindow - const const TSharedPtr<class "FArmyRectBayWindow> - 矩形飘窗
	* @return bool - true 表示绘制成功
	*/
	bool DrawRectBayWindow(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyRectBayWindow> InRectBayWindow);
	bool DrawRectBayWindow2(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyRectBayWindow> InRectBayWindow);
	/**
	* 在指定的图框指定的图层上绘制梯形飘窗
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const int32 - 图层名称
	* @param InTrapeBayWindow - const const TSharedPtr<class "FArmyTrapeBayWindow> - 梯形飘窗
	* @return bool - true 表示绘制成功
	*/
	bool DrawTrapeBayWindow(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyTrapeBayWindow> InTrapeBayWindow);
	bool DrawTrapeBayWindow2(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyTrapeBayWindow> InTrapeBayWindow);
	/**
	* 在指定的图框指定的图层上绘制拐角飘窗
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const int32 - 图层名称
	* @param InCornerBayWindow - const const TSharedPtr<class "FArmyCornerBayWindow> - 拐角飘窗
	* @return bool - true 表示绘制成功
	*/
	bool DrawCornerBayWindow(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyCornerBayWindow> InCornerBayWindow);
	bool DrawCornerBayWindow2(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyCornerBayWindow> InCornerBayWindow);
	/**
	* 在指定的图框指定的图层上绘制柱子
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const int32 - 图层名称
	* @param InPillar - const TSharedPtr<"FArmyPillar>  - 柱子
	* @return bool - true 表示绘制成功
	*/
	bool DrawPillar(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyPillar> InPillar);

	/**
	* 在指定的图框指定的图层上绘制顶面梁
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const int32 - 图层名称
	* @param InBeam - const TSharedPtr<"FArmyBeam>   - 梁
	* @return bool - true 表示绘制成功
	*/
	bool DrawBeam(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyBeam> InBeam);
	
	/**
	* 在指定的图框指定的图层上绘制风道
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const int32 - 图层名称
	* @param InAirFlue - const TSharedPtr<"FArmyAirFlue>  - 风道
	* @return bool - true 表示绘制成功
	*/
	bool DrawAirFlue(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyAirFlue>  InAirFlue);

	/**
	* 在指定的图框指定的图层上绘制标准门
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const int32 - 图层名称
	* @param InSingleDoor - const TSharedPtr<class "FArmySingleDoor>  - 标准门
	* @return bool - true 表示绘制成功
	*/
	bool DrawSingleDoor(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmySingleDoor> InSingleDoor);
	bool DrawSingleDoor2(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmySingleDoor> InSingleDoor);
	/**
	* 在指定的图框指定的图层上绘制推拉门
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const int32 - 图层名称
	* @param InSlidingDoor - const TSharedPtr<class "FArmySlidingDoor>  - 推拉门
	* @return bool - true 表示绘制成功
	*/
	bool DrawSlidingDoor(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmySlidingDoor> InSlidingDoor);
	bool DrawSlidingDoor2(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmySlidingDoor> InSlidingDoor);
	/**
	* 在指定的图框指定的图层上绘制入户门
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const int32 - 图层名称
	* @param InSecurityDoor - const TSharedPtr<class "FArmySecurityDoor>  - 入户门
	* @return bool - true 表示绘制成功
	*/
	bool DrawSecurityDoor(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmySecurityDoor> InSecurityDoor);
	bool DrawSecurityDoor2(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmySecurityDoor> InSecurityDoor);
	/**
	 * 在指定的图框指定的图层上绘制实体填充 
	 * @param FrameIndex - const int32 - 图框名称
	 * @param LayerName - const FString - 图层名称
	 * @param Polygon - const TSharedPtr<"FArmyPolygon>  & - 填充线段
	 * @return bool -true	表示绘制成功
	 */
	bool DrawHatchSolid(const int32 FrameIndex,const FString LayerName, const TSharedPtr<"FArmyPolygon> &Polygon);
	
	/**
	 * 在指定的图框指定的图层上绘制样式填充
	 * @param FrameIndex - const int32 -   图框名称
	 * @param LayerName - const FString - 图层名称
	 * @param Style - const FString & -		 填充风格
	 * @param Line - TArray<"FArmyLine> & -  填充线段
	 * @param color - FColor - 绘制颜色
	 * @return bool -	true 表示绘制成功
	*/
	bool DrawHatch(const int32 FrameIndex, const FString LayerName,const FString&Style, double scale, const TSharedPtr<"FArmyPolygon> &Polygon);


	/**
	 * 在指定的图框指定的图层上画线形成块
	 * @param FrameIndex - const int32 -  图框名称
	 * @param LayerName - const FString - 图层名称
	 * @param Lines - TArray<TSharedPtr<"FArmyLine>> & - 
	 * @return bool - 
	 */
	bool DrawLinesAsBlock(const int32 FrameIndex, const FString LayerName,TArray<TSharedPtr<"FArmyLine>> &Lines);

	/**
	* 在指定的图框指定的图层上绘制样式填充
	* @param FrameIndex - const int32 -   图框名称
	* @param LayerName - const FString - 图层名称
	* @param Style - const FString & -		 填充风格
	* @param PolygonList - const TArray<TSharedPtr<"FArmyPolygon>> & -  填充线段
	* @return bool -	true 表示绘制成功
	*/
	bool DrawHatch(const int32 FrameIndex, const FString LayerName, const FString&Style,double scale, const TArray<TSharedPtr<"FArmyPolygon>> PolygonList);

	/**
	* 在指定的图框指定的图层上绘制标注
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const int32 - 图层名称
	* @param InDimensions - const TSharedPtr<class "FArmyDimensions>  - 标注
	* @return bool - true 表示绘制成功
	*/
	bool DrawDimensions(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyDimensions> InDimensions);

	/**
	* 在指定的图框指定的图层上绘制文本
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const int32 - 图层名称
	* @param InTextLabel - const TSharedPtr<class "FArmyTextLabel>  - 文本
	* @return bool - true 表示绘制成功
	*/
	bool DrawTextLabel(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyTextLabel> InTextLabel);
	
	/**
	* 在指定的图框指定的图层上绘制手动标注
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const int32 - 图层名称
	* @param InDownLeadLabel - const TSharedPtr<class "FArmyDownLeadLabel>  - 文本
	* @return bool - true 表示绘制成功
	*/
	bool DrawDownLeadLabel(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyDownLeadLabel> InDownLeadLabel);

	/**
	* 在指定的图框指定的图层上绘制指北针
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const FString - 图层名称
	* @param ComPass -  const TSharedPtr<class "FArmyCompass> - 指北针对象
	* @return bool -  true 表示绘制成功
	*/
	bool DrawCompass(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyCompass> ComPass);

	/**
	* 在指定的图框指定的图层上绘制入户门标志
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const FString - 图层名称
	* @param Entrance -  const TSharedPtr<class "FArmyEntrance> - 指北针对象
	* @return bool -  true 表示绘制成功
	*/
	bool DrawEntrance(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyEntrance> Entrance);

	/**
	* 在指定的图框指定的图层上绘制台面割线
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const FString - 图层名称
	* @param BoardSplitline -  const TSharedPtr<class "FArmyBoardSplitline> - 分割线
	* @return bool -  true 表示绘制成功
	*/
	bool DrawBoardSplitline(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyBoardSplitline> BoardSplitline);

	/**
	* 在指定的图框指定的图层上绘制包立管
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const FString - 图层名称
	* @param LampControlLines -  const TSharedPtr<class "FArmyPackPipe> - 爆裂管
	* @return bool -  true 表示绘制成功
	*/

	bool DrawPackPipe(const int32 FrameIndex, const FString LayerName, const TSharedPtr <class "FArmyPackPipe>PackPipe);

	/**
	* 在指定的图框指定的图层上绘制垭口包边
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const FString - 图层名称
	* @param LampControlLines -  const TSharedPtr<class "FArmyPackPipe> - 爆裂管
	* @return bool -  true 表示绘制成功
	*/
	bool DrawPassEdge(const int32 FrameIndex, const FString LayerName, const TSharedPtr <class "FArmyPass>Pass);
	/**
	* 在指定的图框指定的图层上绘制新垭口包边
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const FString - 图层名称
	* @param LampControlLines -  const TSharedPtr<class "FArmyPackPipe> - 爆裂管
	* @return bool -  true 表示绘制成功
	*/
	bool DrawNewPassEdge(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyNewPass> NewPass);


	/**
	* 在指定的图框指定的图层上绘制灯控线
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const FString - 图层名称
	* @param LampControlLines -  const TSharedPtr<class "FArmyLampControlLines> - 分割线
	* @return bool -  true 表示绘制成功DrawComBlockWithoutRef
	*/
	bool DrawLampControlLines(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyLampControlLines> LampControlLines);

	/**
	* 在指定的图框指定的图层上绘制开阳台
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const FString - 图层名称
	* @param InPunch -  const TSharedPtr<class "FArmyPunch> - 开阳台
	* @return bool -  true 表示绘制成功DrawComBlockWithoutRef
	*/
	bool DrawPunch(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyPunch> InPunch);

	/**
	* 在指定的图框指定的图层上厨柜索引标志
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const int32 - 图层名称
	* @param InAirFlue - const TSharedPtr<"FArmySymbol>  - 索引
	* @return bool - true 表示绘制成功
	*/
	bool DrawSymbol(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmySymbol>  InSymbol);

	/**
	* 在指定的图框指定的图层上绘制柜体
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const int32 - 图层名称
	* @param InAirFlue - const TSharedPtr<"FArmySymbol>  - 索引
	* @return bool - true 表示绘制成功
	*/
	bool DrawCabinet(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyFurniture>  CabinetFurniture);


	/**
	* 在指定的图框指定的图层上绘制自定义封板
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const int32 - 图层名称
	* @param CustomDefine - const TSharedPtr<"FArmyCustomDefine>  - 自定义
	* @return bool - true 表示绘制成功
	*/
	bool DrawCustomDefine(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyCustomDefine>  CustomDefine);

	/**
	* 在指定的图框指定的图层上绘制橱柜上面的构件
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const int32 - 图层名称
	* @param ComponentInfo - const FComponentInfo  - 自定义
	* @return bool - true 表示绘制成功
	*/
	bool DrawWHCComponent(const int32 FrameIndex, const FString LayerName, "FArmyWHCTableObject::FComponentInfo  ComponentInfo);

	/**
	* 在指定的图框指定的图层上绘制橱柜的门板
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const int32 - 图层名称
	* @param DoorSheet -  TSharedPtr<class "FArmyWHCDoorSheet>  - 门板
	* @return bool - true 表示绘制成功
	*/
	bool DrawDoorSheet(const int32 FrameIndex, const FString LayerName, TSharedPtr<class "FArmyWHCDoorSheet> DoorSheet);


	// 保存文件
	bool Save(const FString& FileName);

	// 测试
	bool TestCad(const int32 FrameIndex, const FString LayerName);

private:
 
	// 绘制通用块
	bool DrawComBlock(const int32 FrameIndex, const FString LayerName, const TSharedPtr<class "FArmyCadBundle> &CadBounle);
	//添加可能重复块,不引用
	bool  DrawComBlockWithoutRef(const TSharedPtr<class "FArmyCadBundle> &CadBounle,BlockExt&blockExt);
	//添加引用
	bool  DrawComBlockRef(const int32 FrameIndex, const FString LayerName, const FString&BlockName, const Double4 &offset, const float  Angle);
	//引用重复的块
	bool	AddRealTimeBlock(const TSharedPtr<class "FArmyCadBundle> &CadBounle, const FVector&offsetW, const FString	&BlockName);
	bool	DrawRealTimeBlock(const int32 FrameIndex, const FString LayerName, const FString&BlockName, const FVector &offset, const float  Angle);
	// 绘制窗户通用方法
	bool DrawComWindow(const int32 FrameIndex, const FString LayerName, const TArray< TSharedPtr<"FArmyLine>> LineArray);
	 
	 
	/**
	* 在指定的图框指定的图层上绘制文字
	* @param FrameIndex - const int32 - 图框名称
	* @param LayerName - const FString - 图层名称
	* @param AlignmentPoint - const FVector & - Box 中心
	* @param Text - const FString & - 文字
	* @param TextHeight - float - 文字高度
	* @param BoxWidth - float -Box的宽度
	* @param Angle - float -	角度
	* @param isLeftToRight - bool - 是不是文字从左到右，否则从上到下
	* @return bool - 表示绘制成功
	*/
	bool DrawMultipleText(const int32 FrameIndex, const FString LayerName, const FVector&AlignmentPoint, const FString&Text, float TextHeight, float BoxWidth = 1, float Angle = 0, bool isLeftToRight = true);
	/**
	* 在指定的图框指定的图层上绘制标注
	* @param FrameIndex - const int32 - - 图框名称
	* @param LayerName - const FString - 图层名称
	* @param MesurementStart - const FVector & -  测量点1
	* @param MesureEnd - const FVector & -		   测量点2
	* @param LinePostion - const FVector & -	   标注点
	* @return bool - true 表示绘制成功
	*/
	bool DrawAlignedDimension(const int32 FrameIndex, const FString LayerName, const FVector&MesureStart, const FVector&MesureEnd, const FVector &LinePostion);
private:
	//清理所有图块
	void ClearBlocks();
	// 初始化
	void InitializeArmyAutoCad();
	// 初始化块计数器
	void InitIncrementBlockCounter();
	// 更计数器
	void UpdateIncrementBlockCounter(EIncrementBlockType IncrementBlockType);
	// 获得下一个块的计数值
	int32 GetBlockValue(EIncrementBlockType IncrementBlockType);
	// UTF-8转GBK
	std::string Utf82Gbk(const char*utf8);
	// 根据图框进行坐标装换方法
	Double4 GetCadPosOfFrame(const FVector &SrcVector,int32 FrameIndex);
	// 进行坐标装换方法
	Double4 GetCadPos(const FVector &SrcVector);
	// 进行坐标装换方法
	FVector GetCadVector(const FVector &SrcVector);
	// 根据图框进行坐标装换方法
	FVector GetCadVectorOfFrame(const FVector &SrcVector, int32 FrameIndex);
	// 获得CAD长度
	float GetCadLength(const float SrcLength);
	//矫正String
	std::string CorrectString(const std::string&string);
	//获取AABB
	FBox CalculateBoxMin(const TSharedPtr<class "FArmyConstructionFrame>& frame);
	// 是否可以进行绘制
	bool CheckDraw(int32 FrameIndex,FString LayerName);
	// 获得通用绘制属性
	AttributeDesc GetComAttributeDesc(FString LayerName);
	// 获得绘制属性
	AttributeDesc GetAttributeDescByLayer(TSharedPtr<LayerInfo> InLayer);
	// 计算图框偏移绘制
	float GetFrameOffset(int32 FrameIndex);
	//获取圆弧参数,返回X  起始角度 ，Y终止角度 (0-2*pi)
	FVector  CalculateArcAngular(const FVector&centre,const FVector&startPoint,const FVector&endPoint);
	// 初始化系统标尺块
	void InitSystemBlock();
	// 通过线段数组获得多边形线段
	TSharedPtr<class "FArmyPolyline> GetPolylineByLines(TArray<TSharedPtr<class "FArmyLine>> InLineArray);
	//获取普通窗Key
	FString	 CalculateWindowKey(const TSharedPtr<class "FArmyWindow>&window);
	//获取落地窗Key
	FString CalculateFloorWindowKey(const TSharedPtr<class "FArmyFloorWindow>&window);
	//获取矩形飘窗Key
	FString CalculateRectBayWindowKey(const TSharedPtr<class "FArmyRectBayWindow>&window);
	//获取梯形飘窗Key
	FString CalculateTrapeBayWindowKey(const TSharedPtr<class "FArmyTrapeBayWindow>&window);
	//获取角窗Key
	FString CalculateCornerBayWindowKey(const TSharedPtr<class "FArmyCornerBayWindow>&window);
	//获取普通门Key
	FString CalculateSingleDoorKey(const TSharedPtr<class "FArmySingleDoor> door);
	//获取推拉门Key
	FString CalculateSlidingDoorKey(const TSharedPtr<class "FArmySlidingDoor> door);
	//计算安全门Key
	FString CalculateSecurityDoorKey(const TSharedPtr<class "FArmySecurityDoor> door);
	//计算普通窗偏移
	FVector CalculateWindowOffset(const TSharedPtr<class "FArmyWindow>&window);
	//计算落地窗偏移
	FVector CalculateFloorWindowOffset(const TSharedPtr<class "FArmyFloorWindow>&window);
	//计算矩形飘窗偏移
	FVector CalculateRectBayWindowOffset(const TSharedPtr<class "FArmyRectBayWindow>&window);
	//计算梯形飘窗偏移
	FVector CalculateTrapeBayWindowOffset(const TSharedPtr<class "FArmyTrapeBayWindow>&window);
	//计算角窗偏移
	FVector CalculateCornerBayWindowOffset(const TSharedPtr<class "FArmyCornerBayWindow>&window);
	//获取普通门偏移
	FVector CalculateSingleDoorOffset(const TSharedPtr<class "FArmySingleDoor> door);
	//获取推拉门偏移
	FVector CalculateSlidingDoorOffset(const TSharedPtr<class "FArmySlidingDoor> door);
	//获取安全门偏移
	FVector CalculateSecurityDoorOffset(const TSharedPtr<class "FArmySecurityDoor> door);
	//计算落地窗旋转角度
	float	 CalculateWindowAngle(const TSharedPtr<class "FArmyWindow>&window);
	//计算普通窗旋转角度
	float	 CalculateFloorWindowAngle(const TSharedPtr<class "FArmyFloorWindow>&window);
	//计算矩形飘窗旋转角度
	float	 CalculateRectBayWindowAngle(const TSharedPtr<class "FArmyRectBayWindow>&window);
	//计算梯形飘窗旋转角度
	float	 CalculateTrapeBayWindowAngle(const TSharedPtr<class "FArmyTrapeBayWindow>&window);
	//计算角窗旋转角度
	float	 CalculateCornerBayWindowAngle(const TSharedPtr<class "FArmyCornerBayWindow>&window);
	//计算普通门旋转角度
	float CalculateSingleDoorAngle(const TSharedPtr<class "FArmySingleDoor> door);
	//计算推拉门旋转角度
	float CalculateSlidingDoorAngle(const TSharedPtr<class "FArmySlidingDoor> door);
	//计算安全门旋转角度
	float CalculateSecurityDoorAngle(const TSharedPtr<class "FArmySecurityDoor> door);
	// 构件几何Shape 到 CadBundle
	template<class Type>
	TSharedPtr<"FArmyCadBundle>	GetCadBounleByWindows(Type&type, FVector offset, float Angle);
	template<class Type>
	TSharedPtr<"FArmyCadBundle>	GetCadBounleByDoor(Type&type, FVector offset, float Angle);
 
private:
	// 图层中间间隔距离
	float FrameStepLength;
	// 当前图层名称
	std::string CurrentLayerName;
	// 当前图层名称
	std::string CurrentLayerType;
	// 图层管理器
	TMap<FString, bool>		LayerManager;
	// 图框管理器
	TMap<int, BlockExt>			FrameManager;
	// 构件管理器
	TMap<int32, BlockExt>		ComponentManager;
	// 家具管理器
	TMap<int32, BlockExt>		FurnitureManager;
	// 管理记录功能
	TSet<int32>					BlockRecord;
	// 临时块
	TArray<IBlock*>				TempBlock;
	//接口
	IFactory 							*EntityFactory;
	//画布
	IBlock								*Canvas;
	// 图块计数器
	TMap<EIncrementBlockType, int32> IncrementBlockCounterMap;
	// 默认颜色
	FColor DefaultColor;
	// 系统设置
	TMap<FString, IBlock*> SysBlockMap;
    // 自定义块管理
	TMap<EDefineBlockType, BlockExt>		DefineBlockMap;
	//重复块
	TMap<FString, BlockExt>  RepeatBlock;
};

template<class Type>
TSharedPtr<"FArmyCadBundle>	"FArmyAutoCad::GetCadBounleByWindows(Type&type, FVector offset, float Angle)
{
	TArray<TSharedPtr<"FArmyLine>> LineArray;
	TArray<TPair<FVector, FVector>> ObjLineList;
	type->GetCadLineArray(ObjLineList);
	for (TPair<FVector, FVector> ObjLineIt : ObjLineList)
	{
		//旋转到0度	 
		FRotator Rotator = FRotator(0, Angle*180.f / M_PI, 0);
		ObjLineIt.Key -= offset;
		ObjLineIt.Value -= offset;
		ObjLineIt.Key = Rotator.RotateVector(ObjLineIt.Key);
		ObjLineIt.Value = Rotator.RotateVector(ObjLineIt.Value);
		LineArray.Emplace(MakeShareable(new "FArmyLine(ObjLineIt.Key, ObjLineIt.Value)));
	}
	TSharedPtr<"FArmyCadBundle> Boundle = MakeShareable(new "FArmyCadBundle);
	for (auto It : LineArray)
	{
		Boundle->AddLine(It);
	}
	return Boundle;
}
template<class Type>
TSharedPtr<"FArmyCadBundle>	"FArmyAutoCad::GetCadBounleByDoor(Type&type, FVector offset, float Angle)
{
	TArray<TSharedPtr<"FArmyLine>> LineArray;
	TArray<TPair<FVector, FVector>> ObjLineList;
	type->GetCadLineArray(ObjLineList);
	FRotator Rotator = FRotator(0, Angle*180.f/M_PI, 0);
	for (TPair<FVector, FVector> ObjLineIt : ObjLineList)
	{
		//旋转到0度	 
		ObjLineIt.Key -= offset;
		ObjLineIt.Value -= offset;
		ObjLineIt.Key = Rotator.RotateVector(ObjLineIt.Key);
		ObjLineIt.Value = Rotator.RotateVector(ObjLineIt.Value);
		LineArray.Emplace(MakeShareable(new "FArmyLine(ObjLineIt.Key, ObjLineIt.Value)));
	}


	TSharedPtr<"FArmyCadBundle> Boundle = MakeShareable(new "FArmyCadBundle);
	for (auto It : LineArray)
	{
		Boundle->AddLine(It);
	}

	TSharedPtr<"FArmyArcLine> DoorArc = MakeShareable(new "FArmyArcLine());
	TSharedPtr<"FArmyArcLine> ArcTemp = type->GetDoorArcLine();

	DoorArc->SetClockwise(ArcTemp->IsClockwise());

	FVector Beg = ArcTemp->GetStartPos();
	FVector End = ArcTemp->GetEndPos();
	FVector Mid = ArcTemp->GetPosition();
	Mid -= offset;
	Beg -= offset;
	End -= offset;
	Mid = Rotator.RotateVector(Mid);
	Beg = Rotator.RotateVector(Beg);
	End = Rotator.RotateVector(End);
	DoorArc->SetRadius(FVector::Dist2D(Beg, Mid));
	DoorArc->SetCadPos(Beg, End, Mid);
	if (DoorArc.IsValid())
	{
		Boundle->AddArcLine(DoorArc, "DASHED2");
	}
	return Boundle;
}