#include "SArmyReplaceList.h"
#include "DownloadFileSet.h"
#include "SContentItem.h"
#include "SScrollWrapBox.h"
#include "ArmyCommonTools.h"
#include "ArmyDownloadModule.h"
#include "ArmyResourceModule.h"
#include "ArmyHttpModule.h"
#include "ArmyUser.h"
#include "ArmySlateModule.h"
#include "Runtime/Online/ImageDownload/Public/WebImageCache.h"

SArmyReplaceList::~SArmyReplaceList()
{
    if (Request.IsValid())
    {
        Request->CancelRequest();
    }
}

void SArmyReplaceList::Construct(const FArguments& InArgs)
{
    CategoryCode = InArgs._CategoryCode;
    OnReplace = InArgs._OnReplace;
    OnClose = InArgs._OnClose;
	IsHyPowerMode = InArgs._HyPowerMode.Get();
    DefaultGoodsId = InArgs._DefaultGoodsId;
    Request = nullptr;

    ChildSlot
    [
        SNew(SHorizontalBox)
        
        + SHorizontalBox::Slot()
        .FillWidth(1.f)
        .Padding(0, 40, 2, 0)
        [
            // 商品详情
            CreateDetailInfo()
        ]

        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SBox)
            .WidthOverride(280)
            .HeightOverride(440)
            [
                SNew(SBorder)
                .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
                .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
                .Padding(FMargin(0))
                [
                    SNew(SVerticalBox)

                    // 标题栏
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        CreateTitleBar(InArgs)
                    ]

                    // 筛选项
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        CreateFilterItems()
                    ]

                    // 商品替换列表
                    + SVerticalBox::Slot()
                    .FillHeight(1.f)
                    .Padding(16, 0, 0, 0)
                    [
                        SNew(SOverlay)

                        + SOverlay::Slot()
                        .HAlign(HAlign_Fill)
                        .VAlign(VAlign_Fill)
                        [
                            CreateReplaceList(InArgs)
                        ]

                        + SOverlay::Slot()
                        .HAlign(HAlign_Center)
                        .VAlign(VAlign_Top)
                        .Padding(0, 53, 0, 0)
                        [
                            CreateNoResourceTips(InArgs)
                        ]
                    ]
                ]
            ]
        ]
    ];

    ReqNextPage();
}

void SArmyReplaceList::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    for (int32 i = 0; i < ContentItemUIList.Num(); i++)
    {
        if (ContentItemUIList[i]->GetItemState() == EContentItemState::EDownloading)
        {
			TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = ContentItemList[i]->GetResObjNoComponent();
			if (resArr.Num() < 1)
				continue;
            TSharedPtr<FDownloadFileSet> TheTask = FArmyDownloadModule::Get().GetDownloadManager()->GetTask(resArr[0]->FileName);
            if (TheTask.IsValid())
            {
                // 设置下载进度
                float Progress = TheTask->GetProgress();
                ContentItemUIList[i]->SetDownloadText(FText::FromString(FString::Printf(TEXT("%.2f%%"), Progress)));
                ContentItemUIList[i]->SetPercent(Progress);
            }
        }
    }
}

void SArmyReplaceList::ReqNextPage()
{
    NoResourceTipWidget->SetVisibility(EVisibility::Collapsed);

    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindLambda([this](FArmyHttpResponse Response) {
        if (Response.bWasSuccessful)
        {
            const TSharedPtr<FJsonObject> Data = Response.Data->GetObjectField("data");
            Data->TryGetNumberField("totalPage", TotalPageNum);
            const TArray<TSharedPtr<FJsonValue>> List = Data->GetArrayField("list");

            for (auto& ListIt : List)
            {
                TSharedPtr<FJsonObject> GoodsObj = ListIt->AsObject();
                FContentItemPtr ContentItem = IsHyPowerMode?
											FArmyCommonTools::ParseHydropowerContentItemFromJson(GoodsObj)
											: FArmyCommonTools::ParseHardModeContemItemFromJson(GoodsObj);
                if (ContentItem.IsValid())
                {
                    ContentItemList.Add(ContentItem);
                }
            }

            // 填充列表
            FillList();

            ++PageIndex;
        }
    });

    RebuildFullUrl();
    Request = FArmyHttpModule::Get().CreateDBJGetRequest(FullUrl, CompleteDelegate);
    Request->ProcessRequest();
}

void SArmyReplaceList::FillList()
{
    if (ContentItemList.Num() == 0)
    {
        NoResourceTipWidget->SetVisibility(EVisibility::Visible);
    }
    else
    {
        NoResourceTipWidget->SetVisibility(EVisibility::Collapsed);

        for (int32 i = ContentItemUIList.Num(); i < ContentItemList.Num(); ++i)
        {
            FContentItemPtr ContentItem = ContentItemList[i];

            TSharedPtr<SContentItem> Item =
                SNew(SContentItem)
                .ItemCode(i)
                .ItemWidth(112)
                .ItemHeight(140)
                .ThumbnailPadding(0)
                .ThumbnailURL(ContentItem->ThumbnailURL)
                .ThumbnailImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.DefaultImage")))
                .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
                .TextColor(FArmyStyle::Get().GetColor("Color.FF969799"))
                .OnClicked(this, &SArmyReplaceList::OnContentItemSelected)
                .DisplayText(FText::FromString(ContentItem->Name))
                .DetailInfo(EVisibility::Visible)
                .OnInfoHovered_Lambda([this](int32 ItemCode) {
                    HoveredContentItem = ContentItemList[ItemCode];
                })
                .OnInfoUnhovered_Lambda([this](int32 ItemCode) {
                    HoveredContentItem = nullptr;
                });

            Item->SetItemState(GetContentItemState(ContentItem));


            SWB_ReplaceList->AddItem(Item);
            SWB_ReplaceList->ResetScrollNotify();
            ContentItemUIList.Add(Item);

            // 初始化默认选中项
            if (DefaultGoodsId != -1 && ContentItem->ID == DefaultGoodsId)
            {
                OnContentItemSelected(i);
            }
        }
    }
}

void SArmyReplaceList::ClearList()
{
	ContentItemList.Reset();
	SWB_ReplaceList->ClearChildren();
	ContentItemUIList.Empty();
}

TSharedRef<SWidget> SArmyReplaceList::CreateTitleBar(const FArguments& InArgs)
{
    return
        SNew(SBox)
        .HeightOverride(40)
        [
            SNew(SBorder)
            .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF212224"))
            .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
            .Padding(FMargin(0))
            [
                SNew(SHorizontalBox)
                
                + SHorizontalBox::Slot()
                .AutoWidth()
                .HAlign(HAlign_Left)
                .VAlign(VAlign_Center)
                .Padding(16, 0, 0, 0)
                [
                    SNew(STextBlock)
                    .TextStyle(FArmyStyle::Get(), "ArmyText_12")
                    .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFC8C9CC"))
                    .Text(InArgs._Title)
                ]

                + SHorizontalBox::Slot()
                .FillWidth(1.f)
                .HAlign(HAlign_Right)
                .VAlign(VAlign_Center)
                [
                    SNew(SBox)
                    .WidthOverride(30)
                    .HeightOverride(30)
                    [
                        SNew(SButton)
                        .ButtonStyle(FArmyStyle::Get(), "Button.CloseApplication")
                        .OnClicked_Lambda([this]{
                            OnClose.ExecuteIfBound();
                            return FReply::Handled();
                        })
                    ]
                ]
            ]
        ];
}

void SArmyReplaceList::RebuildFullUrl()
{
    FullUrl = FString::Printf(TEXT("/api/bim_goods/replace_query?categoryCode=%s&page=%d&rows=%d&setMealId=%d"),
        *CategoryCode, PageIndex, 18, FArmyUser::Get().GetMealID());
}

TSharedRef<SWidget> SArmyReplaceList::CreateReplaceList(const FArguments& InArgs)
{
    return
        SAssignNew(SWB_ReplaceList, SScrollWrapBox)
        .InnerSlotPadding(FVector2D(16, 16))
        .Delegate_ScrollToEnd_Lambda([this]() {
            if (PageIndex <= TotalPageNum)
            {
                ReqNextPage();
            }
        });
}

TSharedRef<SWidget> SArmyReplaceList::CreateDetailInfo()
{
    return
        SNew(SBorder)
        .Padding(FMargin(0))
        .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
        .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
        .Visibility_Lambda([this]() {
            return HoveredContentItem.IsValid() ? EVisibility::Visible : EVisibility::Collapsed;
        })
        [
            SNew(SVerticalBox)

            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SBox)
                .WidthOverride(300)
                .HeightOverride(300)
                [
                    SAssignNew(DetailImage, SImage)
                    .Image_Lambda([this]() {
                        if (HoveredContentItem.IsValid())
                        {
                            FString ThumbnailURL = HoveredContentItem->ThumbnailURL;
                            if (!ThumbnailURL.IsEmpty())
                            {
                                return FArmySlateModule::Get().WebImageCache->Download(ThumbnailURL).Get().Attr().Get();
                            }
                        }

                        return FArmyStyle::Get().GetBrush("Icon.DefaultImage");
                    })
                ]
            ]

            // 商品名称
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(16, 8, 0, 0)
            .HAlign(HAlign_Left)
            [
                SNew(STextBlock)
                .TextStyle(FArmyStyle::Get(), "ArmyText_16")
                .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFC8C9CC"))
                .Text_Lambda([this]() {
                    if (HoveredContentItem.IsValid())
                    {
                        return FText::FromString(*HoveredContentItem->Name);
                    }

                    return FText();
                })
            ]

            // 品牌名称
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(16, 8, 0, 0)
            .HAlign(HAlign_Left)
            [
                SNew(STextBlock)
                .TextStyle(FArmyStyle::Get(), "ArmyText_12")
                .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FF969799"))
                .Text_Lambda([this]() {
                    if (HoveredContentItem.IsValid() && HoveredContentItem->ProObj.IsValid())
                    {
                        FString BrandText = TEXT("品牌：") + HoveredContentItem->ProObj->brandName;
                        return FText::FromString(*BrandText);
                    }

                    return FText();
                })
            ]

            // 尺寸信息
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(16, 8, 0, 0)
            .HAlign(HAlign_Left)
            [
                SNew(STextBlock)
                .TextStyle(FArmyStyle::Get(), "ArmyText_12")
                .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FF969799"))
                .Text_Lambda([this]() {
                    if (HoveredContentItem.IsValid() && HoveredContentItem->ProObj.IsValid())
                    {
                        FString SizeText = FString::Printf(TEXT("尺寸：长%dx宽%dx高%d（cm）"),
                            (int32)HoveredContentItem->ProObj->Length,
                            (int32)HoveredContentItem->ProObj->Width,
                            (int32)HoveredContentItem->ProObj->Height);
                        return FText::FromString(*SizeText);
                    }

                    return FText();
                })
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(16, 8, 0, 0)
            .HAlign(HAlign_Left)
            [
                SNew(STextBlock)
                .TextStyle(FArmyStyle::Get(), "ArmyText_14")
                .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFFF9800"))
                .Text_Lambda([this]() {
                    if (HoveredContentItem.IsValid() && HoveredContentItem->ProObj.IsValid())
                    {
                        FString PriceText = FString::Printf(TEXT("￥%.2f"), HoveredContentItem->ProObj->Price);
                        return FText::FromString(*PriceText);
                    }

                    return FText();
                })
            ]
        ];
}

TSharedRef<SWidget> SArmyReplaceList::CreateNoResourceTips(const FArguments& InArgs)
{
    return
        SAssignNew(NoResourceTipWidget, SVerticalBox)
        
        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        [
            SNew(SImage)
            .Image(FArmyStyle::Get().GetBrush("Icon.NoResourceImage_Replace"))
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(0, 20, 0, 0)
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        [
            SNew(STextBlock)
            .TextStyle(FArmyStyle::Get(), "ArmyText_12")
            .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FF969799"))
            .Text(InArgs._NoResourceTipText)
            .Justification(ETextJustify::Center)
        ];
}

EContentItemState SArmyReplaceList::GetContentItemState(FContentItemPtr Item)
{
	TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = Item->GetResObjNoComponent();
	if (resArr.Num() < 1)
		return EContentItemState::ENotReady;

	if (Item->ResourceType == HardDXF)
	{
		int ReadyNum = 0;
		for (int i = 0; i < resArr.Num(); i++)
		{
			EFileExistenceState FileState = FArmyResourceModule::Get().GetResourceManager()->
				CheckFileExistState(resArr[i]->FilePath, resArr[i]->FileMD5);

			resArr[i]->FileState = FileState;

			EFileExistenceState ComponentFileState = FileState;
			if (Item->GetComponent().IsValid())
			{
				ComponentFileState = FArmyResourceModule::Get().GetResourceManager()->
					CheckFileExistState(Item->GetComponent()->FilePath, Item->GetComponent()->FileMD5);
				Item->GetComponent()->FileState = ComponentFileState;
			}

			if (FileState == EFileExistenceState::NotExist || ComponentFileState == EFileExistenceState::NotExist)
			{
				return EContentItemState::ENotReady;
			}
			else if (FileState == EFileExistenceState::PendingDownload || ComponentFileState == EFileExistenceState::PendingDownload)
			{
				return EContentItemState::EPending;
			}
			else if (FileState == EFileExistenceState::Exist)
			{
				return EContentItemState::EReady;
			}
			else if (FileState == EFileExistenceState::Complete)
			{
				ReadyNum++;
			}
			
		}
		if (ReadyNum == resArr.Num())
			return EContentItemState::EReady;
		else
			return EContentItemState::ENotReady;
	}
	else
	{
		EFileExistenceState FileState = FArmyResourceModule::Get().GetResourceManager()->CheckFileExistState(resArr[0]->FilePath, resArr[0]->FileMD5);

		resArr[0]->FileState = FileState;

		EFileExistenceState ComponentFileState = FileState;
		if (Item->GetComponent().IsValid())
		{
			ComponentFileState = FArmyResourceModule::Get().GetResourceManager()->CheckFileExistState(Item->GetComponent()->FilePath, Item->GetComponent()->FileMD5);
			Item->GetComponent()->FileState = ComponentFileState;
		}

		if (FileState == EFileExistenceState::NotExist || ComponentFileState == EFileExistenceState::NotExist)
		{
			return EContentItemState::ENotReady;
		}
		else if (FileState == EFileExistenceState::PendingDownload || ComponentFileState == EFileExistenceState::PendingDownload)
		{
			return EContentItemState::EPending;
		}
		else if (FileState == EFileExistenceState::Exist)
		{
			return EContentItemState::EReady;
		}
		else if (FileState == EFileExistenceState::Complete)
		{
			return EContentItemState::EReady;
		}

		return EContentItemState::ENotReady;
	}
    
}

void SArmyReplaceList::OnContentItemSelected(int32 ItemCode)
{
    EContentItemState State = ContentItemUIList[ItemCode]->GetItemState();
    if (State == EContentItemState::EReady)
    {
        for (auto& It : ContentItemUIList)
        {
            It->SetSelected(false);
        }
        ContentItemUIList[ItemCode]->SetSelected(true);
        SelectedSContenItem = ContentItemUIList[ItemCode];

        // 触发替换事件
        OnReplace.ExecuteIfBound(ContentItemList[ItemCode]);
    }
    else
    {
        // 下载资源
        DownloadResource(ItemCode);
    }
}

void SArmyReplaceList::DownloadResource(const int32 ItemCode)
{
    FContentItemPtr ContentItem = ContentItemList[ItemCode];
    TSharedPtr<SContentItem> ContentItemWidget = ContentItemUIList[ItemCode];
    if (ContentItem.IsValid() && ContentItemWidget.IsValid())
    {
        TArray<FDownloadSpace::FDownloadFileInfo> FileInfos;

        // 点击进入下载或进入下载队列等待
        if (ContentItemWidget->GetItemState() == EContentItemState::ENotReady)
        {
            if (ContentItem->ResourceType == EResourceType::HardPAK)
            {
                TArray<TSharedPtr<FContentItemSpace::FResObj> > ResArr = ContentItem->GetResObjNoComponent();
                if (ResArr.Num() == 0)
                {
                    return;
                }
                    
                // 下载模型
                if (!(ResArr[0]->FileState == EFileExistenceState::Exist || ResArr[0]->FileState == EFileExistenceState::Complete))
                {
                    FileInfos.Add(FDownloadSpace::FDownloadFileInfo(
                        ContentItem->ID, ResArr[0]->FilePath, ResArr[0]->FileURL, ResArr[0]->FileMD5));
                }

                //// 下载构件
                //if (ContentItem->GetComponent().IsValid() && !(ContentItem->GetComponent()->FileState == EFileExistenceState::Exist || ContentItem->GetComponent()->FileState == EFileExistenceState::Complete))
                //{
                //    FileInfos.Add(FDownloadSpace::FDownloadFileInfo(
                //        ContentItem->ID, ContentItem->GetComponent()->FilePath, ContentItem->GetComponent()->FileURL, ContentItem->GetComponent()->FileMD5));
                //}


				// 下载构件
				if (ContentItem->GetComponent().IsValid())
				{
					// 下载俯视图图例
					if (!(ContentItem->GetComponent()->FileState == EFileExistenceState::Exist || ContentItem->GetComponent()->FileState == EFileExistenceState::Complete))
					{
						FileInfos.Add(FDownloadSpace::FDownloadFileInfo(
							ContentItem->ID, ContentItem->GetComponent()->FilePath, ContentItem->GetComponent()->FileURL, ContentItem->GetComponent()->FileMD5));
					}

					// 下载其他图例
					for (auto It : ContentItem->GetComponent()->MapResObj)
					{
						TSharedPtr<FResObj> CurrentRes = It.Value;
						if (CurrentRes.IsValid() && !(CurrentRes->FileState == EFileExistenceState::Exist || CurrentRes->FileState == EFileExistenceState::Complete))
						{
							FileInfos.Add(FDownloadSpace::FDownloadFileInfo(
								ContentItem->ID, CurrentRes->FilePath, CurrentRes->FileURL, CurrentRes->FileMD5));
						}
					}
				}

            }
			else if (ContentItem->ResourceType == EResourceType::Texture)
			{
				TArray<TSharedPtr<FContentItemSpace::FResObj> > ResArr = ContentItem->GetResObjNoComponent();
				if (ResArr.Num() == 0)
				{
					return;
				}

				if (!(ResArr[0]->FileState == EFileExistenceState::Exist || ResArr[0]->FileState == EFileExistenceState::Complete))
				{
					FileInfos.Add(FDownloadSpace::FDownloadFileInfo(
						ContentItem->ID, ResArr[0]->FilePath, ResArr[0]->FileURL, ResArr[0]->FileMD5));
				}		
			}
			else if (ContentItem->ResourceType == EResourceType::HardDXF)
			{
				TArray<TSharedPtr<FContentItemSpace::FResObj> > ResArr = ContentItem->GetResObjNoComponent();
				if (ResArr.Num() == 0)
				{
					return;
				}
				for (int i = 0; i < ResArr.Num(); i++)
				{
					if (!(ResArr[i]->FileState == EFileExistenceState::Exist || ResArr[i]->FileState == EFileExistenceState::Complete))
					{
						FileInfos.Add(FDownloadSpace::FDownloadFileInfo(
							ContentItem->ID, ResArr[i]->FilePath, ResArr[i]->FileURL, ResArr[i]->FileMD5));
					}
				}
			}
			//@郭子阳 添加水电模式下载
			else if (ContentItem->ResourceType == EResourceType::Hydropower)
			{
				TArray<TSharedPtr<FContentItemSpace::FResObj> > ResArr = ContentItem->GetResObjNoComponent();
				if (ResArr.Num() == 0)
				{
					return;
				}

				// 下载模型
				if (!(ResArr[0]->FileState == EFileExistenceState::Exist || ResArr[0]->FileState == EFileExistenceState::Complete))
				{
					FileInfos.Add(FDownloadSpace::FDownloadFileInfo(
						ContentItem->ID, ResArr[0]->FilePath, ResArr[0]->FileURL, ResArr[0]->FileMD5));
				}

				//// 下载构件
				//if (ContentItem->GetComponent().IsValid() && !(ContentItem->GetComponent()->FileState == EFileExistenceState::Exist || ContentItem->GetComponent()->FileState == EFileExistenceState::Complete))
				//{
				//	FileInfos.Add(FDownloadSpace::FDownloadFileInfo(
				//		ContentItem->ID, ContentItem->GetComponent()->FilePath, ContentItem->GetComponent()->FileURL, ContentItem->GetComponent()->FileMD5));
				//}

				// 下载构件
				if (ContentItem->GetComponent().IsValid())
				{
					// 下载俯视图图例
					if (!(ContentItem->GetComponent()->FileState == EFileExistenceState::Exist || ContentItem->GetComponent()->FileState == EFileExistenceState::Complete))
					{
						FileInfos.Add(FDownloadSpace::FDownloadFileInfo(
							ContentItem->ID, ContentItem->GetComponent()->FilePath, ContentItem->GetComponent()->FileURL, ContentItem->GetComponent()->FileMD5));
					}

					// 下载其他图例
					for (auto It : ContentItem->GetComponent()->MapResObj)
					{
						TSharedPtr<FResObj> CurrentRes = It.Value;
						if (CurrentRes.IsValid() && !(CurrentRes->FileState == EFileExistenceState::Exist || CurrentRes->FileState == EFileExistenceState::Complete))
						{
							FileInfos.Add(FDownloadSpace::FDownloadFileInfo(
								ContentItem->ID, CurrentRes->FilePath, CurrentRes->FileURL, CurrentRes->FileMD5));
						}
					}
				}

			}
        }

        // 将需要下载的文件打包成一个下载任务
        if (FileInfos.Num() > 0)
        {
            TSharedPtr<FDownloadFileSet> NewTask = FArmyDownloadModule::Get().GetDownloadManager()->AddMultiTask(FileInfos);
            NewTask->OnDownloadFinished.BindSP(this, &SArmyReplaceList::OnDownloadFinished, ItemCode);
            ContentItemWidget->SetItemState(EContentItemState::EDownloading);
        }
    }
}

void SArmyReplaceList::OnDownloadFinished(bool bWasSucceed, const int32 ItemIndex)
{
    if (ItemIndex < 0 || ItemIndex >= ContentItemUIList.Num())
    {
        return;
    }

    TSharedPtr<SContentItem> ContentItem = ContentItemUIList[ItemIndex];
    if (ContentItem.IsValid())
    {
        if (bWasSucceed)
        {
            ContentItem->SetItemState(EReady);

            // 在没有缓存的电脑上打开方案，第一次选中会去下载模型；下载完模型需要在此设置选中状态
            if (!SelectedSContenItem.IsValid())
            {
                OnContentItemSelected(ItemIndex);
            }
        }
        else
        {
            ContentItem->SetItemState(ENotReady);
        }
    }
}

void SArmyReplaceButton::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SBox)
        .WidthOverride(114)
        .HeightOverride(48)
        [
            SNew(SHorizontalBox)

            // 缩略图
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SBox)
                .WidthOverride(48)
                .HeightOverride(48)
                [
                    SAssignNew(ThumbnailImage, SImage)
					.Image(InArgs._ThumbnailBrush)
                ]
            ]

            // 替换按钮
            + SHorizontalBox::Slot()
            .FillWidth(1)
            [
                SNew(SButton)
                .ContentPadding(FMargin(0))
                .ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.FF2E2F32"))
                .OnClicked(InArgs._OnClicked)
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center)
                [
                    SNew(SImage)
                    .Image(FArmyStyle::Get().GetBrush("Icon.Replace"))
                ]
            ]
        ]
    ];
}

void SArmyReplaceButton::SetSelectedImage(TAttribute<const FSlateBrush*> InImage)
{
	ThumbnailImage->SetImage(InImage.Get());
}