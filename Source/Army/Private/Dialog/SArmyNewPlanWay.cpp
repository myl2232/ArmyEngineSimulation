#include "SArmyNewPlanWay.h"
#include "SBoxPanel.h"
#include "SArmyWayButton.h"
#include "ArmyGameInstance.h"
#include "ArmyHomeModeController.h"

void SArmyNewPlanWay::Construct(const FArguments & InArgs)
{	
	ChildSlot
	[
		SNew(SBox)
		.WidthOverride(516)
		.HeightOverride(336)
		.Padding(FMargin(80))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			//.Padding(50, 20, 20, 10)
			[
				SNew(SArmyWayButton)
				.Width(168)
				.Height(176)
				.ButtonText(FText::FromString(TEXT("新建户型")))
				.ButtonImage(FArmyStyle::Get().GetBrush("NewPlanWay.New"))
				.OnDelegate_Clicked(this, &SArmyNewPlanWay::OnNewClicked)
			]
			+ SHorizontalBox::Slot()
			.Padding(20, 0, 0, 0)
			[
				SNew(SArmyWayButton)
				.Width(168)
				.Height(176)
				.ButtonText(FText::FromString(TEXT("导入底图")))	
				.ButtonImage(FArmyStyle::Get().GetBrush("NewPlanWay.Import"))
				.OnDelegate_Clicked(this, &SArmyNewPlanWay::OnImportClicked)
			]
		]
	];
}

void SArmyNewPlanWay::OnNewClicked()
{	
	if (GGI) {
		TSharedPtr<FArmyHomeModeController> TempController = StaticCastSharedPtr<FArmyHomeModeController>(GGI->DesignEditor->CurrentController);
		GGI->Window->DismissModalDialog();
		TempController->OnInternalWallCommand();
	}
}

void SArmyNewPlanWay::OnImportClicked()
{
	//if (GGI) {
	//	TSharedPtr<FArmyHomeModeController> TempController = StaticCastSharedPtr<FArmyHomeModeController>(GGI->DesignEditor->CurrentController);
	//	GGI->Window->DismissModalDialog();
	//	TempController->OnImportFacsimile();		
	//}
}
