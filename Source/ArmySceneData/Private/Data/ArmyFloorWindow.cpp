#include "ArmyFloorWindow.h"
#include "ArmyShapeBoardActor.h"
#include "ArmyWindowActor.h"
#include "ArmyActorConstant.h"

#define FloorWindowModelLength 180
#define FloorWindowModelHeight 240
FArmyFloorWindow::FArmyFloorWindow() : FArmyWindow()
{
	Length = FloorWindowModelLength;
	Height = FloorWindowModelHeight;
	ObjectType = OT_FloorWindow;
	WindowType = 3;
    SetName(TEXT("落地窗"));
	HeightToFloor = 0.0f;
}

FArmyFloorWindow::FArmyFloorWindow(FArmyFloorWindow* Copy) : FArmyWindow(Copy)
{

}

FArmyFloorWindow::~FArmyFloorWindow()
{

}

void FArmyFloorWindow::GenerateWindowPillars(UWorld* World)
{
	GenerateOutPillars(World);
	FVector vertical = HorizontalDirection.RotateAngleAxis(-90, FVector(0, 0, 1)).GetSafeNormal();
	FVector first, second, third, fourth;
	if (WindowType == 1 || WindowType == 2)
	{
		int number = (int)(WindowType)+1;
		TArray<FVector> tempVertices;
		for (int i = 1; i < number; i++)
		{
			tempVertices.Empty();
			first = StartPoint->Pos + i * Length / number* HorizontalDirection + vertical *  MiddlePillarRectLength / 2;
			second = first + HorizontalDirection * MiddlePillarRectLength;
			third = second - vertical * MiddlePillarRectLength;
			fourth = third - HorizontalDirection * MiddlePillarRectLength;
			tempVertices.Push(first + FVector(0, 0, HeightToFloor + BottomOrTopThickness));
			tempVertices.Push(second + FVector(0, 0, HeightToFloor + BottomOrTopThickness));
			tempVertices.Push(third + FVector(0, 0, HeightToFloor + BottomOrTopThickness));
			tempVertices.Push(fourth + FVector(0, 0, HeightToFloor + BottomOrTopThickness));
			HoleWindow->AddPillarPoints(tempVertices, Height - BottomOrTopThickness * 2);

		}
	}
	else if (WindowType == 3)
	{
		TArray<FVector> tempVertices;
		first = StartPoint->Pos + HorizontalDirection * (Length / 3.0f) + vertical * MiddlePillarRectLength / 2;
		second = first + HorizontalDirection * MiddlePillarRectLength;
		third = second - vertical * MiddlePillarRectLength;
		fourth = third - HorizontalDirection * MiddlePillarRectLength;
		tempVertices.Push(first + FVector(0, 0, HeightToFloor + BottomOrTopThickness));
		tempVertices.Push(second + FVector(0, 0, HeightToFloor + BottomOrTopThickness));
		tempVertices.Push(third + FVector(0, 0, HeightToFloor + BottomOrTopThickness));
		tempVertices.Push(fourth + FVector(0, 0, HeightToFloor + BottomOrTopThickness));
		HoleWindow->AddPillarPoints(tempVertices, Height - BottomOrTopThickness * 2);

		tempVertices.Empty();
		first = StartPoint->Pos + vertical * MiddlePillarRectLength / 2;
		second = first + HorizontalDirection * Length;
		third = second - vertical * MiddlePillarRectLength;
		fourth = third - HorizontalDirection * Length;
		tempVertices.Push(first + FVector(0, 0, HeightToFloor + 2.0f / 3.0f* Height));
		tempVertices.Push(second + FVector(0, 0, HeightToFloor + 2.0f / 3.0f* Height));
		tempVertices.Push(third + FVector(0, 0, HeightToFloor + 2.0f / 3.0f* Height));
		tempVertices.Push(fourth + FVector(0, 0, HeightToFloor + 2.0f / 3.0f* Height));
		HoleWindow->AddPillarPoints(tempVertices, MiddlePillarRectLength);


	}

	HoleWindow->CurrentWindowType = FLOOR_WINDOW;
	HoleWindow->WinowLength = Length;
	HoleWindow->WindowHeight = Height;
	HoleWindow->WindowOffsetGroundHeight = 0.0f;
    HoleWindow->Tags.Add(XRActorTag::Immovable);
	HoleWindow->Tags.Add(XRActorTag::Window);
	HoleWindow->Tags.Add(XRActorTag::CanNotDelete);
}
void FArmyFloorWindow::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyWindow::SerializeToJson(JsonWriter);
	SERIALIZEREGISTERCLASS(JsonWriter, FArmyFloorWindow)
}



