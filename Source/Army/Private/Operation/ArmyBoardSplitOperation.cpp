#include "ArmyBoardSplitOperation.h"
#include "ArmySceneData.h"
#include "ArmyBoardSplitLine.h"
#include "ArmyToolsModule.h"
#include "ArmyGameInstance.h"

TSharedPtr<FArmyBoardSplitline> FArmyBoardSplitOperation::Exec_AddBoardSplitLine(const FVector& Point)
{
	if (!CurrentBoardSplitline.IsValid())
	{
		CurrentBoardSplitline = MakeShareable(new FArmyBoardSplitline());
		CurrentBoardSplitline->SetBaseStart(Point);
		FArmyToolsModule::Get().GetMouseCaptureTool()->SetCurrentPoint(FVector2D(Point.X, Point.Y));
	}
	else
	{
		CurrentBoardSplitline->SetBaseEnd(Point);
		XRArgument Arg = XRArgument(1).ArgBoolean(true).ArgUint32(E_ConstructionModel).ArgString(CurrentBoardSplitline->GetName());
		FArmySceneData::Get()->Add(CurrentBoardSplitline, Arg);

		SCOPE_TRANSACTION(TEXT("添加台面分割线"));
		CurrentBoardSplitline->Create();
		CurrentBoardSplitline = NULL;
	}
    return CurrentBoardSplitline;
}
void FArmyBoardSplitOperation::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (CurrentBoardSplitline.IsValid())
	{
		CurrentBoardSplitline->Draw(PDI,View);
	}
}
void FArmyBoardSplitOperation::BeginOperation(XRArgument InArg)
{
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = true;
	FArmyToolsModule::Get().GetRectSelectTool()->End();
}
void FArmyBoardSplitOperation::EndOperation()
{
	CurrentBoardSplitline = NULL;
	FArmyOperation::EndOperation();
}
void FArmyBoardSplitOperation::MouseMove(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y)
{
	if (CurrentBoardSplitline.IsValid())
	{
		CurrentBoardSplitline->SetBaseEnd(FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0));
	}
}
bool FArmyBoardSplitOperation::InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event)
{
    if (InViewPort->KeyState(EKeys::LeftMouseButton) && Event == IE_Pressed)
    {
		Exec_AddBoardSplitLine(FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0));
    }
	else if (InViewPort->KeyState(EKeys::RightMouseButton) && Event == IE_Pressed)
	{
		EndOperation();
	}
    return false;
}