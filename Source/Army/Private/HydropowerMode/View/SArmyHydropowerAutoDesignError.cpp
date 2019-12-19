#include "SArmyHydropowerAutoDesignError.h"
#include "STextBlock.h"
#include "SComboBox.h"
#include "SScrollWrapBox.h"
#include "ArmyStyle.h"
#include "SButton.h"
#include "SArmyImageTextButton.h"
#include "SScrollBox.h"
#include "ArmyHydropowerDataManager.h"


typedef TSharedPtr<FAutoDesignResultInfo> FAutoDesignResultInfoPtr;

 class SResultInfoRow : public STableRow< FAutoDesignResultInfoPtr >
 {
 public:

	 SLATE_BEGIN_ARGS(SResultInfoRow)
		 :_bSuccessed(true)
	 {}
		 SLATE_ARGUMENT(bool,bSuccessed)
		 
		 SLATE_ARGUMENT(FText,Name)
		 
	     SLATE_ARGUMENT(FText,Content)
		 
	     SLATE_END_ARGS()
 private:
	 bool bSuccess;
 public:

	 /**
	 * Constructs this widget.
	 */
	 void Construct(const FArguments& InArgs,const TSharedRef<STableViewBase>& InOwnerTable)
	 {
		 bSuccess = InArgs._bSuccessed;
		 STableRow< FAutoDesignResultInfoPtr >::Construct(
			 typename STableRow<FAutoDesignResultInfoPtr >::FArguments()
			 .Style(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.FF28292B"))
			 .Content()
			 [
				 SNew(SBox)
				 .WidthOverride(380)
				 .HeightOverride(48)
				.Padding(FMargin(0,4,0,4)) 
				[
					SNew(SBorder)
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF38393D"))
					.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.AutoWidth()
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						.Padding(10,0,0,0)
						[
							SNew(SImage)
							.Image_Raw(this,&SResultInfoRow::GetImage)
						]
						+SHorizontalBox::Slot()
						.AutoWidth()
						.HAlign(HAlign_Left)
						.VAlign(VAlign_Center)
						.Padding(16,0,0,0)
						[
							SNew(STextBlock)
							.Text(InArgs._Name)
							.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
						]
					+SHorizontalBox::Slot()
					.HAlign(HAlign_Right)
					.VAlign(VAlign_Center)
					.FillWidth(144)
					.Padding(90,0,20,0)
					[
						SNew(STextBlock)
						.Text(InArgs._Content)
						.Justification(ETextJustify::Right)
						.ColorAndOpacity(FLinearColor(FColor(0xFFFF4457)))
						.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
					]
				 ]	
				]
			 ]
		 ,InOwnerTable
		 );
	 }

	 const FSlateBrush* GetImage()const
	 {
		 if (bSuccess)
		 {
			 return FArmyStyle::Get().GetBrush("Icon.Success");
		 }
		 else
		 {
			 return FArmyStyle::Get().GetBrush("Icon.Warning");
		 }
	 }
 };



void SArmyHydropowerAutoDesignError::Construct(const FArguments& InArgs)
{
	ChildSlot
		[
			SNew(SBox)
			.WidthOverride(400)
			.HeightOverride(344)
			//.HAlign(HAlign_Right)
			//.VAlign(VAlign_Top)
			//.Padding(FMargin(0, 86, 20, 0))
		
			[
				SNew(SBorder)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF28292B"))
				.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
				[

					SNew(SVerticalBox)
					+SVerticalBox::Slot()
					.HAlign(HAlign_Left)
					.Padding(0,0,0,0)
					.AutoHeight()
					[
						SNew(SBox)
						.HeightOverride(84)
						.WidthOverride(400)
						[
							SNew(SBorder)
						.Padding(0)
						.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF353638"))
						.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
						[
							SNew(SHorizontalBox)
							+SHorizontalBox::Slot()
							.VAlign(VAlign_Top)
							.HAlign(HAlign_Left)
							.Padding(20,20,0,0)
							.AutoWidth()
							[
								SNew(SBox)
								.MinDesiredWidth(24)
								.MinDesiredHeight(24)
								.HAlign(HAlign_Left)
								.VAlign(VAlign_Top)
								[
									SNew(SImage)
									.Image(FArmyStyle::Get().GetBrush(InArgs._TitleIcon.Get()))
								]
							]
						+SHorizontalBox::Slot()
						.HAlign(EHorizontalAlignment::HAlign_Left)
						.VAlign(VAlign_Top)
						.Padding(8,20,0,0)
						.AutoWidth()
						[
							SNew(SVerticalBox)
							+SVerticalBox::Slot()
							.Padding(0,0,0,16.f)
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Bottom)
							[
								SNew(STextBlock)
								.TextStyle(FArmyStyle::Get(),"ArmyText_16")
								.Text(InArgs._Title)
								.Justification(ETextJustify::Left)
							] 
							+SVerticalBox::Slot()
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Top)
							[
								SNew(STextBlock)
								.TextStyle(FArmyStyle::Get(),"ArmyText_12")
								.Text(InArgs._Content)
								.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.Gray.FFC8C9CC"))
								.Justification(ETextJustify::Left)
							]
						]
					]		
						]
						
				]
		+SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Top)
		.Padding(50,20,50,0)
		.AutoHeight()
		[
			SNew(SBox)
			.MinDesiredWidth(300)
			.MinDesiredHeight(170)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Top)
			[
				SNew(SImage)
				.Image(FArmyStyle::Get().GetBrush(InArgs._ContentIcon.Get()))
			]
		]
		+SVerticalBox::Slot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Center)
		.Padding(0,20,50,20)
		.AutoHeight()
		[
			SNew(SBox)
			.WidthOverride(80)
			.HeightOverride(30)
			[
				SNew(SButton)
				.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Gray.FF353638"))
			.OnClicked_Lambda([&]()
		{
			this->SetVisibility(EVisibility::Hidden);
			return FReply::Handled();
		})
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("我知道了")))
					.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
				]
			]
		]
		]
			
		]
		
		
	];
}

FReply SArmyHydropowerAutoDesignError::OnMouseButtonDown(const FGeometry& MyGeometry,const FPointerEvent& MouseEvent)
{

	return FReply::Handled();
}

void SArmyHydropowerAutoDesignFinish::Construct(const FArguments& InArgs)
 {
	 ChildSlot
	 [
		 SNew(SVerticalBox)
		 +SVerticalBox::Slot()
		.AutoHeight()
		 .Padding(0,0,0,1)
		 [
			 SNew(SBox)
			 .MaxDesiredHeight(InArgs._MaxListHeight)
			.MaxDesiredWidth(420)
			 [
				 SNew(SBorder)
				 .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
				.Padding(FMargin(20,10,20,0))
				.VAlign(VAlign_Center)
				 [
					 SAssignNew(this->ListView,ErrorList)
					 .ItemHeight(48)
					 .ListItemsSource(InArgs._OptionsSource)
					 .OnGenerateRow(this,&SArmyHydropowerAutoDesignFinish::GenerateMenuItemRow)
					 .SelectionMode(ESelectionMode::None)
				 ]
			 ]
		 ]
		+SVerticalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(0)
		[
			SNew(SImage)
			.ColorAndOpacity(FLinearColor(FColor(0XF343538)))
			.Image(FArmyStyle::Get().GetBrush("Icon.DownArrow_Gray"))
		]
		 +SVerticalBox::Slot()
		 .HAlign(HAlign_Right)
		 .VAlign(VAlign_Center)
		 .Padding(0,20,20,20)
		 .AutoHeight()
		 [
			SNew(SBox)
			.WidthOverride(80)
			.HeightOverride(30)
			[
				SNew(SButton)
				.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Gray.FF353638"))
				.OnClicked(InArgs._OnClicked)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("关闭")))
					.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
				]
			]
		 ]
		
	 ];
 }

 TSharedRef<ITableRow> SArmyHydropowerAutoDesignFinish::GenerateMenuItemRow(TSharedPtr<FAutoDesignResultInfo> InItem,const TSharedRef<STableViewBase>& OwnerTable)
 {
	 return SNew(SResultInfoRow,OwnerTable)
				.bSuccessed(InItem->bSuccessed)
				.Name(FText::FromString(InItem->LoopName))
				.Content(FText::FromString(InItem->ResultInfo));
 }
