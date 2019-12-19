#include "SArmyPageQuery.h"

//每页显示多少数据
const int32 SArmyPageQuery::EachPageShowNum = 10;
//显示最大页码UI个数，最小不能小于5
const int32 SArmyPageQuery::MaxPageItemNum = 8;
const FString SArmyPageQuery::Ellipsis = TEXT("...");
void SArmyPageQuery::Construct(const FArguments& InArgs)
{
	//EachPageCount = InArgs._EachPageCount;
	//DataCount = InArgs._DataCount;
	ChildSlot
		[
			SNew(SBorder)
			.Padding(FMargin(0, 32,0,32))
			.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF262729"))
			.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
			.HAlign(HAlign_Fill)
			[
				SAssignNew(PageHorizontalBox, SHorizontalBox)
			]
		];
}

void SArmyPageQuery::Init()
{
	PageCount = FMath::CeilToInt(DataCount * 1.0 / SArmyPageQuery::EachPageShowNum);

	GenerateUI();
}

void SArmyPageQuery::GenerateUI()
{
	TSharedPtr<SArmyPageItem> TempPageItem;
	for (int32 i = 0; i < PageCount + 2; ++i)
	{
		if (i == MaxPageItemNum + 1)
		{
			TempPageItem = SNew(SArmyPageItem)
				.TextContentStr(TEXT(">"))
				.OnItemClicked(this, &SArmyPageQuery::OnItemClicked);
			MapPageItems.Add(i, TempPageItem);
			AddPageItem(TempPageItem);
			OnItemClicked(MapPageItems.FindRef(1)->GetTextContentStr());

			return;
		}

		if (i == 0 )
		{
			TempPageItem = SNew(SArmyPageItem)
				.TextContentStr(TEXT("<"))
				.OnItemClicked(this, &SArmyPageQuery::OnItemClicked);
		}
		else if(i == PageCount + 1)
		{
			TempPageItem = SNew(SArmyPageItem)
				.TextContentStr(TEXT(">"))
				.OnItemClicked(this, &SArmyPageQuery::OnItemClicked);
			
		}
		else
		{
			TempPageItem = SNew(SArmyPageItem)
				.OnItemClicked(this, &SArmyPageQuery::OnItemClicked)
				.TextContentStr(FString::FromInt(i));
		}
		MapPageItems.Add(i, TempPageItem);
		AddPageItem(TempPageItem);
	}
}

void SArmyPageQuery::OnItemClicked(const FString & InText)
{
	if (CurrentPage == InText || InText.Equals(Ellipsis))
	{
		//选择页未改变或者为省略号直接返回
		return;
	}

	int32 CurrentPageTemp = InText.IsNumeric() ? FCString::Atoi(*InText) : -1;
	if (CurrentPageTemp != -1)
	{

		//得到选择页面的UI索引
		int32 CurrentItemIndex = -1;
		for (const auto & Ref : MapPageItems)
		{
			if (Ref.Value->GetTextContentStr() == InText)
			{
				CurrentItemIndex = Ref.Key;
			}
		}

		if (CurrentItemIndex == -1)
		{
			//点击按钮非数字直接返回，规避省略号
			return;
		}

		CurrentPage = InText;

		int32 Index = 1;//起始页UI索引
		if (PageCount <= MaxPageItemNum)//总页数小于最大显示页数
		{
			for (Index; Index <= PageCount; Index++)
			{
				if (Index == CurrentItemIndex)
				{
					ActivePageByContent(CurrentItemIndex);
					break;
				}
			}
		}
		else if(PageCount > MaxPageItemNum)//总页数大于MaxPageItemNum = 8页，则加载MaxPageItemNum = 8页
		{
			if (CurrentItemIndex == 1)
			{
				TSharedPtr<SArmyPageItem> PageQuery1 = MapPageItems.FindRef(MapPageItems.Num() - 2);
				PageQuery1->SetTextContentStr(FString::FromInt(PageCount));

				//倒数第二项设置省略号
				TSharedPtr<SArmyPageItem> PageQuery = MapPageItems.FindRef(MapPageItems.Num() - 3);
				PageQuery->SetTextContentStr(Ellipsis);

				//激活第一项
				ActivePageByContent(CurrentItemIndex);

				//更新紧挨项的下标
				for (int32 j = CurrentItemIndex + 1; j < MapPageItems.Num() - 3; j++)
				{
					TSharedPtr<SArmyPageItem> PageQueryTemp = MapPageItems.FindRef(j);
					TSharedPtr<SArmyPageItem> PageQueryTempPre = MapPageItems.FindRef(j - 1);
					FString currentIndex = FString::FromInt(FCString::Atoi(*PageQueryTempPre->GetTextContentStr()) + 1);
					PageQueryTemp->SetTextContentStr(currentIndex);
				}
			}
			else if (CurrentItemIndex == MaxPageItemNum)
			{
				//第二页设置为省略号
				TSharedPtr<SArmyPageItem> PageQuery = MapPageItems.FindRef(2);
				PageQuery->SetTextContentStr(Ellipsis);
				if (CurrentItemIndex == MaxPageItemNum)//选择的是最后一项
				{
					//倒数第二项设置省略号
					TSharedPtr<SArmyPageItem> PageQuery = MapPageItems.FindRef(MapPageItems.Num() - 3);
					PageQuery->SetTextContentStr(Ellipsis);

					//激活最后一项
					ActivePageByContent(CurrentItemIndex);

					//更新紧挨项的下标
					for (int32 j = CurrentItemIndex - 1; j > 2; j--)
					{
						TSharedPtr<SArmyPageItem> PageQueryTemp = MapPageItems.FindRef(j);
						TSharedPtr<SArmyPageItem> PageQueryTempPre = MapPageItems.FindRef(j + 1);
						FString currentIndex = FString::FromInt(FCString::Atoi(*PageQueryTempPre->GetTextContentStr()) - 1);
						PageQueryTemp->SetTextContentStr(currentIndex);
					}
				}
			}
			else if (CurrentItemIndex < MaxPageItemNum)
			{
				for (Index; Index <= MaxPageItemNum; Index++)
				{
					if (Index == CurrentItemIndex)
					{
						ActivePageByContent(CurrentItemIndex);
						break;
					}
				}

				//获取最后一项页码
				TSharedPtr<SArmyPageItem> PageQuery1 = MapPageItems.FindRef(MapPageItems.Num() - 2);
				PageQuery1->SetTextContentStr(FString::FromInt(PageCount));
				int32 LastIndex = FCString::Atoi(*PageQuery1->GetTextContentStr());
				//获取当年项的页码
				TSharedPtr<SArmyPageItem> PageQuery2 = MapPageItems.FindRef(CurrentItemIndex);
				int32 CurrentIndex = FCString::Atoi(*PageQuery2->GetTextContentStr());

				//计算当前项页号是否与最后一项能连接上
				bool bUpdateIndex = (LastIndex - CurrentIndex == (MapPageItems.Num() - 2 - CurrentItemIndex)) ? false : true;

				if (CurrentItemIndex <= MaxPageItemNum - 2 && bUpdateIndex)
				{
					//倒数第二项设置省略号
					TSharedPtr<SArmyPageItem> PageQuery = MapPageItems.FindRef(MapPageItems.Num() - 3);
					PageQuery->SetTextContentStr(Ellipsis);

					//最后一项设置最大页码
					TSharedPtr<SArmyPageItem> PageQueryLast = MapPageItems.FindRef(MapPageItems.Num() - 2);
					PageQueryLast->SetTextContentStr(FString::FromInt(PageCount));
				}
			}
		}
	}
	else if (InText.Equals(TEXT("<")))
	{
		int32 ActivePageIndex = GetCurrentActiveKey();
		if (ActivePageIndex == -1)
			return;
		if (ActivePageIndex != 1)
		{
			TSharedPtr<SArmyPageItem> Item = MapPageItems.FindRef(ActivePageIndex - 1);
			if (Item->GetTextContentStr().Equals(Ellipsis))//如果上一项为省略号
			{
				//获取当年项的页码
				TSharedPtr<SArmyPageItem> PageQueryCurrent = MapPageItems.FindRef(ActivePageIndex);
				int32 PageCurrentIndex = FCString::Atoi(*PageQueryCurrent->GetTextContentStr());

				for (int32 i = ActivePageIndex - 1; i < MapPageItems.Num() - 1; i++)
				{
					TSharedPtr<SArmyPageItem> PageQueryLast = MapPageItems.FindRef(i);
					FString TextStr = PageQueryLast->GetTextContentStr();
					if (TextStr.IsNumeric())
					{
						int32 CurrentIndex = FCString::Atoi(*TextStr);

						PageQueryLast->SetTextContentStr(FString::FromInt(CurrentIndex - 1));
					}
				}
				ActivePageByContent(ActivePageIndex);

				//序号相连时，取消第二项的省略号
				if (PageCurrentIndex - 1 == ActivePageIndex)
				{
					TSharedPtr<SArmyPageItem> PageQuerySecond = MapPageItems.FindRef(2);
					PageQuerySecond->SetTextContentStr(FString::FromInt(2));
				}

				//获取最后一项页码
				TSharedPtr<SArmyPageItem> PageQuery1 = MapPageItems.FindRef(MapPageItems.Num() - 2);
				PageQuery1->SetTextContentStr(FString::FromInt(PageCount));
				int32 LastIndex = FCString::Atoi(*PageQuery1->GetTextContentStr());
				//获取当年项的页码
				TSharedPtr<SArmyPageItem> PageQuery2 = MapPageItems.FindRef(ActivePageIndex);
				int32 CurrentIndex = FCString::Atoi(*PageQuery2->GetTextContentStr());

				CurrentPage = PageQuery2->GetTextContentStr();

				bool bUpdateIndex = (LastIndex - CurrentIndex == (MapPageItems.Num() - 2 - ActivePageIndex)) ? false : true;

				if (bUpdateIndex)
				{
					//倒数第二项设置省略号
					TSharedPtr<SArmyPageItem> PageQuery = MapPageItems.FindRef(MapPageItems.Num() - 3);
					PageQuery->SetTextContentStr(Ellipsis);

					//最后一项设置最大页码
					TSharedPtr<SArmyPageItem> PageQueryLast = MapPageItems.FindRef(MapPageItems.Num() - 2);
					PageQueryLast->SetTextContentStr(FString::FromInt(PageCount));
				}
			}
			else
			{
				ActivePageByContent(ActivePageIndex - 1);
				TSharedPtr<SArmyPageItem> PageQuery2 = MapPageItems.FindRef(ActivePageIndex - 1);
				CurrentPage = PageQuery2->GetTextContentStr();
			}
		}
		else
		{
			TSharedPtr<SArmyPageItem> PageQueryFirst = MapPageItems.FindRef(0);
			PageQueryFirst->SetbActive(false);
			PageQueryFirst->SetbSelect(false);
		}
	}
	else if (InText.Equals(TEXT(">")))
	{
		int32 ActivePageIndex = GetCurrentActiveKey();
		if (ActivePageIndex == -1)
			return;
		if (ActivePageIndex != MaxPageItemNum)
		{
			TSharedPtr<SArmyPageItem> Item = MapPageItems.FindRef(ActivePageIndex + 1);
			if (Item->GetTextContentStr().Equals(Ellipsis))//如果上一项为省略号
			{
				//获取当年项的页码
				TSharedPtr<SArmyPageItem> PageQueryCurrent = MapPageItems.FindRef(ActivePageIndex);
				int32 PageCurrentIndex = FCString::Atoi(*PageQueryCurrent->GetTextContentStr());

				for (int32 i = ActivePageIndex + 1; i > 0; i--)
				{
					TSharedPtr<SArmyPageItem> PageQueryLast = MapPageItems.FindRef(i);
					FString TextStr = PageQueryLast->GetTextContentStr();
					if (TextStr.IsNumeric())
					{
						int32 CurrentIndex = FCString::Atoi(*TextStr);

						PageQueryLast->SetTextContentStr(FString::FromInt(CurrentIndex + 1));
					}
				}

				ActivePageByContent(ActivePageIndex);

				//获取最后一项页码
				TSharedPtr<SArmyPageItem> PageQuery1 = MapPageItems.FindRef(MapPageItems.Num() - 2);
				PageQuery1->SetTextContentStr(FString::FromInt(PageCount));
				int32 LastIndex = FCString::Atoi(*PageQuery1->GetTextContentStr());
				//获取当年项的页码
				TSharedPtr<SArmyPageItem> PageQuery2 = MapPageItems.FindRef(ActivePageIndex);
				int32 CurrentIndex = FCString::Atoi(*PageQuery2->GetTextContentStr());

				CurrentPage = PageQuery2->GetTextContentStr();
				
				bool bUpdateIndex = (LastIndex - CurrentIndex == (MapPageItems.Num() - 2 - ActivePageIndex)) ? false : true;

				//序号相连时，取消倒数第二项的省略号
				if (!bUpdateIndex)
				{
					TSharedPtr<SArmyPageItem> PageQuerySecond = MapPageItems.FindRef(MapPageItems.Num() - 3);
					PageQuerySecond->SetTextContentStr(FString::FromInt(PageCount - 1));
				}

				if (ActivePageIndex < CurrentIndex)
				{
					//第二项设置省略号
					TSharedPtr<SArmyPageItem> PageQuery = MapPageItems.FindRef(2);
					PageQuery->SetTextContentStr(Ellipsis);

					//第一项设置最小页码
					TSharedPtr<SArmyPageItem> PageQueryLast = MapPageItems.FindRef(1);
					PageQueryLast->SetTextContentStr(FString::FromInt(1));
				}
			}
			else
			{
				ActivePageByContent(ActivePageIndex + 1);
				TSharedPtr<SArmyPageItem> PageQuery2 = MapPageItems.FindRef(ActivePageIndex + 1);
				CurrentPage = PageQuery2->GetTextContentStr();
			}
		}
		else
		{
			TSharedPtr<SArmyPageItem> PageQueryLast = MapPageItems.FindRef(MapPageItems.Num() - 1);
			PageQueryLast->SetbActive(false);
			PageQueryLast->SetbSelect(false);
		}
	}
}

void SArmyPageQuery::ActivePageByContent(const FString & InText)
{
	for (const auto & Ref : MapPageItems)
	{
		Ref.Value->SetbActive(false);
		Ref.Value->SetbSelect(false);
		if (Ref.Value->GetTextContentStr() == InText)
		{
			Ref.Value->SetbActive(true);
			Ref.Value->SetbSelect(true);
		}
	}
}

void SArmyPageQuery::ActivePageByContent(const int32 & InKey)
{
	for (const auto & Ref : MapPageItems)
	{
		Ref.Value->SetbActive(false);
		Ref.Value->SetbSelect(false);
		if (Ref.Key == InKey)
		{
			Ref.Value->SetbActive(true);
			Ref.Value->SetbSelect(true);
		}
	}
}

int32 SArmyPageQuery::GetCurrentActiveKey()
{
	for (const auto & Ref : MapPageItems)
	{
		if (Ref.Value->GetbSelect() && Ref.Value->GetTextContentStr().IsNumeric())
		{
			return Ref.Key;
		}
	}
	return -1;
}

void SArmyPageQuery::UpDatePage(int32 InPlanCount)
{
	DataCount = InPlanCount;
	Init();
}

void SArmyPageQuery::AddPageItem(TSharedPtr<class SWidget> WidItem)
{
	PageHorizontalBox->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(0, 0, 16, 0)
		[
			WidItem.ToSharedRef()
		];
}

void SArmyPageItem::Construct(const FArguments& InArgs)
{
	TextContentStr = InArgs._TextContentStr;
	OnItemClicked = InArgs._OnItemClicked;
	ChildSlot
	[
		SNew(SBox)
		.WidthOverride(32)
		.HeightOverride(32)
		[
			SNew(SButton)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.FF2E2F32"))
			//.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF9D9FA5"))
			.OnClicked(this, &SArmyPageItem::OnClick)
			.Content()
			[
				SAssignNew(TextContent, STextBlock)
				.Text(this, &SArmyPageItem::GetTextContent)
				.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
				.ColorAndOpacity(this,&SArmyPageItem::GetTextColor)
				.HighlightColor(FLinearColor(FColor(0XFFFFFFFF)))
			]
		]
	];
		
}

FSlateColor SArmyPageItem::GetTextColor() const
{
	if (bSelect)
	{
		return FArmyStyle::Get().GetColor("Color.FFFD9800");
	}
	else
	{
		return FLinearColor::White;
	}
}

FReply SArmyPageItem::OnClick()
{
	bActive = true;
	bSelect = true;
	OnItemClicked.ExecuteIfBound(TextContentStr);
	return FReply::Unhandled();
}
