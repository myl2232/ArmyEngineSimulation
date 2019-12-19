#pragma once

#include "ZipUtilityInterface.h"
#include "ZipOperation.h"
#include "Async/TaskGraphInterfaces.h"



enum EZipUtilityCompressionFormat
{
	COMPRESSION_FORMAT_UNKNOWN,
	COMPRESSION_FORMAT_SEVEN_ZIP,
	COMPRESSION_FORMAT_ZIP,
	COMPRESSION_FORMAT_GZIP,
	COMPRESSION_FORMAT_BZIP2,
	COMPRESSION_FORMAT_RAR,
	COMPRESSION_FORMAT_TAR,
	COMPRESSION_FORMAT_ISO,
	COMPRESSION_FORMAT_CAB,
	COMPRESSION_FORMAT_LZMA,
	COMPRESSION_FORMAT_LZMA86
};


enum EZipUtilityCompressionLevel
{
	COMPRESSION_LEVEL_NONE,
	COMPRESSION_LEVEL_FAST,
	COMPRESSION_LEVEL_NORMAL
};

class SevenZipCallbackHandler;
class FZipFileFunctionInternalCallback;

/** 
 A blueprint function library encapsulating all zip operations for both C++ and blueprint use. 
 For some operations a UZipOperation object may be returned, if you're interested in it, ensure
 you guard it from garbage collection by e.g. storing it as a UProperty, otherwise you may safely
 ignore it.
*/
class ZIPUTILITY_API FZipFileFunctionLibrary
{
	
public:
	/**启动，目前里面主要用于加载依赖的第三方dll*/
	static void Startup();

	/** 目前主要用于释放所加载的第三方dll */
	static void ShutDown();

	static void MoveFileTo(const FString& From, const FString& To);

	/* Unzips file in archive containing Name via ListFilesInArchive/UnzipFiles. Automatically determines compression if unknown. Calls ZipUtilityInterface progress events. */
	static bool UnzipFileNamed(const FString& archivePath, const FString& Name, TSharedPtr<IZipUtilityInterface> ZipUtilityInterfaceDelegate, EZipUtilityCompressionFormat format = COMPRESSION_FORMAT_UNKNOWN);
	
	/* Unzips file in archive containing Name at destination path via ListFilesInArchive/UnzipFilesTo. Automatically determines compression if unknown. Calls ZipUtilityInterface progress events. */	
	static bool UnzipFileNamedTo(const FString& archivePath, const FString& Name, const FString& destinationPath, TSharedPtr<IZipUtilityInterface> ZipUtilityInterfaceDelegate, EZipUtilityCompressionFormat format = COMPRESSION_FORMAT_UNKNOWN);

	/* Unzips the given file indexes in archive at destination path. Automatically determines compression if unknown. Calls ZipUtilityInterface progress events. */	
	static TSharedPtr<FZipOperation, ESPMode::ThreadSafe> UnzipFilesTo(const TArray<int32> fileIndices, const FString& archivePath, const FString& destinationPath, TSharedPtr<IZipUtilityInterface> ZipUtilityInterfaceDelegate, EZipUtilityCompressionFormat format = COMPRESSION_FORMAT_UNKNOWN);

	/* Unzips the given file indexes in archive at current path. Automatically determines compression if unknown. Calls ZipUtilityInterface progress events. */	
	static TSharedPtr<FZipOperation, ESPMode::ThreadSafe> UnzipFiles(const TArray<int32> fileIndices, const FString& ArchivePath, TSharedPtr<IZipUtilityInterface> ZipUtilityInterfaceDelegate, EZipUtilityCompressionFormat format = COMPRESSION_FORMAT_UNKNOWN);

	/* Unzips archive at current path. Automatically determines compression if unknown. Calls ZipUtilityInterface progress events. */	
	static TSharedPtr<FZipOperation, ESPMode::ThreadSafe> Unzip(const FString& ArchivePath, TSharedPtr<IZipUtilityInterface> ZipUtilityInterfaceDelegate, EZipUtilityCompressionFormat Format = COMPRESSION_FORMAT_UNKNOWN);

	/* Lambda C++ simple variant*/
	static TSharedPtr<FZipOperation, ESPMode::ThreadSafe> UnzipWithLambda(	const FString& ArchivePath,
									TFunction<void()> OnDoneCallback,
									TFunction<void(float)> OnProgressCallback = nullptr,
									EZipUtilityCompressionFormat format = COMPRESSION_FORMAT_UNKNOWN);

	/* Unzips archive at destination path. Automatically determines compression if unknown. Calls ZipUtilityInterface progress events. */	
	static TSharedPtr<FZipOperation, ESPMode::ThreadSafe> UnzipTo(const FString& ArchivePath, const FString& DestinationPath, TSharedPtr<IZipUtilityInterface> ZipUtilityInterfaceDelegate, EZipUtilityCompressionFormat format = COMPRESSION_FORMAT_UNKNOWN);

	/* Compresses the file or folder given at path and places the file in the same root folder. Calls ZipUtilityInterface progress events. Not all formats are supported for compression.*/	
	static TSharedPtr<FZipOperation, ESPMode::ThreadSafe> Zip(	const FString& FileOrFolderPath,
						TSharedPtr<IZipUtilityInterface> ZipUtilityInterfaceDelegate,
						EZipUtilityCompressionFormat Format = COMPRESSION_FORMAT_SEVEN_ZIP, 
						TEnumAsByte<EZipUtilityCompressionLevel> Level = COMPRESSION_LEVEL_NORMAL);

	/* Lambda C++ simple variant*/
	static TSharedPtr<FZipOperation, ESPMode::ThreadSafe> ZipWithLambda(	const FString& ArchivePath,
								TFunction<void()> OnDoneCallback,
								TFunction<void(float)> OnProgressCallback = nullptr,
								EZipUtilityCompressionFormat Format = COMPRESSION_FORMAT_UNKNOWN,
								TEnumAsByte<EZipUtilityCompressionLevel> Level = COMPRESSION_LEVEL_NORMAL);


	/*Queries Archive content list, calls ZipUtilityInterface list events (OnFileFound)*/	
	static bool ListFilesInArchive(const FString& ArchivePath, TSharedPtr<IZipUtilityInterface> ZipUtilityInterfaceDelegate, EZipUtilityCompressionFormat format = COMPRESSION_FORMAT_UNKNOWN);

	static FGraphEventRef RunLambdaOnGameThread(TFunction< void()> InFunction);

};


