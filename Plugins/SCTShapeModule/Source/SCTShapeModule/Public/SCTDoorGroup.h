/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SCTDoorGroup.h
* @Description 门组类
*
* @Author 赵志强
* @Date 2018年10月31日
* @Version 1.0
*/
#pragma once


#include "SCTShape.h"
#include "SCTGlobalDataForShape.h"
#include "SCTDoorSheet.h"

class FSCTShape;
class FBoardShape;
class FAccessoryShape;
class FSpaceDividDoorSheet;
class FVariableAreaDoorSheet;

/** 门组基类 */
class SCTSHAPEMODULE_API FDoorGroup : public FSCTShape
{
public:
	FDoorGroup();
	virtual ~FDoorGroup();

public:
	/** 解析JSon文件创建型录 */
	virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;

	/** 将型录转存为JSon文件 */
	virtual void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	/** 拷贝创建新的型录 */
	virtual void CopyTo(FSCTShape* OutShape) override;

	/** 创建型录Actor */
	virtual ASCTShapeActor* SpawnShapeActor() override;
	/** 用于创建子型录用于点选的Actor */
	void SpawnActorsForSelected(FName InProfileName);
	/** 设置ProfileName */
	virtual void SetCollisionProfileName(FName InProfileName) override;

	/** 获取型录中的所有资源Url */
	virtual void GetResourceUrls(TArray<FString> &OutResourceUrls) override;
	/** 获取型录中的所有资源路径 */
	virtual void GetFileCachePaths(TArray<FString> &OutFileCachePaths) override;

	/** 显示、隐藏门组(递归处理子级空间) */
	virtual void ShowDoorSheetsActor();
	virtual void HideDoorSheetsActor();
	/** 隐藏门组中所有ShapeActors */
	virtual void HiddenDoorGroupActors(bool bHidden);
	virtual bool GetDoorGoupIsHIdden() const { return bIsDoorGroupHiddlen; }

public:
	/** 获取门组总共的厚度值 */
	virtual float GetDoorTotalDepth();

	/** 获取W、D、H的变化范围，同时返回当前值 */
	virtual float GetShapeWidthRange(float& MinValue, float& MaxValue);
	virtual float GetShapeDepthRange(float& MinValue, float& MaxValue);
	virtual float GetShapeHeightRange(float& MinValue, float& MaxValue);
	/** 判断输入空间尺寸值是否有效 */
	virtual bool IsValidForWidth(float InValue);
	virtual bool IsValidForDepth(float InValue);
	virtual bool IsValidForHeight(float InValue);

	/** 门组外延值 */
	float GetUpExtensionValue() { return UpExtensionValue; };
	float GetDownExtensionValue() { return DownExtensionValue; };
	float GetLeftExtensionValue() { return LeftExtensionValue; };
	float GetRightExtensionValue() { return RightExtensionValue; };
	virtual void SetUpExtensionValue(float InValue) {};
	virtual void SetDownExtensionValue(float InValue) {};
	virtual void SetLeftExtensionValue(float InValue) {};
	virtual void SetRightExtensionValue(float InValue) {};
public:
	const TArray<TSharedPtr<FAccessoryShape>> & GetMetals() const { return Metals; }
	TArray<TSharedPtr<FAccessoryShape>> & GetMetals() { return  Metals; }
private:
	/** 五金 */		
	void SetMetals(const TArray<TSharedPtr<FAccessoryShape>> & InMetals) { Metals = InMetals; }
public:
	/**通过名称以及类型，从备用五金中查找ID*/
	int32 GetOptionMetalsIdByNameAndType(const FString & InName, const EMetalsType InType);

	/** 是否存包含指定Id的五金*/
	bool IsContainSpecialMetal(const int32 InShapeId);
	
	/** 获取所有五金可替换项 */
	const TMap<int32, TSharedPtr<FSCTShape>> & GetOptionShapeMap()const { return OpetionsMetalsShapesList; }

protected:
	/** 对五金可选项进行列表分类 */
	virtual void ProcessOptionsMetalType() {}	

	void AddMetal(const TSharedPtr<FAccessoryShape> & InShape);

protected:
	TMap<int32, TSharedPtr<FSCTShape>> OpetionsMetalsShapesList;
	TArray<TSharedPtr<FAccessoryShape>>    Metals; /*!< 当前正在使用的五金 */	
	TArray<EMetalsType> FilterSpawnMetalType;
	TArray<EMetalsType> FilterParseMetalType;
private:
	TArray<FDoorSheetShapeBase::FAccessoryToDownload> ToDownloadAccessoryList;	/*!< 需要下载的五金列表*/
protected:
	// 门组外延值: 1上延，2下延，3左延，4右延
	float UpExtensionValue = 0.0;
	float DownExtensionValue = 0.0;
	float LeftExtensionValue = 0.0;
	float RightExtensionValue = 0.0;

	//用于点选的ProfileName
	FName BoundBoxProfileName;
	bool bIsDoorGroupHiddlen = false;
};

//移门组
class SCTSHAPEMODULE_API FSlidingDoor : public FDoorGroup
{
public:
	FSlidingDoor();
	virtual ~FSlidingDoor();

public:
	/** 解析JSon文件创建型录 */
	virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;

	/** 将型录转存为JSon文件 */
	virtual void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	/** 拷贝创建新的型录 */
	virtual void CopyTo(FSCTShape* OutShape) override;

	/** 创建型录Actor */
	virtual ASCTShapeActor* SpawnShapeActor() override;
	/** 设置ProfileName */
	virtual void SetCollisionProfileName(FName InProfileName) override;

	/** 获取型录中的所有资源Url */
	virtual void GetResourceUrls(TArray<FString> &OutResourceUrls) override;
	/** 获取型录中的所有资源路径 */
	virtual void GetFileCachePaths(TArray<FString> &OutFileCachePaths) override;

	/** 显示、隐藏门组(递归处理子级空间) */
	virtual void ShowDoorSheetsActor() override;
	virtual void HideDoorSheetsActor() override;
	/** 隐藏门组中所有ShapeActors */
	virtual void HiddenDoorGroupActors(bool bHidden) override;
	virtual bool GetDoorGoupIsHIdden() const override;

public:
	/** 设置门组宽度 */
	virtual bool SetShapeWidth(float InIntValue) override;
	virtual bool SetShapeWidth(const FString &InStrValue) override;
	/** 设置门组深度 */
	virtual bool SetShapeDepth(float InIntValue) override;
	virtual bool SetShapeDepth(const FString &InStrValue) override;
	/** 设置门组厚度 */
	virtual bool SetShapeHeight(float InIntValue) override;
	virtual bool SetShapeHeight(const FString &InStrValue) override;

public:
	/** 获取门组总共的厚度值 */
	virtual float GetDoorTotalDepth() override;

	/** 获取W、D、H的变化范围，同时返回当前值(门板数量不变的情况计算) */
	virtual float GetShapeWidthRange(float& MinValue, float& MaxValue) override;
	virtual float GetShapeDepthRange(float& MinValue, float& MaxValue) override;
	virtual float GetShapeHeightRange(float& MinValue, float& MaxValue) override;
	/** 判断输入空间尺寸值是否有效(门板数量可以改变) */
	virtual bool IsValidForWidth(float InValue) override;
	virtual bool IsValidForDepth(float InValue) override;
	virtual bool IsValidForHeight(float InValue) override;

	/** 门组外延值 */
	virtual void SetUpExtensionValue(float InValue) override;
	virtual void SetDownExtensionValue(float InValue) override;
	virtual void SetLeftExtensionValue(float InValue) override;
	virtual void SetRightExtensionValue(float InValue) override;

public:
	//获取所有门板
	const TArray<TSharedPtr<FSpaceDividDoorSheet>>& GetDoorSheets() { return DoorSheets; }

	/** 首扇门内外信息 */
	int32 GetFirstPosition() const;
	void SetFirstPosition(const int32 InPosition);

	/** 交错方式 */
	int32 GetInterleavingMode() const;
	void SetInterleavintMode(const int32 InMode);

	/** 重叠宽度 */
	int32 GetOverlapWidth() const { return OverlapWidth; }
	void SetOverlapWidth(const int32 InOverlapWidth);

	/** 轨道预设值 */
	float GetSlidewayPresetValue() const { return SlidewayPresetValue; }
	void SetSlidewayPresetValue(float InValue) { SlidewayPresetValue = InValue; }

	/** 掩盖方式 */
	int32 GetCorverType() const { return CorverType; }
	void SetCorverType(const int32 InType) { CorverType = InType; }

	/** 上下滑轨 */
	void  SetUpSlideway(TSharedPtr<FAccessoryShape> InSlideway);
	void  SetDownSlideway(TSharedPtr<FAccessoryShape> InSlideway);
	float GetUpSlideMeasure();
	float GetDownSlideMeasure();
	bool  ChangeSlidwayByTopSlidwayName(const FString & InTopSlidwayName);
	

	/** 获取所有移门顶底滑轨中上滑轨的名称以及当前正在使用的上滑轨名称*/
	void GetAllOptionTopSlidWay(TArray<FString> & OutTopSlidWayNameList, FString & OutCurTopSlideWayName);
	const TArray<TSharedPtr<FAccessoryShape>>  & GetAllAccessShapes() const { return AllValidSlideways; }

	/** 获取上下垫板 */
	void SetUpPlateBoard(TSharedPtr<FBoardShape> InBoard);
	TSharedPtr<FBoardShape> GetUpPlateBoard();
	void SetDownPlateBoard(TSharedPtr<FBoardShape> InBoard);
	TSharedPtr<FBoardShape> GetDownPlateBoard();
	/** 修改上下垫板厚度 */
	bool SetUpPlateBoardHeight(float InIndex);
	bool SetDownPlateBoardHeight(float InIndex);
	/** 设置上下垫板的显示与隐藏 */
	bool IsUpPlateBoardValid();
	void SetUpPlateBoardValid(bool bValid = true);
	bool IsDownPlateBoardValid();
	void SetDownPlateBoardValid(bool bValid = true);
	const TSharedPtr<FAccessoryShape> & GetUpSlideWayShape() const { return UpSlideWay; }
	const TSharedPtr<FAccessoryShape> &	GetDownSlideWayShape() const { return DownSlideWay; }


	/** 移门门板 */
	void InsertDoorSheet(TSharedPtr<FSpaceDividDoorSheet> InDoorSheet);
	void DeleteDoorSheetAt(int32 InIndex);

	/** 更新门板的尺寸与坐标 */
	void UpdateDoorGroup();

	/** 获取原始门板数量 */
	int32 GetOriginDoorSheetNum() const { return OriginDoorCount; }

	/** 获取可以设置的门板数量 -- 单一原始门板的情况下这个才有效 */
	const TArray<int32> & GetAvaliableDoorSheetNum() const { return AvaliableDoorSheetNum; };

	/**  设置期望的门板数量 */
	void SetExpectDoorSheetNum(const int32 InValue) { ExpectDoorSheetNum = InValue; }
	int32 GetExpectDoorSheetNum() const { return ExpectDoorSheetNum;}
	bool IsExpectDoorSheetNumValid() const { return ExpectDoorSheetNum != 0; }
	void ResetExpectDoorSheetnum() { ExpectDoorSheetNum = 0; }
	
private:
	/** 解析创建上下垫板 */
	FBoardShape* ParsePlateBoardShape(const TSharedPtr<FJsonObject>& InJsonObject);

	/** 根据Id获取指定的滑轨五金 */
	FAccessoryShape* GetSlidewayShapeByID(int32 InID);

	/** 计算指定索引门板位于外轨 */
	bool IsDoorSheetLocateOutside(int32 InIndex);

private:
	//移门门板
	TArray<TSharedPtr<FSpaceDividDoorSheet>> DoorSheets;
	// 门板定义数量
	int32 OriginDoorCount = 0;

	// 上下垫板
	bool bUpPlateBoardValid = true;
	TSharedPtr<FBoardShape> UpPlateBoard;
	bool bDownPlateBoardValid = true;
	TSharedPtr<FBoardShape> DownPlateBoard;

	// 上下滑轨
	TSharedPtr<FAccessoryShape>	UpSlideWay;
	TSharedPtr<FAccessoryShape>	DownSlideWay;

	// 首扇门的位置，1 内，2外
	int32 FirstPosition = 0;
	// 门的交错方式 1 内外交错，2对称交错
	int32 InterleavingMode = 0;
	// 重叠位宽度
	float OverlapWidth = 0;

	// 轨道预设值（用于柜体内部尺寸修改）
	float SlidewayPresetValue = 0.0;

	// 掩盖方式，1 内嵌，2外盖 默认设置来源于企业工艺设置
	int32 CorverType;

	TArray<int32> AvaliableDoorSheetNum; /*!< 可选的开门方向*/
private:
	//可替换滑轨
	TArray<TPair<int32, int32>> SlidWayGroups;
	TArray<TSharedPtr<FAccessoryShape>> AllValidSlideways;
	int32 ExpectDoorSheetNum = 0;	/*!< 预期的门板数量*/

	//上下垫板的厚度和深度（添加垫板时计算）
	float UpPlateHeight = 0.0;
	float UpPlateDepth = 0.0;
	float DownPlateHeight = 0.0;
	float DownPlateDepth = 0.0;

	//上下滑轨的减尺以及深度和厚度（添加滑轨时计算）
	float UpSlideMeasure = 0.0;
	float UpSlideDepth = 0.0;
	float UpSlideHeight = 0.0;
	float DownSlideMeasure = 0.0;
	float DownSlideDepth = 0.0;
	float DownSlideHeight = 0.0;	
	TSharedPtr<FString> SrcJsonStr = nullptr;
};


//掩门组
class FTextSelectAttri;
class FNumberRangeAttri;
class SCTSHAPEMODULE_API FSideHungDoor : public FDoorGroup
{
public:
	enum class EFrontBoardType : uint8
	{
		E_None = 0,						   /*!< 未设置*/
		E_JustFrontBoard = 1,			   /*!< 只有前封板*/
		E_FrontBoardAndAssitBoard = 2	   /*!< 既有封板也有帮板*/
	};

	enum class EFrontBoardPositionType : uint8
	{
		E_None = 0,					/*!< 未设置 */
		E_DoorLeft = 1,				/*!< 门左边*/
		E_DoorRight = 2             /*!< 门右边*/
	};

	enum class EFrontBoardCornerType : uint8
	{
		E_None = 0,					/*!< 无切角 */
		E_RithtTopCorner = 1,		/*!< 右上切角*/
		E_LeftTopCorner = 2         /*!< 左上切角*/
	};

	enum class EFrontBoardLinkageRule : uint8
	{
		E_Linkage_JustDoor = 0,			   /*!< 仅掩门变化 */
		E_Linkage_JustFrontBoard = 1,	   /*!< 仅前封板变化 */		
		E_Linkeage_DoorAndForntBoard = 2   /*!< 掩门以及前封板联动 */
	};

	enum class EHandleMetalType : int8
	{
		E_MT_None = -1,
		E_MT_SHAKE_HANDS = 0,				/*!< 明装拉手*/
		E_MT_LATERAL_BOX_SHAKE_HAND = 1,	/*!< 箱体拉手*/
		E_MT_SEALING_SIDE_HANDLE = 2		/*!< 封边拉手*/
	};

	enum class ELateralBoxShakeHandSubType : int8
	{
		E_MT_None = -1,
		E_LBSH_WALL_CUPBOARD_HANDLE  = 0,	/*!< 吊柜下拉手*/
		E_LBSH_FLOOR_CUPBOARD_HANDLE = 1,	/*!< 地柜上拉手*/
		LBSH_C_HANDL = 2					/*!< 横装箱体拉手*/
	};

	enum class EDoorOpenType : uint8
	{
		E_HingedDoor , /*!< 平开门*/
		E_TipUpDoor    /*!< 上翻门*/
	};
public:
	FSideHungDoor();
	virtual ~FSideHungDoor();

public:
	/** 掩门可变区域门板 */
	class FVariableAreaDoorSheetForSideHungDoor final : public  FVariableAreaDoorSheet
	{
	public:
		FVariableAreaDoorSheetForSideHungDoor();
		virtual ~FVariableAreaDoorSheetForSideHungDoor() = default;
		FVariableAreaDoorSheetForSideHungDoor(const FVariableAreaDoorSheetForSideHungDoor &) = delete;
		FVariableAreaDoorSheetForSideHungDoor & operator =(const FVariableAreaDoorSheetForSideHungDoor &) = delete;
	public:
		virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override { FVariableAreaDoorSheet::ParseFromJson(InJsonObject); SetShapeType(ST_VariableAreaDoorSheet_ForSideHungDoor); }
		virtual void ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override { FVariableAreaDoorSheet::ParseShapeFromJson(InJsonObject); SetShapeType(ST_VariableAreaDoorSheet_ForSideHungDoor); }
		/** 解析JSon文件创建型录 */
		virtual void ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
		/** 将型录转存为JSon文件 */
		virtual void SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

		/** 拷贝创建新的型录 */
		virtual void CopyTo(FSCTShape* OutShape);

		virtual void  ReCalDividBlocksPositionAndSize() override;
		virtual ASCTShapeActor* SpawnShapeActor() override;
	public:

		/** 拉手*/
		const FAuxiliaryMetalInfo & GetCurrentHandleInfo() const { return CurrentHandleLocationInfo; }
		FAuxiliaryMetalInfo & GetCurrentHandleInfo() { return CurrentHandleLocationInfo; }
		void SetCurrentHandleInfo(const FAuxiliaryMetalInfo & InInfo) { CurrentHandleLocationInfo = InInfo; RecalHandlePosition(); }
		const TArray<FAuxiliaryMetalInfo> & GetAuxiliaryHandleList() const { return HandleList; }

		/** 铰链拉手*/
		const FAuxiliaryMetalInfo & GetCurrentHingeleInfo() const { return CurrentHingeLocationInfo; }
		FAuxiliaryMetalInfo & GetCurrentHingeleInfo() { return CurrentHingeLocationInfo; }
		void SetCurrentHingeInfo(const FAuxiliaryMetalInfo & InInfo) { CurrentHingeLocationInfo = InInfo; }
		const TArray<FAuxiliaryMetalInfo> & GetAuxiliaryHingeList() const { return HingeList; }

		/** 重新生成拉手*/
		SCTSHAPEMODULE_API void ReBuildHandle();
		/** 重新计算拉手的位置 */
		SCTSHAPEMODULE_API void RecalHandlePosition();
		/** 重新生成铰链*/
		SCTSHAPEMODULE_API void ReBuildHingele();

		/** 获取可视化门板尺寸，如果有封边拉手，则将会把封边拉手的尺寸考虑进来*/
		SCTSHAPEMODULE_API virtual FVector GetDoorSheetVisualSize() override;
		
	private:
		/** 提取拉手的五金 */
		virtual void ProcessOptionsMetalType() override;
	private:

		// 可用拉手列表
		TArray<FAuxiliaryMetalInfo> HandleList;
		// 默认把手位置信息
		FAuxiliaryMetalInfo CurrentHandleLocationInfo;

		// 可用铰链列表
		TArray<FAuxiliaryMetalInfo> HingeList;
		FAuxiliaryMetalInfo CurrentHingeLocationInfo;
	};

public:
	/** 解析JSon文件创建型录 */
	virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;

	/** 将型录转存为JSon文件 */
	virtual void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	/** 拷贝创建新的型录 */
	virtual void CopyTo(FSCTShape* OutShape) override;

	/** 创建型录Actor */
	virtual ASCTShapeActor* SpawnShapeActor() override;
	/** 设置ProfileName */
	virtual void SetCollisionProfileName(FName InProfileName) override;

	/** 获取型录中的所有资源Url */
	virtual void GetResourceUrls(TArray<FString> &OutResourceUrls) override;
	/** 获取型录中的所有资源路径 */
	virtual void GetFileCachePaths(TArray<FString> &OutFileCachePaths) override;

	/** 显示、隐藏门组(递归处理子级空间) */
	virtual void ShowDoorSheetsActor() override;
	virtual void HideDoorSheetsActor() override;
	/** 隐藏门组中所有ShapeActors */
	virtual void HiddenDoorGroupActors(bool bHidden) override;
	virtual bool GetDoorGoupIsHIdden() const override;

public:
	/** 设置门组宽度 */
	virtual bool SetShapeWidth(float InIntValue) override;
	virtual bool SetShapeWidth(const FString &InStrValue) override;
	/** 设置门组深度 */
	virtual bool SetShapeDepth(float InIntValue) override;
	virtual bool SetShapeDepth(const FString &InStrValue) override;
	/** 设置门组厚度 */
	virtual bool SetShapeHeight(float InIntValue) override;
	virtual bool SetShapeHeight(const FString &InStrValue) override;

public:
	/** 获取门组总共的厚度值 */
	virtual float GetDoorTotalDepth() override;

	/** 获取门板尺寸变化范围(门板数量不变的情况计算) */
	float GetDoorSheetWidthRange(float& MinValue, float& MaxValue);
	/** 获取W、D、H的变化范围，同时返回当前值(门板数量不变的情况计算) */
	virtual float GetShapeWidthRange(float& MinValue, float& MaxValue) override;
	virtual float GetShapeDepthRange(float& MinValue, float& MaxValue) override;
	virtual float GetShapeHeightRange(float& MinValue, float& MaxValue) override;
	/** 判断门板尺寸是否有效(门板数量可以改变) */
	bool IsValidForSheetWidth(float InValue);
	/** 判断输入空间尺寸值是否有效(门板数量可以改变) */
	virtual bool IsValidForWidth(float InValue) override;
	virtual bool IsValidForDepth(float InValue) override;
	virtual bool IsValidForHeight(float InValue) override;

	/** 门组外延值 */
	virtual void SetUpExtensionValue(float InValue) override;
	virtual void SetDownExtensionValue(float InValue) override;
	virtual void SetLeftExtensionValue(float InValue) override;
	virtual void SetRightExtensionValue(float InValue) override;

	/** 封板设置 */
	void SetFrontBoardType(const EFrontBoardType InType);
	EFrontBoardType GetFrontBoardType() const { return FrontBoardType; }

	/** 封板位置类型 */
	void SetFrontBoardPositionType(const EFrontBoardPositionType InType) { FrontPositionType = InType; }
	EFrontBoardPositionType GetFrontBoardPositionType() const { return FrontPositionType; }

	/** 封板切角设置 */
	void SetFrontBoardCornerType(const EFrontBoardCornerType InType);
	EFrontBoardCornerType GetFrontBoardCornerType() const { return FrontBoardCornerType; }

	/** 前封板基材 */
	void SetFrontBoardSubstrate(const int32 InValue) { FrontBoardSubsrate = InValue; }
	int32 GetFrontBoardSubstrate() const { return FrontBoardSubsrate; }

	/** 封板封边 */
	const int32 GetFrontBoardEdgeBanding() const { return FrontBoardEdgeBanding; }
	void SetFrontBoardEdgeBanding(const int32 InEdgeBanding) { FrontBoardEdgeBanding = InEdgeBanding; }

		
	/** 前封板宽度 */
	float GetFrontBoardWidth() const { return FrontBoardWidth; }
	bool SetFrontBoardWidth(const float InValue);

	/** 前封板高度 */
	float GetFrontBoardHeight() const { return const_cast<FSideHungDoor*>(this)->GetShapeHeight(); }

	/** 前封板深度*/
	float GetFrontBoardDepth() const { return FrontBoardDepth; }
	bool SetFrontBoardDepth(const float InValue);

	/** 前封板上延*/
	float GetFrontBoardTopExtern() const { return FrontBoardTopExtern; }
	bool SetFrontBoardTopExtern(const float InValue);

	/** 前封板下延 */
	float GetFrontBoardBottomExtern() const { return FrontBoardBottomExtern; }
	bool SetFrontBoardBottomExtern(const float InValue);

	/** 前封板切角宽度*/
	float GetFrontBoardCornerCutWidth() { return FrontBoardCornerCutWidth; }
	bool SetFrontBoardCornerCutWidth(const float InValue);

	/** 前封板最小宽度 */
	float GetFrontBoardMinWidth() const { return FrontBoardMinWidth; }
	void SetFrontBoardMinWidth(const float InValue) { FrontBoardMinWidth = InValue; }

	/** 前封板切角高度 */
	float GetFrontBoardCornerCutHeight() const { return FrontBoardCornerCutHeight; }
	bool SetFrontBoardCornerCutHeight(const float InValue);

	/** 帮板宽度 */
	float GetAssistBoardWidth() const { return AssistBoardWith; }
	bool SetAssistBoardWidth(const float InValue);

	/** 帮板高度 */
	float GetAssistBoardHeight() const { return const_cast<FSideHungDoor*>(this)->GetShapeHeight(); }
	bool SetAssistBoardHeight(const float InValue);

	/** 帮板深度*/
	float GetAssistBoardDepth() const { return AsssistBoardDepth; }
	bool SetAssistBoardDepth(const float InValue);

	/** 帮板上延 */
	float GetAssistBoardTopExtern() const { return AssistBoardTopExtern; }
	bool SetAssistBoardTopExtern(const float InValue);

	/** 帮板下延 */
	float GetAssistBoardBottomExtern() const { return AssistBoardBottomExtern; }
	bool SetAsssistBoardBottomExtern(const float InValue);

	/** 帮板距封板 */
	float GetAssistBoardLengthToFrontBoard() const { return AssistBoardLengthToFrontBoard; }
	bool SetAssistBoardLengthToFrontBoard(const float InValue);

	/** 柜体变化时候，掩门以及前封板的联动规则 */
	EFrontBoardLinkageRule GetFrontBoardLinkageRule() const { return FrontBoardLinkageRule; }
	void SetFrontBoardLinkageRule(const EFrontBoardLinkageRule InLinkageRule) { FrontBoardLinkageRule = InLinkageRule; }

	/** 前封板材质 */
	const FDoorSheetShapeBase::FDoorPakMetaData & GetFrontBoardMaterial() const { return FrontBoardMaterial; }	
	void SetFrontBoardMaterial(const FDoorSheetShapeBase::FDoorPakMetaData & InMaterialData);

	/** 帮板材质 */
	const FDoorSheetShapeBase::FDoorPakMetaData & GetAssistBoardMaterial() const { return AssistBoardMaterial; }
	void SetAssistBoardMaterial(const FDoorSheetShapeBase::FDoorPakMetaData & InMaterialData);

	/** 帮板基材 */	
	void SetAssistBoardSubstrate(const int32 InValue) { AssistBoardSubstrate = InValue; }
	int32 GetAssistBoardSubstrate() const { return AssistBoardSubstrate; }

	/** 帮板封边 */
	const int32 GetAssistBoardEdgeBanding() const { return AssistBoardEdgeBanding; }
	void SetAssistBoardEdgeBanding(const int32 InEdgeBanding) { AssistBoardEdgeBanding = InEdgeBanding; }

public:
	/** 掩门单开门、对开门方式: 1 单开门，2 是对开门*/
	bool SetDoorSheetNum(int32 InType);
	/** 获取掩门门板数量（标识单开门、对开门） */
	int32 GetDoorSheetNum() const { return DoorSheets.Num(); }

	EDoorOpenType GetDoorOpenType() const;

	/** 对开门宽度比例 */
	bool SetDoorWidthScales(TArray<float> InWidthScales);

	/** 门缝值 */
	bool SetUpDownGapValue(float InValue);
	float GetUpDownGapValue() const { return UpDownGap; }
	bool SetLeftRightGapValue(float InValue);
	float GetLeftRightGapValue() const { return LeftRightGap; }
	bool SetBackGapValue(float InValue);
	float GetBackGapValue() const { return BackGap; }

	/** 四边掩盖方式 */
	int32 GetUpCorverType() { return UpCorverType; };
	void SetUpCorverType(int32 InType);
	int32 GetDownCorverType() { return DownCorverType; };
	void SetDownCorverType(int32 InType);
	int32 GetLeftCorverType() { return LeftCorverType; };
	void SetLeftCorverType(int32 InType);
	int32 GetRightCorverType() { return RightCorverType; };
	void SetRightCorverType(int32 InType);

	/** 掩门门板 */
	void InsertDoorSheet(TSharedPtr<FVariableAreaDoorSheetForSideHungDoor> InDoorSheet);
	void DeleteDoorSheetAt(int32 InIndex);

	/** 更新门板的尺寸与坐标 */
	void UpdateDoorGroup();

	TArray<TSharedPtr<FVariableAreaDoorSheetForSideHungDoor>> & GetDoorSheets() { return DoorSheets; }
	const TArray<TSharedPtr<FVariableAreaDoorSheetForSideHungDoor>> & GetDoorSheets() const { return DoorSheets; }

	/** 设置拉手类型 */
	EHandleMetalType GetHanleType() const { return HanleType; }
	bool SetHanleType(const EHandleMetalType InHanleType);

public:
	/** 掩门门组拉手*/
	const FDoorSheetShapeBase::FAuxiliaryMetalInfo & GetCurrentHandleInfo() const { return CurrentHandleLocationInfo; }
	FDoorSheetShapeBase::FAuxiliaryMetalInfo & GetCurrentHandleInfo() { return CurrentHandleLocationInfo; }
	void SetCurrentHandleInfo(const FDoorSheetShapeBase::FAuxiliaryMetalInfo & InInfo) { CurrentHandleLocationInfo = InInfo; }
	const TArray<FDoorSheetShapeBase::FAuxiliaryMetalInfo> & GetAuxiliaryHandleList() const { return HandleList; }

	/* 设置箱体拉手Z方向相对门板的偏移量*/
	bool SetHanleZValue(const float InValue);

	void ReBuildHandle(const int32 InID);
private:
	// 重置前封板
	void ReSetFrontBoard();
	// 设置前封板
	void SetFrontBoard();
	// 重置帮板
	void ReSetAssistBoard();
	// 设置帮板
	void SetAssistBoard();
	// 处理五金
	virtual void ProcessOptionsMetalType() override;
	/** 根据门组宽度自动调整门板数量 */
	bool AutoModifyDoorSheetCountByDoorGroupWidth();
	/** 获取门单个板范围以及当前尺寸 */
	void GetSingleDoorSheetRange(float & OutMin, float & OutMax, float OutCur);
	/** 判断指定宽度的情况下是否满足单开门对开门*/
	void IsSingleAndDoubleValid(const float InW,bool & OutSingleDoorSheet,bool & OutDoubleSheet);
private:
	//掩门门板
	TArray<TSharedPtr<FVariableAreaDoorSheetForSideHungDoor>> DoorSheets;
	// 门板尺寸比例
	TArray<float> DoorWidthScales = {1.0, 1.0};
	// 门板定义数量
	int32 OriginDoorCount = 0;

private:
	//四边掩盖方式:0-未定义，1-全盖，2-半盖，3-内嵌，4-平盖
	int32 UpCorverType = 0;
	int32 DownCorverType = 0;
	int32 LeftCorverType = 0;
	int32 RightCorverType = 0;
	
	//门缝值
	float LeftRightGap = 0.0;
	float UpDownGap = 0.0;
	float BackGap = 0.0;

	// 插脚板相关属性设置
	TSharedPtr<FBoardShape>	 FrontBoard;
	EFrontBoardType FrontBoardType = EFrontBoardType::E_None;
	EFrontBoardPositionType FrontPositionType = EFrontBoardPositionType::E_DoorRight;
	EFrontBoardCornerType FrontBoardCornerType = EFrontBoardCornerType::E_None;
	EFrontBoardLinkageRule FrontBoardLinkageRule = EFrontBoardLinkageRule::E_Linkage_JustFrontBoard;
	int32 FrontBoardSubsrate = -1;	
	int32 FrontBoardEdgeBanding = -1;
	float FrontBoardWidth = 0.0f;
	float FrontBoardMinWidth = 0.0f;
	float FrontBoardHeight = 0.0f;
	float FrontBoardDepth = 0.0f;
	float FrontBoardTopExtern = 0.0f;
	float FrontBoardBottomExtern = 0.0f;
	float FrontBoardCornerCutWidth = 0.0f;
	float FrontBoardCornerCutHeight = 0.0f;
	FDoorSheetShapeBase::FDoorPakMetaData FrontBoardMaterial;
	TSharedPtr<FBoardShape>  AssistBoard;
	int32 AssistBoardSubstrate = -1;
	int32 AssistBoardEdgeBanding = -1;
	float AssistBoardWith = 0.0f;
	float AssistBoardHeight = 0.0f;
	float AsssistBoardDepth = 0.0f;
	float AssistBoardTopExtern = 0.0f;
	float AssistBoardBottomExtern = 0.0f;
	float AssistBoardLengthToFrontBoard = 0.0f;
	FDoorSheetShapeBase::FDoorPakMetaData AssistBoardMaterial;
private:
	// 可用拉手列表
	TArray<FDoorSheetShapeBase::FAuxiliaryMetalInfo> HandleList;
	// 默认把手位置信息
	FDoorSheetShapeBase::FAuxiliaryMetalInfo CurrentHandleLocationInfo;
	ELateralBoxShakeHandSubType BoxHanleSubType = ELateralBoxShakeHandSubType::E_MT_None;
	EHandleMetalType HanleType = EHandleMetalType::E_MT_None;
	
};

// 抽面门组
class UMaterialInterface;
class SCTSHAPEMODULE_API FDrawerDoorShape final : public FDoorGroup
{
public:
	enum class EHandleMetalType : int8
	{
		E_MT_None = -1,
		E_MT_SHAKE_HANDS = 0,				/*!< 明装拉手*/
		E_MT_LATERAL_BOX_SHAKE_HAND = 1,	/*!< 箱体拉手*/
		E_MT_SEALING_SIDE_HANDLE = 2		/*!< 封边拉手*/
	};

	enum class ELateralBoxShakeHandSubType : int8
	{
		E_MT_None = -1,
		E_LBSH_WALL_CUPBOARD_HANDLE = 0,	/*!< 吊柜下拉手*/
		E_LBSH_FLOOR_CUPBOARD_HANDLE = 1,	/*!< 地柜上拉手*/
		LBSH_C_HANDL = 2					/*!< 横装箱体拉手*/
	};
public:
	FDrawerDoorShape();
	FDrawerDoorShape(const FDrawerDoorShape &) = delete;
	FDrawerDoorShape & operator =(const FDrawerDoorShape &) = delete;
	virtual ~FDrawerDoorShape() = default;

public:
	/** 抽面可变区域门板 */
	class FVariableAreaDoorSheetForDrawer final : public  FVariableAreaDoorSheet
	{
	public:
		FVariableAreaDoorSheetForDrawer();
		virtual ~FVariableAreaDoorSheetForDrawer() = default;
		FVariableAreaDoorSheetForDrawer(const FVariableAreaDoorSheetForDrawer &) = delete;
		FVariableAreaDoorSheetForDrawer & operator =(const FVariableAreaDoorSheetForDrawer &) = delete;
	public:
		/** 解析JSon文件创建型录 */
		virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
		virtual void ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
		virtual void ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
		virtual void ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;

		/** 将型录转存为JSon文件 */
		virtual void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
		virtual void SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
		virtual void SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
		virtual void SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

		virtual ASCTShapeActor* SpawnShapeActor() override;

		/** 获取型录中的所有资源Url */
		virtual void GetResourceUrls(TArray<FString> &OutResourceUrls);

		/** 获取型录中的所有资源路径 */
		virtual void GetFileCachePaths(TArray<FString> &OutFileCachePaths);

		/** 拷贝创建新的型录 */
		virtual void CopyTo(FSCTShape* OutShape);

		virtual void ReCalDividBlocksPositionAndSize() override;

		/** 更新材质-- 抽面材质不进行旋转 */
		virtual void UpdateMaterial(UMaterialInterface * InMaterialInterface);
	public:
		int32 GetOpenDirection() const { return 0; }
		/** 获取默认锁 */
		FAuxiliaryMetalInfo GetDefaultLockInfo() const;
		void SetDefaultLockInfo(const FAuxiliaryMetalInfo & InInfo) { DefaultLockLocationInfo = InInfo; RecalLockPosition(); }

		/** 获取默认拉手*/
		SCTSHAPEMODULE_API FAuxiliaryMetalInfo  GetDefaultHandleInfo() const;
		void SetDefaultHandleInfo(const FAuxiliaryMetalInfo & InInfo) { DefaultHandleLocationInfo = InInfo; }

		/** 获取可用锁列表 */
		const TArray<FAuxiliaryMetalInfo> & GetAuxiliaryLockList() const { return LockList; }

		/** 获取可用拉手列表 */
		const TArray<FAuxiliaryMetalInfo> & GetAuxiliaryHandleList() const { return HandleList; }

		TSharedPtr<FNumberRangeAttri> GetHandleAttriX() { return HandleAttriX; }
		bool SetHandlePosX(const FString &InStrValue);
		bool SetHandlePosX(float InIntValue) { return SetHandlePosX(FString::Printf(TEXT("%f"), InIntValue)); }
		TSharedPtr<FNumberRangeAttri> GetHandleAttriZ() { return HandleAttriZ; }
		bool SetHandlePosZ(const FString &InStrValue);
		bool SetHandlePosZ(float InIntValue) { return SetHandlePosZ(FString::Printf(TEXT("%f"), InIntValue)); }
		TSharedPtr<FNumberRangeAttri> GetLockAttriX() { return LockAttriX; }
		bool SetLockPosX(const FString &InStrValue);
		bool SetLockPosX(float InIntValue) { return SetLockPosX(FString::Printf(TEXT("%f"), InIntValue)); }
		TSharedPtr<FNumberRangeAttri> GetLockAttriZ() { return LockAttriZ; }
		bool SetLockPosZ(const FString &InStrValue);
		bool SetLockPosZ(float InIntValue) { return SetLockPosZ(FString::Printf(TEXT("%f"), InIntValue)); }

		/** 重新生成拉手或者锁 */
		void ReBuildHandleOrLock(const bool InIsHanle = true);

		TSharedPtr<FTextSelectAttri> GetHanleRotateDirectionAttri() { return HanleRotateDirection; }
		TSharedPtr<FTextSelectAttri> GetHanleHPosSelectAttri() { return HandlePosHSelectXAtt; }
		TSharedPtr<FTextSelectAttri> GetHandleVPosSelectAttri() { return HandlePosVSelectXAtt; }
		TSharedPtr<FTextSelectAttri> GetLockRotateDirectionAttri() { return LockRotateDirection; }
		TSharedPtr<FTextSelectAttri> GetLockHPosSelectAttri() { return LockPosHSelectXAtt; }
		TSharedPtr<FTextSelectAttri> GetLockVPosSelectAttri() { return LockPosVSelectXAtt; }
		TSharedPtr<FTextSelectAttri> GetHandleListSelectAttri() { return HandleListSelectAtt; }
		TSharedPtr<FTextSelectAttri> GetLockListSelectAttri() { return LockListSelectAtt; }
	public:
		/** 重新计算拉手的位置 */
		void SCTSHAPEMODULE_API RecalHandlePosition();
	
	private:
		/** 重新计算锁的位置*/
		void  RecalLockPosition();

		/** 提取拉手与锁的五金 */
		virtual void ProcessOptionsMetalType() override;
	private:

		// 可用的锁列表
		TArray<FAuxiliaryMetalInfo> LockList;
		// 可用拉手列表
		TArray<FAuxiliaryMetalInfo> HandleList;
		// 默认把手位置信息
		FAuxiliaryMetalInfo DefaultHandleLocationInfo;
		// 默认锁的位置信息
		FAuxiliaryMetalInfo DefaultLockLocationInfo;
		TSharedPtr<FNumberRangeAttri> HandleAttriX;
		TSharedPtr<FNumberRangeAttri> HandleAttriZ;
		TSharedPtr<FNumberRangeAttri> LockAttriX;
		TSharedPtr<FNumberRangeAttri> LockAttriZ;
		TSharedPtr<FTextSelectAttri>  HanleRotateDirection;
		TSharedPtr<FTextSelectAttri> HandlePosHSelectXAtt;
		TSharedPtr<FTextSelectAttri> HandlePosVSelectXAtt;
		TSharedPtr<FTextSelectAttri> LockRotateDirection;
		TSharedPtr<FTextSelectAttri> LockPosHSelectXAtt;
		TSharedPtr<FTextSelectAttri> LockPosVSelectXAtt;
		TSharedPtr<FTextSelectAttri> HandleListSelectAtt;
		TSharedPtr<FTextSelectAttri> LockListSelectAtt;
	};
public:
	/** 解析JSon文件创建型录 */
	virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;

	/** 将型录转存为JSon文件 */
	virtual void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	/** 拷贝创建新的型录 */
	virtual void CopyTo(FSCTShape* OutShape);

	/** 创建型录Actor */
	virtual ASCTShapeActor* SpawnShapeActor() override;
	/** 设置ProfileName */
	virtual void SetCollisionProfileName(FName InProfileName) override;

	/** 获取型录中的所有资源Url */
	virtual void GetResourceUrls(TArray<FString> &OutResourceUrls);
	/** 获取型录中的所有资源路径 */
	virtual void GetFileCachePaths(TArray<FString> &OutFileCachePaths);

	/** 隐藏门组中所有ShapeActors */
	virtual void HiddenDoorGroupActors(bool bHidden) override;

	virtual bool GetDoorGoupIsHIdden() const override;

public:
	/** 获取门组总共的厚度值 */
	virtual float GetDoorTotalDepth() override;

	/** 设置门组宽度 */
	virtual bool SetShapeWidth(float InIntValue) override;
	virtual bool SetShapeWidth(const FString &InStrValue) override;
	/** 设置门组深度 */
	virtual bool SetShapeDepth(float InIntValue) override;
	virtual bool SetShapeDepth(const FString &InStrValue) override;
	/** 设置门组厚度 */
	virtual bool SetShapeHeight(float InIntValue) override;
	virtual bool SetShapeHeight(const FString &InStrValue) override;

public:
	/** 门组外延值 */
	virtual void SetUpExtensionValue(float InValue) override;
	virtual void SetDownExtensionValue(float InValue) override;
	virtual void SetLeftExtensionValue(float InValue) override;
	virtual void SetRightExtensionValue(float InValue) override;

public:
	/** 抽面面板 */
	TSharedPtr<FVariableAreaDoorSheetForDrawer> GetDrawDoorSheet() { return Drawface; }
	void SetDrawerDoorSheet(TSharedPtr<FVariableAreaDoorSheetForDrawer> InShape);

	/** 门缝值 */
	bool SetUpDownGapValue(float InValue);
	float GetUpDownGapValue() const { return UpDownGap; }
	bool SetLeftRightGapValue(float InValue);
	float GetLeftRightGapValue() const { return LeftRightGap; }

	/** 更新门板的尺寸与坐标 */
	bool UpdateDoorGroup();

	/***/
	void ReCalDividBlocksPositionAndSize()
	{
		Drawface->SetShapeHeight(GetShapeHeight());
		Drawface->SetShapeWidth(GetShapeWidth());
	}

public:
	const  FString & GetSubstrateName() const { return Drawface->GetSubstrateName(); }
	const  FDoorSheetShapeBase::FDoorPakMetaData & GetEdgeBanding() const { return Drawface->GetEdgeBanding(); }
	/** 材质 */
	const FDoorSheetShapeBase::FDoorPakMetaData & GetMaterial() const { return Drawface->GetMaterial(); }
	FDoorSheetShapeBase::FDoorPakMetaData & GetMaterial() { return Drawface->GetMaterial(); }
	void SetMaterial(const FDoorSheetShapeBase::FDoorPakMetaData & InMaterial) { Drawface->SetMaterial(InMaterial); }

	void ReBuildHandleOrLock(const bool InIsHanle = true) { Drawface->ReBuildHandleOrLock(InIsHanle); }

	const int32 GetOpenDirection() const { return Drawface->GetOpenDirection(); }
	void UpdateMaterial(UMaterialInterface * InMaterialInterface) { Drawface->UpdateMaterial(InMaterialInterface); }

	/** 获取默认锁 */
	FDoorSheetShapeBase::FAuxiliaryMetalInfo GetDefaultLockInfo() const { return Drawface->GetDefaultLockInfo(); }
	void SetDefaultLockInfo(const FDoorSheetShapeBase::FAuxiliaryMetalInfo & InInfo) { Drawface->SetDefaultLockInfo(InInfo); }

	/** 获取默认拉手*/
	FDoorSheetShapeBase::FAuxiliaryMetalInfo GetDefaultHandleInfo() const {return Drawface->GetDefaultHandleInfo(); }
	void SetDefaultHandleInfo(const FDoorSheetShapeBase::FAuxiliaryMetalInfo & InInfo) { Drawface->SetDefaultHandleInfo(InInfo); }

	/** 获取可用锁列表 */
	const TArray<FDoorSheetShapeBase::FAuxiliaryMetalInfo> & GetAuxiliaryLockList() const { return Drawface->GetAuxiliaryLockList(); }

	/** 获取可用拉手列表 */
	const TArray<FDoorSheetShapeBase::FAuxiliaryMetalInfo> & GetAuxiliaryHandleList() const { return Drawface->GetAuxiliaryHandleList(); }


	/** 拉手与锁的相关UI设置 */
	TSharedPtr<FNumberRangeAttri> GetHandleAttriX() { return Drawface->GetHandleAttriX(); }
	bool SetHandlePosX(const FString &InStrValue) { return Drawface->SetHandlePosX(InStrValue); }
	bool SetHandlePosX(float InValue) { return Drawface->SetHandlePosX(InValue); }
	TSharedPtr<FNumberRangeAttri> GetHandleAttriZ() { return Drawface->GetHandleAttriZ(); }
	bool SetHandlePosZ(const FString &InStrValue) { return Drawface->SetHandlePosZ(InStrValue); }
	bool SetHandlePosZ(float InValue) { return Drawface->SetHandlePosZ(InValue); }
	TSharedPtr<FNumberRangeAttri> GetLockAttriX() { return Drawface->GetLockAttriX(); }
	bool SetLockPosX(const FString &InStrValue) { return Drawface->SetLockPosX(InStrValue); }
	bool SetLockPosX(float InValue) { return Drawface->SetLockPosX(InValue); }
	TSharedPtr<FNumberRangeAttri> GetLockAttriZ() { return Drawface->GetLockAttriZ(); }
	bool SetLockPosZ(const FString &InStrValue) { return Drawface->SetLockPosZ(InStrValue); }
	bool SetLockPosZ(float InValue) { return Drawface->SetLockPosZ(InValue); }

	TSharedPtr<FTextSelectAttri> GetHanleRotateDirectionAttri() { return Drawface->GetHanleRotateDirectionAttri(); }
	TSharedPtr<FTextSelectAttri> GetHanleHPosSelectAttri() { return Drawface->GetHanleHPosSelectAttri(); }
	TSharedPtr<FTextSelectAttri> GetHandleVPosSelectAttri() { return Drawface->GetHandleVPosSelectAttri(); }
	TSharedPtr<FTextSelectAttri> GetLockRotateDirectionAttri() { return Drawface->GetLockRotateDirectionAttri(); }
	TSharedPtr<FTextSelectAttri> GetLockHPosSelectAttri() { return Drawface->GetLockHPosSelectAttri(); }
	TSharedPtr<FTextSelectAttri> GetLockVPosSelectAttri() { return Drawface->GetLockVPosSelectAttri(); }
	TSharedPtr<FTextSelectAttri> GetHandleListSelectAttri() { return Drawface->GetHandleListSelectAttri(); }
	TSharedPtr<FTextSelectAttri> GetLockListSelectAttri() { return Drawface->GetLockListSelectAttri(); }

public:
	/** 抽面门组拉手*/
	const FDoorSheetShapeBase::FAuxiliaryMetalInfo & GetDoorGroupCurrentHandleInfo() const { return CurrentHandleLocationInfo; }
	FDoorSheetShapeBase::FAuxiliaryMetalInfo & GetDoorGroupCurrentHandleInfo() { return CurrentHandleLocationInfo; }
	void SetDoorGroupCurrentHandleInfo(const FDoorSheetShapeBase::FAuxiliaryMetalInfo & InInfo) { CurrentHandleLocationInfo = InInfo; }
	const TArray<FDoorSheetShapeBase::FAuxiliaryMetalInfo> & GetDoorGroupAuxiliaryHandleList() const { return HandleList; }

	/* 设置箱体拉手Z方向相对门板的偏移量*/
	bool SetDoorGroupHanleZValue(const float InValue);

	void ReBuildDoorGroupHandle(const int32 InID);

	/** 设置拉手类型 */
	EHandleMetalType GetDoorGroupHanleType() const { return HanleType; }
	bool SetDoorGroupHanleType(const EHandleMetalType InHanleType);
private:
	// 处理五金
	virtual void ProcessOptionsMetalType() override;
private:
	// 可用拉手列表
	TArray<FDoorSheetShapeBase::FAuxiliaryMetalInfo> HandleList;
	// 默认把手位置信息
	FDoorSheetShapeBase::FAuxiliaryMetalInfo CurrentHandleLocationInfo;
	ELateralBoxShakeHandSubType BoxHanleSubType = ELateralBoxShakeHandSubType::E_MT_None;
	EHandleMetalType HanleType = EHandleMetalType::E_MT_None;

private:
	TSharedPtr<FVariableAreaDoorSheetForDrawer> Drawface = nullptr;

	//门缝值
	float LeftRightGap = 2.0;
	float UpDownGap = 2.0;
	float BackGap = 1.0;
};


