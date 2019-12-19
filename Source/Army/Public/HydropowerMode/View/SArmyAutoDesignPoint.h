#pragma once
/**
 * Copyright 2019 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File SArmyAutoDesignPoint.h
 * @Description 智能布点配置界面
 *
 * @Author 
 * @Date 2019年1月11日
 * @Version 1.0
 */
#pragma once

#include "CoreMinimal.h"
#include "SlateBasics.h"
#include "Widgets/SCompoundWidget.h"
#include "ArmyTypes.h"
//智能布点设置界面
class SArmyAutoDesignPoint : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SArmyAutoDesignPoint)
	{
	}
	SLATE_EVENT(FInt32Delegate, OnClickDelegate)
	SLATE_END_ARGS()
		
public:

	void Construct(const FArguments& InArgs);

	//获得是否勾选了自动生成水位
	bool GetIsAutoWaterInfo();
	//获得是否勾选了自动生成开关
	bool GetIsAutoSwitchInfo();
	//获得是否勾选了自动生成插座
	bool GetIsAutoSocketInfo();

private:
	FReply OnCommand(int32 InV);
	//确定取消代理
	FInt32Delegate OnClickDelegate;

	/** 生成内嵌图片的BoxItem*/
	TSharedPtr<SWidget> MakeAutoWaterBoxItem();
	TSharedPtr<SWidget> MakeAutoSwitchBoxItem();
	TSharedPtr<SWidget> MakeAutoSoketBoxItem();

	//根据勾选的状态改变按钮的状态
	void ChangeCurrentButtonStyle();

private:
	///*边框*/
	//TSharedPtr<SBorder> BorderWater;
	//TSharedPtr<SBorder> BorderSwitch;
	//TSharedPtr<SBorder>	BorderSocket;

	///*图片*/
	//TSharedPtr<SImage> IsWaterCheckImage;
	//TSharedPtr<SImage> IsSwitchCheckImage;
	//TSharedPtr<SImage> IsSocketCheckImage;

	//TSharedPtr<SCheckBox> AutoPointCheckBox;

	//生成UI自动布点设置确定取消按钮
	TSharedRef<SWidget> MakeConfirmCancelButton();

private:
	////是否自动生成点位
	//bool autoWater = false;
	//bool autoSwitch = false;
	//bool autoSocket = false;

	//是否勾选了自动生成水位，开关，插座的三个bool的数组
	//TArray<bool> IsAutoInfo;
	
	/** 内嵌图片的Item*/
	TSharedPtr<class SBoxItem> AutoWaterBoxItem;
	TSharedPtr<class SBoxItem> AutoSwitchBoxItem;
	TSharedPtr<class SBoxItem> AutoSocketBoxItem;

	TSharedPtr<class SButton> ConfirmBtn;

};

//自动布点失败提示框
class SArmyAutoDesignFailWid : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SArmyAutoDesignFailWid)
	{
	}
	/** 确认事件 */
	SLATE_EVENT(FOnClicked, OnConfirm)
	//失败信息
	SLATE_ARGUMENT(FString, FailInfo)
	SLATE_ARGUMENT(int32, FailInfoNum)
	SLATE_ARGUMENT(int32, FailInfoType)/*0: 仅为自动布置的问题； 1：出现了模型下载的问题*/
	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs);
};