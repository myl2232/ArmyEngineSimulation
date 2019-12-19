#include "ArmyAuxiliary.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyMath.h"
#include "ArmyRoom.h"
#include "ArmySceneData.h"
#include "ArmyPolyLine.h"
#include "Engine.h"

#define LOCTEXT_NAMESPACE ""FArmyAuxiliary" 

#undef LOCTEXT_NAMESPACE 

"FArmyAuxiliary::"FArmyAuxiliary()
	:TopLine(MakeShareable(new "FArmyLine)),
	BottomLine(MakeShareable(new "FArmyLine)),
	LeftLine(MakeShareable(new "FArmyLine)),
	RightLine(MakeShareable(new "FArmyLine)),
	YawPolyLine(MakeShareable(new "FArmyPolyline)),
	RollPolyLine(MakeShareable(new "FArmyPolyline)),
	PitchPolyLine(MakeShareable(new "FArmyPolyline)),
	CurrentRoom(nullptr),
	bInRoom(false)
{
	TopLine->SetLineWidth(1.5f);
	BottomLine->SetLineWidth(1.5f);
	LeftLine->SetLineWidth(1.5f);
	RightLine->SetLineWidth(1.5f);

	YawPolyLine->SetLineWidth(1.5f);
	PitchPolyLine->SetLineWidth(1.5f);
	RollPolyLine->SetLineWidth(1.5f);

	YawPolyLine->SetColor(FLinearColor::Gray);
	PitchPolyLine->SetColor(FLinearColor::Gray);
	RollPolyLine->SetColor(FLinearColor::Gray);

	YawPolyLine->SetLineType("FArmyPolyline::Line_Loop);
	PitchPolyLine->SetLineType("FArmyPolyline::Line_Loop);
	RollPolyLine->SetLineType("FArmyPolyline::Line_Loop);

	TopLine->bIsDashLine = true;
	BottomLine->bIsDashLine = true;
	LeftLine->bIsDashLine = true;
	RightLine->bIsDashLine = true;
}

"FArmyAuxiliary::~"FArmyAuxiliary()
{
}

void "FArmyAuxiliary::Init(TSharedPtr<SOverlay> ParentWidget)
{
}

void "FArmyAuxiliary::Draw(FPrimitiveDrawInterface * PDI,const FSceneView * View)
{
	TopLine->Draw(PDI,View);
	BottomLine->Draw(PDI,View);
	LeftLine->Draw(PDI,View);
	RightLine->Draw(PDI,View);
	PitchPolyLine->Draw(PDI,View);
	YawPolyLine->Draw(PDI,View);
	RollPolyLine->Draw(PDI,View);
}

void "FArmyAuxiliary::Draw(const FSceneView * InView,FCanvas * SceneCanvas)
{
}

void "FArmyAuxiliary::Update(UArmyEditorViewportClient * InViwportClient,FVector Position)
{

	PosType = PT_None;
	FVector InMousePoint2D = FVector(Position.X,Position.Y,0);
	TArray<TWeakPtr<"FArmyObject>> RoomList;
	"FArmySceneData::Get ()->GetObjects (E_HomeModel,OT_InternalRoom,RoomList);
	if (RoomList.Num())
	{
		for (auto& RoomObj : RoomList)
		{
			if (RoomObj.Pin()->GetType ()!=OT_InternalRoom)
				continue;
			TSharedPtr<"FArmyRoom> Room = StaticCastSharedPtr<"FArmyRoom>(RoomObj.Pin());
			if (Room->IsPointInRoom(Position))
			{
				if (Room != CurrentRoom)
				{
					CurrentRoom = Room;
					CurrentRoom->GetLines(RoomLines);
					RoomPointes = CurrentRoom->GetWorldPoints(true);
				}
				bInRoom = true;
			
				bool bOnWall = IsOnWall(Position,RoomLines);
				bool bOnTop = IsOnTop(Position);
				bool bOnBottom = IsOnFloor(Position);
				PosType |= "FArmyAuxiliary::PT_SHOWYROOM;
				if (bOnWall)
					PosType |= "FArmyAuxiliary::PT_WALL;
				else
					PosType &=~"FArmyAuxiliary::PT_WALL;
				if (bOnTop)
					PosType |= "FArmyAuxiliary::PT_TOP;
				else
					PosType &= ~"FArmyAuxiliary::PT_TOP;
				if (bOnBottom)
					PosType |= "FArmyAuxiliary::PT_FLOOR;
				else
					PosType &= ~"FArmyAuxiliary::PT_FLOOR;

				UpdatePitch(InViwportClient,Position);
				UpdateYaw(InViwportClient,Position);
				UpdateRoll(InViwportClient,Position);
				break;
			}
		}
	}
	else
	{
		bInRoom = false;
	}

}

void "FArmyAuxiliary::UpdateRoll(UArmyEditorViewportClient* InViwportClient,FVector Position)
{
	FVector Position2D(Position.X,Position.Y,0);
	FVector ForwardPos1,ForwardPos2,ForwardPos3,ForwardPos4;
	float MinLenght1 = -1.f,MinLenght2 = -1.f;
	for (TSharedPtr<"FArmyLine>& RoomLine : RoomLines)
	{
		FVector InterPoint;
		if ("FArmyMath::Line3DIntersectionDirection(RoomLine->GetStart(),RoomLine->GetEnd(),Position2D,FVector::ForwardVector,InterPoint))
		{
			FVector Direction = (Position2D - InterPoint);
			if (MinLenght1 == -1.f)
			{
				MinLenght1 = Direction.Size();
				ForwardPos1 = InterPoint;
			}
			else
			{
				if (MinLenght1 > Direction.Size())
				{
					MinLenght1 = Direction.Size();
					ForwardPos1 = InterPoint;
				}
			}
		}

		if ("FArmyMath::Line3DIntersectionDirection(RoomLine->GetStart(),RoomLine->GetEnd(),Position2D,-FVector::ForwardVector,InterPoint))
		{
			FVector Direction = (Position2D-InterPoint);
			if (MinLenght2==-1.f)
			{
				MinLenght2 = Direction.Size();
				ForwardPos2 = InterPoint;
			}
			else
			{
				if (MinLenght2>Direction.Size())
				{
					MinLenght2 = Direction.Size();
					ForwardPos2 = InterPoint;
				}
			}
		}
	
	}
	ForwardPos3.X = ForwardPos2.X;
	ForwardPos3.Y = ForwardPos2.Y;
	ForwardPos3.Z = 280.f;

	ForwardPos4.X = ForwardPos1.X;
	ForwardPos4.Y = ForwardPos1.Y;
	ForwardPos4.Z = 280.f;
	TArray<FVector> OutVertexes;
	OutVertexes.Add(ForwardPos1);
	OutVertexes.Add(ForwardPos2);
	OutVertexes.Add(ForwardPos3);
	OutVertexes.Add(ForwardPos4);
	RollPolyLine->SetVertices(OutVertexes);
} 

void "FArmyAuxiliary::UpdatePitch(UArmyEditorViewportClient* InViwportClient,FVector Position)
{
	TArray<FVector> Vertexs;
	for (FVector RoomPos : RoomPointes)
	{
		FVector Pos(RoomPos.X,RoomPos.Y,Position.Z);
		Vertexs.Add(Pos);
	}
	if (Vertexs.Num())
	{
		PitchPolyLine->SetVertices(Vertexs);
	}
}

void "FArmyAuxiliary::UpdateYaw(UArmyEditorViewportClient* InViwportClient,FVector Position)
{
	FVector Position2D(Position.X,Position.Y,0);
	FVector ForwardPos1,ForwardPos2,ForwardPos3,ForwardPos4;
	float MinLenght1 = -1.f,MinLenght2 = -1.f;
	for (TSharedPtr<"FArmyLine>& RoomLine:RoomLines)
	{
		FVector InterPoint;
		if ("FArmyMath::Line3DIntersectionDirection(RoomLine->GetStart(),RoomLine->GetEnd(),Position2D,FVector::RightVector,InterPoint))
		{
			FVector Direction = (Position2D-InterPoint);
			if (MinLenght1==-1.f)
			{
				MinLenght1 = Direction.Size();
				ForwardPos1 = InterPoint;
			}
			else
			{
				if (MinLenght1>Direction.Size())
				{
					MinLenght1 = Direction.Size();
					ForwardPos1 = InterPoint;
				}
			}
		}

		if ("FArmyMath::Line3DIntersectionDirection(RoomLine->GetStart(),RoomLine->GetEnd(),Position2D,-FVector::RightVector,InterPoint))
		{
			FVector Direction = (Position2D-InterPoint);
			if (MinLenght2==-1.f)
			{
				MinLenght2 = Direction.Size();
				ForwardPos2 = InterPoint;
			}
			else
			{
				if (MinLenght2>Direction.Size())
				{
					MinLenght2 = Direction.Size();
					ForwardPos2 = InterPoint;
				}
			}
		}

	}
	ForwardPos3.X = ForwardPos2.X;
	ForwardPos3.Y = ForwardPos2.Y;
	ForwardPos3.Z = 280.f;

	ForwardPos4.X = ForwardPos1.X;
	ForwardPos4.Y = ForwardPos1.Y;
	ForwardPos4.Z = 280.f;
	TArray<FVector> OutVertexes;
	OutVertexes.Add(ForwardPos1);
	OutVertexes.Add(ForwardPos2);
	OutVertexes.Add(ForwardPos3);
	OutVertexes.Add(ForwardPos4);
	YawPolyLine->SetVertices(OutVertexes);
}

bool "FArmyAuxiliary::IsOnWall(FVector & InMousePoint,TArray<TSharedPtr<"FArmyLine>>& WallLines)
{
	if (bInRoom)
	{
		for (auto& LineItem : WallLines)
		{
			TSharedPtr<"FArmyLine> Line = LineItem;
			if ("FArmyMath::IsPointOnLine(InMousePoint,Line->GetStart(),Line->GetEnd()))
			{
				return true;
			}
		}
	}
	return false;
}

bool "FArmyAuxiliary::IsOnFloor(FVector & InMousePoint)
{
	if (bInRoom&&FMath::IsNearlyZero(InMousePoint.Z,0.001f))
		return true;
	return false;
}

bool "FArmyAuxiliary::IsOnTop(FVector & InMousePoint)
{
	if (bInRoom&&FMath::IsNearlyZero(InMousePoint.Z-280.f,0.001f))
		return true;
	return false;
}

bool "FArmyAuxiliary::IsOnFloorOrTop(FVector & InMousePoint)
{
	if (IsOnFloor(InMousePoint)||IsOnTop(InMousePoint))
		return true;
	return false;
}
