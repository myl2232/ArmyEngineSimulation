#include "ArmyArcWindow.h"
#include "ArmyArcLine.h"
#include "ArmyLine.h"
#include "ArmyShapeBoardActor.h"
#include "ArmyStyle.h"
#include "ArmyEngineModule.h"
#include "ArmyMath.h"

#define  ArcWindowGlassWidth 10
#define ArcWindowLength 60.f
#define  ArcLineNumSides 128

FArmyArcWindow::FArmyArcWindow()
{
	ObjectType = OT_ArcWindow;
	LeftStaticPoint = MakeShareable(new FArmyEditPoint());
	RightStaticPoint = MakeShareable(new FArmyEditPoint());
	ArcMiddlePoint = MakeShareable(new FArmyEditPoint());
	LeftLine = MakeShareable(new FArmyLine());
	RightLine = MakeShareable(new FArmyLine());
	OutArcLine = MakeShareable(new FArmyArcLine());
	OutArcLine->NumSides = ArcLineNumSides;
	InnearArcLine = MakeShareable(new FArmyArcLine());
	InnearArcLine->NumSides = ArcLineNumSides;
	WindowSill = NULL;
	WindowGlass = NULL;
	WindowWall = NULL;
	WindowPillar = NULL;
	MiddleUpArcLine = MakeShareable(new FArmyArcLine());
	MiddleUpArcLine->NumSides = ArcLineNumSides;
	MiddleDownArcLine = MakeShareable(new FArmyArcLine());
	MiddleDownArcLine->NumSides = ArcLineNumSides;

    SetName(TEXT("弧形窗")/* + GetUniqueID().ToString()*/);
	SetState(OS_Normal);

	HeightToFloor = 50.0f;
	WindowHeight = 180.0f;
}

FArmyArcWindow::FArmyArcWindow(FArmyArcWindow* Copy)
{
	PropertyFlag = Copy->PropertyFlag;
	LeftStaticPoint = MakeShareable(new FArmyEditPoint(Copy->LeftStaticPoint.Get()));
	RightStaticPoint = MakeShareable(new FArmyEditPoint(Copy->RightStaticPoint.Get()));
	ArcMiddlePoint = MakeShareable(new FArmyEditPoint(Copy->ArcMiddlePoint.Get()));

	OutArcLine = MakeShareable(new FArmyArcLine(Copy->OutArcLine.Get()));
	InnearArcLine = MakeShareable(new FArmyArcLine(Copy->InnearArcLine.Get()));
	MiddleUpArcLine = MakeShareable(new FArmyArcLine(Copy->MiddleUpArcLine.Get()));
	MiddleDownArcLine = MakeShareable(new FArmyArcLine(Copy->MiddleDownArcLine.Get()));
	LeftLine = MakeShareable(new FArmyLine(Copy->LeftLine.Get()));
	RightLine = MakeShareable(new FArmyLine(Copy->RightLine.Get()));
    SetName(Copy->GetName());
	ArcWindowMiddlePos = Copy->ArcWindowMiddlePos;
	ArcWindowStartPos = Copy->ArcWindowStartPos;
	ArcWindowEndPos = Copy->ArcWindowEndPos;
	ArcWindowAngle = Copy->ArcWindowAngle;
	ArcWindowCenterPos = Copy->ArcWindowCenterPos;
	ArcWindowThickness = Copy->ArcWindowThickness;
	WindowGlass = Copy->WindowGlass;
	WindowSill = Copy->WindowSill;
	WindowWall = Copy->WindowWall;
	WindowPillar = Copy->WindowPillar;
	HeightToFloor = Copy->HeightToFloor;
	WindowHeight = Copy->WindowHeight;
	SetState(OS_Normal);
}

void FArmyArcWindow::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
	JsonWriter->WriteValue(TEXT("LeftStaticPoint"), *(LeftStaticPoint->Pos.ToString()));
	JsonWriter->WriteValue(TEXT("RightStaticPoint"), *(RightStaticPoint->Pos.ToString()));
	JsonWriter->WriteValue(TEXT("ArcWindowCenterPos"), *(ArcWindowCenterPos.ToString()));
	JsonWriter->WriteValue(TEXT("HeightToFloor"), HeightToFloor);
	JsonWriter->WriteValue(TEXT("WindowHeight"), WindowHeight);
	JsonWriter->WriteValue(TEXT("ArcWindowThickness"), ArcWindowThickness);
	SERIALIZEREGISTERCLASS(JsonWriter, FArmyArcWindow)
}

void FArmyArcWindow::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);

	ArcWindowStartPos.InitFromString(InJsonData->GetStringField("LeftStaticPoint"));
	ArcWindowEndPos.InitFromString(InJsonData->GetStringField("RightStaticPoint"));
	ArcWindowCenterPos.InitFromString(InJsonData->GetStringField("ArcWindowCenterPos"));
	LeftStaticPoint->Pos = ArcWindowStartPos;
	RightStaticPoint->Pos = ArcWindowEndPos;
	
	HeightToFloor = InJsonData->GetNumberField("HeightToFloor");
	WindowHeight = InJsonData->GetNumberField("WindowHeight");
	ArcWindowThickness = InJsonData->GetNumberField("ArcWindowThickness");

	UpdateWindow();


}

void FArmyArcWindow::SetState(EObjectState InState)
{
	State = InState;
	switch (InState)
	{
	case OS_Normal:
		OutArcLine->SetBaseColor(FLinearColor::Green);
		InnearArcLine->SetBaseColor(FLinearColor::Green);
		MiddleUpArcLine->SetBaseColor(FLinearColor::Green);
		MiddleDownArcLine->SetBaseColor(FLinearColor::Green);
		LeftLine->SetBaseColor(FLinearColor::Green);
		RightLine->SetBaseColor(FLinearColor::Green);
		DeselectPoints();
		break;
	case OS_Hovered:
	case OS_Selected:
		OutArcLine->SetBaseColor(FLinearColor::Blue);
		InnearArcLine->SetBaseColor(FLinearColor::Blue);
		MiddleUpArcLine->SetBaseColor(FLinearColor::Blue);
		MiddleDownArcLine->SetBaseColor(FLinearColor::Blue);
		LeftLine->SetBaseColor(FLinearColor::Blue);
		RightLine->SetBaseColor(FLinearColor::Blue);
		break;
	case OS_Disable:
		OutArcLine->SetBaseColor(FLinearColor::Gray);
		InnearArcLine->SetBaseColor(FLinearColor::Gray);
		MiddleUpArcLine->SetBaseColor(FLinearColor::Gray);
		MiddleDownArcLine->SetBaseColor(FLinearColor::Gray);
		LeftLine->SetBaseColor(FLinearColor::Gray);
		RightLine->SetBaseColor(FLinearColor::Gray);
		break;
	default:
		break;
	}
}

void FArmyArcWindow::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		OutArcLine->Draw(PDI, View);
		InnearArcLine->Draw(PDI, View);
		MiddleUpArcLine->Draw(PDI, View);
		MiddleDownArcLine->Draw(PDI, View);
		LeftLine->Draw(PDI, View);
		RightLine->Draw(PDI, View);
		if (State == OS_Selected)
		{
			LeftStaticPoint->Draw(PDI, View);
			RightStaticPoint->Draw(PDI, View);
		//	ArcMiddlePoint->Draw(PDI, View);
		}
	}
}

bool FArmyArcWindow::IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
	//	FVector InterPoint;
	//	FVector2D StartPix, EndPix, MidPix, PosPix, CenterPix;
		float angel = InnearArcLine->GetArcAngle();
		FVector temp0 = (ArcMiddlePoint->Pos - ArcWindowCenterPos).GetSafeNormal();
		FVector temp1 = (Pos - ArcWindowCenterPos).GetSafeNormal();
		float thetha = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(temp1, temp0)));
		float radis = FVector::Dist(Pos, ArcWindowCenterPos);
		float dist = radis - InnearArcLine->GetRadius();
		if (dist >= 0 && dist <= ArcWindowThickness && thetha <= angel / 2.0f)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool FArmyArcWindow::Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	return IsSelected(Pos, InViewportClient);
}

const FBox FArmyArcWindow::GetBounds()
{
	return FBox(ForceInitToZero);
}

TSharedPtr<FArmyEditPoint> FArmyArcWindow::SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
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

TSharedPtr<FArmyEditPoint> FArmyArcWindow::HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
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

void FArmyArcWindow::GenerateWindowModel(UWorld* World)
{
	if (WindowSill == NULL)
	{
		WindowSill = World->SpawnActor<AXRShapeActor>(AXRShapeActor::StaticClass());
		WindowSill->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWindowBaseMaterial());
	}
	if (WindowGlass == NULL)
	{
		WindowGlass = World->SpawnActor<AXRShapeActor>(AXRShapeActor::StaticClass());
		WindowGlass->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetGlassMaterial());
	}
	if (WindowPillar == NULL)
	{
		WindowPillar = World->SpawnActor<AXRShapeActor>(AXRShapeActor::StaticClass());
		WindowPillar->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWindowPillarMaterial());
	}
	if (WindowWall == NULL)
	{
		WindowWall = World->SpawnActor<AXRShapeActor>(AXRShapeActor::StaticClass());
		WindowWall->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWallMaterial());
	}
	GenerateWindowSill();
	GenerateWindowWall();
	GenerateWindowGlass();
	GenerateWindowPillar();

}

void FArmyArcWindow::GenerateWindowSill()
{
	TArray<FVector> innearVertices = InnearArcLine->GetVertices();
	TArray<FVector> outVertices = OutArcLine->GetVertices();
	FArmyMath::ReversePointList(innearVertices);
	outVertices.Append(innearVertices);
	int number = outVertices.Num();
	TArray<FVector> BottomVerices, TopVertices;
	for (int i = 0; i < number; i++)
	{
		BottomVerices.Push(outVertices[i] + FVector(0, 0, HeightToFloor));
	}
	for (int i = 0; i < number; i++)
	{
		TopVertices.Push(outVertices[i] + FVector(0, 0, HeightToFloor + WindowHeight - 5.0f));
	}
	WindowSill->ResetMeshTriangles();
	WindowSill->AddBoardPoints(BottomVerices, 5.0f);
	WindowSill->AddBoardPoints(TopVertices, 5.0f);
	WindowSill->UpdateAllVetexBufferIndexBuffer();
}

void FArmyArcWindow::GenerateWindowGlass()
{
	TArray<FVector> glassVerices = CaculatePolygonVertices(2.0f, HeightToFloor + 5.0f);
	WindowGlass->ResetMeshTriangles();
	WindowGlass->AddBoardPoints(glassVerices, WindowHeight - 10.0f);
	WindowGlass->UpdateAllVetexBufferIndexBuffer();
}
#define  PillarHeight 4.0f
#define UpPillarWidth 2.0f
void FArmyArcWindow::GenerateWindowPillar()
{
	TArray<FVector> UpPillarVerice = CaculatePolygonVertices(3.0f, HeightToFloor + WindowHeight - 5.0f - PillarHeight);
	TArray<FVector> DownPillarVerice = CaculatePolygonVertices(3.0f, HeightToFloor + 5.0f);
	TArray<FVector> MiddlePillarVertice = CaculatePolygonVertices(3.0f, HeightToFloor + 1.0 / 3.0 * WindowHeight);
	WindowPillar->ResetMeshTriangles();
	WindowPillar->AddBoardPoints(UpPillarVerice, PillarHeight);
	WindowPillar->AddBoardPoints(DownPillarVerice, PillarHeight);
	WindowPillar->AddBoardPoints(MiddlePillarVertice, PillarHeight);
	TSharedPtr<FArmyArcLine> tempCircle = MakeShareable(new FArmyArcLine());
	UpdateCirclePos(tempCircle, 0.0f);
	int numberOfWindow = FMath::FloorToInt(InnearArcLine->GetCircleLength() / ArcWindowLength);
	float deltaAngle = InnearArcLine->GetArcAngle() / numberOfWindow;
	TArray<FVector> innearVertices, outVertices;
	for (int i = 0; i <= numberOfWindow; i++)
	{
		FVector tempVertice = FRotator(0, deltaAngle * i, 0).RotateVector(tempCircle->GetStartPos() - tempCircle->GetPosition()) + tempCircle->GetPosition();
		FVector first = tempVertice + FVector(-UpPillarWidth / 2.0f, UpPillarWidth / 2.0f, 0) + FVector(0, 0, HeightToFloor + 5.0f + PillarHeight);
		FVector second = tempVertice + FVector(UpPillarWidth / 2.0f, UpPillarWidth / 2.0f, 0) + FVector(0, 0, HeightToFloor + 5.0f + PillarHeight);
		FVector third = tempVertice + FVector(UpPillarWidth / 2.0f, -UpPillarWidth / 2.0f, 0) + FVector(0, 0, HeightToFloor + 5.0f + PillarHeight);
		FVector fourth = tempVertice + FVector(-UpPillarWidth / 2.0f, -UpPillarWidth / 2.0f, 0) + FVector(0, 0, HeightToFloor + 5.0f + PillarHeight);

		TArray<FVector> tempPillarVers = { first,second,third,fourth };
		WindowPillar->AddBoardPoints(tempPillarVers, WindowHeight - 5 - PillarHeight);
	}
	WindowPillar->UpdateAllVetexBufferIndexBuffer();
}

void FArmyArcWindow::GenerateWindowWall()
{
	FVector first = InnearArcLine->GetStartPos() + FVector(0, 0, HeightToFloor);
	FVector second = OutArcLine->GetStartPos() + FVector(0, 0, HeightToFloor);
	FVector third = first + FVector(0, 0, WindowHeight);
	FVector fourth = second + FVector(0, 0, WindowHeight);
	TArray<FVector> leftWall = { first,third,fourth,second };
	first = InnearArcLine->GetEndPos() + FVector(0, 0, HeightToFloor);
	second = OutArcLine->GetEndPos() + FVector(0, 0, HeightToFloor);
	third = first + FVector(0, 0, WindowHeight);
	fourth = second + FVector(0, 0, WindowHeight);
	TArray<FVector> RighWall = { first,third,fourth,second };
	WindowWall->ResetMeshTriangles();
	WindowWall->AddQuatGemometry(leftWall);
	WindowWall->AddQuatGemometry(RighWall);

	WindowWall->UpdateAllVetexBufferIndexBuffer();
}


TArray<FVector> FArmyArcWindow::CaculatePolygonVertices(float InThickNess, float InHeight)
{
	TSharedPtr<FArmyArcLine> tempInner = MakeShareable(new FArmyArcLine());
	TSharedPtr<FArmyArcLine> tempOuter = MakeShareable(new FArmyArcLine());
	UpdateCirclePos(tempOuter, InThickNess / 2.0f);
	UpdateCirclePos(tempInner, -InThickNess / 2.0f);

	int numberOfWindow = FMath::FloorToInt(InnearArcLine->GetCircleLength() / ArcWindowLength);
	float deltaAngle = InnearArcLine->GetArcAngle() / numberOfWindow;
	TArray<FVector> innearVertices, outVertices;
	for (int i = 0; i <= numberOfWindow; i++)
	{
		FVector tempVertice = FRotator(0, deltaAngle * i, 0).RotateVector(tempInner->GetStartPos() - tempInner->GetPosition()) + tempInner->GetPosition() + FVector(0, 0, InHeight);
		innearVertices.Push(tempVertice);
	}
	for (int i = 0; i <= numberOfWindow; i++)
	{
		FVector tempVertice = FRotator(0, deltaAngle * i, 0).RotateVector(tempOuter->GetStartPos() - tempOuter->GetPosition()) + tempOuter->GetPosition() + FVector(0, 0, InHeight);
		outVertices.Push(tempVertice);
	}
	tempInner = NULL;
	tempOuter = NULL;
	FArmyMath::ReversePointList(innearVertices);
	outVertices.Append(innearVertices);

	return outVertices;

}

EObjectType FArmyArcWindow::GetType() const
{
	return OT_ArcWindow;
}

void FArmyArcWindow::DestroyWindowModel()
{
	if (WindowSill->IsValidLowLevel())
	{
		WindowSill->Destroy();
		WindowSill = NULL;
	}
	if (WindowGlass->IsValidLowLevel())
	{
		WindowGlass->Destroy();
		WindowGlass = NULL;
	}
	if (WindowPillar->IsValidLowLevel())
	{
		WindowPillar->Destroy();
		WindowPillar = NULL;
	}
	if (WindowWall->IsValidLowLevel())
	{
		WindowWall->Destroy();
		WindowWall = NULL;
	}
}

TArray<FVector> FArmyArcWindow::GetClipingBox()
{
	FVector leftBottom = InnearArcLine->GetStartPos();
	FVector rightBottom = InnearArcLine->GetEndPos();
	FVector LeftUp = OutArcLine->GetStartPos();
	FVector RightUp = OutArcLine->GetEndPos();
	FVector MiddleUp = OutArcLine->GetArcMiddlePos() + (OutArcLine->GetArcMiddlePos() - OutArcLine->GetPosition()).GetSafeNormal()*5.0f;
	FVector centerPos = OutArcLine->GetPosition();
	FVector middleDirection = (MiddleUp - centerPos).RotateAngleAxis(-90, FVector(0, 0, 1)).GetSafeNormal();
	FVector UpCenter = MiddleUp + middleDirection * 10.0f;
	FVector2D leftIntersection;
	FVector2D RightInterseciotn;
	FArmyMath::Line2DIntersection(FVector2D(leftBottom), FVector2D(LeftUp), FVector2D(MiddleUp), FVector2D(UpCenter), leftIntersection);
	FArmyMath::Line2DIntersection(FVector2D(rightBottom), FVector2D(RightUp), FVector2D(MiddleUp), FVector2D(UpCenter), RightInterseciotn);
	FVector offset = FVector(0, 0, HeightToFloor + WindowHeight / 2.0f);
	TArray<FVector> results = { leftBottom + offset,FVector(leftIntersection,0.0f) + offset,FVector(RightInterseciotn,0.0f) + offset,rightBottom + offset };

	return results;
}

void FArmyArcWindow::SetArcWindowStartPos(const FVector& InStartPos)
{
	ArcWindowStartPos = InStartPos;
	LeftStaticPoint->Pos = InStartPos;
	UpdateWindow();
}

void FArmyArcWindow::SetArcWindowEndPos(const FVector& InEndPos)
{
	ArcWindowEndPos = InEndPos;
	RightStaticPoint->Pos = InEndPos;
	UpdateWindow();
}

void FArmyArcWindow::SetArcWindowStartAndEndPos(const FVector& InStartPos, const FVector& InEndPos, const FVector& InCenterPos)
{
	ArcWindowStartPos = InStartPos;
	LeftStaticPoint->Pos = InStartPos;
	ArcWindowEndPos = InEndPos;
	RightStaticPoint->Pos = InEndPos;
	ArcWindowCenterPos = InCenterPos;

	UpdateWindow();
}

void FArmyArcWindow::SetArcWindowEndAndCenterPos(const FVector& InEndpos, const FVector& InCenterPos)
{
	ArcWindowEndPos = InEndpos;
	ArcWindowCenterPos = InCenterPos;
	RightStaticPoint->Pos = InEndpos;

	UpdateWindow();
}

void FArmyArcWindow::SetArcWindowThickness(float InThickness)
{
	ArcWindowThickness = InThickness;
	UpdateWindow();
}

void FArmyArcWindow::SetProperty(FVector StartPos, FVector EndPos, FVector MiddlePos, float Thickness)
{
	ArcWindowStartPos = StartPos;
	ArcWindowMiddlePos = MiddlePos;
	ArcWindowEndPos = EndPos;
	ArcWindowThickness = Thickness;
	UpdateWindow();

}

void FArmyArcWindow::SetWindowHeight(float InHeight)
{
	WindowHeight = InHeight;
}

void FArmyArcWindow::SetWindowOffGroundHeight(float InOffsetGround)
{
	HeightToFloor = InOffsetGround;
}

void FArmyArcWindow::UpdateWindow()
{
	ArcWindowAngle = FMath::Acos(FVector::DotProduct((ArcWindowStartPos - ArcWindowCenterPos).GetSafeNormal(), (ArcWindowEndPos - ArcWindowCenterPos).GetSafeNormal()));
	FVector2D p0p1 = FVector2D(ArcWindowCenterPos - ArcWindowStartPos);
	FVector2D p1p2 = FVector2D(ArcWindowEndPos - ArcWindowCenterPos);
	if (FVector2D::CrossProduct(p0p1, p1p2) > 0)
	{
		ArcWindowMiddlePos = FRotator(0, -FMath::RadiansToDegrees(ArcWindowAngle / 2), 0).RotateVector(ArcWindowStartPos - ArcWindowCenterPos) + ArcWindowCenterPos;
	}
	else
	{
		ArcWindowMiddlePos = FRotator(0, FMath::RadiansToDegrees(ArcWindowAngle / 2), 0).RotateVector(ArcWindowStartPos - ArcWindowCenterPos) + ArcWindowCenterPos;
	}
	ArcMiddlePoint->Pos = ArcWindowMiddlePos;
	UpdateCirclePos(OutArcLine, ArcWindowThickness / 2.0f);
	UpdateCirclePos(InnearArcLine, -ArcWindowThickness / 2.0f);
	UpdateCirclePos(MiddleUpArcLine, ArcWindowGlassWidth / 2.0f);
	UpdateCirclePos(MiddleDownArcLine, -ArcWindowGlassWidth / 2.0f);
	LeftLine->SetStart(OutArcLine->GetStartPos());
	LeftLine->SetEnd(InnearArcLine->GetStartPos());
	RightLine->SetStart(OutArcLine->GetEndPos());
	RightLine->SetEnd(InnearArcLine->GetEndPos());
}

void FArmyArcWindow::UpdateCirclePos(TSharedPtr<class FArmyArcLine> InCircle, float offset)
{
	float angle = FMath::RadiansToDegrees(ArcWindowAngle);
	FVector middlePoint = ArcWindowMiddlePos + (ArcWindowMiddlePos - ArcWindowCenterPos).GetSafeNormal()*offset;
	FVector endPoint = FRotator(0, angle / 2, 0).RotateVector(middlePoint - ArcWindowCenterPos) + ArcWindowCenterPos;
	FVector startPoint = FRotator(0, -angle / 2, 0).RotateVector(middlePoint - ArcWindowCenterPos) + ArcWindowCenterPos;

	InCircle->SetTreePointPos(startPoint, endPoint, middlePoint);

}

void FArmyArcWindow::DeselectPoints()
{
	LeftStaticPoint->SetState(FArmyEditPoint::OPS_Normal);
	RightStaticPoint->SetState(FArmyEditPoint::OPS_Normal);
	ArcMiddlePoint->SetState(FArmyEditPoint::OPS_Normal);
}

void FArmyArcWindow::Generate(UWorld* InWorld)
{
	GenerateWindowModel(InWorld);
}
