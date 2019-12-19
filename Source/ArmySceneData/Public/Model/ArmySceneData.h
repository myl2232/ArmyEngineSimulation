#pragma once

#include "ArmyObjectModel.h"
#include "FArmyConstructionItemInterface.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHomeData, Log, All);

DECLARE_MULTICAST_DELEGATE_ThreeParams(SOperatorCallBack, const ARMYArgument&, FObjectPtr, bool);

struct FBSPWallInfo
{
	TArray<TArray<FVector>> PolyVertices;//数组索引0代表最外层Poly（包括和门合并后的），也就是布尔运算的加法多边形
	TArray<FString> AttachWindowOrDoorName;//表示所有在大Poly下被布尔减法的小poly的ID集合(不包括门)
	FVector Normal = FVector::ZeroVector;//面法线
	FString UniqueId;//面本身的唯一标识
	FString UniqueIdForRoomOrHardware;//所属房间的唯一标识
	EObjectType GenerateFromObjectType = OT_WallLine; // 是根据什么生成的墙面片，比如，门洞，窗洞，拆改墙体
};

/**
 * 墙体线信息
 */
struct WallLineInfo
{
	WallLineInfo(TWeakPtr<FArmyLine> InLine, FObjectPtr InOwner) :Line(InLine), Owner(InOwner) {}

	bool operator==(const WallLineInfo& InOther) const
	{
		return Line == InOther.Line && Owner == InOther.Owner;
	}

	TWeakPtr<FArmyLine> Line;//墙体线
	FObjectPtr Owner;//所属对象
};
/**
 * 双墙墙体捕捉信息
 */
struct FDoubleWallCaptureInfo
{
	FDoubleWallCaptureInfo()
		: FirstLine(NULL)
		, SecondLine(NULL)
		, MinDistanceWall(NULL)
		, Pos(ForceInitToZero)
		, Direction(ForceInitToZero)
		, Thickness(24)
	{}

	TSharedPtr<FArmyLine> FirstLine;
	TSharedPtr<FArmyLine> SecondLine;
	FObjectPtr FirstOwner;
	FObjectPtr SecondOwner;
	TSharedPtr<FArmyLine> MinDistanceWall;
	FVector Pos;
	FVector Direction;
	float Thickness;
};

class FArmyRoom;

//@郭子阳 增加room代理
DECLARE_DELEGATE_OneParam(AddRoomDelegate, TSharedPtr<FArmyRoom> );
//@郭子阳 删除room代理
DECLARE_DELEGATE_OneParam(RemoveRoomDelegate, TSharedPtr<FArmyRoom> );

/** 绘制户型业务管理器 */
class ARMYSCENEDATA_API FArmySceneData
{
public:
	FArmySceneData() {}
	~FArmySceneData() {}

	static const TSharedRef<FArmySceneData>& Get();

	static const TSharedRef<FArmySceneData>& GetLayOutData();

	static void CopyModeData(EModelType InSrcData, EModelType InDesData, bool bCopyGUID = true);

	TSharedPtr<XRObjectModel> GetModel(EModelType InModelType);

	/** 添加 */
	void Add(FObjectPtr Object, ARMYArgument InArg, FObjectPtr InParent = NULL, bool bTransaction = false);

	/** 删除 */
	void Delete(FObjectPtr Object, bool bTransaction = false);

	/** 添加到全局绘制 */
	void AddToGlobal(const TArray<FObjectWeakPtr>& Objects);

	/** 清空全局绘制 */
	void EmptyGlobal();

	/**	通过类型获取物体列表 */
	const TArray<TWeakPtr<FArmyObject>>& GetObjects(EModelType InModelType);

	TWeakPtr<FArmyObject> GetObjectByGuid(EModelType InModelType, const FGuid& guid);

	TWeakPtr<FArmyObject> GetObjectByGuidAlone(const FGuid& guid);

	FObjectWeakPtr GetObjectByName(EModelType InModelType, const FString& Name);

	void GetObjects(EModelType InModelType, EObjectType InObjectType, TArray<FObjectWeakPtr>& OutObjects);

	void GetAllObjects(TArray<TWeakPtr<FArmyObject>>& OutArray) const;

	/**
	 * @ 通过空间名称，得到所有room
	 * @param InModelType - EModelType - 模式类型
	 * @param InSpaceName - const FString & - 空间名称
	 * @param InIgnoreOrder - bool - true表示完全匹配空间名称，否则忽略空间名称序号
	 * @param InIgnoreNoBestowName - bool - true表示不计入未命名空间，否则计入
	 * @return TArray<FObjectWeakPtr> -
	 */
	TArray<FObjectWeakPtr> GetObjectBySpaceName(EModelType InModelType, const FString& InSpaceName, bool InIgnoreOrder = false, bool InIgnoreNoBestowName = true);

	/**
	 * @ 通过该空间类型得到最大序号
	 * @param InModelType - EModelType - 模式类型
	 * @param InSpaceName - const FString & - 空间名称忽略序号
	 * @return int32 - 返回序号
	 */
	int32 GetMaxOrderBySpaceName(EModelType InModelType, const FString& InSpaceName);

	/**
	 * @ 降低所有传入的room序号
	 * @param InRoomArr - TArray<FObjectWeakPtr> & - 传入room
	 * @return bool - 计算是否成功
	 */
	bool CalReduceRoomOrder(TArray<FObjectWeakPtr> & InRoomArr);

	/** 绘制 */
	void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	/**	绘制全局数据*/
	void DrawGlobalObject(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	/** HUD绘制 */
	void DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas);

	/** 生成3D */
	void Generate(EObjectType ObjectType);

	/** 重新生成所有3D */
	void RegenerateAll();

	void SetObjectVisible(bool InVisible, EModelType InModelType = E_NoneModel/*ALL*/, EObjectType InObjectType = OT_None/*ALL*/);

	///** 重新计算踢脚线 */
	//void RebuildBaseBoards();

	void Init(UWorld* InWorld);

	void Refresh();

	/** 计算户型的最大包围盒 */
	FBox GetBounds();

	/** 清空数据 */
	void Clear(EModelType InModelType = E_HomeModel);
    void Clear(const TArray<FObjectPtr>& InObjects);

    /** @欧石楠 只清空拆改数据 */
    void ClearModifyData();

	/** 隐藏所有BSP*/
	void HideModelGeometry();

	/**
	 * 设置墙体不透明度
	 * @param Opacity - float - 0为透明，1为不透明
	 */
	void SetHomeOpacity(float Opacity);

	/** @梁晓菲 立面模式顶面视图显示半透明蒙版*/
	void ShowHardModeCeilingOpacity();

	TArray<TSharedPtr<class FArmyRoomSpaceArea>> ForceSelectRoomAreas(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient, TArray<float>& Dists);

	// 获取坐标点所在平面
	void ForceOnRoomAreas(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient, TArray<TSharedPtr<class FArmyRoomSpaceArea>>& OnSelectes);

	TArray<TSharedPtr<class FArmyRoomSpaceArea>> GetRoomAttachedSurfacesWidthRoomID(const FString& roomId);

	TArray<TSharedPtr<class FArmyRoomSpaceArea>> GetRoomAttachedComponentSurfaceWithRoomId(const FString& roomId);

	//@郭子阳 获取扣条
	void GetBuckleConstrucionItem(const FString& roomId, TArray<struct FArmyGoods>& results);

	//@郭子阳 修改房间内所有施工对象的空间
	void ChangeAllConstructionItemInRoom(TSharedPtr<FArmyRoom> Room);

	TArray<TSharedPtr<class FArmyRoomSpaceArea>> GetCurrentRoofAtttachWallRoomSpace(const FString& roomId);

	bool  GetInnearRoomByRoomID(const FString& RoomId, TSharedPtr<class FArmyRoom>& Innearroom);

	bool GetRoomHoleInfoById(const FString& RoomId, TArray<FVector2D>& OutVertices);

	TArray<TSharedPtr<class FArmyBaseArea>> PickOperation(const FVector& pos, class UArmyEditorViewportClient* InViewportClient);

	void Set3DRoomVisible(bool BVisible);
	// 掩藏硬装原始数据
	void SetHardwareModeActorVisible(bool BVisible);
	// 将原始2维数据转换成3维数据
	void ConvertRoomInfoToWallInfo();
	void DeleteSurfaceWhithId();
	void UpdateInnearWallInfo(TSharedPtr<FArmyRoom> inRoom, TArray<FVector>& FinishRoomVertices);

	void DeleteAttachInnerRoomUnUsedWalls(TSharedPtr<FArmyRoom> InnerRoom);

	void CalculateWallAttachHardwareInfo(TSharedPtr<class FArmyWallLine> wall, FBSPWallInfo& attWallInfo, bool originalFace = true);
	void GetInnerRoomRelatedPunch(TSharedPtr<FArmyRoom>Inroom, TArray<TSharedPtr<class FArmyPunch>>& RelatedPunchs);
	void CalculateRoomPunchHoles(TArray<TSharedPtr<FArmyPunch>> RelatedPunchs, const TSharedPtr<class FArmyWallLine> InWallLine, FBSPWallInfo& InWallInfo, bool bColliner = false);
	void CaculateSurfaceInfoWhithWallInfo(const FBSPWallInfo& finishedSurface, const FBSPWallInfo& originalSurface, uint32  supportGoodType);
	void UpdateInnearWallAttachSimpleComponentInfo(const FVector& InPlanceCenter, const FVector& InPlaneXDir, const FVector& InPlaneYDir, float wallLength, TArray<TArray<FVector>>& results);
	void ConvertBspWallInfoToSurfaceHole(const FBSPWallInfo& InWallInfo, FVector& InXDir, FVector& InYDir, FVector& InCenter, uint32& OutSurfaceType, TArray<FVector>& orginalVertices, TArray<TArray<FVector>>& AreaHoles);
	TSharedPtr<FArmyRoomSpaceArea> GetWallByRoomIdAndAttachWallLineId(const FString& AttachRoomId, const FString& AttachWallLineId);
	void UpdateOutWallInfo(TSharedPtr<FArmyRoom> OuterRoom);
	// @曾援 计算不受房间限制的对象(目前只有梁)在3D下生成的面片
	void CalculateOuterRoomSimpleComponent(const TArray< TPair<FString, TArray<TArray<FVector>>> > &InFinishInnerRoomsRoofInfos);
	// 计算房间内、柱子、烟道在3D下生成的面片
	void CalculateInnerRoomSimpleComponent(TSharedPtr<FArmyRoom> InRoom);
	void CaculateWindowHoleSurface();
	void GetHardwareRelateWalllines(TWeakPtr<class FArmyObject> InHardWare, TArray<TSharedPtr<class FArmyWallLine>>& AttachWalllines);
	void CaculatePunchSurface();
	// 计算整个户型的墙体
	void CaculateSolidWall(const TArray<FVector>& HouseVerts, const TArray<TArray<FVector>>& TotalInneroomVerts);

	void CaculatePunchTopSurface(TSharedPtr<FArmyPunch> punch, const TArray<FVector>& OutRoomVerts, const TArray<TArray<FVector>>& IntersectInnerooms);

	TArray<FVector> CalculateHardWareClipBox(TSharedPtr<class FArmyHardware> InHardWare, bool orignalFace = true);

	/** 收集施工算量数据，所有施工算量数据单位均为米(m) */
	void CollectConstructionQuantity(struct FArmyConstructionQuantity& OutCQ);

	
	//@郭子阳 改变房间内施工对象对应的房间ID
	void  ChangeConstructionHelpersSpaceID(TSharedPtr<FArmyRoom> ChangedRoom);


	void GetHardWareObjects(TArray<FObjectWeakPtr>&, EModelType InModelType = E_HomeModel);

	TSharedPtr<class FArmyRoom> GetRoomFromLine(TSharedPtr<class FArmyLine> InLine);

	TSharedPtr<class FArmyRoom> GetRoomFromPoint(TSharedPtr<class FArmyEditPoint> InPoint);

	/** @欧石楠 get/set 是否生成外墙*/
	void SetHasOutWall(bool bHas);

	bool GetHasOutWall() { return bHasOutWall; }

	void SplitSimpleComponentsAndRooms();

	/** @欧石楠 包立管参与空间划分*/
	void SplitPackPipeAndRooms();

	void SplitRooms();

	void MergeRooms();

	/**@欧石楠 获取所有hardware*/
	void GetAllHardWare(TArray<TWeakPtr<class FArmyHardware>> &OutHardwares, EModelType InModelType = E_HomeModel);

	//序列化不同模式
	void SerializeToJsonDifferentMode(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter, EModelType InModelType = E_HomeModel);
	void DeserializationDifferentMode(TSharedPtr<FJsonObject> Data, EModelType InModelType = E_HomeModel);

	void ChangeBaseToModifyModelDelegate();
	void GenerateDataTo3D();

	void Generate3DSurfaceActors();
	void GenerateHardwareModel();

	//@ 生成点位关联的模型
	void GeneratePointPositionModel();

	TSharedPtr<FArmyRoom> GetRelatedRoomByLine(TSharedPtr<FArmyLine> CheckedLine, EModelType InModelType = E_HomeModel);

    /** @欧石楠 如果每帧都实时捕捉墙线的话，建议将需要捕捉的墙线缓存起来，不要实时获取 */
    bool CaptureDoubleLine(const FVector& InPos, EModelType InModelType, FDoubleWallCaptureInfo& OutInfo);
    bool CaptureDoubleLine(TArray<WallLineInfo>& InLines, const FVector& InPos, FDoubleWallCaptureInfo& OutInfo);

	void ForceCaptureDoubleLine(TSharedPtr<FArmyHardware> InHardware, EModelType InModelType = E_HomeModel);

	/** 强制门窗重新捕捉一遍指定房间的墙线 */
	void ForceUpdateHardwaresCapturing(TSharedPtr<FArmyRoom> InRoom);

    /** @欧石楠 通过墙面获取与之邻接的其它墙面 */
    void GetRelatedRoomSpaceAreas(TSharedPtr<FArmyRoomSpaceArea> SourceRoomSpaceArea, TArray< TSharedPtr<FArmyRoomSpaceArea> >& OutRoomSpaceAreas);

    /** @欧石楠 底图 */
    void SetFacsimile(TSharedPtr<class FArmyReferenceImage> InFacsimile);
    TWeakPtr<FArmyReferenceImage> GetFacsimile();

	/** 底图透明度 */
    const int32 GetFacsimileTransparency();
	void SetFacsimileTransparency(const int32 InFacsimileTransparency);

	/** 墙线颜色 */
	const FLinearColor GetWallLinesColor() const { return WallLinesColor; }
	void SetWallLinesColor(const FLinearColor InColor);

public:

	AddRoomDelegate OnRoomAdded;
	RemoveRoomDelegate OnRoomRemoved;

	/**@欧石楠 全局墙高*/
	static float WallHeight;
	static float OutWallThickness;

	/** @欧石楠 最大墙体捕捉厚度 */
	static float MaxWallCaptureThickness;

	static 	float FinishWallThick;

	//不同模式有改动后会发出此多波代理
	static FSimpleMulticastDelegate HomeMode_ModifyMultiDelegate;
	static FSimpleMulticastDelegate ModifyMode_ModifyMultiDelegate;
	static FSimpleMulticastDelegate LayOutMode_ModifyMultiDelegate;

	bool bUpdateHomeModeData = false;//户型模式数据是否更新过
	bool bUpdateModifyModeData = false;//拆改中模式数据是否更新过
	bool bUpdateLayoutModeData = false;//拆改后模式数据是否更新过

	/** true显示拆改中，false显示拆改后 */
	bool bIsDisplayDismantle = true;

	bool bRebuild3DModel = true;//切换到立面或者水电是否重新生成数据

	////////////////////////////////关于户型模式修改后引起立面数据重构//////////////////////////////////////////

public:
	enum EModifyType
	{
		/** 创建 */
		MT_Create,

		/** 删除 */
		MT_Delete,

		/** 修改 */
		MT_Modify,
	};

	struct FModifyConnectedData
	{
		TArray<FString> ConnectRoomIdList;
		EModifyType ModifyType;
	};
	TMap<FString, FModifyConnectedData> ModifyDataMap;

	void PushModifyDataMap(TSharedPtr<class FArmyObject> InObject, uint8 InModifyTpye);

	void PushModifyDataMapFromGUID(FGuid InID, uint8 InModifyTpye);

	void ClearModifyMap();

private:
	/**
	 * @ 创建空间名称与空间的关联关系(注：解决从户型到拆改模型空间名称与空间没法建立关联问题)
	 * @param InDesData - EModelType - 模块类型
	 * @return bool - true表示建立成功，否则建立失败
	 */
	static bool CreateSpaceNameToRoomRelated(EModelType InDesData);

	//@ 在对应模式下生成输入空间的名称标注
	bool GenerateNewRoomSpaceNameLabel(TSharedPtr<FArmyRoom> InRoom, EModelType InModelType);

	/**
	 * @ 将TArray转化为TMap，并按照Key值进行排序
	 * @param InRoomArr - TArray<FObjectWeakPtr> & - 传入的TArray
	 * @return TMap<int32, FObjectWeakPtr> & - 转化后的TMap
	 */
	TMap<int32, FObjectWeakPtr>  GetSortDataFromTArrayToTMap(TArray<FObjectWeakPtr> & InRoomArr);

	// @曾援
	void CalculateSimpleComponent(TSharedPtr<FArmyObject> InSimpleComponent, const FString &InSuffix = TEXT(""), const TArray<FVector> &InOverridedBoundaryVertices = TArray<FVector>());

	/** 施工算量数据收集 */
	void CollectSpaceData(TArray<struct FArmySpaceData>& OutSpaceData);
	void CollectModifyWallData(TArray<FArmySpaceData>& OutModifyWallData);
	void CollectHydropowerData(TArray<FArmySpaceData>& OutHydropowerData);
	void CollectHardwareData(TArray<FArmySpaceData>& OutHardwareData);
	// InType参见XRWHCModeData.h的ECabinetClassify
	void CollectWHCData(struct FArmyWHCModeListData& OutWHCModeData, int32 InType);
	void CollectQuotaData(TMap<int32, TMap<int32, int32> > CheckedId, struct FArmyGoods& OutGoods);

public:
	UWorld* World;

	SOperatorCallBack PreOperationDelegate;
	//添加
	SOperatorCallBack PostOperationDelegate;

	//TArray<class AXRBrush*> RoofBrushList;

	FSimpleMulticastDelegate ChangedHomeDataDelegate;

	bool LoadDataFromJason = false;
	void ClearHardModeData();
	// 整个外墙体actor
	class AXRShapeActor* TotalOutRoomActor = NULL;
	FGuid OutterWallLightMapID;
	// 整个墙体actor
	class AXRShapeActor* TotalSolidWallActor = NULL;

	/**@欧石楠 存储是否生成外墙，临时，后续可能修改*/
	bool bHasOutWall = false;

	//@ 生成点位代理
	FArmyObjectAndUObjectDelegate PointPosGenerateDelegate;

	// TODO: zengy added 临时解决方案，生成木作柜子清单
	DECLARE_DELEGATE_TwoParams(FGetWHCModeListData, struct FArmyWHCModeListData&, int32)
	FGetWHCModeListData GetWHCModeListData;

	/** 标识同步方案时需要忽略的模型ID */
	TArray<int32> IgnoredModelIndexes;

protected:
	TArray<FObjectWeakPtr> GlobalDataArray;
	TMap< uint32, TSharedPtr<XRObjectModel> > ModelMap;
	TMap< uint32, TArray<FObjectWeakPtr>> ObjectArrayMap;

	/**@欧石楠 存储所有hardware 分模式*/
	TArray<TWeakPtr<FArmyHardware>> AllHardwares_Home;
	TArray<TWeakPtr<FArmyHardware>> AllHardwares_Modify;
	TArray<TWeakPtr<FArmyHardware>> AllHardwares_Layout;

	/** 高亮的物体 */
	FObjectPtr HoveredObject;

private:
	/** @梁晓菲 半透明蒙版70%透明度，不下吊*/
	class AXRWallActor* TranslucentRoofActor = nullptr;
	/** @梁晓菲 半透明吊顶蒙版90%透明度，进行了下吊*/
	class AXRWallActor* TranslucentRoofActor_SuspendedCelling = nullptr;
	/** @梁晓菲 计算半透明区域*/
	void CaculateTranslucentRoofActor(TSharedPtr<FArmyBaseArea> Room);

	/** 底图透明度，默认为60% */
	int32 ImageTransparency = 60;

	/** 墙线颜色 */
	FLinearColor WallLinesColor = FLinearColor::White;

	struct VectorAndItemID
	{
		FVector ActorPos;
		int32 ItemID;
		VectorAndItemID(FVector InActorPos, int32 InItemID)
		{
			ActorPos = InActorPos;
			ItemID = InItemID;
		}
		friend bool operator==(const VectorAndItemID& temp1, const VectorAndItemID& temp2)
		{
			if (temp1.ActorPos == temp2.ActorPos && temp1.ItemID == temp2.ItemID)
			{
				return true;
			}
			return false;
		}
		friend bool operator!=(const VectorAndItemID& temp1, const VectorAndItemID& temp2)
		{
			if (temp1.ActorPos == temp2.ActorPos && temp1.ItemID == temp2.ItemID)
			{
				return false;
			}
			return true;
		}
		friend uint32 GetTypeHash(const VectorAndItemID& temp)
		{
			return FCrc::MemCrc_DEPRECATED(&temp, sizeof(VectorAndItemID));
		}
		void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
		{
			JsonWriter->WriteValue("ActorPos", ActorPos.ToString());
			JsonWriter->WriteValue("ItemID", ItemID);
		}

		void Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
		{
			ActorPos.InitFromString(InJsonData->GetStringField("ActorPos"));
			ItemID = InJsonData->GetIntegerField("ItemID");
		}
	};

	//************施工项存储——已废弃，为了兼容旧方案 不予删除***********
	/** @梁晓菲 素材的施工项数据*/
	TMap<int32, TSharedPtr<FJsonObject>> ItemIDJsonDataMap;
	/* @梁晓菲 放样线条UniqueCode与施工项勾选数据*/
	TMap<FString, TSharedPtr<FArmyConstructionItemInterface>> ExtrusionConstructionItemDataMap;
	/* @梁晓菲 ActorUniqueId与施工项勾选数据，程序运行时使用*/
	TMap<int32, TSharedPtr<FArmyConstructionItemInterface>> ActorUniqueIDConstructionItemDataMap;
	/* @梁晓菲 ActorVector与施工项勾选数据，保存加载时使用*/
	TMap<VectorAndItemID, TSharedPtr<FArmyConstructionItemInterface>> ActorVectorConstructionItemData;

public:
	/** @梁晓菲 添加某类素材ItemID从后台获取的施工项数据*/
	void AddItemIDConstructionData(int32 InItemID, TSharedPtr<FJsonObject> InJsonData);
	/** @梁晓菲 通过ItemID获取施工项数据*/
	TSharedPtr<FJsonObject> GetJsonDataByItemID(int32 InItemID);
	/** @梁晓菲 添加Actor的施工项勾选数据*/
	void AddActorConstructionItemData(int32 InActorID, TSharedPtr<FArmyConstructionItemInterface> InConstructionItemData);
	/** @梁晓菲 删除Actor的施工项勾选数据*/
	void DeleteActorConstructionItemData(int32 InActorID);
	/** @梁晓菲 通过ActorUniqueId获取施工项勾选数据*/
	TSharedPtr<FArmyConstructionItemInterface> GetConstructionItemDataByActorUniqueID(int32 InActorID);
	/** @梁晓菲 通过ActorVector获取施工项勾选数据*/
	TSharedPtr<FArmyConstructionItemInterface> GetConstructionItemDataByActorVector(FVector AvtorVector, int32 InItemID);
	/** @梁晓菲 添加放样线条的施工项勾选数据*/
	void AddExtrusionConstructionData(FString ExturesionUniqueCode, TSharedPtr<FArmyConstructionItemInterface> InConstructionItemData);
	/** @梁晓菲 通过放样线条的UniqueCode获取施工项勾选数据*/
	TSharedPtr<FArmyConstructionItemInterface> GetConstructionItemDataByExtrusionUniqueCode(FString ExturesionUniqueCode);
	/** @梁晓菲 删除放样线条的施工项勾选数据*/
	void DeleteExtrusionConstructionItemData(FString ExturesionUniqueCode);

public:
	//@郭子阳
	//************施工项存储——已废弃，为了兼容旧方案 不予删除***********

	//获取施工项
	/*
	*@ ID FContentItem的ID 对象ID ,原始點位輸入-1*ComponentType
	*/
	static TSharedPtr<FJsonObject> GetHyConstructionData(int32 ID);

	static void AddHyConstructionData(int32 ConstructionKey, TSharedPtr<FJsonObject> ConstructionData);

	//出于导出清单的需要，这里是裸指针
	static int32 GetConstructionDataListID(class FArmyFurniture *Furniture);

	//施工项数据存储
	static TMap<int32 /*FContentItem的ID，原始点位为-1*/, TSharedPtr<FJsonObject>  /*对应施工项*/> HyConstructionDataMap;

	//************施工项存储***********
};