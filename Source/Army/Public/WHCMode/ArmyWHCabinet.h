#pragma once
#include "CoreMinimal.h"
#include "ArmyWHCRect.h"
#include "Data/XRObject.h"
#include "Data/WHCModeData/XRWHCModeData.h"
#include "SCTShapeData.h"
#include "Any.h"
#include "ArmyWHCabinetComponent.h"

#define WHC_COVER_DOOR_TYPE_INDEX 0
#define WHC_INDEX_NUM 1

#define SHAPE_OVERLAPPED 						0x000000001
#define SHAPE_OVERLAPPED_BY_ANOTHER 			0x000000002
#define SHAPE_ALIGNED 							0x000000004
#define SHAPE_ALIGEND_BY_ANOTHER				0x000000008

#define PARAM_NAME_CAB_WIDTH_INTERVALS "CabWidthIntervals"
#define PARAM_NAME_CAB_DEPTH_INTERVALS "CabDepthIntervals"
#define PARAM_NAME_CAB_HEIGHT_INTERVALS "CabHeightIntervals"
#define PARAM_NAME_CAB_VENTILATOR_WIDTH_INTERVALS "CabVentilatorWidthIntervals"
#define PARAM_NAME_CAB_MAX_VENTILATOR_WIDTH "CabMaxVentilatorWidth"
#define PARAM_NAME_CAB_MIN_VENTILATOR_WIDTH "CabMinVentilatorWidth"


struct FShapeAccessory
{
	class AXRElecDeviceActor *Actor = nullptr;

	TSharedPtr<FCabinetAccInfo> CabAccInfo;
	TSharedPtr<class FArmyFurniture> Component;

	TArray<TSharedPtr<FShapeAccessory>> AttachedSubAccessories;

	~FShapeAccessory();
	bool SpawnActor(class AActor *InParent);
	struct FSCTVRSObject* GetResourceObj();
	// 更新构件Transform
	void UpdateComponent();
};


class FAnimation;
struct FShapeInRoom : public TSharedFromThis<FShapeInRoom>
{
	int32 Flags = 0;
	int32 mId;
	int32 WallAttachedIndex = -1;
	float AboveGround = 0.0f;
	FGuid RoomAttachedIndex;
	TArray<ECabinetLabelType> Labels;
	class AXRShapeFrame *ShapeFrame = nullptr;
	//TSharedPtr<FShapeAccessory> Accessory;     // remove
	//TArray<class AStaticMeshActor*> ShapeLegs_test; // remove  橱柜调整脚
	TSharedPtr<class FShapeRuler> ShapeWidthRuler_test; // remove
	TSharedPtr<class FSCTShape> Shape;
	//TSharedPtr<class FArmyFurniture> ShapeComponent;
	//TSharedPtr<FPlatformInfo> PlatformInfo;    // 台面
	class AXRShapeTableActor *Platform = nullptr;
	TArray<AXRShapeTableActor*> PlatformFrontSections;
	TArray<AXRShapeTableActor*> PlatformBackSections;

	//actor-component 
	using ECC_TYPE = XRCabinetActor::eCabinetComponentType;
	TSharedPtr<class XRCabinetActor> CabinetActor = nullptr;

	friend class XRCabinetActor;
	struct FDecBoardInfo
	{
		int32 Face : 8;
		int32 Spawned : 8;
		float BoardValue;
		int32 MtlId = 0;
	};
	TArray<FDecBoardInfo> DecBoardInfos;

	class UMaterialInterface *CachedDoorMtl = nullptr;
	class UMaterialInterface *CachedBoardMtl = nullptr;
	class UMaterialInterface *CachedInsertionBoardMtl = nullptr;

	TArray<TSharedPtr<class IArmyWHCModeShapeChange>> ChangeList;

	TArray<TSharedPtr<class FAnimation>> Animations;

	TMap<FName, FAny> ParamList;

	FShapeInRoom();
	~FShapeInRoom();

	void Initialize(FCabinetInfo *InCabInfo, FCabinetWholeComponent *InCabWholeComponent);
	void InitializeParams(FCabinetInfo *InCabInfo);
	void InitializeSelf(FCabinetComponent *InCabComp);
	void InitializeAccessories(FCabinetWholeAccInfo *InCabAccInfo, const TArray<TSharedPtr<FCabinetComponent>>& InAccessories);
	void InitializePlatform(const TSharedPtr<FPlatformInfo> &InPlatformInfo);
	FShapeAccessory* AddSubAccessory(FCabinetAccInfo *InAccInfo, EMetalsType InMetalType, bool bBoundToSocket);
	void AddAndSpawnSubAccessory(FCabinetAccInfo *InAccInfo, EMetalsType InMetalType, bool bBoundToSocket, FVector& RelLocation);
	FShapeAccessory* ReplaceSubAccessory(FCabinetAccInfo *InOldAccInfo, FCabinetAccInfo *InNewAccInfo, EMetalsType InMetalType, bool bBoundToSocket);
	void ReplaceAndSpawnSubAccessory(FCabinetAccInfo *InOldAccInfo, FCabinetAccInfo *InNewAccInfo, 
		EMetalsType InMetalType, bool bBoundToSocket, FVector& RelLocation);
	void DeleteSubAccessory(int32 InIndex);
	void UpdateAccessoryLocation();
	void UpdateDecBoardInfo();

	void Spawn();
	void SpawnShapeFrame();
	void SpawnShapeWidthRuler();
	void SpawnAccessories();
	void SpawnSubAccessory(FShapeAccessory *InAccessory, const FVector &InRelativeLocation);
	void SpawnShapeLegs();
	void SpawnShape();
	void SpawnPlatform();

	void DestroyAccessories();
	void DestroyShapeLegs();
	void DestroyShape();
	void DestroyPlatform();
	void DestroyRuler();

	bool HasValidShape() const;
	bool HasValidAccessory() const;
	bool IsPlatformSpawned() const;

	void ShowShapeLegs(bool bShow);
	void ShowAccessories(bool bShow);

	void UpdateComponents();

	void OnAboveFloorChange();
	// 0 - left, 1 - right
	bool SetSunBoard(int32 nFace);
	void UnsetSunBoard(int32 nFace);
	void ChangeSunBoardMtl(FMtlInfo *InMtlInfo);
	bool IsSunBoard(int32 nFace) const;
	float GetSunBoardWidth(int32 nFace) const;
	
	bool CanGenGlobalTableBoardAndToe() const;
	
	void PrepareShapeAnimations();
	TSharedPtr<FAnimation> FindShapeAnimation(class FSCTShape *InShape) const; 

	class UMaterialInterface* GetDoorMtl() const;
	class UMaterialInterface* GetInsertionBoardMtl() const;
	class UMaterialInterface* GetBottomMtl() const;
	void OnChangeDoorMtl();

	void MakeImmovable();
	void MakeImmovableInternal(AActor *InActor);

	FArmyWHCRect GetRect(FVector* NewPos=nullptr,float* fNewHalfWidth =nullptr);
	bool SetTranslate(FArmyWHCRect& Rect);

	float GetLeftDist( FShapeInRoom* pIgnoredActor=nullptr);
	float GetRightDist(FShapeInRoom* pIgnoredActor = nullptr);
	float GetMatchWidth(float fWidth);
	bool SetNewWidth(float NewWidth);

	template <class ... ArgsType>
	void AddOrSetParam(FName InParamName, ArgsType &&... Args)
	{
		FAny *AnyValue = ParamList.Find(InParamName);
		if (AnyValue == nullptr)
			ParamList.Emplace(InParamName, Forward<ArgsType>(Args)...);
		else
			AnyValue->operator= (Forward<ArgsType>(Args)...);
	}

	bool HasParam(FName InParamName) const
	{
		return ParamList.Contains(InParamName);
	}

	template <class ValueType>
	bool IsParamType(FName InParamName) const 
	{
		const FAny *AnyValue = ParamList.Find(InParamName);
		return AnyValue != nullptr ? AnyValue->IsSameType<ValueType>() : false;
	}

	template <class Type>
	Type* GetParam(FName InParamName) const
	{
		const FAny *AnyValue = ParamList.Find(InParamName);
		return AnyValue != nullptr ? AnyCast<Type>(const_cast<FAny*>(AnyValue)) : nullptr;
	}

public:
		//temp to put it here,
		FShapeAccessory* MakeShapeAccessory(FCabinetAccInfo *InAccInfo);
		void MakeComponent(TSharedPtr<class FArmyFurniture> &OutFurniture, const FString &InComponentPath, EObjectType InObjType, uint32 InBelongClass);

private:
	class UMaterialInterface* LoadDoorMtl();
	class UMaterialInterface* LoadBottomBoardMtl();
	class UMaterialInterface* LoadInsertionBoardMtl();
	void CacheMtl(class UMaterialInterface *&InOutCacheSlot, class UMaterialInterface *InMtlToCache);


	void RefreshShapeWidthRuler();
};