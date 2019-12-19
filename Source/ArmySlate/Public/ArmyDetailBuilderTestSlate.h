//#pragma once
//
//#include "CoreMinimal.h"
//#include "SlateBasics.h"
//#include "Widgets/SCompoundWidget.h"
//#include "SArmyCheckBoxGroup.h"
//#include "SArmyComboBox.h"
//
//class ARMYSLATE_API SArmyDetailBuilderTest : public SCompoundWidget
//{
//public:
//	SLATE_BEGIN_ARGS(SArmyDetailBuilderTest) {}
//	SLATE_END_ARGS()
//
//	void Construct(const FArguments& InArgs);
//
//	TSharedPtr<SWidget> CreateTestDetailWidget();
//
//private:
//	//可调节数值框(SpinBox)
//	TSharedRef<SWidget> CreateActorTranslateXDetail();
//	TSharedRef<SWidget> CreateActorTranslateYDetail();
//	TSharedRef<SWidget> CreateActorTranslateZDetail();
//	//常规按钮
//	TSharedRef<SWidget> CreateTextButtonDetail();
//	//颜色拾取按钮
//	TSharedRef<SWidget> CreateColorButtonDetail();
//	//勾选，单选，复选项
//	TSharedRef<SWidget> CreateCheckBoxDetail();
//	TSharedRef<SWidget> CreateSingleCheckBoxDetail();
//	TSharedRef<SWidget> CreateMultiCheckBoxDetail();
//	//文字下拉列表
//	TSharedRef<SWidget> CreateTextComboBoxDetail();
//	//图片下拉列表
//	TSharedRef<SWidget> CreateTextureComboBoxDetail();
//	//可编辑文字框
//	TSharedRef<SWidget> CreateEditableTextDetail();
//
//private:
//	//物体位置设置
//	void OnActorLocationXChanged(float InValue) { CachedLocationX = InValue; }
//	void OnActorLocationYChanged(float InValue) { CachedLocationY = InValue; }
//	void OnActorLocationZChanged(float InValue) { CachedLocationZ = InValue; }
//	float GetLocationX() const { return CachedLocationX; }
//	float GetLocationY() const { return CachedLocationY; }
//	float GetLocationZ() const { return CachedLocationZ; }
//	float CachedLocationX;
//	float CachedLocationY;
//	float CachedLocationZ;
//	//文字按钮
//	FReply OnButtonClicked() { return FReply::Handled(); }
//	//颜色拾取按钮
//	FReply OnColorButtonClicked();
//	void OnPickColorChanged(FLinearColor InColor) { InColor.A = 1; CachedPickColor = InColor; }
//	FLinearColor GetPickColor() const { return CachedPickColor; }
//	FSlateColor GetPickBorderColor() const { return FSlateColor(CachedPickColor); }
//	FLinearColor CachedPickColor;
//	//勾选框
//	void OnCheckBoxStateChanged(ECheckBoxState InNewState) { CachedCheckBoxState = InNewState; }
//	ECheckBoxState GetCheckBoxState() const { return CachedCheckBoxState; }
//	ECheckBoxState CachedCheckBoxState;
//	//单选框
//	void OnSingleCheckBoxStateChanged(ECheckBoxState InNewState, int32 InIndex) { }
//	TArray<SArmyCheckBoxGroup::FCheckBoxData> SingleCheckBoxData;
//	//复选框
//	void OnMultiCheckBoxStateChanged(ECheckBoxState InNewState, int32 InIndex) { }
//	TArray<SArmyCheckBoxGroup::FCheckBoxData> MultiCheckBoxData;
//	//文字下拉列表
//	void OnTextComboTextChanged(const FString& InString);
//	TArray< TSharedPtr< FString > > TextComboBoxSource;
//	TSharedPtr<SArmyTextComboBox> TextComboBoxWidget;
//	//图片下拉列表
//	void OnTextureComboTextChanged(UTexture* InTexture);
//	TArray< UTexture* > TextureComboBoxSource;
//	TSharedPtr<SArmyTextureComboBox> TextureComboBoxWidget;
//	//文字输入框
//	void OnEditableTextChanged(const FText& InText) { CachedEditableText = InText; }
//	FText GetEditableText() const { return CachedEditableText; }
//	FText CachedEditableText;
//
//
//	//左边CheckBox，右边SpinBox
//	void OnCheckBox1StateChanged(ECheckBoxState InNewState) { CachedCheckBox1State = InNewState; }
//	ECheckBoxState GetCheckBox1State() const { return CachedCheckBox1State; }
//	ECheckBoxState CachedCheckBox1State;
//
//	void OnSpinBoxValueChanged(float InValue) { CachedSpinBoxValue = InValue; }
//	float GetSpinBoxValue() const { return CachedSpinBoxValue; }
//	float CachedSpinBoxValue;
//
//	bool GetSpinBoxValueEnabled() const { return CachedCheckBox1State == ECheckBoxState::Checked; }
//
//private:
//	TSharedPtr<class FArmyDetailBuilder> DetailBuilder;
//
//};