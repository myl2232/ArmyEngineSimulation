#include "SArmyGreatPlanList.h"
#include "ArmyStyle.h"
#include "ArmyDesignEditor.h"
#include "ArmyGameInstance.h"
#include "SArmyPlanList.h"
#include "ArmyHttpModule.h"
#include "SImage.h"
#include "SArmyPageQuery.h"

void SArmyGreatPlanList::Construct(const FArguments& InArgs)
{
   // OnNewProjectClicked = InArgs._OnNewProjectClicked;
	RequestServerHouseDate();
	RequestServerStyleDate();
	RequestServerMeasureDate();
	RequestServerHouseNumDate();

    ChildSlot
    [
        SNew(SBorder)
        .Padding(FMargin(0, 32))
        .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF262729"))
        .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBox)
				.HeightOverride(96.f)
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(80, 0, 16, 0)
					[
						CreateHouseComboBox()
					]

					+SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(16, 0 , 16, 0)
					[
						CreateStyleComboBox()
					]

					+SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(16, 0 , 16,0)
					[
						CreateMeasureComboBox()
					]

					+SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(16,0,0,0)
					[
						CreateHouseNumComboBox()
					]

					+SHorizontalBox::Slot()
 					.VAlign(VAlign_Center)
					.HAlign(HAlign_Right)
					.Padding(0, 0 , 80 , 0)
					[
						SNew(SBox)
						.HeightOverride(32.f)
						.WidthOverride(240.f)
						[
							SNew(SOverlay)
							+SOverlay::Slot()
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Fill)
							[
								SAssignNew(MasterSearchEditableText, SEditableTextBox)
								.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
								.HintText(FText::FromString(TEXT("请输入方案名称")))
 								.OnTextCommitted_Raw(this, &SArmyGreatPlanList::OnSearchTextCommited)
 								.OnTextChanged_Raw(this, &SArmyGreatPlanList::OnTextChange)
 								.Text_Raw(this, &SArmyGreatPlanList::GetSearchTest)
							]

							+SOverlay::Slot()
							.Padding(0, 0, 4, 0)
							.HAlign(HAlign_Right)
							.VAlign(VAlign_Center)
							[
								SNew(SBox)
								.WidthOverride(16)
								.HeightOverride(16)
								[
									SNew(SButton)
									.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Search"))
									//.OnClicked(this, &SArmyModelContentBrowser::OnSearchClicked)
								]
							]
						]
					]
				]
			]
			+ SVerticalBox::Slot()
			[
				SAssignNew(Container, SScrollWrapBox)
				.WrapBoxPadding(FMargin(43, 20, 0, 0))
				.InnerSlotPadding(FVector2D(36, 32))
			]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.AutoHeight()
			[
				SNew(SBox)
				.HeightOverride(96.f)
				[
					SAssignNew(PageQuery,SArmyPageQuery)
				]
			]
		]
    ];
}

TSharedRef<SWidget> SArmyGreatPlanList::CreateHouseComboBox()
{
	return
		SNew(SBox)
		.HeightOverride(32.f)
		.WidthOverride(132.f)
		[
			SAssignNew(ETB_HouseComboBox, SComboBox< TSharedPtr<FArmyKeyValue> >)
			.ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.Black"))
			.ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.PropertyCombobox"))
			.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("ComboBox.Button.Gray"))
			.MaxListHeight(272)
			.HasDownArrow(true)
			.ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
			.OptionsSource(&AllHouseList)
			.OnGenerateWidget_Raw(this, &SArmyGreatPlanList::GenerateComboItem, 0)
			.OnSelectionChanged_Raw(this, &SArmyGreatPlanList::HandleComboItemChanged, 0)
		[
			SNew(STextBlock)
			.ColorAndOpacity(FLinearColor::White)
		.Text_Raw(this, &SArmyGreatPlanList::GetCurrentComboText, 0)
		.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		]
		];
}

TSharedRef<SWidget> SArmyGreatPlanList::CreateStyleComboBox()
{
	return
		SNew(SBox)
		.HeightOverride(32.f)
		.WidthOverride(132.f)
		[
			SAssignNew(ETB_StyleComboBox, SComboBox< TSharedPtr<FArmyKeyValue> >)
			.ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.Black"))
			.ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.PropertyCombobox"))
			.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("ComboBox.Button.Gray"))
			.MaxListHeight(272)
			.HasDownArrow(true)
			.ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
			.OptionsSource(&StyleList)
			.OnGenerateWidget_Raw(this, &SArmyGreatPlanList::GenerateComboItem, 1)
			.OnSelectionChanged_Raw(this, &SArmyGreatPlanList::HandleComboItemChanged, 1)
			[
				SNew(STextBlock)
				.ColorAndOpacity(FLinearColor::White)
				.Text_Raw(this, &SArmyGreatPlanList::GetCurrentComboText, 1)
				.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
			]
		];
		
}

TSharedRef<SWidget> SArmyGreatPlanList::CreateMeasureComboBox()
{
	return
		SNew(SBox)
		.HeightOverride(32.f)
		.WidthOverride(132.f)
		[
			SAssignNew(ETB_MeasureComboBox, SComboBox< TSharedPtr<FArmyKeyValue> >)
			.ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.Black"))
			.ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.PropertyCombobox"))
			.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("ComboBox.Button.Gray"))
			.MaxListHeight(272)
			.HasDownArrow(true)
			.ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
			.OptionsSource(&MeasureList)
			.OnGenerateWidget_Raw(this, &SArmyGreatPlanList::GenerateComboItem, 2)
			.OnSelectionChanged_Raw(this, &SArmyGreatPlanList::HandleComboItemChanged, 2)
			[
				SNew(STextBlock)
				.ColorAndOpacity(FLinearColor::White)
				.Text_Raw(this, &SArmyGreatPlanList::GetCurrentComboText, 2)
				.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
			]
		];
}

TSharedRef<SWidget> SArmyGreatPlanList::CreateHouseNumComboBox()
{
	return
		SNew(SBox)
		.HeightOverride(32.f)
		.WidthOverride(132.f)
		[
			SAssignNew(ETB_HouseNumComboBox, SComboBox< TSharedPtr<FArmyKeyValue> >)
			.ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.Gray"))
			.ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.PropertyCombobox"))
			.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("ComboBox.Button.Gray"))
			.MaxListHeight(272)
			.HasDownArrow(true)
			.ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
			.OptionsSource(&HouseNumList)
			.OnGenerateWidget_Raw(this, &SArmyGreatPlanList::GenerateComboItem, 3)
			.OnSelectionChanged_Raw(this, &SArmyGreatPlanList::HandleComboItemChanged, 3)
			[
				SNew(STextBlock)
				.ColorAndOpacity(FLinearColor::White)
			.Text_Raw(this, &SArmyGreatPlanList::GetCurrentComboText, 3)
			.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
			]
		];
		
}

TSharedRef<SWidget> SArmyGreatPlanList::GenerateComboItem(TSharedPtr<FArmyKeyValue> InItem, int32 InType)
{
	return
		SNew(STextBlock)
		.Text(FText::FromString(InItem->Value))
		.ColorAndOpacity(FLinearColor::White)
		.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"));
}

FText SArmyGreatPlanList::GetCurrentComboText(int32 InType) const
{
	TSharedPtr<FArmyKeyValue> ReturnValue = NULL;
	switch (InType)
	{
	case 0:
		ReturnValue = CurrentAllHouseValue;
		break;
	case 1:
		ReturnValue = CurrentStyle;
		break;
	case 2:
		ReturnValue = CurrentMeasure;
		break;
	case 3:
		ReturnValue = CurrentHouseNum;
		break;
	default:
		break;
	}
	return ReturnValue.IsValid() ? FText::FromString(ReturnValue->Value) : FText();
}

void SArmyGreatPlanList::CallBack_HouseList(struct FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful)
	{
		const TArray<TSharedPtr<FJsonValue>> JArray = Response.Data->GetArrayField("data");

		AllHouseList.Reset();
		for (auto& ArrayIt : JArray)
		{
			TSharedPtr<FJsonObject> JObject = ArrayIt->AsObject();
			int32 Id = JObject->GetIntegerField("id");
			FString Title = JObject->GetStringField("title");
			AllHouseList.Add(MakeShareable(new FArmyKeyValue(Id, Title)));
		}
	}
}

void SArmyGreatPlanList::CallBack_StyleList(struct FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful)
	{
		const TArray<TSharedPtr<FJsonValue>> JArray = Response.Data->GetArrayField("data");

		StyleList.Reset();
		for (auto& ArrayIt : JArray)
		{
			TSharedPtr<FJsonObject> JObject = ArrayIt->AsObject();
			int32 Id = JObject->GetIntegerField("id");
			FString Title = JObject->GetStringField("title");
			StyleList.Add(MakeShareable(new FArmyKeyValue(Id, Title)));
		}
	}
}

void SArmyGreatPlanList::CallBack_MeasureList(struct FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful)
	{
		const TArray<TSharedPtr<FJsonValue>> JArray = Response.Data->GetArrayField("data");

		MeasureList.Reset();
		for (auto& ArrayIt : JArray)
		{
			TSharedPtr<FJsonObject> JObject = ArrayIt->AsObject();
			int32 Id = JObject->GetIntegerField("id");
			FString Title = JObject->GetStringField("title");
			MeasureList.Add(MakeShareable(new FArmyKeyValue(Id, Title)));
		}
	}
}

void SArmyGreatPlanList::CallBack_HouseNumList(struct FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful)
	{
		const TArray<TSharedPtr<FJsonValue>> JArray = Response.Data->GetArrayField("data");

		HouseNumList.Reset();
		for (auto& ArrayIt : JArray)
		{
			TSharedPtr<FJsonObject> JObject = ArrayIt->AsObject();
			int32 Id = JObject->GetIntegerField("id");
			FString Title = JObject->GetStringField("title");
			HouseNumList.Add(MakeShareable(new FArmyKeyValue(Id, Title)));
		}
	}
}

void SArmyGreatPlanList::RequestServerHouseDate()
{
	//全屋
	FArmyHttpRequestCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindRaw(this, &SArmyGreatPlanList::CallBack_HouseList);
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest("/api/list_set_meal/0", CompleteDelegate);
	Request->ProcessRequest();
}

void SArmyGreatPlanList::RequestServerStyleDate()
{
	//风格
	FArmyHttpRequestCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindRaw(this, &SArmyGreatPlanList::CallBack_StyleList);
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest("/api/list_set_meal/0", CompleteDelegate);
	Request->ProcessRequest();
}

void SArmyGreatPlanList::RequestServerMeasureDate()
{
	//面积
	FArmyHttpRequestCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindRaw(this, &SArmyGreatPlanList::CallBack_MeasureList);
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest("/api/list_set_meal/0", CompleteDelegate);
	Request->ProcessRequest();
}

void SArmyGreatPlanList::RequestServerHouseNumDate()
{
	//居室数
	FArmyHttpRequestCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindRaw(this, &SArmyGreatPlanList::CallBack_HouseNumList);
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest("/api/list_set_meal/0", CompleteDelegate);
	Request->ProcessRequest();
}

void SArmyGreatPlanList::HandleComboItemChanged(TSharedPtr<FArmyKeyValue> NewSelection, ESelectInfo::Type SelectInfo, int32 InType)
{
	if (!NewSelection.IsValid()) return;


	switch (InType)
	{
	case 0:
		CurrentAllHouseValue = NewSelection;
		SelectedAllHouseName = NewSelection->Value;
		SelectedAllHouseID = NewSelection->Key;
		//ReqGetMasterPlanList();
		break;
	case 1:
		CurrentStyle = NewSelection;
		SelectedStyle = NewSelection->Value;
		SelectedStyleID = NewSelection->Key;
		break;
	case 2:
		CurrentMeasure = NewSelection;
		SelectedMeasure = NewSelection->Value;
		SelectedMeasureID = NewSelection->Key;
	case 3:
		CurrentHouseNum = NewSelection;
		SelectedHouseNum = NewSelection->Value;
		SelectedHouseNumID = NewSelection->Key;
	default:
		break;
	}
}

void SArmyGreatPlanList::ProcessSearch()
{

}

void SArmyGreatPlanList::OnSearchTextCommited(const FText& _NewText, ETextCommit::Type _Committype)
{
	if (_Committype == ETextCommit::OnEnter)
	{
		ProcessSearch();
	}
}

void SArmyGreatPlanList::OnTextChange(const FText& text)
{
	SearchText.Reset();
	SearchText.Add(text.ToString());
	SearchBoxText = text;
}

FReply SArmyGreatPlanList::OnSearchClicked()
{
	ProcessSearch();
	return FReply::Handled();
}

void SArmyGreatPlanList::ReqGetMasterPlanList(const int32& AllHouseID, const int32& StyleID, const int32& MeasureID, const int32& HouseNumID)
{
	//Widget->ClearChildren();  下面的布局界面
	//Widget ->Reset();

// 	FString Url = FString::Printf(TEXT("/api/project/%d/plan"), AllHouseID, StyleID, MeasureID, HouseNumID);
// 	FArmyHttpRequestCompleteDelegate CompleteDelegate;
// 	CompleteDelegate.BindRaw(this, &SArmyGreatPlanList::ResGetGreatPlanList);
// 	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest(Url, CompleteDelegate);
// 	Request->ProcessRequest();

}

void SArmyGreatPlanList::ResGetGreatPlanList(struct FArmyHttpResponse Response)
{

}

void SArmyGreatPlanList::FillPlanList(TArray<FContentItemPtr> ItemList)
{

}


void SArmyGreatPlanList::AddPlanWidget(TSharedRef<SWidget> Widget)
{
    Container->AddItem(Widget);
}

void SArmyGreatPlanList::EmptyProjectList()
{
    Container->ClearChildren();
}

void SArmyGreatPlanList::UpData(int32 InPlanCount)
{
	PageQuery->UpDatePage(InPlanCount);
}

//
//TSharedRef<SWidget> SArmyGreatPlanList::MakeNewProjectButton()
//{
//    return
//        SNew(SButton)
//        .ContentPadding(FMargin(0))
//        .ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.None"))
//        .OnClicked(OnNewProjectClicked)
//        .OnHovered(this, &SArmyProjectList::OnNewProjectHovered)
//        .OnUnhovered(this, &SArmyProjectList::OnNewProjectUnhovered)
//        [
//            SNew(SBox)
//            .WidthOverride(224)
//            .HeightOverride(300)
//            [
//                SAssignNew(NewProjectBorder, SBorder)
//                .Padding(FMargin(2))
//                .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
//                .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
//                [
//                    SNew(SBorder)
//                    .Padding(FMargin(7))
//                    .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
//                    .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
//                    [
//                        SNew(SVerticalBox)
//
//                        + SVerticalBox::Slot()
//                        .AutoHeight()
//                        [
//                            SNew(SImage)
//                            .Image(FArmyStyle::Get().GetBrush("Icon.NewProject"))
//                        ]
//
//                        + SVerticalBox::Slot()
//                        .FillHeight(1)
//                        .VAlign(VAlign_Center)
//                        .HAlign(HAlign_Center)
//                        [
//                            SNew(STextBlock)
//                            .Text(FText::FromString(TEXT("新建项目")))
//                            .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_14"))
//                        ]
//                    ]
//                ]
//            ]
//        ];
//}
//
//void SArmyGreatPlanList::OnNewProjectHovered()
//{
//    NewProjectBorder->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FFFD9800"));
//}
//
//void SArmyGreatPlanList::OnNewProjectUnhovered()
//{
//    NewProjectBorder->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"));
//}

