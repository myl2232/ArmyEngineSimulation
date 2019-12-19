#include "ArmyDimensionsOperation.h"
#include "ArmySceneData.h"
#include "ArmyDimensions.h"
#include "ArmyToolsModule.h"
#include "ArmySettings.h"
#include "ArmyGameInstance.h"

TSharedPtr<FArmyDimensions> FArmyDimensionsOperation::Exec_AddDimensions(const FVector& Point)
{
	if (!CurrentDimensions.IsValid())
	{
		CurrentDimensions = MakeShareable(new FArmyDimensions());
		CurrentDimensions->SetArrowsType(FArmyDimensions::Diagonal);
		CurrentDimensions->SetType(CurrentType);
		CurrentDimensions->UseLeadLine = true;
		if (CurrentType == OT_InSideWallDimensions)
		{
			CurrentDimensions->SetName(TEXT("InSideWallDimensions"));
			CurrentDimensions->SetTextSize(10);
		}
		else if (CurrentType == OT_OutSideWallDimensions)
		{
			CurrentDimensions->SetName(TEXT("OutSideWallDimensions"));
			CurrentDimensions->SetTextSize(12);
		}
		else
		{
			CurrentDimensions->SetTextSize(GXRSettings->GetSizeDimensionsFontSize());
		}
	}
	if (CurrentDimensions->CurrentState == -1)
	{
		CurrentDimensions->SetBaseStart(Point);
		FArmyToolsModule::Get().GetMouseCaptureTool()->SetCurrentPoint(FVector2D(Point.X, Point.Y));
	}
	else if (CurrentDimensions->CurrentState == 1)
	{
		CurrentDimensions->SetBaseEnd(Point);
	}
	else if (CurrentDimensions->CurrentState == 2)
	{
		CurrentDimensions->CurrentState = 0;
		XRArgument Arg = XRArgument(1).ArgBoolean(true).ArgUint32(E_ConstructionModel).ArgString(CurrentDimensions->GetName());
		FArmySceneData::Get()->Add(CurrentDimensions, Arg);
		//EndOperation();
		CurrentOperation = 2;

		SCOPE_TRANSACTION(TEXT("添加标尺"));
		CurrentDimensions->Create();

		Exec_AddSeriesDimensions(CurrentDimensions);
	}

    return CurrentDimensions;
}
TSharedPtr<FArmyDimensions> FArmyDimensionsOperation::Exec_AddSeriesDimensions(TSharedPtr<FArmyDimensions> PreDimensions)
{
	if (PreDimensions.IsValid())
	{
		CurrentDimensions = MakeShareable(new FArmyDimensions());
		CurrentDimensions->SetArrowsType(FArmyDimensions::Diagonal);
		CurrentDimensions->SetType(PreDimensions->GetType());
		CurrentDimensions->UseLeadLine = true;
		CurrentDimensions->SetTextSize(PreDimensions->GetTextSize());
		CurrentDimensions->SetSeries(PreDimensions->RightStaticPoint->Pos, PreDimensions->RightExtentPoint->Pos, PreDimensions->RightExtentPoint->Pos - PreDimensions->LeftExtentPoint->Pos);
	}
	else if (CurrentDimensions->CurrentState == 3)
	{
		CurrentDimensions->CurrentState = 0;
		XRArgument Arg = XRArgument(1).ArgBoolean(true).ArgUint32(E_ConstructionModel).ArgString(CurrentDimensions->GetName());
		FArmySceneData::Get()->Add(CurrentDimensions, Arg);

		SCOPE_TRANSACTION(TEXT("添加标尺"));
		CurrentDimensions->Create();

		Exec_AddSeriesDimensions(CurrentDimensions);
	}
	return CurrentDimensions;
}
//void FArmyDimensionsOperation::Exec_DeleteDimensions(const FVector& Point)
//{
//	if (CurrentDimensions->CurrentState == 2)
//	{
//		CurrentDimensions->CurrentState = 1;
//	}
//	else if (CurrentDimensions->CurrentState == 1)
//	{
//		CurrentDimensions->CurrentState = -1;
//	}
//	else if (CurrentDimensions->CurrentState == 0)
//	{
//        COMMAND_DELETE(CurrentDimensions);
//		CurrentDimensions = NULL;
//	}
//}
void FArmyDimensionsOperation::DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, const FSceneView* View, FCanvas* Canvas)
{
	if (CurrentDimensions.IsValid())
	{
		CurrentDimensions->DrawHUD(InViewPortClient, ViewPort, View, Canvas);
	}
}
void FArmyDimensionsOperation::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (CurrentDimensions.IsValid())
	{
		CurrentDimensions->Draw(PDI,View);
	}
}
void FArmyDimensionsOperation::BeginOperation(XRArgument InArg)
{
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = true;
	FArmyToolsModule::Get().GetRectSelectTool()->End();
	if (InArg._ArgInt32 == 0)
	{
		CurrentType = OT_Dimensions;
		CurrentOperation = 1;
	}
	else if (InArg._ArgInt32 == 1)
	{
		TArray<FObjectWeakPtr> ObjectArray;
		FArmySceneData::Get()->GetObjects(E_ConstructionModel,CurrentType, ObjectArray);
		FObjectWeakPtr LastObj = ObjectArray.Last();
		if (LastObj.IsValid())
		{
			CurrentOperation = 2;

			FArmyDimensions* Obj = LastObj.Pin()->AsassignObj<FArmyDimensions>();
			if (Obj)
			{
				Exec_AddSeriesDimensions(StaticCastSharedPtr<FArmyDimensions>(LastObj.Pin()));
			}
		}
	}
	else if (InArg._ArgInt32 == 2)
	{
		CurrentType = OT_InSideWallDimensions;
		CurrentOperation = 1;
	}
	else if (InArg._ArgInt32 == 3)
	{
		CurrentType = OT_OutSideWallDimensions;
		CurrentOperation = 1;
	}
}
void FArmyDimensionsOperation::EndOperation()
{
	CurrentDimensions = NULL;
	CurrentOperation = -1;
	FArmyOperation::EndOperation();
}

void FArmyDimensionsOperation::Tick()
{
    if (CurrentDimensions.IsValid())
    {
        CurrentDimensions->UpdateMouse(FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(),0));
    }
}

bool FArmyDimensionsOperation::InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event)
{
    if (InViewPort->KeyState(EKeys::LeftMouseButton) && Event == IE_Pressed)
    {
       // FArmyHomeEditorModule::Get().GetMouseCaptureTool()->SetCurrentPoint(FVector2D(LocalManager.Pin()->GetMousePos()));
		switch (CurrentOperation)
		{
		case 1:
			Exec_AddDimensions(FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0));
			break;
		case 2:
			Exec_AddSeriesDimensions(NULL);
			break;
		default:
			break;
		}
    }
	else if ((InViewPort->KeyState(EKeys::RightMouseButton) || InViewPort->KeyState(EKeys::Escape)) && Event == IE_Pressed)
	{
		EndOperation();
	}
    return false;
}