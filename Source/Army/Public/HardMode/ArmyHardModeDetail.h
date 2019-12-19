/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRHardModeDetail.h
* @Description 硬装工具属性面板
*
* @Author 
* @Date 2018年6月30日
* @Version 1.0
*/
#pragma once

#include "ArmyDetailBuilder.h"
#include "ArmyDetailBase.h"
#include "STextBlockImageSpinbox.h"
#include "ArmyBaseEditStyle.h"
#include "ArmySceneData.h"
#include "ArmySingleDoorDetail.h"



enum EPropertyDetailType
{
	PDT_None = -1,

	//创建瓷砖、地板、墙砖、墙板属性面板
	PDT_FloorTile,

	//水刀拼花
	PDT_WaterKnife = 1,

	//波打线
	PDT_WaveLine = 2,

	//踢脚线、造型线、顶角线
	PDT_DXFModel = 3,

	//墙漆
	PDT_WallPaint = 4,

	//壁纸 、背景墙
	PDT_WallPaper = 5,

	//铝扣板
	PDT_AluminousGussetPlate = 6,

	//灯、浴霸、排风扇
	PDT_PAKModel = 7,

	//灯槽
	PDT_LampSlot = 8,

	//点光源
	PDT_PointLight = 9,
	//射灯
	PDT_SpotLight = 10,
	//反射球
	PDT_ReflectionSphere = 11,
	//普通窗户
	PDT_WindowNormal = 12,
	//矩形飘窗
	PDT_WindowRectBay = 13,
	//水电点位
	PDT_HYDROPOWERMODE = 14,

	PDT_DXFMODELRectLAMPSLOT = 15,

	// 门
	PDT_Door = 16,

	PDT_Pass = 17,

	//绘制区域额毛坯墙
	PDT_PlotArea_RoughWall,
	//绘制区域毛坯顶
	PDT_PlotArea_RoughRoof,
	//绘制区域毛坯地
	PDT_PlotArea_RoughFloor,

	/* 有新类型请在上方添加*/

	//毛坯墙
	PDT_RoughWall,
	//毛坯顶
	PDT_RoughRoof,
	//毛坯地
	PDT_RoughFloor,

	//世界属性
	PDT_WorldSettings,
	PDT_WorldSettingsAdvanced,
};
class FArmyObject;

class FArmyHardModeDetail : public FArmyDetailBase {

public:
	FArmyHardModeDetail();
	virtual ~FArmyHardModeDetail();

public:
	/** 创建属性widget*/
	virtual TSharedRef<SWidget> CreateDetailWidget() override;

	/** 显示选中的UObject属性*/
	virtual void ShowSelectedDetial(UObject* InSelectedObject) override;

	//得到属性面板
	TSharedPtr<SWidgetSwitcher> GetPropertyPanel();
	//得到选择的对象，pak、dxf模型待完善，此处无法获取
	TSharedPtr<class FArmyBaseArea> GetSelectObject();

	TSharedPtr<class FArmyFurniture> GetSelectFurniture();
	//设置选择的对象
	//@ isOrignalsurface 是不是原始面
	void SetSelectObject(TSharedPtr<FArmyBaseArea> _SelectObject,bool isOrignalsurface=false);

	//@郭子阳
	//是不是原始墙面
	bool IsOrignalsurface = false;

	//设置属性面板可见性
	void SetPropertyPanelVisibility(EVisibility _Visibility);
	EVisibility GetPropertyPanelVisibility() { return HardModePropertyPanel->GetVisibility(); }

	void SetEditAreaVisibility(const EVisibility _EditAreaVisibility);

	/** @马云龙 显示世界设置界面 */
	void TryToggleWorldSettings();
public:
	SOperatorCallBack EditAreaDelegate;
protected:
	//材质名称
	TSharedRef<class SEditableTextBox> CreateMaterialName();
	//材质长度
	TSharedPtr<SWidget> CreateScaleXWidget();
	//材质宽度
	TSharedPtr<SWidget> CreateScaleYWidget(bool bReadOnly = false);
	//高度
	TSharedPtr<SWidget> CreateScaleZWidget(bool bReadOnly = false);
	//毛坯长度
	TSharedPtr<SWidget> CreateRoughLengthWidget();
	//毛坯高度
	TSharedPtr<SWidget> CreateRoughHeightWidget();
	//面积
	TSharedPtr<SWidget> CreateAreaWidget();
	//缝隙
	TSharedRef<STextBlockImageSpinbox<float>> CreateGap();
	//角度
	TSharedRef<STextBlockImageSpinbox<int>> CreateAngle();
	//抬高
	TSharedRef<STextBlockImageSpinbox<int>> CreateRaise();
	//X偏移
	TSharedRef<STextBlockImageSpinbox<int>> CreateDeviationX();
	//Y偏移
	TSharedRef<STextBlockImageSpinbox<int>> CreateDeviationY();
	//编辑地面
	void CreateEditArea(class FArmyDetailNode & _DetailNode);

	//应用到房间的所有墙面
	void CreateApplyToAllWall(TSharedPtr<FArmyDetailBuilder>&);

	//对齐网格创建
	TSharedRef<class SWidget> CreateGridPanel();
	//色轮
	TSharedRef<SWidget> CreateWallPaintColor();
	//色号
	TSharedRef<class SWidget> CreateColorName();

	FText GetWallPaintColorValue()const;

	//窗户离地高度
	TSharedRef<SWidget> CreateWindowRaise();

	//强度
	TSharedRef<SWidget> CreateLightIntensity();
	//辐射半径
	TSharedRef<SWidget> CreateLightRadius();
	//光源半径
	TSharedRef<SWidget> CreateLightSourceRadius();
	//光源长度
	TSharedRef<SWidget> CreateLightSourceLength();
	//内径
	TSharedRef<SWidget> CreateSpotLightInnerAngle();
	//外径
	TSharedRef<SWidget> CreateSpotLightOuterAngle();
	//颜色
	TSharedRef<SWidget> CreateLightColor();
	//启用
	TSharedRef<SWidget> CreateLightEnable();
	//投影
	TSharedRef<SWidget> CreateLightCastShadow();
	//间接光强度
	TSharedRef<SWidget> CreateIndirectLightingIntensity();
	//光源类型
	TSharedPtr<SWidget> CreateLightMobilityTypeWidget();
	TSharedPtr<SWidget> CreateLightMobilityTypeNameWidget();

	//反射球半径
	TSharedRef<SWidget> CreateReflectionSphereRadius();
	//反射球亮度
	TSharedRef<SWidget> CreateReflectionSphereBrightness();
	//铺法修改
	TSharedRef<SWidget> CreatePavingMethodWidget();

	TSharedRef<SWidget> CreateWindowStoneMaterialGoods();

	// 是否生成灯带
	TSharedRef<SWidget> CreateLightCheckBoxDetail();
	ECheckBoxState GetLightCheckBoxState()const;
	void OnListCheckBoxStateChanged(ECheckBoxState InNewState);

	//毛坯名称
	TSharedRef<class SEditableTextBox> CreateRoughName();

	/** 门属性面板 */
	void CreateDoorDetail();
	TSharedRef<SWidget> CreateDoorGoods();
	TSharedPtr<FArmyDetailBuilder> DetailDoor;
	TSharedPtr<class SArmyReplaceButton> DoorReplaceButton;
	TSharedPtr<class SArmyDoorReplaceList> DoorGoodsReplaceList;

	/** 垭口属性面板 */
	void CreatePassDetail();
	TSharedRef<SWidget> CreatePassGoods();
	TSharedPtr<FArmyDetailBuilder> DetailPass;
	TSharedPtr<class SArmyDoorReplaceList> PassGoodsReplaceList;

protected:
	/************************************************************************/
	/*      创建各面板                                                      */

	//创建瓷砖、地板、墙砖、墙板属性面板
	//@WallType =0（地）、1（墙）、2（顶）
	void CreateDetailFloorTile(EWallType WallType= EWallType::Wall);

	//水刀拼花,已经没了
	void CreateDetailWaterKnife();

	//波打线
	void CreateDetailWaveLine();

	//踢脚线、造型线、顶角线
	void CreateDetailDXFModel();

	//墙漆
	void CreateDetailWallPaint(EWallType WallType = EWallType::Wall);

	//壁纸 、背景墙
	void CreateDetailWallPaper(EWallType WallType = EWallType::Wall);

	//铝扣板
	void CreateDetailAluminousGussetPlate();

	//灯、浴霸、排风扇以及3D模型
	void CreateDetailPAKModel();

	//灯槽
	void CreateDetailLampSlot();

	//点光源
	void CreateDetailPointLight();

	//射灯
	void CreateDetailSpotLight();

	//反射球
	void CreateDetailReflectionSphere();

	//普通窗
	void CreateDetailWindowNormal();

	//矩形飘窗
	void CreateDetailWindowRectBay();

	//水电点位
	void CreateOutletDetail();

	void CreateRoughWallDetail();
	void CreateRoughFloorDetail();
	void CreateRoughRoofDetail();

	//@郭子阳
	//绘制区域毛坯墙
	void CreatePlotAreaRoughWallDetail();
	void CreatePlotAreaRoughFloorDetail();
	void CreatePlotAreaRoughRoofDetail();

	//@马云龙 立面模式 世界设置
	void CreateWorldSettingsDetail();

	// @zengy 创建抬高或下吊的UI控件
	void CreateRaiseDetailUI(FArmyDetailNode &DetailNode, EWallType WallType);
	/************************************************************************/

	void CreateHasLightDXFDetail();

	/************************************************************************/

	//属性面板
	TSharedRef<SWidget> CreateHardModePropertyPanel();

	//离地高度
	TSharedPtr<SWidget> CreateLocationZWidget();

	//断点改造
	TSharedPtr<SWidget> CreateRefromWidget();

	//宽度
	FText GetScaleX()const;

	//长度
	FText GetScaleY()const;

	//毛坯长度
	FText GetRoughLength()const;

	FText GetWallHeight()const;

	//高度
	FText GetScaleZ()const;

	//面积
	FText GetArea() const;

	//缝隙
	float GetGapWidth()const;
	void OnGapWidthChanged(float InHeight, ETextCommit::Type CommitType);

	//旋转角度
	int GetRotationAngle()const;
	void OnRotationAngleChanged(int InHeight, ETextCommit::Type CommitType);

	//抬高
	int GetRaise()const;
	void OnRaiseChanged(int InHeight, ETextCommit::Type CommitType);

	//x方向偏移
	int GetXDirOffset()const;
	void OnXDirOffsetChanged(int InHeight, ETextCommit::Type CommitType);

	//y方向偏移
	int GetYDirOffset()const;
	void OnYDirOffsetChanged(int InHeight, ETextCommit::Type CommitType);

	//高度
	FText GetWindowRaise()const;

	ECheckBoxState GetReform() const;

	void CheckReform(ECheckBoxState InState);

	FText GetValueText() const;

	void OnGroundHeightChanged(const FText& text);

	void OnGroundCommitted(const FText& InText, const ETextCommit::Type InTextAction);


	FReply OnAlignClicked(AlignmentType AlignType);

	void SetSelecteAlignmentType(AlignmentType AlignType);
protected:
	//材质名称
	TSharedPtr<class SEditableTextBox> MaterialName;
	//材质长度
	TSharedPtr<STextBlockImageSpinbox<int>> MaterialLength;
	//材质宽度
	TSharedPtr<STextBlockImageSpinbox<int>> MaterialWidth;
	//面积
	TSharedPtr<STextBlockImageSpinbox<int>> Area;
	//缝隙
	TSharedPtr<STextBlockImageSpinbox<float>> Gap;
	//角度
	TSharedPtr<STextBlockImageSpinbox<int>> Angle;
	//抬高
	TSharedPtr<STextBlockImageSpinbox<int>> Raise;
	//投影
	TSharedPtr<SCheckBox> PointLightCastShadow;
	//铺法修改
	TSharedPtr<class SArmyPropertyComboBox> PavingMethodModify;

	/** 树状Detial*/
	TSharedPtr<FArmyDetailBuilder> PropertyDetailBuilder;

	//创建瓷砖、地板、墙砖、墙板属性面板
	TSharedPtr<class FArmyDetailBuilder> DetailFloorTile;

	//水刀拼花
	TSharedPtr<class FArmyDetailBuilder> DetailWaterKnife;

	//波打线
	TSharedPtr<class FArmyDetailBuilder> DetailWaveLine;

	//踢脚线、造型线、顶角线
	TSharedPtr<class FArmyDetailBuilder> DetailDXFModel;

	//墙漆
	TSharedPtr<class FArmyDetailBuilder> DetailWallPaint;

	//壁纸 、背景墙
	TSharedPtr<class FArmyDetailBuilder> DetailWallPaper;

	//铝扣板
	TSharedPtr<class FArmyDetailBuilder> DetailAluminousGussetPlate;

	//灯、浴霸、排风扇
	TSharedPtr<class FArmyDetailBuilder> DetailPAKModel;

	//灯槽
	TSharedPtr<class FArmyDetailBuilder> DetailLampSlot;

	//点光源
	TSharedPtr<class FArmyDetailBuilder> DetailPointLight;

	//射灯
	TSharedPtr<class FArmyDetailBuilder> DetailSpotLight;

	//反射球
	TSharedPtr<class FArmyDetailBuilder> DetailReflectionCapture;

	//窗
	TSharedPtr<class FArmyDetailBuilder> DetailWindowNormal;
	//矩形飘窗
	TSharedPtr<class FArmyDetailBuilder> DetailWindowRectBay;

	//水电点位
	TSharedPtr<FArmyDetailBuilder> OutletDetailBuilder;

	//毛坯墙
	TSharedPtr<FArmyDetailBuilder> DetailRoughWall;

	//毛坯顶
	TSharedPtr<FArmyDetailBuilder> DetailRoughRoof;

	//毛坯地
	TSharedPtr<FArmyDetailBuilder> DetailRoughFloor;

	/* @梁晓菲 HardModePropertyPanel属性面板，包括“属性”两个字*/
	TSharedPtr<class SWidget> HardModePropertyPanel;
	TSharedPtr<class SWidgetSwitcher> PropertyPanel;

	//选择的对象
	TSharedPtr<FArmyBaseArea> selectedObject;

	TMap<int32, TSharedPtr<class FArmyDetailBuilder>> DetailBuilderMap;
	TMap<int32, TSharedPtr<class SWidget>> SWidgetMap;

	// 点位默认高度（数组）
	TArray< TSharedPtr<float>>	GroundHeightArray;

	//应用材质和施工项到房间的所有墙面
	FReply ApplyToAllWall();

private:
	void TransformActor(AActor* Actor, const FVector& NewLocation);
	void RotateActor(AActor* Actor, const FRotator& NewRotation);
	void ScaleActor(AActor* Actor, const FVector& NewScale);
	FText GetName() const;
	FText GetRoughName() const;
	FString CachedName;
	float CachedLocationZ;

	float CachedRotationZ;

	void OnScaleXCommited(const FText & InText, ETextCommit::Type CommitType);
	float CachedScaleX = 0.0f;

	void OnScaleYCommited(const FText & InText, ETextCommit::Type CommitType);
	float CachedScaleY = 0.0f;

	void OnScaleZCommited(const FText & InText, ETextCommit::Type CommitType);
	float CachedScaleZ = 0.0f;

	void OnWindowRaiseCommited(const FText & InText, ETextCommit::Type CommitType);
	float CachedWindowRaise;

	//强度
	void OnLight3DIntensityChanged(int Intensity, ETextCommit::Type CommitType);
	int GetLight3DIntensity() const { return CachedLight3DIntensity; }
	int CachedLight3DIntensity;

	//辐射半径
	void OnLight3DRadiusChanged(int InValue, ETextCommit::Type CommitType);
	int GetLight3DRadius() const { return CachedLight3DRadius; }
	int CachedLight3DRadius;

	//光源半径
	void OnLight3DSourceRadiusChanged(int InValue, ETextCommit::Type CommitType);
	int GetLight3DSourceRadius() const { return CachedLight3DSourceRadius; }
	int CachedLight3DSourceRadius;

	//光源长度
	void OnLight3DSourceLengthChanged(int InValue, ETextCommit::Type CommitType);
	int GetLight3DSourceLength() const { return CachedLight3DSourceLength; }
	int CachedLight3DSourceLength;

	//内径
	void OnSpotLight3DInnerAngleChanged(int InValue, ETextCommit::Type CommitType);
	int GetSpotLight3DInnerAngle() const { return CachedSpotLight3DInnerAngle; }
	int CachedSpotLight3DInnerAngle;

	//外径
	void OnSpotLight3DOuterAngleChanged(int InValue, ETextCommit::Type CommitType);
	int GetSpotLight3DOuterAngle() const { return CachedSpotLight3DOuterAngle; }
	int CachedSpotLight3DOuterAngle;

	//光源颜色
	FReply OnColorClicked();
	void OnLight3DColorChanged(const FLinearColor InColor);
	FLinearColor GetLight3DColor() const { return CachedLight3DColor; }
	FSlateColor GetBorderColor()const { return FSlateColor(BorderColor); }
	FLinearColor CachedLight3DColor;
	//用于动态修改颜色按钮的颜色	
	FLinearColor BorderColor;

	//投影
	void OnLight3DCastShadowStateChanged(ECheckBoxState InNewState);
	ECheckBoxState GetLight3DCastShadow() const { return CachedLight3DCastShadow; }
	ECheckBoxState CachedLight3DCastShadow = ECheckBoxState::Unchecked;

	//启用
	void OnLight3DEnableChanged(ECheckBoxState InNewState);
	ECheckBoxState GetLight3DEnable() const { return CachedLight3DEnable; }
	ECheckBoxState CachedLight3DEnable;

	//间接光强度
	void OnIndirectLightingIntensityChanged(float InValue, ETextCommit::Type CommitType);
	float GetIndirectLightingIntensity() const { return CachedIndirectLightingIntensity; }
	float CachedIndirectLightingIntensity;

	//光源类型
	void OnLightMobilityTypeChanged(const int32 Key, const FString& Value);
	FText GetLightMobilityType() const { return FText::FromString(CacheLightMobility); }
	TSharedPtr<SArmyPropertyComboBox> LightMobilityTypeComboBox;
	TSharedPtr<FArmyComboBoxArray> LightMobilityTypeList;
	FString CacheLightMobility;


	//反射球半径
	void OnReflectionSphereRadiusChanged(int InValue, ETextCommit::Type CommitType);
	int GetReflectionSphereRadius() const { return CachedReflectionSphereRadius; }
	int CachedReflectionSphereRadius;

	//反射球强度
	void OnReflectionSphereBrightnessChanged(float InValue, ETextCommit::Type CommitType);
	float GetReflectionSphereBrightness() const { return CachedReflectionSphereBrightness; }
	float CachedReflectionSphereBrightness;

	FText GetEditAreaText()const { return EditArea; }
	void SetEditAreaText(const FText & _EditArea) { EditArea = _EditArea; }

	TSharedPtr<SArmyReplaceList> WindowRectBayMatReplaceList;

	void OnCheckedChange(ECheckBoxState NewCheckedState);
	ECheckBoxState GetWindowStoneWidEnable()const;
	TSharedPtr<SCheckBox> cbxWindowStone;
	ECheckBoxState cbxWindowStoneVisibility;

	//设置编辑地面可见性
	EVisibility GetEditAreaVisibility()const { return EditAreaVisibility; }
	FText EditArea;
	EVisibility EditAreaVisibility;
	FReply OnEditAreaClicked();
	//通过铺法ID创建铺法
	TSharedPtr<FArmyBaseEditStyle> GetBaseEditStyle(const int32 key);

	//属性面板支持的铺法相关
	void OnSlectedPavingMethodChanged(const int32 Key, const FString& Value);
	FText GetPavingMethodName()const { return SelectPavingName; }
	//属性面板显示选择面关联的铺法
	bool ShowPavingMethod(TSharedPtr<FArmyBaseArea> SelectArea);

	void SetDetailNodeProperty(TSharedPtr<FArmyDetailNode> detailNode, bool isReadOnly, bool isEnabled);

	void InitGroundHeightArray();

	FString ConversionStringByStyle(EStyleType _Style);

	/* 色轮点击事件*/
	FReply OnWallPaintColorClicked();
	void OnWallPaintColorChanged(const FLinearColor InColor);
	FSlateColor GetWallPaintBorderColor()const { return FSlateColor(WallPaintBorderColor); }
	FLinearColor WallPaintBorderColor;

	/** @马云龙 世界设置 包括后期，全局设置参数 */
	void InitializeWorldSettings();
	//太阳光亮度
	void OnDirectionalLightIntensityChanged(float InValue, ETextCommit::Type CommitType);
	float GetDirectionalLightIntensity() const { return CachedDirectionalLightIntensity; }
	float CachedDirectionalLightIntensity;
	TSharedRef<SWidget> CreateDirectionalLightIntensity();

	//太阳光颜色
	FReply OnDirectionalLightColorClicked();
	void OnDirectionalLightColorChanged(const FLinearColor InColor);
	FSlateColor GetDirectionalLightColor()const { return FSlateColor(CacheDirectionalLightColor); }
	FSlateColor GetDirectionalLightBorderColor()const { return FSlateColor(CacheDirectionalLightColor); }
	FLinearColor CacheDirectionalLightColor;
	TSharedRef<SWidget> CreateDirectionalLightColor();

	//太阳光间接光亮度
	void OnDirectionalLightIndirectIntensityChanged(float InValue, ETextCommit::Type CommitType);
	float GetDirectionalLightIndirectIntensity() const { return CachedDirectionalLightIndirectIntensity; }
	float CachedDirectionalLightIndirectIntensity;
	TSharedRef<SWidget> CreateDirectionalLightIndirectIntensity();

	//太阳光动静结合态
	void OnDirectionalLightStationaryChanged(ECheckBoxState InNewState);
	ECheckBoxState GetDirectionalLightStationary() const { return CachedDirectionalLightStationary; }
	ECheckBoxState CachedDirectionalLightStationary;
	TSharedRef<SWidget> CreateDirectionalLightStationary();

	//光源角度
	void OnDirectionalLightSourceChanged(float InValue, ETextCommit::Type CommitType);
	float GetDirectionalLightSource() const { return CachedDirectionalLightSource; }
	float CachedDirectionalLightSource;
	TSharedRef<SWidget> CreateDirectionalLightSource();

	//阴影指数
	void OnDirectionalLightShadowExponentChanged(float InValue, ETextCommit::Type CommitType);
	float GetDirectionalLightShadowExponent() const { return CachedDirectionalLightShadowExponent; }
	float CachedDirectionalLightShadowExponent;
	TSharedRef<SWidget> CreateDirectionalLightShadowExponent();

	//太阳角度（高度角）
	void OnDirectionalLightPitchChanged(int32 InValue, ETextCommit::Type CommitType);
	int32 GetDirectionalLightPitch() const { return CachedDirectionalLightPitch; }
	int32 CachedDirectionalLightPitch;
	TSharedRef<SWidget> CreateDirectionalLightPitch();

	//太阳角度（方向角）
	void OnDirectionalLightYawChanged(int32 InValue, ETextCommit::Type CommitType);
	int32 GetDirectionalLightYaw() const { return CachedDirectionalLightYaw; }
	int32 CachedDirectionalLightYaw;
	TSharedRef<SWidget> CreateDirectionalLightYaw();

	//天光亮度
	void OnSkyLightIntensityChanged(float InValue, ETextCommit::Type CommitType);
	float GetSkyLightIntensity() const { return CachedSkyLightIntensity; }
	float CachedSkyLightIntensity;
	TSharedRef<SWidget> CreateSkyLightIntensity();

	//天光间接光亮度
	void OnSkyLightIndirectIntensityChanged(float InValue, ETextCommit::Type CommitType);
	float GetSkyLightIndirectIntensity() const { return CachedSkyLightIndirectIntensity; }
	float CachedSkyLightIndirectIntensity;
	TSharedRef<SWidget> CreateSkyLightIndirectIntensity();

	//曝光亮度
	void OnAutoExposureBiasChanged(float InValue, ETextCommit::Type CommitType);
	float GetAutoExposureBias() const { return CachedAutoExposureBias; }
	float CachedAutoExposureBias;
	TSharedRef<SWidget> CreateAutoExposureBias();

	//后期间接光亮度
	void OnPPIndirectLightingIntensityChanged(float InValue, ETextCommit::Type CommitType);
	float GetPPIndirectLightingIntensity() const { return CachedPPIndirectLightingIntensity; }
	float CachedPPIndirectLightingIntensity;
	TSharedRef<SWidget> CreatePPIndirectLightingIntensity();

	//曝光强度
	void OnPPBloomIntensityChanged(float InValue, ETextCommit::Type CommitType);
	float GetPPBloomIntensity() const { return CachedPPBloomIntensity; }
	float CachedPPBloomIntensity;
	TSharedRef<SWidget> CreatePPBloomIntensity();

	//曝光阈值
	void OnPPBloomThresholdChanged(float InValue, ETextCommit::Type CommitType);
	float GetPPBloomThreshold() const { return CachedPPBloomThreshold; }
	float CachedPPBloomThreshold;
	TSharedRef<SWidget> CreatePPBloomThreshold();

	//静态光照场景缩放
	void OnStaticLightingLevelScaleChanged(float InValue, ETextCommit::Type CommitType);
	float GetStaticLightingLevelScale() const { return CachedStaticLightingLevelScale; }
	float CachedStaticLightingLevelScale;
	TSharedRef<SWidget> CreateStaticLightingLevelScale();

	//间接光反弹次数
	void OnNumIndirectLightingBouncesChanged(int32 InValue, ETextCommit::Type CommitType);
	int32 GetNumIndirectLightingBounces() const { return CachedNumIndirectLightingBounces; }
	int32 CachedNumIndirectLightingBounces;
	TSharedRef<SWidget> CreateNumIndirectLightingBounces();

	//天光反弹次数
	void OnNumSkyLightingBouncesChanged(int32 InValue, ETextCommit::Type CommitType);
	int32 GetNumSkyLightingBounces() const { return CachedNumSkyLightingBounces; }
	int32 CachedNumSkyLightingBounces;
	TSharedRef<SWidget> CreateNumSkyLightingBounces();

	//间接光质量
	void OnIndirectLightingQualityChanged(float InValue, ETextCommit::Type CommitType);
	float GetIndirectLightingQuality() const { return CachedIndirectLightingQuality; }
	float CachedIndirectLightingQuality;
	TSharedRef<SWidget> CreateIndirectLightingQuality();

	//间接光平滑
	void OnIndirectLightingSmoothnessChanged(float InValue, ETextCommit::Type CommitType);
	float GetIndirectLightingSmoothness() const { return CachedIndirectLightingSmoothness; }
	float CachedIndirectLightingSmoothness;
	TSharedRef<SWidget> CreateIndirectLightingSmoothness();

	//最大遮罩距离
	void OnMaxOccllusionDistanceChanged(int32 InValue, ETextCommit::Type CommitType);
	int32 GetMaxOccllusionDistance() const { return CachedMaxOccllusionDistance; }
	int32 CachedMaxOccllusionDistance;
	TSharedRef<SWidget> CreateMaxOccllusionDistance();

	//环境颜色
	FReply OnEnvironmentColorClicked();
	void OnEnvironmentColorChanged(const FLinearColor InColor);
	FSlateColor GetEnvironmentColor()const { return FSlateColor(CacheEnvironmentColor); }
	FSlateColor GetEnvironmentBorderColor()const { return FSlateColor(CacheEnvironmentColor); }
	FLinearColor CacheEnvironmentColor;
	TSharedRef<SWidget> CreateEnvironmentColor();

	//环境强度
	void OnEnvironmentIntensityChanged(float InValue, ETextCommit::Type CommitType);
	float GetEnvironmentIntensity() const { return CachedEnvironmentIntensity; }
	float CachedEnvironmentIntensity;
	TSharedRef<SWidget> CreateEnvironmentIntensity();

	//漫反射值增强
	void OnDiffuseBoostChanged(float InValue, ETextCommit::Type CommitType);
	float GetDiffuseBoost() const { return CachedDiffuseBoost; }
	float CachedDiffuseBoost;
	TSharedRef<SWidget> CreateDiffuseBoost();

	//体积光照密度
	void OnVolumetricLightmapDetailCellSizeChanged(int32 InValue, ETextCommit::Type CommitType);
	int32 GetVolumetricLightmapDetailCellSize() const { return CachedVolumetricLightmapDetailCellSize; }
	int32 CachedVolumetricLightmapDetailCellSize;
	TSharedRef<SWidget> CreateVolumetricLightmapDetailCellSize();

	//体积光照缓存
	void OnVolumetricLightmapMaximumBirckMemoryMbChanged(int32 InValue, ETextCommit::Type CommitType);
	int32 GetVolumetricLightmapMaximumBirckMemoryMb() const { return CachedVolumetricLightmapMaximumBirckMemoryMb; }
	int32 CachedVolumetricLightmapMaximumBirckMemoryMb;
	TSharedRef<SWidget> CreateVolumetricLightmapMaximumBirckMemoryMb();

	//灯带亮度
	void OnCeilingLampIntensityChanged(float InValue, ETextCommit::Type CommitType);
	float GetCeilingLampIntensity() const { return CachedCeilingLampIntensity; }
	float CachedCeilingLampIntensity;
	TSharedRef<SWidget> CreateCeilingLampIntensity();

	//光照图分辨率缩放
	void OnLightmapResScaleChanged(float InValue, ETextCommit::Type CommitType);
	float GetLightmapResScale() const { return CachedLightmapResScale; }
	float CachedLightmapResScale;
	TSharedRef<SWidget> CreateLightmapResScale();

	//顶/底角线光照图分辨率
	void OnSkirtLineStaticLightingResChanged(int32 InValue, ETextCommit::Type CommitType);
	int32 GetSkirtLineStaticLightingRes() const { return CachedSkirtLineStaticLightingRes; }
	int32 CachedSkirtLineStaticLightingRes;
	TSharedRef<SWidget> CreateSkirtLineStaticLightingRes();

	//造型线光照图分辨率
	void OnMoldingLineStaticLightingResChanged(int32 InValue, ETextCommit::Type CommitType);
	int32 GetMoldingLineStaticLightingRes() const { return CachedMoldingLineStaticLightingRes; }
	int32 CachedMoldingLineStaticLightingRes;
	TSharedRef<SWidget> CreateMoldingLineStaticLightingRes();

	//窗户光照图分辨率
	void OnWindowStaticLightingResChanged(int32 InValue, ETextCommit::Type CommitType);
	int32 GetWindowStaticLightingRes() const { return CachedWindowStaticLightingRes; }
	int32 CachedWindowStaticLightingRes;
	TSharedRef<SWidget> CreateWindowStaticLightingRes();

	//@马云龙 调试设置
	//家具参与烘焙
	void OnBuildStaticMeshChanged(ECheckBoxState InNewState);
	ECheckBoxState GetBuildStaticMesh() const { return CachedBuildStaticMesh; }
	ECheckBoxState CachedBuildStaticMesh;
	TSharedRef<SWidget> CreateBuildStaticMesh();

	//显示光照UV
	void OnShowLightmapUVChanged(ECheckBoxState InNewState);
	ECheckBoxState GetShowLightmapUV() const { return CachedShowLightmapUV; }
	ECheckBoxState CachedShowLightmapUV;
	TSharedRef<SWidget> CreateShowLightmapUV();

	//UV通道
	void OnUVChannelChanged(int32 InValue, ETextCommit::Type CommitType);
	int32 GetUVChannel() const { return CachedUVChannel; }
	int32 CachedUVChannel;
	TSharedRef<SWidget> CreateUVChannel();

	//FOV
	void OnFOVChanged(int32 InValue, ETextCommit::Type CommitType);
	int32 GetFOV() const { return CachedFOV; }
	int32 CachedFOV;
	TSharedRef<SWidget> CreateFOV();

	//外景切换
	void OnOutdoorPanoramaIndexChanged(int32 InValue, ETextCommit::Type CommitType);
	int32 GetOutdoorPanoramaIndex() const { return CachedOutdoorPanoramaIndex; }
	int32 CachedOutdoorPanoramaIndex;
	TSharedRef<SWidget> CreateOutdoorPanoramaIndex();

	//外景旋转
	void OnOutdoorPanoramaYawChanged(int32 InValue, ETextCommit::Type CommitType);
	int32 GetOutdoorPanoramaYaw() const { return CachedOutdoorPanoramaYaw; }
	int32 CachedOutdoorPanoramaYaw;
	TSharedRef<SWidget> CreateOutdoorPanoramaYaw();

private:
	//对齐网格9张图
	TSharedPtr<SImage> Image_Left_Top;
	TSharedPtr<SImage> Image_Mid_Top;
	TSharedPtr<SImage> Image_Right_Top;
	TSharedPtr<SImage> Image_Left_Mid;
	TSharedPtr<SImage> Image_Mid_Mid;
	TSharedPtr<SImage> Image_Right_Mid;
	TSharedPtr<SImage> Image_Left_Down;
	TSharedPtr<SImage> Image_Mid_Down;
	TSharedPtr<SImage> Image_Right_Down;

	TSharedPtr<SEditableTextBox> LengthWidget;
	TSharedPtr<SEditableTextBox>  WidthWidget;
	TSharedPtr<SEditableTextBox>  HeightWidget;
	TSharedPtr<SEditableTextBox>  AreaWidget;
	TSharedPtr<SEditableTextBox> ColorNameWidget;
	//铺法
	TSharedPtr<FArmyComboBoxArray> Array_Paving;
	FText SelectPavingName;

	TSharedPtr<class SArmyMulitCategory> ConstructionWallDetail;
	TSharedPtr<class SArmyMulitCategory> ConstructionRoofDetail;
	TSharedPtr<class SArmyMulitCategory> ConstructionFloorDetail;
	TSharedPtr<class SArmyMulitCategory> ConstructionDetail;

public:
	/** 施工项数据获取完毕的回调*/
	//void OnConstructionWallReady();
	//void OnConstructionRoofReady();
	//void OnConstructionFloorReady();

	/** @梁晓菲 此函数可以理解为存档*/
	void ExeBeforeClearDetail();
	enum DetailType
	{
		Index_None=-1,
		/** @梁晓菲 参数说明：0-2墙顶地 3软装 4铺贴类 5波打线 6放样类*/
		DT_RoughFloor = 0,
		DT_RoughWall = 1,
		DT_RoughRoof = 2,
		DT_Actor = 3,
		DT_Paste = 4,
		DT_Boda = 5,
		DT_Extrusion = 6,
		//@ 郭子阳 普通窗窗台石
		DT_NormalWindowStone = 7,
		//@ 郭子阳 飘窗窗台石
		DT_RectWindowStone = 8,
		//@ 郭子阳 新的放样类
		DT_ExtrusionLine = 9,

		DT_Door,
		DT_Pass,

	};

	DetailType detailType;

	void ExeWhileShowDetail(int32 ConstructionType);

	/** @梁晓菲 更新施工项UI*/
	void UpdateConstructionUI(int32 PDTType, TSharedPtr<FJsonObject> InJsonData);

	//当前的json数据
	TSharedPtr<FJsonObject> CurrentJsonData;

	TSharedPtr<class FArmyConstructionItemInterface> CurrentCheckData;

	EPropertyDetailType CurrentDetailType;
	//@郭子阳  获得施工项后的回调
	void ConstructionCallBack(enum class EConstructionRequeryResult ResultInfo, TSharedPtr<FJsonObject> ConstructionData, TSharedPtr<class FArmyConstructionItemInterface> CheckedData,struct ObjectConstructionKey Key, EPropertyDetailType DetailType , TSharedPtr<FArmyObject> Obj);

private:
	//@郭子阳 所选物品所在的房间
	TSharedPtr<FArmyRoom> SelectedRoom ;

};