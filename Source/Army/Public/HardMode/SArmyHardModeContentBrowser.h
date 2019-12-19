/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File SArmyHardModeContentBrowser.h
 * @Description 硬装模式类目
 *
 * @Author 欧石楠
 * @Date 2018年6月25日
 * @Version 1.0
 */

#pragma once

#include "SArmyModelContentBrowser.h"

#include "ArmyTypes.h"
enum ESubMode //所属子模块
{
	SM_None = 0,
	SM_FloorMode = 1,
	SM_WallMode = 2,
	SM_CeilingMode = 3
};
class SArmyHardModeContentBrowser : public SArmyModelContentBrowser
{
public:
	//请求菜单
	//virtual void RequestCategory();

	//请求瀑布流项
	virtual void ReqContentItems(const int32 Key, const int32 Value) override;

	/** 请求搜索内容和填充内容 */
	virtual void RequestSearchContentItems(bool isRequestBrank = false, bool isRequestSize = false/*是否请求品牌或规格筛选项*/) override;

	void SetSubMode(ESubMode _eSubMode) { eSubMode = _eSubMode; }
	ESubMode GetSubMode() { return eSubMode; }

	//显示替换瀑布流
	void ShowReplaceData();

	// @zengy 重置套餐分类
	void OnBeginMode();

protected:
	ESubMode eSubMode;

};