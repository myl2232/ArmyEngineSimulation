#include "SCTProgressBar.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SSpinningImage.h"
#include "Framework/Application/SlateApplication.h"
#include "SBox.h"
#include "DownLoadMgr.h"
#include "SlateBorderBrush.h"

void SProgressBox::Construct(const FArguments& InArgs)
{
	BoxSize = InArgs._BoxSize;
	Callback_Succeed = InArgs._OnSucceedDownload;
	Callback_Failed = InArgs._OnFailedDownload;
	ChildSlot
		[
			SAssignNew(Progress, SBox)
			.WidthOverride(0.0/*BoxSize.X*/)
			.HeightOverride(BoxSize.Y)
			//.MaxDesiredWidth(56.0)
			[
				SNew(SButton)
				.ButtonStyle(new FButtonStyle)
				.ButtonColorAndOpacity(FLinearColor::Yellow)
				.ForegroundColor(FLinearColor::Yellow)
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
			]
		];

	ActiveTimerHandle = RegisterActiveTimer(0.1f, FWidgetActiveTimerDelegate::CreateSP(this, &SProgressBox::ActiveTick));
}

void SProgressBox::SetDownloadTasts(TArray<FString> InFileCachePaths, TArray<FString> InResourceUrls)
{
	FileCachePaths = InFileCachePaths;
	ResourceUrls = InResourceUrls;
	TaskCount = ResourceUrls.Num();
	LoadFileTasks.SetNum(TaskCount);
	for (int32 i=0; i<TaskCount; ++i)
	{
		LoadFileTasks[i] = MakeShareable(new FDownLoadFile(ResourceUrls[i], FileCachePaths[i], "",
			FDownloadTask::FOnFileDownloadFinished::CreateRaw(this, &SProgressBox::OnFileDownloadFinished),
			FDownloadTask::FOnFileDownloadFailed::CreateRaw(this, &SProgressBox::OnFileDownloadFailed)));
	}
}
void SProgressBox::ExecuteProgress()
{
	for (int32 i = 0; i < ResourceUrls.Num(); ++i)
	{
		LoadFileTasks[i]->ExecuteDownloadFile();
	}
}
void SProgressBox::PauseProgress()
{
	for (int32 i = 0; i < LoadFileTasks.Num(); ++i)
	{
		if (FPaths::FileExists(FileCachePaths[i]))
		{
			continue;
		}
		LoadFileTasks[i]->PauseFileLoad();
	}
}
void SProgressBox::ContinuProgress()
{
	for (int32 i = 0; i < LoadFileTasks.Num(); ++i)
	{
		if (FPaths::FileExists(FileCachePaths[i]))
		{
			continue;
		}
		//if(LoadFileTasks[i]->)
		LoadFileTasks[i]->ContinueFileLoad();
	}
}
bool SProgressBox::DetectCacheFiles()
{
	for (int32 i = 0; i < FileCachePaths.Num(); ++i)
	{
		if (!FPaths::FileExists(FileCachePaths[i]))
		{
			return false;
		}
	}
	return true;
}
void SProgressBox::OnFileDownloadFinished(const FString&, const FString&)
{
	if (DetectCacheFiles())
	{
		Callback_Succeed.ExecuteIfBound();
	}
}
void SProgressBox::OnFileDownloadFailed(const FString&, const FString&)
{
	Callback_Failed.ExecuteIfBound();
}

void SProgressBox::SetProgressRate(float InRate)
{
	Progress->SetWidthOverride(BoxSize.X * InRate);
}

float SProgressBox::GetProgress() const
{
	float CurrentValue = 0.0;
	for (int32 i=0; i<TaskCount; ++i)
	{
		CurrentValue += LoadFileTasks[i]->GetRateOfProgress();
	}

	if (TaskCount == 0)
	{
		return BoxSize.X;
	} 
	else
	{
		return BoxSize.X * CurrentValue / TaskCount;
	}
}
EActiveTimerReturnType SProgressBox::ActiveTick(double InCurrentTime, float InDeltaTime)
{
	float CurrentRate = GetProgress();
	if (CurrentRate == BoxSize.X)
	{
		Progress->SetWidthOverride(0.0);
		return EActiveTimerReturnType::Stop;
	} 
	else
	{
		Progress->SetWidthOverride(CurrentRate);
		return EActiveTimerReturnType::Continue;
	}
}


void SProgressBarContent::Construct(const FArguments& InArgs)
{
	FSlateFontInfo FontInfo = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText").Font;
	FontInfo.Size = 14;

	ChildSlot
		[
			SNew(SBox)
			.WidthOverride(186)
			.HeightOverride(40)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.Padding(FMargin(16, 8, 10, 8))
				.AutoWidth()
				[
					SNew(SSpinningImage)
					.Image(InArgs._Image)
				]
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.Padding(FMargin(0))
				[
					SNew(STextBlock)
					.Text(this, &SProgressBarContent::GetDescription)
					//.WrapTextAt(92.0f)
					.ColorAndOpacity(FLinearColor(FColor(0xFFE6E6E6)))
					.Font(FontInfo)
				]
				// + SHorizontalBox::Slot()
				// .VAlign(VAlign_Center)
				// .Padding(FMargin(10, 0, 0, 0))
				// [
				// 	SNew(STextBlock)
				// 	.Text(this, &SProgressBarContent::GetProgress)
				// 	.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 1)))
				// ]
			]
		];

	//ActiveTimerHandle = RegisterActiveTimer(0.3f, FWidgetActiveTimerDelegate::CreateSP(this, &SProgressBarContent::ActiveTick));
}

// void SProgressBarContent::SetMessage(const FText& InJobDescription, float InJobProgress, int32 InNumFinishedJob, int32 InNumTotalJob)
// {
// 	DescriptionText = InJobDescription;
// 	ProgressText = FText::FromString(FString::Printf(TEXT("%.2f%% (%d/%d)"), InJobProgress, InNumFinishedJob, InNumTotalJob));
// }

void SProgressBarContent::SetMessage(const FText& InJobDescription)
{
	DescriptionText = InJobDescription;
}

// EActiveTimerReturnType SProgressBarContent::ActiveTick(double InCurrentTime, float InDeltaTime)
// {
// 	return EActiveTimerReturnType::Continue;
// }

FProgressBar::FProgressBar(const FSlateBrush *InImage)
{
	TSharedPtr<SWindow> ParentWindow = GEngine->GameViewport->GetWindow();

	SAssignNew(ProgressBarWidget, SWindow)
		.Type(EWindowType::GameWindow)
		.Title(FText::FromString(TEXT("执行任务...")))
		.ClientSize(FVector2D(186.0f, 40.0f))
		.SizingRule(ESizingRule::FixedSize)
		.AutoCenter(EAutoCenter::PrimaryWorkArea)
		.HasCloseButton(false)
		.SupportsMinimize(false)
		.SupportsMaximize(false)
		.CreateTitleBar(false);

	ProgressBarWidget->SetContent(SAssignNew(ProgressBarContentWidget, SProgressBarContent).Image(InImage));

	FSlateApplication::Get().AddModalWindow(ProgressBarWidget.ToSharedRef(), ParentWindow, true);
	ProgressBarWidget->ShowWindow();
}

FProgressBar::~FProgressBar()
{
	ProgressBarWidget->RequestDestroyWindow();
}

void FProgressBar::SetMessage(const FText& InJobDescription, float InJobProgress, int32 InNumFinishedJob, int32 InNumTotalJob)
{
	//ProgressBarContentWidget->SetMessage(InJobDescription, InJobProgress, InNumFinishedJob, InNumTotalJob);
	ProgressBarContentWidget->SetMessage(InJobDescription);
}

