#include "ArmyPackageDownloader.h"
#include "ArmyDownloadModule.h"
#include "DownloadFileSet.h"
#include "ArmyHttpModule.h"
#include "ArmyModelEntity.h"
#include "ArmyDesignPackage.h"
#include "ArmyDataTools.h"

FArmyPakageDownloader::FArmyPakageDownloader()
{
 
}

FArmyPakageDownloader::~FArmyPakageDownloader()
{

}

void FArmyPakageDownloader::Tick(float DeltaTime)
{
	UpdateDownloadProgress();
}

bool FArmyPakageDownloader::IsTickable() const
{
	return bTickable;
}

TStatId FArmyPakageDownloader::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FArmyAutoDesignDataManager, STATGROUP_Tickables);
}

/**设置颜值包包含的ID
*	@ 参数为颜值包的模型ID（构件ID）
*/
void FArmyPakageDownloader::AddComponentsList(const TArray<TSharedPtr<class FArmyModelEntity>>  &InModelList)
{
	for (auto It : InModelList)
	{
		if (!this->ComponentList.Contains(It->ItemID))
		{
			this->ComponentList.Add(It->ItemID);
		}
	}
}

/**设置颜值包包含的某一个ID
*	@ 参数为颜值包的一个模型ID（构件ID）
*/
void FArmyPakageDownloader::AddComponent(const  int32  id)
{
	ComponentList.Add(id);
}

void FArmyPakageDownloader::DownloadPakage(FDownloadItemsDelegate CompeleDelegate, const DownloadInfo InLoadInfo)
{
	if (ComponentList.Num() < 1)
	{
		return;
	}
	OnCompeleDelegate = CompeleDelegate;
	LoadInfo = InLoadInfo;
	OnHideDelegate = FBoolDelegate::CreateRaw(this, &FArmyPakageDownloader::OnHideProgressExecuteCallback);

	FileStrings = TEXT("[{\"resourceType\":2,\"ids\":[");
	FileStrings = FileStrings.Append(FString::FromInt(ComponentList[0]));
	for (int32 i = 1; i < ComponentList.Num(); ++i)
	{
		FileStrings = FileStrings.Append(TEXT(",") + FString::FromInt(ComponentList[i]));
	}
	FileStrings = FileStrings.Append(TEXT("]}]"));

	//ComponentList.Empty();

	//显示下载进度
	TAttribute<FText> SyncProgressTextAttr = TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateRaw(this, &FArmyPakageDownloader::GetDownloadProgressText));
	GGI->Window->ShowThrobber(SyncProgressTextAttr);
	
	////请求数据
	//FArmyHttpRequestCompleteDelegate CompleteDelegate;
	//CompleteDelegate.BindRaw(this, &FArmyPakageDownloader::OnCompeleDownload);
	//IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest("/api/synResource", CompleteDelegate, FileStrings);
	//Request->ProcessRequest();

	//请求数据
	FArmyHttpRequestCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindRaw(this, &FArmyPakageDownloader::OnCompeleDownload);
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest("/api/synResource", CompleteDelegate, FileStrings);
	Request->ProcessRequest();

}

/**
* @ 认证回调函数，不要调用
*/
void FArmyPakageDownloader::OnCompeleDownload(FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful)
	{
		PakageContentItems.Reset();
		// 根据同步的结果，创建的下载文件列表
		TArray<FDownloadSpace::FDownloadFileInfo> NeedToDownLoadFiles;

        const TSharedPtr<FJsonObject> JObjectData = Response.Data->GetObjectField("data");
        const TArray<TSharedPtr<FJsonValue>> JArray = JObjectData->GetArrayField("list");

		// 解析返回的列表
		for (auto& It : JArray)
		{
			TSharedPtr<FJsonObject> JObject = It->AsObject();
			const TArray<TSharedPtr<FJsonValue>> JArray2 = JObject->GetArrayField("list");
			for (auto& It2 : JArray2)
			{
                const TSharedPtr<FJsonObject> JObject = It2->AsObject()->GetObjectField("data");
                FContentItemPtr Item = FArmyCommonTools::ParseHardModeContemItemFromJson(JObject);
				if (Item.IsValid())
				{
					PakageContentItems.Add(Item);
				}
			}
		}

		for (auto&It : PakageContentItems)
		{
			if (!It.IsValid())
				continue;
			TArray< TSharedPtr<FResObj> >resArr = It->GetResObjNoComponent();
			for (auto FileIt : resArr)
			{
				// 如果NetMD5或者URL为空，有可能是下架产品，不再作为下载对象
				if (FileIt->FileMD5 == TEXT("") || FileIt->FileURL == TEXT(""))
				{
					continue;
				}

				EFileExistenceState FileState = FArmyResourceModule::Get().GetResourceManager()->CheckFileExistState(FileIt->FilePath, FileIt->FileMD5);
				if (FileState == EFileExistenceState::Complete || FileState == EFileExistenceState::PendingDownload)
				{
					continue;
				}

				NeedToDownLoadFiles.Add(FDownloadSpace::FDownloadFileInfo(It->ID, FileIt->FilePath, FileIt->FileURL, FileIt->FileMD5));
			}
		}

		// 创建下载任务
		if (NeedToDownLoadFiles.Num() > 0)
		{
			DownLoadTasks = FArmyDownloadModule::Get().GetDownloadManager()->AddMultiTask(NeedToDownLoadFiles);
			DownLoadTasks->OnDownloadFinished = OnHideDelegate;
		}
		else
		{
			GGI->Window->HideThrobber();
			OnCompeleDelegate.ExecuteIfBound(LoadInfo);
		}
	}
	else
	{	 
		GGI->Window->HideThrobber();
		GGI->Window->ShowMessage(MT_Warning, L"Connect failed");
	}
}


void FArmyPakageDownloader::OnHideProgressExecuteCallback(bool param)
{
	GGI->Window->HideThrobber();
	OnCompeleDelegate.ExecuteIfBound(LoadInfo);
}

/**
* @ 获取颜值包的数据
*/

TArray<FContentItemPtr> FArmyPakageDownloader::GetPakageContentItems() { return PakageContentItems; }
 
 
void FArmyPakageDownloader::UpdateDownloadProgress()
{
	if (DownLoadTasks.IsValid())
	{
		DownloadProgress = DownLoadTasks->GetProgress();
	}
}

/**
* 获得下载的模型
*/
TArray<TSharedPtr<class FArmyModelEntity>> FArmyPakageDownloader::GetModelList()
{
	TArray<TSharedPtr<class FArmyModelEntity>> ResultArray;
	for (auto It : PakageContentItems)
	{
		if (!It.IsValid())
		{
			continue;
		}

		TSharedPtr<FArmyModelEntity>  Model = MakeShareable(new FArmyModelEntity(It));
		FVector ModelVector = FArmyDataTools::GetContextItemSize(It);
		Model->Length = ModelVector.X;
		Model->Width = ModelVector.Y;
		Model->Height = ModelVector.Z;


		ResultArray.Add(Model);
	}

	return ResultArray;
}

