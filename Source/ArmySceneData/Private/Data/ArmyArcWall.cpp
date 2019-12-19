#include "ArmyArcWall.h"
#include "Primitive2D/XRArcLine.h"
#include "ArmyObject.h"
#include "ArmyEngine/Public/XREngineModule.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyMath.h"
#include "ArmyEditPoint.h"
#include "ArmyStyle.h"

FArmyArcWall::FArmyArcWall() :CurrentState(-1)
{
	ObjectType = OT_ArcWall;
	LeftStaticPoint = MakeShareable(new FArmyEditPoint());
	RightStaticPoint = MakeShareable(new FArmyEditPoint());
	ArcMiddlePoint = MakeShareable(new FArmyEditPoint());
	MArcLine = MakeShareable(new FArmyArcLine());
	bShowPoint = true;
	MainLine = MakeShareable(new FArmyLine);
	OffsetHeight = 0.0f;
	WallRadius = 0;
	SetState(OS_Normal);
	SetPropertyFlag(FLAG_COLLISION, true);
	bBoundingBox = false;
    SetName(TEXT("弧形墙") + GetUniqueID().ToString());
}


FArmyArcWall::FArmyArcWall(FArmyArcWall* Copy)
{
	PropertyFlag = Copy->PropertyFlag;
	bBoundingBox = Copy->bBoundingBox;
	bShowPoint = Copy->bShowPoint;
	WallRadius = Copy->WallRadius;
	ArcWallAngle = Copy->ArcWallAngle;
	LeftStaticPoint = MakeShareable(new FArmyEditPoint(Copy->LeftStaticPoint.Get()));
	RightStaticPoint = MakeShareable(new FArmyEditPoint(Copy->RightStaticPoint.Get()));
	ArcMiddlePoint = MakeShareable(new FArmyEditPoint(Copy->ArcMiddlePoint.Get()));
	MainLine = MakeShareable(new FArmyLine(Copy->MainLine.Get()));
	MArcLine = MakeShareable(new FArmyArcLine(Copy->MArcLine.Get()));
	OffsetHeight = Copy->OffsetHeight;
	ObjectType = Copy->ObjectType;
	SetName(GetName());
	CurrentState = Copy->CurrentState;
	ResultVertices = Copy->ResultVertices;
	centerPosition = Copy->centerPosition;
	IsUp = Copy->IsUp;
	SetState(OS_Normal);
}

FArmyArcWall::~FArmyArcWall()
{

}
void FArmyArcWall::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);

	JsonWriter->WriteValue(TEXT("LeftStaticPoint"), LeftStaticPoint->Pos.ToString());
	JsonWriter->WriteValue(TEXT("RightStaticPoint"), RightStaticPoint->Pos.ToString());
	JsonWriter->WriteValue(TEXT("ArcMiddlePoint"), ArcMiddlePoint->Pos.ToString());
	SERIALIZEREGISTERCLASS(JsonWriter, FArmyArcWall)
}

void FArmyArcWall::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);

	FVector LeftStaticPoint, RightStaticPoint, ArcMiddlePoint;
	LeftStaticPoint.InitFromString(InJsonData->GetStringField("LeftStaticPoint"));
	RightStaticPoint.InitFromString(InJsonData->GetStringField("RightStaticPoint"));
	ArcMiddlePoint.InitFromString(InJsonData->GetStringField("ArcMiddlePoint"));
	SetArcWallThreePoint(LeftStaticPoint, RightStaticPoint, ArcMiddlePoint);
}
void FArmyArcWall::SetState(EObjectState InState)
{
	State = InState;
	switch (InState)
	{
	case OS_Normal:
		MArcLine->SetBaseColor(FLinearColor::White);
		break;
	case OS_Hovered:
		MArcLine->SetBaseColor(FLinearColor::Blue);
		break;
	case OS_Selected:
		MArcLine->SetBaseColor(FLinearColor::Blue);
		break;
	case OS_Disable:
		MArcLine->SetBaseColor(FLinearColor::Gray);
		break;
	default:
		break;
	}
}
void FArmyArcWall::Refresh()
{
	LeftStaticPoint->SetPos(MArcLine->GetStartPos());
	RightStaticPoint->SetPos(MArcLine->GetEndPos());
	ArcMiddlePoint->SetPos(MArcLine->GetArcMiddlePos());
}
void FArmyArcWall::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (CurrentState != 0)
		{
			MainLine->Draw(PDI, View);
		}
		MArcLine->Draw(PDI, View);
		if (State == OS_Selected && bShowPoint)
		{
			LeftStaticPoint->Draw(PDI, View);
			RightStaticPoint->Draw(PDI, View);
			ArcMiddlePoint->Draw(PDI, View);
		}
	}
}

bool FArmyArcWall::IsSelected(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		centerPosition = MArcLine->GetPosition();
		FVector InterPoint;
		FVector2D StartPix, EndPix, MidPix,PosPix;
		InViewportClient->WorldToPixel(MArcLine->GetStartPos(), StartPix);
		InViewportClient->WorldToPixel(MArcLine->GetEndPos(), EndPix);
		InViewportClient->WorldToPixel(MArcLine->GetArcMiddlePos(), MidPix);
		InViewportClient->WorldToPixel(Pos, PosPix);
		return FArmyMath::ArcPointIntersection(FVector(StartPix,0), FVector(EndPix, 0), FVector(MidPix, 0), FVector(PosPix, 0), 10, InterPoint);
	}
	return false;
}

TSharedPtr<FArmyEditPoint> FArmyArcWall::SelectPoint(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	TSharedPtr<FArmyEditPoint> ResultPoint = NULL;
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		DeselectPoints();
		if (GetState() == OS_Selected)
		{
			if (LeftStaticPoint->IsSelected(Pos, InViewportClient)) ResultPoint = LeftStaticPoint;
			else if (RightStaticPoint->IsSelected(Pos, InViewportClient)) ResultPoint = RightStaticPoint;
			else if (ArcMiddlePoint->IsSelected(Pos, InViewportClient)) ResultPoint = ArcMiddlePoint;

			if (ResultPoint.IsValid())
				ResultPoint->SetState(FArmyEditPoint::OPS_Selected);
		}
	}
	return ResultPoint;
}

TSharedPtr<FArmyEditPoint> FArmyArcWall::HoverPoint(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	TSharedPtr<FArmyEditPoint> ResultPoint = NULL;
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (LeftStaticPoint->GetState() != FArmyEditPoint::OPS_Selected && LeftStaticPoint->Hover(Pos, InViewportClient))    ResultPoint = LeftStaticPoint;
		else if (RightStaticPoint->GetState() != FArmyEditPoint::OPS_Selected && RightStaticPoint->Hover(Pos, InViewportClient))   ResultPoint = RightStaticPoint;
		else if (ArcMiddlePoint->GetState() != FArmyEditPoint::OPS_Selected && ArcMiddlePoint->Hover(Pos, InViewportClient)) ResultPoint = ArcMiddlePoint;

		if (ResultPoint.IsValid()) ResultPoint->SetState(FArmyEditPoint::OPS_Hovered);
	}
	return ResultPoint;

}

bool FArmyArcWall::Hover(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		return IsSelected(Pos, InViewportClient);
	}
	return false;
}

void FArmyArcWall::GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs)
{
	//OutLines.Push(MainLine);
}

void const FArmyArcWall::GetAlonePoints(TArray< TSharedPtr<FArmyPoint> >& OutPoints)
{
	OutPoints.Push(MakeShareable(new FArmyPoint(LeftStaticPoint->Pos)));
	OutPoints.Push(MakeShareable(new FArmyPoint(RightStaticPoint->Pos)));
	OutPoints.Push(MakeShareable(new FArmyPoint(ArcMiddlePoint->Pos)));

}

void FArmyArcWall::GetCircleLines(TArray<TSharedPtr<FArmyCircle>>& OutCircle)
{
	//OutCircle.Push(MArcLine);
}

void FArmyArcWall::GetArcLines(TArray<TSharedPtr<class FArmyArcLine>>& OutArcLines)
{
	OutArcLines.Push(MArcLine);
}

const FBox FArmyArcWall::GetBounds()
{
	return FBox(MArcLine->GetVertices());
}

EObjectType FArmyArcWall::GetType() const
{
	return OT_ArcWall;
}

void FArmyArcWall::SetArcWallStart(FVector InStartPoint)
{
	if (CurrentState == -1)
	{
		CurrentState = 1;
	}
	LeftStaticPoint->Pos = InStartPoint;
	RightStaticPoint->Pos = InStartPoint;
	UpdateAuxiliaryLine();
}

void FArmyArcWall::SetArcWallEnd(FVector InEndPoint)
{
	if (CurrentState == -1)
	{
		return;
	}
	else if (CurrentState == 1)
	{
		CurrentState = 2;
	}
	RightStaticPoint->Pos = InEndPoint;
	ArcMiddlePoint->Pos = (LeftStaticPoint->Pos + RightStaticPoint->Pos) / 2;
	UpdateAuxiliaryLine();
}

void FArmyArcWall::SetMousePos(FVector InMousePos)
{
	FVector horizontal = (RightStaticPoint->Pos - LeftStaticPoint->Pos).GetSafeNormal();
	FVector projectionPoint = FMath::ClosestPointOnInfiniteLine(LeftStaticPoint->Pos, RightStaticPoint->Pos, InMousePos);
	FVector offset = (InMousePos - projectionPoint);
	ArcMiddlePoint->Pos = (LeftStaticPoint->Pos + RightStaticPoint->Pos) / 2 + offset;
	UpdateArcVertices();

}

void FArmyArcWall::UpdateMousePosition(FVector InMousePosition)
{
	switch (CurrentState)
	{
	case 1:
	{
		RightStaticPoint->Pos = InMousePosition;
		UpdateAuxiliaryLine();
	}
	break;
	case 2:
	{
		SetMousePos(InMousePosition);
		UpdateArcVertices();
	}
	default:
		break;
	}
}

void FArmyArcWall::UpdateAuxiliaryLine()
{
	MainLine->SetStart(LeftStaticPoint->Pos);
	MainLine->SetEnd(RightStaticPoint->Pos);
}

void FArmyArcWall::SetArcWallThreePoint(FVector InStartPos, FVector InEndPos, FVector InMiddlePos)
{
	LeftStaticPoint->Pos = InStartPos;
	RightStaticPoint->Pos = InEndPos;
	ArcMiddlePoint->Pos = InMiddlePos;
	UpdateArcVertices();
}

void FArmyArcWall::UpdateArcVertices()
{
	MArcLine->SetTreePointPos(LeftStaticPoint->Pos, RightStaticPoint->Pos, ArcMiddlePoint->Pos);
}

void FArmyArcWall::SetOffsetHeight(float dist)
{
	CurrentState = 0;
	FVector middleDown = (LeftStaticPoint->Pos + RightStaticPoint->Pos) / 2;
	FVector vertical = (ArcMiddlePoint->Pos - middleDown).GetSafeNormal();
	ArcMiddlePoint->Pos = middleDown + vertical * dist;
	UpdateArcVertices();
}

float FArmyArcWall::GetOffsetHeight() const
{
	FVector middle = (LeftStaticPoint->Pos + RightStaticPoint->Pos) / 2;
	return (ArcMiddlePoint->Pos - middle).Size();

}

float FArmyArcWall::GetWallRadius() const
{
	return MArcLine->GetRadius();
}

FVector FArmyArcWall::GetCenterPosition() const
{
	return MArcLine->GetPosition();
}

float FArmyArcWall::GetArcAngle() const
{
	return MArcLine->GetArcAngle();
}

bool FArmyArcWall::GetIsUp()
{
	return MArcLine->GetUpDirection();
}

const TArray<FVector>& FArmyArcWall::GetResultVertices() const
{
	return MArcLine->GetVertices();
}

void FArmyArcWall::Generate(UWorld* InWorld)
{
}

void FArmyArcWall::DeselectPoints()
{
	LeftStaticPoint->SetState(FArmyEditPoint::OPS_Normal);
	RightStaticPoint->SetState(FArmyEditPoint::OPS_Normal);
	ArcMiddlePoint->SetState(FArmyEditPoint::OPS_Normal);
}
