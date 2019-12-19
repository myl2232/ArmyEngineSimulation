#include "SArmyPanorama.h"
#include "SlateBasics.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Engine.h"
#include "ArmyStyle.h"
#include "WindowsPlatformApplicationMisc.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyEditor.h"
#include "ArmyPanoramaMgr.h"
#include "ArmySlateModule.h"
#include "WebImageCache.h"
#include "ArmyHttpModule.h"
#include "ArmyGameInstance.h"
#include "SArmyEditableNumberBox.h"
#include "ArmyPlayerController.h"
#include "SArmyTextBlock.h"

//正则匹配判断
bool SArmyPanorama::CheckStringIsVaild(const FString& str, const FString& Reg)
{
	FRegexPattern Pattern(Reg);
	FRegexMatcher regMatcher(Pattern, str);
	regMatcher.SetLimits(0, str.Len());
	return regMatcher.FindNext();
}

//检查电话号码是否有效
bool SArmyPanorama::CheckPhone(const FString& str)
{
	if (str.Len() != 11)
		return false;
	FString reg = TEXT("^1\\d{10}$");	//	[^(.|\n) {0,120}$]
	return SArmyPanorama::CheckStringIsVaild(str, reg);
}

void SArmyPanorama::GetValidTime(const FText& InText)
{
	FString MyValidTime = InText.ToString();
	FString reg = TEXT("^(?!0)(?:[0-9]{1,4}|10000)$");
	if (InText.EqualTo(FText::FromString("")) || SArmyPanorama::CheckStringIsVaild(MyValidTime, reg))
	{
		ValidTime->SetText(InText);
	}
	else
	{
		ValidTime->SetText(FText::FromString(InText.ToString().LeftChop(1)));
	}
}

void SArmyPanorama::GetMyText(const FText& InText)
{
	Introduction = InText.ToString();

	LengthOfIndroduction = InText.ToString().Len();
	if (LengthOfIndroduction >= 120)
	{
		Introduction = InText.ToString().LeftChop(LengthOfIndroduction - 120);
		WarningForIntroductionNum->SetVisibility(EVisibility::Visible);
	}
	else
		WarningForIntroductionNum->SetVisibility(EVisibility::Hidden);

	METB_Introduction->SetText(FText::FromString(Introduction));
}

void SArmyPanorama::GetMyPhoneNum(const FText& InText)
{
	MyInformWhenSendClicked.phone = InText.ToString();
}

TSharedRef<SWidget> SArmyPanorama::GeneratePlatformComboItem(TSharedPtr<FString> InItem)
{
	return
		SNew(SBox)
		.WidthOverride(80)
		.HeightOverride(32)
		.Padding(FMargin(18, 5, 5, 5))
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString(*InItem))
			.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		];
}

//获取“全景图视角大小”下拉框选中的Item
void SArmyPanorama::HandlePlatformComboChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo)
{
	for (int32 i = 0; i < Array_View.Num(); i++)
	{
		if (Item == Array_View[i])
		{
			CurrentPlatformString = *Item.Get();
		}
	}

	//视角大小
	if (CurrentPlatformString == FString(L"标准"))
	{
		MyConfigOfPanorama.EPanoramaAngle = "105";
	}
	else if (CurrentPlatformString == FString(L"远景"))
	{
		MyConfigOfPanorama.EPanoramaAngle = "120";
	}
	else
	{
		MyConfigOfPanorama.EPanoramaAngle = "90";
	}
}

//处理“二维码有效时长”下拉框选中的Item
void SArmyPanorama::HandlePlatformComboChanged_Time(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo)
{
	for (int32 i = 0; i < Array_Resolutions.Num(); i++)
	{
		if (Item == Array_Resolutions[i])
		{
			NumberOfTimeItem = i;
			CurrentPlatformString_Time = *Item.Get();
		}
	}

	//有效时间
	switch (NumberOfTimeItem)
	{
	case 0:
		MyConfigOfPanorama.EValidTime = "-1";
		ValidTime->SetVisibility(EVisibility::Hidden);
		ValidTime_Minute->SetVisibility(EVisibility::Hidden);
		break;

	case 1:
		MyConfigOfPanorama.EValidTime = "15";
		ValidTime->SetVisibility(EVisibility::Hidden);
		ValidTime_Minute->SetVisibility(EVisibility::Hidden);
		break;

	case 2:
		MyConfigOfPanorama.EValidTime = "30";
		ValidTime->SetVisibility(EVisibility::Hidden);
		ValidTime_Minute->SetVisibility(EVisibility::Hidden);
		break;

	case 3:
		MyConfigOfPanorama.EValidTime = "60";
		ValidTime->SetVisibility(EVisibility::Hidden);
		ValidTime_Minute->SetVisibility(EVisibility::Hidden);
		break;

	case 4:
		MyConfigOfPanorama.EValidTime = "1440";
		ValidTime->SetVisibility(EVisibility::Hidden);
		ValidTime_Minute->SetVisibility(EVisibility::Hidden);
		break;

	case 5:
		MyConfigOfPanorama.EValidTime = (MAKE_TEXT("自定义")).ToString();
		ValidTime->SetVisibility(EVisibility::Visible);
		ValidTime_Minute->SetVisibility(EVisibility::Visible);
		break;

	default:
		break;
	}
}

//获取选中的Item的字，“全景图视角大小”
FText SArmyPanorama::GetPlatformComboText() const
{
	return FText::FromString(CurrentPlatformString);
}

//获取选中的Item的字，“二维码有效时长”
FText SArmyPanorama::GetPlatformComboText_Time() const
{
	return FText::FromString(CurrentPlatformString_Time);
}

FReply SArmyPanorama::NextStepClicked_Option()
{
	//如果不是透视图模式，不能生成全景图
	if (GXRPC->GetXRViewMode() != EXRView_FPS)
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("请在透视图模式下生成全景图"));
		return FReply::Handled();
	}

	if (MyConfigOfPanorama.EValidTime == (MAKE_TEXT("自定义")).ToString())
	{
		if (ValidTime->GetText().ToString() == "")
		{
			MyConfigOfPanorama.EValidTime = "-1";
		}
		else
			MyConfigOfPanorama.EValidTime = ValidTime->GetText().ToString();
	}
	MyConfigOfPanorama.Introduction = METB_Introduction->GetText().ToString();
		
	if (BIsSingle)
	{
		//单屋全景图
		FArmyPanoramaMgr::Get().GenerateSingleAndUpload(GVC, MyConfigOfPanorama);
	}
	else
	{
		//全屋全景图
		FArmyPanoramaMgr::Get().GenerateMultipleAndUpload(MyConfigOfPanorama);
	}
	
	return FReply::Handled();
}

void SArmyPanorama::ResetWidget()
{
	MyWidgetSwitcher->SetActiveWidgetIndex(1);
}

void SArmyPanorama::SetShortUrlAndQrCodeUrl(FString URL, FString QrCodeURL, int32 panoId)
{
	//跳转到下一页
	MyWidgetSwitcher->SetActiveWidgetIndex(1);
	CurrentVerificationID = FGuid::NewGuid();

	FString Url = TEXT("/api/pano/codeImage/") + CurrentVerificationID.ToString();
	const FString FullUrl = FString::Printf(TEXT("%s%s"), *FArmyHttpModule::Get().GetServerAddress(), *Url);

	GenericWhiteBox_IdentifyMap->SetContent(
		SNew(SImage)
		.Image(FArmySlateModule::Get().WebImageCache->Download(FullUrl).Get().Attr())
	);

	BuildingMessage->SetText(FText::FromString(TEXT("全景图生成成功，查看请扫描二维码")));
	PanoramaUrl = FText::FromString(URL);
	SendButton->SetEnabled(true);
	ImageContainer->SetContent(
		SNew(SImage)
		.Image(FArmySlateModule::Get().WebImageCache->Download(QrCodeURL).Get().Attr())
	);
	MySCircularThrobberSwitcher->SetActiveWidgetIndex(1);


	MyInformWhenSendClicked.panoId = panoId;
	MyInformWhenSendClicked.uuid = CurrentVerificationID.ToString();
}

void SArmyPanorama::RequestSendSubmit(InformationWhenSendClicked SendClickInformation)
{
	MyInformWhenSendClicked.code = EditableTextBox_Identify->GetText().ToString();
	FString JStr;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JStr);
	
	JsonWriter->WriteObjectStart();

	JsonWriter->WriteValue("phone", MyInformWhenSendClicked.phone);
	JsonWriter->WriteValue("code", MyInformWhenSendClicked.code);
	JsonWriter->WriteValue("uuid", MyInformWhenSendClicked.uuid);
	JsonWriter->WriteValue("panoId", MyInformWhenSendClicked.panoId);

	JsonWriter->WriteObjectEnd();
	JsonWriter->Close();

    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &SArmyPanorama::Callback_RequestSendSubmit);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest("/api/pano/sendSMS", CompleteDelegate, JStr);
    Request->ProcessRequest();
}

void SArmyPanorama::Callback_RequestSendSubmit(FArmyHttpResponse Response)
{
    if (Response.bWasSuccessful)
    {
        //成功，销毁窗口，失败保留窗口
        GGI->Window->DismissModalDialog();
        GGI->Window->ShowMessage(MT_Success, TEXT("短信成功发送"));
    }
    else
    {
        ErrorIdentifyingCode->SetVisibility(EVisibility::Visible);
        GGI->Window->ShowMessage(MT_Warning, TEXT("短信发送失败"));
    }
}

FReply SArmyPanorama::SendClicked_Build()
{
	if (!CheckPhone(MyInformWhenSendClicked.phone))
	{
		ErrorPhoneNum->SetVisibility(EVisibility::Visible);
		return FReply::Handled();
	}
	else
	{
		ErrorPhoneNum->SetVisibility(EVisibility::Collapsed);
		MyDeleForSendButton.ExecuteIfBound(MyInformWhenSendClicked);
		return FReply::Handled();
	}
}

FReply SArmyPanorama::CancelClicked()
{
	GGI->Window->DismissModalDialog();
	return FReply::Handled();
}

void SArmyPanorama::CopyButtonClicked()
{
	FPlatformApplicationMisc::ClipboardCopy(*PanoramaUrl.ToString());
	GGI->Window->ShowMessage(MT_Success, TEXT("成功复制到剪贴板！"));
}

void  SArmyPanorama::URLButtonClicked()
{
	if (!PanoramaUrl.IsEmpty())
	{
		FPlatformMisc::OsExecute(TEXT("open"), *PanoramaUrl.ToString());
	}
}

TSharedRef<SWidget> SArmyPanorama::CreatePage_Option()
{
	//设置image，创建Brush, 各种画刷
	FSlateBrush* GenericWhiteBox = new FSlateImageBrush(FName(*(FPaths::EngineContentDir() / TEXT("Slate/Old/White.png"))), FVector2D(1, 1), FLinearColor(FColor(0XFF0B0B0C)));	//分割线
	FSlateBrush* GenericWhiteBox_Watting = new FSlateImageBrush(FName(*(FPaths::EngineContentDir() / TEXT("Slate/Old/White.png"))), FVector2D(48, 48), FLinearColor::Blue);
	FSlateBrush* ComboButtonNormal = new FSlateImageBrush(FName(*(FPaths::EngineContentDir() / TEXT("Slate/Old/White.png"))), FVector2D(1, 1), FLinearColor(FColor(0XFF37383C)));
	FSlateBrush* MenuBorder = new FSlateImageBrush(FName(*(FPaths::EngineContentDir() / TEXT("Slate/Old/White.png"))), FVector2D(1, 1), FLinearColor(FColor(0XFF37383C)));
	FSlateBrush* ActiveBrushColor = new FSlateImageBrush(FName(*(FPaths::EngineContentDir() / TEXT("Slate/Old/White.png"))), FVector2D(1, 1), FLinearColor(FColor(0XFF28292B)));

	UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, TEXT("/Game/Icons/ArrowDown.ArrowDown"), NULL, LOAD_None, NULL));
	FSlateBrush* DownArrow = new FSlateBrush();
	DownArrow->ImageSize = FVector2D(12, 12);
	DownArrow->SetResourceObject(Texture);

	//层层嵌套，创建样式, 样式的层层嵌套，和UMG一样
	//创建Combobox的样式
	MyButtonStyle = FButtonStyle()
		.SetNormal(*ComboButtonNormal)
		.SetHovered(*ComboButtonNormal)
		.SetPressed(*ComboButtonNormal); //Button Style 的 Normal、Hovered、Pressed

	MyComboButtonStyle = FComboButtonStyle()
		.SetButtonStyle(MyButtonStyle)
		.SetDownArrowImage(*DownArrow)
		.SetMenuBorderBrush(*MenuBorder);

	MyComboBoxStyle = FComboBoxStyle()
		.SetComboButtonStyle(MyComboButtonStyle);

	//创建Item的样式
	MyItemStyle = FTableRowStyle()
		.SetSelectorFocusedBrush(*ComboButtonNormal)				//确保默认项的正常显示
		.SetActiveBrush(*ActiveBrushColor)							//被选中项的背景色
		.SetActiveHoveredBrush(*ActiveBrushColor)					//被选中项鼠标滑过时的背景色
		.SetSelectedTextColor(FLinearColor(FColor(0XFF9D9FA5)))		//下拉框被选中的字的颜色
		.SetTextColor(FLinearColor(FColor(0XFF9D9FA5)))				//下拉框未被选中的字的颜色
		.SetEvenRowBackgroundHoveredBrush(*ActiveBrushColor)		//偶数行鼠标划过的背景
		.SetOddRowBackgroundHoveredBrush(*ActiveBrushColor)			//奇数行鼠标划过的背景
		.SetEvenRowBackgroundBrush(*MenuBorder)						//偶数行背景
		.SetOddRowBackgroundBrush(*MenuBorder);						//奇数行背景

	//全景图视角大小选项数组
	Array_View = { MakeShared<FString>(TEXT("标准"))};
	Array_View.Add(MakeShared<FString>(TEXT("近景")));
	Array_View.Add(MakeShared<FString>(TEXT("远景")));


	//二维码有效时长选项数组
	Array_Resolutions = { MakeShared<FString>(TEXT("永久")), MakeShared<FString>(TEXT("15分钟")), MakeShared<FString>(TEXT("30分钟")), MakeShared<FString>(TEXT("1小时")), MakeShared<FString>(TEXT("24小时")) };
	Array_Resolutions.Add(MakeShared<FString>(TEXT("自定义")));
		
	//相当于初始化，给出Button的默认内容，填充Button
	CurrentPlatformString = *Array_View[0].Get();
	CurrentPlatformString_Time = *Array_Resolutions[0].Get();

	return 
		//SizeBox控制界面大小
		SNew(SBox)
		.WidthOverride(420)
		[
			//Border控制背景色
			SNew(SBorder)
			.Padding(FMargin(20,0,20,0))
			.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
			.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF28292B"))
			[
				//内容部分分为“全景图视角大小”，“二维码有效时长”，“全景图简介”，按钮
				SNew(SVerticalBox)

				//“全景图视角大小”
				+ SVerticalBox::Slot()
				.Padding(0, 20, 0, 0)
				.AutoHeight()
				[
					SNew(SBox)
					.HeightOverride(30)
					[
						//左右结构
						SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							//“全景图视角大小:”汉字
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("全景图视角大小")))
							.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.Gray.FFC8C9CC"))
							.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
						]

						+ SHorizontalBox::Slot()
						.Padding(16, 0, 0, 0)
						.FillWidth(1)
						[
							// 全景图视角大小下拉列表
							SAssignNew(CB_PanoramaVisualAngle, SComboBox<TSharedPtr<FString>>)
							.ContentPadding(FMargin(11, 5, 7, 8))
							.OptionsSource(&Array_View)
							.ForegroundColor(FCoreStyle::Get().GetColor("Color.FFFFFF"))		//加上这一行（无论设置的color是多少），下拉箭头就能是白色
							.OnGenerateWidget(this, &SArmyPanorama::GeneratePlatformComboItem)		//初次生成下拉框时调用OnGenerateWidget
							.OnSelectionChanged(this, &SArmyPanorama::HandlePlatformComboChanged)	//每次更改选中的Item的时候调用
							//.ComboBoxStyle(&MyComboBoxStyle)
							.ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.Black"))
							.ItemStyle(&MyItemStyle)
							[
								SNew(STextBlock)
								.Text(this, &SArmyPanorama::GetPlatformComboText)		//把字显示到Combbox的Button上
								.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
							]
						]
					]
				]

				//“二维码有效时长”
				+ SVerticalBox::Slot()
				.Padding(0, 16, 0, 0)
				.AutoHeight()
				[
					SNew(SBox)
					.HeightOverride(30)
					[
						//左右结构
						SNew(SHorizontalBox)
							
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						[
							//“二维码有效时长:”汉字
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("二维码有效时长")))
							.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.Gray.FFC8C9CC"))
							.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
						]

						+ SHorizontalBox::Slot()
						.Padding(16, 0, 0, 0)
						.AutoWidth()
						[
							SNew(SBox)
							.WidthOverride(80)
							[
								// 二维码有效时长下拉列表
								SAssignNew(CB_QRCodeValidTime, SComboBox<TSharedPtr<FString>>)
								.ContentPadding(FMargin(11, 5, 7, 8))
								.ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.Black"))
								.ItemStyle(&MyItemStyle)
								.ForegroundColor(FCoreStyle::Get().GetColor("Color.000000"))
								.OnGenerateWidget(this, &SArmyPanorama::GeneratePlatformComboItem)
								.OnSelectionChanged(this, &SArmyPanorama::HandlePlatformComboChanged_Time)
								.OptionsSource(&Array_Resolutions)
								.InitiallySelectedItem(Array_Resolutions[0])
								[
									SNew(STextBlock)
									.Text(this, &SArmyPanorama::GetPlatformComboText_Time)
									.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
									.ColorAndOpacity(FLinearColor::White)
								]
							]
						]

						+ SHorizontalBox::Slot()
						.Padding(FMargin(16,0,0,0))
						.FillWidth(1)
						[
							SAssignNew(ValidTime, SArmyEditableNumberBox)
							.Visibility(EVisibility::Hidden)
							.HintText(FText::FromString(TEXT("最大10000")))
							.Style(FCoreStyle::Get(), "Box")
							.OnTextChanged(this, &SArmyPanorama::GetValidTime)
							.ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
							.BackgroundColor(FLinearColor(FColor(0XFF212226)))
						]
						
						+ SHorizontalBox::Slot()
						.Padding(FMargin(16,0,0,0))
						.AutoWidth()
						.HAlign(EHorizontalAlignment::HAlign_Right)
						.VAlign(EVerticalAlignment::VAlign_Center)
						[
							SAssignNew(ValidTime_Minute, STextBlock)
							.Text(FText::FromString(TEXT("分钟")))
							.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
							.ColorAndOpacity(FLinearColor(FColor(0XFFC8C9CC)))
							.Visibility(EVisibility::Hidden)
						]
					]
				]

				//“全景图简介”
				+ SVerticalBox::Slot()
				.Padding(25, 16, 0, 0)
				.AutoHeight()
				[
					SNew(SBox)
					.HeightOverride(112)
					[
						SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
						.AutoWidth()
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Top)
						[
							//“全景图简介”汉字
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("全景图简介")))
							.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.Gray.FFC8C9CC"))
							.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
						]

						+ SHorizontalBox::Slot()
						.Padding(16, 0, 0, 0)
						.FillWidth(1)
						[
							//“全景图简介”输入框
							SAssignNew(METB_Introduction, SMultiLineEditableTextBox)
							.BackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF212224"))
							.ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
							.Padding(FMargin(16, 8, 8, 0))
							.Style(FCoreStyle::Get(), "Box")
							.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
							.HintText(FText::FromString(TEXT("请输入全景图的文字介绍（不超过120字）")))
							.OnTextChanged(this,&SArmyPanorama::GetMyText)
							.AutoWrapText(true)
							.WrapTextAt(0.0)
							.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
						]
					]
				]

				//超字数提示框，初始隐藏
				+SVerticalBox::Slot()
				.Padding(FMargin(102,5,0,0))
				[
					SAssignNew(WarningForIntroductionNum , STextBlock)
					.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
					.ColorAndOpacity(FLinearColor(FColor(0XFFFF5A5A)))
					.Text(FText::FromString(TEXT("输入不超过120字")))
					.Visibility(EVisibility::Hidden)
				]

				//创建按钮
				+ SVerticalBox::Slot()
				.Padding(0, 0, 0, 20)
				.AutoHeight()
				.HAlign(HAlign_Right)
				[

					SNew(SBox)
					.WidthOverride(180)
					.HeightOverride(30)
					[
						//左右结构
						SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
						.FillWidth(1)
						[
							//“生成”按钮
							SNew(SButton)
							.ContentPadding(0)
							.ButtonStyle(FCoreStyle::Get(), "Box")
							.ButtonColorAndOpacity(FLinearColor(FColor(0XFFFD9800)))
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.OnClicked(this,&SArmyPanorama::NextStepClicked_Option)
							[
								SNew(STextBlock)
								.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
								.ColorAndOpacity(FLinearColor::White)
								.Text(FText::FromString(TEXT("生成")))
							]
						]

						+ SHorizontalBox::Slot()
						.FillWidth(1)
						.Padding(FMargin(20, 0, 0, 0))
						[
							SNew(SButton)
							.ButtonStyle(FCoreStyle::Get(), "Box")
							.ButtonColorAndOpacity(FLinearColor(FColor(0XFF353638)))
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.OnClicked(this,&SArmyPanorama::CancelClicked)
							[
								SNew(STextBlock)
								.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
								.ColorAndOpacity(FLinearColor::White)
								.Text(FText::FromString(TEXT("关闭")))
							]
						]
					]
				]
			]
		];
}

TSharedRef<SWidget> SArmyPanorama::CreatePage_Build()
{
	//image
	FSlateBrush* GenericWhiteBox = new FSlateImageBrush(FName(*(FPaths::EngineContentDir() / TEXT("/Slate/Old/White.png"))), FVector2D(1, 1), FLinearColor(FColor(0XFF0B0B0C)));
	FSlateBrush* GenericWhiteBox_Watting = new FSlateImageBrush(FName(*(FPaths::EngineContentDir() / TEXT("/Slate/Old/White.png"))), FVector2D(48, 48), FLinearColor::Blue);
	FSlateBrush* GenericWhiteBox_Gray = new FSlateImageBrush(FName(*(FPaths::EngineContentDir() / TEXT("/Slate/Old/White.png"))), FVector2D(117, 30), FLinearColor::Gray);

	return 
		//SizeBox控制界面大小
		SNew(SBox)
		.WidthOverride(420)
		.MinDesiredHeight(304)
		[
			//Border控制背景色
			SNew(SBorder)
			.Padding(FMargin(20, 0, 20, 0))
			.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
			.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF28292B"))
			[
				SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.Padding(FMargin(0, 20, 0, 0))
				.AutoHeight()
				.HAlign(HAlign_Left)
				[
					SAssignNew(BuildingMessage, STextBlock)
					//.Text(FText::FromString(TEXT("正在生成中，预计生成时间10秒")))
					.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
					.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.Gray.FFC8C9CC"))
				]

				+ SVerticalBox::Slot()
				.Padding(FMargin(0, 16, 0, 0))
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				.AutoHeight()
				[
					SNew(SBox)
					.WidthOverride(380)
					.HeightOverride(136)
					[
						SNew(SBorder)
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(FMargin(0))
						.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
						.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF212224"))
						[
							SNew(SHorizontalBox)
							
							+ SHorizontalBox::Slot()
							.Padding(FMargin(8, 8, 0, 8))
							.AutoWidth()
							[
								SNew(SBox)
								.WidthOverride(120)
								.HeightOverride(120)
								[
									SNew(SBorder)
									.HAlign(EHorizontalAlignment::HAlign_Fill)
									.VAlign(EVerticalAlignment::VAlign_Fill)
									.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
									.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF28292B"))
									[
										SAssignNew(MySCircularThrobberSwitcher, SWidgetSwitcher)

										+ SWidgetSwitcher::Slot()
										.HAlign(EHorizontalAlignment::HAlign_Center)
										.VAlign(VAlign_Center)
										[
											//空白图
											SNew(SImage)
											.Image(FArmyStyle::Get().GetBrush("Icon.DefaultImage_QRCode"))
										]

										+ SWidgetSwitcher::Slot()
										[
											//二维码
											SAssignNew(ImageContainer, SBox)
										]
									]
								]
							]

							+ SHorizontalBox::Slot()
                            .FillWidth(1)
							[
								SNew(SVerticalBox)

								//"打开微信扫一扫，手机查看全景图"
								+ SVerticalBox::Slot()
								.Padding(FMargin(0, 32, 0, 0))
								.AutoHeight()
								.HAlign(HAlign_Center)
								[
									SNew(STextBlock)
									.Text(FText::FromString(TEXT("打开微信扫一扫，手机查看全景图")))
									.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
									.ColorAndOpacity(FLinearColor(FColor(0XFF9D9FA5)))
								]

                                + SVerticalBox::Slot()
                                .FillHeight(1)
                                .HAlign(HAlign_Center)
                                [
                                    SNew(SHorizontalBox)

                                    + SHorizontalBox::Slot()
                                    .VAlign(VAlign_Center)
                                    .AutoWidth()
                                    [
                                        // 超链接
                                        SNew(SArmyTextBlock)
									    .Text(MAKE_TEXT("立即查看"))
									    .OnClicked(this, &SArmyPanorama::URLButtonClicked)
									    .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFFD9800"))
                                    ]
                                    
                                    + SHorizontalBox::Slot()
                                    .VAlign(VAlign_Center)
                                    .Padding(50, 0, 0, 0)
                                    .AutoWidth()
                                    [
                                        // "复制到剪切板"
                                        SNew(SArmyTextBlock)
                                        .Text(MAKE_TEXT("复制链接"))
                                        .OnClicked(this, &SArmyPanorama::CopyButtonClicked)
                                        .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFFD9800"))
                                    ]
                                ]
							]
						]
					]
				]

				+ SVerticalBox::Slot()
				.Padding(FMargin(0, 16, 0, 0))
				.AutoHeight()
				[
					SNew(SHorizontalBox)

					+SHorizontalBox::Slot()
					.VAlign(EVerticalAlignment::VAlign_Center)
					.HAlign(EHorizontalAlignment::HAlign_Left)
					.AutoWidth()
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("接收人手机号")))
						.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
						.ColorAndOpacity(FLinearColor(FColor(0XFFC8C9CC)))
					]

					+SHorizontalBox::Slot()
					.Padding(FMargin(16,0,0,0))
					[
						SNew(SBox)
						.WidthOverride(400)
						.HeightOverride(32)
						[
							SNew(SEditableTextBox)
							.HintText(FText::FromString(TEXT("请输入接收方案人的手机号")))
							.Style(FCoreStyle::Get(), "Box")
							.BackgroundColor(FLinearColor(FColor(0XFF212226)))
							.ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
							.OnTextChanged(this, &SArmyPanorama::GetMyPhoneNum)
						]
					]
				]

				//手机号错误提示，初始隐藏
				+ SVerticalBox::Slot()
				.Padding(FMargin(93, 4, 0, 0))
				.AutoHeight()
				.HAlign(HAlign_Left)
				[
					SAssignNew(ErrorPhoneNum, STextBlock)
					.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
					.ColorAndOpacity(FLinearColor(FColor(0XFFFF5A5A)))
					.Text(FText::FromString(TEXT("手机号输入有误")))
					.Visibility(EVisibility::Collapsed)
				]

				//识别图
				+ SVerticalBox::Slot()
				.Padding(FMargin(0, 16, 0, 0))
				.AutoHeight()
				.HAlign(HAlign_Center)
				[
					SNew(SBox)
					.WidthOverride(400)
					.HeightOverride(30)
					[
						SNew(SHorizontalBox)
						
						+SHorizontalBox::Slot()
						.Padding(FMargin(38,0,0,0))
						.VAlign(EVerticalAlignment::VAlign_Center)
						.AutoWidth()
						[
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("验证码")))
							.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
							.ColorAndOpacity(FLinearColor(FColor(0XFFC8C9CC)))
						]
						//识别结果输入框
						+ SHorizontalBox::Slot()
						.Padding(FMargin(16, 0, 0, 0))
						.FillWidth(1)
						[
							SAssignNew(EditableTextBox_Identify, SEditableTextBox)
							.HintText(FText::FromString(TEXT("请输入验证码")))
							.Style(FCoreStyle::Get(), "Box")
							.BackgroundColor(FLinearColor(FColor(0XFF212226)))
							.ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
						]

						//识别图
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(FMargin(16, 0, 0, 0))
						[
							SAssignNew(GenericWhiteBox_IdentifyMap,SBox)
							[
								SNew(SImage)
								.Image(GenericWhiteBox_Gray)
							]
						]
					]
				]

				//验证码错误提示，初始隐藏
				+ SVerticalBox::Slot()
				.Padding(FMargin(93, 4, 0, 0))
				.AutoHeight()
				.HAlign(HAlign_Left)
				[
					SAssignNew(ErrorIdentifyingCode, STextBlock)
					.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
					.ColorAndOpacity(FLinearColor(FColor(0XFFFF5A5A)))
					.Text(FText::FromString(TEXT("验证码输入有误")))
					.Visibility(EVisibility::Collapsed)
				]

				+ SVerticalBox::Slot()
				.Padding(FMargin(0, 20, 0, 20))
				.FillHeight(1)
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Bottom)
					[
						SNew(SBox)
						.WidthOverride(180)
						.HeightOverride(30)
						[
							//左右结构
							SNew(SHorizontalBox)

							+ SHorizontalBox::Slot()
							.FillWidth(1)
							[
								//“发送”按钮
								SAssignNew(SendButton, SButton)
								.IsEnabled(false)
								.ButtonColorAndOpacity(FLinearColor(FColor(0XFFFD9800)))
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								.ButtonStyle(FCoreStyle::Get(), "Box")
								.OnClicked(this,&SArmyPanorama::SendClicked_Build)
								[
									SNew(STextBlock)
									.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
									.ColorAndOpacity(FLinearColor::White)
									.Text(FText::FromString(TEXT("发送")))
								]
						]

						+ SHorizontalBox::Slot()
						.FillWidth(1)
						.Padding(FMargin(20, 0, 0, 0))
						[
							//取消 按钮
							SNew(SButton)
							.ButtonColorAndOpacity(FLinearColor(FColor(0XFF353638)))
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.ButtonStyle(FCoreStyle::Get(), "Box")
							.OnClicked(this,&SArmyPanorama::CancelClicked)
							[
								SNew(STextBlock)
								.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
								.ColorAndOpacity(FLinearColor::White)
								.Text(FText::FromString(TEXT("取消")))
							]
						]
					]
				]
			]
		];
}

void SArmyPanorama::Construct(const FArguments & args)
{
	BIsSingle = args._BIsSingle;
	FArmyPanoramaMgr::Get().PanoramaCompletedDelegate.BindRaw(this, &SArmyPanorama::SetShortUrlAndQrCodeUrl);
	MyDeleForSendButton.BindRaw(this, &SArmyPanorama::RequestSendSubmit);

	ChildSlot
	[
		SAssignNew(MyWidgetSwitcher,SWidgetSwitcher)

		+ SWidgetSwitcher::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			//设置界面
			CreatePage_Option()
		]

		+ SWidgetSwitcher::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			//全景图生成界面
			CreatePage_Build()
		]
	];
}
