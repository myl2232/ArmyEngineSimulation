/**
* Copyright 2019 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRWorldManager.h
* @Description 世界管理器
*
* @Author 马云龙
* @Date 2019年4月18日
* @Version 1.0
*/

#pragma once
#include "CoreMinimal.h"

class XR_API FArmyWorldManager
{
public:
	FArmyWorldManager();
	~FArmyWorldManager() {}

	static FArmyWorldManager& Get()
	{
		if (!Singleton.IsValid())
		{
			Singleton = MakeShareable(new FArmyWorldManager());
		}
		return *Singleton;
	}

	void Init(UWorld* InWorld);

public:
	//场景中的重要Actor捕捉记录
	class APostProcessVolume* PPV_StaticLighting;
	class APostProcessVolume* PPV_DynamicLighting;
	class ADirectionalLight* StaticSunLight;
	class ADirectionalLight* DynamicSunLight;
	class ASkyLight* DynamicSkyLight;
	class ASkyLight* StaticSkyLight;
	class AStaticMeshActor* SM_OutdoorPanorama;
	class UMaterialInstanceDynamic* MID_OutdoorPanorama;

	//设置外景是否显示
	void SetOutdoorMeshVisibile(bool bInVisible);

	//尝试切换模式。进入水电模式，如果此时已渲染，则自动切换到动态环境；进入立面（木作）模式如果已渲染，切换到静态模式；
	void TryChangeEnvironmentModeConditionally(FString InCurrentMode);

	void SetEnvironmentMode(bool bInDynamic);

	float GetGlobalStaticLightingResScale() { return GlobalStaticLightingResScale; }

	int32 GetSkirtLineStaticLightingRes() { return SkirtLineStaticLightingRes; }

	int32 GetMoldingLineStaticLightingRes() { return MoldingLineStaticLightingRes; }

	int32 GetWindowStaticLightingRes() { return WindowStaticLightingRes; }

	int32 GetCameraFOV();

	int32 GetOutdoorPanoramaIndex() { return OutdoorPanoramaIndex; }

	int32 GetOutdoorPanoramaYaw() { return OutdoorPanoramaYaw; }

	void SetGlobalStaticLightingResScale(float InValue);

	void SetSkirtLineStaticLightingRes(int32 InValue);

	void SetMoldingLineStaticLightingRes(int32 InValue);

	void SetWindowStaticLightingRes(int32 InValue);

	void SetCameraFOV(int32 InValue);

	void SetOutdoorPanoramaIndex(int32 InIndex);

	void SetOutdoorPanoramaYaw(int32 InYaw);

	bool bShowLightmapUV = false;

	bool bBuildStaticMesh = false;

	int32 UVChannelToShow = 1;

	/** @马云龙 是否显示高级世界设置 */
	//@马云龙 临时
	bool bShowAdvancedWorldSettings = false;

	void DirectionalLight_Intensity(float InValue);
	void DirectionalLight_Color(int32 R, int32 G, int32 B);
	void DirectionalLight_IndirectIntensity(float InValue);
	void DirectionalLight_SourceAngle(float InValue);
	void DirectionalLight_ShadowExponent(float InValue);
	void DirectionalLight_Rotation(int32 InPitch, int32 InYaw, int32 InRoll);
	void Postprocess_AutoExposure(float InMin, float InMax, float InUp, float InDown);
	void Postprocess_AutoExposureBias(float InValue);
	void Postprocess_IndirectLightingIntensity(float InValue);
	void WorldSetting_StaticLightingLevelScale(float InValue);
	void WorldSetting_NumIndirectLightingBounces(float InValue);
	void WorldSetting_NumSkyLightingBounces(float InValue);
	void WorldSetting_IndirectLightingQuality(float InValue);
	void WorldSetting_IndirectLightingSmoothness(float InValue);
	void WorldSetting_EnvironmentColor(int32 R, int32 G, int32 B);
	void WorldSetting_EnvironmentIntensity(float InValue);
	void WorldSetting_DiffuseBoost(float InValue);
	void WorldSetting_VolumetricLightmapDetailCellSize(float InValue);
	void WorldSetting_VolumetricLightmapMaximumBirckMemoryMb(float InValue);
	void WorldSetting_AO(float InDirectillOFraction, float InIndirectillOFraction, float InOExponent, float InFullyOSimplesFraction, float InMaxODistance);
	void WorldSetting_SSR(float InIntensity, float InQuality, float InMaxRoughness);
	void CeilingLampIntensity(float InValue);

private:
	/** @马云龙 全局光照图分辨率缩放 */
	float GlobalStaticLightingResScale = 1.f;

	/** @马云龙 顶/底角线光照图分辨率 */
	int32 SkirtLineStaticLightingRes = 0;

	/** @马云龙 造型线光照图分辨率 */
	int32 MoldingLineStaticLightingRes = 0;

	/** @马云龙 窗户光照图分辨率 */
	int32 WindowStaticLightingRes = 0;

	/** @马云龙 摄像机广角 */
	int32 CameraFOV = 90;

	/** @马云龙 外景图片 */
	int32 OutdoorPanoramaIndex = 0;

	/** @马云龙 外景图片旋转Yaw */
	int32 OutdoorPanoramaYaw = 0;

	UWorld* MyWorld;

private:
	static TSharedPtr<FArmyWorldManager> Singleton;
};