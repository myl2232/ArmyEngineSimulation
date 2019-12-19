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
	// ������
	ECWS_Normal,
	// �����ȴ���ʾ��
	ECWS_RollingMessageCancel,
	// ��Ϣȷ�Ͽ�
	ECWS_MessageAutoDispear,
};

class ARMYSLATE_API FArmyModalManager : public FTickableGameObject
{
public:
	FArmyModalManager();
	~FArmyModalManager() {}

	//ʵ��FTickableGameObject�Ľӿ�
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	virtual void Tick(float DeltaTime) override;

	static FArmyModalManager& Get();

	void ShowWindow(ECommonWindowType _WindowType, FText _WindowTitle);
	void ShowCustomWindow(FText _WindowTitle, TSharedPtr<SWidget> _CustomWidget, bool InbModal = true);
	/**
	 * ʹ��ShowCustomWindow�����Ϸ�����һ��������������ʾ�ȴ�״̬
	 * @param InState - ECustomWindowState - ECWS_Normal״̬Ϊ��������ECWS_RollingMessageCancel�����ȴ���ʾ�򣨿�ȡ������ECWS_MessageConfirm��Ϣȷ�Ͽ�
	 * @return void - ��
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

	/** �Զ��崰�ڵȴ���ʾ��Ĺرհ�ť��� */
	void OnOverlayCloseClicked();

	/** ��ǰ�Ƿ�Ϊ�Զ��嵯������ */
	bool bCustomWindow;

	/** ����������ʾ���ͣ��ʱ�� */
	float ErrorMessageDuaration;
	/** �Ѿ�ͣ����ʱ�� */
	float ErrorMessageElapsedTime;
	/** �Ƿ���Ҫ��ʱ */
	bool bCountErrorMessageTime;

	TWeakPtr<class SArmyMessageMini> MessageMini;

private:
	/** ���� */
	static TSharedPtr<FArmyModalManager> XRModalMgr;
};