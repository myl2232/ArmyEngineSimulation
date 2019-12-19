#include "ArmyRegularPolygonAreaOperation.h"
#include "ArmyRegularPolygonArea.h"
#include "ArmyMouseCapture.h"
#include "ArmyHerringBoneStyle.h"
#include "ArmyWhirlwindStyle.h"
#include "SArmyInputBox.h"
#include "SBoxPanel.h"
#include "ArmyModalManager.h"
#include "ArmyRoomSpaceArea.h"
#include "ArmyToolsModule.h"
#include "ArmyGameInstance.h"
#include "ArmyShapeBoardActor.h"
#include "ArmyContinueStyle.h"
#include "ArmySeamlessStyle.h"
#include "ArmyEditorEngine.h"
FArmyRegularPolygonAreaOperation::FArmyRegularPolygonAreaOperation(EModelType InBelongModel) :
	FArmyOperation(InBelongModel),
	TempRegularArea(MakeShareable(new FArmyRegularPolygonArea())),
	DashLine(MakeShareable(new FArmyLine()))
{
	TempRegularArea->SetTempArea();
	TempRegularArea->SetDrawOutLine(true);
	TempRegularArea->SetNumSide(6);
	DashLine->SetBaseColor(FColor::Red);
	SetModifyOperating(false);

	if (TempRegularArea->GetWallActor())
	{
		TempRegularArea->GetWallActor()->SetActorHiddenInGame(true);
	}
}

void FArmyRegularPolygonAreaOperation::Init()
{

}

void FArmyRegularPolygonAreaOperation::InitWidget(TSharedPtr<SOverlay> InParentWidget)
{
	InParentWidget->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(InPutBox, SHorizontalBox)
			+ SHorizontalBox::Slot()
		.HAlign(HAlign_Right)
		.Padding(2, 2, 2, 2)
		.AutoWidth()
		[
			SNew(SBorder)
			.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.BorderBackgroundColor(FSlateColor(FLinearColor(0, 0, 0)))
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("请输入边数")))
		.Font(FSlateFontInfo("Fonts/Roboto-Regular", 12))
		]
		]

	+ SHorizontalBox::Slot()
		.HAlign(HAlign_Center)
		.Padding(2, 2, 2, 2)
		[
			SAssignNew(InputBoxWidget1, SArmyInputBox)
			.OnInputBoxCommitted(FOnTextCommitted::CreateRaw(this, &FArmyRegularPolygonAreaOperation::OnInputBoxCommitted1))
		.OnKeyDown(FInputKeyDelegate::CreateRaw(this, &FArmyRegularPolygonAreaOperation::OnInputBoxKeyDown))
		]
		];

	InPutBox->SetVisibility(EVisibility::Collapsed);

	InParentWidget->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(InputBoxWidget, SArmyInputBox)
			.OnInputBoxCommitted(FOnTextCommitted::CreateRaw(this, &FArmyRegularPolygonAreaOperation::OnInputBoxCommitted))
		.OnKeyDown(FInputKeyDelegate::CreateRaw(this, &FArmyRegularPolygonAreaOperation::OnInputBoxKeyDown))
		];
	InputBoxWidget1->SetInputText(FText::FromString("6"));
}

void FArmyRegularPolygonAreaOperation::SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPrimitive)
{
	if (Object.IsValid())
	{
		SelectedRegularPolyArea = StaticCastSharedPtr<FArmyRegularPolygonArea>(Object);
		TempRegularArea->SetPosition(SelectedRegularPolyArea->GetPosition());
		TempRegularArea->SetStartPoint(SelectedRegularPolyArea->GetStartPoint());
		TempRegularArea->SetRadius(SelectedRegularPolyArea->GetRadius());
		TempRegularArea->SetNumSide(SelectedRegularPolyArea->GetNumSide());
		TempRegularArea->SetUniqueID(SelectedRegularPolyArea->GetUniqueID());
		OperationLine->SetStart(SelectedRegularPolyArea->GetPosition());
		OperationLine->SetEnd(SelectedRegularPolyArea->GetPosition());
		if (CurrentParentArea.IsValid())
		{
			TempRegularArea->SetPlaneInfo(CurrentParentArea->GetXDir(), CurrentParentArea->GetYDir(), CurrentParentArea->GetPlaneCenter());
		}
	}
}

void FArmyRegularPolygonAreaOperation::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (CurrentState == 2)
	{
		FVector Pos = GetWorldFromLocal(TempRegularArea->GetPosition(), CurrentParentArea);
		FVector StartPos = GetWorldFromLocal(TempRegularArea->GetStartPoint(), CurrentParentArea);
		PDI->DrawLine(Pos, StartPos, FLinearColor(FColor(0XFF666666)), 1.0f);
		TempRegularArea->Draw(PDI, View);
	}
	if (IsOperating())
	{

		TempRegularArea->Draw(PDI, View);

	}
}


void FArmyRegularPolygonAreaOperation::ProcessClickInViewPortClient(FViewport* InViewPort, FKey Key, EInputEvent Event)
{
	if (!IsOperating())
	{
		FVector CurrentPoint = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0.0f);
		GetCurrentParentSurfaceByMouse();
		if (CurrentState == 1)
		{
			DashLine->SetStart(CurrentPoint);
			DashLine->SetEnd(CurrentPoint);
			TempRegularArea->SetStartPointAndCenterPoint(CurrentPoint, CurrentPoint);
			CurrentState = 2;
		}
		else if (CurrentState == 2)
		{
			float radius = (CurrentPoint - TempRegularArea->GetPosition()).Size();
			TempRegularArea->SetRadius(radius);
			AddRegularPolygonArea();
			EndOperation();

		}
	}
	else
	{
		if (SelectedRegularPolyArea.IsValid())
		{
			TSharedPtr<FArmyBaseArea> Parent = SelectedRegularPolyArea->GetParentArea();
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

void FArmyRegularPolygonAreaOperation::StartTrackingDueToInput(const FArmyInputEventState& InPutState)
{
    ModifyOperationState = true;

	if (IsOperating()&& CurrentParentArea.IsValid())
	{
		if (SelectedRegularPolyArea.IsValid())
		{
			TempRegularArea->SetRadius(SelectedRegularPolyArea->GetRadius());
			TempRegularArea->SetNumSide(SelectedRegularPolyArea->GetNumSide());
			TempRegularArea->SetPosition(SelectedRegularPolyArea->GetPosition());
			//复制旋转角度
			TempRegularArea->SetStartPoint(SelectedRegularPolyArea->GetStartPoint());

			TSharedPtr<FArmyBaseArea> Parent = SelectedRegularPolyArea->GetParentArea();
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

void FArmyRegularPolygonAreaOperation::StopTracking()
{
	/** @梁晓菲 判断灯槽是否存在，如果存在，需要用灯槽边界点确认是否可以添加在新位置*/
	TArray<FVector> tempOutVertices = TempRegularArea->GetOutArea()->Vertices;
	if (SelectedRegularPolyArea.IsValid())
	{
        if (!SelectedRegularPolyArea->GetLampSlotExtruder()->IsEmpty())
        {
			tempOutVertices = FArmyMath::ExturdePolygon(tempOutVertices, (SelectedRegularPolyArea->GetLampSlotWidth() + 0.2f), false);
        }
	}


	bool CanPlaceHere=false;
	if (CurrentParentArea.IsValid())
	{
		CanPlaceHere = CurrentParentArea->TestAreaCanAddOrNot(tempOutVertices, TempRegularArea->GetUniqueID());
	}
	
	
	if (CanPlaceHere)
	{
		//@郭子阳
		//移动不可以改变当前移动面的父面，移动的面必须在父面所有子面之外
		for (auto &BrotherArea : CurrentParentArea->GetEditAreas())
		{
			if (BrotherArea.Get() == SelectedRegularPolyArea.Get())
			{
				continue;
			}
			if (
				!(!FArmyMath::IsPolygonInPolygon(tempOutVertices, BrotherArea->GetOutArea()->Vertices)
					&& !FArmyMath::IsPolygonInPolygon(BrotherArea->GetOutArea()->Vertices, tempOutVertices))
				)
			{
				CanPlaceHere = false;
				break;
			}
		}
	}

	if (CanPlaceHere)
	{
		if (SelectedRegularPolyArea.IsValid())
		{
			SCOPE_TRANSACTION(TEXT("移动多边形位置"));
            FVector Delta = TempRegularArea->GetPosition() - SelectedRegularPolyArea->GetPosition();
            SelectedRegularPolyArea->OffsetLayingPointPos(Delta);
			SelectedRegularPolyArea->Move(Delta);
			
			GXREditor->SelectNone(true, true, false);
		}
	}
	else if(CurrentState!=0 && CurrentState != 2)
	{
		if (SelectedRegularPolyArea.IsValid())
		{
			TempRegularArea->SetPosition(SelectedRegularPolyArea->GetPosition());
		}
		WarnningInfo();
	}
	//EndOperation();
	ModifyOperationState = false;
}

void FArmyRegularPolygonAreaOperation::BeginOperation(XRArgument InArg /*= XRArgument()*/)
{


	InputBoxWidget->Show(false);
	InputBoxWidget1->Show(false);
	InPutBox->SetVisibility(EVisibility::Collapsed);
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = true;
	CurrentState = 0;
	SethitTime(1);
}

void FArmyRegularPolygonAreaOperation::EndOperation()
{
	FArmyOperation::EndOperation();
	if (SelectedRegularPolyArea.IsValid())
	{
		//SelectedRegularPolyArea->SetState(OS_Normal);
		SelectedRegularPolyArea = NULL;
	}

	InPutBox->SetVisibility(EVisibility::Hidden);
	SetInputVisibility(false);
	InputBoxWidget1->Show(false);
	CurrentState = -1;
	ModifyOperationState = false;
	ModifyOperationTime = 0;
	SetModifyOperating(false);
	SethitTime(1);

	//@郭子阳
	//隐藏临时面
	TempRegularArea->SetRadius(0);

}

void FArmyRegularPolygonAreaOperation::Tick()
{
	if (CurrentState == 0 && !IsOperating())
	{
		FVector CurrentPoint = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0.0f);
		FVector2D ViewportSize;
		GVC->GetViewportSize(ViewportSize);
		FVector2D InputBoxPos;
		InputBoxWidget1->SetFocus(true);

		FVector TempStartPos = GetWorldFromLocal(CurrentPoint, CurrentParentArea);

		GVC->WorldToPixel(TempStartPos, InputBoxPos);
		InputBoxPos -= ViewportSize / 2;
		InPutBox->SetVisibility(EVisibility::HitTestInvisible);
		InputBoxWidget1->Show(true);
		InPutBox->SetRenderTransform(FSlateRenderTransform(InputBoxPos));
	}
	if (CurrentState == 2)
	{
		if (ShouldTick())
		{
			SetInputVisibility(true);
		}
		FVector CurrentPoint = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0.0f);
		if (UseOrthoMode)
		{
			CurrentPoint.X = TempRegularArea->GetPosition().X;
		}
		float radius = (CurrentPoint - TempRegularArea->GetPosition()).Size();
		TempRegularArea->SetStartPoint(CurrentPoint + FVector(0, 0, 1));
		TempRegularArea->SetRadius(radius);
		DashLine->SetEnd(TempRegularArea->GetStartPoint());
		UpdateInputboxPosition();

	}
	if (IsOperating())
	{
		const bool leftMouseBotton = GVC->Viewport->KeyState(EKeys::LeftMouseButton);
		if (leftMouseBotton)
		{
			CurrentMousePos = GetIntersectPointWithPlane(GVC->GetCurrentMousePos(), CurrentParentArea);
			FVector delta = CurrentMousePos - LastMousePos;
			TempRegularArea->SetStartPointAndCenterPoint(TempRegularArea->GetStartPoint() + delta, TempRegularArea->GetPosition() + delta);
			LastMousePos = CurrentMousePos;
		}
	}
}

void FArmyRegularPolygonAreaOperation::UpdateInputBoxInfo(XRArgument InArg, FObjectPtr Object, TSharedPtr<FArmyEditPoint> InOperationPoint, bool bModifyOperation, bool bVisibility, bool bMove)
{
	BeginOperation(InArg);
	ModifyOperationState = true;
	SetModifyOperating(bModifyOperation);
	SetSelected(Object, InOperationPoint);
	SetMoveOperating(bMove);
	if (bMove)
	{
		ModifyOperationTime = 1;
	}
	else
	{
		ModifyOperationTime = -1;
		CurrentState = -1;
		SetInputVisibility(bVisibility);
		UpdateInputboxPosition();
	}
}

void FArmyRegularPolygonAreaOperation::SetInputVisibility(bool visibility)
{
	InputBoxWidget->Show(visibility);
	InputBoxWidget->SetFocus(visibility);
}

void FArmyRegularPolygonAreaOperation::SetCurrentEditSurface(TSharedPtr<class FArmyRoomSpaceArea> RoomSpaceArea)
{
	CurrentAttachArea = RoomSpaceArea;
	//if (TempRegularArea.IsValid())
	//{
	//	TempRegularArea->SetPlaneInfo(CurrentParentArea->GetXDir(), CurrentParentArea->GetYDir(), CurrentParentArea->GetPlaneCenter());
	//	TempRegularArea->SurfaceType = RoomSpaceArea->SurfaceType;
	//}
}



void FArmyRegularPolygonAreaOperation::SetCurrentParentSurface(TSharedPtr<class FArmyBaseArea> BaseArea)
{
	CurrentParentArea = BaseArea;
	if (TempRegularArea.IsValid())
	{
		TempRegularArea->SetPlaneInfo(CurrentParentArea->GetXDir(), CurrentParentArea->GetYDir(), CurrentParentArea->GetPlaneCenter());
		TempRegularArea->SurfaceType = CurrentParentArea->SurfaceType;
	}
}

void FArmyRegularPolygonAreaOperation::GetCurrentParentSurfaceByMouse()
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

void FArmyRegularPolygonAreaOperation::WarnningInfo()
{
	InputBoxWidget->SetFocus(false);
	GGI->Window->ShowMessage(MT_Warning, TEXT("与其他空间区域相交"));

}

void FArmyRegularPolygonAreaOperation::OnInputBoxKeyDown(const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.IsLeftShiftDown())
	{
		if (!UseOrthoMode)
			UseOrthoMode = true;
		else
			UseOrthoMode = false;
	}

}

void FArmyRegularPolygonAreaOperation::OnInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		const int32 LineLength = FCString::Atoi(*InText.ToString());
		FVector direction = TempRegularArea->GetStartPoint() - TempRegularArea->GetPosition();
		FVector tempstart = TempRegularArea->GetPosition() + direction.GetSafeNormal() * LineLength / 10.0f;
		TempRegularArea->SetRadius(LineLength / 10.0f);
		if (!IsOperating() && GetModifyOperating() != true)
		{
			FVector CurrentPoint = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0.0f);
			FVector vecNor = CurrentPoint - TempRegularArea->GetPosition();
			FVector RealCurrentPoint = TempRegularArea->GetPosition() + vecNor.GetSafeNormal()*(LineLength / 10.0f);
			TempRegularArea->SetStartPoint(RealCurrentPoint);
			TempRegularArea->SetRadius(LineLength / 10.0f);
			AddRegularPolygonArea();
			EndOperation();
			GXREditor->SelectNone(true, true, false);
		}
		else
		{
			TempRegularArea->SetPosition(SelectedRegularPolyArea->GetPosition());
			TempRegularArea->SetStartPoint(tempstart);

			/** @梁晓菲 判断灯槽是否存在，如果存在，需要用灯槽边界点确认是否可以添加在新位置*/
			TArray<FVector> tempOutVertices = TempRegularArea->GetOutArea()->Vertices;
            if (!SelectedRegularPolyArea->GetLampSlotExtruder()->IsEmpty())
            {
				tempOutVertices = FArmyMath::ExturdePolygon(tempOutVertices, (SelectedRegularPolyArea->GetLampSlotWidth() + 0.2f), false);
            }

			if (CurrentParentArea->TestAreaCanAddOrNot(tempOutVertices, TempRegularArea->GetUniqueID()))
			{
				ModifyArea();
				GXREditor->SelectNone(true, true, false);
			}
			else
			{
				WarnningInfo();
			}
		}
		SetModifyOperating(false);
		SetInputVisibility(false);
		EndOperation();
	}
}

void FArmyRegularPolygonAreaOperation::OnInputBoxCommitted1(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		const int32 LineLength = FCString::Atoi(*InText.ToString());
		if (LineLength < 3 || LineLength >12)
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("输入边数不得小于3且不得大于12"));
			return;
		}
		InPutBox->SetVisibility(EVisibility::Collapsed);
		InputBoxWidget1->Show(false);
		if (TempRegularArea.IsValid())
		{
			TempRegularArea->SetNumSide(LineLength);
		}
		CurrentState = 1;
	}

}

void FArmyRegularPolygonAreaOperation::UpdateInputboxPosition()
{
	bool bShowInputBox = InputBoxWidget->SetStartAndEnd(TempRegularArea->GetPosition(), TempRegularArea->GetStartPoint());
	if (bShowInputBox)
	{
		FVector2D ViewportSize;
		GVC->GetViewportSize(ViewportSize);
		FVector LineCenter = (TempRegularArea->GetStartPoint() + TempRegularArea->GetPosition()) / 2;
		FVector2D InputBoxPos;

		FVector TempStartPos = GetWorldFromLocal(LineCenter, CurrentParentArea);
		GVC->WorldToPixel(TempStartPos, InputBoxPos);
		InputBoxPos -= ViewportSize / 2;
		InputBoxWidget->SetPos(InputBoxPos);
	}
}

bool FArmyRegularPolygonAreaOperation::IsOperating()
{
	return ModifyOperationState && SelectedRegularPolyArea.IsValid();
}

void FArmyRegularPolygonAreaOperation::AddRegularPolygonArea()
{
	if (!CurrentParentArea.IsValid())
		return;

	/* @梁晓菲 如果不可以添加，报错返回*/
	if (!CurrentParentArea->TestAreaCanAddOrNot(TempRegularArea->GetOutArea()->Vertices, TempRegularArea->GetUniqueID()))
	{
		WarnningInfo();
		return;
	}
	/* @梁晓菲 添加区域*/
	TSharedPtr<FArmyRegularPolygonArea> ResultArea = MakeShareable(new FArmyRegularPolygonArea());
	//@郭子阳
	//先设置区域，否则影响actor的名字
	ResultArea->SurportPlaceArea = CurrentParentArea->SurportPlaceArea;
	ResultArea->SetPlaneInfo(CurrentParentArea->GetXDir(), CurrentParentArea->GetYDir(), CurrentParentArea->GetPlaneCenter());
	ResultArea->SetNumSide(TempRegularArea->GetNumSide());
	ResultArea->SetPosition(TempRegularArea->GetPosition());
	ResultArea->SetStartPoint(TempRegularArea->GetStartPoint());
	ResultArea->SetRadius(TempRegularArea->GetRadius());

	TSharedPtr<FArmyBaseEditStyle> MatStyle;
	if (CurrentParentArea->SurfaceType == 0)
	{
		MatStyle = MakeShareable(new FArmySeamlessStyle());
		MatStyle->SetMainTextureInfo(FArmyEngineModule::Get().GetEngineResource()->GetDefaultTextureFloor(), 400, 400);
		FString Path = TEXT("/Game/XRCommon/HardMode/FloorTexture.FloorTexture");
		MatStyle->SetMainTextureLocalPath(Path);
	}
	else if (CurrentParentArea->SurfaceType == 1)
	{
		MatStyle = MakeShareable(new FArmySeamlessStyle());
		MatStyle->SetMainTextureInfo(FArmyEngineModule::Get().GetEngineResource()->GetDefaultTextureWall(), 400, 400);
		FString Path = TEXT("/Game/XRCommon/HardMode/WallTexture.WallTexture");
		MatStyle->SetMainTextureLocalPath(Path);
	}
	else //if (CurrentParentArea->SurfaceType == 2)
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
//	if (CurrentParentArea->SurfaceType != 0)
		MatStyle->SetInternalDist(0.0f);
	SCOPE_TRANSACTION(TEXT("增加多边形"));
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
	//@郭子阳
	//隐藏临时面
	TempRegularArea->SetRadius(0);
}

void FArmyRegularPolygonAreaOperation::ModifyArea()
{
	SCOPE_TRANSACTION(TEXT("修改多边形大小"));
	SelectedRegularPolyArea->SetRadius(TempRegularArea->GetRadius());
	SelectedRegularPolyArea->SetStartPoint(TempRegularArea->GetStartPoint());
	SelectedRegularPolyArea->RecaculateRelatedArea();
	SelectedRegularPolyArea->Modify();
	EndOperation();
}

