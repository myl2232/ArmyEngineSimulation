#include "SArmySaveAsPlan.h"
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

void SArmySaveAsPlan::Construct(const FArguments & InArgs)
{
	CurrentProjectName = FArmyUser::Get().CurProjectData->Name;
	SelectedProjectID = FArmyUser::Get().GetCurProjectID();

	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(20, 20, 20, 0)
		.VAlign(EVerticalAlignment::VAlign_Top)
		[
			SNew(SBox)
			.WidthOverride(136)
			.HeightOverride(114)
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
				//选择项目
				CreateProjectWidget()
			]

			+ SVerticalBox::Slot()
			.Padding(0, 20, 20, 0)
			.AutoHeight()
			[
				//方案名称
				CreatePlanWidget()
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
			.VAlign(VAlign_Bottom)
			.AutoHeight()
			[
				//选择房型
				CreateHouseTypeWidget()
			]
		]
	];

    Init();
}

TSharedRef<SWidget> SArmySaveAsPlan::CreateProjectWidget()
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
				.Text(FText::FromString(TEXT("选择项目")))
				.TextStyle(FArmyStyle::Get(), "ArmyText_12")
				.Justification(ETextJustify::Center)
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
				SAssignNew(ProjectComboBox, SComboBox< TSharedPtr<FArmyKeyValue> >)
				.ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.None"))
				.ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.PropertyCombobox"))
				.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("ComboBox.Button.FF212224"))
				.OptionsSource(&ProjectList.Array)
				.MaxListHeight(200.f)
				.OnGenerateWidget(this, &SArmySaveAsPlan::OnGenerateComoboWidget)
				.OnSelectionChanged(this, &SArmySaveAsPlan::OnSlectedProjectChanged)
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
							.Text(this, &SArmySaveAsPlan::GetCurrentProject)
						]
	
						+SHorizontalBox::Slot()
						.HAlign(EHorizontalAlignment::HAlign_Right)
						.Padding(FMargin(0,0,0,0))
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

void SArmySaveAsPlan::Init()
{
	ReqGetProjectList();
}

void SArmySaveAsPlan::OnSlectedProjectChanged(TSharedPtr<FArmyKeyValue> NewSelection, ESelectInfo::Type SelectInfo)
{
	if (NewSelection.IsValid()) {

		/**@梁晓菲 遍历ComboboxList，将非选中项设为正常*/
		for (int i = 0; i < ComboboxUIList.Num(); i++)
		{
			if (ComboboxUIList[i]->GetText() != NewSelection->Value)
			{
				ComboboxUIList[i]->SetTextColorAndImageBrushNormal();
			}
		}

		CurrentProjectName = NewSelection->Value;
		SelectedProjectID = NewSelection->Key;
	}
}


void SArmySaveAsPlan::ReqGetProjectList()
{
    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &SArmySaveAsPlan::ResGetProjectList);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest("/api/no-paging/projects", CompleteDelegate);
    Request->ProcessRequest();
}

void SArmySaveAsPlan::ResGetProjectList(FArmyHttpResponse Response)
{
    if (Response.bWasSuccessful)
    {
        const TArray<TSharedPtr<FJsonValue>> JArray = Response.Data->GetArrayField("data");

        ProjectList.Reset();
        for (auto& ArrayIt : JArray)
        {
            TSharedPtr<FJsonObject> JObject = ArrayIt->AsObject();
            int32 Id = JObject->GetIntegerField("id");
            FString Name = JObject->GetStringField("name");
            ProjectList.Add(MakeShareable(new FArmyKeyValue(Id, Name)));
        }
    }

    SelectedProjectID = FArmyUser::Get().GetCurProjectID();

    // 设置当前项目选中项
    TSharedPtr<FArmyKeyValue> SelectedProject = ProjectList.FindByKey(SelectedProjectID);
    if (SelectedProject.IsValid())
    {
        ProjectComboBox->SetSelectedItem(SelectedProject);
    }
}

void SArmySaveAsPlan::ReqSavePlan()
{
    FString JStr;
    TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JStr);

    // 开始写入方案
    JsonWriter->WriteObjectStart();

    JsonWriter->WriteValue("projectId", SelectedProjectID);
    JsonWriter->WriteValue("name", ETB_PlanName->GetText().ToString());
    JsonWriter->WriteValue("fileVersion", GResourceVersion);
    JsonWriter->WriteValue("fileUrl", *PlanFileUrl);
    JsonWriter->WriteValue("fileMd5", *PlanFileMD5);
    JsonWriter->WriteValue("thumbnail", *PlanThumbnailUrl);

    // 开始写入户型
    JsonWriter->WriteObjectStart("home");

    JsonWriter->WriteValue("fileVersion", GResourceVersion);
    JsonWriter->WriteValue("fileUrl", *HomeFileUrl);
    JsonWriter->WriteValue("fileMd5", *HomeFileMD5);
    JsonWriter->WriteValue("bedroom", FCString::Atoi(*ETB_Bedroom->GetText().ToString()));
    JsonWriter->WriteValue("livingRoom", FCString::Atoi(*ETB_LivingRoom->GetText().ToString()));
    JsonWriter->WriteValue("kitchen", FCString::Atoi(*ETB_Kitchen->GetText().ToString()));
    JsonWriter->WriteValue("bathroom", FCString::Atoi(*ETB_Bathroom->GetText().ToString()));

    // 结束写入户型
    JsonWriter->WriteObjectEnd();

	/*@梁晓菲 写入房间在方案内的唯一标志*/
	JsonWriter->WriteArrayStart(TEXT("vrSpaceList"));

	TArray<FObjectWeakPtr> RoomList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
	for (FObjectWeakPtr It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("vrSpaceId", Room->GetUniqueIdOfRoom());
		JsonWriter->WriteValue("spaceId", Room->GetSpaceId());
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

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

    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &SArmySaveAsPlan::ResSavePlan);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest(TEXT("/api/new-plans"), CompleteDelegate, JStr);
    Request->ProcessRequest();
}

void SArmySaveAsPlan::ResSavePlan(FArmyHttpResponse Response)
{
    GGI->Window->HideThrobber();

    if (Response.bWasSuccessful)
    {
        // 切换当前项目
        FArmyUser::Get().CurProjectData->ID = SelectedProjectID;

        const TSharedPtr<FJsonObject>* JData = nullptr;
        if (Response.Data->TryGetObjectField("data", JData))
        {
            FArmyUser::Get().CurPlanData = FArmyCommonTools::ParseContentItemFromJson(*JData);

            const TSharedPtr<FJsonObject>* HomeObj = nullptr;
            if ((*JData)->TryGetObjectField("home", HomeObj))
            {
                FArmyUser::Get().CurHomeData = FArmyCommonTools::ParseContentItemFromJson(*HomeObj);
            }
        }

        GGI->Window->DismissModalDialog();
        GGI->Window->ShowMessage(MT_Success, TEXT("方案保存成功"));
    }
    else
    {
        if (Response.Message.IsEmpty())
        {
            GGI->Window->ShowMessage(MT_Warning, TEXT("方案保存失败"));
        }
        else
        {
            GGI->Window->ShowMessage(MT_Warning, Response.Message);
        }
    }
}

TSharedRef<SWidget> SArmySaveAsPlan::OnGenerateComoboWidget(TSharedPtr<FArmyKeyValue> InItem)
{
	/**@梁晓菲 存储ComboboxItem*/
	TSharedPtr<SArmyComboboxItem> ComboboxItem;

	SAssignNew(ComboboxItem, SArmyComboboxItem)
		.TextNormalColor(FArmyStyle::Get().GetColor("Color.Gray.FFC8C9CC"))
		.TextSelectedColor(FArmyStyle::Get().GetColor("Color.FFFF9800"))
		.Text(FText::FromString(InItem->Value))
		.Height(32);

	ComboboxUIList.Add(ComboboxItem);

	return ComboboxItem.ToSharedRef();
}
