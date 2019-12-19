#pragma once
#include "ArmyViewportClient.h"

DECLARE_DELEGATE_OneParam(PanoramaDelegate, const TArray<uint32>&)

class XRPANORAMA_API FArmyPanoramaController : public FTickableGameObject
{
public:
	FArmyPanoramaController();
	~FArmyPanoramaController();

	/** 单例获取 */
	//static FArmyPanoramaController& Get();
	PanoramaDelegate GetPanoramaDelegate;

	//开始截图
	void StartPanorama(UArmyViewportClient* MyGVC);
	void ProcessScreenShots(FViewport* InViewport);

	class UArmyEditorViewportClient* ViewportClient;

	//实现FTickableGameObject的接口
	virtual bool IsTickable() const override;
    virtual bool IsTickableWhenPaused() const override;
	virtual TStatId GetStatId() const override;
	virtual void Tick(float DeltaTime) override;

private:
	//是否要截图
	bool bScreenshot;
	//当前是否在截图
	bool bProcessingScreenshot;
	//当前渲染的全景图的第几张0-5
	int32 PanoramaMapIndex;
	//当前位置
	FVector CaptureLocation;
	//当前旋转
	FRotator CaptureRotation;
	//距离上一次渲染间隔的帧数
	int32 LastRenderingFrameCount;
	//纵向6张图
	TArray<uint32> VerticalSixFacesData;
	//生成所在的世界
	UWorld* CurWorld;

	//记录生成全景图之前的数据
	float OldExposureMin;
	float OldExposureMax;
	float OldExposureSpeedUp;
	float OldExposureSpeedDown;
	class APostProcessVolume* PPV;

	//static TSharedPtr<FArmyPanoramaController> MyPanoramaController;

	//把曝光变化速度设置为最快
	void SetFastAutoExposure();
	//恢复原来的数值
	void ResetAutoExposure();
};