#include "SArmyNewPlan.h"
#include "ArmyGameInstance.h"
#include "SBox.h"
#include "SBoxPanel.h"
#include "STextBlock.h"
#include "ArmyStyle.h"
#include "SlateTypes.h"
#include "TextLayout.h"
#include "SEditableTextBox.h"
#include "SArmyEditableNumberBox.h"
#include "ArmyHttpModule.h"
#include "ArmyCommonTools.h"
#include "Json.h"
#include "ArmyModalManager.h"
#include "ArmyUser.h"
#include "SArmyEditableTextBox.h"
#include "ArmyResourceModule.h"

void SArmyNewPlan::Construct(const FArguments &InArgs)
{
	ChildSlot
	[
		SNew(SBox)
		.WidthOverride(480)
		[
			SNew(SVerticalBox)

			+SVerticalBox::Slot()
			.Padding(0, 20, 20, 0)			
			.AutoHeight()
			[
				CreatePlanWidget()
			]

			+ SVerticalBox::Slot()
			.Padding(0, 20, 20, 0)
			.AutoHeight()
			[
				CreateHouseTypeWidget()
			]

            + SVerticalBox::Slot()
            .Padding(0, 20, 20, 0)
            .AutoHeight()
            [
                CreateAreaWidget()
            ]

			// 错误信息
			+ SVerticalBox::Slot()
			.Padding(116, 5, 0, 0)
			.AutoHeight()
			[
				SAssignNew(TB_ErrorMessage, STextBlock)
				.Text(FText::FromString(TEXT("方案名称不能为空")))
				.TextStyle(FArmyStyle::Get(), "ArmyText_12")
				.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFFF5A5A"))
				.Visibility(EVisibility::Collapsed)
			]
		]
	];	
}


bool SArmyNewPlan::CheckValid()
{	
	if (ETB_Name->GetText().ToString().TrimStart() == "") {
		TB_ErrorMessage->SetVisibility(EVisibility::Visible);
		return false;
	}
	TB_ErrorMessage->SetVisibility(EVisibility::Collapsed);
	return true;
}

void SArmyNewPlan::OnConfirmClicked()
{
	if (CheckValid()) {
		//@梁晓菲 从菜单项新建方案的时候，需要先把方案内的唯一标识清空，重新赋值
		FArmyUser::Get().RoomUniqueId = -1;
        ReqNewPlan();
	}
}

TSharedRef<SWidget> SArmyNewPlan::CreateHouseTypeWidget()
{
	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(0, 0, 20, 0)
		.AutoWidth()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[
			SNew(SBox)
			.WidthOverride(96)	
			.HAlign(HAlign_Right)
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
					.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
					.IsIntegerOnly(true)
					.Text(FText::FromString(TEXT("0")))
                    .MaxTextLength(1)
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
					.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
					.IsIntegerOnly(true)
					.Text(FText::FromString(TEXT("0")))
                    .MaxTextLength(1)
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
					.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
					.IsIntegerOnly(true)
					.Text(FText::FromString(TEXT("0")))
                    .MaxTextLength(1)
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
					.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
					.IsIntegerOnly(true)
					.Text(FText::FromString(TEXT("0")))
                    .MaxTextLength(1)
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

TSharedRef<SWidget> SArmyNewPlan::CreateAreaWidget()
{
    return
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.Padding(0, 0, 20, 0)
		.AutoWidth()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[
			SNew(SBox)
			.WidthOverride(96)
			.HAlign(HAlign_Right)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("建筑面积")))
				.TextStyle(FArmyStyle::Get(), "ArmyText_12")
				.Justification(ETextJustify::Center)
			]
		]

		+ SHorizontalBox::Slot()
		.Padding(0, 0, 8, 0)
		.FillWidth(1)
		.HAlign(HAlign_Fill)
		[
			SNew(SBox)
			.HeightOverride(32)
			[
				SAssignNew(ETB_Area, SArmyEditableNumberBox)
				.Padding(FMargin(8, 0, 0, 0))
				.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
				.IsFloatOnly(true)
				.HintText(MAKE_TEXT("请输入建筑面积"))
				.Text(FText::FromString("0"))
                .MaxTextLength(6)
			]			
		]
        
        + SHorizontalBox::Slot()				
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
            SNew(STextBlock)
            .TextStyle(FArmyStyle::Get(), "ArmyText_12")
            .Text(FText::FromString(TEXT("㎡")))
		];
}

TSharedRef<SWidget> SArmyNewPlan::CreatePlanWidget()
{
	return
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.Padding(0, 0, 20, 0)
		.AutoWidth()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[
			SNew(SBox)
			.WidthOverride(96)
			.HAlign(HAlign_Right)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("*方案名称")))
				.TextStyle(FArmyStyle::Get(), "ArmyText_12")
				.Justification(ETextJustify::Center)
			]
		]

		+ SHorizontalBox::Slot()
		.FillWidth(1)
		.HAlign(EHorizontalAlignment::HAlign_Fill)		
		[
			SNew(SBox)
			.HeightOverride(32)
			[
				SAssignNew(ETB_Name, SArmyEditableTextBox)
				.HintText(FText::FromString(TEXT("请输入方案名称")))
                .MaxTextLength(20)
			]			
		];
}

void SArmyNewPlan::ReqNewPlan()
{
    if (FArmyUser::Get().CurProjectData.IsValid() && FArmyUser::Get().CurProjectData->ResObjArr.Num() > 0)
    {
        FString JStr;
        TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JStr);
        JsonWriter->WriteObjectStart();

        JsonWriter->WriteValue("projectId", FArmyUser::Get().CurProjectData->ID);
        JsonWriter->WriteValue("name", *ETB_Name->GetText().ToString().TrimStartAndEnd());
        JsonWriter->WriteValue("bedroom", FCString::Atoi(*ETB_Bedroom->GetText().ToString().TrimStartAndEnd()));
        JsonWriter->WriteValue("livingRoom", FCString::Atoi(*ETB_LivingRoom->GetText().ToString().TrimStartAndEnd()));
        JsonWriter->WriteValue("kitchen", FCString::Atoi(*ETB_Kitchen->GetText().ToString().TrimStartAndEnd()));
        JsonWriter->WriteValue("bathroom", FCString::Atoi(*ETB_Bathroom->GetText().ToString().TrimStartAndEnd()));
        JsonWriter->WriteValue("floorArea", FCString::Atof(*ETB_Area->GetText().ToString().TrimStartAndEnd()));
        JsonWriter->WriteObjectEnd();
        JsonWriter->Close();

        FArmyHttpRequestCompleteDelegate CompleteDelegate;
        CompleteDelegate.BindRaw(this, &SArmyNewPlan::ResNewPlan);
        IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest("/api/plans", CompleteDelegate, JStr);
        Request->ProcessRequest();

        GGI->Window->ShowThrobber(MAKE_TEXT("创建方案..."));
    }
}

void SArmyNewPlan::ResNewPlan(FArmyHttpResponse Response)
{
    GGI->Window->HideThrobber();

    if (Response.bWasSuccessful)
    {
        // 记录当前新建方案的数据
        const TSharedPtr<FJsonObject> JData = Response.Data->GetObjectField("data");
        FContentItemPtr PlanData = FArmyCommonTools::ParseContentItemFromJson(JData);
        FArmyUser::Get().CurPlanData = PlanData;

        // 加载默认资源
        GGI->DesignEditor->LoadCommonResource();

        // 数据下载完成，将解析的数据存储起来
        FArmyResourceModule::Get().GetResourceManager()->AppendSynList(GGI->DesignEditor->SynContentItems);

        // 清空场景
        GGI->DesignEditor->ClearAll();
        GGI->DesignEditor->Reload();

        // @欧石楠 如果没有初始化DesignEditor，则需要初始化并present
        if (!GGI->DesignEditor->Inited())
        {
            GGI->DesignEditor->Init();
            GGI->Window->PresentViewController(GGI->DesignEditor);
        }

        GGI->Window->DismissModalDialog();
        GGI->Window->ShowMessage(MT_Success, TEXT("创建方案成功"));
    }
    else
    {
        if (Response.Message.IsEmpty())
        {
            GGI->Window->ShowMessage(MT_Warning, TEXT("创建方案失败"));
        }
        else
        {
            GGI->Window->ShowMessage(MT_Warning, Response.Message);
        }
    }
}

