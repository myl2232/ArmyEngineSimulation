#pragma once

#include "CoreMinimal.h"
#include "SlateBasics.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SComboBox.h"
#include "Engine/Texture.h"
#include "ArmyTypes.h"
#include "ArmyComboBoxItemSlate.h"

DECLARE_DELEGATE_OneParam(FOnComboBoxSelectionChanged, const FString&);

class ARMYSLATE_API SArmyPropertyComboBox : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyPropertyComboBox)
        : _Width(114)
        , _Height(24)
		, _BoxImagePadding(FMargin(16, 0, 8, 0))
    {}

    /** ��ʾ������Դ */
    SLATE_ARGUMENT(TSharedPtr<FArmyComboBoxArray>, OptionsSource)

    SLATE_ARGUMENT(int32, Width)

    SLATE_ARGUMENT(int32, Height)

	//@ 下拉列表子项水平排列方式
	SLATE_ATTRIBUTE(EHorizontalAlignment, BoxItemHorizontalAlignment)

    /** ��ʾ���������� */
    SLATE_ATTRIBUTE(FText, Value)

	//下拉列表左侧图片padding
	SLATE_ARGUMENT(FMargin, BoxImagePadding)

    /** ѡ����仯�ص� */
    SLATE_EVENT(FInt32StringDelegate, OnSelectionChanged)

    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    /** ����ѡ���� */
    void SetSelectedItem(TSharedPtr<FArmyKeyValue> InItem);

    /** ˢ�������б����� */
    void RefreshOptions();

private:
    TSharedRef<SWidget> OnGenerateWidget(TSharedPtr<FArmyKeyValue> InItem);

    void OnSelectionChanged_Internal(TSharedPtr<FArmyKeyValue> NewSelection, ESelectInfo::Type SelectInfo);

    /** ���� ������ �������� */
    void OnComboBoxOpen();

    /** ���� ������ �������ر� */
    void OnComboBoxClose();

private:
    TSharedPtr< SComboBox< TSharedPtr<FArmyKeyValue> > > ComboBoxWidget;

    TSharedPtr<FArmyComboBoxArray> OptionsSource;

    /** @������ UI�б� */
    TArray< TSharedPtr<SArmyComboboxItem> > ComboboxUIList;

    TSharedPtr<SImage> ArrowImage;

    TSharedPtr<FArmyKeyValue> SelectedItem;

	/**@��ҵ˳ ����ѡ�����ݵ��ı���*/
	TSharedPtr<STextBlock> ValueTextBlock;

    FInt32StringDelegate OnSelectionChanged;

	//@ 下拉列表子项水平排列方式
	EHorizontalAlignment BoxItemHorizontalAlignment;
	TAttribute<FMargin> BoxImagePadding;//下拉列表左侧图片padding

};

class ARMYSLATE_API SArmyTextPulldownBox : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArmyTextPulldownBox){}
	SLATE_ARGUMENT(TArray< TSharedPtr<FString> >, ComboItemList)
	SLATE_ARGUMENT(FString, DisplayString)	
	SLATE_EVENT(FOnComboBoxSelectionChanged, OnComboBoxSelectionChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);	

	void SetDisplayContent(FString content);

	void OnComboboxOpen();//���� ������ ��������
	void OnComboboxClose();//���� ������ �������ر�

	FText GetDisplayContent()const;
private:
	void OnComboBoxSelectionChangedEvent(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);
	TSharedRef<SWidget> OnComboBoxGenerateWidgetEvent(TSharedPtr<FString> InItem);

	TArray< TSharedPtr<FString> > ComboItemList;
	FString DisplayString;
	TSharedPtr< SComboBox< TSharedPtr<FString> > > MyComboBox;
	TSharedPtr<STextBlock> NumText;
	TSharedPtr<SImage> ArrowImage;
	FOnComboBoxSelectionChanged OnComboBoxSelectionChanged;	
};