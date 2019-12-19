#pragma once

#include "WFULambdaRunnable.h"
#include "HAL/ThreadSafeBool.h"
#include "AbstructInterface.h"

class IWFUFolderWatchInterface;
class IWFUFileListInterface;

//Struct to Track which delegate is watching files
struct FWatcher
{
	TSharedPtr<IAbstructInterface,ESPMode::ThreadSafe> Delegate;
	
	FString Path;

	TSharedFuture<void> ThreadFuture = nullptr;
	
	FThreadSafeBool ShouldRun = true;
	
};

inline bool operator==(const FWatcher& lhs, const FWatcher& rhs)
{
	return lhs.Delegate == rhs.Delegate;
}

class WINDOWSFILEUTILITY_API FWindowsFileUtilityFunctionLibrary 
{	
		
public:
	static bool DoesFileExist(const FString& FullPath);

	/*Expects full path including name. you can use this function to rename files.*/	
	static bool MoveFileTo(const FString& From, const FString& To);

	/*Expects full path including folder name.*/	
	static bool CreateDirectoryAt(const FString& FullPath);

	/*Deletes file (not directory). Expects full path.*/	
	static bool DeleteFileAt(const FString& FullPath);

	/*Deletes empty folders only. Expects full path.*/	
	static bool DeleteEmptyFolder(const FString& FullPath);

	/*Dangerous function, not exposed to blueprint. */	
	static bool DeleteFolderRecursively(const FString& FullPath);

	/** Watch a folder for change. WatcherDelegate should respond to FolderWatchInterface*/	
	static void WatchFolder(const FString& FullPath, TSharedPtr<IWFUFolderWatchInterface, ESPMode::ThreadSafe> WatcherDelegate);

	/** Stop watching a folder for change. WatcherDelegate should respond to FolderWatchInterface*/	
	static void StopWatchingFolder(const FString& FullPath, TSharedPtr<IWFUFolderWatchInterface, ESPMode::ThreadSafe> WatcherDelegate);

	/** List the contents, expects UFileListInterface*/	
	static void ListContentsOfFolder(const FString& FullPath, TSharedPtr<IWFUFileListInterface, ESPMode::ThreadSafe> ListDelegate);

	//Convenience C++ callback
	static void ListContentsOfFolderToCallback(const FString& FullPath, TFunction<void(const TArray<FString>&, const TArray<FString>&)> OnListCompleteCallback);

	//Todo: add watch folder with threadsafe boolean passthrough
	//static void ListContentsOfFolderToCallback(const FString& FullPath, TFunction<void(const TArray<FString>&, const TArray<FString>&)> OnListCompleteCallback);

private:
	static void WatchFolderOnBgThread(const FString& FullPath, const FWatcher* Watcher);
	static TMap<FString, TArray<FWatcher>> Watchers;
};