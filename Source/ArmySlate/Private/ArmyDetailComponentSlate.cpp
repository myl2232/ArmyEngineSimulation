#include "ArmyDetailComponentSlate.h"
#include "SEditableTextBox.h"
#include "Runtime/Analytics/Analytics/Public/Interfaces/IAnalyticsProvider.h"
#include "SColorPicker.h"
#include "ArmyStyle.h"
#include "SScrollBar.h"
#include "SButton.h"
#include "SCheckBox.h"

void SArmyDetailButton::Construct(const FArguments& InArgs)
{	
	ChildSlot
	[
		SNew(SButton)			
		.OnClicked(InArgs._OnClicked)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.ButtonStyle(InArgs._ButtonStyle)		
		.ContentPadding(FMargin(0, 6, 0, 6))
		.Content()
		[
			SNew(STextBlock)
			.Text(InArgs._Text)
			.TextStyle(FArmyStyle::Get(), "ArmyText_12")
		]
	];
}


void SArmyDetailColorButton::Construct(const FArguments& InArgs)
{
	BorderColor = InArgs._Color;
	OnColorChangedDelegate = InArgs._OnColorChanged;

	ChildSlot
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBox)
			[
				SNew(STextBlock)
				.Text(InArgs._Text)
				.TextStyle(FArmyStyle::Get(), "ArmyText_12")
				.ColorAndOpacity(FLinearColor::White)
			]
		]
		+ SHorizontalBox::Slot()
		.Padding(FMargin(10, 0, 0, 0))
		.FillWidth(1.f)
		[
			SNew(SButton)
			.OnClicked(this, &SArmyDetailColorButton::OnColorClicked)
			.ContentPadding(FMargin(0))
			.Content()
			[
				SNew(SBorder)
				.BorderBackgroundColor(this, &SArmyDetailColorButton::GetBorderColor)
				.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
			]
		]
	];
}

void SArmyDetailColorButton::SetBorderColor(const FLinearColor & color)
{
	BorderColor = color;
}

FReply SArmyDetailColorButton::OnColorClicked()
{
	FColorPickerArgs PickerArgs;
	PickerArgs.InitialColorOverride = BorderColor.Get();
	PickerArgs.InitialColorOverride.A = 1.f;
	PickerArgs.bUseAlpha = false;	
	PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &SArmyDetailColorButton::OnColorChanged);
	OpenColorPicker(PickerArgs);

	return FReply::Handled();
}

void SArmyDetailColorButton::OnColorChanged(const FLinearColor _Color)
{	
	BorderColor = _Color;	
	OnColorChangedDelegate.ExecuteIfBound(_Color);
}

void SArmyDetailEditableTextBox::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.WidthOverride(80)
			.HeightOverride(30)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(InArgs._Text)
				.TextStyle(FArmyStyle::Get(), "ArmyText_12")
				.ColorAndOpacity(FLinearColor::White)
			]
		]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(FMargin(10, 0, 0, 0))
		.FillWidth(1.f)
		[
			SNew(SEditableTextBox)
			.Text(InArgs._EditableText)		
			.OnTextChanged(InArgs._OnTextChanged)
			.OnTextCommitted(InArgs._OnTextCommitted)
		]
	];
}

void SArmyDetailInputValue::Construct(const FArguments& InArgs)
{
	ValueAttribute = InArgs._Value;

	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBox)
			.WidthOverride(80)
			[
				SNew(STextBlock)
				.Text(InArgs._Text)
				.TextStyle(FArmyStyle::Get(), "ArmyText_12")
				.ColorAndOpacity(InArgs._FlagColor)
			]
		]
		+ SHorizontalBox::Slot()
		.Padding(FMargin(10, 0, 0, 0))
		.FillWidth(1.f)
		[
			SAssignNew(MySpinBox, SSpinBox<float>)			
			.Value(this, &SArmyDetailInputValue::OnGetValueForSpinBox)
			.Delta(InArgs._Delta)
			.MinSliderValue(InArgs._MinSliderValue)
			.MaxSliderValue(InArgs._MaxSliderValue)
			.MaxValue(InArgs._MaxValue)
			.MinValue(InArgs._MinValue)
			.OnValueChanged(InArgs._OnValueChanged)
			.OnValueCommitted(InArgs._OnValueCommitted)
			.TypeInterface(InArgs._TypeInterface)		
		]
	];

	MySpinBox->SetForegroundColor(FLinearColor::White);
}

void SArmyDetailCheckBox::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBox)
			.WidthOverride(80)
			[
				SNew(STextBlock)
				.Text(InArgs._Text)
				.TextStyle(FArmyStyle::Get(), "ArmyText_12")
				.ColorAndOpacity(FLinearColor::White)
			]
		]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.Padding(FMargin(10, 0, 0, 0))
		.FillWidth(1.f)
		[
			SAssignNew(MyCheckBox, SCheckBox)			
			.IsChecked(InArgs._IsChecked)
			.OnCheckStateChanged(InArgs._OnCheckStateChanged)
		]
	];
}


void SArmyDetailWidget::Construct(const FArguments & InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.Padding(FMargin(0, 10, 0, 0))
		.AutoHeight()
		.HAlign(HAlign_Center)
		[
			SNew(STextBlock)
			.Text(InArgs._TitleText)
			.TextStyle(FArmyStyle::Get(), "ArmyText_12")
			.Visibility(InArgs._TitleText.ToString() == "" ? EVisibility::Collapsed : EVisibility::SelfHitTestInvisible)
		]
		+ SVerticalBox::Slot()
		.Padding(FMargin(0, 10, 0, 0))
		[
			SAssignNew(Container, SScrollBox)
		]
	];
}

void SArmyDetailWidget::AddDetailItem(TSharedRef<SWidget> _ItemWidget)
{
	Container->AddSlot()
	.Padding(FMargin(0, 10, 0, 0))
	[
		_ItemWidget
	];
}
