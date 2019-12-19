#include "ArmyBaseRectOperation.h"
#include "ArmyMouseCapture.h"
#include "ArmyToolsModule.h"

FArmyBaseRectOperation::FArmyBaseRectOperation(EModelType InBelongModel):FArmyOperation(InBelongModel)
{

}

void FArmyBaseRectOperation::InitWidget(TSharedPtr<SOverlay> InParentWidget)
{
	InParentWidget->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(WidthInputBoxWidget, SArmyInputBox)
			.OnInputBoxCommitted(FOnTextCommitted::CreateRaw(this, &FArmyBaseRectOperation::OnWidthInputBoxCommitted))
			.OnKeyDown(FInputKeyDelegate::CreateRaw(this, &FArmyBaseRectOperation::OnInputBoxKeyDown))
		];
	InParentWidget->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(HeightInputBoxWidget, SArmyInputBox)
			.OnInputBoxCommitted(FOnTextCommitted::CreateRaw(this, &FArmyBaseRectOperation::OnHeightInputBoxCommitted))
			.OnKeyDown(FInputKeyDelegate::CreateRaw(this, &FArmyBaseRectOperation::OnInputBoxKeyDown))
		];
}

void FArmyBaseRectOperation::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (CurrentState == 1)
	{
		RectArea->Draw(PDI, View);
	}
}

void FArmyBaseRectOperation::BeginOperation(XRArgument InArg /*= XRArgument()*/)
{
	RectArea = MakeShareable(new FArmyRect);
	WidthInputBoxWidget->Show(false);
	HeightInputBoxWidget->Show(false);
	CurrentState = 0;
}

void FArmyBaseRectOperation::EndOperation()
{
	RectArea = NULL;
	WidthInputBoxWidget->Show(false);
	HeightInputBoxWidget->Show(false);
	CurrentState = -1;
}

void FArmyBaseRectOperation::Tick()
{
	if (CurrentState == 1)
	{
		//need check huaen
		FVector CurrentPos = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0);
		FVector offset = CurrentPos - RectArea->Pos;
		RectArea->Width = offset.X;
		RectArea->Height = offset.Y;
		UpateInputBoxPosition();
	}
}

bool FArmyBaseRectOperation::InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event)
{
	if (InViewPort->KeyState(EKeys::LeftMouseButton))
	{
		if (CurrentState == 0)
		{
			//need check huaen
			RectArea->Pos = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0);
			RectArea->Width = 0.0f;
			RectArea->Height = 0.0f;
			CurrentState = 1;
		}
		else if (CurrentState == 1)
		{
			//need check huaen
			//LocalManager.Pin()->EndOperation();
		}

	}
	return false;
}

void FArmyBaseRectOperation::UpateInputBoxPosition()
{
	FVector start, end;
	start = RectArea->Pos;
	end = RectArea->Pos + FVector(0, 1, 0)*RectArea->Height;
	UpdateBoxWidget(HeightInputBoxWidget, start, end);

	start = RectArea->Pos;
	end = RectArea->Pos + FVector(1, 0, 0) * RectArea->Width;
	UpdateBoxWidget(WidthInputBoxWidget, start, end);
}

void FArmyBaseRectOperation::UpdateBoxWidget(TSharedPtr<SArmyInputBox> WidthInputBoxWidget, FVector start, FVector end)
{
	bool bShowEditInputBox = WidthInputBoxWidget->SetStartAndEnd(start, end);
	if (bShowEditInputBox)
	{
		WidthInputBoxWidget->SetFocus(true);

		// 设置输入框位置
		FVector2D ViewportSize;
		//need check huaen
		GVC->GetViewportSize(ViewportSize);
		FVector LineCenter = (start + end) / 2;
		FVector2D InputBoxPos;
		GVC->WorldToPixel(LineCenter, InputBoxPos);
		InputBoxPos -= ViewportSize / 2;
		WidthInputBoxWidget->SetPos(InputBoxPos);
	}
	else
	{
		WidthInputBoxWidget->SetFocus(false);
	}
}
void FArmyBaseRectOperation::OnInputBoxKeyDown(const FKeyEvent& InKeyEvent)
{
	//need check huaen
	//LocalManager.Pin()->OnInputBoxKeyDown(InKeyEvent);
}

void FArmyBaseRectOperation::OnWidthInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		const int32 LineLength = FCString::Atoi(*InText.ToString());
		//need check huaen
		FVector CurrentPos = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0);
		FVector offset = CurrentPos - RectArea->Pos;
		RectArea->Width = offset.X >= 0 ? (LineLength / 10.0f) : (-LineLength / 10.0f);
		UpateInputBoxPosition();
		WidthInputBoxWidget->SetFocus(false);
		HeightInputBoxWidget->SetFocus(true);
	}
}

void FArmyBaseRectOperation::OnHeightInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		const int32 LineLength = FCString::Atoi(*InText.ToString());
		//need check huaen
		FVector CurrentPos = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0);
		FVector offset = CurrentPos - RectArea->Pos;
		RectArea->Height = offset.Y >= 0 ? (LineLength / 10.0f) : (-LineLength / 10.0f);
		//need check huaen
		//LocalManager.Pin()->EndOperation();
	}
}
