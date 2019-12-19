#pragma once
#include "CoreMinimal.h"
#include "CollisionQueryParams.h"
#include "ArmyRoom.h"
#include "ArmyWHCabinetAutoMgr.h"
#include "ArmyWHCModeFrontBoardCabModifier.h"
#include "Serialization/JsonWriter.h"
#include "SceneView.h"
#include "SceneManagement.h"

#define ECC_WHCShape ECC_GameTraceChannel1

enum class ECabinetOperationState : int8
{
    Idle,
    AboutToPlaceCabinet,
    PlacingCabinet,
    CabinetPlaced
};

class FSCTShape;
class FShapeRuler;
class ASCTShapeActor;

class ICabinetMoveCommand
{
public:
    ICabinetMoveCommand(class FArmyWHCModeCabinetOperation *InCabOperation) : CabOperation(InCabOperation) {}
    virtual ~ICabinetMoveCommand() {}
    virtual void ProcessCommand(struct FArmyWHCPoly *InPolyRoom, const FVector &InDesiredPosition, FShapeInRoom *InShapeInRoom, const TArray<TSharedPtr<FShapeInRoom>> &InAllShapes) = 0;

protected:
    FArmyWHCModeCabinetOperation *CabOperation;
};

class FRegularMoveCommand final : public ICabinetMoveCommand
{
public:
    FRegularMoveCommand(class FArmyWHCModeCabinetOperation *InCabOperation) : ICabinetMoveCommand(InCabOperation) {}
    virtual void ProcessCommand(struct FArmyWHCPoly *InPolyRoom, const FVector &InDesiredPosition, FShapeInRoom *InShapeInRoom, const TArray<TSharedPtr<FShapeInRoom>> &InAllShapes) override;
private:
    /*
        0 - 完全无碰撞
        1 - 和其它柜子有碰撞，经过了调整，和柜子不再有碰撞了
        2 - 和其它柜子有碰撞，经过了调整，仍然和某个柜子有碰撞
        3 - 在房间的某个角落
        4 - 和房间有碰撞
    */
    int32 ShapeCollision(struct FArmyWHCPoly *InPolyRoom, FShapeInRoom *InShape, FVector &ShapePosition, FArmyWHCRect &OutRect, bool &bOutChangeDir, bool &bOutAutoWidth);
};

class FVentilatorCabOverlappedCommand final : public ICabinetMoveCommand
{
public:
    FVentilatorCabOverlappedCommand(class FArmyWHCModeCabinetOperation *InCabOperation) : ICabinetMoveCommand(InCabOperation) {}
    virtual void ProcessCommand(struct FArmyWHCPoly *InPolyRoom, const FVector &InDesiredPosition, FShapeInRoom *InShapeInRoom, const TArray<TSharedPtr<FShapeInRoom>> &InAllShapes) override;
};

class FVentilatorCabAlignedCommand final : public ICabinetMoveCommand
{
public:
    FVentilatorCabAlignedCommand(class FArmyWHCModeCabinetOperation *InCabOperation) : ICabinetMoveCommand(InCabOperation) {}
    virtual void ProcessCommand(struct FArmyWHCPoly *InPolyRoom, const FVector &InDesiredPosition, FShapeInRoom *InShapeInRoom, const TArray<TSharedPtr<FShapeInRoom>> &InAllShapes) override;
};

class FArmyWHCModeCabinetOperation
{
public:
    struct FAlignedShapeInfo
    {
        FShapeInRoom *PassiveShape;
        FVector RelativeOffset;
        FArmyWHCRect CombinedRect;
    };

public:
    DECLARE_DELEGATE_OneParam(FOnWHCItemSelected, class AActor*)

    enum EHideCommandFlag
    {
        EHideCF_DoorGroup = 0x01,
        EHideCF_OnGroundCab = 0x02,
        EHideCF_HangCab = 0x04,
        EHideCF_Furniture = 0x08,
        EHideCF_Hydropower = 0x10
    };

    enum ERulerVisibilityFlag
    {
        ERulerVF_Self = 0x01,
        ERulerVF_Env = 0x02
    };

    FArmyWHCModeCabinetOperation();
    ~FArmyWHCModeCabinetOperation();

    void OnQuit();
    void BeginMode();
    void EndMode();
    void BeginOperation(struct FCabinetInfo *InCabInfo, struct FCabinetWholeComponent *InCabWholeComponent);
    void ResetOperation();
    void ClearScene();
    void ClearCabinet();
    void ClearWardrobe();
    void ClearOther();

    void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI);

    void ShowShapeWidthRuler(bool bShow);
    void UpdateShapeRulerPosition();
    void SpawnShapeWidthRulers();
    void DestroyShapeWidthRulers();

    void ShowShapeDistanceRuler(bool bShow);
    void UpdateShapeDistanceRulerPosition();
    void SpawnShapeDistanceRulers();
    void DestroyShapeDistanceRulers();

    float GetSelectedShapeLeftMoveRange() const;
    float GetSelectedShapeRightMoveRange() const;
    float GetSelectedShapeBackRange() const;

    void MoveShapeLeft(float Delta);
    void MoveShapeRight(float Delta);
    void MoveShapeBack(float Delta);

    void ShowCabinetDoors(bool bShow);
    void ShowOnGroundCabinets(bool bShow,bool bChangeMode=false);
    void ShowHangCabinets(bool bShow, bool bChangeMode = false);
    void ShowFurniture(bool bShow);
    void ShowHydropower(bool bShow);

    int32 GetHideCommandFlag() const { return HideCommandFlag; }
    int32 GetRulerVisibilityFlag() const { return RulerVisibilityFlag; }
    bool IsAnyWHCItemSelected() const;

    void AddFrontBoardCabinet(FShapeInRoom *InCabinet);
    bool IsFrontBoardCabinet(FShapeInRoom *InCabinet) const;
    void TryModifySelectedCabinet();

    FOnWHCItemSelected& GetWHCItemSelected() { return OnWHCItemSelected; }

    void Callback_LMousePressed(const FVector2D &InMousePosition);
    void Callback_LMouseReleased(const FVector2D &InMousePosition);
    void Callback_RMousePressed(const FVector2D &InMousePosition);
    void Callback_RMouseReleased(const FVector2D &InMousePosition);
    void Callback_KeyPressed(const FString &InKeyStr);
    void Callback_KeyReleased(const FString &InKeyStr);
    void Callback_MouseMove(const FVector2D &InMousePosition);   
    bool Callback_MouseCapturedMove(const FVector2D &InMousePosition);
    void Callback_DeleteSelected();

    // TODO: 临时解决方案
    // 计算选中柜子到墙的距离
    void CalculateShapeDistanceFromWall(FShapeInRoom *InShape);

    class AXRShapeFrame* GetShapeFrameFromSelected() const;
    void UpdateLastSelected(AActor *InNewLastSelected);
    void ClearSelection();
    void UpdateHighlight();

    // 统计清单
    void CabinetList(struct FArmyWHCModeListData& OutWHCModeData, int32 InType);

    void AddOverlappedPair(FShapeInRoom *InActiveShape, FShapeInRoom *InPassiveShape);
    void RemoveOverlappedPairByActive(FShapeInRoom *InActiveShape);
    void RemoveOverlappedPair(FShapeInRoom *InShape);
    bool IsOverlappedPair(FShapeInRoom *InActiveShape, FShapeInRoom *InPassiveShape) const;
    bool IsOverlappedPairByActive(FShapeInRoom *InActiveShape) const;
    bool IsOverlappedPairByPassive(FShapeInRoom *InPassiveShape) const;
    bool IsBelongedToAnyOverlappedPair(FShapeInRoom *InShape) const;
    FShapeInRoom* GetOverlappedPassive(FShapeInRoom *InActiveShape) const;

    void AddAlignedPair(FShapeInRoom *InActiveShape, FShapeInRoom *InPassiveShape);
    void RemoveAlignedPairByActive(FShapeInRoom *InActiveShape);
    void RemoveAlignedPair(FShapeInRoom *InShape);
    bool IsAlignedPair(FShapeInRoom *InActiveShape, FShapeInRoom *InPassiveShape) const;
    bool IsAlignedPairByActive(FShapeInRoom *InActiveShape) const;
    bool IsAlignedPairByPassive(FShapeInRoom *InPassiveShape) const;
    bool IsBelongedToAnyAlignedPair(FShapeInRoom *InShape) const;
    FShapeInRoom* GetAlignedPassive(FShapeInRoom *InActiveShape) const;
    FShapeInRoom* GetAlignedActiveShapeByPassive(FShapeInRoom *InPassiveShape) const;
    const FAlignedShapeInfo* GetAlignedShapeInfoByPassive(FShapeInRoom *InPassiveShape) const;

    FShapeInRoom* CheckOverlapped(FShapeInRoom *InActiveShape, const TArray<TSharedPtr<FShapeInRoom>> &InPlacedShapes, const FVector *InDesiredPosition = nullptr);
    FShapeInRoom* CheckAligned(FShapeInRoom *InActiveShape, const TArray<TSharedPtr<FShapeInRoom>> &InPlacedShapes, float Tolerance, const FVector *InDesiredPosition = nullptr);

private:
    TArray<class FArmyRoom*> GetRooms(const FString &InRoomLabel) const;
	bool PlaceShapeRect(FShapeInRoom *InShape, const FVector2D &InMousePosition);

    bool PlaceShape(FShapeInRoom *InShape, const FVector2D &InMousePosition);
    void PlaceShapeAgainstOtherShapes(FShapeInRoom *InTestShape, FVector &OutPosition);
    void PlaceShapeAgainstWall(FShapeInRoom *InTestShape, FVector &OutPosition);

    // 给定屏幕鼠标点，确定和房间地面或墙面的交点
    bool To3DPosition(const FVector2D &InMousePosition, const TArray<class FArmyRoom*> &InRooms, float InAboveGround, int32 &OutRoomIndex, FVector &OutPositionInRoom) const;
    // 检查某个柜子是否和其它任意已经放入房间的柜子相交
    bool CheckShapeOverlapped(FShapeInRoom *InTestShape) const;
    // 检查两个柜子是否相交
    bool CheckTwoShapeOverlapped(FShapeInRoom *InTestShape, FVector &InTestPosition, FShapeInRoom *InOtherShape, float OffsetLength = 0.0f) const;
    // 检查柜子是否穿过了某个碰撞平面
    bool CheckShapePenetrateWall(FShapeInRoom *InTestShape, FVector &InTestPosition, class AXRWallActor *InWallActor) const;
    // 获取鼠标点选到的柜子
    class AActor* ShapeFrameHitTest(const TArray<struct HActor*> &InHActorArr) const;
    // 获取鼠标点选到的型录组件
    class AActor* ShapeActorHitTest(const TArray<struct HActor*> &InHActorArr) const;
    // 获取鼠标点选到的台面等对象
    class AActor* ShapeAccessoryHitTest(const TArray<struct HActor*> &InHActorArr) const;
    class AActor* ShapeTableHitTest(const TArray<struct HActor*> &InHActorArr) const;
    
    // 清空距离标尺所绑定的Actor
    void ClearDistanceRulerBinding();
    // 切换距离标尺的位置
    void SwitchDistanceRulerBinding(FShapeInRoom *InNewShape);
    // 显隐距离标尺
    void ShowShapeDistanceRulerInternal(bool bShow);

    TArray<struct HActor*> GetHActorArray(int32 X, int32 Y) const;

    void HighlightSelected();
    void DisplayHighlight(FSCTShape *InShape, class AActor *ActorAttachedTo, const FVector &RelLocation, const FRotator &RelRotation);

    // 为插脚柜增加修改项
    void MakeFrontBoardCabChangeItem(FShapeInRoom *InShape, int32 InNumDoors, float InStartPoint, float InEndPoint);

private:
    void Callback_LeftRulerLengthChanged(const FString&);
    void Callback_RightRulerLengthChanged(const FString&);
    void Callback_BackRulerLengthChanged(const FString&);
    void Callback_RoofRulerLengthChanged(const FString&);
    void Callback_FloorRulerLengthChanged(const FString&);

    void Callback_LeftObjRulerLengthChanged(const FString&);
    void Callback_RightObjRulerLengthChanged(const FString&);
    void Callback_BackObjRulerLengthChanged(const FString&);
    void Callback_RoofObjRulerLengthChanged(const FString&);
    void Callback_FloorObjRulerLengthChanged(const FString&);

    void UpdateRuler(FShapeRuler *InRuler, float InLength, const FVector &InRulerLocation);

private:
    ECabinetOperationState OpState;
    uint8 MouseButtonFlag;
    int8 HideCommandFlag;
    int8 RulerVisibilityFlag;
    int8 RulerCreationFlag;
	bool mbFirstMoveShape;

    FVector CachedShapePosition;
    FRotator CachedShapeRotation;

    FShapeInRoom *PlacingShape;

    AActor *LastSelected;
    AActor *LastChildItemSelected;

    class ASCTWireframeActor *SelectedWireframe;

    TArray<class FArmyRoom*> Kitchens;

    TSharedPtr<FShapeRuler> LeftDistanceRuler;
    TSharedPtr<FShapeRuler> RightDistanceRuler;
    TSharedPtr<FShapeRuler> RoofDistanceRuler;
    TSharedPtr<FShapeRuler> FloorDistanceRuler;
    TSharedPtr<FShapeRuler> BackDistanceRuler;

    TSharedPtr<FShapeRuler> LeftObjDistanceRuler;
    TSharedPtr<FShapeRuler> RightObjDistanceRuler;
    TSharedPtr<FShapeRuler> RoofObjDistanceRuler;
    TSharedPtr<FShapeRuler> FloorObjDistanceRuler;
    TSharedPtr<FShapeRuler> BackObjDistanceRuler;

    FArmyWHCFrontBoardCabModifier FrontBoardCabModifier;

    float LeftDistanceCache;
    float RightDistanceCache;
    float BackDistanceCache;

    float LeftObjDistanceCache;
    float RightObjDistanceCache;
    float BackObjDistanceCache;

    FOnWHCItemSelected OnWHCItemSelected;

    TMap<FShapeInRoom*, FShapeInRoom*> OverlappedMap;
    TMap<FShapeInRoom*, FAlignedShapeInfo> AlignedMap;

    TArray<TSharedPtr<ICabinetMoveCommand>> MoveCommandList;
};