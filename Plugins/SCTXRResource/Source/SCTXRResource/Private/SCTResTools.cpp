// Fill out your copyright notice in the Description page of Project Settings.

#include "SCTResTools.h"
#include "Regex.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "AES.h"
#include "PhysicsEngine/BodySetup.h"
#include "SCTXRResourceModule.h"
#include "PlatformFilemanager.h"
#include "SCTCityHash.h"

FSCTResTools::FSCTResTools()
{

}

bool FSCTResTools::RemoveFileNameExtension(FString& _FileName)
{
	int32 pos = _FileName.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	if (pos != -1)
	{
		_FileName = _FileName.Mid(0, pos);
		return true;
	}
	return false;
}

FString FSCTResTools::GetFileExtension(FString& _FileName)
{
	int32 pos = _FileName.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	if (pos != -1)
	{
		return _FileName.RightChop(pos + 1);
	}
	return TEXT("");
}

FString FSCTResTools::GetDrawHomeDir(bool _Test /*= false*/)
{
	return FPaths::ProjectContentDir() + TEXT("DBJCache/DrawHome/");
}

FString FSCTResTools::GetCookedHomeDir(bool _Test)
{
	if (_Test)
		return FPaths::ProjectContentDir() + TEXT("TESTPAK/Home/");
	else
		return FPaths::ProjectContentDir() + TEXT("DBJCache/CookedHome/");
}

FString FSCTResTools::GetModelDir(bool _Test /*= false*/)
{
	return FPaths::ProjectContentDir() + TEXT("DBJCache/Model/");
}

FString FSCTResTools::GetGroupPlanDir(bool _Test /*= false*/)
{
	if (_Test)
		return FPaths::ProjectContentDir() + TEXT("TESTPAK/GroupPlan/");
	else
		return FPaths::ProjectContentDir() + TEXT("DBJCache/GroupPlan/");
}

FString FSCTResTools::GetAutoDesignGroupPlanDir(bool _Test /*= false*/)
{
	if (_Test)
		return FPaths::ProjectContentDir() + TEXT("TESTPAK/AutoDesignGroupPlan/");
	else
		return FPaths::ProjectContentDir() + TEXT("DBJCache/AutoDesignGroupPlan/");
}

FString FSCTResTools::GetLevelPlanDir(bool _Test /*= false*/)
{
	if (_Test)
		return FPaths::ProjectContentDir() + TEXT("TESTPAK/LevelPlan/");
	else
		return FPaths::ProjectContentDir() + TEXT("DBJCache/LevelPlan/");
}

FString FSCTResTools::GetFileNameFromURL(FString& _URL)
{
	int32 pos = _URL.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	FString FileName = _URL.RightChop(pos + 1);
	return FileName;
}

FString FSCTResTools::GetXMLNameFromPath(FString& _XMLPath)
{
	int32 pos = _XMLPath.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	FString XMLName = _XMLPath.RightChop(pos + 1);
	return XMLName;
}

FString FSCTResTools::GetFileNameFromPath(FString& _FilePath, FString _FileExtention)
{
	int32 pos = _FilePath.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	FString FileName = _FilePath.RightChop(pos + 1);
	if (!_FileExtention.IsEmpty())
	{
		FileName.RemoveFromEnd(_FileExtention, ESearchCase::IgnoreCase);
	}
	return FileName;
}

FString FSCTResTools::GetFolderFromPath(FString& _FilePath)
{
	int32 pos = _FilePath.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	if (pos != -1)
	{
		return _FilePath.Mid(0, pos);
	}
	return "";
}

FString FSCTResTools::GetMD5FromFileName(FString _FileName)
{
	RemoveFileNameExtension(_FileName);
	//_FileName.RemoveFromEnd(TEXT(".pak"));
	int32 pos = _FileName.Find(TEXT("_&"), ESearchCase::IgnoreCase, ESearchDir::FromStart);
	if (pos != -1)
		return _FileName.RightChop(pos + 2);
	return TEXT("");
}

FString FSCTResTools::GetMD5FromFilePath(FString _FilePath)
{
	int32 pos = _FilePath.Find(TEXT("_&"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	if (pos != -1)
	{
		_FilePath = _FilePath.RightChop(pos + 2);
		RemoveFileNameExtension(_FilePath);
		_FilePath.RemoveFromEnd(TEXT(".pak"));
		return _FilePath;
	}
	return TEXT("");
}

FString FSCTResTools::GetExtFromFilePath(FString _FilePath)
{
	int32 pos = _FilePath.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	FString Ext = _FilePath.RightChop(pos);
	return Ext;
}

int32 FSCTResTools::GetPakIDFromPakName(FString _FileNameWithMD5)
{
	int32 pos = _FileNameWithMD5.Find(TEXT("_&"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	if (pos != -1)
	{
		return FCString::Atoi(*_FileNameWithMD5.Mid(0, pos));
	}
	return FCString::Atoi(*_FileNameWithMD5);
}



int32 FSCTResTools::GetFileIDFromName(FString _FileNameWithMD5)
{
	int32 pos = _FileNameWithMD5.Find(TEXT("_"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	if (pos != -1)
	{
		return FCString::Atoi(*_FileNameWithMD5.Mid(0, pos));
	}
	return FCString::Atoi(*_FileNameWithMD5);
}

uint32 FSCTResTools::GetFileIDFromNameHashValue(FString _FileName)
{	
	return CityHash32(TCHAR_TO_ANSI(*_FileName), _FileName.Len());
}

FString FSCTResTools::GetPureItemID(FString _OldItemID)
{
	//原来版本ID格式为M_5001，新版本需要转换为5001
	int32 pos = -1;
	_OldItemID.FindChar('_', pos);
	if (pos != -1)
	{
		return _OldItemID.RightChop(pos + 1);
	}
	return _OldItemID;
}

bool FSCTResTools::CheckTeleNumberValid(FString _str)
{
	const FRegexPattern Pattern(TEXT("^(1)\\d{10}$"));
	FRegexMatcher Matcher(Pattern, _str);
	if (Matcher.FindNext())
		return true;
	else
		return false;
}

bool FSCTResTools::CheckNumberValid(FString _str)
{
	const FRegexPattern Pattern(TEXT("^(0|[1-9][0-9]*)$"));
	FRegexMatcher Matcher(Pattern, _str);
	if (Matcher.FindNext())
		return true;
	else
		return false;
}

bool FSCTResTools::CheckIDCodeValid(FString _str)
{
	const FRegexPattern Pattern(TEXT("^\\d{17}(\\d|X|x)$"));
	FRegexMatcher Matcher(Pattern, _str);
	if (Matcher.FindNext())
		return true;

	return false;
}

bool FSCTResTools::CheckPasswordValid(FString _str)
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

bool FSCTResTools::CheckEmpty(FString _str)
{
	const FRegexPattern Pattern(TEXT("^\\s*|\\s*$"));
	FRegexMatcher Matcher(Pattern, _str);
	if (Matcher.FindNext())
		return false;

	return true;
}

bool FSCTResTools::CheckAreaValid(FString _str)
{
	const FRegexPattern Pattern(TEXT("^([1-9][0-9]*)$"));
	FRegexMatcher Matcher(Pattern, _str);
	if (Matcher.FindNext())
		return true;

	return false;
}

FString FSCTResTools::GetSaveDirExcel()
{
	return FPaths::ProjectSavedDir() + TEXT("SaveExcel/");
}

FString FSCTResTools::GetSaveDirPlan()
{
	return FPaths::ProjectSavedDir() + TEXT("SavePlan/");
}

FString FSCTResTools::GetSaveDirDIYHome()
{
	return FPaths::ProjectSavedDir() + TEXT("SaveDIYHome/");
}

FString FSCTResTools::GetSaveDirDatabase()
{
	return FPaths::ProjectSavedDir() + TEXT("SaveDatabase/");
}

FString FSCTResTools::GetSaveDirHomeStat()
{
	return FPaths::ProjectSavedDir() + TEXT("SaveHomeStat/");
}

FString FSCTResTools::GetExternalDir()
{
	return FPaths::ProjectContentDir() + TEXT("External/");
}

FString FSCTResTools::GetSaveScreenshot()
{
	return FPaths::ProjectSavedDir() + TEXT("SaveScreenshot/");
}

FString FSCTResTools::GetVR360SixFacesDir()
{
	return FPaths::ProjectSavedDir() + TEXT("VR360/");
}

FString FSCTResTools::GetPlanNameFromXMLName(FString _XMLName)
{
	FString result = _XMLName;
	int32 pos;
	result.FindChar('_', pos);
	result = result.RightChop(pos + 1);
	result.FindChar('_', pos);
	result = result.RightChop(pos + 1);

	pos = result.Find(TEXT("_"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	result = result.Mid(0, pos);

	return result;
}

FString FSCTResTools::GetDayHourMinStringFromSeconds(int32 _Seconds)
{
	int32 days = _Seconds / 86400;
	int32 hours = _Seconds % 86400 / 3600;
	int32 mins = _Seconds - days * 86400 - hours * 3600;
	int32 secs = mins % 60;
	mins = mins / 60;
	//int32 mins = _Seconds % 86400 & 3600 / 60;
	return FString::Printf(TEXT("%d天%d小时%d分%d秒"), days, hours, mins, secs);
}

FString FSCTResTools::EncryptString(FString _Str)
{
	return _Str;
	//TArray<uint8> UserNameBufferEncrypt;
	//UserNameBufferEncrypt.SetNum(_Str.Len());
	//memcpy(UserNameBufferEncrypt.GetData(), TCHAR_TO_ANSI(*_Str), _Str.Len());
	////Encrypt
	//FString UserNameEncrypt;
	//FAES::EncryptData(UserNameBufferEncrypt.GetData(), 16);
	//FFileHelper::BufferToString(UserNameEncrypt, UserNameBufferEncrypt.GetData(), UserNameBufferEncrypt.Num());
	////memcpy((void*)(*UserNameEncrypt), UserNameBufferEncrypt.GetData(), UserNameBufferEncrypt.Num());
	////UserNameEncrypt = UTF8_TO_TCHAR(UserNameBufferEncrypt.GetData());
	////TArray<uint8> UserNameBufferDecrypt;
	////UserNameBufferDecrypt.SetNum(UserNameEncrypt.Len());
	////memcpy(UserNameBufferDecrypt.GetData(), TCHAR_TO_ANSI(*UserNameEncrypt), UserNameEncrypt.Len());

	//TArray<uint8> UserNameBufferDecrypt;
	//UserNameBufferDecrypt.SetNum(UserNameEncrypt.Len());
	//memcpy(UserNameBufferDecrypt.GetData(), TCHAR_TO_ANSI(*UserNameEncrypt), UserNameEncrypt.Len());
	////UserNameBufferDecrypt.GetData() = TCHAR_TO_UTF8(*UserNameEncrypt);

	////Decrypt
	//FString UserNameDecrypt;
	//FAES::DecryptData(UserNameBufferDecrypt.GetData(), 16);
	//FFileHelper::BufferToString(UserNameDecrypt, UserNameBufferDecrypt.GetData(), UserNameBufferDecrypt.Num());
	////memcpy(TCHAR_TO_ANSI(*UserNameDecrypt), UserNameBufferDecrypt.GetData(), UserNameBufferDecrypt.Num());
	////UserNameDecrypt = UTF8_TO_TCHAR(UserNameBufferDecrypt.GetData());
	//return UserNameEncrypt;
}

FString FSCTResTools::DecryptString(FString _Str)
{
	return _Str;
	//TArray<uint8> UserNameBufferEncrypt;
	//UserNameBufferEncrypt.SetNum(_Str.Len());
	//memcpy(UserNameBufferEncrypt.GetData(), TCHAR_TO_ANSI(*_Str), _Str.Len());
	////Encrypt
	//FString UserNameDecrypt;
	//FAES::DecryptData(UserNameBufferEncrypt.GetData(), 16);
	//UserNameDecrypt = UTF8_TO_TCHAR(UserNameBufferEncrypt.GetData());

	//return UserNameDecrypt;
}

void FSCTResTools::RedirectHttp(FString _URL)
{
	if (_URL == TEXT(""))
		return;
	FPlatformMisc::OsExecute(TEXT("open"), *_URL);
}

void FSCTResTools::VRSWriteLog(FString _str)
{
	UE_LOG(ResLog, Warning, TEXT("%s"), *_str);
}

bool FSCTResTools::XROpenDirectoryDialog(const FString& DialogTitle, const FString& DefaultPath, FString& OutFolderName)
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (DesktopPlatform)
	{
		void* ParentWindowWindowHandle = NULL;
		ParentWindowWindowHandle = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
		FString FolderName;
		const bool bFolderSelected = DesktopPlatform->OpenDirectoryDialog(
			ParentWindowWindowHandle,
			DialogTitle,
			DefaultPath,
			OutFolderName
		);

		if (bFolderSelected)
		{
			return true;
		}
	}
	return false;
}

bool FSCTResTools::XROpenFileDialog(const FString& DialogTitle, const FString& DefaultPath, TArray<FString>& OutFileNames, FString _FileType, bool _bMulty)
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (DesktopPlatform)
	{
		void* ParentWindowWindowHandle = NULL;
		ParentWindowWindowHandle = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
		FString FolderName;
		const bool bFolderSelected = DesktopPlatform->OpenFileDialog(
			ParentWindowWindowHandle,
			DialogTitle,
			DefaultPath,
			TEXT(""),
			_FileType,//TEXT("Pictures (*.jpg)|*.jpg;*.jpeg|Pictures (*.png)|*.png"),
			_bMulty ? EFileDialogFlags::Multiple : EFileDialogFlags::None,
			OutFileNames
		);

		if (bFolderSelected)
		{
			return true;
		}
	}
	return false;
}

FString FSCTResTools::AddCommaForNumber(FString _Number)
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

FString FSCTResTools::AddCommaForNumber(float _Number)
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

void FSCTResTools::DeleteFile(FString& Filename)
{
	if (!FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*Filename))
	{
		UE_LOG(ResLog, Warning, TEXT("Failed to delete temp file: %s"), *Filename);
	}
}

bool FSCTResTools::ContainsPointLightComponent(AActor* _Actor)
{
	return false;
}

bool FSCTResTools::ContainsSpotLightComponent(AActor* _Actor)
{
	return false;
}

bool FSCTResTools::ContainsLightComponent(AActor* _Actor)
{
	return false;
}

bool FSCTResTools::GetLightComponentCastShadow(bool _bPointLight, AActor* _Actor)
{
	return false;
}

float FSCTResTools::GetLightComponentIntensity(bool _bPointLight, AActor* _Actor)
{
	return 0.f;
}

FLinearColor FSCTResTools::GetLightComponentColor(bool _bPointLight, AActor* _Actor)
{
	return FLinearColor(1,1,1,1);
}

float FSCTResTools::GetLightComponentRadius(bool _bPointLight, AActor* _Actor)
{
	return 0.f;
}


float FSCTResTools::GetSpotLightComponentRadius(AActor* _Actor)
{
	return 0.f;
}

float FSCTResTools::GetSpotLightComponentInnerAngle(AActor* _Actor)
{
	return 0.f;
}

float FSCTResTools::GetSpotLightComponentOutterAngle(AActor* _Actor)
{
	return 0.f;
}

UTextureLightProfile* FSCTResTools::GetSpotLightComponentIES(AActor* _Actor)
{
	return NULL;
}

//Set
void FSCTResTools::SetLightComponentCastShadow(bool _bPointLight, AActor* _Actor, bool _CastShadow)
{
}

void FSCTResTools::SetLightComponentIntensity(bool _bPointLight, AActor* _Actor, float _Intensity)
{
}

void FSCTResTools::SetLightComponentColor(bool _bPointLight, AActor* _Actor, FLinearColor _Color)
{
}

void FSCTResTools::SetLightComponentRadius(bool _bPointLight, AActor* _Actor, float _Radius)
{
}


void FSCTResTools::SetSpotLightComponentRadius(AActor* _Actor, float _Raidus)
{
}

void FSCTResTools::SetSpotLightComponentInnerAngle(AActor* _Actor, float _InnerAngle)
{
}

void FSCTResTools::SetSpotLightComponentOutterAngle(AActor* _Actor, float _OutterAngle)
{
}

void FSCTResTools::SetSpotLightComponentIES(AActor* _Actor, UTextureLightProfile* _IES)
{
}

