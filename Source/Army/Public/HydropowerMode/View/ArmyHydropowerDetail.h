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

#include "ArmyDetailBase.h"
#include "ArmyTypes.h"
#include "STextBlockImageSpinbox.h"
#include "IArmyHttp.h"
#include "SArmyMulitCategory.h"
#include "SArmyEditableTextBox.h"
 /** 详情面板类型 */
namespace EHydropowerDetailType
{
	enum Type
	{
		RefromType = 1, //显示断点改造
		TrapType = 2, //存水弯
		GroundHeight = 4,//离地高度
		PROPERTY_LENGHT = 8, //长
		PROPERTY_WIDTH = 16, //宽
		PROPERTY_HEIGHT = 32, //高
		PROPERTY_RADIUS = 64, //管径
		PROPERTY_SIZE = 128, //管长
	};

	enum HydropowerPropertyType 
	{
		None, //空
		OutLet,		//配电线、开关、插座
		Socket,    //插座
		Water,		//冷热水
		PrePT,		//预装点位
		Line,		//各种管线
		PipeFitting,//各种管件
	};

};

class SDropDownTextBox : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS (SDropDownTextBox)
		:_OptionsSource ()
	{
	}
	SLATE_ARGUMENT (const TArray< TSharedPtr<float> >*,OptionsSource)

	SLATE_EVENT (FOnTextChanged,OnTextChanged)

	SLATE_EVENT (FOnTextCommitted,OnTextCommitted)

	SLATE_ATTRIBUTE(FText,Text)

	SLATE_END_ARGS ()
public:

	void Construct (const FArguments& InArgs);
	TSharedRef<SWidget> OnGenerateWidget (TSharedPtr<float>  InItem);
	void OnWinowTypeChange (TSharedPtr<float>  NewSelection,ESelectInfo::Type SelectInfo);
	TOptional<float> GetValue ()const;
	void OnValueChanged (const FText& text);
	void OnTextCommitted (const FText& InText,const ETextCommit::Type InTextAction);
	EVisibility ShowComBox ()const;
private:
	TSharedPtr< SComboBox< TSharedPtr<float> >>  ComboBox;
	TSharedPtr<class SEditableTextBox > TextBlock;
	const TArray< TSharedPtr<float> >* OptionsSource;
	float CurrentValue;
	FOnTextChanged OnTextChangedCallback;
	FOnTextCommitted OnTextCommittedCallback;
};


class FArmyHydropowerDetail : public FArmyDetailBase
{
public:
	FArmyHydropowerDetail();
	~FArmyHydropowerDetail (){};
public:
	/** 创建属性widget*/
	virtual TSharedRef<SWidget> CreateDetailWidget ();

	/** 显示选中的XRObject属性*/
	virtual void ShowSelectedDetial (TSharedPtr<FArmyObject> InSelectedObject);

	/** 显示选中的UObject属性*/
	virtual void ShowSelectedDetial (UObject* InSelectedObject);

	//清空选择
	void Clear();

public:
	void InitShowType();
	bool IsRefrom ();
	bool IsShowGroundHeight ()const;
	bool IsDrain ();
	void InitGroundHeightArray ();
	void ConfigDetailScrollBox (int32 _Type);

	FText GetFurnitureName() const { return FText::FromString(CachedFurnitureName); }
	FString CachedFurnitureName;
	/**
	*  长度
	*/
	FText GetLenght() const;
	/**
	*  宽度
	*/
	FText GetWidth() const;
	/**
	*  高度
	*/
	FText GetHeight() const;
	/**
	*  管径
	*/
	FText GetRadius () const;
	ECheckBoxState GetReform () const;
	void CheckReform (ECheckBoxState InState);
	FText GetValueText ()const;
	TSharedRef<SWidget> OnDrainGenerateWidget (TSharedPtr<struct FArmyKeyValue>  InItem);
	void OnDrainTypeChanged (TSharedPtr<struct FArmyKeyValue>  NewSelection,ESelectInfo::Type SelectInfo);
	FText GetDrainTypeName () const;
	void OnGroundHeightChanged (const FText& text);
	void OnGroundCommitted (const FText& InText,const ETextCommit::Type InTextAction);


	void OnSlectedFlagChanged(const int32 Key,const FString& Value);
	FText GetFlag()const;

	ECheckBoxState GetGroup () const;
	void CheckGroup (ECheckBoxState InState);
public:
	TSharedPtr<class SScrollBox> DetailScrollBox;
	TSharedPtr<SCheckBox> CheckBox;	
	FArmyComboBoxArray DrainArray;
	TSharedPtr<FArmyComboBoxArray> FlagArray;
	TArray< TSharedPtr<float>>	GroundHeightArray;
	int32 ShowType;

	EVisibility IsFlagShow() const;
	/**@欧石楠 水电模式属性栏重构*/
private:
	TWeakPtr<FArmyObject> SelectObject;
private:

	void CreateOutletDetail();
	void CreateSocketDetail();
	void CreateWaterDetail();
	void CreatePrePTDetail();
	void CreateLineDetail();
	void CreatePipeFittingDetail();
	void CreateDefaultDetail();
	
private:
	//名称
	TSharedPtr<SWidget> CreateNameWidget();

	//墙体半透明
	TSharedRef<SWidget> CreateVisibility();

	//长
	TSharedPtr<SWidget> CreateScaleXWidget();

	//宽
	TSharedPtr<SWidget> CreateScaleYWidget();

	//高
	TSharedPtr<SWidget> CreateScaleZWidget();

	//管径
	TSharedPtr<SWidget> CreateRadiusWidget();

	//离地高度(ComboBox)
	TSharedPtr<SWidget> CreateLocationZWidget();
	
	//@郭子阳
	//离地高度(EditableTextBox)
	TSharedPtr<SWidget> CreateSingleLocationZWidget();
	TWeakPtr<SEditableTextBox> SingleLocationZWidget;
	//@郭子阳
	//设置离地高度框的可编辑性
	void SetSingleLocationZWidgetEditable(bool Value);


	//存水弯
	TSharedPtr<SWidget> CreateDrainWidget();

	//断点改造
	TSharedPtr<SWidget> CreateRefromWidget();	

	//相连接
	TSharedPtr<SWidget> CreateGroupWidget();

	//插座标记 
	TSharedPtr<SWidget> CreateSocketFlagWidget();

	FText LocationZText;
	float LocationZ;

private:

	//@郭子阳
	//离地高度框是否可编辑
	bool CanSingleLocationZWidgetEdit = true;

	TSharedPtr<class SWidgetSwitcher> DetailWidgetSwitcher;

	TMap<EHydropowerDetailType::HydropowerPropertyType, TSharedPtr<SWidget>> DetailWidgetMap;
	EHydropowerDetailType::HydropowerPropertyType SelectedType;

	TMap<EHydropowerDetailType::HydropowerPropertyType,TSharedPtr<FArmyDetailBuilder>> DetailBuilderMap;

	//DetailBuilder
	TSharedPtr<FArmyDetailBuilder> OutletDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> WaterDetailBuilder;	
	TSharedPtr<FArmyDetailBuilder> PrePTDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> LineDetailBuilder;
	TSharedPtr<FArmyDetailBuilder> PipeFittingDetailBuilder;	
	TSharedPtr<FArmyDetailBuilder> DefaultDetailBuilder;
	TSharedPtr< SSpinBox<int32> > Visibly;

	float CachedScaleX;
	float CachedScaleY;
	float CachedScaleZ;

	int32 Opacity;

//@郭子阳
//**断点改造相关*****

private:
	enum OffsetType
	{
		X=0,
		Y=1,
		Z=2
	};

	TSharedPtr<SEditableTextBox> OffsetWidget[3] ={nullptr,nullptr,nullptr};

	FVector Offsets  = FVector(0.0f,0.0f,0.0f);

	void UpdateOffset(FVector NewOffset);

	//是否停用
	bool IsStopped;

	//断点改造展开后的窗口
	TSharedPtr<SWidget> BreakPointWidget = SNullWidget::NullWidget ;// TSharedPtr<SNullWidget>();

	//void OnOffsetValueChanged(OffsetType type, int32 Value);

	/** 缩略图画刷 */
	TAttribute<const FSlateBrush*> CurrentThumbnailImageBrush=nullptr;

    

	//创建偏移距离文本框
	TSharedPtr<SWidget>  CreateOffsetWidget(OffsetType Type);
	//创建替换
	TSharedPtr<SWidget>  CreateReplaceWidget();
	//替换列表UI
	TSharedPtr<class SArmyReplaceList>  HyReplaceList;
	//替换按钮
	TSharedPtr<class SArmyReplaceButton>  HyReplaceButton;
	//创建停用
	TSharedPtr<SWidget>  CreateStopUsingWidget();
	
	//停用的两个按钮
	TSharedPtr<SCheckBox> Start;
	TSharedPtr<SCheckBox> Stop;

	void StartOrStopReforming(ECheckBoxState InState);
	//void OnOffsetChanged(const FText& InText, const ETextCommit::Type InTextAction);

	//开始断点改造
	void StartReforming();

	//创建断点改造checkbox
	TSharedPtr<SWidget>  CreateBPReformCheckWidget();
	//获取已选择Obj的构件
	TSharedPtr<class FArmyFurniture> GetFurniture(bool NativeFurniture);
	//原始点位构件
	TSharedPtr<FArmyFurniture> NativeFurniture;

	//获取当前选择的actor的缩略图
	 const FSlateBrush * GetThumbnailBrush() const;

	 /*更新断点改造的UI
	 * @Type 当前选择的obj类型
	*/
	 void UpdateBreakPointUI(EHydropowerDetailType::HydropowerPropertyType Type);
	 //更新断点改造的UI数据
	 void UpdateBreakPointUIData(EHydropowerDetailType::HydropowerPropertyType Type);

	 //当替换按钮按下时发生
	 FReply OnReplaceButtonClicked();

//*****断点改造结束


//****施工项开始

	 //施工项数据解析器
	 TSharedPtr<SArmyMulitCategory> ConstructionData= MakeShareable(new SArmyMulitCategory);

	 //施工项请求代理
	// FArmyHttpRequestCompleteDelegate ConstructionOptionsRequestCompleteDelegate;

	 //刷新施工项
	 void UpdateConstructionOptions(EHydropowerDetailType::HydropowerPropertyType Type);
	
	 //根据Json刷新施工项UI
	 //@ ConstructionJsonData 原始数据
	 //@ CheckedData 勾选信息
	 void UpdateConstructionOptionsUI(enum class EConstructionRequeryResult ResultInfo, TSharedPtr<FJsonObject> ConstructionJsonData, TSharedPtr<class FArmyConstructionItemInterface> CheckedData,struct ObjectConstructionKey Key);

	 //类别的顺序
	 TArray<FName> CatergoryOrder;

	 //根据Json刷新施工项UI
	// void UpdateConstructionOptionsUI(TSharedPtr<FJsonObject> jsondata);

	 //施工项json
	 TArray<TSharedPtr<FJsonObject>> ConstructionJsons;



	//刷新物体详情UI
	 void UpdateGlobalCategoryUI();
	 //刷新物体详情Data
	 void UpdateGlobalCategoryData();
	 //当前选择物品的房间
	 TSharedPtr<FArmyRoom> SelectedRoom;
};

