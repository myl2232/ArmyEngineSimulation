#include "ArmyHardModeDetail.h"
#include "SWidgetSwitcher.h"
#include "SArmyDetailComponent.h"
#include "SColorPicker.h"
#include "STextBlockImageSpinbox.h"
#include "SArmyComboBox.h"
#include "ArmyEditorEngine.h"
#include "ArmySlateModule.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/SkyLight.h"
#include "Engine/DirectionalLight.h"
#include "Runtime/Engine/Classes/Components/DirectionalLightComponent.h"
#include "EngineUtils.h"
#include "Engine/PointLight.h"
#include "Engine/SpotLight.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "ArmyWorldManager.h"
#include "StaticLightingPrivate.h"

void FArmyHardModeDetail::CreateWorldSettingsDetail()
{
	//--------------------------------------------------------------------------------------------------SimpleWorldSettingsBuilder
	TSharedPtr<FArmyDetailBuilder> SimpleWorldSettingsBuilder = MakeShareable(new FArmyDetailBuilder());

	FArmyDetailNode& SimpleCategory = SimpleWorldSettingsBuilder->AddCategory("SimpleRenderingSettings", TEXT("全局设置"));
	SimpleCategory.AddChildNode("AutoExposureBias", TEXT("整体亮度"), CreateAutoExposureBias()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	SimpleCategory.AddChildNode("PPBloomIntensity", TEXT("曝光强度"), CreatePPBloomIntensity()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));

	//--------------------------------------------------------------------------------------------------AdvancedWorldSettingsBuilder

	TSharedPtr<FArmyDetailBuilder> AdvancedWorldSettingsBuilder = MakeShareable(new FArmyDetailBuilder());

	FArmyDetailNode& Category0 = AdvancedWorldSettingsBuilder->AddCategory("DirectionalLight", TEXT("太阳光设置"));
	Category0.AddChildNode("DirectionalLightIntensity", TEXT("亮度"), CreateDirectionalLightIntensity()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category0.AddChildNode("DirectionalLightColor", TEXT("颜色"), CreateDirectionalLightColor()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category0.AddChildNode("DirectionalLightIndirectIntensity", TEXT("间接光亮度"), CreateDirectionalLightIndirectIntensity()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category0.AddChildNode("DirectionalLightStationary", TEXT("动静结合态"), CreateDirectionalLightStationary()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category0.AddChildNode("DirectionalLightSource", TEXT("光源角度"), CreateDirectionalLightSource()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category0.AddChildNode("DirectionalLightShadowExponent", TEXT("阴影指数"), CreateDirectionalLightShadowExponent()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category0.AddChildNode("DirectionalLightPitch", TEXT("高度角"), CreateDirectionalLightPitch()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category0.AddChildNode("DirectionalLightYaw", TEXT("方向角"), CreateDirectionalLightYaw()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));

	FArmyDetailNode& CategoryStaticSkyLight = AdvancedWorldSettingsBuilder->AddCategory("SkyLight", TEXT("天光设置"));
	CategoryStaticSkyLight.AddChildNode("SkyLightIntensity", TEXT("亮度"), CreateSkyLightIntensity()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	CategoryStaticSkyLight.AddChildNode("SkyLightIndirectIntensity", TEXT("间接光亮度"), CreateSkyLightIndirectIntensity()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));

	FArmyDetailNode& Category1 = AdvancedWorldSettingsBuilder->AddCategory("Postprocess", TEXT("后期设置"));
	Category1.AddChildNode("AutoExposureBias", TEXT("整体亮度"), CreateAutoExposureBias()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category1.AddChildNode("PPIndirectLightingIntensity", TEXT("间接光亮度"), CreatePPIndirectLightingIntensity()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category1.AddChildNode("PPBloomIntensity", TEXT("曝光强度"), CreatePPBloomIntensity()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category1.AddChildNode("PPBloomThreshold", TEXT("曝光阈值"), CreatePPBloomThreshold()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));

	FArmyDetailNode& Category2 = AdvancedWorldSettingsBuilder->AddCategory("WorldSetting", TEXT("世界设置"));
	Category2.AddChildNode("StaticLightingLevelScale", TEXT("静态光照场景缩放"), CreateStaticLightingLevelScale()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category2.AddChildNode("NumIndirectLightingBounces", TEXT("间接光反弹次数"), CreateNumIndirectLightingBounces()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category2.AddChildNode("NumSkyLightingBounces", TEXT("天光反弹次数"), CreateNumSkyLightingBounces()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category2.AddChildNode("IndirectLightingQuality", TEXT("间接光质量"), CreateIndirectLightingQuality()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category2.AddChildNode("IndirectLightingSmoothness", TEXT("间接光平滑"), CreateIndirectLightingSmoothness()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category2.AddChildNode("MaxOccllusionDistance", TEXT("最大遮罩距离"), CreateMaxOccllusionDistance()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category2.AddChildNode("EnvironmentColor", TEXT("环境颜色"), CreateEnvironmentColor()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category2.AddChildNode("EnvironmentIntensity", TEXT("环境亮度"), CreateEnvironmentIntensity()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category2.AddChildNode("DiffuseBoost", TEXT("漫反射值增益"), CreateDiffuseBoost()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category2.AddChildNode("VolumetricLightmapDetailCellSize", TEXT("体积光照密度"), CreateVolumetricLightmapDetailCellSize()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category2.AddChildNode("VolumetricLightmapMaximumBirckMemoryMb", TEXT("体积光照缓存"), CreateVolumetricLightmapMaximumBirckMemoryMb()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category2.AddChildNode("CeilingLampIntensity", TEXT("灯带亮度"), CreateCeilingLampIntensity()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));

	FArmyDetailNode& Category3 = AdvancedWorldSettingsBuilder->AddCategory("LightMapResolution", TEXT("光照图分辨率设置"));
	Category3.AddChildNode("LightmapResScale", TEXT("全局缩放"), CreateLightmapResScale()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category3.AddChildNode("SkirtLineStaticLightingRes", TEXT("顶/底角线分辨率"), CreateSkirtLineStaticLightingRes()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category3.AddChildNode("MoldingLineStaticLightingRes", TEXT("造型线分辨率"), CreateMoldingLineStaticLightingRes()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	Category3.AddChildNode("WindowStaticLightingRes", TEXT("窗户分辨率"), CreateWindowStaticLightingRes()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));

	FArmyDetailNode& CategoryOther = AdvancedWorldSettingsBuilder->AddCategory("OtherSetting", TEXT("其他设置"));
	CategoryOther.AddChildNode("BuildStaticMesh", TEXT("渲染家具"), CreateBuildStaticMesh()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	CategoryOther.AddChildNode("ShowLightmapUV", TEXT("显示光照UV"), CreateShowLightmapUV()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	CategoryOther.AddChildNode("UVChannel", TEXT("UV通道"), CreateUVChannel()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	CategoryOther.AddChildNode("CameraFOV", TEXT("相机广角"), CreateFOV()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	CategoryOther.AddChildNode("OutdoorPanoramaIndex", TEXT("外景更换"), CreateOutdoorPanoramaIndex()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	CategoryOther.AddChildNode("OutdoorPanoramaYaw", TEXT("外景旋转"), CreateOutdoorPanoramaYaw()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));


	DetailBuilderMap.Add(EPropertyDetailType::PDT_WorldSettings, SimpleWorldSettingsBuilder);
	SWidgetMap.Add(EPropertyDetailType::PDT_WorldSettings, SimpleWorldSettingsBuilder->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_WorldSettings)->Get()->AsShared()
		];


	DetailBuilderMap.Add(EPropertyDetailType::PDT_WorldSettingsAdvanced, AdvancedWorldSettingsBuilder);
	SWidgetMap.Add(EPropertyDetailType::PDT_WorldSettingsAdvanced, AdvancedWorldSettingsBuilder->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_WorldSettingsAdvanced)->Get()->AsShared()
		];
}

void FArmyHardModeDetail::InitializeWorldSettings()
{
	UDirectionalLightComponent* DCM = NULL;
	if (FArmyWorldManager::Get().StaticSunLight)
		DCM = Cast<UDirectionalLightComponent>(FArmyWorldManager::Get().StaticSunLight->GetLightComponent());
	APostProcessVolume* PPV_StaticLighting = FArmyWorldManager::Get().PPV_StaticLighting;

	float CeilingLampIntensity = 600.f;
	for (TActorIterator<APointLight> It(GWorld); It; ++It)
	{
		UPointLightComponent* LM = (*It)->PointLightComponent;
		if (LM->GetName() == "LampSlotPointLightComponent")
		{
			CeilingLampIntensity = LM->Intensity;
			break;
		}
	}

	CachedDirectionalLightIntensity = DCM->Intensity;
	OnDirectionalLightColorChanged(DCM->LightColor);
	CachedDirectionalLightIndirectIntensity = DCM->IndirectLightingIntensity;
	CachedDirectionalLightStationary = DCM->Mobility == EComponentMobility::Stationary ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	CachedDirectionalLightSource = DCM->LightmassSettings.LightSourceAngle;
	CachedDirectionalLightShadowExponent = DCM->LightmassSettings.ShadowExponent;
	CachedDirectionalLightPitch = FArmyWorldManager::Get().StaticSunLight->GetActorRotation().Pitch;
	CachedDirectionalLightYaw = FArmyWorldManager::Get().StaticSunLight->GetActorRotation().Yaw;
	CachedSkyLightIntensity = FArmyWorldManager::Get().StaticSkyLight->GetLightComponent()->Intensity;
	CachedSkyLightIndirectIntensity = FArmyWorldManager::Get().StaticSkyLight->GetLightComponent()->IndirectLightingIntensity;
	CachedAutoExposureBias = PPV_StaticLighting->Settings.AutoExposureBias;
	CachedPPIndirectLightingIntensity = PPV_StaticLighting->Settings.IndirectLightingIntensity;
	CachedPPBloomIntensity = PPV_StaticLighting->Settings.BloomIntensity;
	CachedPPBloomThreshold = PPV_StaticLighting->Settings.BloomThreshold;
	CachedStaticLightingLevelScale = GWorld->GetWorldSettings()->LightmassSettings.StaticLightingLevelScale;
	CachedNumIndirectLightingBounces = GWorld->GetWorldSettings()->LightmassSettings.NumIndirectLightingBounces;
	CachedNumSkyLightingBounces = GWorld->GetWorldSettings()->LightmassSettings.NumSkyLightingBounces;
	CachedIndirectLightingQuality = GWorld->GetWorldSettings()->LightmassSettings.IndirectLightingQuality;
	CachedIndirectLightingSmoothness = GWorld->GetWorldSettings()->LightmassSettings.IndirectLightingSmoothness;
	CachedMaxOccllusionDistance = GWorld->GetWorldSettings()->LightmassSettings.MaxOcclusionDistance;
	OnEnvironmentColorChanged(GWorld->GetWorldSettings()->LightmassSettings.EnvironmentColor);
	CachedEnvironmentIntensity = GWorld->GetWorldSettings()->LightmassSettings.EnvironmentIntensity;
	CachedDiffuseBoost = GWorld->GetWorldSettings()->LightmassSettings.DiffuseBoost;
	CachedVolumetricLightmapDetailCellSize = GWorld->GetWorldSettings()->LightmassSettings.VolumetricLightmapDetailCellSize;
	CachedVolumetricLightmapMaximumBirckMemoryMb = GWorld->GetWorldSettings()->LightmassSettings.VolumetricLightmapMaximumBrickMemoryMb;
	CachedCeilingLampIntensity = CeilingLampIntensity;
	CachedLightmapResScale = FArmyWorldManager::Get().GetGlobalStaticLightingResScale();
	CachedSkirtLineStaticLightingRes = FArmyWorldManager::Get().GetSkirtLineStaticLightingRes();
	CachedMoldingLineStaticLightingRes = FArmyWorldManager::Get().GetMoldingLineStaticLightingRes();
	CachedWindowStaticLightingRes = FArmyWorldManager::Get().GetWindowStaticLightingRes();
	CachedBuildStaticMesh = FArmyWorldManager::Get().bBuildStaticMesh ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	CachedShowLightmapUV = FArmyWorldManager::Get().bShowLightmapUV ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	CachedUVChannel = FArmyWorldManager::Get().UVChannelToShow;
	CachedFOV = FArmyWorldManager::Get().GetCameraFOV();
	CachedOutdoorPanoramaIndex = FArmyWorldManager::Get().GetOutdoorPanoramaIndex();
	CachedOutdoorPanoramaYaw = FArmyWorldManager::Get().GetOutdoorPanoramaYaw();
}

void FArmyHardModeDetail::TryToggleWorldSettings()
{
	//@马云龙 临时
	if (FStaticLightingManager::Get()->IsValidLightMap())
	{
		TSharedPtr<SWidget> *TempWidget = SWidgetMap.Find(FArmyWorldManager::Get().bShowAdvancedWorldSettings ? EPropertyDetailType::PDT_WorldSettingsAdvanced : EPropertyDetailType::PDT_WorldSettings);
		if (TempWidget)
		{
			SetPropertyPanelVisibility(EVisibility::Visible);
			PropertyPanel->SetActiveWidget(TempWidget->Get()->AsShared());
			InitializeWorldSettings();
			return;
		}
	}
	SetPropertyPanelVisibility(EVisibility::Collapsed);
}

void FArmyHardModeDetail::OnDirectionalLightIntensityChanged(float InValue, ETextCommit::Type CommitType)
{
	CachedDirectionalLightIntensity = InValue;
	if (FArmyWorldManager::Get().StaticSunLight)
		FArmyWorldManager::Get().StaticSunLight->GetLightComponent()->SetIntensity(InValue);
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateDirectionalLightIntensity()
{
	return
		SNew(STextBlockImageSpinbox<float>)
		.Value(this, &FArmyHardModeDetail::GetDirectionalLightIntensity)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnDirectionalLightIntensityChanged)
		.MinValue(0.0)
		.MaxValue(20.0)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(0.1)//设置微调数值
		;
}

FReply FArmyHardModeDetail::OnDirectionalLightColorClicked()
{
	FColorPickerArgs PickerArgs;
	PickerArgs.InitialColorOverride = CacheDirectionalLightColor;
	PickerArgs.InitialColorOverride.A = 1.f;
	PickerArgs.bUseAlpha = false;
	PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &FArmyHardModeDetail::OnDirectionalLightColorChanged);
	OpenColorPicker(PickerArgs);
	return FReply::Handled();
}

void FArmyHardModeDetail::OnDirectionalLightColorChanged(const FLinearColor InColor)
{
	CacheDirectionalLightColor = InColor;
	CacheDirectionalLightColor.A = 1.f;
	if (FArmyWorldManager::Get().StaticSunLight)
		FArmyWorldManager::Get().StaticSunLight->GetLightComponent()->SetLightColor(InColor);
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateDirectionalLightColor()
{
	return
		SNew(SButton)
		.OnClicked(this, &FArmyHardModeDetail::OnDirectionalLightColorClicked)
		.ContentPadding(FMargin(0))
		.Content()
		[
			SNew(SBorder)
			.HAlign(HAlign_Fill)
			.BorderBackgroundColor(this, &FArmyHardModeDetail::GetDirectionalLightBorderColor)
			.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
		];
}

void FArmyHardModeDetail::OnDirectionalLightIndirectIntensityChanged(float InValue, ETextCommit::Type CommitType)
{
	CachedDirectionalLightIndirectIntensity = InValue;
	if (FArmyWorldManager::Get().StaticSunLight)
		FArmyWorldManager::Get().StaticSunLight->GetLightComponent()->SetIndirectLightingIntensity(InValue);
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateDirectionalLightIndirectIntensity()
{
	return
		SNew(STextBlockImageSpinbox<float>)
		.Value(this, &FArmyHardModeDetail::GetDirectionalLightIndirectIntensity)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnDirectionalLightIndirectIntensityChanged)
		.MinValue(0.0)
		.MaxValue(20)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(0.1)//设置微调数值
		;
}

void FArmyHardModeDetail::OnDirectionalLightStationaryChanged(ECheckBoxState InNewState)
{
	CachedDirectionalLightStationary = InNewState;
	if (FArmyWorldManager::Get().StaticSunLight)
		FArmyWorldManager::Get().StaticSunLight->GetLightComponent()->SetMobility( InNewState == ECheckBoxState::Checked ? EComponentMobility::Stationary : EComponentMobility::Static);
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateDirectionalLightStationary()
{
	return
		SNew(SCheckBox)
		.Style(FArmyStyle::Get(), "SingleCheckBox")
		.IsChecked(this, &FArmyHardModeDetail::GetDirectionalLightStationary)
		.OnCheckStateChanged(this, &FArmyHardModeDetail::OnDirectionalLightStationaryChanged);
}

void FArmyHardModeDetail::OnDirectionalLightSourceChanged(float InValue, ETextCommit::Type CommitType)
{
	if (FArmyWorldManager::Get().StaticSunLight)
	{
		CachedDirectionalLightSource = InValue;
		UDirectionalLightComponent* DC = Cast<UDirectionalLightComponent>(FArmyWorldManager::Get().StaticSunLight->GetLightComponent());
		if (DC)
		{
			DC->LightSourceAngle = InValue;
		}
	}
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateDirectionalLightSource()
{
	return
		SNew(STextBlockImageSpinbox<float>)
		.Value(this, &FArmyHardModeDetail::GetDirectionalLightSource)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnDirectionalLightSourceChanged)
		.MinValue(0.0)
		.MaxValue(5)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(0.1)//设置微调数值
		;
}

void FArmyHardModeDetail::OnDirectionalLightShadowExponentChanged(float InValue, ETextCommit::Type CommitType)
{
	if (FArmyWorldManager::Get().StaticSunLight)
	{
		CachedDirectionalLightShadowExponent = InValue;
		UDirectionalLightComponent* DC = Cast<UDirectionalLightComponent>(FArmyWorldManager::Get().StaticSunLight->GetLightComponent());
		if (DC)
		{
			DC->LightmassSettings.ShadowExponent = InValue;
		}
	}
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateDirectionalLightShadowExponent()
{
	return
		SNew(STextBlockImageSpinbox<float>)
		.Value(this, &FArmyHardModeDetail::GetDirectionalLightShadowExponent)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnDirectionalLightShadowExponentChanged)
		.MinValue(0.0)
		.MaxValue(4.0)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(0.1)//设置微调数值
		;
}

void FArmyHardModeDetail::OnDirectionalLightPitchChanged(int32 InValue, ETextCommit::Type CommitType)
{
	if (FArmyWorldManager::Get().StaticSunLight)
	{
		CachedDirectionalLightPitch = InValue;
		FRotator Rot = FArmyWorldManager::Get().StaticSunLight->GetActorRotation();
		FArmyWorldManager::Get().DynamicSunLight->SetActorRotation(FRotator(InValue, Rot.Yaw, Rot.Roll).Quaternion());
		FArmyWorldManager::Get().StaticSunLight->SetActorRotation(FRotator(InValue, Rot.Yaw, Rot.Roll).Quaternion());
		FArmyWorldManager::Get().StaticSunLight->InvalidateLightingCache();
	}
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateDirectionalLightPitch()
{
	return
		SNew(STextBlockImageSpinbox<int32>)
		.Value(this, &FArmyHardModeDetail::GetDirectionalLightPitch)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnDirectionalLightPitchChanged)
		.MinValue(-180)
		.MaxValue(180)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(1)//设置微调数值
		;
}

void FArmyHardModeDetail::OnDirectionalLightYawChanged(int32 InValue, ETextCommit::Type CommitType)
{
	if (FArmyWorldManager::Get().StaticSunLight)
	{
		CachedDirectionalLightYaw = InValue;
		FRotator Rot = FArmyWorldManager::Get().StaticSunLight->GetActorRotation();
		FArmyWorldManager::Get().DynamicSunLight->SetActorRotation(FRotator(Rot.Pitch, InValue, Rot.Roll).Quaternion());
		FArmyWorldManager::Get().StaticSunLight->SetActorRotation(FRotator(Rot.Pitch, InValue, Rot.Roll).Quaternion());
		FArmyWorldManager::Get().StaticSunLight->InvalidateLightingCache();
	}
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateDirectionalLightYaw()
{
	return
		SNew(STextBlockImageSpinbox<int32>)
		.Value(this, &FArmyHardModeDetail::GetDirectionalLightYaw)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnDirectionalLightYawChanged)
		.MinValue(-180)
		.MaxValue(180)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(1)//设置微调数值
		;
}

void FArmyHardModeDetail::OnSkyLightIntensityChanged(float InValue, ETextCommit::Type CommitType)
{
	CachedSkyLightIntensity = InValue;
	if (FArmyWorldManager::Get().StaticSkyLight)
		FArmyWorldManager::Get().StaticSkyLight->GetLightComponent()->SetIntensity(InValue);
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateSkyLightIntensity()
{
	return
		SNew(STextBlockImageSpinbox<float>)
		.Value(this, &FArmyHardModeDetail::GetSkyLightIntensity)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnSkyLightIntensityChanged)
		.MinValue(0.0)
		.MaxValue(20.0)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(0.01)//设置微调数值
		;
}

void FArmyHardModeDetail::OnSkyLightIndirectIntensityChanged(float InValue, ETextCommit::Type CommitType)
{
	CachedSkyLightIndirectIntensity = InValue;
	if (FArmyWorldManager::Get().StaticSkyLight)
		FArmyWorldManager::Get().StaticSkyLight->GetLightComponent()->SetIndirectLightingIntensity(InValue);
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateSkyLightIndirectIntensity()
{
	return
		SNew(STextBlockImageSpinbox<float>)
		.Value(this, &FArmyHardModeDetail::GetSkyLightIndirectIntensity)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnSkyLightIndirectIntensityChanged)
		.MinValue(0.0)
		.MaxValue(20)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(0.01)//设置微调数值
		;
}

void FArmyHardModeDetail::OnAutoExposureBiasChanged(float InValue, ETextCommit::Type CommitType)
{
	if (FArmyWorldManager::Get().PPV_StaticLighting)
	{
		CachedAutoExposureBias = InValue;
		FArmyWorldManager::Get().PPV_StaticLighting->Settings.AutoExposureBias = InValue;
	}
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateAutoExposureBias()
{
	return
		SNew(STextBlockImageSpinbox<float>)
		.Value(this, &FArmyHardModeDetail::GetAutoExposureBias)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnAutoExposureBiasChanged)
		.MinValue(-8.0)
		.MaxValue(8.0)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(0.1)//设置微调数值
		;
}

void FArmyHardModeDetail::OnPPIndirectLightingIntensityChanged(float InValue, ETextCommit::Type CommitType)
{
	if (FArmyWorldManager::Get().PPV_StaticLighting)
	{
		CachedPPIndirectLightingIntensity = InValue;
		FArmyWorldManager::Get().PPV_StaticLighting->Settings.IndirectLightingIntensity = InValue;
	}
}

TSharedRef<SWidget> FArmyHardModeDetail::CreatePPIndirectLightingIntensity()
{
	return
		SNew(STextBlockImageSpinbox<float>)
		.Value(this, &FArmyHardModeDetail::GetPPIndirectLightingIntensity)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnPPIndirectLightingIntensityChanged)
		.MinValue(0.0)
		.MaxValue(4.0)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(0.1)//设置微调数值
		;
}

void FArmyHardModeDetail::OnPPBloomIntensityChanged(float InValue, ETextCommit::Type CommitType)
{
	if (FArmyWorldManager::Get().PPV_StaticLighting)
	{
		CachedPPBloomIntensity = InValue;
		FArmyWorldManager::Get().PPV_StaticLighting->Settings.BloomIntensity = InValue;
	}
}

TSharedRef<SWidget> FArmyHardModeDetail::CreatePPBloomIntensity()
{
	return
		SNew(STextBlockImageSpinbox<float>)
		.Value(this, &FArmyHardModeDetail::GetPPBloomIntensity)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnPPBloomIntensityChanged)
		.MinValue(0.0)
		.MaxValue(8.0)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(0.01)//设置微调数值
		;
}

void FArmyHardModeDetail::OnPPBloomThresholdChanged(float InValue, ETextCommit::Type CommitType)
{
	if (FArmyWorldManager::Get().PPV_StaticLighting)
	{
		CachedPPBloomThreshold = InValue;
		FArmyWorldManager::Get().PPV_StaticLighting->Settings.BloomThreshold = InValue;
	}
}

TSharedRef<SWidget> FArmyHardModeDetail::CreatePPBloomThreshold()
{
	return
		SNew(STextBlockImageSpinbox<float>)
		.Value(this, &FArmyHardModeDetail::GetPPBloomThreshold)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnPPBloomThresholdChanged)
		.MinValue(-1.0)
		.MaxValue(8.0)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(0.01)//设置微调数值
		;
}

void FArmyHardModeDetail::OnStaticLightingLevelScaleChanged(float InValue, ETextCommit::Type CommitType)
{
	CachedStaticLightingLevelScale = InValue;
	GWorld->GetWorldSettings()->LightmassSettings.StaticLightingLevelScale = InValue;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateStaticLightingLevelScale()
{
	return
		SNew(STextBlockImageSpinbox<float>)
		.Value(this, &FArmyHardModeDetail::GetStaticLightingLevelScale)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnStaticLightingLevelScaleChanged)
		.MinValue(0.0)
		.MaxValue(4.0)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(0.1)//设置微调数值
		;
}

void FArmyHardModeDetail::OnNumIndirectLightingBouncesChanged(int32 InValue, ETextCommit::Type CommitType)
{
	CachedNumIndirectLightingBounces = InValue;
	GWorld->GetWorldSettings()->LightmassSettings.NumIndirectLightingBounces = InValue;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateNumIndirectLightingBounces()
{
	return
		SNew(STextBlockImageSpinbox<int32>)
		.Value(this, &FArmyHardModeDetail::GetNumIndirectLightingBounces)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnNumIndirectLightingBouncesChanged)
		.MinValue(1)
		.MaxValue(20)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(1)//设置微调数值
		;
}

void FArmyHardModeDetail::OnNumSkyLightingBouncesChanged(int32 InValue, ETextCommit::Type CommitType)
{
	CachedNumSkyLightingBounces = InValue;
	GWorld->GetWorldSettings()->LightmassSettings.NumSkyLightingBounces = InValue;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateNumSkyLightingBounces()
{
	return
		SNew(STextBlockImageSpinbox<int32>)
		.Value(this, &FArmyHardModeDetail::GetNumSkyLightingBounces)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnNumSkyLightingBouncesChanged)
		.MinValue(1)
		.MaxValue(20)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(1)//设置微调数值
		;
}

void FArmyHardModeDetail::OnIndirectLightingQualityChanged(float InValue, ETextCommit::Type CommitType)
{
	CachedIndirectLightingQuality = InValue;
	GWorld->GetWorldSettings()->LightmassSettings.IndirectLightingQuality = InValue;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateIndirectLightingQuality()
{
	return
		SNew(STextBlockImageSpinbox<float>)
		.Value(this, &FArmyHardModeDetail::GetIndirectLightingQuality)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnIndirectLightingQualityChanged)
		.MinValue(0.0)
		.MaxValue(4.0)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(0.01)//设置微调数值
		;
}

void FArmyHardModeDetail::OnIndirectLightingSmoothnessChanged(float InValue, ETextCommit::Type CommitType)
{
	CachedIndirectLightingSmoothness = InValue;
	GWorld->GetWorldSettings()->LightmassSettings.IndirectLightingSmoothness = InValue;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateIndirectLightingSmoothness()
{
	return
		SNew(STextBlockImageSpinbox<float>)
		.Value(this, &FArmyHardModeDetail::GetIndirectLightingSmoothness)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnIndirectLightingSmoothnessChanged)
		.MinValue(0.0)
		.MaxValue(6.0)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(0.01)//设置微调数值
		;
}

void FArmyHardModeDetail::OnMaxOccllusionDistanceChanged(int32 InValue, ETextCommit::Type CommitType)
{
	CachedMaxOccllusionDistance = InValue;
	GWorld->GetWorldSettings()->LightmassSettings.MaxOcclusionDistance = InValue;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateMaxOccllusionDistance()
{
	return
		SNew(STextBlockImageSpinbox<int32>)
		.Value(this, &FArmyHardModeDetail::GetMaxOccllusionDistance)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnMaxOccllusionDistanceChanged)
		.MinValue(100)
		.MaxValue(500)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(1)//设置微调数值
		;
}

FReply FArmyHardModeDetail::OnEnvironmentColorClicked()
{
	FColorPickerArgs PickerArgs;
	PickerArgs.InitialColorOverride = CacheEnvironmentColor;
	PickerArgs.InitialColorOverride.A = 1.f;
	PickerArgs.bUseAlpha = false;
	PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &FArmyHardModeDetail::OnEnvironmentColorChanged);
	OpenColorPicker(PickerArgs);
	return FReply::Handled();
}

void FArmyHardModeDetail::OnEnvironmentColorChanged(const FLinearColor InColor)
{
	CacheEnvironmentColor = InColor;
	CacheEnvironmentColor.A = 1.f;
	GWorld->GetWorldSettings()->LightmassSettings.EnvironmentColor = InColor.ToFColor(true);
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateEnvironmentColor()
{
	return
		SNew(SButton)
		.OnClicked(this, &FArmyHardModeDetail::OnEnvironmentColorClicked)
		.ContentPadding(FMargin(0))
		.Content()
		[
			SNew(SBorder)
			.HAlign(HAlign_Fill)
			.BorderBackgroundColor(this, &FArmyHardModeDetail::GetEnvironmentBorderColor)
			.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
		];
}

void FArmyHardModeDetail::OnEnvironmentIntensityChanged(float InValue, ETextCommit::Type CommitType)
{
	CachedEnvironmentIntensity = InValue;
	GWorld->GetWorldSettings()->LightmassSettings.EnvironmentIntensity = InValue;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateEnvironmentIntensity()
{
	return
		SNew(STextBlockImageSpinbox<float>)
		.Value(this, &FArmyHardModeDetail::GetEnvironmentIntensity)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnEnvironmentIntensityChanged)
		.MinValue(0.0)
		.MaxValue(30.0)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(0.01)//设置微调数值
		;
}

void FArmyHardModeDetail::OnDiffuseBoostChanged(float InValue, ETextCommit::Type CommitType)
{
	CachedDiffuseBoost = InValue;
	GWorld->GetWorldSettings()->LightmassSettings.DiffuseBoost = InValue;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateDiffuseBoost()
{
	return
		SNew(STextBlockImageSpinbox<float>)
		.Value(this, &FArmyHardModeDetail::GetDiffuseBoost)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnDiffuseBoostChanged)
		.MinValue(0.0)
		.MaxValue(6.0)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(0.01)//设置微调数值
		;
}

void FArmyHardModeDetail::OnVolumetricLightmapDetailCellSizeChanged(int32 InValue, ETextCommit::Type CommitType)
{
	CachedVolumetricLightmapDetailCellSize = InValue;
	GWorld->GetWorldSettings()->LightmassSettings.VolumetricLightmapDetailCellSize = InValue;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateVolumetricLightmapDetailCellSize()
{
	return
		SNew(STextBlockImageSpinbox<int32>)
		.Value(this, &FArmyHardModeDetail::GetVolumetricLightmapDetailCellSize)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnVolumetricLightmapDetailCellSizeChanged)
		.MinValue(1)
		.MaxValue(1000)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(1)//设置微调数值
		;
}

void FArmyHardModeDetail::OnVolumetricLightmapMaximumBirckMemoryMbChanged(int32 InValue, ETextCommit::Type CommitType)
{
	CachedVolumetricLightmapMaximumBirckMemoryMb = InValue;
	GWorld->GetWorldSettings()->LightmassSettings.VolumetricLightmapMaximumBrickMemoryMb = InValue;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateVolumetricLightmapMaximumBirckMemoryMb()
{
	return
		SNew(STextBlockImageSpinbox<int32>)
		.Value(this, &FArmyHardModeDetail::GetVolumetricLightmapMaximumBirckMemoryMb)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnVolumetricLightmapMaximumBirckMemoryMbChanged)
		.MinValue(1)
		.MaxValue(500)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(1)//设置微调数值
		;
}

void FArmyHardModeDetail::OnCeilingLampIntensityChanged(float InValue, ETextCommit::Type CommitType)
{
	CachedCeilingLampIntensity = InValue;
	for (TActorIterator<APointLight> It(GWorld); It; ++It)
	{
		UPointLightComponent* LM = (*It)->PointLightComponent;
		if (LM->GetName() == "LampSlotPointLightComponent")
		{
			LM->SetIntensity(InValue);
		}
	}
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateCeilingLampIntensity()
{
	return
		SNew(STextBlockImageSpinbox<float>)
		.Value(this, &FArmyHardModeDetail::GetCeilingLampIntensity)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnCeilingLampIntensityChanged)
		.MinValue(0.0)
		.MaxValue(5000.0)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(1)//设置微调数值
		;
}

void FArmyHardModeDetail::OnLightmapResScaleChanged(float InValue, ETextCommit::Type CommitType)
{
	CachedLightmapResScale = InValue;
	FArmyWorldManager::Get().SetGlobalStaticLightingResScale(InValue);
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateLightmapResScale()
{
	return
		SNew(STextBlockImageSpinbox<float>)
		.Value(this, &FArmyHardModeDetail::GetLightmapResScale)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnLightmapResScaleChanged)
		.MinValue(0.1)
		.MaxValue(5.0)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(0.1)//设置微调数值
		;
}

void FArmyHardModeDetail::OnSkirtLineStaticLightingResChanged(int32 InValue, ETextCommit::Type CommitType)
{
	CachedSkirtLineStaticLightingRes = InValue;
	FArmyWorldManager::Get().SetSkirtLineStaticLightingRes(InValue);
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateSkirtLineStaticLightingRes()
{
	return
		SNew(STextBlockImageSpinbox<int32>)
		.Value(this, &FArmyHardModeDetail::GetSkirtLineStaticLightingRes)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnSkirtLineStaticLightingResChanged)
		.MinValue(2)
		.MaxValue(1024)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(1)//设置微调数值
		;
}

void FArmyHardModeDetail::OnMoldingLineStaticLightingResChanged(int32 InValue, ETextCommit::Type CommitType)
{
	CachedMoldingLineStaticLightingRes = InValue;
	FArmyWorldManager::Get().SetMoldingLineStaticLightingRes(InValue);
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateMoldingLineStaticLightingRes()
{
	return
		SNew(STextBlockImageSpinbox<int32>)
		.Value(this, &FArmyHardModeDetail::GetMoldingLineStaticLightingRes)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnMoldingLineStaticLightingResChanged)
		.MinValue(2)
		.MaxValue(1024)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(1)//设置微调数值
		;
}

void FArmyHardModeDetail::OnWindowStaticLightingResChanged(int32 InValue, ETextCommit::Type CommitType)
{
	CachedWindowStaticLightingRes = InValue;
	FArmyWorldManager::Get().SetWindowStaticLightingRes(InValue);
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateWindowStaticLightingRes()
{
	return
		SNew(STextBlockImageSpinbox<int32>)
		.Value(this, &FArmyHardModeDetail::GetWindowStaticLightingRes)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnWindowStaticLightingResChanged)
		.MinValue(2)
		.MaxValue(1024)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(1)//设置微调数值
		;
}

void FArmyHardModeDetail::OnBuildStaticMeshChanged(ECheckBoxState InNewState)
{
	CachedBuildStaticMesh = InNewState;
	FArmyWorldManager::Get().bBuildStaticMesh = CachedBuildStaticMesh == ECheckBoxState::Checked;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateBuildStaticMesh()
{
	return
		SNew(SCheckBox)
		.Style(FArmyStyle::Get(), "SingleCheckBox")
		.IsChecked(this, &FArmyHardModeDetail::GetBuildStaticMesh)
		.OnCheckStateChanged(this, &FArmyHardModeDetail::OnBuildStaticMeshChanged);
}

void FArmyHardModeDetail::OnShowLightmapUVChanged(ECheckBoxState InNewState)
{
	CachedShowLightmapUV = InNewState;
	FArmyWorldManager::Get().bShowLightmapUV = CachedShowLightmapUV == ECheckBoxState::Checked;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateShowLightmapUV()
{
	return
		SNew(SCheckBox)
		.Style(FArmyStyle::Get(), "SingleCheckBox")
		.IsChecked(this, &FArmyHardModeDetail::GetShowLightmapUV)
		.OnCheckStateChanged(this, &FArmyHardModeDetail::OnShowLightmapUVChanged);
}

void FArmyHardModeDetail::OnUVChannelChanged(int32 InValue, ETextCommit::Type CommitType)
{
	CachedUVChannel = InValue;
	FArmyWorldManager::Get().UVChannelToShow = CachedUVChannel;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateUVChannel()
{
	return
		SNew(STextBlockImageSpinbox<int32>)
		.Value(this, &FArmyHardModeDetail::GetUVChannel)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnUVChannelChanged)
		.MinValue(0)
		.MaxValue(1)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(1)//设置微调数值
		;
}

void FArmyHardModeDetail::OnFOVChanged(int32 InValue, ETextCommit::Type CommitType)
{
	CachedFOV = InValue;
	FArmyWorldManager::Get().SetCameraFOV(InValue);
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateFOV()
{
	return
		SNew(STextBlockImageSpinbox<int32>)
		.Value(this, &FArmyHardModeDetail::GetFOV)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnFOVChanged)
		.MinValue(20)
		.MaxValue(180)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(1)//设置微调数值
		;
}

void FArmyHardModeDetail::OnOutdoorPanoramaIndexChanged(int32 InValue, ETextCommit::Type CommitType)
{
	CachedOutdoorPanoramaIndex = InValue;
	FArmyWorldManager::Get().SetOutdoorPanoramaIndex(InValue);
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateOutdoorPanoramaIndex()
{
	return
		SNew(STextBlockImageSpinbox<int32>)
		.Value(this, &FArmyHardModeDetail::GetOutdoorPanoramaIndex)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnOutdoorPanoramaIndexChanged)
		.MinValue(0)
		.MaxValue(9)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(1)//设置微调数值
		;
}

void FArmyHardModeDetail::OnOutdoorPanoramaYawChanged(int32 InValue, ETextCommit::Type CommitType)
{
	CachedOutdoorPanoramaYaw = InValue;
	FArmyWorldManager::Get().SetOutdoorPanoramaYaw(InValue);
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateOutdoorPanoramaYaw()
{
	return
		SNew(STextBlockImageSpinbox<int32>)
		.Value(this, &FArmyHardModeDetail::GetOutdoorPanoramaYaw)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnOutdoorPanoramaYawChanged)
		.MinValue(0)
		.MaxValue(360)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(1)//设置微调数值
		;
}



