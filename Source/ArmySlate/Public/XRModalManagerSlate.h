// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "Tickable.h"

enum ECommonWindowType
{
	E_RollingMessage,
	E_RollingMessageCancel,
	E_MessageConfirm,
	E_MessageConfirmCancel,
	E_Custom,
};

enum ECustomWindowState
{
	// 不弹出
	ECWS_Normal,
	// 弹出等待提示框
	ECWS_RollingMessageCancel,
	// 消息确认框
	ECWS_MessageAutoDispear,
};

class ARMYSLATE_API FArmyModalManager : public FTickableGameObject
{
public:
	FArmyModalManager();
	~FArmyModalManager() {}

	//实现FTickableGameObject的接口
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	virtual void Tick(float DeltaTime) override;

	static FArmyModalManager& Get();

	void ShowWindow(ECommonWindowType _WindowType, FText _WindowTitle);
	void ShowCustomWindow(FText _WindowTitle, TSharedPtr<SWidget> _CustomWidget, bool InbModal = true);
	/**
	 * 使用ShowCustomWindow后，在上方叠加一个弹出框，用来显示等待状态
	 * @param InState - ECustomWindowState - ECWS_Normal状态为不弹出，ECWS_RollingMessageCancel弹出等待提示框（可取消），ECWS_MessageConfirm消息确认框
	 * @return void - 无
	 */
	void SetCustomWindowState(ECustomWindowState InState);
	void HideWindow();

	void SetMessage(FText _Message);

	FSimpleDelegate ConfirmDelegate;
	FSimpleDelegate CancelDelegate;
	FSimpleDelegate OverlayCancelDelegate;

private:
    void MakeWindowWidget(const FText& InWindowTitle, TSharedPtr<class SWidget> InMessageWidget, bool bCreateTitle = true);

private:
	TSharedPtr<SWindow> MyWindow;
	FText GetMessageText() const { return MessageText; }
	FText MessageText;

	void OnConfirmClicked();
	void OnCancelClicked();

	/** 自定义窗口等待提示框的关闭按钮点击 */
	void OnOverlayCloseClicked();

	/** 当前是否为自定义弹出窗口 */
	bool bCustomWindow;

	/** 弹出错误提示框的停留时间 */
	float ErrorMessageDuaration;
	/** 已经停留的时间 */
	float ErrorMessageElapsedTime;
	/** 是否需要计时 */
	bool bCountErrorMessageTime;

	TWeakPtr<class SArmyMessageMini> MessageMini;

private:
	/** 单例 */
	static TSharedPtr<FArmyModalManager> XRModalMgr;
};