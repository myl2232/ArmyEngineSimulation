#include "ArmyRectAreaOperation.h"

#include "ArmyCore/Public/Math/XRMath.h"
#include "ArmyRectArea.h"
#include "ArmyWorkerStyle.h"
#include "ArmyCrossStyle.h"
#include "ArmyWhirlwindStyle.h"
#include "ArmyTrapezoidStyle.h"
#include "ArmyRoomSpaceArea.h"
#include "ArmyMouseCapture.h"
#include "SArmyInputBox.h"
#include "ArmyModalManager.h"
#include "ArmyRoomSpaceArea.h"
#include "ArmyRect.h"
#include "SceneManagement.h"
#include "ArmyShapeBoardActor.h"
#include "ArmyToolsModule.h"
#include "ArmyGameInstance.h"
#include "ArmyContinueStyle.h"
#include "ArmySeamlessStyle.h"
#include "ArmyEditorEngine.h"
#include "ArmyLayingPoint.h"
#include "ArmyHardModeController.h"
#include "ArmyWallActor.h"
#define  RulerOffset 12.0f

FArmyRectAreaOperation::FArmyRectAreaOperation(EModelType InBelongModel) :
	FArmyOperation(InBelongModel),
	TempRectArea(MakeShareable(new FArmyRectArea())),
	RectArea(MakeShareable(new FArmyRect())),
	OperationPoint(NULL)
{

	TempRectArea->SetDrawOutLine(true);

	if (TempRectArea->GetWallActor())
	{
		TempRectArea->GetWallActor()->SetActorHiddenInGame(true);
	}
	SetModifyOperating(false);
}

void FArmyRectAreaOperation::Init()
{

}

void FArmyRectAreaOperation::InitWidget(TSharedPtr<SOverlay> InParentWidget)
{
	InParentWidget->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(UpWidthInputBoxWidget, SArmyInputBox)
			.OnInputBoxCommitted(FOnTextCommitted::CreateRaw(this, &FArmyRectAreaOperation::OnUpWidthInputBoxCommitted))
		.OnKeyDown(FInputKeyDelegate::CreateRaw(this, &FArmyRectAreaOperation::OnInputBoxKeyDown))
		];
	InParentWidget->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(DownWidthInputBoxWidget, SArmyInputBox)
			.OnInputBoxCommitted(FOnTextCommitted::CreateRaw(this, &FArmyRectAreaOperation::OnUpWidthInputBoxCommitted))
		.OnKeyDown(FInputKeyDelegate::CreateRaw(this, &FArmyRectAreaOperation::OnInputBoxKeyDown))
		];
	InParentWidget->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(LeftHeightInputBoxWidget, SArmyInputBox)
			.OnInputBoxCommitted(FOnTextCommitted::CreateRaw(this, &FArmyRectAreaOperation::OnLeftHeightInputBoxCommitted))
		.OnKeyDown(FInputKeyDelegate::CreateRaw(this, &FArmyRectAreaOperation::OnInputBoxKeyDown))
		];
	InParentWidget->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(RightHeightInputBoxWidget, SArmyInputBox)
			.OnInputBoxCommitted(FOnTextCommitted::CreateRaw(this, &FArmyRectAreaOperation::OnLeftHeightInputBoxCommitted))
		.OnKeyDown(FInputKeyDelegate::CreateRaw(this, &FArmyRectAreaOperation::OnInputBoxKeyDown))
		];
}

void FArmyRectAreaOperation::SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPrimitive)
{
	if (Object.IsValid())
	{
		SelectedRectArea = StaticCastSharedPtr<FArmyRectArea>(Object);
		SelectedRectArea->BoundaryDriveEvent.BindRaw(this, &FArmyRectAreaOperation::RefreshBoundaryDriveEvent);
		OperationLine->SetStart(SelectedRectArea->GetCenterPosition());
		OperationLine->SetEnd(SelectedRectArea->GetCenterPosition());
		TempRectArea->SetRectWidth(SelectedRectArea->GetRectWidth());
		TempRectArea->SetRectHeight(SelectedRectArea->GetRectHeight());
		TempRectArea->SetLeftUpCornerPos(SelectedRectArea->GetLeftUpCornerPos());
		TempRectArea->SetUniqueID(SelectedRectArea->GetUniqueID());
		TempRectArea->GetWallActor()->SetActorHiddenInGame(true);

		RectArea->Pos = SelectedRectArea->GetLeftUpCornerPos();
		RectArea->Width = SelectedRectArea->GetRectWidth();
		RectArea->Height = SelectedRectArea->GetRectHeight();
		if (CurrentParentArea.IsValid())
		{
			TempRectArea->SetPlaneInfo(CurrentParentArea->GetXDir(), CurrentParentArea->GetYDir(), CurrentParentArea->GetPlaneCenter());
			TempRectArea->SurfaceType = CurrentParentArea->SurfaceType;
		}
	}
}

void FArmyRectAreaOperation::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (CurrentState == 1)
	{
		TArray<FVector> Vertices = RectArea->GetVertices();

		int number = Vertices.Num();
		for (int i = 0; i < number; ++i)
		{
			FVector startPos = GetWorldFromLocal(Vertices[i%number], CurrentParentArea);
			FVector endPos = GetWorldFromLocal(Vertices[(i + 1) % number], CurrentParentArea);
			PDI->DrawLine(startPos, endPos, FLinearColor(FColor(0XFF666666)), 1.0f);
		}
	}
	else if (IsOperating())
	{
		TempRectArea->Draw(PDI, View);
		DrawHelpRulerAxis(PDI, View, FLinearColor(FColor(0XFFCCCCCC)), 0.0f);
	}
	if (DrawBoundary)
	{
		//绘制被选中的边 @郭子阳注
		PDI->DrawLine(HelpDrawLineStart, HelpDrawLineEnd, FLinearColor(FColor(0XFFFF9800)), 1, 3.0f, 0.0, true);
	}
}

bool FArmyRectAreaOperation::InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event)
{

	return true;
}

void FArmyRectAreaOperation::ProcessClickInViewPortClient(FViewport* InViewPort, FKey Key, EInputEvent Event)
{
	if (!IsOperating())
	{
		if (CurrentState == 0 && TempRectArea.IsValid())
		{

			GetCurrentParentSurfaceByMouse();

			RectArea->Pos = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0.0f);
			RectArea->Width = 0.0f;
			RectArea->Height = 0.0f;
			CurrentState = 1;


		}
		else if (CurrentState == 1)
		{
			AddRectArea();
			EndOperation();
		}

	}
	else
	{
		if (SelectedRectArea.IsValid())
		{
			TSharedPtr<FArmyBaseArea> Parent = SelectedRectArea->GetParentArea();
			if (Parent.IsValid())
			{
				SetCurrentParentSurface(Parent);
			}
		}

		if (!CurrentParentArea.IsValid())
		{
			SetCurrentParentSurface(CurrentAttachArea);
		}

		SelectRectAreaBoundrary(GVC->GetCurrentMousePos(), SelectedRectArea);
	}
}



void FArmyRectAreaOperation::StartTrackingDueToInput(const FArmyInputEventState& InPutState)
{

	if (!SelectedRectArea.IsValid())
	{
		return;
	}

	ModifyOperationState = true;

	if (IsOperating())
	{
		//FVector LocalPositon = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0.0f);
	//	LocalPositon = GetIntersectPointWithPlane(GVC->GetCurrentMousePos(), CurrentAttachArea);

	//	auto selectedArea = CurrentAttachArea->FindArea(LocalPositon);

		if (SelectedRectArea.IsValid())
		{
			DragType = SelectedRectArea->DriveByHoleOrBoundary == 1 ? EDragType::DragEdge : EDragType::DragArea;
			auto flag = SelectedRectArea->GetTransformFlag();
			TempRectArea->SetTransformFlag(flag);
			TempRectArea->StartTransformByEdge();
		}

		//if (DragType == EDragType::DragArea)
		//{
			//整体移动逻辑
		if (SelectedRectArea.IsValid())
		{
			TSharedPtr<FArmyBaseArea> Parent = SelectedRectArea->GetParentArea();
			if (Parent.IsValid())
			{
				SetCurrentParentSurface(Parent);
			}
		}

		if (!CurrentParentArea.IsValid())
		{
			SetCurrentParentSurface(CurrentAttachArea);
		}
		//}
		//else
		//{
			//单边移动

		//}
		TempRectArea->SetRectWidth(SelectedRectArea->GetRectWidth());
		TempRectArea->SetRectHeight(SelectedRectArea->GetRectHeight());
		TempRectArea->SetLeftUpCornerPos(SelectedRectArea->GetLeftUpCornerPos());

		FVector tempPos = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0.0f);
		tempPos = GetIntersectPointWithPlane(GVC->GetCurrentMousePos(), CurrentParentArea);
		LastMousePos = tempPos;
		CurrentMousePos = tempPos;
	}
}

void FArmyRectAreaOperation::StopTracking()
{
	if (IsOperating())
	{
		DragType = EDragType::NoDrag;
		/** @梁晓菲 判断灯槽是否存在，如果存在，需要用灯槽边界点确认是否可以添加在新位置*/
		TArray<FVector> tempOutVertices = TempRectArea->GetOutArea()->Vertices;
		if (!SelectedRectArea->GetLampSlotExtruder()->IsEmpty())
			tempOutVertices = FArmyMath::ExturdePolygon(tempOutVertices, (SelectedRectArea->GetLampSlotWidth() + 0.2f), false);

		bool CanPlaceHere = false;
		if (CurrentParentArea.IsValid())
		{
			CanPlaceHere = CurrentParentArea->TestAreaCanAddOrNot(tempOutVertices, TempRectArea->GetUniqueID());

			//for (auto & SubArea : SelectedRectArea->GetEditAreas())
			//{
			//	if (!CanPlaceHere)
			//	{
			//		break;
			//	}
			//	CanPlaceHere &= TempRectArea->TestAreaCanAddOrNot(SubArea->GetOutArea()->Vertices, SubArea->GetUniqueID());
			//}

		}
		if (CanPlaceHere)
		{
			//@郭子阳
			//移动不可以改变当前移动面的父面，移动的面必须在兄弟面之外
			for (auto &BrotherArea : CurrentParentArea->GetEditAreas())
			{
				if (BrotherArea.Get() == SelectedRectArea.Get())
				{
					continue;
				}
				if (FArmyMath::IsPolygonInPolygon(TempRectArea->GetOutArea()->Vertices, BrotherArea->GetOutArea()->Vertices)
					|| FArmyMath::IsPolygonInPolygon(BrotherArea->GetOutArea()->Vertices, TempRectArea->GetOutArea()->Vertices))
					
				{
					CanPlaceHere = false;
					break;
				}
			}
		}

		if (CanPlaceHere)
		{
			SCOPE_TRANSACTION(TEXT("修改矩形区域位置"));
			FVector Delta = TempRectArea->GetLeftUpCornerPos() - SelectedRectArea->GetLeftUpCornerPos();
			SelectedRectArea->OffsetLayingPointPos(Delta);

			SelectedRectArea->Move(Delta);
			//@郭子阳
			//修改大小
			SelectedRectArea->SetRectWidth(TempRectArea->GetRectWidth());
			SelectedRectArea->SetRectHeight(TempRectArea->GetRectHeight());
			SelectedRectArea->SetLeftUpCornerPos(TempRectArea->GetLeftUpCornerPos());
			SelectedRectArea->RecaculateRelatedArea();
			SelectedRectArea->Modify();
			GXREditor->SelectNone(true, true, false);

            // @欧石楠 更新高亮显示线的位置
            HelpDrawLineStart.X -= Delta.X;
            HelpDrawLineStart.Y += Delta.Y;
            HelpDrawLineEnd.X -= Delta.X;
            HelpDrawLineEnd.Y -= Delta.Y;
		}
		else
		{
			WarnningInfo();
		}
		//EndOperation();
		ModifyOperationState = false;

		//TempRectArea->GetWallActor()->SetActorHiddenInGame(true);
	}
}

void FArmyRectAreaOperation::BeginOperation(XRArgument InArg /*= XRArgument()*/)
{
	IsActive = true;

	FArmyOperation::BeginOperation(InArg);
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = true;
	TempRectArea->SetDrawOutLine(true);
	if (TempRectArea->GetWallActor())
	{
		TempRectArea->GetWallActor()->SetActorHiddenInGame(true);
	}

	CurrentState = 0;
	SethitTime(1);
}

void FArmyRectAreaOperation::EndOperation()
{
	FArmyOperation::EndOperation();
	if (SelectedRectArea.IsValid())
	{
		//SelectedRectArea->SetState(OS_Normal);
		SelectedRectArea = nullptr;
	}
	DrawBoundary = false;
	HelpDrawLineEnd = HelpDrawLineStart = FVector::ZeroVector;
	CurrentState = 0;
	OperationPoint = nullptr;
	CurrentAttachArea = nullptr;
	CurrentParentArea = nullptr;
	SetInputVisibility(false);
	ModifyOperationState = false;
	ModifyOperationTime = 0;
	SethitTime(1);
	SetModifyOperating(false);

	//@郭子阳 隐藏临时面，不隐藏会由于重叠而发生闪烁
	TempRectArea->SetRectWidth(0);

	IsActive = false;
}

void FArmyRectAreaOperation::Tick()
{
	FVector CurrentPos = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0.0f);
	if (CurrentState == 1)
	{
		if (!CurrentParentArea.IsValid())
			return;
		FVector offset = CurrentPos - RectArea->Pos;
		RectArea->Width = offset.X;
		RectArea->Height = offset.Y;
		UpdateInputBoxPosition();
	}
	else if (IsOperating())
	{
		const bool leftMouseBotton = GVC->Viewport->KeyState(EKeys::LeftMouseButton);
		if (leftMouseBotton)
		{
			CurrentMousePos = GetIntersectPointWithPlane(GVC->GetCurrentMousePos(), CurrentParentArea);
			FVector delta = CurrentMousePos - LastMousePos;

			//@郭子阳
			//临时面单边移动还是整体移动
			if (DragType == EDragType::DragArea)
			{
				TempRectArea->DriveByHoleOrBoundary = 0;
				TempRectArea->Move(delta);// SetCenterPosition(TempRectArea->GetCenterPosition() + delta);
			}
			else if (DragType == EDragType::DragEdge)
			{
				TempRectArea->DriveByHoleOrBoundary = 1;
				TempRectArea->MoveByEdge(delta);
			}
			LastMousePos = CurrentMousePos;
		}
		UpdateInputBoxPosition();
	}
}

void FArmyRectAreaOperation::UpdateInputBoxInfo(XRArgument InArg, FObjectPtr Object, TSharedPtr<FArmyEditPoint> InOperationPoint, bool bModifyOperation, bool bVisibility, bool bMove)
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
		UpdateInputBoxPosition();
	}
}

void FArmyRectAreaOperation::SetInputVisibility(bool visibility)
{
	UpWidthInputBoxWidget->Show(visibility);
	LeftHeightInputBoxWidget->Show(visibility);
	RightHeightInputBoxWidget->Show(visibility);
	DownWidthInputBoxWidget->Show(visibility);
}

void FArmyRectAreaOperation::SetCurrentEditSurface(TSharedPtr<FArmyRoomSpaceArea> RoomSpaceArea)
{
	if (RoomSpaceArea.IsValid())
	{
		CurrentAttachArea = RoomSpaceArea;
		//PlaneXDir = CurrentParentArea->GetXDir();
		//PlaneYDir = CurrentParentArea->GetYDir();
		//PlaneNormal = CurrentParentArea->GetPlaneNormal();
		//PlaneOrginPos = CurrentParentArea->GetPlaneCenter();

		//TempRectArea->SetPlaneInfo(CurrentParentArea->GetXDir(), CurrentParentArea->GetYDir(), CurrentParentArea->GetPlaneCenter());

		if (!CurrentParentArea.IsValid())
		{
			SetCurrentParentSurface(CurrentAttachArea);
		}
	}

}

void FArmyRectAreaOperation::SetCurrentParentSurface(TSharedPtr<FArmyBaseArea> BaseArea)
{
	if (BaseArea.IsValid())
	{
		CurrentParentArea = BaseArea;
		PlaneXDir = CurrentParentArea->GetXDir();
		PlaneYDir = CurrentParentArea->GetYDir();
		PlaneNormal = CurrentParentArea->GetPlaneNormal();
		PlaneOrginPos = CurrentParentArea->GetPlaneCenter();

		TempRectArea->SetPlaneInfo(CurrentParentArea->GetXDir(), CurrentParentArea->GetYDir(), CurrentParentArea->GetPlaneCenter());
		TempRectArea->SurfaceType = CurrentParentArea->SurfaceType;
	}
}

void FArmyRectAreaOperation::SelectRectAreaBoundrary(const FVector2D& InMousePos, TSharedPtr<FArmyObject> rect)
{
	SelectedRectArea = StaticCastSharedPtr<FArmyRectArea>(rect);

	TArray<FVector> tempBoundarys = SelectedRectArea->GetOutArea()->Vertices;
	DrawBoundary = false;
	int number = tempBoundarys.Num();
	for (int i = 0; i < number; ++i)
	{
		FVector start = GetWorldFromLocal(tempBoundarys[i%number], CurrentParentArea);
		FVector end = GetWorldFromLocal(tempBoundarys[(i + 1) % number], CurrentParentArea);

		FVector2D ScreenStart, ScreenEnd;
		GVC->WorldToPixel(start, ScreenStart);
		GVC->WorldToPixel(end, ScreenEnd);

		float dist = FMath::PointDistToSegment(FVector(InMousePos, 0.0), FVector(ScreenStart, 0.0f), FVector(ScreenEnd, 0.0f));
		float delta = FVector::DotProduct(FVector(ScreenEnd - InMousePos, 0.0f), FVector(ScreenStart - InMousePos, 0.0f));
		if (dist < 16 && delta <= 0.0f)
		{
			FVector	startPos = tempBoundarys[i%number];
			FVector endPos = tempBoundarys[(i + 1) % number];
			float offset = CurrentParentArea->SurfaceType == 2 ? -10.0f : 10.0f;
			HelpDrawLineStart = startPos.X * CurrentParentArea->GetXDir() + startPos.Y * CurrentParentArea->GetYDir() + PlaneOrginPos + PlaneNormal * offset;
			HelpDrawLineEnd = endPos.X * CurrentParentArea->GetXDir() + endPos.Y * CurrentParentArea->GetYDir() + PlaneOrginPos + PlaneNormal * offset;
			DrawBoundary = true;
			DisableHightLightArea();
			break;
		}
		else
		{
			DrawBoundary = false;
		}
	}
	if (SelectedRectArea.IsValid())
	{
		SelectedRectArea->DriveByHoleOrBoundary = DrawBoundary ? 1 : 0;
	}

}

void FArmyRectAreaOperation::DrawHelpRulerAxis(FPrimitiveDrawInterface* PDI, const FSceneView* View, FLinearColor color, float thicknes)
{
	PDI->DrawLine(GetWorldFromLocal(UpRulerAxis.StartPos, CurrentParentArea), GetWorldFromLocal(UpRulerAxis.EndPos, CurrentParentArea), color, 1, thicknes, 0.0f, true);
	PDI->DrawLine(GetWorldFromLocal(DownRulerAxis.StartPos, CurrentParentArea), GetWorldFromLocal(DownRulerAxis.EndPos, CurrentParentArea), color, 1, thicknes, 0.0f, true);
	PDI->DrawLine(GetWorldFromLocal(LeftRulerAxis.StartPos, CurrentParentArea), GetWorldFromLocal(LeftRulerAxis.EndPos, CurrentParentArea), color, 1, thicknes, 0.0f, true);
	PDI->DrawLine(GetWorldFromLocal(RightRulerAxis.StartPos, CurrentParentArea), GetWorldFromLocal(RightRulerAxis.EndPos, CurrentParentArea), color, 1, thicknes, 0.0f, true);
}

void FArmyRectAreaOperation::UpdateHelpRulerAxisPos()
{
	if (SelectedRectArea.IsValid())
	{
		FVector pos0 = SelectedRectArea->GetLeftUpCornerPos();
		FVector pos1 = pos0 + FVector(1, 0, 0) * SelectedRectArea->GetRectWidth();
		FVector pos2 = pos1 + FVector(0, 1, 0) * SelectedRectArea->GetRectHeight();
		FVector pos3 = pos0 + FVector(0, 1, 0) * SelectedRectArea->GetRectHeight();
		TArray<FVector> vertices = { pos0,pos1,pos2,pos3 };
		UpRulerAxis.StartPos = vertices[0] + FVector(0, 1, 0) * RulerOffset;
		UpRulerAxis.EndPos = vertices[1] + FVector(0, 1, 0)*RulerOffset;
		RightRulerAxis.StartPos = vertices[1] + FVector(-1, 0, 0) * RulerOffset;
		RightRulerAxis.EndPos = vertices[2] + FVector(-1, 0, 0)*RulerOffset;
		DownRulerAxis.StartPos = vertices[2] + FVector(0, -1, 0) * RulerOffset;
		DownRulerAxis.EndPos = vertices[3] + FVector(0, -1, 0) * RulerOffset;
		LeftRulerAxis.StartPos = vertices[0] + FVector(1, 0, 0)*RulerOffset;
		LeftRulerAxis.EndPos = vertices[3] + FVector(1, 0, 0)*RulerOffset;
	}
}

void FArmyRectAreaOperation::AddRectArea()
{
	if (!CurrentParentArea.IsValid())
		return;

	const TArray<FVector>& Vertexes = RectArea->GetVertices();//引用，非拷贝
	/* @梁晓菲 如果不可以添加，报错返回*/
	if (!CurrentParentArea->TestAreaCanAddOrNot(Vertexes, CurrentParentArea->GetUniqueID()))
	{
		WarnningInfo();
		return;
	}
	/* @梁晓菲 添加区域*/
	TSharedPtr<FArmyRectArea> ResultArea = MakeShareable(new FArmyRectArea());
	ResultArea->SurportPlaceArea = CurrentParentArea->SurportPlaceArea;
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
	else
	{
		MatStyle = MakeShareable(new FArmySeamlessStyle());
		MatStyle->SetMainTextureInfo(FArmyEngineModule::Get().GetEngineResource()->GetDefaultTextureCeiling(), 400, 400);
		FString Path = TEXT("/Game/XRCommon/HardMode/CeilingTexture.CeilingTexture");
		MatStyle->SetMainTextureLocalPath(Path);
	}
	ResultArea->SurfaceType = CurrentParentArea->SurfaceType;
	ResultArea->SetPlaneInfo(CurrentParentArea->GetXDir(), CurrentParentArea->GetYDir(), CurrentParentArea->GetPlaneCenter());
	MatStyle->SetPlaneInfo(ResultArea->GetPlaneCenter(), ResultArea->GetXDir(), ResultArea->GetYDir());
	ResultArea->SetStyle(MatStyle);
	ResultArea->SetVertice(Vertexes);
	FGuid GUID = CurrentParentArea->GetUniqueID();
	ResultArea->SetParentGuid(GUID);
	ResultArea->ConstructionItemData->SetConstructionItemCheckedId(CurrentParentArea->ConstructionItemData->CheckedId);
	ResultArea->ConstructionItemData->bHasSetted = true;

	XRArgument arg;
	arg._ArgUint32 = E_HardModel;
	//if (CurrentParentArea->SurfaceType != 0)
	MatStyle->SetInternalDist(0.0f);
	SCOPE_TRANSACTION(TEXT("添加矩形区域"));

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
	ResultArea->BoundaryDriveEvent.BindRaw(this, &FArmyRectAreaOperation::RefreshBoundaryDriveEvent);
	ResultArea->PositionChanged.AddRaw(this, &FArmyRectAreaOperation::DisableHightLightArea);
}

void FArmyRectAreaOperation::ModifyRectArea()
{
	SCOPE_TRANSACTION(TEXT("修改矩形区域"));
	SelectedRectArea->SetRectWidth(TempRectArea->GetRectWidth());
	SelectedRectArea->SetRectHeight(TempRectArea->GetRectHeight());
	SelectedRectArea->SetLeftUpCornerPos(TempRectArea->GetLeftUpCornerPos());
	SelectedRectArea->RecaculateRelatedArea();
	SelectedRectArea->Modify();
}

void FArmyRectAreaOperation::RefreshBoundaryDriveEvent()
{
	DrawBoundary = false;
	if (SelectedRectArea.IsValid())
	{
		SelectedRectArea->DriveByHoleOrBoundary = 0;
		RectArea->Pos = SelectedRectArea->GetLeftUpCornerPos();
		RectArea->Width = SelectedRectArea->GetRectWidth();
		RectArea->Height = SelectedRectArea->GetRectHeight();
		UpdateInputBoxPosition();
	}
	if (HelpHightLightArea)
		HelpHightLightArea->SetActorHiddenInGame(true);
	EndOperation();
}


void FArmyRectAreaOperation::DisableHightLightArea()
{
	if (HelpHightLightArea)
		HelpHightLightArea->SetActorHiddenInGame(true);
}

void FArmyRectAreaOperation::GetCurrentParentSurfaceByMouse()
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

void FArmyRectAreaOperation::OnRectDrawFinished()
{

}

void FArmyRectAreaOperation::WarnningInfo()
{
	GGI->Window->ShowMessage(MT_Warning, TEXT("与其他空间区域相交"));

}

void FArmyRectAreaOperation::UpdateInputBoxPosition()
{
	UpdateHelpRulerAxisPos();
	FVector start, end;
	start = LeftRulerAxis.StartPos;
	end = LeftRulerAxis.EndPos;
	UpdateBoxWidget(LeftHeightInputBoxWidget, start, end, false);
	start = UpRulerAxis.StartPos;
	end = UpRulerAxis.EndPos;
	UpdateBoxWidget(UpWidthInputBoxWidget, start, end, true);

	start = RightRulerAxis.StartPos;
	end = RightRulerAxis.EndPos;
	UpdateBoxWidget(RightHeightInputBoxWidget, start, end, false);
	start = DownRulerAxis.StartPos;
	end = DownRulerAxis.EndPos;
	UpdateBoxWidget(DownWidthInputBoxWidget, start, end, true);
}

void FArmyRectAreaOperation::UpdateBoxWidget(TSharedPtr<SArmyInputBox> InputBox, FVector start, FVector end, bool IsHorizontal)
{
	bool bShowEditInputBox = InputBox->SetStartAndEnd(start, end);
	if (bShowEditInputBox)
	{
		FVector2D ViewportSize;
		GVC->GetViewportSize(ViewportSize);

		FVector startPos = GetWorldFromLocal(start, CurrentParentArea);
		FVector endPos = GetWorldFromLocal(end, CurrentParentArea);

		FVector LineCenter = (startPos + endPos) / 2;
		FVector2D InputBoxPos;
		GVC->WorldToPixel(LineCenter, InputBoxPos);
		InputBoxPos -= ViewportSize / 2;
		if (!IsHorizontal)
		{
			FQuat2D quat2D(PI / 2);
			FMatrix2x2 matrix(quat2D);
			FTransform2D trans(matrix, InputBoxPos);
			InputBox->SetRenderTransformPivot(FVector2D(0.5, 0.5));

			InputBox->SetTransform(trans);
			InputBox->GetDesiredSize();

		}
		else
			InputBox->SetPos(InputBoxPos);
	}

}
void FArmyRectAreaOperation::OnInputBoxKeyDown(const FKeyEvent& InKeyEvent)
{

}

void FArmyRectAreaOperation::OnUpWidthInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		const int32 LineLength = FCString::Atoi(*InText.ToString());
		if (!IsOperating() && GetModifyOperating() == false)
		{
			FVector CurrentPos = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0.0f);
			FVector offset = CurrentPos - RectArea->Pos;
			RectArea->Width = offset.X >= 0 ? (LineLength / 10.0f) : (-LineLength / 10.0f);
			UpdateInputBoxPosition();
			UpWidthInputBoxWidget->Show(false);
		}
		else
		{
			TempRectArea->SetRectWidth(LineLength / 10.0f);

			/** @梁晓菲 判断灯槽是否存在，如果存在，需要用灯槽边界点确认是否可以添加在新位置*/
			TArray<FVector> tempOutVertices = TempRectArea->GetOutArea()->Vertices;
            if (!SelectedRectArea->GetLampSlotExtruder()->IsEmpty())
            {
				tempOutVertices = FArmyMath::ExturdePolygon(tempOutVertices, (SelectedRectArea->GetLampSlotWidth() + 0.2f), false);
            }

			if (CurrentParentArea->TestAreaCanAddOrNot(tempOutVertices, TempRectArea->GetUniqueID()))
			{
				RectArea->Width = LineLength / 10.0f;
				ModifyRectArea();
				UpdateInputBoxPosition();
				GXREditor->SelectNone(true, true, false);
			}
			else
			{
				const FText LineLengthText = FText::FromString(FString::Printf(TEXT("%.0f mm"), SelectedRectArea->GetRectWidth() * 10));
				UpWidthInputBoxWidget->SetInputText(LineLengthText);
				DownWidthInputBoxWidget->SetInputText(LineLengthText);
				WarnningInfo();
			}
		}
		DrawBoundary = false;
	}
}

void FArmyRectAreaOperation::OnLeftHeightInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		const int32 LineLength = FCString::Atoi(*InText.ToString());
		FVector CurrentPos = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0.0f);
		FVector offset = CurrentPos - RectArea->Pos;
		RectArea->Height = offset.Y >= 0 ? (LineLength / 10.0f) : (-LineLength / 10.0f);

		if ((!IsOperating() && GetModifyOperating() != true))
		{
			AddRectArea();
		}
		else
		{
			TempRectArea->SetRectHeight(LineLength / 10.0f);

			/** @梁晓菲 判断灯槽是否存在，如果存在，需要用灯槽边界点确认是否可以添加在新位置*/
			TArray<FVector> tempOutVertices = TempRectArea->GetOutArea()->Vertices;
            if (!SelectedRectArea->GetLampSlotExtruder()->IsEmpty())
            {
				tempOutVertices = FArmyMath::ExturdePolygon(tempOutVertices, (SelectedRectArea->GetLampSlotWidth() + 0.2f), false);
            }

			if (CurrentParentArea->TestAreaCanAddOrNot(tempOutVertices, TempRectArea->GetUniqueID()))
			{
				ModifyRectArea();
				UpdateInputBoxPosition();
				GXREditor->SelectNone(true, true, false);
			}
			else
			{
				const FText LineLengthText = FText::FromString(FString::Printf(TEXT("%.0f mm"), SelectedRectArea->GetRectHeight() * 10));
				LeftHeightInputBoxWidget->SetInputText(LineLengthText);
				RightHeightInputBoxWidget->SetInputText(LineLengthText);
				WarnningInfo();
			}

		}
		LeftHeightInputBoxWidget->Show(false);
		SetModifyOperating(false);
		EndOperation();
	}
}

bool FArmyRectAreaOperation::IsOperating()
{
	return ModifyOperationState && SelectedRectArea.IsValid();
}


