/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRGlobalActionCallBack.h
* @Description 全局命令回调
*
* @Author 欧石楠
* @Date 2018年6月21日
* @Version 1.0
*/

#pragma once
#include "CoreMinimal.h"

//@郭子阳
//任何全局操作都会触发的代理
DECLARE_MULTICAST_DELEGATE(FGlobalActionDelegate)

class FArmyGlobalActionCallBack {

private:
	FArmyGlobalActionCallBack();

public:
	XR_API static FArmyGlobalActionCallBack& Get();

	TSharedPtr<class FUICommandList> GetGlobalCommandsList() { return GlobalCommandList; }

	void SetGlobalCommandsList(TSharedPtr<class FUICommandList> InGlobalCommandList) { GlobalCommandList = InGlobalCommandList; }
private:
	/** singleton*/
	static TSharedPtr<FArmyGlobalActionCallBack> GXRACB;

public:
	/** 新建方案外部接口*/
	void OnNewPlan();

	/** 打开方案外部接口*/
	void OnOpenPlan();

	/** 保存方案外部接口*/
	void OnSavePlan();

	/** 另存方案外部接口*/
	void OnSaveAsPlan();

	/** 检查另存为大师方案权限接口*/
	void OnCheckSaveAsMasterPlan();

	/** 导出清单外部接口*/
	void OnExportList();

	/** @花恩 效果图*/
	void OnGenerateRenderingImage();

	/** @梁晓菲 单屋全景图外部接口*/
	void OnSinglePanorama();

	/** @梁晓菲 全屋全景图外部接口*/
	void OnMultiplePanorama();

    /** 注销登录 */
    void OnLogout();

    /** 退出程序 */
    void OnExit();

	/**@郭子阳 后台管理 */
	void OnBackGroundManager();

    XR_API void Logout();

	FGlobalActionDelegate OnGlobalAction;
private:
    void Exit();

	/** 另存为大师方案接口*/
	void OnSaveAsMasterPlan(struct FArmyHttpResponse Response);
private:
	TSharedPtr<class FUICommandList> GlobalCommandList;

	TSharedPtr<class SArmyNewPlan> NewPlanWidget;
	TSharedPtr<class SArmyOpenPlan> OpenPlanWidget;
	TSharedPtr<class SArmySaveAsMasterPlan> SaveAsMasterPlanWidget;
	TSharedPtr<class SArmyPanorama> PanoramaWidget;
	TSharedPtr<class SArmyRenderingImage> RenderingImageWidget;
};