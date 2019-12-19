// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstructInterface.h"

class WINDOWSFILEUTILITY_API IWFUFolderWatchInterface : public IAbstructInterface
{
public:
	IWFUFolderWatchInterface()
		:IAbstructInterface(EDelegateInterfaceType::WATCH_INTERFACE) {}
public:

	/**
	* Called when a file inside the folder has changed
	* @param FilePath Path of the file that has changed
	*/	
	virtual void OnFileChanged(const FString& FileName, const FString& FilePath) = 0;

	/**
	* Called when a directory inside the folder has changed
	* @param FilePath Path of the file that has changed
	*/	
	virtual void OnDirectoryChanged(const FString& DirectoryName, const FString& DirectoryPath) = 0;
};