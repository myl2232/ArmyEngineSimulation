// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "ArmyLevelExporterT3D.h"
#include "ArmyObjectExporterT3D.h"
#include "EngineUtils.h"
#include "ArmyGroupActor.h"
#include "ArmyEditorEngine.h"
#include "Engine/Selection.h"
#include "ArmyEditor.h"
#include "Model.h"

UArmyLevelExporterT3D::UArmyLevelExporterT3D(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UWorld::StaticClass();
	bText = true;
	PreferredFormatIndex = 0;
	FormatExtension.Add(TEXT("T3D"));
	FormatExtension.Add(TEXT("COPY"));
	FormatDescription.Add(TEXT("Unreal world text"));
	FormatDescription.Add(TEXT("Unreal world text"));
}

bool UArmyLevelExporterT3D::ExportText(const FExportObjectInnerContext* Context, UObject* Object, const TCHAR* Type, FOutputDevice& Ar, FFeedbackContext* Warn, uint32 PortFlags)
{
	UWorld* World = CastChecked<UWorld>(Object);
	APhysicsVolume* DefaultPhysicsVolume = World->GetDefaultPhysicsVolume();

	UnMarkAllObjects(EObjectMark(OBJECTMARK_TagExp | OBJECTMARK_TagImp));

	UPackage* MapPackage = NULL;
	if ((PortFlags & PPF_Copy) == 0)
	{
		// If we are not copying to clipboard, then export objects contained in the map package itself...
		MapPackage = Cast<UPackage>(Object->GetOutermost());
	}

	// this is the top level in the .t3d file
	if (MapPackage)
	{
		Ar.Logf(TEXT("%sBegin Map Name=%s\r\n"), FCString::Spc(TextIndent), *(MapPackage->GetName()));
	}
	else
	{
		Ar.Logf(TEXT("%sBegin Map\r\n"), FCString::Spc(TextIndent));
	}

	// are we exporting all actors or just selected actors?
	bool bAllActors = FCString::Stricmp(Type, TEXT("COPY")) != 0 && !bSelectedOnly;

	TextIndent += 3;

	ULevel* Level;

	// start a new level section
	if (FCString::Stricmp(Type, TEXT("COPY")) == 0)
	{
		// for copy and paste, we want to select actors in the current level
		Level = World->GetCurrentLevel();

		// if we are copy/pasting, then we don't name the level - we paste into the current level
		Ar.Logf(TEXT("%sBegin Level\r\n"), FCString::Spc(TextIndent));

		// mark that we are doing a clipboard copy
		PortFlags |= PPF_Copy;
	}
	else
	{
		// for export, we only want the persistent level
		Level = World->PersistentLevel;

		//@todo seamless if we are exporting only selected, should we export from all levels? or maybe from the current level?

		// if we aren't copy/pasting, then we name the level so that when we import, we get the same level structure
		Ar.Logf(TEXT("%sBegin Level NAME=%s\r\n"), FCString::Spc(TextIndent), *Level->GetName());
	}

	TextIndent += 3;

	// loop through all of the actors just in this level
	for (AActor* Actor : Level->Actors)
	{
		// Don't export the default physics volume, as it doesn't have a UModel associated with it
		// and thus will not import properly.
		if (Actor == (AActor *)DefaultPhysicsVolume)
		{
			continue;
		}
		// Ensure actor is not a group if grouping is disabled and that the actor is currently selected
		if (Actor && !Actor->IsA(AArmyGroupActor::StaticClass()) &&
			(bAllActors || Actor->IsSelected()))
		{
			if (true)//Actor->ShouldExport())
			{
				AActor* ParentActor = Actor->GetAttachParentActor();
				FName SocketName = Actor->GetAttachParentSocketName();
                FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, false);
                Actor->DetachFromActor(DetachRules);

				FString ParentActorString = (ParentActor ? FString::Printf(TEXT(" ParentActor=%s"), *ParentActor->GetName()) : TEXT(""));
				FString SocketNameString = ((ParentActor && SocketName != NAME_None) ? FString::Printf(TEXT(" SocketName=%s"), *SocketName.ToString()) : TEXT(""));
				FString GroupActor = (Actor->GroupActor ? FString::Printf(TEXT(" GroupActor=%s"), *Actor->GroupActor->GetName()) : TEXT(""));
				Ar.Logf(TEXT("%sBegin Actor Class=%s Name=%s ObjID=%d SynID=%d Archetype=%s'%s'%s%s%s") LINE_TERMINATOR,
					FCString::Spc(TextIndent), *Actor->GetClass()->GetPathName(), *Actor->GetName(),
					Actor->GetUniqueID(), Actor->GetStatID(),
					*Actor->GetArchetype()->GetClass()->GetPathName(), *Actor->GetArchetype()->GetPathName(), *ParentActorString, *SocketNameString, *GroupActor);

				ExportRootScope = Actor;
				ExportObjectInner(Context, Actor, Ar, PortFlags | PPF_ExportsNotFullyQualified);
				ExportRootScope = nullptr;

				Ar.Logf(TEXT("%sEnd Actor\r\n"), FCString::Spc(TextIndent));
				Actor->AttachToActor(ParentActor, FAttachmentTransformRules::KeepWorldTransform, SocketName);
			}
			else
			{
				GArmyEditor->GetSelectedActors()->Deselect(Actor);
			}
		}
	}

	TextIndent -= 3;

	Ar.Logf(TEXT("%sEnd Level\r\n"), FCString::Spc(TextIndent));

	TextIndent -= 3;

	// Export information about the first selected surface in the map.  Used for copying/pasting
	// information from poly to poly.
	Ar.Logf(TEXT("%sBegin Surface\r\n"), FCString::Spc(TextIndent));
	TCHAR TempStr[256];
	const UModel* Model = World->GetModel();
	for (const FBspSurf& Poly : Model->Surfs)
	{
		if (Poly.PolyFlags & PF_Selected)
		{
			Ar.Logf(TEXT("%sTEXTURE=%s\r\n"), FCString::Spc(TextIndent + 3), *Poly.Material->GetPathName());
			Ar.Logf(TEXT("%sBASE      %s\r\n"), FCString::Spc(TextIndent + 3), SetFVECTOR(TempStr, &(Model->Points[Poly.pBase])));
			Ar.Logf(TEXT("%sTEXTUREU  %s\r\n"), FCString::Spc(TextIndent + 3), SetFVECTOR(TempStr, &(Model->Vectors[Poly.vTextureU])));
			Ar.Logf(TEXT("%sTEXTUREV  %s\r\n"), FCString::Spc(TextIndent + 3), SetFVECTOR(TempStr, &(Model->Vectors[Poly.vTextureV])));
			Ar.Logf(TEXT("%sNORMAL    %s\r\n"), FCString::Spc(TextIndent + 3), SetFVECTOR(TempStr, &(Model->Vectors[Poly.vNormal])));
			Ar.Logf(TEXT("%sPOLYFLAGS=%d\r\n"), FCString::Spc(TextIndent + 3), Poly.PolyFlags);
			break;
		}
	}
	Ar.Logf(TEXT("%sEnd Surface\r\n"), FCString::Spc(TextIndent));

	Ar.Logf(TEXT("%sEnd Map\r\n"), FCString::Spc(TextIndent));


	return 1;
}

void UArmyLevelExporterT3D::ExportComponentExtra(const FExportObjectInnerContext* Context, const TArray<UActorComponent*>& Components, FOutputDevice& Ar, uint32 PortFlags)
{
	// 	for (UActorComponent* ActorComponent : Components)
	// 	{
	// 		if (ActorComponent != nullptr && ActorComponent->GetWorld() != nullptr)
	// 		{
	// 			// Go through all FoliageActors in the world, since we support cross-level bases
	// 			for (TActorIterator<AInstancedFoliageActor> It(ActorComponent->GetWorld()); It; ++It)
	// 			{
	// 				AInstancedFoliageActor* IFA = *It;
	// 				if (!IFA->IsPendingKill())
	// 				{
	// 					auto FoliageInstanceMap = IFA->GetInstancesForComponent(ActorComponent);
	// 					for (const auto& MapEntry : FoliageInstanceMap)
	// 					{
	// 						Ar.Logf(TEXT("%sBegin Foliage FoliageType=%s Component=%s%s"), FCString::Spc(TextIndent), *MapEntry.Key->GetPathName(), *ActorComponent->GetName(), LINE_TERMINATOR);
	// 						for (const FFoliageInstancePlacementInfo* Inst : MapEntry.Value)
	// 						{
	// 							Ar.Logf(TEXT("%sLocation=%f,%f,%f Rotation=%f,%f,%f PreAlignRotation=%f,%f,%f DrawScale3D=%f,%f,%f Flags=%u%s"), FCString::Spc(TextIndent + 3),
	// 								Inst->Location.X, Inst->Location.Y, Inst->Location.Z,
	// 								Inst->Rotation.Pitch, Inst->Rotation.Yaw, Inst->Rotation.Roll,
	// 								Inst->PreAlignRotation.Pitch, Inst->PreAlignRotation.Yaw, Inst->PreAlignRotation.Roll,
	// 								Inst->DrawScale3D.X, Inst->DrawScale3D.Y, Inst->DrawScale3D.Z,
	// 								Inst->Flags,
	// 								LINE_TERMINATOR);
	// 						}
	// 
	// 						Ar.Logf(TEXT("%sEnd Foliage%s"), FCString::Spc(TextIndent), LINE_TERMINATOR);
	// 					}
	// 				}
	// 			}
	// 		}
	// 	}
}

void UArmyLevelExporterT3D::ExportPackageObject(FExportPackageParams& ExpPackageParams) {}
void UArmyLevelExporterT3D::ExportPackageInners(FExportPackageParams& ExpPackageParams) {}

/*------------------------------------------------------------------------------
UObjectExporterT3D implementation.
------------------------------------------------------------------------------*/
UArmyObjectExporterT3D::UArmyObjectExporterT3D(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UObject::StaticClass();
	bText = true;
	PreferredFormatIndex = 0;
	FormatExtension.Add(TEXT("T3D"));
	FormatExtension.Add(TEXT("COPY"));
	FormatDescription.Add(TEXT("Unreal object text"));
	FormatDescription.Add(TEXT("Unreal object text"));
}

bool UArmyObjectExporterT3D::ExportText(const FExportObjectInnerContext* Context, UObject* Object, const TCHAR* Type, FOutputDevice& Ar, FFeedbackContext* Warn, uint32 PortFlags)
{
	EmitBeginObject(Ar, Object, PortFlags);
	ExportObjectInner(Context, Object, Ar, PortFlags);
	EmitEndObject(Ar);

	return true;
}

//#endif