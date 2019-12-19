/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File SArmyHardModeContentBrowser.h
 * @Description 水电模式类目
 *
 * @Author 朱同宽
 * @Date 2018年6月25日
 * @Version 1.0
 */

#pragma once

#include "SArmyCategoryWidget.h"
#include "SArmyModelContentBrowser.h"
#include "ArmyTypes.h"

class SArmyHydropowerModeContentBrowser : public SArmyModelContentBrowser
{
public:
	//请求菜单
	//virtual void RequestCategory ()override;
	//请求瀑布流项
	virtual void ReqContentItems (const int32 Key,const int32 Value) override;

	// @zengy 重置套餐分类
	void OnBeginMode();
	
protected:
	//得到选择的项是否支持下载
	virtual bool GetIsSupportDownload(FContentItemPtr _Item) override;
	/** 请求搜索内容和填充内容 */
	virtual void RequestSearchContentItems(bool isRequestBrank = false, bool isRequestSize = false/*是否请求品牌或规格筛选项*/) override;
};