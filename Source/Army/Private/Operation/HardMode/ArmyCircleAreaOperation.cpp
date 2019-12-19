#include "ArmyCircleAreaOperation.h"
#include "ArmyCircleArea.h"

#include "ArmyWhirlwindStyle.h"
#include "ArmyIntervalStyle.h"
#include "ArmyHerringBoneStyle.h"
#include "ArmyCrossStyle.h"
#include "ArmyContinueStyle.h"
#include "SArmyInputBox.h"
#include "ArmyMouseCapture.h"
#include "ArmyMath.h"
#include "ArmyToolsModule.h"
#include "ArmyModalManager.h"
#include "ArmyRoomSpaceArea.h"
#include "ArmyGameInstance.h"
#include "ArmySeamlessStyle.h"
#include "ArmyShapeBoardActor.h"
#include "ArmyEditorEngine.h"
#include "ArmyWallActor.h"
FArmyCircleAreaOperation::FArmyCircleAreaOperation(EModelType InBelongModel) :
	FArmyOperation(InBelongModel)
{
	TempCircleArea = MakeShareable(new FArmyCircleArea());
	TempCircleArea->SetDrawOutLine(true);
	TempCircleArea->SetIsTempArea();
	if (TempCircleArea->GetWallActor())
	{
		TempCircleArea->GetWallActor()->SetActorHiddenInGame(true);
	}
	SetModifyOperating(false);
}
void FArmyCircleAreaOperation::InitWidget(TSharedPtr<SOverlay> InParentWidget)
{
	InParentWidget->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(InputBoxWidget, SArmyInputBox)
			.OnInputBoxCommitted(FOnTextCommitted::CreateRaw(this, &FArmyCircleAreaOperation::OnInputBoxCommitted))
		];
}

void FArmyCircleAreaOperation::SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPrimitive)
{
	if (Object.IsValid())
	{
		SelectedCircleArea = StaticCastSharedPtr<FArmyCircleArea>(Object);
		TempCircleArea->SetPosition(SelectedCircleArea->GetPosition());
		TempCircleArea->SetRadius(SelectedCircleArea->GetRadius());
		TempCircleArea->SetUniqueID(SelectedCircleArea->GetUniqueID());
		OperationLine->SetStart(SelectedCircleArea->GetPosition());
		OperationLine->SetEnd(SelectedCircleArea->GetPosition());
		if (CurrentParentArea.IsValid())
		{
			TempCircleArea->SetPlaneInfo(CurrentParentArea->GetXDir(), CurrentParentArea->GetYDir(), CurrentParentArea->GetPlaneCenter());
		}
	}
}

void FArmyCircleAreaOperation::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (CurrentState == 1)
	{
		TempCircleArea->Draw(PDI, View);
		FVector startPos = GetWorldFromLocal(TempCircleArea->GetPosition(), CurrentParentArea);
		FVector endPos = GetWorldFromLocal(TempCircleArea->GetPosition() + FVector(1, 0, 0)*TempCircleArea->GetRadius(), CurrentParentArea);
		PDI->DrawLine(startPos, endPos, FLinearColor(FColor(0XFF666666)), 1.0f);
	}
	else if (IsOperating())
	{
		TempCircleArea->Draw(PDI, View);
	}
}

bool FArmyCircleAreaOperation::InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event)
{
	if (Event == IE_Pressed)
	{
		if (Key == EKeys::LeftMouseButton)
		{
			if (!IsOperating())
			{

			}
			else if (ModifyOperationTime == 2)
			{
			}
			if (ModifyOperationTime == 1)
			{
				ModifyOperationTime++;
			}
		}
	}
	return true;
}

void FArmyCircleAreaOperation::ProcessClickInViewPortClient(FViewport* InViewPort, FKey Key, EInputEvent Event)
{
	if (!IsOperating())
	{
		FVector CurrentPoint = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0.0f);
		
		GetCurrentParentSurfaceByMouse();
		if (CurrentState == 0 && TempCircleArea.IsValid())
		{
			TempCircleArea->SetPosition(CurrentPoint);
			TempCircleArea->SetRadius(0);
			TempCircleArea->SetUniqueID(FGuid());
			CurrentState = 1;
		}
		else if (CurrentState == 1)
		{
			AddCircleArea();
			EndOperation();
		}
	}
	else
	{
		if (SelectedCircleArea.IsValid())
		{
			TSharedPtr<FArmyBaseArea> Parent = SelectedCircleArea->GetParentArea();
			if (Parent.IsValid())
			{
				SetCurrentParentSurface(Parent);
			}
		}

		if (!CurrentParentArea.IsValid())
		{
			SetCurrentParentSurface(CurrentAttachArea);
		}

		//SelectRectAreaBoundrary(GVC->GetCurrentMousePos(), SelectedRectArea);
	}
}

void FArmyCircleAreaOperation::StartTrackingDueToInput(const FArmyInputEventState& InPutState)
{
    ModifyOperationState = true;

	if (IsOperating())
	{

		if (SelectedCircleArea.IsValid())
		{
			TSharedPtr<FArmyBaseArea> Parent = SelectedCircleArea->GetParentArea();
			if (Parent.IsValid())
			{
				SetCurrentParentSurface(Parent);
			}
		}

		if (!CurrentParentArea.IsValid())
		{
			SetCurrentParentSurface(CurrentAttachArea);
		}

		FVector tempPos = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0.0f);
		tempPos = GetIntersectPointWithPlane(GVC->GetCurrentMousePos(), CurrentParentArea);
		LastMousePos = tempPos;
		CurrentMousePos = tempPos;
	}
}

void FArmyCircleAreaOperation::StopTracking()
{
	if (IsOperating())
	{
		/** @梁晓菲 判断灯槽是否存在，如果存在，需要用灯槽边界点确认是否可以添加在新位置*/
		TArray<FVector> tempOutVertices = TempCircleArea->GetOutArea()->Vertices;
        if (!SelectedCircleArea->GetLampSlotExtruder()->IsEmpty())
        {
			tempOutVertices = FArmyMath::ExturdePolygon(tempOutVertices, (SelectedCircleArea->GetLampSlotWidth() + 0.2f), false);
        }


		bool CanPlaceHere = false;
		if (CurrentParentArea.IsValid())
		{
			CanPlaceHere = CurrentParentArea->TestAreaCanAddOrNot(tempOutVertices, TempCircleArea->GetUniqueID());
		}
		if (CanPlaceHere)
		{
			//@郭子阳
			//移动不可以改变当前移动面的父面，移动的面必须在父面所有子面之外
			for (auto &BrotherArea : CurrentParentArea->GetEditAreas())
			{
				if (BrotherArea.Get() == SelectedCircleArea.Get())
				{
					continue;
				}
				if (FArmyMath::IsPolygonInPolygon(tempOutVertices, BrotherArea->GetOutArea()->Vertices)
					||FArmyMath::IsPolygonInPolygon(BrotherArea->GetOutArea()->Vertices, tempOutVertices))
	
				{
					CanPlaceHere = false;
					break;
				}
			}
		}


		if (CanPlaceHere)
		{
			SCOPE_TRANSACTION(TEXT("改变圆形区域的位置"));
            FVector Delta = TempCircleArea->GetPosition() - SelectedCircleArea->GetPosition();
            SelectedCircleArea->OffsetLayingPointPos(Delta);
			SelectedCircleArea->Move(Delta);// SetPosition(TempCircleArea->GetPosition());
			
			GXREditor->SelectNone(true, true, false);
		}
		else
		{
			TempCircleArea->SetPosition(SelectedCircleArea->GetPosition());
			WarnningInfo();
		}

		//EndOperation();
		ModifyOperationState = false;
	}
}

void FArmyCircleAreaOperation::BeginOperation(XRArgument InArg /*= XRArgument()*/)
{
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = true;

	CurrentState = 0;
	SethitTime(1);
}

void FArmyCircleAreaOperation::EndOperation()
{
	FArmyOperation::EndOperation();
	if (SelectedCircleArea.IsValid())
	{
		//SelectedCircleArea->SetState(OS_Normal);
		SelectedCircleArea = NULL;
	}
	CurrentState = 0;
	InputBoxWidget->Show(false);
	ModifyOperationState = false;
	ModifyOperationTime = 0;
	SetModifyOperating(false);
	SethitTime(1);

	
}

void FArmyCircleAreaOperation::Tick()
{
	if (CurrentState == 1)
	{
		FVector CurrentPoint = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0.0f);
		float radius = (CurrentPoint - TempCircleArea->GetPosition()).Size();
		{
			TempCircleArea->SetRadius(radius);
		}
		FVector endPosition = TempCircleArea->GetPosition() + FVector(1, 0, 0)*radius;
		bool bShowInputBox = InputBoxWidget->SetStartAndEnd(TempCircleArea->GetPosition(), endPosition);
		if (bShowInputBox)
		{
			FVector2D ViewportSize;
			GVC->GetViewportSize(ViewportSize);

			FVector startPos = GetWorldFromLocal(TempCircleArea->GetPosition(), CurrentParentArea);
			FVector endPos = GetWorldFromLocal(endPosition, CurrentParentArea);

			FVector LineCenter = (startPos + endPos) / 2;
			FVector2D InputBoxPos;
			GVC->WorldToPixel(LineCenter, InputBoxPos);
			InputBoxPos -= ViewportSize / 2;
			InputBoxWidget->SetPos(InputBoxPos);
			if (ShouldTick())
			{
				SetInputVisibility(true);
			}
		}
	}
	else if (IsOperating())
	{
		const bool leftMouseBotton = GVC->Viewport->KeyState(EKeys::LeftMouseButton);
		if (leftMouseBotton)
		{
			CurrentMousePos = GetIntersectPointWithPlane(GVC->GetCurrentMousePos(), CurrentParentArea);
			FVector delta = CurrentMousePos - LastMousePos;
			TempCircleArea->SetPosition(TempCircleArea->GetPosition() + delta);
			LastMousePos = CurrentMousePos;
		}
	}
}

void FArmyCircleAreaOperation::UpdateInputBoxInfo(XRArgument InArg, FObjectPtr Object, TSharedPtr<FArmyEditPoint> InOperationPoint, bool bModifyOperation, bool bVisibility, bool bMove)
{
	BeginOperation(InArg);
	SetModifyOperating(bModifyOperation);
	SetSelected(Object, InOperationPoint);
	ModifyOperationState = true;
	SetMoveOperating(bMove);
	if (bMove)
	{
		ModifyOperationTime = 1;
	}
	else
	{
		ModifyOperationTime = -1;
		CurrentState = -1;
		UpdateInputBoxPosition();
	}
}
void FArmyCircleAreaOperation::SetInputVisibility(bool visibility)
{
	InputBoxWidget->SetFocus(visibility);
	InputBoxWidget->Show(visibility);
}

void FArmyCircleAreaOperation::UpdateInputBoxPosition()
{
	FVector endPos = TempCircleArea->GetPosition() + FVector(1, 0, 0)*TempCircleArea->GetRadius();
	FVector2D ViewportSize;
	GVC->GetViewportSize(ViewportSize);
	FVector LineCenter = (TempCircleArea->GetPosition() + endPos) / 2;
	FVector2D InputBoxPos;
	FVector TempPos = GetWorldFromLocal(LineCenter, CurrentParentArea);
	GVC->WorldToPixel(TempPos, InputBoxPos);
	InputBoxPos -= ViewportSize / 2;
	InputBoxWidget->SetPos(InputBoxPos);
	InputBoxWidget->Show(true);
}

void FArmyCircleAreaOperation::SetCurrentEditSurface(TSharedPtr<class FArmyRoomSpaceArea> RoomSpaceArea)
{
	CurrentAttachArea = RoomSpaceArea;
	//if (TempCircleArea.IsValid())
	//{
	//	TempCircleArea->SetPlaneInfo(CurrentAttachArea->GetXDir(), CurrentAttachArea->GetYDir(), CurrentAttachArea->GetPlaneCenter());
	//	TempCircleArea->SurfaceType = CurrentAttachArea->SurfaceType;
	//}
	if (!CurrentParentArea.IsValid())
	{
		SetCurrentParentSurface(CurrentAttachArea);
	}
}



void FArmyCircleAreaOperation::SetCurrentParentSurface(TSharedPtr<class FArmyBaseArea> BaseArea)
{
	CurrentParentArea = BaseArea;
	if (TempCircleArea.IsValid())
	{
		TempCircleArea->SetPlaneInfo(CurrentParentArea->GetXDir(), CurrentParentArea->GetYDir(), CurrentParentArea->GetPlaneCenter());
		TempCircleArea->SurfaceType = CurrentParentArea->SurfaceType;
	}
}

void FArmyCircleAreaOperation::GetCurrentParentSurfaceByMouse()
{
	FVector LocalPositon = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0.0f);
	LocalPositon = GetIntersectPointWithPlane(GVC->GetCurrentMousePos(), CurrentAttachArea);

	auto selectedArea = CurrentAttachArea->FindArea(LocalPositon);

	if (selectedArea.IsValid())
	{
		SetCurrentParentSurface(selectedArea);
	}
	else
	{
		SetCurrentParentSurface(CurrentAttachArea);
	}
}

void FArmyCircleAreaOperation::WarnningInfo()
{
	GGI->Window->ShowMessage(MT_Warning, TEXT("与其他空间区域相交"));
}

void FArmyCircleAreaOperation::OnInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		if (!IsOperating() && !GetModifyOperating())
		{
			const int32 LineLength = FCString::Atoi(*InText.ToString());
			TempCircleArea->SetRadius(LineLength / 10.0f);
			AddCircleArea();
			GXREditor->SelectNone(true, true, false);
		}
		else
		{
			const int32 LineLength = FCString::Atoi(*InText.ToString());
			TempCircleArea->SetPosition(SelectedCircleArea->GetPosition());
			TempCircleArea->SetRadius(LineLength / 10.0f);

			/** @梁晓菲 判断灯槽是否存在，如果存在，需要用灯槽边界点确认是否可以添加在新位置*/
			TArray<FVector> tempOutVertices = TempCircleArea->GetOutArea()->Vertices;
            if (!SelectedCircleArea->GetLampSlotExtruder()->IsEmpty())
            {
				tempOutVertices = FArmyMath::ExturdePolygon(tempOutVertices, (SelectedCircleArea->GetLampSlotWidth() + 0.2f), false);
            }

			if (CurrentParentArea->TestAreaCanAddOrNot(tempOutVertices, TempCircleArea->GetUniqueID()))
			{
				SCOPE_TRANSACTION(TEXT("修改圆形区域大小"));
				SelectedCircleArea->SetRadius(LineLength / 10.0f);
				SelectedCircleArea->RecaculateRelatedArea();
				SelectedCircleArea->Modify();
				GXREditor->SelectNone(true, true, false);
			}
			else
			{
				const FText LineLengthText = FText::FromString(FString::Printf(TEXT("%.0f mm"), SelectedCircleArea->GetRadius() * 10));
				InputBoxWidget->SetInputText(LineLengthText);
				WarnningInfo();
			}
		}
		SetModifyOperating(false);
		SetInputVisibility(false);
		EndOperation();
	}
}

bool FArmyCircleAreaOperation::IsOperating()
{
	return ModifyOperationState && SelectedCircleArea.IsValid();
}

void FArmyCircleAreaOperation::AddCircleArea()
{
	if (!CurrentParentArea.IsValid())
		return;

	/* @梁晓菲 如果不可以添加，报错返回*/
	if (!CurrentParentArea->TestAreaCanAddOrNot(TempCircleArea->GetOutArea()->Vertices,TempCircleArea->GetUniqueID()))
	{
		WarnningInfo();
		return;
	}
	/* @梁晓菲 添加区域*/
	TSharedPtr<FArmyCircleArea> ResultArea = MakeShareable(new FArmyCircleArea());
	ResultArea->SurportPlaceArea = CurrentParentArea->SurportPlaceArea;
	ResultArea->SetPlaneInfo(CurrentParentArea->GetXDir(), CurrentParentArea->GetYDir(), CurrentParentArea->GetPlaneCenter());
	ResultArea->SetCenterAndRadius(TempCircleArea->GetPosition(), TempCircleArea->GetRadius());
    ResultArea->SurfaceType = CurrentParentArea->SurfaceType;
	TSharedPtr<FArmyBaseEditStyle> MatStyle;
	if (CurrentParentArea->SurfaceType == 0)
	{
		MatStyle = MakeShareable(new FArmySeamlessStyle());
		MatStyle->SetMainTextureInfo(FArmyEngineModule::Get().GetEngineResource()->GetDefaultTextureFloor(), 400, 400);
		FString Path = TEXT("/Game/XRCommon/HardMode/FloorTexture.FloorTexture");// @梁晓菲 保存路径反序列化
		//FString Path = TEXT("/Game/XRCommon/HardMode/WallTexture.WallTexture");
		MatStyle->SetMainTextureLocalPath(Path);
	}
	else if (CurrentParentArea->SurfaceType == 1)
	{
		MatStyle = MakeShareable(new FArmySeamlessStyle());
		MatStyle->SetMainTextureInfo(FArmyEngineModule::Get().GetEngineResource()->GetDefaultTextureWall(), 400, 400);
		FString Path = TEXT("/Game/XRCommon/HardMode/WallTexture.WallTexture");
		MatStyle->SetMainTextureLocalPath(Path);
	}
	else
	{
		MatStyle = MakeShareable(new FArmySeamlessStyle());
		MatStyle->SetMainTextureInfo(FArmyEngineModule::Get().GetEngineResource()->GetDefaultTextureCeiling(), 400, 400);
		FString Path = TEXT("/Game/XRCommon/HardMode/CeilingTexture.CeilingTexture");
		MatStyle->SetMainTextureLocalPath(Path);
	}

	MatStyle->SetPlaneInfo(ResultArea->GetPlaneCenter(), ResultArea->GetXDir(), ResultArea->GetYDir());

	FGuid GUID = CurrentParentArea->GetUniqueID();
	ResultArea->SetParentGuid(GUID);
	ResultArea->ConstructionItemData->SetConstructionItemCheckedId(CurrentParentArea->ConstructionItemData->CheckedId);
	ResultArea->ConstructionItemData->bHasSetted = true;

	ResultArea->SetStyle(MatStyle);

	XRArgument arg;
	arg._ArgUint32 = E_HardModel;
	ResultArea->SurfaceType = CurrentParentArea->SurfaceType;
	//if (CurrentParentArea->SurfaceType != 0)
		MatStyle->SetInternalDist(0.0f);
	SCOPE_TRANSACTION(TEXT("增加圆形区域"));

	//@郭子阳
	//让兄弟面叫爸爸
	for (auto& BrotherArea : CurrentParentArea->GetEditAreas())
	{
		if (ResultArea->TestAreaCanAddOrNot(BrotherArea->GetOutArea()->Vertices, ResultArea->GetUniqueID()))
		{
			BrotherArea->SetParentArea(ResultArea);
		}
	}

	FArmySceneData::Get()->Add(ResultArea, arg, CurrentParentArea);

}

