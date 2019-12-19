/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRSingleDoorDetail.h
 * @Description 单开门属性界面
 *
 * @Author 欧石楠
 * @Date 2018年9月6日
 * @Version 1.0
 */

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "SArmyReplaceList.h"

class FArmySingleDoorDetail : public TSharedFromThis<FArmySingleDoorDetail>
{
public:
    FArmySingleDoorDetail();

    TSharedPtr<SWidget> CreateDetailWidget(TAttribute< TWeakPtr<FArmyObject> > InSelectedObject, TSharedPtr<class SArmyMulitCategory> CategoryDetail = nullptr);

	TSharedPtr<class FArmyDetailBuilder> GetDetailBuilder() { return DetialBuilder; }

	TSharedPtr<class FArmyLayoutModeDetail> LayoutDetail;
private:
    TSharedPtr<SWidget> CreateNameWidget();
    TSharedPtr<SWidget> CreateHeightWidget();
    TSharedPtr<SWidget> CreateWidthWidget();
    TSharedPtr<SWidget> CreateOpenDirectionWidget();

    void OnHeightChanged(const FText& InText, ETextCommit::Type CommitType);
    void OnLengthChanged(const FText& InText, ETextCommit::Type CommitType);

    FText GetName() const;
    FText GetHeight() const;
    FText GetLength() const;
    FText GetThickness() const;

	TSharedPtr<SWidget> CreateSingleDoorMatTypeWidget();
	FText GetSingleDoorWallMatType() const;
	void OnSlectedSingleDoorMatTypeChanged(const int32 Key, const FString& Value);
	TSharedPtr<class SArmyPropertyComboBox> CB_SingleDoorMatType;
	TSharedPtr<FArmyComboBoxArray> SingleDoorMatList;
private:
    /** 选中的物体 */
    TAttribute< TWeakPtr<FArmyObject> > SelectedObject;

    TSharedPtr<class FArmyDetailBuilder> DetialBuilder;	

	TSharedPtr<SArmyReplaceButton> StyleReplaceWidget;
    TSharedPtr<SArmyReplaceList> StyleListWidget;

    TSharedPtr<FArmyComboBoxArray> OpenDirectionList;

    /**@欧石楠 门类型1 标准门 2 防盗门 3 推拉门*/
    int DoorType = 1;

	/** 宽高厚数据类型 */
	int32 DataType;

	/** @欧石楠 款式ID*/
	int32 StyleId;

	/** @欧石楠 宽ID*/
	int32 LengthId;

	/** @欧石楠 高ID*/
	int32 HeightId;

	/** @欧石楠 厚ID*/
	int32 WidthId;	

    /** 所有的选中项 */
    struct FArmySelection
    {
		int32 StyleID;
    } Selection;
};