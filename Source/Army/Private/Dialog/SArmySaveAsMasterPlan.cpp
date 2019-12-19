#include "SArmySaveAsMasterPlan.h"
#include "SEditableTextBox.h"
#include "ArmyGameInstance.h"
#include "ArmyViewportClient.h"
#include "ArmyEngineTools.h"
#include "ArmyStyle.h"
#include "SArmyEditableNumberBox.h"
#include "SArmyMessage.h"
#include "SImage.h"
#include "Json.h"
#include "ArmyUser.h"
#include "ArmyDesignEditor.h"
#include "ArmyCommonTools.h"

#include "Widgets/Input/SMultiLineEditableTextBox.h"

#include "ArmyAutoDesignModel.h"

void SArmySaveAsMasterPlan::Construct(const FArguments & InArgs)
{
	PlanName = FArmyUser::Get().CurPlanData->Name;

	ChildSlot
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.Padding(20, 20, 20, 0)
			.VAlign(EVerticalAlignment::VAlign_Top)
			[
				SNew(SBox)
				.WidthOverride(204)
				.HeightOverride(204)
				[
					SAssignNew(Img_Thumbnail, SImage)
				]
			]
			+ SHorizontalBox::Slot()
			.Padding(20, 0, 20, 0)
			.AutoWidth()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(0, 20, 20, 0)
				.AutoHeight()
				[
					//方案名称
					CreateMasterPlanNameWidget()
				]

				// 错误信息
				+ SVerticalBox::Slot()
				.Padding(84, 5, 0, 0)
				.AutoHeight()
				[
					SAssignNew(TB_ErrorMessage, STextBlock)
					.Text(FText::FromString(TEXT("方案名称不能为空")))
					.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
					.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFFF5A5A"))
					.Visibility(EVisibility::Collapsed)
				]

				+ SVerticalBox::Slot()
				.Padding(0, 20, 20, 0)
				.AutoHeight()
				[
					//选择风格
					CreateProjectWidget()
				]

				+ SVerticalBox::Slot()
				.Padding(0, 20, 20, 0)
				.AutoHeight()
				[
					//户型面积
					CreatePlanAreaWidget()
				]

				+ SVerticalBox::Slot()
				.Padding(0, 20, 20, 0)
				.AutoHeight()
				[
					//选择房型
					CreateHouseTypeWidget()
				]

				+ SVerticalBox::Slot()
				.Padding(0, 20, 20, 0)
				.VAlign(VAlign_Bottom)
				.AutoHeight()
				[
					//方案介绍
					CreatePlanInfoWidget()
				]
			]
		];

	TakePlanScreentshot();
	Init();
}

TSharedRef<SWidget> SArmySaveAsMasterPlan::CreateProjectWidget()
{
	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(0, 0, 20, 0)
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.WidthOverride(64)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("风格")))
				.TextStyle(FArmyStyle::Get(), "ArmyText_12")
				.Justification(ETextJustify::Right)
			]
		]
		+ SHorizontalBox::Slot()
		.FillWidth(1)
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		[
			SNew(SBox)
			.WidthOverride(368)
			.HeightOverride(30)
			[
				SAssignNew(StyleComboBox, SComboBox< TSharedPtr<FArmyKeyValue> >)
				.ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.None"))
				.ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.PropertyCombobox"))
				.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("ComboBox.Button.FF212224"))
				.OptionsSource(&StyleList.Array)
				.MaxListHeight(200.f)
				.OnGenerateWidget(this, &SArmySaveAsMasterPlan::OnGenerateComoboAreaWidget)
				.OnSelectionChanged(this, &SArmySaveAsMasterPlan::OnSlectedStyleChanged)
				.Content()
				[
					SNew(SBox)
					.WidthOverride(368)
					.HeightOverride(30)
					.VAlign(VAlign_Center)
					[
						SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
						.Padding(FMargin(8, 0, 0, 0))
						[
							SNew(STextBlock)
							.TextStyle(FArmyStyle::Get(), "ArmyText_12")
							.Text(this, &SArmySaveAsMasterPlan::GetCurrentStyle)
						]

						+ SHorizontalBox::Slot()
						.HAlign(EHorizontalAlignment::HAlign_Right)
						.Padding(FMargin(0, 0, 0, 0))
						.AutoWidth()
						[
							SNew(SImage)
							.Image(FArmyStyle::Get().GetBrush("Icon.DownArrow_Gray"))
						]
					]
				]
			]
		];
}

TSharedRef<SWidget> SArmySaveAsMasterPlan::CreatePlanAreaWidget()
{
	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(0, 0, 20, 0)
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.WidthOverride(64)
			[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("户型面积")))
			.TextStyle(FArmyStyle::Get(), "ArmyText_12")
			.Justification(ETextJustify::Right)
			]
		]
		+ SHorizontalBox::Slot()
			.FillWidth(1)
			.HAlign(EHorizontalAlignment::HAlign_Fill)
			[
				SNew(SBox)
				.WidthOverride(368)
				.HeightOverride(30)
				[
					SAssignNew(AreaComboBox, SComboBox< TSharedPtr<FArmyKeyValue> >)
					.ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.None"))
					.ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.PropertyCombobox"))
					.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("ComboBox.Button.FF212224"))
					.OptionsSource(&AreaList.Array)
					.MaxListHeight(200.f)
					.OnGenerateWidget(this, &SArmySaveAsMasterPlan::OnGenerateComoboAreaWidget)
					.OnSelectionChanged(this, &SArmySaveAsMasterPlan::OnSlectedAreaChanged)
					.Content()
					[
						SNew(SBox)
						.WidthOverride(368)
						.HeightOverride(30)
						.VAlign(VAlign_Center)
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.Padding(FMargin(8, 0, 0, 0))
							[
								SAssignNew(AreaTextInfo,STextBlock)
								.TextStyle(FArmyStyle::Get(), "ArmyText_12")
								.Text(this, &SArmySaveAsMasterPlan::GetCurrentArea)
							]

							+ SHorizontalBox::Slot()
							.HAlign(EHorizontalAlignment::HAlign_Right)
							.Padding(FMargin(0, 0, 0, 0))
							.AutoWidth()
							[
								SNew(SImage)
								.Image(FArmyStyle::Get().GetBrush("Icon.DownArrow_Gray"))
							]
						]
					]
				]
		];
}

TSharedRef<SWidget> SArmySaveAsMasterPlan::CreatePlanInfoWidget()
{
	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(0, 0, 20, 0)
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.WidthOverride(70)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("方案介绍")))
				.TextStyle(FCoreStyle::Get(), "VRSText_12")
				.Justification(ETextJustify::Right)
			]
		]

	+ SHorizontalBox::Slot()
		.FillWidth(1)
		[
			SNew(SBox)
			.WidthOverride(368)
			.HeightOverride(70)
		[
			SAssignNew(METB_PlanName, SMultiLineEditableTextBox)
			.Padding(FMargin(8, 0, 0, 0))
			.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox.FF212224"))
			.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
			.HintText(FText::FromString(TEXT(" 请输入方案介绍")))
			.OnTextChanged(this, &SArmySaveAsMasterPlan::SetMasterDescText)
			.AutoWrapText(true)
			.WrapTextAt(0.0)
			.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
		]
	];
}

TSharedRef<SWidget> SArmySaveAsMasterPlan::CreateMasterPlanNameWidget()
{
	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(0, 0, 20, 0)
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.WidthOverride(64)
			[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("方案名称")))
			.TextStyle(FCoreStyle::Get(), "VRSText_12")
			.Justification(ETextJustify::Right)
			]
		]

		+ SHorizontalBox::Slot()
		.FillWidth(1)
		[
			SNew(SBox)
			.WidthOverride(368)
			.HeightOverride(32)
			[
				SAssignNew(ETB_PlanName, SEditableTextBox)
				.Padding(FMargin(8, 0, 0, 0))
				.ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
				.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox.FF212224"))
				.Text(FText::FromString(PlanName))
				.AllowContextMenu(false)
			]
		];
}

TSharedRef<SWidget> SArmySaveAsMasterPlan::CreateMasterHouseTypeWidget()
{
	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(0, 0, 20, 0)
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.WidthOverride(64)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("房型")))
		.TextStyle(FArmyStyle::Get(), "ArmyText_12")
		.Justification(ETextJustify::Right)
		]
		]

	// 几室
	+ SHorizontalBox::Slot()
		.FillWidth(1)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.Padding(0, 0, 5, 0)
		.AutoWidth()
		[
			SNew(SBox)
			.WidthOverride(40)
		.HeightOverride(32)
		[
			SAssignNew(ETB_Bedroom, SArmyEditableNumberBox)
			.Padding(FMargin(8, 0, 0, 0))
		.ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox.FF212224"))
		.IsIntegerOnly(true)
		.Text(FText::FromString("0"))
		]
		]

	+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("室")))
		.TextStyle(FArmyStyle::Get(), "ArmyText_12")
		]
		]

	// 几厅
	+ SHorizontalBox::Slot()
		.FillWidth(1)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.Padding(0, 0, 5, 0)
		.AutoWidth()
		[
			SNew(SBox)
			.WidthOverride(40)
		.HeightOverride(32)
		[
			SAssignNew(ETB_LivingRoom, SArmyEditableNumberBox)
			.Padding(FMargin(8, 0, 0, 0))
		.ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox.FF212224"))
		.IsIntegerOnly(true)
		.Text(FText::FromString("0"))
		]
		]

	+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("厅")))
		.TextStyle(FArmyStyle::Get(), "ArmyText_12")
		]
		]

	// 几厨
	+ SHorizontalBox::Slot()
		.FillWidth(1)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.Padding(0, 0, 5, 0)
		.AutoWidth()
		[
			SNew(SBox)
			.WidthOverride(40)
		.HeightOverride(32)
		[
			SAssignNew(ETB_Kitchen, SArmyEditableNumberBox)
			.Padding(FMargin(8, 0, 0, 0))
		.ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox.FF212224"))
		.IsIntegerOnly(true)
		.Text(FText::FromString("0"))
		]
		]

	+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("厨")))
		.TextStyle(FArmyStyle::Get(), "ArmyText_12")
		]
		]

	// 几卫
	+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.Padding(0, 0, 5, 0)
		.AutoWidth()
		[
			SNew(SBox)
			.WidthOverride(40)
		.HeightOverride(32)
		[
			SAssignNew(ETB_Bathroom, SArmyEditableNumberBox)
			.Padding(FMargin(8, 0, 0, 0))
		.ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox.FF212224"))
		.IsIntegerOnly(true)
		.Text(FText::FromString("0"))
		]
		]

	+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("卫")))
		.TextStyle(FArmyStyle::Get(), "ArmyText_12")
		]
		];
}

void SArmySaveAsMasterPlan::Init()
{
	if (FArmyUser::Get().CurPlanData.IsValid())
	{
		/*FArmyCommonTools::ParseContentItemFromJson();*/
		TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = FArmyUser::Get().CurPlanData->GetResObjNoComponent();
		if (resArr.Num() < 1)
			return;
		TSharedPtr<FArmyPlanRes> HomeRes = StaticCastSharedPtr<FArmyPlanRes>(resArr[0]);
		if (HomeRes.IsValid()) {
			ETB_Bedroom->SetText(FText::FromString(FString::Printf(TEXT("%d"), HomeRes->BedroomNum)));
			ETB_LivingRoom->SetText(FText::FromString(FString::Printf(TEXT("%d"), HomeRes->LivingRoomNum)));
			ETB_Kitchen->SetText(FText::FromString(FString::Printf(TEXT("%d"), HomeRes->KitchenNum)));
			ETB_Bathroom->SetText(FText::FromString(FString::Printf(TEXT("%d"), HomeRes->BathroomNum)));
			MasterPlanAreaRangeID = CalAreaRangeID(HomeRes->FloorArea);
		}
	}

	ReqGetStyleList();
	ReqGetAreaList();

}

void SArmySaveAsMasterPlan::OnSlectedStyleChanged(TSharedPtr<FArmyKeyValue> NewSelection, ESelectInfo::Type SelectInfo)
{
	if (NewSelection.IsValid())

		/**@梁晓菲 遍历ComboboxList，将非选中项设为正常*/
		for (int i = 0; i < ComboboxUIStyleList.Num(); i++)
		{
			if (ComboboxUIStyleList[i]->GetText() != NewSelection->Value)
			{
				ComboboxUIStyleList[i]->SetTextColorAndImageBrushNormal();
			}
		}
	MasterPlanStyle = NewSelection->Value;
	MasterPlanStyleID = NewSelection->Key;

}


void SArmySaveAsMasterPlan::OnSlectedAreaChanged(TSharedPtr<FArmyKeyValue> NewSelection, ESelectInfo::Type SelectInfo)
{
	if (NewSelection.IsValid())

		/**@梁晓菲 遍历ComboboxList，将非选中项设为正常*/
		for (int i = 0; i < ComboboxUIStyleList.Num(); i++)
		{
			if (ComboboxUIStyleList[i]->GetText() != NewSelection->Value)
			{
				ComboboxUIStyleList[i]->SetTextColorAndImageBrushNormal();
			}
		}
	MasterPlanArea = NewSelection->Value;
	MasterPlanAreaID = NewSelection->Key;
}

void SArmySaveAsMasterPlan::ReqGetAreaList()
{
	FArmyHttpRequestCompleteDelegate CompleteAreaDelegate;
	CompleteAreaDelegate.BindRaw(this, &SArmySaveAsMasterPlan::ResGetAreaList);
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest("/api/aiPlans/areaRanges", CompleteAreaDelegate);
	Request->ProcessRequest();
}

void SArmySaveAsMasterPlan::ResGetAreaList(FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful)
	{
		const TArray<TSharedPtr<FJsonValue>> JArray = Response.Data->GetArrayField("data");

		AreaList.Reset();
		for (auto& ArrayIt : JArray)
		{
			TSharedPtr<FJsonObject> JObject = ArrayIt->AsObject();
			int32 Id = JObject->GetIntegerField("id");
			FString name = JObject->GetStringField("name");
			AreaList.Add(MakeShareable(new FArmyKeyValue(Id, name)));	
		}

		
		TSharedPtr<FArmyKeyValue> AreaObj = AreaList.FindByKey(MasterPlanAreaRangeID);
		if (AreaObj.IsValid())
		{
			MasterPlanArea = AreaObj->Value;
			MasterPlanAreaID = MasterPlanAreaRangeID;
		}
		else
		{
			MasterPlanAreaID = 1;
			AreaObj = AreaList.FindByKey(MasterPlanAreaID);
			if (AreaObj.IsValid())
			{
				MasterPlanArea = AreaObj->Value;
			}		
		}

	}
}

void SArmySaveAsMasterPlan::ReqGetStyleList()
{
	FArmyHttpRequestCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindRaw(this, &SArmySaveAsMasterPlan::ResGetStyleList);
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest("/api/aiPlans/designStyles", CompleteDelegate);
	Request->ProcessRequest();
}

void SArmySaveAsMasterPlan::ResGetStyleList(struct FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful)
	{
		const TArray<TSharedPtr<FJsonValue>> JArray = Response.Data->GetArrayField("data");

		StyleList.Reset();
		for (auto& ArrayIt : JArray)
		{
			TSharedPtr<FJsonObject> JObject = ArrayIt->AsObject();
			int32 Id = JObject->GetIntegerField("id");
			FString name = JObject->GetStringField("name");
			StyleList.Add(MakeShareable(new FArmyKeyValue(Id, name)));
		}

		TSharedPtr<FArmyKeyValue> StyleObj = StyleList.FindByKey(1);
		if (StyleObj.IsValid())
		{
			MasterPlanStyle = StyleObj->Value;
			MasterPlanStyleID = 1;
		}
	}
}

void SArmySaveAsMasterPlan::ReqSavePlan()
{

	FString JStr;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JStr);

	//大师方案数据
	FString PackageJStr;
	if(!(FArmyAutoDesignModel::Get()->CreateJsonDesignPackage(PackageJStr)) || PackageJStr.Len() < 1)
	{
		GGI->Window->DismissModalDialog();
		GGI->Window->ShowMessage(MT_Warning, TEXT("当前方案下未检测到素材。请完成设计后，再另存为大师方案!"));
		return;
	}

	//FArmyAutoDesignModel::Get()->CreateJsonDesignPackage(PackageJStr);

	// 开始写入智能方案
	JsonWriter->WriteObjectStart();
	//TEXT("")
	JsonWriter->WriteValue("planId", SelectedProjectID);  //原方案ID
	JsonWriter->WriteValue("name", ETB_PlanName->GetText().ToString()); //智能方案名称
	JsonWriter->WriteValue("designStyleId", MasterPlanStyleID); //设计风格ID
	JsonWriter->WriteValue("areaRangeId", MasterPlanAreaID); //面积范围ID
	JsonWriter->WriteValue("thumbnailUrl", PlanThumbnailUrl); //缩略图
	JsonWriter->WriteValue("panoUrl", TEXT("")); //全景图 --待修改
	JsonWriter->WriteValue("description", IntelPlanDescription); //智能方案描述
	JsonWriter->WriteValue("bedroomCount", FCString::Atoi(*ETB_Bedroom->GetText().ToString())); //卧室数量
	JsonWriter->WriteValue("livingRoomCount", FCString::Atoi(*ETB_LivingRoom->GetText().ToString())); //客厅数量
	JsonWriter->WriteValue("kitchenCount", FCString::Atoi(*ETB_Kitchen->GetText().ToString()));  // 厨房数量
	JsonWriter->WriteValue("bathroomCount", FCString::Atoi(*ETB_Bathroom->GetText().ToString())); //卫生间数量

	JsonWriter->WriteValue("planJson", PackageJStr); //智能方案空间和使用商品数据

													 // 结束写入方案
	JsonWriter->WriteObjectEnd();
	JsonWriter->Close();

	//如果之前请求残留，取消
	if (SubmitSaveRequest.IsValid())
	{
		SubmitSaveRequest->OnProcessRequestComplete().Unbind();
		SubmitSaveRequest->CancelRequest();
		SubmitSaveRequest = nullptr;
	}

	FString Url = TEXT("/api/aiPlans");
	FArmyHttpRequestCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindRaw(this, &SArmySaveAsMasterPlan::ResSavePlan);
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest(Url, CompleteDelegate, JStr);
	Request->ProcessRequest();
}

void SArmySaveAsMasterPlan::ResSavePlan(FArmyHttpResponse Response)
{
	GGI->Window->HideThrobber();
	if (Response.bWasSuccessful)
	{
		double code = Response.Data->GetNumberField("code");
		if (code == 1)
		{
			GGI->Window->DismissModalDialog();
			GGI->Window->ShowMessage(MT_Success, TEXT("大师方案保存成功"));
		}
		else
		{
			const FString msgStr = Response.Data->GetStringField("message");
			if (!msgStr.IsEmpty())
			{
				GGI->Window->DismissModalDialog();
				GGI->Window->ShowMessage(MT_Warning, msgStr);
			}
			else
			{
				GGI->Window->DismissModalDialog();
				GGI->Window->ShowMessage(MT_Warning, TEXT("大师方案保存失败!"));
			}
		}
	}
	else
	{
		GGI->Window->ShowMessage(MT_Warning, Response.Message);
	}
}

//////////

void SArmySaveAsMasterPlan::SetMasterDescText(const FText& InText)
{
	IntelPlanDescription = InText.ToString();
}

int32 SArmySaveAsMasterPlan::CalAreaRangeID(double HouseArea)
{
	int32 AreaRangeID = 1;
	if (HouseArea <= 60.0f)
	{
		AreaRangeID = 1;
	}
	else if (HouseArea > 60.0f && HouseArea < 80.0f)
	{
		AreaRangeID = 2;
	}
	else if (HouseArea > 80.0f && HouseArea <= 100.0f)
	{
		AreaRangeID = 3;
	}
	else if (HouseArea >100.0f && HouseArea <= 120.0f)
	{
		AreaRangeID = 4;
	}
	else if (HouseArea > 120.0f && HouseArea <= 160.0f)
	{
		AreaRangeID = 5;
	}
	else if (HouseArea > 160.0f)
	{
		AreaRangeID = 6;
	}
	return AreaRangeID;
}

TSharedRef<SWidget> SArmySaveAsMasterPlan::OnGenerateComoboStyleWidget(TSharedPtr<FArmyKeyValue> InItem)
{
	/**存储ComboboxStyleItem*/
	TSharedPtr<SArmyComboboxItem> ComboboxStyleItem;

	SAssignNew(ComboboxStyleItem, SArmyComboboxItem)
		.TextNormalColor(FArmyStyle::Get().GetColor("Color.Gray.FFC8C9CC"))
		.TextSelectedColor(FArmyStyle::Get().GetColor("Color.FFFF9800"))
		.Text(FText::FromString(InItem->Value))
		.Height(32);

	ComboboxUIAreaList.Add(ComboboxStyleItem);

	return ComboboxStyleItem.ToSharedRef();
}

TSharedRef<SWidget> SArmySaveAsMasterPlan::OnGenerateComoboAreaWidget(TSharedPtr<FArmyKeyValue> InItem)
{
	/**存储ComboboxStyleItem*/
	TSharedPtr<SArmyComboboxItem> ComboboxAreaItem;

	SAssignNew(ComboboxAreaItem, SArmyComboboxItem)
		.TextNormalColor(FArmyStyle::Get().GetColor("Color.Gray.FFC8C9CC"))
		.TextSelectedColor(FArmyStyle::Get().GetColor("Color.FFFF9800"))
		.Text(FText::FromString(InItem->Value))
		.Height(32);

	ComboboxUIStyleList.Add(ComboboxAreaItem);

	return ComboboxAreaItem.ToSharedRef();
}