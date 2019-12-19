#include "ArmyBaseSurfaceController.h"
#include "ArmyReplaceTextureOperation.h"
#include "ArmyRectAreaOperation.h"
#include "ArmyCircleAreaOperation.h"
#include "ArmyRegularPolygonAreaOperation.h"
#include "ArmyFreePolygonAreaOperation.h"
#include "ArmyBaseArea.h"
#include "SContentItem.h"
#include "ArmyRoomSpaceArea.h"
#include "ArmyAxisRuler.h"
#include "SArmyDesignTitleBar.h"
#include "ArmyExtrusionActor.h"
#include "ArmyCircleArea.h"
#include "ArmyRectArea.h"
#include "ArmyRegularPolygonArea.h"
#include "ArmyFreePolygonArea.h"
#include "ArmyEngineModule.h"
#include "ArmyShapeBoardActor.h"
#include "ArmyEditorEngine.h"
#include "ArmyGameInstance.h"
#include "ArmySceneData/Private/Data/HardModeData/Mathematics/EarcutTesselator.h"
#include "ArmySceneData/Public/Data/HardModeData/Mathematics/BspPolygon2d.h"
#include "IArmyActorVisitor.h"
#include "ArmyUser.h"
#include "ArmyFrameCommands.h"
#include "ArmyActorVisitorFactory.h"
#include "ArmyPlayerController.h"
#include "ArmyHardModeController.h"
#include "SArmyCategoryOrganizationManage.h"
#include "ArmyLayingPoint.h"
#include "ArmyHardModeCommonData.h"
#include "ArmyMaterialBrushOperation.h"
#include "ArmyLayingPoint.h"
#include "ArmyCircleArea.h"
#include "ArmyWallActor.h"
#include "ArmyWallLine.h"

void FArmyBaseSurfaceController::PostInit()
{
	TSharedPtr<FArmyReplaceTextureOperation> TempReplace = MakeShareable(new FArmyReplaceTextureOperation(E_HardModel));
	TempReplace->SetEditMode(1);
	
	TSharedPtr<FArmyRectAreaOperation> TempRectOp = MakeShareable(new FArmyRectAreaOperation(E_HardModel));
	TempRectOp->HelpHightLightArea = HightLightActor;
	TSharedPtr<FArmyCircleAreaOperation> TempCircleOp = MakeShareable(new FArmyCircleAreaOperation(E_HardModel));
	TSharedPtr<FArmyRegularPolygonAreaOperation> TempPolygon = MakeShareable(new FArmyRegularPolygonAreaOperation(E_HardModel));
	TSharedPtr<FArmyFreePolygonAreaOperation> tempFreePolygon = MakeShareable(new FArmyFreePolygonAreaOperation(E_HomeModel));

	OperationMap.Add((uint8)EHardModeOperation::HO_DrawRect, TempRectOp);
	OperationMap.Add((uint8)EHardModeOperation::HO_DrawCircle, TempCircleOp);
	OperationMap.Add((uint8)EHardModeOperation::HO_DrawPolygon, TempPolygon);
	OperationMap.Add((uint8)EHardModeOperation::HO_DrawFreePolygon, tempFreePolygon);
	OperationMap.Add((uint8)EHardModeOperation::HO_Replace, TempReplace);

	{
		
		/*{
			TSharedPtr<FArmyMaterialBrushOperation> OP_MaterialBrush = MakeShared< FArmyMaterialBrushOperation>(E_HardModel);
		}*/
		//TSharedPtr<FArmyMaterialBrushOperation> Operation = MakeShared< FArmyMaterialBrushOperation>(E_HardModel);
		OperationMap.Add((uint8)EHardModeOperation::HO_MaterialBrush, HardModCommonOperations:: OP_MaterialBrush);
	}

	for (auto It : OperationMap)
	{
		It.Value->Init();
		It.Value->InitWidget(GVC->ViewportOverlayWidget);
		It.Value->EndOperationDelegate.BindRaw(this, &FArmyBaseSurfaceController::EndOperation);
	}

	AxisRuler = MakeShareable(new FArmyAxisRuler());
	//AxisRuler->SetUseInDrawArea(true);
	AxisRuler->Init(GVC->ViewportOverlayWidget);
	AxisRuler->AxisOperationDelegate.BindRaw(this, &FArmyBaseSurfaceController::MoveDrawArea);

	CheckHasSelectedAreaObj = FCanExecuteAction::CreateRaw(this, &FArmyBaseSurfaceController::CheckHasSelectedArea);

    LayingPoint = MakeShareable(new FArmyLayingPoint);
}

void FArmyBaseSurfaceController::ReplaceTextureOperation(TSharedPtr< FContentItemSpace::FContentItem> NewItem /*= nullptr*/)
{

	SetOperation((uint8)EHardModeOperation::HO_Replace);
	TSharedPtr<FArmyReplaceTextureOperation>	tempOperation = StaticCastSharedPtr<FArmyReplaceTextureOperation>(CurrentOperation);
	if (!NewItem.IsValid())
	{
		tempOperation->SetCurrentItem(LeftPanel->GetSelectedContentItem());
	}
	//tempOperation->SetCurrentSelectItem(LeftPanel->GetSelectItem());
	tempOperation->SetEditMode(1);
	tempOperation->SetCurrentEditSurface(CurrentEditSurface);
	UMaterialInstanceDynamic* material = Cast<UMaterialInstanceDynamic>(HightLightActor->GetMeshComponent()->GetMaterial(0));
	if (material)
	{
		material->SetScalarParameterValue("Alpha", 0.0f);
	}
	AddAreaState = 1;
}
bool FArmyBaseSurfaceController::InputKey(FViewport* Viewport, FKey Key, EInputEvent Event)
{
	if (CurrentEditSurface.IsValid())
	{
		if (Key == EKeys::MouseScrollDown || Key == EKeys::MouseScrollUp)
		{
			FArmyInputEventState InputState(GVC->Viewport, Key, Event);
			GVC->ZoomCanvas(InputState);
			if (CurrentOperation.IsValid())
			{
				CurrentOperation->Tick();
			}
		}
		if (Key == EKeys::Delete)
		{
			OnDeleteCommand();
		}
	}

	FArmyInputEventState InputState(Viewport, Key, Event);
	if (InputState.IsAnyMouseButtonDown()
		&& (Event == IE_Pressed || Event == IE_Released)
		&& (InputState.IsMouseButtonEvent() || InputState.IsCtrlButtonEvent() || InputState.IsAltButtonEvent() || InputState.IsShiftButtonEvent()))
	{

		StartTrackingDueToInput(InputState);
		return true;
	}
	if (IsTracking && !InputState.IsAnyMouseButtonDown() && InputState.IsMouseButtonEvent())
	{
		ProcessClickInViewPortClient(Viewport, Key, Event);
		StopTracking();
	}
	return true;
}

void FArmyBaseSurfaceController::ProcessClickInViewPortClient(FViewport* ViewPort, FKey Key, EInputEvent Event)
{
	float mouseDelta = (CurrentMouseClick - LastMouseClick).Size();
    if (mouseDelta < 1000.0f && mouseDelta > 3.0f)
    {
        return;
    }

    // @欧石楠 选中起铺点
    if (IsLayingPointSelected())
    {
        LayingPoint->SetState(OS_Selected);
        AxisRuler->SetBoundObj(LayingPoint);
        return;
    }
    else
    {
        if (selected.IsValid())
        {
            LayingPoint->SetState(OS_Normal);
        }
        AxisRuler->SetBoundObj(nullptr);
    }

    if (Key == EKeys::LeftMouseButton)
    {
        GXREditor->SelectNone(true, true);
        selected = nullptr;

        TArray<TSharedPtr<FArmyBaseArea>> TotalSelectedObjs = PickOperation(FVector(CurrentMouseClick, 0.f), GVC);
        if (TotalSelectedObjs.Num() > 0)
        {
            selected = TotalSelectedObjs[0];
            LayingPoint->SetArea(selected);
        }
        else
        {
            LayingPoint->SetArea(nullptr);

            int32 HitX = ViewPort->GetMouseX();
            int32 HitY = ViewPort->GetMouseY();
            HHitProxy* HitProxy = ViewPort->GetHitProxy(HitX, HitY);
            if (HitProxy && HitProxy->IsA(HActor::StaticGetType()))
            {
                AActor* Actor = ((HActor*)HitProxy)->Actor;
                if (Actor && Actor->IsValidLowLevel() && Actor->IsA(AArmyExtrusionActor::StaticClass()))
                {
                    GXREditor->SelectActor(Actor, true, true);
                    return;
                }
            }
        }
    }
    else
    {
        if (CurrentOperation.IsValid())
        {
            CurrentOperation->EndOperation();
        }
    }

	if ( AddAreaState == 0 && CurrentOperation != *(OperationMap.Find((uint8)EHardModeOperation::HO_MaterialBrush)) )
    {
		SetModifyAreaOperation(ViewPort, Key, Event);
    }
	if (CurrentOperation.IsValid())
	{
		CurrentOperation->ProcessClickInViewPortClient(ViewPort, Key, Event);
	}
}



void FArmyBaseSurfaceController::SetCurrentOperation(const FVector2D& InPos, TSharedPtr<FArmyBaseArea> InSelectedArea)
{
	if (InSelectedArea.IsValid())
	{

		if (CurrentOperation == OperationMap.FindRef((uint8)EHardModeOperation::HO_Replace))
		{
			ReplaceTextureOperation();
			return;
		}

		if (InSelectedArea->GetType() != OT_RoomSpaceArea)
		{
			if (InSelectedArea->GetType() == OT_RectArea)
			{
				SetOperation((uint8)EHardModeOperation::HO_DrawRect);
				CurrentOperation->SetCurrentEditSurface(CurrentEditSurface);
				TSharedPtr<FArmyRectAreaOperation> tempRectOperation = StaticCastSharedPtr<FArmyRectAreaOperation>(CurrentOperation);
				if (tempRectOperation.IsValid())
				{
					tempRectOperation->SelectRectAreaBoundrary(InPos, selected);
				}
			}
			else if (InSelectedArea->GetType() == OT_CircleArea)
			{
				SetOperation((uint8)EHardModeOperation::HO_DrawCircle);
				CurrentOperation->SetCurrentEditSurface(CurrentEditSurface);
			}
			else if (InSelectedArea->GetType() == OT_FreePolygonArea)
			{
				SetOperation((uint8)EHardModeOperation::HO_DrawFreePolygon);
				CurrentOperation->SetCurrentEditSurface(CurrentEditSurface);
			}
			else if (InSelectedArea->GetType() == OT_PolygonArea)
			{
				SetOperation((uint8)EHardModeOperation::HO_DrawPolygon);
				CurrentOperation->SetCurrentEditSurface(CurrentEditSurface);
			}
			else if (InSelectedArea->GetType() == OT_BodaArea)
			{
				SetOperation((uint8)EHardModeOperation::HO_DrawRect);
				XRArgument InArg;
				CurrentOperation->UpdateInputBoxInfo(InArg, nullptr, NULL, true, false, false);
				TSharedPtr<FArmyBodaArea> tempBodaArea = StaticCastSharedPtr<FArmyBodaArea>(selected);
				TSharedPtr<FArmyRectArea> tempRectArea = StaticCastSharedPtr<FArmyRectArea>(tempBodaArea->AttachParentArea);
				AxisRuler->SetBoundObj(tempRectArea);
				return;
			}
			XRArgument InArg;
			CurrentOperation->UpdateInputBoxInfo(InArg, selected, NULL, true, true, false);
		}
	}
}

void FArmyBaseSurfaceController::ClearCurrentEditorSurfaceData()
{
	if (CurrentEditSurface.IsValid())
	{
		if (CurrentOperation.IsValid())
		{
			CurrentOperation->EndOperation();
		}
		if (AxisRuler.IsValid())
		{
			AxisRuler->SetBoundObj(NULL);
		}

		CurrentEditSurface->ResetSurfaceStyle(true);
		TArray<TSharedPtr<FArmyObject>> AllChildrenObj = CurrentEditSurface->GetChildren();
		for (auto & ItChildren : AllChildrenObj)
		{
			FArmySceneData::Get()->Delete(ItChildren);
		}
		if (SufaceDetail.IsValid())
			SufaceDetail->SetPropertyPanelVisibility(EVisibility::Hidden);
		TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter =
			TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&CurrentEditSurface->RecordBeforeModify);

		// 将XRObject序列化为json字符串
		JsonWriter->WriteObjectStart();
		CurrentEditSurface->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
		JsonWriter->Close();
		CurrentEditSurface->Record = CurrentEditSurface->RecordBeforeModify;
		TransMgr->Reset();
	}
}

void FArmyBaseSurfaceController::TransToCurrentPlane(FVector& InPos)
{
	float x = FVector::DotProduct((InPos - CurrentEditSurface->GetPlaneCenter()), CurrentEditSurface->GetXDir());
	float y = FVector::DotProduct((InPos - CurrentEditSurface->GetPlaneCenter()), CurrentEditSurface->GetYDir());
	InPos = FVector(x, y, 0);
}

void FArmyBaseSurfaceController::TransPlaneToWorld(FVector& InPos)
{
	FVector result = InPos.X * CurrentEditSurface->GetXDir() + InPos.Y * CurrentEditSurface->GetYDir() + CurrentEditSurface->GetPlaneCenter();
	InPos = FVector(result.X, result.Y, 0.0f);
}
struct TempEdge
{
	FVector StartPos;
	FVector EndPos;
	TempEdge(const FVector& start, const FVector& end)
	{
		StartPos = start;
		EndPos = end;
	}
	TempEdge operator()(const FVector& start, const FVector& end)
	{
		StartPos = start;
		EndPos = end;
		return *this;
	}

	bool Intersect(const TempEdge& Other, FVector& IntersectPoint) const
	{
		FVector2D resut = FVector2D::ZeroVector;
		if (FArmyMath::Line2DIntersection(FVector2D(StartPos), FVector2D(EndPos), FVector2D(Other.StartPos), FVector2D(Other.EndPos), resut))
		{
			IntersectPoint = FVector(resut, 0.0);
			return true;
		}
		return false;
	}
};
void FArmyBaseSurfaceController::CalculateCurrentEditSurfaceOutLineInfo()
{
	if (CurrentEditSurface->SurfaceType == 0 || CurrentEditSurface->SurfaceType == 2)
	{
		TArray<TWeakPtr<FArmyObject>> TotalBeamOrPillars;
		FArmySceneData::Get()->GetObjects(E_HomeModel, OT_Punch, TotalBeamOrPillars);

		TArray<FVector> InOutVertices;
		TArray<FVector> OrginalVertices = CurrentEditSurface->GetOutArea()->Vertices;
		for (FVector& iter : OrginalVertices)
		{
			TransPlaneToWorld(iter);
		}
		int number = OrginalVertices.Num();
		InnearSurfaceBoundary = OrginalVertices;
		if (!FArmyMath::IsClockWise(OrginalVertices))
			FArmyMath::ReversePointList(OrginalVertices);
		TArray<TempEdge> NewTotalEdges;
		for (int i = 0; i < number; ++i)
		{
			FVector start = OrginalVertices[i%number];
			FVector end = OrginalVertices[(i + 1) % number];
			FVector third = OrginalVertices[(i + 2) % number];
			FVector fourth = OrginalVertices[(i + 3) % number];
			float width = FindWallWidth(CurrentSufaceHardWare, start, end);
			FVector dir = (end - start).GetSafeNormal();
			FVector extrusDir = dir.RotateAngleAxis(-90, FVector(0, 0, 1));
			start += extrusDir * width;
			end += extrusDir * width;
			TempEdge temp(start, end);
			NewTotalEdges.Push(temp);
		}
		number = NewTotalEdges.Num();
		for (int i = 0; i < number; ++i)
		{
			const TempEdge& edge = NewTotalEdges[i%number];
			const TempEdge& edge0 = NewTotalEdges[(i + 1) % number];
			FVector newPos;
			if (edge.Intersect(edge0, newPos))
				InOutVertices.Push(newPos);
		}

		TArray<TArray<FVector>> currentRoomHoles;
		if (CurrentEditSurface->SurfaceType == 2)
		{
			TArray<TArray<FVector>> tempInterPuchs;
			for (TWeakPtr<FArmyObject> iter : TotalBeamOrPillars)
			{
				TSharedPtr<FArmyPunch> tempPunch = StaticCastSharedPtr<FArmyPunch>(iter.Pin());
				if ((tempPunch->GetHeight() + tempPunch->GetHeightToFloor()) > FArmySceneData::WallHeight)
				{
					const TArray<FVector>& outverts = tempPunch->GetClipingBox();

					if (FArmyMath::ArePolysOverlap(OrginalVertices, outverts))
					{
						tempInterPuchs.Push(outverts);
					}
				}
			}
			TArray<FVector> results;
			if (Translate::CombineArea(OrginalVertices, tempInterPuchs, results))
				currentRoomHoles.Emplace(results);
		}
		else if (CurrentEditSurface->SurfaceType == 0)
		{
			currentRoomHoles.Emplace(OrginalVertices);
		}
		TriangleArea(InOutVertices, currentRoomHoles);

	}

}

void FArmyBaseSurfaceController::DrawRoofFallOffRegion(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	PDI->SetHitProxy(new HHitProxy());
	FDynamicMeshBuilder MeshBuilder;
	int number = PillarBeamAddWallVerts.Num();
	for (int i = 0; i < number; i++)
	{
		MeshBuilder.AddVertex(PillarBeamAddWallVerts[i]);
		if (i % 3 == 0)
		{
			int index = i / 3;
			MeshBuilder.AddTriangle(index * 3, index * 3 + 1, index * 3 + 2);
		}
	}
	MeshBuilder.Draw(PDI, FMatrix::Identity, FArmyEngineModule::Get().GetEngineResource()->GetDefaultWallFillMat()->GetRenderProxy(false), 0, true);
	PDI->SetHitProxy(NULL);

	if (RoofFallOffRegionVerts.Num() == 4)
	{
		FVector normal = CurrentEditSurface->GetPlaneNormal();
		FVector newStart = RoofFallOffRegionVerts[0] + normal * 10.0f;
		FVector newEnd = RoofFallOffRegionVerts[3] + normal * 10.0f;
		PDI->DrawLine(newStart, newEnd, FLinearColor::Red, 1);
	}
	DrawWallAttachPillarOrBeamOrAddWall(View, PDI);
}

void FArmyBaseSurfaceController::DrawWallAttachPillarOrBeamOrAddWall(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	int number = SurfaceHolesInfos.Num();
	for (int i = 0; i < number; ++i)
	{
		FArmyRoomSpaceArea::SurfaceHoleInfo& tempInfo = SurfaceHolesInfos[i];
		int number0 = tempInfo.surfaceVertices.Num();
		for (int j = 0; j < number0; ++j)
		{
			FVector startPos = tempInfo.surfaceVertices[j%number0].X * CurrentEditSurface->GetXDir() + tempInfo.surfaceVertices[j % number0].Y * CurrentEditSurface->GetYDir() + CurrentEditSurface->GetPlaneCenter() + CurrentEditSurface->GetPlaneNormal() * 10.0f;
			FVector endPos = tempInfo.surfaceVertices[(j + 1) % number0].X * CurrentEditSurface->GetXDir() + tempInfo.surfaceVertices[(j + 1) % number0].Y * CurrentEditSurface->GetYDir() + CurrentEditSurface->GetPlaneCenter() + CurrentEditSurface->GetPlaneNormal() * 10.0f;
			DrawDashedLine(PDI, startPos, endPos, FLinearColor::Black, 2.0f, 1, 0);
		}
	}
}

TArray<TSharedPtr<AreaCompare>> FArmyBaseSurfaceController::CombinePaths(Paths InPath)
{
	TArray<AreaCompare> tempAreas;
	for (int i = 0; i < InPath.size(); ++i)
	{
		Path tempPath = InPath[i];
		TArray<FVector> tempOutVertices;
		for (int j = 0; j < tempPath.size(); ++j)
		{
			IntPoint point = tempPath[j];
			tempOutVertices.Push(FVector(point.X / 10000.0f, point.Y / 10000.0f, 0.0f));
		}
		tempAreas.Push(AreaCompare(tempOutVertices));
	}
	tempAreas.Sort();
	TArray<TSharedPtr<AreaCompare>> OrginalAreas;
	for (int i = 0; i < tempAreas.Num(); ++i)
	{
		OrginalAreas.Push(MakeShareable(new AreaCompare(tempAreas[i].M_Area)));
	}
	TArray<TSharedPtr<AreaCompare>> Results;
	while (OrginalAreas.Num() > 0)
	{

		TSharedPtr<AreaCompare> start = OrginalAreas[0];

		OrginalAreas.Remove(start);
		TArray<TSharedPtr<AreaCompare>> RemoveAreas;
		for (int i = 0; i < OrginalAreas.Num(); ++i)
		{
			if (start->IsContainOther(OrginalAreas[i]))
			{
				RemoveAreas.Push(OrginalAreas[i]);
			}
		}
		for (int i = 0; i < RemoveAreas.Num(); ++i)
		{
			OrginalAreas.Remove(RemoveAreas[i]);
		}
		Results.Push(start);

	}

	return Results;
}

float FArmyBaseSurfaceController::FindWallWidth(TArray<TSharedPtr<FArmyHardware>>& AllRoomHardWares, const FVector& start, const FVector& end)
{
	for (TSharedPtr<FArmyHardware>& iter : AllRoomHardWares)
	{
		FVector  pos = iter->GetPos();
		FVector dir = (iter->GetEndPos() - iter->GetStartPos()).GetSafeNormal();
		FVector InDir = (end - start).GetSafeNormal();
		float dotRes = FMath::Abs(FVector::DotProduct(dir, InDir));
		float dist = FArmyMath::CalcPointToLineDistance(pos, start, end);
		if (FMath::IsNearlyEqual(dist, iter->GetWidth() / 2.0f, 0.1f) && FMath::IsNearlyEqual(dotRes, 1.0f, 0.01f))
			return dist * 2;

	}
	return 24.0f;
}



void FArmyBaseSurfaceController::TriangleArea(const TArray<FVector>& OutAreas, const TArray<TArray<FVector>>& AreaHoles)
{
	TArray<FVector2D> points;
	Translate::ClipperIntersecionAndTriangle(OutAreas, AreaHoles, points);
	FVector Tangent = CurrentEditSurface->GetXDir();
	FVector Normal = FVector::CrossProduct(CurrentEditSurface->GetXDir(), CurrentEditSurface->GetYDir());
	int number0 = points.Num();
	FBox2D box(points);

	for (int index = 0; index < number0; ++index)
	{
		float UCoord, VCoord;
		UCoord = (points[index].X - box.Min.X)*10.0f / 1024;
		VCoord = (points[index].Y - box.Min.Y) * 10.0f / 1024;
		SolidWallArea.Push(FDynamicMeshVertex(FVector(points[index], -1.0), Tangent, Normal, FVector2D(UCoord, VCoord), FColor::Black));
	}
}

void FArmyBaseSurfaceController::DrawSolidWall(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	PDI->SetHitProxy(new HHitProxy());
	int	number = SolidWallArea.Num();
	FDynamicMeshBuilder MeshBuilder;
	for (int i = 0; i < number; i++)
	{
		MeshBuilder.AddVertex(SolidWallArea[i]);
		if (i % 3 == 0)
		{
			int index = i / 3;
			MeshBuilder.AddTriangle(index * 3, index * 3 + 1, index * 3 + 2);
		}

	}
	UMaterialInstance* material = CurrentEditSurface->SurfaceType == 0 ? FArmyEngineModule::Get().GetEngineResource()->GetFloorMaterial() : FArmyEngineModule::Get().GetEngineResource()->GetWallMaterial();

	MeshBuilder.Draw(PDI, FMatrix::Identity, material->GetRenderProxy(false), 0, true);
	int InnearNum = InnearSurfaceBoundary.Num();
	for (int i = 0; i < InnearNum; ++i)
	{
		PDI->DrawLine(InnearSurfaceBoundary[i%InnearNum], InnearSurfaceBoundary[(i + 1) % InnearNum], FLinearColor::Black, 1, 2.0f, 0.0, true);
	}
	int OutNum = OutSurfarceBoundary.Num();
	for (int i = 0; i < OutNum; ++i)
	{
		PDI->DrawLine(OutSurfarceBoundary[i%OutNum], OutSurfarceBoundary[(i + 1) % OutNum], FLinearColor::Black, 1, 2.0f, 0.0, true);
	}

	PDI->SetHitProxy(NULL);
}

bool FArmyBaseSurfaceController::CheckHasSelectedArea()
{
	if (selected.Get() != nullptr)
	{
		return true;
	}
	return false;
}

bool FArmyBaseSurfaceController::MoveDrawArea(TSharedPtr<FArmyObject> InSelected, FTransform trans, int32 TransformFlag)
{
	//必须返回true,否则标尺会移动区域
	if (TestIntersectoin(InSelected, trans, TransformFlag))
	{
		return true;
	}

	FArmyBaseArea* DrawArea = dynamic_cast<FArmyBaseArea*>(InSelected.Get());
	if (!DrawArea || !DrawArea->IsPlotArea())
	{
		return true;
	}

	FArmyRectArea* SelectedRect = dynamic_cast<FArmyRectArea*>(InSelected.Get());
	if (SelectedRect && SelectedRect->DriveByHoleOrBoundary)
	{	
		//@郭子阳
		//单边移动
		FVector tempLeftUp = SelectedRect->GetLeftUpCornerPos();
		float tempAreaWidth = SelectedRect->GetRectWidth();
		float tempAreaHeight = SelectedRect->GetRectHeight();
		FVector delta = trans.GetTranslation();
		if (TransformFlag == 1)
		{
			tempAreaWidth += delta.X;
		}
		else if (TransformFlag == 2)
		{
			tempLeftUp = trans.TransformPosition(SelectedRect->GetLeftUpCornerPos());
			tempAreaWidth -= delta.X;
		}
		else if (TransformFlag == 3)
		{
			tempAreaHeight += delta.Y;
		}
		else if (TransformFlag == 4)
		{
			tempLeftUp = trans.TransformPosition(SelectedRect->GetLeftUpCornerPos());
			tempAreaHeight -= delta.Y;
		}

		SelectedRect->SetRectWidth(tempAreaWidth);
		SelectedRect->SetRectHeight(tempAreaHeight);
		SelectedRect->SetLeftUpCornerPos(tempLeftUp);

		//SelectedRect->Move(tempLeftUp - SelectedRect->GetLeftUpCornerPos());
	}
	else
	{
		DrawArea->Move(trans.GetLocation());
	}

    // @欧石楠 移动区域时也需要同步移动起铺点
    if (SelectedRect)
    {
        SelectedRect->OffsetLayingPointPos(trans.GetLocation());
    }

    // @欧石楠 高亮选择面也需要移动
    HightLightActor->ResetMeshTriangles();
    HightLightActor->AddVetries(selected->TempHighLightVertices);
    HightLightActor->UpdateAllVetexBufferIndexBuffer();


	//@郭子阳 移动完刷新顶点
	auto Parent=DrawArea->GetParentArea();
	Parent->RefreshPolyVertices();
	//递归地刷新侧面
	std::function<void (TSharedPtr<FArmyBaseArea>)>
		FreshSide = [&FreshSide](TSharedPtr<FArmyBaseArea> Area) {
		for (auto& SubArea : Area->GetEditAreas())
		{
			FreshSide(SubArea);
		}
		Area->RefreshExtrusionPlane();
	};
	FreshSide(Parent);
	//Parent->RefreshExtrusionPlane();
	return true;
}

bool FArmyBaseSurfaceController::TestIntersectoin(TSharedPtr<FArmyObject> InSelected, FTransform trans, int32 TransformFlag)
{
	
	TArray<TWeakPtr<FArmyObject>>parents = InSelected->GetParents();
	if (parents.Num() == 1)
	{
		TSharedPtr<FArmyBaseArea> Parent = StaticCastSharedPtr<FArmyBaseArea>(parents[0].Pin());
		if (InSelected->GetType() == OT_CircleArea)
		{
			TSharedPtr<FArmyCircleArea> Selected = StaticCastSharedPtr<FArmyCircleArea>(InSelected);
			FVector newPosition = trans.TransformPosition(Selected->GetPosition());
			TSharedPtr<FArmyCircleArea> tempCircle = MakeShareable(new FArmyCircleArea());

			tempCircle->SetPosition(newPosition);
			tempCircle->SetRadius(Selected->GetRadius());
			tempCircle->SetUniqueID(Selected->GetUniqueID());

			TArray<FVector> tempOutVertices = tempCircle->GetOutArea()->Vertices;
			auto ID = tempCircle->GetUniqueID();
			//@郭子阳
			//隐藏这个临时面
			tempCircle->GetWallActor()->SetActorHiddenInGame(true);
			//@郭子阳
			//这个临时面已经无用了，需要立即销毁
			tempCircle->Destroy();
			tempCircle = nullptr;

			/** @梁晓菲 判断灯槽是否存在，如果存在，需要用灯槽边界点确认是否可以添加在新位置*/
            if (!Selected->GetLampSlotExtruder()->IsEmpty())
            {
				tempOutVertices = FArmyMath::ExturdePolygon(tempOutVertices, (Selected->GetLampSlotWidth() + 0.2f), false);
            }

			if (Parent->TestAreaCanAddOrNot(tempOutVertices, ID))
			{
				if (CurrentOperation.IsValid())
				{
					CurrentOperation->EndOperation();
					CurrentOperation = NULL;
				}
				return false;
			}
			else
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("同其他区域相交"));
				return true;
			}
		}
		else if (InSelected->GetType() == OT_RectArea)
		{
			TSharedPtr<FArmyRectArea> Selected = StaticCastSharedPtr<FArmyRectArea>(InSelected);
			FVector tempLeftUp = Selected->GetLeftUpCornerPos();
			float tempAreaWidth = Selected->GetRectWidth();
			float tempAreaHeight = Selected->GetRectHeight();
			if (!Selected->DriveByHoleOrBoundary)
				tempLeftUp = trans.TransformPosition(Selected->GetLeftUpCornerPos());
			else
			{
				FVector delta = trans.GetTranslation();
				if (TransformFlag == 1)
				{
					tempAreaWidth += delta.X;
				}
				else if (TransformFlag == 2)
				{
					tempLeftUp = trans.TransformPosition(Selected->GetLeftUpCornerPos());
					tempAreaWidth -= delta.X;
				}
				else if (TransformFlag == 3)
				{
					tempAreaHeight += delta.Y;
				}
				else if (TransformFlag == 4)
				{
					tempLeftUp = trans.TransformPosition(Selected->GetLeftUpCornerPos());
					tempAreaHeight -= delta.Y;
				}
			}
			TSharedPtr<FArmyRectArea> tempRectArea = MakeShareable(new FArmyRectArea());


			tempRectArea->SetRectWidth(tempAreaWidth);
			tempRectArea->SetRectHeight(tempAreaHeight);
			tempRectArea->SetLeftUpCornerPos(tempLeftUp);
			tempRectArea->SetUniqueID(Selected->GetUniqueID());

			TArray<FVector> tempOutVertices = tempRectArea->GetOutArea()->Vertices;
			auto ID = tempRectArea->GetUniqueID();
			//@郭子阳
			//隐藏这个临时面
			tempRectArea->GetWallActor()->SetActorHiddenInGame(true);

			//@郭子阳
			//这个临时面已经无用了，需要立即销毁
			tempRectArea->Destroy();
			tempRectArea = nullptr;

			/** @梁晓菲 判断灯槽是否存在，如果存在，需要用灯槽边界点确认是否可以添加在新位置*/
            if (!Selected->GetLampSlotExtruder()->IsEmpty())
            {
				tempOutVertices = FArmyMath::ExturdePolygon(tempOutVertices, (Selected->GetLampSlotWidth() + 0.2f), false);
            }

			if (Parent->TestAreaCanAddOrNot(tempOutVertices, ID))
			{
				if (CurrentOperation.IsValid())
				{
					CurrentOperation->EndOperation();
					CurrentOperation = NULL;
				}
				return false;
			}
			else
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("同其他区域相交"));
				return true;
			}
		}
		else if (InSelected->GetType() == OT_PolygonArea)
		{
			TSharedPtr<FArmyRegularPolygonArea> Selected = StaticCastSharedPtr<FArmyRegularPolygonArea>(InSelected);
			TSharedPtr<FArmyRegularPolygonArea> tempPolygon = MakeShareable(new FArmyRegularPolygonArea());
			FVector newPosition = trans.TransformPosition(Selected->GetPosition());
			FVector tempStartPoint = trans.TransformPosition(Selected->GetStartPoint());
			tempPolygon->SetStartPoint(tempStartPoint);
			tempPolygon->SetPosition(newPosition);
			tempPolygon->SetRadius(Selected->GetRadius());
			tempPolygon->SetUniqueID(Selected->GetUniqueID());

			TArray<FVector> tempOutVertices = tempPolygon->GetOutArea()->Vertices;
			auto ID = tempPolygon->GetUniqueID();
			//@郭子阳
			//隐藏这个临时面
			tempPolygon->GetWallActor()->SetActorHiddenInGame(true);
			//这个临时面已经无用了，需要立即销毁
			tempPolygon->Destroy();
			tempPolygon = nullptr;

			/** @梁晓菲 判断灯槽是否存在，如果存在，需要用灯槽边界点确认是否可以添加在新位置*/
            if (!Selected->GetLampSlotExtruder()->IsEmpty())
            {
				tempOutVertices = FArmyMath::ExturdePolygon(tempOutVertices, (Selected->GetLampSlotWidth() + 0.2f), false);
            }

			if (Parent->TestAreaCanAddOrNot(tempOutVertices, ID))
			{
				if (CurrentOperation.IsValid())
				{
					CurrentOperation->EndOperation();
					CurrentOperation = NULL;
				}
				return false;
			}
			else
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("同其他区域相交"));
				return true;
			}
		}
		else if (InSelected->GetType() == OT_FreePolygonArea)
		{
			/* @梁晓菲 自由绘制区域通过四轴坐标移动离墙距离时判断相交*/
			TSharedPtr<FArmyFreePolygonArea> Selected = StaticCastSharedPtr<FArmyFreePolygonArea>(InSelected);
			TArray<FVector> TempVertices;
			for (int32 i = 0; i < Selected->GetOutArea()->Vertices.Num(); i++)
			{
				TempVertices.Add(trans.TransformPosition(Selected->GetOutArea()->Vertices[i]));
			}

			/** @梁晓菲 判断灯槽是否存在，如果存在，需要用灯槽边界点确认是否可以添加在新位置*/
			TArray<FVector> tempOutVertices = TempVertices;
            if (!Selected->GetLampSlotExtruder()->IsEmpty())
            {
				tempOutVertices = FArmyMath::ExturdePolygon(tempOutVertices, (Selected->GetLampSlotWidth() + 0.2f), false);
            }

			if (Parent->TestAreaCanAddOrNot(tempOutVertices, Selected->GetUniqueID()))
			{
				if (CurrentOperation.IsValid())
				{
					CurrentOperation->EndOperation();
					CurrentOperation = NULL;
				}
				return false;
			}
			else
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("同其他区域相交"));
				return true;
			}
		}
	}
    else
    {
        if (LayingPoint->GetState() == OS_Selected)
        {
            FVector NewPos = trans.TransformPosition(LayingPoint->GetPos());

            FVector2D CapturedPos = FVector2D::ZeroVector;
            FArmyToolsModule::Get().GetMouseCaptureTool()->SetCurrentPoint(FVector2D(NewPos));
            if (CapturePoint(CapturedPos))
            {
                selected->SetLayingPointPos(FVector(CapturedPos.X, CapturedPos.Y, 0.f));
                LayingPoint->SetCaptured(true);
            }
            else
            {
                selected->SetLayingPointPos(NewPos);
                LayingPoint->SetCaptured(false);
            }

            selected->GetStyle()->CalculateClipperBricks();
            selected->UpdateWallActor();
        }
    }

	return false;
}

void FArmyBaseSurfaceController::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
    LayingPoint->Draw(PDI, View);
	AxisRuler->Draw(PDI, View);

	FArmyDesignModeController::Draw(View, PDI);

    DrawRoofFallOffRegion(View, PDI);

	if (CurrentEditSurface->SurfaceType == 1 && CurrentEditSurface->GenerateFromObjectType != OT_AddWall)
	{
		CurrentEditSurface->DrawOutLine(PDI, View, FLinearColor(FColor(0XFFCCCCCC)), 3.0f);
	}

	if (bHoverRectEdge)
	{
		PDI->DrawLine(HoverEdgeStart, HoverEdgeEnd, FLinearColor(FColor(0XFFFF9800)), 1, 3.0f, 0.0, true);
	}
	if (CurrentEditSurface->SurfaceType == 0 || CurrentEditSurface->SurfaceType == 2)
	{
		for (TSharedPtr<FArmyHardware> iter : CurrentSufaceHardWare)
		{
			iter->Draw(PDI, View);
		}
		for (TWeakPtr<FArmyObject> iter : CurrentSurfaceSimpleComponent)
		{
			iter.Pin()->Draw(PDI, View);
		}
		if (CurrentEditSurface->GenerateFromObjectType == OT_InternalRoom)
			DrawSolidWall(PDI, View);

		int number = TestInterLines.Num();
		for (int i = 0; i < number; i += 2)
		{
			PDI->DrawLine(TestInterLines[i], TestInterLines[i + 1], FColor::Red, 1, 1.0f);
		}
	}

	//@郭子阳
	//递归地绘制子区域的线框
	std::function<void(TSharedPtr<FArmyBaseArea>)>
	DrawAreaWireFrame = [&DrawAreaWireFrame,&PDI,&View](TSharedPtr<FArmyBaseArea> InArea) {
		InArea->GetStyle()->DrawWireFrame(PDI, View);
		for (auto & SubArea: InArea->GetEditAreas())
		{
			DrawAreaWireFrame(SubArea);
		}
	};

    // @欧石楠 在子模式中绘制瓷砖地板的线框，防止在顶视图模式下缝隙会显示不全的问题
    if (CurrentEditSurface->GetStyle()->HasGoodID())
    {
        DrawAreaWireFrame(StaticCastSharedPtr<FArmyBaseArea>(CurrentEditSurface));
    }

	//@郭子阳
	//绘制所有绘制区域的边框
	for (TSharedPtr<FArmyBaseArea>& iter : CurrentEditSurface->GetEditAreas())
	{	
		iter->DrawOutLineRecursion(PDI, View, FLinearColor(FColor(0XFF666666)), FLinearColor(FColor(0xFF,0xF7,0,255)), 2.0f);
	}

}

void FArmyBaseSurfaceController::EndOperation()
{
	if (CurrentOperation.IsValid() && CurrentOperation == OperationMap.FindRef((uint8)EHardModeOperation::HO_Replace)
		|| CurrentOperation.IsValid() && CurrentOperation == OperationMap.FindRef((uint8)EHardModeOperation::HO_MaterialBrush)
		)
	{
		CurrentOperation->EndOperation();
	}

	FArmyDesignModeController::EndOperation();
	FArmyToolsModule::Get().GetRectSelectTool()->Start();
	AddAreaState = 0;
}
// 设置当前编辑的子平面区域
void FArmyBaseSurfaceController::SetCurrentEditSurface(TSharedPtr<class FArmyRoomSpaceArea> InSurface)
{
    if (!InSurface.IsValid())
    {
		return;
    }

	SufaceDetail->SetEditAreaVisibility(EVisibility::Collapsed);
	GVC->ViewportOverlayWidget->AddSlot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Top)
		.Padding(0, 4, 4, 0)
		[
			ReturnWidget.ToSharedRef()
		];
	CurrentEditSurface = InSurface;

    RoofFallOffRegionVerts.Empty();
    PillarBeamAddWallVerts.Empty();
    SurfaceHolesInfos.Empty();
    SolidWallArea.Empty();
    CurrentSufaceHardWare.Empty();
    CurrentSurfaceSimpleComponent.Empty();

    // @欧石楠 计算墙面、地面、顶面的造型在彼此的投影区域
    if (CurrentEditSurface->SurfaceType == 0)
    {
        CalculateFloorProjectionAreas();
    }
    else if (CurrentEditSurface->SurfaceType == 1)
    {
        CalculateWallProjectionAreas();
    }
    else if (CurrentEditSurface->SurfaceType == 2)
    {
        CalculateRoofProjectionAreas();
    }
    
	const FVector& planeNormal = CurrentEditSurface->GetPlaneNormal();
	const FVector& basePoint = CurrentEditSurface->GetPlaneCenter() + 2.0f * planeNormal;
	SufaceDetail->SetSelectObject(CurrentEditSurface);
	AxisRuler->SetRefCoordinateSystem(basePoint, CurrentEditSurface->GetXDir(), CurrentEditSurface->GetYDir(), planeNormal);
}

void FArmyBaseSurfaceController::Tick(float DeltaSeconds)
{
	if (!CurrentEditSurface.IsValid() )
	{
		FArmyDesignModeController::Tick(DeltaSeconds);
		return;
	}

	TWeakPtr<FArmyRoomSpaceArea> room = CurrentEditSurface.ToSharedRef();
	TArray<TSharedPtr<FArmyBaseArea>> tempAreas = room.Pin()->GetEditAreas();
	TArray<TWeakPtr<FArmyObject>> objects;
	objects.Push(room);
	int number = tempAreas.Num();

	for (int i = 0; i < number; i++)
	{
		TWeakPtr<FArmyBaseArea> temp = tempAreas[i].ToSharedRef();
		objects.Push(temp);
	}

	if (selected.IsValid())
	{
		auto CircleArea = dynamic_cast<FArmyCircleArea*>(selected->GetParentArea().Get());
		if(CircleArea)
		{
			AxisRuler->SetBound(CircleArea->GetPosition(), CircleArea->GetRadius());
		}
		else
		{
			AxisRuler->DisableCircleBound();

            TArray<FVector> Vertices;
            if (LayingPoint->GetState() == OS_Selected)
            {
                selected->GetVertexes(Vertices);
            }
            else
            {
                if (selected->IsPlotArea())
                {
                    auto Parent = selected->GetParentArea();

                    //退出程序时可能导致父面丢失
                    if (Parent.IsValid() && Parent->IsPlotArea())
                    {
                        Parent->GetVertexes(Vertices);
                    }
                }
            }
			
			AxisRuler->SetBound(Vertices);
		}
	}
	else
	{
		TArray<FVector> Vertices;
		AxisRuler->SetBound(Vertices);
	}

	AxisRuler->Update(GVC, objects);
	const bool MiddleMouseButtonDown = GVC->Viewport->KeyState(EKeys::MiddleMouseButton);
	const bool LeftMouse = GVC->Viewport->KeyState(EKeys::LeftMouseButton);
	GVC->GetMousePosition(CurrentMouseClick);
	FArmyDesignModeController::Tick(DeltaSeconds);
	if (ShouldTick())
	{
		if (MiddleMouseButtonDown)
		{
			WorldEndPos = RayIntersectionWithCurrentSurface(CurrentMouseClick);
			FVector worldDragDelta = WorldStartPos - WorldEndPos;
			FVector InDragDelta;
			FRotator InRotDelat;
			GVC->ConvertMovementToDragRot(worldDragDelta, InDragDelta, InRotDelat);

			GVC->MoveViewportCamera(InDragDelta, InRotDelat, false);
		}

        // @欧石楠 移动起铺点
        if (bMovingLayingPoint)
        {
            FVector2D CapturedPos = FVector2D::ZeroVector;
            if (CapturePoint(CapturedPos))
            {
                LayingPoint->SetCaptured(true);
            }
            else
            {
                LayingPoint->SetCaptured(false);
            }
            LayingPoint->SetPos(FVector(CapturedPos.X, CapturedPos.Y, 0.f));
        }
	}

    // @欧石楠 更新起铺点位置，移动了子绘制区域导致的起铺点位置变化
    LayingPoint->Update();
}

TSharedPtr<SWidget> FArmyBaseSurfaceController::MakeCustomTitleBarWidget()
{
	return
		SNew(SBox)
		.HeightOverride(40);
}

void FArmyBaseSurfaceController::BeginMode()
{
	FArmyDesignModeController::BeginMode();

	TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = FArmyUser::Get().CurProjectData->GetResObjNoComponent();
	if (resArr.Num() < 1)
		return;
	//获取项目选择的套餐，搜索默认选择项目套餐
	TSharedPtr<FContentItemSpace::FProjectRes> ProRes = StaticCastSharedPtr<FContentItemSpace::FProjectRes>(resArr[0]);

	TSharedPtr<FArmyKeyValue> defaultPackage = MakeShareable(new FArmyKeyValue(ProRes->SetMealId, ProRes->SetMealName));
	LeftPanel->SetDefaultPackage(defaultPackage);
	LeftPanel->RequestPackage(0);

	//@梁晓菲 进入子模式取消选中该面
	GXREditor->SelectNone(true, true, false);

	//设置此模块默认显示的目录数据
	CategoryOrgManageSurface->SetDefaultSelectOrgination(SArmyCategoryOrganization::OZ_CompanyData);

	//@郭子阳 自动进入材质刷操作
	TSharedPtr<FArmyMaterialBrushOperation>	MatBrush = StaticCastSharedPtr<FArmyMaterialBrushOperation>(*OperationMap.Find((uint8)EHardModeOperation::HO_MaterialBrush));
	if (MatBrush->GetRebeginAtStart())
	{
		//EndOperation();
		SetOperation((uint8)EHardModeOperation::HO_MaterialBrush);
	}

}

bool FArmyBaseSurfaceController::EndMode()
{
	TSharedPtr<FArmyReplaceTextureOperation>	tempOperation = StaticCastSharedPtr<FArmyReplaceTextureOperation>(CurrentOperation);

	//@郭子阳 退出子模式后，立面模式立即进入材质刷操作
	if ((CurrentOperation.IsValid() && CurrentOperation == OperationMap.FindRef((uint8)EHardModeOperation::HO_Replace)
		&& FormerOperation.IsValid() && FormerOperation == OperationMap.FindRef((uint8)EHardModeOperation::HO_MaterialBrush))
		|| CurrentOperation.IsValid() && CurrentOperation == OperationMap.FindRef((uint8)EHardModeOperation::HO_MaterialBrush)
		)
	{
		TSharedPtr<FArmyMaterialBrushOperation>	MatBrush = StaticCastSharedPtr<FArmyMaterialBrushOperation>(*OperationMap.Find((uint8)EHardModeOperation::HO_MaterialBrush));
		MatBrush->SetRebeginAtStart();
	}


	if (CurrentOperation.IsValid())
	{
		CurrentOperation->EndOperation();
	}

	GVC->ViewportOverlayWidget->RemoveSlot(ReturnWidget.ToSharedRef());
	AxisRuler->SetBoundObj(NULL);
	//返回时隐藏此模态对话框
	GGI->Window->DismissModalViewController();
	GVC->LockViewPortClient(false);
	FreeViewModeCallback.ExecuteIfBound();
	HightLightActor->SetActorHiddenInGame(true);

    // @欧石楠 退出子模式时需要隐藏起铺点
    if (CurrentEditSurface.IsValid())
    {
        LayingPoint->Show(false);
    }
    if (selected.IsValid())
    {
        LayingPoint->Show(false);
    }

	FArmyDesignModeController::EndMode();

	return true;
}

void FArmyBaseSurfaceController::Quit()
{
    OnReturnViewAngleCommand();
}

FReply FArmyBaseSurfaceController::OnReturnViewAngleCommand()
{

	TSharedPtr<FArmyHardModeController> TempController = StaticCastSharedPtr<FArmyHardModeController>(GGI->DesignEditor->CurrentController);
	if (TempController.IsValid())
	{
		if (TempController->HardModeDetail.IsValid())
		{
			TempController->HardModeDetail->SetPropertyPanelVisibility(EVisibility::Collapsed);
			TempController->HardModeDetail->ExeBeforeClearDetail();
		

		}
	}
	if (SufaceDetail.IsValid())
	{
		SufaceDetail->ExeBeforeClearDetail();
	}
	EndMode();

	auto MatBrush = StaticCastSharedPtr<FArmyMaterialBrushOperation> (OperationMap.FindRef((uint8)EHardModeOperation::HO_MaterialBrush));
	if (MatBrush->GetRebeginAtStart())
	{
		TempController->OnMaterialBrushCommand();
	}

	if (GXRPC) {
		GXRPC->SetXRViewMode(EXRView_FPS);
	}
	// @梁晓菲 退出子模式显示软装和木作物体,锁定外墙
	TSharedPtr<IArmyActorVisitor> OuterRoomVisitor = FArmyActorVisitorFactory::Get().CreateVisitor(AV_OuterWall);
	OuterRoomVisitor->Lock(true);
	TSharedPtr<IArmyActorVisitor> HardSubModeAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_HardSubMode);
	HardSubModeAV->Show(false);
	TSharedPtr<IArmyActorVisitor> HardModeAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_HardMode);
	HardModeAV->Show(true);

	return FReply::Handled();
}

void FArmyBaseSurfaceController::SetModifyAreaOperation(FViewport* ViewPort, FKey Key, EInputEvent Event)
{
	if (Key == EKeys::LeftMouseButton)
	{
		TSharedPtr<FArmyOperation> SelcetedOperation;
		if (selected.IsValid())
		{
			if (selected->GetType() == OT_RectArea)
			{
				SelcetedOperation = OperationMap.FindRef((uint8)EHardModeOperation::HO_DrawRect);
			}
			else if (selected->GetType() == OT_FreePolygonArea)
			{
				SelcetedOperation = OperationMap.FindRef((uint8)EHardModeOperation::HO_DrawFreePolygon);
			}
			else if (selected->GetType() == OT_CircleArea)
			{
				SelcetedOperation = OperationMap.FindRef((uint8)EHardModeOperation::HO_DrawCircle);
			}
			else if (selected->GetType() == OT_PolygonArea)
			{
				SelcetedOperation = OperationMap.FindRef((uint8)EHardModeOperation::HO_DrawPolygon);
			}
		}
		//if (!CurrentOperation.IsValid() || CurrentOperation != SelcetedOperation || selectedOld != selected)
		//{
			if (selected.IsValid())
			{
				selectedOld = selected;
				if (SufaceDetail.IsValid())
				{
					SufaceDetail->SetSelectObject(selected);
				}
				HightLightActor->ResetMeshTriangles();
				HightLightActor->AddVetries(selected->TempHighLightVertices);
				HightLightActor->UpdateAllVetexBufferIndexBuffer();
				HightLightActor->SetActorHiddenInGame(false);
				UMaterialInstanceDynamic* material = Cast<UMaterialInstanceDynamic>(HightLightActor->GetMeshComponent()->GetMaterial(0));
				if (material)
				{
					material->SetScalarParameterValue("Alpha", 0.1f);
				}
				GXREditor->SelectActor(HightLightActor, true, false);
				if (selected->GetType() != OT_RoomSpaceArea)
				{
					AxisRuler->SetBoundObj(selected);
				}
				else
				{
					AxisRuler->SetBoundObj(NULL);
					if (CurrentOperation.IsValid())
					{
						CurrentOperation->EndOperation();
					}
				}
			}
			else
			{
				AxisRuler->SetBoundObj(NULL);
				if (CurrentOperation.IsValid() && !CurrentOperation->GetMoveOperating())
				{
					CurrentOperation->EndOperation();
				}
				//选择集为空时隐藏属性面板
				if (HightLightActor->IsValidLowLevel())
					HightLightActor->SetActorHiddenInGame(true);
				SufaceDetail->SetPropertyPanelVisibility(EVisibility::Hidden);
			}

			SetCurrentOperation(GVC->GetCurrentMousePos(), selected);
		//}
	}
}

void FArmyBaseSurfaceController::StartTrackingDueToInput(const FArmyInputEventState& InPutState)
{
	GVC->GetMousePosition(LastMouseClick);
	WorldStartPos = RayIntersectionWithCurrentSurface(LastMouseClick);
	WorldEndPos = RayIntersectionWithCurrentSurface(LastMouseClick);
    IsTracking = true;

    // @欧石楠 是否在拖拽移动起铺点
    if (selected.IsValid() && LayingPoint->GetState() == OS_Selected && IsLayingPointSelected())
    {
        bMovingLayingPoint = true;
        FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = true;
    }
    else if (CurrentOperation.IsValid())
	{
		CurrentOperation->StartTrackingDueToInput(InPutState);
	}
}

void FArmyBaseSurfaceController::StopTracking()
{
	//有没有起铺点 @郭子阳
	auto HasLayingPoint = [](TSharedPtr<FArmyBaseArea> InArea)->bool {
		if (!InArea.IsValid() 
			|| !InArea->GetMatStyle().IsValid())
		{
			return false;
		}

		auto style = InArea->GetMatStyle()->GetEditType();

		if (style == OT_StyleTypeNone 
			||style == S_BodaAreaStyle 
			||style == S_SeamlessStyle)
		{
			return false;
		}
		
		return true;
	};

	if (IsTracking)
	{
		IsTracking = false;
        bMovingLayingPoint = false;
        
        if (CurrentOperation.IsValid())
        {
            CurrentOperation->StopTracking();
        }
        else
        {
            FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = false;
            if (selected.IsValid())
            {
                LayingPoint->SetCaptured(false);
            }
        }

		float mouseDelta = (CurrentMouseClick - LastMouseClick).Size();
		if (mouseDelta < 1000.0f && mouseDelta > 0.0f)
		{
			/* @梁晓菲 停止拖动的时候去掉高亮区域*/
			if (HightLightActor)
			{
				HightLightActor->SetActorHiddenInGame(true);
			}

            // @欧石楠 拖拽完起铺点需要重新计算和更新砖块
			// @郭子阳 有起铺点才刷新
            if (selected.IsValid() && HasLayingPoint(selected) )
            {
                selected->GetStyle()->CalculateClipperBricks();
                selected->UpdateWallActor();
            }
		}
	}
}

TArray<TSharedPtr<FArmyBaseArea>> FArmyBaseSurfaceController::PickOperation(const FVector& pos, class UArmyEditorViewportClient* InViewportClient)
{
	TArray< TSharedPtr<FArmyBaseArea> > SelectedObjs;
	if (CurrentEditSurface.IsValid())
	{
		float dist;

		CurrentEditSurface->SetStateRecursively(OS_Normal);

		FVector LocalPositon= FArmyOperation::GetIntersectPointWithPlane(FVector2D(pos), CurrentEditSurface);
		//TSharedPtr<FArmyBaseArea> selectedArea=CurrentEditSurface->FindArea(LocalPositon);
		TSharedPtr<FArmyBaseArea> selectedArea;
		if (bHoverRectEdge &&HoveredArea.IsValid())
		{
			selectedArea = HoveredArea.Pin();
		}
		else
		{
			selectedArea = CurrentEditSurface->FindArea(LocalPositon);
		}
		
		
		if (selectedArea.IsValid())
		{
			selectedArea->SetState(OS_Selected);
			SelectedObjs.Push(selectedArea);
		}
		

		if (CurrentEditSurface->IsSelected(pos, InViewportClient, dist))
        {
            CurrentEditSurface->SetState(OS_Selected);
			SelectedObjs.Push(CurrentEditSurface);
        }
        else
        {
            CurrentEditSurface->SetState(OS_Normal);
        }
	}
	return SelectedObjs;
}

bool FArmyBaseSurfaceController::ShouldTick()
{
	GVC->GetViewportSize(ViewportSize);
	GVC->GetMousePosition(ViewportMousePos);
	if (ViewportSize.X == 0 || ViewportSize.Y == 0)
	{
		return false;
	}

	if (LastViewportMousePos == ViewportMousePos || ViewportMousePos < -ViewportSize)
	{
		return false;
	}

	LastViewportMousePos = ViewportMousePos;

	return true;
}

FVector FArmyBaseSurfaceController::RayIntersectionWithCurrentSurface(const FVector2D& InPos)
{
	FVector MWorldStart, MWorldDir;

	GVC->DeprojectFVector2D(FVector2D(InPos.X, InPos.Y), MWorldStart, MWorldDir);

	FPlane BasePlane(CurrentEditSurface->GetPlaneCenter() + CurrentEditSurface->GetPlaneNormal()*CurrentEditSurface->GetExtrusionHeight(), CurrentEditSurface->GetPlaneNormal());
	FVector MouseWorldPoint = FMath::LinePlaneIntersection(MWorldStart, MWorldStart + MWorldDir * ((GVC->GetViewLocation() - FVector::PointPlaneProject(GVC->GetViewLocation(), BasePlane)).Size() + 100000), BasePlane);
	return MouseWorldPoint;
}

bool FArmyBaseSurfaceController::IsLayingPointSelected()
{
    FVector4 WorldMousePos = GVC->PixelToWorld(CurrentMouseClick.X, CurrentMouseClick.Y, 0.f);
    return selected.IsValid() &&
        selected->GetMatStyle()->HasGoodID() &&
        LayingPoint->IsSelected(FVector(WorldMousePos), GVC);
}

bool FArmyBaseSurfaceController::CapturePoint(FVector2D& OutCapturedPos)
{
    bool bResult = false;

    if (FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture)
    {
        TArray<TSharedPtr<FArmyLine>> Lines;
        TArray<TSharedPtr<FArmyCircle>> Circles;
        TArray<TSharedPtr<FArmyPoint>> Points;
        CollectAllLinesAndPoints(Lines, Points, Circles);
        bResult = FArmyToolsModule::Get().GetMouseCaptureTool()->Capture(GVC, CurrentMouseClick.X, CurrentMouseClick.Y, Lines, Points);
        OutCapturedPos = FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint();
    }

    return bResult;
}

void FArmyBaseSurfaceController::CalculateWallProjectionAreas()
{
    if (CurrentEditSurface->SurfaceType == 1)
    {
        TArray<TWeakPtr<FArmyObject>> AllAddWalls;
        TArray<TWeakPtr<FArmyObject>> AllHalfAddWalls;
        FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_AddWall, AllAddWalls);
        for (TWeakPtr<FArmyObject> iter : AllAddWalls)
        {
            FArmyAddWall* tempAddWall = iter.Pin()->AsassignObj<FArmyAddWall>();
            if (tempAddWall && tempAddWall->GetIsHalfWall())
            {
                AllHalfAddWalls.Emplace(iter);
            }
        }

        TSharedPtr<FArmyRoomSpaceArea> CurrentEditSurfaceRoof = nullptr;
        TArray<TSharedPtr<FArmyRoomSpaceArea>> RoomSurfaces = FArmySceneData::Get()->GetRoomAttachedSurfacesWidthRoomID(CurrentEditSurface->AttachRoomID);
        for (TSharedPtr<FArmyRoomSpaceArea> iter : RoomSurfaces)
        {
            if (iter->SurfaceType == 2)
            {
                CurrentEditSurfaceRoof = iter;
                break;
            }
        }

        TArray<FVector>& tempVertices = CurrentEditSurface->GetOutArea()->Vertices;
        FBox box(tempVertices);
        float width = box.GetSize().X;
        float height = box.GetSize().Y;
        const FVector& planeCenter = CurrentEditSurface->GetPlaneCenter() + CurrentEditSurface->GetPlaneNormal() * 0.f;
        float dropHeight = CurrentEditSurfaceRoof.IsValid() ? CurrentEditSurfaceRoof->GetExtrusionHeight() : 0.f;
        FVector LeftDown = planeCenter - CurrentEditSurface->GetXDir() * width / 2 - CurrentEditSurface->GetYDir() * (height / 2 - dropHeight);
        FVector leftUp = LeftDown - CurrentEditSurface->GetYDir() * dropHeight;
        FVector RightUp = leftUp + width * CurrentEditSurface->GetXDir();
        FVector RightDown = RightUp + CurrentEditSurface->GetYDir() * dropHeight;
        float BaseOffsetGroundHeight = 0.0f;
        if (CurrentEditSurface->GenerateFromObjectType == OT_Beam)
        {
            const FBox Bounds = CurrentEditSurface->GetBounds();
            BaseOffsetGroundHeight = Bounds.Min.Y + planeCenter.Z;
        }

        if (dropHeight != 0)
        {
            RoofFallOffRegionVerts = { LeftDown, leftUp, RightUp, RightDown };
            FArmyRoomSpaceArea::SurfaceHoleInfo fillOffArea;
            fillOffArea.surfaceHoleType = OT_RoomSpaceArea;
            TArray<FVector> tempFillOffVertices;
            for (FVector& iter : RoofFallOffRegionVerts)
            {
                float x = FVector::DotProduct((iter - CurrentEditSurface->GetPlaneCenter()), CurrentEditSurface->GetXDir());
                float y = FVector::DotProduct((iter - CurrentEditSurface->GetPlaneCenter()), CurrentEditSurface->GetYDir());
                tempFillOffVertices.Push(FVector(x, y, 0.0f));
            }
            fillOffArea.surfaceVertices = tempFillOffVertices;
            SurfaceHolesInfos.Push(fillOffArea);
        }

        TArray<TWeakPtr<FArmyObject>> TotalBeamOrPillars;
        FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Beam, TotalBeamOrPillars);
        FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Pillar, TotalBeamOrPillars);
        FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_AirFlue, TotalBeamOrPillars);
        FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_PackPipe, TotalBeamOrPillars);

        TotalBeamOrPillars.Append(AllHalfAddWalls);
		FVector2D WallWorldStartPos = FVector2D(leftUp);
		FVector2D WallWorldEndPos = FVector2D(RightUp);
		for (TWeakPtr<FArmyObject> iter : TotalBeamOrPillars)
		{
			TArray<FVector> vertices;
			float height = FArmySceneData::Get()->WallHeight;
			float OffsetGroundHeight = 0;
			if (iter.Pin()->GetType() == OT_Beam || iter.Pin()->GetType() == OT_Pillar || iter.Pin()->GetType() == OT_PackPipe
				|| iter.Pin()->GetType() == OT_AirFlue || iter.Pin()->GetType() == OT_Punch || iter.Pin()->GetType() == OT_AddWall)
			{
				if (iter.Pin()->GetType() == OT_Punch)
				{
					TSharedPtr<FArmyPunch> tempPunch = StaticCastSharedPtr<FArmyPunch>(iter.Pin());
					vertices = tempPunch->GetClipingBox();
					OffsetGroundHeight = tempPunch->GetHeightToFloor();
					height = tempPunch->GetHeight();
				}
				else if (iter.Pin()->GetType() == OT_Beam)
				{
					TSharedPtr<FArmySimpleComponent> pillar = StaticCastSharedPtr<FArmySimpleComponent>(iter.Pin());
					pillar->GetVertexes(vertices);
					// OffsetGroundHeight = FArmySceneData::Get()->WallHeight - pillar->GetHeight();
					OffsetGroundHeight = FMath::Max(FArmySceneData::Get()->WallHeight - pillar->GetHeight(), BaseOffsetGroundHeight);
					height = pillar->GetHeight();
				}
				else if (iter.Pin()->GetType() == OT_Pillar || iter.Pin()->GetType() == OT_AirFlue)
				{
					TSharedPtr<FArmySimpleComponent> pillar = StaticCastSharedPtr<FArmySimpleComponent>(iter.Pin());
					pillar->GetVertexes(vertices);
					// OffsetGroundHeight = 0.0f;
					OffsetGroundHeight = FMath::Max(0.0f, BaseOffsetGroundHeight);
					height = FArmySceneData::Get()->WallHeight - OffsetGroundHeight;
				}
				else if (iter.Pin()->GetType() == OT_PackPipe)
				{
					FArmyPackPipe* tempPackPipe = iter.Pin()->AsassignObj<FArmyPackPipe>();
					tempPackPipe->GetFillOutsideVertices(vertices);
					// OffsetGroundHeight = 0.0f;
					OffsetGroundHeight = FMath::Max(0.0f, BaseOffsetGroundHeight);
					height = FArmySceneData::Get()->WallHeight - OffsetGroundHeight;
				}
				else if (iter.Pin()->GetType() == OT_AddWall)
				{
					FArmyAddWall* tempAddHalfWall = iter.Pin()->AsassignObj<FArmyAddWall>();
					tempAddHalfWall->GetVertexes(vertices);
					OffsetGroundHeight = 0.0f;
					height = tempAddHalfWall->GetHeight();
				}
			}
			FArmyMath::CleanPolygon(vertices);
			vertices = FArmyMath::ExturdePolygon(vertices, 0.05f, false);
			TArray<FVector2D> pillarOutLines;
			for (FVector& iter : vertices)
			{
				pillarOutLines.Push(FVector2D(iter));
			}
			FVector2D outStart, outEnd;
			TArray<FVector> outVertices;
			if (FArmyMath::IntersectSegmentWithConvexPolygon(WallWorldStartPos, WallWorldEndPos, pillarOutLines, outVertices))
			{
				if (outVertices.Num() >= 2)
				{
					outStart = FVector2D(outVertices[0]);
					outEnd = FVector2D(outVertices[1]);
					FArmyRoomSpaceArea::SurfaceHoleInfo tempInfo;
					tempInfo.surfaceHoleType = iter.Pin()->GetType();
					const FVector& bottomStart = FVector(outStart, OffsetGroundHeight);
					const FVector& botoomEnd = FVector(outEnd, OffsetGroundHeight);
					const FVector& topStart = bottomStart + FVector(0, 0, 1) * height;
					const FVector& topEnd = botoomEnd + FVector(0, 0, 1) * height;
					TArray<FVector> tempInnearHoles = { bottomStart, topStart,topEnd,botoomEnd };

					for (int j = 0; j < tempInnearHoles.Num(); ++j)
					{
						float x = FVector::DotProduct((tempInnearHoles[j] - CurrentEditSurface->GetPlaneCenter()), CurrentEditSurface->GetXDir());
						float y = FVector::DotProduct((tempInnearHoles[j] - CurrentEditSurface->GetPlaneCenter()), CurrentEditSurface->GetYDir());
						tempInnearHoles[j] = FVector(x, y, 0.0f);
					}
					tempInfo.surfaceVertices = tempInnearHoles;
					SurfaceHolesInfos.Push(tempInfo);
				}

			}
		}

        // @欧石楠 计算邻接墙面的造型在当前墙面上的投影区域
        const FVector& SurfaceStart = CurrentEditSurface->GetAttachLine()->GetCoreLine()->GetStart();
        const FVector& SurfaceEnd = CurrentEditSurface->GetAttachLine()->GetCoreLine()->GetEnd();

        TArray< TSharedPtr<FArmyRoomSpaceArea> > RelatedRoomSpaceAreas;
        FArmySceneData::Get()->GetRelatedRoomSpaceAreas(CurrentEditSurface, RelatedRoomSpaceAreas);
        for (auto RoomSpaceArea : RelatedRoomSpaceAreas)
        {
            TArray< TSharedPtr<FArmyBaseArea> > EditAreas = RoomSpaceArea->GetEditAreas();
            for (auto Area : EditAreas)
            {
                // @欧石楠 递归计算墙面子区域在其它墙面的投影
                std::function<void(TSharedPtr<FArmyBaseArea>, TSharedPtr<FArmyBaseArea>, const FVector&, const FVector&, TArray<FArmyRoomSpaceArea::SurfaceHoleInfo>&)>
                    CalculateProjectionArea = [&CalculateProjectionArea](
                        TSharedPtr<FArmyBaseArea> InArea,
                        TSharedPtr<FArmyBaseArea> InCurrentSurface,
                        const FVector& InSurfaceStart,
                        const FVector& InSurfaceEnd,
                        TArray<FArmyRoomSpaceArea::SurfaceHoleInfo>& OutSurfaceHolesInfos)
                {
                    const float ExtrusionHeight = InArea->GetExtrusionHeight();
                    TArray<FVector> Vertexes = InArea->OutArea->Vertices;
                    for (int32 i = 0; i < Vertexes.Num(); ++i)
                    {
                        const FVector UpLeft = InArea->PlaneToTranlate(Vertexes[i]);
                        const FVector UpRight = UpLeft + InArea->GetPlaneNormal() * ExtrusionHeight;
                        const FVector DownLeft = InArea->PlaneToTranlate(Vertexes[(i + 1) % Vertexes.Num()]);
                        const FVector DownRight = DownLeft + InArea->GetPlaneNormal() * ExtrusionHeight;

                        if ((FMath::IsNearlyEqual(UpLeft.X, InSurfaceStart.X, 0.1f) && FMath::IsNearlyEqual(UpLeft.Y, InSurfaceStart.Y, 0.1f) &&
                            FMath::IsNearlyEqual(DownLeft.X, InSurfaceStart.X, 0.1f) && FMath::IsNearlyEqual(DownLeft.Y, InSurfaceStart.Y, 0.1f)) ||
                            (FMath::IsNearlyEqual(UpLeft.X, InSurfaceEnd.X, 0.1f) && FMath::IsNearlyEqual(UpLeft.Y, InSurfaceEnd.Y, 0.1f) &&
                                FMath::IsNearlyEqual(DownLeft.X, InSurfaceEnd.X, 0.1f) && FMath::IsNearlyEqual(DownLeft.Y, InSurfaceEnd.Y, 0.1f)))
                        {
                            FArmyRoomSpaceArea::SurfaceHoleInfo TempInfo;
                            TempInfo.surfaceHoleType = OT_RectArea;
                            TArray<FVector> tempInnearHoles = { UpLeft, UpRight, DownRight, DownLeft };
                            for (int j = 0; j < tempInnearHoles.Num(); ++j)
                            {
                                float x = FVector::DotProduct((tempInnearHoles[j] - InCurrentSurface->GetPlaneCenter()), InCurrentSurface->GetXDir());
                                float y = FVector::DotProduct((tempInnearHoles[j] - InCurrentSurface->GetPlaneCenter()), InCurrentSurface->GetYDir());
                                tempInnearHoles[j] = FVector(x, y, 0.0f);
                            }
                            TempInfo.surfaceVertices = tempInnearHoles;
                            OutSurfaceHolesInfos.Push(TempInfo);
                        }
                    }

                    for (auto & SubArea : InArea->GetEditAreas())
                    {
                        CalculateProjectionArea(SubArea, InCurrentSurface, InSurfaceStart, InSurfaceEnd, OutSurfaceHolesInfos);
                    }
                };
                CalculateProjectionArea(Area, CurrentEditSurface, SurfaceStart, SurfaceEnd, SurfaceHolesInfos);
            }
        }

        // @欧石楠 记录墙面上柱子、吊顶、造型的投影区域的顶点
		if (CurrentEditSurface->GenerateFromObjectType != OT_AddWall)
		{
			int number = SurfaceHolesInfos.Num();
			for (int i = 0; i < number; ++i)
			{
				FArmyRoomSpaceArea::SurfaceHoleInfo& iter = SurfaceHolesInfos[i];
				if (iter.surfaceHoleType == OT_Pillar || iter.surfaceHoleType == OT_Beam || iter.surfaceHoleType == OT_AddWall || iter.surfaceHoleType == OT_RoomSpaceArea || iter.surfaceHoleType == OT_AirFlue
					|| iter.surfaceHoleType == OT_Punch || iter.surfaceHoleType == OT_PackPipe)
				{
					TArray<FVector> tempVerts = iter.surfaceVertices;
					TArray<FVector2D> points;
					TArray<TArray<FVector>> AreaHoles;
					Translate::TriangleAreaWithHole(tempVerts, AreaHoles, points);
					FVector Tangent = CurrentEditSurface->GetXDir();
					FVector Normal = FVector::CrossProduct(CurrentEditSurface->GetXDir(), CurrentEditSurface->GetYDir());
					FBox2D box(points);

					int number0 = points.Num();
					for (int i = 0; i < number0; ++i)
					{
						float UCoord, VCoord;
						UCoord = (points[i].X - box.Min.X) * 2.0f / 8.0f;
						VCoord = (points[i].Y - box.Min.Y) * 2.0f / 12.0f;
						PillarBeamAddWallVerts.Push(FDynamicMeshVertex(CurrentEditSurface->GetXDir() * points[i].X + CurrentEditSurface->GetYDir() * points[i].Y + CurrentEditSurface->GetPlaneCenter() + CurrentEditSurface->GetPlaneNormal()* 10.0f, Tangent, Normal, FVector2D(UCoord, VCoord), FColor::White));
					}
				}
			}
		}
    }
}

void FArmyBaseSurfaceController::CalculateFloorProjectionAreas()
{
    if (CurrentEditSurface->SurfaceType == 0)
    {
        TSharedPtr<FArmyRoom> CurrentAttachRoom;
        if (FArmySceneData::Get()->GetInnearRoomByRoomID(CurrentEditSurface->AttachRoomID, CurrentAttachRoom))
        {
            CurrentAttachRoom->GetHardwareListRelevance(CurrentSufaceHardWare);
            TArray<TWeakPtr<FArmyObject>> tempSimpleComponent;
            CurrentAttachRoom->GetObjectsRelevance(tempSimpleComponent);
            CurrentAttachRoom->GetObjectsRelevanceByObjectVertices(tempSimpleComponent);

            for (TWeakPtr<FArmyObject> iter : tempSimpleComponent)
            {
                if (iter.Pin()->GetType() != OT_Beam)
                {
                    CurrentSurfaceSimpleComponent.Push(iter);
                }
            }

            CalculateCurrentEditSurfaceOutLineInfo();
            
            const float FloorZ = CurrentEditSurface->GetZ();
            TArray< TSharedPtr<FArmyRoomSpaceArea> > RelatedRoomSpaceAreas =
                FArmySceneData::Get()->GetCurrentRoofAtttachWallRoomSpace(CurrentEditSurface->AttachRoomID);
            for (auto RoomSpaceArea : RelatedRoomSpaceAreas)
            {
                if (RoomSpaceArea->SurfaceType != 1)
                {
                    continue;
                }

                TArray< TSharedPtr<FArmyBaseArea> > EditAreas = RoomSpaceArea->GetEditAreas();
                for (auto Area : EditAreas)
                {
                    // @欧石楠 递归计算墙面子区域在其它墙面的投影
                    std::function<void(TSharedPtr<FArmyBaseArea>, TSharedPtr<FArmyBaseArea>, const float, TArray<FArmyRoomSpaceArea::SurfaceHoleInfo>&)>
                        CalculateProjectionArea = [&CalculateProjectionArea](
                            TSharedPtr<FArmyBaseArea> InArea,
                            TSharedPtr<FArmyBaseArea> InCurrentSurface,
                            const float InFoorZ,
                            TArray<FArmyRoomSpaceArea::SurfaceHoleInfo>& OutSurfaceHolesInfos)
                    {
                        const float ExtrusionHeight = InArea->GetExtrusionHeight();
                        TArray<FVector> Vertexes = InArea->OutArea->Vertices;
                        for (int32 i = 0; i < Vertexes.Num(); ++i)
                        {
                            const FVector UpLeft = InArea->PlaneToTranlate(Vertexes[i]);
                            const FVector UpRight = UpLeft + InArea->GetPlaneNormal() * ExtrusionHeight;
                            const FVector DownLeft = InArea->PlaneToTranlate(Vertexes[(i + 1) % Vertexes.Num()]);
                            const FVector DownRight = DownLeft + InArea->GetPlaneNormal() * ExtrusionHeight;

                            if (FMath::IsNearlyEqual(UpLeft.Z, InFoorZ, 0.1f) && FMath::IsNearlyEqual(DownLeft.Z, InFoorZ, 0.1f))
                            {
                                FArmyRoomSpaceArea::SurfaceHoleInfo TempInfo;
                                TempInfo.surfaceHoleType = OT_RectArea;
                                TArray<FVector> tempInnearHoles = { UpLeft, UpRight, DownRight, DownLeft };
                                for (int j = 0; j < tempInnearHoles.Num(); ++j)
                                {
                                    float x = FVector::DotProduct((tempInnearHoles[j] - InCurrentSurface->GetPlaneCenter()), InCurrentSurface->GetXDir());
                                    float y = FVector::DotProduct((tempInnearHoles[j] - InCurrentSurface->GetPlaneCenter()), InCurrentSurface->GetYDir());
                                    tempInnearHoles[j] = FVector(x, y, 0.0f);
                                }
                                TempInfo.surfaceVertices = tempInnearHoles;
                                OutSurfaceHolesInfos.Push(TempInfo);
                            }
                        }

                        for (auto & SubArea : InArea->GetEditAreas())
                        {
                            CalculateProjectionArea(SubArea, InCurrentSurface, InFoorZ, OutSurfaceHolesInfos);
                        }
                    };
                    CalculateProjectionArea(Area, CurrentEditSurface, FloorZ, SurfaceHolesInfos);
                }
            }
        }

        TArray<FVector> WorldOutVerts = CurrentEditSurface->GetOutArea()->Vertices;
        for (FVector& iter : WorldOutVerts)
        {
            TransPlaneToWorld(iter);
        }

        TArray<TWeakPtr<FArmyObject>> AllAddWalls;
        TArray<TWeakPtr<FArmyObject>> AllHalfAddWalls;
        FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_AddWall, AllAddWalls);
        for (TWeakPtr<FArmyObject> iter : AllAddWalls)
        {
            FArmyAddWall* tempAddWall = iter.Pin()->AsassignObj<FArmyAddWall>();
            if (tempAddWall && tempAddWall->GetIsHalfWall())
            {
                AllHalfAddWalls.Emplace(iter);
            }
        }

        for (TWeakPtr<FArmyObject> iter : AllHalfAddWalls)
        {
            TArray<FVector> result;
            iter.Pin()->GetVertexes(result);

            if (Translate::ArePolygonInPolygon(WorldOutVerts, result))
            {
                CurrentSurfaceSimpleComponent.Emplace(iter);
            }
        }
    }
}

void FArmyBaseSurfaceController::CalculateRoofProjectionAreas()
{
    if (CurrentEditSurface->SurfaceType == 2)
    {
        TSharedPtr<FArmyRoom> CurrentAttachRoom;
        if (FArmySceneData::Get()->GetInnearRoomByRoomID(CurrentEditSurface->AttachRoomID, CurrentAttachRoom))
        {
            CurrentAttachRoom->GetHardwareListRelevance(CurrentSufaceHardWare);
            TArray<TWeakPtr<FArmyObject>> tempSimpleComponent;
            CurrentAttachRoom->GetObjectsRelevance(tempSimpleComponent);
            CurrentAttachRoom->GetObjectsRelevanceByObjectVertices(tempSimpleComponent);
            CurrentSurfaceSimpleComponent = tempSimpleComponent;

            CalculateCurrentEditSurfaceOutLineInfo();

            const float FloorZ = CurrentEditSurface->GetZ();
            TArray< TSharedPtr<FArmyRoomSpaceArea> > RelatedRoomSpaceAreas =
                FArmySceneData::Get()->GetCurrentRoofAtttachWallRoomSpace(CurrentEditSurface->AttachRoomID);
            for (auto RoomSpaceArea : RelatedRoomSpaceAreas)
            {
                if (RoomSpaceArea->SurfaceType != 1)
                {
                    continue;
                }

                TArray< TSharedPtr<FArmyBaseArea> > EditAreas = RoomSpaceArea->GetEditAreas();
                for (auto Area : EditAreas)
                {
                    // @欧石楠 递归计算墙面子区域在其它墙面的投影
                    std::function<void(TSharedPtr<FArmyBaseArea>, TSharedPtr<FArmyBaseArea>, const float, TArray<FArmyRoomSpaceArea::SurfaceHoleInfo>&)>
                        CalculateProjectionArea = [&CalculateProjectionArea](
                            TSharedPtr<FArmyBaseArea> InArea,
                            TSharedPtr<FArmyBaseArea> InCurrentSurface,
                            const float InFoorZ,
                            TArray<FArmyRoomSpaceArea::SurfaceHoleInfo>& OutSurfaceHolesInfos)
                    {
                        const float ExtrusionHeight = InArea->GetExtrusionHeight();
                        TArray<FVector> Vertexes = InArea->OutArea->Vertices;
                        for (int32 i = 0; i < Vertexes.Num(); ++i)
                        {
                            const FVector UpLeft = InArea->PlaneToTranlate(Vertexes[i]);
                            const FVector UpRight = UpLeft + InArea->GetPlaneNormal() * ExtrusionHeight;
                            const FVector DownLeft = InArea->PlaneToTranlate(Vertexes[(i + 1) % Vertexes.Num()]);
                            const FVector DownRight = DownLeft + InArea->GetPlaneNormal() * ExtrusionHeight;

                            if (FMath::IsNearlyEqual(UpLeft.Z, InFoorZ, 0.1f) && FMath::IsNearlyEqual(DownLeft.Z, InFoorZ, 0.1f))
                            {
                                FArmyRoomSpaceArea::SurfaceHoleInfo TempInfo;
                                TempInfo.surfaceHoleType = OT_RectArea;
                                TArray<FVector> tempInnearHoles = { UpLeft, UpRight, DownRight, DownLeft };
                                for (int j = 0; j < tempInnearHoles.Num(); ++j)
                                {
                                    float x = FVector::DotProduct((tempInnearHoles[j] - InCurrentSurface->GetPlaneCenter()), InCurrentSurface->GetXDir());
                                    float y = FVector::DotProduct((tempInnearHoles[j] - InCurrentSurface->GetPlaneCenter()), InCurrentSurface->GetYDir());
                                    tempInnearHoles[j] = FVector(x, y, 0.0f);
                                }
                                TempInfo.surfaceVertices = tempInnearHoles;
                                OutSurfaceHolesInfos.Push(TempInfo);
                            }
                        }

                        for (auto & SubArea : InArea->GetEditAreas())
                        {
                            CalculateProjectionArea(SubArea, InCurrentSurface, InFoorZ, OutSurfaceHolesInfos);
                        }
                    };
                    CalculateProjectionArea(Area, CurrentEditSurface, FloorZ, SurfaceHolesInfos);
                }
            }
        }
    }
}

void FArmyBaseSurfaceController::OnRectAreaCommand()
{
	SetOperation((uint8)EHardModeOperation::HO_DrawRect);
	TSharedPtr<FArmyRectAreaOperation> tempOpreation = StaticCastSharedPtr<FArmyRectAreaOperation>(CurrentOperation);
	tempOpreation->SetCurrentEditSurface(CurrentEditSurface);
	AddAreaState = 1;
}

void FArmyBaseSurfaceController::OnCircleAreaCommand()
{
	SetOperation((uint8)EHardModeOperation::HO_DrawCircle);
	TSharedPtr<FArmyCircleAreaOperation> tempCircle = StaticCastSharedPtr<FArmyCircleAreaOperation>(CurrentOperation);
	tempCircle->SetCurrentEditSurface(CurrentEditSurface);
	AddAreaState = 1;


}

void FArmyBaseSurfaceController::OnRegularPolygonAreaCommand()
{
	SetOperation((uint8)EHardModeOperation::HO_DrawPolygon);
	TSharedPtr<FArmyRegularPolygonAreaOperation> tempPolygon = StaticCastSharedPtr<FArmyRegularPolygonAreaOperation>(CurrentOperation);
	tempPolygon->SetCurrentEditSurface(CurrentEditSurface);
	AddAreaState = 1;

}

void FArmyBaseSurfaceController::OnFreePolygonAreaCommand()
{
	SetOperation((uint8)EHardModeOperation::HO_DrawFreePolygon);
	TSharedPtr<FArmyFreePolygonAreaOperation> tempFreePolygon = StaticCastSharedPtr<FArmyFreePolygonAreaOperation>(CurrentOperation);
	tempFreePolygon->SetCurrentEditSurface(CurrentEditSurface);
	AddAreaState = 1;

}

void FArmyBaseSurfaceController::SetPropertyVisibility(EVisibility _Visibility)
{
	SufaceDetail->SetPropertyPanelVisibility(_Visibility);
}

TSharedPtr<SWidget> FArmyBaseSurfaceController::CreateToolBarRightArea()
{
	return
		SNew(SBorder)
		.Padding(0)
		.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF191A1D"))
		.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
		[
			SNew(SBox)
			.HeightOverride(32)
		[
			SNew(SButton)
			.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("HardModeExit.Button"))
		.OnClicked_Raw(this, &FArmyBaseSurfaceController::OnReturnViewAngleCommand)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(20, 0, 0, 0)
		.AutoWidth()
		[
			SNew(SImage)
			.Image(FArmyStyle::Get().GetBrush("Icon.Back"))
		]

	+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(8, 0, 20, 0)
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("退出编辑")))
		.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		]
		]
		]
		];
}

void FArmyBaseSurfaceController::CollectAllLinesAndPoints(TArray< TSharedPtr<FArmyLine> >& InOutLines, TArray< TSharedPtr<FArmyPoint> >& InOutPoints, TArray<TSharedPtr<FArmyCircle>>& InOutCircles)
{
	if (CurrentEditSurface.IsValid())
	{
		TArray<TSharedPtr<FArmyLine>> Lines;
		CurrentEditSurface->GetLines(Lines);

		int number = RoofFallOffRegionVerts.Num();
		for (int i = 0; i < number; ++i)
		{
			FVector start = RoofFallOffRegionVerts[i%number];
			FVector end = RoofFallOffRegionVerts[(i + 1) % number];
			TransToCurrentPlane(start);
			TransToCurrentPlane(end);
			TSharedPtr<FArmyLine> RoofLine = MakeShareable(new FArmyLine(start, end));
			InOutLines.Add(RoofLine);
		}
		InOutLines.Append(Lines);
		for (TSharedPtr<FArmyBaseArea> iter : CurrentEditSurface->GetEditAreas())
		{
			TArray < TSharedPtr<FArmyLine>> Line0;

			TArray< TSharedPtr<FArmyCircle> >circle;
			iter->GetLines(Line0);
			iter->GetCircleLines(circle);
			InOutLines.Append(Line0);
			InOutCircles.Append(circle);
		}
		if (CurrentOperation.IsValid())
		{
			InOutLines.Append(CurrentOperation->GetLines());
		}
	}
}

void FArmyBaseSurfaceController::OnSaveCommand()
{
	FArmyFrameCommands::OnMenuSave();
}

void FArmyBaseSurfaceController::OnDeleteCommand()
{
    // @欧石楠 删除踢脚线
    TArray<AActor*> SeletecdActors;
    GXREditor->GetSelectedActors(SeletecdActors);
    for (AActor* It : SeletecdActors)
    {
        if (It->IsValidLowLevel() && It->IsA(AArmyExtrusionActor::StaticClass()))
        {
            AArmyExtrusionActor* SkirtingLineActor = Cast<AArmyExtrusionActor>(It);
            TSharedPtr<FArmyRoomSpaceArea> RoomSpace = StaticCastSharedPtr<FArmyRoomSpaceArea>(SkirtingLineActor->AttachSurfaceArea.Pin());
            if (RoomSpace.IsValid())
            {
                RoomSpace->RemoveSkitLineActor(SkirtingLineActor);
            }
        }
    }

	if (selected.IsValid())
	{
		if (CurrentOperation.IsValid())
		{
			CurrentOperation->EndOperation();
		}

		if (selected->GetType() != EObjectType::OT_RoomSpaceArea)
		{
			/* @梁晓菲 子模式删除波打线*/
			if (selected->GetType() == EObjectType::OT_BodaArea)
			{
				TSharedPtr<FArmyBodaArea> tempBodaArea = StaticCastSharedPtr<FArmyBodaArea>(selected);
				TSharedPtr<FArmyRectArea> tempRectArea = StaticCastSharedPtr<FArmyRectArea>(tempBodaArea->AttachParentArea);
				tempRectArea->DeleteBodaArea();
			}
			else
			{
				SCOPE_TRANSACTION(TEXT("删除区域"));

				selected->MoveChildAreaToParent();
				FArmySceneData::Get()->Delete(selected);
				selected = NULL;
				AxisRuler->SetBoundObj(NULL);
			}

		}
		else
		{
			SCOPE_TRANSACTION(TEXT("清空地面区域"));
			CurrentEditSurface->ResetSurfaceStyle(false);
			CurrentEditSurface->Modify();
			selected = NULL;
		}
		HightLightActor->SetActorHiddenInGame(true);
        if (SufaceDetail.IsValid())
        {
			SufaceDetail->SetPropertyPanelVisibility(EVisibility::Collapsed);
        }
		bHoverRectEdge = false;

        AxisRuler->SetBoundObj(nullptr);
	}
}

void FArmyBaseSurfaceController::OnEmptyCommand()
{

	if (CurrentOperation.IsValid())
	{
		CurrentOperation->EndOperation();
	}
	GGI->Window->PresentModalDialog(TEXT("是否清空？"), FSimpleDelegate::CreateRaw(this, &FArmyBaseSurfaceController::ClearCurrentEditorSurfaceData));

}

void FArmyBaseSurfaceController::OnUndoCommand()
{
	FArmyFrameCommands::OnMenuUndo();

}

void FArmyBaseSurfaceController::OnRedoCommand()
{
	FArmyFrameCommands::OnMenuRedo();
}

void FArmyBaseSurfaceController::OnReplaceCommand()
{
	//LeftPanel->ShowReplaceData();
}

void FArmyBaseSurfaceController::MouseMove(FViewport* ViewPort, int32 X, int32 Y)
{
	FArmyDesignModeController::MouseMove(ViewPort, X, Y);

	bool TempHoverRectEdge = false;
	if (CurrentEditSurface.IsValid())
	{
        // @欧石楠 悬浮高亮起铺点
        if (selected.IsValid() && LayingPoint->GetState() != OS_Selected)
        {
            FVector4 WorldMousePos = GVC->PixelToWorld(CurrentMouseClick.X, CurrentMouseClick.Y, 0.f);
            if (LayingPoint->Hover(FVector(WorldMousePos), GVC))
            {
                LayingPoint->SetState(OS_Hovered);
                return;
            }
            else
            {
                LayingPoint->SetState(OS_Normal);
            }
        }
        
        if (CurrentEditSurface->GetEditAreas().Num() > 0)
        {
			//@郭子阳
			//递归地寻找单边
		std::function<bool (TSharedPtr<FArmyBaseArea>)> 
			SelectEdge = [&TempHoverRectEdge ,&SelectEdge,this,&X,&Y](TSharedPtr<FArmyBaseArea> Area) {
			

			for(auto & SubArea : Area->GetEditAreas())
			{
				if (SelectEdge(SubArea))
				{
					return true;
				}
			}

			if (Area->GetType() == OT_RectArea)
			{
				TSharedPtr<FArmyRectArea> tempRect = StaticCastSharedPtr<FArmyRectArea>(Area);
				if (tempRect->HoverSingleEdge(FVector2D(X, Y), GVC, HoverEdgeStart, HoverEdgeEnd))
				{
					TempHoverRectEdge = true;
					HoveredArea = tempRect;
					return true;
				}
			}

			return false;
		};

		SelectEdge(CurrentEditSurface);
        }
	}

	if (TempHoverRectEdge)
	{
		bHoverRectEdge = true;
	}
	else
	{
		bHoverRectEdge = false;
		HoveredArea = nullptr;
	}
}


void FArmyBaseSurfaceController::OnMaterialBrushCommand()
{
	//EndOperation();
	SetOperation((uint8)EHardModeOperation::HO_MaterialBrush);

}

