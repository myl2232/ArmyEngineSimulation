#include "ArmyHydropowerPowerSystem.h"
#include "STextBlock.h"
#include "SComboBox.h"
#include "SScrollWrapBox.h"
#include "ArmyStyle.h"
#include "SButton.h"
#include "SArmyImageTextButton.h"
#include "SScrollBox.h"
#include "ArmyHydropowerDataManager.h"
#include "SImage.h"
#include "ArmyRoom.h"
#include "SArmyWindow.h"
#include "ArmySceneData.h"

void FHydropowerPowerSytem::SerializeToJson(TSharedRef<TJsonWriter<TCHAR,TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	JsonWriter->WriteValue("MainCableID",MainCableID);
	JsonWriter->WriteValue ("MainCableName",MainCableName);
	JsonWriter->WriteValue ("MainBreakID",MainBreakID);
	JsonWriter->WriteValue ("MainBreakName",MainBreakName);

	JsonWriter->WriteArrayStart("LoopArray");
	for (auto PowerSystemItem:LoopArray)
	{
		JsonWriter->WriteObjectStart();
		PowerSystemItem->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
}

void FHydropowerPowerSytem::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	MainCableID = InJsonData->GetIntegerField("MainCableID");
	MainCableName = InJsonData->GetStringField("MainCableName");
	MainBreakID = InJsonData->GetIntegerField("MainBreakID");
	MainBreakName = InJsonData->GetStringField("MainBreakName");
	const TArray< TSharedPtr<FJsonValue> > LoopArrayJson = InJsonData->GetArrayField("LoopArray");

	for (auto JsonLoopObj : LoopArrayJson)
	{
		TSharedPtr<FJsonObject> JsonObj = JsonLoopObj->AsObject();
		if (JsonObj.IsValid())
		{
			TSharedPtr<FHydropowerPowerSytemItem> ObjectValue = MakeShareable(new FHydropowerPowerSytemItem());
			ObjectValue->Deserialization(JsonObj);
			LoopArray.AddUnique(ObjectValue);
		}
	}
}

void FHydropowerPowerSytemItem::SerializeToJson(TSharedRef<TJsonWriter<TCHAR,TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	JsonWriter->WriteValue("LoopID",LoopID);
	JsonWriter->WriteValue ("LoopType",LoopType);
	JsonWriter->WriteValue("LoopName",LoopName);
	JsonWriter->WriteValue ("BreakID",BreakID);
	JsonWriter->WriteValue("BreakName",BreakName);
	JsonWriter->WriteValue ("PipeID",PipeID);
	JsonWriter->WriteValue("PipeName",PipeName);
	/* 新增字段，考虑兼容老版本数据 */
	JsonWriter->WriteValue("bManualOrAuto", bManualOrAuto != LOT_Auto ? LOT_Manual : LOT_Auto);

	JsonWriter->WriteArrayStart("RoomArray");
	for (int32 i = 0 ;i<RoomArray.Num();i++)
	{
		TSharedPtr<FArmyObjectIDValue> ObjValue = RoomArray[i];
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue ("ObjectID",ObjValue->ObjectID.ToString());
		JsonWriter->WriteValue ("RoomName",ObjValue->Value);
		JsonWriter->WriteValue("SpaceId", ObjValue->SpaceId);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
}

void FHydropowerPowerSytemItem::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	LoopID = InJsonData->GetIntegerField("LoopID");
	LoopType = InJsonData->GetIntegerField("LoopType");
	LoopName = InJsonData->GetStringField("LoopName");
	BreakID = InJsonData->GetIntegerField("BreakID");
	BreakName = InJsonData->GetStringField("BreakName");
	PipeID = InJsonData->GetIntegerField("PipeID");
	PipeName = InJsonData->GetStringField("PipeName");
	/* 新增字段，考虑兼容老版本数据 */
	bManualOrAuto = InJsonData->GetIntegerField("bManualOrAuto");
	bManualOrAuto = bManualOrAuto != LOT_Auto ? LOT_Manual : LOT_Auto;
	
	const TArray< TSharedPtr<FJsonValue> > RoomJsonArray = InJsonData->GetArrayField("RoomArray");

	for (auto RoomJsonObj:RoomJsonArray)
	{
		TSharedPtr<FJsonObject> RoomObj = RoomJsonObj->AsObject();
		if (RoomObj.IsValid())
		{
			FString RoomIDStr = RoomObj->GetStringField("ObjectID");
			FString RoomName = RoomObj->GetStringField("RoomName");
			int32 SpaceId = RoomObj->GetIntegerField("SpaceId");//兼容老数据
			if (SpaceId == 0)
			{
				TArray<TWeakPtr<FArmyObject>> InObjects;
				FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, InObjects);
				for (int32 i = 0; i < InObjects.Num(); i++)
				{
					FObjectPtr object = InObjects[i].Pin();
					if (object->GetType() != OT_InternalRoom)
					{
						continue;
					}
					TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(object);
					if (Room->GetSpaceName() == RoomName)
					{
						SpaceId = Room->GetSpaceId();
						break;
					}
				}
			}
			//兼容老数据结束
			FGuid RoomID;
			FGuid::Parse(RoomIDStr,RoomID);
			TSharedPtr<FArmyObjectIDValue> RoomKeyValue = MakeShareable(new FArmyObjectIDValue(RoomID,RoomName,SpaceId));
			RoomArray.AddUnique(RoomKeyValue);
		}
	}
}


void SPowerSystemItem::Construct(const FArguments& InArgs)
{
	bNew = InArgs._bNew;
	LoopName = InArgs._LoopName.Get();
	LoopIndex = InArgs._LoopIndex;
	LoopType = InArgs._LoopType;
	bManualOrAuto = InArgs._bManualOrAuto;
	SpaceNameList = InArgs._SpaceNameList;
	CircuitBreakerList = InArgs._CircuitBreakerList.Get();
	PipeProductList = InArgs._PipeProductList.Get();
	CircuitBreakerName = InArgs._CircuitBreakerName.Get();
	PipeLineValue = InArgs._PipeLineValue.Get();
	SelectSpaceNameList = InArgs._SelectSpaceNameList.Get();
	SpaceMultiSelectType= InArgs._SpaceMultiSelectType;

	UpdateSelectedSpacemList();
	UpdateSpaceNameList();

	ChildSlot
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0,15,0,0)
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(144)
				.HeightOverride(32)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(InArgs._LoopName)
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(20,0,0,0)
			.HAlign(HAlign_Left)
			[
				SNew(SBox)
				.WidthOverride(144)
				.HeightOverride(32)
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					SNew(SComboBox< TSharedPtr<FArmyKeyValue> >)
					.ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.None"))
					.ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.PropertyCombobox"))
					.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("ComboBox.Button.Gray1"))
					.OptionsSource(&CircuitBreakerList.Array)
					.MaxListHeight(200.f)
					.OnGenerateWidget(this, &SPowerSystemItem::OnGenerateCircuitBreakerComoboWidget)
					.OnSelectionChanged(this, &SPowerSystemItem::OnSlectedCircuitBreakerProjectChanged)
					.Content()
					[
						SNew(SBox)
						.WidthOverride(216.f)
						.HeightOverride(32.f)
						.VAlign(VAlign_Center)
						[
							SNew(SHorizontalBox)
							+SHorizontalBox::Slot()
							.VAlign(VAlign_Fill)
							.HAlign(HAlign_Fill)
							.MaxWidth(100)
							.AutoWidth()
							[
								SNew(STextBlock)
								.TextStyle(FArmyStyle::Get(), "ArmyText_12")
								.Clipping(EWidgetClipping::ClipToBounds)
								.Text(this, &SPowerSystemItem::GetSelectedCircuitBreaker)
							]
							+SHorizontalBox::Slot()
							.HAlign(HAlign_Right)
							.Padding(FMargin(0,0,0,0))
							[
								SNew(SImage)
								.Image(FArmyStyle::Get().GetBrush("Icon.DownArrow_Gray"))
							]
						]
					]
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(20,0,0,0)
			[
				SNew(SBox)
				.WidthOverride(144)
				.HeightOverride(32)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(SComboBox< TSharedPtr<FArmyKeyValue> >)
					.ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.None"))
					.ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.PropertyCombobox"))
					.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("ComboBox.Button.Gray1"))
					.OptionsSource(&PipeProductList.Array)
					.MaxListHeight(200.f)
					.OnGenerateWidget(this,&SPowerSystemItem::OnGeneratePipeLineComoboWidget)
					.OnSelectionChanged(this,&SPowerSystemItem::OnSlectedPipeLineProjectChanged)
					.Content()
					[
						SNew(SBox)
						.WidthOverride(216.f)
						.HeightOverride(32.f)
						.VAlign(VAlign_Center)
						[
							SNew(SHorizontalBox)
							+SHorizontalBox::Slot()
							.VAlign(VAlign_Fill)
							.HAlign(HAlign_Fill)
							.MaxWidth(100)
							.AutoWidth()
							[
								SNew(STextBlock)
								.TextStyle(FArmyStyle::Get(), "ArmyText_12")
								.Text(this, &SPowerSystemItem::GetSelectedPipeLine)
							]
							+SHorizontalBox::Slot()
							.HAlign(EHorizontalAlignment::HAlign_Right)
							.Padding(FMargin(0,0,0,0))
							//.AutoWidth()
							[
								SNew(SImage)
								.Image(FArmyStyle::Get().GetBrush("Icon.DownArrow_Gray"))
							]
						]
					]
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(20,0,0,0)
			[
				SNew(SBox)
				.WidthOverride(216)
				.HeightOverride(32)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SAssignNew(SpaceNameCombox,SComboBox< TSharedPtr<FArmyObjectIDValue> >)
					.ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.None"))
					.ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.PropertyCombobox"))
					.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("ComboBox.Button.Gray1"))
					.OptionsSource(&CurrentSpaceNameList)
					.MaxListHeight(200.f)
					.OnGenerateWidget(this,&SPowerSystemItem::GenerateSpace)
					.OnSelectionChanged(this,&SPowerSystemItem::OnSlectedSpaceNameChanged) 
					.Content()
					[
						SNew(SBox)
						.WidthOverride(216.f)
						.HeightOverride(32.f)
						.VAlign(VAlign_Center)
						[
							SNew(SHorizontalBox)
							+SHorizontalBox::Slot()
							.VAlign(VAlign_Fill)
							.HAlign(HAlign_Fill)
							.MaxWidth(188)
							.AutoWidth()
							[
								SNew(STextBlock)
								.Clipping(EWidgetClipping::ClipToBounds)
								.TextStyle(FArmyStyle::Get(), "ArmyText_12")
								.Text(this,&SPowerSystemItem::GetSpaceName)
							]
							+SHorizontalBox::Slot()
							.HAlign(EHorizontalAlignment::HAlign_Right)
							.Padding(FMargin(0,0,0,0))
							//.AutoWidth()
							[
								SNew(SImage)
								.Image(FArmyStyle::Get().GetBrush("Icon.DownArrow_Gray"))
							]
						]
					]
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(20,0,0,0)
			[
				SNew(SButton)
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.ButtonStyle(FArmyStyle::Get(),"Button.Del")
				.ButtonColorAndOpacity(FLinearColor(FColor::White))
				.OnClicked(InArgs._DeletePowerItem)
			]
		]
		+SVerticalBox::Slot()
		.Padding(0,20,0,0)
		.AutoHeight()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		[
			SNew(SImage)
			.ColorAndOpacity(this,&SPowerSystemItem::GetPowerLineColor)
			.Image(FArmyStyle::Get().GetBrush("PowerLine.Normal"))
		]

	];
	Shine();
}

void SPowerSystemItem::Shine()
{
	DeltaTime = 0;
	RegisterActiveTimer (0.25f,FWidgetActiveTimerDelegate::CreateSP (this,&SPowerSystemItem::TriggerCategoriesRefresh));
}

EActiveTimerReturnType SPowerSystemItem::TriggerCategoriesRefresh(double InCurrentTime,float InDeltaTime)
{	
	if (DeltaTime < 3)
	{
		bNew = (DeltaTime % 2 == 0);
		DeltaTime += 1;
		return EActiveTimerReturnType::Continue;
	}
	bNew = false;
	return EActiveTimerReturnType::Stop;
	
}


void SPowerSystemItem::UpdateSpaceNameList()
{
	TArray<TSharedPtr<FArmyObjectIDValue>>* AreaList = &CurrentSpaceNameList;
	if (!AreaList)
		return;
	AreaList->Reset(); 
	AreaList->Append(SpaceNameList.Get());
}


FSlateColor SPowerSystemItem::GetSpaceNameColor(TSharedPtr<FArmyObjectIDValue> InItem) const
{
	if (SelectSpaceNameList.Num())
	{
		if (SelectSpaceNameList.Contains(InItem))
		{
			return FLinearColor(FColor(0XFFFF8C00));
		}
	}
	return FArmyStyle::Get().GetColor("Color.E5E6E8FF");
}

/*
void SPowerSystemItem::UpdateSelectedSpacemList()
{
	for (int32 i = 0; i < SelectSpaceNameList.Num(); )
	{
		bool HavethisRoom = false;
		for (auto roominfo : SpaceNameList.Get())
		{
			if(roominfo->ObjectID== SelectSpaceNameList[i]->ObjectID)
			{
				HavethisRoom = true;
			}
		}

		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(FArmyObject::OnGetObjectByID.Execute(SelectSpaceNameList[i]->ObjectID).Pin());
		if (Room.IsValid()&& HavethisRoom)
		{
			SelectSpaceNameList[i]->Value = Room->GetSpaceName();
			i++;
		}
		else
		{
			SelectSpaceNameList.RemoveAt(i);

		}
	}
}*/

void SPowerSystemItem::UpdateSelectedSpacemList()
{
	for (int32 i = 0; i < SelectSpaceNameList.Num(); ++i)
	{
		int32 indexItem = SpaceNameList.Get().IndexOfByPredicate([&](const TSharedPtr<FArmyObjectIDValue>& Item)
		{
			return Item->ObjectID == SelectSpaceNameList[i]->ObjectID;
		});
		if (indexItem != INDEX_NONE)
		{
			SelectSpaceNameList[i]->Value = SpaceNameList.Get()[indexItem]->Value;

		}
		else
		{
			SelectSpaceNameList.RemoveAt(i);
		}
	}
}

FText SPowerSystemItem::GetSpaceName() const
{
	FString JoinedStr = TEXT("");

	for (int32 Index = 0; Index < SelectSpaceNameList.Num(); ++Index)
	{
		//TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(FArmyObject::OnGetObjectByID.Execute(SelectSpaceNameList[Index]->ObjectID).Pin());
			if (!JoinedStr.IsEmpty())
			{
				JoinedStr += TEXT(",");
			}
		
			JoinedStr += SelectSpaceNameList[Index]->Value;
	}
	
	if (JoinedStr.IsEmpty())
	{
		JoinedStr = TEXT("选择房间");
	}
	return FText::FromString(JoinedStr);
}


TSharedRef<SWidget> SPowerSystemItem::OnGenerateCircuitBreakerComoboWidget(TSharedPtr<FArmyKeyValue> InItem)
{
	FText SpaceName = FText::FromString(InItem->Value);
	return SNew(SBox)
		.HeightOverride(32)
		.WidthOverride(196)
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(FMargin(16,0,0,0))
		[
			SNew(STextBlock)
			.Justification(ETextJustify::Center)
		.Text(SpaceName)
		.ColorAndOpacity(this,&SPowerSystemItem::GetCircuitBreakerColor,SpaceName)
		.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		];
}

void SPowerSystemItem::OnSlectedCircuitBreakerProjectChanged(TSharedPtr<FArmyKeyValue> NewLoopItem,ESelectInfo::Type SelectInfo)
{
	
	if (NewLoopItem.IsValid())
	{
		if (!CircuitBreakerName.IsValid())
			CircuitBreakerName = MakeShareable(new FArmyKeyValue);
		CircuitBreakerName->Key = NewLoopItem->Key;
		CircuitBreakerName->Value = NewLoopItem->Value;
	}
}

FText SPowerSystemItem::GetSelectedCircuitBreaker() const
{
	FText Text = FText::FromString(TEXT("选择断路器"));
	if (CircuitBreakerName.IsValid()&&!CircuitBreakerName->Value.IsEmpty())
	{
		Text = FText::FromString(CircuitBreakerName->Value);
	}
	return Text;
}

FSlateColor SPowerSystemItem::GetCircuitBreakerColor(FText InText) const
{
	if (CircuitBreakerName.IsValid())
	{
		FText CurrentSpaceName = FText::FromString(CircuitBreakerName->Value);
		if (InText.EqualTo(CurrentSpaceName))
		{
			return FLinearColor(FColor(0XFFFF8C00));
		}
	}
	return FArmyStyle::Get().GetColor("Color.E5E6E8FF");
}

FSlateColor SPowerSystemItem::GetPowerLineColor() const
{
	if (bNew)
	{
		return FArmyStyle::Get().GetColor("Color.FFFD9800");
	}
	return FArmyStyle::Get().GetColor("Splitter.FF54555C");
}

TSharedRef<SWidget> SPowerSystemItem::OnGeneratePipeLineComoboWidget(TSharedPtr<FArmyKeyValue> InItem)
{
	FText SpaceName = FText::FromString(InItem->Value);
	return SNew(SBox)
		.HeightOverride(32)
		.WidthOverride(196)
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(FMargin(16,0,0,0))
		[
			SNew(STextBlock)
			.Justification(ETextJustify::Center)
			.Text(SpaceName)
			.ColorAndOpacity(this,&SPowerSystemItem::GetPipeLineColor,SpaceName)
			.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		];
}

void SPowerSystemItem::OnSlectedPipeLineProjectChanged(TSharedPtr<FArmyKeyValue> NewLoopItem,ESelectInfo::Type SelectInfo)
{
	if (NewLoopItem.IsValid())
	{
		if (!PipeLineValue.IsValid())
			PipeLineValue = MakeShareable(new FArmyKeyValue);
		PipeLineValue->Key = NewLoopItem->Key;
		PipeLineValue->Value = NewLoopItem->Value;
	}
}

FText SPowerSystemItem::GetSelectedPipeLine() const
{
	FText Text = FText::FromString(TEXT("选择电线,穿线管"));
	if (PipeLineValue.IsValid()&&!PipeLineValue->Value.IsEmpty())
	{
		Text = FText::FromString(PipeLineValue->Value);
	}
	return Text;
}

FSlateColor SPowerSystemItem::GetPipeLineColor(FText InText) const
{
	if (PipeLineValue.IsValid())
	{
		FText CurrentSpaceName = FText::FromString(PipeLineValue->Value);
		if (InText.EqualTo(CurrentSpaceName))
		{
			return FLinearColor(FColor(0XFFFF8C00));
		}
	}
	return FArmyStyle::Get().GetColor("Color.E5E6E8FF");
}

void SPowerSystemItem::GetPowerSystemItem(TSharedPtr<FHydropowerPowerSytemItem>& OutItem)
{
	OutItem->LoopID = LoopIndex;
	OutItem->LoopType = LoopType;
	OutItem->LoopName = LoopName.ToString();
	OutItem->bManualOrAuto = bManualOrAuto;
	
	if (CircuitBreakerName.IsValid())
	{
		OutItem->BreakName = CircuitBreakerName->Value;
		OutItem->BreakID = CircuitBreakerName->Key;
	}
	if (PipeLineValue.IsValid())
	{
		OutItem->PipeID = PipeLineValue->Key;
		OutItem->PipeName = PipeLineValue->Value;
	}
	OutItem->RoomArray.Append(SelectSpaceNameList);
}


bool SPowerSystemItem::IsPowerSystemItemIntegrality()
{
	bool bIntergrality = true;
	bIntergrality = CircuitBreakerName.IsValid()&&!CircuitBreakerName->Value.IsEmpty();
	if (bIntergrality)
	{
		bIntergrality = PipeLineValue.IsValid()&& !PipeLineValue->Value.IsEmpty();
	}
	if (bIntergrality)
	{
		bIntergrality = (SelectSpaceNameList.Num() != 0);
	}
	return bIntergrality;
}


TSharedRef<SWidget> SPowerSystemItem::GenerateSpace(TSharedPtr<FArmyObjectIDValue> InItem)
{
	switch(SpaceMultiSelectType)
	{
		case 0:
			return SNew(SBox)
			.WidthOverride(216)
			.HeightOverride(32)
			.HAlign(HAlign_Left)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.AutoWidth()
				.Padding(6,0,0,0)
				[
					SNew(STextBlock)
					.Justification(ETextJustify::Center)
					.Text(FText::FromString(InItem->Value))
					.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_10"))
					.ColorAndOpacity(this,&SPowerSystemItem::GetSpaceNameColor,InItem)
				]
			];;
		case 1:
			return SNew(SBox)
				.WidthOverride(216)
				.HeightOverride(32)
				.HAlign(HAlign_Left)
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.Padding(16,0,0,0)
				[
					SNew(SCheckBox)
					.Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("SingleCheckBox"))
				.IsChecked(this,&SPowerSystemItem::IsSpaceNameSelected,InItem)
				.OnCheckStateChanged(this,&SPowerSystemItem::OnCheckStateSpaceNameChanged,InItem)
				]
			+SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.AutoWidth()
				.Padding(6,0,0,0)
				[
					SNew(STextBlock)
					.Justification(ETextJustify::Center)
				.Text(FText::FromString(InItem->Value))
				.ColorAndOpacity(this,&SPowerSystemItem::GetSpaceNameColor,InItem)
				.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_10"))
				]
				];;
		default:
			return SNullWidget::NullWidget;
	}
	
}

ECheckBoxState SPowerSystemItem::IsSpaceNameSelected(TSharedPtr<FArmyObjectIDValue> InItem) const
{
	if (SelectSpaceNameList.Num() > 0 )
	{
		const TSharedPtr<FArmyObjectIDValue>* SelectVale = SelectSpaceNameList.FindByPredicate([&](TSharedPtr<FArmyObjectIDValue> Item){
			return Item->ObjectID==InItem->ObjectID;
		});
		if (SelectVale&& SelectVale->IsValid())
		{
			return ECheckBoxState::Checked;
		}
	}
	
	return ECheckBoxState::Unchecked;
}

void SPowerSystemItem::OnCheckStateSpaceNameChanged(ECheckBoxState InCheck,TSharedPtr<FArmyObjectIDValue> InItem)
{
	if (InCheck == ECheckBoxState::Checked)
	{
		SelectSpaceNameList.Add(InItem);
	}
	else if (InCheck == ECheckBoxState::Unchecked)
	{
		if (SelectSpaceNameList.Num()>0)
		{
			const TSharedPtr<FArmyObjectIDValue>* SelectVale = SelectSpaceNameList.FindByPredicate([&](TSharedPtr<FArmyObjectIDValue> Item){
				return Item->ObjectID==InItem->ObjectID;
			});
			if (SelectVale && SelectVale->IsValid())
			{
				int32 index = SelectSpaceNameList.Find(*SelectVale);
				SelectSpaceNameList.RemoveAt(index);
			}
		}
	}
}


void SPowerSystemItem::OnSlectedSpaceNameChanged(TSharedPtr<FArmyObjectIDValue> InItem,ESelectInfo::Type SelectInfo)
{
	if (SpaceMultiSelectType == 1)
		return; 
	if (SelectSpaceNameList.Num()>0)
	{
		SelectSpaceNameList.Empty();
	}
	SelectSpaceNameList.Add(InItem);
}


int32 SPowerSystemItem::OnPaint(const FPaintArgs& Args,const FGeometry& AllottedGeometry,const FSlateRect& MyCullingRect,FSlateWindowElementList& OutDrawElements,int32 LayerId,const FWidgetStyle& InWidgetStyle,bool bParentEnabled) const
{
	return SCompoundWidget::OnPaint(Args,AllottedGeometry,MyCullingRect,OutDrawElements,LayerId,InWidgetStyle,bParentEnabled);
}


void SPowerSystemItem::Tick(const FGeometry& AllottedGeometry,const double InCurrentTime,const float InDeltaTime)
{
	//UpdateSpaceNameList();
	//SpaceNameCombox->RefreshOptions();
	return SCompoundWidget::Tick(AllottedGeometry,InCurrentTime,InDeltaTime);
}

void SArmyHydropowerPowerSystem::Construct(const FArguments &InArgs)
{
	HouseholdCableName = InArgs._HouseholdCableName.Get();
	HouseholdCableList = InArgs._HouseholdCableList.Get();
	LoopList = InArgs._LoopList.Get();
	SpaceNameList = InArgs._SpaceNameList.Get();
	LoopDelegate = InArgs._LoopDelegate;
	CircuitBreakerList = InArgs._CircuitBreakerList.Get();
	OnCannel = InArgs._OnCannel;
	ChildSlot
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(20,8,0,8)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.Padding(0, 0, 20, 0)
			.AutoWidth()
			[
				CreateAddButton()
			]
            
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				CreateAutoAddButton()
			]
		]
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0)
		[
			SNew(SImage)
			.Image(FArmyStyle::Get().GetBrush("Splitter.FF343538"))
		]
		+SVerticalBox::Slot()
		[
		   CreateTitleWidget()
		]
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0)
		[
			SNew(SImage)
			.Image(FArmyStyle::Get().GetBrush("Splitter.FF343538"))
		]
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0)
		[
			CreateSecondWidget()
		]
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0)
		[
				SNew(SImage)
				.Image(FArmyStyle::Get().GetBrush("Splitter.FF343538"))
		]
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0,20,20,20)
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Center)
		[
			CreateSaveAndCannelWidget()
		]
	];

}



TSharedRef<SWidget> SArmyHydropowerPowerSystem::CreateAddButton()
{
	return 
		SNew(SBox)
		.WidthOverride(88)
		.HeightOverride(30)
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SAssignNew(AddLoopButton,SComboBox< TSharedPtr<FPowerSystemLoop> >)
			.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Orange"))
			.ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.None"))
			.ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.PropertyCombobox"))
			.OnSelectionChanged(this,&SArmyHydropowerPowerSystem::AddNewLoop)
			.OnGenerateWidget(this,&SArmyHydropowerPowerSystem::GenerateLoopItem)
			.OptionsSource(&LoopList)
			.MaxListHeight(352)
			.HasDownArrow(true)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				.AutoWidth()
				.Padding(5,0,0,0)
				[	
					SNew(SImage)
					.Image(FArmyStyle::Get().GetBrush("Icon.Add"))
				]
				+SHorizontalBox::Slot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.Padding(7,0,0,0)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("新增回路")))
					.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
				]
			]
		];
}

/**创建自动生成回路按钮*/
TSharedRef<SWidget> SArmyHydropowerPowerSystem::CreateAutoAddButton()
{
	return 
		SNew(SBox)
		.WidthOverride(80)
		.HeightOverride(30)
		[
			SNew(SButton)
			.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Orange"))
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.OnClicked(this, &SArmyHydropowerPowerSystem::AutoAddNewLoop)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("自动生成回路")))
				.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
			]
		];
}


TSharedRef<SWidget> SArmyHydropowerPowerSystem::CreateSecondWidget()
{
	return  
		SNew(SBox)
		.WidthOverride(1200)
		.HeightOverride(444)
		.HAlign(HAlign_Left)
		[ 
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			[
				CreateLeftWidget()
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0)
			[
				SNew(SImage)
				.Image(FArmyStyle::Get().GetBrush("Splitter.FF54555C"))
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0,0,0,0)
			.HAlign(HAlign_Left)
			[
				CreatePowerSystemListWidget()
			]
		]
		;
}



TSharedRef<SWidget> SArmyHydropowerPowerSystem::CreateLeftWidget()
{
	return 
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Center)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Bottom)
			.HAlign(HAlign_Left)
			[
				CreateHouseholdCableComboWidget()
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			
			[
				CreateCircuitBreakerComboWidget()
			]
		]
		+SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Center)
		.Padding(10,0,0,0)
		[
			SNew(SImage)
			.Image(FArmyStyle::Get().GetBrush("Icon.PowerLeft"))
		]
		;
}



TSharedRef<SWidget> SArmyHydropowerPowerSystem::CreateSaveAndCannelWidget()
{
	return 
		 SNew(SHorizontalBox)

        + SHorizontalBox::Slot()
        .Padding(0, 0, 20, 0)
        .AutoWidth()
        [
            SNew(SBox)
            .WidthOverride(80)
            .HeightOverride(30)
            [
                SNew(SButton)
                .ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Orange"))
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center)
                .OnClicked(this, &SArmyHydropowerPowerSystem::OnSavePowerSystem)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("确定")))
                    .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
                ]
            ]
        ]
            
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SBox)
            .WidthOverride(80)
            .HeightOverride(30)
            [
                SNew(SButton)
                .ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Gray.FF353638"))
                .OnClicked_Raw(this, &SArmyHydropowerPowerSystem::OnCancelButtonClicked)
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("取消")))
                    .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
                ]
            ]
        ];
		;
}


void SArmyHydropowerPowerSystem::LoadPowerSystem(TSharedPtr<FHydropowerPowerSytem>& InPowerSystem)
{
	PowerSystemListView->ClearChildren();
	HouseholdCableComboBox->SetSelectedItem(nullptr);
	CircuitBreakerComboBox->SetSelectedItem(nullptr);
	PowerSystemList.Reset();
	if (!HouseholdCableName.IsValid())
		HouseholdCableName = MakeShareable(new FArmyKeyValue);
	HouseholdCableName->Key = InPowerSystem->MainCableID;
	HouseholdCableName->Value = InPowerSystem->MainCableName;

	if (!CircuitBreakerName.IsValid())
		CircuitBreakerName = MakeShareable(new FArmyKeyValue);
	CircuitBreakerName->Key = InPowerSystem->MainBreakID;
	CircuitBreakerName->Value = InPowerSystem->MainBreakName;

	for (size_t i = 0; i<InPowerSystem->LoopArray.Num(); i++)
	{
		TSharedPtr<FHydropowerPowerSytemItem> PowerSystemItemObj  = InPowerSystem->LoopArray[i];
		int32 Index = PowerSystemItemObj->LoopID;
		FString LoopName = PowerSystemItemObj->LoopName;
		TSharedPtr<FPowerSystemLoop> LoopItem ; 
		int32 indexLoopItem = LoopList.IndexOfByPredicate([&](const TSharedPtr<FPowerSystemLoop>& Item)
		{
			return PowerSystemItemObj->LoopType == Item->LoopType;
		});
		if (indexLoopItem!=INDEX_NONE)
		{
			LoopItem = LoopList[indexLoopItem];

			TSharedPtr<FArmyKeyValue> SysCircuitBreakerName = MakeShareable(new FArmyKeyValue(PowerSystemItemObj->BreakID,PowerSystemItemObj->BreakName));
			TSharedPtr<FArmyKeyValue> PipeLineValue = MakeShareable(new FArmyKeyValue(PowerSystemItemObj->PipeID,PowerSystemItemObj->PipeName));
			FArmyComboBoxArray& PipeList = LoopDelegate.Execute(PowerSystemItemObj->LoopType);
			TSharedPtr<SPowerSystemItem> PowerSystemItem = SNew(SPowerSystemItem)
				.bNew(false)
				.LoopName(FText::FromString(LoopName))
				.SpaceMultiSelectType(LoopItem->spaceMultiSelect)
				.SpaceNameList(this,&SArmyHydropowerPowerSystem::GetSpaceNameList,LoopItem,Index)
				.LoopIndex(Index)
				.LoopType(PowerSystemItemObj->LoopType)
				.bManualOrAuto(PowerSystemItemObj->bManualOrAuto)
				.CircuitBreakerName(SysCircuitBreakerName)
				.PipeLineValue(PipeLineValue)
				.CircuitBreakerList(CircuitBreakerList)
				.PipeProductList(PipeList)
				.SelectSpaceNameList(PowerSystemItemObj->RoomArray)
				.DeletePowerItem(this,&SArmyHydropowerPowerSystem::OnDeleteLoopItem,Index);
			/*PowerSystemListView->AddItem(PowerSystemItem);*/
			PowerSystemListView->AddSlot()
				[
					PowerSystemItem.ToSharedRef()
				];
			PowerSystemList.Add(PowerSystemItem);
		}
		//PowerSystemListView->ResetScrollNotify();
	}
}

FReply SArmyHydropowerPowerSystem::OnSavePowerSystem()
{
	if (!PowerSystem.IsValid())
	{
		PowerSystem = MakeShareable(new FHydropowerPowerSytem);
	}
	PowerSystem->reset();
	if (HouseholdCableName.IsValid())
	{
		PowerSystem->MainCableID = HouseholdCableName->Key;
		PowerSystem->MainCableName = HouseholdCableName->Value;
	}
	if (CircuitBreakerName.IsValid())
	{
		PowerSystem->MainBreakID = CircuitBreakerName->Key;
		PowerSystem->MainBreakName = CircuitBreakerName->Value;
	}
	bool bSuccess = true;
	TSharedPtr<SPowerSystemItem> TempUIITem = nullptr;
	for (int32 i = 0 ;i<PowerSystemList.Num();i++)
	{
		TSharedPtr<FHydropowerPowerSytemItem> PowerItem = MakeShareable(new FHydropowerPowerSytemItem());
		TSharedPtr<SPowerSystemItem> PowerSystemUIItem = PowerSystemList[i];
		if (PowerSystemUIItem->IsPowerSystemItemIntegrality())
		{
			PowerSystemUIItem->GetPowerSystemItem(PowerItem);
			PowerSystem->LoopArray.Add(PowerItem);
		}
		else
		{
			TempUIITem = PowerSystemList[i];
			bSuccess = false;
			break;
		}
	}
	if (!bSuccess)
	{
		PowerSystemListView->ScrollDescendantIntoView(TempUIITem);
		TempUIITem->Shine();
		return FReply::Handled();
	}
	OnSave.ExecuteIfBound(PowerSystem);
	return FReply::Handled();
}

FReply SArmyHydropowerPowerSystem::OnCancelButtonClicked()
{
	OnCannel.ExecuteIfBound();
	return FReply::Handled();
}


TSharedRef<SWidget> SArmyHydropowerPowerSystem::CreateTitleWidget()
{
	return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.Padding(441, 0, 0, 0)
			.AutoWidth()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			[
				SNew(SBox)
				.WidthOverride(144)
				.HeightOverride(48)
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("回路")))
					.TextStyle(FCoreStyle::Get(),"VRSText_12")
					.Justification(ETextJustify::Center)
				]
			]
			+ SHorizontalBox::Slot()
			.Padding(0, 0, 0, 0)
			.AutoWidth()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			[
				SNew(SBox)
				.WidthOverride(163)
				.HeightOverride(48)
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("断路器")))
					.TextStyle(FCoreStyle::Get(), "VRSText_12")
					.Justification(ETextJustify::Center)
				]
			]
			+ SHorizontalBox::Slot()
			.Padding(0, 0, 0, 0)
			.AutoWidth()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			[
				SNew(SBox)
				.WidthOverride(164)
				.HeightOverride(48)
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("电线，穿线管")))
					.TextStyle(FCoreStyle::Get(), "VRSText_12")
					.Justification(ETextJustify::Center)
				]
			]
			+ SHorizontalBox::Slot()
			.Padding(0, 0, 0, 0)
			.AutoWidth()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			[
				SNew(SBox)
				.WidthOverride(288)
				.HeightOverride(48)
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("房间名称")))
					.TextStyle(FCoreStyle::Get(), "VRSText_12")
					.Justification(ETextJustify::Center)
				]
			]
	;
}

TSharedRef<SWidget> SArmyHydropowerPowerSystem::CreatePowerSystemListWidget()
{
	SAssignNew(PowerSystemListView,SScrollBox)
		.Orientation(Orient_Vertical);
	return 
		SNew(SOverlay)
		+SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			/*SNew(SBorder)
			.Padding(FMargin(2))
			.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF36373B"))
			.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
			.HAlign(HAlign_Left)
			[*/
				PowerSystemListView.ToSharedRef()
			//]
		]
		
		;
}


TSharedRef<SWidget> SArmyHydropowerPowerSystem::GenerateLoopItem(TSharedPtr<FPowerSystemLoop> InItem)
{
	return  SNew(SBox)
			.HeightOverride(32.f)
			.WidthOverride(88.f)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[ 
				SNew(STextBlock)
				.Text(FText::FromString(InItem->Name))
				//.ColorAndOpacity(FLinearColor::White)
				.TextStyle(FArmyStyle::Get(), "ArmyText_10")
				.Justification(ETextJustify::Center)
			]
		;
}

void SArmyHydropowerPowerSystem::GetNewLoopIndex(int32 InLoopType,int32& LoopIndex,int32& AllLoopIndex)
{
	AllLoopIndex = LoopIndex = 0;
	int32 TempIndex = 0;
	for (int32 i = 0 ;i<PowerSystemList.Num();i++)
	{
		TSharedPtr<SPowerSystemItem> PowerSystemItem = PowerSystemList[i];
		if (PowerSystemItem->GetLoopType() == InLoopType)
		{
			if (PowerSystemItem->GetLoopIndex() > LoopIndex)
			{
				LoopIndex++;
				TempIndex = PowerSystemItem->GetLoopIndex();
			}
		}
		if (PowerSystemItem->GetLoopType()<InLoopType)
		{
			AllLoopIndex = PowerSystemItem->GetLoopIndex();
		}
	}
	LoopIndex +=1;
	TempIndex +=1;
	AllLoopIndex += TempIndex;
}

void SArmyHydropowerPowerSystem::AddNewLoop(TSharedPtr<FPowerSystemLoop> NewLoopItem,ESelectInfo::Type SelectInfo)
{
	if (NewLoopItem.IsValid())
	{
		NewLoop(NewLoopItem);
		AddLoopButton->SetSelectedItem(nullptr);
	}
	
}

void SArmyHydropowerPowerSystem::NewLoop(TSharedPtr<FPowerSystemLoop> NewLoopItem)
{
	FArmyComboBoxArray& PipeList = LoopDelegate.Execute(NewLoopItem->id);
	int32 Index = 1,AllIndex = 0;
	GetNewLoopIndex(NewLoopItem->id,Index,AllIndex);
	FString LoopName = NewLoopItem->Name+FString::FromInt(Index);



	TSharedPtr<SPowerSystemItem> PowerSystemItem = SNew(SPowerSystemItem)
		.LoopName(FText::FromString(LoopName))
		.SpaceNameList(this,&SArmyHydropowerPowerSystem::GetSpaceNameList,NewLoopItem,AllIndex)
		.LoopIndex(AllIndex)
		.LoopType(NewLoopItem->LoopType)
		.bManualOrAuto(LOT_Manual)
		.CircuitBreakerList(CircuitBreakerList)
		.PipeProductList(PipeList)
		.SpaceMultiSelectType(NewLoopItem->spaceMultiSelect)
		.DeletePowerItem(this,&SArmyHydropowerPowerSystem::OnDeleteLoopItem,AllIndex);

	PowerSystemList.Add(PowerSystemItem);
	PowerSystemList.Sort([](const TSharedPtr<SPowerSystemItem>& A,const TSharedPtr<SPowerSystemItem>& B)
	{
		if (A->GetLoopType()<B->GetLoopType())
			return true;
		else if (A->GetLoopType()==B->GetLoopType())
			return A->GetLoopIndex()<B->GetLoopIndex();
		return  false;
	});

	PowerSystemListView->ClearChildren();
	for (int32 i = 0;i<PowerSystemList.Num();i++)
	{
		//PowerSystemListView->AddItem(PowerSystemList[i]);
		PowerSystemListView->AddSlot()
			.Padding(0,0,0,16)
			[
				PowerSystemList[i].ToSharedRef()
			];
	}
	PowerSystemListView->ScrollDescendantIntoView(PowerSystemItem);
}

FReply SArmyHydropowerPowerSystem::AutoAddNewLoop()
{
	TArray<TArray<TSharedPtr<FArmyObjectIDValue>>> RoomArrArr;//房间
	TArray<TSharedPtr<FPowerSystemLoopData>> LoopSampleArr;//房间对应的样本回路
	TSharedPtr<FPowerSystemLoopSample> LoopSample = FArmyHydropowerDataManager::Get()->GetPowerSystemLoopSample();
	if (LoopSample.IsValid())
	{
		//回路匹配核心算法
		AutoNewLoops(LoopSample, RoomArrArr, LoopSampleArr);
	}
	//根据匹配得到的回路数据生成实际可见的回路
	if (LoopSampleArr.Num() > 0 && LoopSampleArr.Num() == RoomArrArr.Num())
	{
		//存在自动生成的回路，修改入户电缆和入户断电器
		TSharedPtr<FPowerSystemJoinHouseWire> joinHouseWire = LoopSample->joinHouseWire;//入户电缆和入户断电器数据
		if (!HouseholdCableName.IsValid())
		{
			HouseholdCableName = MakeShareable(new FArmyKeyValue);
		}
		HouseholdCableName->Key = joinHouseWire->jhwIncomingCableType;
		HouseholdCableName->Value = joinHouseWire->jhwIncomingCableTypeName;

		if (!CircuitBreakerName.IsValid())
		{
			CircuitBreakerName = MakeShareable(new FArmyKeyValue);
		}
		CircuitBreakerName->Key = joinHouseWire->jhwBreakerGoodId;
		CircuitBreakerName->Value = joinHouseWire->jhwBreakerGoodName;

		//当前生成自动回路之前，删除之前存在的自动回路
		TArray<TSharedPtr<SPowerSystemItem>> manualUIItemArr;//保留手动添加的回路
		manualUIItemArr = PowerSystemList.FilterByPredicate([&](const TSharedPtr<SPowerSystemItem>& Item) {
			return Item->GetBManualOrAuto() == LOT_Manual;
		});
		PowerSystemList.Reset();
		PowerSystemList.Append(manualUIItemArr);
		//生成实际可见的回路
		for (int32 index = 0; index < LoopSampleArr.Num(); ++index)
		{
			auto& loopSample = LoopSampleArr[index];
			auto& roomArr = RoomArrArr[index];
			//
			int32 indexItem = LoopList.IndexOfByPredicate([&](const TSharedPtr<FPowerSystemLoop>& Item)
			{
				return loopSample->lloopType == Item->LoopType;
			});
			if (indexItem != INDEX_NONE)
			{
				TSharedPtr<FPowerSystemLoop> LoopTypeItem = LoopList[indexItem];
				//
				//根据FPowerSystemLoop数据获取回路信息
				FArmyComboBoxArray& PipeList = LoopDelegate.Execute(LoopTypeItem->id);
				int32 Index = 1, AllIndex = 0;
				GetNewLoopIndex(LoopTypeItem->id, Index, AllIndex);
				FString LoopName = LoopTypeItem->Name + FString::FromInt(Index);

				//下面的代码是生成一条回路
				TSharedPtr<FArmyKeyValue> SysCircuitBreakerName = MakeShareable(new FArmyKeyValue(loopSample->lBreakerGoodsId, loopSample->lbreakerGoodsName));
				TSharedPtr<FArmyKeyValue> PipeLineValue = MakeShareable(new FArmyKeyValue(loopSample->lwireGoodsId, loopSample->lwireGoodsName));
				TSharedPtr<SPowerSystemItem> PowerSystemItem = SNew(SPowerSystemItem)
					.bNew(false)
					.LoopName(FText::FromString(LoopName))
					.SpaceMultiSelectType(LoopTypeItem->spaceMultiSelect)
					.SpaceNameList(this, &SArmyHydropowerPowerSystem::GetSpaceNameList, LoopTypeItem, AllIndex)
					.LoopIndex(AllIndex)
					.LoopType(LoopTypeItem->LoopType)
					.bManualOrAuto(LOT_Auto)
					.CircuitBreakerName(SysCircuitBreakerName)
					.CircuitBreakerList(CircuitBreakerList)
					.PipeLineValue(PipeLineValue)
					.PipeProductList(PipeList)
					.SelectSpaceNameList(roomArr)
					.DeletePowerItem(this, &SArmyHydropowerPowerSystem::OnDeleteLoopItem, AllIndex);

				PowerSystemList.Add(PowerSystemItem);
			}
		}
		//排序和显示回路
		if (PowerSystemList.Num() > 0)
		{
			PowerSystemList.Sort([](const TSharedPtr<SPowerSystemItem>& A, const TSharedPtr<SPowerSystemItem>& B)
			{
				if (A->GetLoopType() < B->GetLoopType())
					return true;
				else if (A->GetLoopType() == B->GetLoopType())
					return A->GetLoopIndex() < B->GetLoopIndex();
				return  false;
			});
			PowerSystemListView->ClearChildren();
			for (int32 i = 0; i < PowerSystemList.Num(); i++)
			{
				PowerSystemListView->AddSlot()
					.Padding(0, 0, 0, 16)
					[
						PowerSystemList[i].ToSharedRef()
					];
			}
			PowerSystemListView->ScrollDescendantIntoView(PowerSystemList[0]);
		}
	}
	else
	{
		//不存在自动生成的回路，则提示匹配失败
		GGI->Window->ShowMessage(MT_Warning, TEXT("无匹配回路，请检查后台是否已配置回路！"));
	}
	//
	return FReply::Handled();
}

//回路匹配核心算法
void SArmyHydropowerPowerSystem::AutoNewLoops(TSharedPtr<FPowerSystemLoopSample> LoopSample, TArray<TArray<TSharedPtr<FArmyObjectIDValue>>>& RoomArrArr, TArray<TSharedPtr<FPowerSystemLoopData>>& LoopSampleArr)
{
	//实现核心算法
	TArray<TSharedPtr<SPowerSystemItem>> manualLoopArr;//手动添加的回路
	manualLoopArr = PowerSystemList.FilterByPredicate([&](const TSharedPtr<SPowerSystemItem>& Item) {
		return Item->GetBManualOrAuto() == LOT_Manual;
	});
	TArray<TSharedPtr<FArmyObjectIDValue>> SpaceList = SpaceNameList;//当前户型所有房间对象id/名称/空间id
	//以上准备数据完成
	// 1. 样本回路匹配空间（空间过滤）
	TArray<TSharedPtr<FPowerSystemLoopData>> Temp1LoopSampleArr;
	for (auto& loopSample : LoopSample->loopSettingArr)
	{
		//遍历所有回路样本数据，匹配当前户型的room
		for (auto& loopSpace : loopSample->lspaceArr)
		{
			int32 indexItem = SpaceList.IndexOfByPredicate([&](const TSharedPtr<FArmyObjectIDValue>& Item)
			{
				return loopSpace->sid == Item->SpaceId;
			});
			if (indexItem != INDEX_NONE)
			{
				//样本回路匹配空间成功
				Temp1LoopSampleArr.Add(loopSample);
				break;
			}
		}
	}
	// 2. 房间匹配样本回路（房间过滤）
	TArray<TSharedPtr<FArmyObjectIDValue>> Temp2RoomArr;//房间
	TArray<TSharedPtr<FPowerSystemLoopData>> Temp2LoopSampleArr;//房间对应的样本回路
	for (auto& room : SpaceList)
	{
		for (auto& loopSample : Temp1LoopSampleArr)
		{
			int32 indexItem = loopSample->lspaceArr.IndexOfByPredicate([&](const TSharedPtr<FPowerSystemLoopSpace>& Item)
			{
				return Item->sid == room->SpaceId;
			});
			if (indexItem != INDEX_NONE)
			{
				//判断当前房间是否存在相同的回路
				bool isExistLoopType = false;
				for (int32 i = 0; i < Temp2RoomArr.Num(); ++i)
				{
					TSharedPtr<FArmyObjectIDValue> oldRoom = Temp2RoomArr[i];
					TSharedPtr<FPowerSystemLoopData> oldLoopSample = Temp2LoopSampleArr[i];
					if (oldRoom == room && oldLoopSample->lloopType == loopSample->lloopType)
					{
						isExistLoopType = true;
						break;
					}
				}
				if (!isExistLoopType)
				{
					Temp2RoomArr.Add(room);
					Temp2LoopSampleArr.Add(loopSample);
				}
			}
		}
	}
	// 3. 过滤手工添加的回路，以手动回路为主，自动生成的回路只要跟手动回路的房间冲突，就丢弃自动回路
	TArray<TSharedPtr<FArmyObjectIDValue>> Temp3RoomArr;//房间
	TArray<TSharedPtr<FPowerSystemLoopData>> Temp3LoopSampleArr;//房间对应的样本回路
	for (int32 index = 0; index < Temp2LoopSampleArr.Num(); ++index)
	{
		auto& loopSample = Temp2LoopSampleArr[index];
		auto& room = Temp2RoomArr[index];
		//
		bool isContainInOther = false;
		int32 indexItem = manualLoopArr.IndexOfByPredicate([&](const TSharedPtr<SPowerSystemItem>& Item)
		{
			TArray<TSharedPtr<FArmyObjectIDValue>> RoomArr = Item->GetSelectSpaceNameList();
			for (int32 i = 0; i < RoomArr.Num(); ++i)
			{
				TSharedPtr<FArmyObjectIDValue> oldRoom = RoomArr[i];
				if (oldRoom->Value == room->Value && oldRoom->SpaceId == room->SpaceId && loopSample->lloopType == Item->GetLoopType())
				{
					return true;
				}
			}
			return false;
		});
		if (indexItem != INDEX_NONE)
		{
			isContainInOther = true;
		}
		if (!isContainInOther)
		{
			Temp3RoomArr.Add(room);
			Temp3LoopSampleArr.Add(loopSample);
		}
	}
	// 4. 根据回路类型合并同类型空间的回路
	TArray<TArray<TSharedPtr<FArmyObjectIDValue>>> Temp4RoomArrArr;//房间
	TArray<TSharedPtr<FPowerSystemLoopData>> Temp4LoopSampleArr;//房间对应的样本回路
	for (int32 index = 0; index < Temp3LoopSampleArr.Num(); ++index)
	{
		auto& loopSample = Temp3LoopSampleArr[index];
		auto& room = Temp3RoomArr[index];
		//
		TSharedPtr<FPowerSystemLoop> loopType;
		int32 indexItem = LoopList.IndexOfByPredicate([&](const TSharedPtr<FPowerSystemLoop>& Item)
		{
			return loopSample->lloopType == Item->LoopType;
		});
		if (indexItem != INDEX_NONE)
		{
			loopType = LoopList[indexItem];
		}
		if (loopType.IsValid())
		{
			if (loopType->spaceMultiSelect == 0)
			{
				//单选：空调、动力、冰箱
				//不做处理
				TArray<TSharedPtr<FArmyObjectIDValue>> NewRoomArr;
				NewRoomArr.Add(room);
				Temp4RoomArrArr.Add(NewRoomArr);
				Temp4LoopSampleArr.Add(loopSample);
			}
			else if (loopType->spaceMultiSelect == 1)
			{
				//多选：照明、插座
				//需要合并房间
				bool isContainInOther = false;
				for (int32 i = 0; i < Temp4RoomArrArr.Num(); ++i)
				{
					auto& oldLoopSample = Temp4LoopSampleArr[i];
					auto& oldRoomArr = Temp4RoomArrArr[i];
					if (oldRoomArr.Contains(room) && oldLoopSample->lloopType == loopType->LoopType)
					{
						isContainInOther = true;
						break;
					}
				}
				if (!isContainInOther)
				{
					TArray<TSharedPtr<FArmyObjectIDValue>> NewRoomArr;
					NewRoomArr.Add(room);
					for (int32 i = 0; i < Temp3RoomArr.Num(); i++)
					{
						auto& oldRoom = Temp3RoomArr[i];
						auto& olLoopSample = Temp3LoopSampleArr[i];
						if (oldRoom != room && oldRoom->SpaceId == room->SpaceId && olLoopSample->lloopType == loopType->LoopType)
						{
							//同种类型的其他房间的相同类型的回路，进行合并
							if (!NewRoomArr.Contains(oldRoom))
							{
								NewRoomArr.Add(oldRoom);
							}
						}
					}
					Temp4RoomArrArr.Add(NewRoomArr);
					Temp4LoopSampleArr.Add(loopSample);
				}
			}
		}
	}
	// 5. 根据回路类型合并同类型（同一个）的回路
	TArray<TArray<TSharedPtr<FArmyObjectIDValue>>> Temp5RoomArrArr;//房间
	TArray<TSharedPtr<FPowerSystemLoopData>> Temp5LoopSampleArr;//房间对应的样本回路
	for (int32 index = 0; index < Temp4LoopSampleArr.Num(); ++index)
	{
		auto& loopSample = Temp4LoopSampleArr[index];
		auto& roomArr = Temp4RoomArrArr[index];
		//
		TSharedPtr<FPowerSystemLoop> loopType;
		int32 indexItem = LoopList.IndexOfByPredicate([&](const TSharedPtr<FPowerSystemLoop>& Item)
		{
			return loopSample->lloopType == Item->LoopType;
		});
		if (indexItem != INDEX_NONE)
		{
			loopType = LoopList[indexItem];	
		}
		if (loopType.IsValid())
		{
			if (loopType->spaceMultiSelect == 0)
			{
				//单选：空调、动力、冰箱
				//只需要获取相同回路的第一个，其他房间的相同回路丢弃
				TArray<TSharedPtr<FArmyObjectIDValue>> NewRoomArr;
				for (int32 i = 0; i < Temp4LoopSampleArr.Num(); i++)
				{
					auto& oldRoomArr = Temp4RoomArrArr[i];
					auto& olLoopSample = Temp4LoopSampleArr[i];
					if (olLoopSample == loopSample && !Temp5LoopSampleArr.Contains(loopSample))
					{
						NewRoomArr.Append(oldRoomArr);
						//选中了某种类型的房间，则找出是否存在相同类型的房间，若有则分别生成一条回路
						for (int32 j = 0; j < Temp4LoopSampleArr.Num(); j++)
						{
							auto& oldRoomArr1 = Temp4RoomArrArr[j];
							auto& olLoopSample1 = Temp4LoopSampleArr[j];
							if (olLoopSample1 == loopSample && (oldRoomArr1[0]->SpaceId == roomArr[0]->SpaceId && oldRoomArr1[0]->Value != roomArr[0]->Value))
							{
								TArray<TSharedPtr<FArmyObjectIDValue>> NewRoomArr1;
								NewRoomArr1.Append(oldRoomArr1);
								Temp5RoomArrArr.Add(NewRoomArr1);
								Temp5LoopSampleArr.Add(loopSample);
							}
						}
						break;//只需要获取第一个回路，其他回路丢弃
					}
				}
				if (NewRoomArr.Num() > 0)
				{
					Temp5RoomArrArr.Add(NewRoomArr);
					Temp5LoopSampleArr.Add(loopSample);
				}
			}
			else if (loopType->spaceMultiSelect == 1)
			{
				//多选：照明、插座
				//需要合并同一个回路的所有房间
				TArray<TSharedPtr<FArmyObjectIDValue>> NewRoomArr;
				for (int32 i = 0; i < Temp4LoopSampleArr.Num(); i++)
				{
					auto& oldRoomArr = Temp4RoomArrArr[i];
					auto& olLoopSample = Temp4LoopSampleArr[i];
					if (olLoopSample == loopSample && !Temp5LoopSampleArr.Contains(loopSample))
					{
						NewRoomArr.Append(oldRoomArr);
					}
				}
				if (NewRoomArr.Num() > 0)
				{
					Temp5RoomArrArr.Add(NewRoomArr);
					Temp5LoopSampleArr.Add(loopSample);
				}
			}
		}
	}
	// 6. 使用过滤和合并完成的回路和房间数组，生成实际的回路
	for (int32 index = 0; index < Temp5LoopSampleArr.Num(); ++index)
	{
		auto& loopSample = Temp5LoopSampleArr[index];
		auto& roomArr = Temp5RoomArrArr[index];
		if (roomArr.Num() > 0)
		{
			LoopSampleArr.Add(loopSample);
			RoomArrArr.Add(roomArr);
		}
	}
}
//自动生成回路核心算法结束

TArray<TSharedPtr<struct FArmyObjectIDValue>> SArmyHydropowerPowerSystem::GetSpaceNameList(TSharedPtr<FPowerSystemLoop> NewLoopItem,int32 InIndex)const
{
	//过滤
	TArray<TSharedPtr<SPowerSystemItem>> FilterPowerSystemList = PowerSystemList.FilterByPredicate([&](const TSharedPtr<SPowerSystemItem>& InPowerSystemItem)
	{
		return NewLoopItem->LoopType==InPowerSystemItem->GetLoopType();
	});
	TArray<TSharedPtr<FArmyObjectIDValue>> TempSpaceNameList,ResidueSpaceNameList;
	for (auto& PowerSystemItem:FilterPowerSystemList)
	{
		if (!NewLoopItem->spaceRepeat && PowerSystemItem->GetLoopIndex() != InIndex)
			TempSpaceNameList.Append(PowerSystemItem->GetSelectSpaceNameList());
	}

	for (auto& Item:SpaceNameList)
	{
		int32 index = TempSpaceNameList.IndexOfByPredicate([&](const TSharedPtr<FArmyObjectIDValue>& SystemItem)
		{
			return SystemItem->ObjectID == Item->ObjectID;
		});
		if (index == INDEX_NONE)
		{
			ResidueSpaceNameList.Add(Item);
		}
	}
	return ResidueSpaceNameList;
}


FReply SArmyHydropowerPowerSystem::OnDeleteLoopItem(int32 Index)
{
	for (int32 i = 0 ;i< PowerSystemList.Num();i++)
	{
		TSharedPtr<SPowerSystemItem> PowerSystemItem = PowerSystemList[i];
		if (PowerSystemItem->IsLoopIndex(Index))
		{
			PowerSystemListView->RemoveSlot(PowerSystemItem.ToSharedRef());
			PowerSystemList.Remove(PowerSystemItem);
			break;
		}
	}
	return FReply::Handled();
}


TSharedRef<SWidget> SArmyHydropowerPowerSystem::CreateHouseholdCableComboWidget()
{
	return 	SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.HAlign(HAlign_Left)
		.Padding(20,0,0,0)
		[
			SNew(STextBlock)
			.TextStyle(FCoreStyle::Get(),"VRSText_12")
			.Text(FText::FromString(TEXT("进户电缆")))
		]
		+SVerticalBox::Slot()
		.Padding(20, 16, 0, 20)
		.AutoHeight()
		.VAlign(VAlign_Center)
		[

			SNew(SBox)
			.WidthOverride(216.f)
			.HeightOverride(32.f)
			.VAlign(VAlign_Fill)
			[
			SAssignNew(HouseholdCableComboBox, SComboBox< TSharedPtr<FArmyKeyValue> >)
			.ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.None"))
			.ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.PropertyCombobox"))
			.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("ComboBox.Button.Gray1"))
			.OptionsSource(&HouseholdCableList.Array)
			.MaxListHeight(200.f)
			.OnGenerateWidget(this,&SArmyHydropowerPowerSystem::OnGenerateHouseholdCableComboWidget)
			.OnSelectionChanged(this,&SArmyHydropowerPowerSystem::OnSlectedHouseholdCableChanged)
			.Content()
			[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Fill)
					.MaxWidth(168)
					.AutoWidth()
					[
						SNew(STextBlock)
						.TextStyle(FArmyStyle::Get(), "ArmyText_12")
						.Text(this,&SArmyHydropowerPowerSystem::GetSelectedHouseholdCable)
					]
	
					+SHorizontalBox::Slot()
					.HAlign(EHorizontalAlignment::HAlign_Right)
					.Padding(FMargin(0,0,0,0))
					//.AutoWidth()
					[
						SNew(SImage)
						.Image(FArmyStyle::Get().GetBrush("Icon.DownArrow_Gray"))
					]
				]
			]
		]
	;
}


TSharedRef<SWidget> SArmyHydropowerPowerSystem::CreateCircuitBreakerComboWidget()
{
	return
		
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.Padding(20,0,0,0)
			.HAlign(HAlign_Left)
			[
				SNew(STextBlock)
				.TextStyle(FCoreStyle::Get(),"VRSText_12")
				.Text(FText::FromString(TEXT("断路器")))
			]
			+SVerticalBox::Slot()
			.Padding(20, 16, 0, 20)
			.AutoHeight()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			[
				SNew(SBox)
				.WidthOverride(144.f)
				.HeightOverride(32.f)
				.VAlign(VAlign_Fill)
				[
					SAssignNew(CircuitBreakerComboBox, SComboBox< TSharedPtr<FArmyKeyValue> >)
					.ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.None"))
					.ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.PropertyCombobox"))
					.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("ComboBox.Button.Gray1"))
					.OptionsSource(&CircuitBreakerList.Array)
					.MaxListHeight(200.f)
					.OnGenerateWidget(this, &SArmyHydropowerPowerSystem::OnGenerateCircuitBreakerComoboWidget)
					.OnSelectionChanged(this, &SArmyHydropowerPowerSystem::OnSlectedCircuitBreakerProjectChanged)
					.Content()
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.VAlign(VAlign_Fill)
						.HAlign(HAlign_Fill)
						.MaxWidth(110)
						.AutoWidth()
						[
							SNew(STextBlock)
							.TextStyle(FArmyStyle::Get(), "ArmyText_12")
							.Text(this, &SArmyHydropowerPowerSystem::GetSelectedCircuitBreaker)
						]
						+SHorizontalBox::Slot()
						.HAlign(EHorizontalAlignment::HAlign_Right)
						.Padding(FMargin(0,0,0,0))
						//.AutoWidth()
						[
							SNew(SImage)
							.Image(FArmyStyle::Get().GetBrush("Icon.DownArrow_Gray"))
						]
				]
			]
		]
	;
}


TSharedRef<SWidget> SArmyHydropowerPowerSystem::OnGenerateHouseholdCableComboWidget(TSharedPtr<FArmyKeyValue> InItem)
{
	FText SpaceName = FText::FromString(InItem->Value);
	return SNew(SBox)
		.HeightOverride(32)
		.WidthOverride(196)
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(FMargin(16,0,0,0))
		[
			SNew(STextBlock)
			.Justification(ETextJustify::Center)
			.Text(SpaceName)
			.ColorAndOpacity(this,&SArmyHydropowerPowerSystem::GetHouseHoldColor,SpaceName)
			.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		];
}

void SArmyHydropowerPowerSystem::OnSlectedHouseholdCableChanged(TSharedPtr<FArmyKeyValue> NewLoopItem,ESelectInfo::Type SelectInfo)
{
	if (NewLoopItem.IsValid())
	{
		if (!HouseholdCableName.IsValid())
		{
			HouseholdCableName = MakeShareable(new FArmyKeyValue);
		}
		HouseholdCableName->Key = NewLoopItem->Key;
		HouseholdCableName->Value = NewLoopItem->Value;
	}
}

FText SArmyHydropowerPowerSystem::GetSelectedHouseholdCable()const
{
	if (HouseholdCableName.IsValid()&&!HouseholdCableName->Value.IsEmpty())
	{
		return FText::FromString(HouseholdCableName->Value);
	}
	return FText::FromString(TEXT("选择进户电缆"));
}


FSlateColor SArmyHydropowerPowerSystem::GetHouseHoldColor(FText InText) const
{
	if (HouseholdCableName.IsValid())
	{
		FText CurrentSpaceName = FText::FromString(HouseholdCableName->Value);
		if (InText.EqualTo(CurrentSpaceName))
		{
			return FLinearColor(FColor(0XFFFF8C00));
		}
	}
	
	return FArmyStyle::Get().GetColor("Color.E5E6E8FF");
}

TSharedRef<SWidget> SArmyHydropowerPowerSystem::OnGenerateCircuitBreakerComoboWidget(TSharedPtr<FArmyKeyValue> InItem)
{
	FText SpaceName = FText::FromString(InItem->Value);
	return SNew(SBox)
		.HeightOverride(32)
		.WidthOverride(196)
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(FMargin(16,0,0,0))
		[
			SNew(STextBlock)
			.Justification(ETextJustify::Center)
		.Text(SpaceName)
		.ColorAndOpacity(this,&SArmyHydropowerPowerSystem::GetCircuitBreakerColor,SpaceName)
		.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		];
}

void SArmyHydropowerPowerSystem::OnSlectedCircuitBreakerProjectChanged(TSharedPtr<FArmyKeyValue> NewLoopItem,ESelectInfo::Type SelectInfo)
{
	if (NewLoopItem.IsValid())
	{
		CircuitBreakerName->Key = NewLoopItem->Key;
		CircuitBreakerName->Value = NewLoopItem->Value;
	}
}

FText SArmyHydropowerPowerSystem::GetSelectedCircuitBreaker() const
{
	FText Text = FText::FromString(TEXT("选择断路器"));
	if (CircuitBreakerName.IsValid()&&!CircuitBreakerName->Value.IsEmpty())
	{
		Text = FText::FromString(CircuitBreakerName->Value);
	}
	return Text;
}

FSlateColor SArmyHydropowerPowerSystem::GetCircuitBreakerColor(FText InText) const
{
	if (CircuitBreakerName.IsValid())
	{
		FText CurrentSpaceName = FText::FromString(CircuitBreakerName->Value);
		if (InText.EqualTo(CurrentSpaceName))
		{
			return FLinearColor(FColor(0XFFFF8C00));
		}
	}
	
	return FArmyStyle::Get().GetColor("Color.E5E6E8FF");
}


