#pragma once

#include "CoreMinimal.h"
#include "ArmyHardModeData.h"

class XR_API FArmyHardModeExecute
{
public:
	FArmyHardModeExecute() {}
	~FArmyHardModeExecute() {}

	static const TSharedRef<FArmyHardModeExecute>& Get();

	//执行硬装匹配
	void ExecHardMatching(TSharedPtr<class FArmyRoomEntity> Room, TSharedPtr<FArmyHardModeData> InHardwareData);

	void ExecHardwareMatching(TSharedPtr<FArmyHardwareData> InHardModeData);

	void ExecDoors();

	void ClearHardModeData(TSharedPtr<class FArmyRoomEntity> Room);

	void ClearHardwareData();
private:
	void ModellingWallMatching(TSharedPtr<class FArmyRoomEntity> Room);

	void ModellingFloorMatching(TSharedPtr<class FArmyRoomEntity> Room);
	
	void ModellingRoofMatching(TSharedPtr<class FArmyRoomEntity> Room);

	void ModellingBuckleMatching(TSharedPtr<class FArmyRoomEntity> Room);

	void ModellingWindows(TSharedPtr<class FArmyRoomEntity> Room, TSharedPtr<FModellingWall> InModellingWall);


	void ModellingSimpleComponents(TSharedPtr<class FArmyRoomEntity> Room, TSharedPtr<FModellingWall> InModellingWall);

	// 铺贴颜值包
	void ShowDesignPackage(TSharedPtr<class FArmyBaseEditStyle> InStyle, TSharedPtr<FArmyBaseArea> editArea, const TilePasteStyle& InDesignStyle);
	// 铺贴颜值包
	void ShowDesignPackage(TSharedPtr<class FArmyModelEntity> InModelEntity,TSharedPtr<FArmyBaseArea> editArea, const TilePasteStyle& InDesignStyle);
	// 铺贴颜值包通用方法
	void ComShowDesignPackage(TSharedPtr<FContentItemSpace::FContentItem> ContentItem, TSharedPtr<class FArmyConstructionItemInterface> InConstructionItem,TSharedPtr<FArmyBaseArea> editArea, const TilePasteStyle& InDesignStyle);

	TSharedPtr<FArmyBaseArea> CreateInternalSpaceArea(TSharedPtr<FArmyRoomSpaceArea> ParentArea, const TilePasteStyle& InDesignStyle,
		FModellingAreaInfo& InAreaInfo, float BaseAreaWidth = 0.f, float BaseAreaHeight = 0.f);
	TSharedPtr<FArmyBaseArea> CreateInternalArea(TSharedPtr<FArmyRoomSpaceArea> ParentArea, const TilePasteStyle& InDesignStyle,
		FModellingAreaInfo InAreaInfo, float BaseAreaWidth = 0.f, float BaseAreaHeight = 0.f);


private:
	TSharedPtr<FArmyHardModeData> HardModeData;
	TSharedPtr<FArmyHardwareData> HardwareData;
	TSharedPtr<class FArmyRoomEntity> RoomEntity;

	TSharedPtr<FArmyBaseEditStyle> GetEditStyle(EStyleType type);
	
	TSharedPtr<FArmyRoomSpaceArea>  SofaBackSpace;
	TSharedPtr<FArmyRoomSpaceArea>  BedRoomSpace;
};
