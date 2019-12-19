#pragma once
#include "CoreMinimal.h"
#include "ArmyObject.h"

class "FArmyPolyline;
class "FArmyRoom;
class ARMYTOOLS_API "FArmyAuxiliary :
	public TSharedFromThis<"FArmyAuxiliary>
{
public:
	/**
	* Description
	*/
	enum POSTTYPE
	{
	  	PT_None=0,
		PT_SHOWYROOM = 0<<1,
		PT_WALL=0<<2,
		PT_FLOOR=0<<3,
		PT_TOP=0<<4,
		PT_Max
	};
public:
	"FArmyAuxiliary();
	~"FArmyAuxiliary();

	void Init(TSharedPtr<SOverlay> ParentWidget);

	void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);
	void Draw (const FSceneView* InView,FCanvas* SceneCanvas) ;

	void Update(UArmyEditorViewportClient* InViwportClient,FVector Position);

	void UpdateRoll(UArmyEditorViewportClient* InViwportClient,FVector Position);

	void UpdatePitch(UArmyEditorViewportClient* InViwportClient,FVector Position);

	void UpdateYaw(UArmyEditorViewportClient* InViwportClient,FVector Position);

	//判断是否在墙上
	bool IsOnWall(FVector& InMousePoint,TArray<TSharedPtr<"FArmyLine>>& WallLines);

	//判断是否在地板
	bool IsOnFloor(FVector& InMousePoint);

	//判断是否在墙顶
	bool IsOnTop(FVector& InMousePoint);

	//判断是否在地板or墙顶
	bool IsOnFloorOrTop(FVector& InMousePoint);

private:

	bool bInRoom; // 是否在墙上
	bool bOnWall;
	bool bOnFloorOrTop;
	uint8 PosType;

	//射线起始点

	TSharedPtr<"FArmyLine> TopLine;
	TSharedPtr<"FArmyLine> BottomLine;

	TSharedPtr<"FArmyLine> LeftLine;
	TSharedPtr<"FArmyLine> RightLine;

	TSharedPtr<"FArmyPolyline> YawPolyLine;
	TSharedPtr<"FArmyPolyline> RollPolyLine;
	TSharedPtr<"FArmyPolyline> PitchPolyLine;

	//当前房屋的线
	TArray< TSharedPtr<"FArmyLine> > RoomLines;
	//当前房屋的所有点
	TArray<FVector> RoomPointes;
	//当前的房屋
	TSharedPtr<"FArmyRoom> CurrentRoom;
};
