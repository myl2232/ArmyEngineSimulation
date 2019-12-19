#pragma once
#include "Engine/Font.h"
//#include "TextureCompressorModule.h"
#include "ArmyTextureCompressorModule.h"
#include "ArmyTextureSource.h"
#include "ArmySpotLightActor.h"
#include "ArmyEngineResource.generated.h"



UCLASS()
class ARMYENGINE_API UArmyEngineResource : public UObject
{
	GENERATED_BODY()

public:
	UArmyEngineResource();

	UFont* GetDefaultFont();

	UFont* GetCHFont();

	/** @欧石楠 线框模式后处理材质 */
	UMaterialInstanceDynamic* GetPM_Outline();

	/** @欧石楠 生成一个基础颜色材质，需要自行保存变量 在外部调用后addtoroot*/
	UMaterialInstanceDynamic* GetUnStageBaseColorMaterial();

    /** @欧石楠 通过颜色获取对应的材质 */
	UMaterialInstanceDynamic* GetBaseColorMaterial();

	UMaterial* GetDefaultWallFillMat();

    /** @欧石楠 获取50%透明度白色材质 */
    UMaterialInstanceDynamic* GetTranslucentWhiteMaterial();

	UMaterial* GetFontMaterial();

	UMaterial* GetCHFontMaterial();

	/** 绘制线段的默认材质 */
	UMaterial* GetLineMaterial();

	UMaterial* GetDefaultHightLightMaterial();

	UTexture2D* GetDefaultTexture();

	UTexture2D* GetDefaultWhiteTexture();

	/* @欧石楠 墙顶地默认材质*/
	UTexture2D* GetDefaultTextureWall();
	UTexture2D* GetDefaultTextureFloor();
	UTexture2D* GetDefaultTextureCeiling();

	/* @欧石楠 下吊斜线*/
	UMaterial* GetObliqueLineMat();

	UMaterial* GetDefaultFloorTextureMat();

	UMaterial* GetDefaultConstructionMat();

	UMaterial* GetDefaultBridgeStoneMat();

	UMaterial* GetBaseMapMaterial();

	UMaterial* GetBaseColorMat();

	UMaterial* GetMainWallMaterial();

	UMaterial* GetAddWallMaterial();

	UMaterial* GetAWLightgageSteelJoistMaterial();

	UMaterial* GetAWBlackBrickMaterial();

	UMaterial* GetModifyWallMaterial();

	//@	承重墙材质
	UMaterial* GetBearingWallMaterial();

	UMaterial* GetLineTestMaterial();

	UMaterial* GetWindowPillarMaterial();

	UMaterialInstance* GetGlassMaterial();

    /** 门的默认材质 */
    UMaterial* GetSingleDoorMaterial();

	//欧石楠 下载
	UMaterial* GetDownloadMaterial();

	UMaterial* GetRoomSpaceMaterial();

	UMaterial* GetCheckBoxMat(bool InChecked = false);

	/** 墙面的默认材质 */
    UMaterialInstanceDynamic* GetWallMaterial(float Opacity = 1.f);

	/** @欧石楠 根据URL找材质*/
	UMaterialInstanceDynamic* GetMaterialByURL(FString URL);

	/** @欧石楠 立面模式顶面视图半透明蒙板材质*/
	UMaterialInstanceDynamic* GetTranslucentRoofMaterial(float Opacity = 1.f);

    /** 地面的默认材质 */
    UMaterialInstanceDynamic* GetFloorMaterial(float Opacity = 1.f);

	UMaterialInstance* GetWindowBaseMaterial();

	// 管线默认材质实例
	UMaterialInstance* GetLineMaterialIns();

	UTexture2D* GetBSPFloorTexture();

	UTexture2D* GetBSPWallTexture();

	/** 踢脚线的默认材质 */
	UMaterialInstance* GetBaseboardMaterial();

	/** @ 门洞的默认模型 */
	UMaterial* GetPassColorMesh();

	/** @欧石楠 获取场景外景材质实例 */
	UTexture2D* GetOutdoorPanoramaTexture(int32 InIndex);

	/** 点光源类 */
	TSubclassOf<class AArmyPointLightActor> GetPointLightClass();

	/** 射灯类 */
	TSubclassOf<class AArmySpotLightActor> GetSpotLightClass();

	/** 反射球类 */
	TSubclassOf<class AArmyReflectionCaptureActor> GetReflectionCaptureClass();

	/** 热点球类 */
	TSubclassOf<class AArmyLevelAssetBoardActor> GetLevelAssetBoardClass();

	bool WriteRawToTexture(UTexture2D* NewTexture2D, const TArray<uint8>& RawData, bool bGray, bool bUseSRGB = true);

	UTexture2D* ImportTexture(FString _FilePath,bool GenerateMipMap = false);

private:
	//void Generate(UTexture2D& InTexure,const FArmyTextureBuildSettings& InSettings, uint32 InFlags, ITextureCompressorModule* Compressor, FArmyTextureSource& InSouce);

	UPROPERTY()
	UMaterial* PM_Outline;

	UPROPERTY()
	UMaterial* M_DefaultHighLightMat;

	UPROPERTY()
	UMaterialInstanceDynamic* PMID_Outline;

	UPROPERTY()
		UMaterial* M_DefaultBridgeStoneMat;
	UPROPERTY()
		UMaterial* M_DefaultWallFillMat;
	UPROPERTY()
	UMaterial* DefaultConstructionMat;

	UPROPERTY()
		UMaterial* ObliqueLineMat;

	UPROPERTY()
	UMaterial* DefautFloorTextureMat;

	UPROPERTY()
	UTexture2D* DefaultFloorTexture;

	UPROPERTY()
	UTexture2D* DefaultTextureWall;

	UPROPERTY()
	UTexture2D* DefaultTextureFloor;

	UPROPERTY()
	UTexture2D* DefaultTextureCeiling;

	UPROPERTY()
	UTexture2D* DefaultBSPFloorTexture;

	UPROPERTY()
	UTexture2D* DefaultWhiteTexture;

	UPROPERTY()
	UTexture2D* DefaultBSPWallTexture;

	UPROPERTY()
	UMaterial* M_Line;

	UPROPERTY()
	UMaterial* M_MainWall;

	UPROPERTY()
	UMaterial* M_BaseMap;

	UPROPERTY()
	UMaterial* M_BaseColor;

	UPROPERTY()
	UMaterial* M_AddWall;

	UPROPERTY()
		UMaterial* M_AWLightgageSteelJoist;

	UPROPERTY()
		UMaterial* M_AWBlackBrick;

	UPROPERTY()
	UMaterial* M_ModifyWall;

	//@ 承重墙
	UPROPERTY()
	UMaterial* M_BearingWall;

	UPROPERTY()
	UMaterial* M_ProgressCircle;

	UPROPERTY()
	UMaterial* M_RoomSpace;

	UPROPERTY()
	UMaterial* M_LineTest;

	UPROPERTY()
	UMaterialInstanceDynamic* MI_Wall;

    UPROPERTY()
	UMaterialInstanceDynamic* MI_Wall_Opaque;

    UPROPERTY()
	UMaterialInstanceDynamic* MI_Wall_Translucent;

    UPROPERTY()
	UMaterialInstanceDynamic* MI_Floor;

    UPROPERTY()
	UMaterialInstanceDynamic* MI_Floor_Opaque;

    UPROPERTY()
	UMaterialInstanceDynamic* MI_Floor_Translucent;

	UPROPERTY()
	UMaterialInstance* MI_LineIns;

	UPROPERTY()
	UMaterialInstance* MI_Baseboard;

	UPROPERTY()
	UMaterialInstance* MI_Glass;

    UPROPERTY()
    UMaterial* M_SingleDoor;

	UPROPERTY()
	UMaterialInstance* MI_WindowBase;

	UPROPERTY()
	UMaterial* MI_WindowPillar;

	UPROPERTY()
	UTexture* T_Logo;

	UPROPERTY()
	TSubclassOf<class AArmyPointLightActor> PointLightClass;

	UPROPERTY()
	TSubclassOf<class AArmySpotLightActor> SpotLightClass;

	UPROPERTY()
	TSubclassOf<class AArmyReflectionCaptureActor> ReflectionCaptureClass;

	UPROPERTY()
	TSubclassOf<class AArmyLevelAssetBoardActor> LevelAssetBoardClass;

	UPROPERTY()
	UFont* T_Font;

	UPROPERTY()
	UMaterial* T_FontMt;

    UPROPERTY()
    UMaterialInstanceDynamic* MI_BaseColor;

    UPROPERTY()
    UMaterial* M_BaseTexture;

	UPROPERTY()
	UMaterial* M_Pass;

	UPROPERTY()
	UMaterial* M_CheckBoxFalse;

	UPROPERTY()
	UMaterial* M_CheckBoxTrue;

    UPROPERTY()
    UMaterialInstanceDynamic* MI_TranslucentWhite;

	TMap<FString, UTexture2D*> TextureLists;

	/** @欧石楠 外景全景图纹理列表 */
	UPROPERTY()
	TMap<FString, UTexture2D*> OutdoorPanoramaTextures;
};