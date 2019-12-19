#include "SArmyWallSetting.h"
#include "SSPinBox.h"
#include "SBox.h"
#include "SArmyDetailComponent.h"
#include "ArmyGameInstance.h"
#include "ArmyStyle.h"

void SArmyWallSetting::Construct(const FArguments & InArgs)
{
	Init();

	ChildSlot
	[
		SNew(SBox)
		.WidthOverride(480)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(0, 20, 0, 0)
			.AutoHeight()
			.HAlign(HAlign_Left)
			[
				CreateWallHeightWidget()
			]
	/*+ SVerticalBox::Slot()
	.Padding(0, 20, 20, 0)
	.AutoHeight()
	[
		SNew(SBox)
		.WidthOverride(128)
		[
			CreateWallColorWidget()
		]
	]	*/
		]
	];
}

void SArmyWallSetting::OnConfirmClicked()
{
	// TODO:
}

void SArmyWallSetting::Init()
{
	/** 等能获取到存档时再修改*/
	WallHeight = 100.0f;
}

TSharedRef<SWidget> SArmyWallSetting::CreateWallHeightWidget()
{
	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(10, 0, 10, 0)
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.HeightOverride(24)
			.WidthOverride(64)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("墙体高度")))
				.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
				.Justification(ETextJustify::Center)
			]
		]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.HeightOverride(24)
			.WidthOverride(128)
			[
				SNew(SSpinBox<float>)
				.Value(this, &SArmyWallSetting::GetWallHeight)
				.Delta(1.f)
				.MinSliderValue(0.f)
				.MaxSliderValue(10000.f)
				.MinValue(0.0f)
				.MaxValue(10000.f)
				.OnValueChanged(this, &SArmyWallSetting::OnWallHeightChanged)
			]
		];
}

TSharedRef<SWidget> SArmyWallSetting::CreateWallColorWidget()
{
	return SNew(SSpinBox<float>);
}
