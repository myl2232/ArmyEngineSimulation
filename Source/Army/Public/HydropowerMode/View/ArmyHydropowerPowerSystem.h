/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRHydropowerPowerSystem.h
 * @Description 水电模式配电系统
 *
 * @Author 飞舞轻扬
 * @Date 2018年11月5日
 * @Version 1.0
 */

#pragma once
#include "CoreMinimal.h"
#include "DelegateCombinations.h"
#include "Widgets/SCompoundWidget.h"
#include "ArmyTypes.h"
#include "SComboBox.h"
#include "SCheckBox.h"
struct FPowerSystemLoopSample;
struct FPowerSystemLoopData;
struct FHydropowerPowerSytem;
struct FPowerSystemLoop;
struct FArmyObjectIDValue
{
	FGuid ObjectID;
	FString Value;
	int32 SpaceId;
	
	FArmyObjectIDValue(FGuid InObjectID, FString InValue, int32 InSpaceId) :
		ObjectID(InObjectID),
		Value(InValue),
		SpaceId(InSpaceId)
	{

	}
};


DECLARE_DELEGATE_RetVal_OneParam(FArmyComboBoxArray&,FOnXRKeyValueDelegate,int32)
DECLARE_DELEGATE_OneParam(FSavePowerSystemDelegate,TSharedPtr<FHydropowerPowerSytem>&)
DECLARE_DELEGATE(FSampleDelegate)

enum FLoopOpType
{
	LOT_Manual = 0,/*手工生成的回路*/
	LOT_Auto,/*自动生成的回路*/
};

struct FHydropowerPowerSytemItem
{
	int32 LoopID; // 回路id
	int32 LoopType;//回路类型
	int32 BreakID; //断路器id
	int32 PipeID; // 管线商品
	FString LoopName;// 回路名称
	FString BreakName; //断路器名称
	FString PipeName;// 管线商品名称
	int32 bManualOrAuto;// 标识自动生成的或者手工生成的回路
	TArray<TSharedPtr<FArmyObjectIDValue>> RoomArray; // 房间信息
	FHydropowerPowerSytemItem(int32 InLoopID,int32 InLoopType,int32 InBreakID, int32 InPipeID)
		: LoopID(InLoopID),
		LoopType(InLoopType),
		BreakID(InBreakID),
		PipeID(InPipeID),
		bManualOrAuto(LOT_Manual)
	{

	}
	FHydropowerPowerSytemItem()
		:bManualOrAuto(LOT_Manual)
	{

	}

	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR,TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) ;
	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) ;
};

struct FHydropowerPowerSytem
{
	int32 MainCableID ;
	int32 MainBreakID;
	FString MainBreakName;
	FString MainCableName;
	TArray<TSharedPtr<FHydropowerPowerSytemItem>> LoopArray;
	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR,TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) ;
	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) ;
	void reset()
	{
		MainBreakID = 0;
		MainCableName = TEXT("");
		MainBreakID = 0 ;
		MainBreakName = TEXT("");
		LoopArray.Empty();
	}

	void operator=(const TSharedPtr<FHydropowerPowerSytem>& OtherPowerSystem)
	{
		MainBreakID = OtherPowerSystem->MainBreakID;
		MainCableID = OtherPowerSystem->MainCableID;
		MainCableName = OtherPowerSystem->MainCableName;
		MainBreakName = OtherPowerSystem->MainBreakName;
		LoopArray.Empty();
		LoopArray.Append(OtherPowerSystem->LoopArray);
	}
};

/**配电箱系统列表List*/
class SPowerSystemItem : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPowerSystemItem)
		:_bNew(true)
	{}
	SLATE_ARGUMENT(bool,bNew)
	SLATE_ATTRIBUTE(FText,LoopName)
	SLATE_ARGUMENT(int32,LoopIndex)
	SLATE_ARGUMENT(int32,LoopType)
	SLATE_ARGUMENT(int32, bManualOrAuto)
	SLATE_ATTRIBUTE(TArray<TSharedPtr<FArmyObjectIDValue>>,SpaceNameList)
	SLATE_ATTRIBUTE(FText,SpaceNames)
	SLATE_ATTRIBUTE(TSharedPtr<FArmyKeyValue>,CircuitBreakerName)
	SLATE_ATTRIBUTE(TSharedPtr<FArmyKeyValue>,PipeLineValue)
	SLATE_ATTRIBUTE(ECheckBoxState,IsSpaceNameChecked)
	SLATE_EVENT(FOnCheckStateChanged,OnCheckStateSpaceNameChanged)
	SLATE_EVENT(FOnClicked,DeletePowerItem) // 删除Item
	SLATE_ARGUMENT(int32 ,SpaceMultiSelectType);

	SLATE_ATTRIBUTE(FArmyComboBoxArray,CircuitBreakerList)
	SLATE_ATTRIBUTE(FArmyComboBoxArray,PipeProductList)
	SLATE_ATTRIBUTE(TArray<TSharedPtr<FArmyObjectIDValue>>,SelectSpaceNameList);
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled )const override;
	virtual void Tick(const FGeometry& AllottedGeometry,const double InCurrentTime,const float InDeltaTime) override;
	bool IsLoopIndex(int32 InIndex){return InIndex == LoopIndex;}


	void OnSlectedSpaceNameChanged(TSharedPtr<FArmyObjectIDValue> NewLoopItem,ESelectInfo::Type SelectInfo);

	ECheckBoxState IsSpaceNameSelected(TSharedPtr<FArmyObjectIDValue> InItem) const;
	void OnCheckStateSpaceNameChanged(ECheckBoxState InCheck,TSharedPtr<FArmyObjectIDValue> InItem);

	void GetPowerSystemItem(TSharedPtr<FHydropowerPowerSytemItem>& OutItem);
	//判断数据完整性
	bool IsPowerSystemItemIntegrality();
	int32 GetLoopType(){ return LoopType; }
	int32 GetLoopIndex(){return LoopIndex;}
	int32 GetBManualOrAuto() { return bManualOrAuto; }
	TArray<TSharedPtr<FArmyObjectIDValue>>& GetSelectSpaceNameList(){ return SelectSpaceNameList; }
	//闪亮亮
	void Shine();
private:
	/**房间下拉菜单*/
	//void (TSharedPtr<FArmyKeyValue> NewLoopItem,ESelectInfo::Type SelectInfo);
	TSharedRef<SWidget> GenerateSpace(TSharedPtr<FArmyObjectIDValue> InItem);
	FText GetSpaceName() const;
	FSlateColor GetSpaceNameColor(TSharedPtr<FArmyObjectIDValue> InItem)const;
	void UpdateSpaceNameList();

	//刷新已选择的房间列表,剔除不存在的房间，确认房间的名字
	void UpdateSelectedSpacemList();

	/**断路器*/
	TSharedRef<SWidget> OnGenerateCircuitBreakerComoboWidget(TSharedPtr<FArmyKeyValue> InItem);
	void OnSlectedCircuitBreakerProjectChanged(TSharedPtr<FArmyKeyValue> NewLoopItem,ESelectInfo::Type SelectInfo);
	FText GetSelectedCircuitBreaker()const;
	FSlateColor GetCircuitBreakerColor(FText InText)const;

	//闪亮
	FSlateColor GetPowerLineColor( )const;

	/**电线，穿线管*/
	TSharedRef<SWidget> OnGeneratePipeLineComoboWidget(TSharedPtr<FArmyKeyValue> InItem);
	void OnSlectedPipeLineProjectChanged(TSharedPtr<FArmyKeyValue> NewLoopItem,ESelectInfo::Type SelectInfo);
	FText GetSelectedPipeLine()const;
	FSlateColor GetPipeLineColor(FText InText)const;

	

	EActiveTimerReturnType TriggerCategoriesRefresh(double InCurrentTime,float InDeltaTime);
private:
	TSharedPtr<SComboBox< TSharedPtr<FArmyObjectIDValue> > > SpaceNameCombox;
	TSharedPtr<FArmyKeyValue> CircuitBreakerName;
	TSharedPtr<FArmyKeyValue> PipeLineValue ;
	TAttribute<TArray<TSharedPtr<FArmyObjectIDValue>>> SpaceNameList;
	TArray<TSharedPtr<FArmyObjectIDValue>> SelectSpaceNameList;
	TArray<TSharedPtr<FArmyObjectIDValue>> CurrentSpaceNameList;
	/** 断路器数据列表 */
	FArmyComboBoxArray CircuitBreakerList;
	/**电线，穿线管列表*/
	FArmyComboBoxArray PipeProductList;
	FText LoopName;
	int32 LoopIndex;
	int32 LoopType;
	int32 bManualOrAuto;
	int32 DeltaTime;
	bool bNew;
	int32 SpaceMultiSelectType; // 1.多选 2.单选
};


class SArmyHydropowerPowerSystem : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS (SArmyHydropowerPowerSystem)
	{
	}

	SLATE_ATTRIBUTE(TSharedPtr<FArmyKeyValue>,HouseholdCableName)

	SLATE_ATTRIBUTE(FArmyComboBoxArray,HouseholdCableList)

	SLATE_ATTRIBUTE(TArray<TSharedPtr<FPowerSystemLoop>>,LoopList)

	SLATE_ATTRIBUTE(TArray<TSharedPtr<struct FArmyObjectIDValue>>,SpaceNameList)

	SLATE_EVENT(FOnXRKeyValueDelegate,LoopDelegate)

	SLATE_ATTRIBUTE(FArmyComboBoxArray,CircuitBreakerList)

	SLATE_EVENT(FSavePowerSystemDelegate,OnSave)

	SLATE_EVENT(FSampleDelegate,OnCannel)

	SLATE_END_ARGS ()
public:
	void Construct(const FArguments &InArgs);

	void LoadPowerSystem(TSharedPtr<FHydropowerPowerSytem>& InPowerSystem);
	void SetHouseholdCableList(FArmyComboBoxArray& InHouseholdCableList)
	{
		HouseholdCableList = InHouseholdCableList;
	}

	void SetCircuitBreakerList(FArmyComboBoxArray& InCircuitBreakerList)
	{
		CircuitBreakerList = InCircuitBreakerList;
	}

	void SetLoopList(TArray<TSharedPtr<FPowerSystemLoop>>& InLoopList)
	{
		LoopList = InLoopList;
	}
	void SetSpaceNameList(TArray<TSharedPtr<struct FArmyObjectIDValue>>& InSpaceNameList)
	{
		SpaceNameList = InSpaceNameList;
	}
private:
	/**创建添加按钮*/
	TSharedRef<SWidget> CreateAddButton();

	/**创建自动生成回路按钮*/
	TSharedRef<SWidget> CreateAutoAddButton();

	/**创建标题*/
	TSharedRef<SWidget> CreateTitleWidget();

	/**创建二级目录*/
	TSharedRef<SWidget> CreateSecondWidget();

	/**创建左侧列表*/
	TSharedRef<SWidget> CreateLeftWidget();

	/**创建确认取消按钮*/
	TSharedRef<SWidget> CreateSaveAndCannelWidget();

	/**创建进户电缆UI*/
	TSharedRef<SWidget> CreateHouseholdCableComboWidget();

	/**创建断路器UI*/
	TSharedRef<SWidget> CreateCircuitBreakerComboWidget();

	/**创建配电信息列表*/
	TSharedRef<SWidget> CreatePowerSystemListWidget();
private:
	/**添加新的回路*/
	void AddNewLoop(TSharedPtr<FPowerSystemLoop> NewLoopItem,ESelectInfo::Type SelectInfo);
	TSharedRef<SWidget> GenerateLoopItem(TSharedPtr<FPowerSystemLoop> InItem);

	void NewLoop(TSharedPtr<FPowerSystemLoop> NewLoopItem);

	void GetNewLoopIndex(int32 LoopType,int32& LoopIndex,int32& AllLoopIndex);

	/**自动生成回路*/
	FReply AutoAddNewLoop();
	void AutoNewLoops(TSharedPtr<FPowerSystemLoopSample> LoopSample, TArray<TArray<TSharedPtr<FArmyObjectIDValue>>>& RoomArrArr, TArray<TSharedPtr<FPowerSystemLoopData>>& LoopSampleArr);

	//删除回路
	FReply OnDeleteLoopItem(int32 Index);

	/**进户电缆*/
	TSharedRef<SWidget> OnGenerateHouseholdCableComboWidget(TSharedPtr<FArmyKeyValue> InItem);
	void OnSlectedHouseholdCableChanged(TSharedPtr<FArmyKeyValue> NewLoopItem,ESelectInfo::Type SelectInfo);
	FText GetSelectedHouseholdCable()const;
	FSlateColor GetHouseHoldColor(FText InText)const;


	/**断路器*/
	TSharedRef<SWidget> OnGenerateCircuitBreakerComoboWidget(TSharedPtr<FArmyKeyValue> InItem);
	void OnSlectedCircuitBreakerProjectChanged(TSharedPtr<FArmyKeyValue> NewLoopItem,ESelectInfo::Type SelectInfo);
	FText GetSelectedCircuitBreaker()const;
	FSlateColor GetCircuitBreakerColor(FText InText)const;


	FReply OnSavePowerSystem();
	FReply OnCancelButtonClicked();

	TArray<TSharedPtr<struct FArmyObjectIDValue>> GetSpaceNameList(TSharedPtr<FPowerSystemLoop> NewLoopItem,int32 InIndex) const;
private:
	TSharedPtr<SComboBox<TSharedPtr<FArmyKeyValue>>> HouseholdCableComboBox;
	TSharedPtr<SComboBox<TSharedPtr<FArmyKeyValue>>> CircuitBreakerComboBox;
	TSharedPtr<SListView< TSharedPtr<FHydropowerPowerSytemItem> >> ListView;
	TSharedPtr<class SScrollBox> PowerSystemListView;
	TSharedPtr<SComboBox< TSharedPtr<FPowerSystemLoop>>> AddLoopButton;

	TArray<TSharedPtr<SPowerSystemItem>> PowerSystemList;

	TSharedPtr<FArmyKeyValue> HouseholdCableName;
	TSharedPtr<FArmyKeyValue>  CircuitBreakerName;
	/** 进户电缆数据列表 */
	FArmyComboBoxArray HouseholdCableList;
	/** 断路器数据列表 */
	FArmyComboBoxArray CircuitBreakerList;
	/**回路类型数据列表*/
	TArray<TSharedPtr<FPowerSystemLoop>> LoopList;
	/**房间名称数据列表*/
	TArray<TSharedPtr<struct FArmyObjectIDValue>> SpaceNameList;

	TSharedPtr<FHydropowerPowerSytem> PowerSystem;
public:
	FOnXRKeyValueDelegate LoopDelegate; // 新增回路
	FSavePowerSystemDelegate OnSave;
	FSampleDelegate OnCannel;
};