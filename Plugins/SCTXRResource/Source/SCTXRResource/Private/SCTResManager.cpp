#include "SCTResManager.h"
#include "SceneViewport.h"
#include "XmlFile.h"
#include "PlatformFilemanager.h"
#include "Materials/MaterialInstanceConstant.h"
#include "SCTResTools.h"
#include "Engine/StaticMeshActor.h"
#include "Components/SpotLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereReflectionCaptureComponent.h"
//#include "ArmyDownloadModule.h"
#include "SCTXRCommonTypes.h"

USCTXRResourceManager::USCTXRResourceManager()
{
	bDIYHome = true;
	bLoadDIYHome = false;
	bLoadEnvironmentAssets = false;
}

USCTXRResourceManager::~USCTXRResourceManager()
{
	
}

void USCTXRResourceManager::Init()
{
	FCoreDelegates::GetPakEncryptionKeyDelegate().BindUObject(this, &USCTXRResourceManager::GetAESKey);

	bGodMode = false;
	DLCFileMode = 1;//pak
	CurLoadPlanVersion = 0;
	LocalPlatformFile = &FPlatformFileManager::Get().GetPlatformFile();
	PakPlatform = MakeShareable(new FPakPlatformFile());
	if (LocalPlatformFile != nullptr)
	{
		if (PakPlatform.IsValid())
		{
			PakPlatform->Initialize(LocalPlatformFile, TEXT(""));
		}
	}

	//特殊定义
	StaticMIParaList.Add(-1, FSCTXRParameter(TEXT("漫反射贴图"), TEXT("Diffuse_UseTexture"), TEXT("Diffuse_UseTexture"), false));
	StaticMIParaList.Add(-2, FSCTXRParameter(TEXT("阴影贴图"), TEXT("Opacity_UseTexture"), TEXT("Opacity_UseTexture"), false));
	StaticMIParaList.Add(-3, FSCTXRParameter(TEXT("法线贴图编号"), TEXT("NormalMapIndex"), TEXT("NormalMapIndex"), 0.f, 100.f, -2.f));//-1默认自带法线，-2不带法线，0-N对应系统提供可选的序号
	StaticMIParaList.Add(-4, FSCTXRParameter(TEXT("材质类型编号"), TEXT("MaterialTypeIndex"), TEXT("MaterialTypeIndex"), 0.f, 100.f, 0.f));
	//模型材质参数对照表
	StaticMIParaList.Add(0, FSCTXRParameter(TEXT("环境阴影_强度"), TEXT("AO_intensity"), TEXT("AO_intensity"), 0.f, 1.f));
	StaticMIParaList.Add(1, FSCTXRParameter(TEXT("环境阴影_对比度"), TEXT("AO_Contrast"), TEXT("AO_Contrast"), 0.f, 1.f));
	StaticMIParaList.Add(2, FSCTXRParameter(TEXT("固有色"), TEXT("Color"), TEXT("Color")));
	StaticMIParaList.Add(3, FSCTXRParameter(TEXT("贴图混色"), TEXT("Color_Map_Blend"), TEXT("Color_Map_Blend")));
	StaticMIParaList.Add(4, FSCTXRParameter(TEXT("固有色_重复度_横向"), TEXT("Color_Map_Tiling_U"), TEXT("Color_Map_Tiling_U"), 0.f, 100.f));
	StaticMIParaList.Add(5, FSCTXRParameter(TEXT("固有色_重复度_纵向"), TEXT("Color_Map_Tiling_V"), TEXT("Color_Map_Tiling_V"), 0.f, 100.f));
	StaticMIParaList.Add(6, FSCTXRParameter(TEXT("固有色_旋转"), TEXT("Color_Map_RotateUV"), TEXT("Color_Map_RotateUV"), 0.f, 1.f));
	StaticMIParaList.Add(7, FSCTXRParameter(TEXT("自发光"), TEXT("Emissive_Color"), TEXT("Emissive_Color")));
	StaticMIParaList.Add(8, FSCTXRParameter(TEXT("金属"), TEXT("Metalic"), TEXT("Metalic"), 0.f, 1.f));
	StaticMIParaList.Add(9, FSCTXRParameter(TEXT("法线_强度"), TEXT("Normal_Intensity"), TEXT("Normal_Intensity"), -10.f, 10.f));
	StaticMIParaList.Add(10, FSCTXRParameter(TEXT("法线_重复度_横向"), TEXT("Normal_Map_Tiling_U"), TEXT("Normal_Map_Tiling_U"), 0.f, 100.f));
	StaticMIParaList.Add(11, FSCTXRParameter(TEXT("法线_重复度_纵向"), TEXT("Normal_Map_Tiling_V"), TEXT("Normal_Map_Tiling_V"), 0.f, 100.f));
	StaticMIParaList.Add(12, FSCTXRParameter(TEXT("法线_旋转"), TEXT("Normal_Map_RotateUV"), TEXT("Normal_Map_RotateUV"), 0.f, 1.f));
	StaticMIParaList.Add(13, FSCTXRParameter(TEXT("粗糙"), TEXT("Roughness"), TEXT("Roughness"), 0.f, 1.f));
	StaticMIParaList.Add(14, FSCTXRParameter(TEXT("高光"), TEXT("Specular"), TEXT("Specular"), 0.f, 1.f));
	StaticMIParaList.Add(15, FSCTXRParameter(TEXT("强度"), TEXT("Intensity"), TEXT("Intensity"), 0.f, 1.f));
	StaticMIParaList.Add(16, FSCTXRParameter(TEXT("固有色_偏移_横向"), TEXT("Color_Map_Offset_U"), TEXT("Color_Map_Offset_U"), 0.f, 100.f));
	StaticMIParaList.Add(17, FSCTXRParameter(TEXT("固有色_偏移_纵向"), TEXT("Color_Map_Offset_V"), TEXT("Color_Map_Offset_V"), 0.f, 100.f));
	StaticMIParaList.Add(18, FSCTXRParameter(TEXT("法线_偏移_横向"), TEXT("Normal_Map_Offset_U"), TEXT("Normal_Map_Offset_U"), 0.f, 100.f));
	StaticMIParaList.Add(19, FSCTXRParameter(TEXT("法线_偏移_纵向"), TEXT("Normal_Map_Offset_V"), TEXT("Normal_Map_Offset_V"), 0.f, 100.f));
	//-----------------------------------------------------100
	StaticMIParaList.Add(100, FSCTXRParameter(TEXT("固有色/贴图混色"), TEXT("Diffuse_Color"), TEXT("Diffuse_Color")));
	StaticMIParaList.Add(101, FSCTXRParameter(TEXT("固有色_偏移_横向"), TEXT("Diffuse_UV_OffsetU"), TEXT("Diffuse_UV_OffsetU"), 0.f, 100.f));
	StaticMIParaList.Add(102, FSCTXRParameter(TEXT("固有色_偏移_纵向"), TEXT("Diffuse_UV_OffsetV"), TEXT("Diffuse_UV_OffsetV"), 0.f, 100.f));
	StaticMIParaList.Add(103, FSCTXRParameter(TEXT("固有色_重复度_横向"), TEXT("Diffuse_UV_TilingU"), TEXT("Diffuse_UV_TilingU"), 0.f, 1000.f));
	StaticMIParaList.Add(104, FSCTXRParameter(TEXT("固有色_重复度_纵向"), TEXT("Diffuse_UV_TilingV"), TEXT("Diffuse_UV_TilingV"), 0.f, 1000.f));
	StaticMIParaList.Add(105, FSCTXRParameter(TEXT("固有色_旋转"), TEXT("Diffuse_UV_Rotate"), TEXT("Diffuse_UV_Rotate"), -360.f, 360.f));

	StaticMIParaList.Add(106, FSCTXRParameter(TEXT("环境阴影_强度"), TEXT("AO_intensity"), TEXT("AO_intensity"), 0.f, 1.f));
	StaticMIParaList.Add(107, FSCTXRParameter(TEXT("环境阴影_对比度"), TEXT("AO_Contrast"), TEXT("AO_Contrast"), 0.f, 1.f));

	StaticMIParaList.Add(108, FSCTXRParameter(TEXT("菲尼尔边缘_颜色"), TEXT("Fresnel_Color"), TEXT("Fresnel_Color")));
	StaticMIParaList.Add(109, FSCTXRParameter(TEXT("菲尼尔边缘_指数幂"), TEXT("Fresnel_Power"), TEXT("Fresnel_Power"), 0.f, 5.f));

	StaticMIParaList.Add(110, FSCTXRParameter(TEXT("固有色_色相调整"), TEXT("AdjustColor_HueShift"), TEXT("AdjustColor_HueShift"), -1.f, 1.f));
	StaticMIParaList.Add(111, FSCTXRParameter(TEXT("固有色_鲜艳程度"), TEXT("AdjustColor_Saturaion"), TEXT("AdjustColor_Saturaion"), 0.f, 10.f));

	StaticMIParaList.Add(112, FSCTXRParameter(TEXT("模拟毛绒边缘_颜色"), TEXT("Diffuse_Fuzzy_Color"), TEXT("Diffuse_Fuzzy_Color")));
	StaticMIParaList.Add(113, FSCTXRParameter(TEXT("模拟毛绒边缘_中心压暗"), TEXT("Diffuse_Fuzzy_CoreDarkness"), TEXT("Diffuse_Fuzzy_CoreDarkness"), 0.f, 5.f));
	StaticMIParaList.Add(114, FSCTXRParameter(TEXT("模拟毛绒边缘_边缘增强"), TEXT("Diffuse_Fuzzy_EdgeBrightness"), TEXT("Diffuse_Fuzzy_EdgeBrightness"), 0.f, 5.f));
	StaticMIParaList.Add(115, FSCTXRParameter(TEXT("模拟毛绒边缘_边缘指数幂"), TEXT("Diffuse_Fuzzy_Power"), TEXT("Diffuse_Fuzzy_Power"), 0.f, 5.f));

	StaticMIParaList.Add(116, FSCTXRParameter(TEXT("Diffuse_Rust"), TEXT("Diffuse_Rust"), TEXT("Diffuse_Rust"), 0.f, 1.f));
	StaticMIParaList.Add(117, FSCTXRParameter(TEXT("Diffuse_Silk"), TEXT("Diffuse_Silk"), TEXT("Diffuse_Silk"), 0.f, 1.f));
	StaticMIParaList.Add(118, FSCTXRParameter(TEXT("Diffuse_Silk_OffsetU"), TEXT("Diffuse_Silk_OffsetU"), TEXT("Diffuse_Silk_OffsetU"), 0.f, 100.f));
	StaticMIParaList.Add(119, FSCTXRParameter(TEXT("Diffuse_Silk_OffsetV"), TEXT("Diffuse_Silk_OffsetV"), TEXT("Diffuse_Silk_OffsetV"), 0.f, 100.f));
	StaticMIParaList.Add(120, FSCTXRParameter(TEXT("Diffuse_Silk_TilingU"), TEXT("Diffuse_Silk_TilingU"), TEXT("Diffuse_Silk_TilingU"), 0.f, 1000.f));
	StaticMIParaList.Add(121, FSCTXRParameter(TEXT("Diffuse_Silk_TilingV"), TEXT("Diffuse_Silk_TilingV"), TEXT("Diffuse_Silk_TilingV"), 0.f, 1000.f));
	StaticMIParaList.Add(122, FSCTXRParameter(TEXT("Diffuse_Silk_Rotate"), TEXT("Diffuse_Silk_Rotate"), TEXT("Diffuse_Silk_Rotate"), -360.f, 360.f));

	StaticMIParaList.Add(123, FSCTXRParameter(TEXT("固有色/贴图混色_2"), TEXT("Diffuse_2_Color"), TEXT("Diffuse_2_Color")));
	StaticMIParaList.Add(124, FSCTXRParameter(TEXT("固有色_2_偏移_横向"), TEXT("Diffuse_2_UV_OffsetU"), TEXT("Diffuse_2_UV_OffsetU"), 0.f, 100.f));
	StaticMIParaList.Add(125, FSCTXRParameter(TEXT("固有色_2_偏移_纵向"), TEXT("Diffuse_2_UV_OffsetV"), TEXT("Diffuse_2_UV_OffsetV"), 0.f, 100.f));
	StaticMIParaList.Add(126, FSCTXRParameter(TEXT("固有色_2_重复度_横向"), TEXT("Diffuse_2_UV_TilingU"), TEXT("Diffuse_2_UV_TilingU"), 0.f, 1000.f));
	StaticMIParaList.Add(127, FSCTXRParameter(TEXT("固有色_2_重复度_纵向"), TEXT("Diffuse_2_UV_TilingV"), TEXT("Diffuse_2_UV_TilingV"), 0.f, 1000.f));
	StaticMIParaList.Add(128, FSCTXRParameter(TEXT("固有色_2_旋转"), TEXT("Diffuse_2_UV_Rotate"), TEXT("Diffuse_2_UV_Rotate"), -360.f, 360.f));

	StaticMIParaList.Add(129, FSCTXRParameter(TEXT("固有色_2_色相调整"), TEXT("AdjustColor_2_HueShift"), TEXT("AdjustColor_2_HueShift"), -1.f, 1.f));
	StaticMIParaList.Add(130, FSCTXRParameter(TEXT("固有色_2_鲜艳程度"), TEXT("AdjustColor_2_Saturaion"), TEXT("AdjustColor_2_Saturaion"), 0.f, 10.f));
	StaticMIParaList.Add(131, FSCTXRParameter(TEXT("固有色_明度"), TEXT("AdjustColor_Brightness"), TEXT("AdjustColor_Brightness"), -1.f, 1.f));
	StaticMIParaList.Add(132, FSCTXRParameter(TEXT("固有色_2_明度"), TEXT("AdjustColor_2_Brightness"), TEXT("AdjustColor_2_Brightness"), -1.f, 1.f));
	StaticMIParaList.Add(133, FSCTXRParameter(TEXT("固有色_对比度"), TEXT("AdjustColor_Contrast"), TEXT("AdjustColor_Contrast"), 0.f, 1.f));
	StaticMIParaList.Add(134, FSCTXRParameter(TEXT("固有色_2_对比度"), TEXT("AdjustColor_2_Contrast"), TEXT("AdjustColor_2_Contrast"), 0.f, 1.f));

	StaticMIParaList.Add(135, FSCTXRParameter(TEXT("投射_固有色_偏移_横向"), TEXT("Projective_Diffuse_UV_OffsetU"), TEXT("Projective_Diffuse_UV_OffsetU"), 0.f, 100.f));
	StaticMIParaList.Add(136, FSCTXRParameter(TEXT("投射_固有色_偏移_纵向"), TEXT("Projective_Diffuse_UV_OffsetV"), TEXT("Projective_Diffuse_UV_OffsetV"), 0.f, 100.f));
	StaticMIParaList.Add(137, FSCTXRParameter(TEXT("投射_固有色_重复度_横向"), TEXT("Projective_Diffuse_UV_TilingU"), TEXT("Projective_Diffuse_UV_TilingU"), 0.f, 1000.f));
	StaticMIParaList.Add(138, FSCTXRParameter(TEXT("投射_固有色_重复度_纵向"), TEXT("Projective_Diffuse_UV_TilingV"), TEXT("Projective_Diffuse_UV_TilingV"), 0.f, 1000.f));
	StaticMIParaList.Add(139, FSCTXRParameter(TEXT("投射_固有色_旋转"), TEXT("Projective_Diffuse_UV_Rotate"), TEXT("Projective_Diffuse_UV_Rotate"), -360.f, 360.f));

	//-----------------------------------------------------200
	StaticMIParaList.Add(200, FSCTXRParameter(TEXT("金属"), TEXT("Metallic"), TEXT("Metallic"), 0.f, 1.f));
	StaticMIParaList.Add(201, FSCTXRParameter(TEXT("金属_2"), TEXT("Metallic_2"), TEXT("Metallic_2"), 0.f, 1.f));
	//-----------------------------------------------------300
	StaticMIParaList.Add(300, FSCTXRParameter(TEXT("高光"), TEXT("Specular"), TEXT("Specular"), 0.f, 1.f));
	StaticMIParaList.Add(301, FSCTXRParameter(TEXT("高光_2"), TEXT("Specular_2"), TEXT("Specular_2"), 0.f, 1.f));
	
	StaticMIParaList.Add(302, FSCTXRParameter(TEXT("高光_对比度"), TEXT("Specular_Contrast"), TEXT("Specular_Contrast"), 0.f, 1.f));
	StaticMIParaList.Add(303, FSCTXRParameter(TEXT("高光_偏移_横向"), TEXT("Specular_UV_Offset_U"), TEXT("Specular_UV_Offset_U"), 0.f, 100.f));
	StaticMIParaList.Add(304, FSCTXRParameter(TEXT("高光_偏移_纵向"), TEXT("Specular_UV_Offset_V"), TEXT("Specular_UV_Offset_V"), 0.f, 100.f));
	StaticMIParaList.Add(305, FSCTXRParameter(TEXT("高光_重复度_横向"), TEXT("Specular_UV_Tiling_U"), TEXT("Specular_UV_Tiling_U"), 0.f, 1000.f));
	StaticMIParaList.Add(306, FSCTXRParameter(TEXT("高光_重复度_纵向"), TEXT("Specular_UV_Tiling_V"), TEXT("Specular_UV_Tiling_V"), 0.f, 1000.f));
	StaticMIParaList.Add(307, FSCTXRParameter(TEXT("高光_旋转"), TEXT("Specular_UV_Ratote"), TEXT("Specular_UV_Ratote"), -360.f, 360.f));
	
	StaticMIParaList.Add(308, FSCTXRParameter(TEXT("高光_2_对比度"), TEXT("Specular_2_Contrast"), TEXT("Specular_2_Contrast"), 0.f, 1.f));
	StaticMIParaList.Add(309, FSCTXRParameter(TEXT("高光_2_偏移_横向"), TEXT("Specular_2_UV_Offset_U"), TEXT("Specular_2_UV_Offset_U"), 0.f, 100.f));
	StaticMIParaList.Add(310, FSCTXRParameter(TEXT("高光_2_偏移_纵向"), TEXT("Specular_2_UV_Offset_V"), TEXT("Specular_2_UV_Offset_V"), 0.f, 100.f));
	StaticMIParaList.Add(311, FSCTXRParameter(TEXT("高光_2_重复度_横向"), TEXT("Specular_2_UV_Tiling_U"), TEXT("Specular_2_UV_Tiling_U"), 0.f, 1000.f));
	StaticMIParaList.Add(312, FSCTXRParameter(TEXT("高光_2_重复度_纵向"), TEXT("Specular_2_UV_Tiling_V"), TEXT("Specular_2_UV_Tiling_V"), 0.f, 1000.f));
	StaticMIParaList.Add(313, FSCTXRParameter(TEXT("高光_2_旋转"), TEXT("Specular_2_UV_Rotate"), TEXT("Specular_2_UV_Rotate"), -360.f, 360.f));
	
	StaticMIParaList.Add(314, FSCTXRParameter(TEXT("投射_高光_偏移_横向"), TEXT("Projective_Specular_UV_Offset_U"), TEXT("Projective_Specular_UV_Offset_U"), 0.f, 100.f));
	StaticMIParaList.Add(315, FSCTXRParameter(TEXT("投射_高光_偏移_纵向"), TEXT("Projective_Specular_UV_Offset_V"), TEXT("Projective_Specular_UV_Offset_V"), 0.f, 100.f));
	StaticMIParaList.Add(316, FSCTXRParameter(TEXT("投射_高光_重复度_横向"), TEXT("Projective_Specular_UV_Tiling_U"), TEXT("Projective_Specular_UV_Tiling_U"), 0.f, 1000.f));
	StaticMIParaList.Add(317, FSCTXRParameter(TEXT("投射_高光_重复度_纵向"), TEXT("Projective_Specular_UV_Tiling_V"), TEXT("Projective_Specular_UV_Tiling_V"), 0.f, 1000.f));
	StaticMIParaList.Add(318, FSCTXRParameter(TEXT("投射_高光_旋转"), TEXT("Projective_Specular_UV_Ratote"), TEXT("Projective_Specular_UV_Ratote"), -360.f, 360.f));
	//-----------------------------------------------------400
	StaticMIParaList.Add(400, FSCTXRParameter(TEXT("粗糙度"), TEXT("Roughness"), TEXT("Roughness"), 0.f, 1.f));
	StaticMIParaList.Add(401, FSCTXRParameter(TEXT("Roughness_ScurbMetal"), TEXT("Roughness_ScurbMetal"), TEXT("Roughness_ScurbMetal"), 0.f, 5.f));
	StaticMIParaList.Add(402, FSCTXRParameter(TEXT("Roughness_ScurbMetal_Texture"), TEXT("Roughness_ScurbMetal_Texture"), TEXT("Roughness_ScurbMetal_Texture"), 0.f, 1.f));
	StaticMIParaList.Add(403, FSCTXRParameter(TEXT("最大粗糙度"), TEXT("Roughness_Min"), TEXT("Roughness_Min"), 0.f, 1.f));
	StaticMIParaList.Add(404, FSCTXRParameter(TEXT("最小粗糙度"), TEXT("Roughness_Max"), TEXT("Roughness_Max"), 0.f, 1.f));
	StaticMIParaList.Add(405, FSCTXRParameter(TEXT("粗糙度_2"), TEXT("Roughness_2"), TEXT("Roughness_2"), 0.f, 1.f));

	//StaticMIParaList.Add(406, FSCTXRParameter(TEXT("粗糙度_对比度"), TEXT("Roughness_Contrast"), TEXT("Roughness_Contrast"), 0.f, 1.f));
	//StaticMIParaList.Add(407, FSCTXRParameter(TEXT("粗糙度_偏移_横向"), TEXT("Roughness_UV_Offset_U"), TEXT("Roughness_UV_Offset_U"), 0.f, 100.f));
	//StaticMIParaList.Add(408, FSCTXRParameter(TEXT("粗糙度_偏移_纵向"), TEXT("Roughness_UV_Offset_V"), TEXT("Roughness_UV_Offset_V"), 0.f, 100.f));
	//StaticMIParaList.Add(409, FSCTXRParameter(TEXT("粗糙度_重复度_横向"), TEXT("Roughness_UV_Tiling_U"), TEXT("Roughness_UV_Tiling_U"), 0.f, 100.f));
	//StaticMIParaList.Add(410, FSCTXRParameter(TEXT("粗糙度_重复度_纵向"), TEXT("Roughness_UV_Tiling_V"), TEXT("Roughness_UV_Tiling_V"), 0.f, 100.f));
	//StaticMIParaList.Add(411, FSCTXRParameter(TEXT("粗糙度_旋转"), TEXT("Roughness_UV_Ratote"), TEXT("Roughness_UV_Ratote"), -360.f, 360.f));

	//StaticMIParaList.Add(412, FSCTXRParameter(TEXT("粗糙度_2_对比度"), TEXT("Roughness_2_Contrast"), TEXT("Roughness_2_Contrast"), 0.f, 1.f));
	//StaticMIParaList.Add(413, FSCTXRParameter(TEXT("粗糙度_2_偏移_横向"), TEXT("Roughness_2_UV_Offset_U"), TEXT("Roughness_2_UV_Offset_U"), 0.f, 100.f));
	//StaticMIParaList.Add(414, FSCTXRParameter(TEXT("粗糙度_2_偏移_纵向"), TEXT("Roughness_2_UV_Offset_V"), TEXT("Roughness_2_UV_Offset_V"), 0.f, 100.f));
	//StaticMIParaList.Add(415, FSCTXRParameter(TEXT("粗糙度_2_重复度_横向"), TEXT("Roughness_2_UV_Tiling_U"), TEXT("Roughness_2_UV_Tiling_U"), 0.f, 1000.f));
	//StaticMIParaList.Add(416, FSCTXRParameter(TEXT("粗糙度_2_重复度_纵向"), TEXT("Roughness_2_UV_Tiling_V"), TEXT("Roughness_2_UV_Tiling_V"), 0.f, 1000.f));
	//StaticMIParaList.Add(417, FSCTXRParameter(TEXT("粗糙度_2_旋转"), TEXT("Roughness_2_UV_Rotate"), TEXT("Roughness_2_UV_Rotate"), -360.f, 360.f));

	//StaticMIParaList.Add(418, FSCTXRParameter(TEXT("投射_粗糙度_偏移_横向"), TEXT("Projective_Roughness_UV_Offset_U"), TEXT("Projective_Roughness_UV_Offset_U"), 0.f, 100.f));
	//StaticMIParaList.Add(419, FSCTXRParameter(TEXT("投射_粗糙度_偏移_纵向"), TEXT("Projective_Roughness_UV_Offset_V"), TEXT("Projective_Roughness_UV_Offset_V"), 0.f, 100.f));
	//StaticMIParaList.Add(420, FSCTXRParameter(TEXT("投射_粗糙度_重复度_横向"), TEXT("Projective_Roughness_UV_Tiling_U"), TEXT("Projective_Roughness_UV_Tiling_U"), 0.f, 1000.f));
	//StaticMIParaList.Add(421, FSCTXRParameter(TEXT("投射_粗糙度_重复度_纵向"), TEXT("Projective_Roughness_UV_Tiling_V"), TEXT("Projective_Roughness_UV_Tiling_V"), 0.f, 1000.f));
	//StaticMIParaList.Add(422, FSCTXRParameter(TEXT("投射_粗糙度_旋转"), TEXT("Projective_Roughness_UV_Ratote"), TEXT("Projective_Roughness_UV_Ratote"), -360.f, 360.f));

	//-----------------------------------------------------500
	StaticMIParaList.Add(500, FSCTXRParameter(TEXT("自发光"), TEXT("Emissive_Color"), TEXT("Emissive_Color"), 0.f, 1.f));
	StaticMIParaList.Add(501, FSCTXRParameter(TEXT("焦散_着色"), TEXT("Caustic_ColorTint"), TEXT("Caustic_ColorTint")));
	StaticMIParaList.Add(502, FSCTXRParameter(TEXT("焦散_强度"), TEXT("Caustic_Intensity"), TEXT("Caustic_Intensity"), 0.f, 200000.f));
	StaticMIParaList.Add(503, FSCTXRParameter(TEXT("焦散_颜色指数幂"), TEXT("Caustic_Power"), TEXT("Caustic_Power"), 1.f, 5.f));
	StaticMIParaList.Add(504, FSCTXRParameter(TEXT("焦散_尺寸"), TEXT("Caustic_Size"), TEXT("Caustic_Size"), 0.f, 1000.f));
	StaticMIParaList.Add(505, FSCTXRParameter(TEXT("焦散_长宽比"), TEXT("Caustic_UVratio"), TEXT("Caustic_UVratio"), 0.f, 5000.f));
	StaticMIParaList.Add(506, FSCTXRParameter(TEXT("焦散_彩度"), TEXT("Caustic_Chrome"), TEXT("Caustic_Chrome"), 0.f, 1.f));
	StaticMIParaList.Add(507, FSCTXRParameter(TEXT("焦散_扰乱_尺寸"), TEXT("Caustic_Disdort_Size"), TEXT("Caustic_Disdort_Size"), 0.f, 10.f));
	StaticMIParaList.Add(508, FSCTXRParameter(TEXT("焦散_扰乱_速度"), TEXT("Caustic_Disdort_Speed"), TEXT("Caustic_Disdort_Speed"), 0.f, 1.f));

	StaticMIParaList.Add(509, FSCTXRParameter(TEXT("自发光_遮罩_重复度_纵向"), TEXT("EmissiveMask_UV_TilingU"), TEXT("EmissiveMask_UV_TilingU"), 0.f, 1000.f));
	StaticMIParaList.Add(510, FSCTXRParameter(TEXT("自发光_遮罩_重复度_横向"), TEXT("EmissiveMask_UV_TilingV"), TEXT("EmissiveMask_UV_TilingV"), 0.f, 1000.f));
	StaticMIParaList.Add(511, FSCTXRParameter(TEXT("自发光_遮罩_偏移_横向"), TEXT("EmissiveMask_UV_OffsetU"), TEXT("EmissiveMask_UV_OffsetU"), 0.f, 100.f));
	StaticMIParaList.Add(512, FSCTXRParameter(TEXT("自发光_遮罩_偏移_纵向"), TEXT("EmissiveMask_UV_OffsetV"), TEXT("EmissiveMask_UV_OffsetV"), 0.f, 100.f));
	StaticMIParaList.Add(513, FSCTXRParameter(TEXT("自发光_遮罩_旋转"), TEXT("EmissiveMask_UV_Rotate"), TEXT("EmissiveMask_UV_Rotate"), -360.f, 360.f));
	StaticMIParaList.Add(514, FSCTXRParameter(TEXT("自发光_遮罩_反相"), TEXT("EmissiveMask_Invert"), TEXT("EmissiveMask_Invert"), 0.f, 1.f));
	StaticMIParaList.Add(515, FSCTXRParameter(TEXT("自发光_遮罩_对比度"), TEXT("EmissiveMask_Contrast"), TEXT("EmissiveMask_Contrast"), 0.f, 10.f));
	StaticMIParaList.Add(516, FSCTXRParameter(TEXT("自发光_遮罩_偏移"), TEXT("EmissiveMask_Offset"), TEXT("EmissiveMask_Offset"), -1.f, 1.f));

	//-----------------------------------------------------600
	StaticMIParaList.Add(600, FSCTXRParameter(TEXT("法线_强度"), TEXT("Normal_Strength"), TEXT("Normal_Strength"), -5.f, 5.f));
	StaticMIParaList.Add(601, FSCTXRParameter(TEXT("法线_偏移_横向"), TEXT("NormalMap_UV_Offset_U"), TEXT("NormalMap_UV_Offset_U"), 0.f, 100.f));
	StaticMIParaList.Add(602, FSCTXRParameter(TEXT("法线_偏移_纵向"), TEXT("NormalMap_UV_Offset_V"), TEXT("NormalMap_UV_Offset_V"), 0.f, 100.f));
	StaticMIParaList.Add(603, FSCTXRParameter(TEXT("法线_重复度_横向"), TEXT("NormalMap_UV_Tiling_U"), TEXT("NormalMap_UV_Tiling_U"), 0.f, 1000.f));
	StaticMIParaList.Add(604, FSCTXRParameter(TEXT("法线_重复度_纵向"), TEXT("NormalMap_UV_Tiling_V"), TEXT("NormalMap_UV_Tiling_V"), 0.f, 1000.f));
	StaticMIParaList.Add(605, FSCTXRParameter(TEXT("法线_旋转"), TEXT("NormalMap_UV_Rotate"), TEXT("NormalMap_UV_Ratote"), -360.f, 360.f));
	StaticMIParaList.Add(606, FSCTXRParameter(TEXT("细节法线_强度"), TEXT("NormalDetail_Strength"), TEXT("NormalDetail_Strength"), -5.f, 5.f));
	StaticMIParaList.Add(607, FSCTXRParameter(TEXT("细节法线_偏移_横向"), TEXT("NormalDetial_UV_Offset_U"), TEXT("NormalDetial_UV_Offset_U"), 0.f, 100.f));
	StaticMIParaList.Add(608, FSCTXRParameter(TEXT("细节法线_偏移_纵向"), TEXT("NormalDetial_UV_Offset_V"), TEXT("NormalDetial_UV_Offset_V"), 0.f, 100.f));
	StaticMIParaList.Add(609, FSCTXRParameter(TEXT("细节法线_重复度_横向"), TEXT("NormalDetial_UV_Tiling_U"), TEXT("NormalDetial_UV_Tiling_U"), 0.f, 1000.f));
	StaticMIParaList.Add(610, FSCTXRParameter(TEXT("细节法线_重复度_纵向"), TEXT("NormalDetial_UV_Tiling_V"), TEXT("NormalDetial_UV_Tiling_V"), 0.f, 1000.f));
	StaticMIParaList.Add(611, FSCTXRParameter(TEXT("细节法线_旋转"), TEXT("NormalDetial_UV_Rotate"), TEXT("NormalDetial_UV_Ratote"), -360.f, 360.f));

	StaticMIParaList.Add(612, FSCTXRParameter(TEXT("法线_强度_2"), TEXT("Normal_2_Strength"), TEXT("Normal_2_Strength"), -5.f, 5.f));
	StaticMIParaList.Add(613, FSCTXRParameter(TEXT("法线_偏移_横向_2"), TEXT("NormalMap_2_UV_Offset_U"), TEXT("NormalMap_2_UV_Offset_U"), 0.f, 100.f));
	StaticMIParaList.Add(614, FSCTXRParameter(TEXT("法线_偏移_纵向_2"), TEXT("NormalMap_2_UV_Offset_V"), TEXT("NormalMap_2_UV_Offset_V"), 0.f, 100.f));
	StaticMIParaList.Add(615, FSCTXRParameter(TEXT("法线_重复度_横向_2"), TEXT("NormalMap_2_UV_Tiling_U"), TEXT("NormalMap_2_UV_Tiling_U"), 0.f, 1000.f));
	StaticMIParaList.Add(616, FSCTXRParameter(TEXT("法线_重复度_纵向_2"), TEXT("NormalMap_2_UV_Tiling_V"), TEXT("NormalMap_2_UV_Tiling_V"), 0.f, 1000.f));
	StaticMIParaList.Add(617, FSCTXRParameter(TEXT("法线_旋转_2"), TEXT("NormalMap_2_UV_Ratote"), TEXT("NormalMap_2_UV_Ratote"), -360.f, 360.f));

	StaticMIParaList.Add(618, FSCTXRParameter(TEXT("投射_法线_偏移_横向"), TEXT("Projective_NormalMap_UV_Offset_U"), TEXT("Projective_NormalMap_UV_Offset_U"), 0.f, 100.f));
	StaticMIParaList.Add(619, FSCTXRParameter(TEXT("投射_法线_偏移_纵向"), TEXT("Projective_NormalMap_UV_Offset_V"), TEXT("Projective_NormalMap_UV_Offset_V"), 0.f, 100.f));
	StaticMIParaList.Add(620, FSCTXRParameter(TEXT("投射_法线_重复度_横向"), TEXT("Projective_NormalMap_UV_Tiling_U"), TEXT("Projective_NormalMap_UV_Tiling_U"), 0.f, 1000.f));
	StaticMIParaList.Add(621, FSCTXRParameter(TEXT("投射_法线_重复度_纵向"), TEXT("Projective_NormalMap_UV_Tiling_V"), TEXT("Projective_NormalMap_UV_Tiling_V"), 0.f, 1000.f));
	StaticMIParaList.Add(622, FSCTXRParameter(TEXT("投射_法线_旋转"), TEXT("Projective_NormalMap_UV_Ratote"), TEXT("Projective_NormalMap_UV_Ratote"), -360.f, 360.f));

	//---------------------------------------------------700
	StaticMIParaList.Add(700, FSCTXRParameter(TEXT("透明度"), TEXT("Opacity"), TEXT("Opacity"), 0.f, 1.f));
	StaticMIParaList.Add(701, FSCTXRParameter(TEXT("透明影响模糊"), TEXT("Mask_AffectBlur"), TEXT("Mask_AffectBlur"), 0.f, 1.f));
	StaticMIParaList.Add(702, FSCTXRParameter(TEXT("透明影响反射率"), TEXT("Mask_AffectRefraction"), TEXT("Mask_AffectRefraction"), 0.f, 1.f));
	StaticMIParaList.Add(703, FSCTXRParameter(TEXT("Hollow-out_Style"), TEXT("Hollow-out_Style"), TEXT("Hollow-out_Style"), 0.f, 5.f));
	StaticMIParaList.Add(704, FSCTXRParameter(TEXT("遮罩_法线强度"), TEXT("Hollow-out_NormalStrength"), TEXT("Hollow-out_NormalStrength"), -5.f, 5.f));
	StaticMIParaList.Add(705, FSCTXRParameter(TEXT("遮罩_偏移_横向"), TEXT("Hollow-out_UV_OffsetU"), TEXT("Hollow-out_UV_OffsetU"), 0.f, 100.f));
	StaticMIParaList.Add(706, FSCTXRParameter(TEXT("遮罩_偏移_纵向"), TEXT("Hollow-out_UV_OffsetV"), TEXT("Hollow-out_UV_OffsetV"), 0.f, 100.f));
	StaticMIParaList.Add(707, FSCTXRParameter(TEXT("遮罩_旋转"), TEXT("Hollow-out_UV_Rotate"), TEXT("Hollow-out_UV_Rotate"), -360.f, 360.f));
	StaticMIParaList.Add(708, FSCTXRParameter(TEXT("遮罩_重复度_纵向"), TEXT("Hollow-out_UV_TilingU"), TEXT("Hollow-out_UV_TilingU"), 0.f, 1000.f));
	StaticMIParaList.Add(709, FSCTXRParameter(TEXT("遮罩_重复度_横向"), TEXT("Hollow-out_UV_TilingV"), TEXT("Hollow-out_UV_TilingV"), 0.f, 1000.f));
	StaticMIParaList.Add(710, FSCTXRParameter(TEXT("次表面散射_透光率"), TEXT("SSS_Opacity"), TEXT("SSS_Opacity"), 0.f, 1.f));
	StaticMIParaList.Add(711, FSCTXRParameter(TEXT("次表面散射_内部材质颜色"), TEXT("SSS_Color"), TEXT("SSS_Color")));
	StaticMIParaList.Add(712, FSCTXRParameter(TEXT("次表面散射_固有色混色"), TEXT("SSS_Color_BlendWithDiffuse"), TEXT("SSS_Color_BlendWithDiffuse"), 0.f, 1.f));
	StaticMIParaList.Add(713, FSCTXRParameter(TEXT("透明度_指数幂"), TEXT("Opacity_Power"), TEXT("Opacity_Power"), 0.f, 5.f));
	StaticMIParaList.Add(714, FSCTXRParameter(TEXT("最小透明度"), TEXT("Opacity_Min"), TEXT("Opacity_Min"), 0.f, 1.f));
	StaticMIParaList.Add(715, FSCTXRParameter(TEXT("最大透明度"), TEXT("Opacity_Max"), TEXT("Opacity_Max"), 0.f, 1.f));
	StaticMIParaList.Add(716, FSCTXRParameter(TEXT("透明度贴图_黑色透明度"), TEXT("Mask_Map_Color1"), TEXT("Mask_Map_Color1"), 0.f, 1.f));
	StaticMIParaList.Add(717, FSCTXRParameter(TEXT("透明度贴图_白色透明度"), TEXT("Mask_Map_Color2"), TEXT("Mask_Map_Color2"), 0.f, 1.f));
	StaticMIParaList.Add(718, FSCTXRParameter(TEXT("深度衰减"), TEXT("Opacity_FadeDistance"), TEXT("Opacity_FadeDistance"), 0.f, 1000.f));
	StaticMIParaList.Add(719, FSCTXRParameter(TEXT("遮罩_相反"), TEXT("Mask_Invert"), TEXT("Mask_Invert"), 0.f, 1.f));
	StaticMIParaList.Add(720, FSCTXRParameter(TEXT("遮罩_偏移"), TEXT("Mask_Offset"), TEXT("Mask_Offset"), -1.f, 1.f));
	StaticMIParaList.Add(721, FSCTXRParameter(TEXT("遮罩_对比度"), TEXT("Mask_Contrast"), TEXT("Mask_Contrast"), 0.f, 10.f));

	StaticMIParaList.Add(722, FSCTXRParameter(TEXT("透明度_重复度_纵向"), TEXT("Opacity_UV_TilingU"), TEXT("Opacity_UV_TilingU"), 0.f, 1000.f));
	StaticMIParaList.Add(723, FSCTXRParameter(TEXT("透明度_重复度_横向"), TEXT("Opacity_UV_TilingV"), TEXT("Opacity_UV_TilingV"), 0.f, 1000.f));
	StaticMIParaList.Add(724, FSCTXRParameter(TEXT("透明度_偏移_横向"), TEXT("Opacity_UV_OffsetU"), TEXT("Opacity_UV_OffsetU"), 0.f, 100.f));
	StaticMIParaList.Add(725, FSCTXRParameter(TEXT("透明度_偏移_纵向"), TEXT("Opacity_UV_OffsetV"), TEXT("Opacity_UV_OffsetV"), 0.f, 100.f));
	StaticMIParaList.Add(726, FSCTXRParameter(TEXT("透明度_旋转"), TEXT("Opacity_UV_Rotate"), TEXT("Opacity_UV_Rotate"), -360.f, 360.f));
	StaticMIParaList.Add(727, FSCTXRParameter(TEXT("透明度_反相"), TEXT("Opacity_Invert"), TEXT("Opacity_Invert"), 0.f, 1.f));
	StaticMIParaList.Add(728, FSCTXRParameter(TEXT("透明度_对比度"), TEXT("Opacity_Contrast"), TEXT("Opacity_Contrast"), 0.f, 10.f));
	StaticMIParaList.Add(729, FSCTXRParameter(TEXT("透明度_偏移"), TEXT("Opacity_Offset"), TEXT("Opacity_Offset"), -1.f, 1.f));

	StaticMIParaList.Add(730, FSCTXRParameter(TEXT("层遮罩_重复度_纵向"), TEXT("LayerMask_UV_TilingU"), TEXT("LayerMask_UV_TilingU"), 0.f, 1000.f));
	StaticMIParaList.Add(731, FSCTXRParameter(TEXT("层遮罩_重复度_横向"), TEXT("LayerMask_UV_TilingV"), TEXT("LayerMask_UV_TilingV"), 0.f, 1000.f));
	StaticMIParaList.Add(732, FSCTXRParameter(TEXT("层遮罩_偏移_横向"), TEXT("LayerMask_UV_OffsetU"), TEXT("LayerMask_UV_OffsetU"), 0.f, 100.f));
	StaticMIParaList.Add(733, FSCTXRParameter(TEXT("层遮罩_偏移_纵向"), TEXT("LayerMask_UV_OffsetV"), TEXT("LayerMask_UV_OffsetV"), 0.f, 100.f));
	StaticMIParaList.Add(734, FSCTXRParameter(TEXT("层遮罩_旋转"), TEXT("LayerMask_UV_Rotate"), TEXT("LayerMask_UV_Rotate"), -360.f, 360.f));
	StaticMIParaList.Add(735, FSCTXRParameter(TEXT("层遮罩_反相"), TEXT("LayerMask_Invert"), TEXT("LayerMask_Invert"), 0.f, 1.f));
	StaticMIParaList.Add(736, FSCTXRParameter(TEXT("层遮罩_对比度"), TEXT("LayerMask_Contrast"), TEXT("LayerMask_Contrast"), 0.f, 10.f));
	StaticMIParaList.Add(737, FSCTXRParameter(TEXT("层遮罩_偏移"), TEXT("LayerMask_Offset"), TEXT("LayerMask_Offset"), -1.f, 1.f));

	//-----------------------------------------------------800
	StaticMIParaList.Add(800, FSCTXRParameter(TEXT("折射率"), TEXT("Refraction_Bias"), TEXT("Refraction_Bias"), -1.f, 1.f));
	StaticMIParaList.Add(801, FSCTXRParameter(TEXT("RefractionMap_Strength"), TEXT("RefractionMap_Strength"), TEXT("RefractionMap_Strength"), 0.f, 1.f));
	StaticMIParaList.Add(802, FSCTXRParameter(TEXT("RefractionMap_Repeat"), TEXT("RefractionMap_Repeat"), TEXT("RefractionMap_Repeat"), 0.f, 1000.f));
	StaticMIParaList.Add(803, FSCTXRParameter(TEXT("RefractionMap_UVratio"), TEXT("RefractionMap_UVratio"), TEXT("RefractionMap_UVratio"), 0.f, 1000.f));
	//-----------------------------------------------------900
	StaticMIParaList.Add(900, FSCTXRParameter(TEXT("模糊度"), TEXT("Blur_Distance"), TEXT("Blur_Distance"), 0.f, 1.f));
	StaticMIParaList.Add(901, FSCTXRParameter(TEXT("速度"), TEXT("Speed"), TEXT("Speed"), 0.f, 10.f));
	StaticMIParaList.Add(902, FSCTXRParameter(TEXT("位置偏移"), TEXT("PositionOffset"), TEXT("PositionOffset"), 0.f, 10.f));
	StaticMIParaList.Add(903, FSCTXRParameter(TEXT("AO影响偏移"), TEXT("AO_AffectOffset"), TEXT("AO_AffectOffset"), 0.f, 1.f));
	StaticMIParaList.Add(904, FSCTXRParameter(TEXT("偏移密度"), TEXT("OffsetDensity"), TEXT("OffsetDensity"), 1.f, 256.f));

	for (auto& It : StaticMIParaList)
	{
		It.Value.ID = It.Key;
	}
	//模型属性参数列表
	//-----------------------------------------------------
	StaticModelParaList.Add(0, FSCTXRParameter(TEXT("点光源投影"), TEXT("PointLightCastShadow"), TEXT("PointLightCastShadow"), false));
	StaticModelParaList.Add(1, FSCTXRParameter(TEXT("点光源亮度"), TEXT("PointLightIntensity"), TEXT("PointLightIntensity"), 0.f, 20000.f));
	StaticModelParaList.Add(2, FSCTXRParameter(TEXT("点光源半径"), TEXT("PointLightRadius"), TEXT("PointLightRadius"), 0.f, 1000.f));
	StaticModelParaList.Add(3, FSCTXRParameter(TEXT("点光源颜色"), TEXT("PointLightColor"), TEXT("PointLightColor")));
	StaticModelParaList.Add(4, FSCTXRParameter(TEXT("射灯投影"), TEXT("SpotLightCastShadow"), TEXT("SpotLightCastShadow"), false));
	StaticModelParaList.Add(5, FSCTXRParameter(TEXT("射灯亮度"), TEXT("SpotLightIntensity"), TEXT("SpotLightIntensity"), 0.f, 20000.f));
	StaticModelParaList.Add(6, FSCTXRParameter(TEXT("射灯半径"), TEXT("SpotLightRadius"), TEXT("SpotLightRadius"), 0.f, 1000.f));
	StaticModelParaList.Add(7, FSCTXRParameter(TEXT("射灯内角"), TEXT("SpotLightInnerAngle"), TEXT("SpotLightInnerAngle"), 0.f, 90.f));
	StaticModelParaList.Add(8, FSCTXRParameter(TEXT("射灯外角"), TEXT("SpotLightOutterAngle"), TEXT("SpotLightOutterAngle"), 0.f, 90.f));
	StaticModelParaList.Add(9, FSCTXRParameter(TEXT("射灯颜色"), TEXT("SpotLightColor"), TEXT("SpotLightColor")));
	StaticModelParaList.Add(10, FSCTXRParameter(TEXT("射灯IES"), TEXT("SpotLightIES"), TEXT("SpotLightIES"), 0.f, 1.f));

	for (auto& It : StaticModelParaList)
	{
		It.Value.ID = It.Key;
	}
	//Common9 V7版新材质系统信息列表 材质球资源名称，中文显示名称，暴露的参数ID
	//-----------------------------------------------------
	//基础材质
	StaticCommonMaterialList.Add(FSCTCommonMaterialData("V7_Base_Plastic_MT", TEXT("壁纸（塑料）"), TArray<int32>({ 100,101,102,103,104,105, 106,107,110,111,131,133, 200,300,302,303,304,305,306,307, 400,406,407,408,409,410,411, 600,601,602,603,604,605 })));
	StaticCommonMaterialList.Add(FSCTCommonMaterialData("V7_Base_Ceramics_MT", TEXT("瓷砖（石材）"), TArray<int32>({ 100,101,102,103,104,105, 106,107,110,111,131,133, 200,300,302,303,304,305,306,307, 400,406,407,408,409,410,411, 600,601,602,603,604,605 })));
	StaticCommonMaterialList.Add(FSCTCommonMaterialData("V7_Base_Wood_MT", TEXT("地板（木质）"), TArray<int32>({ 100,101,102,103,104,105, 106,107,110,111,131,133, 200,300,302,303,304,305,306,307, 400,406,407,408,409,410,411, 600,601,602,603,604,605 })));
	StaticCommonMaterialList.Add(FSCTCommonMaterialData("V7_Base_Metal_MT", TEXT("金属材质"), TArray<int32>({ 100,101,102,103,104,105, 106,107,110,111,131,133, 200,300,302,303,304,305,306,307, 400,406,407,408,409,410,411, 600,601,602,603,604,605 })));
	StaticCommonMaterialList.Add(FSCTCommonMaterialData("V7_Base_Mirror_MT", TEXT("镜面材质"), TArray<int32>({ 100,101,102,103,104,105, 106,107,110,111,131,133, 200,300,302,303,304,305,306,307, 400,406,407,408,409,410,411, 600,601,602,603,604,605 })));
	StaticCommonMaterialList.Add(FSCTCommonMaterialData("V7_Base_Soil_MT", TEXT("泥土材质"), TArray<int32>({ 100,101,102,103,104,105, 106,107,110,111,131,133, 200,300,302,303,304,305,306,307, 400,406,407,408,409,410,411, 600,601,602,603,604,605 })));

	StaticCommonMaterialList.Add(FSCTCommonMaterialData("V7_Base_Emissive_MT", TEXT("发光材质"), TArray<int32>({ 100,101,102,103,104,105, 106,107,110,111,131,133, 200,300,302,303,304,305,306,307, 400,406,407,408,409,410,411, 500,509,510,511,512,513,514,515,516, 600,601,602,603,604,605 })));

	//布料材质
	StaticCommonMaterialList.Add(FSCTCommonMaterialData("V7_Cloth_Simple_MT", TEXT("布料材质"), TArray<int32>({ 100,101,102,103,104,105, 106,107,110,111,131,133, 112,113,114,115, 200,300,302,303,304,305,306,307, 400,406,407,408,409,410,411, 600,601,602,603,604,605 })));
	StaticCommonMaterialList.Add(FSCTCommonMaterialData("V7_Cloth_Leather_MT", TEXT("皮革材质"), TArray<int32>({ 100,101,102,103,104,105, 106,107,110,111,131,133, 112,113,114,115, 200,300,302,303,304,305,306,307, 400,406,407,408,409,410,411, 600,601,602,603,604,605 })));
	StaticCommonMaterialList.Add(FSCTCommonMaterialData("V7_Cloth_Slik_MT", TEXT("丝绸材质"), TArray<int32>({ 100,101,102,103,104,105, 106,107,110,111,131,133, 112,113,114,115, 200,300,302,303,304,305,306,307, 400,406,407,408,409,410,411, 600,601,602,603,604,605 })));

	StaticCommonMaterialList.Add(FSCTCommonMaterialData("V7_SSS_MT", TEXT("玉（透光）"), TArray<int32>({ 100,101,102,103,104,105, 106,107,110,111,131,133, 200,300,302,303,304,305,306,307, 400,406,407,408,409,410,411, 500,509,510,511,512,513,514,515,516, 600,601,602,603,604,605, 700,711,712 })));
	StaticCommonMaterialList.Add(FSCTCommonMaterialData("V7_SSS_Plant_MT", TEXT("植物材质"), TArray<int32>({ 100,101,102,103,104,105, 106,107,110,111,131,133, 200,300,302,303,304,305,306,307, 400,406,407,408,409,410,411, 500,509,510,511,512,513,514,515,516, 600,601,602,603,604,605, 700,711,712, 901,902,904 })));
	StaticCommonMaterialList.Add(FSCTCommonMaterialData("V7_Layer_MT", TEXT("双层材质"), TArray<int32>({ 100,101,102,103,104,105, 123,124,125,126,127,128, 106,107,110,111,131,133, 129,130,132,134, 200,201,300,301,302,303,304,305,306,307,308,309,310,311,312,313, 400,406,407,408,409,410,411, 405,412,413,414,415,416,417, 600,601,602,603,604,605, 612,613,614,615,616,617, 730,731,732,733,734,735,736,737 })));
	StaticCommonMaterialList.Add(FSCTCommonMaterialData("V7_Glass_Simple_MT", TEXT("玻璃材质"), TArray<int32>({ 100,101,102,103,104,105, 106,107,110,111,131,133, 112,113,114,115, 200,300,302,303,304,305,306,307, 400,406,407,408,409,410,411, 600,601,602,603,604,605, 700,722,723,724,725,726,727,728,729, 800 })));
	StaticCommonMaterialList.Add(FSCTCommonMaterialData("V7_Glass_Caustic_MT", TEXT("水晶材质"), TArray<int32>({ 100,101,102,103,104,105, 106,107,110,111,131,133, 112,113,114,115, 200,300, 400, 501,502,503,504, 600,601,602,603,604,605, 700,722,723,724,725,726,727,728,729, 800 })));
	StaticCommonMaterialList.Add(FSCTCommonMaterialData("V7_Glass_Blur_MT", TEXT("磨砂玻璃材质"), TArray<int32>({ 100,101,102,103,104,105, 106,107,110,111,131,133, 112,113,114,115, 200,300, 400,500, 600,601,602,603,604,605, 700,722,723,724,725,726,727,728,729, 800, 900 })));
	StaticCommonMaterialList.Add(FSCTCommonMaterialData("V7_Yarn_MT", TEXT("纱材质"), TArray<int32>({ 100,101,102,103,104,105, 106,107,110,111,131,133, 112,113,114,115, 200,300,302,303,304,305,306,307, 400,406,407,408,409,410,411, 600,601,602,603,604,605, 700,722,723,724,725,726,727,728,729, 713,714,715 })));
	StaticCommonMaterialList.Add(FSCTCommonMaterialData("V7_Shadow_MT", TEXT("阴影材质"), TArray<int32>({ 100,500,729 })));

	StaticCommonMaterialList.Add(FSCTCommonMaterialData("V7_Base_WallPaper_MT", TEXT("壁纸（纸质）"), TArray<int32>({ 100,101,102,103,104,105, 106,107,110,111,131,133, 200,300, 400,406,407,408,409,410,411, 600,601,602,603,604,605 })));	

	LoadCommonData();
}

void USCTXRResourceManager::BeginDestroy()
{
	Super::BeginDestroy();
	//BeginReleaseResource(SharedThumbnailRT);
	//FlushRenderingCommands();
	//delete SharedThumbnailRT;
}

FString USCTXRResourceManager::GetDebugInfo()
{
	FString DebugStr;
	DebugStr.Append(FString::Printf(TEXT("信息缓存：%d个"), SynList.Num()));

	for (int32 i = 0; i < SynList.Num(); i++)
		DebugStr.Append(FString::Printf(TEXT("\r%d- ID:%d Name:%s"), i, SynList[i]->ID, *SynList[i]->Name));

	return DebugStr;
}

ESCTResourceType USCTXRResourceManager::GetFileType(FString _PakName)
{
	if (_PakName.StartsWith(TEXT("H_"), ESearchCase::IgnoreCase))
		return ESCTResourceType::CookedHome;
	else if (_PakName.StartsWith(TEXT("M_"), ESearchCase::IgnoreCase))
		return ESCTResourceType::MoveableMesh;
	else if (_PakName.StartsWith(TEXT("W_"), ESearchCase::IgnoreCase))
		return ESCTResourceType::Material;
	else if (_PakName.StartsWith(TEXT("A_"), ESearchCase::IgnoreCase))
		return ESCTResourceType::BlueprintClass;
	else if (_PakName.StartsWith(TEXT("HP_"), ESearchCase::IgnoreCase))
		return ESCTResourceType::LevelPlan;
	else if (_PakName.StartsWith(TEXT("GP_"), ESearchCase::IgnoreCase))
		return ESCTResourceType::GroupPlan;

	return ESCTResourceType::None;
}

FVector USCTXRResourceManager::GetVecFromColor(FLinearColor _Color)
{
	FVector Vec;
	Vec.X = _Color.R;
	Vec.Y = _Color.G;
	Vec.Z = _Color.B;

	return Vec;
}

FString USCTXRResourceManager::GetPathFromFileName(ESCTResourceType _PakType, FString& _PakName, bool _Test)
{
	FString FilePath;
	//绘制户型文件
	if (_PakType == ESCTResourceType::DrawHome)
		FilePath = FSCTResTools::GetDrawHomeDir(_Test) + _PakName;
	//烘焙户型文件
	else if (_PakType == ESCTResourceType::CookedHome)
		FilePath = FSCTResTools::GetCookedHomeDir(_Test) + _PakName;
	//模型文件
	else if (_PakType == ESCTResourceType::MoveableMesh)
		FilePath = FSCTResTools::GetModelDir(_Test) + _PakName;
	//材质文件
	else if (_PakType == ESCTResourceType::Material)
		FilePath = FSCTResTools::GetModelDir(_Test) + _PakName;
	//Actor文件
	else if (_PakType == ESCTResourceType::BlueprintClass)
		FilePath = FSCTResTools::GetModelDir(_Test) + _PakName;
	//组方案
	else if (_PakType == ESCTResourceType::GroupPlan)
		FilePath = FSCTResTools::GetGroupPlanDir(_Test) + _PakName;
	//户型方案
	else if (_PakType == ESCTResourceType::LevelPlan)
		FilePath = FSCTResTools::GetLevelPlanDir(_Test) + _PakName;

	return FilePath;
}

int32 USCTXRResourceManager::GetItemIDFromObj(UObject* _Obj)
{
	if (_Obj)
	{
		int32 SynID = _Obj->GetSynID();
		if (SynList.IsValidIndex(SynID))
		{
			return SynList[SynID]->ID;
		}
	}
	return -1;
}

int32 USCTXRResourceManager::GetItemIDFromActor(AActor* _Actor)
{
	if (_Actor)
	{
		int32 SynID = _Actor->GetSynID();
		if (SynList.IsValidIndex(SynID))
		{
			return SynList[SynID]->ID;
		}
	}
	return -1;
}

FSCTVRSObject* USCTXRResourceManager::GetObjFromObjID(int32 _ObjID)
{
	if (ObjList.IsValidIndex(_ObjID))
	{
		return &ObjList[_ObjID];
	}
	return NULL;
}

TSharedPtr<FSCTContentItemSpace::FContentItem> USCTXRResourceManager::GetContentItemFromID(int32 _SynID)
{
	if (SynList.IsValidIndex(_SynID))
	{
		return SynList[_SynID];
	}
	return NULL;
}

TSharedPtr<FSCTContentItemSpace::FContentItem> USCTXRResourceManager::GetContentItemFromItemID(ESCTResourceType InResType, int32 InID)
{
	for (auto& It : SynList)
	{
		if (It->ResourceType == InResType && It->ID == InID)
		{
			return It;
		}
	}

	return NULL;
}

FString USCTXRResourceManager::GetMaterialFullName(UMaterialInterface* _MI)
{
	//如果壁纸进行了参数修改，则必定从MaterialInstance 变为 MaterialInstanceDynamic，名称也会改变，需要获取其Parent也就是MaterialInstance才能通过GetPakNameFromLongPackageName查找到对应的Pak名称
	UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(_MI);
	FString MatFullName = _MI->GetPathName();
	if (MID)
	{
		//如果发现路径为Common包里的内容，则不取Parent，这种DIM都是后来新的材质加载机制，也就是pak里只有Texture，程序动态创建DMI
		if (MatFullName.StartsWith(TEXT("/Game/DLC/Common/")))
		{
			MatFullName = MID->GetPathName();
		}
		else
		{
			MatFullName = MID->Parent->GetPathName();
			if (MatFullName.StartsWith(TEXT("/Game/DLC/Common/")))
			{
				MatFullName = MID->GetPathName();
			}
		}
	}
	return MatFullName;
}

void USCTXRResourceManager::GetParentMostPath(UMaterialInterface* _MI, TArray<FString>& _Paths)
{
	TempPaths.Reset();

	GetParentMostPathInternal(_MI);
	
	for (auto& It : TempPaths)
	{
		It.RemoveFromStart(TEXT("/Game/"));
		int32 pos = It.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		if (pos != -1)
		{
			It = It.Mid(0, pos);
		}
	}
	_Paths = TempPaths;
}

void USCTXRResourceManager::GetParentMostPathInternal(UMaterialInterface* _MI)
{
	while (true)
	{
		UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(_MI);
		UMaterialInstanceConstant* MIC = Cast<UMaterialInstanceConstant>(_MI);
		UMaterialInstance* MI = Cast<UMaterialInstance>(_MI);
		UMaterial* Mat = Cast<UMaterial>(_MI);

		if (MID)
		{
			//TempPaths.Add(MID->GetPathName());
			if (MID->Parent)
				GetParentMostPathInternal(MID->Parent);
		}
		else if (MIC)
		{
			TempPaths.Add(MIC->GetPathName());
			if (MIC->Parent)
				GetParentMostPathInternal(MIC->Parent);
		}
		else if (MI)
		{
			TempPaths.Add(MI->GetPathName());
			if (MI->Parent)
				GetParentMostPathInternal(MI->Parent);
		}
		else if (Mat)
		{
			TempPaths.Add(Mat->GetPathName());
		}
		
		return;
	}
}

FString USCTXRResourceManager::GetCurMapPakName()
{
	FString MapName = CurWorld->GetOuter()->GetPathName();
	for (auto& It : LevelPathMap)
	{
		if (It.Value == MapName)
			return It.Key;
	}
	return TEXT("");
}

FString USCTXRResourceManager::FindLevelPath(FString _HomePakName, bool _Test)
{
	FString LevelPath;
	FString PakPath = USCTXRResourceManager::GetPathFromFileName(ESCTResourceType::CookedHome, _HomePakName, _Test);
	FString* result = LevelPathMap.Find(_HomePakName);
	if (result == NULL)
	{
		TArray<FString> PakFilenames;
		MountPakReturnFiles(PakPath, PakFilenames);

		for (int32 k = 0; k < PakFilenames.Num(); k++)
		{
			FString Filename(PakFilenames[k]);

			if (!Filename.EndsWith(TEXT(".umap")))
				continue;

			Filename.RemoveFromEnd(TEXT(".umap"), ESearchCase::IgnoreCase);
			int32 pos = Filename.Find(TEXT("/Content/"), ESearchCase::IgnoreCase);
			Filename = Filename.RightChop(pos + 8);
			Filename = TEXT("/Game") + Filename;
			LevelPathMap.Add(_HomePakName, Filename);
			return Filename;
		}
	}
	else
	{
		return *result;
	}
	return TEXT("");
}

bool USCTXRResourceManager::CloneObject(UObject* _NewObject, UObject* _OldObject)
{
	//根据ObjID查找原有Obj
	FSCTVRSObject* ResultObj = GetObjFromObjID(_OldObject->GetObjID());
	if (ResultObj)
	{
		//设置新Object的ObjID
		_NewObject->SetObjID(ObjList.Num());
		_NewObject->SetSynID(_OldObject->GetSynID());
		FSCTVRSObject NewVRSObj = *ResultObj;
		//仅仅把Object替换为NewObject
		NewVRSObj.SetFirstObject(_NewObject);
		//MaterialList里的DIM不要忘记一起替换
		if (NewVRSObj.MaterialList.Num() > 0)
		{
			UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(_NewObject);
			NewVRSObj.MaterialList[0].DIM = MID;
		}
		ObjList.Add(NewVRSObj);
		return true;
	}
	return false;
}

UMaterialInstanceDynamic* USCTXRResourceManager::CreateMID(UMaterialInterface* _MI)
{
	if (_MI)
	{
		UMaterialInstance* MIS = Cast<UMaterialInstance>(_MI);
		UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(_MI);
		UMaterialInstanceConstant* MIC = Cast<UMaterialInstanceConstant>(_MI);
		//如果源材质已经是动态材质，则根据它的Parent新创建一个MID，并复制源材质的动态参数。最后Add到FileList中。
		if (MID)
		{
			UMaterialInstanceDynamic* NewMID = UMaterialInstanceDynamic::Create(MID->Parent,nullptr);
			NewMID->CopyParameterOverrides(MID);
			if (CloneObject(NewMID, _MI))
				return NewMID;
		}
		//如果源材质只是一个MIS，则根据这个MIS直接创建MID。最后Add到FileList中。
		else if (MIS)
		{
			UMaterialInstanceDynamic* NewMID = NULL;
			//如果是UMaterialInstanceConstant，则需要使用它的Parent材质创建MID
			if (MIC)
			{
				NewMID = UMaterialInstanceDynamic::Create(MIS->Parent, nullptr);
				NewMID->CopyParameterOverrides(MIS);
				return NewMID;
			}
			else
			{
				NewMID = UMaterialInstanceDynamic::Create(MIS, nullptr);
				if (CloneObject(NewMID, _MI))
					return NewMID;
			}
		}
		//如果是一个Material，直接创建MID
		else
		{
			UMaterialInstanceDynamic* NewMID = NULL;
			NewMID = UMaterialInstanceDynamic::Create(_MI, nullptr);
			return NewMID;
		}
	}
	return NULL;
}

UStaticMeshComponent* USCTXRResourceManager::GetActorSMC(AActor* _InActor)
{
	AStaticMeshActor* SMA = Cast<AStaticMeshActor>(_InActor);
	if (SMA)
	{
		return SMA->GetStaticMeshComponent();
	}
	return NULL;
}

ESCTActorType USCTXRResourceManager::GetActorType(AActor* _InActor)
{
	AStaticMeshActor* SMA = Cast<AStaticMeshActor>(_InActor);
	if (SMA)
	{
		if (_InActor->Tags.Num() > 0 && _InActor->Tags[0] == "MoveableMeshActor")
			return ESCTActorType::Moveable;
		else
			return ESCTActorType::Static;
	}
	else if (_InActor && _InActor->Tags.Num() > 0)
	{
		if (_InActor->Tags[0] == "BlueprintVRSActor")
			return ESCTActorType::Blueprint;
		else if (_InActor->Tags[0] == "BlueprintVRSActor_DIYHome_Window")
			return ESCTActorType::Blueprint_Window;
		else if (_InActor->Tags[0] == "EnvironmentAsset")
			return ESCTActorType::EnvironmentAsset;
		else
			return ESCTActorType::Other;
	}
	return ESCTActorType::Other;
}

AActor* USCTXRResourceManager::CreateActor(UWorld* _OwnerWorld, TSharedPtr<FSCTContentItemSpace::FContentItem> _SynData, FVector _Location, FRotator _Rotation, FVector _Scale)
{
	CurWorld = _OwnerWorld;
	int32 OutObjID = 0;
	int32 OutSynID = 0;
	AActor* NewActor = NULL;
	FSCTVRSObject* NewVRSObj = LoadObj(_SynData, OutObjID, OutSynID);
	if (NewVRSObj)
	{
		NewActor = CreateActorInternal(NewVRSObj, OutObjID, OutSynID, _Location, _Rotation, _Scale);
	}
	//UploadActorSize();
	return NewActor;
}

UMaterialInterface* USCTXRResourceManager::CreateMaterial(UWorld* _OwnerWorld, TSharedPtr<FSCTContentItemSpace::FContentItem> _SynData)
{
	CurWorld = _OwnerWorld;
	int32 OutSynID, OutObjID;
	UMaterialInterface* NewMaterial = NULL;
	FSCTVRSObject* NewVRSObj = LoadCustomObj(_SynData, OutObjID, OutSynID);
	if (NewVRSObj)
	{
		NewMaterial = CreateMaterialInternal(NewVRSObj, OutObjID, OutSynID);
	}
	return NewMaterial;
}

UMaterialInterface* USCTXRResourceManager::CreateCustomMaterial(FString _FilePath)
{
	FString EmptyParm = TEXT("");
	return CreateCustomMaterial(_FilePath, EmptyParm);
}

UMaterialInterface * USCTXRResourceManager::CreateCustomMaterial(const FString & _FilePath, const FString & _Param)
{

	FString FilePath = _FilePath;
	FString FileParm = _Param;
	int32 OutSynID, OutObjID;
	UMaterialInterface* NewMaterial = NULL;

	FString FileName = FSCTResTools::GetFileNameFromPath(FilePath);
#if 0
	int32 FileID = FSCTResTools::GetFileIDFromName(FileName);
#else
	//FixMe hash值是一个uint32的值，这样使用比较暴力，
	int32 FileID = static_cast<int32>(FSCTResTools::GetFileIDFromNameHashValue(FileName));
#endif
	FString MaterialParameter = FileParm;
	if (MaterialParameter.IsEmpty() == false)
	{
		TSharedPtr<FJsonObject> JSONObject;
		TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(FileParm);
		bool readresult = FJsonSerializer::Deserialize(Reader, JSONObject);
		if (!readresult) return nullptr;
		MaterialParameter = JSONObject->GetStringField(TEXT("materialParameter"));
	}
	TSharedPtr<FSCTContentItemSpace::FModelRes> ModelRes = MakeShareable(new FSCTContentItemSpace::FModelRes(FileName, _FilePath, "", "0", MaterialParameter, ""));
	TSharedPtr<FSCTContentItemSpace::FContentItem> SynData = MakeShareable(new FSCTContentItemSpace::FContentItem(ESCTResourceType::Material, FileID, FileName, "", ModelRes));
	FSCTVRSObject* NewVRSObj = LoadCustomObj(SynData, OutObjID, OutSynID);
	if (NewVRSObj)
	{
		NewMaterial = CreateMaterialInternal(NewVRSObj, OutObjID, OutSynID);
	}
	return NewMaterial;
}

FSCTVRSObject * USCTXRResourceManager::LoadCustomObj(const FString & _FilePath)
{
	return LoadCustomObj(_FilePath,TEXT(""));
}

FSCTVRSObject * USCTXRResourceManager::LoadCustomObj(const FString & _FilePath, const FString & _Param)
{
	FString FilePath = _FilePath;
	const FString FileParm = _Param;
	int32 OutSynID, OutObjID;	
	FString FileName = FSCTResTools::GetFileNameFromPath(FilePath);	
	int32 FileID = static_cast<int32>(FSCTResTools::GetFileIDFromNameHashValue(FileName));	
	FString MaterialParameter = _Param;
	if (_Param.IsEmpty() == false)
	{
		TSharedPtr<FJsonObject> JSONObject;
		TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(FileParm);
		bool readresult = FJsonSerializer::Deserialize(Reader, JSONObject);
		if (!readresult) return nullptr;
		MaterialParameter = JSONObject->GetStringField(TEXT("materialParameter"));
	}
	
	TSharedPtr<FSCTContentItemSpace::FModelRes> ModelRes = MakeShareable(new FSCTContentItemSpace::FModelRes(FileName, _FilePath, "", "0", MaterialParameter, ""));
	TSharedPtr<FSCTContentItemSpace::FContentItem> SynData = MakeShareable(new FSCTContentItemSpace::FContentItem(ESCTResourceType::Material, FileID, FileName, "", ModelRes));
	return LoadCustomObj(SynData, OutObjID, OutSynID);
}

UStaticMesh * USCTXRResourceManager::CreateStaticMesh(const FString & _FilePath, const FString & _Param)
{
	FString FilePath = _FilePath;
	FString FileParm = _Param;
	int32 OutSynID, OutObjID;
	UStaticMesh * NewMesh = NULL;

	FString FileName = FSCTResTools::GetFileNameFromPath(FilePath);
	int32 FileID = FSCTResTools::GetFileIDFromName(FileName);
	TSharedPtr<FSCTContentItemSpace::FModelRes> ModelRes = MakeShareable(new FSCTContentItemSpace::FModelRes(FileName, _FilePath, "", "0", FileParm, ""));
	TSharedPtr<FSCTContentItemSpace::FContentItem> SynData = MakeShareable(new FSCTContentItemSpace::FContentItem(ESCTResourceType::MoveableMesh, FileID, FileName, "", ModelRes));
	FSCTVRSObject* NewVRSObj = LoadCustomObj(SynData, OutObjID, OutSynID);
	if (NewVRSObj)
	{
		NewMesh = Cast<UStaticMesh>(NewVRSObj->GetFirstObject());;
	}
	return NewMesh;
}

FSCTVRSObject* USCTXRResourceManager::LoadCustomObj(TSharedPtr<FSCTContentItemSpace::FContentItem> _SynData, int32& _OutObjID, int32& _OutSynID)
{
	if (!FPaths::FileExists(*_SynData->ResObj->FilePath))
		return NULL;

	//---------------------------------------------------------------------------------把同步数据插入到SynList中
	TSharedPtr<FSCTContentItemSpace::FContentItem> ResultSynData = NULL;
	int32 ResultSynIndex;
	for (ResultSynIndex = SynList.Num() - 1; ResultSynIndex >= 0; ResultSynIndex--)
	{
		if (SynList[ResultSynIndex]->ID == _SynData->ID)
		{
			ResultSynData = SynList[ResultSynIndex];
			break;
		}
	}

	if (ResultSynData.IsValid())
	{
		_OutSynID = ResultSynIndex;
	}
	else
	{
		//FSCTContentItemSpace::FContentItem NewSynData = _SynData;
		_OutSynID = SynList.Num();
		SynList.Add(_SynData);
	}
	//---------------------------------------------------------------------------------倒序查找是否已经载入，因为最近时间加载的模型趋向于用户使用
	FSCTVRSObject* ResultObj = NULL;
	int32 ResultObjIndex;
	for (ResultObjIndex = ObjList.Num() - 1; ResultObjIndex >= 0; ResultObjIndex--)
	{
		if (ObjList[ResultObjIndex].ItemID == _SynData->ID)
		{
			ResultObj = &ObjList[ResultObjIndex];
			break;
		}
	}

	//如果有记录
	if (ResultObj)
	{
		//如果FirstObject不为空
		if (ResultObj->GetFirstObject())
		{
			//如果FirstObject是一个材质类型，则需要复制一份，并返回
			if (ResultObj->FileType == ESCTResourceType::Material)
			{
				UMaterialInterface* MI = Cast<UMaterialInterface>(ResultObj->GetFirstObject());
				UMaterialInstanceDynamic* MID = CreateMID(MI);
				if (MID)
				{
					MID->SetSynID(_OutSynID);
					_OutObjID = ObjList.Num() - 1;
					return &ObjList.Last();
				}
			}
			//否则直接返回
			else
			{
				_OutObjID = ResultObjIndex;
				return &ObjList[ResultObjIndex];
			}
		}
	}
	//如果没有记录，则尝试加载
	else
	{
		FSCTVRSObject NewVRSObj;
		NewVRSObj.ItemID = _SynData->ID;
		NewVRSObj.FileName = _SynData->ResObj->FileName;
		NewVRSObj.FilePath = _SynData->ResObj->FilePath;
		NewVRSObj.Param = StaticCastSharedPtr<FSCTContentItemSpace::FModelRes>(_SynData->ResObj)->MaterialParameter;
		NewVRSObj.SynID = _OutSynID;
		//如果加载成功，则新添加一条记录（这样以后无需每次都从Pak加载）
		if (LoadObjInternal(NewVRSObj))
		{
			_OutObjID = ObjList.Num();
			NewVRSObj.GetFirstObject()->SetObjID(ObjList.Num());
			NewVRSObj.GetFirstObject()->SetSynID(_OutSynID);
			if (NewVRSObj.GetSkeletalMeshObject())
				NewVRSObj.GetSkeletalMeshObject()->SetObjID(ObjList.Num());
			if (NewVRSObj.GetAnimSequenceObject())
				NewVRSObj.GetAnimSequenceObject()->SetObjID(ObjList.Num());
			if (NewVRSObj.GetLampMeshObject())
				NewVRSObj.GetLampMeshObject()->SetObjID(ObjList.Num());
			ObjList.Add(NewVRSObj);
			return &ObjList.Last();
		}
	}

	return NULL;
}

AActor* USCTXRResourceManager::CreateActorFromID(UWorld* _OwnerWorld, int32 _FileID, FVector _Location, FRotator _Rotation, FVector _Scale)
{
	CurWorld = _OwnerWorld;
	int32 OutObjID = 0;
	int32 OutSynID = 0;
	FSCTVRSObject* NewVRSObj = LoadObjFromFileID(_FileID, OutObjID, OutSynID);
	if (NewVRSObj)
	{
		return CreateActorInternal(NewVRSObj, OutObjID, OutSynID, _Location, _Rotation, _Scale);
	}
	return NULL;
}

FSCTVRSObject* USCTXRResourceManager::LoadObj(TSharedPtr<FSCTContentItemSpace::FContentItem> _SynData, int32& _OutObjID, int32& _OutSynID)
{
	int32 ItemID = _SynData->ID;
	FString FilePath = _SynData->ResObj->FilePath;
	if (!FPaths::FileExists(*FilePath))
	{
		return NULL;
	}

	//---------------------------------------------------------------------------------把同步数据插入到SynList中
	TSharedPtr<FSCTContentItemSpace::FContentItem> ResultSynData = NULL;
	int32 ResultSynIndex;
	for (ResultSynIndex = SynList.Num() - 1; ResultSynIndex >= 0; ResultSynIndex--)
	{
		if (SynList[ResultSynIndex]->ID == ItemID)
		{
			ResultSynData = SynList[ResultSynIndex];
			break;
		}
	}

	if (ResultSynData.IsValid())
	{
		_OutSynID = ResultSynIndex;
		//在下载时（UDownloadItem）因为有检测xml是否存在，如果存在则会尝试继续下载，但是这个时候下载的内容是没有智能设计方案ID的并且存储到了缓存，导致在智能设计替换后的actor对应的ContentItem没有方案ID而无法做下一步智能替换
// 		if (ResultSynData->ResObj.AutoDesignPlanID == -1 || 
// 			ResultSynData->ResObj.AutoDesignSpaceTypeID == -1 || 
// 			ResultSynData->ResObj.AutoDesignActorTypeID == -1)
// 		{
// 			ResultSynData->ResObj.AutoDesignPlanID = _SynData.ResObj.AutoDesignPlanID;
// 			ResultSynData->ResObj.AutoDesignSpaceTypeID = _SynData.ResObj.AutoDesignSpaceTypeID;
// 			ResultSynData->ResObj.AutoDesignActorTypeID = _SynData.ResObj.AutoDesignActorTypeID;
// 			ResultSynData->ResObj.AutoDesignActorClassID = _SynData.ResObj.AutoDesignActorClassID;
// 			ResultSynData->ResObj.AutoDesignActorDefault = _SynData.ResObj.AutoDesignActorDefault;
// 		}
	}
	else
	{
		//FSCTContentItemSpace::FContentItem NewSynData = _SynData;
		_OutSynID = SynList.Num();
		SynList.Add(_SynData);
	}

	//---------------------------------------------------------------------------------倒序查找是否已经载入，因为最近时间加载的模型趋向于用户使用
	FSCTVRSObject* ResultObj = NULL;
	int32 ResultObjIndex;
	for (ResultObjIndex = ObjList.Num() - 1; ResultObjIndex >= 0; ResultObjIndex--)
	{
		if (ObjList[ResultObjIndex].ItemID == ItemID)
		{
			ResultObj = &ObjList[ResultObjIndex];
			break;
		}
	}

	//如果有记录
	if (ResultObj)
	{
		//如果FirstObject不为空
		if (ResultObj->GetFirstObject())
		{
			//如果FirstObject是一个材质类型，则需要复制一份，并返回
			if (ResultObj->FileType == ESCTResourceType::Material)
			{
				UMaterialInterface* MI = Cast<UMaterialInterface>(ResultObj->GetFirstObject());
				UMaterialInstanceDynamic* MID = CreateMID(MI);
				if (MID)
				{
					MID->SetSynID(_OutSynID);
					_OutObjID = ObjList.Num() - 1;
					return &ObjList.Last();
				}
			}
			//否则直接返回
			else
			{
				_OutObjID = ResultObjIndex;
				return &ObjList[ResultObjIndex];
			}
		}
	}
	//如果没有记录，则尝试加载
	else
	{
		FSCTVRSObject NewVRSObj;
		NewVRSObj.ItemID = _SynData->ID;
		NewVRSObj.FileName = _SynData->ResObj->FileName;
		NewVRSObj.FilePath = _SynData->ResObj->FilePath;
		NewVRSObj.FileType = _SynData->ResourceType;
		NewVRSObj.Param = StaticCastSharedPtr<FSCTContentItemSpace::FModelRes>(_SynData->ResObj)->MaterialParameter;
		NewVRSObj.SynID = _OutSynID;
		//如果加载成功，则新添加一条记录（这样以后无需每次都从Pak加载）
		if(LoadObjInternal(NewVRSObj))
		{
			_OutObjID = ObjList.Num();
			NewVRSObj.GetFirstObject()->SetObjID(ObjList.Num());
			NewVRSObj.GetFirstObject()->SetSynID(_OutSynID);
			if (NewVRSObj.GetSkeletalMeshObject())
				NewVRSObj.GetSkeletalMeshObject()->SetObjID(ObjList.Num());
			if (NewVRSObj.GetAnimSequenceObject())
				NewVRSObj.GetAnimSequenceObject()->SetObjID(ObjList.Num());
			if (NewVRSObj.GetLampMeshObject())
				NewVRSObj.GetLampMeshObject()->SetObjID(ObjList.Num());
			ObjList.Add(NewVRSObj);
			return &ObjList.Last();
		}
	}
	
	return NULL;
}

FSCTVRSObject* USCTXRResourceManager::LoadObjFromFileID(int32 _ItemID, int32& _OutObjID, int32& _OutSynID)
{
	TSharedPtr<FSCTContentItemSpace::FContentItem> ResultSynData = NULL;
	int32 ResultSynIndex;
	for (ResultSynIndex = SynList.Num() - 1; ResultSynIndex >= 0; ResultSynIndex--)
	{
		//ItemID相同时不会插入
		if (SynList[ResultSynIndex]->ID == _ItemID)
		{
			ResultSynData = SynList[ResultSynIndex];
			break;
		}
	}

	if (ResultSynData.IsValid())
	{
		_OutSynID = ResultSynIndex;
	}
	//倒序查找是否已经载入，因为最近时间加载的模型趋向于用户使用
	FSCTVRSObject* ResultObj = NULL;
	int32 ResultObjIndex;
	for (ResultObjIndex = ObjList.Num() - 1; ResultObjIndex >= 0; ResultObjIndex--)
	{
		if (ObjList[ResultObjIndex].ItemID == _ItemID)
		{
			ResultObj = &ObjList[ResultObjIndex];
			break;
		}
	}
	//如果列表里没有该记录，则mount，并载入object，存入记录
	if (ResultObj)
	{
		//既有记录，object又不为空，直接返回
		if (ResultObj->GetFirstObject())
		{
			//如果FirstObject是一个材质类型，则需要复制一份，并返回
			if (ResultObj->FileType == ESCTResourceType::Material)
			{
				UMaterialInterface* MI = Cast<UMaterialInterface>(ResultObj->GetFirstObject());
				UMaterialInstanceDynamic* MID = CreateMID(MI);
				if (MID)
				{
					MID->SetSynID(_OutSynID);
					_OutObjID = ObjList.Num() - 1;
					//复制出来的材质，要恢复到Server的默认值，返回后再根据Para字段去覆盖得出最新的效果
					ObjList.Last().ResetMaterialParameters();
					return &ObjList.Last();
				}
			}
			//否则直接返回
			else
			{
				_OutObjID = ResultObjIndex;
				return &ObjList[ResultObjIndex];
			}
		}
	}
	//如果没有记录，则尝试加载
	else if(SynList.IsValidIndex(ResultSynIndex))
	{
		FSCTVRSObject NewVRSObj;
		NewVRSObj.ItemID = SynList[ResultSynIndex]->ID;
		NewVRSObj.FileName = SynList[ResultSynIndex]->ResObj->FileName;
		NewVRSObj.FilePath = SynList[ResultSynIndex]->ResObj->FilePath;
		NewVRSObj.FileType = SynList[ResultSynIndex]->ResourceType;
		NewVRSObj.Param = StaticCastSharedPtr<FSCTContentItemSpace::FModelRes>(SynList[ResultSynIndex]->ResObj)->MaterialParameter;
		NewVRSObj.SynID = _OutSynID;
		//如果加载成功，则新添加一条记录（这样以后无需每次都从Pak加载）
		if (LoadObjInternal(NewVRSObj))
		{
			_OutObjID = ObjList.Num();
			NewVRSObj.GetFirstObject()->SetObjID(ObjList.Num());
			NewVRSObj.GetFirstObject()->SetSynID(ResultSynIndex);
			if (NewVRSObj.GetSkeletalMeshObject())
				NewVRSObj.GetSkeletalMeshObject()->SetObjID(ObjList.Num());
			if (NewVRSObj.GetAnimSequenceObject())
				NewVRSObj.GetAnimSequenceObject()->SetObjID(ObjList.Num());
			if (NewVRSObj.GetLampMeshObject())
				NewVRSObj.GetLampMeshObject()->SetObjID(ObjList.Num());
			ObjList.Add(NewVRSObj);
			return &ObjList.Last();
		}
	}

	return NULL;
}

bool USCTXRResourceManager::LoadObjInternal(FSCTVRSObject& _OutObj)
{
	FPlatformFileManager::Get().SetPlatformFile(*PakPlatform);
	//-----------------------------------------------------------创建模型
	TArray<FString> PakFilenames;
	MountPakReturnFiles(_OutObj.FilePath, PakFilenames, 3, false);

	//查找Pak包里是否有xml，有则尝试解析里面的信息，这是新的模型加载机制，用来为模型动态创建材质实例，防止材质格式升级带来的问题。
	FString XMLPath = TEXT("");
	for (int32 j = 0; j < PakFilenames.Num(); j++)
	{
		FString Filename(PakFilenames[j]);
		if (Filename.EndsWith(TEXT("pakInfo.xml")))
		{
			int32 pos = Filename.Find(TEXT("/Content/"), ESearchCase::IgnoreCase);
			Filename = Filename.RightChop(pos + 9);
			Filename = FPaths::ProjectContentDir() + Filename;
			Filename = FPaths::ConvertRelativePathToFull(Filename);
			XMLPath = Filename;
			break;
		}
	}
	//如果找到XML，进行解析，创建MaterialData
	if (XMLPath != TEXT(""))
	{
		FString Content;
		FFileHelper::LoadFileToString(Content, *XMLPath);
		FXmlFile XMLFile(Content, EConstructMethod::ConstructFromBuffer);
		FXmlNode* XMLRootNode;
		XMLRootNode = XMLFile.GetRootNode();
		TMap<FString, UTexture*> TextureMap;
		USkeletalMesh* SkeletalMeshObject = NULL;
		UAnimSequence* AnimSequenceObject = NULL;
		UStaticMesh* StaticMeshObject = NULL;
		UBlueprintGeneratedClass* BPClass = NULL;
		FVector BoundExtent = FVector::ZeroVector;
		FVector BoundOrigin = FVector::ZeroVector;
		if (XMLRootNode)
		{
			//有pak.xml文件并且可以正常加载，则视为标准化pak文件
			_OutObj.bNormalized = true;
			//版本号
			FString XMLVersion = XMLRootNode->GetAttribute(TEXT("Version"));
			//"Model"：静态家具；"Actor"：可交互的蓝图物体Actor；"Material"：材质
			FString ObjectType = XMLRootNode->GetAttribute(TEXT("Type"));
			//当ObjectType为Actor类型时，才会查询该值。
			//当值为"Fruniture"时，意为可交互的家具，内有SkeletalMesh及动画信息；值为"Lamp"时，意为可交互的灯，内有StaticMesh以及点/射光源的位置信息
			FString ActorTemplateType;
			FXmlNode* ActorTemplateNode = XMLRootNode->FindChildNode(TEXT("ActorTemplate"));
			if (ActorTemplateNode)
				ActorTemplateType = ActorTemplateNode->GetAttribute(TEXT("Type"));
			//获取pak的xml版本号，用来判断材质的加载方式
			GetContentItemFromID(_OutObj.SynID)->ResObj->Version = FCString::Atoi(*XMLVersion);

			//确定要加载的是什么类型物体
			if (ObjectType == "Model")
				_OutObj.FileType = ESCTResourceType::MoveableMesh;
			else if (ObjectType == "Material")
				_OutObj.FileType = ESCTResourceType::Material;
			else if (ObjectType == "Actor")
				_OutObj.FileType = ESCTResourceType::BlueprintClass;

			//如果是Actor交互物体，需要进一步确认是蒙皮物体，还是交互灯物体
			ESCTResourceType ThePakType = _OutObj.FileType;
			if (ThePakType == ESCTResourceType::BlueprintClass)
			{
				if (ActorTemplateType == TEXT("Furniture"))
					ThePakType = ESCTResourceType::SkeletalMesh;
				else if (ActorTemplateType == TEXT("Lamp"))
					ThePakType = ESCTResourceType::Light;
			}

			//一次遍历，根据ThePakType，加载需要的Object
			for (int32 j = 0; j < PakFilenames.Num(); j++)
			{
				FString Filename(PakFilenames[j]);
				UObject* LoadedObject = NULL;

				if (Filename.EndsWith(TEXT(".uasset")))
				{
					//拼出UObject的加载路径
					Filename.RemoveFromEnd(TEXT(".uasset"), ESearchCase::IgnoreCase);
					int32 pos = Filename.Find(TEXT("/Content/"), ESearchCase::IgnoreCase);
					Filename = Filename.RightChop(pos + 8);
					Filename = TEXT("/Game") + Filename;

					//从路径里截取最后面的文件名
					pos = Filename.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
					FString TextureName = Filename.RightChop(pos + 1);

					//加载到内存
					LoadedObject = StaticLoadObject(UObject::StaticClass(), NULL, *Filename);

					//如果为UTexture类型，则存入贴图映射表
					UTexture* TextureObject = Cast<UTexture>(LoadedObject);

					if (TextureObject)
					{
						TextureMap.Add(TextureName, TextureObject);
					}
					//如果是静态家具
					else if (ThePakType == ESCTResourceType::MoveableMesh)
					{
						if (Cast<UStaticMesh>(LoadedObject))
						{
							_OutObj.SetFirstObject(LoadedObject);
							_OutObj.SetLampMeshObject(Cast<UStaticMesh>(LoadedObject));
						}
					}
					//如果是材质，如果不是动态材质，创建一个
					else if (ThePakType == ESCTResourceType::Material)
					{
						UMaterialInterface* MI = Cast<UMaterialInterface>(LoadedObject);
						UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(LoadedObject);
						if (MI && !MID)
						{
							MID = CreateMID(MI);
							if (MID)
							{
								_OutObj.SetFirstObject(MID);
								//某些材质没有pak.xml，所有MaterialList为空，此处手动添加进去
								if (_OutObj.MaterialList.Num() == 0)
								{
									TArray<FSCTXRParameter> TempParaList;
									_OutObj.MaterialList.Add(FSCTModelMaterialData(TEXT(""), FSCTCommonMaterialData::E_None, MID, TempParaList));
								}
							}
						}
					}
					//如果音频
// 					else if (ThePakType == ESCTResourceType::Wav)
// 					{
// 						if (Cast<USoundBase>(LoadedObject))
// 							_OutObj.SetFirstObject(LoadedObject);
// 					}
					//如果是蓝图物体，调整路径，尝试加载
					else if (ThePakType == ESCTResourceType::BlueprintClass)
					{
						int32 pos2 = Filename.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
						FString str = Filename.RightChop(pos2 + 1);
						str += TEXT("_C");
						Filename += TEXT(".") + str;
						LoadedObject = StaticLoadObject(UObject::StaticClass(), NULL, *Filename);
						UBlueprintGeneratedClass* BPObj = Cast<UBlueprintGeneratedClass>(LoadedObject);
						if (BPObj)
							_OutObj.SetFirstObject(BPObj);
					}		
					// 如果是骨骼网格
					else if (ThePakType == ESCTResourceType::SkeletalMesh)
					{
						USkeletalMesh* SkeletalMeshObj = Cast<USkeletalMesh>(LoadedObject);
						if (SkeletalMeshObj)
							_OutObj.SetSkeletalMeshObject(SkeletalMeshObj);
						else
						{
							UAnimSequence* AnimObj = Cast<UAnimSequence>(LoadedObject);
							if (AnimObj)
								_OutObj.SetAnimSequenceObject(AnimObj);
						}
					}
					//如果是灯具
					else if (ThePakType == ESCTResourceType::Light)
					{
						UStaticMesh* LampStaticMesh = Cast<UStaticMesh>(LoadedObject);
						if (LampStaticMesh)
							_OutObj.SetLampMeshObject(LampStaticMesh);
					}
				}
			}

			//如果是可交互物体，则通过路径找到common包里的蓝图模板，并载入
			if (ObjectType == TEXT("Actor") && ActorTemplateNode)
			{
				FString TemplatePath = ActorTemplateNode->GetAttribute(TEXT("Path"));
				TemplatePath = TemplatePath.Replace(TEXT("\\"), TEXT("/"));
				TemplatePath = TEXT("/Game/") + TemplatePath;

				int32 pos = TemplatePath.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
				FString str = TemplatePath.RightChop(pos + 1);
				str += TEXT("_C");
				TemplatePath += TEXT(".") + str;
				UObject* LoadedObject = StaticLoadObject(UObject::StaticClass(), NULL, *TemplatePath);

				UBlueprintGeneratedClass* BPObj = Cast<UBlueprintGeneratedClass>(LoadedObject);
				if (BPObj)
				{
					_OutObj.SetFirstObject(BPObj);
				}

				FString BoundOriginStr = ActorTemplateNode->GetAttribute(TEXT("BoundOrigin"));
				FString BoundExtentStr = ActorTemplateNode->GetAttribute(TEXT("BoundBoxExtent"));
				if (!BoundOriginStr.IsEmpty())
					_OutObj.BoundOrigin.InitFromString(BoundOriginStr);
				if (!BoundExtentStr.IsEmpty())
					_OutObj.BoundExtent.InitFromString(BoundExtentStr);

				//如果是灯具，还需要加载光源信息
				if (ActorTemplateType == TEXT("Lamp"))
				{
					FXmlNode* LampNode = XMLRootNode->FindChildNode(TEXT("Lamp"));

					FString ColorStr = LampNode->GetAttribute(TEXT("Color"));
					FString IntensityStr = LampNode->GetAttribute(TEXT("Intensity"));
					FLinearColor TheColor;
					TheColor.InitFromString(ColorStr);
					float TheIntensity = FCString::Atof(*IntensityStr);

					_OutObj.LampColor = TheColor;
					_OutObj.LampIntensity = TheIntensity;

					const TArray<FXmlNode*> LampInfoNodes = LampNode->GetChildrenNodes();
					for (int32 i = 0; i < LampInfoNodes.Num(); i++)
					{
						FXmlNode* LampInfoNode = LampInfoNodes[i];
						FString LampType = LampInfoNode->GetAttribute(TEXT("Type"));
						FString LocationS = LampInfoNode->GetAttribute(TEXT("Location"));
						FString RotationS = LampInfoNode->GetAttribute(TEXT("Rotation"));
						
						FVector Location;
						Location.InitFromString(LocationS);
						FVector Scale = { 1, 1, 1 };
						FRotator Rotation;
						Rotation.InitFromString(RotationS);
						FTransform t(Rotation, Location, Scale);
						
						if (LampType == TEXT("Point"))
						{
							_OutObj.PointArray.Add(t);
						}
						else
						{
							_OutObj.SpotArray.Add(t);
						}
					}
				}
			}
			
			//模型，加载里面MaterialList节点，创建对应的UMaterialInstanceDynamic
			if (ObjectType == TEXT("Model") || ObjectType == TEXT("Actor"))
			{
				//获取插槽列表
				FXmlNode* SocketListNode = XMLRootNode->FindChildNode(TEXT("SocketList"));
				if (SocketListNode)
				{
					const TArray<FXmlNode*> ChildrenArray = SocketListNode->GetChildrenNodes();

					for (auto& It : ChildrenArray)
					{
						FVector SocketLocation;
						FRotator SocketRotation;
						FString SocketName;
						FXmlNode* SocketNameNode = It->FindChildNode("Name");
						FXmlNode* SocketLocationNode = It->FindChildNode("Location");
						FXmlNode* SocketRotationNode = It->FindChildNode("Rotation");
						if (SocketNameNode)
							SocketName = SocketNameNode->GetContent();
						if (SocketLocationNode)
							SocketLocation.InitFromString(SocketLocationNode->GetContent());
						if (SocketRotationNode)
							SocketRotation.InitFromString(SocketRotationNode->GetContent());
						_OutObj.SocketList.Add(FSCTVRSObject::FSocketData(SocketName, SocketLocation, SocketRotation));
					}
				}

				//获取模型吸附面和原点位置偏移量
				FXmlNode* SettingsNode = XMLRootNode->FindChildNode(TEXT("Settings"));
				if (SettingsNode)
				{
					FXmlNode* SnapTypeNode = SettingsNode->FindChildNode(TEXT("SnapType"));
					if (SnapTypeNode)
						_OutObj.SnapType = FSCTVRSObject::EMeshSnapType(FCString::Atoi(*SnapTypeNode->GetContent()));
					FXmlNode* OriginOffsetNode = SettingsNode->FindChildNode(TEXT("OriginOffset"));
					if (OriginOffsetNode)
						_OutObj.OriginOffset.InitFromString(OriginOffsetNode->GetContent());
				}

				const TArray<FXmlNode*> ChildrenArray = XMLRootNode->GetChildrenNodes();
				TArray<FXmlNode*> MaterialArray;
				for (auto& It : ChildrenArray)
				{
					if (It->GetTag() == TEXT("Material"))
					{
						MaterialArray.Add(It);
					}
				}
				for (int32 i = 0; i < MaterialArray.Num(); i++)
				{
					FSCTModelMaterialData ModelMaterialData = CreateModelMaterialData(GetContentItemFromID(_OutObj.SynID)->ResObj->Version, MaterialArray[i], TextureMap);
					_OutObj.MaterialList.Add(ModelMaterialData);
				}
			}
			//材质，读取用到的漫反射图，法线图，尺寸信息
			else if (ObjectType == TEXT("Material"))
			{
				FString MaterialName;
				FVector2D TextureSize = FVector2D(800, 800);
				UTexture* DiffuseMap = NULL;
				UTexture* NormalMap = NULL;

				const FXmlNode* TextureTypeNode = XMLRootNode->FindChildNode(TEXT("TextureType"));
				MaterialName = TextureTypeNode->GetContent();

				const TArray<FXmlNode*> ChildrenNodes = XMLRootNode->GetChildrenNodes();
				for (int32 i = 0; i < ChildrenNodes.Num(); i++)
				{
					FXmlNode *ChildNode = ChildrenNodes[i];
					if (ChildNode->GetTag() == TEXT("ColorMap"))
					{
						FString ColorMapStr = ChildNode->GetContent();
						FString TextureFileName = FPaths::GetBaseFilename(ColorMapStr);
						DiffuseMap = TextureMap[TextureFileName];
					}
					else if (ChildNode->GetTag() == TEXT("NormalMap"))
					{
						FString NormalMapStr = ChildNode->GetContent();
						FString TextureFileName = FPaths::GetBaseFilename(NormalMapStr);
						NormalMap = TextureMap[TextureFileName];
					}
					else if (ChildNode->GetTag() == TEXT("TextureSize"))
					{
						FString TextureSizeStr = ChildNode->GetContent();
						TextureSize.InitFromString(TextureSizeStr);
						TextureSize.X = 800.f / TextureSize.X;
						TextureSize.Y = 800.f / TextureSize.Y;
					}
				}
				FSCTModelMaterialData MaterialData = CreateMaterialData(MaterialName, TextureSize, DiffuseMap, NormalMap);
				_OutObj.SetFirstObject(MaterialData.DIM);
				_OutObj.MaterialList.Add(MaterialData);
			}
			//模型，加载里面MaterialList节点，创建对应的UMaterialInstanceDynamic
			//--------------------------------------------------------------------------------解析从网络获取的Para字符串，然后覆盖ParaList数据
			if (!_OutObj.Param.IsEmpty())
			{
				TArray<FSCTModelMaterialData> ServerParaList;
				if (ConvertJsonToModelMaterialList(ObjectType == TEXT("Material"), _OutObj.Param, ServerParaList))
				{
					//5.0之前的服务器数据没有SlotName，则直接按照顺序一一覆盖
					if (ServerParaList.Num() > 0 && ServerParaList[0].SlotName.IsEmpty())
					{
						//只有服务器材质个数跟 模型 id个数一致  才会覆盖
						if (ServerParaList.Num() == _OutObj.MaterialList.Num())
						{
							for (int32 i = 0; i < ServerParaList.Num(); i++)
							{
								//遍历，查找相匹配的ID
								for (auto& ServerParaIt2 : ServerParaList[i].ParaList)
								{
									for (auto& OutMatDataIt2 : _OutObj.MaterialList[i].ParaList)
									{
										//ID匹配成功
										if (ServerParaIt2.ID == OutMatDataIt2.ID)
										{
											if (ServerParaIt2.VarType == FSCTXRParameter::E_Float)
											{
												OutMatDataIt2.Value = ServerParaIt2.Value;
												OutMatDataIt2.ServerValue = ServerParaIt2.Value;
												_OutObj.MaterialList[i].DIM->SetScalarParameterValue(FName(*ServerParaIt2.ParaName), ServerParaIt2.Value);
											}
											else if (ServerParaIt2.VarType == FSCTXRParameter::E_Vec3)
											{
												FLinearColor ColorValue;
												FVector ColorVector = ServerParaIt2.Color;
												OutMatDataIt2.Color = ColorVector;
												OutMatDataIt2.ServerColor = ColorVector;
												ColorValue.R = ColorVector.X;
												ColorValue.G = ColorVector.Y;
												ColorValue.B = ColorVector.Z;
												_OutObj.MaterialList[i].DIM->SetVectorParameterValue(FName(*ServerParaIt2.ParaName), ColorValue);
											}
											break;
										}
									}
								}
							}
						}
						//否则就视为不匹配，红字警告
						else
						{
							_OutObj.MaterialParaMismatch = true;
						}
					}
					//5.0保存的都有SlotName数据，则需要匹配SlotName
					else
					{
						//遍历，找到匹配的SlotName
						for (auto& ServerParaIt : ServerParaList)
						{
							for (auto& OutMatDataIt : _OutObj.MaterialList)
							{
								//SlotName匹配成功
								if (ServerParaIt.SlotName == OutMatDataIt.SlotName)
								{
									//遍历，查找相匹配的ID
									for (auto& ServerParaIt2 : ServerParaIt.ParaList)
									{
										for (auto& OutMatDataIt2 : OutMatDataIt.ParaList)
										{
											//ID匹配成功
											if (ServerParaIt2.ID == OutMatDataIt2.ID)
											{
												if (ServerParaIt2.VarType == FSCTXRParameter::E_Float)
												{
													OutMatDataIt2.Value = ServerParaIt2.Value;
													OutMatDataIt2.ServerValue = ServerParaIt2.Value;
													//如果是材质类型修改，则需要创建新的材质类型，并替换
													if (ServerParaIt2.ID == -4)
														ChangeModelMaterialData(ServerParaIt2.Value, &OutMatDataIt);
													if (ServerParaIt2.ID == -3)
													{
														OutMatDataIt.NormalMap_Override = GetNormalTexture((int32)ServerParaIt2.Value);
														if(OutMatDataIt.NormalMap_Override == EmptyTexture || OutMatDataIt.NormalMap_Override == NULL)
														{
															OutMatDataIt.DIM->SetScalarParameterValue("Normal_UseTexture", 0.f);
															OutMatDataIt.DIM->SetTextureParameterValue("Normal_Map", NULL);
														}
														else
														{
															OutMatDataIt.DIM->SetScalarParameterValue("Normal_UseTexture", 1.f);
															OutMatDataIt.DIM->SetTextureParameterValue("Normal_Map", OutMatDataIt.NormalMap_Override);
														}
													}
													else
														OutMatDataIt.DIM->SetScalarParameterValue(FName(*ServerParaIt2.ParaName), ServerParaIt2.Value);
												}
												else if (ServerParaIt2.VarType == FSCTXRParameter::E_Vec3)
												{
													FLinearColor ColorValue;
													FVector ColorVector = ServerParaIt2.Color;
													OutMatDataIt2.Color = ColorVector;
													OutMatDataIt2.ServerColor = ColorVector;
													ColorValue.R = ColorVector.X;
													ColorValue.G = ColorVector.Y;
													ColorValue.B = ColorVector.Z;
													OutMatDataIt.DIM->SetVectorParameterValue(FName(*ServerParaIt2.ParaName), ColorValue);
												}
												else if (ServerParaIt2.VarType == FSCTXRParameter::E_Bool)
												{
													OutMatDataIt2.bCheck = ServerParaIt2.bCheck;
													OutMatDataIt2.bServerCheck = ServerParaIt2.bCheck;
													OutMatDataIt.DIM->SetScalarParameterValue(FName(*ServerParaIt2.ParaName), ServerParaIt2.bCheck ? 1.f : 0.f);
												}
												break;
											}
										}
									}
									break;
								}
							}
						}
					}
				}
			}
			//--------------------------------------------------------------------------------
		}
	}
	else
	{
		for (int32 j = 0; j < PakFilenames.Num(); j++)
		{
			FString Filename(PakFilenames[j]);
			UObject* LoadedObject = NULL;

			if (Filename.EndsWith(TEXT(".uasset")))
			{
				//拼出UObject的加载路径
				Filename.RemoveFromEnd(TEXT(".uasset"), ESearchCase::IgnoreCase);
				int32 pos = Filename.Find(TEXT("/Content/"), ESearchCase::IgnoreCase);
				Filename = Filename.RightChop(pos + 8);
				Filename = TEXT("/Game") + Filename;

				//从路径里截取最后面的文件名
				pos = Filename.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
				FString TextureName = Filename.RightChop(pos + 1);

				//加载到内存
				LoadedObject = StaticLoadObject(UObject::StaticClass(), NULL, *Filename);

				//如果是静态家具
				if (Cast<UStaticMesh>(LoadedObject))
				{
					_OutObj.SetFirstObject(LoadedObject);
					_OutObj.FileType = ESCTResourceType::MoveableMesh;
					break;
				}
			}
		}
	}
	FPlatformFileManager::Get().SetPlatformFile(*LocalPlatformFile);
	if (_OutObj.GetFirstObject())
		return true;
	return false;
}

AActor* USCTXRResourceManager::CreateActorInternal(FSCTVRSObject* _ObjectInfo, int32 _ObjID, int32 _SynID, FVector _Location, FRotator _Rotation, FVector _Scale)
{
	AActor* CreatedActor = NULL;
	if (_ObjectInfo->FileType == ESCTResourceType::MoveableMesh)
	{
		UStaticMesh* SM = Cast<UStaticMesh>(_ObjectInfo->GetFirstObject());
		if (SM != NULL)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnInfo.bNoFail = true;
			SpawnInfo.ObjectFlags = RF_Transient;
			AStaticMeshActor* SMA = CurWorld->SpawnActor<AStaticMeshActor>(SpawnInfo);
			SMA->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
			SMA->SetActorLocation(_Location);
			SMA->SetActorRotation(_Rotation);
			SMA->SetActorScale3D(_Scale);
			SMA->GetStaticMeshComponent()->SetStaticMesh(SM);
			SMA->GetStaticMeshComponent()->SetCollisionProfileName("BlockAllDynamic");
			SMA->GetStaticMeshComponent()->SetCollisionObjectType(ECC_VRSSM);
			SMA->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_VRSSM, ECR_Block);
			SMA->Tags.Reset();
			SMA->Tags.Add(FName("MoveableMeshActor"));
			for (int32 i = 0; i < _ObjectInfo->MaterialList.Num(); i++)
			{
				SMA->GetStaticMeshComponent()->SetMaterialByName(FName(*_ObjectInfo->MaterialList[i].SlotName), _ObjectInfo->MaterialList[i].DIM);
			}

			CreatedActor = SMA;
			CreatedActor->SetObjID(_ObjID);
			CreatedActor->SetSynID(_SynID);

			if (bDIYHome)
				SMA->GetStaticMeshComponent()->SetMobility(EComponentMobility::Static);
			else
				SMA->GetStaticMeshComponent()->IndirectLightingCacheQuality = EIndirectLightingCacheQuality::ILCQ_Volume;
		}
	}
	else if (_ObjectInfo->FileType == ESCTResourceType::BlueprintClass)
	{
		UBlueprintGeneratedClass* BP = Cast<UBlueprintGeneratedClass>(_ObjectInfo->GetFirstObject());
		if (BP != NULL)
		{
			AActor* BlueprintActor = CurWorld->SpawnActor<AActor>(BP, _Location, _Rotation);
			//如果是交互物体，把SkeletalObject和AnimSequenceObject传入InitActor函数
			if (_ObjectInfo->GetSkeletalMeshObject())
			{
				UFunction* Function = BlueprintActor->FindFunction(FName(TEXT("InitActor")));
				if (Function)
				{
					struct FInitActorPars
					{
						USkeletalMesh* SkObj;
						UAnimSequence* AnimObj;
						FVector BoundOrigin;
						FVector BoundExtent;
					};
					FInitActorPars Pars;
					Pars.SkObj = _ObjectInfo->GetSkeletalMeshObject();
					Pars.AnimObj = _ObjectInfo->GetAnimSequenceObject();
					Pars.BoundOrigin = _ObjectInfo->BoundOrigin;
					Pars.BoundExtent = _ObjectInfo->BoundExtent;
					BlueprintActor->ProcessEvent(Function, &Pars);
				}
			}
			else if (_ObjectInfo->GetLampMeshObject())
			{
				UFunction* InitFunction = BlueprintActor->FindFunction(FName(TEXT("InitActor")));
				if (InitFunction)
				{
					struct FInitActorPars
					{
						UStaticMesh* LampObj;
						FLinearColor LightColor;
						float AttenuationRadius;
						float PointLightIntensity;
						float SpotLightIntensity;
						float SpotLightConeAngle;
						TArray<FTransform> PointLightTransforms;
						TArray<FTransform> SpotLightTransforms;
					};
					FInitActorPars Pars;
					Pars.LampObj = _ObjectInfo->GetLampMeshObject();
					Pars.LightColor = _ObjectInfo->LampColor;
					Pars.AttenuationRadius = 1000.f;
					Pars.PointLightIntensity = _ObjectInfo->LampIntensity;
					Pars.SpotLightIntensity = _ObjectInfo->LampIntensity / 120.f;
					Pars.SpotLightConeAngle = 80.f;
					Pars.PointLightTransforms = _ObjectInfo->PointArray;
					Pars.SpotLightTransforms = _ObjectInfo->SpotArray;
					BlueprintActor->ProcessEvent(InitFunction, &Pars);
				}
			}

			BlueprintActor->SetActorScale3D(_Scale);
			//遍历该蓝图，把SMC设置为ECC_VRSSM
			TArray<USceneComponent*> ChildList;
			BlueprintActor->GetRootComponent()->GetChildrenComponents(true, ChildList);
			ChildList.Add(BlueprintActor->GetRootComponent());
			for (int32 i = 0; i < ChildList.Num(); i++)
			{
				UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(ChildList[i]);
				if (SMC)
				{
					SMC->SetCollisionProfileName("BlockAllDynamic");
					SMC->SetCollisionObjectType(ECC_VRSSM);
					SMC->SetCollisionResponseToChannel(ECC_VRSSM, ECR_Block);
				}
				//如果是工具上传的可交互蓝图，则遍历到其中的SkeletalMeshComponent，然后设置材质
				if (_ObjectInfo->GetSkeletalMeshObject())
				{
					USkeletalMeshComponent* SKC = Cast<USkeletalMeshComponent>(ChildList[i]);
					if (SKC)
					{
						for (int32 j = 0; j < _ObjectInfo->MaterialList.Num(); j++)
						{
							SKC->SetMaterialByName(FName(*_ObjectInfo->MaterialList[j].SlotName), _ObjectInfo->MaterialList[j].DIM);
						}
					}
				}
				if (_ObjectInfo->GetLampMeshObject())
				{
					UStaticMeshComponent* SMC2 = Cast<UStaticMeshComponent>(ChildList[i]);
					if (SMC2)
					{
						for (int32 j = 0; j < _ObjectInfo->MaterialList.Num(); j++)
						{
							SMC2->SetMaterialByName(FName(*_ObjectInfo->MaterialList[j].SlotName), _ObjectInfo->MaterialList[j].DIM);
						}
					}
				}
			}
			BlueprintActor->Tags.Reset();
			BlueprintActor->Tags.Add(FName("BlueprintVRSActor"));

			CreatedActor = BlueprintActor;
			CreatedActor->SetObjID(_ObjID);
			CreatedActor->SetSynID(_SynID);

			if (_ObjectInfo->GetLampMeshObject())
			{
				//获取灯具属性参数，并应用
				TArray<FSCTXRParameter> ModelSettingsList;
				TSharedPtr<FSCTContentItemSpace::FContentItem> ResultSynData = GetContentItemFromID(_SynID);

				
				if (ResultSynData.IsValid() && !StaticCastSharedPtr<FSCTContentItemSpace::FModelRes>(ResultSynData->ResObj)->LightParameter.IsEmpty())
				{
					ConvertJsonToModelSettingsList(StaticCastSharedPtr<FSCTContentItemSpace::FModelRes>(ResultSynData->ResObj)->LightParameter, ModelSettingsList);
					SetActorLightComponentParameters(BlueprintActor, ModelSettingsList);
				}
			}
		}
	}
	else if (_ObjectInfo->FileType == ESCTResourceType::Material)
	{
		UMaterialInterface* Mat = Cast<UMaterialInterface>(_ObjectInfo->GetFirstObject());		
	}	
	return CreatedActor;
}

UMaterialInterface* USCTXRResourceManager::CreateMaterialInternal(FSCTVRSObject* _ObjectInfo, int32 _ObjID, int32 _SynID)
{
	UMaterialInterface* NewMaterial = NULL;
	
	if (_ObjectInfo->FileType == ESCTResourceType::Material)
	{
		NewMaterial = Cast<UMaterialInterface>(_ObjectInfo->GetFirstObject());
		if (NewMaterial != NULL)
		{
			NewMaterial->SetSynID(_SynID);
		}
	}
	return NewMaterial;
}

void USCTXRResourceManager::MountPakReturnFiles(FString _PakPath, TArray<FString>& OutPakFilenameList, int32 _Order, bool _IDontWantToMount)
{
	FPakFile* PakFile = new FPakFile(PakPlatform.Get(), *_PakPath, false);

	if (FPlatformProperties::RequiresCookedData())
	{
		FString MP = PakFile->GetMountPoint();
		int32 pos1 = MP.Find(TEXT("/Content/"), ESearchCase::IgnoreCase);
		FString MP2 = MP.RightChop(pos1);
		MP = TEXT("../../../XR") + MP2;
		if (!_IDontWantToMount)
			PakPlatform.Get()->Mount(*_PakPath, _Order, *MP);
	}
	else
	{
		FString MP = PakFile->GetMountPoint();
		int32 pos1 = MP.Find(TEXT("/Content/"), ESearchCase::IgnoreCase);
		FString MP2 = MP.RightChop(pos1 + 1);
		MP = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + MP2;
		if (!_IDontWantToMount)
			PakPlatform.Get()->Mount(*_PakPath, _Order, *MP);
	}

	PakFile->FindFilesAtPath(OutPakFilenameList, *PakFile->GetMountPoint(), true, false, true);
	delete PakFile;
	PakFile = nullptr;
}

void USCTXRResourceManager::MountPak(FString _PakPath)
{
	FPakFile* PakFile = new FPakFile(PakPlatform.Get(), *_PakPath, false);

	if (FPlatformProperties::RequiresCookedData())
	{
		FString MP = PakFile->GetMountPoint();
		int32 pos1 = MP.Find(TEXT("/Content/"), ESearchCase::IgnoreCase);
		FString MP2 = MP.RightChop(pos1);
		MP = TEXT("../../../XR") + MP2;
		PakPlatform.Get()->Mount(*_PakPath, 3, *MP);
	}
	else
	{
		FString MP = PakFile->GetMountPoint();
		int32 pos1 = MP.Find(TEXT("/Content/"), ESearchCase::IgnoreCase);
		FString MP2 = MP.RightChop(pos1 + 1);
		MP = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + MP2;
		PakPlatform.Get()->Mount(*_PakPath, 3, *MP);
	}

	delete PakFile;
	PakFile = nullptr;
}

void USCTXRResourceManager::LoadCommonData()
{
	FPlatformFileManager::Get().SetPlatformFile(*PakPlatform);

	//当前只调用Common_9和Common_60
	TArray<FString> Filenames;
	MountPakReturnFiles(FPaths::ProjectContentDir() + "DBJCache/Common/Common_60.pak", Filenames, 4);

	//加载18种常用材质球，用来随时更换
	for (int32 i = 0; i < StaticCommonMaterialList.Num(); i++)
	{
		StaticCommonMaterialList[i].MI = (UMaterialInterface*)StaticLoadObject(UMaterialInterface::StaticClass(), NULL, *StaticCommonMaterialList[i].MaterialPath);
	}

	//加载IES资源
	IESList.Reset();
	for (int32 i = 0; i < 30; i++)
	{
		FString TempName = FString::Printf(TEXT("/Game/DLC/Common/60/IES/T_IES_%d"), i);
		IESList.Add((UTexture*)StaticLoadObject(UTexture::StaticClass(), NULL, *TempName));
	}

	//加载EmptyTexture法线资源
	EmptyTexture = (UTexture*)StaticLoadObject(UTexture::StaticClass(), NULL, TEXT("/Game/DLC/Common/60/NormalMap/T_未设置"));

	//加载Cloth法线资源
	NormalMapList_Cloth.Reset();
	for (int32 i = 1; i <= 8; i++)
	{
		FString TempName = FString::Printf(TEXT("/Game/DLC/Common/60/NormalMap/Cloth/T_布料%d"), i);
		NormalMapList_Cloth.Add((UTexture*)StaticLoadObject(UTexture::StaticClass(), NULL, *TempName));
	}

	//加载Leather法线资源
	NormalMapList_Leather.Reset();
	for (int32 i = 1; i <= 6; i++)
	{
		FString TempName = FString::Printf(TEXT("/Game/DLC/Common/60/NormalMap/Leather/T_皮革%d"), i);
		NormalMapList_Leather.Add((UTexture*)StaticLoadObject(UTexture::StaticClass(), NULL, *TempName));
	}

	//加载Metal法线资源
	NormalMapList_Metal.Reset();
	for (int32 i = 1; i <= 1; i++)
	{
		FString TempName = FString::Printf(TEXT("/Game/DLC/Common/60/NormalMap/Metal/T_金属%d"), i);
		NormalMapList_Metal.Add((UTexture*)StaticLoadObject(UTexture::StaticClass(), NULL, *TempName));
	}

	//加载Wood法线资源
	NormalMapList_Wood.Reset();
	for (int32 i = 1; i <= 11; i++)
	{
		FString TempName = FString::Printf(TEXT("/Game/DLC/Common/60/NormalMap/Wood/T_木材%d"), i);
		NormalMapList_Wood.Add((UTexture*)StaticLoadObject(UTexture::StaticClass(), NULL, *TempName));
	}

	FPlatformFileManager::Get().SetPlatformFile(*LocalPlatformFile);
}

void USCTXRResourceManager::UnloadModelPak()
{
	for (auto& It : ObjList)
	{
		PakPlatform->Unmount(*It.FilePath);
	}
	ObjList.Reset();
	SynList.Reset();
	//CommentateActors.Reset();
}

void USCTXRResourceManager::SetMaterialRotation(UMaterialInstanceDynamic* _MID, float _Angle/*, bool _bUseProjectiveUV*/)
{
	if (_MID)
	{
		//如果角度变为90度，或从90度变到非90度，则切换UV
		float OldAngle = 0.f;
		//_MID->GetScalarParameterValue(TEXT("Color_Map_RotateUV"), OldAngle);
		_MID->GetScalarParameterValue(TEXT("Diffuse_UV_Rotate"), OldAngle);
		//检查当前材质是否开启了投射UV
		float UseProjectiveUV = 0.f;
		_MID->GetScalarParameterValue(TEXT("UseProjectiveUV"), UseProjectiveUV);

		int32 OldAngleEx = (int32)(OldAngle*360.f)%180;
		int32 AngleEx = (int32)(_Angle*360.f)%180;

		int32 TempA = FMath::Abs(OldAngleEx % 180);
		int32 TempB = FMath::Abs(AngleEx % 180);
		if ( (TempA %180 != 90 && TempB %180 == 90) ||
			 (TempA %180 == 90 && TempB %180 != 90))
		{
			//SwitchMaterialUV(_MID);
		}
		if (UseProjectiveUV == 1.f)
		{
			_MID->SetScalarParameterValue(TEXT("Projective_Diffuse_UV_Rotate"), _Angle);
			_MID->SetScalarParameterValue(TEXT("Projective_NormalMap_UV_Ratote"), _Angle);
		}
		else
		{
			_MID->SetScalarParameterValue(TEXT("AO_Map_RotateUV"), _Angle);
			_MID->SetScalarParameterValue(TEXT("Diffuse_UV_Rotate"), _Angle);
			_MID->SetScalarParameterValue(TEXT("Metalic_Map_RotateUV"), _Angle);
			_MID->SetScalarParameterValue(TEXT("NormalMap_UV_Ratote"), _Angle);
			_MID->SetScalarParameterValue(TEXT("Normal_Map_Detail_RotateUV"), _Angle);
			//_MID->SetScalarParameterValue(TEXT("Roughness_Map_RotateUV"), _Angle);
			_MID->SetScalarParameterValue(TEXT("Specular_Map_RotateUV"), _Angle);
		}
// 		else
// 		{
// 			_Angle = _Angle / 360.f;
// 			_MID->SetScalarParameterValue(TEXT("AO_Map_RotateUV"), _Angle);
// 			_MID->SetScalarParameterValue(TEXT("Color_Map_RotateUV"), _Angle);
// 			_MID->SetScalarParameterValue(TEXT("Metalic_Map_RotateUV"), _Angle);
// 			_MID->SetScalarParameterValue(TEXT("Normal_Map_RotateUV"), _Angle);
// 			_MID->SetScalarParameterValue(TEXT("Normal_Map_Detail_RotateUV"), _Angle);
// 			_MID->SetScalarParameterValue(TEXT("Roughness_Map_RotateUV"), _Angle);
// 			_MID->SetScalarParameterValue(TEXT("Specular_Map_RotateUV"), _Angle);
// 		}
	}
}

void USCTXRResourceManager::SetMaterialOffsetU(UMaterialInstanceDynamic* _MID, float _OffsetU)
{
	if (_MID)
	{
		//检查当前材质是否开启了投射UV
		float UseProjectiveUV = 0.f;
		_MID->GetScalarParameterValue(TEXT("UseProjectiveUV"), UseProjectiveUV);

		if (UseProjectiveUV == 1.f)
		{
			_MID->SetScalarParameterValue(TEXT("Projective_Diffuse_UV_OffsetU"), _OffsetU);
			_MID->SetScalarParameterValue(TEXT("Projective_NormalMap_UV_Offset_U"), _OffsetU);
		}
		else
		{
			_MID->SetScalarParameterValue(TEXT("Diffuse_UV_OffsetU"), _OffsetU);
			_MID->SetScalarParameterValue(TEXT("NormalMap_UV_Offset_U"), _OffsetU);
		}
		//else
		//{
		//	_MID->SetScalarParameterValue(TEXT("Color_Map_Offset_U"), _OffsetU);
		//	_MID->SetScalarParameterValue(TEXT("Normal_Map_Offset_U"), _OffsetU);
		//}
	}
}

void USCTXRResourceManager::SetMaterialOffsetV(UMaterialInstanceDynamic* _MID, float _OffsetV)
{
	if (_MID)
	{
		//检查当前材质是否开启了投射UV
		float UseProjectiveUV = 0.f;
		_MID->GetScalarParameterValue(TEXT("UseProjectiveUV"), UseProjectiveUV);

		if (UseProjectiveUV == 1.f)
		{
			_MID->SetScalarParameterValue(TEXT("Projective_Diffuse_UV_OffsetV"), _OffsetV);
			_MID->SetScalarParameterValue(TEXT("Projective_NormalMap_UV_Offset_V"), _OffsetV);
		}
		else
		{
			_MID->SetScalarParameterValue(TEXT("Diffuse_UV_OffsetV"), _OffsetV);
			_MID->SetScalarParameterValue(TEXT("NormalMap_UV_Offset_V"), _OffsetV);
		}
		//else
		//{
		//	_MID->SetScalarParameterValue(TEXT("Color_Map_Offset_V"), _OffsetV);
		//	_MID->SetScalarParameterValue(TEXT("Normal_Map_Offset_V"), _OffsetV);
		//}
	}
}

void USCTXRResourceManager::SetMaterialTilingU(UMaterialInstanceDynamic* _MID, float _TilingU)
{
	if (_MID)
	{
		//检查当前材质是否开启了投射UV
		float UseProjectiveUV = 0.f;
		_MID->GetScalarParameterValue(TEXT("UseProjectiveUV"), UseProjectiveUV);

		if (UseProjectiveUV == 1.f)
		{
			_MID->SetScalarParameterValue(TEXT("Projective_Diffuse_UV_TilingU"), _TilingU);
			_MID->SetScalarParameterValue(TEXT("Projective_NormalMap_UV_Tiling_U"), _TilingU);
		}
		else
		{
			_MID->SetScalarParameterValue(TEXT("Diffuse_UV_TilingU"), _TilingU);
			_MID->SetScalarParameterValue(TEXT("NormalMap_UV_Tiling_U"), _TilingU);
		}
		//else
		//{
		//	_MID->SetScalarParameterValue(TEXT("Color_Map_Tiling_U"), _TilingU);
		//	_MID->SetScalarParameterValue(TEXT("Normal_Map_Tiling_U"), _TilingU);
		//}
	}
}

void USCTXRResourceManager::SetMaterialTilingV(UMaterialInstanceDynamic* _MID, float _TilingV)
{
	if (_MID)
	{
		//检查当前材质是否开启了投射UV
		float UseProjectiveUV = 0.f;
		_MID->GetScalarParameterValue(TEXT("UseProjectiveUV"), UseProjectiveUV);

		if (UseProjectiveUV == 1.f)
		{
			_MID->SetScalarParameterValue(TEXT("Projective_Diffuse_UV_TilingV"), _TilingV);
			_MID->SetScalarParameterValue(TEXT("Projective_NormalMap_UV_Tiling_V"), _TilingV);
		}
		else
		{
			_MID->SetScalarParameterValue(TEXT("Diffuse_UV_TilingV"), _TilingV);
			_MID->SetScalarParameterValue(TEXT("NormalMap_UV_Tiling_V"), _TilingV);
		}
		//else
		//{
		//	_MID->SetScalarParameterValue(TEXT("Color_Map_Tiling_V"), _TilingV);
		//	_MID->SetScalarParameterValue(TEXT("Normal_Map_Tiling_V"), _TilingV);
		//}
	}
}

float USCTXRResourceManager::GetMaterialRotation(UMaterialInstanceDynamic* _MID)
{
	float UseProjectiveUV = 0.f;
	float RotateAngle = 0;
	_MID->GetScalarParameterValue(TEXT("UseProjectiveUV"), UseProjectiveUV);

	if (UseProjectiveUV == 1.f)
		_MID->GetScalarParameterValue(TEXT("Projective_Diffuse_UV_Rotate"), RotateAngle);
	else
		_MID->GetScalarParameterValue(TEXT("Diffuse_UV_Rotate"), RotateAngle);
	//else
	//{
	//	_MID->GetScalarParameterValue(TEXT("Color_Map_RotateUV"), RotateAngle);
	//	RotateAngle *= 360.f;
	//}
	return RotateAngle;
}

float USCTXRResourceManager::GetMaterialOffsetU(UMaterialInstanceDynamic* _MID)
{
	float UseProjectiveUV = 0.f;
	float OffsetU = 0.f;
	_MID->GetScalarParameterValue(TEXT("UseProjectiveUV"), UseProjectiveUV);

	if (UseProjectiveUV == 1.f)
		_MID->GetScalarParameterValue(TEXT("Projective_Diffuse_UV_OffsetU"), OffsetU);
	else
		_MID->GetScalarParameterValue(TEXT("Diffuse_UV_OffsetU"), OffsetU);

	//else
	//	_MID->GetScalarParameterValue(TEXT("Color_Map_Offset_U"), OffsetU);

	return OffsetU;
}

float USCTXRResourceManager::GetMaterialOffsetV(UMaterialInstanceDynamic* _MID)
{
	float UseProjectiveUV = 0.f;
	float OffsetV = 0.f;
	_MID->GetScalarParameterValue(TEXT("UseProjectiveUV"), UseProjectiveUV);

	if (UseProjectiveUV == 1.f)
		_MID->GetScalarParameterValue(TEXT("Projective_Diffuse_UV_OffsetV"), OffsetV);
	else
		_MID->GetScalarParameterValue(TEXT("Diffuse_UV_OffsetV"), OffsetV);
	//else
	//	_MID->GetScalarParameterValue(TEXT("Color_Map_Offset_V"), OffsetV);

	return OffsetV;
}

float USCTXRResourceManager::GetMaterialTilingU(UMaterialInstanceDynamic* _MID)
{
	float UseProjectiveUV = 0.f;
	float TilingU = 0.f;
	_MID->GetScalarParameterValue(TEXT("UseProjectiveUV"), UseProjectiveUV);

	if (UseProjectiveUV == 1.f)
		_MID->GetScalarParameterValue(TEXT("Projective_Diffuse_UV_TilingU"), TilingU);
	else
		_MID->GetScalarParameterValue(TEXT("Diffuse_UV_TilingU"), TilingU);
	//else
	//	_MID->GetScalarParameterValue(TEXT("Color_Map_Tiling_U"), TilingU);

	return TilingU;
}

float USCTXRResourceManager::GetMaterialTilingV(UMaterialInstanceDynamic* _MID)
{
	float UseProjectiveUV = 0.f;
	float TilingV = 0.f;
	_MID->GetScalarParameterValue(TEXT("UseProjectiveUV"), UseProjectiveUV);

	if (UseProjectiveUV == 1.f)
		_MID->GetScalarParameterValue(TEXT("Projective_Diffuse_UV_TilingV"), TilingV);
	else
		_MID->GetScalarParameterValue(TEXT("Diffuse_UV_TilingV"), TilingV);
	//else
	//	_MID->GetScalarParameterValue(TEXT("Color_Map_Tiling_V"), TilingV);

	return TilingV;
}

bool USCTXRResourceManager::GetMaterialUseProjectiveUV(UMaterialInstanceDynamic* _MID)
{
	float UseProjectiveUV = 0.f;
	_MID->GetScalarParameterValue(TEXT("UseProjectiveUV"), UseProjectiveUV);

	return UseProjectiveUV == 1.f ? true : false;
}

void USCTXRResourceManager::ClearActorOverridenMaterials(AActor* _Actor)
{
	if (_Actor)
	{
		FSCTVRSObject* ResultObj = GetObjFromObjID(_Actor->GetObjID());
		if (ResultObj)
		{
			UMeshComponent* TheMeshCOM = NULL;
			TInlineComponentArray<USceneComponent*> SceneComponents;
			_Actor->GetComponents(SceneComponents);
			//如果是工具制作的标准化模型，只需要到第一个MeshComponent，获取它的材质列表
			if (ResultObj->bNormalized)
			{
				for (USceneComponent* SceneComp : SceneComponents)
				{
					UMeshComponent* MeshCOM = Cast<UMeshComponent>(SceneComp);
					if (MeshCOM)
					{
						TheMeshCOM = MeshCOM;
						break;
					}
				}
			}
			//如果是非标准化的
			else
			{
				//当是AStaticMeshActor的时候，获取贴在StaticMeshComponent上的壁纸瓷砖类的材质
				AStaticMeshActor* SMA = Cast<AStaticMeshActor>(_Actor);
				if (SMA)
				{
					TheMeshCOM = SMA->GetStaticMeshComponent();
				}
				//如果是蓝图Actor，则需要找它的第一个MeshComponent，获取它的材质列表
				else
				{
					for (USceneComponent* SceneComp : SceneComponents)
					{
						UMeshComponent* MeshCOM = Cast<UMeshComponent>(SceneComp);
						if (MeshCOM)
						{
							TheMeshCOM = MeshCOM;
							break;
						}
					}
				}
			}

			if (ResultObj->bNormalized)
			{
				//遍历材质列表，显示材质按钮
				for (int32 i = 0; i < ResultObj->MaterialList.Num(); i++)
				{
					TheMeshCOM->SetMaterialByName(FName(*ResultObj->MaterialList[i].SlotName), ResultObj->MaterialList[i].DIM);
				}
			}
			else
			{
				TheMeshCOM->OverrideMaterials.Reset();
			}
		}
		else
		{
			ESCTActorType ActorType = GetActorType(_Actor);
			if (ActorType == ESCTActorType::Blueprint_Window)
			{
				UMeshComponent* TheMeshCOM = NULL;
				TInlineComponentArray<USceneComponent*> SceneComponents;
				_Actor->GetComponents(SceneComponents);
				//获取目标MeshComponent
				for (USceneComponent* SceneComp : SceneComponents)
				{
					UMeshComponent* MeshCOM = Cast<UMeshComponent>(SceneComp);
					if (MeshCOM)
					{
						TheMeshCOM = MeshCOM;
						break;
					}
				}
				if (TheMeshCOM)
				{
					TheMeshCOM->OverrideMaterials.Reset();
					TheMeshCOM->RecreateRenderState_Concurrent();
				}
			}
		}
	}
}

void USCTXRResourceManager::ResetActorTransform(AActor* _Actor)
{
	if (_Actor)
	{
		FTransform NewTransform = _Actor->GetActorTransform();
		NewTransform.SetRotation(FRotator(0,0,0).Quaternion());
		NewTransform.SetScale3D(FVector(1,1,1));
		_Actor->SetActorTransform(NewTransform);
	}
}

void USCTXRResourceManager::SwitchMaterialUV(UMaterialInstanceDynamic* _MID)
{
	float OldU, OldV;

	//_MID->GetScalarParameterValue(TEXT("Color_Map_Tiling_U"), OldU);
	//_MID->GetScalarParameterValue(TEXT("Color_Map_Tiling_V"), OldV);

	//_MID->SetScalarParameterValue(TEXT("Color_Map_Tiling_U"), OldV);
	//_MID->SetScalarParameterValue(TEXT("Color_Map_Tiling_V"), OldU);
	//_MID->SetScalarParameterValue(TEXT("Normal_Map_Tiling_U"), OldV);
	//_MID->SetScalarParameterValue(TEXT("Normal_Map_Tiling_V"), OldU);

	//考虑了如果使用投射UV
	//float UseProjectiveUV = 0.f;
	//_MID->GetScalarParameterValue(TEXT("UseProjectiveUV"), UseProjectiveUV);
	//if (UseProjectiveUV == 1.f)
	//{

	OldU = GetMaterialTilingU(_MID);
	OldV = GetMaterialTilingV(_MID);

	SetMaterialTilingU(_MID, OldV);
	SetMaterialTilingV(_MID, OldU);

		//_MID->GetScalarParameterValue(TEXT("Diffuse_UV_TilingU"), OldU);
		//_MID->GetScalarParameterValue(TEXT("Diffuse_UV_TilingV"), OldV);

		//_MID->SetScalarParameterValue(TEXT("Diffuse_UV_TilingU"), OldV);
		//_MID->SetScalarParameterValue(TEXT("Diffuse_UV_TilingV"), OldU);
		//_MID->SetScalarParameterValue(TEXT("NormalMap_UV_Tiling_U"), OldV);
		//_MID->SetScalarParameterValue(TEXT("NormalMap_UV_Tiling_V"), OldU);
	//}
}

FString USCTXRResourceManager::ConvertOldMaterialNameToV7(FString _OldMaterialName)
{
	FString NewMaterialName;
	if (_OldMaterialName == TEXT("Wallpaper") || _OldMaterialName == TEXT("Wallpaint"))
		NewMaterialName = TEXT("V7_Base_WallPaper");/*TEXT("V7_Base_Plastic");*/
	else if (_OldMaterialName == TEXT("Floor"))
		NewMaterialName = TEXT("V7_Base_Wood");
	else if (_OldMaterialName == TEXT("GroundTile"))
		NewMaterialName = TEXT("V7_Base_Ceramics");
	else
		NewMaterialName = _OldMaterialName;
	return NewMaterialName;
}

int32 USCTXRResourceManager::ConvertOldMaterialIDToV7(int32 _OldID)
{
	int32 NewID = 0;
	if (_OldID <= 19)
	{
		if (_OldID == 0)
			NewID = 106;
		else if (_OldID == 1)
			NewID = 107;
		else if (_OldID == 2)
			NewID = 100;
		else if (_OldID == 3)
			NewID = 100;
		else if (_OldID == 4)
			NewID = 103;
		else if (_OldID == 5)
			NewID = 104;
		else if (_OldID == 6)
			NewID = 105;
		else if (_OldID == 7)
			NewID = 500;
		else if (_OldID == 8)
			NewID = 200;
		else if (_OldID == 9)
			NewID = 600;
		else if (_OldID == 10)
			NewID = 603;
		else if (_OldID == 11)
			NewID = 604;
		else if (_OldID == 12)
			NewID = 605;
		else if (_OldID == 13)
			NewID = 400;
		else if (_OldID == 14)
			NewID = 300;
		else if (_OldID == 15)
			NewID = 729;
		else if (_OldID == 16)
			NewID = 101;
		else if (_OldID == 17)
			NewID = 102;
		else if (_OldID == 18)
			NewID = 601;
		else if (_OldID == 19)
			NewID = 602;
	}
	else
	{
		NewID = _OldID;
	}
	return NewID;
}

float USCTXRResourceManager::ConvertOldOffsetToV7(float _OldOffset)
{
	return FMath::Abs(_OldOffset) * 100.f;
}

float USCTXRResourceManager::ConvertOldProjectiveTilingToV7(float _OldTiling)
{
	return _OldTiling * 0.4f;
}

bool USCTXRResourceManager::ConvertJsonToModelMaterialList(bool _bConverToV7, FString _ParaStr, TArray<FSCTModelMaterialData>& _OutMaterialList)
{
	if (!_ParaStr.IsEmpty())
	{
		_OutMaterialList.Empty();
		TSharedPtr<FJsonObject> JSONObject;
		TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(_ParaStr);
		bool readresult = FJsonSerializer::Deserialize(Reader, JSONObject);
		if (readresult)
		{
			const TArray<TSharedPtr<FJsonValue>> *arrayPtr = nullptr;
			arrayPtr = &JSONObject->GetArrayField("D");
			if (arrayPtr != nullptr)
			{
				//遍历材质数组
				for (int32 i = 0; i < (*arrayPtr).Num(); ++i)
				{
					FSCTModelMaterialData MaterialItem;
					const TArray<TSharedPtr<FJsonValue>> *arrayPtr2 = nullptr;
					arrayPtr2 = &(*arrayPtr)[i]->AsArray();
					if (arrayPtr2 != nullptr)
					{
						//遍历每个材质的N个材质参数
						for (int32 j = 0; j < (*arrayPtr2).Num(); ++j)
						{
							FString ParaItemStr = (*arrayPtr2)[j]->AsString();
							//查看第一个字段是否为SlotName
							if (j==0 && ParaItemStr.StartsWith(TEXT("SN:")))
							{
								if (ParaItemStr.RemoveFromStart(TEXT("SN:")))
								{
									MaterialItem.SlotName = ParaItemStr;
									continue;
								}
							}
							//解析字段
							int32 pos = ParaItemStr.Find(TEXT("*"));
							if (pos != -1)
							{
								FSCTXRParameter ParaItem;
								//解析出该参数的对应的序号，到StaticMIParaList查找，得出它是什么类型
								FString IDStr = ParaItemStr.Mid(0, pos);
								int32 ID = FCString::Atoi(*IDStr);
								ParaItemStr = ParaItemStr.RightChop(pos + 1);

								//此处做一个旧版本ID到新材质系统V7版本的映射
								if (_bConverToV7)
									ID = ConvertOldMaterialIDToV7(ID);

								FSCTXRParameter* result2 = StaticMIParaList.Find(ID);
								if (result2)
								{
									ParaItem.ID = ID;
									if (result2->VarType == FSCTXRParameter::E_Float)
									{
										float FValue = FCString::Atof(*ParaItemStr);
										ParaItem.VarType = FSCTXRParameter::E_Float;
										ParaItem.Value = FValue;
										ParaItem.ServerValue = FValue;
										ParaItem.ParaName = result2->ParaName;
									}
									else if (result2->VarType == FSCTXRParameter::E_Vec3)
									{
										FVector VectorValue;
										//解析Vector类型的XYZ三个值
										pos = ParaItemStr.Find(TEXT("-"));
										if (pos != -1)
										{
											FString ValueStr = ParaItemStr.Mid(0, pos);
											ParaItemStr = ParaItemStr.RightChop(pos + 1);
											VectorValue.X = FCString::Atof(*ValueStr);
										}
										pos = ParaItemStr.Find(TEXT("-"));
										if (pos != -1)
										{
											FString ValueStr = ParaItemStr.Mid(0, pos);
											ParaItemStr = ParaItemStr.RightChop(pos + 1);
											VectorValue.Y = FCString::Atof(*ValueStr);
										}
										if (!ParaItemStr.IsEmpty())
										{
											VectorValue.Z = FCString::Atof(*ParaItemStr);
										}
										ParaItem.VarType = FSCTXRParameter::E_Vec3;
										ParaItem.Color = VectorValue;
										ParaItem.ServerColor = VectorValue;
										ParaItem.ParaName = result2->ParaName;
									}
									else if (result2->VarType == FSCTXRParameter::E_Bool)
									{
										float FValue = FCString::Atof(*ParaItemStr);
										ParaItem.VarType = FSCTXRParameter::E_Bool;
										ParaItem.bCheck = FValue == 1.f;
										ParaItem.bServerCheck = ParaItem.bCheck;
										ParaItem.bOriginalCheck = ParaItem.bCheck;
										ParaItem.ParaName = result2->ParaName;
									}
								}
								MaterialItem.ParaList.Add(ParaItem);
							}
						}
					}

					_OutMaterialList.Add(MaterialItem);
				}
			}

			return true;
		}
	}
	return false;
}

FString USCTXRResourceManager::ConvertModelMaterialListToJson(TArray<FSCTModelMaterialData>& _MaterialList, bool _bOnlyServerDirty, bool _bOnlyOriginalDirty)
{
	FString JStr;
	bool bEmpty = true;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JStr);
	JsonWriter->WriteObjectStart();
	//------------------------------------------------parameter
	JsonWriter->WriteArrayStart(TEXT("D"));
	for (auto& It : _MaterialList)
	{
		TArray<FString> ValueList;
		for (auto& It2 : It.ParaList)
		{
			if (_bOnlyServerDirty)
			{
				if (It2.CheckServerDirty())
				{
					FString ValueStr = FString::Printf(TEXT("%d*"), It2.ID);
					if (It2.VarType == FSCTXRParameter::E_Float)
						ValueStr.Append(FString::Printf(TEXT("%.2f"), It2.Value));
					else if (It2.VarType == FSCTXRParameter::E_Vec3)
						ValueStr.Append(FString::Printf(TEXT("%.2f-%.2f-%.2f"), It2.Color.X, It2.Color.Y, It2.Color.Z));
					else if (It2.VarType == FSCTXRParameter::E_Bool)
						ValueStr.Append(FString::Printf(TEXT("%.f"), It2.bCheck ? 1.f : 0.f));
					ValueList.Add(ValueStr);
					bEmpty = false;
				}
			}
			else if (_bOnlyOriginalDirty)
			{
				if (It2.CheckOriginalDirty())
				{
					FString ValueStr = FString::Printf(TEXT("%d*"), It2.ID);
					if (It2.VarType == FSCTXRParameter::E_Float)
						ValueStr.Append(FString::Printf(TEXT("%.2f"), It2.Value));
					else if (It2.VarType == FSCTXRParameter::E_Vec3)
						ValueStr.Append(FString::Printf(TEXT("%.2f-%.2f-%.2f"), It2.Color.X, It2.Color.Y, It2.Color.Z));
					else if (It2.VarType == FSCTXRParameter::E_Bool)
						ValueStr.Append(FString::Printf(TEXT("%.f"), It2.bCheck ? 1.f : 0.f));
					ValueList.Add(ValueStr);
					bEmpty = false;
				}
			}
			else
			{
				FString ValueStr = FString::Printf(TEXT("%d*"), It2.ID);
				if (It2.VarType == FSCTXRParameter::E_Float)
					ValueStr.Append(FString::Printf(TEXT("%.2f"), It2.Value));
				else if (It2.VarType == FSCTXRParameter::E_Vec3)
					ValueStr.Append(FString::Printf(TEXT("%.2f-%.2f-%.2f"), It2.Color.X, It2.Color.Y, It2.Color.Z));
				else if (It2.VarType == FSCTXRParameter::E_Bool)
					ValueStr.Append(FString::Printf(TEXT("%.f"), It2.bCheck ? 1.f : 0.f));
				ValueList.Add(ValueStr);
			}
		}

		if (ValueList.Num() > 0)
		{
			JsonWriter->WriteArrayStart();
			//把SlotName写进去
			JsonWriter->WriteValue(FString::Printf(TEXT("SN:%s"), *It.SlotName));
			for (auto It2 : ValueList)
				JsonWriter->WriteValue(It2);
			JsonWriter->WriteArrayEnd();
		}
	}
	JsonWriter->WriteArrayEnd();
	//------------------------------------------------parameter
	JsonWriter->WriteObjectEnd();
	JsonWriter->Close();
	//如果没有任何需要记录的，直接清空
	if ((_bOnlyServerDirty || _bOnlyOriginalDirty) && bEmpty)
		JStr = TEXT("");
	return JStr;
}

FSCTModelMaterialData USCTXRResourceManager::CreateMaterialData(FString _MaterialName, FVector2D _TextureSize, UTexture* _DiffuseMap, UTexture* _NormalMap)
{
	//将旧的材质球名称映射到新的V7版本的对应材质球名称
	FString NewMaterialName = ConvertOldMaterialNameToV7(_MaterialName);
	NewMaterialName.Append(TEXT("_MT"));
	FString MatPath = FString::Printf(TEXT("/Game/DLC/Common/9/Materials/%s"), *NewMaterialName);
	UObject* TempObj = StaticLoadObject(UObject::StaticClass(), NULL, *MatPath);
	UMaterialInterface* ParentMat = Cast<UMaterialInterface>(TempObj);

	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(ParentMat, nullptr);
	TArray<FSCTXRParameter> ParaList;
	FSCTCommonMaterialData::EMaterialType MaterialType = FSCTCommonMaterialData::E_None;

	if (MID)
	{
		//油漆特殊处理
		if (_MaterialName == TEXT("Wallpaint"))
		{
			MID->SetScalarParameterValue(TEXT("Diffuse_UseTexture"), 0.f);
			MID->SetVectorParameterValue(TEXT("Diffuse_Color"), FLinearColor(1, 1, 1, 1));
		}
		else
		{
			if (_DiffuseMap)
			{
				MID->SetScalarParameterValue(TEXT("Diffuse_UseTexture"), 1.f);
				MID->SetTextureParameterValue(TEXT("Diffuse_Map"), _DiffuseMap);
}
			if (_NormalMap)
			{
				MID->SetScalarParameterValue(TEXT("Normal_UseTexture"), 1.f);
				MID->SetTextureParameterValue(TEXT("Normal_Map"), _NormalMap);
			}
			SetMaterialTilingU(MID, _TextureSize.X);
			SetMaterialTilingV(MID, _TextureSize.Y);
		}

		//找出当前材质需要暴露的参数
		TArray<int32> IDList;
		int32 i = 0;
		for (auto& It : StaticCommonMaterialList)
		{
			if (It.MaterialName == NewMaterialName)
			{
				IDList = It.ExposedParameters;
				MaterialType = (FSCTCommonMaterialData::EMaterialType)i;
				break;
			}
			i++;
		}
		//将材质球的默认值填充到列表中
		for (auto& It : IDList)
		{
			FSCTXRParameter* result = StaticMIParaList.Find(It);
			if (result)
			{
				float FloatValue = 0.f;
				FLinearColor ColorValue;
				FVector VectorValue;
				if (result->VarType == FSCTXRParameter::E_Float)
				{
					MID->GetScalarParameterValue(FName(*result->ParaName), FloatValue);
					ParaList.Add(FSCTXRParameter(result->ParaDisplayName, result->ParaDisplayNameEN, result->ParaName, result->MinValue, result->MaxValue, FloatValue));
				}
				else if (result->VarType == FSCTXRParameter::E_Vec3)
				{
					MID->GetVectorParameterValue(FName(*result->ParaName), ColorValue);
					VectorValue.X = ColorValue.R;
					VectorValue.Y = ColorValue.G;
					VectorValue.Z = ColorValue.B;
					ParaList.Add(FSCTXRParameter(result->ParaDisplayName, result->ParaDisplayNameEN, result->ParaName, VectorValue));
				}
				ParaList[ParaList.Num() - 1].ID = It;
			}
		}
	}
	return FSCTModelMaterialData(TEXT(""), MaterialType, MID, ParaList);
}

FSCTModelMaterialData USCTXRResourceManager::CreateModelMaterialData(int32 _Version, FXmlNode* _MaterialNode, TMap<FString, UTexture*>& _TextureMap)
{
	if (_Version == 100)
		return CreateModelMaterialData_Internal_BeforeV7(_MaterialNode, _TextureMap);
	else
		return CreateModelMaterialData_Internal_V7(_MaterialNode, _TextureMap);
}

FSCTModelMaterialData USCTXRResourceManager::CreateModelMaterialData_Internal_BeforeV7(FXmlNode* _MaterialNode, TMap<FString, UTexture*>& _TextureMap)
{
	FXmlNode *child = _MaterialNode;

	FString ParentName = child->GetAttribute(TEXT("ParentName"));
	FString SlotName = child->GetAttribute(TEXT("SlotName"));
	FString MaterialCommon = child->GetAttribute(TEXT("Common"));

	if (ParentName == TEXT("undefined") || ParentName == TEXT(""))
	{
		ParentName = TEXT("Base");
		MaterialCommon = TEXT("2");
	}

	FString MatPath = FString::Printf(TEXT("/Game/DLC/Common/%s/Materials/%s"), *MaterialCommon, *ParentName);
	UObject* TempObj = StaticLoadObject(UObject::StaticClass(), NULL, *MatPath);
	UMaterialInterface* ParentMat = Cast<UMaterialInterface>(TempObj);
	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(ParentMat, nullptr);
	//创建贴图，设置材质参数
	const TArray<FXmlNode*> ChildrenNodes = child->GetChildrenNodes();
	TArray<FSCTXRParameter> ParaList;

	for (int32 j = 0; j < ChildrenNodes.Num(); j++)
	{
		FXmlNode *ChildNode = ChildrenNodes[j];

		if (ChildNode->GetTag() == TEXT("Texture"))
		{
			FString UVStr = ChildNode->GetAttribute(TEXT("UV"));
			FString MapType = ChildNode->GetAttribute(TEXT("MapType"));
			FString UseTexture = ChildNode->GetAttribute(TEXT("UseTexture"));
			
			//if (MapType == TEXT("Shadow"))
			//	MapType = TEXT("Opacity")

			float UseTextureCon = FCString::Atof(*UseTexture);
			MID->SetScalarParameterValue(FName(*(MapType + TEXT("_UseTexture"))), UseTextureCon);
			//如果是绘制户型，则AO效果通过Diffuse混合输出。如果是烘焙户型则通过Ambient Occulusion输出。
			if (bDIYHome)
			{
				MID->SetScalarParameterValue(TEXT("AO_OverlayStyle"), 0.f);
			}

			if (UseTexture == TEXT("0"))
			{
				FString RStr = ChildNode->GetAttribute(TEXT("R"));
				int32 RInt = FCString::Atoi(*RStr);
				FString GStr = ChildNode->GetAttribute(TEXT("G"));
				int32 GInt = FCString::Atoi(*GStr);
				FString BStr = ChildNode->GetAttribute(TEXT("B"));
				int32 BInt = FCString::Atoi(*BStr);
				FLinearColor PureColor(FColor(RInt, GInt, BInt));

				FString ColorStr = MapType + TEXT("_Color");
				MID->SetVectorParameterValue(FName(*ColorStr), PureColor);
			}
			else
			{
				FString Filename = ChildNode->GetAttribute(TEXT("Filename"));
				//此处对多层路径进行提出  AAA\\BBB.jpg  --->  BBB.jpg
				int32 pos = Filename.Find(TEXT("\\"));
				if (pos != -1)
				{
					Filename = Filename.RightChop(pos + 1);
				}
				int32 UVInt = FCString::Atoi(*UVStr) - 1;
				FString TextureFileName = FPaths::GetBaseFilename(Filename);
				UTexture* ThisTexture = _TextureMap[TextureFileName];
				if (ThisTexture)
				{
					MID->SetScalarParameterValue(FName(*(MapType + TEXT("_UV"))), UVInt);
					MID->SetTextureParameterValue(FName(*(MapType + TEXT("_Map"))), ThisTexture);
				}
			}
		}
		else if (ChildNode->GetTag() == TEXT("Parameter"))
		{
			FString IDStr2 = ChildNode->GetAttribute(TEXT("ID"));
			TArray<int32> IDList;
			int32 pos = IDStr2.Find(TEXT("-"));
			while (pos != -1)
			{
				FString ThisIDStr = IDStr2.Mid(0, pos);
				int32 ThisID = FCString::Atoi(*ThisIDStr);
				//ThisID = ConvertOldMaterialIDToV7(ThisID);
				IDList.Add(ThisID);
				IDStr2 = IDStr2.RightChop(pos + 1);
				pos = IDStr2.Find(TEXT("-"));
			}
			if (!IDStr2.IsEmpty())
			{
				int32 ThisID = FCString::Atoi(*IDStr2);
				//ThisID = ConvertOldMaterialIDToV7(ThisID);
				IDList.Add(ThisID);
			}
			for (int32 k = 0; k < IDList.Num(); k++)
			{
				FSCTXRParameter* result = StaticMIParaList.Find(IDList[k]);
				if (result)
				{
					float FloatValue = 0.f;
					FLinearColor ColorValue;
					FVector VectorValue;
					if (result->VarType == FSCTXRParameter::E_Float)
					{
						MID->GetScalarParameterValue(FName(*result->ParaName), FloatValue);
						ParaList.Add(FSCTXRParameter(result->ParaDisplayName, result->ParaDisplayNameEN, result->ParaName, result->MinValue, result->MaxValue, FloatValue));
					}
					else if (result->VarType == FSCTXRParameter::E_Vec3)
					{
						MID->GetVectorParameterValue(FName(*result->ParaName), ColorValue);
						VectorValue.X = ColorValue.R;
						VectorValue.Y = ColorValue.G;
						VectorValue.Z = ColorValue.B;
						ParaList.Add(FSCTXRParameter(result->ParaDisplayName, result->ParaDisplayNameEN, result->ParaName, VectorValue));
					}
					ParaList[ParaList.Num() - 1].ID = IDList[k];
				}
			}
		}
	}

	return FSCTModelMaterialData(SlotName, FSCTCommonMaterialData::E_None, MID, ParaList);
}

FSCTModelMaterialData USCTXRResourceManager::CreateModelMaterialData_Internal_V7(FXmlNode* _MaterialNode, TMap<FString, UTexture*>& _TextureMap)
{
	int32 MaterialTypeIndex = GetMaterialTypeIndex(_MaterialNode->GetAttribute("ParentName"));
	FSCTModelMaterialData MaterialData = CreateModelMaterialDataFromName_V7(MaterialTypeIndex);

	if (MaterialData.DIM)
	{
		MaterialData.SlotName = _MaterialNode->GetAttribute("SlotName");
		//创建贴图，设置材质参数
		for (auto& It : _MaterialNode->GetChildrenNodes())
		{
			if (It->GetTag() == "Texture")
			{
				FString UVStr = It->GetAttribute("UV");
				FString MapType = It->GetAttribute("MapType");
				FString UseTexture = It->GetAttribute("UseTexture");

				float UseTextureCon = FCString::Atof(*UseTexture);
				MaterialData.DIM->SetScalarParameterValue(FName(*(MapType + "_UseTexture")), UseTextureCon);
				//如果是绘制户型，则AO效果通过Diffuse混合输出。如果是烘焙户型则通过Ambient Occulusion输出。
				if (bDIYHome)
					MaterialData.DIM->SetScalarParameterValue("AO_OverlayStyle", 0.f);

				if (UseTexture == "0")
				{
					FString RStr = It->GetAttribute("R");
					int32 RInt = FCString::Atoi(*RStr);
					FString GStr = It->GetAttribute("G");
					int32 GInt = FCString::Atoi(*GStr);
					FString BStr = It->GetAttribute("B");
					int32 BInt = FCString::Atoi(*BStr);
					FLinearColor PureColor(FColor(RInt, GInt, BInt));

					FString ColorStr = MapType + "_Color";
					MaterialData.DIM->SetVectorParameterValue(FName(*ColorStr), PureColor);
				}
				else
				{
					FString Filename = It->GetAttribute("Filename");
					//此处对多层路径进行提出  AAA\\BBB.jpg  --->  BBB.jpg
					int32 pos = Filename.Find("\\");
					if (pos != -1)
						Filename = Filename.RightChop(pos + 1);
					int32 UVInt = FCString::Atoi(*UVStr) - 1;
					FString TextureFileName = FPaths::GetBaseFilename(Filename);
					UTexture* ThisTexture = _TextureMap[TextureFileName];
					if (ThisTexture)
					{
						MaterialData.DIM->SetScalarParameterValue(FName(*(MapType + "_UV")), UVInt);
						MaterialData.DIM->SetTextureParameterValue(FName(*(MapType + "_Map")), ThisTexture);

						if (MapType == "Diffuse")
							MaterialData.DiffuseMap = ThisTexture;
						else if (MapType == "Normal")
							MaterialData.NormalMap = ThisTexture;
						else if (MapType == "Opacity")
							MaterialData.OpacityMap = ThisTexture;
					}
				}
			}
		}

		AddCustomMaterialParametersForMaterialData_V7(MaterialData);
	}

	return MaterialData;
}

FSCTModelMaterialData USCTXRResourceManager::CreateModelMaterialDataFromName_V7(int32 _MaterialTypeIndex)
{
	if (_MaterialTypeIndex == -1)
	{
		return FSCTModelMaterialData();
	}
	//找出当前材质需要暴露的参数
	FSCTCommonMaterialData::EMaterialType MaterialType = (FSCTCommonMaterialData::EMaterialType) _MaterialTypeIndex;
	TArray<int32> ExposedParameters = StaticCommonMaterialList[MaterialType].ExposedParameters;
	TArray<FSCTXRParameter> ParaList;
	UMaterialInstanceDynamic* MID;

	MID = UMaterialInstanceDynamic::Create(StaticCommonMaterialList[MaterialType].MI, NULL);
	if (MID)
	{
		//将材质球的默认值填充到列表中
		for (int32 i = 0; i < ExposedParameters.Num(); i++)
		{
			FSCTXRParameter* result = StaticMIParaList.Find(ExposedParameters[i]);
			if (result)
			{
				float FloatValue = 0.f;
				FLinearColor ColorValue;
				FVector VectorValue;
				if (result->VarType == FSCTXRParameter::E_Float)
				{
					MID->GetScalarParameterValue(FName(*result->ParaName), FloatValue);
					ParaList.Add(FSCTXRParameter(result->ParaDisplayName, result->ParaDisplayNameEN, result->ParaName, result->MinValue, result->MaxValue, FloatValue));
				}
				else if (result->VarType == FSCTXRParameter::E_Vec3)
				{
					MID->GetVectorParameterValue(FName(*result->ParaName), ColorValue);
					VectorValue.X = ColorValue.R;
					VectorValue.Y = ColorValue.G;
					VectorValue.Z = ColorValue.B;
					ParaList.Add(FSCTXRParameter(result->ParaDisplayName, result->ParaDisplayNameEN, result->ParaName, VectorValue));
				}
				ParaList[ParaList.Num() - 1].ID = ExposedParameters[i];
			}
		}
	}

	return FSCTModelMaterialData("", MaterialType, MID, ParaList);
}

void USCTXRResourceManager::AddCustomMaterialParametersForMaterialData_V7(FSCTModelMaterialData& _OutMaterialData)
{
	FSCTXRParameter P_DiffuseUseTexture = StaticMIParaList[-1];
	FSCTXRParameter P_OpacityUseTexture = StaticMIParaList[-2];
	FSCTXRParameter P_NormalMapIndex = StaticMIParaList[-3];
	FSCTXRParameter P_MaterialTypeIndex = StaticMIParaList[-4];
	
	P_MaterialTypeIndex.SetInitializedValue((float)_OutMaterialData.MaterialType);
	P_MaterialTypeIndex.ID = StaticMIParaList[-4].ID;

	P_NormalMapIndex.SetInitializedValue(_OutMaterialData.NormalMap ? -1 : -2);
	P_NormalMapIndex.ID = StaticMIParaList[-3].ID;

	P_OpacityUseTexture.SetInitializedCheck(_OutMaterialData.OpacityMap != NULL);
	P_OpacityUseTexture.ID = StaticMIParaList[-2].ID;

	P_DiffuseUseTexture.SetInitializedCheck(_OutMaterialData.DiffuseMap != NULL);
	P_DiffuseUseTexture.ID = StaticMIParaList[-1].ID;
	
	//V7材质要默认添加一下四个材质参数，用来记录当前材质是否发生类型变化，是否启用漫反射贴图，阴影贴图，法线贴图编号
	//0 材质类型编号 1 法线序号 2 启用阴影贴图 3 启用漫反射贴图
	_OutMaterialData.ParaList.Insert(P_DiffuseUseTexture, 0);
	_OutMaterialData.ParaList.Insert(P_OpacityUseTexture, 0);
	_OutMaterialData.ParaList.Insert(P_NormalMapIndex, 0);
	_OutMaterialData.ParaList.Insert(P_MaterialTypeIndex, 0);
}

int32 USCTXRResourceManager::GetNormalTextureIndex(UTexture* _Texture)
{
 	int32 BaseIndex = 0;
 	for (int32 i = 0; i < NormalMapList_Cloth.Num(); i++)
 	{
 		if (NormalMapList_Cloth[i] == _Texture)
 			return BaseIndex + i;
 	}
 	BaseIndex += 100;
 
 	for (int32 i = 0; i < NormalMapList_Metal.Num(); i++)
 	{
 		if (NormalMapList_Metal[i] == _Texture)
 			return BaseIndex + i;
 	}
 	BaseIndex += 100;
 
 	for (int32 i = 0; i < NormalMapList_Wood.Num(); i++)
 	{
 		if (NormalMapList_Wood[i] == _Texture)
 			return BaseIndex + i;
 	}
 	BaseIndex += 100;
 
 	for (int32 i = 0; i < NormalMapList_Leather.Num(); i++)
 	{
 		if (NormalMapList_Leather[i] == _Texture)
 			return BaseIndex + i;
 	}
 
 	if (_Texture == EmptyTexture)
 	{
 		return -2;
 	}
	return -1;
}

UTexture* USCTXRResourceManager::GetNormalTexture(int32 _Index)
{
 	int32 TypeIndex = _Index / 100;
 	int32 ElementIndex = _Index - TypeIndex * 100;
 
 	if (TypeIndex == 0 && NormalMapList_Cloth.IsValidIndex(ElementIndex))
 		return NormalMapList_Cloth[ElementIndex];
 
 	else if (TypeIndex == 1 && NormalMapList_Metal.IsValidIndex(ElementIndex))
 		return NormalMapList_Metal[ElementIndex];
 
 	else if (TypeIndex == 2 && NormalMapList_Wood.IsValidIndex(ElementIndex))
 		return NormalMapList_Wood[ElementIndex];
 
 	else if (TypeIndex == 3 && NormalMapList_Leather.IsValidIndex(ElementIndex))
 		return NormalMapList_Leather[ElementIndex];

	return NULL;
}

int32 USCTXRResourceManager::GetMaterialTypeIndex(FString _MaterialName, bool _bDisplayName /*= false*/)
{
	if (_bDisplayName)
	{
		for (int32 i = 0; i < StaticCommonMaterialList.Num(); i++)
		{
			if (StaticCommonMaterialList[i].MaterialDisplayName == _MaterialName)
			{
				return i;
			}
		}
	}
	else
	{
		for (int32 i = 0; i < StaticCommonMaterialList.Num(); i++)
		{
			if (StaticCommonMaterialList[i].MaterialName == _MaterialName)
			{
				return i;
			}
		}
	}
	return -1;
}

void USCTXRResourceManager::ChangeModelMaterialData(int32 _NewMaterialTypeIndex, FSCTModelMaterialData* _OldMaterialData)
{
	FSCTModelMaterialData NewMaterialData = CreateModelMaterialDataFromName_V7(_NewMaterialTypeIndex);

	if (_OldMaterialData->DiffuseMap)
	{
		float UV = 0;
		_OldMaterialData->DIM->GetScalarParameterValue("Diffuse_UV", UV);
		NewMaterialData.DIM->SetScalarParameterValue("Diffuse_UseTexture", 1.f);
		NewMaterialData.DIM->SetScalarParameterValue("Diffuse_UV", UV);
		NewMaterialData.DIM->SetTextureParameterValue("Diffuse_Map", _OldMaterialData->DiffuseMap);
		NewMaterialData.DiffuseMap = _OldMaterialData->DiffuseMap;
	}
	if (_OldMaterialData->NormalMap)
	{
		float UV = 0;
		_OldMaterialData->DIM->GetScalarParameterValue("Normal_UV", UV);
		NewMaterialData.DIM->SetScalarParameterValue("Normal_UseTexture", 1.f);
		NewMaterialData.DIM->SetScalarParameterValue("Normal_UV", UV);
		NewMaterialData.DIM->SetTextureParameterValue("Normal_Map", _OldMaterialData->NormalMap);
		NewMaterialData.NormalMap = _OldMaterialData->NormalMap;
	}
	if (_OldMaterialData->OpacityMap)
	{
		float UV = 0;
		_OldMaterialData->DIM->GetScalarParameterValue("Opacity_UV", UV);
		NewMaterialData.DIM->SetScalarParameterValue("Opacity_UseTexture", 1.f);
		NewMaterialData.DIM->SetScalarParameterValue("Opacity_UV", UV);
		NewMaterialData.DIM->SetTextureParameterValue("Opacity_Map", _OldMaterialData->OpacityMap);
		NewMaterialData.OpacityMap = _OldMaterialData->OpacityMap;
	}
	_OldMaterialData->DIM = NewMaterialData.DIM;
	_OldMaterialData->MaterialType = NewMaterialData.MaterialType;

	AddCustomMaterialParametersForMaterialData_V7(NewMaterialData);
	//材质类型编号 要记录原来初始化的参数，在导出Json的时候回根据ServerDirty来决定是否导出该字段
	NewMaterialData.ParaList[0].ServerValue = _OldMaterialData->ParaList[0].ServerValue;
	NewMaterialData.ParaList[0].OriginalValue = _OldMaterialData->ParaList[0].OriginalValue;
	//法线序号
	NewMaterialData.ParaList[1].ServerValue = _OldMaterialData->ParaList[1].ServerValue;
	NewMaterialData.ParaList[1].OriginalValue = _OldMaterialData->ParaList[1].OriginalValue;
	//启用阴影贴图
	NewMaterialData.ParaList[2].bServerCheck = _OldMaterialData->ParaList[2].bServerCheck;
	NewMaterialData.ParaList[2].bOriginalCheck = _OldMaterialData->ParaList[2].bOriginalCheck;
	//启用漫反射贴图
	NewMaterialData.ParaList[3].bServerCheck = _OldMaterialData->ParaList[3].bServerCheck;
	NewMaterialData.ParaList[3].bOriginalCheck = _OldMaterialData->ParaList[3].bOriginalCheck;

	_OldMaterialData->NormalMap_Override = NULL;

	_OldMaterialData->ParaList = NewMaterialData.ParaList;
}

bool USCTXRResourceManager::ConvertJsonToModelSettingsList(FString _ParaStr, TArray<FSCTXRParameter>& _OutSettingsList)
{
	if (!_ParaStr.IsEmpty())
	{
		_OutSettingsList.Empty();
		TSharedPtr<FJsonObject> JSONObject;
		TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(_ParaStr);
		bool readresult = FJsonSerializer::Deserialize(Reader, JSONObject);
		if (readresult)
		{
			const TArray<TSharedPtr<FJsonValue>> *arrayPtr = nullptr;
			arrayPtr = &JSONObject->GetArrayField("M");
			if (arrayPtr != nullptr)
			{
				//遍历模型参数数组
				for (int32 i = 0; i < (*arrayPtr).Num(); ++i)
				{
					FString ParaItemStr = (*arrayPtr)[i]->AsString();
					
					//解析字段
					int32 pos = ParaItemStr.Find(TEXT("*"));
					if (pos != -1)
					{
						FSCTXRParameter ParaItem;
						//解析出该参数的对应的序号，到StaticMIParaList查找，得出它是什么类型
						FString IDStr = ParaItemStr.Mid(0, pos);
						int32 ID = FCString::Atoi(*IDStr);
						ParaItemStr = ParaItemStr.RightChop(pos + 1);

						FSCTXRParameter* result = StaticModelParaList.Find(ID);
						if (result)
						{
							ParaItem.ID = ID;
							if (result->VarType == FSCTXRParameter::E_Bool)
							{
								float FValue = FCString::Atof(*ParaItemStr);
								ParaItem.VarType = FSCTXRParameter::E_Bool;
								ParaItem.bCheck = FValue == 1.f ? true : false;
								ParaItem.ParaName = result->ParaName;
							}
							else if (result->VarType == FSCTXRParameter::E_Float)
							{
								float FValue = FCString::Atof(*ParaItemStr);
								ParaItem.VarType = FSCTXRParameter::E_Float;
								ParaItem.Value = FValue;
								ParaItem.ServerValue = FValue;
								ParaItem.ParaName = result->ParaName;
							}
							else if (result->VarType == FSCTXRParameter::E_Vec3)
							{
								FVector VectorValue;
								//解析Vector类型的XYZ三个值
								pos = ParaItemStr.Find(TEXT("-"));
								if (pos != -1)
								{
									FString ValueStr = ParaItemStr.Mid(0, pos);
									ParaItemStr = ParaItemStr.RightChop(pos + 1);
									VectorValue.X = FCString::Atof(*ValueStr);
								}
								pos = ParaItemStr.Find(TEXT("-"));
								if (pos != -1)
								{
									FString ValueStr = ParaItemStr.Mid(0, pos);
									ParaItemStr = ParaItemStr.RightChop(pos + 1);
									VectorValue.Y = FCString::Atof(*ValueStr);
								}
								if (!ParaItemStr.IsEmpty())
								{
									VectorValue.Z = FCString::Atof(*ParaItemStr);
								}
								ParaItem.VarType = FSCTXRParameter::E_Vec3;
								ParaItem.Color = VectorValue;
								ParaItem.ServerColor = VectorValue;
								ParaItem.ParaName = result->ParaName;
							}
						}
						_OutSettingsList.Add(ParaItem);
					}
				}
			}
			return true;
		}
	}
	return false;
}

FString USCTXRResourceManager::ConvertModelSettingsListToJson(TArray<FSCTXRParameter>& _SettingsList)
{
	FString JStr;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JStr);
	JsonWriter->WriteObjectStart();
	//------------------------------------------------parameter
	JsonWriter->WriteArrayStart(TEXT("M"));
	for (auto& It : _SettingsList)
	{
		FString ValueStr;
		ValueStr.Append(FString::Printf(TEXT("%d*"), It.ID));
		if (It.VarType == FSCTXRParameter::E_Bool)
		{
			ValueStr.Append(FString::Printf(TEXT("%d"), It.bCheck ? 1 : 0));
		}
		else if (It.VarType == FSCTXRParameter::E_Float)
		{
			ValueStr.Append(FString::Printf(TEXT("%.2f"), It.Value));
		}
		else if (It.VarType == FSCTXRParameter::E_Vec3)
		{
			ValueStr.Append(FString::Printf(TEXT("%.2f-%.2f-%.2f"), It.Color.X, It.Color.Y, It.Color.Z));
		}
		JsonWriter->WriteValue(ValueStr);
	}
	JsonWriter->WriteArrayEnd();
	//------------------------------------------------parameter
	JsonWriter->WriteObjectEnd();
	JsonWriter->Close();
	return JStr;
}

void USCTXRResourceManager::CollectActorLightComponentsParameters(AActor* _Actor, TArray<FSCTXRParameter>& _OutSettingsList)
{
	_OutSettingsList.Reset();
	if (_Actor)
	{
		FSCTVRSObject* ResultObj = GetObjFromObjID(_Actor->GetObjID());
		if (ResultObj)
		{
			//如果是工具制作的标准化交互模型
			if (ResultObj->bNormalized)
			{
				TInlineComponentArray<USceneComponent*> SceneComponents;
				_Actor->GetComponents(SceneComponents);
				TArray<USpotLightComponent*> Spots;
				TArray<UPointLightComponent*> Points;
				for (USceneComponent* SceneComp : SceneComponents)
				{
					UPointLightComponent* LightCOM = Cast<UPointLightComponent>(SceneComp);
					USpotLightComponent* LightCOM2 = Cast<USpotLightComponent>(SceneComp);
					if (LightCOM2)
						Spots.Add(LightCOM2);
					else if (LightCOM)
						Points.Add(LightCOM);
				}

				if (Points.Num() >0)
				{
					FVector VColor;
					FLinearColor LColor = Points[0]->GetLightColor();
					VColor.X = LColor.R;
					VColor.Y = LColor.G;
					VColor.Z = LColor.B;
					_OutSettingsList.Add(FSCTXRParameter("", "", "PointLightCastShadow", Points[0]->CastShadows));
					_OutSettingsList.Last().ID = 0;
					_OutSettingsList.Add(FSCTXRParameter("", "", "PointLightIntensity", 0, 0, Points[0]->Intensity));
					_OutSettingsList.Last().ID = 1;
					_OutSettingsList.Add(FSCTXRParameter("", "", "PointLightRadius", 0, 0, Points[0]->AttenuationRadius));
					_OutSettingsList.Last().ID = 2;
					_OutSettingsList.Add(FSCTXRParameter("", "", "PointLightColor", VColor));
					_OutSettingsList.Last().ID = 3;
				}
				if (Spots.Num() > 0)
				{
					FVector VColor;
					FLinearColor LColor = Spots[0]->GetLightColor();
					VColor.X = LColor.R;
					VColor.Y = LColor.G;
					VColor.Z = LColor.B;
					int32 IESIndex = -1;

					_OutSettingsList.Add(FSCTXRParameter("", "", "SpotLightCastShadow", Spots[0]->CastShadows));
					_OutSettingsList.Last().ID = 4;
					_OutSettingsList.Add(FSCTXRParameter("", "", "SpotLightIntensity", 0, 0, Spots[0]->Intensity));
					_OutSettingsList.Last().ID = 5;
					_OutSettingsList.Add(FSCTXRParameter("", "", "SpotLightRadius", 0, 0, Spots[0]->AttenuationRadius));
					_OutSettingsList.Last().ID = 6;
					_OutSettingsList.Add(FSCTXRParameter("", "", "SpotLightInnerAngle", 0, 0, Spots[0]->InnerConeAngle));
					_OutSettingsList.Last().ID = 7;
					_OutSettingsList.Add(FSCTXRParameter("", "", "SpotLightOutterAngle", 0, 0, Spots[0]->OuterConeAngle));
					_OutSettingsList.Last().ID = 8;
					_OutSettingsList.Add(FSCTXRParameter("", "", "SpotLightColor", VColor));
					_OutSettingsList.Last().ID = 9;
					_OutSettingsList.Add(FSCTXRParameter("", "", "SpotLightIES", 0, 0, IESIndex));
					_OutSettingsList.Last().ID = 10;
				}
			}
		}
	}
}

void USCTXRResourceManager::SetActorLightComponentParameters(AActor* _Actor, TArray<FSCTXRParameter>& _SettingsList)
{
}

UMeshComponent* USCTXRResourceManager::GetFirstMeshComponent(AActor* _Actor)
{
	FSCTVRSObject* ResultObj = GetObjFromObjID(_Actor->GetObjID());

	UMeshComponent* NewMeshCOM = NULL;
	TInlineComponentArray<USceneComponent*> SceneComponents;
	_Actor->GetComponents(SceneComponents);
	//如果是工具制作的标准化模型，只需要到第一个MeshComponent，获取它的材质列表
	if (ResultObj->bNormalized)
	{
		for (USceneComponent* SceneComp : SceneComponents)
		{
			UMeshComponent* MeshCOM = Cast<UMeshComponent>(SceneComp);
			if (MeshCOM)
			{
				NewMeshCOM = MeshCOM;
				break;
			}
		}
	}
	//如果是非标准化的
	else
	{
		//当是AStaticMeshActor的时候，获取贴在StaticMeshComponent上的壁纸瓷砖类的材质
		AStaticMeshActor* SMA = Cast<AStaticMeshActor>(_Actor);
		if (SMA)
		{
			NewMeshCOM = SMA->GetStaticMeshComponent();
		}
		//如果是蓝图Actor，则需要找它的第一个MeshComponent，获取它的材质列表
		else
		{
			for (USceneComponent* SceneComp : SceneComponents)
			{
				UMeshComponent* MeshCOM = Cast<UMeshComponent>(SceneComp);
				if (MeshCOM)
				{
					NewMeshCOM = MeshCOM;
					break;
				}
			}
		}
	}

	return NewMeshCOM;
}

bool USCTXRResourceManager::OverrideMaterialData(FSCTModelMaterialData& _DesData, FSCTModelMaterialData& _SrcData)
{
	if (_DesData.DIM)
	{
		for (auto& It : _SrcData.ParaList)
		{
			for (auto& It2 : _DesData.ParaList)
			{
				if (It2.ID == It.ID)
				{
					//数值覆盖
					It2.OverrideValue(It);
					//应用到MID中
					if (It2.VarType == FSCTXRParameter::E_Float)
						_DesData.DIM->SetScalarParameterValue(FName(*It2.ParaName), It2.Value);
					else if (It2.VarType == FSCTXRParameter::E_Vec3)
					{
						FLinearColor LColor = FLinearColor(It2.Color.X, It2.Color.Y, It2.Color.Z, 1.f);
						_DesData.DIM->SetVectorParameterValue(FName(*It2.ParaName), LColor);
					}
					break;
				}
			}
		}
	}
	return false;
}

void USCTXRResourceManager::GetMeshMaterialDataFromXMLNode(const FXmlNode* _InNode, FSCTMeshMaterialData& _OutData, FString& _OutActorName)
{
	FString ItemID;
	FString ParaStr;
	float RotateUV = 0.f;
	float OffsetU = 0.f;
	float OffsetV = 0.f;
	float TilingU = 0.f;
	float TilingV = 0.f;
	bool bUseProjectiveUV = false;

	_OutActorName = _InNode->GetTag();

	//5.5版本将采用新的xml格式存储3d户型的材质信息
	if (CurLoadPlanVersion >= 5500)
	{
		//原来版本ID格式为M_5001，新版本需要转换为5001
		ItemID = _InNode->GetAttribute(TEXT("ID"));
		//ItemID = FSCTResTools::GetPureItemID(ItemID);
		ParaStr = _InNode->GetAttribute(TEXT("Str"));
		FString RotateUVStr = _InNode->GetAttribute(TEXT("UVR"));
		FString OffsetUStr = _InNode->GetAttribute(TEXT("SU"));
		FString OffsetVStr = _InNode->GetAttribute(TEXT("SV"));
		FString TilingUStr = _InNode->GetAttribute(TEXT("TU"));
		FString TilingVStr = _InNode->GetAttribute(TEXT("TV"));
		FString UseProjectiveUVStr = _InNode->GetAttribute(TEXT("UP"));

		RotateUV = FCString::Atof(*RotateUVStr);
		OffsetU = FCString::Atof(*OffsetUStr);
		OffsetV = FCString::Atof(*OffsetVStr);
		TilingU = FCString::Atof(*TilingUStr);
		TilingV = FCString::Atof(*TilingVStr);
		bUseProjectiveUV = UseProjectiveUVStr == TEXT("1") ? true : false;

		//从-1到1的取值范围映射到0-100
		if (CurLoadPlanVersion < 6200)
		{
			OffsetU = USCTXRResourceManager::ConvertOldOffsetToV7(OffsetU);
			OffsetV = USCTXRResourceManager::ConvertOldOffsetToV7(OffsetV);
			if (bUseProjectiveUV)
			{
				TilingU = USCTXRResourceManager::ConvertOldProjectiveTilingToV7(TilingU);
				TilingV = USCTXRResourceManager::ConvertOldProjectiveTilingToV7(TilingV);
			}
		}
	}
	//旧版本
	else
	{
		if (_InNode->GetContent() == TEXT(""))
		{
			const FXmlNode *MatFullNameNode = _InNode->FindChildNode(TEXT("PakName"));
			ItemID = MatFullNameNode->GetContent();
			//ItemID = FSCTResTools::GetPureItemID(ItemID);

			const FXmlNode *RotateAngleNode = _InNode->FindChildNode(TEXT("RotateAngle"));
			if (RotateAngleNode)
			{
				FString RotateStr = RotateAngleNode->GetContent();
				RotateUV = FCString::Atof(*RotateStr);
			}
			const FXmlNode *ParaStrNode = _InNode->FindChildNode(TEXT("ParaStr"));
			if (ParaStrNode)
			{
				ParaStr = ParaStrNode->GetContent();
			}
		}
		else
		{
			ItemID = _InNode->GetContent();
			//ItemID = FSCTResTools::GetPureItemID(ItemID);
		}
	}

	_OutData.PakID = FCString::Atoi(*ItemID);
	_OutData.ParaStr = ParaStr;
	_OutData.RotateUV = RotateUV;
	_OutData.OffsetU = OffsetU;
	_OutData.OffsetV = OffsetV;
	_OutData.TilingU = TilingU;
	_OutData.TilingV = TilingV;
	_OutData.bProjectiveUV = bUseProjectiveUV;
}

void USCTXRResourceManager::AppendSynList(TArray<TSharedPtr<FSCTContentItemSpace::FContentItem>> _NewList)
{
	for (auto& It : _NewList)
	{
		bool bFound = false;
		for (auto& It2 : SynList)
		{
			if (It2->ID == It->ID)
			{
				bFound = true;
			}
		}

		if (!bFound)
		{
			SynList.Add(It);
		}
	}
}

void USCTXRResourceManager::ClearData()
{
	//LoadedHomeItemData = FSCTContentItemSpace::FContentItem();
	//LoadedHomeItemData.ResObj = MakeShareable(new FSCTContentItemSpace::FResObj());
	//LoadedPlanItemData = FSCTContentItemSpace::FContentItem();
	//LoadedPlanItemData.ResObj = MakeShareable(new FSCTContentItemSpace::FResObj());
	UnloadModelPak();
}



FSCTXRResourceManagerInstatnce::FSCTXRResourceManagerInstatnce()
	:XRManager(nullptr)
{}

FSCTXRResourceManagerInstatnce::~FSCTXRResourceManagerInstatnce()
{

}
FSCTXRResourceManagerInstatnce & FSCTXRResourceManagerInstatnce::GetIns()
{
	if (Singleton == nullptr)
	{
		Singleton = new FSCTXRResourceManagerInstatnce();
	}
	return *Singleton;
}

void FSCTXRResourceManagerInstatnce::ReleaseIns()
{
	if (Singleton != nullptr)
	{
		delete Singleton;
		Singleton = nullptr;
	}
}

USCTXRResourceManager & FSCTXRResourceManagerInstatnce::GetResourceManager()
{
	check(XRManager);
	return *XRManager;
}

bool FSCTXRResourceManagerInstatnce::StartUp()
{
	XRManager = NewObject<USCTXRResourceManager>();
	if (!GIsEditor)
		XRManager->Init();
	return true;
}

bool FSCTXRResourceManagerInstatnce::ShutDown()
{
	return true;
}


FSCTXRResourceManagerInstatnce*  FSCTXRResourceManagerInstatnce::Singleton = nullptr;