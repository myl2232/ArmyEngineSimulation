/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SArmyRenderingImage.h
* @Description 生成效果图UI
*
* @Author HUAEN
* @Date 2018年6月21日
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
	//界面
	TSharedRef<SWidget> WidgetChoose();
	TSharedRef<SWidget> WidgetSuccess();
	TSharedRef<SWidget> WidgetFail();
	//控件
	TSharedPtr<SWidgetSwitcher> MyWidgetSwitcher;

private:
	/** 截图分辨率 */
	int32 RenderType;
	bool bRipperIsExportJpg;
	bool bRipperIsScreenshot;

	/** 效果图文件路径 */
	FString DesignSketchFilePath;
	/** 截图相关数据 */
	TArray< TArray<FColor> > ScreenShotMapArray;
	TArray< FString > RenderingImages;
	int32 ResWidth;
	int32 ResHight;
	/** 本帧设置分辨率，下一帧开始这个分辨率，启用截图 */
	bool bCustomScreenshot = false;
	
private:
	ECheckBoxState IsSelected(int32 InType) const;
	void OnCheckStateChanged(ECheckBoxState InCheck, int32 InType);
	/** 生成效果图 */
	FReply OnGenerateClicked();
	/** 打开生成好的效果图文件 */
	FReply OnOpenDesignSketchFile();
	/** 打开生成好的效果图位置 */
	FReply OnOpenDesignSketchDir();
	/** 关闭 */
	FReply CancelClicked();

private:
	void ProcessScreenShots(FViewport* InViewport);
	void HandleCustomScreenshot();
	/** 逐张截图 最后拼合 可实现高分辨率截图不至于显存过载 */
	void MapScreenShot(FViewport* InViewport);
	void ExportJpg(FViewport* InViewport);

	void BGetScreenData(bool BEnd);
	void ShutDown();

	//上传效果图
	bool UploadConstructionImage(const TArray<uint8>& InData);
	void Callback_RequestUploadImageFile(TSharedPtr<FAliyunOssResponse> Response);
	void RequestSaveRenderingImage();
	void Callback_RequestSaveRenderingImage(struct FArmyHttpResponse Response);
};