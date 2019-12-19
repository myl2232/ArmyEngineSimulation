#include "ArmyCarouselPanelSlate.h"
#include "SWidgetSwitcher.h"
#include "SOverlay.h"
#include "Engine.h"
#include "ArmyStyle.h"

void SArmyCarouselPanel::Construct(const FArguments & InArgs)
{				
	ChildSlot
	[
		SNew(SOverlay)
		+SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SAssignNew(ContentWidgetSwitcher, SScrollBox)	
			.Style(&FArmyStyle::Get().GetWidgetStyle<FScrollBoxStyle>("LoginScrollBox"))
			.Orientation(EOrientation::Orient_Horizontal)
			.ScrollBarVisibility(EVisibility::Collapsed)
			.ConsumeMouseWheel(EConsumeMouseWheel::Never)
			.ScrollBarAlwaysVisible(false)
			.NavigationDestination(EDescendantScrollDestination::Center)
		]
		+SOverlay::Slot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Bottom)
		.Padding(0, 0, 24, 25)
		[
			SAssignNew(CarouselButtonsContiner, SHorizontalBox)
		]
	];			
	ContentWidgetSwitcher->SetScrollBarRightClickDragAllowed(false);
	RegisterActiveTimer(1.f / 60.f, FWidgetActiveTimerDelegate::CreateRaw(this, &SArmyCarouselPanel::ActiveTick));
}


void SArmyCarouselPanel::AddCarouselItem(TSharedPtr<SWidget> InWidget)
{
	AllCarouselWidgets.Add(InWidget);
	ContentWidgetSwitcher->AddSlot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			InWidget->AsShared()
		];

	AllCarouselButtons.Add(SNew(SArmyCarouselButton)
		.RelatedIndex(AllCarouselWidgets.Num() - 1)
		.OnHovered(FSimpleDelegate::CreateRaw(this, &SArmyCarouselPanel::SwitchByButton)));

	CarouselButtonsContiner->AddSlot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(8, 0, 0, 0)
		[
			AllCarouselButtons[AllCarouselWidgets.Num() - 1]->AsShared()
		];
}

void SArmyCarouselPanel::AddCarouselItemArray(TArray<TSharedPtr<SWidget>> InWidgetArray)
{
	AllCarouselWidgets.Append(InWidgetArray);
	for (auto It : InWidgetArray)
	{
		ContentWidgetSwitcher->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				It->AsShared()
			];
		AllCarouselButtons.Add(SNew(SArmyCarouselButton)
			.RelatedIndex(AllCarouselWidgets.Num() - 1)
			.OnHovered(FSimpleDelegate::CreateRaw(this, &SArmyCarouselPanel::SwitchByButton)));
		CarouselButtonsContiner->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Padding(8, 0, 0, 0)
			[
				AllCarouselButtons[AllCarouselWidgets.Num() - 1]->AsShared()
			];
	}
}

void SArmyCarouselPanel::SwitchByIndex(int Index)
{
	if (AllCarouselWidgets.Num() > 0)
	{		
		if (Index < AllCarouselWidgets.Num())
		{
			ContentWidgetSwitcher->ScrollDescendantIntoView(AllCarouselWidgets[Index]);
		}
		else
		{
			CurrentIndex = 0;
			ContentWidgetSwitcher->ScrollDescendantIntoView(AllCarouselWidgets[CurrentIndex]);
		}

		for (auto It : AllCarouselButtons)
		{
			if (It->RelatedIndex == Index)
			{
				It->SetCurrent();
			}
			else
			{
				It->SetUnCarousel();
			}
		}

		GEngine->GameViewport->GetWorld()->GetTimerManager().SetTimer(MessageTimeHandle, FTimerDelegate::CreateRaw(this, &SArmyCarouselPanel::SwitchToNext), CarouseInteral, false);
	}	
}

void SArmyCarouselPanel::SwitchToNext()
{
	if (++CurrentIndex >= AllCarouselWidgets.Num())
	{
		CurrentIndex = 0;
	}	
	SwitchByIndex(CurrentIndex);
}

void SArmyCarouselPanel::SwitchByButton()
{
	for (auto It : AllCarouselButtons)
	{
		if (It->GetIsHovered())
		{
			SwitchByIndex(It->RelatedIndex);
		}
	}
}

EActiveTimerReturnType SArmyCarouselPanel::ActiveTick(double InCurrentTime, float InDeltaTime)
{
	return EActiveTimerReturnType::Continue;
}

void SArmyCarouselButton::Construct(const FArguments & InArgs)
{
	RelatedIndex = InArgs._RelatedIndex;
	OnHovered = InArgs._OnHovered;
	ChildSlot
	[
		SNew(SBox)
		.WidthOverride(16)
		.HeightOverride(16)
		[
			SAssignNew(CarouselButton, SButton)
			.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.CarouseButton"))
			.OnHovered(FSimpleDelegate::CreateRaw(this, &SArmyCarouselButton::OnCarouseButtonHovered))
			.OnUnhovered(FSimpleDelegate::CreateRaw(this, &SArmyCarouselButton::OnCarouseButtonUnHover))
		]		
	];
}

void SArmyCarouselButton::SetCurrent()
{
	CarouselButton->SetBorderBackgroundColor(FLinearColor(FColor(0XFFFF8C00)));
}

void SArmyCarouselButton::SetUnCarousel()
{
	CarouselButton->SetBorderBackgroundColor(FLinearColor(FColor(0XFFFFFFFF)));
}

void SArmyCarouselButton::OnCarouseButtonHovered()
{
	bIsHovered = true;
	CarouselButton->SetBorderBackgroundColor(FLinearColor(FColor(0XFFFF8C00)));
	OnHovered.ExecuteIfBound();
}

void SArmyCarouselButton::OnCarouseButtonUnHover()
{
	bIsHovered = false;
}
