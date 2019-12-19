#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArmyCommonTypes.h"
#include "ArmyAutoDesignSettings.h"
#include "ArmyModelEntity.h"
#include "ArmyRoomEntity.h"

#define   AI_MODEL_LEFT				FVector(-1.f,0.f,0.f)
#define   AI_MODEL_RIGHT			FVector(1.f,0.f,0.f)
#define   AI_MODEL_UP					FVector(0.f,0.f,1.f)
#define   AI_MODEL_DOWN			FVector(0.f,0.f,-1.f)
#define   AI_MODEL_FORWARD		FVector(0.f,1.f,0.f)
#define   AI_MODEL_BACKWARD	FVector(0.f,-1.f,0.f)
#define   MAX_RAY_LIMITATION	1000.f

struct ModelInfoStruct
{
	int32 AiCode;
	int32 AiId;
	int32 InnerType;
	ModelInfoStruct(int32 InAiCode, int32 InAiId,int32 InInnerType)
	{
		AiCode = InAiCode;
		AiId = InAiId;
		InnerType = InInnerType;
	}
};

class ARMYSCENEDATA_API FArmyDataTools
{
public:

	//方位
	enum EDirection
	{
		AI_DT_None = 0,
		AI_DT_MODEL_LEFT,
		AI_DT_MODEL_RIGHT,
		AI_DT_MODEL_UP,
		AI_DT_MODEL_DOWN,
		AI_DT_MODEL_FORWARD,
		AI_DT_MODEL_BACKWARD
	};

	// 初始化静态变量
	static void InitStaticData();

	/** 获取模型指定方向上一定距离的位置,目标模型方向与原始模型保持一致
	*
	* @param SrcModel - const AActor & -	原始Actor
	* @param Distance - float -				距离指定方向的长度
	* @param DestModel - AActor & -			目标Actor
	* @param InDir - EDirection -			指定的方向（上、下、左、右、前、后）
	* @param ModelType - int -				默认0
	* @return void -						无返回
	*/
	static void  ModelOnDirectionOfModel(const AActor& SrcModel, float Distance, AActor &DestModel, EDirection InDir, int ModelType = 0);

	///** 获取模型左边一定距离的位置,目标模型方向与原始模型保持一致 
	// * 
	// * @param SrcModel - const AActor & -	原始Actor
	// * @param Distance - float -						距离左边的长度
	// * @param DestModel - AActor & -			目标Actor
	// * @param ModelType - int -						默认0
	// * @return void -											无返回
	// */
	//static void  ModelOnLeftOfModel(const AActor& SrcModel, float Distance, AActor &DestModel, int ModelType = 0);

	///** 获取模型右边一定距离的位置,目标模型方向与原始模型保持一致  
	// * 
	// * @param SrcModel - const AActor & -	原始Actor
	// * @param Distance - float -						距离左边的长度
	// * @param DestModel - AActor & -			目标Actor
	// * @param ModelType - int -						默认0
	// * @return void -											无返回
	// */
	//static void ModelOnRightOfModel(const AActor& SrcModel, float Distance, AActor &DestModel, int ModelType = 0);

	///** 获取模型上边一定距离的位置,目标模型方向与原始模型保持一致
	//*
	//* @param SrcModel - const AActor & -	原始Actor
	//* @param Distance - float -						距离上边的长度
	//* @param DestModel - AActor & -			目标Actor
	//* @param ModelType - int -						默认0
	//* @return void -											无返回
	//*/
	//static  void ModelOnUpOfModel(const AActor&SrcModel, float Distance, AActor&DestModel, int ModelType = 0);

	///** 获取模型下边一定距离的位置,目标模型方向与原始模型保持一致
	//*
	//* @param SrcModel - const AActor & -		原始Actor
	//* @param Distance - float -						距离下边的长度
	//* @param DestModel - AActor & -			目标Actor
	//* @param ModelType - int -						默认0
	//* @return void -											无返回
	//*/
	//static  void ModelOnDownOfModel(const AActor&SrcModel, float Distance, AActor&DestModel, int ModelType = 0);

 //
	///** 获取模型前边一定距离的位置,目标模型方向与原始模型保持一致
	//*
	//* @param SrcModel - const AActor & -		原始Actor
	//* @param Distance - float -						距离前边的长度
	//* @param DestModel - AActor & -			目标Actor
	//* @param ModelType - int -						默认0
	//* @return void -											无返回
	//*/
	//static  void ModelOnForwardOfModel(const AActor&SrcModel, float Distance, AActor&DestModel, int ModelType = 0);

	///** 获取模型后边一定距离的位置,目标模型方向与原始模型保持一致 
	//*
	//* @param SrcModel - const AActor & -		原始Actor
	//* @param Distance - float -						距离后边的长度
	//* @param DestModel - AActor & -			目标Actor
	//* @param ModelType - int -						默认0
	//* @return void -											无返回
	//*/
	//static  void ModelOnBackwardOfModel(const AActor&SrcModel, float Distance, AActor&DestModel, int ModelType = 0);

	/**
	 * 获取模型指定方向上一定距离的位置
	 * @param SrcModel - const AActor & - 原始Actor
	 * @param Distance - float -		  距离指定方向的长度
	 * @param OutPos - FVector & -		  返回坐标位置
	 * @param InDir - EDirection -        指定的方向（上、下、左、右、前、后）
	 * @param ModelType - int - 
	 * @return void - 
	 */
	static void PosOnDirectionOfModel(const AActor& SrcModel, float Distance, FVector &OutPos, EDirection InDir, int ModelType = 0);

	///** 获取模型左面一定距离的位置 
	//*
	//* @param SrcModel - const AActor & -		原始Actor
	//* @param Distance - float -						距离左面的长度
	//* @param OutPos - FVector & -					引用坐标返回
	//* @return void -											无返回
	//*/
	//static void PosOnLeftOfModel(const AActor& SrcModel, float Distance, FVector &OutPos, int ModelType = 0);

	///** 获取模型右面一定距离的位置 
	//*
	//* @param SrcModel - const AActor & -		原始Actor
	//* @param Distance - float -						距离右面的长度
	//* @param OutPos - FVector & -					引用坐标返回
	//* @return void -											无返回
	//*/
	//static void PosOnRightOfModel(const AActor& SrcModel, float Distance, FVector &OutPos, int ModelType = 0);

	///** 获取模型上面一定距离的位置 
	//*
	//* @param SrcModel - const AActor & -		原始Actor
	//* @param Distance - float -						距离上面的长度
	//* @param OutPos - FVector & -					引用坐标返回
	//* @return void -											无返回
	//*/
	//static void PosOnUpOfModel(const AActor& SrcModel, float Distance, FVector &OutPos, int ModelType = 0);

	///** 获取模型下面一定距离的位置 
	//*
	//* @param SrcModel - const AActor & -		原始Actor
	//* @param Distance - float -						距离下面的长度
	//* @param OutPos - FVector & -					引用坐标返回
	//* @return void -											无返回
	//*/
	//static void PosOnDownOfModel(const AActor& SrcModel, float Distance, FVector &OutPos, int ModelType = 0);

	///** 获取模型前面一定距离的位置 
	//*
	//* @param SrcModel - const AActor & -		原始Actor
	//* @param Distance - float -						距离前面的长度
	//* @param OutPos - FVector & -					引用坐标返回
	//* @return void -											无返回
	//*/
	//static void PosOnForwardOfModel(const AActor& SrcModel, float Distance, FVector &OutPos, int ModelType = 0);
	//
	///** 获取模型后面一定距离的位置 
	//*
	//* @param SrcModel - const AActor & -		原始Actor
	//* @param Distance - float -						距离后面的长度
	//* @param OutPos - FVector & -					引用坐标返回
	//* @return void -											无返回
	//*/
	//static void PosOnBackwardOfModel(const AActor& SrcModel, float Distance, FVector &OutPos, int ModelType = 0);

 
	/**
	 *  通过模型在指定方向上获取墙体，（modelDir一般输入如下变量AI_MODEL_LEFT、AI_MODEL_RIGHT、AI_MODEL_FORWARD、AI_MODEL_BACKWARD）
	 * @param SrcModel - const AActor & -    目标模型
	 * @return TSharedPtr<class FArmyWallLine> -	返回WallLine，模型后面没有墙体 为NULL
	 */
	static  TSharedPtr<class FArmyWallLine> GetWallLineByModelInDir(const AActor&SrcModel, EDirection ModelDir = EDirection::AI_DT_MODEL_BACKWARD);
	 

	 
	/**
	 *  获取模型所在房间
	 * @param SrcModel - const AActor & -  模型
	 * @return TSharedPtr<class FArmyRoom> -返回房间，可以为NULL，表示模型没有处在合理空间内
	 */
	static  TSharedPtr<class FArmyRoom> GetRoomByModel(const AActor&SrcModel);

 
	/**
	 * 获取房间所有的模型
	 * @param Room - const TSharedPtr<class FArmyRoom> & -  目标房间
	 * @param OutArray - TArray<TSharedPtr<class FArmyModelEntity>> & -  输出的模型数组
	 * @return void - 
	 */
	static void  GetAllModelsByRoom(const TSharedPtr<class FArmyRoom>&Room,TArray<TSharedPtr<class FArmyModelEntity>> &OutArray);

	/** 转换 */
 
	/**
	 * 模型中线到指定方向墙体,离地一定距离的点
	 * @param SrcModel - const AActor & -   原始模型
	 * @param DistToGround - const float -   离地面高
	 * @param Out - FVector & -					  输出点位			
	 * @return bool -		错误，背面无墙体			
	 */
	static bool PointIntervalGroundOnDirProjPoint(const AActor&SrcModel, const float DistToGround, FVector&Out,EDirection InDir = AI_DT_MODEL_BACKWARD);

	/**  
	* 模型中线到背面墙体,离地一定距离的模型
	* @param SrcModel - const AActor & -   原始模型
	* @param DistToGround - const float -   离地面高
	* @param Out - FVector & -					  被移动的目标模型
	* @return bool -		错误，背面无墙体
	*/
	static bool ModelIntervalGroundOnBackProjPoint(const AActor&SrcModel, const float DistToGround, AActor&DestModel);

	 
 
	/**
	 * 模型中线到背面墙体左边一定距离.离地一定距离的点
	 * @param SrcModel - const AActor & -  原始模型
	 * @param DistToLeft - const float -		 模型左边长度
	 * @param DistToGround - const float - 离地高度
	 * @param Out - FVector & -					输出点位
	 * @return bool -					错误，背面无墙体
	 */
	static bool PointIntervalGroundOnLeftOfBackProjPoint(const AActor&SrcModel, const float DistToLeft, const float DistToGround, FVector &Out);

	/**
	* 模型中线到背面墙体左边一定距离.离地一定距离的模型
	* @param SrcModel - const AActor & -  原始模型
	* @param DistToLeft - const float -		 模型左边长度
	* @param DistToGround - const float - 离地高度
	* @param AActor - AActor & -				被移动的模型
	* @return bool -					错误，背面无墙体
	*/
	static bool ModelIntervalGroundOnLeftOfBackProjPoint(const AActor&SrcModel, const float DistToLeft, const float DistToGround, AActor&DestModel);

 
	/**
	 * *模型中线到背面墙体右边一定距离.离地一定距离的点
	 * @param SrcModel - const AActor & -  原始模型
	 * @param DistToLeft - const float -		  模型右边长度
	 * @param DistToGround - const float -  离地高度
	 * @param Out - FVector & -				    输出点位
	 * @return bool - 错误，背面无墙体
	 */
	static bool PointIntervalGroundOnRightOfBackProjPoint(const AActor&SrcModel, const float DistToRight, const float DistToGround,FVector &Out);

	/**
	* *模型中线到背面墙体右边一定距离.离地一定距离的模型
	* @param SrcModel - const AActor & -  原始模型
	* @param DistToLeft - const float -		  模型右边长度
	* @param DistToGround - const float -  被移动的模型
	* @param DestModel - AActor & -				    输出点位
	* @return bool - 错误，背面无墙体
	*/
	static bool ModelIntervalGroundOnRightOfBackProjPoint(const AActor&SrcModel, const float DistToLeft, const float DistToGround, AActor&DestModel);

	 
	/**
	 *   离地一定距离,离线段开始点一定距离的点(指向EndPoint)
	 * @param Line - const FArmyLine & -			线段
	 * @param distToGround - const float -		离地高度
	 * @param distToStartPoint - float -			线段开始点距离
	 * @return FVector -									输出点位
	 */
	static FVector PointIntervalGroundDistWithStartPoint(const FArmyLine&Line, const float distToGround, float distToStartPoint);

	/**
	*   离地一定距离,离线段开始点一定距离的点(指向EndPoint)
	* @param Line - const FArmyLine & -			线段
	* @param distToGround - const float -		离地高度
	* @param distToStartPoint - float -			线段开始点距离
	* @param actor - AActor -							被移动的模型
	* @return void 								 
	*/
	static void ModelIntervalGroundDistWithEndPoint(const FArmyLine&Line, const float distToGround, float distToStartPoint, AActor &actor);

	 
	/**
	 * 模型背面墙体的左方向，方向依靠墙体
	 * @param SrcModel - const AActor & -		传入模型
	 * @param diretion - FVector -						引用方向
	 * @return bool -												返回false 模型背面没有墙体
	 */
	static bool ModelProjPointOnWallLeftDirection(const AActor&SrcModel,FVector &diretion);

	/**
	* 模型背面墙体的右方向，方向依靠墙体
	* @param SrcModel - const AActor & -			传入模型
	* @param diretion - FVector -						引用方向
	* @return bool -												返回false 模型背面没有墙体
	*/
	static bool ModelProjPointOnWallRightDirection(const AActor&SrcModel, FVector &diretion);

 
	/**
	 * 获取墙体向内的法线,归一化后的
	 * @param Room - const TSharedPtr<FArmyRoom> & -			 目标房间
	 * @param WallLine - const TSharedPtr<FArmyWallLine> & -  目标的墙线 
	 * @return FVector -																 返回法线
	 */
	static FVector GetWallNormalInRoom(const TSharedPtr<FArmyRoom>&Room, const TSharedPtr<FArmyWallLine>&WallLine);

 
	/**
	 * 获取法线的左方向(实际为右面)
	 * @param Normal - const FVector & -   墙线的法线，向内
	 * @param WallLine - const TSharedPtr<FArmyWallLine> & -  输入墙线
	 * @return FVector -  输出左方向
	 */
	static FVector GetLeftDirectionByNormal(const FVector& Normal, const TSharedPtr<FArmyWallLine>&WallLine);

	/**
	* 获取法线的右方向(实际为右面)
	* @param Normal - const FVector & -							墙线的法线，向内
	* @param WallLine - const TSharedPtr<FArmyWallLine> & -  输入墙线
	* @return FVector -  输出左方向
	*/
	static FVector GetRightDirectionByNormal(const FVector& Normal, const TSharedPtr<FArmyWallLine>&WallLine);

	/** 门窗所在的的墙线*/
	static  TSharedPtr<class FArmyWallLine> GetWallLineByHardware(const TSharedPtr<class FArmyHardware> RefHardware, const TSharedPtr<class FArmyRoom> RefRoom);
	
	/** 开门后与门同侧的墙体（普通门）*/
	static TSharedPtr<class FArmyWallLine> GetSameSideWallByHardware(const TSharedPtr<class FArmyHardware> RefHardware, const TSharedPtr<class FArmyRoom> RefRoom, bool NormalDoor = true);
	
	/** 开门后与门不同侧的墙体（普通门）*/
	static  TSharedPtr<class FArmyWallLine> GetNotSameSideWallByHardware(const TSharedPtr<class FArmyHardware> RefHardware, const TSharedPtr<class FArmyRoom> RefRoom, bool NormalDoor = true);
	
	/** 开门后与门垂直的墙体（普通门）*/
	static  TSharedPtr<class FArmyWallLine> GetVerticalWallByHardware(const TSharedPtr<class FArmyHardware> RefHardware, const TSharedPtr<class FArmyRoom> RefRoom);
	
	/** 开门后与门背后的墙体（普通门）*/
	static  TSharedPtr<class FArmyWallLine> GetBackWallByHardware(const TSharedPtr<class FArmyHardware> RefHardware, const TSharedPtr<class FArmyRoom> RefRoom);
	
	/** 开门后与门背后的墙体（推拉门）*/
	static  TSharedPtr<class FArmyWallLine> GetLeftWallByHardware(const TSharedPtr<class FArmyHardware> RefHardware, const TSharedPtr<class FArmyRoom> RefRoom);
	
	/** 开门后与门背后的墙体（推拉门）*/
	static  TSharedPtr<class FArmyWallLine> GetRightWallByHardware(const TSharedPtr<class FArmyHardware> RefHardware, const TSharedPtr<class FArmyRoom> RefRoom);

	/** 获取实际大小  Long Width Height */
	static FVector GetContextItemSize(FContentItemPtr Pointer);

	/** 获取模型前向量，归一化*/
	static FVector GetModelForwardVector(AActor*actor);

	/** 获取模型左向量，归一化*/
	static FVector GetModelLeftVector(AActor*actor);
  
	/** 获得模型的构件编码*/
	static EComponentID GetComponentId(FContentItemPtr ContentItem);

	// 房间名称变换
	static ERoomType GetRoomType(int32 SpaceId);
	
	// 获得与门有关的房间
	static TArray<TSharedPtr<class FArmyRoom>> GetRoomsByHardware(TSharedPtr<class FArmyHardware> Hardware);

	// 获得当前户型的房间类型列表
	static TSet<int32> GetRoomTypeArray();

	// 计算Z轴坐标位置
	static float CalculateLocationZ(TSharedPtr<class FArmyModelEntity> Model);

	/**
	 * @ 计算模型包围盒与墙的最近距离
	 * @param InActor - AActor * - 输入要计算的模型
	 * @param InWallLine - TSharedPtr<FArmyWallLine> - 输入墙线
	 * @return float - 返回距离
	 */
	static float CalModelFromWallDistance(AActor * InActor,TSharedPtr<FArmyWallLine> InWallLine);

	/**
	* 通过房间获得模型类型-写颜值包使用
	* @param FArmyModelEntity - ModelEntity - 模型对象
	* @return int32 -0:硬装  1:软装 2：配饰  3：全局硬装  9其他 -1 无效
	*/
	static int32 GetLayoutModelType(TSharedPtr<class FArmyModelEntity> ModelEntity,int32 SpaceId);


	/**
	* @ 纪仁泽 通过点位在指定方向上获取墙体
	* @param InPoint - const FVector & - 目标点位
	* @param Limitattion - float & - 射线距离
	* @return TSharedPtr<class FArmyWallLine> -	返回WallLine，点位后面没有墙体 为NULL
	*/
	static  TSharedPtr<class FArmyWallLine> GetWallLineByPoint(const FVector& InPoint, const float& Limitattion, const TSharedPtr<FArmyRoomEntity>& InRoomEntity, EDirection ModelDir = EDirection::AI_DT_MODEL_BACKWARD);

	/**
	* 计算模型是否与房间内其他模型相交（不考虑墙上的门窗）
	* @param InActor - AActor * - 传入要检测的模型
	* @param InRoomTable - TSharedPtr<class FArmyRoomEntity> - 要检测的模型所在的房间
	* @param IgnoreActorArr - const TSet<AActor * > & - 传入需要忽略碰撞的模型列表
	* @return bool - true表示与其他模型碰撞，否则不碰撞
	*/
	static bool CalModelIntersectWithOtherUnWallModel(AActor * InActor, TSharedPtr<FArmyRoomEntity> InRoomTable, const TSet<AActor*> & IgnoreActorArr);


	/**
	* 计算模型是否与房间内其他模型相交（不考虑墙上的窗，考虑门）
	* @param InActor - AActor * - 传入要检测的模型
	* @param InRoomTable - TSharedPtr<class FArmyRoomEntity> - 要检测的模型所在的房间
	* @param IgnoreActorArr - const TSet<AActor * > & - 传入需要忽略碰撞的模型列表
	* @return bool - true表示与其他模型碰撞，否则不碰撞
	*/
	static bool CalModelIntersectWithOtherUnWindowsModel(AActor * InActor, TSharedPtr<FArmyRoomEntity> InRoomTable, const TSet<AActor*> & IgnoreActorArr);

    /**
	 * 计算模型是否去房间内其他模型相交
	 * @param InActor - AActor * - 传入要检测的模型
	 * @param InRoomTable - TSharedPtr<class FArmyRoomEntity> - 要检测的模型所在的房间
	 * @param IgnoreActorArr - const TSet<AActor * > & - 传入需要忽略碰撞的模型列表
	 * @return bool - true表示与其他模型碰撞，否则不碰撞
	 */
	static bool CalModelIntersectWithOtherModel(AActor * InActor, TSharedPtr<class FArmyRoomEntity> InRoomTable, const TSet<AActor*> & IgnoreActorArr);



	/**
	* @纪仁泽 计算模型能否调整到房间内
	* @param InModelEntity - TSharedPtr<FArmyModelEntity> - 输入要计算的模型
	* @param InRoomEntity - TSharedPtr<FArmyRoomEntity> - 房间
	* @return bool - 返回是否能够调整到房间内
	*/
	static bool CalModelAdjustToRoom(TSharedPtr<FArmyModelEntity>& InModelEntity,TSharedPtr<FArmyRoomEntity>& InRoomEntity);

	// 通过后台返回的两个aicode，形成新的aicode
	static int32 GetNewAiCode(int32 AiCode1, int32 AiCode2);
	// 通过商品id返回aicode
	static EAIComponentCode GetAiCodeByItemId(int32 ItemId);
	// 通过商品id返回新的位置信息
	static FVector GetNewLocationByAiCode(TSharedPtr<FArmyModelEntity>& InModelEntity);

	/* 获取多边形（各边必须是垂直或者水平的）中的面积最大矩形,Polygon中的点必须是有序的 @Hawk */
	static void GetMaxQuadInPolygon(const TArray<FVector>& Polygon, FVector& Center, TArray<FVector>& PointArr, FBox& Box);

	/** 柱子风道是否是组成房间的一部分
	* @(如果是,返回true)
	*/
	static bool IsObjectOfRoom(const TSharedPtr<FArmyRoom>&InRoom, const TSharedPtr<FArmyObject> InObj);

	/** 柱子风道包立管关联的空间
	* @(返回相关联的房间列表)
	*/
	static TArray<TSharedPtr<FArmyRoom>>  GetObjectRelateRooms(const TSharedPtr<FArmyObject>& InObj);


	static int32 BimLayoutIndex;
private:

	/**获取模型背部与墙体焦点
	*@(如果没有命中墙体,返回错误)
	*/
	static bool GetIntersectionPointBetweenModelWithWall(const AActor&SrcModel, FVector &Vector, EDirection InDir = AI_DT_MODEL_BACKWARD);

	/** 计算模型背部墙体,焦点,左方向
	* @(如果没有命中墙体,返回错误)
	*/
	static bool GetIntersectionPointWallLeftDirection(const AActor&SrcModel, FVector &Vector, TSharedPtr<FArmyWallLine>&Line, FVector &Direction);

	/** 计算模型背部墙体,焦点,右方向
	* @(如果没有命中墙体,返回错误)
	*/
	static bool GetIntersectionPointWallRightDirection(const AActor&SrcModel, FVector &Vector, TSharedPtr<FArmyWallLine>&Line, FVector &Direction);

	/**
	 * 枚举转化函数
	 * @param InEDir - EDirection - 输入枚举值
	 * @return FVector - 获取对应的方向向量
	 */
	static FVector GetVecByEDirection(EDirection InEDir);
	static TMap<int32, TArray<EAIComponentCode>> GlobalRoomCodeMap;
};