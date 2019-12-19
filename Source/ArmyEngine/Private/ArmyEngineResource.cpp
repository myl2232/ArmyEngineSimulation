#include "ArmyEngineResource.h"
#include "ArmyPointLightActor.h"
#include "ArmySpotLightActor.h"
#include "ArmyStyle.h"
#include "ArmyReflectionCaptureActor.h"
#include "ArmyLevelAssetBoardActor.h"
#include "Misc/FileHelper.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ArmyTextureDerivedDataTask.h"
#include "UObject/ConstructorHelpers.h"

UArmyEngineResource::UArmyEngineResource()
{
	static ConstructorHelpers::FObjectFinderOptional<UFont> Font(TEXT("/Game/Font/simsunb"));
	static ConstructorHelpers::FObjectFinderOptional<UMaterial> TextMaterial(TEXT("/Engine/EngineMaterials/DefaultTextMaterialOpaque"));
	T_Font = Font.Get();
	T_FontMt = TextMaterial.Get();
}

UFont* UArmyEngineResource::GetDefaultFont()
{
	return T_Font;
}


UMaterialInstanceDynamic * UArmyEngineResource::GetPM_Outline()
{
	if (!PM_Outline) {
		PM_Outline = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/PostProcess/PM_Outline.PM_Outline"), NULL, LOAD_None, NULL));
		PM_Outline->AddToRoot();
	}
	if (!PMID_Outline) {
		PMID_Outline = UMaterialInstanceDynamic::Create(PM_Outline, this);
		PMID_Outline->AddToRoot();
	}
	return PMID_Outline;
}

UMaterialInstanceDynamic * UArmyEngineResource::GetUnStageBaseColorMaterial()
{
	UMaterial* M_BaseColor = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/M_BaseColor.M_BaseColor"), NULL, LOAD_None, NULL));
	return UMaterialInstanceDynamic::Create(M_BaseColor, this);	
}

UMaterialInstanceDynamic* UArmyEngineResource::GetBaseColorMaterial()
{
    if (!MI_BaseColor)
    {
        UMaterial* M_BaseColor = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/M_BaseColor.M_BaseColor"), NULL, LOAD_None, NULL));
        MI_BaseColor = UMaterialInstanceDynamic::Create(M_BaseColor, this);
        MI_BaseColor->SetVectorParameterValue("BaseColor", FArmyStyle::Get().GetColor("Color.FFE6E6E6"));
        MI_BaseColor->AddToRoot();
    }

    return MI_BaseColor;
}

UMaterialInstanceDynamic* UArmyEngineResource::GetTranslucentWhiteMaterial()
{
    if (!MI_TranslucentWhite)
    {
        UMaterial* M_TranslucentWhite = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/Widget/M_TranslucentWhite.M_TranslucentWhite"), NULL, LOAD_None, NULL));
        MI_TranslucentWhite = UMaterialInstanceDynamic::Create(M_TranslucentWhite, this);
        MI_TranslucentWhite->AddToRoot();
    }

    return MI_TranslucentWhite;
}

UMaterial* UArmyEngineResource::GetFontMaterial()
{
	return T_FontMt;
}

UMaterial* UArmyEngineResource::GetLineMaterial()
{
	if (!M_Line) M_Line = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/M_Line.M_Line"), NULL, LOAD_None, NULL));
	return M_Line;
}

UMaterial* UArmyEngineResource::GetDefaultHightLightMaterial()
{
	if (!M_DefaultHighLightMat)
		M_DefaultHighLightMat = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/M_LineHighLight.M_LineHighLight"), NULL, LOAD_None, NULL));
	return M_DefaultHighLightMat;
}

bool UArmyEngineResource::WriteRawToTexture(UTexture2D* NewTexture2D, const TArray<uint8>& RawData, bool bGray, bool bUseSRGB/* = true*/)
{
	int32 Height = NewTexture2D->GetSizeY();
	int32 Width = NewTexture2D->GetSizeX();

	bool bValidTexture = false;
	// Fill in the base mip for the texture we created
	uint8* MipData = (uint8*)NewTexture2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	for (int32 y = 0; y < Height; y++)
	{
		if (bGray)
		{
			uint8* DestPtr = &MipData[y * Width * sizeof(FColor)];
			const uint8* SrcPtr = &((uint8*)(RawData.GetData()))[(Height - 1 - y) * Width];
			for (int32 x = 0; x < Width; x++)
			{
				//如果有一个像素值不为0 则该图片为有效图片
				if (*SrcPtr != 0)
				{
					bValidTexture = true;
				}
				*DestPtr++ = *SrcPtr;
				*DestPtr++ = *SrcPtr;
				*DestPtr++ = *SrcPtr;
				*DestPtr++ = 255;
				SrcPtr++;
			}
		}
		else
		{
			uint8* DestPtr = &MipData[(Height - 1 - y) * Width * sizeof(FColor)];
			const FColor* SrcPtr = &((FColor*)(RawData.GetData()))[(Height - 1 - y) * Width];
			for (int32 x = 0; x < Width; x++)
			{
				//如果有一个像素值不为0 则该图片为有效图片
				if (SrcPtr->B != 0 || SrcPtr->G != 0 || SrcPtr->R != 0)
				{
					bValidTexture = true;
				}
				*DestPtr++ = SrcPtr->B;
				*DestPtr++ = SrcPtr->G;
				*DestPtr++ = SrcPtr->R;
				*DestPtr++ = SrcPtr->A;
				SrcPtr++;
			}
		}
	}
	NewTexture2D->PlatformData->Mips[0].BulkData.Unlock();

	// Set options
	NewTexture2D->SRGB = bUseSRGB;
#if WITH_EDITORONLY_DATA
	NewTexture2D->CompressionNone = true;
	NewTexture2D->MipGenSettings = TMGS_NoMipmaps;
#endif
	NewTexture2D->CompressionSettings = TC_EditorIcon;

	NewTexture2D->UpdateResource();

	return bValidTexture;
}
#define USEMIPMAP
UTexture2D* UArmyEngineResource::ImportTexture(FString _FilePath, bool mipMap)
{
	UTexture2D* tempTexture = TextureLists.FindRef(_FilePath);
	if (tempTexture != NULL)
		return tempTexture;
	else
	{
		TArray<uint8> ImageData;
		if (FFileHelper::LoadFileToArray(ImageData, *_FilePath))
		{

			IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
			TSharedPtr<IImageWrapper> ImageWrappers[3] =
			{
				ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG),
				ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG),
				ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP),
			};
			if (!mipMap)
			{
				int32 i = 0;
				for (auto ImageWrapper : ImageWrappers)
				{
					if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
					{
						ERGBFormat RawFormat;
						if (i == 0)
						{
							RawFormat = ERGBFormat::BGRA;
						}
						else
							RawFormat = ImageWrapper->GetFormat();


						int32 BitDepth = ImageWrapper->GetBitDepth();
						const TArray<uint8>* RawData = NULL;
						if (ImageWrapper->GetRaw(RawFormat, BitDepth, RawData))
						{

							tempTexture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight());
							{
								bool bValidTexture = WriteRawToTexture(tempTexture, *RawData, ImageWrapper->GetFormat() == ERGBFormat::Gray);
								if (!bValidTexture)
								{
									return NULL;
								}
								tempTexture->AddToRoot();

							}

						}
					}
					i++;
				}
			}
			else {

				int32 i = 0;
				for (auto ImageWrapper : ImageWrappers)
				{
					if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
					{
						ERGBFormat RawFormat;

						RawFormat = ImageWrapper->GetFormat();

						const TArray<uint8>* RawData = NULL;
						ETextureSourceFormat TextureFormat = TSF_Invalid;
						int32 BitDepth = ImageWrapper->GetBitDepth();

						if (i == 0)
						{
							if (RawFormat == ERGBFormat::Gray)
							{
								if (BitDepth <= 8)
								{
									TextureFormat = TSF_G8;
									RawFormat = ERGBFormat::Gray;
									BitDepth = 8;
								}
								else if (BitDepth == 16)
								{
									TextureFormat = TSF_RGBA16;
									RawFormat = ERGBFormat::RGBA;
									BitDepth = 16;
								}
							}
							else if (RawFormat == ERGBFormat::RGBA || RawFormat == ERGBFormat::BGRA)
							{
								if (BitDepth <= 8)
								{
									TextureFormat = TSF_BGRA8;
									RawFormat = ERGBFormat::BGRA;
									BitDepth = 8;
								}
								else if (BitDepth == 16)
								{
									TextureFormat = TSF_RGBA16;
									RawFormat = ERGBFormat::RGBA;
									BitDepth = 16;
								}
							}
							else if (TextureFormat == TSF_Invalid)
							{
								return NULL;
							}
						}
						else
						{


							if (RawFormat == ERGBFormat::Gray)
							{
								if (BitDepth <= 8)
								{
									TextureFormat = TSF_G8;
									RawFormat = ERGBFormat::Gray;
									BitDepth = 8;
								}

							}
							else if (RawFormat == ERGBFormat::RGBA)
							{
								if (BitDepth <= 8)
								{
									TextureFormat = TSF_BGRA8;
									RawFormat = ERGBFormat::BGRA;
									BitDepth = 8;
								}

							}
							if (TextureFormat == TSF_Invalid)
							{
								return NULL;
							}
						}
						if (ImageWrapper->GetRaw(RawFormat, BitDepth, RawData))
						{
							tempTexture = NewObject<UTexture2D>(GetTransientPackage(), NAME_None, RF_Transient);
							{
								FArmyTextureBuildSettings setttting;
								tempTexture->LODGroup = TEXTUREGROUP_World;
								setttting.TextureFormatName = TEXT("AutoDXT");
								FArmyTextureSource Source;
								Source.Init(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), 1, 1, TextureFormat);
								tempTexture->SRGB = BitDepth < 16;
								setttting.bSRGB = tempTexture->SRGB;
								uint8* MipData = Source.LockMip(0);
								FMemory::Memcpy(MipData, RawData->GetData(), RawData->Num());
								Source.UnlockMip(0);
								//Generate(*tempTexture, setttting, 0, NULL, Source);
								tempTexture->UpdateResource();
								tempTexture->AddToRoot();

							}

						}
					}
					i++;
				}

			}

			TextureLists.Add(_FilePath, tempTexture);
		}
	}

	return tempTexture;
}
//void UArmyEngineResource::Generate(UTexture2D& InTexure, const FArmyTextureBuildSettings& InSettings, uint32 InFlags, ITextureCompressorModule* Compressor, FArmyTextureSource& InSouce)
//{
//	uint32 Flags = InFlags;
//	if (!Compressor)
//	{
//		Compressor = &FModuleManager::LoadModuleChecked<ITextureCompressorModule>(TEXTURE_COMPRESSOR_MODULENAME);
//	}
//	FTexturePlatformData* platformData = new FTexturePlatformData();
//	FArmyTextureCachedDerivedDataWorker Worker(Compressor, platformData, &InTexure, InSettings, Flags, InSouce);
//
//	Worker.DoWork();
//	Worker.Finalize();
//
//	InTexure.PlatformData = platformData;
//}
UTexture2D* UArmyEngineResource::GetDefaultTexture()
{
	if (!DefaultFloorTexture)
	{

		/*FString _FilePath = FPaths::ProjectContentDir() / TEXT("Textures/TestBrick.jpg");

		DefaultFloorTexture = ImportTexture(_FilePath);*/

		DefaultFloorTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, TEXT("/Game/Textures/TestBrick.TestBrick"), NULL, LOAD_None, NULL));
	}

	return DefaultFloorTexture;
}

UTexture2D* UArmyEngineResource::GetDefaultTextureWall()
{
	if (!DefaultTextureWall)
	{
		DefaultTextureWall = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, TEXT("/Game/ArmyCommon/HardMode/CeilingTexture.CeilingTexture"), NULL, LOAD_None, NULL));
	}
	return DefaultTextureWall;
}

UTexture2D* UArmyEngineResource::GetDefaultTextureFloor()
{
	if (!DefaultTextureFloor)
	{
		DefaultTextureFloor = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, TEXT("/Game/ArmyCommon/HardMode/CeilingTexture.CeilingTexture"), NULL, LOAD_None, NULL));
	}
	return DefaultTextureFloor;
}

UTexture2D* UArmyEngineResource::GetDefaultTextureCeiling()
{
	if (!DefaultTextureCeiling)
	{
		DefaultTextureCeiling = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, TEXT("/Game/ArmyCommon/HardMode/CeilingTexture.CeilingTexture"), NULL, LOAD_None, NULL));
	}
	return DefaultTextureCeiling;
}

UMaterial* UArmyEngineResource::GetObliqueLineMat()
{
	if (!ObliqueLineMat)
	{
		ObliqueLineMat = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/M_LowerHangingAreaObliqueLine.M_LowerHangingAreaObliqueLine"), NULL, LOAD_None, NULL));

	}
	return ObliqueLineMat;
}

UTexture2D* UArmyEngineResource::GetDefaultWhiteTexture()
{
	if (!DefaultWhiteTexture)
	{
		DefaultWhiteTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/Default/TDefualtWhitePic.TDefualtWhitePic"), NULL, LOAD_None, NULL));

	}
	return DefaultWhiteTexture;
}
UMaterial* UArmyEngineResource::GetCheckBoxMat(bool InChecked)
{
	if (InChecked)
	{
		if (!M_CheckBoxTrue)
		{
			M_CheckBoxTrue = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/M_CheckBoxTrue.M_CheckBoxTrue"), NULL, LOAD_None, NULL));
		}
		return M_CheckBoxTrue;
	}
	else
	{
		if (!M_CheckBoxFalse)
		{
			M_CheckBoxFalse = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/M_CheckBoxFalse.M_CheckBoxFalse"), NULL, LOAD_None, NULL));
		}
		return M_CheckBoxFalse;
	}
}

UMaterial* UArmyEngineResource::GetDefaultFloorTextureMat()
{
	if (!DefautFloorTextureMat)
	{
		DefautFloorTextureMat = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/FloorTextureMat.FloorTextureMat"), NULL, LOAD_None, NULL));

	}
	return DefautFloorTextureMat;
}


UMaterial* UArmyEngineResource::GetDefaultConstructionMat()
{
	if (!DefaultConstructionMat)
	{
		DefaultConstructionMat = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/ConstructionMat.ConstructionMat"), NULL, LOAD_None, NULL));

	}
	return DefaultConstructionMat;
}

UMaterial* UArmyEngineResource::GetDefaultBridgeStoneMat()
{
	if (!M_DefaultBridgeStoneMat) M_DefaultBridgeStoneMat = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/M_BridgeStone.M_BridgeStone"), NULL, LOAD_None, NULL));
	if (M_DefaultBridgeStoneMat)M_DefaultBridgeStoneMat->AddToRoot();
	return M_DefaultBridgeStoneMat;
}


UMaterial* UArmyEngineResource::GetDefaultWallFillMat()
{
	if (!M_DefaultWallFillMat)
		M_DefaultWallFillMat = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/M_WallFill.M_WallFill"), NULL, LOAD_None, NULL));
	if (M_DefaultWallFillMat)
		M_DefaultWallFillMat->AddToRoot();
	return M_DefaultWallFillMat;
}

UMaterial* UArmyEngineResource::GetMainWallMaterial()
{
	if (!M_MainWall) M_MainWall = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/M_MainWall.M_MainWall"), NULL, LOAD_None, NULL));
	return M_MainWall;
}

UMaterial* UArmyEngineResource::GetBaseMapMaterial()
{
	if (!M_BaseMap) M_BaseMap = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/M_BaseMap.M_BaseMap"), NULL, LOAD_None, NULL));
	return M_BaseMap;
}

UMaterial* UArmyEngineResource::GetBaseColorMat()
{
	if (!M_BaseColor) M_BaseColor = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/M_BaseColor.M_BaseColor"), NULL, LOAD_None, NULL));
	return M_BaseColor;
}


UMaterial* UArmyEngineResource::GetAddWallMaterial()
{
	if (!M_AddWall) M_AddWall = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/M_AddWall.M_AddWall"), NULL, LOAD_None, NULL));
	return M_AddWall;
}

UMaterial * UArmyEngineResource::GetAWLightgageSteelJoistMaterial()
{
	if (!M_AWLightgageSteelJoist) M_AWLightgageSteelJoist = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/M_AWLightgageSteelJoist.M_AWLightgageSteelJoist"), NULL, LOAD_None, NULL));
	return M_AWLightgageSteelJoist;
}

UMaterial * UArmyEngineResource::GetAWBlackBrickMaterial()
{
	if (!M_AWBlackBrick) M_AWBlackBrick = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/M_AWBlackBrick.M_AWBlackBrick"), NULL, LOAD_None, NULL));
	return M_AWBlackBrick;
}

UMaterial* UArmyEngineResource::GetModifyWallMaterial()
{
	if (!M_ModifyWall) M_ModifyWall = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/M_ModifyWall.M_ModifyWall"), NULL, LOAD_None, NULL));
	return M_ModifyWall;
}

UMaterial* UArmyEngineResource::GetBearingWallMaterial()
{
	if (!M_BearingWall) M_BearingWall = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/M_BearingWall.M_BearingWall"), NULL, LOAD_None, NULL));
	return M_BearingWall;
}

UMaterial* UArmyEngineResource::GetWindowPillarMaterial()
{
	if (!MI_WindowPillar) MI_WindowPillar = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/Default/T_Pillar.T_Pillar"), NULL, LOAD_None, NULL));
	return MI_WindowPillar;
}
UMaterial* UArmyEngineResource::GetLineTestMaterial()
{
	if (!M_LineTest) M_LineTest = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/M_LineTest.M_LineTest"), NULL, LOAD_None, NULL));
	return M_LineTest;
}
UMaterialInstance* UArmyEngineResource::GetGlassMaterial()
{
	if (!MI_Glass)
	{
		MI_Glass = Cast<UMaterialInstance>(StaticLoadObject(UMaterialInstance::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/MI_Glass.MI_Glass"), NULL, LOAD_None, NULL));
		//MI_Glass = Cast<UMaterialInstance>(StaticLoadObject(UMaterialInstance::StaticClass(), NULL, TEXT("/Game/DLC/Common/9/Materials/V7_Glass_Simple_MT.V7_Glass_Simple_MT"), NULL, LOAD_None, NULL));
		//MI_Glass = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Glass_Simple.Glass_Simple"), NULL, LOAD_None, NULL));
	}
	return MI_Glass;
}


UMaterial* UArmyEngineResource::GetSingleDoorMaterial()
{
	if (!M_SingleDoor)
	{
		M_SingleDoor = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/M_SingleDoor.M_SingleDoor"), NULL, LOAD_None, NULL));
	}
	return M_SingleDoor;
}

UMaterial* UArmyEngineResource::GetDownloadMaterial()
{
	if (!M_ProgressCircle)
	{
		M_ProgressCircle = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/ProgressCircle/M_ProgressCircle.M_ProgressCircle"), NULL, LOAD_None, NULL));
	}
	return M_ProgressCircle;
}

UMaterial* UArmyEngineResource::GetRoomSpaceMaterial()
{
	if (!M_RoomSpace) M_RoomSpace = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/M_RoomSpace.M_RoomSpace"), NULL, LOAD_None, NULL));
	return M_RoomSpace;
}

UMaterialInstanceDynamic* UArmyEngineResource::GetWallMaterial(float Opacity/* = 1.f*/)
{
    if (!MI_Wall_Opaque)
    {
		//@  ArmyLightmass 临时
		//UMaterial* M_Wall = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/Default/M_Test1.M_Test1"), NULL, LOAD_None, NULL));
        UMaterial* M_Wall = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/Default/M_Concrete_Poured2.M_Concrete_Poured2"), NULL, LOAD_None, NULL));
        MI_Wall_Opaque = UMaterialInstanceDynamic::Create(M_Wall, this);
    }
    if (!MI_Wall_Translucent)
    {
        UMaterial* M_Wall_Translucent = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/Default/M_Concrete_Poured2_Translucent.M_Concrete_Poured2_Translucent"), NULL, LOAD_None, NULL));
        MI_Wall_Translucent = UMaterialInstanceDynamic::Create(M_Wall_Translucent, this);
    }

    if (Opacity >= 1.f)
    {
        return MI_Wall_Opaque;
    }
    else
    {
        MI_Wall_Translucent->SetScalarParameterValue("Opacity", Opacity);
        return MI_Wall_Translucent;
    }
}

UMaterialInstanceDynamic* UArmyEngineResource::GetMaterialByURL(FString URL)
{
	UMaterial* M_Wall = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, *URL, NULL, LOAD_None, NULL));
	UMaterialInstanceDynamic* MI_Wall = UMaterialInstanceDynamic::Create(M_Wall, this);
	return MI_Wall;
}

UMaterialInstanceDynamic* UArmyEngineResource::GetTranslucentRoofMaterial(float Opacity /*= 1.f*/)
{
	UMaterial* M_CeilingRoof_Translucent = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/Default/M_CeilingRoof_Translucent.M_CeilingRoof_Translucent"), NULL, LOAD_None, NULL));
	UMaterialInstanceDynamic* MI_CeilingRoof_Translucent = UMaterialInstanceDynamic::Create(M_CeilingRoof_Translucent, this);
	MI_CeilingRoof_Translucent->SetScalarParameterValue("Opacity", Opacity);
	return MI_CeilingRoof_Translucent;
}

UMaterialInstance* UArmyEngineResource::GetWindowBaseMaterial()
{
	if (!MI_WindowBase) MI_WindowBase = Cast<UMaterialInstance>(StaticLoadObject(UMaterialInstance::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/Default/MI_WindowBase.MI_WindowBase"), NULL, LOAD_None, NULL));
	return MI_WindowBase;
}

UMaterialInstance* UArmyEngineResource::GetLineMaterialIns()
{
	if (!MI_LineIns)
		MI_LineIns = Cast<UMaterialInstance>(StaticLoadObject(UMaterialInstance::StaticClass(), NULL, TEXT("/Game/ArmyCommon/M_HydropowerLine_Inst.M_HydropowerLine_Inst"), NULL, LOAD_None, NULL));
	return MI_LineIns;
}

UMaterialInstanceDynamic* UArmyEngineResource::GetFloorMaterial(float Opacity/* = 1.f*/)
{
    if (!MI_Floor_Opaque)
    {
		//@  ArmyLightmass 临时
		//UMaterial* M_Floor = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/Default/M_Test1.M_Test1"), NULL, LOAD_None, NULL));
		UMaterial* M_Floor = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/Default/M_Concrete_Poured1.M_Concrete_Poured1"), NULL, LOAD_None, NULL));
        MI_Floor_Opaque = UMaterialInstanceDynamic::Create(M_Floor, this);
    }
    if (!MI_Floor_Translucent)
    {
        UMaterial* M_Floor_Translucent = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/Default/M_Concrete_Poured1_Translucent.M_Concrete_Poured1_Translucent"), NULL, LOAD_None, NULL));
        MI_Floor_Translucent = UMaterialInstanceDynamic::Create(M_Floor_Translucent, this);
    }

    if (Opacity >= 1.f)
    {
        return MI_Floor_Opaque;
    }
    else
    {
        MI_Floor_Translucent->SetScalarParameterValue("Opacity", Opacity);
        return MI_Floor_Translucent;
    }
}

UTexture2D* UArmyEngineResource::GetBSPFloorTexture()
{
	if (!DefaultBSPFloorTexture)
	{
		DefaultBSPFloorTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/Default/T_Floor.T_Floor"), NULL, LOAD_None, NULL));
	}
	return DefaultBSPFloorTexture;
}

UTexture2D* UArmyEngineResource::GetBSPWallTexture()
{
	if (!DefaultBSPWallTexture)
	{
		DefaultBSPWallTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/Default/T_Wall.T_Wall"), NULL, LOAD_None, NULL));

	}
	return DefaultBSPWallTexture;
}

UMaterialInstance* UArmyEngineResource::GetBaseboardMaterial()
{
	if (!MI_Baseboard) MI_Baseboard = Cast<UMaterialInstance>(StaticLoadObject(UMaterialInstance::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/Default/MI_SkirttingLine.MI_SkirttingLine"), NULL, LOAD_None, NULL));
	return MI_Baseboard;
}

UMaterial* UArmyEngineResource::GetPassColorMesh()
{
	if (!M_Pass) M_Pass = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Material/M_PassColor.M_PassColor"), NULL, LOAD_None, NULL));
	return M_Pass;
}

UTexture2D* UArmyEngineResource::GetOutdoorPanoramaTexture(int32 InIndex)
{
	FString KeyStr = FString::Printf(TEXT("T_%d"), InIndex);
	UTexture2D** Result = OutdoorPanoramaTextures.Find(KeyStr);
	if(Result)
	{
		return *Result;
	}
	else
	{
		FString FilePath = FString::Printf(TEXT("/Game/ArmyCommon/Panorama/T_%d.T_%d"), InIndex, InIndex);
		UTexture2D* Tex = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *FilePath, NULL, LOAD_None, NULL));
		if (Tex)
		{
			OutdoorPanoramaTextures.Add(KeyStr, Tex);
			return Tex;
		}
	}

	return NULL;
}

TSubclassOf<AArmyPointLightActor> UArmyEngineResource::GetPointLightClass()
{
	if (!PointLightClass) PointLightClass = StaticLoadClass(AArmyPointLightActor::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Blueprint/BP_PointLight.BP_PointLight_C"), NULL, LOAD_None, NULL);
	return PointLightClass;
}

TSubclassOf<AArmySpotLightActor> UArmyEngineResource::GetSpotLightClass()
{
	if (!SpotLightClass) SpotLightClass = StaticLoadClass(AArmySpotLightActor::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Blueprint/BP_SpotLight.BP_SpotLight_C"), NULL, LOAD_None, NULL);
	return SpotLightClass;
}

TSubclassOf<AArmyReflectionCaptureActor> UArmyEngineResource::GetReflectionCaptureClass()
{
	if (!ReflectionCaptureClass) ReflectionCaptureClass = StaticLoadClass(AArmyReflectionCaptureActor::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Blueprint/BP_ReflectionSphere.BP_ReflectionSphere_C"), NULL, LOAD_None, NULL);
	return ReflectionCaptureClass;
}

TSubclassOf<AArmyLevelAssetBoardActor> UArmyEngineResource::GetLevelAssetBoardClass()
{
	if (!LevelAssetBoardClass) LevelAssetBoardClass = StaticLoadClass(AArmyLevelAssetBoardActor::StaticClass(), NULL, TEXT("/Game/ArmyCommon/Blueprint/BP_LevelAssetBoard.BP_LevelAssetBoard_C"), NULL, LOAD_None, NULL);
	return LevelAssetBoardClass;
}
