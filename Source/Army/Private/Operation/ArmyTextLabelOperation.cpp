#include "ArmyTextLabelOperation.h"
#include "ArmyToolsModule.h"
#include "ArmyRectSelect.h"

FArmyTextLabelOperation::FArmyTextLabelOperation(EModelType InBelongModel)
	: FArmyOperation(InBelongModel)
{
}

FArmyTextLabelOperation::~FArmyTextLabelOperation()
{
}
void FArmyTextLabelOperation::BeginOperation(XRArgument InArg)
{
	FArmyOperation::BeginOperation(InArg);
	FArmyToolsModule::Get().GetRectSelectTool()->End();
}

void FArmyTextLabelOperation::EndOperation()
{
	SelectTextLabel = nullptr;
	FArmyOperation::EndOperation();
}

bool FArmyTextLabelOperation::InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event)
{
	if (Key == EKeys::LeftMouseButton && Event == IE_Released)
	{
		//SelectTextLabel->SetState(OS_Normal);
		EndOperation();
	}
	return false;
}

void FArmyTextLabelOperation::MouseDrag(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, FKey Key)
{
	if (SelectTextLabel.IsValid())
	{
		FObjectPtr RelateObj = nullptr;
		int32 RelateTag;
		SelectTextLabel->GetObjTag(RelateObj, RelateTag);
		FVector2D MousePos;
		GVC->GetMousePosition(MousePos);
		FVector WorldPos = GVC->PixelToWorld(MousePos.X, MousePos.Y, 0);
		WorldPos.Z = 0;
		if (RelateObj.IsValid() && RelateObj->GetType() == OT_InternalRoom)
		{
			TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(RelateObj);
			if (Room.IsValid() && Room->IsPointInRoom(WorldPos))
			{
				SelectTextLabel->SetWorldPosition(WorldPos);
			}
		}
		else
		{
			SelectTextLabel->SetWorldPosition(WorldPos);
		}
	}
}

void FArmyTextLabelOperation::SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPoint)
{
	if (Object.IsValid())
	{
		SelectTextLabel = StaticCastSharedPtr<FArmyTextLabel>(Object);
	}
}
