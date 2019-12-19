#include "ArmyEditorModeManager.h"
#include "ArmyEditorEngine.h"

FArmyEditorModeTools::FArmyEditorModeTools():
	bIsTracking(false)
	, CoordSystem(ArmyCOORD_World)

{
	if (!WITH_EDITOR)
	{
		// Register our callback for actor selection changes
		USelection::SelectNoneEvent.AddRaw(this, &FArmyEditorModeTools::OnEditorSelectNone);
		USelection::SelectionChangedEvent.AddRaw(this, &FArmyEditorModeTools::OnEditorSelectionChanged);
		USelection::SelectObjectEvent.AddRaw(this, &FArmyEditorModeTools::OnEditorSelectionChanged);
	}
}

FArmyEditorModeTools::~FArmyEditorModeTools()
{

}

void FArmyEditorModeTools::AddReferencedObjects(FReferenceCollector& Collector)
{

}

void FArmyEditorModeTools::SetPivotLocation(const FVector& Location, const bool bIncGridBase)
{
	CachedLocation = PivotLocation = SnappedLocation = Location;
	if (bIncGridBase)
	{
		GridBase = Location;
	}
}

bool FArmyEditorModeTools::StartTracking(UArmyEditorViewportClient* InViewportClient, FViewport* InViewport)
{
	bIsTracking = true;
	bool bTransactionHandled = false;

	CachedLocation = PivotLocation;	// Cache the pivot location

	/*for (const auto& Mode : Modes)
	{
		bTransactionHandled |= Mode->StartTracking(InViewportClient, InViewport);
	}*/
	bTransactionHandled = true;

	return bTransactionHandled;
}

bool FArmyEditorModeTools::EndTracking(UArmyEditorViewportClient* InViewportClient, FViewport* InViewport)
{
	bIsTracking = false;
	bool bTransactionHandled = false;

	/*for (const auto& Mode : Modes)
	{
		bTransactionHandled |= Mode->EndTracking(InViewportClient, InViewportClient->Viewport);
	}*/
	bTransactionHandled = true;

	CachedLocation = PivotLocation;	// Clear the pivot location

	return bTransactionHandled;
}

bool FArmyEditorModeTools::MouseEnter(UArmyEditorViewportClient* InViewportClient, FViewport* Viewport, int32 X, int32 Y)
{
	return true;

}

bool FArmyEditorModeTools::MouseLeave(UArmyEditorViewportClient* InViewportClient, FViewport* Viewport)
{
	return true;

}

bool FArmyEditorModeTools::MouseMove(UArmyEditorViewportClient* InViewportClient, FViewport* Viewport, int32 X, int32 Y)
{
	return true;

}

bool FArmyEditorModeTools::ReceivedFocus(UArmyEditorViewportClient* InViewportClient, FViewport* Viewport)
{
	return true;

}

bool FArmyEditorModeTools::LostFocus(UArmyEditorViewportClient* InViewportClient, FViewport* Viewport)
{
	return true;
}

bool FArmyEditorModeTools::UsesTransformWidget() const
{
	return true;

}

bool FArmyEditorModeTools::UsesTransformWidget(FArmyWidget::EWidgetMode CheckMode) const
{
	return true;

}

bool FArmyEditorModeTools::CapturedMouseMove(UArmyEditorViewportClient* InViewportClient, FViewport* InViewport, int32 InMouseX, int32 InMouseY)
{
	return true;

}

bool FArmyEditorModeTools::AllowsViewportDragTool() const
{
	bool bCanUseDragTool = false;
// 	for (const TSharedPtr<FEdMode>& Mode : Modes)
// 	{
// 		bCanUseDragTool |= Mode->AllowsViewportDragTool();
// 	}
	bCanUseDragTool = true;
	return bCanUseDragTool;
}

void FArmyEditorModeTools::SelectNone()
{
// 	for (int32 ModeIndex = 0; ModeIndex < Modes.Num(); ++ModeIndex)
// 	{
// 		const TSharedPtr<FEdMode>& Mode = Modes[ModeIndex];
// 		Mode->SelectNone();
// 	}

}

bool FArmyEditorModeTools::BoxSelect(FBox& InBox, bool InSelect)
{
	bool bHandled = false;
// 	for (int32 ModeIndex = 0; ModeIndex < Modes.Num(); ++ModeIndex)
// 	{
// 		const TSharedPtr<FEdMode>& Mode = Modes[ModeIndex];
// 		bHandled |= Mode->BoxSelect(InBox, InSelect);
// 	}
	return bHandled;
}

bool FArmyEditorModeTools::FrustumSelect(const FConvexVolume& InFrustum, bool InSelect)
{
	bool bHandled = false;
// 	for (int32 ModeIndex = 0; ModeIndex < Modes.Num(); ++ModeIndex)
// 	{
// 		const TSharedPtr<FEdMode>& Mode = Modes[ModeIndex];
// 		bHandled |= Mode->FrustumSelect(InFrustum, InSelect);
// 	}
	return bHandled;
}

void FArmyEditorModeTools::DrawHUD(UArmyEditorViewportClient* InViewportClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas)
{
// 	for (const auto& Mode : Modes)
// 	{
// 		Mode->DrawHUD(InViewportClient, Viewport, View, Canvas);
// 	}

}

FVector FArmyEditorModeTools::GetWidgetLocation() const
{
	return PivotLocation;
}

void FArmyEditorModeTools::SetWidgetMode(FArmyWidget::EWidgetMode InWidgetMode)
{
	WidgetMode = InWidgetMode;
}

FArmyWidget::EWidgetMode FArmyEditorModeTools::GetWidgetMode() const
{
	return WidgetMode;
}

void FArmyEditorModeTools::CycleWidgetMode()
{
	if (WidgetMode == FArmyWidget::WM_Translate)
	{
		WidgetMode = FArmyWidget::WM_Rotate;
	}
	else if (WidgetMode == FArmyWidget::WM_Rotate)
	{
		WidgetMode = FArmyWidget::WM_Scale;
	}
	else if (WidgetMode == FArmyWidget::WM_Scale)
	{
		WidgetMode = FArmyWidget::WM_Translate;
	}
	else
	{
		WidgetMode = FArmyWidget::WM_Translate;
	}
}

bool FArmyEditorModeTools::InputDelta(UArmyEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale)
{
	return true;
}

bool FArmyEditorModeTools::AllowWidgetMove() const
{
	return true;
}

EArmyCoordSystem FArmyEditorModeTools::GetCoordSystem(bool bGetRawValue /*= false*/)
{
	if (!bGetRawValue && (GetWidgetMode() == FArmyWidget::WM_Scale))
	{
		return ArmyCOORD_Local;
	}
	else
	{
		return CoordSystem;
	}
}

void FArmyEditorModeTools::SetCoordSystem(EArmyCoordSystem NewCoordSystem)
{
	CoordSystem = NewCoordSystem;
}

USelection* FArmyEditorModeTools::GetSelectedActors() const
{
	return GArmyEditor->GetSelectedActors();
}

USelection* FArmyEditorModeTools::GetSelectedObjects() const
{
	return GArmyEditor->GetSelectedObjects();
}

USelection* FArmyEditorModeTools::GetSelectedComponents() const
{
	return GArmyEditor->GetSelectedComponents();
}

FMatrix FArmyEditorModeTools::GetCustomDrawingCoordinateSystem()
{
	FMatrix Matrix = FMatrix::Identity;

	switch (GetCoordSystem())
	{
	case ArmyCOORD_Local:
	{
		// Let the current mode have a shot at setting the local coordinate system.
		// If it doesn't want to, create it by looking at the currently selected actors list.

		bool CustomCoordinateSystemProvided = false;
// 		for (const auto& Mode : Modes)
// 		{
// 			if (Mode->GetCustomDrawingCoordinateSystem(Matrix, nullptr))
// 			{
// 				CustomCoordinateSystemProvided = true;
// 				break;
// 			}
// 		}

		if (!CustomCoordinateSystemProvided)
		{
			const int32 Num = GetSelectedActors()->CountSelections<AActor>();

			// Coordinate system needs to come from the last actor selected
			if (Num > 0)
			{
				Matrix = FQuatRotationMatrix(GetSelectedActors()->GetBottom<AActor>()->GetActorQuat());
			}
		}

		if (!Matrix.Equals(FMatrix::Identity))
		{
			Matrix.RemoveScaling();
		}
	}
	break;

	case ArmyCOORD_World:
		break;

	default:
		break;
	}

	return Matrix;
}

FMatrix FArmyEditorModeTools::GetCustomInputCoordinateSystem()
{
	return GetCustomDrawingCoordinateSystem();
}

void FArmyEditorModeTools::OnEditorSelectionChanged(UObject* NewSelection)
{
	AActor* Actor = Cast<AActor>(NewSelection);
	if (Actor != nullptr)
	{
		//@fixme - why isn't this using UObject::IsSelected()?
		if (GArmyEditor->GetSelectedActors()->IsSelected(Actor))
		{
			SetPivotLocation(Actor->GetActorLocation(), false);
		}
	}
}

void FArmyEditorModeTools::OnEditorSelectNone()
{

}
