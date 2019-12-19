#include "ArmyCheckBoxGroupSlate.h"
#include "ArmyStyle.h"

void SArmyCheckBoxGroup::Construct(const FArguments& InArgs)
{
	ItemSource = InArgs._ItemSource;
	bMultiCheck = InArgs._bMultiCheck;
	Delegate_CheckStateChanged = InArgs._OnCheckStateChanged;

	ChildSlot
		.HAlign(HAlign_Fill)
		[
			SAssignNew(WB_Container, SWrapBox)
			.UseAllottedWidth(InArgs._bAutoWrap)
		];

	//单选框只要有一个默认选中状态，如果没有，强制第一个默认选中
	if (!bMultiCheck)
	{
		bool bHasDefault = false;
		for (auto& It : (*ItemSource))
		{
			bHasDefault = It.CheckBoxState == ECheckBoxState::Checked;
		}
		if (!bHasDefault && (*ItemSource).IsValidIndex(0))
		{
			(*ItemSource)[0].CheckBoxState = ECheckBoxState::Checked;
		}
	}

	//根据ItemSource创建，左边CheckBox，右边Text
	for (int32 i = 0; i<(*ItemSource).Num(); i++)
	{
		TSharedPtr<SCheckBox> NewCheckBox = SNew(SCheckBox)
			.OnCheckStateChanged(this, &SArmyCheckBoxGroup::OnCheckStateChanged, i)
			.IsChecked(this, &SArmyCheckBoxGroup::IsItemChecked, i);
		MyCheckBoxes.Add(NewCheckBox);

		WB_Container->AddSlot()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					NewCheckBox.ToSharedRef()
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				[
					SNew(STextBlock)
					.Text(FText::FromString((*ItemSource)[i].CheckBoxName))
					.TextStyle(FArmyStyle::Get(), "ArmyText_10")
					.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 1)))
				]
			];
	}
}

ECheckBoxState SArmyCheckBoxGroup::IsItemChecked(int32 _ItemIndex) const
{
	if ((*ItemSource).IsValidIndex(_ItemIndex))
		return (*ItemSource)[_ItemIndex].CheckBoxState;
	else
		return ECheckBoxState::Unchecked;
}

void SArmyCheckBoxGroup::OnCheckStateChanged(ECheckBoxState _NewState, int32 _ItemIndex)
{
	if (bMultiCheck)
	{
		if ((*ItemSource).IsValidIndex(_ItemIndex))
		{
			(*ItemSource)[_ItemIndex].CheckBoxState = _NewState;
			Delegate_CheckStateChanged.ExecuteIfBound(_NewState, _ItemIndex);
		}
	}
	else
	{
		if ((*ItemSource).IsValidIndex(_ItemIndex))
		{
			if (_NewState == ECheckBoxState::Checked)
			{
				for (auto& It : (*ItemSource))
				{
					It.CheckBoxState = ECheckBoxState::Unchecked;
				}
				(*ItemSource)[_ItemIndex].CheckBoxState = _NewState;
				Delegate_CheckStateChanged.ExecuteIfBound(_NewState, _ItemIndex);
			}
			else
			{
				(*ItemSource)[_ItemIndex].CheckBoxState = ECheckBoxState::Checked;
			}
		}
	}
}
