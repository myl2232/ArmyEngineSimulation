/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File SArmyProjectList.h
 * @Description 项目列表
 *
 * @Author 欧石楠
 * @Date 2018年6月14日
 * @Version 1.0
 */

#pragma once

#include "SScrollWrapBox.h"

#include "Widgets/SCompoundWidget.h"

class SArmyProjectList : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyProjectList) {}

    /** 项目列表瀑布流加载 */
    SLATE_EVENT(FSimpleDelegate, OnProjectListScrollToEnd)

    /** 新建项目事件 */
    SLATE_EVENT(FOnClicked, OnNewProjectClicked)

    SLATE_END_ARGS()
    
    void Construct(const FArguments& InArgs);

    void AddProjectWidget(TSharedRef<SWidget> Widget);

    /** 清空项目列表 */
    void EmptyProjectList();

private:
    /** 创建新建项目按钮 */
    TSharedRef<SWidget> MakeNewProjectButton();

    /** 新建按钮交互 */
    void OnNewProjectHovered();
    void OnNewProjectUnhovered();
    TSharedPtr<SBorder> NewProjectBorder;
    FOnClicked OnNewProjectClicked;

private:
    TSharedPtr<SScrollWrapBox> Container;

    TSharedPtr<class SArmyNewProject> NewProjectWidget;
};