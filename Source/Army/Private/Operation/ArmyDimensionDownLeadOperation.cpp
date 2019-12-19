#pragma once
#include "ArmyDimensionDownLeadOperation.h"
#include "ArmyRectSelect.h"
#include "ArmyDimensions.h"
#include "ArmyToolsModule.h"

FArmyDimensionDownLeadOperation::FArmyDimensionDownLeadOperation(EModelType InBelongModel) :FArmyOperation(InBelongModel)
{

}

void FArmyDimensionDownLeadOperation::BeginOperation(XRArgument InArg /*= XRArgument()*/)
{
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = true;
	FArmyToolsModule::Get().GetRectSelectTool()->End();
	//FArmyToolsModule::Get().GetRectSelectTool()->BIsConstruction = true;//关闭捕捉
	CurrentArg = InArg;

	if (CurrentArg._ArgInt32 == 1)
	{
		FArmyToolsModule::Get().GetRectSelectTool()->End();

		CurrentOperationIndex = 1;
		RectSelect->Start();
		CurrentDimension = MakeShareable(new FArmyDimensions);
	}
}

void FArmyDimensionDownLeadOperation::EndOperation()
{
	//FArmyToolsModule::Get().GetRectSelectTool()->BIsConstruction = false;//关闭捕捉
	CurrentArg = XRArgument();
	CurrentDimension = NULL;
	OperationPoint = NULL;
	CurrentOperationIndex = 0;
	FArmyOperation::EndOperation();
}

void FArmyDimensionDownLeadOperation::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (CurrentDimension.IsValid())
	{
		CurrentDimension->Draw(PDI, View);
	}
}

void FArmyDimensionDownLeadOperation::DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, const FSceneView* View, FCanvas* Canvas)
{
	if (CurrentDimension.IsValid())
	{
		CurrentDimension->DrawHUD(InViewPortClient, ViewPort, View, Canvas);
	}
}

bool FArmyDimensionDownLeadOperation::InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event)
{
	if (Event == IE_Pressed)
	{
		if (OperationPoint.IsValid() && 
			CurrentDimension.IsValid() && 
			FMath::PointDistToSegment(OperationPoint->GetPos(), CurrentDimension->MainLine->GetStart(), CurrentDimension->MainLine->GetEnd()) < 0.001)
		{
			CurrentDimension->StopUseLeadLine();
		}

		EndOperation();
	}
	return false;
}

void FArmyDimensionDownLeadOperation::MouseMove(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y)
{
	CONSTEXPR float MinDistance = 10.0f;

	if (CurrentArg._ArgInt32 == 2)
	{
		if (OperationPoint.IsValid() && CurrentDimension.IsValid())
		{
			FVector2D CapturePoint = FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint();

			FVector TargetPos(CapturePoint, 0);
			const FVector& ClosestPoint = FMath::ClosestPointOnLine(CurrentDimension->MainLine->GetStart(), CurrentDimension->MainLine->GetEnd(), TargetPos);
			if ((ClosestPoint - TargetPos).Size() < MinDistance)
			{
				TargetPos = ClosestPoint;
			}

			OperationPoint->SetPos(TargetPos);
			CurrentDimension->SetForceUseLeadLine();
			CurrentDimension->Refresh();
		}
	}

}

void FArmyDimensionDownLeadOperation::SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPrimitive)
{
	if (Object.IsValid() && (Object->GetType() == OT_InSideWallDimensions || Object->GetType() == OT_OutSideWallDimensions || Object->GetType() == OT_AddWallDimensions || Object->GetType() == OT_DeleteWallDimensions))
	{
		CurrentDimension = StaticCastSharedPtr<FArmyDimensions>(Object);
		OperationPoint = StaticCastSharedPtr<FArmyEditPoint>(InOperationPrimitive);
	}
	//吊顶标注
	//开关标注
	//插座标注
	else if (Object.IsValid() && Object->GetType() == OT_Dimensions)
	{
		FArmyDimensions* Dimensions = Object->AsassignObj< FArmyDimensions >();
		if (Dimensions/*&&Dimensions->ClassType != -1*/)//类型为-1表示为手动标注类型
		{
			CurrentDimension = StaticCastSharedPtr<FArmyDimensions>(Object);
			OperationPoint = StaticCastSharedPtr<FArmyEditPoint>(InOperationPrimitive);
		}
	}
}

FObjectPtr FArmyDimensionDownLeadOperation::GetOperationObject()
{
	return CurrentDimension;
}

TSharedPtr<FArmyEditPoint> FArmyDimensionDownLeadOperation::GetOperationPoint()
{
	return OperationPoint;
}
