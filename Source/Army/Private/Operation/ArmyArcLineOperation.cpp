#pragma once
#include "ArmyArcLineOperation.h"
#include "ArmyMouseCapture.h"
#include "ArmyToolsModule.h"
#include "ArmyGameInstance.h"
#include "ArmyMath.h"

FArmyArcLineOperation::FArmyArcLineOperation(EModelType InBelongModel)
	:FArmyOperation(InBelongModel)
{

}

void FArmyArcLineOperation::SetSelectedPrimitive(TSharedPtr<FArmyPrimitive> InPrimitive,TSharedPtr<FArmyEditPoint> InOperationPoint)
{
	CurrentArcLine = StaticCastSharedPtr<FArmyArcLine>(InPrimitive);
	CurrentOperationPoint = InOperationPoint;
}

bool FArmyArcLineOperation::InputKey(class UArmyEditorViewportClient* InViewPortClient,class FViewport* InViewPort,FKey Key,EInputEvent Event)
{
	if (CacheArg._ArgBoolean)
	{
		if (InViewPortClient->IsOrtho())
		{
			if (Event==IE_Pressed)
			{
				if (Key==EKeys::LeftMouseButton)
				{
					EndOperation();
				}
			}
		}
	}
	return false;
}

void FArmyArcLineOperation::MouseMove(class UArmyEditorViewportClient* InViewPortClient,FViewport* ViewPort,int32 X,int32 Y)
{
	if (CacheArg._ArgBoolean)
	{
		if (CurrentOperationPoint.IsValid()&&CurrentArcLine.IsValid())
		{
			if (CurrentOperationPoint->OperationType==XROP_MOVE)
			{
				//need check huaen
				FVector EditOffSet = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(),0)-CurrentOperationPoint->GetPos();
				FTransform Trans;
				Trans.SetLocation(EditOffSet);
				FVector WorldPosition = CurrentOperationPoint->GetPos();
				FVector Location = Trans.TransformPosition(WorldPosition);

				FVector Start = CurrentArcLine->GetStartPos();
				FVector End = CurrentArcLine->GetEndPos();
				FVector Dir = FVector::CrossProduct((Start-End).GetSafeNormal(),FVector::UpVector);
				FVector Middle = (Start +End)/2;
				FVector Midlle = Middle+10.f*Dir;
				FVector Location1 = FArmyMath::GetProjectionPoint(Location,Middle,Midlle);
				CurrentOperationPoint->SetPos(Location1);
				CurrentArcLine->SetArcMiddlePos(Location1);
				//CurrentArcLine->ApplyTransform(Trans);
			}
			else if (CurrentOperationPoint->OperationType==XROP_SCALE)
			{
			}
			else if (CurrentOperationPoint->OperationType==XROP_ROTATE)
			{
			}
		}
	}
}

void FArmyArcLineOperation::BeginOperation(XRArgument InArg /*= XRArgument()*/)
{
	CacheArg = InArg;
	FArmyToolsModule::Get().GetRectSelectTool()->End();
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = true;
	MouseCaptureStatus = FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel = 0;
}

void FArmyArcLineOperation::EndOperation()
{
	CacheArg.ArgBoolean(false);
	CurrentArcLine = NULL;
	CurrentOperationPoint = NULL;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel = MouseCaptureStatus;
	FArmyOperation::EndOperation();
}

void FArmyArcLineOperation::Draw(FPrimitiveDrawInterface* PDI,const FSceneView* View)
{

}
