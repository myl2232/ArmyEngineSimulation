#pragma once
#include "CoreMinimal.h"
#include "ArmyRoom.h"
#include "ArmyShapeRuler.h"

#include "ArmyExtrusionActor.h"
#include "ArmyWHCabinet.h"
#include "Data/WHCModeData/XRWHCModeData.h"
#include "Data/XRWHCTableObject.h"
#include "ArmyWHCRect.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Actor/VertexType.h"


#define WHC_GEN_TABLE_FLAG 0x00000001
#define WHC_GEN_TOP_FLAG 0x00000002
#define WHC_GEN_TOE_FLAG 0x00000004
#define WHC_GEN_WAIST_FLAG 0x00000008
#define WHC_GEN_TOE_GAP_FLAG 0x00000010
#define WHC_GEN_TOP_GAP_FLAG 0x00000020

const FString GDefaultToeDxf = TEXT("../../../XR/Content/WHC/tijiaoban.dxf");
const FString GDefaultFrontDxf = TEXT("../../../XR/Content/WHC/houdang45.dxf");
const FString GDefaultBackDxf = TEXT("../../../XR/Content/WHC/test45mm.dxf");
const FString GDefaultTopDxf = TEXT("../../../XR/Content/WHC/60hang.dxf");


class XRWHCabinetIComponent;
class CabComponentsPool final {
public:
	struct Location {
		int32 value;
		int32 signatureCapture;
	};

	struct Entry {
		int32 signature;
		TSharedPtr<XRWHCabinetIComponent> tpComponent;
	};

public:
	CabComponentsPool();

	inline Entry& Get(int32 location) {
		return InstanceList[location];
	}
public:

	Location AddComponent(TSharedPtr<XRWHCabinetIComponent> tpComponent);
	void RemoveComponent(int32 location, int32 signature);

protected:
	bool IsTransformComponent(int64 hashCodes);
private:

	TArray<Entry> InstanceList;
	TArray<int32> FreeList, FreeTransformList;

	static const int32 defaultCapacity;
};
class AXRShapeTableActor;
class AXRWhcGapActor;
class FArmyWHCabinetMgr
{
public:
	enum ETrimType
	{
		ETrim_Toe = 0,
		ETrim_PlatformFront,
		ETrim_PlatformBack,
		ETrim_Top,
		ETrim_Num
	};
	struct FTrimInfo
	{
		FTrimInfo(AXRShapeTableActor* InActor, const TArray<FVector2D>& InPath) :TrimActor(InActor), TrimPath(InPath){}
		AXRShapeTableActor* TrimActor;
		TArray<FVector2D> TrimPath;
	};
	FArmyWHCabinetMgr();
	~FArmyWHCabinetMgr();
	void OnRefreshRoom();
	bool OnBeginMode();
	bool OnEndMode();
	bool OnResLoaded();
	bool OnClear();
	bool OnNewShape(FShapeInRoom* pShape);
	bool OnShapeInited(FShapeInRoom* pShape);
	bool OnPalceShapeBegin(FShapeInRoom* pShape);
	bool OnPalceShapeEnd(FShapeInRoom *pShape);
	bool OnRefreshShape(int32 iType );
	//
	FORCEINLINE TArray<TSharedPtr<FShapeInRoom>>& GetPlaceShapes() { return PlacedShapes; }
	//
	static FArmyWHCabinetMgr* Get();
	bool Init();
	bool GenTableBoard();
	bool GenHangGapBoard();
	bool GroupCabinet(TMap<int32, int32>& CabinetTop,bool bFloor);
	UMaterialInterface* LoadMtl(FString MtlName);
	void LoadMesh(const FString &InMeshFilePath, const FString &InMeshJson);
	int32 GetMesh(const FString &InMeshFilePath);
	UStaticMesh* GetStaticMesh(const FString &InMeshFilePath);
	
	
	
	float GetTableHeight(bool bIngorShow=false );
	float GetTableSplineWidth();
	float GetTableSplineElapse();
	

	
	//
	bool InitRoom();
	const TMap<FGuid, TSharedPtr<FArmyWHCPoly>>& GetRooms() const { return mRooms; }
	bool Collision(FShapeInRoom* pShape, FArmyWHCRect* pRect = nullptr, float Offset = 0.0f, bool bPillar = false, const TArray<FShapeInRoom*> &IgnoredShapes = TArray<FShapeInRoom*>());
	bool Collision(FShapeInRoom *pShape, FArmyWHCRect& rect, FArmyWHCRect& RectOther, float Offset = 0.0f, const TArray<FShapeInRoom*> &IgnoredShapes = TArray<FShapeInRoom*>());
	bool CollisionPillar(FArmyWHCRect& rect, FArmyWHCRect* RectRet = nullptr, float Offset = 0.0f, FArmyWHCPoly* pRoom=nullptr);
	FArmyWHCLineSeg* CollisionRoom(FArmyWHCRect& rect, const FGuid& RoomId, float OffsetX = 0.0f, float OffsetY = 0.0f);
	FArmyWHCLineSeg* CollisionRoomSeg(FArmyWHCRect& rect, const FGuid& RoomId, int32 Idx, float OffsetX = 0.0f, float OffsetY = 0.0f);
	FArmyWHCLineSeg* CollisionNextRoomSeg(FArmyWHCRect& rect, const FGuid& RoomId, int32 Idx, float OffsetX = 0.0f, float OffsetY = 0.0f);
	FArmyWHCLineSeg* CollisionPreRoomSeg(FArmyWHCRect& rect, const FGuid& RoomId, int32 Idx, float OffsetX = 0.0f, float OffsetY = 0.0f);
	FVector2D GetNearSeg(FVector2D Pos, const FGuid& RoomId, int32& Idx, float& fDistance);
	bool GroupShapeRoom();//����������ڵķ���
	FArmyWHCPoly* GetPolyByPos(FVector2D Pos);//�ҵ������ڵķ���
	FArmyWHCPoly* GetPolyById(const FGuid& Id);
	FArmyWHCPoly* CollisionRoom(FVector& PosStart, FVector& Dir, float InAboveGround, int32 &OutRoomIndex, FVector &OutPositionInRoom);

	void RefreshTableMesh(const TArray<FPUVVertex>& Vertices, const TArray<uint16>& Indices);
	void CollectTableTrimOutline(const TArray<FArmyWHCTableObject::FWHCTableBatchData>& InBatchDataArray);
	float GenGapActor(int32 nType, FVector2D StartPos, FVector2D EndPos, float fBottom, float fTop, UMaterialInstanceDynamic* pMtl);
	void GenGapActor(const TArray<FPUVVertex>& Vers, const TArray<uint16>& Faces, UMaterialInstanceDynamic* pMtl);
	void GenTrimActor(int32 TrimType, const TArray<FVector2D>& Vers, bool bClose, float PlaceHigh);

	void GetGapActors(int32 InType/*-1: all 0: floor 1: hang*/, TArray<AXRWhcGapActor*>& OutArray);
	void GetTrimPath(int32 InType, TArray<FArmyWHCabinetMgr::FTrimInfo>& OutPaths) const;
	const TArray<AXRShapeTableActor*>& GetTrimActors(int32 InType) const;

	void SetPlatformInfo(FPlatformInfo* InPlatformInfo);
	void ResetPlatformInfo();
	void SetFrontTrim(FTableInfo* pTableInfo);
	void ResetFrontTrim();
	void SetTopTrim(FTableInfo* pTableInfo);
	void ResetTopTrim();
	void SetToeTrim(FTableInfo* pTableInfo);
	void ResetToeTrim();
	void RefreshPlatformMtl();
	void SetTopTrimMtl(FMtlInfo* InTopInfo);
	void SetToeTrimMtl(FMtlInfo* InToeInfo);
	
	
	FPlatformInfo* GetPlatformInfo() const;
	FTableInfo* GetFrontInfo() const;
	FTableInfo* GetBackInfo() const;
	FTableInfo* GetTopInfo() const;
	FTableInfo* GetToeInfo() const;

	void GenTable(bool bVisble);
	void GenFloorTrim(bool bVisble);
	void GenHangTrim(bool bVisble);
	void GenFloorGap(bool bVisble);
	void GenHangGap(bool bVisble);

	void SetTableVisible(bool bVisible);
	void SetTrimVisible(ETrimType TrimType, bool bVisible);
	void SetFloorGapVisible(bool bVisible);
	void SetHangGapVisible(bool bVisible);

	void SetGenTable(bool bGenTable);
	bool IsGenTable() const;
	void SetGenTop(bool bGenTop);
	bool IsGenTop() const;
	void SetGenToe(bool bGenToe);
	bool IsGenToe() const;
	void SetGenWaist(bool bGenWaist);
	bool IsGenWaist() const;
	void SetGenToeGap(bool bGenToeGap);
	bool IsGenToeGap() const;
	void SetGenTopGap(bool bGenTopGap);
	bool IsGenTopGap() const;

	void LoadTopMtl();

	void UpdateShapeAccessoryLocation();

	
	//
	bool OnUIAboveGroundChangeSingle(float fValue, FShapeInRoom* pShapeInRoom);
	bool OnUIAboveGroundChange(float fValue);
	bool OnUptValueCommitted(float fValue, FShapeInRoom* pShapeInRoom);
	bool OnFrontValueCommitted(float fValue, FShapeInRoom* pShapeInRoom);
	bool OnRightValueCommitted(float fValue, FShapeInRoom* pShapeInRoom);
	bool AdjustCabinetNewWidth(FShapeInRoom* pShapeInRoom,float fNewWidth);
	bool CalculateFloorHeight();
	float GetAboveGround(int nType);
	
	const FVector GetSizeGapMesh() const { return mSizeGapMesh; }

	CabComponentsPool& CabinetComponentsPool() { return mCabComponetsPool;}
			
private:
	void DestroyTable();
	void DestroyFloorTrim();
	void DestroyHangTrim();
	void DestroyFloorGap();
	void DestroyHangGap();
	AXRWhcGapActor* GenGapActor(FVector2D StartPos, FVector2D EndPos, float Height, float zStart, UMaterialInstanceDynamic* pMtl);
	bool InitPillar(EObjectType eType);
	bool InitPackPipe();
	UMaterialInterface* GetHangOrHighDoorMtl() const;
	UMaterialInterface* GetTableMtl() const;
	UMaterialInterface* GetToeMtl() const;
	

public:
	float	mFloorShapeHight;
	float	mTableFrontExtrude;//前沿突出量
	
private:
	TMap<FGuid, TSharedPtr<FArmyWHCPoly>> mRooms;

	TArray<TSharedPtr<FShapeInRoom>> PlacedShapes;
	TArray<AXRShapeTableActor*>	mTableActors;
	TArray<AXRShapeTableActor*>	mTrimActors[ETrim_Num];
	TArray<AXRShapeTableActor*>	mGapActors;
	TArray<AXRWhcGapActor*>	mGapFloors;
	TArray<AXRWhcGapActor*>	mGapHangs;

	TMultiMap<ETrimType, FTrimInfo> TrimPathMap;

	// 保存每种造型线条使用的dxf文件路径
	FString	mTrimNames[ETrim_Num];
	TMap<FString, UMaterialInterface*> mMaterials;

	// 图纸模式所有台面的FArmyObject
	TArray<TWeakPtr<class FArmyWHCTableObject>> TableObjects;


	CabComponentsPool mCabComponetsPool;

	int32 mPlatformId = 0;
	int32 mFrontTrimId = 0;
	int32 mBackTrimId = 0;
	int32 mTopTrimId = 0;
	int32 mTopMtlId = 0;
	int32 mToeTrimId = 0;
	int32 mToeMtlId = 0;

	uint32 GenFlag = 0;

	UMaterialInterface *DefaultPlatformMtl = nullptr;
	UMaterialInterface *DefaultToeMtl = nullptr;

	UStaticMesh*	mGapMesh;
	FVector			mSizeGapMesh;
	UMaterialInterface *FloorTrimMtl;
	TMap<FString, int32> LoadedMeshMap;

	static FArmyWHCabinetMgr* Singleton;
};

#define gFArmyWHCabinMgr  FArmyWHCabinetMgr::Get()
#define gCabinMgr		(*FArmyWHCabinetMgr::Get())
#define FArmyWHPlaceShapes FArmyWHCabinetMgr::Get()->GetPlaceShapes()