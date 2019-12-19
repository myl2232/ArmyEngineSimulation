#include "ArmyRulerLine.h"
#include "ArmyViewportClient.h"

FArmyRulerLine::FArmyRulerLine()
	: LeftDashLine(MakeShareable(new FArmyLine()))
	, RightDashLine(MakeShareable(new FArmyLine()))
	, UpDashLine(MakeShareable(new FArmyLine()))
{
	LeftDashLine->bIsDashLine = true;
	LeftDashLine->SetLineColor(FLinearColor::Gray);
	RightDashLine->bIsDashLine = true;
	RightDashLine->SetLineColor(FLinearColor::Gray);
	UpDashLine->bIsDashLine = true;
	UpDashLine->SetLineColor(FLinearColor::Gray);

	GVC->ViewportOverlayWidget->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(InputBoxWidget, SArmyInputBox)
			.OnInputBoxCommitted(Delegate_OnTextCommitted)
			.Visibility(EVisibility::Hidden)
		];
}

FArmyRulerLine::~FArmyRulerLine()
{
    // @欧石楠 析构时需要将输入框从视口中移除掉
    if (InputBoxWidget.IsValid() && GVC && GVC->ViewportOverlayWidget.IsValid())
    {
        GVC->ViewportOverlayWidget->RemoveSlot(InputBoxWidget->AsShared());
        InputBoxWidget = nullptr;
    }
}

void FArmyRulerLine::Update(FVector Begin, FVector End, FVector LeftOffset, FVector RightOffset,bool bIgnoreZ)
{
	if (RightOffset == FVector::ZeroVector)
	{
		RightOffset = LeftOffset;
	}

	DashLineUpDir = LeftOffset.GetSafeNormal();

	LeftDashLine->SetStart(Begin);
	LeftDashLine->SetEnd(Begin + LeftOffset);

	RightDashLine->SetStart(End);
	RightDashLine->SetEnd(End + RightOffset);

	UpDashLine->SetStart(LeftDashLine->GetEnd());
	UpDashLine->SetEnd(RightDashLine->GetEnd());

	FVector2D ViewportSize;
	GVC->GetViewportSize(ViewportSize);
	FVector LineCenter;
	FVector2D InputBoxPos;

	//控件本身的输入框
	bUseLRLine ? InputBoxWidget->SetStartAndEnd((Begin + End) / 2.f, (UpDashLine->GetStart() + UpDashLine->GetEnd()) / 2.f, bIgnoreZ) : InputBoxWidget->SetStartAndEnd(UpDashLine->GetStart(), UpDashLine->GetEnd(),bIgnoreZ);
	LineCenter = bUseLRLine ? (Begin + End + LeftOffset) / 2.f : (UpDashLine->GetStart() + UpDashLine->GetEnd()) / 2.f;
	GVC->WorldToPixel(LineCenter, InputBoxPos);
	InputBoxPos -= ViewportSize / 2;
	InputBoxWidget->SetPos(InputBoxPos);
	if (!bUpdateShowInDraw)
	{
		InputBoxWidget->Show(true);
	}
}

void FArmyRulerLine::Draw(FPrimitiveDrawInterface * PDI, const FSceneView * View)
{
	LeftDashLine->Draw(PDI, View);
	RightDashLine->Draw(PDI, View);
	UpDashLine->Draw(PDI, View);
	if (bUpdateShowInDraw)
	{
		InputBoxWidget->Show(true);
	}	
}

void FArmyRulerLine::ShowInputBox(bool bShow)
{
	if ((InputBoxWidget->GetVisibility() == EVisibility::Visible) != bShow)
	{
		InputBoxWidget->Show(bShow);
		if (!bShow)
		{
			InputBoxWidget->SetFocus(false);
		}
	}
}
