// Fill out your copyright notice in the Description page of Project Settings.

#include "SCTXRCommonTools.h"
#include "Regex.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "AES.h"
//#include "PhysicsEngine/BodySetup.h"
#include "PlatformFilemanager.h"

FSCTXRCommonTools::FSCTXRCommonTools()
{
}

FString FSCTXRCommonTools::GetPathFromFileName(ESCTResourceType InResourceType, const FString& InFileName)
{
	FString FilePath;
	//绘制户型文件
	if (InResourceType == ESCTResourceType::DrawHome)
		FilePath = GetDrawHomeDir() + InFileName;
	//烘焙户型文件
	else if (InResourceType == ESCTResourceType::CookedHome)
		FilePath = GetCookedHomeDir() + InFileName;
	//模型文件
	else if (InResourceType == ESCTResourceType::MoveableMesh)
		FilePath = GetModelDir() + InFileName;
	//材质文件
	else if (InResourceType == ESCTResourceType::Material)
		FilePath = GetModelDir() + InFileName;
	//Actor文件
	else if (InResourceType == ESCTResourceType::BlueprintClass)
		FilePath = GetModelDir() + InFileName;
	//组方案
	else if (InResourceType == ESCTResourceType::GroupPlan)
		FilePath = GetGroupPlanDir() + InFileName;
	//户型方案
	else if (InResourceType == ESCTResourceType::LevelPlan)
		FilePath = GetLevelPlanDir() + InFileName;
	//地板拼花方案
	else if (InResourceType == ESCTResourceType::Texture)
		FilePath = GetFloorTextureDir()+ InFileName;

	return FilePath;
}

bool FSCTXRCommonTools::RemoveFileNameExtension(FString& _FileName)
{
	int32 pos = _FileName.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	if (pos != -1)
	{
		_FileName = _FileName.Mid(0, pos);
		return true;
	}
	return false;
}

FString FSCTXRCommonTools::GetFileExtension(FString& _FileName)
{
	int32 pos = _FileName.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	if (pos != -1)
	{
		return _FileName.RightChop(pos + 1);
	}
	return TEXT("");
}

FString FSCTXRCommonTools::GetDrawHomeDir()
{
	return FPaths::ProjectContentDir() + TEXT("DBJCache/DrawHome/");
}
FString FSCTXRCommonTools::GetConstructionDir()
{
	return FPaths::ProjectContentDir() + TEXT("DBJCache/Construction/");
}
FString FSCTXRCommonTools::GetCookedHomeDir()
{
	return FPaths::ProjectContentDir() + TEXT("DBJCache/CookedHome/");
}

FString FSCTXRCommonTools::GetModelDir()
{
	return FPaths::ProjectContentDir() + TEXT("DBJCache/Model/");
}

FString FSCTXRCommonTools::GetGroupPlanDir()
{
	return FPaths::ProjectContentDir() + TEXT("DBJCache/GroupPlan/");
}

FString FSCTXRCommonTools::GetAutoDesignGroupPlanDir()
{
	return FPaths::ProjectContentDir() + TEXT("DBJCache/AutoDesignGroupPlan/");
}

FString FSCTXRCommonTools::GetLevelPlanDir()
{
	return FPaths::ProjectContentDir() + TEXT("DBJCache/LevelPlan/");
}

FString FSCTXRCommonTools::GetFloorTextureDir()
{
	return FPaths::ProjectContentDir() + TEXT("DBJCache/FloorTexture/");
}

FString FSCTXRCommonTools::GetFileNameFromPath(FString& _FilePath)
{
	int32 pos = _FilePath.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	FString FileName = _FilePath.RightChop(pos + 1);
	
	return FileName;
}

FString FSCTXRCommonTools::GetFolderFromPath(FString& _FilePath)
{
	int32 pos = _FilePath.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	if (pos != -1)
	{
		return _FilePath.Mid(0, pos);
	}
	return "";
}

int32 FSCTXRCommonTools::GetFileIDFromName(FString _FileNameWithMD5)
{
	int32 pos = _FileNameWithMD5.Find(TEXT("_"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	if (pos != -1)
	{
		return FCString::Atoi(*_FileNameWithMD5.Mid(0, pos));
	}
	return FCString::Atoi(*_FileNameWithMD5);
}

bool FSCTXRCommonTools::CheckTeleNumberValid(FString _str)
{
	const FRegexPattern Pattern(TEXT("^(1)\\d{10}$"));
	FRegexMatcher Matcher(Pattern, _str);
	if (Matcher.FindNext())
		return true;
	else
		return false;
}

bool FSCTXRCommonTools::CheckNumberValid(FString _str)
{
	const FRegexPattern Pattern(TEXT("^(0|[1-9][0-9]*)$"));
	FRegexMatcher Matcher(Pattern, _str);
	if (Matcher.FindNext())
		return true;
	else
		return false;
}

bool FSCTXRCommonTools::CheckIDCodeValid(FString _str)
{
	const FRegexPattern Pattern(TEXT("^\\d{17}(\\d|X|x)$"));
	FRegexMatcher Matcher(Pattern, _str);
	if (Matcher.FindNext())
		return true;

	return false;
}

bool FSCTXRCommonTools::CheckPasswordValid(FString _str)
{
	//字母开头，字母数字下划线组成，6-18长度
	//const FRegexPattern Pattern(TEXT("^[a-zA-Z]\\w{5,17}$"));
	//密码有6-18个字母、数字组成
	const FRegexPattern Pattern(TEXT("^\\w{6,18}$"));
	FRegexMatcher Matcher(Pattern, _str);
	if (Matcher.FindNext())
		return true;

	return false;
}

bool FSCTXRCommonTools::CheckEmpty(FString _str)
{
	const FRegexPattern Pattern(TEXT("^\\s*|\\s*$"));
	FRegexMatcher Matcher(Pattern, _str);
	if (Matcher.FindNext())
		return false;

	return true;
}

bool FSCTXRCommonTools::CheckAreaValid(FString _str)
{
	const FRegexPattern Pattern(TEXT("^([1-9][0-9]*)$"));
	FRegexMatcher Matcher(Pattern, _str);
	if (Matcher.FindNext())
		return true;

	return false;
}

FString FSCTXRCommonTools::AddCommaForNumber(FString _Number)
{
	int32 pos = _Number.Find(TEXT("."));
	if (pos != -1)
	{
		pos -= 3;
		while (pos >= 1)
		{
			_Number.InsertAt(pos, TEXT(","));
			pos -= 3;
		}
		return _Number;
	}
	else
	{
		pos = _Number.Len();
		pos -= 3;
		while (pos >= 1)
		{
			_Number.InsertAt(pos, TEXT(","));
			pos -= 3;
		}
		return _Number;
	}
}

FString FSCTXRCommonTools::AddCommaForNumber(float _Number)
{
	FString str = FString::Printf(TEXT("%.2f"), _Number);
	int32 pos = str.Find(TEXT("."));
	if (pos != -1)
	{
		pos -= 3;
		while (pos >= 1)
		{
			str.InsertAt(pos, TEXT(","));
			pos -= 3;
		}
		return str;
	}
	else
	{
		pos = str.Len();
		pos -= 3;
		while (pos >= 1)
		{
			str.InsertAt(pos, TEXT(","));
			pos -= 3;
		}
		return str;
	}
}

void FSCTXRCommonTools::DeleteFiles(int32 InID, ESCTResourceType InType)
{
	FString FileExtension;
	if (InType == ESCTResourceType::DrawHome ||
		InType == ESCTResourceType::LevelPlan ||
		InType == ESCTResourceType::GroupPlan ||
		InType == ESCTResourceType::Construction ||
		InType == ESCTResourceType::CustomlizedFloor ||
		InType == ESCTResourceType::CustomlizedCeling ||
		InType == ESCTResourceType::CustomlizedWall
		)
	{
		FileExtension = "json";
	}
	else
	{
		FileExtension = "pak";
	}

	FString FileFolder = FSCTXRCommonTools::GetPathFromFileName(InType, "");
	FString FileNameMatchedID = FString::Printf(TEXT("%d_*.%s"), InID, *FileExtension);
	TArray<FString> Filenames;
	IFileManager::Get().FindFilesRecursive(Filenames, *FileFolder, *FileNameMatchedID, true, false);

	for (auto& It : Filenames)
	{
		FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*It);
	}
}

//TSharedPtr<FProjectContentItem> FSCTXRCommonTools::ParseProjectContentItemFromJson(TSharedPtr<FJsonObject> InJsonObject)
//{
//	if (InJsonObject.IsValid())
//	{
//		int32 ID = InJsonObject->GetIntegerField("id");
//		FString Name = InJsonObject->GetStringField("name");
//		FString ThumbnailURL = InJsonObject->GetStringField("thumbnailUrl");
//
//		FString CreateDateStr = InJsonObject->GetStringField("createDate");
//		int64 CreateDate = FCString::Atoi64(*CreateDateStr);
//
//		FString UpdateDateStr = InJsonObject->GetStringField("updateDate");
//		int64 UpdateDate = FCString::Atoi64(*UpdateDateStr);
//
//		return MakeShareable(new FProjectContentItem(ID, Name, ThumbnailURL, CreateDate, UpdateDate));
//	}
//	return NULL;
//}

FSCTCategoryContentItem FSCTXRCommonTools::ParseCategoryFromJson(TSharedPtr<FJsonObject> InJsonObject)
{
	if (InJsonObject.IsValid())
	{
		FString Name = InJsonObject->GetStringField("name");
		int32 Key = InJsonObject->GetIntegerField("key");
		int32 Value = InJsonObject->GetIntegerField("value");
		FSCTCategoryContentItem CategoryData = FSCTCategoryContentItem(Name, Key, Value);
		const TArray<TSharedPtr<FJsonValue>> JArray = InJsonObject->GetArrayField("categoryList");

		for (auto& ArrayIt : JArray)
		{
			CategoryData.CategoryList.Add(ParseCategoryFromJson(ArrayIt->AsObject()));
		}

		return CategoryData;
	}
	return FSCTCategoryContentItem();
}

TSharedPtr<FSCTContentItemSpace::FContentItem> FSCTXRCommonTools::ParseTextureContentItemFromJson(TSharedPtr<FJsonObject> InJsonObject, int32 categoryID, ESCTPavingMethodType PavingType)
{
	if (InJsonObject.IsValid())
	{
		//获取缩略图相关信息
		int32 ID = InJsonObject->GetIntegerField("id");
		FString Name = InJsonObject->GetStringField("name");
		FString ThumbnailURL = InJsonObject->GetStringField("thumbnailUrl");
		int32 resourceType = InJsonObject->GetIntegerField("resourceType");
		TSharedPtr<FSCTContentItemSpace::FContentItem> contentItem = MakeShareable(new FSCTContentItemSpace::FContentItem());
		TSharedPtr<FSCTContentItemSpace::FResObj> ResPtr = MakeShareable(new FSCTContentItemSpace::FArmyTextureRes());
		TSharedPtr<FSCTContentItemSpace::FArmyTextureRes> TxtRes = StaticCastSharedPtr<FSCTContentItemSpace::FArmyTextureRes>(ResPtr);
		TxtRes->FileThumbnailURL = ThumbnailURL;
		TxtRes->EPavingMethod = ESCTPavingMethodType(PavingType);
		TxtRes->ThumbnailID = ID;
		contentItem->ResourceType = ESCTResourceType(resourceType);
		contentItem->ID = ID;
		contentItem->Name = Name;
		//int index = 0;

		const TArray<TSharedPtr<FJsonValue>> JArrayPre = InJsonObject->GetArrayField("previewGoodsList");
		for (auto &ArrayIt: JArrayPre)
		{
			
			TSharedPtr<FSCTContentItemSpace::FArmyTextureRes::TextureStruct> txtStr = ParseTextureStructItemFromJson(ArrayIt,categoryID, ESCTResourceType(resourceType));
			TxtRes->TextureArr.Add(txtStr);
			//index++;
		}
		contentItem->ResObj = ResPtr;
		return contentItem;
	}
	return nullptr;
}

TSharedPtr<FSCTContentItemSpace::FContentItem> FSCTXRCommonTools::ParseTextureReplaceContentItemFromJson(TSharedPtr<FJsonValue> InJsonObject, int32 categoryID, ESCTPavingMethodType PavingType)
{
	if (InJsonObject.IsValid())
	{
		TSharedPtr<FSCTContentItemSpace::FArmyTextureRes::TextureStruct> txtStr = ParseTextureStructItemFromJson(InJsonObject, categoryID, ESCTResourceType(0));
			
		//设置缩略图相关信息
		int32 ID = txtStr->ID;
		FString Name = txtStr->TextureName;
		FString ThumbnailURL = txtStr->RealFileThumbnailURL;
		int32 resourceType = txtStr->ResourceType;
		TSharedPtr<FSCTContentItemSpace::FContentItem> contentItem = MakeShareable(new FSCTContentItemSpace::FContentItem());
		TSharedPtr<FSCTContentItemSpace::FResObj> ResPtr = MakeShareable(new FSCTContentItemSpace::FArmyTextureRes());
		TSharedPtr<FSCTContentItemSpace::FArmyTextureRes> TxtRes = StaticCastSharedPtr<FSCTContentItemSpace::FArmyTextureRes>(ResPtr);
		TxtRes->FileThumbnailURL = ThumbnailURL;
		TxtRes->EPavingMethod = ESCTPavingMethodType(PavingType);
		TxtRes->ThumbnailID = ID;
		contentItem->ResourceType = ESCTResourceType(resourceType);
		contentItem->ID = ID;
		contentItem->Name = Name;

		TxtRes->TextureArr.Add(txtStr);

		contentItem->ResObj = ResPtr;
		return contentItem;
	}
	return nullptr;
}

TSharedPtr<FSCTContentItemSpace::FArmyTextureRes::TextureStruct> FSCTXRCommonTools::ParseTextureStructItemFromJson(TSharedPtr<FJsonValue> ArrayIt, int32 categoryID, ESCTResourceType resourceType)
{
	//获取缩略图关联的纹理相关信息
	int32 IDPre = ArrayIt->AsObject()->GetIntegerField("id");
	FString imagePre = ArrayIt->AsObject()->GetStringField("image");
	FString NamePre = ArrayIt->AsObject()->GetStringField("name");
	int32 Width = ArrayIt->AsObject()->GetIntegerField("customWidth");
	int32 Length = ArrayIt->AsObject()->GetIntegerField("customLength");
	int32 Height = ArrayIt->AsObject()->GetIntegerField("customHeight");
	int32 resType = 0;
	if ((int32)resourceType == 0)
	{
		resType = ArrayIt->AsObject()->GetIntegerField("resourceType");//此处资源类型从服务器获取
	}
	else
	{
		resType = (int32)resourceType;
	}
	//const TSharedPtr<FJsonObject> JObjectSource = ArrayIt->AsObject()->GetObjectField("resource");

	//获取法线贴图数据
	const TSharedPtr<FJsonObject> JArrayNormal = ArrayIt->AsObject()->GetObjectField("normalMap");
	FString NormalMd5 = JArrayNormal->GetStringField("fileMd5");
	FString NormalUrl = JArrayNormal->GetStringField("fileUrl");
	FString NameAndExtensionNormal = FString::Printf(TEXT("%d_%s.%s"), IDPre, *NormalMd5, *GetFileExtension(NormalUrl));
	FString FilePathNormal = GetPathFromFileName(ESCTResourceType(resType), NameAndExtensionNormal);

	//获取真实材质贴图数据
	const TSharedPtr<FJsonObject> JArrayTexture = ArrayIt->AsObject()->GetObjectField("textureMap");
	FString TextureMd5 = JArrayTexture->GetStringField("fileMd5");
	FString TextureUrl = JArrayTexture->GetStringField("fileUrl");
	FString NameAndExtensionTexture = FString::Printf(TEXT("%d_%s.%s"), IDPre, *TextureMd5, *GetFileExtension(TextureUrl));
	FString FilePathTexture = GetPathFromFileName(ESCTResourceType(resType), NameAndExtensionTexture);

	bool upShelves = ArrayIt->AsObject()->GetBoolField("upShelves");

	TSharedPtr<FSCTContentItemSpace::FArmyTextureRes::TextureStruct> txtStr = MakeShareable<FSCTContentItemSpace::FArmyTextureRes::TextureStruct>(new FSCTContentItemSpace::FArmyTextureRes::TextureStruct);
	TSharedPtr<FSCTContentItemSpace::FArmyTextureRes::NoramlAndTexture> txtNormal = MakeShareable<FSCTContentItemSpace::FArmyTextureRes::NoramlAndTexture>(new FSCTContentItemSpace::FArmyTextureRes::NoramlAndTexture);
	TSharedPtr<FSCTContentItemSpace::FArmyTextureRes::NoramlAndTexture> txtTexture = MakeShareable<FSCTContentItemSpace::FArmyTextureRes::NoramlAndTexture>(new FSCTContentItemSpace::FArmyTextureRes::NoramlAndTexture);
	txtStr->ID = IDPre;
	txtStr->TextureName = NamePre;
	txtStr->Length = Length;
	txtStr->Width = Width;
	txtStr->Height = Height;
	txtStr->CategoryID = categoryID;
	txtStr->RealFileThumbnailURL = imagePre;
	txtStr->ResourceType = resType;
	//txtStr->EPavingMethod = PavingType;

	//存储法线贴图
	txtNormal->MD5 = NormalMd5;
	txtNormal->FileURL = NormalUrl;
	txtNormal->FilePath = FilePathNormal;
	txtStr->NoramlAndTextureArr.Add(txtNormal);

	//存储真实纹理
	txtTexture->MD5 = TextureMd5;
	txtTexture->FileURL = TextureUrl;
	txtTexture->FilePath = FilePathTexture;
	txtStr->NoramlAndTextureArr.Add(txtTexture);
	return txtStr;
}

TSharedPtr<FSCTContentItemSpace::FContentItem> FSCTXRCommonTools::ParseContentItemFromJson(TSharedPtr<FJsonObject> InJsonObject)
{
	if (InJsonObject.IsValid())
	{
		const TSharedPtr<FJsonObject> JObject = InJsonObject->GetObjectField("data");
		int32 Version = InJsonObject->GetIntegerField("version");
		if (JObject.IsValid())
		{
			int32 ID = JObject->GetIntegerField("id");
			FString Name = JObject->GetStringField("name");
			FString ThumbnailURL = JObject->GetStringField("thumbnailUrl");
			int32 ResourceType = JObject->GetIntegerField("resourceType");
			TSharedPtr<FSCTContentItemSpace::FResObj> ResPtr;

			const TSharedPtr<FJsonObject> ResourceObject = JObject->GetObjectField("resource");
			if (ResourceObject.IsValid())
			{
				FString FileURL = ResourceObject->GetStringField("fileUrl");
				FString FileMD5 = ResourceObject->GetStringField("fileMd5");
				FString FileName = FString::Printf(TEXT("%d_%s.%s"), ID, *FileMD5, *GetFileExtension(FileURL));
				FString FilePath = GetPathFromFileName(ESCTResourceType(ResourceType), FileName);

				if (ResourceType == (int32)ESCTResourceType::MoveableMesh || ResourceType == (int32)ESCTResourceType::Material || ResourceType == (int32)ESCTResourceType::BlueprintClass)
				{
					FString MaterialParameter = ResourceObject->GetStringField("materialParameter");
					FString LightParameter = ResourceObject->GetStringField("lightParameter");
					//创建ModelRes
					ResPtr = MakeShareable(new FSCTContentItemSpace::FModelRes(FileName, FilePath, FileURL, FileMD5, MaterialParameter, LightParameter));
				}
				else if (ResourceType == (int32)ESCTResourceType::Project)
				{
					FString CreateDateStr = ResourceObject->GetStringField("createDate");
					int64 CreateDate = FCString::Atoi64(*CreateDateStr);

					FString UpdateDateStr = ResourceObject->GetStringField("updateDate");
					int64 UpdateDate = FCString::Atoi64(*UpdateDateStr);
					//创建ProjectRes
					ResPtr = MakeShareable(new FSCTContentItemSpace::FProjectRes(FileName, FilePath, FileURL, FileMD5, CreateDate, UpdateDate));
				}
				else if (ResourceType == (int32)ESCTResourceType::DrawHome)
				{
					int32 HomeId = JObject->GetIntegerField("id");
					FString HomeName = JObject->GetStringField("name");
					int32 BedRoomNum = ResourceObject->GetIntegerField("bedroom");
					int32 LivingRoomNum = ResourceObject->GetIntegerField("livingRoom");
					int32 KitchenNum = ResourceObject->GetIntegerField("kitchen");
					int32 BathroomNum = ResourceObject->GetIntegerField("bathroom");
					float FloorArea = ResourceObject->GetNumberField("floorArea");
					float InnerArea = ResourceObject->GetNumberField("innerArea");
					int32 ProvinceId = ResourceObject->GetIntegerField("provinceId");
					int32 CityId = ResourceObject->GetIntegerField("cityId");
					int32 AreaId = ResourceObject->GetIntegerField("areaId");
					int32 VillageId = ResourceObject->GetIntegerField("village");
					FString AddressDetail = ResourceObject->GetStringField("addressDetail");
					int32 OrientationId = ResourceObject->GetIntegerField("homeForward");
					int32 BuildingTypeId = ResourceObject->GetIntegerField("buildingType");

					FString ConstructionFileURL = ResourceObject->GetStringField("constructionFileUrl");
					FString ConstructionFileMD5 = ResourceObject->GetStringField("constructionFileMd5");
					// 创建resource
					ResPtr = MakeShareable(new FSCTContentItemSpace::FArmyHomeRes(
						FileName,
						FilePath,
						FileURL,
						FileMD5,
						HomeId,
						HomeName,
						BedRoomNum,
						LivingRoomNum,
						KitchenNum,
						BathroomNum,
						FloorArea,
						InnerArea,
						ProvinceId,
						CityId,
						AreaId,
						VillageId,
						AddressDetail,
						OrientationId,
						BuildingTypeId
						));
				}
				else
				{
					ResPtr = MakeShareable(new FSCTContentItemSpace::FResObj(FileName, FilePath, FileURL, FileMD5));
				}
			}
			//创建ContentItem
			return MakeShareable(new FSCTContentItemSpace::FContentItem(
				ESCTResourceType(ResourceType), ID, Name, ThumbnailURL, ResPtr)
			);
		}
	}
	return NULL;
}

