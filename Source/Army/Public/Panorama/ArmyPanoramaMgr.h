// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Http.h"
#include "ArmyPanoramaTypes.h"
#include "ArmyViewportClient.h"
#include "SArmyPanorama.h"
#include "ArmyRoom.h"

static const int32 ResolutionWidth = 1536;
static const int32 CaptureInterval = 3;
//多张全景图同时上传的最多线程数
static const int32 UploadPanoramaMax = 10;
static const int32 PanoramaEyeHeight = 160;
static const int32 PanoramaCompressQuality = 80;

enum EWorkState
{
	//空闲
	E_NoneState,
	//渲染中
	E_Rendering,
	//上传中
	E_Uploading,
};

enum EWorkType
{
	E_NoneType,
	//生成单张全景图，保存到本地
	E_GenerateSingleLocal,
	//生成多张全景图，保存到本地
	E_GenerateMulipleLocal,
	//生成单张全景图，上传到服务器
	E_GenerateSingleAndUpload,
	//生成多张全景图，上传到服务器
	E_GenerateMulipleAndUpload,
};

class FArmyPanoramaMgr : public FTickableGameObject
{
public:
	FArmyPanoramaMgr();
	//----------------------------------FTickableGameObject接口
	virtual bool IsTickable() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual TStatId GetStatId() const override;
	virtual void Tick(float DeltaTime) override;
	
	void SetActorList(TArray<FActorInfo>& InActorArray);

	static FArmyPanoramaMgr& Get();
	//取消所有任务
	void ClearAll();
	/** 取消残留的请求 */
	void CancelRequestIfRequest();

	//1 准备截图以及上传，调用FArmyPanoramaController::StartPanorama()进行截图
	void GenerateSingleAndUpload(UArmyViewportClient* MyGVC, ConfigOfPanorama MyConfigOfPanorama);
	//3 获取截图
	void GetPanorama(const TArray<uint32>& Panorama);
	//4 单屋全景图截取完成
	void OnePanoramaFinished();
	//5 开始上传单张全景图
	void StartUploadSingle();/* 没有 6了*/
	//7 Tick上传队列，一旦发现全部上传完毕则通知服务器，获取token之后上传全景图到阿里云
	FTimerHandle TH_CheckUploadingList;
	void TickUploadItems();
	//8 提交全景图信息给后台服务器,获取URL等
	void RequestSubmitPanorama();
	void Callback_RequestSubmitPanorama(FArmyHttpResponse Response);

	//1 获取房屋们的中心点
	void GenerateMultipleAndUpload(ConfigOfPanorama MyConfigOfPanorama);
	//2 每个房屋截取图片
	FTimerHandle TH_CheckIsScreenshot;
	void TickScreenshot();
	/*3，4同上，去掉5，继续调用7*/
	//7 填充导航图数据
	void FillnavMapPicData();
	//截取平面图，做成导航图 
	void OnScreenShot(FViewport* InViewport);

	//计算组成室内全景图不同房间之间连接结构信息
	void CalcLevelInfo();

	static bool IsInBox(FVector _Loc, FVector _Min, FVector _Max);

	bool GetCurVRIndex(FVector _Loc, int32& _BuildingIndex, int32& _LevelIndex, int32& _RoomIndex, int32& _RoomBoxIndex);

	FHomeInfo_Room* GetRoomInfo(int32 _RoomBoxID);

	FHomeInfo_Room* GetRoomInfo(FVector _Loc);

	FHomeInfo_RoomBox* GetRoomBoxInfoByLocation(FVector _Loc);

	FHomeInfo_RoomBox* GetRoomBoxInfo(int32 _RoomBoxID);

	FHomeInfo_HotPoint CreateHotPoint(FVector _Loc);

	static FVector2D GetVR360SpotLocation(FVector _CenterLoc, FRotator _CenterRot, FVector _SpotLoc);

	void FillVRHomeInfo(FWorldPanoData& CurHomeVRInfo);

	int32 IsInRoomId(FVector Point);

	//单屋全景图判断相机在那个房间内
	TSharedPtr<FArmyRoom> GetCurrentRoom();
	//是否在相机所在的房间内
	bool IsPointInRoom(FVector ActorLoc);
	//将商品热点放进房间里
	void SetSpotPointToRoom();
	//导航热点放进房间里，包括门（标准门推拉门）和垭口
	void SetDoorAndPassToRoom();

	//导出，处理全景图的功能函数
	FPanoramaCompletedDelegate PanoramaCompletedDelegate;
	FPanoramaFailedDelegate PanoramaFailedDelegate;

private:
	//上传总进度0-100
	float Progress;
	FText GetProgressText() const { return FText::FromString(FString::Printf(TEXT("全景图生成进度%.2f%%"), Progress)); }

	static TSharedPtr<FArmyPanoramaMgr> XRPanoramaMgr;
	
	//当前世界的全景图信息集合
	FWorldPanoData WorldPanoData;
	//全屋全景图的统一名称前缀
	FString FileNamePreFix;
	//视口
	UArmyViewportClient* ViewportClient;
	//生成所在的世界
	UWorld* CurWorld;
	//当前位置
	FVector CaptureLocation;
	//当前旋转
	FRotator CaptureRotation;
	//工作状态
	EWorkState WorkState;
	//工作类型
	EWorkType WorkType;
	//图片输出到本地的路径
	FString SavePath;
	//纵向6张图
	TArray<uint32> VerticalSixFacesData;
	//正在上传的数量
	int32 UploadingCount;
	//上传成功的全景图数量
	int32 UploadSuccessCount;
	//全景图上传队列
	TArray<TSharedPtr<class FArmyPanoramaUploadItem>> UploadingList;
	//提交请求
	TSharedPtr<IHttpRequest> SubmitRequest;

	//用来作为缩略图的全景图索引
	int32 ThumbnailPanoIndex;
	//热点列表
	TArray<FActorInfo> ActorArray;

	////等待处理上传的全部图片数量总和
	//int32 TotalIamge;
	////已经完成处理的图片数量
	//int32 ImageProcessedCount;
	////已经完成上传的图片数量
	//int32 ImageUploadedCount;
	////正在进行上传的任务
	//int32 UploadingTaskCount;
	////传入UploadList的Key，不唯一
	//int32 UploadingIndex;
	
	//当前处理的是单张还是多张全景图
	bool bMultiple;
	//单屋全景图数据
	TArray<uint8> ImageData;
	//全屋全景图数据
	TArray<TArray<uint8>> Array_ImageData;
	//全景图存放路径
	FString dir;
	//全景图设置
	ConfigOfPanorama MyConfigOfPanorama;
	//单张全景图的名称及包含的二维热点信息
	FPanoScreenshot Screenshot;
	//单屋全景图的房间名称
	FString SingleRoomName;
	//房间中心点列表
	TArray<FVector> RoomCenterList;
	//当前是否正在截图
	bool bBusy;
	//一共需要截取的图片数量
	int32 TotalScreenShotIamge;
	//当前正在截取的是第几张图片
	int32 CurrentScreenShotImage;
	//图片是否已经添加至上传列表
	bool bAddToList;	
	//全屋全景图名称
	TArray<FString> FileName;
	//全屋全景图的名称及包含的二维热点信息
	TArray<FPanoScreenshot> ScreenshotList;
	//外墙的平面坐标
	FVector2D OutRoom2D;
};
