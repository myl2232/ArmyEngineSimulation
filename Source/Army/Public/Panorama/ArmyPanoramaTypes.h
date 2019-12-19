#pragma once
#include "Engine.h"

DECLARE_DELEGATE_ThreeParams(FPanoramaCompletedDelegate, FString /*URL*/, FString /*CodeURL*/, int32 /*panoId*/);
DECLARE_DELEGATE_TwoParams(FPanoramaFailedDelegate, int32 /*error code*/, FString /*Message*/);

//全景图数据流存储及其状态
struct FPanoData
{
	enum EImageDataState
	{
		None,
		Generated,
		Uploading,
		Uploaded,
		Failed,
	};

	TArray<uint8> ImageData;
	FString ImageFileName;
	EImageDataState ImageState;

	FPanoData()
	{
		ImageState = EImageDataState::None;
	}

	//填充数据
	void FillData(FString _ImageFileName, TArray<uint8>& _ImageData)
	{
		ImageFileName = _ImageFileName;
		ImageData = _ImageData;
		ImageState = EImageDataState::Generated;
	}

	void ClearData()
	{
		ImageData.Reset();
		ImageFileName.Empty();
		ImageState = None;
	}
};

//门
struct FHomeInfo_Door
{
	FVector Location;
	bool FrontOpen;
	bool LeftOpen;
	uint8 OpenMode;
	int32 RoomBoxID0;
	int32 RoomBoxID1;
	//--------------------------------------后期计算
	//门一侧所连接的热点
	struct FHomeInfo_HotPoint* HotPoint0;
	//门一侧所连接的热点
	struct FHomeInfo_HotPoint* HotPoint1;

	FHomeInfo_Door() {}

	FHomeInfo_Door(FVector _Location, bool _FrontOpen, bool _LeftOpen, uint8 _OpenMode, int32 _RoomBoxID0, int32 _RoomBoxID1)
	{
		Location = _Location;
		FrontOpen = _FrontOpen;
		LeftOpen = _LeftOpen;
		OpenMode = _OpenMode;
		RoomBoxID0 = _RoomBoxID0;
		RoomBoxID1 = _RoomBoxID1;
		HotPoint0 = NULL;
		HotPoint1 = NULL;
	}
};

//热点
struct FHomeInfo_HotPoint
{
	FVector Location;
	int32 ID;
	//--------------------------------------后期计算
	//所在的房间
	struct FHomeInfo_Room* OwnerRoom;
	//所在RoomBox的名称
	FString Label;
	//6张图片
	FPanoData PanoImageData;

	FHomeInfo_HotPoint() {}
	FHomeInfo_HotPoint(int32 _ID, FVector _Location, FString _Label)
	{
		ID = _ID;
		Location = _Location;
		Label = _Label;
	}

	void ClearData()
	{
		PanoImageData.ClearData();
	}
};

//楼梯
struct FHomeInfo_Stair
{
	FVector Location;
	int32 RoomBoxID0;
	int32 RoomBoxID1;
	//--------------------------------------后期计算
	//楼梯一侧所连接的热点
	FHomeInfo_HotPoint* HotPoint0;
	//楼梯一侧所连接的热点
	FHomeInfo_HotPoint* HotPoint1;
	FHomeInfo_Stair() {}

	FHomeInfo_Stair(FVector _Location, int32 _RoomBoxID0, int32 _RoomBoxID1)
	{
		Location = _Location;
		RoomBoxID0 = _RoomBoxID0;
		RoomBoxID1 = _RoomBoxID1;
	}
};

//房间盒子
struct FHomeInfo_RoomBox
{
	FVector Min;
	FVector Max;
	FString BoxName;
	int32 ID;
	FHomeInfo_Room* OwnerRoom;

	FHomeInfo_RoomBox() {}
	FHomeInfo_RoomBox(int32 _ID, FVector _Min, FVector _Max, FString _BoxName, FHomeInfo_Room* _OwnerRoom)
	{
		ID = _ID;
		Min = _Min;
		Max = _Max;
		BoxName = _BoxName;
		OwnerRoom = _OwnerRoom;
	}
};

//多个房间盒子组成房间
struct FHomeInfo_Room
{
	TArray<FHomeInfo_RoomBox> RoomBoxList;
	int32 ID;

	FHomeInfo_Room() {}
	FHomeInfo_Room(int32 _ID)
	{
		ID = _ID;
	}
	//--------------------------------------后期计算
	//本房间内的所有门
	TArray<FHomeInfo_Door*> MyDoorList;
	//所有热点
	TArray<FHomeInfo_HotPoint*> MyHotPointList;
	//所有楼梯
	TArray<FHomeInfo_Stair*> MyStairList;
	//房间的3D坐标点集
	TArray<FVector> RoomPoints;

	FHomeInfo_HotPoint* GetClosestHotPoint(FVector _Loc)
	{
		FHomeInfo_HotPoint* ClosestHotPoint = NULL;
		float MinDist = 9999999999999.f;
		for (int32 i = 0; i < MyHotPointList.Num(); i++)
		{
			float Dist = (MyHotPointList[i]->Location - _Loc).Size();
			if (Dist <= MinDist)
			{
				ClosestHotPoint = MyHotPointList[i];
				MinDist = Dist;
			}
		}

		return ClosestHotPoint;
	}
};

//多个房间组成楼层
struct FHomeInfo_Level
{
	TArray<FHomeInfo_Room> RoomList;
	FVector StartLocation;
	FVector2D CullPlane;
	int32 ID;

	FHomeInfo_Level() {}
	FHomeInfo_Level(int32 _ID, FVector _StartLocation, FVector2D _CullPlane)
	{
		ID = _ID;
		StartLocation = _StartLocation;
		CullPlane = _CullPlane;
	}
};

//多个楼层组成一幢楼
struct FHomeInfo_Building
{
	TArray<FHomeInfo_Level> LevelList;
	FVector CameraTargetOffset;
	float CameraTopDist;
	float CameraTopYaw;
	float CameraTPSDist;
	FRotator CameraTPSRot;

	int32 ID;

	FHomeInfo_Building() {}
	FHomeInfo_Building(int32 _ID, FVector _CameraTargetOffset, float _CameraTopDist, float _CameraTopYaw, float _CameraTPSDist, FRotator _CameraTPSRot)
	{
		ID = _ID;
		CameraTargetOffset = _CameraTargetOffset;
		CameraTopDist = _CameraTopDist;
		CameraTopYaw = _CameraTopYaw;
		CameraTPSDist = _CameraTPSDist;
		CameraTPSRot = _CameraTPSRot;
	}
};

//整个场景的全景图信息
struct FWorldPanoData
{
	TArray<FHomeInfo_Building> BuildingList;
	TMap<int32, FHomeInfo_Door> DoorList;
	TMap<int32, FHomeInfo_HotPoint> HotPointList;
	TMap<int32, FHomeInfo_Stair> StairList;
	FHomeInfo_HotPoint HotPointSingle;
	//全景图配置信息是否正确
	bool bValid;
	void ClearData()
	{
		BuildingList.Reset();
		DoorList.Reset();
		HotPointList.Reset();
		StairList.Reset();
		bValid = true;
	}
};

//全景图中的热点单位
struct FPanoSpot
{
	int32 AtH;
	int32 AtV;
	FString LinkFileName;
	int32 SpotType;
	FString FurId;
	FPanoSpot() {}
	FPanoSpot(int32 _AtH, int32 _AtV, FString _LinkFileName, int32 _SpotType = 1, FString _FurId = TEXT(""))
	{
		AtH = _AtH;
		AtV = _AtV;
		LinkFileName = _LinkFileName;
		SpotType = _SpotType;
		FurId = _FurId;
	}
};

//每张全景图的名称及包含的二维热点信息
struct FPanoScreenshot
{
	int32 RoomID;
	FString FileName;
	FString Title;
	FString navMapRatioX;
	FString navMapRatioY;
	TArray<FPanoSpot> SpotList;

	FPanoScreenshot() {}
	FPanoScreenshot(int32 _RoomID, FString _FileName, FString _Title, FString _navMapRatioX, FString _navMapRatioY)
	{
		RoomID = _RoomID;
		FileName = _FileName;
		Title = _Title;
		navMapRatioX = _navMapRatioX;
		navMapRatioY = _navMapRatioY;
	}
};

//物体列表信息
struct FActorInfo
{
	int32 RoomID;
	int32 ActorID;
	//0:Actor,	1:Material
	int32 Type;
	FVector Location;
	FActorInfo()
	{
		RoomID = -1;
		ActorID = 0;
		Type = 0;
		Location = FVector(0, 0, 0);
	}
};