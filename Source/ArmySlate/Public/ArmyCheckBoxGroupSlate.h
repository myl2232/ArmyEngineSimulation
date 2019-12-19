#pragma once

#include "CoreMinimal.h"
#include "SlateBasics.h"
#include "Widgets/SCompoundWidget.h"
/**
* 单选，复选框组合
*/
class ARMYSLATE_API SArmyCheckBoxGroup : public SCompoundWidget
{

public:
	DECLARE_DELEGATE_TwoParams(FCheckStateInt32Delegate, ECheckBoxState, int32);

	struct FCheckBoxData
	{
		FCheckBoxData(FString _CheckBoxName, ECheckBoxState _CheckBoxState = ECheckBoxState::Unchecked)
			: CheckBoxName(_CheckBoxName)
			, CheckBoxState(_CheckBoxState)
		{}
		~FCheckBoxData() {}
		FString CheckBoxName;
		ECheckBoxState CheckBoxState;
	};

	SLATE_BEGIN_ARGS(SArmyCheckBoxGroup) 
		: _bAutoWrap(true)
		, _bMultiCheck(false)
	{}
		//是否复选
		SLATE_ARGUMENT(bool, bMultiCheck)
		//CheckBox组合是否自动换行
		SLATE_ARGUMENT(bool, bAutoWrap)
		//状态变化代理
		SLATE_EVENT(FCheckStateInt32Delegate, OnCheckStateChanged)
		//CheckBox的名称列表
		SLATE_ARGUMENT(TArray<FCheckBoxData>*, ItemSource)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	ECheckBoxState IsItemChecked(int32 _ItemIndex) const;
	void OnCheckStateChanged(ECheckBoxState _NewState, int32 _ItemIndex);

	FCheckStateInt32Delegate Delegate_CheckStateChanged;
	bool bMultiCheck;
	TSharedPtr<SWrapBox> WB_Container;
	TArray<TSharedPtr<SCheckBox>> MyCheckBoxes;
	TArray<FCheckBoxData>* ItemSource;
};