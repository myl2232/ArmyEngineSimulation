#include "ZipFileFunctionLibrary.h"
#include "ZipUtilityPrivatePCH.h"
#include "ZipFileFunctionInternalCallback.h"
#include "ListCallback.h"
#include "ProgressCallback.h"
#include "Interfaces/IPluginManager.h"
#include "WFULambdaRunnable.h"
#include "ZULambdaDelegate.h"
#include "SevenZipCallbackHandler.h"
#include "WindowsFileUtilityFunctionLibrary.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "HAL/PlatformFile.h"

#include "7zpp.h"
#include "WindowsFileUtilityFunctionLibrary.h"

using namespace SevenZip;

//Private Namespace
namespace{

	//Threaded Lambda convenience wrappers - Task graph is only suitable for short duration lambdas, but doesn't incur thread overhead
	FGraphEventRef RunLambdaOnAnyThread(TFunction< void()> InFunction)
	{
		return FFunctionGraphTask::CreateAndDispatchWhenReady(InFunction, TStatId(), nullptr, ENamedThreads::AnyThread);
	}

	//Uses proper threading, for any task that may run longer than about 2 seconds.
	void RunLongLambdaOnAnyThread(TFunction< void()> InFunction)
	{
		FWFULambdaRunnable::RunLambdaOnBackGroundThread(InFunction);
	}

	// Run the lambda on the queued threadpool
	IQueuedWork* RunLambdaOnThreadPool(TFunction< void()> InFunction)
	{
		return FWFULambdaRunnable::AddLambdaToQueue(InFunction);
	}

	//Private static vars
	SevenZipLibrary SZLib;

	//Utility functions
	FString PluginRootFolder()
	{		
		return FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("ZipUtility"));
	}

	FString DLLPath()
	{	   	   
#if _WIN64

		FString PlatformString = FString(TEXT("Win64"));
#else
		FString PlatformString = FString(TEXT("Win32"));
#endif
		//Swap these to change which license you wish to fall under for zip-utility

		FString DLLString = FString("7z.dll");		//Using 7z.dll: GNU LGPL + unRAR restriction
		//FString dllString = FString("7za.dll");	//Using 7za.dll: GNU LGPL license, crucially doesn't support .zip out of the box

		return FPaths::ConvertRelativePathToFull(FPaths::Combine(*PluginRootFolder(), TEXT("ThirdParty/7zpp/dll"), *PlatformString, *DLLString));
	}

	FString ReversePathSlashes(FString forwardPath)
	{
		return forwardPath.Replace(TEXT("/"), TEXT("\\"));
	}

	bool IsValidDirectory(FString& Directory, FString& FileName, const FString& Path)
	{
		bool Found = Path.Split(TEXT("/"), &Directory, &FileName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		//try a back split
		if (!Found)
		{
			Found = Path.Split(TEXT("\\"), &Directory, &FileName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		}

		//No valid Directory found
		if (!Found)
			return false;
		else
			return true;
	}

	SevenZip::CompressionLevelEnum libZipLevelFromUELevel(EZipUtilityCompressionLevel ueLevel) {
		switch (ueLevel)
		{
		case COMPRESSION_LEVEL_NONE:
			return SevenZip::CompressionLevel::None;
		case COMPRESSION_LEVEL_FAST:
			return SevenZip::CompressionLevel::Fast;
		case COMPRESSION_LEVEL_NORMAL:
			return SevenZip::CompressionLevel::Normal;
		default:
			return SevenZip::CompressionLevel::None;
		}
	}

	SevenZip::CompressionFormatEnum libZipFormatFromUEFormat(EZipUtilityCompressionFormat UeFormat) {
		switch (UeFormat)
		{
		case COMPRESSION_FORMAT_UNKNOWN:
			return CompressionFormat::Unknown;
		case COMPRESSION_FORMAT_SEVEN_ZIP:
			return CompressionFormat::SevenZip;
		case COMPRESSION_FORMAT_ZIP:
			return CompressionFormat::Zip;
		case COMPRESSION_FORMAT_GZIP:
			return CompressionFormat::GZip;
		case COMPRESSION_FORMAT_BZIP2:
			return CompressionFormat::BZip2;
		case COMPRESSION_FORMAT_RAR:
			return CompressionFormat::Rar;
		case COMPRESSION_FORMAT_TAR:
			return CompressionFormat::Tar;
		case COMPRESSION_FORMAT_ISO:
			return CompressionFormat::Iso;
		case COMPRESSION_FORMAT_CAB:
			return CompressionFormat::Cab;
		case COMPRESSION_FORMAT_LZMA:
			return CompressionFormat::Lzma;
		case COMPRESSION_FORMAT_LZMA86:
			return CompressionFormat::Lzma86;
		default:
			return CompressionFormat::Unknown;
		}
	}

	FString defaultExtensionFromUEFormat(EZipUtilityCompressionFormat ueFormat) 
	{
		switch (ueFormat)
		{
		case COMPRESSION_FORMAT_UNKNOWN:
			return FString(TEXT(".dat"));
		case COMPRESSION_FORMAT_SEVEN_ZIP:
			return FString(TEXT(".7z"));
		case COMPRESSION_FORMAT_ZIP:
			return FString(TEXT(".zip"));
		case COMPRESSION_FORMAT_GZIP:
			return FString(TEXT(".gz"));
		case COMPRESSION_FORMAT_BZIP2:
			return FString(TEXT(".bz2"));
		case COMPRESSION_FORMAT_RAR:
			return FString(TEXT(".rar"));
		case COMPRESSION_FORMAT_TAR:
			return FString(TEXT(".tar"));
		case COMPRESSION_FORMAT_ISO:
			return FString(TEXT(".iso"));
		case COMPRESSION_FORMAT_CAB:
			return FString(TEXT(".cab"));
		case COMPRESSION_FORMAT_LZMA:
			return FString(TEXT(".lzma"));
		case COMPRESSION_FORMAT_LZMA86:
			return FString(TEXT(".lzma86"));
		default:
			return FString(TEXT(".dat"));
		}
	}

	using namespace std;

	

	//Background Thread convenience functions
	TSharedPtr<FZipOperation, ESPMode::ThreadSafe> UnzipFilesOnBGThreadWithFormat(const TArray<int32> FileIndices, const FString& ArchivePath, const FString& DestinationDirectory,TSharedPtr<IZipUtilityInterface> ProgressDelegate, EZipUtilityCompressionFormat Format)
	{
		TSharedPtr<FZipOperation, ESPMode::ThreadSafe> ZipOperation = MakeShareable(new FZipOperation());
		
		IQueuedWork* Work = RunLambdaOnThreadPool([ProgressDelegate, FileIndices, ArchivePath, DestinationDirectory, Format, ZipOperation] 
		{
			IPlatformFile & Platform = FPlatformFileManager::Get().GetPlatformFile();
			if (Platform.DirectoryExists(*DestinationDirectory))
			{
				Platform.DeleteDirectoryRecursively(*DestinationDirectory);
				Platform.CreateDirectory(*DestinationDirectory);
			}

			FSevenZipCallbackHandler PrivateCallback;
			PrivateCallback.ProgressDelegate = ProgressDelegate;
			ZipOperation->SetCallbackHandler(&PrivateCallback);
			
			SevenZipExtractor Extractor(SZLib, *ArchivePath);

			if (Format == COMPRESSION_FORMAT_UNKNOWN)
			{
				if (!Extractor.DetectCompressionFormat())
				{
					UE_LOG(LogTemp, Log, TEXT("auto-compression detection did not succeed, passing in unknown format to 7zip library."));
				}
			}
			else
			{
				Extractor.SetCompressionFormat(libZipFormatFromUEFormat(Format));
			}			
			const int32 NumberFiles = FileIndices.Num(); 
			unsigned int* Indices = new unsigned int[NumberFiles]; 

			for (int32 idx = 0; idx < NumberFiles; idx++)
			{
				Indices[idx] = FileIndices[idx]; 
			}
						
			Extractor.ExtractFilesFromArchive(Indices, NumberFiles, *DestinationDirectory, &PrivateCallback);			
			delete Indices;			
			ZipOperation->SetCallbackHandler(nullptr);
		});

		ZipOperation->SetThreadPoolWorker(Work);
		return ZipOperation;
	}
	
	TSharedPtr<FZipOperation, ESPMode::ThreadSafe> UnzipOnBGThreadWithFormat(const FString& ArchivePath, const FString& DestinationDirectory, TSharedPtr<IZipUtilityInterface> ProgressDelegate, EZipUtilityCompressionFormat Format)
	{
		TSharedPtr<FZipOperation, ESPMode::ThreadSafe> ZipOperation = MakeShareable(new FZipOperation());
		IQueuedWork* Work = RunLambdaOnThreadPool([ProgressDelegate, ArchivePath, DestinationDirectory, Format, ZipOperation] 
		{
			FSevenZipCallbackHandler PrivateCallback;
			PrivateCallback.ProgressDelegate = ProgressDelegate;
			ZipOperation->SetCallbackHandler(&PrivateCallback);			
			SevenZipExtractor Extractor(SZLib, *ArchivePath);
			if (Format == COMPRESSION_FORMAT_UNKNOWN)
			{
				if (!Extractor.DetectCompressionFormat())
				{
					UE_LOG(LogTemp, Log, TEXT("auto-compression detection did not succeed, passing in unknown format to 7zip library."));
				}
			}
			else
			{
				Extractor.SetCompressionFormat(libZipFormatFromUEFormat(Format));
			}

			Extractor.ExtractArchive(*DestinationDirectory, &PrivateCallback);			
			ZipOperation->SetCallbackHandler(nullptr);
		});

		ZipOperation->SetThreadPoolWorker(Work);
		return ZipOperation;
	}

	void ListOnBGThread(const FString& Path, const FString& Directory, const TSharedPtr<IZipUtilityInterface> ListDelegate, EZipUtilityCompressionFormat Format)
	{
		//RunLongLambdaOnAnyThread - this shouldn't take long, but if it lags, swap the lambda methods
		RunLambdaOnAnyThread([ListDelegate, Path, Format, Directory] {
			FSevenZipCallbackHandler PrivateCallback;
			PrivateCallback.ProgressDelegate = ListDelegate;
			SevenZipLister Lister(SZLib, *Path);

			if (Format == COMPRESSION_FORMAT_UNKNOWN) 
			{
				if (!Lister.DetectCompressionFormat())
				{
					UE_LOG(LogTemp, Log, TEXT("auto-compression detection did not succeed, passing in unknown format to 7zip library."));
				}
			}
			else
			{
				Lister.SetCompressionFormat(libZipFormatFromUEFormat(Format));
			}

			if (!Lister.ListArchive(&PrivateCallback))
			{
				// If ListArchive returned false, it was most likely because the compression format was unsupported
				// Call OnDone with a failure message, make sure to call this on the game thread.
				if (ListDelegate.IsValid())
				{
					//UE_LOG(LogClass, Warning, TEXT("ZipUtility: Unknown failure for list operation on %s"), *Path);
					FZipFileFunctionLibrary::RunLambdaOnGameThread([ListDelegate, Path] 
					{
						ListDelegate->OnDone(*Path, EZipUtilityCompletionState::FAILURE_UNKNOWN);
					});
				}
			}
		});
	}

	TSharedPtr<FZipOperation, ESPMode::ThreadSafe> ZipOnBGThread(const FString& Path, const FString& FileName, const FString& Directory, const TSharedPtr<IZipUtilityInterface> ProgressDelegate, EZipUtilityCompressionFormat UeCompressionformat, EZipUtilityCompressionLevel UeCompressionlevel)
	{
		
		TSharedPtr<FZipOperation, ESPMode::ThreadSafe> ZipOperation = MakeShareable(new FZipOperation());
		
		IQueuedWork* Work = RunLambdaOnThreadPool([ProgressDelegate, FileName, Path, UeCompressionformat, UeCompressionlevel, Directory, ZipOperation] 
		{
			FSevenZipCallbackHandler PrivateCallback;
			PrivateCallback.ProgressDelegate = ProgressDelegate;
			ZipOperation->SetCallbackHandler(&PrivateCallback);

			//Set the zip format
			EZipUtilityCompressionFormat UeFormat = UeCompressionformat;

			if (UeFormat == COMPRESSION_FORMAT_UNKNOWN) 
			{
				UeFormat = COMPRESSION_FORMAT_ZIP;
			}
			//Disallow creating .rar archives as per unrar restriction, this won't work anyway so redirect to 7z
			else if (UeFormat == COMPRESSION_FORMAT_RAR) 
			{
				//UE_LOG(LogClass, Warning, TEXT("ZipUtility: Rar compression not supported for creating archives, re-targeting as 7z."));
				UeFormat = COMPRESSION_FORMAT_SEVEN_ZIP;
			}
			
			//concatenate the output filename
			FString OutputFileName = FString::Printf(TEXT("%s/%s%s"), *Directory, *FileName, *defaultExtensionFromUEFormat(UeFormat));
			//UE_LOG(LogClass, Log, TEXT("\noutputfile is: <%s>\n path is: <%s>"), *outputFileName, *path);
			
			SevenZipCompressor compressor(SZLib, *ReversePathSlashes(OutputFileName));
			compressor.SetCompressionFormat(libZipFormatFromUEFormat(UeFormat));
			compressor.SetCompressionLevel(libZipLevelFromUELevel(UeCompressionlevel));

			if (PathIsDirectory(*Path))
			{
				//UE_LOG(LogClass, Log, TEXT("Compressing Folder"));
				compressor.CompressDirectory(*ReversePathSlashes(Path), &PrivateCallback);
			}
			else
			{
				//UE_LOG(LogClass, Log, TEXT("Compressing File"));
				compressor.CompressFile(*ReversePathSlashes(Path), &PrivateCallback);
			}

			// Null out the callback handler
			ZipOperation->SetCallbackHandler(nullptr);
			//Todo: expand to support zipping up contents of current folder
			//compressor.CompressFiles(*ReversePathSlashes(path), TEXT("*"),  &PrivateCallback);
		});
		ZipOperation->SetThreadPoolWorker(Work);
 


		return ZipOperation;
	}

}//End private namespace

void FZipFileFunctionLibrary::Startup()
{
	SZLib.Load(*DLLPath());
}

void FZipFileFunctionLibrary::ShutDown()
{
	SZLib.Free();
}

void FZipFileFunctionLibrary::MoveFileTo(const FString& From, const FString& To)
{
 	FWindowsFileUtilityFunctionLibrary::MoveFileTo(From, To);
}

bool FZipFileFunctionLibrary::UnzipFileNamed(const FString& archivePath, const FString& Name, TSharedPtr<IZipUtilityInterface> ZipUtilityInterfaceDelegate, EZipUtilityCompressionFormat format /*= COMPRESSION_FORMAT_UNKNOWN*/)
{	
	TSharedPtr<FZipFileFunctionInternalCallback> InternalCallback = MakeShareable(new FZipFileFunctionInternalCallback());
	InternalCallback->SetCallback(Name, ZipUtilityInterfaceDelegate, format);
	ListFilesInArchive(archivePath, InternalCallback, format);
	return true;
}

bool FZipFileFunctionLibrary::UnzipFileNamedTo(const FString& archivePath, const FString& Name, const FString& destinationPath, TSharedPtr<IZipUtilityInterface> ZipUtilityInterfaceDelegate, EZipUtilityCompressionFormat format /*= COMPRESSION_FORMAT_UNKNOWN*/)
{
	TSharedPtr<FZipFileFunctionInternalCallback> InternalCallback = MakeShareable(new FZipFileFunctionInternalCallback());
	InternalCallback->SetCallback(Name, destinationPath, ZipUtilityInterfaceDelegate, format);
	ListFilesInArchive(archivePath, InternalCallback, format);

	return true;
}

TSharedPtr<FZipOperation, ESPMode::ThreadSafe> FZipFileFunctionLibrary::UnzipFilesTo(const TArray<int32> fileIndices, const FString & archivePath, const FString & destinationPath, TSharedPtr<IZipUtilityInterface> ZipUtilityInterfaceDelegate, EZipUtilityCompressionFormat format)
{
	return UnzipFilesOnBGThreadWithFormat(fileIndices, archivePath, destinationPath, ZipUtilityInterfaceDelegate, format);
}

TSharedPtr<FZipOperation, ESPMode::ThreadSafe> FZipFileFunctionLibrary::UnzipFiles(const TArray<int32> fileIndices, const FString & ArchivePath, TSharedPtr<IZipUtilityInterface> ZipUtilityInterfaceDelegate, EZipUtilityCompressionFormat format)
{
	FString Directory;
	FString FileName;

	//Check Directory validity
	if (!IsValidDirectory(Directory, FileName, ArchivePath))
	{
		return nullptr;
	}
		
	if (fileIndices.Num() == 0)
	{
		return nullptr;
	}
	return UnzipFilesTo(fileIndices, ArchivePath, Directory, ZipUtilityInterfaceDelegate, format);
}

TSharedPtr<FZipOperation, ESPMode::ThreadSafe> FZipFileFunctionLibrary::Unzip(const FString& ArchivePath, TSharedPtr<IZipUtilityInterface> ZipUtilityInterfaceDelegate, EZipUtilityCompressionFormat Format /*= COMPRESSION_FORMAT_UNKNOWN*/)
{
	FString Directory;
	FString FileName;

	//Check Directory validity
	if (!IsValidDirectory(Directory, FileName, ArchivePath) || !FWindowsFileUtilityFunctionLibrary::DoesFileExist(ArchivePath))
	{
		ZipUtilityInterfaceDelegate->OnDone(ArchivePath, EZipUtilityCompletionState::FAILURE_NOT_FOUND);
		return nullptr;
	}

	return UnzipTo(ArchivePath, Directory, ZipUtilityInterfaceDelegate, Format);
}

TSharedPtr<FZipOperation, ESPMode::ThreadSafe> FZipFileFunctionLibrary::UnzipWithLambda(const FString& ArchivePath, TFunction<void()> OnDoneCallback, TFunction<void(float)> OnProgressCallback, EZipUtilityCompressionFormat Format)
{
	TSharedPtr<FZULambdaDelegate> LambdaDelegate = MakeShareable(new FZULambdaDelegate());
	LambdaDelegate->SetOnDoneCallback([LambdaDelegate, OnDoneCallback]()
	{
		OnDoneCallback();		
	});
	LambdaDelegate->SetOnProgessCallback(OnProgressCallback);

	return Unzip(ArchivePath, LambdaDelegate, Format);
}


TSharedPtr<FZipOperation, ESPMode::ThreadSafe> FZipFileFunctionLibrary::UnzipTo(const FString& ArchivePath, const FString& DestinationPath, TSharedPtr<IZipUtilityInterface> ZipUtilityInterfaceDelegate, EZipUtilityCompressionFormat Format)
{

	return UnzipOnBGThreadWithFormat(ArchivePath, DestinationPath, ZipUtilityInterfaceDelegate, Format);
}

TSharedPtr<FZipOperation, ESPMode::ThreadSafe> FZipFileFunctionLibrary::Zip(const FString& ArchivePath, TSharedPtr<IZipUtilityInterface> ZipUtilityInterfaceDelegate, EZipUtilityCompressionFormat Format, TEnumAsByte<EZipUtilityCompressionLevel> Level)
{
	FString Directory;
	FString FileName;

	//Check Directory and File validity
	if (!IsValidDirectory(Directory, FileName, ArchivePath) || !FWindowsFileUtilityFunctionLibrary::DoesFileExist(ArchivePath))	
	{
		ZipUtilityInterfaceDelegate->OnDone(ArchivePath, EZipUtilityCompletionState::FAILURE_NOT_FOUND);
		return nullptr;
	}

	return ZipOnBGThread(ArchivePath, FileName, Directory, ZipUtilityInterfaceDelegate, Format, Level);
}

TSharedPtr<FZipOperation, ESPMode::ThreadSafe> FZipFileFunctionLibrary::ZipWithLambda(const FString& ArchivePath, TFunction<void()> OnDoneCallback, TFunction<void(float)> OnProgressCallback /*= nullptr*/, EZipUtilityCompressionFormat Format /*= COMPRESSION_FORMAT_UNKNOWN*/, TEnumAsByte<EZipUtilityCompressionLevel> Level /*=COMPRESSION_LEVEL_NORMAL*/)
{
	TSharedPtr<FZULambdaDelegate> LambdaDelegate = MakeShareable(new FZULambdaDelegate());	
	LambdaDelegate->SetOnDoneCallback([OnDoneCallback, LambdaDelegate]() 
	{
		OnDoneCallback();		
	});
	LambdaDelegate->SetOnProgessCallback(OnProgressCallback);

	return Zip(ArchivePath, LambdaDelegate, Format);
}

bool FZipFileFunctionLibrary::ListFilesInArchive(const FString& path, TSharedPtr<IZipUtilityInterface> ListDelegate, EZipUtilityCompressionFormat format)
{
	FString Directory;
	FString FileName;

	//Check Directory validity
	if (!IsValidDirectory(Directory, FileName, path))
	{
		return false;
	}

	ListOnBGThread(path, Directory, ListDelegate, format);
	return true;
}

FGraphEventRef FZipFileFunctionLibrary::RunLambdaOnGameThread(TFunction< void()> InFunction)
{
	return FFunctionGraphTask::CreateAndDispatchWhenReady(InFunction, TStatId(), nullptr, ENamedThreads::GameThread);
}
