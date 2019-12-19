#include "ArmyHydropowerDetail.h"
#include "SArmyObjectOutliner.h"
#include "ArmyTypes.h"
#include "ArmyPipeline.h"
#include "ArmyFurniture.h"
#include "ArmyPipePoint.h"
#include "SArmyDetailComponent.h"
#include "SWidgetSwitcher.h"
#include "ArmyActor.h"
#include "ArmyGroupActor.h"
#include "ArmyEditorEngine.h"
#include "ArmySceneData.h"
#include "ArmyHydropowerDataManager.h"
#include "ArmyTypes.h"
#include "SArmyComboBox.h"
#include "ArmyResourceModule.h"
#include "SArmyComboBox.h"
#include "ArmySlateModule.h"
#include "ArmyDownloadModule.h"
#include "SArmyModelContentBrowser.h"
#include "ArmyGameInstance.h"
#include "SArmyReplaceList.h"
#include "ArmyHydropowerModeController.h"
#include "Runtime/Online/ImageDownload/Public/WebImageCache.h"
#include "ArmyFurnitureActor.h"
#include "ArmyCommonTools.h"
#include "ArmyFurniture.h"
#include "ArmyConstructionManager.h"
#include "ArmyPipelineActor.h"

#define defaultOpacity 100

void SDropDownTextBox::Construct(const FArguments& InArgs)
{
	OptionsSource = InArgs._OptionsSource;
	OnTextChangedCallback = InArgs._OnTextChanged;
	OnTextCommittedCallback = InArgs._OnTextCommitted;
	ChildSlot
		.Padding(0, 0, 0, 0)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		.Padding(FMargin(0, 0, 0, 0))
		[
			SAssignNew(ComboBox, SComboBox< TSharedPtr<float> >)
			.ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.Gray"))
		.ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.Property"))
		.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("ComboBox.Button.Gray1"))

		.ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
		.OptionsSource(InArgs._OptionsSource)
		.MaxListHeight(200)
		.Visibility(this, &SDropDownTextBox::ShowComBox)
		.OnGenerateWidget(this, &SDropDownTextBox::OnGenerateWidget)
		.OnSelectionChanged(this, &SDropDownTextBox::OnWinowTypeChange)
		.Content()
		[
			SNew(STextBlock)
		]
		]
	+ SOverlay::Slot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		.Padding(0, 0, 24, 0)
		[
			SNew(SEditableTextBox)
			.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox.FF2F3033"))
		.Text(InArgs._Text)
		.OnTextChanged(this, &SDropDownTextBox::OnValueChanged)
		.OnTextCommitted(this, &SDropDownTextBox::OnTextCommitted)
		]

		];
}

TSharedRef<SWidget> SDropDownTextBox::OnGenerateWidget(TSharedPtr<float> InItem)
{
	float Value = *InItem.Get();
	return
		SNew(SBox)
		.HeightOverride(24)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.Padding(FMargin(10, 0, 0, 0))
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.ColorAndOpacity(FLinearColor::White)
		.Text(FText::FromString(FString::SanitizeFloat(Value)))
		.TextStyle(FCoreStyle::Get(), "VRSText_10")
		]
		];
}

void SDropDownTextBox::OnWinowTypeChange(TSharedPtr<float>  NewSelection, ESelectInfo::Type SelectInfo)
{
	if (!NewSelection.IsValid())
		return;
	CurrentValue = *NewSelection.Get();
	FString Str = FString::SanitizeFloat(CurrentValue);
	FText text = FText::FromString(Str);
	OnTextChangedCallback.ExecuteIfBound(text);
	//TextBlock->SetText (text);
}

TOptional<float> SDropDownTextBox::GetValue() const
{
	TOptional<float> value = 0.f;
	if (OptionsSource && OptionsSource->Num())
		value = *((*OptionsSource)[0].Get());
	return value;
}

void SDropDownTextBox::OnValueChanged(const FText& text)
{

}

void SDropDownTextBox::OnTextCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	OnTextCommittedCallback.ExecuteIfBound(InText, InTextAction);
}

EVisibility SDropDownTextBox::ShowComBox() const
{
	return  OptionsSource && OptionsSource->Num() >= 2 ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed;
}

FArmyHydropowerDetail::FArmyHydropowerDetail()
	:ShowType(0),
	FlagArray(MakeShareable(new FArmyComboBoxArray()))
{
	CatergoryOrder.Add("GlobalCategory");
	CatergoryOrder.Add("RefromCategory");
	CatergoryOrder.Add("Construction");

	DrainArray.Add(MakeShareable(new FArmyKeyValue(DT_None, TEXT("无"))));
	DrainArray.Add(MakeShareable(new FArmyKeyValue(DT_P, TEXT("P型管"))));
	DrainArray.Add(MakeShareable(new FArmyKeyValue(DT_U, TEXT("U型管"))));
	DrainArray.Add(MakeShareable(new FArmyKeyValue(DT_S, TEXT("S型管"))));

	//FlagArray->Add(MakeShareable(new FArmyKeyValue(ESF_None,TEXT("无"))));
	FlagArray->Add(MakeShareable(new FArmyKeyValue(ESF_Normal, TEXT("普通插座"))));
	FlagArray->Add(MakeShareable(new FArmyKeyValue(ESF_AirConditioner, TEXT("空调插座"))));
	FlagArray->Add(MakeShareable(new FArmyKeyValue(ESF_Refrigerator, TEXT("冰箱插座"))));
	FlagArray->Add(MakeShareable(new FArmyKeyValue(ESF_Power, TEXT("动力插座"))));
}

TSharedRef<SWidget> FArmyHydropowerDetail::CreateDetailWidget()
{
	SAssignNew(DetailWidgetSwitcher, SWidgetSwitcher);

	CreateOutletDetail();
	CreateSocketDetail();
	CreateWaterDetail();
	CreatePrePTDetail();
	CreateLineDetail();
	CreatePipeFittingDetail();
	CreateDefaultDetail();
	DetailWidget = DetailWidgetSwitcher;
	//HiddenDetailWidget(EVisibility::Hidden);
	return DetailWidget->AsShared();
}

//选择管线会触发此函数
void FArmyHydropowerDetail::ShowSelectedDetial(TSharedPtr<FArmyObject> InSelectedObject)
{
	//管线就没有对应的房间
	SelectedRoom = nullptr;

	if (InSelectedObject.IsValid())
	{
		SelectObject = InSelectedObject;
		SelectedUObject = InSelectedObject->GetRelevanceActor();
		CachedFurnitureName = SelectObject.Pin()->GetName();
		InitShowType();
		if (DetailWidgetMap.Find(SelectedType)->IsValid())
		{
			Offsets = FVector::ZeroVector;
			NativeFurniture = nullptr;

			UpdateGlobalCategoryUI();
			UpdateConstructionOptions(SelectedType);
		
			DetailWidgetSwitcher->SetActiveWidget(DetailWidgetMap.Find(SelectedType)->Get()->AsShared());
			HiddenDetailWidget(EVisibility::Visible);
		}
		else
		{
			DetailWidgetSwitcher->SetActiveWidget(DetailWidgetMap.Find(EHydropowerDetailType::HydropowerPropertyType::None)->Get()->AsShared());
		}

	}
	else
	{
		SelectObject = nullptr;
		SelectedUObject = nullptr;
		DetailWidgetSwitcher->SetActiveWidget(DetailWidgetMap.Find(EHydropowerDetailType::HydropowerPropertyType::None)->Get()->AsShared());
	}
}

void FArmyHydropowerDetail::ShowSelectedDetial(UObject* InSelectedObject)
{    

	SelectedRoom = nullptr;

	if (SelectedUObject != InSelectedObject)
	{
		//关闭替换窗口
		if (HyReplaceList.IsValid())
		{
			GGI->WindowOverlay->RemoveSlot(HyReplaceList.ToSharedRef());
			//HyReplaceList->
			HyReplaceList = nullptr;
			SelectedUObject = nullptr;
		}
	}

	SelectedUObject = InSelectedObject;
	if (SelectedUObject)
	{
		AActor* SelectedActor = Cast<AActor>(SelectedUObject);
		FVector ActorSize = CalcActorSize(SelectedActor);
		CachedScaleX = FMath::Abs(ActorSize.X * 10.f);
		CachedScaleY = FMath::Abs(ActorSize.Y * 10.f);
		CachedScaleZ = FMath::Abs(ActorSize.Z * 10.f);

		AXRActor * ParentActor = Cast<AXRActor>(SelectedActor->GetOwner());
		if (ParentActor)
		{
			TSharedPtr<FArmyObject> Object = ParentActor->GetRelevanceObject().Pin();
			if (Object.IsValid())
			{
				SelectObject = Object;
				CachedFurnitureName = SelectedActor->GetName();
				InitGroundHeightArray();
			}

		}
		InitShowType();
		if (SelectedType == EHydropowerDetailType::None)
		{
			auto WidgetToShow = DetailWidgetMap.Find(EHydropowerDetailType::None)->Get()->AsShared();
			DetailWidgetSwitcher->SetActiveWidget(WidgetToShow);
			return;
		}
		if (DetailWidgetMap.Find(SelectedType)->IsValid())
		{	
			TSharedPtr<FArmyFurniture> furniture = StaticCastSharedPtr<FArmyFurniture>(SelectObject.Pin());;
			if(furniture.IsValid())
			{	
				NativeFurniture = furniture->GetRepresentedFurniture(true);
				
				 Offsets = NativeFurniture->GetOffset();
				//绑定偏移代理
				NativeFurniture->FurnitureOffsetedDelegate.BindRaw(this, &FArmyHydropowerDetail::UpdateOffset);
				SelectedRoom = furniture->GetRepresentedFurniture(false)->GetRoom();
				//刷新Ui
				UpdateGlobalCategoryUI();
				UpdateBreakPointUI(SelectedType);
				UpdateConstructionOptions(SelectedType);

				SetSingleLocationZWidgetEditable(!(NativeFurniture->IsOrignalPoint() && !NativeFurniture->IsBPreforming()));
			}
			auto WidgetToShow = DetailWidgetMap.Find(SelectedType)->Get()->AsShared();
			DetailWidgetSwitcher->SetActiveWidget(WidgetToShow);
			DetailBuilderMap[SelectedType]->RefreshTree();
			HiddenDetailWidget(EVisibility::Visible);
		}
		else
		{
			DetailWidgetSwitcher->SetActiveWidget(DetailWidgetMap.Find(EHydropowerDetailType::HydropowerPropertyType::None)->Get()->AsShared());
		}

	}
	else
	{
		SelectObject = nullptr;
		DetailWidgetSwitcher->SetActiveWidget(DetailWidgetMap.Find(EHydropowerDetailType::HydropowerPropertyType::None)->Get()->AsShared());
	}

}

void FArmyHydropowerDetail::Clear()
{
	UObject * obj = nullptr;
	TSharedPtr<FArmyObject> XRObj = nullptr;
	ShowSelectedDetial(obj);
	ShowSelectedDetial(XRObj);
}

void FArmyHydropowerDetail::CreateOutletDetail()
{
	OutletDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	//FArmyDetailNode& nbh = OutletDetailBuilder->AddCategory("kij4167", TEXT("物体qaw详情"));
	//nbh.AddChildNode("ComponentName", TEXT("名称"), CreateNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	FArmyDetailNode& DetailCategory = OutletDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("长度(mm)"), CreateScaleXWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Width", TEXT("宽度(mm)"), CreateScaleYWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateScaleZWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("GroundHeight", TEXT("离地高度(mm)"), CreateLocationZWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	

	DetailWidgetMap.Add(EHydropowerDetailType::OutLet, OutletDetailBuilder->BuildDetail());
	DetailBuilderMap.Add(EHydropowerDetailType::OutLet, OutletDetailBuilder);

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EHydropowerDetailType::OutLet)->Get()->AsShared()
		];
}

void FArmyHydropowerDetail::CreateSocketDetail()
{
	OutletDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = OutletDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("长度(mm)"), CreateScaleXWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Width", TEXT("宽度(mm)"), CreateScaleYWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateScaleZWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("GroundHeight", TEXT("离地高度(mm)"), CreateLocationZWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	FArmyDetailNode& FlagDetailNode = DetailCategory.AddChildNode("Flag", TEXT("标记"), CreateSocketFlagWidget());
	FlagDetailNode.ValueContentOverride().HAlign(HAlign_Fill);
	FlagDetailNode.IfVisibility = TAttribute<EVisibility>(this, &FArmyHydropowerDetail::IsFlagShow);


	DetailWidgetMap.Add(EHydropowerDetailType::Socket, OutletDetailBuilder->BuildDetail());
	DetailBuilderMap.Add(EHydropowerDetailType::Socket, OutletDetailBuilder);

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EHydropowerDetailType::Socket)->Get()->AsShared()
		];
}

void FArmyHydropowerDetail::CreateWaterDetail()
{
	WaterDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = WaterDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Radius", TEXT("管径(mm)"), CreateRadiusWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("GroundHeight", TEXT("离地高度(mm)"), CreateLocationZWidget()).ValueContentOverride().HAlign(HAlign_Fill);


	DetailWidgetMap.Add(EHydropowerDetailType::Water, WaterDetailBuilder->BuildDetail());
	DetailBuilderMap.Add(EHydropowerDetailType::Water, WaterDetailBuilder);

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EHydropowerDetailType::Water)->Get()->AsShared()
		];
}

void FArmyHydropowerDetail::CreatePrePTDetail()
{
	PrePTDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = PrePTDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Radius", TEXT("管径"), CreateRadiusWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("GroundHeight", TEXT("离地高度(mm)"), CreateLocationZWidget()).ValueContentOverride().HAlign(HAlign_Fill);


	DetailWidgetMap.Add(EHydropowerDetailType::PrePT, PrePTDetailBuilder->BuildDetail());
	DetailBuilderMap.Add(EHydropowerDetailType::PrePT, PrePTDetailBuilder);

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EHydropowerDetailType::PrePT)->Get()->AsShared()
		];
}

void FArmyHydropowerDetail::CreateLineDetail()
{
	LineDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = LineDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("长度(mm)"), CreateScaleXWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Radius", TEXT("管径"), CreateRadiusWidget()).ValueContentOverride().HAlign(HAlign_Fill);


	DetailCategory.AddChildNode("Group", TEXT("相连接"), CreateGroupWidget()).ValueContentOverride().HAlign(HAlign_Fill);


	DetailWidgetMap.Add(EHydropowerDetailType::Line, LineDetailBuilder->BuildDetail());
	DetailBuilderMap.Add(EHydropowerDetailType::Line, LineDetailBuilder);

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EHydropowerDetailType::Line)->Get()->AsShared()
		];
}

void FArmyHydropowerDetail::CreatePipeFittingDetail()
{
	PipeFittingDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = PipeFittingDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Radius", TEXT("管径"), CreateRadiusWidget()).ValueContentOverride().HAlign(HAlign_Fill);


	DetailWidgetMap.Add(EHydropowerDetailType::PipeFitting, PipeFittingDetailBuilder->BuildDetail());
	DetailBuilderMap.Add(EHydropowerDetailType::PipeFitting, PipeFittingDetailBuilder);

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EHydropowerDetailType::PipeFitting)->Get()->AsShared()
		];
}

void FArmyHydropowerDetail::CreateDefaultDetail()
{
	DefaultDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = DefaultDetailBuilder->AddCategory("GlobalCategory", TEXT("全局设置"));

	DetailCategory.AddChildNode("ComponentName", TEXT("墙体半透明"), CreateVisibility()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EHydropowerDetailType::None, DefaultDetailBuilder->BuildDetail());
	DetailBuilderMap.Add(EHydropowerDetailType::None, DefaultDetailBuilder);

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EHydropowerDetailType::None)->Get()->AsShared()
		];
}


TSharedRef<SWidget> FArmyHydropowerDetail::CreateVisibility()
{
	return
		SAssignNew(Visibly, SSpinBox<int32>)
        .Style(FArmyStyle::Get(), "SpinBoxStyle")
        .Font(FSlateFontInfo("PingFangSC-Regular", 12))
        .Delta(0.01f)
        .MinSliderValue(0)
        .MaxSliderValue(100)
        .MinValue(0)
        .MaxValue(100)
        .Value_Lambda([this]() {
            return FArmyHydropowerDataManager::Get()->GetCurrentOpcity();
        })
        .OnValueChanged_Lambda([this](int32 InValue) {
            FArmyHydropowerDataManager::Get()->SetCurrentOpacity(InValue);
            Opacity = InValue;
            FArmySceneData::Get()->SetHomeOpacity(Opacity / 100.f);
        })
        .OnValueCommitted_Lambda([this](int32 InValue, ETextCommit::Type CommitType) {
            if (CommitType == ETextCommit::OnEnter)
            {
                FArmyHydropowerDataManager::Get()->SetCurrentOpacity(InValue);
                Opacity = InValue;
                FArmySceneData::Get()->SetHomeOpacity(Opacity / 100.f);
            }
        });
}


TSharedPtr<SWidget> FArmyHydropowerDetail::CreateNameWidget()
{
	return
		SNew(SEditableTextBox)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.Text(this, &FArmyHydropowerDetail::GetFurnitureName)
		.IsReadOnly(true);
}

TSharedPtr<SWidget> FArmyHydropowerDetail::CreateScaleXWidget()
{
	return
		SNew(SEditableTextBox)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsReadOnly(true)
		.Text(this, &FArmyHydropowerDetail::GetLenght);
}

TSharedPtr<SWidget> FArmyHydropowerDetail::CreateScaleYWidget()
{
	return
		SNew(SEditableTextBox)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsReadOnly(true)
		.Text(this, &FArmyHydropowerDetail::GetWidth);
}

TSharedPtr<SWidget> FArmyHydropowerDetail::CreateScaleZWidget()
{
	return
		SNew(SEditableTextBox)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsReadOnly(true)
		.Text(this, &FArmyHydropowerDetail::GetHeight);
}

TSharedPtr<SWidget> FArmyHydropowerDetail::CreateRadiusWidget()
{
	return
		SNew(SEditableTextBox)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsReadOnly(true)
		.Text(this, &FArmyHydropowerDetail::GetRadius);
}

TSharedPtr<SWidget> FArmyHydropowerDetail::CreateLocationZWidget()
{
	return
		SNew(SDropDownTextBox)
		.Text(this, &FArmyHydropowerDetail::GetValueText)
		.OptionsSource(&GroundHeightArray)
		.OnTextChanged(this, &FArmyHydropowerDetail::OnGroundHeightChanged)
		.OnTextCommitted(this, &FArmyHydropowerDetail::OnGroundCommitted);
}

TSharedPtr<SWidget> FArmyHydropowerDetail::CreateSingleLocationZWidget()
{
	auto widget=	SNew(SEditableTextBox)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.Text(this, &FArmyHydropowerDetail::GetValueText)
		//.OptionsSource(&GroundHeightArray)
		//.OnTextChanged(this, &FArmyHydropowerDetail::OnGroundHeightChanged)
		.OnTextCommitted(this, &FArmyHydropowerDetail::OnGroundCommitted);
	SingleLocationZWidget = widget;
	CanSingleLocationZWidgetEdit = true;
	return widget;
}

void FArmyHydropowerDetail::SetSingleLocationZWidgetEditable(bool Value)
{
	if (SingleLocationZWidget.IsValid() && CanSingleLocationZWidgetEdit != Value)
	{
		CanSingleLocationZWidgetEdit = Value;
		if (CanSingleLocationZWidgetEdit)
		{
			SingleLocationZWidget.Pin()->SetStyle(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"));
		}
		else
		{
			SingleLocationZWidget.Pin()->SetStyle(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox.FF2F3033"));
		}
	}

}

TSharedPtr<SWidget> FArmyHydropowerDetail::CreateDrainWidget()
{
	return
		SNew(SComboBox< TSharedPtr<FArmyKeyValue> >)
		.OptionsSource(&DrainArray.Array)
		.MaxListHeight(200)
		.OnGenerateWidget(this, &FArmyHydropowerDetail::OnDrainGenerateWidget)
		.OnSelectionChanged(this, &FArmyHydropowerDetail::OnDrainTypeChanged)
		.Content()
		[
			SNew(STextBlock)
			.Text(this, &FArmyHydropowerDetail::GetDrainTypeName)
		];
}

TSharedPtr<SWidget> FArmyHydropowerDetail::CreateRefromWidget()
{
	//BreakPointWidget = CreateBreakPointWidget();
	return
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth()
		[
			SAssignNew(CheckBox, SCheckBox)
			.OnCheckStateChanged(this, &FArmyHydropowerDetail::CheckReform)
		.IsChecked_Raw(this, &FArmyHydropowerDetail::GetReform)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("SingleCheckBox"))
		]

	+ SHorizontalBox::Slot()
		.Padding(FMargin(10, 0, 0, 0))
		.FillWidth(1.f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("")))
		.TextStyle(FArmyStyle::Get(), "ArmyText_12")
		]
		]
	;

}

TSharedPtr<SWidget> FArmyHydropowerDetail::CreateGroupWidget()
{
	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SAssignNew(CheckBox, SCheckBox)
			.OnCheckStateChanged(this, &FArmyHydropowerDetail::CheckGroup)
			.Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("SingleCheckBox"))
			.IsChecked_Raw(this, &FArmyHydropowerDetail::GetGroup)
		]

	+ SHorizontalBox::Slot()
		.Padding(FMargin(10, 0, 0, 0))
		.FillWidth(1.f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("")))
		.TextStyle(FArmyStyle::Get(), "ArmyText_12")
		];
}

TSharedPtr<SWidget> FArmyHydropowerDetail::CreateSocketFlagWidget()
{
	return
		SNew(SArmyPropertyComboBox)
		.OptionsSource(FlagArray)
		.OnSelectionChanged(this, &FArmyHydropowerDetail::OnSlectedFlagChanged)
		.Value(this, &FArmyHydropowerDetail::GetFlag);
}

 void FArmyHydropowerDetail::UpdateOffset(FVector NewOffset)
{
	if (Offsets != NewOffset)
	{
		bool ShouldUpdateConstruction =
			(Offsets == FVector::ZeroVector&&NewOffset != FVector::ZeroVector)
			|| (NewOffset == FVector::ZeroVector&&Offsets != FVector::ZeroVector);

		Offsets = NewOffset;

		if (ShouldUpdateConstruction)
		{
			//刷新施工项
			UpdateConstructionOptions(SelectedType);
		}
	}
}

 TSharedPtr<SWidget> FArmyHydropowerDetail::CreateOffsetWidget(OffsetType Type)
 {
	 //.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox")) \	
#define OffsetWidget(OffsetTypeValue)	 SAssignNew(OffsetWidget[(int32)OffsetTypeValue], SEditableTextBox)   \
	.Text_Lambda([&]()->FText {  \
	return  FText::FromString(FString::FromInt((int32)((Offsets[(int32)OffsetTypeValue])*10))); })  \
		.OnTextCommitted_Lambda([&](const FText& InText, const ETextCommit::Type InTextAction)   \
	{ \
	float NewLength = (FCString::Atoi(*InText.ToString())/10.0f);  \
    NewLength= FMath::GridSnap(NewLength,0.1f); \
	FVector NewOffset=Offsets;\
	switch (OffsetTypeValue) \
		{  \
		case OffsetType::X:\
		NewOffset.X = NewLength; \
			break;  \
		case OffsetType::Y:\
		NewOffset.Y = NewLength; \
			break; \
		case OffsetType::Z:\
		NewOffset.Z = NewLength; \
			break;\
		}\
	TSharedPtr<FArmyHydropowerModeController> TempController = StaticCastSharedPtr<FArmyHydropowerModeController>(GGI->DesignEditor->CurrentController); \
	TempController->ApplyOffset(GetFurniture(true), NewOffset);\
})  \
	.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox")) 

	switch (Type)
	{
	case OffsetType::X:
		return	OffsetWidget(OffsetType::X);
	case OffsetType::Y:
		return	OffsetWidget(OffsetType::Y);
	case OffsetType::Z:
		return	OffsetWidget(OffsetType::Z);
	default :
		return	OffsetWidget(OffsetType::X);
	}

	
}

TSharedPtr<SWidget> FArmyHydropowerDetail::CreateReplaceWidget()
{	
	return SAssignNew(HyReplaceButton, SArmyReplaceButton)
		.ThumbnailBrush_Raw(this, &FArmyHydropowerDetail::GetThumbnailBrush)
	.OnClicked_Raw(this, &FArmyHydropowerDetail::OnReplaceButtonClicked);
}

TSharedPtr<SWidget> FArmyHydropowerDetail::CreateStopUsingWidget()
{
	return SNew(SHorizontalBox)

	+ SHorizontalBox::Slot()
		[
			SAssignNew(Start, SCheckBox)
			.Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("CheckBox.RadioBox"))
		
			.OnCheckStateChanged_Lambda([&] (ECheckBoxState InState){
			if(InState== ECheckBoxState::Checked)
			{
				UpdateOffset(NativeFurniture->GetOffset());
				TSharedPtr<FArmyHydropowerModeController> TempController = StaticCastSharedPtr<FArmyHydropowerModeController>(GGI->DesignEditor->CurrentController);
				TempController->StartOriginalPoint(GetFurniture(true));
				UpdateBreakPointUI(SelectedType);
				DetailWidgetSwitcher->SetActiveWidget(DetailWidgetMap.Find(SelectedType)->Get()->AsShared());
			}
	})
			.IsChecked_Lambda([this] {
			if (GetFurniture(true).IsValid() && (!GetFurniture(true)->IsBPStopped()))
			{
				return ECheckBoxState::Checked;
			}
			else
			{
				return ECheckBoxState::Unchecked;
			}
				})
		]
	+ SHorizontalBox::Slot()
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("启用")))

		.TextStyle(FArmyStyle::Get(), "ArmyText_12")
		]

	
	+ SHorizontalBox::Slot()
		[
			SAssignNew(Stop, SCheckBox)
			.Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("CheckBox.RadioBox"))
			.OnCheckStateChanged_Lambda([&](ECheckBoxState InState) {
			if (InState == ECheckBoxState::Checked)
			{
				TSharedPtr<FArmyHydropowerModeController> TempController = StaticCastSharedPtr<FArmyHydropowerModeController>(GGI->DesignEditor->CurrentController);
				TempController->StopOriginalPoint(GetFurniture(true));
				UpdateBreakPointUI(SelectedType);
				DetailWidgetSwitcher->SetActiveWidget(DetailWidgetMap.Find(SelectedType)->Get()->AsShared());
			}
			})
			.IsChecked_Lambda([this] {
			if (GetFurniture(true).IsValid() && GetFurniture(true)->IsBPStopped())
			{
				return ECheckBoxState::Checked;
			}
			else
			{
				return ECheckBoxState::Unchecked;
			}
	})
		]
	+ SHorizontalBox::Slot()
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(STextBlock)

			.Text(FText::FromString(TEXT("停用")))
		.TextStyle(FArmyStyle::Get(), "ArmyText_12")
		]
	;
}

void FArmyHydropowerDetail::StartOrStopReforming(ECheckBoxState InState)
{
	TSharedPtr<FArmyHydropowerModeController> TempController = StaticCastSharedPtr<FArmyHydropowerModeController>(GGI->DesignEditor->CurrentController);
	if (InState == ECheckBoxState::Checked && !GetFurniture(true)->IsBPStopped())
	{  
		TempController ->StartBPReform( GetFurniture(true));
		SetSingleLocationZWidgetEditable(true);
	}
	else if (InState == ECheckBoxState::Unchecked)
	{
		TempController->StopBPReform(GetFurniture(true));
		SetSingleLocationZWidgetEditable(false);
	}
	UpdateBreakPointUI(SelectedType);
}


void FArmyHydropowerDetail::StartReforming()
{
	GetFurniture(true)->StartReforming();
	UpdateBreakPointUI(SelectedType);
}

TSharedPtr<SWidget> FArmyHydropowerDetail::CreateBPReformCheckWidget()
{
	return
			SNew(SCheckBox)
			.Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("SingleCheckBox"))
			.IsChecked_Lambda([this] {
				if (GetFurniture(true)->IsBPreforming())
				{

					return ECheckBoxState::Checked;
				}else
				{
					return ECheckBoxState::Unchecked;
				}
			})
			.OnCheckStateChanged(this, &FArmyHydropowerDetail::StartOrStopReforming)
	;
}

TSharedPtr<FArmyFurniture> FArmyHydropowerDetail::GetFurniture(bool bNativeFurniture)
{
	if (bNativeFurniture)
	{
		return NativeFurniture;
	}
	return NativeFurniture->GetRepresentedFurniture(false);
}

const FSlateBrush * FArmyHydropowerDetail::GetThumbnailBrush() const
{
	TSharedPtr<FArmyFurniture> furniture = StaticCastSharedPtr<FArmyFurniture>(SelectObject.Pin());
	if (!furniture.IsValid())
	{
		return FArmyStyle::Get().GetBrush("Icon.DefaultImage");
	}

	TSharedPtr<FArmyFurniture> Furniture= furniture->GetRepresentedFurniture(false);
	if (Furniture == NativeFurniture)
	{
		return FArmyStyle::Get().GetBrush("Icon.DefaultImage");
	}

	UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
	TSharedPtr<FContentItemSpace::FContentItem> ActorItemInfo = ResMgr->GetContentItemFromID(Furniture->GetRelevanceActor()->GetSynID());

	return 	FArmySlateModule::Get().WebImageCache->Download(ActorItemInfo->ThumbnailURL).Get().GetBrush();
	
}


void FArmyHydropowerDetail::UpdateBreakPointUI(EHydropowerDetailType::HydropowerPropertyType Type)
{

	UpdateBreakPointUIData(Type);

	auto Builder = DetailBuilderMap.Find(Type)->Get()->AsShared();

	auto WidgetToRemove = *(DetailWidgetMap.Find(Type));
	DetailWidgetSwitcher->RemoveSlot(WidgetToRemove.ToSharedRef());
	auto WidgetToAdd = Builder->BuildDetail();
	DetailWidgetMap.Remove(Type);
	DetailWidgetMap.Add(Type, WidgetToAdd);
	DetailWidgetSwitcher->AddSlot()
		[
			WidgetToAdd.ToSharedRef()
		];
	DetailWidgetSwitcher->SetActiveWidget(DetailWidgetMap.Find(SelectedType)->Get()->AsShared());
}

void FArmyHydropowerDetail::UpdateBreakPointUIData(EHydropowerDetailType::HydropowerPropertyType Type)
{
	auto Builder = DetailBuilderMap.Find(Type)->Get()->AsShared();
	auto Furnature = GetFurniture(true);
	bool CanNativeReform = Furnature->CanNativeBPReform();

		//清楚断点改造所有内容
		auto GlobalCategory = Builder->GetCategory("GlobalCategory");
		if (GlobalCategory.IsValid())
		{
			GlobalCategory->RemoveChildNode("StopUsing");
			GlobalCategory->RemoveChildNode("RefromCheck"); //断点改造checbox
		}
		Builder->RemoveCategory("RefromCategory");

	if (!CanNativeReform)
	{
		//看看是不是能进行非原始点位的断点改造

		//检查是不是点位
		if (!SelectObject.Pin()->GetPropertyFlag(FArmyObject::FLAG_POINTOBJ))
		{
			return;
		}

		GlobalCategory->AddChildNode("RefromCheck", TEXT("断点改造"), CreateRefromWidget()).ValueContentOverride().HAlign(HAlign_Fill).VAlign(VAlign_Center);
		return;
	}

	bool CanStop = Furnature->CanBPStopUsing();
	bool CanReplace = Furnature->CanBPReplace();
	bool CanMove = Furnature->CanBPMove();

	if (CanStop)
	{
		auto GlobalCategory = Builder->GetCategory("GlobalCategory");
			GlobalCategory->AddChildNode("StopUsing", TEXT("原始点位状态"), CreateStopUsingWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	}

	if (Furnature->IsBPStopped())
	{
		return;
	}

	if (!Furnature->IsBPreforming())
	{
		auto GlobalCategory = Builder->GetCategory("GlobalCategory");
			GlobalCategory->AddChildNode("RefromCheck", TEXT("断点改造"), CreateBPReformCheckWidget()).ValueContentOverride().HAlign(EHorizontalAlignment::HAlign_Fill);
		return;
	}

		auto& ReformCategory = Builder->AddCategory("RefromCategory", TEXT("断点改造"), CreateBPReformCheckWidget());
		//ReformCategory.HAlign(EHorizontalAlignment::HAlign_Right)
		//ReformCategory.ValueContentOverride().HAlign(EHorizontalAlignment::HAlign_Right);
		if (CanMove)
		{
			//刷新位移数据
			UpdateOffset(Furnature->GetOffset());

			ReformCategory.AddChildNode("X", TEXT("位移X(mm)"), CreateOffsetWidget(OffsetType::X)).ValueContentOverride().HAlign(HAlign_Fill);
			ReformCategory.AddChildNode("Y", TEXT("位移Y(mm)"), CreateOffsetWidget(OffsetType::Y)).ValueContentOverride().HAlign(HAlign_Fill);
			ReformCategory.AddChildNode("Z", TEXT("位移Z(mm)"), CreateOffsetWidget(OffsetType::Z)).ValueContentOverride().HAlign(HAlign_Fill);
		}

		if (CanReplace)
		{
			ReformCategory.AddChildNode("Replace", TEXT("替换"), CreateReplaceWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		}
}

FReply FArmyHydropowerDetail::OnReplaceButtonClicked()
{
	if (HyReplaceList.IsValid())
	{
		GGI->WindowOverlay->RemoveSlot(HyReplaceList.ToSharedRef());
		HyReplaceList = nullptr;
		return FReply::Handled();
	}
	auto Furniture = GetFurniture(true);
	
	FString CategoryCode = FArmyCommonTools::GetCategoryCodeByLocalID(Furniture->GetComponentID());
	HyReplaceList = SNew(SArmyReplaceList)
		.HyPowerMode(true)
		.CategoryCode(CategoryCode)
		.OnReplace_Lambda([this](FContentItemPtr ContentItem) {

		TSharedPtr<FArmyHydropowerModeController> TempController = StaticCastSharedPtr<FArmyHydropowerModeController>(GGI->DesignEditor->CurrentController);
		TempController->Replace(ContentItem, GetFurniture(true));
		
		auto bush = this->GetThumbnailBrush();
		if (bush)
		{
			HyReplaceButton->SetSelectedImage(bush);
		}
		//关闭窗口
		if (HyReplaceList.IsValid())
		{
			GGI->WindowOverlay->RemoveSlot(HyReplaceList.ToSharedRef());
			HyReplaceList = nullptr;
		}
	})
		.OnClose_Lambda([this]() {
		GGI->WindowOverlay->RemoveSlot(HyReplaceList.ToSharedRef());
		HyReplaceList = nullptr;
	})
		.Title(FText::FromString(TEXT("替换")));
	GGI->WindowOverlay->AddSlot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Top)
		.Padding(0, 550, 242, 0)
		[
			HyReplaceList.ToSharedRef()
		];

	return FReply::Handled();
}


void FArmyHydropowerDetail::UpdateGlobalCategoryUI()
{
	UpdateGlobalCategoryData();

	auto WidgetToRemove = *(DetailWidgetMap.Find(SelectedType));
	DetailWidgetSwitcher->RemoveSlot(WidgetToRemove.ToSharedRef());

	auto Builder = DetailBuilderMap.Find(SelectedType)->Get()->AsShared();
	Builder->SortCatergory(CatergoryOrder);
	auto WidgetToAdd = Builder->BuildDetail();
	DetailWidgetMap.Remove(SelectedType);
	DetailWidgetMap.Add(SelectedType, WidgetToAdd);
	DetailWidgetSwitcher->AddSlot()
		[
			WidgetToAdd.ToSharedRef()
		];
}

void FArmyHydropowerDetail::UpdateGlobalCategoryData()
{

	auto Builder = DetailBuilderMap.Find(SelectedType)->Get()->AsShared();
	
	//清空物体详情
	{
		auto GlobalCategory = Builder->GetCategory("GlobalCategory");
		if (GlobalCategory.IsValid())
		{
			GlobalCategory->ClearChildren();
		}
		else
		{
			Builder->AddCategory("GlobalCategory", TEXT("物体详情"));
		}
	}

	auto GlobalCategory = Builder->GetCategory("GlobalCategory");
	
	switch (SelectedType)
	{
	case EHydropowerDetailType::OutLet:
		GlobalCategory->AddChildNode("ComponentName", TEXT("名称"), CreateNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		GlobalCategory->AddChildNode("Length", TEXT("长度(mm)"), CreateScaleXWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		GlobalCategory->AddChildNode("Width", TEXT("宽度(mm)"), CreateScaleYWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		GlobalCategory->AddChildNode("Height", TEXT("高度(mm)"), CreateScaleZWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		if (GetFurniture(true).IsValid() && GetFurniture(true)->Altitudes.Num() > 1)
		{
			//有多个高度
			GlobalCategory->AddChildNode("GroundHeight", TEXT("离地高度(mm)"), CreateLocationZWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		}
		else
		{
			GlobalCategory->AddChildNode("GroundHeight", TEXT("离地高度(mm)"), CreateSingleLocationZWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		}
		break;
	case EHydropowerDetailType::Socket:
		GlobalCategory->AddChildNode("ComponentName", TEXT("名称"), CreateNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		GlobalCategory->AddChildNode("Length", TEXT("长度(mm)"), CreateScaleXWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		GlobalCategory->AddChildNode("Width", TEXT("宽度(mm)"), CreateScaleYWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		GlobalCategory->AddChildNode("Height", TEXT("高度(mm)"), CreateScaleZWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		
		if (GetFurniture(true).IsValid() && GetFurniture(true)->Altitudes.Num() > 1)
		{
			//有多个高度
			GlobalCategory->AddChildNode("GroundHeight", TEXT("离地高度(mm)"), CreateLocationZWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		}
		else
		{
			GlobalCategory->AddChildNode("GroundHeight", TEXT("离地高度(mm)"), CreateSingleLocationZWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		}

		if (GetFurniture(true).IsValid() && GetFurniture(true)->IsNormalSocket())
		{
			GlobalCategory->AddChildNode("Flag", TEXT("标记"), CreateSocketFlagWidget());
		}
		break;
	case EHydropowerDetailType::Water:
	case EHydropowerDetailType::PrePT:
		GlobalCategory->AddChildNode("ComponentName", TEXT("名称"), CreateNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		GlobalCategory->AddChildNode("Radius", TEXT("管径"), CreateRadiusWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		if (GetFurniture(true).IsValid() && GetFurniture(true)->Altitudes.Num() > 1)
		{
			//有多个高度
			GlobalCategory->AddChildNode("GroundHeight", TEXT("离地高度(mm)"), CreateLocationZWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		}
		else
		{
			GlobalCategory->AddChildNode("GroundHeight", TEXT("离地高度(mm)"), CreateSingleLocationZWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		}
		break;
	case EHydropowerDetailType::Line:
		GlobalCategory->AddChildNode("ComponentName", TEXT("名称"), CreateNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		GlobalCategory->AddChildNode("Length", TEXT("长度(mm)"), CreateScaleXWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		GlobalCategory->AddChildNode("Radius", TEXT("管径"), CreateRadiusWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		GlobalCategory->AddChildNode("Group", TEXT("相连接"), CreateGroupWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		break;
	case EHydropowerDetailType::PipeFitting:
		GlobalCategory->AddChildNode("ComponentName", TEXT("名称"), CreateNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		GlobalCategory->AddChildNode("Radius", TEXT("管径"), CreateRadiusWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		break;

	case EHydropowerDetailType::None:
	default:
		GlobalCategory->SetNodeDisplayName(FText::FromString(TEXT("全局设置")));
		GlobalCategory->AddChildNode("ComponentName", TEXT("墙体半透明"), CreateVisibility()).ValueContentOverride().HAlign(HAlign_Fill);
		break;
	}

}

void FArmyHydropowerDetail::UpdateConstructionOptions(EHydropowerDetailType::HydropowerPropertyType Type)
{
	TSharedPtr<FArmyFurniture>SelectedFurniture = GetFurniture(true);

	auto TempController = dynamic_cast<FArmyHydropowerModeController*>(GGI->DesignEditor->CurrentController.Get());
	if (!TempController)
	{
		return;
	}
	TempController->OnConstructionDataGot.BindRaw(this, &FArmyHydropowerDetail::UpdateConstructionOptionsUI);
	
	//是不是原始点位
	bool IsNativePoint = false;
	//有没有发生移位
	bool HasOffset = false;
	//是不是替代品
	bool IsSubstitute = false;

	if (SelectedFurniture.IsValid() && (SelectedFurniture->IsOrignalPoint()|| SelectedFurniture->IsSustitute()))
	{
		IsNativePoint = true;
	}

	if(IsNativePoint)
	{
		HasOffset = SelectedFurniture->GetOffset() != FVector::ZeroVector;
		IsSubstitute = SelectedFurniture->IsReplaced() && SelectedFurniture->IsBPreforming() ;
	}

	if (IsNativePoint && (!IsSubstitute))
	{
		if (HasOffset)
		{
			//获取原始点位纯移位施工项
			TempController->RequestConstructionList(SelectedFurniture);
			return;
		}
		else
		{
			//原始点位既无移位也无改造，删除施工项；
			auto Builder = DetailBuilderMap.Find(SelectedType)->Get()->AsShared();
			Builder->RemoveCategory("Construction");
			return;
		}
	}
	//此时只能是替代点位或新增点位
	//获取非纯移位施工项
	AActor *SelectedActor = Cast<AActor>(SelectedUObject);
	//TempController->RequestConstructionList(SelectedActor, HasOffset, IsSubstitute);
	TempController->RequestConstructionList(SelectedActor);

}

void FArmyHydropowerDetail::UpdateConstructionOptionsUI(EConstructionRequeryResult ResultInfo, TSharedPtr<FJsonObject> ConstructionJsonData, TSharedPtr< FArmyConstructionItemInterface> CheckedData, ObjectConstructionKey Key)
{
	if (!SelectObject.IsValid())
	{
		return;
	}
	//清空数据  
	ConstructionData->RefreshCheckData(nullptr);

	int32 SpaceID = -1;
	if (SelectedRoom.IsValid())
	{
		SpaceID = SelectedRoom->GetSpaceId();
	}
	ConstructionData->Init(ConstructionJsonData, SpaceID);

	static TSharedPtr< FArmyConstructionItemInterface> CurrentCheckedData=nullptr;
	static ObjectConstructionKey CurrentKey;
	CurrentKey = Key;
	CurrentCheckedData = CheckedData;

   ConstructionData->RefreshCheckData(CurrentCheckedData);

	auto Nodes = ConstructionData->GetDetailNodes();
	auto Builder = DetailBuilderMap.Find(SelectedType)->Get()->AsShared();
	if (Nodes.Num() <= 0)
	{
		//清除施工项
		Builder->RemoveCategory("Construction");
	}
	else
	{
		auto ConstructionCategory = Builder->GetCategory("Construction");
		if (!ConstructionCategory.IsValid())
		{
			Builder->AddCategory("Construction", TEXT("施工项"), nullptr);
			ConstructionCategory = Builder->GetCategory("Construction");
		}
		ConstructionCategory->ClearChildren();
		for (auto ConstructionNode:Nodes)
		{
			ConstructionCategory->AddChildNode(ConstructionNode);
		}
	}

	auto WidgetToRemove = *(DetailWidgetMap.Find(SelectedType));
	DetailWidgetSwitcher->RemoveSlot(WidgetToRemove.ToSharedRef());

	Builder->SortCatergory(CatergoryOrder);
	auto WidgetToAdd = Builder->BuildDetail();
	DetailWidgetMap.Remove(SelectedType);
	DetailWidgetMap.Add(SelectedType, WidgetToAdd);
	DetailWidgetSwitcher->AddSlot()
		[
			WidgetToAdd.ToSharedRef()
		];

	/*让施工项面板根据勾选自动展开*/
	ConstructionData->RefreshItemExpand(Builder->GetTreeView());
	ConstructionData->ConstructionStateChangeDelegate.BindLambda([&]() {
		CurrentCheckedData->bHasSetted = true;
		CurrentCheckedData->SetConstructionItemCheckedId(this->ConstructionData->GetCheckedData());
		XRConstructionManager::Get()->SaveConstructionData(CurrentKey.ObjectID, CurrentKey.Parameters, CurrentCheckedData);
	});

	TSharedPtr<FArmyFurniture>SelectedFurniture = GetFurniture(true);

	DetailWidgetSwitcher->SetActiveWidget(DetailWidgetMap.Find(SelectedType)->Get()->AsShared());
}

void FArmyHydropowerDetail::InitShowType()
{
	EObjectType TempSelectType = SelectObject.Pin()->GetType();
	SelectedType = EHydropowerDetailType::None;


	if (TempSelectType == OT_Component_None/*无构件的开关、插座、点位*/ || TempSelectType == OT_ComponentBase)
	{
		TSharedPtr<FArmyFurniture> furniture = StaticCastSharedPtr<FArmyFurniture>(SelectObject.Pin());
		if (furniture->IsEleBox() 
			|| furniture->IsSwitch()
			|| furniture->IsGasMeter() //燃气表
			|| furniture->IsWaterSeparator()
			)/*电箱*//*开关**/
		{
			SelectedType = EHydropowerDetailType::OutLet;
		}
		if (furniture->IsSocket())
		{
			SelectedType = EHydropowerDetailType::Socket;
		}
	}

	/*冷热水 上水点位 中水点位*/
	if (TempSelectType == OT_ComponentBase) {
		TSharedPtr<FArmyFurniture> furniture = StaticCastSharedPtr<FArmyFurniture>(SelectObject.Pin());
		if (furniture->IsToilet()  
			|| furniture->IsWaterSupply() 
			|| furniture->IsWaterNormalPoint()
			)
		{
			SelectedType = EHydropowerDetailType::Water;
		}
	}

	/*预装点位*/
	if (TempSelectType == OT_ComponentBase)
	{
		TSharedPtr<FArmyFurniture> furniture = StaticCastSharedPtr<FArmyFurniture>(SelectObject.Pin());
		if (furniture->IsPreLoad())
		{
			SelectedType = EHydropowerDetailType::PrePT;
		}
	}
	if (TempSelectType == OT_Drain_Point)
	{
		SelectedType = EHydropowerDetailType::PrePT;
	}

	/*线路*/
	if (SelectObject.Pin()->IsPipeLine()) {
		SelectedType = EHydropowerDetailType::Line;
	}

	/*管件*/
	if (SelectObject.Pin()->IsPipeLineLinker()) {
		SelectedType = EHydropowerDetailType::PipeFitting;
	}

	if (SelectObject.Pin()->IsPipeLine())
	{
		ShowType = EHydropowerDetailType::PROPERTY_LENGHT | EHydropowerDetailType::PROPERTY_RADIUS | EHydropowerDetailType::RefromType;
	}
	if (TempSelectType == OT_HotWaterTube_Linker || TempSelectType == OT_ColdWaterTube_Linker || TempSelectType == OT_Drain_Linker)
	{
		ShowType = EHydropowerDetailType::RefromType | EHydropowerDetailType::PROPERTY_RADIUS;
	}
}

bool FArmyHydropowerDetail::IsRefrom()
{
	if (SelectObject.IsValid())
	{
		TSharedPtr<FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectObject.Pin());
		if (Furniture.IsValid())
		{
			return Furniture->GetType() != OT_Water_Supply;
		}
	}
	return true;
}

bool FArmyHydropowerDetail::IsShowGroundHeight() const
{
	if (SelectObject.IsValid())
	{
		if (!(SelectObject.Pin()->GetType() > OT_Hydropower_Begin &&
			SelectObject.Pin()->GetType() < OT_Hydropower_End))
		{
			return true;
		}
	}
	return false;
}

bool FArmyHydropowerDetail::IsDrain()
{
	if (SelectObject.IsValid())
	{
		TSharedPtr<FArmyPipeline> Line = StaticCastSharedPtr<FArmyPipeline>(SelectObject.Pin());
	}
	return false;
}

void FArmyHydropowerDetail::InitGroundHeightArray()
{
	GroundHeightArray.Reset();
	if (!IsShowGroundHeight())
		return;
	TSharedPtr< FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectObject.Pin());
	TArray<float> Data = Furniture->Altitudes;
	for (int32 i = 0; i < Data.Num(); i++)
		GroundHeightArray.Add(MakeShareable(new float(Data[i])));
}

void FArmyHydropowerDetail::ConfigDetailScrollBox(int32 _Type)
{
	DetailScrollBox->ClearChildren();

	TSharedPtr<SArmyDetailWidget> DetailWidget =
		SNew(SArmyDetailWidget)
		.TitleText(FText::FromString(TEXT("属性")));

	DetailWidget->AddDetailItem(

		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("名字")))
		.TextStyle(FCoreStyle::Get(), "VRSText_10")
		]
	+ SHorizontalBox::Slot()
		.Padding(FMargin(10, 0, 0, 0))
		.FillWidth(1.f)
		[
			SNew(SEditableTextBox)
			.Text(this, &FArmyHydropowerDetail::GetFurnitureName)
		.IsEnabled(false)
		]

	);
	if ((_Type & EHydropowerDetailType::PROPERTY_LENGHT) == EHydropowerDetailType::PROPERTY_LENGHT)
	{
		DetailWidget->AddDetailItem
		(
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("长度(mm)")))
			.TextStyle(FCoreStyle::Get(), "VRSText_10")
			]
		+ SHorizontalBox::Slot()
			.Padding(FMargin(10, 0, 0, 0))
			.FillWidth(1.f)
			[
				SNew(SEditableTextBox)
				.Text(this, &FArmyHydropowerDetail::GetLenght)
			.IsEnabled(false)
			]
		);
	}
	if ((_Type & EHydropowerDetailType::PROPERTY_WIDTH) == EHydropowerDetailType::PROPERTY_WIDTH)
	{
		DetailWidget->AddDetailItem
		(
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("宽度(mm)")))
			.TextStyle(FCoreStyle::Get(), "VRSText_10")
			]
		+ SHorizontalBox::Slot()
			.Padding(FMargin(10, 0, 0, 0))
			.FillWidth(1.f)
			[
				SNew(SEditableTextBox)
				.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
			.IsReadOnly(true)
			.Text(this, &FArmyHydropowerDetail::GetWidth)
			]
		);
	}
	if ((_Type & EHydropowerDetailType::PROPERTY_HEIGHT) == EHydropowerDetailType::PROPERTY_HEIGHT)
	{
		DetailWidget->AddDetailItem
		(
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("高度(mm)")))
			.TextStyle(FCoreStyle::Get(), "VRSText_10")
			]
		+ SHorizontalBox::Slot()
			.Padding(FMargin(10, 0, 0, 0))
			.FillWidth(1.f)
			[
				SNew(SEditableTextBox)
				.Text(this, &FArmyHydropowerDetail::GetHeight)
			.IsEnabled(false)
			]
		);
	}
	if ((_Type & EHydropowerDetailType::PROPERTY_RADIUS) == EHydropowerDetailType::PROPERTY_RADIUS)
	{
		DetailWidget->AddDetailItem
		(
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("管径(mm)")))
			.TextStyle(FCoreStyle::Get(), "VRSText_10")
			]
		+ SHorizontalBox::Slot()
			.Padding(FMargin(10, 0, 0, 0))
			.FillWidth(1.f)
			[
				SNew(SEditableTextBox)
				.Text(this, &FArmyHydropowerDetail::GetRadius)
			.IsEnabled(false)
			]
		);
	}
	if ((_Type&EHydropowerDetailType::GroundHeight) == EHydropowerDetailType::GroundHeight)
	{
		DetailWidget->AddDetailItem(

			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("离地高度")))
			.TextStyle(FCoreStyle::Get(), "VRSText_10")
			]
		+ SHorizontalBox::Slot()
			.Padding(FMargin(10, 0, 0, 0))
			.FillWidth(1.f)
			[
				SNew(SDropDownTextBox)
				.Text(this, &FArmyHydropowerDetail::GetValueText)
			.OptionsSource(&GroundHeightArray)
			//.OnTextChanged (this,&FArmyHydropowerDetail::OnGroundHeightChanged)
			.OnTextCommitted(this, &FArmyHydropowerDetail::OnGroundCommitted)
			]
		+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("mm")))
			.TextStyle(FCoreStyle::Get(), "VRSText_10")
			]
		);
	}

	if ((_Type & EHydropowerDetailType::TrapType) == EHydropowerDetailType::TrapType)
	{
		DetailWidget->AddDetailItem
		(
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("存水弯")))
			.TextStyle(FCoreStyle::Get(), "VRSText_10")
			]
		+ SHorizontalBox::Slot()
			.Padding(FMargin(10, 0, 0, 0))
			.FillWidth(1.f)
			[
				SNew(SComboBox< TSharedPtr<FArmyKeyValue> >)
				.OptionsSource(&DrainArray.Array)
			.MaxListHeight(200)
			.OnGenerateWidget(this, &FArmyHydropowerDetail::OnDrainGenerateWidget)
			.OnSelectionChanged(this, &FArmyHydropowerDetail::OnDrainTypeChanged)
			.Content()
			[
				SNew(STextBlock)
				.Text(this, &FArmyHydropowerDetail::GetDrainTypeName)
			]

			]
		);
	}

	if ((_Type & EHydropowerDetailType::RefromType) == EHydropowerDetailType::RefromType)
	{
		DetailWidget->AddDetailItem(
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(CheckBox, SCheckBox)
				.OnCheckStateChanged(this, &FArmyHydropowerDetail::CheckReform)
			.IsChecked_Raw(this, &FArmyHydropowerDetail::GetReform)
			]
		+ SHorizontalBox::Slot()
			.Padding(FMargin(10, 0, 0, 0))
			.FillWidth(1.f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("")))
			.TextStyle(FCoreStyle::Get(), "VRSText_10")
			]
		);
	}
	DetailScrollBox->AddSlot()
		[
			DetailWidget.ToSharedRef()
		];
}

FText FArmyHydropowerDetail::GetLenght() const
{
	if (SelectObject.IsValid())
	{
		if (SelectObject.Pin()->IsPipeLine() )
		{
			//先判断是不是选中了一组管

			TSharedPtr<FArmyPipeline> Line = StaticCastSharedPtr<FArmyPipeline>(SelectObject.Pin());
			AXRGroupActor* InActorParent = nullptr;
			bool SelectedAGroup = false;
			AActor* actor = Line->GetRelevanceActor();
			if (actor->IsValidLowLevel())
			{
				InActorParent = AXRGroupActor::GetParentForActor(actor);
				if (InActorParent&&InActorParent->IsValidLowLevel())
				{
					SelectedAGroup = true;
				}
			}

			if(!SelectedAGroup)
			{ 
				int32 Lenght = Line->GetLenght() * 10;
				return FText::AsNumber(Lenght);
			}
			else
			{
				int32 Lenght = 0;
				for (const auto & Actor : InActorParent->GroupActors)
				{
					auto LineActor=Cast<AXRPipelineActor>(Actor);
					if (!LineActor)
					{
						continue;
					}

					auto CurrentLine=StaticCastSharedPtr<FArmyPipeline>(LineActor->GetRelevanceObject().Pin());
					Lenght+= CurrentLine->GetLenght() * 10;
				}
				return FText::AsNumber(Lenght);

			}
		}
		else
		{
			return FText::FromString(FString::Printf(TEXT("%.2f"), CachedScaleX));
		}
	}
	return FText();
}

FText FArmyHydropowerDetail::GetWidth() const
{
	return FText::FromString(FString::Printf(TEXT("%.2f"), CachedScaleY));
}

FText FArmyHydropowerDetail::GetHeight() const
{
	return FText::FromString(FString::Printf(TEXT("%.2f"), CachedScaleZ));
}

FText FArmyHydropowerDetail::GetRadius() const
{
	if (SelectObject.IsValid())
	{
		if (SelectObject.Pin()->IsPipeLine())
		{
			TSharedPtr<FArmyPipeline> Line = StaticCastSharedPtr<FArmyPipeline>(SelectObject.Pin());
			return FText::FromString(*Line->GetRadiusStr());
		}
		else if (SelectObject.Pin()->GetType() >= OT_StrongElectricity_25_Linker && SelectObject.Pin()->GetType() <= OT_ElectricLinker)
		{
			TSharedPtr<FArmyPipePoint> Point = StaticCastSharedPtr<FArmyPipePoint>(SelectObject.Pin());
			return FText::FromString(*Point->GetRadiusStr());
		}
		else if (SelectObject.Pin()->GetRelevanceActor() && SelectObject.Pin()->GetRelevanceActor()->Tags.Contains(XRActorTag::OriginalPoint))
		{
			//原始点位
			TSharedPtr<FArmyFurniture> FurnitureComponent = StaticCastSharedPtr<FArmyFurniture>(SelectObject.Pin());
			if (FurnitureComponent.IsValid() && FurnitureComponent->GetFurniturePro().IsValid())
			{
				return FText::FromString(FString::Printf(TEXT("%.2f"), FurnitureComponent->GetFurniturePro()->GetRadius() * 10.f));
			}
			return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
		}
		else if (SelectObject.Pin()->GetType() == OT_ComponentBase)
		{
			TSharedPtr<FArmyFurniture> FurnitureObj = StaticCastSharedPtr<FArmyFurniture>(SelectObject.Pin());
			AActor* RelevanceActor = FurnitureObj->GetRelevanceActor();
			if (RelevanceActor)
			{
				UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
				TSharedPtr<FContentItemSpace::FContentItem> Item = ResMgr->GetContentItemFromID(RelevanceActor->GetSynID());
				if (Item.IsValid())
				{
					TSharedPtr<FHydropowerProductObj> ProductObjPtr = StaticCastSharedPtr<FHydropowerProductObj>(Item->ProObj);
					if (ProductObjPtr.IsValid())
					{
						return FText::FromString(FString::SanitizeFloat(ProductObjPtr->Radius));
					}
				}
			}
		}
	}
	return FText();
}

ECheckBoxState FArmyHydropowerDetail::GetReform() const
{
	if (SelectObject.IsValid())
	{
		TSharedPtr<FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectObject.Pin());
		TSharedPtr<FArmyPipeline> line = StaticCastSharedPtr<FArmyPipeline>(SelectObject.Pin());
		TSharedPtr<FArmyPipePoint> point = StaticCastSharedPtr<FArmyPipePoint>(SelectObject.Pin());
		if (SelectObject.Pin()->GetType() == OT_ComponentBase || SelectObject.Pin()->GetType() == OT_Component_None)
		{
			return  Furniture->bReform ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
		}
		else if (SelectObject.Pin()->IsPipeLine())
		{
			return  line->bReform ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
		}
		else if (SelectObject.Pin()->IsPipeLineLinker())
		{
			return  point->bReform ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
		}
	}
	return ECheckBoxState::Unchecked;
}

void FArmyHydropowerDetail::CheckReform(ECheckBoxState InState)
{
	if (CheckBox.IsValid() && CheckBox->GetCheckedState() != InState)
	{
		if (InState == ECheckBoxState::Checked)
		{

		}
		AActor* actor = SelectObject.Pin()->GetRelevanceActor();
		if (actor && actor->IsValidLowLevel())
		{
			AXRGroupActor* InActorParent = AXRGroupActor::GetParentForActor(actor);
			if (InActorParent && InActorParent->IsValidLowLevel())
			{
				TArray<AActor*> OutGroupActors;
				InActorParent->GetGroupActors(OutGroupActors);
				for (AActor* Actor : OutGroupActors)
				{
					AXRActor* ActorItem = Cast<AXRActor>(Actor);
					if (ActorItem&& ActorItem->IsValidLowLevel())
					{
						FObjectWeakPtr ObjectItem = ActorItem->GetRelevanceObject();
						if (ObjectItem.IsValid())
						{
							TSharedPtr<FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(ObjectItem.Pin());
							TSharedPtr<FArmyPipeline> line = StaticCastSharedPtr<FArmyPipeline>(ObjectItem.Pin());
							TSharedPtr<FArmyPipePoint> point = StaticCastSharedPtr<FArmyPipePoint>(ObjectItem.Pin());
							if (ObjectItem.Pin()->GetType() == OT_ComponentBase|| SelectObject.Pin()->GetType() == OT_Component_None)
							{
								Furniture->bReform = InState == ECheckBoxState::Checked ? true : false;
							}
							else if (ObjectItem.Pin()->IsPipeLine())
							{
								line->SetReformState(InState == ECheckBoxState::Checked ? true : false);

							}
							else if (ObjectItem.Pin()->IsPipeLineLinker())
							{
								point->bReform = InState == ECheckBoxState::Checked ? true : false;
								point->ChangeColor();
							}
						}

					}

				}
			}
			else
			{
				TSharedPtr<FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectObject.Pin());
				TSharedPtr<FArmyPipeline> line = StaticCastSharedPtr<FArmyPipeline>(SelectObject.Pin());
				TSharedPtr<FArmyPipePoint> point = StaticCastSharedPtr<FArmyPipePoint>(SelectObject.Pin());
				if (SelectObject.Pin()->GetType() == OT_ComponentBase || SelectObject.Pin()->GetType() == OT_Component_None)
				{
					Furniture->bReform = InState == ECheckBoxState::Checked ? true : false;
				}
				else if (SelectObject.Pin()->IsPipeLine())
				{
					line->SetReformState(InState == ECheckBoxState::Checked ? true : false);

				}
				else if (SelectObject.Pin()->IsPipeLineLinker())
				{
					point->bReform = InState == ECheckBoxState::Checked ? true : false;
					point->ChangeColor();
				}
			}
		}
	}
}

FText FArmyHydropowerDetail::GetValueText() const
{

	if (SelectedUObject&&IsShowGroundHeight())
	{
		AActor* SelectedActor = Cast<AActor>(SelectedUObject);
		float LocationZ = SelectedActor->GetActorLocation().Z;
		FText Text = FText::FromString(FString::FormatAsNumber(LocationZ * 10));
		return Text;
	}
	return FText();
}

TSharedRef<SWidget> FArmyHydropowerDetail::OnDrainGenerateWidget(TSharedPtr<FArmyKeyValue> InItem)
{
	return
		SNew(SBox)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.Padding(FMargin(10, 0, 0, 0))
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[
			SNew(STextBlock)
			.Text(FText::FromString(InItem->Value))
		.TextStyle(FCoreStyle::Get(), "VRSText_10")
		]
		];
}

void FArmyHydropowerDetail::OnDrainTypeChanged(TSharedPtr<FArmyKeyValue> NewSelection, ESelectInfo::Type SelectInfo)
{
}

FText FArmyHydropowerDetail::GetDrainTypeName() const
{
	return FText();
}

void FArmyHydropowerDetail::OnGroundHeightChanged(const FText& text)
{

	float Value = FCString::Atof(*text.ToString());
	if (SelectObject.IsValid() && IsShowGroundHeight())
	{
		TSharedPtr<FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectObject.Pin());
		Furniture->SetAltitude(Value / 10.f);
		Furniture->UpdateData();
	}
	if (SelectedUObject)
	{
		AActor* SelectedActor = Cast<AActor>(SelectedUObject);
		FVector Location = SelectedActor->GetActorLocation();
		Location.Z = Value / 10.f;
		SelectedActor->SetActorLocation(Location);
	}
}

void FArmyHydropowerDetail::OnGroundCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (!CanSingleLocationZWidgetEdit)
	{
		return;
	}

	if (InTextAction == ETextCommit::OnEnter)
	{
		float Value = FCString::Atof(*InText.ToString());
		if (SelectObject.IsValid() && IsShowGroundHeight())
		{
			TSharedPtr<FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectObject.Pin());
			Furniture->SetAltitude(Value / 10.f);
			Furniture->UpdateData();
		}
		if (SelectedUObject)
		{
			AActor* SelectedActor = Cast<AActor>(SelectedUObject);
			FVector Location = SelectedActor->GetActorLocation();
			Location.Z = Value / 10.f;
			SelectedActor->SetActorRelativeLocation(Location);
		}
	}

}

void FArmyHydropowerDetail::OnSlectedFlagChanged(const int32 Key, const FString& Value)
{
	if (SelectObject.IsValid())
	{
		EObjectType TempSelectType = SelectObject.Pin()->GetType();
		if (TempSelectType == OT_ComponentBase)
		{
			TSharedPtr<FArmyFurniture> furniture = StaticCastSharedPtr<FArmyFurniture>(SelectObject.Pin());
			if (furniture->IsNormalSocket())/*电箱*//*开关*//*插座*/
			{
				furniture->SetSocketType((ESocketFlag)Key);
			}
		}
	}
}

FText FArmyHydropowerDetail::GetFlag() const
{
	if (SelectObject.IsValid())
	{
		EObjectType TempSelectType = SelectObject.Pin()->GetType();
		if (TempSelectType == OT_ComponentBase)
		{
			TSharedPtr<FArmyFurniture> furniture = StaticCastSharedPtr<FArmyFurniture>(SelectObject.Pin());
			if (furniture->IsNormalSocket())/*电箱*//*开关*//*插座*/
			{
				int32 Key = furniture->GetSocketType();
				TSharedPtr<FArmyKeyValue> KeyValue = FlagArray->FindByKey(Key);
				if (KeyValue.IsValid())
				{
					return FText::FromString(KeyValue->Value);
				}
			}
		}
	}
	FText text = FText::FromString(TEXT(""));
	return text;
}

ECheckBoxState FArmyHydropowerDetail::GetGroup() const
{
	if (!SelectObject.IsValid())
		return ECheckBoxState::Unchecked;
	if (SelectObject.Pin()->IsPipeLine())
	{
		TSharedPtr<FArmyPipeline> line = StaticCastSharedPtr<FArmyPipeline>(SelectObject.Pin());
		AActor* actor = line->GetRelevanceActor();
		if (actor->IsValidLowLevel())
		{
			AXRGroupActor* InActorParent = AXRGroupActor::GetParentForActor(actor);
			if (InActorParent&&InActorParent->IsValidLowLevel())
			{
				return ECheckBoxState::Checked;
			}
		}
	}
	return ECheckBoxState::Unchecked;
}

void FArmyHydropowerDetail::CheckGroup(ECheckBoxState InState)
{
	if (CheckBox.IsValid() && ECheckBoxState::Checked == InState)
	{
		TSharedPtr<FArmyPipeline> line = StaticCastSharedPtr<FArmyPipeline>(SelectObject.Pin());
		if (SelectObject.Pin()->IsPipeLine())
		{
			TArray<FObjectPtr> Lines;
			line->GetConnectLines(Lines);
			TArray<AActor*> Actors;
			for (int32 i = 0; i < Lines.Num(); i++)
			{
				Actors.AddUnique(Lines[i]->GetRelevanceActor());
			}
			GXREditor->CreateGroupActor(Actors);
		}
	}
	else
	{
		GXREditor->edactUngroupFromSelected();
	}
	if (SelectObject.Pin()->IsPipeLine())
	{
		TSharedPtr<FArmyPipeline> line = StaticCastSharedPtr<FArmyPipeline>(SelectObject.Pin());
		AActor* actor = line->GetRelevanceActor();
		if (actor->IsValidLowLevel())
		{
			const bool bShouldSelect = true;
			const bool bNotifyAfterSelect = false;
			const bool bSelectEvenIfHidden = true;
	
			GXREditor->SelectNone(true, true);
			GXREditor->SelectActor(actor, bShouldSelect, bNotifyAfterSelect, bSelectEvenIfHidden);
			GXREditor->NoteSelectionChange();
		}
	}

}

EVisibility FArmyHydropowerDetail::IsFlagShow() const
{
	if (SelectObject.IsValid())
	{
		EObjectType TempSelectType = SelectObject.Pin()->GetType();
		if (TempSelectType == OT_ComponentBase)
		{
			TSharedPtr<FArmyFurniture> furniture = StaticCastSharedPtr<FArmyFurniture>(SelectObject.Pin());
			if (furniture->IsNormalSocket())/*电箱*//*开关*//*插座*/
			{
				return EVisibility::Visible;
			}
		}
	}
	return EVisibility::Collapsed;
}

