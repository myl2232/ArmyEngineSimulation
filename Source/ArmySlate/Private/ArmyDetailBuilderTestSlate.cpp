//#include "SArmyDetailBuilderTest.h"
//#include "ArmyDetailBuilder.h"
//#include "Runtime/AppFramework/Public/Widgets/Colors/SColorPicker.h"
//#include "ArmySlateModule.h"
//
//void SArmyDetailBuilderTest::Construct(const FArguments& InArgs)
//{
//	ChildSlot
//		[
//			CreateTestDetailWidget().ToSharedRef()
//		];
//}
//
//TSharedPtr<SWidget> SArmyDetailBuilderTest::CreateTestDetailWidget()
//{
//	DetailBuilder = MakeShareable(new FArmyDetailBuilder());
//
//	//-----------------------------常用组件分类---------------------------------
//	FArmyDetailNode& Category0 = DetailBuilder->AddCategory("Category_CommonWidgetTypeOne", TEXT("常用组件分类"));
//
//	//可编辑文字
//	Category0.AddChildNode("EditableText", TEXT("可编辑文字框"), CreateEditableTextDetail());
//
//	//位移XYZ设置演示
//	FArmyDetailNode& Group0 = Category0.AddChildNode("Group_Translate", TEXT("位移"));
//	Group0.AddChildNode("Property0", TEXT("X轴"))
//		.ValueContentOverride()
//		.HAlign(HAlign_Fill)
//		[
//			CreateActorTranslateXDetail()
//		];
//	Group0.AddChildNode("Property1", TEXT("Y轴"))
//		.ValueContentOverride()
//		.HAlign(HAlign_Fill)
//		[
//			CreateActorTranslateYDetail()
//		];
//	Group0.AddChildNode("Property2", TEXT("Z轴"))
//		.ValueContentOverride()
//		.HAlign(HAlign_Fill)
//		[
//			CreateActorTranslateZDetail()
//		];
//
//	//按钮内嵌文字演示
//	FArmyDetailNode& Group1 = Category0.AddChildNode("Group_Button", TEXT("按钮"));
//	Group1.AddChildNode("Property0", TEXT("按钮名称"))
//		.ValueContentOverride()
//		.HAlign(HAlign_Fill)
//		[
//			CreateColorButtonDetail()
//		];
//	Group1.AddChildNode("Property1", TEXT("按钮名称"), SNew(SButton));
//	Group1.AddChildNode("Property2")
//		.WholeContentOverride()
//		[
//			CreateTextButtonDetail()
//		];
//
//	//勾选，单选，复选项演示
//	FArmyDetailNode& Group2 = Category0.AddChildNode("Group_CheckBox", TEXT("勾/单/复选框"));
//	Group2.AddChildNode("Property0", TEXT("勾选项名称"), CreateCheckBoxDetail());
//	Group2.AddChildNode("Property1", TEXT("单选项名称"), CreateSingleCheckBoxDetail());
//	Group2.AddChildNode("Property2", TEXT("复选项名称"), CreateMultiCheckBoxDetail())
//		.ValueContentOverride()
//		[
//			CreateMultiCheckBoxDetail()
//		]
//		.NameContentOverride()
//		.VAlign(VAlign_Top);
//		
//
//	//下拉列表
//	FArmyDetailNode& Group3 = Category0.AddChildNode("Group_ComboBox", TEXT("下拉列表"));
//	Group3.AddChildNode("Property0", TEXT("文字列表主题"), CreateTextComboBoxDetail());
//	Group3.AddChildNode("Property1", TEXT("图片列表主题"), CreateTextureComboBoxDetail());
//
//	//-----------------------------扩展复杂的组件---------------------------------
//	FArmyDetailNode& Category1 = DetailBuilder->AddCategory("Category_ComplexWidgetTypeOne", TEXT("复杂的组件分类"));
//
//	CachedCheckBox1State = ECheckBoxState::Unchecked;
//	//左边勾选框，右边SpinBox
//	Category1.AddChildNode("Property0")
//		.NameContentOverride()
//		[
//			SNew(SHorizontalBox)
//			+ SHorizontalBox::Slot()
//			.AutoWidth()
//			[
//				SNew(SCheckBox)
//				.IsChecked(this, &SArmyDetailBuilderTest::GetCheckBox1State)
//				.OnCheckStateChanged(this, &SArmyDetailBuilderTest::OnCheckBox1StateChanged)
//			]
//			+ SHorizontalBox::Slot()
//			.HAlign(HAlign_Left)
//			[
//				SNew(STextBlock)
//				.Text(FText::FromString(TEXT("启用数值调整")))
//				.TextStyle(FArmyStyle::Get(), "ArmyText_10")
//				.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 1)))
//			]
//		]
//		.ValueContentOverride()
//		[
//			SNew(SSpinBox<float>)
//			.Value(this, &SArmyDetailBuilderTest::GetSpinBoxValue)
//			.Delta(1.f)
//			.MinSliderValue(-10000.f)
//			.MaxSliderValue(10000.f)
//			.MinValue(TNumericLimits<float>::Lowest())
//			.MaxValue(TNumericLimits<float>::Max())
//			.OnValueChanged(this, &SArmyDetailBuilderTest::OnSpinBoxValueChanged)
//			.IsEnabled(this, &SArmyDetailBuilderTest::GetSpinBoxValueEnabled)
//		];
//
//
//	//扩展的组
//	FArmyDetailNode& Group10 = Category1.AddChildNode("Group10")
//		.NameContentOverride()
//		[
//			SNew(SButton)
//		]
//		.ValueContentOverride()
//		[
//			SNew(SButton)
//		];
//	Group10.AddChildNode("Property0", TEXT("属性1"))
//		.ValueContentOverride()
//		.HAlign(HAlign_Fill)
//		[
//			SNew(SSpinBox<float>)
//		];
//	Group10.AddChildNode("Property1", TEXT("属性2"))
//		.ValueContentOverride()
//		.HAlign(HAlign_Fill)
//		[
//			SNew(SSpinBox<float>)
//		];
//	Group10.AddChildNode("Property2", TEXT("属性3"))
//		.ValueContentOverride()
//		.HAlign(HAlign_Fill)
//		[
//			SNew(SSpinBox<float>)
//		];
//	Group10.AddChildNode("Property3")
//		.WholeContentOverride()
//		.HAlign(HAlign_Fill)
//		[
//			SNew(SBox)
//			//.HeightOverride(600)
//			[
//				SNew(SWrapBox)
//				.UseAllottedWidth(true)
//				+ SWrapBox::Slot()
//					[SNew(SButton)]
//				+ SWrapBox::Slot()
//					[SNew(SButton)]
//				+ SWrapBox::Slot()
//					[SNew(SButton)]
//				+ SWrapBox::Slot()
//					[SNew(SButton)]
//				+ SWrapBox::Slot()
//					[SNew(SButton)]
//				+ SWrapBox::Slot()
//					[SNew(SButton)]
//				+ SWrapBox::Slot()
//					[SNew(SButton)]
//				+ SWrapBox::Slot()
//					[SNew(SButton)]
//				+ SWrapBox::Slot()
//					[SNew(SButton)]
//				+ SWrapBox::Slot()
//					[SNew(SButton)]
//				+ SWrapBox::Slot()
//					[SNew(SButton)]
//				+ SWrapBox::Slot()
//					[SNew(SButton)]
//				+ SWrapBox::Slot()
//					[SNew(SButton)]
//				+ SWrapBox::Slot()
//					[SNew(SButton)]
//				+ SWrapBox::Slot()
//					[SNew(SButton)]
//			]
//		];
//
//	return DetailBuilder->BuildDetail();
//}
//
//TSharedRef<SWidget> SArmyDetailBuilderTest::CreateActorTranslateXDetail()
//{
//	CachedLocationX = 0.f;
//	return
//		SNew(SSpinBox<float>)
//		//.Style(&GRes->SpinBoxStyle)
//		.Value(this, &SArmyDetailBuilderTest::GetLocationX)
//		.Delta(1.f)
//		.MinSliderValue(-10000.f)
//		.MaxSliderValue(10000.f)
//		.MinValue(TNumericLimits<float>::Lowest())
//		.MaxValue(TNumericLimits<float>::Max())
//		.OnValueChanged(this, &SArmyDetailBuilderTest::OnActorLocationXChanged);
//		//.OnValueCommitted(this, &SArmyDetailBuilderTest::GetLocationX)
//		//.TypeInterface(InArgs._TypeInterface)
//}
//
//TSharedRef<SWidget> SArmyDetailBuilderTest::CreateActorTranslateYDetail()
//{
//	CachedLocationY = 0.f;
//	return
//		SNew(SSpinBox<float>)
//		//.Style(&GRes->SpinBoxStyle)
//		.Value(this, &SArmyDetailBuilderTest::GetLocationY)
//		.Delta(1.f)
//		.MinSliderValue(-10000.f)
//		.MaxSliderValue(10000.f)
//		.MinValue(TNumericLimits<float>::Lowest())
//		.MaxValue(TNumericLimits<float>::Max())
//		.OnValueChanged(this, &SArmyDetailBuilderTest::OnActorLocationYChanged);
//}
//
//TSharedRef<SWidget> SArmyDetailBuilderTest::CreateActorTranslateZDetail()
//{
//	CachedLocationZ = 0.f;
//	return
//		SNew(SSpinBox<float>)
//		//.Style(&GRes->SpinBoxStyle)
//		.Value(this, &SArmyDetailBuilderTest::GetLocationZ)
//		.Delta(1.f)
//		.MinSliderValue(-10000.f)
//		.MaxSliderValue(10000.f)
//		.MinValue(TNumericLimits<float>::Lowest())
//		.MaxValue(TNumericLimits<float>::Max())
//		.OnValueChanged(this, &SArmyDetailBuilderTest::OnActorLocationZChanged);
//}
//
//TSharedRef<SWidget> SArmyDetailBuilderTest::CreateTextButtonDetail()
//{
//	return
//		SNew(SButton)
//		.OnClicked(this, &SArmyDetailBuilderTest::OnButtonClicked)
//		.HAlign(HAlign_Center)
//		.VAlign(VAlign_Center)
//		[
//			SNew(STextBlock)
//			.Text(FText::FromString(TEXT("按钮名称")))
//			.TextStyle(FArmyStyle::Get(), "ArmyText_10")
//			.ColorAndOpacity(FSlateColor(FLinearColor(0, 0, 0, 1)))
//		];
//}
//
//TSharedRef<SWidget> SArmyDetailBuilderTest::CreateColorButtonDetail()
//{
//	CachedPickColor = FLinearColor(1,0,0,1);
//	return
//		SNew(SButton)
//		.OnClicked(this, &SArmyDetailBuilderTest::OnColorButtonClicked)
//		.ContentPadding(FMargin(0))
//		.Content()
//		[
//			SNew(SBorder)
//			.BorderBackgroundColor(this, &SArmyDetailBuilderTest::GetPickBorderColor)
//			.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
//		];
//}
//
//TSharedRef<SWidget> SArmyDetailBuilderTest::CreateCheckBoxDetail()
//{
//	return
//		SNew(SHorizontalBox)
//		+ SHorizontalBox::Slot()
//		.AutoWidth()
//		[
//			SNew(SCheckBox)
//			.IsChecked(this, &SArmyDetailBuilderTest::GetCheckBoxState)
//			.OnCheckStateChanged(this, &SArmyDetailBuilderTest::OnCheckBoxStateChanged)
//		]
//		+ SHorizontalBox::Slot()
//		.HAlign(HAlign_Left)
//		[
//			SNew(STextBlock)
//			.Text(FText::FromString(TEXT("单选项名称")))
//			.TextStyle(FArmyStyle::Get(), "ArmyText_10")
//			.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 1)))
//		];
//}
//
//TSharedRef<SWidget> SArmyDetailBuilderTest::CreateSingleCheckBoxDetail()
//{
//	SingleCheckBoxData.Reset();
//	SingleCheckBoxData.Add(SArmyCheckBoxGroup::FCheckBoxData(TEXT("选项一"), ECheckBoxState::Checked));
//	SingleCheckBoxData.Add(SArmyCheckBoxGroup::FCheckBoxData(TEXT("选项二")));
//	SingleCheckBoxData.Add(SArmyCheckBoxGroup::FCheckBoxData(TEXT("选项三")));
//	SingleCheckBoxData.Add(SArmyCheckBoxGroup::FCheckBoxData(TEXT("选项四")));
//
//	return SNew(SArmyCheckBoxGroup)
//			.ItemSource(&SingleCheckBoxData)
//			.bAutoWrap(true)
//			.bMultiCheck(false)
//			.OnCheckStateChanged(this, &SArmyDetailBuilderTest::OnSingleCheckBoxStateChanged);
//}
//
//TSharedRef<SWidget> SArmyDetailBuilderTest::CreateMultiCheckBoxDetail()
//{
//	MultiCheckBoxData.Reset();
//	MultiCheckBoxData.Add(SArmyCheckBoxGroup::FCheckBoxData(TEXT("选项一"), ECheckBoxState::Checked));
//	MultiCheckBoxData.Add(SArmyCheckBoxGroup::FCheckBoxData(TEXT("选项二")));
//	MultiCheckBoxData.Add(SArmyCheckBoxGroup::FCheckBoxData(TEXT("选项三"), ECheckBoxState::Checked));
//	MultiCheckBoxData.Add(SArmyCheckBoxGroup::FCheckBoxData(TEXT("选项四")));
//
//	return SNew(SArmyCheckBoxGroup)
//		.ItemSource(&MultiCheckBoxData)
//		.bAutoWrap(false)
//		.bMultiCheck(true)
//		.OnCheckStateChanged(this, &SArmyDetailBuilderTest::OnMultiCheckBoxStateChanged);
//}
//
//TSharedRef<SWidget> SArmyDetailBuilderTest::CreateTextComboBoxDetail()
//{
//	TextComboBoxSource.Reset();
//	TextComboBoxSource.Add(MakeShareable(new FString(TEXT("选项一"))));
//	TextComboBoxSource.Add(MakeShareable(new FString(TEXT("选项二"))));
//	TextComboBoxSource.Add(MakeShareable(new FString(TEXT("选项三"))));
//	TextComboBoxSource.Add(MakeShareable(new FString(TEXT("选项四"))));
//
//	return
//	SAssignNew(TextComboBoxWidget, SArmyTextComboBox)
//		.ComboItemList(TextComboBoxSource)
//		.OnComboBoxSelectionChanged(this, &SArmyDetailBuilderTest::OnTextComboTextChanged)
//		.InitialSelected(*(TextComboBoxSource[0].Get()));
//}
//
//TSharedRef<SWidget> SArmyDetailBuilderTest::CreateTextureComboBoxDetail()
//{
//	/*UArmyResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
//	if (ResMgr)
//	{
//		TextureComboBoxSource.Reset();
//		TextureComboBoxSource.Add(ResMgr->EmptyTexture);
//		for (auto& It : ResMgr->IESList)
//		{
//			TextureComboBoxSource.Add(It);
//		}
//	}*/
//
//	UTexture* Tex = LoadObject<UTexture>(NULL, TEXT("/Game/ArmyCommon/Texture/T_Ground_Grass_N.T_Ground_Grass_N"));
//	UTexture* Tex1 = LoadObject<UTexture>(NULL, TEXT("/Game/ArmyCommon/Material/Default/T_Floor0.T_Floor0"));
//	UTexture* Tex2 = LoadObject<UTexture>(NULL, TEXT("/Game/ArmyCommon/Material/Default/T_Wall.T_Wall"));
//	TextureComboBoxSource.Reset();
//	TextureComboBoxSource.Add(Tex);
//	TextureComboBoxSource.Add(Tex1);
//	TextureComboBoxSource.Add(Tex2);
//
//	return
//		SAssignNew(TextureComboBoxWidget, SArmyTextureComboBox)
//		.ComboItemList(TextureComboBoxSource)
//		.OnComboBoxSelectionChanged(this, &SArmyDetailBuilderTest::OnTextureComboTextChanged)
//		.InitialSelected(TextureComboBoxSource[0]);
//}
//
//TSharedRef<SWidget> SArmyDetailBuilderTest::CreateEditableTextDetail()
//{
//	CachedEditableText = FText::FromString(TEXT("可编辑文字内容"));
//	return
//		SNew(SEditableTextBox)
//		.Text(this, &SArmyDetailBuilderTest::GetEditableText)
//		//.Style(&GRes->EditableTextBoxStyle)
//		.OnTextChanged(this, &SArmyDetailBuilderTest::OnEditableTextChanged);
//		//.OnTextCommitted(InArgs._OnTextCommitted);
//}
//
//FReply SArmyDetailBuilderTest::OnColorButtonClicked()
//{
//	FColorPickerArgs PickerArgs;
//	PickerArgs.InitialColorOverride = CachedPickColor;
//	PickerArgs.InitialColorOverride.A = 1.f;
//	PickerArgs.bUseAlpha = false;
//	PickerArgs.DisplayGamma = TAttribute<float>::Create(TAttribute<float>::FGetter::CreateUObject(GEngine, &UEngine::GetDisplayGamma));
//	PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &SArmyDetailBuilderTest::OnPickColorChanged);
//	OpenColorPicker(PickerArgs);
//
//	return FReply::Handled();
//}
//
//void SArmyDetailBuilderTest::OnTextComboTextChanged(const FString& InString)
//{
//
//}
//
//void SArmyDetailBuilderTest::OnTextureComboTextChanged(UTexture* InTexture)
//{
//
//}
