// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object.h"
#include "IPlatformFilePak.h"
#include "Runtime/Engine/Classes/Animation/AnimSequence.h"
#include "XmlParser.h"
#include "SCTResTypes.h"
#include "GenericPlatformChunkInstall.h"
#include "SCTXRTypes.h"
#include "SCTXRCommonTypes.h"
#include "Engine/StaticMesh.h"
#include "SCTResManager.generated.h"

class FSlateTextureRenderTarget2DResource;

#define ECC_VRSSM		ECC_GameTraceChannel4

USTRUCT()
struct SCTXRRESOURCE_API FSCTVRSObject
{
public:
	//吸附面类型
	enum class EMeshSnapType
	{
		MST_None = 0,
		//墙面
		MST_Wall,
		//地面
		MST_Bottom,
		//顶面
		MST_Top,
		//台面
		MST_TableFace,
	};
public:
	GENERATED_USTRUCT_BODY()
	//数组元素内容定义如下	0:UStaticMesh/UMaterialInterface/UBlueprintClass  1:SkeltalMesh/UStaticMesh(Lamp)  2:UAnimSequence
	UPROPERTY()
	TArray<UObject*> Objects;
	UPROPERTY()
	TArray<FSCTModelMaterialData> MaterialList;
	//服务器保存的参数个数跟模型的材质ID号不一致，匹配失败，给出红色字体警告
	bool MaterialParaMismatch;
	FVector BoundOrigin;
	FVector BoundExtent;
	TArray<FTransform> PointArray;
	TArray<FTransform> SpotArray;
	FLinearColor LampColor;
	float  LampIntensity;
	//服务器同步数据
	int32 ItemID;
	FString FileName;
	FString FilePath;
	ESCTResourceType FileType;
	FString Param;
	//标准化导入的pak（用工具导入制作的模型，必定有pak.xml文件）
	bool bNormalized;
	int32 SynID;

	//吸附面类型
	EMeshSnapType SnapType;
	//模型旋转轴点类型
	FVector OriginOffset;
	//纵向多张贴图拼合，拼图的数量
	int32 MapCount;
	//纵向多张贴图拼合，每个贴图的尺寸
	FVector2D MapSize;

	//插槽信息
	struct FSocketData
	{
		FSocketData(FString InName, FVector InLocation, FRotator InRotation)
			: Name(InName), Location(InLocation), Rotation(InRotation)
		{}
		FString Name;
		FVector Location;
		FRotator Rotation;
	};

	//插槽列表
	TArray<FSocketData> SocketList;

	FSCTVRSObject()
	{
		bNormalized = false;
		MaterialParaMismatch = false;
		BoundOrigin = FVector::ZeroVector;
		BoundExtent = FVector::ZeroVector;
	}
	~FSCTVRSObject()
	{
		bNormalized = false;
	}

	void ResetMaterialParameters()
	{
		if (MaterialList.IsValidIndex(0))
		{
			MaterialList[0].ResetToServer();
		}
	}

	UObject* GetFirstObject()
	{
		if (Objects.IsValidIndex(0))
		{
			return Objects[0];
		}
		else
			return NULL;
	}

	USkeletalMesh* GetSkeletalMeshObject()
	{
		if (Objects.IsValidIndex(1))
		{
			return Cast<USkeletalMesh>(Objects[1]);
		}
		else
			return NULL;
	}

	UAnimSequence* GetAnimSequenceObject()
	{
		if (Objects.IsValidIndex(2))
		{
			return Cast<UAnimSequence>(Objects[2]);
		}
		else
			return NULL;
	}

	UStaticMesh* GetLampMeshObject()
	{
		if (Objects.IsValidIndex(1))
		{
			return Cast<UStaticMesh>(Objects[1]);
		}
		else
			return NULL;
	}

	void SetFirstObject(UObject* _Object)
	{
		if (Objects.IsValidIndex(0))
		{
			Objects[0] = _Object;
		}
		else
		{
			Objects.Add(_Object);
		}
	}

	void SetObjects(TArray<UObject*> _Objects)
	{
		Objects = _Objects;
	}

	void SetSkeletalMeshObject(USkeletalMesh* _SkeletalObj)
	{
		if (Objects.Num() == 0)
		{
			Objects.Add(NULL);
			Objects.Add(_SkeletalObj);
		}
		else if (Objects.Num() == 1)
		{
			Objects.Add(_SkeletalObj);
		}
		else if (Objects.Num() == 2)
		{
			Objects[1] = _SkeletalObj;
		}
	}

	void SetLampMeshObject(UStaticMesh* _StaticObj)
	{
		if (Objects.Num() == 0)
		{
			Objects.Add(NULL);
			Objects.Add(_StaticObj);
		}
		else if (Objects.Num() == 1)
		{
			Objects.Add(_StaticObj);
		}
		else if (Objects.Num() == 2)
		{
			Objects[1] = _StaticObj;
		}
	}

	void SetAnimSequenceObject(UAnimSequence* _AnimObj)
	{
		if (Objects.Num() == 0)
		{
			Objects.Add(NULL);
			Objects.Add(NULL);
			Objects.Add(_AnimObj);
		}
		if (Objects.Num() == 1)
		{
			Objects.Add(NULL);
			Objects.Add(_AnimObj);
		}
		else if (Objects.Num() == 2)
		{
			Objects.Add(_AnimObj);
		}
		else if (Objects.Num() == 3)
		{
			Objects[2] = _AnimObj;
		}
	}
};

class FSCTXRResourceManagerInstatnce;

/**
 * @brief 资源管理加载缓存模块，本模块有马云龙提供
 * @note  修改记录：
 *			2018年8月7日:欧石楠
 *					1.私有化构造函数，析构函数通过FSCTXRResourceManagerInstatnce来提供外部单例化
 */
UCLASS()
class SCTXRRESOURCE_API USCTXRResourceManager : public UObject
{
	GENERATED_BODY()
	
private:
	friend class FSCTXRResourceManagerInstatnce;
	
	USCTXRResourceManager();
	~USCTXRResourceManager();
public:

	void Init();

	virtual void BeginDestroy() override;

	FString GetDebugInfo();

	static ESCTResourceType GetFileType(FString _PakName);
	
	static FVector GetVecFromColor(FLinearColor _Color);
	//PakType 0 Home  1 Model  2 Material 3 Actor    例如：LR_0  =====>>  ../../../XR/Game/DBJCache/LR/LR_0.pak 
	static FString GetPathFromFileName(ESCTResourceType _PakType, FString& _PakName, bool _Test = false);

	//传入一个UObject，返回ItemID，比如5001。如果WithPackageID为true，则返回5001::0
	int32 GetItemIDFromObj(UObject* _Obj);

	//传入一个AActor，返回ItemID，比如5001。如果WithPackageID为true，则返回5001::0
	int32 GetItemIDFromActor(AActor* _Actor);

	//传入一个ObjID，如果存在则返回一个FSCTVRSObject指针
	FSCTVRSObject* GetObjFromObjID(int32 _ObjID);

	//传入一个SynID，如果存在则返回一个FModelSynData指针
	TSharedPtr<FSCTContentItemSpace::FContentItem> GetContentItemFromID(int32 _SynID);
	
	/**
	 * 根据资源ID和资源类型，获取ContentItem。比如当前如果只知道HomeID，则可以通过HomeID和ResourceType来获取ContentItem
	 * @param InResType - ESCTResourceType - 资源类型
	 * @param InID - int32 - 资源ID
	 * @return FSCTContentItemSpace::FContentItem* - 资源具体数据
	 */
	TSharedPtr<FSCTContentItemSpace::FContentItem> GetContentItemFromItemID(ESCTResourceType InResType, int32 InID);

	//传入一个UMaterialInterface，根据它父材质的类型，返回它的路径
	FString GetMaterialFullName(UMaterialInterface* _MI);

	//获取材质的父材质的每一层路径
	void GetParentMostPath(UMaterialInterface* _MI, TArray<FString>& _Paths);

	//遍历LevelPathMap映射表，根据当前加载的Level的路径，返回Level所在的Pak文件名称
	FString GetCurMapPakName();


	//遍历LevelPathMap映射表，根据Pak文件名称，返回Level的路径
	FString FindLevelPath(FString _HomePakName, bool _Test = false);
	
	//传入一个新的UObject和旧的UObject，如果旧的UObject存在于ObjList中，则复制一份，插入ObjList末尾，并把新的UObject设置进去。
	bool CloneObject(UObject* _NewObject, UObject* _OldObject);

	//传入一个UMaterialInterface，创建一个UMaterialInstanceDynamic。这个过程中会调用CloneObject。这样保证所有材质Object都独立信息不重复。
	UMaterialInstanceDynamic* CreateMID(UMaterialInterface* _MI);
	

	//传入AActor，如果它是一个AStaticMeshActor，则返回它的UStaticMeshComponent
	UStaticMeshComponent* GetActorSMC(AActor* _InActor);

	//获取AActor类型
	ESCTActorType GetActorType(AActor* _InActor);

	//传入服务器同步数据，调用LoadObj，直接创建Actor，它会从硬盘中加载新资源。
	AActor* CreateActor(UWorld* _OwnerWorld, TSharedPtr<FSCTContentItemSpace::FContentItem> _SynData, FVector _Location = FVector::ZeroVector, FRotator _Rotation = FRotator::ZeroRotator, FVector _Scale = FVector(1, 1, 1));
	//传入服务器同步数据，调用LoadObj，直接创建Material
	UMaterialInterface* CreateMaterial(UWorld* _OwnerWorld, TSharedPtr<FSCTContentItemSpace::FContentItem> _SynData);
	
	//通过PakID来创建Actor，Actor所用得到的资源一定是已经存在于ObjList中，否则创建失败。它不会从硬盘中加载资源。
	AActor* CreateActorFromID(UWorld* _OwnerWorld, int32 _FileID, FVector _Location = FVector::ZeroVector, FRotator _Rotation = FRotator::ZeroRotator, FVector _Scale = FVector(1, 1, 1));
	
	//加载新资源，如果资源已存在于ObjList中，则直接使用。
	FSCTVRSObject* LoadObj(TSharedPtr<FSCTContentItemSpace::FContentItem> _SynData, int32& _OutObjID, int32& _OutSynID);
	
	//通过PakID直接加载已存在于ObjList中的资源，否则创建失败。他不会从硬盘中加载资源。
	FSCTVRSObject* LoadObjFromFileID(int32 _FileID, int32& _OutObjID, int32& _OutSynID);

	/**
	* @brief 创建材质，扩展一个参数附件参数字符串
	* @param[in] _FilePath  路径
	* @return 返回材质实例，如果是null，证明创建失败
	*/
	UMaterialInterface* CreateCustomMaterial(FString _FilePath);

	/**
	 * @brief 创建材质，扩展一个参数附件参数字符串
	 * @param[in] _FilePath  路径
	 * @param[in] _Param 材质参数 
	 * @return 返回材质实例，如果是null，证明创建失败
	 */
	UMaterialInterface* CreateCustomMaterial(const FString & _FilePath,const FString & _Param);

	FSCTVRSObject*  LoadCustomObj(const FString &  _FilePath);

	FSCTVRSObject*  LoadCustomObj(const FString &  _FilePath, const FString & _Param);

	FSCTVRSObject* LoadCustomObj(TSharedPtr<FSCTContentItemSpace::FContentItem> _SynData, int32& _OutObjID, int32& _OutSynID);

	UStaticMesh * CreateStaticMesh(const FString & _FilePath, const FString & _Param);


	//是否开启UseProjectiveUV功能，会对应两套不同的材质参数。
	void SetMaterialRotation(UMaterialInstanceDynamic* _MID, float _Angle);
	void SetMaterialOffsetU(UMaterialInstanceDynamic* _MID, float _OffsetU);
	void SetMaterialOffsetV(UMaterialInstanceDynamic* _MID, float _OffsetV);
	void SetMaterialTilingU(UMaterialInstanceDynamic* _MID, float _TilingU);
	void SetMaterialTilingV(UMaterialInstanceDynamic* _MID, float _TilingV);

	float GetMaterialRotation(UMaterialInstanceDynamic* _MID);
	float GetMaterialOffsetU(UMaterialInstanceDynamic* _MID);
	float GetMaterialOffsetV(UMaterialInstanceDynamic* _MID);
	float GetMaterialTilingU(UMaterialInstanceDynamic* _MID);
	float GetMaterialTilingV(UMaterialInstanceDynamic* _MID);
	bool GetMaterialUseProjectiveUV(UMaterialInstanceDynamic* _MID);

	//恢复模型的默认材质
	void ClearActorOverridenMaterials(AActor* _Actor);

	//恢复模型的尺寸信息
	void ResetActorTransform(AActor* _Actor);

	//颠倒U和V的值
	void SwitchMaterialUV(UMaterialInstanceDynamic* _MID);
	//映射旧的材质球名称到新的V7版，如 WallPaper->V7_Base_Plastic, Floor->V7_Base_Wood, GroundTile->V7_Base_Ceramics
	static FString ConvertOldMaterialNameToV7(FString _OldMaterialName);
	//映射旧的材质参数到新的V7版，主要是0-19
	static int32 ConvertOldMaterialIDToV7(int32 _OldID);
	//6200版本之前的材质OffsetUV取值范围为-1-1，6200及之后版本范围0-100，要做一个映射
	static float ConvertOldOffsetToV7(float _OldOffset);
	//6200版本之前的材质Tiling映射到V7版本需要乘0.4f（UseProjectiveUV==1）
	static float ConvertOldProjectiveTilingToV7(float _OldTiling);

	//把从服务器获取的材质参数Json字符串转换为模型材质参数列表
	bool ConvertJsonToModelMaterialList(bool _bConverToV7, FString _ParaStr, TArray<FSCTModelMaterialData>& _OutMaterialList);
	//把模型材质列表信息转换成Json字符串。bOnlyDirty为true时，会先调用CheckDirty，只有发生变化的值才会保存转换成Json
	FString ConvertModelMaterialListToJson(TArray<FSCTModelMaterialData>& _MaterialList, bool _bOnlyServerDirty, bool _bOnlyOriginalDirty = false);
	//根据输入的材质名称，创建MID，生成新的FSCTModelMaterialData
	FSCTModelMaterialData CreateMaterialData(FString _MaterialName, FVector2D _TextureSize = FVector2D(800,800), UTexture* _DiffuseMap = NULL, UTexture* _NormalMap = NULL);
	//解析模型文件中的XML，创建材质。
	FSCTModelMaterialData CreateModelMaterialData(int32 _Version, FXmlNode* _MaterialNode, TMap<FString, UTexture*>& _TextureMap);
	//解析模型文件中的XML，创建材质。V7之前就材质系统使用。
	FSCTModelMaterialData CreateModelMaterialData_Internal_BeforeV7(FXmlNode* _MaterialNode, TMap<FString, UTexture*>& _TextureMap);
	//解析模型文件中的XML，创建材质。V7使用。
	FSCTModelMaterialData CreateModelMaterialData_Internal_V7(FXmlNode* _MaterialNode, TMap<FString, UTexture*>& _TextureMap);
	//根据V7材质名称，创建一个MID，并设置所涉及的参数默认值
	FSCTModelMaterialData CreateModelMaterialDataFromName_V7(int32 _MaterialTypeIndex);
	//为传入的MaterialData添加自定义的一些参数到ParaList中，比如是否启用漫反射贴图，法线贴图序号，材质类型序号等
	void AddCustomMaterialParametersForMaterialData_V7(FSCTModelMaterialData& _OutMaterialData);
	//获取材质类型序号 布料0-99 金属100-199 木质200-299 皮革300-399
	int32 GetNormalTextureIndex(UTexture* _Texture);
	UTexture* GetNormalTexture(int32 _Index);
	//根据传入的材质名称返回材质序号
	int32 GetMaterialTypeIndex(FString _MaterialName, bool _bDisplayName = false);
	//根据材质名称修改已存在的MaterialData
	void ChangeModelMaterialData(int32 _NewMaterialTypeIndex, FSCTModelMaterialData* _OldMaterialData);

	//把从服务器获取的参数Json字符串转换为模型属性参数列表
	bool ConvertJsonToModelSettingsList(FString _ParaStr, TArray<FSCTXRParameter>& _OutSettingsList);
	FString ConvertModelSettingsListToJson(TArray<FSCTXRParameter>& _SettingsList);
	void CollectActorLightComponentsParameters(AActor* _Actor, TArray<FSCTXRParameter>& _OutSettingsList);
	void SetActorLightComponentParameters(AActor* _Actor, TArray<FSCTXRParameter>& _SettingsList);

	//从Actor中查找第一个网格组件，如果是StaticMeshActor，则返回它的StaticMeshComponent，如果是蓝图物体遍历它的Components，返回第一个MeshComponent
	UMeshComponent* GetFirstMeshComponent(AActor* _Actor);

	//把一份材质参数数据覆盖到另一份数据上。其中_SrcData的Object不会覆盖_DesData中的Object，仅仅是覆盖ParaList参数，最后会_DesData.ParaList[i].DIM->SetParameters...
	bool OverrideMaterialData(FSCTModelMaterialData& _DesData, FSCTModelMaterialData& _SrcData);

	//传入一个XML Node，得出材质信息，以及ActorName
	void GetMeshMaterialDataFromXMLNode(const FXmlNode* _InNode, FSCTMeshMaterialData& _OutData, FString& _OutActorName);

	void AppendSynList(TArray<TSharedPtr<FSCTContentItemSpace::FContentItem>> _NewList);

	//退出户型时调用
	void ClearData();

	const ANSICHAR* GetAESKey() { return AESKey; }
	const ANSICHAR* AESKey = "8DEF1FC02DE17BBC4283C7DEB882CF99\0";

private:
	//载入Common包Pak
	void LoadCommonData();
	AActor* CreateActorInternal(FSCTVRSObject* _ObjectInfo, int32 _ObjID, int32 _SynID, FVector _Location = FVector::ZeroVector, FRotator _Rotation = FRotator::ZeroRotator, FVector _Scale = FVector(1, 1, 1));
	UMaterialInterface* CreateMaterialInternal(FSCTVRSObject* _ObjectInfo, int32 _ObjID, int32 _SynID);
	bool LoadObjInternal(FSCTVRSObject& _OutObj);
	void MountPakReturnFiles(FString _PakPath, TArray<FString>& OutPakFilenameList, int32 _Order = 3, bool _IDontWantToMount = false);
	void MountPak(FString _PakPath);
	//卸载所有模型资源
	void UnloadModelPak();
	void GetParentMostPathInternal(UMaterialInterface* _MI);
	TArray<FString> TempPaths;
public:
	FName PendingLevelName;
	//XML路径。如果该变量不为空，则会在打开任意户型时，尝试加载户型方案
	FString PendingLoadHomePlanXMLPath;
	//XML路径。如果该变量不为空，则会在打开任意户型时，尝试加载家具组方案
	FString PendingLoadGroupPlanXMLPath;
	//XML路径。如果该变量不为空，则在打开DIYTemplate地图时，第一件事就是加载该xml文件，解析并构建户型
	FString PendingLoadDIYHomeXML;

	//材质参数静态对照表
	TMap<int32, FSCTXRParameter> StaticMIParaList;
	//模型设置参数静态对照表
	TMap<int32, FSCTXRParameter> StaticModelParaList;
	//Common9中的新材质球（V7版）名称信息对照表 
	UPROPERTY()
		TArray<FSCTCommonMaterialData> StaticCommonMaterialList;
	UPROPERTY()
		UTexture* EmptyTexture;
	UPROPERTY()
		TArray<UTexture*> IESList;
	UPROPERTY()
		TArray<UTexture*> NormalMapList_Cloth;
	UPROPERTY()
		TArray<UTexture*> NormalMapList_Metal;
	UPROPERTY()
		TArray<UTexture*> NormalMapList_Wood;
	UPROPERTY()
		TArray<UTexture*> NormalMapList_Leather;

	//点击进入户型，通过户型的Pak名称，来尝试加载Pak文件，然后找出LevelPath，并且Open之
	TMap<FString, FString> LevelPathMap;
	//载入方案的xml版本不同，会有不同的处理
	int32 CurLoadPlanVersion;

	TSharedPtr<FPakPlatformFile> PakPlatform;
	IPlatformFile* LocalPlatformFile;
	//0:uasset 1:pak 2:other
	int8 DLCFileMode;
	bool bGodMode;
	UPROPERTY()
		TArray<FSCTVRSObject> ObjList;
	TArray<TSharedPtr<FSCTContentItemSpace::FContentItem>> SynList;

	//当前户型为绘制户型
	bool bDIYHome;
	//当前加载了之前保存的绘制户型，而不是从新开始绘制
	bool bLoadDIYHome;
	//记录了当前户型载入过程中是否载入了光源等物体，如果是，则在清空方案的时候不会清空。
	bool bLoadEnvironmentAssets;

	struct ActorSizeInfo
	{
		FString ID = TEXT("-1");
		int32 TypeID = 0;
		float Length = 0;
		float Width = 0;
		float Height = 0;
	};
	TArray<ActorSizeInfo> UpdateSizeActorList;

	UWorld* CurWorld;

};

/**
 *@brief 为USCTXRResourceManager提供单例封装
 */
class SCTXRRESOURCE_API FSCTXRResourceManagerInstatnce
{
private:
	FSCTXRResourceManagerInstatnce();
	~FSCTXRResourceManagerInstatnce();
public:	
  static FSCTXRResourceManagerInstatnce & GetIns();
  static void ReleaseIns();

public:

	/** 在此函数里面进行USCTXRResourceManager的初始化 */
	bool StartUp();

	/** 在此函数里面进行USCTXRResourceManager的反初始化 */
	bool ShutDown();

	USCTXRResourceManager & GetResourceManager();
private:
	UPROPERTY()
	USCTXRResourceManager * XRManager ;
	static FSCTXRResourceManagerInstatnce* Singleton;
};



