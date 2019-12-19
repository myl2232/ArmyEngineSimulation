/**
* Copyright 2018 ������Ƽ����޹�˾.
* All Rights Reserved.
* 
*
* @File SArmyRenderingImage.h
* @Description ����Ч��ͼUI
*
* @Author HUAEN
* @Date 2018��6��21��
* @Version 1.0
*/

#pragma once

#include "AliyunOss.h"
#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "ArmyTypes.h"
#include "SWidgetSwitcher.h"
#include "SlateStyle.h"
#include "IHttpRequest.h"
#include "SlateTypes.h"

class SArmyRenderingImage : public SCompoundWidget {

	SLATE_BEGIN_ARGS(SArmyRenderingImage) {}
	SLATE_END_ARGS()

	void Construct(const FArguments &InArgs);
	
private:
	//����
	TSharedRef<SWidget> WidgetChoose();
	TSharedRef<SWidget> WidgetSuccess();
	TSharedRef<SWidget> WidgetFail();
	//�ؼ�
	TSharedPtr<SWidgetSwitcher> MyWidgetSwitcher;

private:
	/** ��ͼ�ֱ��� */
	int32 RenderType;
	bool bRipperIsExportJpg;
	bool bRipperIsScreenshot;

	/** Ч��ͼ�ļ�·�� */
	FString DesignSketchFilePath;
	/** ��ͼ������� */
	TArray< TArray<FColor> > ScreenShotMapArray;
	TArray< FString > RenderingImages;
	int32 ResWidth;
	int32 ResHight;
	/** ��֡���÷ֱ��ʣ���һ֡��ʼ����ֱ��ʣ����ý�ͼ */
	bool bCustomScreenshot = false;
	
private:
	ECheckBoxState IsSelected(int32 InType) const;
	void OnCheckStateChanged(ECheckBoxState InCheck, int32 InType);
	/** ����Ч��ͼ */
	FReply OnGenerateClicked();
	/** �����ɺõ�Ч��ͼ�ļ� */
	FReply OnOpenDesignSketchFile();
	/** �����ɺõ�Ч��ͼλ�� */
	FReply OnOpenDesignSketchDir();
	/** �ر� */
	FReply CancelClicked();

private:
	void ProcessScreenShots(FViewport* InViewport);
	void HandleCustomScreenshot();
	/** ���Ž�ͼ ���ƴ�� ��ʵ�ָ߷ֱ��ʽ�ͼ�������Դ���� */
	void MapScreenShot(FViewport* InViewport);
	void ExportJpg(FViewport* InViewport);

	void BGetScreenData(bool BEnd);
	void ShutDown();

	//�ϴ�Ч��ͼ
	bool UploadConstructionImage(const TArray<uint8>& InData);
	void Callback_RequestUploadImageFile(TSharedPtr<FAliyunOssResponse> Response);
	void RequestSaveRenderingImage();
	void Callback_RequestSaveRenderingImage(struct FArmyHttpResponse Response);
};