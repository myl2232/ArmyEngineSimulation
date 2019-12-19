#pragma once
#include "ArmyIndexSymbolOperation.h"
#include "ArmyMouseCapture.h"
#include "ArmyToolsModule.h"
#include "ArmyGameInstance.h"

FArmyIndexSymbolOperation::FArmyIndexSymbolOperation(EModelType InBelongModel)
    : FArmyOperation(InBelongModel)
{
	
}
void FArmyIndexSymbolOperation::SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPrimitive)
{
	if (Object->AsassignObj<FArmySymbolRectRegion>())
	{
		CurrentOperationObject = StaticCastSharedPtr<FArmySymbolRectRegion>(Object);
		CurrentOperationPoint = StaticCastSharedPtr<FArmyEditPoint>(InOperationPrimitive);
		CurrentOperationPoint->SetState(FArmyPrimitive::OPS_Selected);
	}
}
bool FArmyIndexSymbolOperation::InputKey(UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event)
{
	if (CacheArg._ArgBoolean)
	{
		if (Event == IE_Pressed && (Key == EKeys::LeftMouseButton || Key == EKeys::RightMouseButton))
		{
			EndOperation();
		}
	}
	return false;
}
void FArmyIndexSymbolOperation::MouseMove(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y)
{
	if (CacheArg._ArgBoolean)
	{
		if (CurrentOperationPoint.IsValid() && CurrentOperationObject.IsValid())
		{
			FVector EditOffSet = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0) - CurrentOperationPoint->GetPos();
			CurrentOperationObject->Update(CurrentOperationPoint, EditOffSet);
		}
	}
}
void FArmyIndexSymbolOperation::MouseDrag(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, FKey Key)
{

}
void FArmyIndexSymbolOperation::BeginOperation(XRArgument InArg)
{
	CacheArg = InArg;
	FArmyToolsModule::Get().GetRectSelectTool()->End();
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = true;
	MouseCaptureStatus = FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel = 0;
}
void FArmyIndexSymbolOperation::EndOperation()
{
	if (CurrentOperationPoint.IsValid())CurrentOperationPoint->SetState(FArmyPrimitive::OPS_Normal);
	CacheArg.ArgBoolean(false);
	CurrentOperationObject = NULL;
	CurrentOperationPoint = NULL;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel = MouseCaptureStatus;
	FArmyOperation::EndOperation();
}

void FArmyIndexSymbolOperation::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	
}
