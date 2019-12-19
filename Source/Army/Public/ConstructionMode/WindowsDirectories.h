// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"

/** The different directory identifiers */

namespace MEFileDialogFlags
{
	enum Type
	{
		None = 0x00, // No flags
		Multiple = 0x01  // Allow multiple file selections
	};
}

namespace ELastWindowsDirectory
{
	enum Type
	{
		UNR,
		BRUSH,
		FBX,
		FBX_ANIM,
		GENERIC_IMPORT,
		GENERIC_EXPORT,
		GENERIC_OPEN,
		GENERIC_SAVE,
		MESH_IMPORT_EXPORT,
		WORLD_ROOT,
		LEVEL,
		PROJECT,
		NEW_ASSET,
		MAX
	};
};

namespace GreatPlanFileDialog
{
	bool SaveFile(const FString& Title, const FString& FileTypes, FString& InOutLastPath, const FString& DefaultFile, FString& OutFilename);
}

class XR_API FWindowsDirectories
{
public:
	static FWindowsDirectories& Get();

	/** Initializes the "LastDir" array with default directories for loading/saving files */
	void LoadLastDirectories();

	/** Writes the current "LastDir" array back out to the config files */
	void SaveLastDirectories();

	/**
	 *	Fetches the last directory used for the specified type
	 *
	 *	@param	InLastDir	the identifier for the directory type
	 *	@return	FString		the string that was last set
	 */
	FString GetLastDirectory(ELastWindowsDirectory::Type InLastDir ) const;

	
	/**
	 *	Sets the last directory used for the specified type
	 *
	 *	@param	InLastDir	the identifier for the directory type
	 *	@param	InLastStr	the string to set it as
	 */
	void SetLastDirectory(ELastWindowsDirectory::Type InLastDir, const FString& InLastStr );


private:
	/** Array of the last directories used for various editor windows */
	FString LastDir[ELastWindowsDirectory::MAX];
};
