#pragma once
#include "CoreMinimal.h"
#include "Any.h"
#include "SCTShapeData.h"
#include "SCTBoardShape.h"
#include "Misc/Paths.h"

/** 前向声明 */


class FSCTOutline;


struct SCTSHAPEMODULE_API FMaterialItemData
{
	FString Name;
	FString ThumbnailUrl;
	FString PakUrl;
	FString PakMD5;
	int32 ItemId;
	FString OptimizeParam;
};

struct SCTSHAPEMODULE_API FSpaceBoardItemData
{
	int64 BoardID;
	FString Name;
	FString ThumbnailUrl;
	FString SubstrateName;
	FString MaterialName;
	int32 Height;
};

struct SCTSHAPEMODULE_API FDoorItemData
{
	int32 ID = -1;
	FString Name;
	FString ThumbnailUrl;
	float MinHeight = 0.0f;
	float MaxHeight = 0.0f;
	float MinWidth  = 0.0f;
	float MaxWidth  = 0.0f;
};

struct 	SCTSHAPEMODULE_API SBaseCommonItemData
{
	int32 ID = -1;
	FString Name;
	FString ThumbnailUrl;
};


struct SCTSHAPEMODULE_API FInsertionBoardItemData : public 	SBaseCommonItemData
{
};

struct SCTSHAPEMODULE_API FLateralBoxHandleItemData : public SBaseCommonItemData
{
};

struct SCTSHAPEMODULE_API FServerTag
{
	FGuid Guid;
	FString Name;
	FString Description;
	FString Data;
};

class SCTSHAPEMODULE_API FGlobalDataForShape
{
public:
	static FGlobalDataForShape * GetInsPtr();
	static void ReleaseInsPtr();
private:
	static TSharedPtr<FGlobalDataForShape> GlobalDataForShape;
private:
	FGlobalDataForShape() {}	
public:

	/** 设置包边列表 */
	void SetCoverdEdgeMapList(const TArray<TMap<FString, FAny>>& edgeMtMapList) { CoverdEdgeMapList = edgeMtMapList; }
	/** 获取包边列表map */
	const TArray<TMap<FString, FAny>> & GetCoverdEdgeMapList() const { return CoverdEdgeMapList; }
	/** 获取包边列表map */
	TArray<TMap<FString, FAny>> & GetCoverdEdgeMapList() { return CoverdEdgeMapList; }

	/** 铝箔 */
	void SetVeneerMtMapList(const TArray<TMap<FString, FAny>>& veneerMtMapList) { VeneerMtMapList = veneerMtMapList; }
	const TArray<TMap<FString, FAny>> & GetVeneerMtMapList() const { return VeneerMtMapList; }
	TArray<TMap<FString, FAny>> & GetVeneerMtMapList() { return VeneerMtMapList; }
	bool GetSingleVennerMat(int32 & OutId,FString & OutPakUrl, FString & OutPakMd5, FString & OutParameter);
	
	/** 设置封边列表map */
	void SetEdgeBandingList(const TArray<TMap<FString, FAny>> & InEdgeBandingList) { EdgeBandingList = InEdgeBandingList; }
	/** 获取封边列表map */
	const TArray<TMap<FString, FAny>> & GetEdgeBandingList() const { return EdgeBandingList; }
	/** 获取封边列表map */
	TArray<TMap<FString, FAny>> & GetEdgeBandingList() { return EdgeBandingList; }
	/** 获取封边名称列表 */
	bool  GetEdgeBandingNameLIst(TArray<FString> & OutNameList);
	bool GetSpecialEdgeMaterialInfo(const int32 InId, FString & OutName);
	/** 根据封边ID，获取封边索引 */
	bool GetEdgeBandingIndexById(const int32 InId, int32 & OutIndex);
	/** 根据封边的名称，获取ID */
	bool GetEdgeBandingIdByName(const FString & InName, int32 & OutId);

	/** 获取空间轮廓列表 */
	TArray<TSharedPtr<FSCTOutline>>& GetSpaceOutlineList() { return SpaceOutlineList; };
	const TArray<TSharedPtr<FSCTOutline>>& GetSpaceOutlineList() const { return SpaceOutlineList; };
	TSharedPtr<FSCTOutline> GetSpaceOutlineByType(EOutlineType InType);
	TSharedPtr<FSCTOutline> GetSpaceOutlineByName(const FString& InName);

	/** 设置基材列表map */
	void SetBaseMaterialList(const TArray<TMap<FString, FAny>> & InEdgeBandingList) { BaseMaterialList = InEdgeBandingList; }
	/** 获取基材列表map */
	const TArray<TMap<FString, FAny>> & GetBaseMaterialList() const { return BaseMaterialList; }
	/** 获取基材列表map */
	TArray<TMap<FString, FAny>> & GetBaseMaterialList() { return BaseMaterialList; }
	/** 根据基材ID与基材分类，获取基材名称 */
	bool GetSpecialBaseMaterialInfo(const int32 InId, const int32 InCategory,FString & OutName);
	/** 根据基材ID与基材分类，获取基材厚度列表 */
	bool GetSpecialBaseMaterialInfo(const int32 InId, const int32 InCategory, TArray<double> & OutList);
	// 根据基材ID以及基材分类，获取基材索引
	bool GetSpecialBaseMaterialIndex(const int32 InId, const int32 InCategory, int32 & OutIndex);
    //根据基材ID获取基材列表
	TArray<FString> GetBaseMtlNameListInfo(const int32& InCategory);
	/** 通过基材的分类以及名称，获取基材的ID */
	bool GetSpecialBaseMaterialId(const int32 InCategory, const FString & InRefName, int32 & OutId);

	/** 材质列表 */
	const TMap<int32, TArray<TSharedPtr<FMaterialItemData>>>& GetMaterialItemListDict() const { return MaterialItemListDict; }
	TMap<int32, TArray<TSharedPtr<FMaterialItemData>>>& GetMaterialItemListDict() { return MaterialItemListDict; }	
	

	/** 抽面材质列表 */
	const TMap<int32, TArray<TSharedPtr<FMaterialItemData>>>& GetDrawerMaterialItemListDict() const { return DrawerMaterialItemListDict; }
	TMap<int32, TArray<TSharedPtr<FMaterialItemData>>>& GetDrawerMaterialItemListDict() { return DrawerMaterialItemListDict; }


	/** 默认材质列表 - 板材、石板、玻璃*/
	void SetDefaultMaterialList(const TArray<TMap<FString, FAny>> & InDefaultMatterialList) { DefatultMaterialList = InDefaultMatterialList; }
	const TArray<TMap<FString, FAny>> & GetDefaultMaterialList() const { return DefatultMaterialList; }
	TArray<TMap<FString, FAny>> & GetDefaultMaterialList() { return DefatultMaterialList; }
	bool GetSingleDefaultMaterialInfo(const int32 InCategory, int32 & OutID,FString & OutUrl,
		FString & OutMd5,FString & OutName,FString & OutThumbnailUrl);
	// 获取材质名称列表
	TArray<FString> GetDefaultMtlNameListInfo(const int32& InSubstrateCategory);


	/**  门的默认设置项 */
	void SetDoorDefaultSeeting(const TMap<int32, TSharedPtr<FDoorDefaultInfo>> & InDoorDefaultSettingList) { DoorDefalutSettingList = InDoorDefaultSettingList; }
	const TMap<int32, TSharedPtr<FDoorDefaultInfo>> & GetDoorDefaultSettingList() const { return DoorDefalutSettingList; }
	TMap<int32, TSharedPtr<FDoorDefaultInfo>> & GetDoorDefaultSettingList() { return DoorDefalutSettingList; }
	const TSharedPtr<FDoorDefaultInfo> GetDoorMetalsDefaultLocation(const int32 InDoorType) const;
	/** 获取抽面拉手默认设置*/
	const TSharedPtr<FDoorSubWidgetDefaultInfoBase> GetDoorHanleMetalsDefaultLocation(const int32 InDoorType,const EMetalsType InMetalType,const int32 InDoorOpenDirection = -1) const;

	//筛选空间板件列表
	const TArray<TSharedPtr<FSpaceBoardItemData>>& GetSpaceBoardItemList() const { return SpaceBoardItemDatas; }
	void SetSpaceBoardItemList(const TArray<TSharedPtr<FSpaceBoardItemData>>& Items) { SpaceBoardItemDatas = Items; }

	//空间板件列表
	const TArray<TSharedPtr<FBoardShape>>& GetSpaceBoardList() const { return SpaceBoardList; }
	void SetSpaceBoardList(const TArray<TSharedPtr<FBoardShape>>& BoardList) { SpaceBoardList = BoardList; }
	/** 存储门窗口中所需的门列表 */
	const TArray<TSharedPtr<FDoorItemData>>& GetDoorItemList() const { return DoorIteamList; }
	TArray<TSharedPtr<FDoorItemData>>& GetDoorItemList() { return DoorIteamList; }

	/** 门资源列表共有多少页 */
	int32 GetDoorListPageCount() const { return DoorListPageCount; }
	void SetDoorListPageCount(const int32 InDoorPageCount) { DoorListPageCount = InDoorPageCount; }

	/** 电器饰条资源列表 */
	const TArray<TSharedPtr<FInsertionBoardItemData>>& GetInsertionBoardItemList() const { return InsertionBoardList; }
	TArray<TSharedPtr<FInsertionBoardItemData>>& GetInsertionBoardItemList() { return InsertionBoardList; }

	/** 获取箱体拉手列表共有多少页 */
	int32 GetLateralBoxHanlePageCount() const { return LateralBoxHandlePageCount; }
	void SetLateralBoxHandlePageCount(const int32 InLateralBoxHandlePageCount) { LateralBoxHandlePageCount = InLateralBoxHandlePageCount; }

	/** 箱体拉手资源列表 */
	const TArray<TSharedPtr<FLateralBoxHandleItemData>>& GetLateralBoxHandleItemList() const { return LateralBoxHandleList; }
	TArray<TSharedPtr<FLateralBoxHandleItemData>>& GetLateralBoxHandleItemList() { return LateralBoxHandleList; }

	
	/** 电器饰条资源一共有多少个 */
	const int32 GetInsertionBoardListPageCount() const { return InsertionBoardPageCount; }
	void SetInsertionBoardListPageCount(const int32 InPageCount) { InsertionBoardPageCount = InPageCount; }

	/** 保存时候可选的标签列表 */
	const TMap<int32, TArray<TPair<int32, FString>>> & GetTagListMap() const { return TagListMap; }
	TMap<int32, TArray<TPair<int32, FString>>> & GetTagListMap() { return TagListMap; }

	/** 插角封板 */
	void SetPinBoard(const FPinBoardDefaultInfo & InPinBoard) { PinBoard = InPinBoard; }
	const FPinBoardDefaultInfo & GetPinBoard() const { return PinBoard; }
	FPinBoardDefaultInfo & GetPinBoard() { return PinBoard; }

	/** 缓存跟目录 */
	void SetRootDir(const FString & InRootDir) { RootDir = InRootDir; }
	const FString & GetRootDir() const { return RootDir; }

	/** 标签池 */
	const TMap<EShapeType, TArray<FServerTag>> & GetServerTagsPool() const { return ServerTags; }
	TMap<EShapeType, TArray<FServerTag>> & GetServerTagsPool()  { return ServerTags; }
	bool HasSpecialTypeTagsPool(EShapeType InType) const { return ServerTags.Contains(InType); }
	const TArray<FServerTag> & GetSpecialTypeTagsPool(EShapeType InType) const;

private:
	TArray<TMap<FString, FAny>> BaseMaterialList;
	TArray<TSharedPtr<FSpaceBoardItemData>> SpaceBoardItemDatas;
	TArray<TSharedPtr<FBoardShape>> SpaceBoardList;
	TArray<TMap<FString, FAny>> CoverdEdgeMapList;
	TArray<TMap<FString, FAny>> VeneerMtMapList;
	TArray<TMap<FString, FAny>> EdgeBandingList;
	TArray<TMap<FString, FAny>>  DefatultMaterialList;
	TMap<int32,  TSharedPtr<FDoorDefaultInfo>> DoorDefalutSettingList;
	TMap<int32, TArray<TSharedPtr<FMaterialItemData>>> MaterialItemListDict;
	TMap<int32, TArray<TSharedPtr<FMaterialItemData>>> DrawerMaterialItemListDict;
	TArray<TSharedPtr<FSCTOutline>> SpaceOutlineList;
	TArray<TSharedPtr<FDoorItemData>> DoorIteamList;
	TArray<TSharedPtr<FInsertionBoardItemData>> InsertionBoardList;
	TArray<TSharedPtr<FLateralBoxHandleItemData>> LateralBoxHandleList;
	TMap<int32, TArray<TPair<int32, FString>>> TagListMap;
	FPinBoardDefaultInfo  PinBoard;
	int32 DoorListPageCount = 0;
	int32 InsertionBoardPageCount = 0;
	int32 LateralBoxHandlePageCount = 0;
	FString RootDir = FPaths::ProjectContentDir();
	TMap<EShapeType, TArray<FServerTag>> ServerTags;
};
