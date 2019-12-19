// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "WindowsDirectories.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "Misc/ConfigCacheIni.h"
#include "Framework/Application/SlateApplication.h"
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"

FWindowsDirectories& FWindowsDirectories::Get()
{
	static FWindowsDirectories Directories;
	return Directories;
}

namespace GreatPlanFileDialog
{
	/**
	* @param Title                  The title of the dialog
	* @param FileTypes              Filter for which file types are accepted and should be shown
	* @param InOutLastPath          Keep track of the last location from which the user attempted an import
	* @param DefaultFile            Default file name to use for saving.
	* @param OutOpenFilenames       The list of filenames that the user attempted to open
	*
	* @return true if the dialog opened successfully and the user accepted; false otherwise.
	*/
	bool SaveFile(const FString& Title, const FString& FileTypes, FString& InOutLastPath, const FString& DefaultFile, FString& OutFilename)
	{
		OutFilename = FString();

		IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
		bool bFileChosen = false;
		TArray<FString> OutFilenames;
		if (DesktopPlatform)
		{
			bFileChosen = DesktopPlatform->SaveFileDialog(
				FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
				Title,
				InOutLastPath,
				DefaultFile,
				FileTypes,
				MEFileDialogFlags::None,
				OutFilenames
			);
		}

		bFileChosen = (OutFilenames.Num() > 0);

		if (bFileChosen)
		{
			// User successfully chose a file; remember the path for the next time the dialog opens.
			InOutLastPath = OutFilenames[0];
			OutFilename = OutFilenames[0];
		}

		return bFileChosen;
	}
}


void FWindowsDirectories::LoadLastDirectories()
{
	for( int32 CurDirectoryIndex = 0; CurDirectoryIndex < ARRAY_COUNT( LastDir ); ++CurDirectoryIndex )
	{
		LastDir[CurDirectoryIndex].Reset();
	}

	// NOTE: We append a "2" to the section name to enforce backwards compatibility.  "Directories" is deprecated.
	GConfig->GetString( TEXT("Directories2"), TEXT("UNR"),				LastDir[ELastWindowsDirectory::UNR],					GEditorPerProjectIni );
	GConfig->GetString( TEXT("Directories2"), TEXT("BRUSH"),			LastDir[ELastWindowsDirectory::BRUSH],				GEditorPerProjectIni );
	GConfig->GetString( TEXT("Directories2"), TEXT("FBX"),				LastDir[ELastWindowsDirectory::FBX],					GEditorPerProjectIni );
	GConfig->GetString( TEXT("Directories2"), TEXT("FBXAnim"),			LastDir[ELastWindowsDirectory::FBX_ANIM],			GEditorPerProjectIni );
	GConfig->GetString( TEXT("Directories2"), TEXT("GenericImport"),	LastDir[ELastWindowsDirectory::GENERIC_IMPORT],		GEditorPerProjectIni );
	GConfig->GetString( TEXT("Directories2"), TEXT("GenericExport"),	LastDir[ELastWindowsDirectory::GENERIC_EXPORT],		GEditorPerProjectIni );
	GConfig->GetString( TEXT("Directories2"), TEXT("GenericOpen"),		LastDir[ELastWindowsDirectory::GENERIC_OPEN],		GEditorPerProjectIni );
	GConfig->GetString( TEXT("Directories2"), TEXT("GenericSave"),		LastDir[ELastWindowsDirectory::GENERIC_SAVE],		GEditorPerProjectIni );
	GConfig->GetString( TEXT("Directories2"), TEXT("MeshImportExport"),	LastDir[ELastWindowsDirectory::MESH_IMPORT_EXPORT],	GEditorPerProjectIni );
	GConfig->GetString( TEXT("Directories2"), TEXT("WorldRoot"),		LastDir[ELastWindowsDirectory::WORLD_ROOT],			GEditorPerProjectIni );
	GConfig->GetString( TEXT("Directories2"), TEXT("Level"),			LastDir[ELastWindowsDirectory::LEVEL],					GEditorPerProjectIni );
	GConfig->GetString( TEXT("Directories2"), TEXT("Project"),			LastDir[ELastWindowsDirectory::PROJECT],				GEditorPerProjectIni );

	// Set up some defaults if they're note defined in the ini
	const FString DefaultDir = FPaths::ProjectContentDir();
	for( int32 CurDirectoryIndex = 0; CurDirectoryIndex < ARRAY_COUNT( LastDir ); ++CurDirectoryIndex )
	{
		if (LastDir[ CurDirectoryIndex ].IsEmpty())
		{
			// Default all directories to the game content folder
			if (CurDirectoryIndex == ELastWindowsDirectory::LEVEL)
			{
				const FString DefaultMapDir = FPaths::ProjectContentDir() / TEXT("Maps");
				if( IFileManager::Get().DirectoryExists( *DefaultMapDir ) )
				{
					LastDir[CurDirectoryIndex] = DefaultMapDir;
					continue;
				}
			}
			else if (CurDirectoryIndex == ELastWindowsDirectory::PROJECT)
			{
				LastDir[CurDirectoryIndex] = FPaths::RootDir();
				continue;
			}

			// Set to the default dir
			LastDir[ CurDirectoryIndex ] = DefaultDir;
		}
	}
}

/** Writes the current "LastDir" array back out to the config files */
void FWindowsDirectories::SaveLastDirectories()
{
	// Save out default file directories
	GConfig->SetString( TEXT("Directories2"), TEXT("UNR"),				*LastDir[ELastWindowsDirectory::UNR],				GEditorPerProjectIni );
	GConfig->SetString( TEXT("Directories2"), TEXT("BRUSH"),			*LastDir[ELastWindowsDirectory::BRUSH],				GEditorPerProjectIni );
	GConfig->SetString( TEXT("Directories2"), TEXT("FBX"),				*LastDir[ELastWindowsDirectory::FBX],				GEditorPerProjectIni );
	GConfig->SetString( TEXT("Directories2"), TEXT("FBXAnim"),			*LastDir[ELastWindowsDirectory::FBX_ANIM],			GEditorPerProjectIni );
	GConfig->SetString( TEXT("Directories2"), TEXT("GenericImport"),	*LastDir[ELastWindowsDirectory::GENERIC_IMPORT],		GEditorPerProjectIni );
	GConfig->SetString( TEXT("Directories2"), TEXT("GenericExport"),	*LastDir[ELastWindowsDirectory::GENERIC_EXPORT],		GEditorPerProjectIni );
	GConfig->SetString( TEXT("Directories2"), TEXT("GenericOpen"),		*LastDir[ELastWindowsDirectory::GENERIC_OPEN],		GEditorPerProjectIni );
	GConfig->SetString( TEXT("Directories2"), TEXT("GenericSave"),		*LastDir[ELastWindowsDirectory::GENERIC_SAVE],		GEditorPerProjectIni );
	GConfig->SetString( TEXT("Directories2"), TEXT("MeshImportExport"),	*LastDir[ELastWindowsDirectory::MESH_IMPORT_EXPORT],	GEditorPerProjectIni );
	GConfig->SetString( TEXT("Directories2"), TEXT("WorldRoot"),		*LastDir[ELastWindowsDirectory::WORLD_ROOT],			GEditorPerProjectIni );
	GConfig->SetString( TEXT("Directories2"), TEXT("Level"),			*LastDir[ELastWindowsDirectory::LEVEL],				GEditorPerProjectIni );
	GConfig->SetString( TEXT("Directories2"), TEXT("Project"),			*LastDir[ELastWindowsDirectory::PROJECT],				GEditorPerProjectIni );
}

FString FWindowsDirectories::GetLastDirectory( const ELastWindowsDirectory::Type InLastDir ) const
{
	if ( InLastDir >= 0 && InLastDir < ARRAY_COUNT( LastDir ) )
	{
		return LastDir[InLastDir];
	}
	return FPaths::ProjectContentDir();
}


void FWindowsDirectories::SetLastDirectory( const ELastWindowsDirectory::Type InLastDir, const FString& InLastStr )
{
	if ( InLastDir >= 0 && InLastDir < ARRAY_COUNT( LastDir ) )
	{
		LastDir[InLastDir] = InLastStr;
	}
}
