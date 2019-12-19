/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRHomeModeDetail.h
* @Description 户型模式属性面板内容
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

class FArmyHomeModeDetail : public FArmyDetailBase {
public:

	/** 创建属性widget*/
	virtual TSharedRef<SWidget> CreateDetailWidget() override;

	/** 显示选中的XRObject属性*/
	virtual void ShowSelectedDetial(TSharedPtr<FArmyObject> InSelectedObject) override;

    TWeakPtr<FArmyObject> GetSelectedObject() const { return SelectedXRObject; }	

	TSharedPtr<class FArmySingleDoorDetail> SingleDoorDetail;	

private:
	void Init();	
	FText GetName() const { return FText::FromString(CachedName); }
	FString CachedName;	

	void CreateWallSettingDetail();
	void CreateSingleDoorDetail();	
	void CreateWindowDetail();
	void CreateFloorWindowDetail();
	void CreateSpaceDetail();
	void CreatePassDetail();
	void CreateDoorHoleDetail();
	void CreatePunchDetail();//开洞
	void CreatePillarDetail();//柱子
	void CreateAirFlueDetail();//烟道
	void CreateBeamDetail();//横梁	
	void CreateCurtainBoxDetail();//窗帘盒子
	void CreateRectBayWindowDetail();
	void CreateTrapeBayWindowDetail();

	void CreateModifyWallDetail();//拆改墙
	void CreateAddWallDetail();//新建墙
	void CreateBearingWallDetail();//承重墙

    void OnSpaceTypeListReady();

	void CreateDrainPointDetail();//下水主管道
	void CreateClosestoolDetail();//坐便下水
	void CreateHighElvBoxDetail();//强电箱
	void CreateGasMainPipeDetail();//燃气主管
	void CreateGasMeterDetail();//燃气表
	void CreateWaterHotDetail();//热水点位
	void CreateWaterChilledDetail();//冷水点位
	void CreateWaterNormalDetail();//中水点位
	void CreateWaterSeparatorDetail();//分集水器
	void CreateBasinDetail();//地漏下水
	void CreateEleBoxLDetail();//分集水器
	void CreateEnterRoomWaterDetail();//入户水点位

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
	FText GetAddWallLength() const;
	FText GetAddWallHeight() const;	
	FText GetAddWallMatType() const;

	void OnAddWallLengthChanged(const FText& InText, ETextCommit::Type CommitType);
	void OnAddWallHeightChanged(const FText& InText, ETextCommit::Type CommitType);	

	TSharedPtr<SWidget> CreateAddWallLengthWidget();
	TSharedPtr<SWidget> CreateAddWallHeightWidget();	
	TSharedPtr<SWidget> CreateAddWallMatTypeWidget();

	void OnSlectedAddMatTypeChanged(const int32 Key, const FString& Value);
	TSharedPtr<SArmyPropertyComboBox> CB_AddMatType;
	TSharedPtr<FArmyComboBoxArray> AddWallMatList;
	//-------------------------BearingWall-------------------------	
	FText GetBearingWallLength() const;

	void OnBearingWallLengthChanged(const FText& InText, ETextCommit::Type CommitType);

	TSharedPtr<SWidget> CreateBearingWallLengthWidget();

	//-------------------------WallSetting-------------------------
	void OnWallHeightChanged(const FText& InText, ETextCommit::Type CommitType);
    void OnWallColorChanged(const FLinearColor InColor);

	//-------------------------Common-------------------------	
	FText GetCommonLength() const;
	FText GetCommonWidth() const;
	FText GetCommonHeight() const;
	FText GetCommonFloorAltitude() const;

	void OnCommonLengthChanged(const FText& InText, ETextCommit::Type CommitType);
	void OnCommonWidthChanged(const FText& InText, ETextCommit::Type CommitType);
	void OnCommonHeightChanged(const FText& InText, ETextCommit::Type CommitType);
	void OnCommonFloorAltitudeChanged(const FText& InText, ETextCommit::Type CommitType);

	TSharedPtr<SWidget> CreateCommonNameWidget();
	TSharedPtr<SWidget> CreateCommonLengthWidget();
	TSharedPtr<SWidget> CreateCommonHeightWidget();
	TSharedPtr<SWidget> CreateCommonFloorAltitudeWidget();

	//-------------------------Pass垭口---------------------------
	TSharedRef<SWidget> CreatePassCheckBoxDetail();
	void OnPassCheckBoxStateChanged(ECheckBoxState InNewState);
	ECheckBoxState GetPassCheckBoxState() const;

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

	//-------------------------Punch----------------------------	
	//FText GetPunchGroungHeight() const;
	FText GetPunchDepth() const;
	//void OnPunchGroundHeightChanged(const FText& InText, ETextCommit::Type CommitType);
	void OnPunchDepthChanged(const FText& InText, ETextCommit::Type CommitType);
	//TSharedPtr<SWidget> CreatePunchGroundHeightWidget();
	TSharedPtr<SWidget> CreatePunchDepthWidget();

	//-------------------------Window----------------------------	
	TSharedPtr<SWidget> CreateWindowStoneCheckBoxWidget();
	void OnWindowStoneCheckBoxStateChanged(ECheckBoxState InNewState);
	ECheckBoxState GetWindowStoneCheckBoxState() const;
	//FText GetWindowGroungHeight() const;	
	//void OnWindowGroundHeightChanged(const FText& InText, ETextCommit::Type CommitType);	
	//TSharedPtr<SWidget> CreateWindowGroundHeightWidget();

	//-------------------------FloorWindow-----------------------	
	//TODO : Floor Window special function
	//TSharedPtr<SWidget> CreateFloorWindowAltitudeWidget();
	//-------------------------RectBayWindow---------------------		

	FText GetRectBayWindowDepth() const;
	void OnRectBayWindowDepthChanged(const FText& InText, ETextCommit::Type CommitType);
	TSharedPtr<SWidget> CreateRectBayWindowDepthWidget();

	//FText GetRectBayWindowHeight() const;
	//void OnRectBayWindowHeightChanged(const FText& InText, ETextCommit::Type CommitType);
	//TSharedPtr<SWidget> CreateRectBayWindowHeightWidget();

	FText GetRectBayWindowType() const;
	TSharedPtr<SWidget> CreateRectBayWindowTypeWidget();
	void OnSlectedRectBayTypeChanged(const int32 Key, const FString& Value);
	TSharedPtr<SArmyPropertyComboBox> RectBayWindowTypeComboBox;
	TSharedPtr<FArmyComboBoxArray> RectBayWindowTypeList;

	TSharedPtr<SWidget> CreateRectBayWindowStoneWidget();
	void OnRectBayWindowStoneCheckBoxStateChanged(ECheckBoxState InNewState);
	ECheckBoxState GetRectBayWindowStoneCheckBoxState() const;
	//-------------------------TrapeBayWindow---------------------	

	TSharedPtr<SWidget> CreateBayWindowOutLengthWidget();
	FText GetBayWindowOutLength() const;
	void OnBayWindowOutLengthChanged(const FText& InText, ETextCommit::Type CommitType);

	//-------------------------Space----------------------------
    FText GetSpaceName() const;
	FText GetSpaceArea() const;
	FText GetSpacePerimeter() const;
	int GetOriginGroundHeight()const;
	TSharedPtr<SWidget> CreateSpaceNameWidget();
	TSharedPtr<SWidget> CreateSpaceAreaWidget();
	TSharedPtr<SWidget> CreateSpacePerimeterWidget();
	TSharedPtr<SWidget> CreateSpaceOriginGroundHeightWidget();
	void OnSlectedSpaceNameChanged(const int32 Key, const FString& Value);
	void OnOriginGroundHeightChanged(int InValue);
    TSharedPtr<SArmyPropertyComboBox> CB_SpaceName;

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

	//-------------------------SimpleComponent-------------------------	
	//长
	FText GetSimpleComponentLength()const;
	void OnSimpleComponentLengthChanged(const FText& InText, ETextCommit::Type CommitType);
	//宽
	FText GetSimpleComponentWidth()const;
	void OnSimpleComponentWidthChanged(const FText& InText, ETextCommit::Type CommitType);
	//高
	FText GetSimpleComponentHeight()const;
	void OnSimpleComponentHeightChanged(const FText& InText, ETextCommit::Type CommitType);
	//角度
	FText GetSimpleComponentAngle()const;
	void OnSimpleComponentAngleChanged(const FText& InText, ETextCommit::Type CommitType);

	void OnAutoAdaptCheckBoxStateChanged(ECheckBoxState InNewState);
	ECheckBoxState GetAutoAdaptCheckBoxState() const;

	void OnAutoAdaptDirCheckBoxStateChanged(ECheckBoxState InNewState);
	ECheckBoxState GetAutoAdaptDirCheckBoxState() const;

	TSharedPtr<SWidget> CreateSimpleComponentLengthWidget();
	TSharedPtr<SWidget> CreateSimpleComponentWidthWidget();
	TSharedPtr<SWidget> CreateSimpleComponentHeightWidget();
	TSharedPtr<SWidget> CreateSimpleComponentAngleWidget();
	TSharedPtr<SWidget> CreateSimpleComponentAutoAdaptCheckWidget();
	TSharedPtr<SWidget> CreateSimpleComponentAutoAdaptDirCheckWidget();	

	//-------------------------PointPosition-------------------------	
	//长
	FText GetPointPositionLength()const;
	void OnPointPositionLengthChanged(const FText& InText, ETextCommit::Type CommitType);
	//宽
	FText GetPointPositionWidth()const;
	void OnPointPositionWidthChanged(const FText& InText, ETextCommit::Type CommitType);
	//高
	FText GetPointPositionHeight()const;
	void OnPointPositionHeightChanged(const FText& InText, ETextCommit::Type CommitType);
	//管径
	FText GetPipeRadius()const;
	void OnPipeRadiusChanged(const FText& InText, ETextCommit::Type CommitType);
	//离地高度
	FText GetPointPositionAltitude()const;
	void OnPointPositionAltitudeChanged(const FText& InText, ETextCommit::Type CommitType);

	//------尺寸可修改
	TSharedPtr<SWidget> CreatePointPositionLengthWidget();
	TSharedPtr<SWidget> CreatePointPositionWidthWidget();
	TSharedPtr<SWidget> CreatePointPositionHeightWidget();
	TSharedPtr<SWidget> CreatePointPositionAltitudeWidget();
	TSharedPtr<SWidget> CreatePipeRadiusWidget();
	TSharedPtr<SWidget> CreateModifyComboBoxWidget();


	//------尺寸不可修改
	TSharedPtr<SWidget> CreatePointPositionLengthReadOnlyWidget();
	TSharedPtr<SWidget> CreatePointPositionWidthReadOnlyWidget();
	TSharedPtr<SWidget> CreatePointPositionHeightReadOnlyWidget();
	TSharedPtr<SWidget> CreatePointPositionAltitudeReadOnlyWidget();
	TSharedPtr<SWidget> CreatePipeRadiusReadOnlyWidget();

	FText GetModifyComboBoxSize() const;
	void OnSlectedModifyComboBoxChanged(const int32 Key, const FString& Value);
	TSharedPtr<SArmyPropertyComboBox> CB_Modify;
	TSharedPtr<FArmyComboBoxArray> ModifyPointLocation;
private:
	/**
	 * @ 获取hardware离地高度 (不同的子类目前有多个版本的离地高度，后期考虑重构，目前暂时这样实现)
	 * @param InHardWare - TSharedPtr<class FArmyHardware> - 
	 * @return float - 
	 */
	//float GetHardWareAltitude(TSharedPtr<class FArmyHardware> InHardWare);

private:
	TSharedPtr<class SWidgetSwitcher> DetailWidgetSwitcher;
	TMap<EObjectType, TSharedPtr<SWidget>> DetailWidgetMap;	

	TSharedPtr<SWidget> WallSettingWidget;
	//DetailBuilder
	TSharedPtr<FArmyDetailBuilder> WallDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> SpaceDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> WindowsDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> FloorWindowsDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> SlidingDoorDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> PassDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> DoorHoleDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> PunchDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> PillarDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> FlueDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> BeamDetailBuilder;	
	TSharedPtr<FArmyDetailBuilder> CurtainBoxDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> RectBayWindowDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> TrapeBayWindowDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> ModifyWallDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> AddWallDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> BearingWallDetailBuilder;	

    TAttribute< TWeakPtr<FArmyObject> > SelectedObjectAttribute;

	//-------------------------PointPosition-------------------------	
	TSharedPtr<FArmyDetailBuilder> DrainPointBuilder;
	TSharedPtr<FArmyDetailBuilder> ClosestoolBuilder;
	TSharedPtr<FArmyDetailBuilder> HighElvBoxBuilder;
	TSharedPtr<FArmyDetailBuilder> LowElvBoxBuilder;
	TSharedPtr<FArmyDetailBuilder> HighGasMainPipeBuilder;
	TSharedPtr<FArmyDetailBuilder> HighGasMeterBuilder;
	TSharedPtr<FArmyDetailBuilder> HighWaterHotBuilder;
	TSharedPtr<FArmyDetailBuilder> HighWaterChilledBuilder;
	TSharedPtr<FArmyDetailBuilder> HighWaterNormalBuilder;
	TSharedPtr<FArmyDetailBuilder> HighWaterSeparatorBuilder;
	TSharedPtr<FArmyDetailBuilder> BasinBuilder;
	TSharedPtr<FArmyDetailBuilder> EnterRoomWaterBuilder;
};