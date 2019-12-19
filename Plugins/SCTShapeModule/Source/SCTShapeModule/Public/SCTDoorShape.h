///**
//* Copyright 2018 北京伯睿科技有限公司.
//* All Rights Reserved.
//* 
//*
//* @File SCTInsideShape.h
//* @Description 门
//*
//* @Author 赵志强
//* @Date 2018年5月15日
//* @Version 1.0
//*/
//#pragma once
//
//#include "SCTShape.h"
//#include "CoreMinimal.h"
//#include "SCTAttribute.h"
//
//class FAccessoryShape;
//class FDecorationShape;
//
///** Pak资源所需的存储属性 */
//struct FDoorPakMetaData
//{
//	int64 ID = 0;
//	FString Name;
//	FString Url;
//	FString MD5;
//	FString ThumbnailUrl;
//};
//
///**合页、铰链规则*/
//struct FDoorHingeNumberRule
//{
//	int32 Number = 0;
//	int32 Min = 0;
//	int32 Max = 0;
//};
//
//
///** 默认位置信息 */
//struct FDoorSubWidgetDefaultLocation
//{
//	int32 Hpos				= -1;		/*!< 水平位置：1 距左边；2 距右边；3 水平居中；*/
//	float HposVal			= -1.0f;	/*!< 水平位置距离*/
//	int   Vpos				= -1;		/*!< 垂直位置：1 距上边；2 距下边；3 水平居中；4 公式；*/
//	float VposVAl			= -1.0f;	/*!< 垂直位置距离*/
//	int32 RotateDirection   = 0;		/*!< 旋转方向：0 保持不变；1 左旋90度；2 右旋90度；*/
//};
//
///** 可用五金信息 */
//struct FAuxiliaryMetalInfo
//{
//	FString Name;
//	int32 ID = -1;
//	FDoorSubWidgetDefaultLocation Location;
//};
//
//struct FAccessoryToDownload
//{
//	FString Url;
//	FString Md5;
//	int32 ModelID;
//	float UpdateTime;
//};
//
///* 五金位置 */
//struct FMetalDefaultLocation : public FDoorSubWidgetDefaultLocation
//{
//	int32 MetalType;
//	FString MetalName;
//};
//
///* 把手位置 */
//struct FDoorKnobDefaultLocation : public FDoorSubWidgetDefaultLocation
//{};
//
///* 抽面位置 */
//struct FDrawFaceDefaultLocation : public FDoorSubWidgetDefaultLocation
//{};
//
///** 门默认属性信息 */
//struct FDoorDefaultInfo
//{
//	int32 ID;
//	int32 DoorType;	/*!< 门类型 1 移门; 2掩门; 3 抽面*/
//	int32 SubstrateId; /*!< 默认基材*/
//	int32 DoorSlotValue; /*!< 门缝值*/
//	int32 CoverUpWay;  /*!< 掩盖方式 1内嵌；2外盖*/
//	int32 RotationDirection; /*!< 旋转方向 0 保持不变，默认； 1 左旋转90度； 2 右旋转90 */
//	TSharedPtr<TArray<TSharedPtr<FDoorHingeNumberRule>>> HingNumberRuleList; /*!< 铰链数量规则*/
//	TSharedPtr<TArray<TSharedPtr<FMetalDefaultLocation>>> MetalDefaultLocationList;/*!< 五金位置*/
//	TSharedPtr<TArray<TSharedPtr<FDoorKnobDefaultLocation>>> DoorKnobDefalutLocationList;/*!< 拉手默认位置*/
//	TSharedPtr<TArray<TSharedPtr<FDrawFaceDefaultLocation>>> DrawFaceDefaultLocationList; /*!< 抽面位置*/
//};
//
//
//
////门基类
//class SCTSHAPEMODULE_API FDoorShape : public FSCTShape
//{
//public:
//	FDoorShape() = default;
//	virtual ~FDoorShape() = default;
//
//public:
//	/** 解析JSon文件创建型录 */
//	virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
//	virtual void ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
//	virtual void ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
//	virtual void ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
//
//	/** 将型录转存为JSon文件 */
//	virtual void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
//	virtual void SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
//	virtual void SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
//	virtual void SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
//
//	/** 拷贝创建新的型录 */
//	virtual void CopyTo(FSCTShape* OutShape) override;
//
//	/** 创建型录Actor */
//	virtual ASCTShapeActor* SpawnShapeActor() override;
//
//	/** 设置ProfileName */
//	virtual void SetCollisionProfileName(FName InProfileName) override;
//
//	/** 获取型录中的所有资源Url */
//	virtual void GetResourceUrls(TArray<FString> &OutResourceUrls);
//	/** 获取型录中的所有资源路径 */
//	virtual void GetFileCachePaths(TArray<FString> &OutFileCachePaths);
//	/** 检查重复资源 */
//	virtual void CheckResourceUrlsAndCachePaths(TArray<FString> &OutResourceUrls, TArray<FString> &OutFileCachePaths);
//
//	/** 更新材质 */
//	virtual void UpdateMaterial(UMaterialInterface * InMaterialInterface) {}
//
//public:
//	virtual void ReCalDividBlocksPositionAndSize() = 0;
//public:	
//
//	/** 门缝 */
//	const TArray<int32> & GetDoorSlotValues() const { return DoorSlotValues; }
//	TArray<int32> & GetDoorSlotValues() { return DoorSlotValues; }
//	void SetDoorSlotValues(const TArray<int32> & InDoorSlotValues) { DoorSlotValues = InDoorSlotValues; }
//
//	/** 延伸 */
//	const TArray<int32> & GetExtensionValues() const {return ExtensionValues; }
//	TArray<int32> & GetExtensionValues() { return ExtensionValues; }
//	void SetExtensionValues(const TArray<int32> & InExtensionValues) { ExtensionValues = InExtensionValues; }
//
//	/** 基材 */
//	const int32 GetSubstrateID() const { return SubstrateID; }
//	void SetSubstrateID(const int32 InSubstrateID) { SubstrateID = InSubstrateID; }
//	const FString & GetSubstrateName() const { return SubstrateName; }
//	void SetSubstrateName(const FString & InSubstrateName) { SubstrateName = InSubstrateName; }
//	const TMap<int32, TSharedPtr<FSCTShape>> & GetOptionShapeMap()const { return OpetionsMetalsShapesList; }
//
//
//	/** 材质 */
//	const FDoorPakMetaData & GetMaterial() const { return Material; }
//	FDoorPakMetaData & GetMaterial() { return Material; }
//	void SetMaterial(const FDoorPakMetaData & InMaterial) { Material = InMaterial; }
//protected:
//	/** 对五金可选项进行列表分类 */
//	virtual void ProcessOptionsMetalType() {}
//
//private:	
//	TArray<int32> DoorSlotValues;	 /*!< 门缝值 数组下标说明： 1水平，2垂直，默认值来源企业工艺设置*/
//	TArray<int32> ExtensionValues;	 /*!< 延伸值 数组下标说明: 1上延，2下延，3左延，4右延，默认值是0*/	
//	int32 SubstrateID;				 /*!< 基材ID */
//	FString SubstrateName;	         /*!< 基材名称 */
//	TArray<FAccessoryToDownload> ToDownloadAccessoryList;/*!< 需要下载的五金列表*/
//protected:
//	TMap<int32, TSharedPtr<FSCTShape>> OpetionsMetalsShapesList;
//	FDoorPakMetaData Material;		 /*!<  掩门材质 */
//};
//
////抽面类
//class SCTSHAPEMODULE_API  FDrawerShape : public FDoorShape
//{
//public:
//	FDrawerShape();
//	virtual ~FDrawerShape();
//public:
//	/** 解析JSon文件创建型录 */
//	virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
//	virtual void ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
//	virtual void ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
//	virtual void ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
//
//	/** 将型录转存为JSon文件 */
//	virtual void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
//	virtual void SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
//	virtual void SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
//	virtual void SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
//
//	/** 拷贝创建新的型录 */
//	virtual void CopyTo(FSCTShape* OutShape) override;
//
//	/** 创建型录Actor */
//	virtual ASCTShapeActor* SpawnShapeActor() override;
//
//	/** 添加子对象 */
//	void AddBaseChildShape(const TSharedPtr<FSCTShape> &InShape);
//
//	/** 设置ProfileName */
//	virtual void SetCollisionProfileName(FName InProfileName) override;
//
//	/** 获取型录中的所有资源Url */
//	virtual void GetResourceUrls(TArray<FString> &OutResourceUrls) override;
//
//	/** 获取型录中的所有资源路径 */
//	virtual void GetFileCachePaths(TArray<FString> &OutFileCachePaths) override;
//public:
//	virtual void UpdateMaterial(UMaterialInterface * InMaterialInterface) override;
//public:
//	/** 门切块 */
//	const TArray<TSharedPtr<FDecorationShape>> & GetDividBlocks() const { return DividBlocks; }
//	TArray<TSharedPtr<FDecorationShape>> & GetDividBlocks() { return DividBlocks; }
//	void SetDividBlocks(const TArray<TSharedPtr<FDecorationShape>> & InDividBlocks) { DividBlocks = InDividBlocks; }
//
//	/** 五金 */
//	const TArray<TSharedPtr<FAccessoryShape>> & GetMetals() const { return Metals; }
//	TArray<TSharedPtr<FAccessoryShape>> & GetMetals() { return  Metals; }
//	void SetMetals(const TArray<TSharedPtr<FAccessoryShape>> & InMetals) { Metals = InMetals; }	
//
//	/** 获取默认锁 */
//	FAuxiliaryMetalInfo GetDefaultLockInfo() const;
//	void SetDefaultLockInfo(const FAuxiliaryMetalInfo & InInfo) { DefaultLockLocationInfo = InInfo; RecalLockPosition(); }
//
//	/** 获取默认拉手*/
//	FAuxiliaryMetalInfo GetDefaultHandleInfo() const;
//	void SetDefaultHandleInfo(const FAuxiliaryMetalInfo & InInfo) { DefaultHandleLocationInfo = InInfo; RecalHandlePosition();}
//
//	/** 获取可用锁列表 */
//	const TArray<FAuxiliaryMetalInfo> & GetAuxiliaryLockList() const { return LockList; }
//
//	/** 获取可用拉手列表 */
//	const TArray<FAuxiliaryMetalInfo> & GetAuxiliaryHandleList() const { return HandleList; }
//
//	/** 封边 */
//	const FDoorPakMetaData & GetEdgeBanding() const { return EdgeBanding; }
//	FDoorPakMetaData & GetEdgeBanding() { return EdgeBanding; }
//	void SetEdgeBanding(const FDoorPakMetaData & InEdgeBanding) { EdgeBanding = InEdgeBanding; }
//
//	/** 连接方式 */
//	const TArray<int32> & GetConnectMode() const { return ConnectionMode; }
//	TArray<int32> & GetConnectMode() { return ConnectionMode; }
//	void SetConnectMode(const TArray<int32> & InConnectMode) { ConnectionMode = InConnectMode; }
//
//	/** 开门方向 */
//	int32 GetOpenDirection() const { return OpenDirection; }
//	void SetOpenDirection(const int32 InOpenDirection) { OpenDirection = InOpenDirection; }
//
//	/** 掩盖方式 */
//	int32 GetConverUpWay() const { return ConverUpWay; }
//	void SetConverUpWay(const int32 InConverUpWay) { ConverUpWay = InConverUpWay; }
//
//	TSharedPtr<FNumberRangeAttri> GetHandleAttriX() { return HandleAttriX; }
//	bool SetHandlePosX(const FString &InStrValue);
//	bool SetHandlePosX(float InIntValue);
//	TSharedPtr<FNumberRangeAttri> GetHandleAttriZ() { return HandleAttriZ; }
//	bool SetHandlePosZ(const FString &InStrValue);
//	bool SetHandlePosZ(float InIntValue);
//	TSharedPtr<FNumberRangeAttri> GetLockAttriX() { return LockAttriX; }
//	bool SetLockPosX(const FString &InStrValue);
//	bool SetLockPosX(float InIntValue);
//	TSharedPtr<FNumberRangeAttri> GetLockAttriZ() { return LockAttriZ; }
//	bool SetLockPosZ(const FString &InStrValue);
//	bool SetLockPosZ(float InIntValue);	
//
//	/** 重新生成拉手或者锁 */
//	void ReBuildHandleOrLock(const bool InIsHanle = true);
//
//	TSharedPtr<FTextSelectAttri> GetHanleRotateDirectionAttri() { return HanleRotateDirection; }
//	TSharedPtr<FTextSelectAttri> GetHanleHPosSelectAttri() { return HandlePosHSelectXAtt; }
//	TSharedPtr<FTextSelectAttri> GetHandleVPosSelectAttri() { return HandlePosVSelectXAtt; }
//	TSharedPtr<FTextSelectAttri> GetLockRotateDirectionAttri() { return LockRotateDirection; }
//	TSharedPtr<FTextSelectAttri> GetLockHPosSelectAttri() { return LockPosHSelectXAtt; }
//	TSharedPtr<FTextSelectAttri> GetLockVPosSelectAttri() { return LockPosVSelectXAtt; }
//	TSharedPtr<FTextSelectAttri> GetHandleListSelectAttri() { return HandleListSelectAtt; }
//	TSharedPtr<FTextSelectAttri> GetLockListSelectAttri() { return LockListSelectAtt; }
//
//private:
//	/** 重新计算切块坐标 */	
//	virtual void ReCalDividBlocksPositionAndSize() override;
//	/** 重新计算拉手的位置 */
//	void RecalHandlePosition();
//	/** 重新计算锁的位置*/
//	void RecalLockPosition();
//	/** 对五金可选项进行分类 */
//	virtual void ProcessOptionsMetalType() override;
//protected:		
//	//分割模块	
//	TArray<TSharedPtr<FDecorationShape>>  DividBlocks;
//	//五金
//	TArray<TSharedPtr<FAccessoryShape>>    Metals;
//
//	// 可用的锁列表
//	TArray<FAuxiliaryMetalInfo> LockList;
//	// 可用拉手列表
//	TArray<FAuxiliaryMetalInfo> HandleList;
//	// 默认把手位置信息
//	FAuxiliaryMetalInfo DefaultHandleLocationInfo;
//	// 默认锁的位置信息
//	FAuxiliaryMetalInfo DefaultLockLocationInfo;
//	TSharedPtr<FNumberRangeAttri> HandleAttriX;
//	TSharedPtr<FNumberRangeAttri> HandleAttriZ;
//	TSharedPtr<FNumberRangeAttri> LockAttriX;
//	TSharedPtr<FNumberRangeAttri> LockAttriZ;
//	TSharedPtr<FTextSelectAttri>  HanleRotateDirection;
//	TSharedPtr<FTextSelectAttri> HandlePosHSelectXAtt;
//	TSharedPtr<FTextSelectAttri> HandlePosVSelectXAtt;
//	TSharedPtr<FTextSelectAttri> LockRotateDirection;
//	TSharedPtr<FTextSelectAttri> LockPosHSelectXAtt;
//	TSharedPtr<FTextSelectAttri> LockPosVSelectXAtt;
//	TSharedPtr<FTextSelectAttri> HandleListSelectAtt;
//	TSharedPtr<FTextSelectAttri> LockListSelectAtt;	
//protected:
//	FDoorPakMetaData EdgeBanding;		/*!<  掩门封边 */	
//	TArray<int32>	ConnectionMode;		/*!< 连接方式 1铰链，2气压撑 */
//	int32 OpenDirection = 1;			/*!< 开门方向 0不开门，1左开门，2右开门*/
//	int32 ConverUpWay = 1;				/*!< 掩盖方式，1 内嵌，2外盖 默认设置来源于企业工艺设置 */
//};
//
//
////掩门（平开门）类
//class SCTSHAPEMODULE_API SideHungDoor : public FDrawerShape
//{
//public:
//	SideHungDoor() = default;
//	virtual ~SideHungDoor() = default;
//};
//
//
//class AActor;
//class FBoardShape;
////移门（推拉门）类
//class SCTSHAPEMODULE_API SlidingDoor : public FDoorShape
//{
//private:
//	/** 门 */
//	class FDoor
//	{		
//	public:
//		~FDoor() { DestoryActor(); }
//	private:
//		class FDividBlock
//		{			
//		public:			
//			void ParseInfoFromJson(const TSharedPtr<FJsonObject>& InJsonObject);
//			void SaveInfoToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
//			void SpawnActor(AActor * InParentActor,const FDoor & InParentDoor);
//			/** 重新计算尺寸与坐标 */
//			void RecalSizeAndPosition(const FDoor & InParentDoor);
//			void ResetDividPosition();
//			/** 资源拷贝 */
//			TSharedPtr<FDividBlock> CloneDividBlock() const;
//			void DestoryActor();			
//		public:		
//			TArray<TSharedPtr<FDividBlock>> ChildrenBlocks;
//			int32 SectionalMaterialId = -1;
//			float PosX = 0.0f;
//			float PosY = 0.0f;
//			float PosZ = 0.0f;
//			float Width = 0.0f;
//			float Height = 0.0f;	
//			FString DividRation;
//			bool IsRation = false;
//			float DividValue = 0.0f;
//			int32 DividDirection = 0; /*!< 0 不切分，1 竖向切分，2横向切分*/
//			TArray<AActor *> StaticMeshActors;
//			bool bIsRoot = false;
//		};
//
//		enum class ESectionalMaterialType : uint8
//		{
//			UnDefine	 = 0, /*!< 未定义 */
//			CommonCore	 = 1, /*!< 普通门芯 */
//			Louver_Core  = 2, /*!< 百叶门芯 */
//			Top_H		 = 3, /*!< 上横 */
//			Middle_H     = 4, /*!< 中横 */
//			Bottom_H	 = 5, /*!< 下横 */
//			Left_V		 = 6, /*!< 左竖 */
//			Middle_V	 = 7, /*!< 中竖 */
//			Right_V      = 8  /*!< 右竖 */
//		};
//
//		class FSectionalMaterial
//		{
//		public:
//			int32 Id;
//			int32 Type;
//			int32 DisplayMode; // 1 平铺 2 拉伸
//			FString PakUrl;					
//			FString OptimizeParm;
//			FString Md5;
//			float Width;
//			float Height;	
//			float Depth;
//			FString CacheFilePath;
//			bool IsShutter = false; /*!< 是否是百叶,只有是门芯的情况下，才考虑是否是百叶*/
//		public:
//			float GetCareSpace() const;
//		};	
//
//		class FSpaceRange
//		{
//		public:
//			float Min;
//			float Max;
//			float Current;
//		};		
//	public:
//		void SpawnActor(AActor * InParentActor);
//		void ParseInfoFromJson(const TSharedPtr<FJsonObject>& InJsonObject);
//		void SaveInfoToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);		
//		void GetResourceUrls(TArray<FString> &OutResourceUrls);		
//		void GetFileCachePaths(TArray<FString> &OutFileCachePaths);	
//		void DestoryActor();
//		TSharedPtr<FDoor> CloneDoor() const;
//		/**  根据模型尺寸，获取真正的深度值 */
//		float GetTrueDepth() const;
//	
//		/** 左竖 */
//		const TSharedPtr<FSectionalMaterial>  & GetLeftV() const { return SectionalMaterials[GetIndexByType(ESectionalMaterialType::Left_V)]; }
//		/** 中竖 */
//		const TSharedPtr<FSectionalMaterial>  & GetMiddleV() const { return SectionalMaterials[GetIndexByType(ESectionalMaterialType::Middle_V)]; }
//		/** 右竖 */
//		const TSharedPtr<FSectionalMaterial>  & GetRightV() const { return SectionalMaterials[GetIndexByType(ESectionalMaterialType::Right_V)]; }
//		/** 上横 */
//		const TSharedPtr<FSectionalMaterial>  & GetTopH() const { return SectionalMaterials[GetIndexByType(ESectionalMaterialType::Top_H)]; }
//		/** 中横 */
//		const TSharedPtr<FSectionalMaterial>  & GetMiddleH() const { return SectionalMaterials[GetIndexByType(ESectionalMaterialType::Middle_H)]; }
//		/** 下横 */
//		const TSharedPtr<FSectionalMaterial>  & GetBottomH() const { return SectionalMaterials[GetIndexByType(ESectionalMaterialType::Bottom_H)]; }
//		/** 获取指定索引的门芯 */
//		const TSharedPtr<FSectionalMaterial>  & GetCore(const int32 Index) const;
//
//		/** 重新计算尺寸与坐标 */
//		void RecalSizeAndPosition();
//
//		/** 坐标与尺寸重置 */
//		void ResetDividPosition();
//	private:
//		TPair<int32, int32> GetIndexByType(const ESectionalMaterialType InType) const;
//		const  FString  GetCachePath(const int32 InSectionMaterialId,const FString & InMd5) const;
//	public:
//		TSharedPtr<FDividBlock> RootBlock;
//		TMap<TPair<int32,int32>, TSharedPtr<FSectionalMaterial>> SectionalMaterials;
//		int32 Id;
//		FString Name;
//		FString Code;
//		int32 Type;
//		FSpaceRange WidhtRange;
//		FSpaceRange HeightRange;
//		FSpaceRange DepthRange;
//		bool IsInner = false;
//	};
//
//	/** 上下垫板 */
//	class FPlateBoard
//	{
//	public:
//		void SpawnActor(AActor * InParentActor);
//		void ParseInfoFromJson(const TSharedPtr<FJsonObject>& InJsonObject,FSCTShape * InParentShape);
//		void SaveInfoToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
//		void GetResourceUrls(TArray<FString> &OutResourceUrls);
//		void GetFileCachePaths(TArray<FString> &OutFileCachePaths);
//		TSharedPtr<FPlateBoard> ClonePlateBoard(FSCTShape * InParentShape);
//		void ReCalPlateBoardPositionAndSize(const float InDoorWidth, const float InDoorHeight, float & OutZCullValue, float & OutZIncreaseValue,
//			float & OutZDecreaseValue,float & OutYIncreaseValue,float & OutTopYIncreaseVlue,float & OutBottomIncreaseValue );
//		void SetVisiable(const bool bInVisiable = true);
//		bool GetVisiable() const { return bShow; }
//	public:
//		float Depth = 0.0f;
//		int32 SubstrateId = 0;
//		float Height = 0.0f;
//		FDoorPakMetaData Material;
//		FDoorPakMetaData EdgeBanding;
//		FDoorPakMetaData Substrate;
//		TSharedPtr<FBoardShape> BoardShape;
//		bool IsTopBoard = false;
//		bool bShow = true;
//	};
//public:
//	SlidingDoor() = default;
//	virtual ~SlidingDoor();
//public:
//	/** 解析JSon文件创建型录 */
//	virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
//	virtual void ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
//	virtual void ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
//	virtual void ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
//
//	/** 将型录转存为JSon文件 */
//	virtual void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
//	virtual void SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
//	virtual void SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
//	virtual void SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
//public:
//
//	/** 拷贝创建新的型录 */
//	virtual void CopyTo(FSCTShape* OutShape) override;
//
//	/** 创建型录Actor */
//	virtual ASCTShapeActor* SpawnShapeActor() override;
//
//	/** 添加子对象 */
//	void AddBaseChildShape(const TSharedPtr<FSCTShape> &InShape) {}
//
//	/** 设置ProfileName */
//	virtual void SetCollisionProfileName(FName InProfileName) override;
//
//	/** 获取型录中的所有资源Url */
//	virtual void GetResourceUrls(TArray<FString> &OutResourceUrls) override;
//
//	/** 获取型录中的所有资源路径 */
//	virtual void GetFileCachePaths(TArray<FString> &OutFileCachePaths) override;
//
//public:
//	
//	/** 首扇门内外信息 */
//	int32 GetFirstDoorPosition() const { return FirstDoorPosition; }
//	void SetFirstDoorPosition(const int32 InFirstDoorPosition) { FirstDoorPosition = InFirstDoorPosition; }
//	
//	/** 交错方式 */
//	int32 GetDoorInterleavingMode() const { return DoorInterleavingMode; }
//	void SetDoorInterleavintMode(const int32 InDoorInterleavingMode) { DoorInterleavingMode = InDoorInterleavingMode; }
//
//	/** 重叠宽度 */
//	int32 GetOverlapWidth() const { return OverlapWidth; }
//	void SetOverlapWidth(const int32 InOverlapWidth) { OverlapWidth = InOverlapWidth; }
//
//	/** 获取内移值 */
//	int32 GetIntroskipValue() const { return IntroskipValue; }
//	void SetIntroskipValue(const int32 InIntroskipValue) { IntroskipValue = InIntroskipValue; }
//
//	/** 掩盖方式 */
//	int32 GetConverUpWay() const { return ConverUpWay; }
//	void SetConverUpWay(const int32 InConverUpWay) { ConverUpWay = InConverUpWay; }
//
//	/** 基材 */
//	int32 GetSubstrateType() { return SubstrateType; }
//	void SetSubstrateType(const int32 InSubstrateType) { SubstrateType = InSubstrateType; }
//	const FString & GetSubstrateName() { return SubstrateName; }
//	const void SetSubstrateName(const FString & InSubstrateName) { SubstrateName = InSubstrateName; }
//
//	/** 是否存在上下垫板 */
//	bool HasBottomPlateBoard() const;
//	bool HasTopPlateBoard() const ;	
//	void GetTopPlateBoardShowInfo(FString & OutSubstrateName, FString & OutMaterialName, FString & OutEdgeBaingingName,
//		float & OutW, float & OutDepth,TArray<float> & HegithList,float & OutCurHeight);
//	void GetBottomPlateBoardShowInfo(FString & OutSubstrateName, FString & OutMaterialName, FString & OutEdgeBaingingName,
//		float & OutW, float & OutDepth, TArray<float> & HegithList, float & OutCurHeight);
//	/** 获取上垫板高度 */
//	float GetTopPlateBoardHeight() const;
//	/** 获取下垫板高度 */
//	float GetBottomPlateBoardHeight() const;
//
//	/** 设置上下垫板的显示与隐藏 */
//	void SetTopPlateBoardVisiable(const bool bInShow = true);
//	void SetBottomPlateBoardVisiable(const bool bInShow = true);
//private:
//	/** 重新计算尺寸与坐标 */	
//	virtual void ReCalDividBlocksPositionAndSize() override;
//	void DestoryActor();
//	TSharedPtr<FPlateBoard> GetTopPlateBoard();
//	TSharedPtr<FPlateBoard> GetBottomPlateBoard();
//private:
//	TArray<TSharedPtr<FDoor>> Doors;	
//	int32 FirstDoorPosition		= 0; /*!< 首扇门的位置，1 内，2外 */
//	int32 DoorInterleavingMode  = 0; /*!< 门的交错方式 1 内外交错，2对称交错*/
//	int32 OverlapWidth		    = 0; /*!< 重叠位宽度*/
//	int32 IntroskipValue        = 0; /*!< 内移值 */
//	int32 OriginDoorCount       = 0; /*!< 门组内门的个数*/
//	TArray<TSharedPtr<FPlateBoard>> PlateBoards; /*!< 上下垫板*/
//	TArray<TSharedPtr<FAccessoryShape>>	SlideWays;	/*!< 滑轨*/
//	int32 ConverUpWay;				/*!< 掩盖方式，1 内嵌，2外盖 默认设置来源于企业工艺设置 */
//	int32  SubstrateType = 0;		/*!< 基材类型 */
//	FString SubstrateName;			/*!< 基材名称 */
//};
//
