#include "ArmyEditorEngine.h"
#include "ArmyGroupActor.h"
#include "Misc/MessageDialog.h"
#include "Engine/Selection.h"

AArmyGroupActor* UArmyEditorEngine::CreateGroupActor(TArray<AActor*>& Actors)
{
	AArmyGroupActor* SpawnedGroupActor=NULL;
	{
		TArray<AActor*> ActorsToAdd;

		ULevel* ActorLevel=NULL;

		bool bActorsInSameLevel=true;
		for(int32 i = 0 ;i<Actors.Num();i++)
		{
			AActor* Actor=Actors[i];
			if(!ActorLevel)
			{
				ActorLevel=Actor->GetLevel();
			}
			else if(ActorLevel!=Actor->GetLevel())
			{
				bActorsInSameLevel=false;
				break;
			}

			if(Actor->IsA(AActor::StaticClass())&&!Actor->IsA(AArmyGroupActor::StaticClass()))
			{
				// Add each selected actor to our new group
				// Adding an actor will remove it from any existing groups.
				ActorsToAdd.Add(Actor);

			}
		}

		if(bActorsInSameLevel)
		{
			if(ActorsToAdd.Num()>1)
			{
				check(ActorLevel);
				// Store off the current level and make the level that contain the actors to group as the current level
				UWorld* World=ActorLevel->OwningWorld;
				check(World);
				{
					//const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "Group_Regroup", "Regroup Ctrl+G"));

					FActorSpawnParameters SpawnInfo;
					SpawnInfo.OverrideLevel=ActorLevel;
					SpawnedGroupActor=World->SpawnActor<AArmyGroupActor>(SpawnInfo);

					for(int32 ActorIndex=0; ActorIndex<ActorsToAdd.Num(); ++ActorIndex)
					{
						SpawnedGroupActor->Add(*ActorsToAdd[ActorIndex]);
					}

					SpawnedGroupActor->CenterGroupLocation();
					SpawnedGroupActor->Lock();
				}
			}
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok,NSLOCTEXT("UnrealEd","Group_CantCreateGroupMultipleLevels","Can't group the selected actors because they are in different levels."));
		}
	}
	return SpawnedGroupActor;
}

AArmyGroupActor* UArmyEditorEngine::edactRegroupFromSelected()
{
	AArmyGroupActor* SpawnedGroupActor = NULL;
	//if (GVRSEditor->bGroupingActive)
	{
		TArray<AActor*> ActorsToAdd;
		for (FSelectionIterator It(GArmyEditor->GetSelectedActorIterator()); It; ++It)
		{
			AActor* Actor=CastChecked<AActor>(*It);
			if (Actor->IsA(AActor::StaticClass()) && !Actor->IsA(AArmyGroupActor::StaticClass()))
			{
				ActorsToAdd.Add(Actor);
			}
		}

		SpawnedGroupActor = CreateGroupActor(ActorsToAdd);
	}
	return SpawnedGroupActor;
}


void UArmyEditorEngine::edactUngroupFromSelected()
{
	//if (GEditor->bGroupingActive)
	{
		TArray<AArmyGroupActor*> OutermostGroupActors;

		for (FSelectionIterator It(GArmyEditor->GetSelectedActorIterator()); It; ++It)
		{
			AActor* Actor = CastChecked<AActor>(*It);

			// Get the outermost locked group
			AArmyGroupActor* OutermostGroup = AArmyGroupActor::GetRootForActor(Actor, true);
			if (OutermostGroup == NULL)
			{
				// Failed to find locked root group, try to find the immediate parent
				OutermostGroup = AArmyGroupActor::GetParentForActor(Actor);
			}

			if (OutermostGroup)
			{
				OutermostGroupActors.AddUnique(OutermostGroup);
			}
		}

		if (OutermostGroupActors.Num())
		{
			//const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "Group_Disband", "Disband Group"));
			for (int32 GroupIndex = 0; GroupIndex < OutermostGroupActors.Num(); ++GroupIndex)
			{
				AArmyGroupActor* GroupActor = OutermostGroupActors[GroupIndex];
				GroupActor->ClearAndRemove();
			}
		}
		SelectionUngroupEvent.ExecuteIfBound(OutermostGroupActors);
	}
}

void UArmyEditorEngine::edactAddToGroup()
{
	//if (GEditor->bGroupingActive)
	{
		AArmyGroupActor::AddSelectedActorsToSelectedGroup();
	}
}