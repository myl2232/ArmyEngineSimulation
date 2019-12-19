#include "../../Public/Frame/SArmyPlanPrice.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "Widgets/Layout/SSplitter.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Images/SImage.h"
#include "ArmyStyle.h"
#include "ArmyConstructionQuantity.h"
#include "ArmyGameInstance.h"

void SArmyPlanPriceItem::SetCount(int32 newCount)
{
	Count = newCount;
	CountText->SetText(FText::FromString(FString::Printf(TEXT("%d项"), Count)));
}

void SArmyPlanPriceItem::SetPrice(float newPrice)
{
	Price = FMath::GridSnap(newPrice,0.01f);
	PriceText->SetText(FText::FromString(FString::Printf(TEXT("¥%.2f"), Price)));
}


void SArmyPlanPriceItem::Construct(const FArguments & InArgs)
{
	static int32 ItemHeight = 106;
	static int32 ItemWidth = 160;
	//TypeName = InArgs._TypeName;


	ChildSlot
		[
			SNew(SBox)
			//.HeightOverride(ItemHeight)
			.WidthOverride(ItemWidth)
			.Padding(FMargin(0))
			[
				SNew(SBorder)
				.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF383A3D"))
				.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
				//.VAlign(EVerticalAlignment::VAlign_Top)
				.HAlign(EHorizontalAlignment::HAlign_Left)
				.Padding(FMargin(14, 13, 0,16))
				[	
					
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0,0,0,0)
					[
						SAssignNew(PriceText, STextBlock)
						//SNew(STextBlock)
						.Text(FText::FromString(TEXT("1234.00")))
						.TextStyle(FArmyStyle::Get(), "ArmyText_18")
					]
					+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0,8,0,16)
					[
						SNew(STextBlock)
						.Text(InArgs._TypeName)
						.TextStyle(FArmyStyle::Get(), "ArmyText_12")
					]
					+ SVerticalBox::Slot()
						.HAlign(EHorizontalAlignment::HAlign_Left)
						.VAlign(EVerticalAlignment::VAlign_Center)
						.AutoHeight()
						.Padding(0, 0, 0, 8)
					[ 
						SNew(SBorder)
						.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF424447"))
						//.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
						.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
						.VAlign(EVerticalAlignment::VAlign_Center)
						.HAlign(EHorizontalAlignment::HAlign_Center)
						.Padding(FMargin(8,4,8,4))
						
						
						[
							SAssignNew(CountText, STextBlock)
						
							.Text(FText::FromString(TEXT("0项")))
							.TextStyle(FArmyStyle::Get(), "ArmyText_12")
						]
					]
				]
			]

		];
}

void SArmyPlanPrice::Construct(const FArguments & InArgs)
{
	static int32 DialogHeight = 328;
	static int32 DialogWidth = 768;


	//Items.SetNum(4);
	Items = {
		SNew(SArmyPlanPriceItem).TypeName(FText::FromString(TEXT("主材"))),
		SNew(SArmyPlanPriceItem).TypeName(FText::FromString(TEXT("定制商品"))),
		SNew(SArmyPlanPriceItem).TypeName(FText::FromString(TEXT("软装"))),
		SNew(SArmyPlanPriceItem).TypeName(FText::FromString(TEXT("施工项")))
	};

	ChildSlot
		[
			SNew(SBorder)
			.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.99000000"))
			.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
				.VAlign(EVerticalAlignment::VAlign_Center)
				.HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SNew(SBox)
			.HeightOverride(DialogHeight)
		.WidthOverride(DialogWidth)
		[
			SNew(SBorder)
			.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
		.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
		.Padding(FMargin(0))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
				//.VAlign(EVerticalAlignment::VAlign_Fill)
			[
				SNew(SBox)
				.HeightOverride(48)
				[
					SNew(SBorder)
					.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF212224"))
					.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
					.Padding(FMargin(0))
					.VAlign(EVerticalAlignment::VAlign_Center)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
							.AutoWidth()
							.HAlign(HAlign_Left)
							.VAlign(VAlign_Center)
							.Padding(16, 0, 0, 0)
						[
							SNew(STextBlock)
							.TextStyle(FArmyStyle::Get(), "ArmyText_14")
								.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFC8C9CC"))
								.Text(FText::FromString(TEXT("查看方案估价")))
						]
						+ SHorizontalBox::Slot()
							.Padding(0, 0, 5, 0)
							.HAlign(EHorizontalAlignment::HAlign_Right)
							.VAlign(EVerticalAlignment::VAlign_Center)
						[
							//关闭按钮
							SNew(SBox)
								.WidthOverride(30)
								.HeightOverride(30)
							[
								SNew(SButton)
									.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.CloseApplication"))
									.OnClicked_Lambda([this] {
										this->SetVisibility(EVisibility::Collapsed);
										return FReply::Handled();
										})
							]
							
						]
					]
					/*[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("X")))
					]*/
				]

			]



			+SVerticalBox::Slot()
			.VAlign(EVerticalAlignment::VAlign_Top)
			.AutoHeight()
			[
					SNew(SBox)
					.HeightOverride(64)
					.VAlign(EVerticalAlignment::VAlign_Center)
					.Padding(FMargin(40,0,40,0))
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()	
						.HAlign(EHorizontalAlignment::HAlign_Left)
						[
							SAssignNew(TotalPrice, SRichTextBlock)
							//SNew(SRichTextBlock)
							.Text(FText::FromString(TEXT("<XRRichTextBlock.Price>¥123456.78</>")))
							.TextStyle(FArmyStyle::Get(), "ArmyText_12")
							.DecoratorStyleSet(&FArmyStyle::Get())
				//+ SRichTextBlock::HyperlinkDecorator(TEXT("browser"), FSlateHyperlinkRun::FOnClick::CreateStatic(&OnBrowserLinkClicked, AsShared()))
						]

						+ SHorizontalBox::Slot()
							.VAlign(EVerticalAlignment::VAlign_Center)
							.FillWidth(1)
							.Padding(8, 0, 0, 0)
							.HAlign(EHorizontalAlignment::HAlign_Left)
						[
							//SAssignNew(TotalPrice, SRichTextBlock)
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("当前方案预估售价")))
							.TextStyle(FArmyStyle::Get(), "ArmyText_12")
				//+ SRichTextBlock::HyperlinkDecorator(TEXT("browser"), FSlateHyperlinkRun::FOnClick::CreateStatic(&OnBrowserLinkClicked, AsShared()))
						]


						+ SHorizontalBox::Slot()
							.HAlign(EHorizontalAlignment::HAlign_Right)
							.VAlign(EVerticalAlignment::VAlign_Center)
							.AutoWidth()
						[
							SAssignNew(OnlyGoodsConstructionCheckBox, SCheckBox)
						//	SNew(SCheckBox)
							//.IsChecked(this, &FArmyHardModeDetail::GetLight3DEnable)
							//.OnCheckStateChanged(this, &FArmyHardModeDetail::OnLight3DEnableChanged)
							.Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("SingleCheckBox"))
							.OnCheckStateChanged_Raw(this, &SArmyPlanPrice::OnOnlyGoodsConstructionChecked)
						]
						+ SHorizontalBox::Slot()
							.Padding(8,0,0,0)
							.HAlign(EHorizontalAlignment::HAlign_Right)
							.VAlign(EVerticalAlignment::VAlign_Center)
							.AutoWidth()
						[
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("仅统计商品及施工项")))
							.TextStyle(FArmyStyle::Get(), "ArmyText_12")
						]
						
					]
				
				]

			+ SVerticalBox::Slot()
				.Padding(32, 0, 32, 40)
				.FillHeight(1)
				//.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.Padding(8,0,8,0)
					[
						Items[0].ToSharedRef()
					]

					+ SHorizontalBox::Slot()
						.Padding(8, 0, 8, 0)
					[

						Items[1].ToSharedRef()
					]
					+ SHorizontalBox::Slot()
						.Padding(8, 0, 8, 0)
					[
						Items[3].ToSharedRef()
					]
					+ SHorizontalBox::Slot()
						.Padding(8, 0, 8, 0)
					[
						Items[2].ToSharedRef()
					]
				
			]

				+ SVerticalBox::Slot()
					.VAlign(EVerticalAlignment::VAlign_Bottom)
					.AutoHeight()
				[
					SNew(SBox)
					.HeightOverride(1)
					[
						SNew(SImage)
						.Image(FArmyStyle::Get().GetBrush("Splitter.Gray.FF424447"))
					]
					//.Text(FText::FromString(TEXT("当前方案预估售价：123")))
				]

				+ SVerticalBox::Slot()
					.VAlign(EVerticalAlignment::VAlign_Bottom)
					.HAlign(EHorizontalAlignment::HAlign_Right)
					.AutoHeight()
				//.FillHeight(1)
				[
					SNew(SBox)
					.HeightOverride(70)
					//.VAlign(EVerticalAlignment::VAlign_Center)
					[	
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
							.HAlign(EHorizontalAlignment::HAlign_Right)
							//.VAlign(VAlign_Center)
							.Padding(0,20,0,20)
							.AutoWidth()
						[
							SNew(SButton)
							.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Orange"))
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.OnClicked_Lambda([this]() {
								this->OnShowDetail.ExecuteIfBound();
								return FReply::Handled();
							})
							[
								//SNew(SBox)
								//.WidthOverride(80)
								//.HAlign(EHorizontalAlignment::HAlign_Center)
									//[
										SNew(STextBlock)
										.Text(FText::FromString(TEXT("   查看详细清单   ")))
										.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
							
									//]
							]
						]

						+ SHorizontalBox::Slot()
							.HAlign(EHorizontalAlignment::HAlign_Right)
							.AutoWidth()
							.Padding(20,20,20,20)
						[
							SNew(SButton)
							.ButtonStyle(FCoreStyle::Get(), "Box")
							.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Gray.FF353638"))
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
					
							
							.OnClicked_Lambda([this] {
							this->SetVisibility(EVisibility::Collapsed);
							return FReply::Handled();
							})
				
							[
								SNew(SBox)
								.WidthOverride(80)
								.HAlign(EHorizontalAlignment::HAlign_Center)
								[
									SNew(STextBlock)
									.Text(FText::FromString(TEXT("    关闭    ")))
									.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
								]	
							]
						]
					]
				]

		]]
		]
	];
}

void SArmyPlanPrice::SetDataSource(TSharedPtr<FArmyPlanPrice> Data)
{
	DataSource = Data;
	Update();
}

void SArmyPlanPrice::SetTotalPrice(float inPrice)
{
	if (!TotalPrice.IsValid())
	{
		return;
	}

	TotalPrice->SetText(FText::FromString(FString::Printf(TEXT("<XRRichTextBlock.Price>¥%.2f</>"), inPrice)));
}

void SArmyPlanPrice::Update()
{
	if (!OnlyGoodsConstructionCheckBox.IsValid())
	{
		//只能在UI建立之后才能调用Update函数
		return;
	}
	
	//是否仅统计商品及施工项
	bool	OnlyGoodsAndConstructionData = OnlyGoodsConstructionCheckBox->IsChecked();

	if (OnlyGoodsAndConstructionData)
	{
		SetTotalPrice(DataSource->SaleOrQuotaTotal);
	}
	else
	{
		SetTotalPrice(DataSource->Total);
	}

	for (int32 i=0;i<4;i++)
	{
		auto *CurrentItem =DataSource->Items.FindByPredicate([i](const FArmyPlanPriceItem& Item) {
			return  Item.Type - 1 == i;
		});

		if (!CurrentItem)
		{
			Items[i]->SetCount(0);
			Items[i]->SetPrice(0);
		}
		else if (!OnlyGoodsAndConstructionData)
		{
			Items[i]->SetCount(CurrentItem->Number);
			Items[i]->SetPrice(CurrentItem->Total);
		}
		else
		{
			Items[i]->SetCount(CurrentItem->SaleOrQuotaNumber);
			Items[i]->SetPrice(CurrentItem->SaleOrQuotaTotal);
		}
	}


}

void SArmyPlanPrice::OnOnlyGoodsConstructionChecked(ECheckBoxState NewState)
{
	Update();
}


void SArmyPlanPrice::Show()
{
	SetVisibility(EVisibility::Visible);

	

	auto children = GGI->WindowOverlay->GetChildren();

	for (int32 i = 0; i < children->Num(); i++)
	{
		if (children->GetChildAt(i).operator->()== this)
		{
			return;
		}
	}

	GGI->WindowOverlay->AddSlot()
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		.VAlign(EVerticalAlignment::VAlign_Fill)
		[
			this->AsShared()
		];
}