#pragma once
#include "ArmyViewportClient.h"

DECLARE_DELEGATE_OneParam(PanoramaDelegate, const TArray<uint32>&)

class XRPANORAMA_API FArmyPanoramaController : public FTickableGameObject
{
public:
	FArmyPanoramaController();
	~FArmyPanoramaController();

	/** ������ȡ */
	//static FArmyPanoramaController& Get();
	PanoramaDelegate GetPanoramaDelegate;

	//��ʼ��ͼ
	void StartPanorama(UArmyViewportClient* MyGVC);
	void ProcessScreenShots(FViewport* InViewport);

	class UArmyEditorViewportClient* ViewportClient;

	//ʵ��FTickableGameObject�Ľӿ�
	virtual bool IsTickable() const override;
    virtual bool IsTickableWhenPaused() const override;
	virtual TStatId GetStatId() const override;
	virtual void Tick(float DeltaTime) override;

private:
	//�Ƿ�Ҫ��ͼ
	bool bScreenshot;
	//��ǰ�Ƿ��ڽ�ͼ
	bool bProcessingScreenshot;
	//��ǰ��Ⱦ��ȫ��ͼ�ĵڼ���0-5
	int32 PanoramaMapIndex;
	//��ǰλ��
	FVector CaptureLocation;
	//��ǰ��ת
	FRotator CaptureRotation;
	//������һ����Ⱦ�����֡��
	int32 LastRenderingFrameCount;
	//����6��ͼ
	TArray<uint32> VerticalSixFacesData;
	//�������ڵ�����
	UWorld* CurWorld;

	//��¼����ȫ��ͼ֮ǰ������
	float OldExposureMin;
	float OldExposureMax;
	float OldExposureSpeedUp;
	float OldExposureSpeedDown;
	class APostProcessVolume* PPV;

	//static TSharedPtr<FArmyPanoramaController> MyPanoramaController;

	//���ع�仯�ٶ�����Ϊ���
	void SetFastAutoExposure();
	//�ָ�ԭ������ֵ
	void ResetAutoExposure();
};