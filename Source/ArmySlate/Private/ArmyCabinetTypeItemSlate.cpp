#include "ArmyCabinetTypeItemSlate.h"
#include "SBoxPanel.h"
#include "SBox.h"
#include "ArmyScrollWrapBoxSlate.h"
#include "STextBlock.h"
#include "ArmyStyle.h"
#include "ArmyCabinetItemSlate.h"

void SArmyCabinetTypeItem::Construct(const FArguments& InArgs)
{
	ItemCode = InArgs._ItemCode;

	TSharedPtr<SWidget> OutWidget = NULL;

	OutWidget =
	SNew(SBox)
	.WidthOverride(InArgs._ItemWidth)
	.HeightOverride(InArgs._ItemHeight)
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(FMargin(16.f,4.f))
		[
			SAssignNew(CabinetTypeText,STextBlock)
			.Text(FText::FromString(TEXT("请选择烟机类型（单选）")))
		    .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		    .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FF969799"))
		]
	    +SVerticalBox::Slot()
		.Padding(16.f,8.f,0.f,24.f)
		.FillHeight(1.0)
		[
			SAssignNew(CabinetTypeListWidget,SScrollWrapBox).InnerSlotPadding(FVector2D(8.f,8.f))
		]
	];

	ChildSlot[OutWidget.ToSharedRef()];
}

void SArmyCabinetTypeItem::AddItem(TSharedPtr<SWidget> ItemWidget)
{
	CabinetTypeListWidget->AddItem(ItemWidget);
}