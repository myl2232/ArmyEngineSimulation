#include "ArmyHomeModeDetail.h"
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
#include "SColorPicker.h"

TSharedRef<SWidget> FArmyHomeModeDetail::CreateDetailWidget()
{
	Init();

    FArmyUser::Get().OnSpaceTypeListReady.AddRaw(this, &FArmyHomeModeDetail::OnSpaceTypeListReady);

	SAssignNew(DetailWidgetSwitcher, SWidgetSwitcher);

	CreateWallSettingDetail();
	CreateSingleDoorDetail();
	CreateWindowDetail();
	CreateFloorWindowDetail();
	CreatePassDetail();
	CreateDoorHoleDetail();
	CreatePunchDetail();
	CreatePillarDetail();
	CreateAirFlueDetail();
	CreateBeamDetail();
	CreateCurtainBoxDetail();
	CreateRectBayWindowDetail();
	CreateTrapeBayWindowDetail();
	CreateModifyWallDetail();
	CreateAddWallDetail();

	CreateBearingWallDetail();

	//点位属性
	CreateDrainPointDetail();
	CreateClosestoolDetail();
	CreateHighElvBoxDetail();
	CreateGasMeterDetail();
	CreateGasMainPipeDetail();
	CreateWaterHotDetail();//热水点位
	CreateWaterChilledDetail();//冷水点位
	CreateWaterNormalDetail();//中水点位
	CreateWaterSeparatorDetail();//分集水器
	CreateEleBoxLDetail();//弱电箱
	CreateBasinDetail();//地漏下水
	CreateEnterRoomWaterDetail();//入户水点位

	DetailWidget = DetailWidgetSwitcher;
	return DetailWidget->AsShared();
}

void FArmyHomeModeDetail::ShowSelectedDetial(TSharedPtr<FArmyObject> InSelectedObject)
{
	SelectedXRObject = InSelectedObject;
	if (SelectedXRObject.IsValid()) {
		CachedName = SelectedXRObject->GetName();
		
		TSharedPtr<SWidget> *TempWidget;
		//防盗门与标准门同样处理
		if (SelectedXRObject->GetType() == OT_SecurityDoor)
		{
			TempWidget = DetailWidgetMap.Find(OT_Door);
		}
		else
		{
			TempWidget = DetailWidgetMap.Find(SelectedXRObject->GetType());
		}		

		if (SelectedXRObject->GetType() == OT_ComponentBase)
		{
			TSharedPtr<FArmyFurniture> Fur = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
			if (Fur.IsValid())
			{

				TempWidget = DetailWidgetMap.Find((EObjectType)Fur->GetFurniturePro()->GetObjectType());
			}

		}

		if (TempWidget)
        {						
			DetailWidgetSwitcher->SetActiveWidget(TempWidget->Get()->AsShared());
			HiddenDetailWidget(EVisibility::Visible);
		}
		else
        {
			SelectedXRObject = nullptr;
			DetailWidgetSwitcher->SetActiveWidget(WallSettingWidget->AsShared());
		}
		
	}
	else { 
		DetailWidgetSwitcher->SetActiveWidget(WallSettingWidget->AsShared());
	}
}

void FArmyHomeModeDetail::Init()
{
    SelectedObjectAttribute = TAttribute< TWeakPtr<FArmyObject> >::Create(TAttribute< TWeakPtr<FArmyObject> >::FGetter::CreateRaw(this, &FArmyHomeModeDetail::GetSelectedObject));

    RectBayWindowTypeList = MakeShareable(new FArmyComboBoxArray);
	RectBayWindowTypeList->Add(MakeShareable(new FArmyKeyValue(0, TEXT("无挡板"))));	
	RectBayWindowTypeList->Add(MakeShareable(new FArmyKeyValue(1, TEXT("左侧挡板"))));
	RectBayWindowTypeList->Add(MakeShareable(new FArmyKeyValue(2, TEXT("右侧挡板"))));
	RectBayWindowTypeList->Add(MakeShareable(new FArmyKeyValue(3, TEXT("双侧挡板"))));

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

	ModifyPointLocation = MakeShareable(new FArmyComboBoxArray);
	ModifyPointLocation->Add(MakeShareable(new FArmyKeyValue(0, TEXT("20"))));
	ModifyPointLocation->Add(MakeShareable(new FArmyKeyValue(1, TEXT("25"))));
	
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateCommonNameWidget()
{
	return
		SNew(SEditableTextBox)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.Text(this, &FArmyHomeModeDetail::GetName)
		.IsReadOnly(true)
        .IsEnabled(false);
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateCommonLengthWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetCommonLength)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnCommonLengthChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true);
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateCommonHeightWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetCommonHeight)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnCommonHeightChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true);
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateCommonFloorAltitudeWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetCommonFloorAltitude)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnCommonFloorAltitudeChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

void FArmyHomeModeDetail::CreateWallSettingDetail()
{	
	WallDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = WallDetailBuilder->AddCategory("GlobalCategory", TEXT("全局设置"));

	DetailCategory.AddChildNode("WallHeight", TEXT("墙体高度(mm)"), SNew(SArmyEditableNumberBox)
        .Text_Lambda([this]() {
            return FText::FromString(FString::Printf(TEXT("%.0f"), FArmySceneData::WallHeight * 10.f));
        })
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnWallHeightChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		.SelectAllTextWhenFocused(true)
    ).ValueContentOverride().HAlign(HAlign_Fill);

    DetailCategory.AddChildNode("WallColor", TEXT("墙线颜色"), SNew(SButton)
        .OnClicked_Lambda([this]() {
            FColorPickerArgs PickerArgs;
            PickerArgs.InitialColorOverride = FArmySceneData::Get()->GetWallLinesColor();
	        PickerArgs.bUseAlpha = true;
	        PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &FArmyHomeModeDetail::OnWallColorChanged);
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

void FArmyHomeModeDetail::CreateSingleDoorDetail()
{
    SingleDoorDetail = MakeShareable(new FArmySingleDoorDetail());
    TSharedPtr<SWidget> SingleDoorDetailWidget = SingleDoorDetail->CreateDetailWidget(SelectedObjectAttribute);
	DetailWidgetMap.Add(EObjectType::OT_Door, SingleDoorDetailWidget);

	DetailWidgetSwitcher->AddSlot()
		[
            SingleDoorDetailWidget->AsShared()
		];
}

void FArmyHomeModeDetail::CreateWindowDetail()
{
	WindowsDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = WindowsDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("宽度(mm)"), CreateCommonLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateCommonHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("GroundHeight", TEXT("离地高度(mm)"), CreateCommonFloorAltitudeWidget()/*CreateWindowGroundHeightWidget()*/).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("WindowStone", TEXT("生成窗台石"), CreateWindowStoneCheckBoxWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_Window, WindowsDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_Window)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreateFloorWindowDetail()
{
	FloorWindowsDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = FloorWindowsDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("宽度(mm)"), CreateCommonLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateCommonHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);	
	DetailCategory.AddChildNode("WindowStone", TEXT("生成窗台石"), CreateWindowStoneCheckBoxWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_FloorWindow, FloorWindowsDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_FloorWindow)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreateSpaceDetail()
{
	SpaceDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = SpaceDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("空间名称"), CreateSpaceNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Area", TEXT("面积(㎡)"), CreateSpaceAreaWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Perimeter", TEXT("周长(m)"), CreateSpacePerimeterWidget()).ValueContentOverride().HAlign(HAlign_Fill);	
	DetailCategory.AddChildNode("GroundHeight", TEXT("原始面高度(mm)"), CreateSpaceOriginGroundHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_InternalRoom, SpaceDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_InternalRoom)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreatePassDetail()
{
	PassDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = PassDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("宽度(mm)"), CreateCommonLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateCommonHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	//DetailCategory.AddChildNode("Generate", TEXT("包边"), CreatePassCheckBoxDetail()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_Pass, PassDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_Pass)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreateDoorHoleDetail()
{
	DoorHoleDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = DoorHoleDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("AddDoorHole", TEXT("补门洞"), CreateAddDoorHoleMatTypeWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("ModifyDoorHole", TEXT("拆门洞"), CreateModifyDoorHoleMatTypeWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_DoorHole, DoorHoleDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_DoorHole)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreatePunchDetail()
{
	PunchDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = PunchDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("洞宽(mm)"), CreateCommonLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("洞高(mm)"), CreateCommonHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Depth", TEXT("洞深"), CreatePunchDepthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("FloorDist", TEXT("离地高度(mm)"), CreateCommonFloorAltitudeWidget()/*CreatePunchGroundHeightWidget()*/).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_Punch, PunchDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_Punch)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreatePillarDetail()
{
	PillarDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = PillarDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("长度(mm)"), CreateSimpleComponentLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Width", TEXT("宽度(mm)"), CreateSimpleComponentWidthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Angle", TEXT("角度(°)"), CreateSimpleComponentAngleWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("AutoAdapt", TEXT("自适应"), CreateSimpleComponentAutoAdaptCheckWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_Pillar, PillarDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_Pillar)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreateAirFlueDetail()
{
	FlueDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = FlueDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("长度(mm)"), CreateSimpleComponentLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Width", TEXT("宽度(mm)"), CreateSimpleComponentWidthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Angle", TEXT("角度(°)"), CreateSimpleComponentAngleWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("AutoAdapt", TEXT("自适应"), CreateSimpleComponentAutoAdaptCheckWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_AirFlue, FlueDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_AirFlue)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreateBeamDetail()
{
	BeamDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = BeamDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("长度(mm)"), CreateSimpleComponentLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Width", TEXT("宽度(mm)"), CreateSimpleComponentWidthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Altitude", TEXT("高度(mm)"), CreateSimpleComponentHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Angle", TEXT("角度(°)"), CreateSimpleComponentAngleWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("AutoAdapt", TEXT("自适应"), CreateSimpleComponentAutoAdaptCheckWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_Beam, BeamDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_Beam)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreateCurtainBoxDetail()
{
	CurtainBoxDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = CurtainBoxDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("长度(mm)"), CreateComponentLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Width", TEXT("宽度(mm)"), CreateComponentWidthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Altitude", TEXT("高度(mm)"), CreateComponentHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Angle", TEXT("角度(°)"), CreateComponentAngleWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_CurtainBox, CurtainBoxDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_CurtainBox)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreateRectBayWindowDetail()
{
	RectBayWindowDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = RectBayWindowDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("内窗宽度(mm)"), CreateCommonLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);	
	DetailCategory.AddChildNode("OutLength", TEXT("外窗宽度(mm)"), CreateBayWindowOutLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateCommonHeightWidget()/*CreateRectBayWindowHeightWidget()*/).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Altitude", TEXT("离地高度(mm)"), CreateCommonFloorAltitudeWidget()/*CreateRectBayWindowAltitudeWidget()*/).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Depth", TEXT("进深(mm)"), CreateRectBayWindowDepthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Type", TEXT("挡板"), CreateRectBayWindowTypeWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("WindowStone", TEXT("生成窗台石"), CreateRectBayWindowStoneWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_RectBayWindow, RectBayWindowDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_RectBayWindow)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreateTrapeBayWindowDetail()
{	
	TrapeBayWindowDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = TrapeBayWindowDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("内窗宽度(mm)"), CreateCommonLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("OutLength", TEXT("外窗宽度(mm)"), CreateBayWindowOutLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateCommonHeightWidget() /*CreateRectBayWindowHeightWidget()*/).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Altitude", TEXT("离地高度(mm)"), CreateCommonFloorAltitudeWidget()/*CreateRectBayWindowAltitudeWidget()*/).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Depth", TEXT("进深(mm)"), CreateRectBayWindowDepthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Type", TEXT("挡板"), CreateRectBayWindowTypeWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("WindowStone", TEXT("生成窗台石"), CreateRectBayWindowStoneWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_TrapeBayWindow, TrapeBayWindowDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_TrapeBayWindow)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreateModifyWallDetail()
{
	ModifyWallDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = ModifyWallDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("宽度(mm)"), CreateModifyWallLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);	
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateModifyWallHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("FloorDist", TEXT("离地高度(mm)"), CreateModifyWallFloorDistWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Material", TEXT("材质"), CreateModifyWallMatTypeWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_ModifyWall, ModifyWallDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_ModifyWall)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreateAddWallDetail()
{
	AddWallDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = AddWallDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	//DetailCategory.AddChildNode("Length", TEXT("宽度(mm)"), CreateAddWallLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateAddWallHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Material", TEXT("材质"), CreateAddWallMatTypeWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_AddWall, AddWallDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_AddWall)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreateBearingWallDetail()
{
	BearingWallDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = BearingWallDetailBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("长度(mm)"), CreateBearingWallLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	
	DetailWidgetMap.Add(EObjectType::OT_BearingWall, BearingWallDetailBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_BearingWall)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::OnSpaceTypeListReady()
{
    CreateSpaceDetail();
}

void FArmyHomeModeDetail::CreateDrainPointDetail()
{
	DrainPointBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = DrainPointBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("PipeRadius", TEXT("管径(mm)"), CreatePipeRadiusReadOnlyWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("PipeHeight", TEXT("高度(mm)"), CreatePointPositionHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_Drain_Point, DrainPointBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_Drain_Point)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreateClosestoolDetail()
{
	ClosestoolBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = ClosestoolBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("PipeRadius", TEXT("管径(mm)"), CreatePipeRadiusReadOnlyWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	//DetailCategory.AddChildNode("PipeHeight", TEXT("高度(mm)"), CreatePointPositionHeightReadOnlyWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	
	DetailWidgetMap.Add(EObjectType::OT_Closestool, ClosestoolBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_Closestool)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreateHighElvBoxDetail()
{
	HighElvBoxBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = HighElvBoxBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("长度(mm)"), CreatePointPositionLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Width", TEXT("宽度(mm)"), CreatePointPositionWidthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreatePointPositionHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Altitude", TEXT("离地高度(mm)"), CreatePointPositionAltitudeWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_EleBoxH_Point, HighElvBoxBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_EleBoxH_Point)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreateGasMainPipeDetail()
{
	HighGasMainPipeBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = HighGasMainPipeBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Radius", TEXT("管径(mm)"), CreatePipeRadiusWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreatePointPositionHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	//DetailCategory.AddChildNode("Altitude", TEXT("离地高度(mm)"), CreatePointPositionAltitudeWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_Gas_MainPipe, HighGasMainPipeBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_Gas_MainPipe)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreateGasMeterDetail()
{
	HighGasMeterBuilder= MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = HighGasMeterBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("长度(mm)"), CreatePointPositionLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Width", TEXT("厚度(mm)"), CreatePointPositionWidthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreatePointPositionHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Altitude", TEXT("离地高度(mm)"), CreatePointPositionAltitudeWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_Gas_Meter, HighGasMeterBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_Gas_Meter)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreateWaterHotDetail()
{
	HighWaterHotBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = HighWaterHotBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Radius", TEXT("管径(mm)"), CreatePipeRadiusReadOnlyWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Altitude", TEXT("离地高度(mm)"), CreatePointPositionAltitudeWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_Water_Hot_Point, HighWaterHotBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_Water_Hot_Point)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreateWaterChilledDetail()
{
	HighWaterChilledBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = HighWaterChilledBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Radius", TEXT("管径(mm)"), CreatePipeRadiusReadOnlyWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Altitude", TEXT("离地高度(mm)"), CreatePointPositionAltitudeWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_Water_Chilled_Point, HighWaterChilledBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_Water_Chilled_Point)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreateWaterNormalDetail()
{
	HighWaterNormalBuilder= MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = HighWaterNormalBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Radius", TEXT("管径(mm)"), CreatePipeRadiusReadOnlyWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Altitude", TEXT("离地高度(mm)"), CreatePointPositionAltitudeWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_Water_Normal_Point, HighWaterNormalBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_Water_Normal_Point)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreateWaterSeparatorDetail()
{
	HighWaterSeparatorBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = HighWaterSeparatorBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("长度(mm)"), CreatePointPositionLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Width", TEXT("厚度(mm)"), CreatePointPositionWidthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreatePointPositionHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Altitude", TEXT("离地高度(mm)"), CreatePointPositionAltitudeWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_Water_Separator_Point, HighWaterSeparatorBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_Water_Separator_Point)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreateBasinDetail()
{
	BasinBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = BasinBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Radius", TEXT("管径(mm)"), CreatePipeRadiusReadOnlyWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreatePointPositionHeightReadOnlyWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_Basin, BasinBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_Basin)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreateEleBoxLDetail()
{
	LowElvBoxBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = LowElvBoxBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Length", TEXT("长度(mm)"), CreatePointPositionLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Width", TEXT("宽度(mm)"), CreatePointPositionWidthWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Height", TEXT("高度(mm)"), CreatePointPositionHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Altitude", TEXT("离地高度(mm)"), CreatePointPositionAltitudeWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_EleBoxL_Point, LowElvBoxBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_EleBoxL_Point)->Get()->AsShared()
		];
}

void FArmyHomeModeDetail::CreateEnterRoomWaterDetail()
{
	EnterRoomWaterBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& DetailCategory = EnterRoomWaterBuilder->AddCategory("GlobalCategory", TEXT("物体详情"));

	DetailCategory.AddChildNode("ComponentName", TEXT("名称"), CreateCommonNameWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Radius", TEXT("管径(mm)"), CreatePipeRadiusReadOnlyWidget()).ValueContentOverride().HAlign(HAlign_Fill);
	DetailCategory.AddChildNode("Altitude", TEXT("离地高度(mm)"), CreatePointPositionAltitudeWidget()).ValueContentOverride().HAlign(HAlign_Fill);

	DetailWidgetMap.Add(EObjectType::OT_Water_Supply, EnterRoomWaterBuilder->BuildDetail());

	DetailWidgetSwitcher->AddSlot()
		[
			DetailWidgetMap.Find(EObjectType::OT_Water_Supply)->Get()->AsShared()
		];
}

FText FArmyHomeModeDetail::GetModifyWallLength() const
{
	TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(SelectedXRObject);
	if (ModifyWall.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), ModifyWall->GetLength() * 10.f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

FText FArmyHomeModeDetail::GetModifyWallHeight() const
{
	TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(SelectedXRObject);
	if (ModifyWall.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), ModifyWall->GetHeight() * 10.f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

FText FArmyHomeModeDetail::GetModifyWallFloorDist() const
{
	TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(SelectedXRObject);
	if (ModifyWall.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), ModifyWall->GetFloorHeight() * 10.f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

FText FArmyHomeModeDetail::GetModifyWallMatType() const
{
	TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(SelectedXRObject);
	if (ModifyWall.IsValid())
	{
		return ModifyWall->GetMatType();
	}
	return FText();
}

void FArmyHomeModeDetail::OnModifyWallLengthChanged(const FText & InText, ETextCommit::Type CommitType)
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

void FArmyHomeModeDetail::OnModifyWallHeightChanged(const FText & InText, ETextCommit::Type CommitType)
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

void FArmyHomeModeDetail::OnModifyWallFloorDistChanged(const FText & InText, ETextCommit::Type CommitType)
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

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateModifyWallLengthWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetModifyWallLength)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnModifyWallLengthChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateModifyWallHeightWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetModifyWallHeight)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnModifyWallHeightChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateModifyWallFloorDistWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetModifyWallFloorDist)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnModifyWallFloorDistChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateModifyWallMatTypeWidget()
{
	return
		SAssignNew(CB_ModifyMatType, SArmyPropertyComboBox)
		.OptionsSource(ModifyWallMatList)
		.OnSelectionChanged(this, &FArmyHomeModeDetail::OnSlectedModifyMatTypeChanged)
		.Value(this, &FArmyHomeModeDetail::GetModifyWallMatType);
}

void FArmyHomeModeDetail::OnSlectedModifyMatTypeChanged(const int32 Key, const FString & Value)
{
	TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(SelectedXRObject);
	if (ModifyWall.IsValid())
	{
		ModifyWall->SetMatType(FText::FromString(Value));
	}
}

FText FArmyHomeModeDetail::GetAddWallLength() const
{
	TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(SelectedXRObject);
	if (AddWall.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

FText FArmyHomeModeDetail::GetAddWallHeight() const
{
	TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(SelectedXRObject);
	if (AddWall.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), AddWall->GetHeight() * 10.f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

FText FArmyHomeModeDetail::GetAddWallMatType() const
{
	TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(SelectedXRObject);
	if (AddWall.IsValid())
	{
		return FText::FromString(AddWall->GetMatType());
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyHomeModeDetail::OnAddWallLengthChanged(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		float InPosX = FCString::Atof(*InText.ToString());
		TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(SelectedXRObject);
		if (AddWall.IsValid())
		{
			//SCOPE_TRANSACTION(TEXT("修改新建墙的长度"));			
		}
	}
}

void FArmyHomeModeDetail::OnAddWallHeightChanged(const FText & InText, ETextCommit::Type CommitType)
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

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateAddWallLengthWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetAddWallLength)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnAddWallLengthChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateAddWallHeightWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetAddWallHeight)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnAddWallHeightChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}


TSharedPtr<SWidget> FArmyHomeModeDetail::CreateAddWallMatTypeWidget()
{
	return
		SAssignNew(CB_AddMatType, SArmyPropertyComboBox)
		.OptionsSource(AddWallMatList)
		.OnSelectionChanged(this, &FArmyHomeModeDetail::OnSlectedAddMatTypeChanged)
		.Value(this, &FArmyHomeModeDetail::GetAddWallMatType);
}

void FArmyHomeModeDetail::OnSlectedAddMatTypeChanged(const int32 Key, const FString & Value)
{
	TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(SelectedXRObject);
	if (AddWall.IsValid())
	{
		AddWall->SetMatType(Value);
	}
}

FText FArmyHomeModeDetail::GetBearingWallLength() const
{
	TSharedPtr<FArmyBearingWall> BearingWall = StaticCastSharedPtr<FArmyBearingWall>(SelectedXRObject);
	if (BearingWall.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), BearingWall->GetLength() * 10.f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyHomeModeDetail::OnBearingWallLengthChanged(const FText& InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		float InPosX = FCString::Atof(*InText.ToString());
		TSharedPtr<FArmyBearingWall> BearingWall = StaticCastSharedPtr<FArmyBearingWall>(SelectedXRObject);
		if (BearingWall.IsValid())
		{
			//SCOPE_TRANSACTION(TEXT("修改拆改墙的长度"));
			BearingWall->SetLength(InPosX / 10.f);
		}
	}
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateBearingWallLengthWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetBearingWallLength)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnBearingWallLengthChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		.IsEnabled(false)
		.IsReadOnly(true)
		;
}

void FArmyHomeModeDetail::OnWallHeightChanged(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		float InHeight = FCString::Atof(*InText.ToString());
		if (InHeight != FArmySceneData::WallHeight)
		{			
			FArmySceneData::WallHeight = FMath::Clamp(InHeight, 2000.f, 10000.f) / 10.f;
			/** @欧石楠 修改全局墙高之后，将全部的整墙高度随之修改*/
			TArray<TWeakPtr<FArmyObject>> AllWholeWall;
			FArmySceneData::Get()->GetObjects(EModelType::E_ModifyModel, EObjectType::OT_AddWall, AllWholeWall);
			FArmySceneData::Get()->GetObjects(EModelType::E_ModifyModel, EObjectType::OT_IndependentWall, AllWholeWall);
			for (auto It : AllWholeWall)
			{
				TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(It.Pin());
				if (AddWall.IsValid() && !AddWall->GetIsHalfWall())
				{
					AddWall->SetHeight(FArmySceneData::WallHeight);
				}
			}

			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
	}		
}

void FArmyHomeModeDetail::OnWallColorChanged(const FLinearColor InColor)
{
    // 修改墙体颜色
    FArmySceneData::Get()->SetWallLinesColor(InColor);
}

FText FArmyHomeModeDetail::GetCommonLength() const
{
	TSharedPtr<FArmyHardware> HardWare = StaticCastSharedPtr<FArmyHardware>(SelectedXRObject);
	if (HardWare.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), HardWare->GetLength()*10.0f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

FText FArmyHomeModeDetail::GetCommonWidth() const
{
	TSharedPtr<FArmyHardware> HardWare = StaticCastSharedPtr<FArmyHardware>(SelectedXRObject);
	if (HardWare.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), HardWare->GetWidth()*10.0f));	
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

FText FArmyHomeModeDetail::GetCommonHeight() const
{
	TSharedPtr<FArmyHardware> HardWare = StaticCastSharedPtr<FArmyHardware>(SelectedXRObject);
	if (HardWare.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), HardWare->GetHeight()*10.0f));		
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

FText FArmyHomeModeDetail::GetCommonFloorAltitude() const
{
	TSharedPtr<FArmyHardware> HardWare = StaticCastSharedPtr<FArmyHardware>(SelectedXRObject);
	if (HardWare.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), HardWare->GetHeightToFloor()*10.0f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyHomeModeDetail::OnCommonLengthChanged(const FText & InText, ETextCommit::Type CommitType)
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

void FArmyHomeModeDetail::OnCommonWidthChanged(const FText & InText, ETextCommit::Type CommitType)
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

void FArmyHomeModeDetail::OnCommonHeightChanged(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		float InPosX = FCString::Atof(*InText.ToString());
		TSharedPtr<FArmyHardware> HardWare = StaticCastSharedPtr<FArmyHardware>(SelectedXRObject);
		if (HardWare.IsValid())
		{

			//@ 离地高度+窗高不能大于房高
			float WallHeight = HardWare->GetType() == OT_Punch ? FArmySceneData::WallHeight + 20 : FArmySceneData::WallHeight;
			if ((HardWare->GetHeightToFloor() + InPosX / 10) <= WallHeight)
			{
				SCOPE_TRANSACTION(TEXT("修改门窗的高度"));			
				HardWare->SetHeight(InPosX / 10.f);		
				HardWare->Modify();
				FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
			}
		}
	}
}

void FArmyHomeModeDetail::OnCommonFloorAltitudeChanged(const FText& InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		float InPosX = FCString::Atof(*InText.ToString());
		TSharedPtr<FArmyHardware> HardWare = StaticCastSharedPtr<FArmyHardware>(SelectedXRObject);
		if (HardWare.IsValid())
		{
			//@ 离地高度+窗高不能大于房高
			float WallHeight = HardWare->GetType() == OT_Punch ? FArmySceneData::WallHeight + 20 : FArmySceneData::WallHeight;
			if ((HardWare->GetHeight() + InPosX / 10) <= WallHeight)
			{
				SCOPE_TRANSACTION(TEXT("修改门窗的离地高度"));
				HardWare->SetHeightToFloor(InPosX / 10.f);
				HardWare->Modify();
				FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
			}
			else
			{
				/**@欧石楠 超高提示*/
				GGI->Window->ShowMessage(MT_Warning, TEXT("控件离地高度加自身高度已超出房高，请合理修改离地高度和自身高度！"));
			}
		}
	}
}

//void FArmyHomeModeDetail::OnWindowGroundHeightChanged(const FText& InText, ETextCommit::Type CommitType)
//{
//	if (CommitType == ETextCommit::OnEnter)
//	{
//		float InPosX = FCString::Atof(*InText.ToString());
//		if (SelectedXRObject->GetType() == OT_Window)
//		{
//			TSharedPtr<FArmyWindow> SelectedWindow = StaticCastSharedPtr<FArmyWindow>(SelectedXRObject);
//			//@ 离地高度+窗高不能大于房高
//			if (SelectedWindow.IsValid())
//			{
//				if ((SelectedWindow->GetHeight() + InPosX / 10) <= FArmySceneData::WallHeight)
//				{
//					SCOPE_TRANSACTION(TEXT("修改窗的离地高度"));
//					SelectedWindow->SetHeightToFloor(InPosX / 10.f);
//					SelectedWindow->Modify();
//					FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
//				}
//				/*else
//				{
//					GGI->Window->ShowMessage(MT_Warning, TEXT("窗户高度已超出房高"));
//				}*/
//			}
//		}
//	}
//}

//TSharedPtr<SWidget> FArmyHomeModeDetail::CreateWindowGroundHeightWidget()
//{
//	return 
//		SNew(SArmyEditableNumberBox)
//		.Text(this, &FArmyHomeModeDetail::GetWindowGroungHeight)
//		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnWindowGroundHeightChanged)
//		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
//		.IsFloatOnly(true)
//		;
//}

//FText FArmyHomeModeDetail::GetRectBayWindowAltitude() const
//{
//	TSharedPtr<FArmyRectBayWindow> SelectedRectBayWindow = StaticCastSharedPtr<FArmyRectBayWindow>(SelectedXRObject);
//	if (SelectedRectBayWindow.IsValid())
//	{
//		float TempAltitude = SelectedRectBayWindow->GetHeightToFloor();
//		return FText::FromString(FString::Printf(TEXT("%.2f"), TempAltitude * 10.f));
//	}
//	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
//}

//void FArmyHomeModeDetail::OnRectBayWindowAltitudeChanged(const FText & InText, ETextCommit::Type CommitType)
//{
//	if (CommitType == ETextCommit::OnEnter)
//	{
//		float Altitude = FCString::Atof(*InText.ToString());
//		TSharedPtr<FArmyRectBayWindow> SelectedRectBayWindow = StaticCastSharedPtr<FArmyRectBayWindow>(SelectedXRObject);
//		if (SelectedRectBayWindow.IsValid() && (SelectedRectBayWindow->GetHeight() + Altitude / 10) <= FArmySceneData::WallHeight)
//		{
//			SCOPE_TRANSACTION(TEXT("修改飘窗的离地高度"));
//			
//			SelectedRectBayWindow->SetHeightToFloor(Altitude / 10.f);
//			SelectedRectBayWindow->Modify();
//			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
//		}
//	}
//}

//TSharedPtr<SWidget> FArmyHomeModeDetail::CreateRectBayWindowAltitudeWidget()
//{
//	return
//		SNew(SArmyEditableNumberBox)
//		.Text(this, &FArmyHomeModeDetail::GetRectBayWindowAltitude)
//		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnRectBayWindowAltitudeChanged)
//		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
//		.IsFloatOnly(true)
//		;
//}

FText FArmyHomeModeDetail::GetRectBayWindowDepth() const
{
	TSharedPtr<FArmyRectBayWindow> SelectedRectBayWindow = StaticCastSharedPtr<FArmyRectBayWindow>(SelectedXRObject);
	if (SelectedRectBayWindow.IsValid())
	{
		float TempDepth = SelectedRectBayWindow->GetWindowHoleDepth();
		return FText::FromString(FString::Printf(TEXT("%.2f"), TempDepth * 10.f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyHomeModeDetail::OnRectBayWindowDepthChanged(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		TSharedPtr<FArmyRectBayWindow> SelectedRectBayWindow = StaticCastSharedPtr<FArmyRectBayWindow>(SelectedXRObject);
		if (SelectedRectBayWindow.IsValid())
		{
			SCOPE_TRANSACTION(TEXT("修改飘窗的进深"));			
			float Depth = FCString::Atof(*InText.ToString());
			SelectedRectBayWindow->SetWindowHoleDepth(Depth / 10.f);
			SelectedRectBayWindow->Modify();
			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateRectBayWindowDepthWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetRectBayWindowDepth)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnRectBayWindowDepthChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

//FText FArmyHomeModeDetail::GetRectBayWindowHeight() const
//{
//	TSharedPtr<FArmyRectBayWindow> SelectedRectBayWindow = StaticCastSharedPtr<FArmyRectBayWindow>(SelectedXRObject);
//	if (SelectedRectBayWindow.IsValid())
//	{
//		float TempHeight = SelectedRectBayWindow->GetHeight();
//		return FText::FromString(FString::Printf(TEXT("%.2f"), TempHeight * 10.f));
//	}
//	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
//}

//void FArmyHomeModeDetail::OnRectBayWindowHeightChanged(const FText & InText, ETextCommit::Type CommitType)
//{
//	if (CommitType == ETextCommit::OnEnter)
//	{
//		TSharedPtr<FArmyRectBayWindow> SelectedRectBayWindow = StaticCastSharedPtr<FArmyRectBayWindow>(SelectedXRObject);
//		if (SelectedRectBayWindow.IsValid())
//		{
//			float Height = FCString::Atof(*InText.ToString());
//			if ((SelectedRectBayWindow->GetHeightToFloor() + Height / 10) <= FArmySceneData::WallHeight)
//			{
//				SCOPE_TRANSACTION(TEXT("修改飘窗的高度"));
//				SelectedRectBayWindow->SetHeight(Height / 10.f);
//				SelectedRectBayWindow->Modify();
//				FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
//			}
//		}
//	}
//}

//TSharedPtr<SWidget> FArmyHomeModeDetail::CreateRectBayWindowHeightWidget()
//{
//	return
//		SNew(SArmyEditableNumberBox)
//		.Text(this, &FArmyHomeModeDetail::GetRectBayWindowHeight)
//		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnRectBayWindowHeightChanged)
//		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
//		.IsFloatOnly(true)
//		;
//}

FText FArmyHomeModeDetail::GetRectBayWindowType() const
{
	TSharedPtr<FArmyRectBayWindow> SelectedRectBayWindow = StaticCastSharedPtr<FArmyRectBayWindow>(SelectedXRObject);
	if (SelectedRectBayWindow.IsValid())
	{		
		for (auto It : RectBayWindowTypeList->Array)
		{
			if (It->Key == SelectedRectBayWindow->GetWindowType())
			{
				return FText::FromString(It->Value);
			}
		}		
	}
	return FText();
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateRectBayWindowTypeWidget()
{
	return
		SAssignNew(RectBayWindowTypeComboBox, SArmyPropertyComboBox)
		.OptionsSource(RectBayWindowTypeList)
		.OnSelectionChanged(this, &FArmyHomeModeDetail::OnSlectedRectBayTypeChanged)
		.Value(this, &FArmyHomeModeDetail::GetRectBayWindowType);
}

void FArmyHomeModeDetail::OnSlectedRectBayTypeChanged(const int32 Key, const FString & Value)
{
	TSharedPtr<FArmyRectBayWindow> SelectedRectBayWindow = StaticCastSharedPtr<FArmyRectBayWindow>(SelectedXRObject);
	if (SelectedRectBayWindow.IsValid())
	{
		SelectedRectBayWindow->SetWindowType(Key);
		FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
	}
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateRectBayWindowStoneWidget()
{
	return
		SNew(SCheckBox)
		.Style(FArmyStyle::Get(), "SingleCheckBox")
		.IsChecked(this, &FArmyHomeModeDetail::GetRectBayWindowStoneCheckBoxState)
		.OnCheckStateChanged(this, &FArmyHomeModeDetail::OnRectBayWindowStoneCheckBoxStateChanged);
}

void FArmyHomeModeDetail::OnRectBayWindowStoneCheckBoxStateChanged(ECheckBoxState InNewState)
{
	TSharedPtr<FArmyRectBayWindow> SelectedWindow = StaticCastSharedPtr<FArmyRectBayWindow>(SelectedXRObject);
	if (SelectedWindow.IsValid())
	{
		SelectedWindow->SetIfGenerateWindowStone(InNewState == ECheckBoxState::Checked ? true : false);
	}
}

ECheckBoxState FArmyHomeModeDetail::GetRectBayWindowStoneCheckBoxState() const
{
	TSharedPtr<FArmyRectBayWindow> SelectedWindow = StaticCastSharedPtr<FArmyRectBayWindow>(SelectedXRObject);
	if (SelectedWindow.IsValid())
	{
		return SelectedWindow->GetIfGenerateWindowStone() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}
	return ECheckBoxState::Unchecked;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateBayWindowOutLengthWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetBayWindowOutLength)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnBayWindowOutLengthChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

FText FArmyHomeModeDetail::GetBayWindowOutLength() const
{
	TSharedPtr<FArmyRectBayWindow> SelectedRectBayWindow = StaticCastSharedPtr<FArmyRectBayWindow>(SelectedXRObject);
	if (SelectedRectBayWindow.IsValid())
	{
		float TempOutLength = SelectedRectBayWindow->GetOutWindowLength();
		return FText::FromString(FString::Printf(TEXT("%.2f"), TempOutLength * 10.f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyHomeModeDetail::OnBayWindowOutLengthChanged(const FText & InText, ETextCommit::Type CommitType)
{
	TSharedPtr<FArmyRectBayWindow> SelectedRectBayWindow = StaticCastSharedPtr<FArmyRectBayWindow>(SelectedXRObject);
	if (SelectedRectBayWindow.IsValid())
	{
		SCOPE_TRANSACTION(TEXT("修改飘窗的外窗长度"));
		float OutLength = FCString::Atof(*InText.ToString());
		SelectedRectBayWindow->SetOutWindowLength(OutLength / 10.f);
		SelectedRectBayWindow->Modify();
		FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
	}
}

FText FArmyHomeModeDetail::GetSpaceName() const
{
    TSharedPtr<FArmyRoom> SelectedRoom = StaticCastSharedPtr<FArmyRoom>(SelectedXRObject);
    if (SelectedRoom.IsValid())
    {
        return FText::FromString(SelectedRoom->GetSpaceName());
    }

    return FText();
}

FText FArmyHomeModeDetail::GetSpaceArea() const
{
	TSharedPtr<FArmyRoom> SelectedRoom = StaticCastSharedPtr<FArmyRoom>(SelectedXRObject);
	if (SelectedRoom.IsValid())
	{
		float TempArea;
		float TempPerimeter;
		SelectedRoom->GetRoomAreaAndPerimeter(TempArea, TempPerimeter);
		return FText::FromString(FString::Printf(TEXT("%.2f"), TempArea));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

FText FArmyHomeModeDetail::GetSpacePerimeter() const
{
	TSharedPtr<FArmyRoom> SelectedRoom = StaticCastSharedPtr<FArmyRoom>(SelectedXRObject);
	if (SelectedRoom.IsValid())
	{
		float TempArea;
		float TempPerimeter;
		SelectedRoom->GetRoomAreaAndPerimeter(TempArea, TempPerimeter);
		return FText::FromString(FString::Printf(TEXT("%.2f"), TempPerimeter));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

int FArmyHomeModeDetail::GetOriginGroundHeight() const
{
	TSharedPtr<FArmyRoom> SelectedRoom = StaticCastSharedPtr<FArmyRoom>(SelectedXRObject);
	if (SelectedRoom.IsValid())
	{
		return (int)SelectedRoom->GetOriginGroundHeight();
	}	
	return 0;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateSpaceNameWidget()
{
    return
        SAssignNew(CB_SpaceName, SArmyPropertyComboBox)
        .OptionsSource(FArmyUser::Get().SpaceTypeList)
        .OnSelectionChanged(this, &FArmyHomeModeDetail::OnSlectedSpaceNameChanged)
        .Value(this, &FArmyHomeModeDetail::GetSpaceName);
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateSpaceAreaWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetSpaceArea)		
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))		
		.IsReadOnly(true)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateSpacePerimeterWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetSpacePerimeter)		
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsReadOnly(true)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateSpaceOriginGroundHeightWidget()
{
	return
		SNew(SSpinBox<int>)
		.Value(this, &FArmyHomeModeDetail::GetOriginGroundHeight)
		.Style(FArmyStyle::Get(), "SpinBoxStyle")
		.Font(FSlateFontInfo("PingFangSC-Regular", 12))
		.Delta(1)
		.MinSliderValue(0)
		.MaxSliderValue(10000)
		.MinValue(-1000)
		.MaxValue(1000)
		.OnValueChanged(this, &FArmyHomeModeDetail::OnOriginGroundHeightChanged)
		;
}

void FArmyHomeModeDetail::OnSlectedSpaceNameChanged(const int32 Key, const FString& Value)
{
    TSharedPtr<FArmyRoom> SelectedRoom = StaticCastSharedPtr<FArmyRoom>(SelectedXRObject);
    if (SelectedRoom.IsValid() && SelectedRoom->GetSpaceId() != Key)
    {
        SelectedRoom->SetSpaceId(Key);
		SelectedRoom->ModifySpaceName(Value);
    }
}

void FArmyHomeModeDetail::OnOriginGroundHeightChanged(int InValue)
{
	TSharedPtr<FArmyRoom> SelectedRoom = StaticCastSharedPtr<FArmyRoom>(SelectedXRObject);
	if (SelectedRoom.IsValid())
	{
		SelectedRoom->SetOriginGroundHeight(InValue);
	}
}

FText FArmyHomeModeDetail::GetComponentLength() const
{
	TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
	if (Furniture.IsValid())
	{		
		return FText::FromString(FString::Printf(TEXT("%.2f"), Furniture->GetBeamLength()*10.0f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyHomeModeDetail::OnComponentLengthChanged(const FText& InText, ETextCommit::Type CommitType)
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

FText FArmyHomeModeDetail::GetComponentWidth() const
{
	TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
	if (Furniture.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), Furniture->GetBeamWidth()*10.0f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyHomeModeDetail::OnComponentWidthChanged(const FText& InText, ETextCommit::Type CommitType)
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

FText FArmyHomeModeDetail::GetComponentHeight() const
{
	TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
	if (Furniture.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), Furniture->GetThickness()*10.0f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));	
}

void FArmyHomeModeDetail::OnComponentHeightChanged(const FText& InText, ETextCommit::Type CommitType)
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

FText FArmyHomeModeDetail::GetComponentThickness() const
{
	TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
	if (Furniture.IsValid())
	{		
		return FText::FromString(FString::Printf(TEXT("%.2f"), Furniture->GetThickness()*10.0f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyHomeModeDetail::OnComponentThicknessChanged(const FText& InText, ETextCommit::Type CommitType)
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

FText FArmyHomeModeDetail::GetComponentAngle() const
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

void FArmyHomeModeDetail::OnComponentAngleChanged(const FText& InText, ETextCommit::Type CommitType)
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

FText FArmyHomeModeDetail::GetComponentAltitude() const
{
	TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
	if (Furniture.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), Furniture->GetAltitude()*10.0f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyHomeModeDetail::OnComponentAltitudeChanged(const FText& InText, ETextCommit::Type CommitType)
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

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateComponentLengthWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetComponentLength)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnComponentLengthChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateComponentWidthWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetComponentWidth)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnComponentWidthChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateComponentHeightWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetComponentHeight)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnComponentHeightChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateComponentThicknessWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetComponentThickness)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnComponentThicknessChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateComponentAngleWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetComponentAngle)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnComponentAngleChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateComponentAltitudeWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetComponentAltitude)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnComponentAltitudeChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

FText FArmyHomeModeDetail::GetSimpleComponentLength() const
{
	TSharedPtr<FArmySimpleComponent> SimpleComponent = StaticCastSharedPtr<FArmySimpleComponent>(SelectedXRObject);
	if (SimpleComponent.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), SimpleComponent->GetLength()*10.0f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyHomeModeDetail::OnSimpleComponentLengthChanged(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		TSharedPtr<FArmySimpleComponent>SimpleComponent = StaticCastSharedPtr<FArmySimpleComponent>(SelectedXRObject);
		if (SimpleComponent.IsValid())
		{
			if (SimpleComponent->GetAutoAdapt())
			{
				return;
			}
			SCOPE_TRANSACTION(TEXT("修改简单构件的长度"));
			float Length = FCString::Atof(*InText.ToString());
			SimpleComponent->SetLength(Length / 10.f);
			SimpleComponent->Modify();
			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

FText FArmyHomeModeDetail::GetSimpleComponentWidth() const
{
	TSharedPtr<FArmySimpleComponent> SimpleComponent = StaticCastSharedPtr<FArmySimpleComponent>(SelectedXRObject);
	if (SimpleComponent.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), SimpleComponent->GetWidth()*10.0f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyHomeModeDetail::OnSimpleComponentWidthChanged(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		TSharedPtr<FArmySimpleComponent>SimpleComponent = StaticCastSharedPtr<FArmySimpleComponent>(SelectedXRObject);
		if (SimpleComponent.IsValid())
		{
			SCOPE_TRANSACTION(TEXT("修改简单构件的宽度"));
			float Width = FCString::Atof(*InText.ToString());
			SimpleComponent->SetWidth(Width / 10.f);
			SimpleComponent->AutoAdapt();
			SimpleComponent->Modify();
			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

FText FArmyHomeModeDetail::GetSimpleComponentHeight() const
{
	TSharedPtr<FArmySimpleComponent> SimpleComponent = StaticCastSharedPtr<FArmySimpleComponent>(SelectedXRObject);
	if (SimpleComponent.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), SimpleComponent->GetHeight()*10.0f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyHomeModeDetail::OnSimpleComponentHeightChanged(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		TSharedPtr<FArmySimpleComponent>SimpleComponent = StaticCastSharedPtr<FArmySimpleComponent>(SelectedXRObject);
		if (SimpleComponent.IsValid())
		{
			SCOPE_TRANSACTION(TEXT("修改简单构件的高度"));
			float Height = FCString::Atof(*InText.ToString());
			SimpleComponent->SetHeight(Height / 10.f);
			SimpleComponent->Modify();
			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

FText FArmyHomeModeDetail::GetSimpleComponentAngle() const
{
	TSharedPtr<FArmySimpleComponent> SimpleComponent = StaticCastSharedPtr<FArmySimpleComponent>(SelectedXRObject);
	if (SimpleComponent.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), SimpleComponent->GetAngle()));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyHomeModeDetail::OnSimpleComponentAngleChanged(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		TSharedPtr<FArmySimpleComponent>SimpleComponent = StaticCastSharedPtr<FArmySimpleComponent>(SelectedXRObject);
		if (SimpleComponent.IsValid())
		{
			SCOPE_TRANSACTION(TEXT("修改简单构件的角度"));
			float Angle = FCString::Atof(*InText.ToString());
			SimpleComponent->SetAngle(Angle);
			SimpleComponent->AutoAdapt();
			SimpleComponent->Modify();
			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

void FArmyHomeModeDetail::OnAutoAdaptCheckBoxStateChanged(ECheckBoxState InNewState)
{
	TSharedPtr<FArmySimpleComponent> SimpleComponent = StaticCastSharedPtr<FArmySimpleComponent>(SelectedXRObject);
	if (SimpleComponent.IsValid())
	{		
		SimpleComponent->SetAutoAdapt(InNewState == ECheckBoxState::Checked ? true : false);		
	}
}

ECheckBoxState FArmyHomeModeDetail::GetAutoAdaptCheckBoxState() const
{
	TSharedPtr<FArmySimpleComponent> SimpleComponent = StaticCastSharedPtr<FArmySimpleComponent>(SelectedXRObject);
	if (SimpleComponent.IsValid())
	{		
		return SimpleComponent->GetAutoAdapt() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}	
	return ECheckBoxState::Checked;
}

void FArmyHomeModeDetail::OnAutoAdaptDirCheckBoxStateChanged(ECheckBoxState InNewState)
{
	TSharedPtr<FArmySimpleComponent> SimpleComponent = StaticCastSharedPtr<FArmySimpleComponent>(SelectedXRObject);
	if (SimpleComponent.IsValid())
	{		
		SimpleComponent->SetAutoAdaptDir(InNewState == ECheckBoxState::Checked ? true : false);
	}
}

ECheckBoxState FArmyHomeModeDetail::GetAutoAdaptDirCheckBoxState() const
{
	TSharedPtr<FArmySimpleComponent> SimpleComponent = StaticCastSharedPtr<FArmySimpleComponent>(SelectedXRObject);
	if (SimpleComponent.IsValid())
	{
		return SimpleComponent->GetAutoAdaptDir() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}
	return ECheckBoxState::Checked;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateSimpleComponentLengthWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetSimpleComponentLength)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnSimpleComponentLengthChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateSimpleComponentWidthWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetSimpleComponentWidth)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnSimpleComponentWidthChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateSimpleComponentHeightWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetSimpleComponentHeight)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnSimpleComponentHeightChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateSimpleComponentAngleWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetSimpleComponentAngle)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnSimpleComponentAngleChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateSimpleComponentAutoAdaptCheckWidget()
{
	return
		SNew(SCheckBox)
        .Style(FArmyStyle::Get(), "SingleCheckBox")
		.IsChecked(this, &FArmyHomeModeDetail::GetAutoAdaptCheckBoxState)
		.OnCheckStateChanged(this, &FArmyHomeModeDetail::OnAutoAdaptCheckBoxStateChanged);
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateSimpleComponentAutoAdaptDirCheckWidget()
{
	return
		SNew(SCheckBox)
        .Style(FArmyStyle::Get(), "SingleCheckBox")
		.IsChecked(this, &FArmyHomeModeDetail::GetAutoAdaptDirCheckBoxState)
		.OnCheckStateChanged(this, &FArmyHomeModeDetail::OnAutoAdaptDirCheckBoxStateChanged);
}

FText FArmyHomeModeDetail::GetPointPositionLength() const
{
	TSharedPtr<FArmyFurniture> FurnitureComponent = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
	if (FurnitureComponent.IsValid() && FurnitureComponent->GetFurniturePro().IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), FurnitureComponent->GetFurniturePro()->GetLength() * 10.f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyHomeModeDetail::OnPointPositionLengthChanged(const FText& InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
		if (Furniture.IsValid() && Furniture->GetFurniturePro().IsValid())
		{
			SCOPE_TRANSACTION(TEXT("修改长度"));
			float Length = FCString::Atof(*InText.ToString()) / 10.f;
			Furniture->GetFurniturePro()->SetLength(Length);
			Furniture->Modify();
			//Furniture->ModifyFurniturePro_LayoutData(0, Length);
			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

FText FArmyHomeModeDetail::GetPointPositionWidth() const
{
	TSharedPtr<FArmyFurniture> FurnitureComponent = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
	if (FurnitureComponent.IsValid() && FurnitureComponent->GetFurniturePro().IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), FurnitureComponent->GetFurniturePro()->GetWidth() * 10.f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyHomeModeDetail::OnPointPositionWidthChanged(const FText& InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
		if (Furniture.IsValid() && Furniture->GetFurniturePro().IsValid())
		{
			SCOPE_TRANSACTION(TEXT("修改宽度"));
			float Width = FCString::Atof(*InText.ToString()) / 10.f;
			Furniture->GetFurniturePro()->SetWidth(Width);
			Furniture->Modify();
			//Furniture->ModifyFurniturePro_LayoutData(1, Width);
			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

FText FArmyHomeModeDetail::GetPointPositionHeight() const
{
	TSharedPtr<FArmyFurniture> FurnitureComponent = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
	if (FurnitureComponent.IsValid() && FurnitureComponent->GetFurniturePro().IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), FurnitureComponent->GetFurniturePro()->GetHeight() * 10.f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyHomeModeDetail::OnPointPositionHeightChanged(const FText& InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
		if (Furniture.IsValid() && Furniture->GetFurniturePro().IsValid())
		{
			float Height = FCString::Atof(*InText.ToString()) / 10.f;
			float Altitude = Furniture->GetFurniturePro()->GetAltitude() > 0 ? Furniture->GetFurniturePro()->GetAltitude() : 0;
			if ((Altitude + Height) <= FArmySceneData::WallHeight)
			{
				SCOPE_TRANSACTION(TEXT("修改高度"));
				Furniture->GetFurniturePro()->SetHeight(Height);
				Furniture->Modify();
				//Furniture->ModifyFurniturePro_LayoutData(2, Height);
				FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
			}
		}
	}
}

FText FArmyHomeModeDetail::GetPipeRadius() const
{
	TSharedPtr<FArmyFurniture> FurnitureComponent = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
	if (FurnitureComponent.IsValid() && FurnitureComponent->GetFurniturePro().IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), FurnitureComponent->GetFurniturePro()->GetRadius() * 10.f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyHomeModeDetail::OnPipeRadiusChanged(const FText& InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
		if (Furniture.IsValid() && Furniture->GetFurniturePro().IsValid())
		{
			SCOPE_TRANSACTION(TEXT("修改管径"));
			float Radius = FCString::Atof(*InText.ToString()) / 10.f;
			Furniture->GetFurniturePro()->SetRadius(Radius);
			Furniture->Modify();
			//Furniture->ModifyFurniturePro_LayoutData(4, Radius);
			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

FText FArmyHomeModeDetail::GetPointPositionAltitude() const
{
	TSharedPtr<FArmyFurniture> FurnitureComponent = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
	if (FurnitureComponent.IsValid() && FurnitureComponent->GetFurniturePro().IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), FurnitureComponent->GetFurniturePro()->GetAltitude() * 10.f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

void FArmyHomeModeDetail::OnPointPositionAltitudeChanged(const FText& InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		TSharedPtr<FArmyFurniture>Furniture = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
		if (Furniture.IsValid() && Furniture->GetFurniturePro().IsValid())
		{
			float Altitude = FCString::Atof(*InText.ToString()) / 10.f;
			float Height = Furniture->GetFurniturePro()->GetHeight() > 0 ? Furniture->GetFurniturePro()->GetHeight() : 0;
			if ((Height + Altitude) <= FArmySceneData::WallHeight)
			{
				Furniture->GetFurniturePro()->SetAltitude(Altitude);
				Furniture->Modify();
				//Furniture->ModifyFurniturePro_LayoutData(3, Altitude);
				FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
			}
		}
	}
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreatePointPositionLengthWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetPointPositionLength)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnPointPositionLengthChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreatePointPositionWidthWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetPointPositionWidth)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnPointPositionWidthChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreatePointPositionHeightWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetPointPositionHeight)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnPointPositionHeightChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreatePointPositionAltitudeWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetPointPositionAltitude)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnPointPositionAltitudeChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreatePipeRadiusWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetPipeRadius)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnPipeRadiusChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true);
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateModifyComboBoxWidget()
{
	return
		SAssignNew(CB_Modify, SArmyPropertyComboBox)
		.OptionsSource(ModifyPointLocation)
		.OnSelectionChanged(this, &FArmyHomeModeDetail::OnSlectedModifyComboBoxChanged)
		.Value(this, &FArmyHomeModeDetail::GetModifyComboBoxSize);
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreatePointPositionLengthReadOnlyWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetPointPositionLength)
		//.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnPointPositionLengthChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		.IsReadOnly(true)
		.IsEnabled(false)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreatePointPositionWidthReadOnlyWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetPointPositionWidth)
		//.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnPointPositionWidthChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		.IsEnabled(false)
		.IsReadOnly(true)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreatePointPositionHeightReadOnlyWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetPointPositionHeight)
		//.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnPointPositionHeightChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		.IsReadOnly(true)
		.IsEnabled(false)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreatePointPositionAltitudeReadOnlyWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetPointPositionAltitude)
		//.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnPointPositionAltitudeChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		.IsReadOnly(true)
		.IsEnabled(false)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreatePipeRadiusReadOnlyWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetPipeRadius)
		//.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnPipeRadiusChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		.IsEnabled(false)
		.IsReadOnly(true);
}

FText FArmyHomeModeDetail::GetModifyComboBoxSize() const
{
	TSharedPtr<FArmyFurniture> Fur = StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
	if (Fur.IsValid() && Fur->GetFurniturePro().IsValid())
	{
		return FText::FromString(FString::SanitizeFloat(Fur->GetFurniturePro()->GetRadius() * 10.f)) ;
	}
	return FText();
}

void FArmyHomeModeDetail::OnSlectedModifyComboBoxChanged(const int32 Key, const FString& Value)
{
	TSharedPtr<FArmyFurniture> Fur= StaticCastSharedPtr<FArmyFurniture>(SelectedXRObject);
	if (Fur.IsValid() && Fur->GetFurniturePro().IsValid())
	{
		Fur->GetFurniturePro()->SetRadius(FCString::Atof(*Value) / 10.f);
		FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
	}
}

//float FArmyHomeModeDetail::GetHardWareAltitude(TSharedPtr<FArmyHardware> InHardWare)
//{
//	float Altitude = 0.f;
//	if (InHardWare.IsValid())
//	{
//		TSharedPtr<FArmyWindow> Window = nullptr;
//		TSharedPtr<FArmyRectBayWindow> RectBayWindow = nullptr;
//		TSharedPtr<FArmyFloorWindow> FloorWindow = nullptr;
//		TSharedPtr<FArmyTrapeBayWindow> TrapeBayWindow = nullptr;
//		TSharedPtr<FArmyPunch> Punch = nullptr;
//		switch (InHardWare->GetType())
//		{
//		case  OT_Window:
//			Window = StaticCastSharedPtr<FArmyWindow>(InHardWare);
//			Altitude = Window->GetHeightToFloor();
//			break;
//		case  OT_RectBayWindow:
//			RectBayWindow = StaticCastSharedPtr<FArmyRectBayWindow>(InHardWare);
//			Altitude = RectBayWindow->GetHeightToFloor();
//			break;
//		case  OT_FloorWindow:
//			FloorWindow = StaticCastSharedPtr<FArmyFloorWindow>(InHardWare);
//			Altitude = FloorWindow->GetHeightToFloor();
//			break;
//		case  OT_TrapeBayWindow:
//			TrapeBayWindow = StaticCastSharedPtr<FArmyTrapeBayWindow>(InHardWare);
//			Altitude = TrapeBayWindow->GetHeightToFloor();
//			break;
//		case  OT_Punch:
//			Punch = StaticCastSharedPtr<FArmyPunch>(InHardWare);
//			Altitude = Punch->GetHeightToFloor();
//			break;
//		default:
//			break;
//		}
//	}
//
//	return (Altitude >= 0 && Altitude <= FArmySceneData::WallHeight) ? Altitude : 0.f;
//}

TSharedRef<SWidget> FArmyHomeModeDetail::CreatePassCheckBoxDetail()
{
	return
		SNew(SCheckBox)
        .Style(FArmyStyle::Get(), "SingleCheckBox")
		.IsChecked(this, &FArmyHomeModeDetail::GetPassCheckBoxState)
		.OnCheckStateChanged(this, &FArmyHomeModeDetail::OnPassCheckBoxStateChanged);
}

void FArmyHomeModeDetail::OnPassCheckBoxStateChanged(ECheckBoxState InNewState)
{	
	TSharedPtr<FArmyPass> SelectedPass = StaticCastSharedPtr<FArmyPass>(SelectedXRObject);
	if (SelectedPass.IsValid())
	{
		SelectedPass->SetIfGeneratePassModel(InNewState == ECheckBoxState::Checked ? true : false);
	}		
}

ECheckBoxState FArmyHomeModeDetail::GetPassCheckBoxState() const
{
	TSharedPtr<FArmyPass> SelectedPass = StaticCastSharedPtr<FArmyPass>(SelectedXRObject);
	if (SelectedPass.IsValid())
	{
		return SelectedPass->GetIfGeneratePassModel() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;		
	}
	return ECheckBoxState::Unchecked;
}

void FArmyHomeModeDetail::OnSlectedAddDoorHoleMatTypeChanged(const int32 Key, const FString& Value)
{
	TSharedPtr<FArmyPass> DoorHole = StaticCastSharedPtr<FArmyPass>(SelectedXRObject);
	if (DoorHole.IsValid())
	{
		DoorHole->SetAddDoorHoleMatType(FText::FromString(Value));
	}
}

void FArmyHomeModeDetail::OnSlectedModifyDoorHoleMatTypeChanged(const int32 Key, const FString& Value)
{
	TSharedPtr<FArmyPass> DoorHole = StaticCastSharedPtr<FArmyPass>(SelectedXRObject);
	if (DoorHole.IsValid())
	{
		DoorHole->SetModifyDoorHoleMatType(FText::FromString(Value));
	}
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateAddDoorHoleMatTypeWidget()
{
	return
		SAssignNew(CB_AddDoorHoleMatType, SArmyPropertyComboBox)
		.OptionsSource(AddDoorHoleMatList)
		.OnSelectionChanged(this, &FArmyHomeModeDetail::OnSlectedAddDoorHoleMatTypeChanged)
		.Value(this, &FArmyHomeModeDetail::GetAddDoorHoleMatType);
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateModifyDoorHoleMatTypeWidget()
{
	return
		SAssignNew(CB_ModifyDoorHoleMatType, SArmyPropertyComboBox)
		.OptionsSource(ModifyDoorHoleMatList)
		.OnSelectionChanged(this, &FArmyHomeModeDetail::OnSlectedModifyDoorHoleMatTypeChanged)
		.Value(this, &FArmyHomeModeDetail::GetModifyDoorHoleMatType);
}

FText FArmyHomeModeDetail::GetAddDoorHoleMatType() const
{
	TSharedPtr<FArmyPass> DoorHole = StaticCastSharedPtr<FArmyPass>(SelectedXRObject);
	if (DoorHole.IsValid())
	{
		return DoorHole->GetAddDoorHoleMatType();
	}
	return FText::FromString(FString::Printf(TEXT("无")));
}

FText FArmyHomeModeDetail::GetModifyDoorHoleMatType() const
{
	TSharedPtr<FArmyPass> DoorHole = StaticCastSharedPtr<FArmyPass>(SelectedXRObject);
	if (DoorHole.IsValid())
	{
		return DoorHole->GetModifyDoorHoleMatType();
	}
	return FText::FromString(FString::Printf(TEXT("无"), 0.0f));
}

//FText FArmyHomeModeDetail::GetPunchGroungHeight() const
//{
//	TSharedPtr<FArmyPunch> SelectedPunch = StaticCastSharedPtr<FArmyPunch>(SelectedXRObject);
//	if (SelectedPunch.IsValid())
//	{
//		return FText::FromString(FString::Printf(TEXT("%.2f"), SelectedPunch->GetHeightToFloor()*10.0f));
//	}
//	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
//}

FText FArmyHomeModeDetail::GetPunchDepth() const
{
	TSharedPtr<FArmyPunch> SelectedPunch = StaticCastSharedPtr<FArmyPunch>(SelectedXRObject);
	if (SelectedPunch.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("%.2f"), SelectedPunch->GetPunchDepth()*10.0f));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
}

//void FArmyHomeModeDetail::OnPunchGroundHeightChanged(const FText & InText, ETextCommit::Type CommitType)
//{
//	if (CommitType == ETextCommit::OnEnter)
//	{
//		float Altitude = FCString::Atof(*InText.ToString()) / 10;
//		TSharedPtr<FArmyPunch> SelectedPunch = StaticCastSharedPtr<FArmyPunch>(SelectedXRObject);
//		if (SelectedPunch.IsValid() && SelectedPunch->GetHeight() + Altitude <= FArmySceneData::WallHeight + 20)
//		{
//			SCOPE_TRANSACTION(TEXT("修改开洞的离地高度"));
//			SelectedPunch->SetHeightToFloor(Altitude);
//			SelectedPunch->Modify();
//			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
//		}
//	}
//}

void FArmyHomeModeDetail::OnPunchDepthChanged(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		TSharedPtr<FArmyPunch> SelectedPunch = StaticCastSharedPtr<FArmyPunch>(SelectedXRObject);
		if (SelectedPunch.IsValid())
		{
			SCOPE_TRANSACTION(TEXT("修改开洞的深度"));
			float Altitude = FCString::Atof(*InText.ToString());
			SelectedPunch->SetPunchDepth(Altitude / 10.f);
			SelectedPunch->Modify();
			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
	}
}

//TSharedPtr<SWidget> FArmyHomeModeDetail::CreatePunchGroundHeightWidget()
//{
//	return
//		SNew(SArmyEditableNumberBox)
//		.Text(this, &FArmyHomeModeDetail::GetPunchGroungHeight)
//		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnPunchGroundHeightChanged)
//		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
//		.IsFloatOnly(true)
//		;
//}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreatePunchDepthWidget()
{
	return
		SNew(SArmyEditableNumberBox)
		.Text(this, &FArmyHomeModeDetail::GetPunchDepth)
		.OnTextCommitted_Raw(this, &FArmyHomeModeDetail::OnPunchDepthChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.IsFloatOnly(true)
		;
}

TSharedPtr<SWidget> FArmyHomeModeDetail::CreateWindowStoneCheckBoxWidget()
{
	return
		SNew(SCheckBox)
		.Style(FArmyStyle::Get(), "SingleCheckBox")
		.IsChecked(this, &FArmyHomeModeDetail::GetWindowStoneCheckBoxState)
		.OnCheckStateChanged(this, &FArmyHomeModeDetail::OnWindowStoneCheckBoxStateChanged);
}

void FArmyHomeModeDetail::OnWindowStoneCheckBoxStateChanged(ECheckBoxState InNewState)
{
	TSharedPtr<FArmyWindow> SelectedWindow = StaticCastSharedPtr<FArmyWindow>(SelectedXRObject);
	if (SelectedWindow.IsValid())
	{
		SelectedWindow->SetIfGenerateWindowStone(InNewState == ECheckBoxState::Checked ? true : false);
	}
}

ECheckBoxState FArmyHomeModeDetail::GetWindowStoneCheckBoxState() const
{
	TSharedPtr<FArmyWindow> SelectedWindow = StaticCastSharedPtr<FArmyWindow>(SelectedXRObject);
	if (SelectedWindow.IsValid())
	{
		return SelectedWindow->GetIfGenerateWindowStone() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}
	return ECheckBoxState::Unchecked;
}

//
//FText FArmyHomeModeDetail::GetWindowGroungHeight() const
//{
//	TSharedPtr<FArmyWindow> SelectedWindow = StaticCastSharedPtr<FArmyWindow>(SelectedXRObject);
//	if (SelectedWindow.IsValid())
//	{
//		return FText::FromString(FString::Printf(TEXT("%.2f"), SelectedWindow->GetHeightToFloor()*10.0f));		
//	}	
//	return FText::FromString(FString::Printf(TEXT("%.2f"), 0.0f));
//}
