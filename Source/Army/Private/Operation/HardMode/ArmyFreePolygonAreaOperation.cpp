#include "ArmyFreePolygonAreaOperation.h"
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
#include "ArmyFreePolygonArea.h"
#include "ArmyTrapezoidStyle.h"
#include "ArmyGameInstance.h"
#include "ArmySeamlessStyle.h"
#include "ArmyTools/Public/XRClipper.h"
#include "ArmyEditorEngine.h"
FArmyFreePolygonAreaOperation::FArmyFreePolygonAreaOperation(EModelType InBelongModel) :
	FArmyOperation(InBelongModel)
{
	SelectedArea = NULL;
	TempPolygonVertices.Empty();
	BDrawTempLine = true;
	bShouldTick = true;
}

void FArmyFreePolygonAreaOperation::InitWidget(TSharedPtr<SOverlay> InParentWidget)
{
	InParentWidget->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(InputBoxWidget, SArmyInputBox)
			.OnInputBoxCommitted(FOnTextCommitted::CreateRaw(this, &FArmyFreePolygonAreaOperation::OnInputBoxCommitted))
		];
}

void FArmyFreePolygonAreaOperation::SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPrimitive)
{
	if (Object.IsValid())
	{
		CurrentState = -1;
		SelectedArea = StaticCastSharedPtr<FArmyFreePolygonArea>(Object);
		TempPolygonVertices = SelectedArea->GetOutArea()->Vertices;

		if (SelectedArea.IsValid() && SelectedArea->GetParentArea().IsValid())
		{
			SetCurrentParentSurface(SelectedArea->GetParentArea());
		}
		else
		{
			SetCurrentParentSurface(CurrentAttachArea);
		}
	}
}

void FArmyFreePolygonAreaOperation::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (CurrentState == 0)
	{
		int number = PolygonVertices.Num();
		if (number > 0)
		{
			FVector last = GetWorldFromLocal(PolygonVertices[number - 1], CurrentParentArea);
			FVector CurrentPoint = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0.0f);
			CurrentPoint = GetWorldFromLocal(CurrentPoint, CurrentParentArea);
			if (BDrawTempLine)
			{
				PDI->DrawLine(last, CurrentPoint, FLinearColor(FColor(0XFF666666)), 1);
			}
			if (number >= 2)
			{
				for (int i = 0; i < number - 1; ++i)
				{
					FVector start = GetWorldFromLocal(PolygonVertices[i], CurrentParentArea);
					FVector end = GetWorldFromLocal(PolygonVertices[i + 1], CurrentParentArea);
					PDI->DrawLine(start, end, FLinearColor(FColor(0XFF666666)), 1);
				}
			}
		}
	}
	else if (IsOperating())
	{
		int32 Num = TempPolygonVertices.Num();
		for (int32 i = 0; i < Num; i++)
		{
			FVector start = GetWorldFromLocal(TempPolygonVertices[i % Num], CurrentParentArea);
			FVector end = GetWorldFromLocal(TempPolygonVertices[(i + 1) % Num], CurrentParentArea);
			PDI->DrawLine(start, end, FLinearColor(FColor(0XFF666666)), 1);
		}
	}
}

void FArmyFreePolygonAreaOperation::ProcessClickInViewPortClient(FViewport* InViewPort, FKey Key, EInputEvent Event)
{
	if (!SelectedArea.IsValid())
	{
		if (Key == EKeys::LeftMouseButton)
		{
			if (CurrentState == 0)
			{
				GetCurrentParentSurfaceByMouse();
				FVector CurrentPoint = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0.0f);
				FArmyToolsModule::Get().GetMouseCaptureTool()->SetCurrentPoint(FVector2D(CurrentPoint));
				if (PolygonVertices.IsValidIndex(0) && PolygonVertices[0] == CurrentPoint)
				{
					AddPolyArea();
					EndOperation();
				}
				else
				{
					PolygonVertices.Add(CurrentPoint);
				}
			}
		}
		else if (Key == EKeys::RightMouseButton)
		{
			if (FArmyMath::IsPolygonSimple(PolygonVertices))
			{
				AddPolyArea();
			}
			else
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("自相交"));
			}
			EndOperation();
		}
	}
	else
	{
		if (SelectedArea.IsValid())
		{
			TSharedPtr<FArmyBaseArea> Parent = SelectedArea->GetParentArea();
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

/* @梁晓菲 自由多边形选择的时候不能出现输入框！！！*/
void FArmyFreePolygonAreaOperation::UpdateInputBoxInfo(XRArgument InArg, FObjectPtr Object, TSharedPtr<FArmyEditPoint> InOperationPoint, bool bModifyOperation, bool bVisibility, bool bMove)
{
	ModifyOperationState = true;
	SetSelected(Object, InOperationPoint);
}

void FArmyFreePolygonAreaOperation::BeginOperation(XRArgument InArg /*= XRArgument()*/)
{
	FArmyOperation::BeginOperation(InArg);
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = true;
	CurrentState = 0;
	PolygonVertices.Empty();
	SethitTime(1);
}

void FArmyFreePolygonAreaOperation::EndOperation()
{
	CurrentState = -1;
	if (SelectedArea.IsValid())
	{
		//SelectedArea->SetState(OS_Normal);
		SelectedArea = NULL;
	}
	PolygonVertices.Empty();
	FArmyOperation::EndOperation();
	ModifyOperationState = false;
	SetInputVisibility(false);
	BDrawTempLine = true;
	bShouldTick = true;
}

void FArmyFreePolygonAreaOperation::Tick()
{
	if (CurrentState == 0)
	{
		FVector CurrentPoint = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0.0f);
		if (PolygonVertices.Num() > 0)
		{
			FVector lastVector = PolygonVertices[PolygonVertices.Num() - 1];
			bool bShowInputBox = false;
			bShouldTick = ShouldTick();
			if (bShouldTick)
			{
				BDrawTempLine = true;
				bShowInputBox = InputBoxWidget->SetStartAndEnd(lastVector, CurrentPoint);
			}
			if (bShowInputBox)
			{
				FVector2D ViewportSize;
				GVC->GetViewportSize(ViewportSize);

				FVector startPos = GetWorldFromLocal(lastVector, CurrentParentArea);
				FVector endPos = GetWorldFromLocal(CurrentPoint, CurrentParentArea);

				FVector LineCenter = (startPos + endPos) / 2;
				FVector2D InputBoxPos;
				GVC->WorldToPixel(LineCenter, InputBoxPos);
				InputBoxPos -= ViewportSize / 2;
				InputBoxWidget->SetPos(InputBoxPos);
				if (bShouldTick)
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
			TArray<FVector> TempVertexes;
			for (int32 i = 0; i < TempPolygonVertices.Num(); i++)
			{
				TempPolygonVertices[i] += delta;
			}
			LastMousePos = CurrentMousePos;
		}
	}
}

void FArmyFreePolygonAreaOperation::SetCurrentEditSurface(TSharedPtr<class FArmyRoomSpaceArea> RoomSpaceArea)
{
	CurrentAttachArea = RoomSpaceArea;
}

void FArmyFreePolygonAreaOperation::GetCurrentParentSurfaceByMouse()
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

void FArmyFreePolygonAreaOperation::SetCurrentParentSurface(TSharedPtr<class FArmyBaseArea> BaseArea)
{
	CurrentParentArea = BaseArea;
}

TArray< TSharedPtr<class FArmyLine> >& FArmyFreePolygonAreaOperation::GetLines()
{
	CaputerLines.Empty();
	int number = PolygonVertices.Num();
	if (number > 2)
	{
		for (int i = 0; i < number - 1; ++i)
		{
			TSharedPtr<FArmyLine> tempLine = MakeShareable(new FArmyLine(PolygonVertices[i], PolygonVertices[i + 1]));
			CaputerLines.Push(tempLine);

		}
	}
	return CaputerLines;
}

void FArmyFreePolygonAreaOperation::StartTrackingDueToInput(const FArmyInputEventState& InPutState)
{
	ModifyOperationState = true;

	if (SelectedArea.IsValid())
	{

		/*if (SelectedRegularPolyArea.IsValid())
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
		}*/
		TempPolygonVertices = SelectedArea->GetOutVertices();


		FVector tempPos = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0.0f);
		tempPos = GetIntersectPointWithPlane(GVC->GetCurrentMousePos(), CurrentParentArea);
		LastMousePos = tempPos;
		CurrentMousePos = tempPos;
	}

	/* @梁晓菲 自由多边形只有在绘制的时候会出现输入框 */
}

void FArmyFreePolygonAreaOperation::StopTracking()
{
	if (IsOperating())
	{
		/** @梁晓菲 判断灯槽是否存在，如果存在，需要用灯槽边界点确认是否可以添加在新位置*/
		TArray<FVector> tempOutVertices = TempPolygonVertices;
        if (!SelectedArea->GetLampSlotExtruder()->IsEmpty())
        {
			tempOutVertices = FArmyMath::ExturdePolygon(tempOutVertices, (SelectedArea->GetLampSlotWidth() + 0.2f), false);
        }

		bool CanPlaceHere = false;
		if(CurrentParentArea.IsValid())
		{ 
			CanPlaceHere = CurrentParentArea->TestAreaCanAddOrNot(tempOutVertices, SelectedArea->GetUniqueID());
		}
		if (CanPlaceHere)
		{
			//@郭子阳
			//移动不可以改变当前移动面的父面，移动的面必须在父面所有子面之外
			for (auto &BrotherArea : CurrentParentArea->GetEditAreas())
			{
				if (BrotherArea.Get() == SelectedArea.Get())
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
			if (SelectedArea.IsValid())
			{
				SCOPE_TRANSACTION(TEXT("移动多边形位置"));
                TArray<FVector> OutVertices = SelectedArea->GetOutVertices();
                FVector Delta = TempPolygonVertices[0] - OutVertices[0];
                SelectedArea->OffsetLayingPointPos(Delta);
				PolygonVertices = TempPolygonVertices;
				SelectedArea->Move(Delta);
				//SelectedArea->SetVertices(PolygonVertices);
				//SelectedArea->RecaculateRelatedArea();
				//SelectedArea->Modify();
				GXREditor->SelectNone(true, true, false);
			}
		}
		else
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("与其他空间区域相交"));
		}
		//EndOperation();
		ModifyOperationState = false;
	}
}

void FArmyFreePolygonAreaOperation::AddPolyArea()
{
	if (!CurrentParentArea.IsValid())
		return;

	/* @梁晓菲 如果不可以添加，报错返回*/
	if (!CurrentParentArea->TestAreaCanAddOrNot(PolygonVertices, CurrentParentArea->GetUniqueID()))
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("与其他空间区域相交"));
		return;
	}

	/* @梁晓菲 添加区域*/
	TSharedPtr<FArmyFreePolygonArea> ResultArea = MakeShareable(new FArmyFreePolygonArea());
	ResultArea->SurportPlaceArea = CurrentParentArea->SurportPlaceArea;
	ResultArea->SetPlaneInfo(CurrentParentArea->GetXDir(), CurrentParentArea->GetYDir(), CurrentParentArea->GetPlaneCenter());
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

	ResultArea->SetPlaneInfo(CurrentParentArea->GetXDir(), CurrentParentArea->GetYDir(), CurrentParentArea->GetPlaneCenter());
	FArmyMath::RemoveComPointOnPolygon(PolygonVertices);
	ResultArea->SetVertices(PolygonVertices);
	MatStyle->SetPlaneInfo(ResultArea->GetPlaneCenter(), ResultArea->GetXDir(), ResultArea->GetYDir());
	ResultArea->SetStyle(MatStyle);
	FGuid GUID = CurrentParentArea->GetUniqueID();
	ResultArea->SetParentGuid(GUID);
	ResultArea->ConstructionItemData->SetConstructionItemCheckedId(CurrentParentArea->ConstructionItemData->CheckedId);
	ResultArea->ConstructionItemData->bHasSetted = true;

	XRArgument arg;
	arg._ArgUint32 = E_HardModel;
	ResultArea->SurfaceType = CurrentParentArea->SurfaceType;
	//if (CurrentParentArea->SurfaceType != 0)
		MatStyle->SetInternalDist(0.0f);
	SCOPE_TRANSACTION(TEXT("添加自由区域"));
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

void FArmyFreePolygonAreaOperation::SetInputVisibility(bool visibility)
{
	InputBoxWidget->SetFocus(visibility);
	InputBoxWidget->Show(visibility);
}

void FArmyFreePolygonAreaOperation::OnInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		if (CurrentState == 0)
		{
			const int32 LineLength = FCString::Atoi(*InText.ToString());
			if (LineLength > 0 && PolygonVertices.Num() > 0)
			{
				FVector CurrentPoint = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0.0f);
				FVector direction = CurrentPoint - PolygonVertices[PolygonVertices.Num() - 1];
				FVector NewPointPosition = PolygonVertices[PolygonVertices.Num() - 1] + direction.GetSafeNormal()*LineLength / 10.f;
				PolygonVertices.Add(NewPointPosition);
				//确定输入数值后，就不绘制鼠标到上一个点的线了
				BDrawTempLine = false;
			}
		}
	}
}

bool FArmyFreePolygonAreaOperation::IsOperating()
{
	return ModifyOperationState && SelectedArea.IsValid();
}

