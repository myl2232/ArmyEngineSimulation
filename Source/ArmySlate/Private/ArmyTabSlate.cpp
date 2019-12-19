//#include "SArmyTab.h"
//
//void SArmyTab::Construct(const FArguments& InArgs)
//{
//	ChildSlot
//	[
//		SNew(SBorder)
//		.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
//		.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
//		.Padding(FMargin(0,0,0,0))
//		[
//			SNew(SVerticalBox)
//
//			+ SVerticalBox::Slot()
//            .AutoHeight()
//			[
//				SNew(SBox)
//			    .HeightOverride(32)
//		        [
//			        //SNew(SBorder)
//			        //.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF212224"))
//		         //   .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
//		         //   .Padding(FMargin(2, 0, 2, 0))
//		         //   //.VAlign(VAlign_Center)
//		         //   .Content()
//		         //   [
//			           SAssignNew(TabWellContainer, SHorizontalBox)
//		           // ]
//		        ]
//			]
//
//			+ SVerticalBox::Slot()
//            .FillHeight(1)
//			.HAlign(HAlign_Right)
//			.VAlign(VAlign_Bottom)
//			.Padding(FMargin(10, 10, 10, 10))
//			[
//				SAssignNew(TabContentSwitcher, SWidgetSwitcher)
//			]
//		]
//	];
//
//	ActiveTimerHandle = RegisterActiveTimer(1.f / 60.f, FWidgetActiveTimerDelegate::CreateSP(this, &SArmyTab::ActiveTick));
//}
//
//void SArmyTab::AddTab(FText InLabel, FString InUniqueID, TSharedPtr<SWidget> InWidget)
//{
//	TabWellContainer->AddSlot()
//		.Padding(FMargin(2,0,0,0))
//		[
//			SNew(SBorder)
//			.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF212224"))
//			.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
//			.Padding(FMargin(5, 0, 5, 0))
//			.VAlign(VAlign_Center)
//			.Content()
//			[
//				SNew(STextBlock)
//				.Text(InLabel)
//				.TextStyle(FArmyStyle::Get(), "ArmyText_12")
//			]
//		];
//
//	TabContentSwitcher->AddSlot()
//		.VAlign(VAlign_Fill)
//		.HAlign(HAlign_Fill)
//		[
//			InWidget.ToSharedRef()
//		];
//}
//
//bool SArmyTab::RemoveTab(FString InUniqueID)
//{
//	return true;
//}
//
//bool SArmyTab::InvokeTab(FString InUniqueID)
//{
//	return true;
//}
//
//TSharedPtr<SWidget> SArmyTab::GetTabWidget(FString InUniqueID)
//{
//	return nullptr;
//}
//
//void SArmyTab::SetTabVisible(bool InVisible, FString InUniqueID)
//{
//
//}
//
//EActiveTimerReturnType SArmyTab::ActiveTick(double InCurrentTime, float InDeltaTime)
//{
//    return EActiveTimerReturnType::Continue;
//}
