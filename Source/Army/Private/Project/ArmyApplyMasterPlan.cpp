#include "ArmyApplyMasterPlan.h"
#include "SArmyApplyMasterPlan.h"
#include "ArmyGameInstance.h"
#include "SArmyPreviewMasterPlan.h"


FArmyApplyMasterPlan::FArmyApplyMasterPlan()   
{
}

bool FArmyApplyMasterPlan::ApplyMasterPlan()
{

	ApplyMasterPlanWidget = SNew(SArmyApplyMasterPlan)
	.OnClickDelegate(this, &FArmyApplyMasterPlan::OnApplyWidgetClick);
	GGI->Window->PresentModalDialog(

		ApplyMasterPlanWidget->AsShared());

	return true;
}



void FArmyApplyMasterPlan::PreviewMasterPlan()
{
	PreviewMasterPlanWidget = SNew(SArmyPreviewMasterPlan)
		.OnClickDelegate(this, &FArmyApplyMasterPlan::OnPreviewWidgetClick);
	GGI->Window->PresentModalDialog(

		PreviewMasterPlanWidget->AsShared());
}

void FArmyApplyMasterPlan::OnApplyWidgetClick(int32 inN)
{
	if (inN == 1)
	{

	}
	GGI->Window->DismissModalDialog();
}

void FArmyApplyMasterPlan::OnPreviewWidgetClick(int32 inN)
{
	if (inN == 1)
	{

	}
	GGI->Window->DismissModalDialog();
}

