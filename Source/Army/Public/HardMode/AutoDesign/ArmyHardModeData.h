#pragma once

#include "CoreMinimal.h"
#include "ArmyCommonTypes.h"
#include "ArmyRoomSpaceArea.h"
#include "ArmyModelEntity.h"

#include "ArmyContinueStyle.h"
#include "ArmyWhirlwindStyle.h"
#include "ArmyIntervalStyle.h"
#include "ArmySeamlessStyle.h"
#include "ArmyCrossStyle.h"
#include "ArmyWorkerStyle.h"
#include "ArmyContinueStyle.h"
#include "ArmyHerringBoneStyle.h"
#include "ArmyTrapezoidStyle.h"

//瓷砖铺贴样式
struct TilePasteStyle
{
	EStyleType EdityType = S_SeamlessStyle;
	AlignmentType M_AlignmentType = LeftTop;
	float InternalDist;
	float XDirOffset;
	float YDirOffset;
	float RotationAngle;
	float HeightToFloor;
	FLinearColor GapColor;
	FLinearColor WallPaintColor;
	FString ColorPaintValueKey;
	bool IsHaveWallPaintColor;

	TilePasteStyle() :
		EdityType(S_SeamlessStyle),
		M_AlignmentType(LeftTop),
		InternalDist(0.f),
		XDirOffset(0.f),
		YDirOffset(0.f),
		RotationAngle(0.f),
		HeightToFloor(0.f),
		GapColor(FLinearColor(0.3f, 0.3f, 0.3f, 1.f)),
		WallPaintColor(FLinearColor(1.f, 1.f, 1.f, 1.f)),
		IsHaveWallPaintColor(true)
	{

	}

	// 序列化为josn数据
	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter, const TilePasteStyle& InStyle);
	// 反序列化模型json数据
	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);
};
// 
struct FModellingHelpAreaInfo
{
	TilePasteStyle DesignStyle;
	TSharedPtr<class FArmyModelEntity> ModellingMaterial;
};
// 单个区域造型（基础面不会有此结构）
struct FModellingAreaInfo
{
	uint8 AreaType;					  //0-矩形，1-圆形，2-规则多边形，3-自由多边形
	uint8 MaterialType;				  //材质样式：0-壁纸，1-瓷砖
	TilePasteStyle DesignStyle;		  //只有瓷砖才有此属性
	TArray<FVector> ModellingVertices;//造型墙上的顶点信息（不包括基础墙面的点）
	FVector LeftUpCornerPoint;		  //只有矩形有此信息
	float AreaWidth = 0.f;				  //同上
	float AreaHeight = 0.f;			      //同上
	FVector Center;				      //只有圆有此信息
	float Radius = 0.f;					  //同上
	int32 NumSide;						//只有多边形有此信息
	FVector Position;
	FVector StartPoint;
	float ExtrusionHeight = 0.f;			  //下吊高度（只有吊顶有此信息）
	TSharedPtr<class FArmyModelEntity> ModellingMaterial;//造型面的材质
	TSharedPtr<class FArmyModelEntity> LampSlotActor;//造型面的灯带
	TSharedPtr<class FArmyModelEntity> ModelingLine;//造型线

	TArray<FModellingHelpAreaInfo>  ModellingHelpAreas; // 辅助侧面

	bool bHasLampWall = false;//造型上是否有壁灯
	FVector LampWallPos;//壁灯基于造型面的相对坐标
	FVector LampWallWorldPos;//世界坐标
	FVector LampWallNormal;//方向
};

// 单面造型墙
struct FModellingWall
{
	uint8 WallType;							//0:空造型，1:有造型（后期可能会扩展类型）
	uint32 ModellingWallType;				//0:普通铺贴，1:卧室床头，2:卧室床位，3:客厅电视柜后面，4:靠近沙发后面
	uint8 MaterialType;						//基础墙面材质样式：0-壁纸，1-瓷砖
	TilePasteStyle DesignStyle;				//只有瓷砖才有此属性
	TSharedPtr<class FArmyModelEntity> BaseWallMaterial;//基础墙面的材质
	float BaseAreaWidth;					//基础面的宽度（为了计算造型面的缩放比例）
	float BaseAreaHeight;
	TArray<FModellingAreaInfo> SpaceAreaList;
};
// 单屋顶面造型
struct FModellingRoof
{
	uint8 MaterialType;				  //材质样式：0-壁纸，1-瓷砖
	TilePasteStyle DesignStyle;		  //瓷砖铺贴样式
	TSharedPtr<class FArmyModelEntity> BaseRoofMaterial;//基础面的材质
	TSharedPtr<class FArmyModelEntity> BaseRoofSkirtingLine;//顶脚线
	float ExtrusionHeight = 0;//基础面下吊高度
	uint8 WallType;							//0:空造型，1:有造型
	TArray<FModellingAreaInfo> SpaceAreaList;//造型列表
	float BaseAreaWidth;					//基础面的宽度（为了计算造型面的缩放比例）
	float BaseAreaHeight;
};
// zhx Use Store Buckle ContentItem And VertorCode
struct BuckleCode
{
	TSharedPtr<FContentItemSpace::FContentItem> ContentItem;
	FString UniqueCodeString;
	BuckleCode(TSharedPtr<FContentItemSpace::FContentItem> _ContentItem, FString _UniqueCodeString)
	{
		ContentItem = _ContentItem;
		UniqueCodeString = _UniqueCodeString;
	}
};
// 单屋地面造型
struct FModellingFloor
{
	TilePasteStyle DesignStyle;

	TSharedPtr<class FArmyModelEntity> BaseFloorMaterial;//基础地面的材质

	TArray<TArray<FVector>> ModellingVertices;//地面上的顶点信息（不包括基础地面的点）
	TArray<TSharedPtr<class FArmyModelEntity>> ModellingMaterialList;
	//TArray<FAreaInfo> AreaInfoList;//每个多边形的铺贴信息列表（与上面的数组顺序一一对应）
	TSharedPtr<class FArmyModelEntity> BaseFloorSkirtingLine;//踢脚线
};

struct FWindowStone
{
	TSharedPtr<class FArmyModelEntity> StoneMaterial;
	ERoomType RoomType;
};
struct FBridgeStone
{
	TSharedPtr<class FArmyModelEntity> StoneMaterial;
};
struct FModellingBuckle
{
	TSharedPtr<class FArmyModelEntity> BuckleMaterial;
};
struct FPassModel
{
	TSharedPtr<class FArmyModelEntity> PassMaterial;
};
class XR_API FArmyHardwareData : public TSharedFromThis<FArmyHardwareData>
{
public:
	FArmyHardwareData();

	FArmyHardwareData(TArray<TSharedPtr<FArmyModelEntity>> InHardModeArray);
	// 序列化为josn数据
	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
	// 反序列化模型json数据
	void Deserialization(const TArray<TSharedPtr<FArmyModelEntity>> & HardArray);

	TArray<TSharedPtr<FWindowStone>> WindowStoneList;
	TArray<TSharedPtr<FBridgeStone>> BridgeStoneList;
	TArray<TSharedPtr<FPassModel>> PassModelList;

	TArray<TSharedPtr<class FArmyModelEntity>> GetHardModelList();

	TArray<TSharedPtr<FArmyModelEntity>> HardModeArray;
	TSharedPtr<FArmyModelEntity> GetModelEntityByType(EHardModeType hardModeType);

};

class XR_API FArmyHardModeData : public TSharedFromThis<FArmyHardModeData>
{
public:
	FArmyHardModeData();
	FArmyHardModeData(TSharedPtr<class FArmyRoomEntity> RoomEntity);
	~FArmyHardModeData();
	static bool ModelEntityIsValid(TSharedPtr<class FArmyModelEntity> ModelEntity);

	// 获得硬装模型列表
	TArray<TSharedPtr<class FArmyModelEntity>> GetHardModelList();
	TArray<TSharedPtr<class FArmyModelEntity>> GetHardSimpleModelList();

	// 获得墙面对象
	TArray<TSharedPtr<FModellingWall>> GetModellingWallList();
	TSharedPtr<FModellingWall> GetModellingWallByType(int type);
	// 获得地面对象
	TSharedPtr<FModellingFloor> GetModellingFloor();
	//获得顶面对象
	TSharedPtr<FModellingRoof> GetModellingRoof();
	// 获得扣条对象
	TSharedPtr<FModellingBuckle> GetModellingBuckle();
	// 序列化为josn数据
	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
	// 反序列化模型json数据
	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData,const TArray<TSharedPtr<FArmyModelEntity>> & HardArray);
	// 获得顶角线高度，自身高度
	float GetCrownMouldingHeight();
	// 获得吊顶下吊，自身高度
	float GetCeilingDropOff();

	void GetRoomSpaceListFromBed(TMap<int,TSharedPtr<FArmyRoomSpaceArea>>& OutRoomSpaceList, TSharedPtr<FArmyRoomEntity>& InRoom);

	TSharedPtr<FArmyRoomSpaceArea> GetRoomSpaceFromSofaOrTVBench(TSharedPtr<FArmyRoomEntity>& InRoom, EAIComponentCode InCode);

	static void ExtractMaximumRectangleInPolygonSimply(const TArray<FVector>&Polygon, FVector & Center, FBox & box);
private:
	// 墙面数据
	TArray<TSharedPtr<FModellingWall>> ModellingWallList;
	// 地面数据
	TSharedPtr<FModellingFloor> ModellingFloor;
	// 顶面数据
	TSharedPtr<FModellingRoof> ModellingRoof;
	// 扣条
	TSharedPtr<FModellingBuckle> ModellingBuckle;

	TArray<TSharedPtr<FArmyModelEntity>> HardModeArray;
	TSharedPtr<FArmyModelEntity> GetModelEntityByType(EHardModeType hardModeType);

	void AddUniqueModelEntity(TArray<TSharedPtr<FArmyModelEntity>> & Array,TSharedPtr<FArmyModelEntity> Entity);
};
