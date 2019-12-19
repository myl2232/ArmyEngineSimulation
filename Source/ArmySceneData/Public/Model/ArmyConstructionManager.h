/**
* Copyright 2019 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRConstructionManager.h
* @Description 施工项管理器，管理施工项的读写。每个施工项都有对应
*
* @Author 郭子阳
* @Date 2019年4月4日
* @Version 1.1
*/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Misc/Guid.h"
#include "Dom/JsonObject.h"

//枚举EConstructionType需要序列化
#include "ArmyConstructionManager.generated.h"

//施工项数据版本, 不一定和bim版本号一致
enum class EConstructionVersion
{
	V140=140, //1.4版本的施工项 ,这个版本的施工项没有参数,也不存在施工项管理器,施工项数据存放在HomeData的Map表中
	V150=150, //1.5版本的施工项， 从这个版本开始立面的完成面有了参数. 铺贴类增加了参数: 铺法PaveMethod、墙顶地EWallType
			  //将水电模式所有施工项加入

	V160=160, //1.6~V1.8版本施工项，  将立面原始面的施工项添加到Manager中管理，对下吊的顶面和未下吊的顶面做了区分,
			  //V1.7加入灯槽、窗台石施工项，
			  //在V1.8中加入门和哑口的施工项
	V190=190, //V1.9加入空间ID参数
};

//当前施工项数据版本号,不一定和bim版本号一致
const EConstructionVersion CurrentConstructionDataVersion = EConstructionVersion::V190;

//勾选信息
using CheckedData = TSharedPtr<class FArmyConstructionItemInterface>;
//施工项原始信息
using OriginalData = TSharedPtr<FJsonObject>;



//施工项类型
//！！！！对该枚举进行了序列化操作，请不要修改枚举名
UENUM()
enum class EConstructionType
{	
	All, //任何类型的施工项
	NormalGoods,//普通商品的施工项
	PureOrignalOffset, //原始点位纯移位施工项
	OriginalSurface, //立面模式原始面的施工项
	Pave, //铺贴类
	//SkirtingLine, //踢脚线
};



//用于查询施工项的参数
struct ARMYSCENEDATA_API ConstructionPatameters
{	
	bool operator  ==(const ConstructionPatameters & Orther) const
	{
		return IntParam1 == Orther.IntParam1
			&&IntParam2 == Orther.IntParam2
			&&IntParam3 == Orther.IntParam3
			&&IntParam4 == Orther.IntParam4
			//参数相同不考虑空间
			//&&SpaceIDParam == Orther.SpaceIDParam
			&&IntParam6 == Orther.IntParam6
			&&Type == Orther.Type
			&&Version== Orther.Version
			;
	}

	EConstructionType GetConstructionType()
	{
		return Type;
	}

	//保存Parameter
	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) const ;
	//反序列化Parameter
	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);

	/**计算hash值 */
	friend uint32 GetTypeHash( const ConstructionPatameters &  Key)
	{
		    uint32 GetTypeHash(int32);
		 	TArray<uint32> Hashes = {
			GetTypeHash((int32)Key.Type),
			GetTypeHash(Key.IntParam1),
			GetTypeHash(Key.IntParam2),
			GetTypeHash(Key.IntParam3),
			GetTypeHash(Key.IntParam4),
			//GetTypeHash(Key.SpaceIDParam),
			GetTypeHash(Key.IntParam6),
			GetTypeHash(Key.Version),
		};

		uint32 Result= Hashes[0];
		for (int32 i=1;i<Hashes.Num();i++)
		{
			Result= HashCombine(Result, Hashes[i]);
		}
		return Result;
	}

	//根据参数生成查询用的url
	FString GetQueryUrl();

	//软装、水电模式开关插座强电箱之类
	void SetNormalGoodsInfo(int32 GoodsID,int32 SpaceID=-1);

	//水电模式纯移施工项数据
	//@ InterfaceID 向后台查询数据时需要的参数
	void SetPureOrignalOffset(enum EComponentID ComponentID, int32 SpaceID = -1);

	//铺贴类，不推荐使用
	//@ GoodsID 商品ID
	//@ WallType 墙面类型
	//@ PaveMethod 铺法
	//@ HasHung 是否是下吊面
	void SetPaveInfo(int32 GoodsID,
		enum class EWallType WallType,
		enum EStyleType PaveMethod,
		bool HasHung = false,
		EConstructionVersion NewVersion = CurrentConstructionDataVersion
		);


	//铺贴类，V1.8版本之前使用
	//@ Style 墙面Style，必须确保有商品
	//@ WallType 墙面类型
	//@ HasHung 顶面是否下吊
	//@ SpaceID 空间ID
	void SetPaveInfo(TSharedPtr < class  FArmyBaseEditStyle > StyleWidthGoodsid
		, enum class EWallType WallType
		,bool HasHung =false
		);

	//铺贴类
	//@ Style 墙面Style，必须确保有商品
	//@ WallType 墙面类型
	//@ SpaceID 空间ID
	void SetPaveInfo2(TSharedPtr < class  FArmyBaseEditStyle > Style
		, enum class EWallType WallType
		, int32 SpaceID=-1
	);


	//原始墙面的施工项
	//@WallType 墙面类型
	//@Hung 是否有下吊
	void  SetOriginalSurface(EWallType WallType, bool HasHung = false,int32 SpaceID=-1);


	//设施施工项版本数据，用于施工项版本升级后保存旧数据
	void SetVersion(EConstructionVersion OldVersion)
	{
		Version =(int32) OldVersion;
	}


	//设置参数应有的空间ID
	void SetSpaceID(int32 newSpaceID)
	{
		SpaceIDParam = newSpaceID;
	}
    
	int32 GetSpaceID() const { return SpaceIDParam; }

	//施工项比较版本大小
	bool VersionLessThan(EConstructionVersion VersionToCompare);
private:
	friend class XRConstructionManager;
	friend struct ObjectConstructionKey;

	EConstructionType Type; //施工项类型
	int32  Version= (int32)CurrentConstructionDataVersion;  //施工项版本号，仅用于施工项数据升级
	//开始写参数
	//第一个整数参数
	int32 IntParam1 = -1;
	//第二个整数参数
	int32 IntParam2 = -1;
	//第三个整数参数
	int32 IntParam3 = -1;
	//第四个整数参数
	int32 IntParam4 = -1;
	//空间ID参数,专门表示空间ID,在生成Url时并不起作用
	int32 SpaceIDParam = -1;
	//第六个整数参数
	int32 IntParam6 = -1;
	//参数写完

};

//对象存储的施工项对应的key值
struct ARMYSCENEDATA_API ObjectConstructionKey
{
	FGuid ObjectID;
	ConstructionPatameters Parameters;

	//空构造函数，用于反序列化
	ObjectConstructionKey() {};

	ObjectConstructionKey(const FGuid& ObjectID, ConstructionPatameters Parameters)
	{
		this->ObjectID = ObjectID;
		this->Parameters = Parameters;
	}

	bool operator ==(const ObjectConstructionKey & Orther) const
	{
		return ObjectID == Orther.ObjectID
			&& (Parameters) == (Orther.Parameters)
		    &&Parameters.SpaceIDParam == Orther.Parameters.SpaceIDParam;
	}

	friend uint32 GetTypeHash(const ObjectConstructionKey&  Key)
	{
		int32 SpaceID = Key.Parameters.GetSpaceID();
		TArray<uint32> Hashes = {
			GetTypeHash(Key.Parameters),
			GetTypeHash(Key.ObjectID),
			GetTypeHash(SpaceID),
		};

		uint32 Result = Hashes[0];
		for (int32 i = 1; i<Hashes.Num(); i++)
		{
			Result = HashCombine(Result, Hashes[i]);
		}
		return Result;
	}

	//保存Key
	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) const;
	//反序列化Key
	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);
};



//施工项获取的结果
enum class EConstructionRequeryResult
{
	Failed=false, //获取失败
	Succeed=true, //获取成功
	TimeOut, //超时
};

//施工项数据代理
DECLARE_DELEGATE_FourParams(FConstructionDataDelegate,  EConstructionRequeryResult , TSharedPtr<FJsonObject>, CheckedData,ObjectConstructionKey);

class FArmyRoom;
class IArmyConstructionHelper;
//施工项管理工具
class  ARMYSCENEDATA_API XRConstructionManager
{

public:
	//初始化
	void Init();

	//将引用计数器设为友元
	friend class SharedPointerInternals::TIntrusiveReferenceController<XRConstructionManager>;
	static TSharedPtr <XRConstructionManager> Get();

	//异步向服务器查询施工项数据。
	//调用几次，就回调几次，但是相同的ObjectID 和Parameter 只查询一次
	//@ CallBack 查询得到结果后的回调
	//@ ObjectID 对应的XRObjectID
	//@ Parameter 施工项参数
	void TryToFindConstructionData( FGuid ObjectID, ConstructionPatameters Parameter, FConstructionDataDelegate CallBack);

	//获取参数对应的Json，不会触发异步查询
	OriginalData GetJsonData( const ConstructionPatameters& Parameter);

	//保存施工项
	void SaveConstructionData( FGuid ObjectID, ConstructionPatameters Parameter, CheckedData NewCheckData,bool Copy=true);

	//获取已保存的勾选数据
	//这个函数会导出和这个object相关的所有施工项
	CheckedData GetSavedCheckedData(const FGuid& ObjectID,EConstructionType Type= EConstructionType::All);

	//获取已保存的勾选数据,如果找不到就从原始json提取默认施工项或是创建一个空的
	CheckedData GetSavedCheckedData(const FGuid& ObjectID, const ConstructionPatameters& Parameter);

	//从json文件中获取施工项数据
	//@RetVal 返回默认勾选的施工项
	//@OrignalData 从服务器获取的原始施工项数据
	//@outUI 用来展示施工项的UI
	CheckedData ParseJson(TSharedPtr<FJsonObject> inOrignalData, TSharedPtr<class SArmyMulitCategory>* outUI=nullptr);

	//保存施工项
	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
	//反序列化施工项
	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);

public:

	//原始施工项数据
	struct OriginalConstructionData
	{
		//默认勾选信息
		CheckedData DefaultChecked = nullptr;
		OriginalData OriginalData = nullptr;
	};

private:


	//原始施工项数据
	TMap<ConstructionPatameters /*施工项参数*/, OriginalConstructionData /*与参数对应的原始施工项以及默认勾选数据*/> OriginalConstructionDataMap;

	//添加原始施工项数据
	void AddOriginalConstructionData(ConstructionPatameters Param, OriginalData Data);

	//获取默认的勾选项
	//CheckedData GetDefaultCheckedData(ObjectConstructionKey Key);

	//与XRObject关联的已勾选的施工项
	TMap<ObjectConstructionKey /*对象Guid以及参数*/, CheckedData/*对象对应的施工项勾选信息*/> CheckedDatas;

	//添加数据到CheckedDatas
	void AddCheckedConstructionData(ObjectConstructionKey, CheckedData);

	//获取施工项之后的回调
	TMultiMap<ObjectConstructionKey,TArray<FConstructionDataDelegate>> CallBacks;
	
	//当前的请求队列
	TArray<ObjectConstructionKey> CurrentQuery;

	//等待请求的队列
	TArray<ObjectConstructionKey> QueryQueue;

	//超时的请求
	//TArray<ObjectConstructionKey> TimeOutQueryQueue;

	//尝试开始一个请求
	void TryStartAQuery();

	//超时计时器
	//FTimerHandle TimeOutTimer;

	//通过key查询获得响应时的回调
	void OnGetResponseForKeyQuery(struct FArmyHttpResponse Response, ObjectConstructionKey Query);
	
	//通过Parameter获得响应时的回调
	void OnGetResponseForParameterQuery(struct FArmyHttpResponse Response, ConstructionPatameters Parameter);

	//被保护的施工项请求，比如原始墙面的施工项。
	//放在这个数组中的参数及对应的施工项不会随着反序列化被清除掉
	TArray<ConstructionPatameters> ProtectedConstructionParameter;

	//施工对象
	TArray<TWeakPtr<IArmyConstructionHelper>> ConstructionItems;


	//当拆改添加房间时发生
	void OnRoomAdded(TSharedPtr<FArmyRoom> Room);

	void OnRoomSpaceIDChanged(TSharedPtr<FArmyRoom> Room);
};