#include "ArmyLayoutModeDetail.h"
#include "ArmyObject.h"
#include "SWidgetSwitcher.h"
#include "ArmySingleDoor.h"
#include "ArmyWindow.h"
#include "ArmyPass.h"
#include "ArmyRoom.h"
#include "SArmyEditableNumberBox.h"
#include "SComboBox.h"
#include "ArmyGameInstance.h"
#include "ArmyRectBayWindow.h"
#include "ArmyUser.h"
#include "ArmySingleDoorDetail.h"
#include "ArmyTrapeBayWindow.h"
#include "ArmyModifyWall.h"
#include "ArmyPunch.h"
#include "ArmySimpleComponent.h"
#include "ArmyBearingWall.h"

#include "SArmyMulitCategory.h"
#include "FArmyConstructionItemInterface.h"
#include "SColorPicker.h"
#include "ArmyRegion.h"


TSharedRef<SWidget> FArmyLayoutModeDetail::CreateDetailWidget()
{
	Init();

    FArmyUser::Get().OnSpaceTypeListReady.AddRaw(this, &FArmyLayoutModeDetail::OnSpaceTypeListReady);					
	FArmyUser::Get().OnConstructionNewPassDataReady.BindRaw(this, &FArmyLayoutModeDetail::OnConstructionOriginWallNewPassReady);

	FArmyUser::Get().OnConstructionOriginPassDataReady.BindRaw(this, &FArmyLayoutModeDetail::OnConstructionOriginPassReady);		
	FArmyUser::Get().OnConstructionPackPipeDataReady.BindRaw(this, &FArmyLayoutModeDetail::OnConstructionPackPipeReady);


	ConstructionModifyWallDetail = MakeShareable(new SArmyMulitCategory);	

	ConstructionAddWallDetail = MakeShareable(new SArmyMulitCategory);	

	ConstructionIndepentWallDetail = MakeShareable(new SArmyMulitCategory);	

	ConstructionNewPassDetail = MakeShareable(new SArmyMulitCategory);		

	ConstructionSingleDoorDetail = MakeShareable(new SArmyMulitCategory);

	ConstructionSlidingDoorDetail = MakeShareable(new SArmyMulitCategory);

	ConstructionPassDetail = MakeShareable(new SArmyMulitCategory);

	ConstructionDoorHoleDetail = MakeShareable(new SArmyMulitCategory);

	ConstructionPackPipeDetail = MakeShareable(new SArmyMulitCategory);

	ConstructionItemPassDetail = MakeShareable(new SArmyMulitCategory);

	SAssignNew(DetailWidgetSwitcher, SWidgetSwitcher);
	CreateWallSettingDetail();		

	//点位属性
	CreateDrainPointDetail();
	CreateClosestoolDetail();

	CreateAddWallDetail();
	CreateIndependentWall();
	CreateModifyWallDetail();

	DetailWidget = DetailWidgetSwitcher;
	//HiddenDetailWidget(EVisibility::Hidden);
	return DetailWidget->AsShared();
}

void FArmyLayoutModeDetail::ShowSelectedDetial(TSharedPtr<FArmyObject> InSelectedObject)
{
	SelectedXRObject = InSelectedObject;
	if (SelectedXRObject.IsValid()) {
		CachedName = SelectedXRObject->GetName();
		
		TSharedPtr<SWidget> *TempWidget;
        TempWidget = DetailWidgetMap.Find(SelectedXRObject->GetType());

		if (TempWidget)
        {						
			ExeWhileShowDetail(SelectedXRObject->GetType());
			TempWidget = DetailWidgetMap.Find(SelectedXRObject->GetType());
			DetailWidgetSwitcher->SetActiveWidget(TempWidget->Get()->AsShared());
			HiddenDetailWidget(EVisibility::Visible);			
		}
		else
        {						
			SelectedXRObject = nullptr;			
			DetailWidgetSwitcher->SetActiveWidget(WallSettingWidget->AsShared());
		}		
	}
	else
	{		
		SelectedXRObject = nullptr;
		DetailWidgetSwitcher->SetActiveWidget(WallSettingWidget->AsShared());
	}	
}

void FArmyLayoutModeDetail::ShowSelectedDetial(TSharedPtr<FArmyObject> InSelectedObject, bool bHiddenSelected)
{
	if (bHiddenSelected)
	{
		SelectedXRObject = InSelectedObject;
		if (SelectedXRObject.IsValid())
		{
			TSharedPtr<SWidget> *TempWidget;
			TempWidget = DetailWidgetMap.Find(SelectedXRObject->GetType());

			if (TempWidget)
			{
				ExeWhileShowDetail(SelectedXRObject->GetType());								
			}			
		}		
	}
	else
	{
		ShowSelectedDetial(InSelectedObject);
	}
}

void FArmyLayoutModeDetail::Init()
{
    SelectedObjectAttribute = TAttribute< TWeakPtr<FArmyObject> >::Create(TAttribute< TWeakPtr<FArmyObject> >::FGetter::CreateRaw(this, &FArmyLayoutModeDetail::GetSelectedObject));

	ModifyWallMatList = MakeShareable(new FArmyComboBoxArray);
	ModifyWallMatList->Add(MakeShareable(new FArmyKeyValue(0, TEXT("轻体砖"))));
	ModifyWallMatList->Add(MakeShareable(new FArmyKeyValue(1, TEXT("红砖"))));
	ModifyWallMatList->Add(MakeShareable(new FArmyKeyValue(2, TEXT("轻钢龙骨隔墙"))));
	ModifyWallMatList->Add(MakeShareable(new FArmyKeyValue(3, TEXT("保温板+石膏板保温"))));
	ModifyWallMatList->Add(MakeShareable(new FArmyKeyValue(4, TEXT("轻体砖保温填充材质"))));

	AddWallMatList = MakeShareable(new FArmyComboBoxArray);
	AddWallMatList->Add(MakeShareable(new FArmyKeyValue(0, TEXT("轻体砖"))));
	AddWallMatList->Add(MakeShareable(new FArmyKeyValue(1, TEXT("红砖"))));
	AddWallMatList->Add(MakeShareable(new FArmyKeyValue(2, TEXT("轻钢龙骨隔墙"))));
	AddWallMatList->Add(MakeShareable(new FArmyKeyValue(3, TEXT("保温板+石膏板保温"))));
	AddWallMatList->Add(MakeShareable(new FArmyKeyValue(4, TEXT("轻体砖保温填充材质"))));

	AddDoorHoleMatList = MakeShareable(new FArmyComboBoxArray);
	AddDoorHoleMatList->Add(MakeShareable(new FArmyKeyValue(0, TEXT("大芯板"))));
	AddDoorHoleMatList->Add(MakeShareable(new FArmyKeyValue(1, TEXT("轻体砖"))));

	ModifyDoorHoleMatList = MakeShareable(new FArmyComboBoxArray);
	ModifyDoorHoleMatList->Add(MakeShareable(new FArmyKeyValue(0, TEXT("轻体砖"))));
	ModifyDoorHoleMatList->Add(MakeShareable(new FArmyKeyValue(1, TEXT("红砖"))));
	ModifyDoorHoleMatList->Add(MakeShareable(new FArmyKeyValue(2, TEXT("轻钢龙骨隔墙"))));
	ModifyDoorHoleMatList->Add(MakeShareable(new FArmyKeyValue(3, TEXT("保温板+石膏板保温"))));
	ModifyDoorHoleMatList->Add(MakeShareable(new FArmyKeyValue(4, TEXT("轻体砖保温填充材质"))));

	PassMatList = MakeShareable(new FArmyComboBoxArray);
	PassMatList->Add(MakeShareable(new FArmyKeyValue(0, TEXT("轻体砖"))));
	PassMatList->Add(MakeShareable(new FArmyKeyValue(1, TEXT("红砖"))));
	PassMatList->Add(MakeShareable(new FArmyKeyValue(2, TEXT("轻钢龙骨隔墙"))));
	PassMatList->Add(MakeShareable(new FArmyKeyValue(3, TEXT("保温板+石膏板保温"))));
	PassMatList->Add(MakeShareable(new FArmyKeyValue(4, TEXT("轻体砖保温填充材质"))));

	NewPassMatList = MakeShareable(new FArmyComboBoxArray);
	NewPassMatList->Add(MakeShareable(new FArmyKeyValue(0, TEXT("轻体砖"))));
	NewPassMatList->Add(MakeShareable(new FArmyKeyValue(1, TEXT("红砖"))));
	NewPassMatList->Add(MakeShareable(new FArmyKeyValue(2, TEXT("轻钢龙骨隔墙"))));
	NewPassMatList->Add(MakeShareable(new FArmyKeyValue(3, TEXT("保温板+石膏板保温"))));
	NewPassMatList->Add(MakeShareable(new FArmyKeyValue(4, TEXT("轻体砖保温填充材质"))));
	
	SlidingMatList = MakeShareable(new FArmyComboBoxArray);
	SlidingMatList->Add(MakeShareable(new FArmyKeyValue(0, TEXT("轻体砖"))));
	SlidingMatList->Add(MakeShareable(new FArmyKeyValue(1, TEXT("红砖"))));
	SlidingMatList->Add(MakeShareable(new FArmyKeyValue(2, TEXT("轻钢龙骨隔墙"))));
	SlidingMatList->Add(MakeShareable(new FArmyKeyValue(3, TEXT("保温板+石膏板保温"))));
	SlidingMatList->Add(MakeShareable(new FArmyKeyValue(4, TEXT("轻体砖保温填充材质"))));
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateCommonNameWidget()
{
	return
		SNew(SEditableTextBox)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.Text(this, &FArmyLayoutModeDetail::GetName)
		.IsReadOnly(true);
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateCommonLengthWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetCommonLength)
		.OnTextCommitted_Raw(this, &FArmyLayoutModeDetail::OnCommonLengthChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true);
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateCommonHeightWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetCommonHeight)
		.OnTextCommitted_Raw(this, &FArmyLayoutModeDetail::OnCommonHeightChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true);
}

void FArmyLayoutModeDetail::CreateWallSettingDetail()
{
	WallDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = WallDetailBuilder->AddCategory("GlobalCategory", TEXT("全局设置"));

	DetailCategory.AddChildNode("WallHeight", TEXT("墙体高度(mm)"), SNew(SArmyEditableNumberBox)
		.IsEnabled(false)
		.Text(this, &FArmyLayoutModeDetail::GetWallHeight)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		.SelectAllTextWhenFocused(true)
    ).ValueContentOverride().HAlign(HAlign_Fill);

    DetailCategory.AddChildNode("WallColor", TEXT("墙线颜色"), SNew(SButton)
        .OnClicked_Lambda([this]() {
            FColorPickerArgs PickerArgs;
            PickerArgs.InitialColorOverride = FArmySceneData::Get()->GetWallLinesColor();
	        PickerArgs.bUseAlpha = true;
	        PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &FArmyLayoutModeDetail::OnWallColorChanged);
	        OpenColorPicker(PickerArgs);
	        return FReply::Handled();
        })
        .ContentPadding(0)
        .Content()
        [
            SNew(SBorder)
            .Padding(FMargin(0))
            .HAlign(HAlign_Fill)
            .BorderBackgroundColor_Lambda([this]() {
                return FArmySceneData::Get()->GetWallLinesColor();
            })
            .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
        ]
    ).ValueContentOverride().HAlign(HAlign_Fill);

    DetailCategory.AddChildNode("ImageTransparency", TEXT("底图透明度(%)"), SNew(SSpinBox<int32>)
        .Style(FArmyStyle::Get(), "SpinBoxStyle")
        .Font(FSlateFontInfo("PingFangSC-Regular", 12))
        .Delta(0.01f)
        .MinSliderValue(0)
        .MaxSliderValue(100)
        .MinValue(0)
        .MaxValue(100)
        .IsEnabled_Lambda([this]() {
            return FArmySceneData::Get()->GetFacsimile().IsValid();
        })
        .Value_Lambda([this]() {
            return FArmySceneData::Get()->GetFacsimileTransparency();
        })
        .OnValueChanged_Lambda([this](int32 InValue) {
            FArmySceneData::Get()->SetFacsimileTransparency(InValue);
        })
        .OnValueCommitted_Lambda([this](int32 InValue, ETextCommit::Type CommitType) {
            if (CommitType == ETextCommit::OnEnter)
            {
                FArmySceneData::Get()->SetFacsimileTransparency(InValue);
            }
        })
    ).ValueContentOverride().HAlign(HAlign_Fill);

	WallSettingWidget = WallDetailBuilder->BuildDetail();
	DetailWidgetSwitcher->AddSlot()
		[
			WallSettingWidget->AsShared()
		];	
}

void FArmyLayoutModeDetail::CreateSingleDoorDetail()
{
	ConstructionSingleDoorDetail->Init(FArmyUser::Get().ConstructionOriginPassData);

	SingleDoorDetail = MakeShareable(new FArmySingleDoorDetail());
	TSharedPtr<SWidget> SingleDoorDetailWidget = SingleDoorDetail->CreateDetailWidget(SelectedObjectAttribute, ConstructionSingleDoorDetail);
	DetailWidgetMap.Add(EObjectType::OT_Door, SingleDoorDetailWidget);

	DetailWidgetSwitcher->AddSlot()
		[
            SingleDoorDetailWidget->AsShared()
		];

	ConstructionSingleDoorDetail->RefreshItemExpand(SingleDoorDetail->GetDetailBuilder()->GetTreeView());
}

void FArmyLayoutModeDetail::CreateSlidingDoorDetail()
{
	SlidingDoorDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = SlidingDoorDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Width", TEXT("宽度(mm)"), CreateCommonLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateSlidingHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
    DetailCategory.AddChildNode("OpenDirection", TEXT("开门方向"), CreateOpenDirectionWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_SlidingDoor, SlidingDoorDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_SlidingDoor)->Get()->AsShared()
		];
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateOpenDirectionWidget()
{
    if (!OpenDirectionList.IsValid())
    {
        OpenDirectionList = MakeShareable(new FArmyComboBoxArray);
        OpenDirectionList->Add(MakeShareable(new FArmyKeyValue(0, TEXT("请选择开门方向"))));
        OpenDirectionList->Add(MakeShareable(new FArmyKeyValue(1, TEXT("内开/左开"))));
        OpenDirectionList->Add(MakeShareable(new FArmyKeyValue(2, TEXT("内开/右开"))));
        OpenDirectionList->Add(MakeShareable(new FArmyKeyValue(3, TEXT("外开/左开"))));
        OpenDirectionList->Add(MakeShareable(new FArmyKeyValue(4, TEXT("外开/右开"))));
    }

    return
        SNew(SArmyPropertyComboBox)
        .OptionsSource(OpenDirectionList)
        .OnSelectionChanged_Lambda([this](const int32 Key, const FString& Value) {
            TSharedPtr<FArmySlidingDoor> SlidingDoor = StaticCastSharedPtr<FArmySlidingDoor>(SelectedXRObject);
	        if (SlidingDoor.IsValid())
	        {
                SlidingDoor->SetOpenDirectionID(Key);
	        }
        })
        .Value_Lambda([this]() {
            TSharedPtr<FArmySlidingDoor> SlidingDoor = StaticCastSharedPtr<FArmySlidingDoor>(SelectedXRObject);
            if (SlidingDoor.IsValid())
            {
                TSharedPtr<FArmyKeyValue> Selection = OpenDirectionList->FindByKey(SlidingDoor->GetOpenDirectionID());
                if (Selection.IsValid())
                {
                    return FText::FromString(Selection->Value);
                }
            }

            return FText();
        });
}

void FArmyLayoutModeDetail::CreateSpaceDetail(bool bIsModify)
{
	SpaceDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = SpaceDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("空间名称"), CreateSpaceNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Area", TEXT("面积(㎡)"), CreateSpaceAreaWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Perimeter", TEXT("周长(m)"), CreateSpacePerimeterWidget()).ValueContentOverride().HAlign(HAlign_Fill);	
	if (!bIsModify)
	{
		DetailCategory.AddChildNode("GroundHeight", TEXT("完成面高度(mm)"), CreateSpaceFinishGroundHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	}	

	TSharedPtr<SWidget> *TempWidget = DetailWidgetMap.Find(EObjectType::OT_InternalRoom);
	if (TempWidget)
	{
		DetailWidgetSwitcher->RemoveSlot(TempWidget->Get()->AsShared());
	}

	DetailWidgetMap.Add(EObjectType::OT_InternalRoom, SpaceDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_InternalRoom)->Get()->AsShared()
		];
}

void FArmyLayoutModeDetail::CreatePassDetail()
{
	PassDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = PassDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("宽度(mm)"), CreatePassLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreatePassHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Generate", TEXT("包边"), CreatePassCheckBoxDetail()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("FillPass", TEXT("填门洞"), CreateFillPassCheckBoxDetail()).ValueContentOverride().HAlign(HAlign_Fill);	
	
	FArmyDetailNode& ConstructionItemCategory = PassDetailBuilder->AddCategory("ConstructionItem", TEXT("施工项"));
	for (auto It : ConstructionPassDetail->GetDetailNodes())
	{
		ConstructionItemCategory.AddChildNode(It);
	}

	TSharedPtr<SWidget> *TempWidget = DetailWidgetMap.Find(EObjectType::OT_Pass);
	if (TempWidget)
	{
		DetailWidgetSwitcher->RemoveSlot(TempWidget->Get()->AsShared());
	}

	DetailWidgetMap.Add(EObjectType::OT_Pass, PassDetailBuilder->BuildDetail());
	
	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_Pass)->Get()->AsShared()
		];
	
	ConstructionPassDetail->RefreshItemExpand(PassDetailBuilder->GetTreeView());

	if (!DetailWidgetMap.Find(EObjectType::OT_Door))
	{
		OnConstructionNewWallSingleDoorReady();
	}

	if (!DetailWidgetMap.Find(EObjectType::OT_SlidingDoor))
	{
		OnConstructionNewWallSlidingDoorReady();
	}
}

void FArmyLayoutModeDetail::CreateNewPassDetail()
{
	NewPassDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = NewPassDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));
	TSharedPtr<FArmyNewPass> Pass = StaticCastSharedPtr<FArmyNewPass>(SelectedXRObject);
	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("宽度(mm)"), CreateCommonLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateCommonHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	if (Pass.IsValid() && Pass->InWallType == 0)
	{
		DetailCategory.AddChildNode("MatType", TEXT("墙体材质"), CreateNewPassMatTypeWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	}
	DetailCategory.AddChildNode("Generate", TEXT("包边"), CreateNewPassCheckBoxDetail()).ValueContentOverride().HAlign(HAlign_Fill);	

	FArmyDetailNode& ConstructionItemCategory = NewPassDetailBuilder->AddCategory("ConstructionItem", TEXT("施工项"));

	for (auto It : ConstructionNewPassDetail->GetDetailNodes())
	{
		ConstructionItemCategory.AddChildNode(It);
	}

	//多次添加之前先清空
	TSharedPtr<SWidget> *TempWidget = DetailWidgetMap.Find(EObjectType::OT_NewPass);
	if (TempWidget)
	{
		DetailWidgetSwitcher->RemoveSlot(TempWidget->Get()->AsShared());
	}

	DetailWidgetMap.Add(EObjectType::OT_NewPass, NewPassDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_NewPass)->Get()->AsShared()
		];
	ConstructionNewPassDetail->RefreshItemExpand(NewPassDetailBuilder->GetTreeView());
}

void FArmyLayoutModeDetail::CreateDoorHoleDetail()
{
	DoorHoleDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = DoorHoleDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("AddDoorHole", TEXT("补门洞"), CreateAddDoorHoleMatTypeWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("ModifyDoorHole", TEXT("拆门洞"), CreateModifyDoorHoleMatTypeWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	FArmyDetailNode& ConstructionItemCategory = DoorHoleDetailBuilder->AddCategory("ConstructionItem", TEXT("施工项"));

	for (auto It : ConstructionDoorHoleDetail->GetDetailNodes())
	{
		ConstructionItemCategory.AddChildNode(It);
	}	

	DetailWidgetMap.Add(EObjectType::OT_DoorHole, DoorHoleDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_DoorHole)->Get()->AsShared()
		];
}

void FArmyLayoutModeDetail::CreateSplitRegionDetail()
{
	SplitRegionDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = SplitRegionDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("空间名称"), CreateRegionSpaceNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("RegionName", TEXT("区域名称"), CreateSplitRegionNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Area", TEXT("面积(㎡)"), CreateSplitRegionAreaWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Perimeter", TEXT("周长(m)"), CreateSplitRegionPerimeterWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_Region/*OT_SplitRegion*/, SplitRegionDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_Region/*OT_SplitRegion*/)->Get()->AsShared()
		];
}

void FArmyLayoutModeDetail::CreateModifyWallDetail()
{
	ModifyWallDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = ModifyWallDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("宽度(mm)"), CreateModifyWallLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);	
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateModifyWallHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("FloorDist", TEXT("离地高度(mm)"), CreateModifyWallFloorDistWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Material", TEXT("材质"), CreateModifyWallMatTypeWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	FArmyDetailNode& ConstructionItemCategory = ModifyWallDetailBuilder->AddCategory("ConstructionItem", TEXT("施工项"));

	for (auto It : ConstructionModifyWallDetail->GetDetailNodes())
	{
		ConstructionItemCategory.AddChildNode(It);
	}

	//多次添加之前先清空
	TSharedPtr<SWidget> *TempWidget = DetailWidgetMap.Find(EObjectType::OT_ModifyWall);
	if (TempWidget)
	{
		DetailWidgetSwitcher->RemoveSlot(TempWidget->Get()->AsShared());
	}

	DetailWidgetMap.Add(EObjectType::OT_ModifyWall, ModifyWallDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_ModifyWall)->Get()->AsShared()
		];
	ConstructionModifyWallDetail->RefreshItemExpand(ModifyWallDetailBuilder->GetTreeView());
}

void FArmyLayoutModeDetail::CreateAddWallDetail()
{
	AddWallDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = AddWallDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Type", TEXT("类型"), CreateAddWallHalfWallWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Width", TEXT("厚度(mm)"), CreateAddWallWidthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateAddWallHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Dir", TEXT("逆时针"), CreateAddWallDirWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Material", TEXT("材质"), CreateAddWallMatTypeWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	FArmyDetailNode& ConstructionItemCategory = AddWallDetailBuilder->AddCategory("ConstructionItem", TEXT("施工项"));

	for (auto It : ConstructionAddWallDetail->GetDetailNodes())
	{
		ConstructionItemCategory.AddChildNode(It);
	}

	//多次添加之前先清空
	TSharedPtr<SWidget> *TempWidget = DetailWidgetMap.Find(EObjectType::OT_AddWall);
	if (TempWidget)
	{
		DetailWidgetSwitcher->RemoveSlot(TempWidget->Get()->AsShared());
	}

	DetailWidgetMap.Add(EObjectType::OT_AddWall, AddWallDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_AddWall)->Get()->AsShared()
		];
	ConstructionAddWallDetail->RefreshItemExpand(AddWallDetailBuilder->GetTreeView());
}

void FArmyLayoutModeDetail::CreateIndependentWall()
{
	IndependentWallDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = IndependentWallDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Type", TEXT("类型"), CreateAddWallHalfWallWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Width", TEXT("厚度(mm)"), CreateAddWallWidthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateAddWallHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Dir", TEXT("逆时针"), CreateAddWallDirWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Material", TEXT("材质"), CreateAddWallMatTypeWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	FArmyDetailNode& ConstructionItemCategory = IndependentWallDetailBuilder->AddCategory("ConstructionItem", TEXT("施工项"));

	for (auto It : ConstructionIndepentWallDetail->GetDetailNodes())
	{
		ConstructionItemCategory.AddChildNode(It);
	}

	//多次添加之前先清空
	TSharedPtr<SWidget> *TempWidget = DetailWidgetMap.Find(EObjectType::OT_IndependentWall);
	if (TempWidget)
	{
		DetailWidgetSwitcher->RemoveSlot(TempWidget->Get()->AsShared());
	}

	DetailWidgetMap.Add(EObjectType::OT_IndependentWall, IndependentWallDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_IndependentWall)->Get()->AsShared()
		];
	ConstructionIndepentWallDetail->RefreshItemExpand(IndependentWallDetailBuilder->GetTreeView());
}

void FArmyLayoutModeDetail::CreatePackPipeDetail()
{
	PackPipeDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = PackPipeDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreatePackPipeHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Thickness", TEXT("厚度(mm)"), CreatePackPipeThicknessWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("长度(mm)"), CreatePackPipeLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	FArmyDetailNode& ConstructionItemCategory = PackPipeDetailBuilder->AddCategory("ConstructionItem", TEXT("施工项"));

	for (auto It : ConstructionPackPipeDetail->GetDetailNodes())
	{
		ConstructionItemCategory.AddChildNode(It);
	}

	DetailWidgetMap.Add(EObjectType::OT_PackPipe, PackPipeDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_PackPipe)->Get()->AsShared()
		];
	ConstructionPackPipeDetail->RefreshItemExpand(PackPipeDetailBuilder->GetTreeView());
}

void FArmyLayoutModeDetail::ExeWhileAddWallShowDetail()
{
	TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(SelectedXRObject);
	if (AddWall.IsValid() && FArmyUser::Get().ConstructionAddWallData.Contains(AddWall->GetSelectedMaterialID()))
	{		
		ConstructionAddWallDetail->Init(FArmyUser::Get().ConstructionAddWallData[AddWall->GetSelectedMaterialID()]);
		CreateAddWallDetail();
		ConstructionAddWallDetail->RefreshCheckData(AddWall->ConstructionItemData);
	}	
}

void FArmyLayoutModeDetail::ExeWhileIndepentWallShowDetail()
{
	TSharedPtr<FArmyIndependentWall> IndependentWall = StaticCastSharedPtr<FArmyIndependentWall>(SelectedXRObject);
	if (IndependentWall.IsValid() && FArmyUser::Get().ConstructionAddWallData.Contains(IndependentWall->GetSelectedMaterialID()))
	{
		ConstructionIndepentWallDetail->Init(FArmyUser::Get().ConstructionAddWallData[IndependentWall->GetSelectedMaterialID()]);
		CreateIndependentWall();
		ConstructionIndepentWallDetail->RefreshCheckData(IndependentWall->ConstructionItemData);
	}
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreatePackPipeHeightWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetPackPipeHeight)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		.IsEnabled(false)
		.IsReadOnly(true)
		;
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreatePackPipeLengthWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetPackPipeLength)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		.IsEnabled(false)
		.IsReadOnly(true)
		;
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreatePackPipeThicknessWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetPackPipeThickness)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		.IsEnabled(false)
		.IsReadOnly(true)
		;
}

FText FArmyLayoutModeDetail::GetPackPipeLength() const
{
	TSharedPtr<FArmyPackPipe> PackPipe = StaticCastSharedPtr<FArmyPackPipe>(SelectedXRObject);
	if (PackPipe.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.0f"), PackPipe->GetLength() * 10.f));
	}
	return FText::FromString(FString::Printf(TEXT("%.0f"), 0.0f));
}

FText FArmyLayoutModeDetail::GetPackPipeHeight() const
{
	TSharedPtr<FArmyPackPipe> PackPipe = StaticCastSharedPtr<FArmyPackPipe>(SelectedXRObject);
	if (PackPipe.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.0f"), PackPipe->GetHeight() * 10.f));
	}
	return FText::FromString(FString::Printf(TEXT("%.0f"), 0.0f));
}

FText FArmyLayoutModeDetail::GetPackPipeThickness() const
{
	TSharedPtr<FArmyPackPipe> PackPipe = StaticCastSharedPtr<FArmyPackPipe>(SelectedXRObject);
	if (PackPipe.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.0f"), PackPipe->GetThickness() * 10.f));
	}
	return FText::FromString(FString::Printf(TEXT("%.0f"), 0.0f));
}

void FArmyLayoutModeDetail::OnSpaceTypeListReady()
{
    CreateSpaceDetail();
	CreateSplitRegionDetail();
}

void FArmyLayoutModeDetail::OnConstructionOriginWallNewPassReady()
{
	if (SelectedXRObject.IsValid())
	{
		TSharedPtr<FArmyNewPass> NewPass = StaticCastSharedPtr<FArmyNewPass>(SelectedXRObject);
		if (NewPass.IsValid() && NewPass->GetType() == OT_NewPass)
		{
			TArray<TSharedPtr<FJsonObject>> TempJsonDataArray;
			if (FArmyUser::Get().ConstructionNewPassByMatData.Contains(NewPass->GetSelectedMaterialID()))
			{
				TempJsonDataArray.Add(FArmyUser::Get().ConstructionNewPassByMatData[NewPass->GetSelectedMaterialID()]);
			}			
			TempJsonDataArray.Add(FArmyUser::Get().ConstructionOriginPassData);
			ConstructionNewPassDetail->Init(TempJsonDataArray);
		}		
	}
	else
	{
		ConstructionNewPassDetail->Init(FArmyUser::Get().ConstructionNewPassData);
	}	
	CreateNewPassDetail();
}

void FArmyLayoutModeDetail::OnConstructionNewWallNewPassReady()
{
	ConstructionNewPassDetail->Init(FArmyUser::Get().ConstructionOriginPassData);
	CreateNewPassDetail();
}

void FArmyLayoutModeDetail::OnConstructionOriginPassReady()
{
	ConstructionPassDetail->Init(FArmyUser::Get().ConstructionOriginPassData);
	OnConstructionPassReady();
	CreatePassDetail();
	OnConstructionNewWallNewPassReady();
}

void FArmyLayoutModeDetail::OnConstructionDoorHoleReady()
{
	CreateDoorHoleConstruction(nullptr);
}

void FArmyLayoutModeDetail::OnConstructionPassReady()
{
	ConstructionPassDetail->Init(FArmyUser::Get().ConstructionOriginPassData);	

	PassDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = PassDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("宽度(mm)"), CreatePassLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreatePassHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Generate", TEXT("包边"), CreatePassCheckBoxDetail()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("FillPass", TEXT("填门洞"), CreateFillPassCheckBoxDetail()).ValueContentOverride().HAlign(HAlign_Fill);

	FArmyDetailNode& ConstructionItemCategory = PassDetailBuilder->AddCategory("ConstructionItem", TEXT("施工项"));
	for (auto It : ConstructionPassDetail->GetDetailNodes())
	{
		ConstructionItemCategory.AddChildNode(It);
	}

	TSharedPtr<SWidget> *TempWidget = DetailWidgetMap.Find(EObjectType::OT_Pass);
	if (TempWidget)
	{
		DetailWidgetSwitcher->RemoveSlot(TempWidget->Get()->AsShared());
	}

	DetailWidgetMap.Add(EObjectType::OT_Pass, PassDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_Pass)->Get()->AsShared()
		];
	ConstructionPassDetail->RefreshItemExpand(PassDetailBuilder->GetTreeView());

	if (!DetailWidgetMap.Find(EObjectType::OT_DoorHole))
	{
		OnConstructionDoorHoleReady();
	}		
	ConstructionPassDetail->RefreshItemExpand(PassDetailBuilder->GetTreeView());			
}

void FArmyLayoutModeDetail::OnConstructionOriginWallSingleDoorReady()
{
	TSharedPtr<FArmySingleDoor> Door = StaticCastSharedPtr<FArmySingleDoor>(SelectedXRObject);
	TArray<TSharedPtr<FJsonObject>> TempJsonDataArray;
	TempJsonDataArray.Add(FArmyUser::Get().ConstructionOriginPassData);
	if (Door.IsValid() && FArmyUser::Get().ConstructionNewPassByMatData.Contains(Door->GetSelectedMaterialID()))
	{
		TempJsonDataArray.Add(FArmyUser::Get().ConstructionNewPassByMatData[Door->GetSelectedMaterialID()]);		
	}	
	ConstructionSingleDoorDetail->Init(TempJsonDataArray);
	SingleDoorDetail = MakeShareable(new FArmySingleDoorDetail());
	TSharedPtr<SWidget> SingleDoorDetailWidget = SingleDoorDetail->CreateDetailWidget(SelectedObjectAttribute, ConstructionSingleDoorDetail);
	SingleDoorDetail->LayoutDetail = this->SharedThis(this);
	TSharedPtr<SWidget> *TempWidget = DetailWidgetMap.Find(EObjectType::OT_Door);
	if (TempWidget)
	{
		DetailWidgetSwitcher->RemoveSlot(TempWidget->Get()->AsShared());
	}

	DetailWidgetMap.Add(EObjectType::OT_Door, SingleDoorDetailWidget);

	DetailWidgetSwitcher->AddSlot()
		[
			SingleDoorDetailWidget->AsShared()
		];

	ConstructionSingleDoorDetail->RefreshItemExpand(SingleDoorDetail->GetDetailBuilder()->GetTreeView());
}

void FArmyLayoutModeDetail::OnConstructionNewWallSingleDoorReady()
{
	ConstructionSingleDoorDetail->Init(FArmyUser::Get().ConstructionOriginPassData);

	SingleDoorDetail = MakeShareable(new FArmySingleDoorDetail());
	TSharedPtr<SWidget> SingleDoorDetailWidget = SingleDoorDetail->CreateDetailWidget(SelectedObjectAttribute, ConstructionSingleDoorDetail);	
	TSharedPtr<SWidget> *TempWidget = DetailWidgetMap.Find(EObjectType::OT_Door);
	if (TempWidget)
	{
		DetailWidgetSwitcher->RemoveSlot(TempWidget->Get()->AsShared());
	}

	DetailWidgetMap.Add(EObjectType::OT_Door, SingleDoorDetailWidget);

	DetailWidgetSwitcher->AddSlot()
		[
			SingleDoorDetailWidget->AsShared()
		];

	ConstructionSingleDoorDetail->RefreshItemExpand(SingleDoorDetail->GetDetailBuilder()->GetTreeView());
}

void FArmyLayoutModeDetail::OnConstructionOriginWallSlidingDoorReady()
{
	TArray<TSharedPtr<FJsonObject>> TempJsonDataArray;
	TempJsonDataArray.Add(FArmyUser::Get().ConstructionOriginPassData);	
	TSharedPtr<FArmySlidingDoor> Door = StaticCastSharedPtr<FArmySlidingDoor>(SelectedXRObject);
	if (Door.IsValid() && FArmyUser::Get().ConstructionNewPassByMatData.Contains(Door->GetSelectedMaterialID()))
	{
		TempJsonDataArray.Add(FArmyUser::Get().ConstructionNewPassByMatData[Door->GetSelectedMaterialID()]);
	}	
	ConstructionSlidingDoorDetail->Init(TempJsonDataArray);

	SlidingDoorDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = SlidingDoorDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Width", TEXT("宽度(mm)"), CreateCommonLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateCommonHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("OpenDirection", TEXT("开门方向"), CreateOpenDirectionWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	if (Door.IsValid())
	{
		if (Door->InWallType == 0 && !Door->bHasConnectPass)
		{
			DetailCategory.AddChildNode("MatType", TEXT("材质"), CreateSlidingMatTypeWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		}
	}

	FArmyDetailNode& ConstructionItemCategory = SlidingDoorDetailBuilder->AddCategory("ConstructionItem", TEXT("施工项"));

	for (auto It : ConstructionSlidingDoorDetail->GetDetailNodes())
	{
		ConstructionItemCategory.AddChildNode(It);
	}

	TSharedPtr<SWidget> *TempWidget = DetailWidgetMap.Find(EObjectType::OT_SlidingDoor);
	if (TempWidget)
	{
		DetailWidgetSwitcher->RemoveSlot(TempWidget->Get()->AsShared());
	}

	DetailWidgetMap.Add(EObjectType::OT_SlidingDoor, SlidingDoorDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_SlidingDoor)->Get()->AsShared()
		];

	ConstructionSlidingDoorDetail->RefreshItemExpand(SlidingDoorDetailBuilder->GetTreeView());
}

void FArmyLayoutModeDetail::OnConstructionNewWallSlidingDoorReady()
{
	ConstructionSlidingDoorDetail->Init(FArmyUser::Get().ConstructionOriginPassData);

	SlidingDoorDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = SlidingDoorDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Width", TEXT("宽度(mm)"), CreateCommonLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateSlidingHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("OpenDirection", TEXT("开门方向"), CreateOpenDirectionWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	FArmyDetailNode& ConstructionItemCategory = SlidingDoorDetailBuilder->AddCategory("ConstructionItem", TEXT("施工项"));

	for (auto It : ConstructionSlidingDoorDetail->GetDetailNodes())
	{
		ConstructionItemCategory.AddChildNode(It);
	}

	TSharedPtr<SWidget> *TempWidget = DetailWidgetMap.Find(EObjectType::OT_SlidingDoor);
	if (TempWidget)
	{
		DetailWidgetSwitcher->RemoveSlot(TempWidget->Get()->AsShared());
	}

	DetailWidgetMap.Add(EObjectType::OT_SlidingDoor, SlidingDoorDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_SlidingDoor)->Get()->AsShared()
		];

	ConstructionSlidingDoorDetail->RefreshItemExpand(SlidingDoorDetailBuilder->GetTreeView());
}

void FArmyLayoutModeDetail::OnConstructionPackPipeReady()
{
	ConstructionPackPipeDetail->Init(FArmyUser::Get().ConstructionPackPipeData);
	CreatePackPipeDetail();
}

void FArmyLayoutModeDetail::CreateDrainPointDetail()
{
	DrainPointBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = DrainPointBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("PipeRadius", TEXT("管径(mm)"), CreatePipeRadiusWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("PipeHeight", TEXT("高度(mm)"), CreatePipeHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_Drain_Point, DrainPointBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_Drain_Point)->Get()->AsShared()
		];
}

void FArmyLayoutModeDetail::CreateClosestoolDetail()
{
	ClosestoolBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = ClosestoolBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("PipeRadius", TEXT("管径(mm)"), CreatePipeRadiusWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("PipeHeight", TEXT("高度(mm)"), CreatePipeHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	
	DetailWidgetMap.Add(EObjectType::OT_Closestool, ClosestoolBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_Closestool)->Get()->AsShared()
		];
}

void FArmyLayoutModeDetail::CreatePassConstruction(TSharedPtr<SArmyMulitCategory> ConstructionItem)
{
	if (!ConstructionItem.IsValid())
	{
		return;
	}
	TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(SelectedXRObject);
	PassDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = PassDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("宽度(mm)"), CreatePassLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreatePassHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	if (Pass.IsValid() && Pass->GetIfFillPass())
	{
		DetailCategory.AddChildNode("MatType", TEXT("墙体材质"), CreatePassMatTypeWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	}	
	DetailCategory.AddChildNode("Generate", TEXT("包边"), CreatePassCheckBoxDetail()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("FillPass", TEXT("填门洞"), CreateFillPassCheckBoxDetail()).ValueContentOverride().HAlign(HAlign_Fill);

	FArmyDetailNode& ConstructionItemCategory = PassDetailBuilder->AddCategory("ConstructionItem", TEXT("施工项"));
	for (auto It : ConstructionItem->GetDetailNodes())
	{
		ConstructionItemCategory.AddChildNode(It);
	}

	TSharedPtr<SWidget> *TempWidget = DetailWidgetMap.Find(EObjectType::OT_Pass);
	if (TempWidget)
	{
		DetailWidgetSwitcher->RemoveSlot(TempWidget->Get()->AsShared());
	}

	DetailWidgetMap.Add(EObjectType::OT_Pass, PassDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_Pass)->Get()->AsShared()
		];
	ConstructionItem->RefreshItemExpand(PassDetailBuilder->GetTreeView());
}

void FArmyLayoutModeDetail::CreateDoorHoleConstruction(TSharedPtr<class SArmyMulitCategory> ConstructionItem)
{	
	DoorHoleDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = DoorHoleDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));
	TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(SelectedXRObject);
	if (Pass.IsValid())
	{
		if (Pass->bHasAdd)
		{
			DetailCategory.AddChildNode("AddDoorHole", TEXT("补门洞"), CreateAddDoorHoleMatTypeWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		}
		if (Pass->bHasModify)
		{
			DetailCategory.AddChildNode("ModifyDoorHole", TEXT("拆门洞"), CreateModifyDoorHoleMatTypeWidget()).ValueContentOverride().HAlign(HAlign_Fill);
		}
	}
	FArmyDetailNode& ConstructionItemCategory = DoorHoleDetailBuilder->AddCategory("ConstructionItem", TEXT("施工项"));

	if (ConstructionItem.IsValid())
	{
		for (auto It : ConstructionItem->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}	

	TSharedPtr<SWidget> *TempWidget = DetailWidgetMap.Find(EObjectType::OT_DoorHole);
	if (TempWidget)
	{
		DetailWidgetSwitcher->RemoveSlot(TempWidget->Get()->AsShared());
	}

	DetailWidgetMap.Add(EObjectType::OT_DoorHole, DoorHoleDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_DoorHole)->Get()->AsShared()
		];
	if (ConstructionItem.IsValid())
	{
		ConstructionItem->RefreshItemExpand(DoorHoleDetailBuilder->GetTreeView());
	}	
}

FText FArmyLayoutModeDetail::GetWallHeight() const
{
	return FText::FromString(FString::Printf(TEXT("%.0f"), FArmySceneData::WallHeight * 10.f));
}

void FArmyLayoutModeDetail::OnWallColorChanged(const FLinearColor InColor)
{
    // 修改墙体颜色
    FArmySceneData::Get()->SetWallLinesColor(InColor);
}

FText FArmyLayoutModeDetail::GetModifyWallLength() const
{
	TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(SelectedXRObject);
	if (ModifyWall.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), ModifyWall->GetLength() * 10.f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

FText FArmyLayoutModeDetail::GetModifyWallHeight() const
{
	TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(SelectedXRObject);
	if (ModifyWall.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), ModifyWall->GetHeight() * 10.f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

FText FArmyLayoutModeDetail::GetModifyWallFloorDist() const
{
	TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(SelectedXRObject);
	if (ModifyWall.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), ModifyWall->GetFloorHeight() * 10.f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

FText FArmyLayoutModeDetail::GetModifyWallMatType() const
{
	TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(SelectedXRObject);
	if (ModifyWall.IsValid())
	{
		return ModifyWall->GetMatType();
	}
	return FText();
}

void FArmyLayoutModeDetail::OnModifyWallLengthChanged(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		float InPosX = FCString::Atof(*InText.ToString());
		TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(SelectedXRObject);
		if (ModifyWall.IsValid())
		{
			//SCOPE_TRANSACTION(TEXT("修改拆改墙的长度"));
			ModifyWall->SetLength(InPosX / 10.f);
			//ModifyWall->Modify();
			FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

void FArmyLayoutModeDetail::OnModifyWallHeightChanged(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		float InPosX = FCString::Atof(*InText.ToString());
		TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(SelectedXRObject);
		if (ModifyWall.IsValid())
		{
			//SCOPE_TRANSACTION(TEXT("修改拆改墙的高度"));			
			ModifyWall->SetHeight(InPosX / 10.f);
			//ModifyWall->Modify();
			FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

void FArmyLayoutModeDetail::OnModifyWallFloorDistChanged(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		float InPosX = FCString::Atof(*InText.ToString());
		TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(SelectedXRObject);
		if (ModifyWall.IsValid())
		{
			//SCOPE_TRANSACTION(TEXT("修改拆改墙的离地高度"));			
			ModifyWall->SetFloorHeight(InPosX / 10.f);
			//ModifyWall->Modify();
			FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateModifyWallLengthWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetModifyWallLength)
		.OnTextCommitted_Raw(this, &FArmyLayoutModeDetail::OnModifyWallLengthChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateModifyWallHeightWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetModifyWallHeight)
		.OnTextCommitted_Raw(this, &FArmyLayoutModeDetail::OnModifyWallHeightChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		.IsReadOnly(true)
		;
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateModifyWallFloorDistWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetModifyWallFloorDist)
		.OnTextCommitted_Raw(this, &FArmyLayoutModeDetail::OnModifyWallFloorDistChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		.IsReadOnly(true)
		;
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateModifyWallMatTypeWidget()
{
	return
		SAssignNew(CB_ModifyMatType, SArmyPropertyComboBox)
		.OptionsSource(ModifyWallMatList)
		.OnSelectionChanged(this, &FArmyLayoutModeDetail::OnSlectedModifyMatTypeChanged)
		.Value(this, &FArmyLayoutModeDetail::GetModifyWallMatType);
}

void FArmyLayoutModeDetail::OnSlectedModifyMatTypeChanged(const int32 Key, const FString & Value)
{
	TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(SelectedXRObject);
	if (ModifyWall.IsValid() && ModifyWall->GetType() == OT_ModifyWall)
	{
		ModifyWall->SetMatType(FText::FromString(Value));
		ShowSelectedDetial(SelectedXRObject);
	}
}

void FArmyLayoutModeDetail::ExeWhileModifyWallShowDetail()
{
	TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(SelectedXRObject);
	if (ModifyWall.IsValid() && ModifyWall->GetType() == OT_ModifyWall && FArmyUser::Get().ConstructionModifyWallData.Contains(ModifyWall->GetSelectedMaterialID()))
	{		
		ConstructionModifyWallDetail->Init(FArmyUser::Get().ConstructionModifyWallData[ModifyWall->GetSelectedMaterialID()]);
		CreateModifyWallDetail();
		ConstructionModifyWallDetail->RefreshCheckData(ModifyWall->ConstructionItemData);
	}
}

FText FArmyLayoutModeDetail::GetAddWallWidth() const
{
	TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(SelectedXRObject);
	if (AddWall.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), AddWall->GetWidth() * 10.f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

FText FArmyLayoutModeDetail::GetAddWallHeight() const
{
	TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(SelectedXRObject);
	if (AddWall.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), AddWall->GetHeight() * 10.f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

FText FArmyLayoutModeDetail::GetAddWallMatType() const
{
	TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(SelectedXRObject);
	if (AddWall.IsValid())
	{
		return FText::FromString(AddWall->GetMatType());
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyLayoutModeDetail::OnAddWallWidthChanged(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		float InPosX = FCString::Atof(*InText.ToString());
		TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(SelectedXRObject);
		if (AddWall.IsValid())
		{
			float TempWidth = AddWall->GetWidth();
			AddWall->SetWidth(FMath::Clamp(InPosX, 50.f, 500.f) / 10.f);
			if (AddWall->RegenerateWall())
			{
				FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();
			}
			else
			{
				AddWall->SetWidth(TempWidth);
			}			
		}
	}
}

void FArmyLayoutModeDetail::OnAddWallHeightChanged(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		float InPosX = FCString::Atof(*InText.ToString());
		TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(SelectedXRObject);
		if (AddWall.IsValid())
		{
			//SCOPE_TRANSACTION(TEXT("修改新建墙的高度"));	
			AddWall->SetHeight(InPosX / 10.f);
		}
	}
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateAddWallWidthWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetAddWallWidth)
		.OnTextCommitted_Raw(this, &FArmyLayoutModeDetail::OnAddWallWidthChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateAddWallHeightWidget()
{
	return
		SAssignNew(AddWallEditBox, SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetAddWallHeight)
		.OnTextCommitted_Raw(this, &FArmyLayoutModeDetail::OnAddWallHeightChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		.IsEnabled(this, &FArmyLayoutModeDetail::GetAddWallEditBoxState)
		//.IsReadOnly(true)
		;
}


TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateAddWallMatTypeWidget()
{
	return
		SAssignNew(CB_AddMatType, SArmyPropertyComboBox)
		.OptionsSource(AddWallMatList)
		.OnSelectionChanged(this, &FArmyLayoutModeDetail::OnSlectedAddMatTypeChanged)
		.Value(this, &FArmyLayoutModeDetail::GetAddWallMatType);
}

bool FArmyLayoutModeDetail::GetAddWallEditBoxState() const
{
	TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(SelectedXRObject);
	if (AddWall.IsValid())
	{
		return AddWall->GetIsHalfWall() ? true : false;
	}
	return false;
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateAddWallHalfWallWidget()
{
	return
		SNew(SHorizontalBox)		
		+SHorizontalBox::Slot()
		[
			SAssignNew(WholeWallCheckBox, SCheckBox)
			.Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("CheckBox.RadioBox"))
			.IsChecked(this, &FArmyLayoutModeDetail::GetWholeWallCheckBoxState)
			.OnCheckStateChanged(this, &FArmyLayoutModeDetail::OnWholeWallCheckBoxStateChanged)
		]
		+ SHorizontalBox::Slot()				
		.Padding(1, 0, 0, 0)
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("整墙")))
			.Justification(ETextJustify::Center)
			.TextStyle(FArmyStyle::Get(), "ArmyText_12")
		]
		+SHorizontalBox::Slot()		
		.Padding(10, 0, 0, 0)
		[
			SAssignNew(HalfWallCheckBox, SCheckBox)
			.Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("CheckBox.RadioBox"))
			.IsChecked(this, &FArmyLayoutModeDetail::GetHalfWallCheckBoxState)
			.OnCheckStateChanged(this, &FArmyLayoutModeDetail::OnHalfWallCheckBoxStateChanged)
		]
		+ SHorizontalBox::Slot()
		.Padding(1, 0, 0, 0)
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("半墙")))
			.Justification(ETextJustify::Center)
			.TextStyle(FArmyStyle::Get(), "ArmyText_12")
		]
		;
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateAddWallDirWidget()
{
	return
		SAssignNew(AddWallDirCheckBox, SCheckBox)
		.Style(FArmyStyle::Get(), "SingleCheckBox")
		.IsChecked(this, &FArmyLayoutModeDetail::GetAddWallDirCheckBoxState)
		.OnCheckStateChanged(this, &FArmyLayoutModeDetail::OnAddWallDirCheckBoxStateChanged);
}

ECheckBoxState FArmyLayoutModeDetail::GetAddWallDirCheckBoxState() const
{
	TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(SelectedXRObject);
	if (AddWall.IsValid())
	{
		return AddWall->GetInternalExtrusion() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}
	return ECheckBoxState::Unchecked;
}

void FArmyLayoutModeDetail::OnAddWallDirCheckBoxStateChanged(ECheckBoxState InNewState)
{
	TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(SelectedXRObject);
	if (AddWall.IsValid())
	{
		AddWall->SetInternalExtrusion(InNewState == ECheckBoxState::Checked ? true : false);
		if (AddWall->RegenerateWall())
		{
			FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();
		}						
		else
		{
			AddWall->SetInternalExtrusion(!AddWall->GetInternalExtrusion());
		}
	}
}

ECheckBoxState FArmyLayoutModeDetail::GetWholeWallCheckBoxState() const
{
	TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(SelectedXRObject);
	if (AddWall.IsValid())
	{
		return AddWall->GetIsHalfWall() ? ECheckBoxState::Unchecked : ECheckBoxState::Checked; 
	}
	return ECheckBoxState::Unchecked;
}

ECheckBoxState FArmyLayoutModeDetail::GetHalfWallCheckBoxState() const
{
	TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(SelectedXRObject);
	if (AddWall.IsValid())
	{
		return AddWall->GetIsHalfWall() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}
	return ECheckBoxState::Unchecked;
}

void FArmyLayoutModeDetail::OnWholeWallCheckBoxStateChanged(ECheckBoxState InNewState)
{	
	TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(SelectedXRObject);
	if (AddWall.IsValid())
	{
		AddWall->SetIsHalfWall(false);
		//AddWallEditBox->SetText(FText::AsNumber(FArmySceneData::WallHeight));
		AddWall->SetHeight(FArmySceneData::WallHeight);
		//AddWallEditBox->SetEnabled(false);
	}
}

void FArmyLayoutModeDetail::OnHalfWallCheckBoxStateChanged(ECheckBoxState InNewState)
{	
	TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(SelectedXRObject);
	if (AddWall.IsValid())
	{
		AddWall->SetIsHalfWall(true);
		//AddWallEditBox->SetEnabled(true);
		//AddWallEditBox->SetText(FText::AsNumber(FArmySceneData::WallHeight / 2.f));
		AddWall->SetHeight(FArmySceneData::WallHeight / 2.f);
	}
}

void FArmyLayoutModeDetail::OnSlectedAddMatTypeChanged(const int32 Key, const FString & Value)
{
	TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(SelectedXRObject);
	if (AddWall.IsValid())
	{
		AddWall->SetMatType(Value);
		ShowSelectedDetial(SelectedXRObject);
	}
}

FText FArmyLayoutModeDetail::GetCommonLength() const
{
	TSharedPtr<FArmyHardware> HardWare = StaticCastSharedPtr<FArmyHardware>(SelectedXRObject);
	if (HardWare.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), HardWare->GetLength()*10.0f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

FText FArmyLayoutModeDetail::GetCommonWidth() const
{
	TSharedPtr<FArmyHardware> HardWare = StaticCastSharedPtr<FArmyHardware>(SelectedXRObject);
	if (HardWare.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), HardWare->GetWidth()*10.0f));	
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

FText FArmyLayoutModeDetail::GetCommonHeight() const
{
	TSharedPtr<FArmyHardware> HardWare = StaticCastSharedPtr<FArmyHardware>(SelectedXRObject);
	if (HardWare.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), HardWare->GetHeight()*10.0f));		
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyLayoutModeDetail::OnCommonLengthChanged(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		float InPosX = FCString::Atof(*InText.ToString());
		TSharedPtr<FArmyHardware> HardWare = StaticCastSharedPtr<FArmyHardware>(SelectedXRObject);
		if (HardWare.IsValid())
		{
            SCOPE_TRANSACTION(TEXT("修改门窗的长度"));
            HardWare->SetLength(InPosX / 10.f);      
			HardWare->Modify();			
			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

void FArmyLayoutModeDetail::OnCommonWidthChanged(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		float InPosX = FCString::Atof(*InText.ToString());
		TSharedPtr<FArmyHardware> HardWare = StaticCastSharedPtr<FArmyHardware>(SelectedXRObject);
        if (HardWare.IsValid())
        {
			SCOPE_TRANSACTION(TEXT("修改门窗的宽度"));			
            HardWare->SetWidth(InPosX / 10.f);		
			HardWare->Modify();
			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
        }
	}
}

void FArmyLayoutModeDetail::OnCommonHeightChanged(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		float InPosX = FCString::Atof(*InText.ToString());
		TSharedPtr<FArmyHardware> HardWare = StaticCastSharedPtr<FArmyHardware>(SelectedXRObject);
		if (HardWare.IsValid())
		{
			SCOPE_TRANSACTION(TEXT("修改门窗的高度"));			
			HardWare->SetHeight(InPosX / 10.f);		
			HardWare->Modify();
			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

void FArmyLayoutModeDetail::ExeWhileDoorHoleShowDetail()
{
	TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(SelectedXRObject);
	if (Pass.IsValid() && Pass->GetType() == OT_DoorHole)
	{		
		TArray<TSharedPtr<FJsonObject>> TempJsonDataArray;
		//补门洞
		if (Pass->bHasAdd)
		{
			TempJsonDataArray.Add(FArmyUser::Get().ConstructionAddPassData);			
		}
		//拆门洞
		if (Pass->bHasModify)
		{
			TempJsonDataArray.Add(FArmyUser::Get().ConstructionModifyPassData);			
		}
		//原始门洞
		TempJsonDataArray.Add(FArmyUser::Get().ConstructionOriginPassData);
		//垂直补
		if (Pass->bPassHasVerticalAdd)
		{
			TempJsonDataArray.Add(FArmyUser::Get().ConstructionVerticalAdd);
		}
		//垂直拆
		if (Pass->bPassHasVerticalModify)
		{
			TempJsonDataArray.Add(FArmyUser::Get().ConstructionVerticalModify);
		}
		ConstructionDoorHoleDetail->Init(TempJsonDataArray);

		CreateDoorHoleConstruction(ConstructionDoorHoleDetail);
		ConstructionDoorHoleDetail->RefreshCheckData(Pass->ConstructionItemData);
	}
}

void FArmyLayoutModeDetail::ExeWhileSingleDoorShowDetail()
{
	TSharedPtr<FArmySingleDoor> Door = StaticCastSharedPtr<FArmySingleDoor>(SelectedXRObject);
	if (Door.IsValid() && Door->GetType() == OT_Door)
	{		
		//单独显示填门洞施工项
		if (Door->InWallType == 0 && !Door->bHasConnectPass)//在原始墙体上
		{
			OnConstructionOriginWallSingleDoorReady();
		}
		//单独显示补门洞施工项
		else if (Door->InWallType == 1 || Door->bHasConnectPass)//在新建墙体上
		{
			OnConstructionNewWallSingleDoorReady();
		}
		ConstructionSingleDoorDetail->RefreshCheckData(Door->ConstructionItemData);
	}
}

void FArmyLayoutModeDetail::ExeWhileSlidingDoorShowDetail()
{
	TSharedPtr<FArmySlidingDoor> Door = StaticCastSharedPtr<FArmySlidingDoor>(SelectedXRObject);
	if (Door.IsValid() && Door->GetType() == OT_SlidingDoor)
	{		
		//单独显示填门洞施工项
		if (Door->InWallType == 0 && !Door->bHasConnectPass)//在原始墙体上
		{
			OnConstructionOriginWallSlidingDoorReady();
		}
		//单独显示补门洞施工项
		else if (Door->InWallType == 1 || Door->bHasConnectPass)//在新建墙体上
		{
			OnConstructionNewWallSlidingDoorReady();
		}
		ConstructionSlidingDoorDetail->RefreshCheckData(Door->ConstructionItemData);
	}
}

void FArmyLayoutModeDetail::ExeWhilePackPipeShowDetail()
{
	TSharedPtr<FArmyPackPipe> PackPipe = StaticCastSharedPtr<FArmyPackPipe>(SelectedXRObject);
	if (PackPipe.IsValid() && PackPipe->GetType() == OT_PackPipe)
	{		
		ConstructionPackPipeDetail->RefreshCheckData(PackPipe->ConstructionItemData);
	}
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateSlidingHeightWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetCommonHeight)
		.OnTextCommitted_Raw(this, &FArmyLayoutModeDetail::OnSlidingHeightChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true);
}

void FArmyLayoutModeDetail::OnSlidingHeightChanged(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		float InPosX = FCString::Atof(*InText.ToString());
		TSharedPtr<FArmySlidingDoor> HardWare = StaticCastSharedPtr<FArmySlidingDoor>(SelectedXRObject);
		if (HardWare.IsValid())
		{
			SCOPE_TRANSACTION(TEXT("修改推拉门的高度"));
			HardWare->SetHeight(InPosX / 10.f);
			HardWare->Modify();
			HardWare->UpdataConnectedDoorHole();
			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateSlidingMatTypeWidget()
{
	return
		SAssignNew(CB_SlidingMatType, SArmyPropertyComboBox)
		.OptionsSource(SlidingMatList)
		.OnSelectionChanged(this, &FArmyLayoutModeDetail::OnSlectedSlidingMatTypeChanged)
		.Value(this, &FArmyLayoutModeDetail::GetSlidingWallMatType);
}

FText FArmyLayoutModeDetail::GetSlidingWallMatType() const
{
	TSharedPtr<FArmySlidingDoor> Door = StaticCastSharedPtr<FArmySlidingDoor>(SelectedXRObject);
	if (Door.IsValid())
	{
		return FText::FromString(Door->GetMatType());
	}
	return FText();
}

void FArmyLayoutModeDetail::OnSlectedSlidingMatTypeChanged(const int32 Key, const FString & Value)
{
	TSharedPtr<FArmySlidingDoor> Door = StaticCastSharedPtr<FArmySlidingDoor>(SelectedXRObject);
	if (Door.IsValid() && Door->GetType() == OT_SlidingDoor)
	{
		Door->SetMatType(Value);
		ShowSelectedDetial(SelectedXRObject);
	}
}

FText FArmyLayoutModeDetail::GetSpaceName() const
{
    TSharedPtr<FArmyRoom> SelectedRoom = StaticCastSharedPtr<FArmyRoom>(SelectedXRObject);
    if (SelectedRoom.IsValid() && SelectedRoom->GetType() == OT_InternalRoom)
    {
        return FText::FromString(SelectedRoom->GetSpaceName());
    }

    return FText();
}

FText FArmyLayoutModeDetail::GetSpaceArea() const
{
	TSharedPtr<FArmyRoom> SelectedRoom = StaticCastSharedPtr<FArmyRoom>(SelectedXRObject);
	if (SelectedRoom.IsValid() && SelectedRoom->GetType() == OT_InternalRoom)
	{
		float TempArea;
		float TempPerimeter;
		SelectedRoom->GetRoomAreaAndPerimeter(TempArea, TempPerimeter);
		return FText::FromString(FString::Printf(TEXT("%.2f"), TempArea));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

FText FArmyLayoutModeDetail::GetSpacePerimeter() const
{
	TSharedPtr<FArmyRoom> SelectedRoom = StaticCastSharedPtr<FArmyRoom>(SelectedXRObject);
	if (SelectedRoom.IsValid() && SelectedRoom->GetType() == OT_InternalRoom)
	{
		float TempArea;
		float TempPerimeter;
		SelectedRoom->GetRoomAreaAndPerimeter(TempArea, TempPerimeter);
		return FText::FromString(FString::Printf(TEXT("%.2f"), TempPerimeter));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

int FArmyLayoutModeDetail::GetFinishGroundHeight() const
{
	TSharedPtr<FArmyRoom> SelectedRoom = StaticCastSharedPtr<FArmyRoom>(SelectedXRObject);
	if (SelectedRoom.IsValid() && SelectedRoom->GetType() == OT_InternalRoom)
	{
		return (int)SelectedRoom->GetFinishGroundHeight();
	}
	return 0;
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateSpaceNameWidget()
{
    return
        SAssignNew(CB_SpaceName, SArmyPropertyComboBox)
        .OptionsSource(FArmyUser::Get().SpaceTypeList)
        .OnSelectionChanged(this, &FArmyLayoutModeDetail::OnSlectedSpaceNameChanged)
        .Value(this, &FArmyLayoutModeDetail::GetSpaceName);
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateSpaceAreaWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetSpaceArea)		
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))		
		.IsReadOnly(true)
		;
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateSpacePerimeterWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetSpacePerimeter)		
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsReadOnly(true)
		;
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateSpaceFinishGroundHeightWidget()
{
	return
		SNew(SSpinBox<int>)
		.Value(this, &FArmyLayoutModeDetail::GetFinishGroundHeight)
		.Style(FArmyStyle::Get(), "SpinBoxStyle")
		.Font(FSlateFontInfo("PingFangSC-Regular", 12))
		.Delta(1)
		.MinSliderValue(0)
		.MaxSliderValue(10000)
		.MinValue(-1000)
		.MaxValue(1000)
		.OnValueChanged(this, &FArmyLayoutModeDetail::OnFinishGroundHeightChanged)
		;
}

void FArmyLayoutModeDetail::OnSlectedSpaceNameChanged(const int32 Key, const FString& Value)
{
    TSharedPtr<FArmyRoom> SelectedRoom = StaticCastSharedPtr<FArmyRoom>(SelectedXRObject);
    if (SelectedRoom.IsValid() && SelectedRoom->GetSpaceId() != Key)
    {
        SelectedRoom->SetSpaceId(Key);
		SelectedRoom->ModifySpaceName(Value);
    }
}

void FArmyLayoutModeDetail::OnFinishGroundHeightChanged(int InValue)
{
	TSharedPtr<FArmyRoom> SelectedRoom = StaticCastSharedPtr<FArmyRoom>(SelectedXRObject);
	if (SelectedRoom.IsValid())
	{
		SelectedRoom->SetFinishGroundHeight(InValue);
	}
}

void FArmyLayoutModeDetail::EexWhileRoomShowDetail()
{
	if (SelectedXRObject.IsValid() && SelectedXRObject->GetType() == OT_InternalRoom)
	{
		if (SelectedXRObject->GetPropertyFlag(FArmyObject::FLAG_MODIFY))
		{
			CreateSpaceDetail(true);
		}
		else if (SelectedXRObject->GetPropertyFlag(FArmyObject::FLAG_LAYOUT))
		{
			CreateSpaceDetail(false);
		}
	}
}

/*********************** SplitRegion Start********************************/
FText FArmyLayoutModeDetail::GetRegionSpaceName() const
{
	TSharedPtr<FArmyRegion> SelectedRegion = StaticCastSharedPtr<FArmyRegion>(SelectedXRObject);
	if (SelectedRegion.IsValid())
	{
		return FText::FromString(SelectedRegion->GetRegionSpaceName());
	}

	return FText();
}
FText FArmyLayoutModeDetail::GetSplitRegionName() const
{
	TSharedPtr<FArmyRegion> SelectedRegion = StaticCastSharedPtr<FArmyRegion>(SelectedXRObject);
	if (SelectedRegion.IsValid())
	{
		return FText::FromString(SelectedRegion->GetSplitRegionName());
	}

	return FText();
}

FText FArmyLayoutModeDetail::GetSplitRegionArea() const
{
	TSharedPtr<FArmyRegion> SelectedRegion = StaticCastSharedPtr<FArmyRegion>(SelectedXRObject);
	if (SelectedRegion.IsValid())
	{
		float TempArea;
		float TempPerimeter;
		SelectedRegion->GetSplitAreaAndPerimeter(TempArea, TempPerimeter);
		return FText::FromString(FString::Printf(TEXT("%.2f"), TempArea));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

FText FArmyLayoutModeDetail::GetSplitReginoPerimeter() const
{
	TSharedPtr<FArmyRegion> SelectedRegion = StaticCastSharedPtr<FArmyRegion>(SelectedXRObject);
	if (SelectedRegion.IsValid())
	{
		float TempArea;
		float TempPerimeter;
		SelectedRegion->GetSplitAreaAndPerimeter(TempArea, TempPerimeter);
		return FText::FromString(FString::Printf(TEXT("%.2f"), TempPerimeter));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

// 区域所在房间名称
TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateRegionSpaceNameWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetRegionSpaceName)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsReadOnly(true)
		;
}

// 区域名称
TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateSplitRegionNameWidget()
{
	return
		SAssignNew(CB_SplitRegionName, SArmyPropertyComboBox)
		.OptionsSource(FArmyUser::Get().SpaceTypeList)
		.OnSelectionChanged(this, &FArmyLayoutModeDetail::OnSlectedSplitRegionNameChanged)
		.Value(this, &FArmyLayoutModeDetail::GetSplitRegionName);
}

// 区域面积
TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateSplitRegionAreaWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetSplitRegionArea)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsReadOnly(true)
		;
}

// 区域周长
TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateSplitRegionPerimeterWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetSplitReginoPerimeter)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsReadOnly(true)
		;
}
void FArmyLayoutModeDetail::OnSlectedSplitRegionNameChanged(const int32 Key, const FString& Value)
{
	TSharedPtr<FArmyRegion> SelectedRegion = StaticCastSharedPtr<FArmyRegion>(SelectedXRObject);
	if (SelectedRegion.IsValid())
	{
		SelectedRegion->SetSplitRegionId(Key);
		SelectedRegion->SetSplitRegionName(Value);
	}
}
/************************ SplitRegion End *************************/

FText FArmyLayoutModeDetail::GetComponentLength() const
{
	TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
	if (Furniture.IsValid())
	{		
		return FText::FromString(FString::Printf(TEXT("%.2f"), Furniture->GetBeamLength()*10.0f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyLayoutModeDetail::OnComponentLengthChanged(const FText& InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
		if (Furniture.IsValid())
		{
			SCOPE_TRANSACTION(TEXT("修改构件的长度"));
			float Length = FCString::Atof(*InText.ToString());
			Furniture->SetBeamLength(Length / 10.f);
			Furniture->Modify();
			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

FText FArmyLayoutModeDetail::GetComponentWidth() const
{
	TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
	if (Furniture.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), Furniture->GetBeamWidth()*10.0f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyLayoutModeDetail::OnComponentWidthChanged(const FText& InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
		if (Furniture.IsValid())
		{
			SCOPE_TRANSACTION(TEXT("修改构件的宽度"));			
			float Width = FCString::Atof(*InText.ToString());
			Furniture->SetBeamWidth(Width / 10.f);			
			Furniture->Modify();
			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

FText FArmyLayoutModeDetail::GetComponentHeight() const
{
	TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
	if (Furniture.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), Furniture->GetThickness()*10.0f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));	
}

void FArmyLayoutModeDetail::OnComponentHeightChanged(const FText& InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
		if (Furniture.IsValid())
		{
			SCOPE_TRANSACTION(TEXT("修改构件的高度"));
			float Height = FCString::Atof(*InText.ToString());
			Furniture->SetThickness(Height / 10.f);
			Furniture->Modify();
			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

FText FArmyLayoutModeDetail::GetComponentThickness() const
{
	TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
	if (Furniture.IsValid())
	{		
		return FText::FromString(FString::Printf(TEXT("%.2f"), Furniture->GetThickness()*10.0f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyLayoutModeDetail::OnComponentThicknessChanged(const FText& InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
		if (Furniture.IsValid())
		{
			SCOPE_TRANSACTION(TEXT("修改构件的粗细度"));
			float Thickness = FCString::Atof(*InText.ToString());			
			Furniture->SetThickness(Thickness / 10.f);			
			Furniture->Modify();
		}
	}
}

FText FArmyLayoutModeDetail::GetComponentAngle() const
{
	TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
	if (Furniture.IsValid())
	{
		FRotator Rot;
		FVector Tran;
		FVector Scale;
		Furniture->GetTransForm(Rot, Tran, Scale);
		if (Rot.Yaw > 0)
		{
			return FText::FromString(FString::Printf(TEXT("%.2f"), FMath::CeilToFloat(Rot.Yaw)));
		}
		else
		{
			return FText::FromString(FString::Printf(TEXT("%.2f"), FMath::FloorToFloat(Rot.Yaw)));			
		}
		
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyLayoutModeDetail::OnComponentAngleChanged(const FText& InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
		if (Furniture.IsValid())
		{
			SCOPE_TRANSACTION(TEXT("修改构件的角度"));
			float InAngle = FCString::Atof(*InText.ToString());			
			Furniture->SetRotation(FRotator(0, InAngle, 0));	
			Furniture->Modify();
		}
	}
}

FText FArmyLayoutModeDetail::GetComponentAltitude() const
{
	TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
	if (Furniture.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), Furniture->GetAltitude()*10.0f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyLayoutModeDetail::OnComponentAltitudeChanged(const FText& InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
		if (Furniture.IsValid())
		{
			SCOPE_TRANSACTION(TEXT("修改构件的离地高度"));
			float Altitude = FCString::Atof(*InText.ToString());			
			Furniture->SetAltitude(Altitude / 10.f);			
			Furniture->Modify();
		}
	}
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateComponentLengthWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetComponentLength)
		.OnTextCommitted_Raw(this, &FArmyLayoutModeDetail::OnComponentLengthChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateComponentWidthWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetComponentWidth)
		.OnTextCommitted_Raw(this, &FArmyLayoutModeDetail::OnComponentWidthChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateComponentHeightWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetComponentHeight)
		.OnTextCommitted_Raw(this, &FArmyLayoutModeDetail::OnComponentHeightChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateComponentThicknessWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetComponentThickness)
		.OnTextCommitted_Raw(this, &FArmyLayoutModeDetail::OnComponentThicknessChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateComponentAngleWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetComponentAngle)
		.OnTextCommitted_Raw(this, &FArmyLayoutModeDetail::OnComponentAngleChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateComponentAltitudeWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetComponentAltitude)
		.OnTextCommitted_Raw(this, &FArmyLayoutModeDetail::OnComponentAltitudeChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

FText FArmyLayoutModeDetail::GetPipeRadius() const
{
	TSharedPtr<FArmyFurniture> FurnitureComponent = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
	if (FurnitureComponent.IsValid() && FurnitureComponent->GetFurniturePro().IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), FurnitureComponent->GetFurniturePro()->GetRadius() * 10.0f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyLayoutModeDetail::OnPipeRadiusChanged(const FText& InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
		if (Furniture.IsValid() && Furniture->GetFurniturePro().IsValid())
		{
			SCOPE_TRANSACTION(TEXT("修改管径"));
			float Radius = FCString::Atof(*InText.ToString());
			Furniture->GetFurniturePro()->SetRadius(Radius / 10.f);
			Furniture->Modify();
		}
	}
}

FText FArmyLayoutModeDetail::GetPipeHeight() const
{
	//TSharedPtr<FArmyFurniture> FurnitureComponent = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
	//if (FurnitureComponent.IsValid())
	//{
	//	return FText::FromString(FString::Printf(TEXT("%.2f"), FurnitureComponent->GetPipeHeight()*10.0f));
	//}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyLayoutModeDetail::OnPipeHeightChanged(const FText& InText, ETextCommit::Type CommitType)
{
	//if (CommitType == ETextCommit::OnEnter)
	//{
	//	TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
	//	if (Furniture.IsValid())
	//	{
	//		SCOPE_TRANSACTION(TEXT("修改管高"));
	//		float Height = FCString::Atof(*InText.ToString());
	//		Furniture->SetPipeHeight(Height / 10.f);
	//		Furniture->Modify();
	//	}
	//}
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreatePipeRadiusWidget()
{
	return
		SAssignNew(PipeRadiusWid, SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetPipeRadius)
		.OnTextCommitted_Raw(this, &FArmyLayoutModeDetail::OnPipeRadiusChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		.IsEnabled(false)
		.IsReadOnly(true);
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreatePipeHeightWidget()
{
	return
		SAssignNew(PipeHeightWid, SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetPipeHeight)
		.OnTextCommitted_Raw(this, &FArmyLayoutModeDetail::OnPipeHeightChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		.IsReadOnly(true)
		.IsEnabled(false)
		.IsReadOnly(true);
}

void FArmyLayoutModeDetail::ExeWhileShowDetail(EObjectType ObjType)
{
	switch (ObjType)
	{
	case OT_InternalRoom:
		EexWhileRoomShowDetail();
		break;
	case OT_AddWall:
		ExeWhileAddWallShowDetail();
		break;
	case OT_IndependentWall:
		ExeWhileIndepentWallShowDetail();
		break;
	case OT_ModifyWall:
		ExeWhileModifyWallShowDetail();
		break;
	case OT_NewPass:
		ExeWhileNewPassShowDetail();
		break;
	case OT_Pass:
		ExeWhilePassShowDetail();
		break;
	case OT_DoorHole:
		ExeWhileDoorHoleShowDetail();
		break;
	case OT_Door:
		ExeWhileSingleDoorShowDetail();
		break;
	case OT_SlidingDoor:
		ExeWhileSlidingDoorShowDetail();
		break;
	case OT_PackPipe:
		ExeWhilePackPipeShowDetail();
		break;
	}
}


TSharedRef<SWidget> FArmyLayoutModeDetail::CreatePassCheckBoxDetail()
{
	return
		SAssignNew(GenerateCheckBox, SCheckBox)
        .Style(FArmyStyle::Get(), "SingleCheckBox")
		.IsChecked(this, &FArmyLayoutModeDetail::GetPassCheckBoxState)
		.OnCheckStateChanged(this, &FArmyLayoutModeDetail::OnPassCheckBoxStateChanged);
}

TSharedRef<SWidget> FArmyLayoutModeDetail::CreateFillPassCheckBoxDetail()
{
	return
		SNew(SCheckBox)
		.Style(FArmyStyle::Get(), "SingleCheckBox")
		.IsChecked(this, &FArmyLayoutModeDetail::GetFillPassCheckBoxState)
		.OnCheckStateChanged(this, &FArmyLayoutModeDetail::OnFillPassCheckBoxStateChanged);
}

TSharedRef<SWidget> FArmyLayoutModeDetail::CreateNewPassCheckBoxDetail()
{
	return
		SNew(SCheckBox)
		.Style(FArmyStyle::Get(), "SingleCheckBox")
		.IsChecked(this, &FArmyLayoutModeDetail::GetNewPassCheckBoxState)
		.OnCheckStateChanged(this, &FArmyLayoutModeDetail::OnNewPassCheckBoxStateChanged);
}

void FArmyLayoutModeDetail::OnNewPassCheckBoxStateChanged(ECheckBoxState InNewState)
{
	TSharedPtr<FArmyNewPass> SelectedPass = StaticCastSharedPtr<FArmyNewPass>(SelectedXRObject);
	if (SelectedPass.IsValid())
	{		
		SelectedPass->SetIfGeneratePassModel(InNewState == ECheckBoxState::Checked ? true : false);
		FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();
	}
}

ECheckBoxState FArmyLayoutModeDetail::GetNewPassCheckBoxState() const
{
	TSharedPtr<FArmyNewPass> SelectedPass = StaticCastSharedPtr<FArmyNewPass>(SelectedXRObject);
	if (SelectedPass.IsValid())
	{
		return SelectedPass->GetIfGeneratePassModel() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}
	return ECheckBoxState::Unchecked;
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateNewPassMatTypeWidget()
{
	return
		SAssignNew(CB_NewPassMatType, SArmyPropertyComboBox)
		.OptionsSource(NewPassMatList)
		.OnSelectionChanged(this, &FArmyLayoutModeDetail::OnSlectedNewPassMatTypeChanged)
		.Value(this, &FArmyLayoutModeDetail::GetNewPassWallMatType);
}

FText FArmyLayoutModeDetail::GetNewPassWallMatType() const
{
	TSharedPtr<FArmyNewPass> Pass = StaticCastSharedPtr<FArmyNewPass>(SelectedXRObject);
	if (Pass.IsValid())
	{
		return FText::FromString(Pass->GetMatType());
	}
	return FText();
}

void FArmyLayoutModeDetail::OnSlectedNewPassMatTypeChanged(const int32 Key, const FString & Value)
{
	TSharedPtr<FArmyNewPass> Pass = StaticCastSharedPtr<FArmyNewPass>(SelectedXRObject);
	if (Pass.IsValid() && Pass->GetType() == OT_NewPass)
	{
		Pass->SetMatType(Value);
		ShowSelectedDetial(SelectedXRObject);
	}
}

void FArmyLayoutModeDetail::ExeWhileNewPassShowDetail()
{
	TSharedPtr<FArmyNewPass> NewPass = StaticCastSharedPtr<FArmyNewPass>(SelectedXRObject);
	if (NewPass.IsValid() && NewPass->GetType() == OT_NewPass)
	{		
		//在原始墙体上
		if (NewPass->InWallType == 0)
		{
			OnConstructionOriginWallNewPassReady();
		}
		//在新建墙体上
		else if (NewPass->InWallType == 1)
		{
			OnConstructionNewWallNewPassReady();
		}
		ConstructionNewPassDetail->RefreshCheckData(NewPass->ConstructionItemData);
	}
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreatePassLengthWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetCommonLength)		
		.OnTextCommitted_Raw(this, &FArmyLayoutModeDetail::OnPassLengthChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true);
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreatePassHeightWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyLayoutModeDetail::GetCommonHeight)		
		.OnTextCommitted_Raw(this, &FArmyLayoutModeDetail::OnPassHeightChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

void FArmyLayoutModeDetail::OnPassCheckBoxStateChanged(ECheckBoxState InNewState)
{	
	TSharedPtr<FArmyPass> SelectedPass = StaticCastSharedPtr<FArmyPass>(SelectedXRObject);
	if (SelectedPass.IsValid())
	{
		if (SelectedPass->GetIfFillPass())
		{
			return;
		}
		SelectedPass->SetIfGeneratePassModel(InNewState == ECheckBoxState::Checked ? true : false);
	}		
}

void FArmyLayoutModeDetail::OnFillPassCheckBoxStateChanged(ECheckBoxState InNewState)
{
	TSharedPtr<FArmyPass> SelectedPass = StaticCastSharedPtr<FArmyPass>(SelectedXRObject);
	if (SelectedPass.IsValid())
	{
		SelectedPass->SetIfFillPass(InNewState == ECheckBoxState::Checked ? true : false);		
		FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();	
		//发生变化时需要重新使用默认数据
		SelectedPass->ConstructionItemData->bHasSetted = false;
		/**更新填门洞或者取消填门洞后的属性面板*/
		ShowSelectedDetial(SelectedXRObject);
	}
}

ECheckBoxState FArmyLayoutModeDetail::GetPassCheckBoxState() const
{
	TSharedPtr<FArmyPass> SelectedPass = StaticCastSharedPtr<FArmyPass>(SelectedXRObject);
	if (SelectedPass.IsValid())
	{
		return SelectedPass->GetIfGeneratePassModel() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;		
	}
	return ECheckBoxState::Unchecked;
}

ECheckBoxState FArmyLayoutModeDetail::GetFillPassCheckBoxState() const
{
	TSharedPtr<FArmyPass> SelectedPass = StaticCastSharedPtr<FArmyPass>(SelectedXRObject);
	if (SelectedPass.IsValid())
	{
		return SelectedPass->GetIfFillPass() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}
	return ECheckBoxState::Unchecked;
}

void FArmyLayoutModeDetail::OnPassLengthChanged(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		float InLength = FCString::Atof(*InText.ToString());
		TSharedPtr<FArmyPass> TempPass = StaticCastSharedPtr<FArmyPass>(SelectedXRObject);
		if (TempPass.IsValid() && !TempPass->GetIfFillPass())
		{
			SCOPE_TRANSACTION(TEXT("修改门窗的长度"));
			TempPass->SetLength(InLength / 10.f);
			TempPass->Modify();
			TempPass->UpdateModifyPass();
			ShowSelectedDetial(SelectedXRObject);
			FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

void FArmyLayoutModeDetail::OnPassHeightChanged(const FText& InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		float InLength = FCString::Atof(*InText.ToString());
		TSharedPtr<FArmyPass> TempPass = StaticCastSharedPtr<FArmyPass>(SelectedXRObject);
		if (TempPass.IsValid() && !TempPass->GetIfFillPass())
		{
			SCOPE_TRANSACTION(TEXT("修改门洞的高度"));
			TempPass->SetHeight(InLength / 10.f);
			TempPass->Modify();			
			TempPass->UpdateVerticalState();
			ShowSelectedDetial(SelectedXRObject);
			FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreatePassMatTypeWidget()
{
	return
		SAssignNew(CB_PassMatType, SArmyPropertyComboBox)
		.OptionsSource(PassMatList)
		.OnSelectionChanged(this, &FArmyLayoutModeDetail::OnSlectedPassMatTypeChanged)
		.Value(this, &FArmyLayoutModeDetail::GetPassWallMatType);
}

FText FArmyLayoutModeDetail::GetPassWallMatType() const
{
	TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(SelectedXRObject);
	if (Pass.IsValid())
	{
		return FText::FromString(Pass->GetMatType());
	}
	return FText();
}

void FArmyLayoutModeDetail::OnSlectedPassMatTypeChanged(const int32 Key, const FString& Value)
{
	TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(SelectedXRObject);
	if (Pass.IsValid() && Pass->GetType() == OT_Pass)
	{
		Pass->SetMatType(Value);
		ShowSelectedDetial(SelectedXRObject);
	}
}

void FArmyLayoutModeDetail::ExeWhilePassShowDetail()
{
	TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(SelectedXRObject);
	if (Pass.IsValid() && Pass->GetType() == OT_Pass)
	{		
		TArray<TSharedPtr<FJsonObject>> TempJsonDataArray;
		//单独显示填门洞施工项
		if (Pass->GetIfFillPass())
		{
			TempJsonDataArray.Add(FArmyUser::Get().ConstructionFillPassData[Pass->GetSelectedMaterialID()]);
		}
		else
		{
			//显示补门洞施工项
			if (Pass->bPassHasAdd)
			{
				TempJsonDataArray.Add(FArmyUser::Get().ConstructionAddPassData);				
			}
			//显示拆门洞施工项
			if (Pass->bPassHasModify)
			{
				TempJsonDataArray.Add(FArmyUser::Get().ConstructionModifyPassData);				
			}
			//显示原始门洞
			TempJsonDataArray.Add(FArmyUser::Get().ConstructionOriginPassData);			
			//垂直补
			if (Pass->bPassHasVerticalAdd)
			{
				TempJsonDataArray.Add(FArmyUser::Get().ConstructionVerticalAdd);
			}
			//垂直拆
			if (Pass->bPassHasVerticalModify)
			{
				TempJsonDataArray.Add(FArmyUser::Get().ConstructionVerticalModify);
			}
		}		

		ConstructionItemPassDetail->Init(TempJsonDataArray, -1);

		CreatePassConstruction(ConstructionItemPassDetail);

		ConstructionItemPassDetail->RefreshCheckData(Pass->ConstructionItemData);				
	}
}

void FArmyLayoutModeDetail::OnSlectedAddDoorHoleMatTypeChanged(const int32 Key, const FString& Value)
{
	TSharedPtr<FArmyPass> DoorHole = StaticCastSharedPtr<FArmyPass>(SelectedXRObject);
	if (DoorHole.IsValid())
	{
		DoorHole->SetAddDoorHoleMatType(FText::FromString(Value));
	}
}

void FArmyLayoutModeDetail::OnSlectedModifyDoorHoleMatTypeChanged(const int32 Key, const FString& Value)
{
	TSharedPtr<FArmyPass> DoorHole = StaticCastSharedPtr<FArmyPass>(SelectedXRObject);
	if (DoorHole.IsValid())
	{
		DoorHole->SetModifyDoorHoleMatType(FText::FromString(Value));
	}
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateAddDoorHoleMatTypeWidget()
{
	return
		SAssignNew(CB_AddDoorHoleMatType, SArmyPropertyComboBox)
		.OptionsSource(AddDoorHoleMatList)
		.OnSelectionChanged(this, &FArmyLayoutModeDetail::OnSlectedAddDoorHoleMatTypeChanged)
		.Value(this, &FArmyLayoutModeDetail::GetAddDoorHoleMatType);
}

TSharedPtr<SWidget> FArmyLayoutModeDetail::CreateModifyDoorHoleMatTypeWidget()
{
	return
		SAssignNew(CB_ModifyDoorHoleMatType, SArmyPropertyComboBox)
		.OptionsSource(ModifyDoorHoleMatList)
		.OnSelectionChanged(this, &FArmyLayoutModeDetail::OnSlectedModifyDoorHoleMatTypeChanged)
		.Value(this, &FArmyLayoutModeDetail::GetModifyDoorHoleMatType);
}

FText FArmyLayoutModeDetail::GetAddDoorHoleMatType() const
{
	TSharedPtr<FArmyPass> DoorHole = StaticCastSharedPtr<FArmyPass>(SelectedXRObject);
	if (DoorHole.IsValid())
	{
		return DoorHole->GetAddDoorHoleMatType();
	}
	return FText::FromString(FString::Printf(TEXT("无")));
}

FText FArmyLayoutModeDetail::GetModifyDoorHoleMatType() const
{
	TSharedPtr<FArmyPass> DoorHole = StaticCastSharedPtr<FArmyPass>(SelectedXRObject);
	if (DoorHole.IsValid())
	{
		return DoorHole->GetModifyDoorHoleMatType();
	}
	return FText::FromString(FString::Printf(TEXT("无"), 0.0f));
}
