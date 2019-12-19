/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRLayoutModeDetail.h
 * @Description 拆改户型属性面板内容
 *
 * @Author 欧石楠
 * @Date 2018-07-10
 * @Version 1.0
 */

#pragma once

#include "ArmyDetailBase.h"
#include "ArmyTypes.h"
#include "ArmyObject.h"
#include "SArmyComboboxItem.h"
#include "SArmyComboBox.h"

class FArmyLayoutModeDetail : public FArmyDetailBase {
public:

	/** 创建属性widget*/
	virtual TSharedRef<SWidget> CreateDetailWidget() override;

	/** 显示选中的XRObject属性*/
	virtual void ShowSelectedDetial(TSharedPtr<FArmyObject> InSelectedObject) override;

	/** 显示选中的XRObject属性*/
	virtual void ShowSelectedDetial(TSharedPtr<FArmyObject> InSelectedObject, bool bHiddenSelected) override;

    TWeakPtr<FArmyObject> GetSelectedObject() const { return SelectedXRObject; }	

	TSharedPtr<class FArmySingleDoorDetail> SingleDoorDetail;

private:
	void Init();	
	FText GetName() const { return FText::FromString(CachedName); }
	FString CachedName;	
	void CreateWallSettingDetail();
	void CreateSingleDoorDetail();
	void CreateSpaceDetail(bool bIsModify = true);
	void CreatePassDetail();
	void CreateNewPassDetail();
	void CreateDoorHoleDetail();

    void CreateSlidingDoorDetail();
    TSharedPtr<SWidget> CreateOpenDirectionWidget();
    TSharedPtr<FArmyComboBoxArray> OpenDirectionList;

	/** 创建分割后区域的detail @纪仁泽 */
	void CreateSplitRegionDetail();

	void CreateModifyWallDetail();//拆改墙
	void CreateAddWallDetail();//新建墙

	void CreateIndependentWall();//独立墙
	void CreatePackPipeDetail();//包立管

    void OnSpaceTypeListReady();

	void CreateDrainPointDetail();//下水主管道
	void CreateClosestoolDetail();//坐便下水

	//创建门洞相关的施工项
	void CreatePassConstruction(TSharedPtr<class SArmyMulitCategory> ConstructionItem);

	//创建门洞相关的施工项
	void CreateDoorHoleConstruction(TSharedPtr<class SArmyMulitCategory> ConstructionItem);


	/**施工项数据获取完毕的回调*/				
	void OnConstructionOriginWallNewPassReady();
	void OnConstructionNewWallNewPassReady();

	void OnConstructionOriginPassReady();
	void OnConstructionDoorHoleReady();
	void OnConstructionPassReady();	

	void OnConstructionOriginWallSingleDoorReady();
	void OnConstructionNewWallSingleDoorReady();

	void OnConstructionOriginWallSlidingDoorReady();
	void OnConstructionNewWallSlidingDoorReady();

	void OnConstructionPackPipeReady();

	//---------------------------------------------------------
	void ExeWhileModifyWallShowDetail();
	void ExeWhileAddWallShowDetail();
	void ExeWhileIndepentWallShowDetail();
	void ExeWhilePassShowDetail();
	void ExeWhileNewPassShowDetail();
	void ExeWhileDoorHoleShowDetail();
	void ExeWhileSingleDoorShowDetail();
	void ExeWhileSlidingDoorShowDetail();
	void ExeWhilePackPipeShowDetail();
	//------------------------sliding door---------------------------------
	TSharedPtr<SWidget> CreateSlidingHeightWidget();
	void OnSlidingHeightChanged(const FText & InText, ETextCommit::Type CommitType);
	TSharedPtr<SWidget> CreateSlidingMatTypeWidget();
	FText GetSlidingWallMatType() const;
	void OnSlectedSlidingMatTypeChanged(const int32 Key, const FString& Value);
	TSharedPtr<SArmyPropertyComboBox> CB_SlidingMatType;
	TSharedPtr<FArmyComboBoxArray> SlidingMatList;
	//-------------------------WallSetting-------------------------	
	FText GetWallHeight() const;
    void OnWallColorChanged(const FLinearColor InColor);

	//-------------------------ModifyWall-------------------------	
	FText GetModifyWallLength() const;
	FText GetModifyWallHeight() const;
	FText GetModifyWallFloorDist() const;
	FText GetModifyWallMatType() const;

	void OnModifyWallLengthChanged(const FText& InText, ETextCommit::Type CommitType);	
	void OnModifyWallHeightChanged(const FText& InText, ETextCommit::Type CommitType);
	void OnModifyWallFloorDistChanged(const FText& InText, ETextCommit::Type CommitType);
	
	TSharedPtr<SWidget> CreateModifyWallLengthWidget();
	TSharedPtr<SWidget> CreateModifyWallHeightWidget();
	TSharedPtr<SWidget> CreateModifyWallFloorDistWidget();
	TSharedPtr<SWidget> CreateModifyWallMatTypeWidget();

	void OnSlectedModifyMatTypeChanged(const int32 Key, const FString& Value);
	TSharedPtr<SArmyPropertyComboBox> CB_ModifyMatType;
	TSharedPtr<FArmyComboBoxArray> ModifyWallMatList;
	
	//-------------------------AddWall-------------------------	
	FText GetAddWallWidth() const;
	FText GetAddWallHeight() const;	
	FText GetAddWallMatType() const;

	void OnAddWallWidthChanged(const FText& InText, ETextCommit::Type CommitType);
	void OnAddWallHeightChanged(const FText& InText, ETextCommit::Type CommitType);	

	TSharedPtr<SWidget> CreateAddWallWidthWidget();
	TSharedPtr<SWidget> CreateAddWallHeightWidget();	
	TSharedPtr<SWidget> CreateAddWallMatTypeWidget();

	bool GetAddWallEditBoxState() const;
	TSharedPtr<class SArmyEditableNumberBox> AddWallEditBox;

	TSharedPtr<SWidget> CreateAddWallHalfWallWidget();
	TSharedPtr<SCheckBox> WholeWallCheckBox;
	TSharedPtr<SCheckBox> HalfWallCheckBox;

	TSharedPtr<SWidget> CreateAddWallDirWidget();
	TSharedPtr<SCheckBox> AddWallDirCheckBox;
	ECheckBoxState GetAddWallDirCheckBoxState() const;
	void OnAddWallDirCheckBoxStateChanged(ECheckBoxState InNewState);

	ECheckBoxState GetWholeWallCheckBoxState() const;

	ECheckBoxState GetHalfWallCheckBoxState() const;

	void OnWholeWallCheckBoxStateChanged(ECheckBoxState InNewState);
	void OnHalfWallCheckBoxStateChanged(ECheckBoxState InNewState);

	void OnSlectedAddMatTypeChanged(const int32 Key, const FString& Value);
	TSharedPtr<SArmyPropertyComboBox> CB_AddMatType;
	TSharedPtr<FArmyComboBoxArray> AddWallMatList;	
	
	//-------------------------PackPipe----------------------------
	TSharedPtr<SWidget> CreatePackPipeHeightWidget();
	TSharedPtr<SWidget> CreatePackPipeLengthWidget();
	TSharedPtr<SWidget> CreatePackPipeThicknessWidget();

	FText GetPackPipeLength() const;
	FText GetPackPipeHeight() const;
	FText GetPackPipeThickness() const;

	//-------------------------Common-------------------------	
	FText GetCommonLength() const;
	FText GetCommonWidth() const;
	FText GetCommonHeight() const;

	void OnCommonLengthChanged(const FText& InText, ETextCommit::Type CommitType);
	void OnCommonWidthChanged(const FText& InText, ETextCommit::Type CommitType);
	void OnCommonHeightChanged(const FText& InText, ETextCommit::Type CommitType);

	TSharedPtr<SWidget> CreateCommonNameWidget();
	TSharedPtr<SWidget> CreateCommonLengthWidget();
	TSharedPtr<SWidget> CreateCommonHeightWidget();

	//-------------------------Pass垭口---------------------------
	TSharedRef<SWidget> CreatePassCheckBoxDetail();
	TSharedRef<SWidget> CreateFillPassCheckBoxDetail();
	TSharedPtr<SWidget> CreatePassLengthWidget();
	TSharedPtr<SWidget> CreatePassHeightWidget();
	void OnPassCheckBoxStateChanged(ECheckBoxState InNewState);
	void OnFillPassCheckBoxStateChanged(ECheckBoxState InNewState);
	ECheckBoxState GetPassCheckBoxState() const;
	ECheckBoxState GetFillPassCheckBoxState() const;

	void OnPassLengthChanged(const FText& InText, ETextCommit::Type CommitType);
	void OnPassHeightChanged(const FText& InText, ETextCommit::Type CommitType);
	
	TSharedPtr<SWidget> CreatePassMatTypeWidget();
	FText GetPassWallMatType() const;
	void OnSlectedPassMatTypeChanged(const int32 Key, const FString& Value);
	TSharedPtr<SArmyPropertyComboBox> CB_PassMatType;
	TSharedPtr<FArmyComboBoxArray> PassMatList;
	//-------------------------NewPass新开垭口---------------------------
	TSharedRef<SWidget> CreateNewPassCheckBoxDetail();			
	void OnNewPassCheckBoxStateChanged(ECheckBoxState InNewState);	
	ECheckBoxState GetNewPassCheckBoxState() const;	

	TSharedPtr<SWidget> CreateNewPassMatTypeWidget();
	FText GetNewPassWallMatType() const;
	void OnSlectedNewPassMatTypeChanged(const int32 Key, const FString& Value);
	TSharedPtr<SArmyPropertyComboBox> CB_NewPassMatType;
	TSharedPtr<FArmyComboBoxArray> NewPassMatList;
	//-------------------------门洞-------------------------------
	void OnSlectedAddDoorHoleMatTypeChanged(const int32 Key, const FString& Value);
	void OnSlectedModifyDoorHoleMatTypeChanged(const int32 Key, const FString& Value);
	TSharedPtr<SArmyPropertyComboBox> CB_AddDoorHoleMatType;
	TSharedPtr<SArmyPropertyComboBox> CB_ModifyDoorHoleMatType;
	TSharedPtr<FArmyComboBoxArray> AddDoorHoleMatList;
	TSharedPtr<FArmyComboBoxArray> ModifyDoorHoleMatList;

	TSharedPtr<SWidget> CreateAddDoorHoleMatTypeWidget();
	TSharedPtr<SWidget> CreateModifyDoorHoleMatTypeWidget();

	FText GetAddDoorHoleMatType() const;
	FText GetModifyDoorHoleMatType() const;

	TSharedPtr<SCheckBox> GenerateCheckBox;	
	//-------------------------Space----------------------------
    FText GetSpaceName() const;
	FText GetSpaceArea() const;
	FText GetSpacePerimeter() const;
	int GetFinishGroundHeight()const;
	TSharedPtr<SWidget> CreateSpaceNameWidget();
	TSharedPtr<SWidget> CreateSpaceAreaWidget();
	TSharedPtr<SWidget> CreateSpacePerimeterWidget();
	TSharedPtr<SWidget> CreateSpaceFinishGroundHeightWidget();
	void OnSlectedSpaceNameChanged(const int32 Key, const FString& Value);
	void OnFinishGroundHeightChanged(int InValue);
    TSharedPtr<SArmyPropertyComboBox> CB_SpaceName;

	void EexWhileRoomShowDetail();
	//-------------------------SplitRegion-------------------------
	/** 
	*  分割区域右侧栏操作 @纪仁泽
	*  分割区域名称 面积 周长 （空间名称使用SpaceName @See Space）
	*/

	/** 获取信息 */
	FText GetRegionSpaceName() const;
	FText GetSplitRegionName() const;
	FText GetSplitRegionArea() const;
	FText GetSplitReginoPerimeter() const;
	/** 创建分割区域Wideget 分割区域名称 面积 周长 */
	TSharedPtr<SWidget> CreateRegionSpaceNameWidget();
	TSharedPtr<SWidget> CreateSplitRegionNameWidget();
	TSharedPtr<SWidget> CreateSplitRegionAreaWidget();
	TSharedPtr<SWidget> CreateSplitRegionPerimeterWidget();
	/** 对分割区域名称进行选择 */
	void OnSlectedSplitRegionNameChanged(const int32 Key, const FString& Value);

	TSharedPtr<SArmyPropertyComboBox> CB_SplitRegionName;

	//-------------------------Furniture-------------------------		
	//长
	FText GetComponentLength()const;
	void OnComponentLengthChanged(const FText& InText, ETextCommit::Type CommitType);
	//宽
	FText GetComponentWidth()const;
	void OnComponentWidthChanged(const FText& InText, ETextCommit::Type CommitType);
	//高
	FText GetComponentHeight()const;
	void OnComponentHeightChanged(const FText& InText, ETextCommit::Type CommitType);
	//粗细
	FText GetComponentThickness()const;
	void OnComponentThicknessChanged(const FText& InText, ETextCommit::Type CommitType);
	//角度
	FText GetComponentAngle()const;
	void OnComponentAngleChanged(const FText& InText, ETextCommit::Type CommitType);
	//高度
	FText GetComponentAltitude()const;
	void OnComponentAltitudeChanged(const FText& InText, ETextCommit::Type CommitType);

	TSharedPtr<SWidget> CreateComponentLengthWidget();
	TSharedPtr<SWidget> CreateComponentWidthWidget();
	TSharedPtr<SWidget> CreateComponentHeightWidget();
	TSharedPtr<SWidget> CreateComponentThicknessWidget();
	TSharedPtr<SWidget> CreateComponentAngleWidget();
	TSharedPtr<SWidget> CreateComponentAltitudeWidget();

	//-------------------------PointPosition-------------------------	

	//管径
	FText GetPipeRadius()const;
	void OnPipeRadiusChanged(const FText& InText, ETextCommit::Type CommitType);
	//高度
	FText GetPipeHeight()const;
	void OnPipeHeightChanged(const FText& InText, ETextCommit::Type CommitType);

	TSharedPtr<SWidget> CreatePipeRadiusWidget();
	TSharedPtr<SWidget> CreatePipeHeightWidget();

private:
	void ExeWhileShowDetail(EObjectType ObjType);	
private:
	TSharedPtr<class SWidgetSwitcher> DetailWidgetSwitcher;
	TMap<EObjectType, TSharedPtr<SWidget>> DetailWidgetMap;	

	TSharedPtr<SWidget> WallSettingWidget;
	//DetailBuilder	
	TSharedPtr<FArmyDetailBuilder> WallDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> SpaceDetailBuilder;	
	TSharedPtr<FArmyDetailBuilder> SlidingDoorDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> PassDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> NewPassDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> DoorHoleDetailBuilder;	
	TSharedPtr<FArmyDetailBuilder> CurtainBoxDetailBuilder;	
	TSharedPtr<FArmyDetailBuilder> ModifyWallDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> AddWallDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> IndependentWallDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> PackPipeDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> SplitRegionDetailBuilder; //区域分割的DetailBuider @纪仁泽

	TSharedPtr<FArmyDetailBuilder> TestDetailBuilder;

    TAttribute< TWeakPtr<FArmyObject> > SelectedObjectAttribute;

	//-------------------------PointPosition-------------------------	
	TSharedPtr<FArmyDetailBuilder> DrainPointBuilder;
	TSharedPtr<FArmyDetailBuilder> ClosestoolBuilder;


	TSharedPtr<SWidget> PipeRadiusWid;
	TSharedPtr<SWidget> PipeHeightWid;

	int32 PassDataCount = 0;	

	TSharedPtr<class SArmyMulitCategory> ConstructionModifyWallDetail;
	TSharedPtr<class SArmyMulitCategory> ConstructionAddWallDetail;
	TSharedPtr<class SArmyMulitCategory> ConstructionIndepentWallDetail;	
	TSharedPtr<class SArmyMulitCategory> ConstructionNewPassDetail;	

	TSharedPtr<class SArmyMulitCategory> ConstructionSingleDoorDetail;
	TSharedPtr<class SArmyMulitCategory> ConstructionSlidingDoorDetail;

	TSharedPtr<class SArmyMulitCategory> ConstructionPassDetail;
	TSharedPtr<class SArmyMulitCategory> ConstructionDoorHoleDetail;

	TSharedPtr<class SArmyMulitCategory> ConstructionPackPipeDetail;

	TSharedPtr<class SArmyMulitCategory> ConstructionItemPassDetail;

};