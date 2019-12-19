#pragma once
#include "ArmyTypes.h"
#include "Widgets/SCompoundWidget.h"

class ARMYSLATE_API SArmyProgressBar : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArmyProgressBar)
	{}
	SLATE_EVENT(FSimpleDelegate, OnCanceled)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void SetCurrentLabel(const FText& InLabel);
	void SetCurrentText(const FText& InText);
	void SetCurrentPercent(float InPercent);
private:
	FText GetCurrentLabel() const;
	FText GetCurrentValue() const;
	TOptional<float> GetProgressBarPercent() const;
	FReply OnCancel();
private:
	FSimpleDelegate OnCancelDelegate;

	FText CurrentLabel;
	FText CurrentText;
	float CurrentPercent = 0;
};