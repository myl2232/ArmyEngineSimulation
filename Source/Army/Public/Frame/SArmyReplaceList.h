/**
 * Copyright 2019 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File SArmyReplaceList.h
 * @Description 模型替换列表
 *
 * @Author 欧石楠
 * @Date 2019年1月2日
 * @Version 1.0
 */

#pragma once

#include "SContentItem.h"
#include "Widgets/SCompoundWidget.h"
#include "ArmyCommonTypes.h"

DECLARE_DELEGATE_OneParam(FOnReplaceDelegate, FContentItemPtr);

/**
 * 商品替换列表
 */
class SArmyReplaceList : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyReplaceList)
        : _DefaultGoodsId(-1)
    {}

    /** 商品品类代码 */
    SLATE_ARGUMENT(FString, CategoryCode)

    /** 默认选中的商品id */
    SLATE_ARGUMENT(int32, DefaultGoodsId)

    /** 标题 */
    SLATE_ATTRIBUTE(FText, Title)

	/**是不是水电模式 */
	SLATE_ATTRIBUTE(bool, HyPowerMode)

    /** 无资源时的提示信息 */
    SLATE_ATTRIBUTE(FText, NoResourceTipText)

    /** 替换事件 */
    SLATE_EVENT(FOnReplaceDelegate, OnReplace)

    /** 关闭 */
    SLATE_EVENT(FSimpleDelegate, OnClose)

    SLATE_END_ARGS()

    ~SArmyReplaceList();
    
    void Construct(const FArguments& InArgs);

    //~ Begin SArmyReplaceList Interface
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
    //~ End SArmyReplaceList Interface

    void ReqNextPage();
    void FillList();

	void ClearList();

    TSharedPtr<class SContentItem> GetSelectedSContentItem() { return SelectedSContenItem; }

protected:
    /** 创建筛选项UI */
    virtual TSharedRef<SWidget> CreateFilterItems() { return SNullWidget::NullWidget; }

    /** 重新构建url地址 */
    virtual void RebuildFullUrl();

protected:
    FString FullUrl;

    int32 DefaultGoodsId;

    TSharedPtr<class IArmyHttpRequest> Request;

    int32 TotalPageNum = 0;

private:
    TSharedRef<SWidget> CreateTitleBar(const FArguments& InArgs);
    TSharedRef<SWidget> CreateReplaceList(const FArguments& InArgs);
    TSharedRef<SWidget> CreateDetailInfo();
    TSharedRef<SWidget> CreateNoResourceTips(const FArguments& InArgs);

    /** 得到选择项的下载状态 */
    EContentItemState GetContentItemState(FContentItemPtr Item);

    void OnContentItemSelected(int32 ItemCode);

    /** 下载资源 */
    void DownloadResource(const int32 ItemCode);

    void OnDownloadFinished(bool bWasSucceed, const int32 ItemIndex);

	int32 DataID = -1;
	bool bIsInit = false;

	bool IsHyPowerMode = false;
private:
    TSharedPtr<class SScrollWrapBox> SWB_ReplaceList;
    TSharedPtr<SVerticalBox> NoResourceTipWidget;
    TSharedPtr<class SImage> DetailImage;

    TArray< TSharedPtr<SContentItem> > ContentItemUIList;
	TSharedPtr<SContentItem> SelectedSContenItem;

    TArray<FContentItemPtr> ContentItemList;
    FContentItemPtr HoveredContentItem = nullptr;

    TAttribute<const FSlateBrush*> SlateBrush;

    FString CategoryCode;

    FOnReplaceDelegate OnReplace;

    FSimpleDelegate OnClose;

    /** 页码 */
    int32 PageIndex = 1;

    /** 资源类型 */
    EResourceType ResourceType;
};

/**
 * 替换按钮
 */
class SArmyReplaceButton : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyReplaceButton)
        : _ThumbnailBrush(FArmyStyle::Get().GetBrush("Icon.DefaultImage"))
    {}

    SLATE_EVENT(FOnClicked, OnClicked)

    /** 选中的缩略图 */
    SLATE_ATTRIBUTE(const FSlateBrush*, ThumbnailBrush)

    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

	void SetSelectedImage(TAttribute<const FSlateBrush*> InImage);

private:
    TWeakPtr<SArmyReplaceList> ReplaceListWidget;

	TSharedPtr<SImage> ThumbnailImage;
};