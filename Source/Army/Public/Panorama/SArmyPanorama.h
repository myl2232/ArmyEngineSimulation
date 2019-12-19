/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SArmyPanorama.h
* @Description 全景图
*
* @Author 梁晓菲
* @Date 2018年8月22日
* @Version 1.0
*/
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Slate.h"
#include "DelegateCombinations.h"
#include "SNumericEntryBox.h"
#include "IHttpRequest.h"

DECLARE_DELEGATE_OneParam(FDeleForSendButton, struct InformationWhenSendClicked)

//封装成结构体，通过代理将信息传递出去
struct ConfigOfPanorama
{
	FString EPanoramaAngle;					//视角大小
	FString	EValidTime;						//有效时间
	FString Introduction;					//简介
};

struct InformationWhenSendClicked
{
	FString phone;	//接收短信的手机号
	FString code;	//验证码
	FString uuid;	//验证码对应的随机码
	int32 panoId;	//全景图Id
};

class SArmyPanorama : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArmyPanorama) {}

	/** 当前面板关闭 */
	//将代理暴露出去，使其可以被外界绑定
	SLATE_ARGUMENT(bool, BIsSingle)

	SLATE_END_ARGS()

	void Construct(const FArguments & args);

	TSharedRef<SWidget> CreatePage_Option();
	TSharedRef<SWidget> CreatePage_Build();

	//这是一个对Widget的生成，用于Combobox
	TSharedRef<SWidget> GeneratePlatformComboItem(TSharedPtr<FString> InItem);

	//处理选中的Item
	void HandlePlatformComboChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo);
	void HandlePlatformComboChanged_Time(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo);

	//获取选中的Item的字，
	FText GetPlatformComboText() const;
	FText GetPlatformComboText_Time() const;

	//TSharedPtr<class SNumericEntryBox<int>> ValidTime;
	TSharedPtr<class SArmyEditableNumberBox> ValidTime;
	TSharedPtr<class STextBlock> ValidTime_Minute;

	//数组保存下拉框内容
	TArray<TSharedPtr <FString>> Array_Resolutions;
	TArray<TSharedPtr <FString>> Array_View;

	//存储当前选中的Item
	FString CurrentPlatformString;
	int32 NumberOfAngleItem;
	FString CurrentPlatformString_Time;
	int32 NumberOfTimeItem;

	//处理各种按钮被点击，需要调用代理
	FReply NextStepClicked_Option();
	FReply SendClicked_Build();
	FReply CancelClicked();
	void CopyButtonClicked();
	void  URLButtonClicked();

	//正则
	bool CheckStringIsVaild(const FString& str, const FString& Reg);
	bool CheckPhone(const FString& str);

	FDeleForSendButton MyDeleForSendButton;
	FSimpleDelegate Delegate_SinglePanorama;
	//FSimpleDelegate Delegate_MultiplePanorama;

	ConfigOfPanorama MyConfigOfPanorama;	//全景图设置

	void ResetWidget();

	void SetShortUrlAndQrCodeUrl(FString URL, FString QrCodeURL, int32 panoId);

	void RequestSendSubmit(InformationWhenSendClicked SendClickInformation);
	void Callback_RequestSendSubmit(struct FArmyHttpResponse Response);

private:
	bool BIsSingle;

	//设置界面样式
	FButtonStyle MyButtonStyle;
	FComboButtonStyle MyComboButtonStyle;
	FComboBoxStyle MyComboBoxStyle;
	FTableRowStyle MyItemStyle;


	//保存Widget
	TSharedPtr<SWidgetSwitcher> MyWidgetSwitcher;
	TSharedPtr<SWidgetSwitcher> MySCircularThrobberSwitcher;
	TSharedPtr<SMultiLineEditableTextBox> METB_Introduction;
	TSharedPtr<STextBlock> WarningForIntroductionNum;
	TSharedPtr<SBox> ImageContainer;
	TSharedPtr<SBox> GenericWhiteBox_IdentifyMap;
	TSharedPtr<SEditableTextBox> EditableTextBox_Identify;

	/** 全景图视角大小下拉列表 */
	TSharedPtr< SComboBox< TSharedPtr<FString> > > CB_PanoramaVisualAngle;

	/** 二维码有效时长下拉列表 */
	TSharedPtr< SComboBox< TSharedPtr<FString> > > CB_QRCodeValidTime;
	void GetValidTime(const FText& InText);

	InformationWhenSendClicked MyInformWhenSendClicked;	//点击 发送 时传递的信息

	//全景图简介文字
	void GetMyText(const FText& InText);
	FText GetIntroduction() { return FText::FromString(Introduction); }

	FString Introduction;

	//电话号码
	void GetMyPhoneNum(const FText& InText);

	/** 全景图Url */
	FText PanoramaUrl;

	//QRCode 图
	TSharedPtr<SImage> MyQRCodeImage;

	//字数
	int32 LengthOfIndroduction;

	FGuid CurrentVerificationID;

	//电话号码和验证码错误提示框
	TSharedPtr<STextBlock> ErrorPhoneNum;
	TSharedPtr<STextBlock> ErrorIdentifyingCode;

	//生成状态的提示框
	TSharedPtr<STextBlock> BuildingMessage;

	//发送按钮
	TSharedPtr<SButton> SendButton;
};
