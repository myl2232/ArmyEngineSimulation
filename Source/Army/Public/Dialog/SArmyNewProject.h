/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File SArmyNewProject.h
 * @Description 新建项目对话框
 *
 * @Author 欧石楠
 * @Date 2018年6月26日
 * @Version 1.0
 */

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "ArmyTypes.h"
#include "ArmyStyle.h"
#include "SComboBox.h"

/** 用户输入的项目信息 */
struct FArmyProjectInputInfo
{
	/** 项目名 */
	FString ProjectName;

    /** 套餐ID */
    int32 SetMealID;

	/** 项目编码 */
	FString ProjectCode;

	/** 业主姓名 */
	FString OwnerName;

	/** 业主联系电话 */
	FString OwnerPhoneNumber;

	/** 省ID */
	int32 ProvinceID;

	/** 市ID */
	int32 CityID;

	/** 区ID */
	int32 DistrictID;

	/** 小区名称 */
	FString VillageName;

    /** 详细地址 */
    FString DetailAddress;
};

class SArmyNewProject : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArmyNewProject) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

    /** 获取用户输入的项目信息 */
    const FArmyProjectInputInfo GetProjectInputInfo();

	/**@欧石楠检查内容*/
	bool CheckIsValid();

public:
	/**@ 梁晓菲 用户输入的消息*/
	TSharedPtr<class SArmyEditableTextBox> ETB_ProjectName;
	TSharedPtr<class SArmyEditableTextBox> ETB_ProjectCode;
	TSharedPtr< SComboBox< TSharedPtr<FArmyKeyValue> > > ETB_SetMealID;
	TSharedPtr<class SArmyEditableTextBox> ETB_OwnerName;
	TSharedPtr<class SArmyEditableTextBox> ETB_OwnerPhoneNumber;
	TSharedPtr< SComboBox< TSharedPtr<FArmyKeyValue> > > ETB_ProvinceComboBox;
	TSharedPtr< SComboBox< TSharedPtr<FArmyKeyValue> > > ETB_CityComboBox;
	TSharedPtr< SComboBox< TSharedPtr<FArmyKeyValue> > > ETB_DistrictComboBox;
    TSharedPtr<SArmyEditableTextBox> ETB_Village;
	TSharedPtr<class SArmyEditableTextBox> ETB_DetailAddress;

	/** 错误提示 */
	TSharedPtr<class STextBlock> TB_ErrorMessage;

private:
	void RequestServerData();
	void CallBack_GroupList(struct FArmyHttpResponse Response);
	void RequestAreaData(int32 InParentID,int32 InDeep = 0);
	void CallBack_AreaList(struct FArmyHttpResponse Response, int32 InPareenID);
    void ReqVillageData();

	TSharedRef<SWidget> GenerateComboItem(TSharedPtr<FArmyKeyValue> InItem, int32 InType);
	void HandleComboItemChanged(TSharedPtr<FArmyKeyValue> NewSelection, ESelectInfo::Type SelectInfo, int32 InType);
	FText GetCurrentComboText(int32 InType) const;

	/**@梁晓菲 分开创建*/
    TSharedRef<SWidget> CreateProjectName();
	TSharedRef<SWidget> CreateContractID();
	TSharedRef<SWidget> CreateSetMealID();
	TSharedRef<SWidget> CreateOwnerName();
	TSharedRef<SWidget> CreateOwnerPhoneNumber();
	TSharedRef<SWidget> CreateProvinceComboBox();
	TSharedRef<SWidget> CreateCityComboBox();
	TSharedRef<SWidget> CreateDistrictComboBox();
	TSharedRef<SWidget> CreateVillage();
    TSharedRef<SWidget> CreateDetailAddress();

private:
	TArray< TSharedPtr<FArmyKeyValue> > GroupTypeList;
	TSharedPtr<FArmyKeyValue> CurrentGroupValue = MakeShareable(new FArmyKeyValue(-1,TEXT("请选择套餐")));//当前套餐

	TArray< TSharedPtr<FArmyKeyValue> > ProvinceList;
	TSharedPtr<FArmyKeyValue> CurrentProvince = MakeShareable(new FArmyKeyValue(-1, TEXT("请选择省")));//当前省

	TArray< TSharedPtr<FArmyKeyValue> > CityList;
	TSharedPtr<FArmyKeyValue> DefalutCity = MakeShareable(new FArmyKeyValue(-1, TEXT("请选择市")));//当前市
	TSharedPtr<FArmyKeyValue> CurrentCity = DefalutCity;//当前市

	TArray< TSharedPtr<FArmyKeyValue> > DistrictList;
	TSharedPtr<FArmyKeyValue> DefaultDistrict = MakeShareable(new FArmyKeyValue(-1, TEXT("请选择区/县")));//当前区
	TSharedPtr<FArmyKeyValue> CurrentDistrict = DefaultDistrict;//当前区

    TArray< TSharedPtr<FArmyKeyValue> > VillageList;
    TSharedPtr<FArmyKeyValue> CurrentVillageValue = MakeShareable(new FArmyKeyValue(-1, TEXT("请选择小区")));
};