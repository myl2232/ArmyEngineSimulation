#pragma once

#include "CoreMinimal.h"
#include "SCTShape.h"
#include "SCTShapeData.h"
#include "Json.h"


struct FSHandlePositionForType
{
protected:
	enum class EHanleType : uint8
	{
		E_None,
		E_MT_SHAKE_HANDS,				/*!< 明装拉手*/
		E_MT_LATERAL_BOX_SHAKE_HAND,	/*!< 箱体拉手*/
		E_MT_SEALING_SIDE_HANDLE		/*!< 封边拉手*/
	};
protected:
	FSHandlePositionForType(EHanleType InHanleType) :HanleType(InHanleType) {}
	EHanleType HanleType = EHanleType::E_None;
public:
	virtual bool IsValid() const = 0;
	virtual ~FSHandlePositionForType() = default;
};

/** 划分接口类 */
class IDoorSheetDivision
{
public:
	/** 通过解析Json来生成门板切分结构 */
	virtual void ParseInfoFromJson(const TSharedPtr<FJsonObject>& InJsonObject) = 0;

	/** 存储门组切分结构 */
	virtual void SaveInfoToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) = 0;

};

//门板基类
class FAccessoryShape;
class SCTSHAPEMODULE_API FDoorSheetShapeBase : public FSCTShape
{
public:

	/** 可用五金信息 */
	struct FAuxiliaryMetalInfo
	{
		FString Name;
		int32 ID = -1;
		TSharedPtr<FDoorSubWidgetDefaultInfoBase> Location;
	};

	struct FAccessoryToDownload
	{
		FString Url;
		FString Md5;
		int32 ModelID;
		float UpdateTime;
		FString OptimizeParam;
	};

	/* 拉手五金位置 */
	struct FHanleMetalDefaultLocation
	{
		int32 MetalType;
		FString MetalName;
		TSharedPtr<FDoorSubWidgetDefaultInfoBase> Location;
	};

	/** 门Pak资源 */
	struct FDoorPakMetaData : public FCommonPakData
	{
	};

public:
	FDoorSheetShapeBase() = default;
	virtual ~FDoorSheetShapeBase() = default;

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

	/** 获取型录中的所有资源Url */
	virtual void GetResourceUrls(TArray<FString> &OutResourceUrls) override;

	/** 获取型录中的所有资源路径 */
	virtual void GetFileCachePaths(TArray<FString> &OutFileCachePaths) override;

	virtual void SetCollisionProfileName(FName InProfileName) override;
public:
	virtual void ReCalDividBlocksPositionAndSize() = 0;

	/** 基材 */
	const int32 GetSubstrateID() const { return SubstrateID; }
	void SetSubstrateID(const int32 InSubstrateID) { SubstrateID = InSubstrateID; }
	const FString & GetSubstrateName() const { return SubstrateName; }
	void SetSubstrateName(const FString & InSubstrateName) { SubstrateName = InSubstrateName; }	

	/** 封边 */
	const FDoorSheetShapeBase::FDoorPakMetaData & GetEdgeBanding() const { return EdgeBanding; }
	FDoorSheetShapeBase::FDoorPakMetaData & GetEdgeBanding() { return EdgeBanding; }
	void SetEdgeBanding(const FDoorSheetShapeBase::FDoorPakMetaData & InEdgeBanding) { EdgeBanding = InEdgeBanding; }

	/** 材质 */
	const FDoorPakMetaData & GetMaterial() const { return Material; }
	FDoorPakMetaData & GetMaterial() { return Material; }
	void SetMaterial(const FDoorPakMetaData & InMaterial) { Material = InMaterial; }
	bool HasReplaceMaterial() const { return bHasOptionalMaterial; }

	/** 五金 */
	const TArray<TSharedPtr<FAccessoryShape>> & GetMetals() const { return Metals; }
	TArray<TSharedPtr<FAccessoryShape>> & GetMetals() { return  Metals; }
	void SetMetals(const TArray<TSharedPtr<FAccessoryShape>> & InMetals) { Metals = InMetals; }

	/**通过名称以及类型，从备用五金中查找ID*/
	int32 GetOptionMetalsIdByNameAndType(const FString & InName, const EMetalsType InType);

	/** 通过ID，从备用五金中查找五金的分类*/
	EMetalsType GetMetalTypeByID(const int32 InId);

	FString  GetMetalNameByID(const int32 InId);

	const TMap<int32, TSharedPtr<FSCTShape>> & GetOptionShapeMap()const { return OpetionsMetalsShapesList; }
	
protected:
	/** 对五金可选项进行列表分类 */
	virtual void ProcessOptionsMetalType() {}

	void AddMetal(const TSharedPtr<FAccessoryShape> & InShape);
	public:
		TArray<EMetalsType> FilterSpawnMetalType;
		TArray<EMetalsType> FilterParseMetalType;
private:	
	TArray<FAccessoryToDownload> ToDownloadAccessoryList;	/*!< 需要下载的五金列表*/
protected:
	TMap<int32, TSharedPtr<FSCTShape>> OpetionsMetalsShapesList;
	int32 SubstrateID;				 /*!< 基材ID */
	FString SubstrateName;	         /*!< 基材名称 */
	FDoorPakMetaData Material;		 /*!< 材质 */
	FDoorPakMetaData EdgeBanding;	 /*!< 材质 */									 
	TArray<TSharedPtr<FAccessoryShape>>    Metals; /*!< 当前正在使用的五金 */
	bool bHasOptionalMaterial = false;	/*!< 是否存在可替换材质 */	
	bool bSpawnMetalActor = true;	/*!< 是否生成五金Actor*/
};


/** 空间划分门板*/
class SCTSHAPEMODULE_API FSpaceDividDoorSheet  final : public  FDoorSheetShapeBase
{
public:
	FSpaceDividDoorSheet();
	virtual ~FSpaceDividDoorSheet() = default;
	FSpaceDividDoorSheet(const FSpaceDividDoorSheet &) = delete;
	FSpaceDividDoorSheet & operator =(const FSpaceDividDoorSheet &) = delete;
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

	virtual void SetCollisionProfileName(FName InProfileName) override;
public:
	/** 将所有型材中深度最大的值，作为门板的深度值 */
	float GetOrCalDoorTrueDepth();
private:
	/** 型材 */
	class FSectionalMaterial
	{
	public:
		struct FSectionSizeProsBase
		{
			float Depth = 0.0f;			/*!< 深度*/			
			bool IsValid() const { return !FMath::IsNearlyZero(Depth); }
		};
		/** 边框型材（上横，下横）尺寸属性信息 */
		struct FSideTopAndBottomHSecionSizePros final : public FSectionSizeProsBase
		{		
			float Height = 0.0f;
			float BayonetHeight = 0.0f;	/*!< 插槽高度*/
			float BayonetDepth = 0.0f;	/*!< 插槽深度*/			
		};
		/** 边框型材（左竖，右竖）尺寸属性信息 */
		struct FSideLeftAndRightVSectionPros final : public FSectionSizeProsBase
		{
			float Width = 0.0f;
			float BayonetHeight = 0.0f;	/*!< 插槽高度*/
			float BayonetDepth = 0.0f;	/*!< 插槽深度*/
		};
		/** 内部型材（中横，中竖）尺寸属性信息 */
		struct FInsideSectionSizePros final : public FSectionSizeProsBase
		{			
			float Height = 0.0f;
			float UpBayonetHeight = 0.0f;	    /*!< 上插槽高*/
			float DownBayonetHeight = 0.0f;  	/*!< 下插槽高*/
			float BayonetDepth = 0.0f;			/*!< 上插槽深*/		
		};	
		/** 属性类型 */
		enum class EProsType : uint8
		{
			ENone,
			ETopAndBottomH, /*!< 上下横*/
			ELeftAndRightV,	/*!< 左右竖*/
			EInside			/*!< 内方*/
		};
	public:
		enum class ESectionalMaterialType : uint8
		{
			UnDefine    = 0,	/*!< 未定义 */
			CommonCore  = 1,	/*!< 普通门芯 */
			Louver_Core = 2,    /*!< 百叶门芯 */
			Top_H       = 3,	/*!< 上横 */
			Middle_H    = 4,	/*!< 中横 */
			Bottom_H    = 5,	/*!< 下横 */
			Left_V      = 6,	/*!< 左竖 */
			Middle_V    = 7,	/*!< 中竖 */
			Right_V     = 8		/*!< 右竖 */
		};
		enum class EDisplayModel : uint8
		{
			UnDefine = 0,  /*!< 未定义 */
			Tiled	 = 1,  /*!< 平铺 */
			Stretch  = 2   /*!< 拉伸*/
		};
	public:
		int32 Id;
		ESectionalMaterialType Type;
		EDisplayModel DisplayMode;
		FString PakUrl;
		FString OptimizeParm;
		FString Md5;
		float Width;
		float Height;
		float Depth;
		FString CacheFilePath;
		bool IsShutter = false; /*!< 是否是百叶,只有是门芯的情况下，才考虑是否是百叶*/
		FSideTopAndBottomHSecionSizePros TopAndBottomSectionSizePros;
		FSideLeftAndRightVSectionPros LeftAndRightSectionSizePros;
		FInsideSectionSizePros InSideSectionSizePros;
	public:
		float GetCareSpace() const;
		/** 型材属性类型 */
		EProsType GetProsType() const;
	};

	/** 空间划分 */
	class FSpaceDivid : IDoorSheetDivision
	{
	public:
		virtual ~FSpaceDivid() { DestoryActor(); }
	public:
		virtual void ParseInfoFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
		virtual void SaveInfoToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

		void SpawnActor(AActor * InParentActor, const FSpaceDividDoorSheet & InParentDoor);
		/** 重新计算尺寸与坐标 */
		void RecalSizeAndPosition(const FSpaceDividDoorSheet & InParentDoor);

		void SetCollisionProfileName(const FName & InProfileName);

		void ResetDividPosition();
		/** 资源拷贝 */
		TSharedPtr<FSpaceDivid> CloneDividBlock() const;
		void DestoryActor();
	public:
		TArray<TSharedPtr<FSpaceDivid>> ChildrenBlocks;
		int32 SectionalMaterialId = -1;
		float PosX = 0.0f;
		float PosY = 0.0f;
		float PosZ = 0.0f;
		float Width = 0.0f;
		float Height = 0.0f;
		FString DividRation;
		bool IsRation = false;
		float DividValue = 0.0f;
		int32 DividDirection = 0; /*!< 0 不切分，1 竖向切分，2横向切分*/
		TArray<AActor *> StaticMeshActors;
		bool bIsRoot = false;
	};
public:
	/** 左竖 */
	const TSharedPtr<FSectionalMaterial>  & GetLeftV() const { return SectionalMaterials[GetIndexByType(FSectionalMaterial::ESectionalMaterialType::Left_V)]; }
	/** 中竖 */
	const TSharedPtr<FSectionalMaterial>  & GetMiddleV() const { return SectionalMaterials[GetIndexByType(FSectionalMaterial::ESectionalMaterialType::Middle_V)]; }
	/** 右竖 */
	const TSharedPtr<FSectionalMaterial>  & GetRightV() const { return SectionalMaterials[GetIndexByType(FSectionalMaterial::ESectionalMaterialType::Right_V)]; }
	/** 上横 */
	const TSharedPtr<FSectionalMaterial>  & GetTopH() const { return SectionalMaterials[GetIndexByType(FSectionalMaterial::ESectionalMaterialType::Top_H)]; }
	/** 中横 */
	const TSharedPtr<FSectionalMaterial>  & GetMiddleH() const { return SectionalMaterials[GetIndexByType(FSectionalMaterial::ESectionalMaterialType::Middle_H)]; }
	/** 下横 */
	const TSharedPtr<FSectionalMaterial>  & GetBottomH() const { return SectionalMaterials[GetIndexByType(FSectionalMaterial::ESectionalMaterialType::Bottom_H)]; }
	/** 获取指定索引的门芯 */
	const TSharedPtr<FSectionalMaterial>  & GetCore(const int32 Index) const;
private:
	TTuple<TPair<float, float>, TPair<float, float>, TPair<float, float>, TPair <float, float>> GetSloatAndDepthOffset(const FSpaceDivid * InDivid) const;

private:
	TPair<int32, int32> GetIndexByType(const FSectionalMaterial::ESectionalMaterialType InType) const;
	const  FString  GetCachePath(const int32 InSectionMaterialId, const FString & InMd5) const;
	const TSharedPtr<FSpaceDivid> & GetRootBlock() const { return RootBlock; }
private:
	TSharedPtr<FSpaceDivid> RootBlock;
	TMap<TPair<int32, int32>, TSharedPtr<FSectionalMaterial>> SectionalMaterials;
	bool IsInner = false;	
	float ModelDepth = 0.0f; /*!< 存储所有型材中深度的最大值 */
};

class FDividBlockShape;

/** 可变区域门板 */
class SCTSHAPEMODULE_API FVariableAreaDoorSheet : public  FDoorSheetShapeBase
{
public:
	/**封边拉手位置信息存储*/
	struct FSSealingSideHandle : public FSHandlePositionForType
	{
	public:
		FSSealingSideHandle() :FSHandlePositionForType(EHanleType::E_MT_SEALING_SIDE_HANDLE) {}
		virtual bool IsValid() const override { return VPostionType != -1; }			
		int32 VPostionType = -1; /*!< 1 上， 2 下*/
	};
	/** 明装拉手位置信息存储*/
	struct FSShakeHanle : public FSHandlePositionForType
	{
		FSShakeHanle() :FSHandlePositionForType(EHanleType::E_MT_SHAKE_HANDS) {}
		virtual bool IsValid() const override { return HPositionType != -1 && VPositionType != -1; }
		int32 HPositionType = -1;	   /*!< 水平位置：1 距左边；2 距右边；3 水平居中；*/
		float HPositionValue = 0.0f;   /*!< 水平位置距离*/
		int32 VPositionType = -1;	   /*!< 垂直位置：1 距上边；2 距下边；3 水平居中*/
		float VPositionValue = 0.0f;   /*!< 垂直位置距离*/
		int32 RotateDirection = 0;	   /*!< 旋转方向：0 保持不变；1 左旋90度；2 右旋90度；*/
	};
public:
	enum class EDoorOpenDirection : uint8
	{
		E_None,	 /*!< 不开门 */
		E_Left , /*!< 左开门 */
		E_Right, /*!< 右开门 */
		E_Top	 /*!< 上翻门 */
	};	
public:
	FVariableAreaDoorSheet();
	virtual ~FVariableAreaDoorSheet() {}
	FVariableAreaDoorSheet(const FVariableAreaDoorSheet &) = delete;
	FVariableAreaDoorSheet & operator =(const FVariableAreaDoorSheet &) = delete;
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

	virtual void SetCollisionProfileName(FName InProfileName) override;

	virtual void ReCalDividBlocksPositionAndSize() override;

	/** 可选的开门方向列表  */
	const TArray<EDoorOpenDirection> & GetAvailableOpenDirections() const { return AvailableOpenDirections; }
	TArray<EDoorOpenDirection> & GetAvailableOpenDirections() { return AvailableOpenDirections; }

	/** 获取当前开门方向 */
	EDoorOpenDirection GetOpenDoorDirection() const;
	void SetOpenDoorDirectionIndex(const int32 InDoorDirectionIndex) { OpenDoorDirectionIndex = InDoorDirectionIndex; }
	void SetOpenDoorDirection(const EDoorOpenDirection InDoorDirection);
	int32 GetOpenDoorDirectionIndex() const { return OpenDoorDirectionIndex; }
	int32 GetOpenDoorDirectionValue() const { return StaticCast<int32>(AvailableOpenDirections.IsValidIndex(OpenDoorDirectionIndex) ? AvailableOpenDirections[OpenDoorDirectionIndex] : EDoorOpenDirection::E_None); }

	/** 明装类型的拉手位置是否有效 */
	bool IsShakeHanlePositionValid() const { return ShakeHanleTypePosition.IsValid(); }
	/** 封边拉手位置是否有效 */
	bool IsSealingSideHandleValid() const { return  SealingSideHandleTypePosition.IsValid(); }
	const FSShakeHanle & GetShakeHanleTypePosition() const { return ShakeHanleTypePosition; }
	FSShakeHanle & GetShakeHanleTypePosition() { return ShakeHanleTypePosition; }
	const FSSealingSideHandle & GetSealingSideHandlePosition() const { return SealingSideHandleTypePosition; }
	FSSealingSideHandle & GetSealingSideHandlePosition() { return SealingSideHandleTypePosition; }
	
	/** 获取可视化尺寸 -- 如果有封边拉手，则返回的尺寸会是门板尺寸加上封边拉手尺寸 */
	 virtual FVector GetDoorSheetVisualSize() { return FVector{ GetShapeWidth(),GetShapeDepth(),GetShapeHeight() }; }
public:
	/** 门切块 */
	void AddDividBlock(TSharedPtr<FDividBlockShape> InBlock);
	const TArray<TSharedPtr<FDividBlockShape>> & GetDividBlocks() const { return DividBlocks; }
	TArray<TSharedPtr<FDividBlockShape>> & GetDividBlocks() { return DividBlocks; }
	void SetDividBlocks(const TArray<TSharedPtr<FDividBlockShape>> & InDividBlocks) { DividBlocks = InDividBlocks; }
	/** 更新材质 */
	virtual void UpdateMaterial(UMaterialInterface * InMaterialInterface);
protected:
	//分割模块	
	TArray<TSharedPtr<FDividBlockShape>>  DividBlocks;
	// 支持的所有开门方向
	TArray<EDoorOpenDirection> AvailableOpenDirections = { EDoorOpenDirection::E_Left,EDoorOpenDirection::E_Right,EDoorOpenDirection::E_Top,EDoorOpenDirection::E_None};	
	int32 OpenDoorDirectionIndex = 0;     /*!< 开门方向：0：假门；1；左开门；2：右开门*/		
	FSShakeHanle ShakeHanleTypePosition;	/*!< 明装类型的拉手位置信息 */
	FSSealingSideHandle  SealingSideHandleTypePosition; /*!< 封边拉手位置信息存储*/
};

