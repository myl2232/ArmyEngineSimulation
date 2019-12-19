#pragma once
#include "ArmyEditObjectOperation.h"
#include "ArmyMouseCapture.h"
#include "ArmyToolsModule.h"
#include "ArmyGameInstance.h"

FArmyEditObjectOperation::FArmyEditObjectOperation(EModelType InBelongModel)
    : FArmyOperation(InBelongModel)
{
	
}
void FArmyEditObjectOperation::SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPrimitive)
{
	CurrentOperationObject = Object;
	CurrentOperationPoint = StaticCastSharedPtr<FArmyEditPoint>(InOperationPrimitive);
}
bool FArmyEditObjectOperation::InputKey(UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event)
{
	if (CacheArg._ArgBoolean)
	{
		if (InViewPortClient->IsOrtho())
		{
			if (Event == IE_Pressed)
			{
				if (Key == EKeys::LeftMouseButton)
				{
					if (CacheArg._ArgString == TEXT("NewObject"))
					{
						switch (CurrentOperationObject->GetType())
						{
						case OT_TextLabel:
						{
							TSharedPtr<FArmyTextLabel> LabelObj = StaticCastSharedPtr<FArmyTextLabel>(CurrentOperationObject);
							LabelObj->StartEditText();
						}
						default:
							break;
						}
						FArmySceneData::Get()->Add(CurrentOperationObject, XRArgument(1).ArgBoolean(true).ArgUint32(BelongModel));
					}
					else
					{
						SCOPE_TRANSACTION(TEXT("变换"));
						//need check huaen
						CurrentOperationObject->Modify();
					}
					EndOperation();
				}
			}
		}
	}
	return false;
}
void FArmyEditObjectOperation::MouseMove(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y)
{
	if (CacheArg._ArgBoolean)
	{
		if (CurrentOperationPoint.IsValid() && CurrentOperationObject.IsValid())
		{
			if (CurrentOperationPoint->OperationType == XROP_MOVE)
			{
				//need check huaen
				FVector EditOffSet = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0) - CurrentOperationPoint->GetPos();
				FTransform Trans;
				Trans.SetLocation(EditOffSet);
				CurrentOperationObject->ApplyTransform(Trans);
			}
			else if (CurrentOperationPoint->OperationType == XROP_SCALE)
			{
				FVector BasePos = CurrentOperationObject->GetBasePos();
				FVector BaseOffSet = CurrentOperationPoint->GetPos() - BasePos;
				//need check huaen
				FVector CurrentOffSet = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0) - BasePos;
				
				float TempScale = CurrentOffSet.Size() / BaseOffSet.Size();
				FTransform Trans;
				Trans.SetScale3D(FVector(TempScale, TempScale,1));
				CurrentOperationObject->ApplyTransform(Trans);
			}
			else if (CurrentOperationPoint->OperationType == XROP_ROTATE)
			{
				FVector BasePos = CurrentOperationObject->GetBasePos();
				FVector BaseOffSet = CurrentOperationPoint->GetPos() - BasePos;
				//need check huaen
				FVector CurrentOffSet = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0) - BasePos;

				FTransform Trans;
				Trans.SetRotation(FQuat::FindBetweenVectors(BaseOffSet, CurrentOffSet));
				CurrentOperationObject->ApplyTransform(Trans);
			}
		}
	}
}
void FArmyEditObjectOperation::BeginOperation(XRArgument InArg)
{
	CacheArg = InArg;
	FArmyToolsModule::Get().GetRectSelectTool()->End();
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = true;
	MouseCaptureStatus = FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel = 0;
}
void FArmyEditObjectOperation::EndOperation()
{
	CacheArg.ArgBoolean(false);
	CurrentOperationObject = NULL;
	CurrentOperationPoint = NULL;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel = MouseCaptureStatus;
	FArmyOperation::EndOperation();
}

void FArmyEditObjectOperation::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (CacheArg._ArgString == TEXT("NewObject"))
	{
		CurrentOperationObject->Draw(PDI, View);
	}
}
