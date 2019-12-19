// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "AbstructInterface.h"

class WINDOWSFILEUTILITY_API IWFUFileListInterface : public IAbstructInterface
{
public:
	IWFUFileListInterface()
		:IAbstructInterface(EDelegateInterfaceType::LIST_INTERFACE)
	{

	}

	/**
	* Called when a file has been found inside the folder of choice
	* @param FileName of the found file.
	* @param Size in bytes of the found file.
	* @param FilePath of the file that was found
	*/
	virtual void OnListFileFound(const FString& FileName, int32 ByteCount, const FString& FilePath) = 0;

	/**
	* Called when a directory has been found inside the folder of choice
	* @param DirectoryName of the found directory.
	* @param FilePath of the file that was found
	*/	
	virtual  void OnListDirectoryFound(const FString& DirectoryName, const FString& FilePath) = 0;

	/**
	* Called when the listing operation has completed.
	* @param DirectoryPath Path of the directory
	* @param Files array of files found
	*/	
	virtual void OnListDone(const FString& DirectoryPath, const TArray<FString>& Files, const TArray<FString>& Folders) = 0;
};