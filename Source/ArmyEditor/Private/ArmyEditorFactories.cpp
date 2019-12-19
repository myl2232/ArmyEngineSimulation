#include "CoreMinimal.h"
#include "ArmyLevelFactory.h"
#include "ArmyEditor.h"
#include "ArmyGroupActor.h"
#include "ArmyEditorEngine.h"
#include "GameFramework/DefaultPhysicsVolume.h"

UObject* UArmyLevelFactory::FactoryCreateText(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const TCHAR*& Buffer, const TCHAR* BufferEnd, FFeedbackContext* Warn)
{
	//FEditorDelegates::OnAssetPreImport.Broadcast(this, Class, InParent, Name, Type);

	UWorld* World = GWorld;
	//@todo locked levels - if lock state is persistent, do we need to check for whether the level is locked?
// #ifdef MULTI_LEVEL_IMPORT
// 	// this level is the current level for pasting. If we get a named level, not for pasting, we will look up the level, and overwrite this
// 	ULevel*				OldCurrentLevel = World->GetCurrentLevel();
// 	check(OldCurrentLevel);
// #endif

	UPackage* RootMapPackage = Cast<UPackage>(InParent);
	TMap<FString, UPackage*> MapPackages;
	TMap<AActor*, AActor*> MapActors;
	// Assumes data is being imported over top of a new, valid map.
	FParse::Next(&Buffer);
	if (GetBEGIN(&Buffer, TEXT("MAP")))
	{
		if (RootMapPackage)
		{
			FString MapName;
			if (FParse::Value(Buffer, TEXT("Name="), MapName))
			{
				// Advance the buffer
				Buffer += FCString::Strlen(TEXT("Name="));
				Buffer += MapName.Len();
				// Check to make sure that there are no naming conflicts
				if (RootMapPackage->Rename(*MapName, nullptr, REN_Test | REN_ForceNoResetLoaders))
				{
					// Rename it!
					RootMapPackage->Rename(*MapName, nullptr, REN_ForceNoResetLoaders);
				}
				else
				{
					Warn->Logf(ELogVerbosity::Warning, TEXT("The Root map package name : '%s', conflicts with the existing object : '%s'"), *RootMapPackage->GetFullName(), *MapName);
					//FEditorDelegates::OnAssetPostImport.Broadcast(this, nullptr);
					return nullptr;
				}

				// Stick it in the package map
				MapPackages.Add(MapName, RootMapPackage);
			}
		}
	}
	else
	{
		return World;
	}

	bool bIsExpectingNewMapTag = false;

	// Unselect all actors.
	GArmyEditor->SelectNone(false, false);

	// Mark us importing a T3D (only from a file, not from copy/paste).
	//GVRSEditor->IsImportingT3D = (FCString::Stricmp(Type, TEXT("paste")) != 0) && (FCString::Stricmp(Type, TEXT("move")) != 0);
	//GIsImportingT3D = GVRSEditor->IsImportingT3D;

	// We need to detect if the .t3d file is the entire level or just selected actors, because we
	// don't want to replace the WorldSettings and BuildBrush if they already exist. To know if we
	// can skip the WorldSettings and BuilderBrush (which will always be the first two actors if the entire
	// level was exported), we make sure the first actor is a WorldSettings, if it is, and we already had
	// a WorldSettings, then we skip the builder brush
	// In other words, if we are importing a full level into a full level, we don't want to import
	// the WorldSettings and BuildBrush
	bool bShouldSkipImportSpecialActors = false;
	bool bHitLevelToken = false;

	FString MapPackageText;

	int32 ActorIndex = 0;

	//@todo locked levels - what needs to happen here?


	// Maintain a list of a new actors and the text they were created from.
	TMap<AActor*, FString> NewActorMap;
	TMap< FString, AArmyGroupActor* > NewGroups; // Key=The orig actor's group's name, Value=The new actor's group.

											 // Maintain a lookup for the new actors, keyed by their source FName.
	TMap<FName, AActor*> NewActorsFNames;

	// Maintain a lookup of the new actors to their parent and socket attachment if provided.
	struct FAttachmentDetail
	{
		const FName ParentName;
		const FName SocketName;
		FAttachmentDetail(const FName InParentName, const FName InSocketName) : ParentName(InParentName), SocketName(InSocketName) {}
	};
	TMap<AActor*, FAttachmentDetail> NewActorsAttachmentMap;

	FString StrLine;
	while (FParse::Line(&Buffer, StrLine))
	{
		const TCHAR* Str = *StrLine;

		// If we're still waiting to see a 'MAP' tag, then check for that
		if (bIsExpectingNewMapTag)
		{
			if (GetBEGIN(&Str, TEXT("MAP")))
			{
				bIsExpectingNewMapTag = false;
			}
			else
			{
				// Not a new map tag, so continue on
			}
		}
		else if (GetEND(&Str, TEXT("MAP")))
		{
			// End of brush polys.
			bIsExpectingNewMapTag = true;
		}
		else if (GetBEGIN(&Str, TEXT("LEVEL")))
		{
			bHitLevelToken = true;
#ifdef MULTI_LEVEL_IMPORT
			// try to look up the named level. if this fails, we will need to create a new level
			if (ParseObject<ULevel>(Str, TEXT("NAME="), World->GetCurrentLevel(), World->GetOuter()) == false)
			{
				// get the name
				FString LevelName;
				// if there is no name, that means we are pasting, so just put this guy into the CurrentLevel - don't make a new one
				if (FParse::Value(Str, TEXT("NAME="), LevelName))
				{
					// create a new named level
					World->SetCurrentLevel(new(World->GetOuter(), *LevelName)ULevel(FObjectInitializer(), FURL(nullptr)));
				}
			}
#endif
		}
		else if (GetEND(&Str, TEXT("LEVEL")))
		{
#ifdef MULTI_LEVEL_IMPORT
			// any actors outside of a level block go into the current level
			World->SetCurrentLevel(OldCurrentLevel);
#endif
		}
		else if (GetBEGIN(&Str, TEXT("ACTOR")))
		{
			UClass* TempClass;
			if (ParseObject<UClass>(Str, TEXT("CLASS="), TempClass, ANY_PACKAGE))
			{
				// Get actor name.
				FName ActorUniqueName(NAME_None);
				FName ActorSourceName(NAME_None);
				FParse::Value(Str, TEXT("NAME="), ActorSourceName);
				ActorUniqueName = ActorSourceName;
				// Make sure this name is unique.
				AActor* Found = nullptr;
				if (ActorUniqueName != NAME_None)
				{
					// look in the current level for the same named actor
					Found = FindObject<AActor>(World->GetCurrentLevel(), *ActorUniqueName.ToString());
				}
				if (Found)
				{
					ActorUniqueName = MakeUniqueObjectName(World->GetCurrentLevel(), TempClass, ActorUniqueName);
				}

				FString ObjIDStr;
				FParse::Value(Str, TEXT("ObjID="), ObjIDStr);
				int32 ObjID = FCString::Atoi(*ObjIDStr);

				FString SynIDStr;
				FParse::Value(Str, TEXT("SynID="), SynIDStr);
				int32 SynID = FCString::Atoi(*SynIDStr);

				// Get parent name for attachment.
				FName ActorParentName(NAME_None);
				FParse::Value(Str, TEXT("ParentActor="), ActorParentName);

				// Get socket name for attachment.
				FName ActorParentSocket(NAME_None);
				FParse::Value(Str, TEXT("SocketName="), ActorParentSocket);

				// if an archetype was specified in the Begin Object block, use that as the template for the ConstructObject call.
				FString ArchetypeName;
				AActor* Archetype = nullptr;
				if (FParse::Value(Str, TEXT("Archetype="), ArchetypeName))
				{
					// if given a name, break it up along the ' so separate the class from the name
					FString ObjectClass;
					FString ObjectPath;
					if (FPackageName::ParseExportTextPath(ArchetypeName, &ObjectClass, &ObjectPath))
					{
						// find the class
						UClass* ArchetypeClass = (UClass*)StaticFindObject(UClass::StaticClass(), ANY_PACKAGE, *ObjectClass);
						if (ArchetypeClass)
						{
							if (ArchetypeClass->IsChildOf(AActor::StaticClass()))
							{
								// if we had the class, find the archetype
								Archetype = Cast<AActor>(StaticFindObject(ArchetypeClass, ANY_PACKAGE, *ObjectPath));
							}
							else
							{
								Warn->Logf(ELogVerbosity::Warning, TEXT("Invalid archetype specified in subobject definition '%s': %s is not a child of Actor"),
									Str, *ObjectClass);
							}
						}
					}
				}

				// If we're pasting from a class that belongs to a map we need to duplicate the class and use that instead
// 				if (FBlueprintEditorUtils::IsAnonymousBlueprintClass(TempClass))
// 				{
// 					UBlueprint* NewBP = DuplicateObject(CastChecked<UBlueprint>(TempClass->ClassGeneratedBy), World->GetCurrentLevel(), *FString::Printf(TEXT("%s_BPClass"), *ActorUniqueName.ToString()));
// 					if (NewBP)
// 					{
// 						NewBP->ClearFlags(RF_Standalone);
// 
// 						FKismetEditorUtilities::CompileBlueprint(NewBP, false, true);
// 
// 						TempClass = NewBP->GeneratedClass;
// 
// 						// Since we changed the class we can't use an Archetype,
// 						// however that is fine since we will have been editing the CDO anyways
// 						Archetype = nullptr;
// 					}
// 				}

				if (TempClass->IsChildOf(AWorldSettings::StaticClass()))
				{
					// if we see a WorldSettings, then we are importing an entire level, so if we
					// are importing into an existing level, then we should not import the next actor
					// which will be the builder brush
					check(ActorIndex == 0);

					// if we have any actors, then we are importing into an existing level
					if (World->GetCurrentLevel()->Actors.Num())
					{
						check(World->GetCurrentLevel()->Actors[0]->IsA(AWorldSettings::StaticClass()));

						// full level into full level, skip the first two actors
						bShouldSkipImportSpecialActors = true;
					}
				}

				// Get property text.
				FString PropText, PropertyLine;
				while
					(GetEND(&Buffer, TEXT("ACTOR")) == 0
						&& FParse::Line(&Buffer, PropertyLine))
				{
					PropText += *PropertyLine;
					PropText += TEXT("\r\n");
				}

				// If we need to skip the WorldSettings and BuilderBrush, skip the first two actors.  Note that
				// at this point, we already know that we have a WorldSettings and BuilderBrush in the .t3d.
				if (false/*FLevelUtils::IsLevelLocked(World->GetCurrentLevel())*/)
				{
					//UE_LOG(LogEditorFactories, Warning, TEXT("Import actor: The requested operation could not be completed because the level is locked."));
					//FEditorDelegates::OnAssetPostImport.Broadcast(this, nullptr);
					return nullptr;
				}
				else if (!(bShouldSkipImportSpecialActors && ActorIndex < 2))
				{
					// Don't import the default physics volume, as it doesn't have a UModel associated with it
					// and thus will not import properly.
					if (!TempClass->IsChildOf(ADefaultPhysicsVolume::StaticClass()))
					{
						// Create a new actor.
						FActorSpawnParameters SpawnInfo;
						SpawnInfo.Name = ActorUniqueName;
						SpawnInfo.Template = Archetype;
						SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
						//if (GVRSEditor->bIsSimulatingInEditor)
						//{
							// During SIE, we don't want to run construction scripts on a BP until it is completely constructed
						//	SpawnInfo.bDeferConstruction = true;
						//}
						AActor* NewActor = World->SpawnActor(TempClass, nullptr, nullptr, SpawnInfo);

						if (NewActor)
						{
							if (/*GVRSEditor->bGroupingActive && */!Cast<AArmyGroupActor>(NewActor))
							{
								NewActor->SetObjID(ObjID);
								NewActor->SetSynID(SynID);
								bool bGrouped = false;

								AArmyGroupActor** tmpNewGroup = nullptr;
								// We need to add all the objects we selected into groups with new objects that were in their group before.
								FString GroupName;
								if (FParse::Value(Str, TEXT("GroupActor="), GroupName))
								{
									tmpNewGroup = NewGroups.Find(GroupName);
									bGrouped = true;
								}

								// Does the group exist?
								if (tmpNewGroup)
								{
									AArmyGroupActor* NewActorGroup = *tmpNewGroup;

									// Add it to the group.
									NewActorGroup->Add(*NewActor);
								}
								else if (bGrouped)
								{
									// Create a new group and add the actor.
									AArmyGroupActor* SpawnedGroupActor = NewActor->GetWorld()->SpawnActor<AArmyGroupActor>();
									SpawnedGroupActor->Add(*NewActor);

									// Place the group in the map so we can find it later.
									NewGroups.Add(GroupName, SpawnedGroupActor);
									//FActorLabelUtilities::SetActorLabelUnique(SpawnedGroupActor, GroupName);
								}

								// If we're copying a sub-group, add add duplicated group to original parent
								// If we're just copying an actor, only append it to the original parent group if unlocked
								if (Found)
								{
									AArmyGroupActor* FoundParent = AArmyGroupActor::GetParentForActor(Found);
									if (FoundParent && (Found->IsA(AArmyGroupActor::StaticClass()) || !FoundParent->IsLocked()))
									{
										FoundParent->Add(*NewActor);
									}
								}
							}

							// Store the new actor and the text it should be initialized with.
							NewActorMap.Add(NewActor, *PropText);

							// Store the copy to original actor mapping
							MapActors.Add(NewActor, Found);

							// Store the new actor against its source actor name (not the one that may have been made unique)
							if (ActorSourceName != NAME_None)
							{
								NewActorsFNames.Add(ActorSourceName, NewActor);
							}

							// Store the new actor with its parent's FName, and socket FName if applicable
							if (ActorParentName != NAME_None)
							{
								NewActorsAttachmentMap.Add(NewActor, FAttachmentDetail(ActorParentName, ActorParentSocket));
							}
						}
					}
				}

				// increment the number of actors we imported
				ActorIndex++;
			}
		}
		else if (GetBEGIN(&Str, TEXT("MAPPACKAGE")))
		{
			// Get all the text.
			while ((GetEND(&Buffer, TEXT("MAPPACKAGE")) == 0) && FParse::Line(&Buffer, StrLine))
			{
				MapPackageText += *StrLine;
				MapPackageText += TEXT("\r\n");
			}
		}
	}

	// Import actor properties.
	// We do this after creating all actors so that actor references can be matched up.
	AWorldSettings* WorldSettings = World->GetWorldSettings();

// 	if (/*GIsImportingT3D &&*/ (MapPackageText.Len() > 0))
// 	{
// 		UPackageFactory* PackageFactory = NewObject<UPackageFactory>();
// 		check(PackageFactory);
// 
// 		FName NewPackageName(*(RootMapPackage->GetName()));
// 
// 		const TCHAR* MapPkg_BufferStart = *MapPackageText;
// 		const TCHAR* MapPkg_BufferEnd = MapPkg_BufferStart + MapPackageText.Len();
// 		PackageFactory->FactoryCreateText(UPackage::StaticClass(), nullptr, NewPackageName, RF_NoFlags, 0, TEXT("T3D"), MapPkg_BufferStart, MapPkg_BufferEnd, Warn);
// 	}

	// Pass 1: Sort out all the properties on the individual actors
	bool bIsMoveToStreamingLevel = (FCString::Stricmp(Type, TEXT("move")) == 0);
	for (auto& ActorMapElement : NewActorMap)
	{
		AActor* Actor = ActorMapElement.Key;

		// Import properties if the new actor is 
		bool		bActorChanged = false;
		FString*	PropText = &(ActorMapElement.Value);
		if (PropText)
		{
 			if (true)//Actor->ShouldImport(PropText, bIsMoveToStreamingLevel))
 			{
 				//Actor->PreEditChange(nullptr);
 				ImportObjectProperties((uint8*)Actor, **PropText, Actor->GetClass(), Actor, Actor, Warn, 0, INDEX_NONE, NULL, &NewActorsFNames);
 				bActorChanged = true;
 
 				GArmyEditor->SelectActor(Actor, true, false, true);
 			}
 			else // This actor is new, but rejected to import its properties, so just delete...
 			{
 				Actor->Destroy();
 			}
		}
		//else
			//if (!Actor->IsA(AInstancedFoliageActor::StaticClass()))
			//{
				// This actor is old
			//}

		// If this is a newly imported static brush, validate it.  If it's a newly imported dynamic brush, rebuild it.
		// Previously, this just called bspValidateBrush.  However, that caused the dynamic brushes which require a valid BSP tree
		// to be built to break after being duplicated.  Calling RebuildBrush will rebuild the BSP tree from the imported polygons.
		
// 		ABrush* Brush = Cast<ABrush>(Actor);
// 		if (bActorChanged && Brush && Brush->Brush)
// 		{
// 			const bool bIsStaticBrush = Brush->IsStaticBrush();
// 			if (bIsStaticBrush)
// 			{
// 				FArmyBSPOps::bspValidateBrush(Brush->Brush, true, false);
// 			}
// 			else
// 			{
// 				FArmyBSPOps::RebuildBrush(Brush->Brush);
// 			}
// 		}

// 		// Copy brushes' model pointers over to their BrushComponent, to keep compatibility with old T3Ds.
// 		if (Brush && bActorChanged)
// 		{
// 			if (Brush->GetBrushComponent()) // Should always be the case, but not asserting so that old broken content won't crash.
// 			{
// 				Brush->GetBrushComponent()->Brush = Brush->Brush;
// 
// 				// We need to avoid duplicating default/ builder brushes. This is done by destroying all brushes that are CSG_Active and are not
// 				// the default brush in their respective levels.
// 				if (Brush->IsStaticBrush() && Brush->BrushType == Brush_Default)
// 				{
// 					bool bIsDefaultBrush = false;
// 
// 					// Iterate over all levels and compare current actor to the level's default brush.
// 					for (int32 LevelIndex = 0; LevelIndex < World->GetNumLevels(); LevelIndex++)
// 					{
// 						ULevel* Level = World->GetLevel(LevelIndex);
// 						if (Level->GetDefaultBrush() == Brush)
// 						{
// 							bIsDefaultBrush = true;
// 							break;
// 						}
// 					}
// 
// 					// Destroy actor if it's a builder brush but not the default brush in any of the currently loaded levels.
// 					if (!bIsDefaultBrush)
// 					{
// 						World->DestroyActor(Brush);
// 
// 						// Since the actor has been destroyed, skip the rest of this iteration of the loop.
// 						continue;
// 					}
// 				}
// 			}
// 		}
// 
// 		// If the actor was imported . . .
// 		if (bActorChanged)
// 		{
// 			// Let the actor deal with having been imported, if desired.
// 			Actor->PostEditImport();
// 
// 			// Notify actor its properties have changed.
// 			Actor->PostEditChange();
// 		}
	}

	// Pass 2: Sort out any attachment parenting on the new actors now that all actors have the correct properties set
	for (auto It = MapActors.CreateIterator(); It; ++It)
	{
		AActor* const Actor = It.Key();

		// Fixup parenting
		FAttachmentDetail* ActorAttachmentDetail = NewActorsAttachmentMap.Find(Actor);
		if (ActorAttachmentDetail != nullptr)
		{
			AActor* ActorParent = nullptr;
			// Try to find the new copy of the parent
			AActor** NewActorParent = NewActorsFNames.Find(ActorAttachmentDetail->ParentName);
			if (NewActorParent != nullptr)
			{
				ActorParent = *NewActorParent;
			}
			// Try to find an already existing parent
			if (ActorParent == nullptr)
			{
				ActorParent = FindObject<AActor>(World->GetCurrentLevel(), *ActorAttachmentDetail->ParentName.ToString());
			}
			// Parent the actors
			if (ActorParent != nullptr)
			{
				// Make sure our parent isn't selected (would cause GVRSEditor->ParentActors to fail)
				const bool bParentWasSelected = ActorParent->IsSelected();
				if (bParentWasSelected)
				{
					GArmyEditor->SelectActor(ActorParent, false, false, true);
				}

				//GVRSEditor->ParentActors(ActorParent, Actor, ActorAttachmentDetail->SocketName);

				if (bParentWasSelected)
				{
					GArmyEditor->SelectActor(ActorParent, true, false, true);
				}
			}
		}
	}

	// Go through all the groups we added and finalize them.
	for (TMap< FString, AArmyGroupActor* >::TIterator It(NewGroups); It; ++It)
	{
		It.Value()->CenterGroupLocation();
		It.Value()->Lock();
	}

	// Mark us as no longer importing a T3D.
	//GVRSEditor->IsImportingT3D = 0;
	//GIsImportingT3D = false;

	//FEditorDelegates::OnAssetPostImport.Broadcast(this, World);

	return World;
}
