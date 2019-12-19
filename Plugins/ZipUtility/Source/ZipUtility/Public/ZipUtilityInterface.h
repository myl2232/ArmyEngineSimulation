// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

enum EZipUtilityCompletionState
{
	SUCCESS,
	FAILURE_NOT_FOUND,
	FAILURE_UNKNOWN
};


class ZIPUTILITY_API IZipUtilityInterface
{

public:

	/**
	* Called during process as it completes. Currently updates on per file progress.
	* @param percentage - percentage done
	*/	
	virtual void OnProgress(const FString& archive, float percentage, int32 bytes) = 0;

	/**
	* Called when whole process is complete (e.g. unzipping completed on archive)
	*/	
	virtual void OnDone(const FString& archive, EZipUtilityCompletionState CompletionState) = 0;

	/**
	* Called at beginning of process (NB this only supports providing size information for up to 2gb) TODO: fix 32bit BP size issue
	*/	
	virtual void OnStartProcess(const FString& archive, int32 bytes) = 0;

	/**
	* Called when file process is complete
	* @param path - path of the file that finished
	*/	
	virtual void OnFileDone(const FString& archive, const FString& file) = 0;


	/**
	* Called when a file is found in the archive (e.g. listing the entries in the archive)
	* @param path - path of file
	* @param size - compressed size
	*/	
	virtual void OnFileFound(const FString& archive, const FString& file, int32 size) = 0;
};