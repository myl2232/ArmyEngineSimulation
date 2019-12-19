#include "SArmyNewProject.h"
#include "SEditableTextBox.h"
#include "STextBlock.h"
#include "SArmyEditableNumberBox.h"
#include "SArmyEditableTextBox.h"
#include "string.h"
#include "ArmyHttpModule.h"

void SArmyNewProject::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SBox)
        .WidthOverride(460)
        .Padding(FMargin(20, 20, 20, 0))
        [
            SNew(SVerticalBox)

            // 项目名称
            + SVerticalBox::Slot()
			.Padding(0, 20, 0, 0)
            .AutoHeight()
            [
				CreateProjectName()
            ]

            // 选择套餐
            + SVerticalBox::Slot()
			.Padding(0, 20, 0, 0)
            .AutoHeight()
            [
				CreateSetMealID()
            ]

            // 合同编号
            + SVerticalBox::Slot()
			.Padding(0, 20, 0, 0)
            .AutoHeight()
            [
				CreateContractID()
            ]

            // 业主名称
            + SVerticalBox::Slot()
			.Padding(0, 20, 0, 0)
            .AutoHeight()
            [
				CreateOwnerName()
            ]

            // 业主电话
            + SVerticalBox::Slot()
			.Padding(0, 20, 0, 0)
            .AutoHeight()
            [
				CreateOwnerPhoneNumber()
            ]

			//所在地区
			+ SVerticalBox::Slot()
			.Padding(0, 20, 0, 0)
			.AutoHeight()
			[		
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("所在地区")))
					.TextStyle(FCoreStyle::Get(), "VRSText_12")
					.Justification(ETextJustify::Right)
				]

				+ SHorizontalBox::Slot()
				.FillWidth(1)
				[
					SNew(SBox)
					.WidthOverride(368)
					.HeightOverride(30)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.Padding(20, 0, 20, 0)
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(SBox)
							.WidthOverride(100)
							.HeightOverride(32)
							[
								CreateProvinceComboBox()
							]
						]

						+ SHorizontalBox::Slot()
						.Padding(0, 0, 20, 0)
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(SBox)
							.WidthOverride(100)
							.HeightOverride(32)
							[
								CreateCityComboBox()
							]
						]

						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(SBox)
							.WidthOverride(100)
							.HeightOverride(32)
							[
								CreateDistrictComboBox()
							]
						]
					]
				]
			]

            // 所在小区
            + SVerticalBox::Slot()
			.Padding(0, 20, 0, 0)
            .AutoHeight()
            [
				CreateVillage()
            ]

            // 详细地址
            + SVerticalBox::Slot()
			.Padding(0, 20, 0, 0)
            .AutoHeight()
            [
				CreateDetailAddress()
            ]

			// @欧石楠错误信息
			+ SVerticalBox::Slot()
			.Padding(80, 5, 0, 0)
			.AutoHeight()
			[
				SAssignNew(TB_ErrorMessage, STextBlock)
				.Text(FText::FromString(TEXT("项目名称不能为空")))
				.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
				.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFFF5A5A"))
				.Visibility(EVisibility::Collapsed)
			]
		]
    ];

	RequestServerData();
}

const FArmyProjectInputInfo SArmyNewProject::GetProjectInputInfo()
{
	/**@梁晓菲 获取用户输入的信息*/
	FArmyProjectInputInfo Result;
    Result.ProjectName = ETB_ProjectName->GetText().ToString().TrimStartAndEnd();
	Result.SetMealID = CurrentGroupValue->Key;
	Result.ProjectCode = ETB_ProjectCode->GetText().ToString().TrimStartAndEnd();
	Result.OwnerName = ETB_OwnerName->GetText().ToString();
	Result.OwnerPhoneNumber = ETB_OwnerPhoneNumber->GetText().ToString();
	Result.ProvinceID = CurrentProvince->Key;
	Result.CityID = CurrentCity->Key;
	Result.DistrictID = CurrentDistrict->Key;
	Result.VillageName = ETB_Village->GetText().ToString().TrimStartAndEnd();
    Result.DetailAddress = ETB_DetailAddress->GetText().ToString().TrimStartAndEnd();

	return Result;
}

bool SArmyNewProject::CheckIsValid()
{
    if (ETB_ProjectName->GetText().ToString().IsEmpty())
    {
        TB_ErrorMessage->SetText(MAKE_TEXT("请输入项目名称"));
        TB_ErrorMessage->SetVisibility(EVisibility::Visible);
        return false;
    }

    if (CurrentGroupValue->Key == -1)
    {
        TB_ErrorMessage->SetText(FText::FromString(TEXT("请选择套餐")));
        TB_ErrorMessage->SetVisibility(EVisibility::Visible);
        return false;
    }

	TB_ErrorMessage->SetVisibility(EVisibility::Collapsed);
	return true;
}

TSharedRef<SWidget> SArmyNewProject::CreateContractID()
{
	return
        SNew(SHorizontalBox)

        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("项目编码")))
            .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
        ]

        + SHorizontalBox::Slot()
        .Padding(20, 0, 0, 0)
        .FillWidth(1)
        [
            SNew(SBox)
            .HeightOverride(32)
            [
                SAssignNew(ETB_ProjectCode, SArmyEditableTextBox)
                .HintText(FText::FromString(TEXT("请输入项目编码")))
                .MaxTextLength(20)
            ]
	    ];
}

TSharedRef<SWidget> SArmyNewProject::CreateSetMealID()
{
	return
        SNew(SBox)
        .HeightOverride(32)
        [
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("*所选套餐")))
                .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
            ]

            + SHorizontalBox::Slot()
            .Padding(20, 0, 0, 0)
            .FillWidth(1)
            [	
		        SAssignNew(ETB_SetMealID, SComboBox< TSharedPtr<FArmyKeyValue> >)
		        .ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.PropertyCombobox"))
		        .ComboBoxStyle(FArmyStyle::Get(), "ComboBox")
		        .HasDownArrow(true)
		        .ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
		        .OptionsSource(&GroupTypeList)
		        .OnGenerateWidget(this, &SArmyNewProject::GenerateComboItem,0)
		        .OnSelectionChanged(this, &SArmyNewProject::HandleComboItemChanged,0)
		        [
			        SNew(STextBlock)
			        .ColorAndOpacity(FLinearColor::White)
			        .Text(this, &SArmyNewProject::GetCurrentComboText, 0)
                    .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		        ]
	        ]
        ];
}

TSharedRef<SWidget> SArmyNewProject::GenerateComboItem(TSharedPtr<FArmyKeyValue> InItem, int32 InType)
{
	return
        SNew(STextBlock)
        .Text(FText::FromString(InItem->Value))
        .ColorAndOpacity(FLinearColor::White)
        .TextStyle(FArmyStyle::Get(), "ArmyText_12");
}

void SArmyNewProject::HandleComboItemChanged(TSharedPtr<FArmyKeyValue> NewSelection, ESelectInfo::Type SelectInfo, int32 InType)
{
    if (!NewSelection.IsValid())
    {
        return;
    }

	switch (InType)
	{
	case 0:
		CurrentGroupValue = NewSelection;
		break;

	case 1:
	{
		if (CurrentProvince != NewSelection)
		{
			CurrentProvince = NewSelection;
			CurrentCity = DefalutCity;
			ETB_CityComboBox->RefreshOptions();
			RequestAreaData(NewSelection->Key, InType + 1);
		}
	}
		break;

	case 2:
	{
		if (CurrentCity != NewSelection)
		{
			CurrentCity = NewSelection;
			CurrentDistrict = DefaultDistrict;
			ETB_DistrictComboBox->RefreshOptions();
			RequestAreaData(NewSelection->Key, InType + 1);
		}
	}
		break;

	case 3:
		CurrentDistrict = NewSelection;
		break;

	default:
		break;
	}
}
FText SArmyNewProject::GetCurrentComboText(int32 InType) const
{
	TSharedPtr<FArmyKeyValue> ReturnValue = NULL;
	switch (InType)
	{
	case 0:
		ReturnValue = CurrentGroupValue;
		break;
	case 1:
		ReturnValue = CurrentProvince;
		break;
	case 2:
		ReturnValue = CurrentCity;
		break;
	case 3:
		ReturnValue = CurrentDistrict;
		break;
	default:
		break;
	}
	return ReturnValue.IsValid() ? FText::FromString(ReturnValue->Value) : FText();
}

TSharedRef<SWidget> SArmyNewProject::CreateProjectName()
{
    return
        SNew(SBox)
        .HeightOverride(32)
        [
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("*项目名称")))
                .TextStyle(FArmyStyle::Get(), "ArmyText_12")
            ]

            + SHorizontalBox::Slot()
            .Padding(20, 0, 0, 0)
            .FillWidth(1)
            [	
                SAssignNew(ETB_ProjectName, SArmyEditableTextBox)
                .HintText(FText::FromString(TEXT("请输入项目名称")))
                .MaxTextLength(20)
	        ]
        ];
}

void SArmyNewProject::RequestServerData()
{
	//套餐
    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &SArmyNewProject::CallBack_GroupList);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest("/api/list_set_meal/0", CompleteDelegate);
    Request->ProcessRequest();

	RequestAreaData(0,1);
}

void SArmyNewProject::CallBack_GroupList(FArmyHttpResponse Response)
{
    if (Response.bWasSuccessful)
    {
        const TArray<TSharedPtr<FJsonValue>> JArray = Response.Data->GetArrayField("data");

        GroupTypeList.Reset();
        for (auto& ArrayIt : JArray)
        {
            TSharedPtr<FJsonObject> JObject = ArrayIt->AsObject();
            int32 Id = JObject->GetIntegerField("id");
            FString Title = JObject->GetStringField("title");
            GroupTypeList.Add(MakeShareable(new FArmyKeyValue(Id, Title)));
        }
    }
}

void SArmyNewProject::RequestAreaData(int32 InParentID, int32 InDeep)
{
	//地区
    FString Url = "/common/areas/procity/" + FString::FromInt(InParentID);
    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &SArmyNewProject::CallBack_AreaList, InDeep);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(Url, CompleteDelegate);
    Request->ProcessRequest();
}

void SArmyNewProject::CallBack_AreaList(FArmyHttpResponse Response, int32 InPareenID)
{
    if (Response.bWasSuccessful)
    {
        const TArray<TSharedPtr<FJsonValue>> JArray = Response.Data->GetArrayField("data");

        TArray<TSharedPtr<FArmyKeyValue>>* AreaList = NULL;
        if (InPareenID == 1)
        {
            AreaList = &ProvinceList;
        }
        else if (InPareenID == 2)
        {
            AreaList = &CityList;
        }
        else if (InPareenID == 3)
        {
            AreaList = &DistrictList;
        }

        if (!AreaList)
        {
            return;
        }

        AreaList->Reset();
        for (auto& ArrayIt : JArray)
        {
            TSharedPtr<FJsonObject> JObject = ArrayIt->AsObject();

            int32 Id = JObject->GetIntegerField("id");
            FString Name = JObject->GetStringField("name");
            AreaList->Add(MakeShareable(new FArmyKeyValue(Id, Name)));
        }
    }
}

void SArmyNewProject::ReqVillageData()
{

}

TSharedRef<SWidget> SArmyNewProject::CreateOwnerName()
{
	return
        SNew(SHorizontalBox)

        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("业主姓名")))
            .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
        ]

        + SHorizontalBox::Slot()
        .Padding(20, 0, 0, 0)
        .FillWidth(1)
        [
            SNew(SBox)
            .HeightOverride(32)
            [
                SAssignNew(ETB_OwnerName, SArmyEditableTextBox)
                .HintText(FText::FromString(TEXT("请输入业主姓名")))
                .MaxTextLength(20)
            ]
	    ];
}

TSharedRef<SWidget> SArmyNewProject::CreateOwnerPhoneNumber()
{
	return

    SNew(SHorizontalBox)

    + SHorizontalBox::Slot()
    .AutoWidth()
    .VAlign(VAlign_Center)
    [
        SNew(STextBlock)
        .Text(FText::FromString(TEXT("联系电话")))
        .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
    ]

    + SHorizontalBox::Slot()
    .Padding(20, 0, 0, 0)
    .FillWidth(1)
    [
        SNew(SBox)
        .HeightOverride(32)
        [
            SAssignNew(ETB_OwnerPhoneNumber, SArmyEditableTextBox)
            .HintText(FText::FromString(TEXT("请输入业主联系电话")))
            .MaxTextLength(20)
        ]
	];
}

TSharedRef<SWidget> SArmyNewProject::CreateProvinceComboBox()
{
	return
		SAssignNew(ETB_ProvinceComboBox, SComboBox< TSharedPtr<FArmyKeyValue> >)
		.ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.PropertyCombobox"))
		.ComboBoxStyle(FArmyStyle::Get(), "ComboBox")
		.MaxListHeight(272)
		.HasDownArrow(true)
		.ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
		.OptionsSource(&ProvinceList)
		.OnGenerateWidget(this, &SArmyNewProject::GenerateComboItem,1)
		.OnSelectionChanged(this, &SArmyNewProject::HandleComboItemChanged,1)
		[
			SNew(STextBlock)
			.ColorAndOpacity(FLinearColor::White)
			.Text(this, &SArmyNewProject::GetCurrentComboText,1)
            .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		];
}

TSharedRef<SWidget> SArmyNewProject::CreateCityComboBox()
{
	return
	    SAssignNew(ETB_CityComboBox, SComboBox< TSharedPtr<FArmyKeyValue> >)
		.ItemStyle(FArmyStyle::Get(), "TableRow.PropertyCombobox")
		.ComboBoxStyle(FArmyStyle::Get(), "ComboBox")
		.HasDownArrow(true)
		.ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
		.OptionsSource(&CityList)
		.OnGenerateWidget(this, &SArmyNewProject::GenerateComboItem, 2)
		.OnSelectionChanged(this, &SArmyNewProject::HandleComboItemChanged, 2)
		[
			SNew(STextBlock)
			.ColorAndOpacity(FLinearColor::White)
			.Text(this, &SArmyNewProject::GetCurrentComboText, 2)
            .TextStyle(FArmyStyle::Get(), "ArmyText_12")
		];
}

TSharedRef<SWidget> SArmyNewProject::CreateDistrictComboBox()
{
	return
	    SAssignNew(ETB_DistrictComboBox, SComboBox< TSharedPtr<FArmyKeyValue> >)
		.ItemStyle(FArmyStyle::Get(), "TableRow.PropertyCombobox")
		.ComboBoxStyle(FArmyStyle::Get(), "ComboBox")
		.HasDownArrow(true)
		.ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
		.OptionsSource(&DistrictList)
		.OnGenerateWidget(this, &SArmyNewProject::GenerateComboItem, 3)
		.OnSelectionChanged(this, &SArmyNewProject::HandleComboItemChanged, 3)
		[
			SNew(STextBlock)
			.ColorAndOpacity(FLinearColor::White)
			.Text(this, &SArmyNewProject::GetCurrentComboText, 3)
            .TextStyle(FArmyStyle::Get(), "ArmyText_12")
		];
}

TSharedRef<SWidget> SArmyNewProject::CreateVillage()
{
    return
        SNew(SHorizontalBox)

        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("所在小区")))
            .TextStyle(FArmyStyle::Get(), "ArmyText_12")
        ]

        + SHorizontalBox::Slot()
        .Padding(20, 0, 0, 0)
        .FillWidth(1)
        [
            SNew(SBox)
            .HeightOverride(32)
            [
                SAssignNew(ETB_Village, SArmyEditableTextBox)
                .HintText(FText::FromString(TEXT("请输入小区")))
                .MaxTextLength(30)
            ]
	    ];
}

TSharedRef<SWidget> SArmyNewProject::CreateDetailAddress()
{
    return
	    SNew(SHorizontalBox)

	    + SHorizontalBox::Slot()
	    .AutoWidth()
	    .VAlign(VAlign_Center)
	    [
		    SNew(STextBlock)
		    .Text(FText::FromString(TEXT("详细地址")))
		    .TextStyle(FArmyStyle::Get(), "ArmyText_12")
	    ]

	    + SHorizontalBox::Slot()
	    .Padding(20, 0, 0, 0)
	    .FillWidth(1)
	    [
		    SNew(SBox)
		    .HeightOverride(32)
		    [
			    SAssignNew(ETB_DetailAddress, SArmyEditableTextBox)
			    .HintText(FText::FromString(TEXT("请输入详细地址")))
                .MaxTextLength(30)
		    ]
	    ];
}