#include "ArmyEditorViewportClickHandlers.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyEditorEngine.h"
#include "Engine/Selection.h"

#define LOCTEXT_NAMESPACE "ClickHandlers"

namespace ArmyClickHandlers
{
	bool ClickActor(UArmyEditorViewportClient* ViewportClient, AActor* Actor, const FArmyViewportClick& Click, bool bAllowSelectionChange)
	{
		//如果右键选中了一个Actor，并且这个物体已经被选中，则弹出右键菜单
		TArray<AActor*> Actors;
		bool bHasSelected = false;
		GArmyEditor->GetSelectedActors(Actors);
		for (auto& It : Actors)
		{
			if (Actors.Contains(Actor))
			{
				bHasSelected = true;
				break;
			}
		}

		if (Click.GetKey() == EKeys::RightMouseButton)
		{
			if (bHasSelected)
			{
				GArmyEditor->RightSelectedEvent.ExecuteIfBound();
				return false;
			}
			//如果这个Actor之前没有被选中，则直接取消选中
			else
			{
				GArmyEditor->SelectNone(true, true, false);
				GArmyEditor->RightSelectedEvent.ExecuteIfBound();
				ViewportClient->CancelDrop();
				return false;
			}
		}
		else if (Click.IsAltDown())
		{
			
		}

		//@欧石楠 临时
		if (Actor && Actor->IsSelectable())
		{
			if (bAllowSelectionChange)
			{
				//@刘克祥 AutoMultiSelectEnabled（）全局使用开启多选功能
				if (Click.IsControlDown()||ViewportClient->AutoMultiSelectEnabled())
				{
					const bool bSelect = !Actor->IsSelected();
					GArmyEditor->SelectActor(Actor, bSelect, true, true);
				}
				else
				{
					if(!bHasSelected)GArmyEditor->DeselectActor(false, true, false);
					GArmyEditor->SelectActor(Actor, true, true, true);
				}
			}
			return false;
		}
		return false;
	}

	void ClickBackdrop(UArmyEditorViewportClient* ViewportClient, const FArmyViewportClick& Click)
	{
		if (Click.GetKey() == EKeys::LeftMouseButton)
		{
			if (!Click.IsControlDown())
			{
				//const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "ClickingBackground", "Clicking Background"));
				//UE_LOG(LogEditorViewport, Log, TEXT("Clicking Background"));
				GArmyEditor->SelectNone(true, true);
			}
		}
	}

	void ClickSurface(UArmyEditorViewportClient* ViewportClient, UModel* Model, int32 iSurf, const FArmyViewportClick& Click)
	{
		// Gizmos can cause BSP surfs to become selected without this check
		if (Click.GetKey() == EKeys::RightMouseButton && Click.IsControlDown())
		{
			return;
		}

		// Remember hit location for actor-adding.
		FBspSurf& Surf = Model->Surfs[iSurf];

		//const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "SelectBrushSurface", "Select Brush Surface"));
		bool bDeselectAlreadyHandled = false;
		bool bIsActorAlreadySelected = Surf.Actor && Surf.Actor->IsSelected();

		USelection* SelectedActors = GArmyEditor->GetSelectedActors();
		SelectedActors->BeginBatchSelectOperation();

		// We are going to handle the notification ourselves
		const bool bNotify = false;
		if (1)//GetDefault<ULevelEditorViewportSettings>()->bClickBSPSelectsBrush)
		{
			// Add to the actor selection set the brush actor that belongs to this BSP surface.
			// Check Surf.Actor, as it can be NULL after deleting brushes and before rebuilding BSP.
			//if (Surf.Actor)
			{
				if (!Click.IsControlDown())
				{
					GArmyEditor->SelectNone(false, true);
					bDeselectAlreadyHandled = true;
				}
				// If the builder brush is selected, first deselect it.
				/*for (FSelectionIterator It(*SelectedActors); It; ++It)
				{
					ABrush* Brush = Cast<ABrush>(*It);
					if (Brush && FActorEditorUtils::IsABuilderBrush(Brush))
					{
						GEditor->SelectActor(Brush, false, bNotify);
						break;
					}
				}*/
				if (Surf.Actor)
					GArmyEditor->SelectActor(Surf.Actor, true, bNotify);
			}
		}

		// Select or deselect surfaces.
		{
			if (!Click.IsControlDown() && !bDeselectAlreadyHandled)
			{
				GArmyEditor->SelectNone(false, true);
			}
			//Model->ModifySurf(iSurf, false);
			Surf.PolyFlags ^= PF_Selected;

			// If there are no surfaces selected now, deselect the actor
			if (/*!Model->HasSelectedSurfaces() &&*/ Surf.Actor)
			{
				GArmyEditor->SelectActor(Surf.Actor, false, bNotify);
				bIsActorAlreadySelected = false;
			}
		}

		SelectedActors->EndBatchSelectOperation(false);

		if (!bIsActorAlreadySelected)
		{
			GArmyEditor->NoteSelectionChange();
		}
	}
}

#undef LOCTEXT_NAMESPACE
