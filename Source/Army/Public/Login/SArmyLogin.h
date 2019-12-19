/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File SArmyLogin.h
 * @Description 登录界面
 *
 * @Author 欧石楠
 * @Date 2018年6月7日
 * @Version 1.0
 */

#pragma once

#include "ArmyTypes.h"
#include "SCheckBox.h"
#include "SArmyEditableTextBox.h"

class SArmyLogin : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyLogin) {}

    /** 登录回调，返回账号和密码 */
    SLATE_EVENT(FTwoStringDelegate, OnLogin)

    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
	void OnPwdCommitted(const FText& InText, ETextCommit::Type CommitType);

	void OnCheckStateChanged(ECheckBoxState InNewState);
	ECheckBoxState IsAutoChecked() const;

    FReply OnLoginButtonClicked();

    void OnApply();

	/**@欧石楠 忘记密码*/
	void OnForgetPwd();

    TSharedRef<SWidget> CreateOptionBox();
    TSharedRef<SWidget> CreateVersionTab();

public:
	/** @欧石楠 增加修改自动登录和密码文本*/
	//void SetAutoLoginCheckState(ECheckBoxState State);
	void EmptyPwdText();

public:
    TSharedPtr<SArmyEditableTextBox> ETB_Username;
    TSharedPtr<SArmyEditableTextBox> ETB_Pwd;
    TSharedPtr<SCheckBox> CB_RememberMe;
    TSharedPtr<SCheckBox> CB_AutoLogin;
    TSharedPtr<class SButton> Btn_Login;
    TSharedPtr<class STextBlock> TB_Version;
    TSharedPtr<STextBlock> TB_ErrorMessage;

private:
    FTwoStringDelegate OnLogin;
};