#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
//#include "SteamVRFunctionLibrary.h"
#include "ArmyPawn.h"
#include "ArmyTypes.h"
#include "ArmyPlayerController.generated.h"

UCLASS()
class XR_API AXRPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AXRPlayerController();
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	virtual void UpdateRotation(float DeltaTime) override;
	void PostInit();
	/** 同步HTC手柄的位置行为 */
	void TickViveController();

	AXRPawn* GetMyPawn();
	TSharedPtr<class FArmyHardModeController> GetHardPC();
	TSharedPtr<class FArmyHardModeController> HardPC;
public:
	/** 是否处于VR Mode*/
	bool IsVRMode();
	/** 开启或者关闭VR Mode*/
	bool EnableVRMode(bool _bEnable);
	/** 在全屏/VR模式下 按下回车键 变为正常模式 */
	void OnExitFullScreenMode();
	/** 设置画面质量*/
	void SetQualityLevels(int32 _Level);
	/** @欧石楠获取当前画面质量*/
	int32 GetQualityLevels();
	bool IsHightQualityLevel();
	/** 开/关全局方向光*/
	void ShowDirectionalLight(bool _bShow);

	/** 设置视图模式--顶视图--透视图--环绕图*/
	void SetXRViewMode(EXRViewMode _Mode);
	EXRViewMode GetXRViewMode() { return CurrentViewMode; }

	UFUNCTION(Exec)
		void EXE_Stereo(bool bEnable);
	UFUNCTION(Exec)
		void EXE_SetVRTestModeEnabled(bool bEnable);
	UFUNCTION(Exec)
		void EXE_SetCustomCursor(int32 _Mode);
	UFUNCTION(Exec)
		void EXE_TestJson();
	/** @欧石楠 线框模式*/
	UFUNCTION(Exec)
		void EXE_ViewMode(int32 InMode);

	/** @马云龙 Lightmass功能 */
	UFUNCTION(Exec)
		void EXE_BuildStaticLighting(int32 InQuality = 0);
	UFUNCTION(Exec)
		void EXE_ClearStaticLighting();
	UFUNCTION(Exec)
		void EXE_SaveStaticLighting();
	UFUNCTION(Exec)
		void EXE_LoadStaticLighting();
	UFUNCTION(Exec)
		void EXE_LoadStaticLightingEx();
	UFUNCTION(Exec)
		void EXE_LoadPackageTest();
	UFUNCTION(Exec)
		void EXE_UnloadLightMapDataPackage();
	UFUNCTION(Exec)
		void EXE_RenderSomethingToRGB();
	UFUNCTION(Exec)
		void EXE_CreateFileTest();
	UFUNCTION(Exec)
		void EXE_CloseFileTest();
	UFUNCTION(Exec)
		void EXE_ComputeSelectedMeshUV2InEditor(bool bAutoWrap);
	UFUNCTION(Exec)
		void EXE_ComputeLevelUV2InEditor();
	UFUNCTION(Exec)
		void EXE_DeleteSelectedActor();
	UFUNCTION(Exec)
		void EXE_MoveSelectedActor(float InX, float InY, float InZ);
	UFUNCTION(Exec)
		void EXE_SetDrawUVChannel(int32 InDrawUVChannel);
	UFUNCTION(Exec)
		void EXE_SetDrawUVComponentIndex(int32 InDrawUVComponentIndex);
	UFUNCTION(Exec)
		void EXE_AddDrawDebugPoint(float InX, float InY, float InZ);
	UFUNCTION(Exec)
		void EXE_SetLightMobility(int32 InLightType, int32 InMobilityType);
	UFUNCTION(Exec)
		void EXE_SetLightVisibility(int32 InType, bool InNewVisibility);
	UFUNCTION(Exec)
		void EXE_SetSelectedLightRadius(float InNewRadius);
	UFUNCTION(Exec)
		void EXE_SetSelectedLightIntensity(float InIntensity);
	UFUNCTION(Exec)
		void EXE_SetLightAffectWorld(int32 InType, bool InNewAffectWorld);
	UFUNCTION(Exec)
		void EXE_GetAllComponents();
	UFUNCTION(Exec)
		void EXE_DeleteActor(int32 InType);
	UFUNCTION(Exec)
		void EXE_SetCubeMobility(int32 InType);
	UFUNCTION(Exec)
		void EXE_GenerateTestMesh();
	UFUNCTION(Exec)
		void EXE_GenerateTestMeshWithTools();
	UFUNCTION(Exec)
		void EXE_GenerateTestMeshWithTools2();

	UFUNCTION(Exec)
		void EXE_TestCreateProcMesh();
	UFUNCTION(Exec)
		void EXE_TestCreateOrUpdateProcMesh(int32 InSectionIndex);
	UFUNCTION(Exec)
		void EXE_TestTransform();
	
	UFUNCTION(Exec)
		void EXE_InvalidateSelectedLighting();
	UFUNCTION(Exec)
		void EXE_SetSelectedActorComponentVisibility(int32 ChildComponentIndex, bool bVisible);
	UFUNCTION(Exec)
		void EXE_SetSelectedLightMobility(bool bMoveable);
	UFUNCTION(Exec)
		void EXE_SetEnvironmentMode(bool bDynamic);
	UFUNCTION(Exec)
		void EXE_SetNotificationPercent(int32 InPercent);
	UFUNCTION(Exec)
		void EXE_CollectActors();
	UFUNCTION(Exec)
		void EXE_SetCubesDynamic();
	UFUNCTION(Exec)
		void EXE_SetMaterialToSelected();
	UFUNCTION(Exec)
		void EXE_HoldMap();
	UFUNCTION(Exec)
		void EXE_CreateDoorBlock();
	UFUNCTION(Exec)
		void EXE_UpdateSelectedRS();
	UFUNCTION(Exec)
		void EXE_SetRSMovable();
	UFUNCTION(Exec)
		void EXE_SetRSStatic();
	UFUNCTION(Exec)
		void EXE_UpdateReflectionCaptures();
	UFUNCTION(Exec)
		void EXE_RecreateMeshState();
	UFUNCTION(Exec)
		void EXE_GenerateDoorBlock();
	UFUNCTION(Exec)
		void EXE_InvalidateSelected();
	UFUNCTION(Exec)
		void EXE_ShowAdvancedWorldSettings(bool bVisible = true);
	UFUNCTION(Exec)
		void EXE_SetAllPointLightIntensity(float InValue);
	UFUNCTION(Exec)
		void EXE_SetAllSpotLightIntensity(float InValue);
	UFUNCTION(Exec)
		void EXE_SetQualityLevel(int32 InLevel);
	UFUNCTION(Exec)
		void EXE_SetAllStaticMeshMobility(bool bDynamic);
	//=====================================
	//太阳光的亮度 建议值0~10
	UFUNCTION(Exec)
		void EXE_DirectionalLight_Intensity(float InValue);
	//太阳光的颜色
	UFUNCTION(Exec)
		void EXE_DirectionalLight_Color(int32 R, int32 G, int32 B);
	//太阳光的间接光亮度
	UFUNCTION(Exec)
		void EXE_DirectionalLight_IndirectIntensity(float InValue);
	//光源角度
	UFUNCTION(Exec)
		void EXE_DirectionalLight_SourceAngle(float InValue);
	//阴影指数
	UFUNCTION(Exec)
		void EXE_DirectionalLight_ShadowExponent(float InValue);
	//太阳光的角度
	UFUNCTION(Exec)
		void EXE_DirectionalLight_Rotation(int32 InPitch, int32 InYaw, int32 InRoll);

	//自动曝光行为参数
	UFUNCTION(Exec)
		void EXE_Postprocess_AutoExposure(float InMin, float InMax, float InUp, float InDown);
	//自动曝光整体亮度偏移 建议值 -2~2
	UFUNCTION(Exec)
		void EXE_Postprocess_AutoExposureBias(float InValue);
	//后期处理 间接光亮度
	UFUNCTION(Exec)
		void EXE_Postprocess_IndirectLightingIntensity(float InValue);

	//烘焙之前的设置，建议值0.5 ~ 1.5 值越小，烘焙效果越细腻，耗时越长
	UFUNCTION(Exec)
		void EXE_WorldSetting_StaticLightingLevelScale(float InValue);
	//烘焙之前的设置，间接光反弹次数，建议值 2~10 次数越多，耗时越长，效果越好
	UFUNCTION(Exec)
		void EXE_WorldSetting_NumIndirectLightingBounces(float InValue);
	//烘焙之前的设置，天光反弹次数
	UFUNCTION(Exec)
		void EXE_WorldSetting_NumSkyLightingBounces(float InValue);
	//烘焙之前的设置，间接光质量，建议值 1~4
	UFUNCTION(Exec)
		void EXE_WorldSetting_IndirectLightingQuality(float InValue);
	//间接光平滑，建议值 0.5~6
	UFUNCTION(Exec)
		void EXE_WorldSetting_IndirectLightingSmoothness(float InValue);
	//环境颜色，即阴影颜色，当前默认值为白色，所以阴影不够明显，但是可以靠天空环境照亮全屋
	UFUNCTION(Exec)
		void EXE_WorldSetting_EnvironmentColor(int32 R, int32 G, int32 B);
	//环境的亮度
	UFUNCTION(Exec)
		void EXE_WorldSetting_EnvironmentIntensity(float InValue);
	//模型材质的漫反射值，对于烘焙亮度效果的贡献值 建议值 0.1~6
	UFUNCTION(Exec)
		void EXE_WorldSetting_DiffuseBoost(float InValue);
	//烘焙完之后在空间中生成的光点密集度，用来照亮动态家具。单位cm，建议值10~1000，当前值为30，意为每个30cm生成一个光点
	UFUNCTION(Exec)
		void EXE_WorldSetting_VolumetricLightmapDetailCellSize(float InValue);
	UFUNCTION(Exec)
		void EXE_WorldSetting_VolumetricLightmapMaximumBirckMemoryMb(float InValue);

	UFUNCTION(Exec)
		void EXE_WorldSetting_AO(float InDirectillOFraction, float InIndirectillOFraction, float InOExponent, float InFullyOSimplesFraction, float InMaxODistance);
	UFUNCTION(Exec)
		void EXE_WorldSetting_SSR(float InIntensity, float InQuality, float InMaxRoughness);

	//所有灯带的亮度 当前值为400
	UFUNCTION(Exec)
		void EXE_CeilingLampIntensity(float InValue);
	//=====================================


	void MoveForward(float Val);

	/** @欧石楠 当视图切换时*/
	void OnViewChanged(int32 InNewValue,int32 InOldValue);

public:
	/**@欧石楠当切换视图时需要特殊操作的代理*/
	FMultiTwoInt32Delegate OnViewChangedDelegate;
	UPROPERTY()
		class UMapBuildDataRegistry* TmpMapData;
protected:
	void OnViveLeftTrigger();
	void OnViveRightTrigger();

private:
    /** 冻结场景中的默认Actor，不让其被选中 */
    void FreezeDefaultLevelActor();

private:
	bool bPostInit;
	AXRPawn* MyPawn;
	//VR模式相关变量
	bool bVRModeEnabled;
	bool bVRTestModeEnabled;
	bool bVRTeleportEnabled;
	FVector VRTeleportLoc;
	AActor* HTCInteractiveActorIgnored;
	AActor* HTCInteractiveActor;
	float HTCFocusedActorDuaration;

	TMap<float, class ADirectionalLight*> DirectionalLightList;
	TArray<class UDirectionalLightComponent*> DirectionalLightCOMs;

	//记录上一次第一人称视角时的旋转，用于从其他视角恢复第一人称视角时使用。
	FRotator LastFPSRot;

	EXRViewMode CurrentViewMode;

	//维护一个类型数组，决定哪些类型的obj可以被TOP_VIEW
	TArray<enum EObjectType> TypeArray;

	IAsyncReadFileHandle* TestHandle;
	UPackage* TestPackage;
	AActor* TempActor;
	TWeakPtr<SNotificationItem> LightBuildNotification;
};

extern XR_API AXRPlayerController* GXRPC;