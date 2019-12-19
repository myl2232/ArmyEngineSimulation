// Fill out your copyright notice in the Description page of Project Settings.

#include "ArmyCommonTools.h"
#include "ArmyObject.h"
#include "Regex.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "AES.h"
//#include "PhysicsEngine/BodySetup.h"
#include "PlatformFilemanager.h"
#include "ArmyCommonTypes.h"
#include "ArmyDataTools.h"


//给水点位
#define CTWaterSupply "GC0693"

// 配电箱
#define CTEleBox "00040001" 
//强电箱
#define CTStrongEleBox "GC0299"
//燃气表
#define CTGas "GC0015"
//水盆下水
#define CTWaterBasin "GC0694"
//地漏下水
#define CTFloorDrain "GC0695"
//马桶下水
#define CTClosestoolDrain "GC0696"
//热水口
#define CTHotWater "GC0096"
//冷水口
#define CTColdWater "GC0687"
//冷热水口
#define CTHotColdWater "GC0686"
//分集水器
#define CTCollectSeprateWater "GC0043"

//弱电箱
#define CTWeakEleBox "GC0303"
// 开关
#define CTSWITCH "00040012"
// 强电插座
#define  CTSocketStrong "GC0688"
//弱电插座
#define CTSocketWeak "GC0671"
//电脑插座
#define  CTSocketNet "GC0314"
//电视插座
#define  CTSocketTV "GC0323"

//电脑+电话插座
#define  CTSocketNetPhone "GC0355"

//电话插座
#define  CTSocketPhone "GC0329"

//电脑+电视插座
#define  CTSocketNetTV "GC0361"
//电话+电视插座
#define  CTSocketPhoneTV "GC0367"
//音频插座
#define  CTSocketMusic "GC0373"

//电源插座
#define  CTSocketSupply "GC0311"
//联排插座
#define  CTSocketLian "GC0378"


// 水路点位
#define CTWater_Point "000600180004"

// 预装点位
#define CTPreload "GC0690"
//2.5平方强电线路
#define CTStrong_25h "GC0497"
//4平方强电线路
#define CTStrong_4 "GC0503"
//6平方强电线路
#define CTStrong_6 "GC0689"
//照明线路
#define CTLight_Wire "GC0724"

//单控强电布线
#define CTStrong_Single "GC0509"
//双控强电布线
#define CTStrong_Double "GC0517"
//电视线
#define CTWeakElectricity_TV "GC0525"
//网线
#define CTWeakElectricity_Net "GC0531"
//电话线
#define CTWeakElectricity_Phone "GC0528"
//冷水管
#define CTColdWaterTube "GC0070"
//热水管
#define CTHotWaterTube "GC0088"
//排水管
#define CTDrain "GC0092"

#define PT_PipeDiameter "GP1196"

using namespace FContentItemSpace;

FArmyCommonTools::FArmyCommonTools()
{
}

void FArmyCommonTools::InitStaticData()
{
	GlobalComponentCodeMap.Add(TEXT("M0032"), EC_Socket_Five_Point);
	GlobalComponentCodeMap.Add(TEXT("M0033"), EC_Socket_Five_On_Point);
	GlobalComponentCodeMap.Add(TEXT("M0034"), EC_Socket_Five_On_Dislocation);
	GlobalComponentCodeMap.Add(TEXT("M0035"), EC_Socket_Five_Point_Dislocation);
	GlobalComponentCodeMap.Add(TEXT("M0036"), EC_Socket_Four_Point);
	GlobalComponentCodeMap.Add(TEXT("M0037"), EC_Socket_Four_On_Point);
	GlobalComponentCodeMap.Add(TEXT("M0038"), EC_Socket_Three_On_Point);
	GlobalComponentCodeMap.Add(TEXT("M0039"), EC_Socket_Three);
	GlobalComponentCodeMap.Add(TEXT("M0040"), EC_Socket_Three_On_Point_3A);
	GlobalComponentCodeMap.Add(TEXT("M0041"), EC_Socket_Three_3A);
	GlobalComponentCodeMap.Add(TEXT("M0042"), EC_Socket_Three_On_Point_10A);
	GlobalComponentCodeMap.Add(TEXT("M0043"), EC_Socket_Air_Conditioner);
	GlobalComponentCodeMap.Add(TEXT("M0044"), EC_Socket_USB);
	GlobalComponentCodeMap.Add(TEXT("M0045"), EC_Socket_Air_Conditionner_Normal);
	GlobalComponentCodeMap.Add(TEXT("M0046"), EC_Socket_HotWater);
	GlobalComponentCodeMap.Add(TEXT("M0047"), EC_Socket_PaiYan);
	GlobalComponentCodeMap.Add(TEXT("M0048"), EC_Socket_Spatter);
	GlobalComponentCodeMap.Add(TEXT("M0049"), EC_Socket_IT);
	GlobalComponentCodeMap.Add(TEXT("M0050"), EC_Socket_IT_TV);
	GlobalComponentCodeMap.Add(TEXT("M0051"), EC_Socket_TV);
	GlobalComponentCodeMap.Add(TEXT("M0052"), EC_Socket_Phone);
	GlobalComponentCodeMap.Add(TEXT("M0053"), EC_Socket_IT_Phone);
	GlobalComponentCodeMap.Add(TEXT("M0054"), EC_Socket_TV_Phone);
	GlobalComponentCodeMap.Add(TEXT("M0055"), EC_Socket_TP);
	GlobalComponentCodeMap.Add(TEXT("M0056"), EC_Socket_TV_Couplet);
	GlobalComponentCodeMap.Add(TEXT("M0057"), EC_EleBoxL_Point);
	GlobalComponentCodeMap.Add(TEXT("M0058"), EC_EleBoxH_Point);
	GlobalComponentCodeMap.Add(TEXT("M0059"), EC_Switch_BathHeater);
	GlobalComponentCodeMap.Add(TEXT("M0060"), EC_Switch_Double);
	GlobalComponentCodeMap.Add(TEXT("M0061"), EC_Switch_Single);
	GlobalComponentCodeMap.Add(TEXT("M0062"), EC_Switch4O2_Point);
	GlobalComponentCodeMap.Add(TEXT("M0063"), EC_Switch4O1_Point);
	GlobalComponentCodeMap.Add(TEXT("M0064"), EC_Switch3O2_Point);
	GlobalComponentCodeMap.Add(TEXT("M0065"), EC_Switch3O1_Point);
	GlobalComponentCodeMap.Add(TEXT("M0066"), EC_Switch2O2_Point);
	GlobalComponentCodeMap.Add(TEXT("M0067"), EC_Switch2O1_Point);
	GlobalComponentCodeMap.Add(TEXT("M0175"), EC_Switch1O2_Point);//后台名称写错需要修改 以前是M0068
	GlobalComponentCodeMap.Add(TEXT("M0069"), EC_Switch1O1_Point);
	GlobalComponentCodeMap.Add(TEXT("M0070"), EC_Water_Chilled_Point);
	GlobalComponentCodeMap.Add(TEXT("M0071"), EC_Water_Hot_Chilled_Point);
	GlobalComponentCodeMap.Add(TEXT("M0072"), EC_Dewatering_Point);
	GlobalComponentCodeMap.Add(TEXT("M0073"), EC_Water_Supply);
	GlobalComponentCodeMap.Add(TEXT("M0074"), EC_Drain_Point);
	GlobalComponentCodeMap.Add(TEXT("M0075"), EC_Basin);
	GlobalComponentCodeMap.Add(TEXT("M0076"), EC_Water_Basin);
	GlobalComponentCodeMap.Add(TEXT("M0077"), EC_Water_Hot_Point);
	GlobalComponentCodeMap.Add(TEXT("M0078"), EC_Closestool);
	GlobalComponentCodeMap.Add(TEXT("M0080"), EC_Supply_Same_Tee);
	GlobalComponentCodeMap.Add(TEXT("M0081"), EC_Supply_Diff_Tee);
	GlobalComponentCodeMap.Add(TEXT("M0082"), EC_Supply_Same_45_Tee);
	GlobalComponentCodeMap.Add(TEXT("M0083"), EC_Supply_Diff_45_Tee);
	GlobalComponentCodeMap.Add(TEXT("M0084"), EC_Supply_NoPlane_Same_Tee);
	GlobalComponentCodeMap.Add(TEXT("M0085"), EC_Supply_NoPlane_Diff_Tee);
	GlobalComponentCodeMap.Add(TEXT("M0086"), EC_Supply_Same_90_Flexure);
	GlobalComponentCodeMap.Add(TEXT("M0087"), EC_Supply_Diff_90_Flexure);
	GlobalComponentCodeMap.Add(TEXT("M0088"), EC_Supply_Same_45_Flexure);
	GlobalComponentCodeMap.Add(TEXT("M0089"), EC_Supply_Diff_45_Flexure);
	GlobalComponentCodeMap.Add(TEXT("M0090"), EC_Supply_Same_Direct);
	GlobalComponentCodeMap.Add(TEXT("M0091"), EC_Supply_Diff_Direct);
	GlobalComponentCodeMap.Add(TEXT("M0092"), EC_Supply_Plane_Same_FourLinks);
	GlobalComponentCodeMap.Add(TEXT("M0093"), EC_Supply_Plane_Diff_FourLinks);
	GlobalComponentCodeMap.Add(TEXT("M0094"), EC_Supply_NoPlane_Same_FourLinks);
	GlobalComponentCodeMap.Add(TEXT("M0095"), EC_Supply_NoPlane_Diff_FourLinks);
	GlobalComponentCodeMap.Add(TEXT("M0096"), EC_Drain_Bottle_Tee);
	GlobalComponentCodeMap.Add(TEXT("M0097"), EC_Drain_Same_Tee);
	GlobalComponentCodeMap.Add(TEXT("M0098"), EC_Drain_Diff_Tee);
	GlobalComponentCodeMap.Add(TEXT("M0099"), EC_Drain_Same_45_Tee);
	GlobalComponentCodeMap.Add(TEXT("M0100"), EC_Drain_Diff_45_Tee);
	GlobalComponentCodeMap.Add(TEXT("M0101"), EC_Drain_Same_Direct);
	GlobalComponentCodeMap.Add(TEXT("M0102"), EC_Drain_Diff_Direct);
	GlobalComponentCodeMap.Add(TEXT("M0103"), EC_Drain_Same_45_Flexure);
	GlobalComponentCodeMap.Add(TEXT("M0104"), EC_Drain_Diff_45_Flexure);
	GlobalComponentCodeMap.Add(TEXT("M0105"), EC_Drain_Same_90_Flexure);
	GlobalComponentCodeMap.Add(TEXT("M0106"), EC_Drain_Diff_90_Flexure);
	GlobalComponentCodeMap.Add(TEXT("M0107"), EC_Drain_Plane_Same_FourLinks);
	GlobalComponentCodeMap.Add(TEXT("M0108"), EC_Drain_Plane_Diff_FourLinks);
	GlobalComponentCodeMap.Add(TEXT("M0109"), EC_Drain_Same_45_FourLinks);
	GlobalComponentCodeMap.Add(TEXT("M0110"), EC_Drain_Diff_45_FourLinks);
	GlobalComponentCodeMap.Add(TEXT("M0111"), EC_Drain_NoPlane_Same_FourLinks);
	GlobalComponentCodeMap.Add(TEXT("M0112"), EC_Drain_NoPlane_Diff_FourLinks);
	GlobalComponentCodeMap.Add(TEXT("M0113"), EC_Trap_P);
	GlobalComponentCodeMap.Add(TEXT("M0114"), EC_Trap_S);
	GlobalComponentCodeMap.Add(TEXT("M0115"), EC_Trap_U);
	GlobalComponentCodeMap.Add(TEXT("M0116"), EC_Bridge_Same);
	GlobalComponentCodeMap.Add(TEXT("M0117"), EC_Bridge_Diff);
	GlobalComponentCodeMap.Add(TEXT("M0118"), EC_Gas_MainPipe);
	GlobalComponentCodeMap.Add(TEXT("M0119"), EC_Gas_Meter);
	//@欧石楠 客厅联排插座
	GlobalComponentCodeMap.Add(TEXT("M0223"), EC_Socket_TV_Couplet);

	//GlobalHardModelCodeMap.Add(TEXT("GC0064"), EC_Gas_Meter);//瓷砖 	 
	//GlobalHardModelCodeMap.Add(TEXT("GC0108"), EC_Gas_Meter);//地板 	 
	GlobalHardModelCodeMap.Add(TEXT("GC0270"), CAT_CrownMoulding);//顶角线 	
	GlobalHardModelCodeMap.Add(TEXT("GC0282"), CAT_FloorKickingLine);//踢脚线 	
	GlobalHardModelCodeMap.Add(TEXT("GC0683"), CAT_LampSlot);//灯槽 	 
	GlobalHardModelCodeMap.Add(TEXT("GC0287"), CAT_MoldingLine);//造型线 

	GlobalHardModelCodeMap.Add(TEXT("GC0294"), CAT_WallPaper);//内墙乳胶漆 
	GlobalHardModelCodeMap.Add(TEXT("GC0162"), CAT_WallPaper);//壁纸 	 
	GlobalHardModelCodeMap.Add(TEXT("GC0679"), CAT_AntiCornerBrickMethod);//波打线 	
	//GlobalHardModelCodeMap.Add(TEXT("GC0234"), EC_Gas_Meter);//室内门 	
	//GlobalHardModelCodeMap.Add(TEXT("GC0237"), EC_Gas_Meter);//防盗门 	
	GlobalHardModelCodeMap.Add(TEXT("GC0669"), CAT_Pass);//垭口 	 
	//GlobalHardModelCodeMap.Add(TEXT("GC0026"), EC_Gas_Meter);//窗台石 	
	//GlobalHardModelCodeMap.Add(TEXT("GC0240"), EC_Gas_Meter);//推拉门 	
	GlobalHardModelCodeMap.Add(TEXT("GC0124"), CAT_Buckle);//扣条 	 



	//GlobalComponentCodeMap.Add(Server_EC_Water_Normal_Point, EC_Water_Normal_Point);
	//GlobalComponentCodeMap.Add(Server_EC_Water_Separator_Point, EC_Water_Separator_Point);


}
FString FArmyCommonTools::GetComponentCodeByLocalID(EComponentID InID)
{
	for (auto& It : GlobalComponentCodeMap)
	{
		if (It.Value == InID)
		{
			return It.Key;
		}
	}
	return TEXT("");
}
FString FArmyCommonTools::GetCategoryCodeByLocalID(EComponentID InID)
{
	switch (InID)
	{
	case EComponentID::EC_EleBoxH_Point:
		return TEXT(CTStrongEleBox);
	case EComponentID::EC_EleBoxL_Point:
		return TEXT(CTWeakEleBox);
	case EC_Gas_Meter:// = 1201 //燃气表
		return TEXT(CTGas);
	case	EC_Water_Normal_Point: //中水点位
		return TEXT(CTWeakEleBox);
	case	EC_Water_Separator_Point://分集水器
		return TEXT(CTCollectSeprateWater);
	case	EC_Water_Chilled_Point: //= 17,//冷水点位
		return TEXT(CTColdWater);
	case	EC_Water_Hot_Chilled_Point: //= 18,//冷热水
		return TEXT(CTHotColdWater);
	case	EC_Basin ://= 60,//地漏下水
		return TEXT(CTFloorDrain);
	case	EC_Water_Basin:// = 59,//水盆下水
		return TEXT(CTWaterBasin);
	case	EC_Water_Hot_Point:// = 55,//热水点位
		return TEXT(CTHotWater);
	case	EC_Closestool:// = 31,//马桶下水
		return TEXT(CTClosestoolDrain);
	}
	return FString();
}
EComponentID FArmyCommonTools::GetDefaultComponentLocalIDByCategoryID(const FString & InCode)
{
	static TArray<FString> SwitchList = {
	TEXT("GC0401"),
	TEXT("GC0425"),
	TEXT("GC0429"),
	TEXT("GC0433"),
	TEXT("GC0436"),
	TEXT("GC0438"),
	TEXT("GC0441"),
	TEXT("GC0446"),
	TEXT("GC0450"),
	TEXT("GC0457"),
	TEXT("GC0461"),
	TEXT("GC0468"),
	TEXT("GC0539"),
	TEXT("GC0727"),
	};

	static TArray<FString>  StrongSlot = {
		TEXT("GC0311"),
		TEXT("GC0314"),
		TEXT("GC0323"),
		TEXT("GC0329"),
		TEXT("GC0355"),
		TEXT("GC0361"),
		TEXT("GC0367"),
		TEXT("GC0373"),
		TEXT("GC0378"),
		TEXT("GC0713"),
		TEXT("GC0714"),
		
	};
	//水点位
	if (InCode == TEXT(CTWaterSupply)
		|| InCode == TEXT(CTColdWater)
		|| InCode == TEXT(CTHotWater)
		|| InCode == TEXT(CTWater_Point)
		|| InCode == TEXT(CTHotColdWater)
		)
	{
		return EC_Water_Defualt;
	}

	//开关
	if (SwitchList.Contains(InCode))
	{
		return EC_Switch_Defualt;
	}

	//强电箱
	if (InCode == TEXT(CTEleBox)
		|| InCode == TEXT(CTStrongEleBox)
		)
	{
		return EC_EleBoxH_Point;
	}

	//弱电箱
	if (InCode == TEXT(CTWeakEleBox)
		)
	{
		return EC_EleBoxL_Point;
	}

	//强电插座
	if (StrongSlot .Contains(InCode))
	{
		return EC_Socket_H_Defualt;
	}

	//弱电插座
	if (InCode == TEXT(CTSocketWeak))
	{
		return EC_Socket_L_Defualt;
	}


	return EComponentID();
}
EComponentID FArmyCommonTools::GetComponentLocalIDByCode(const FString& InCode)
{
	return GlobalComponentCodeMap.FindRef(InCode);
}
FString FArmyCommonTools::GetPathFromFileName(EResourceType InResourceType, const FString& InFileName)
{
	FString FilePath;
	//绘制户型文件
	if (InResourceType == EResourceType::Home)
		FilePath = GetDrawHomeDir() + InFileName;
	//烘焙户型文件
	else if (InResourceType == EResourceType::CookedHome)
		FilePath = GetCookedHomeDir() + InFileName;
	//模型文件
	else if (InResourceType == EResourceType::MoveableMesh || InResourceType == EResourceType::HardPAK || InResourceType == EResourceType::Hydropower)
		FilePath = GetModelDir() + InFileName;
	//材质文件
	else if (InResourceType == EResourceType::Material)
		FilePath = GetModelDir() + InFileName;
	//Actor文件
	else if (InResourceType == EResourceType::BlueprintClass)
		FilePath = GetModelDir() + InFileName;
	//组方案
	else if (InResourceType == EResourceType::GroupPlan)
		FilePath = GetGroupPlanDir() + InFileName;
	//户型方案
	else if (InResourceType == EResourceType::Plan)
		FilePath = GetLevelPlanDir() + InFileName;
	//地板拼花方案
	else if (InResourceType == EResourceType::Texture)
		FilePath = GetFloorTextureDir() + InFileName;
	else if (InResourceType == ComponentDXF || InResourceType == EResourceType::HardDXF)
		FilePath = GetComponentDir() + InFileName;
    else if (InResourceType == Facsimile) // @欧石楠 底图
    {
        FilePath = GetFacsimilesDir() + InFileName;
    }

	return FilePath;
}

bool FArmyCommonTools::RemoveFileNameExtension(FString& _FileName)
{
	int32 pos = _FileName.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	if (pos != -1)
	{
		_FileName = _FileName.Mid(0, pos);
		return true;
	}
	return false;
}

FString FArmyCommonTools::GetFileExtension(FString& _FileName)
{
	int32 pos = _FileName.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	if (pos != -1)
	{
		return _FileName.RightChop(pos + 1);
	}
	return TEXT("");
}

FString FArmyCommonTools::GetCommonDir()
{
	return FPaths::ProjectContentDir() + TEXT("ZKCache/Common/");
}

FString FArmyCommonTools::GetDrawHomeDir()
{
	return FPaths::ProjectContentDir() + TEXT("ZKCache/DrawHome/");
}
FString FArmyCommonTools::GetConstructionDir()
{
	return FPaths::ProjectContentDir() + TEXT("ZKCache/Construction/");
}
FString FArmyCommonTools::GetCookedHomeDir()
{
	return FPaths::ProjectContentDir() + TEXT("ZKCache/CookedHome/");
}

FString FArmyCommonTools::GetModelDir()
{
	return FPaths::ProjectContentDir() + TEXT("ZKCache/Model/");
}

FString FArmyCommonTools::GetGroupPlanDir()
{
	return FPaths::ProjectContentDir() + TEXT("ZKCache/GroupPlan/");
}

FString FArmyCommonTools::GetAutoDesignGroupPlanDir()
{
	return FPaths::ProjectContentDir() + TEXT("ZKCache/AutoDesignGroupPlan/");
}

FString FArmyCommonTools::GetLevelPlanDir()
{
	return FPaths::ProjectContentDir() + TEXT("ZKCache/LevelPlan/");
}

FString FArmyCommonTools::GetFloorTextureDir()
{
	return FPaths::ProjectContentDir() + TEXT("ZKCache/FloorTexture/");
}
FString FArmyCommonTools::GetComponentDir()
{
	return FPaths::ProjectContentDir() + TEXT("ZKCache/Component/");
}

FString FArmyCommonTools::GetFacsimilesDir()
{
    return FPaths::ProjectContentDir() + TEXT("ZKCache/Facsimiles/");
}

FString FArmyCommonTools::GetFileNameFromPath(FString& _FilePath)
{
	int32 pos = _FilePath.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	FString FileName = _FilePath.RightChop(pos + 1);

	return FileName;
}

FString FArmyCommonTools::GetFolderFromPath(FString& _FilePath)
{
	int32 pos = _FilePath.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	if (pos != -1)
	{
		return _FilePath.Mid(0, pos);
	}
	return "";
}

int32 FArmyCommonTools::GetFileIDFromName(FString _FileNameWithMD5)
{
	int32 pos = _FileNameWithMD5.Find(TEXT("_"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	if (pos != -1)
	{
		return FCString::Atoi(*_FileNameWithMD5.Mid(0, pos));
	}
	return FCString::Atoi(*_FileNameWithMD5);
}

bool FArmyCommonTools::CheckTeleNumberValid(FString _str)
{
	const FRegexPattern Pattern(TEXT("^(1)\\d{10}$"));
	FRegexMatcher Matcher(Pattern, _str);
	if (Matcher.FindNext())
		return true;
	else
		return false;
}

bool FArmyCommonTools::CheckNumberValid(FString _str)
{
	const FRegexPattern Pattern(TEXT("^(0|[1-9][0-9]*)$"));
	FRegexMatcher Matcher(Pattern, _str);
	if (Matcher.FindNext())
		return true;
	else
		return false;
}

bool FArmyCommonTools::CheckIDCodeValid(FString _str)
{
	const FRegexPattern Pattern(TEXT("^\\d{17}(\\d|X|x)$"));
	FRegexMatcher Matcher(Pattern, _str);
	if (Matcher.FindNext())
		return true;

	return false;
}

bool FArmyCommonTools::CheckPasswordValid(FString _str)
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

bool FArmyCommonTools::CheckEmpty(FString _str)
{
	const FRegexPattern Pattern(TEXT("^\\s*|\\s*$"));
	FRegexMatcher Matcher(Pattern, _str);
	if (Matcher.FindNext())
		return false;

	return true;
}

bool FArmyCommonTools::CheckAreaValid(FString _str)
{
	const FRegexPattern Pattern(TEXT("^([1-9][0-9]*)$"));
	FRegexMatcher Matcher(Pattern, _str);
	if (Matcher.FindNext())
		return true;

	return false;
}

FString FArmyCommonTools::AddCommaForNumber(FString _Number)
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

FString FArmyCommonTools::AddCommaForNumber(float _Number)
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

int32 FArmyCommonTools::ConvertVersionStrToInt(FString InVersionStr)
{
	int32 FindResult = -1;
	while (InVersionStr.FindChar('.', FindResult))
	{
		InVersionStr.RemoveAt(FindResult);
		FindResult = -1;
	}
	return FCString::Atoi(*InVersionStr);
}

void FArmyCommonTools::DeleteFiles(int32 InID, EResourceType InType)
{
	FString FileExtension;
	if (InType == EResourceType::Home ||
		InType == EResourceType::Plan ||
		InType == EResourceType::GroupPlan ||
		InType == EResourceType::Construction ||
		InType == EResourceType::CustomlizedFloor ||
		InType == EResourceType::CustomlizedCeling ||
		InType == EResourceType::CustomlizedWall
		)
	{
		FileExtension = "json";
	}
	else
	{
		FileExtension = "pak";
	}

	FString FileFolder = FArmyCommonTools::GetPathFromFileName(InType, "");
	FString FileNameMatchedID = FString::Printf(TEXT("%d_*.%s"), InID, *FileExtension);
	TArray<FString> Filenames;
	IFileManager::Get().FindFilesRecursive(Filenames, *FileFolder, *FileNameMatchedID, true, false);

	for (auto& It : Filenames)
	{
		FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*It);
	}
}

//TSharedPtr<FProjectContentItem> FArmyCommonTools::ParseProjectContentItemFromJson(TSharedPtr<FJsonObject> InJsonObject)
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
void FArmyCommonTools::ParseObjectGroupsFromJson(TSharedPtr<FJsonObject> InJsonObject, TArray<TSharedPtr<FObjectGroupContentItem>>& OutArray)
{
	if (InJsonObject.IsValid())
	{
		int32 ModelID = InJsonObject->GetIntegerField("id");
		FString ItemPath("");
		FString ModeName("");


		ParseObjectGroupsFromJson(InJsonObject, ModeName, ItemPath, OutArray);
	}
}

void FArmyCommonTools::ParseObjectGroupsFromJson(TSharedPtr<FJsonObject> InJsonObject, FString& ModeName, FString& ItemPath, TArray<TSharedPtr<FObjectGroupContentItem>>& OutArray)
{
	if (InJsonObject.IsValid())
	{
		int32 ModelID = InJsonObject->GetIntegerField("id");
		ModeName = ModeName.Equals("") ? InJsonObject->GetStringField("directoryName") : ModeName;
		const TArray< TSharedPtr<FJsonValue> > childDirectory = InJsonObject->GetArrayField("childDirectory");

		if (childDirectory.Num())
		{
			for (auto child : childDirectory)
			{
				TSharedPtr<FJsonObject> childObj = child->AsObject();
				FString NewIemPath = ItemPath.Equals("") ? InJsonObject->GetStringField("directoryName") : ItemPath + ("/") + InJsonObject->GetStringField("directoryName");
				ParseObjectGroupsFromJson(childObj, ModeName, NewIemPath, OutArray);
			}
		}
		else
		{
			TSharedPtr<FObjectGroupContentItem> GroupItem = MakeShareable(new FObjectGroupContentItem);
			GroupItem->ModelID = ModelID;
			GroupItem->ModelName = ModeName;
			if (InJsonObject.IsValid())
			{
				GroupItem->ItemPath = ItemPath.Equals("") ? InJsonObject->GetStringField("directoryName") : ItemPath + TEXT("/") + InJsonObject->GetStringField("directoryName");;
				GroupItem->VisibleInOtherModel = InJsonObject->GetIntegerField("facadeHydropowerShowMenu");
				const TArray< TSharedPtr<FJsonValue> >JsonCategories = InJsonObject->GetArrayField("categories");
				for (auto JsonV : JsonCategories)
				{
					GroupItem->CategoryList.Add(JsonV->AsNumber());
				}
				OutArray.Add(GroupItem);
			}
			return;
		}
	}
}

FCategoryContentItem FArmyCommonTools::ParseCategoryFromJson(TSharedPtr<FJsonObject> InJsonObject)
{
	if (InJsonObject.IsValid())
	{
		FString name = InJsonObject->GetStringField("name");
		int32 level = InJsonObject->GetIntegerField("level");
		int32 id = InJsonObject->GetIntegerField("id");
		int32 parentId = InJsonObject->GetIntegerField("parentId");
		FCategoryContentItem categordata = FCategoryContentItem(name, level, id, parentId);
		const TArray<TSharedPtr<FJsonValue>> JArray = InJsonObject->GetArrayField("menuVOList");

		for (auto& iter : JArray)
		{
			categordata.CategoryList.Emplace(ParseCategoryFromJson(iter->AsObject()));
		}

		return categordata;
	}
	return FCategoryContentItem();
}


TSharedPtr<FContentItem> FArmyCommonTools::ParseContentItemFromJson(TSharedPtr<FJsonObject> InJsonObject)
{
	if (InJsonObject.IsValid())
	{
        TArray< TSharedPtr<FResObj> > ResPtr;

        int32 BusinessType = InJsonObject->GetIntegerField("businessType");
        if (BusinessType == BT_PROJECT) // 项目
        {
            TSharedPtr<FProjectRes> ProjectRes = MakeShareable(new FProjectRes);
            InJsonObject->TryGetNumberField("setMealId", ProjectRes->SetMealId);
            InJsonObject->TryGetStringField("setMealName", ProjectRes->SetMealName);
            InJsonObject->TryGetStringField("code", ProjectRes->Code);
            InJsonObject->TryGetStringField("customerName", ProjectRes->CustomerName);
            InJsonObject->TryGetStringField("customerPhone", ProjectRes->CustomerPhone);
            InJsonObject->TryGetStringField("province", ProjectRes->ProvinceName);
            InJsonObject->TryGetStringField("city", ProjectRes->CityName);
            InJsonObject->TryGetStringField("area", ProjectRes->AreaName);
            InJsonObject->TryGetStringField("communityName", ProjectRes->Villages);
            InJsonObject->TryGetStringField("addressDetails", ProjectRes->DetailAddress);

            ResPtr.Add(ProjectRes);

            int32 Id = -1;
            FString Name, Thumbnail;
            InJsonObject->TryGetNumberField("id", Id);
            InJsonObject->TryGetStringField("name", Name);
            InJsonObject->TryGetStringField("thumbnail", Thumbnail);

            return MakeShareable(new FContentItem(EResourceType::Project, Id, Name, Thumbnail, ResPtr));
        }
        else if (BusinessType == BT_PLAN) // 方案
        {
            int32 Id = -1;
            FString Name, Thumbnail;
            InJsonObject->TryGetNumberField("id", Id);
            InJsonObject->TryGetStringField("name", Name);
            InJsonObject->TryGetStringField("thumbnail", Thumbnail);

            TSharedPtr<FArmyPlanRes> PlanRes = MakeShareable(new FArmyPlanRes);
            InJsonObject->TryGetStringField("name", PlanRes->Name);
            InJsonObject->TryGetNumberField("bathroom", PlanRes->BathroomNum);
            InJsonObject->TryGetNumberField("bedroom", PlanRes->BedroomNum);
            InJsonObject->TryGetNumberField("kitchen", PlanRes->KitchenNum);
            InJsonObject->TryGetNumberField("livingRoom", PlanRes->LivingRoomNum);
            InJsonObject->TryGetNumberField("floorArea", PlanRes->FloorArea);
            
            const TSharedPtr<FJsonObject>* ResourceObj = nullptr;
            if (InJsonObject->TryGetObjectField("resource", ResourceObj))
            {
                int32 ResourceTypeValue = 0;
                if ((*ResourceObj)->TryGetNumberField("type", ResourceTypeValue))
                {
                    const TSharedPtr<FJsonObject>* ResourceDataObj = nullptr;
                    if ((*ResourceObj)->TryGetObjectField("data", ResourceDataObj))
                    {
                        (*ResourceDataObj)->TryGetStringField("md5", PlanRes->FileMD5);
                        (*ResourceDataObj)->TryGetStringField("url", PlanRes->FileURL);
                        PlanRes->FileName = FString::Printf(TEXT("%d_%s.%s"), Id, *PlanRes->FileMD5, *GetFileExtension(PlanRes->FileURL));
                        PlanRes->FilePath = GetPathFromFileName(EResourceType::Plan, PlanRes->FileName);
                    }
                }
            }

            ResPtr.Add(PlanRes);

            return MakeShareable(new FContentItem(EResourceType::Plan, Id, Name, Thumbnail, ResPtr));
        }
        else if (BusinessType == BT_HOME) // 户型
        {
            int32 Id = -1;
            InJsonObject->TryGetNumberField("id", Id);

            TSharedPtr<FArmyHomeRes> HomeRes = MakeShareable(new FArmyHomeRes);
            InJsonObject->TryGetNumberField("maxVrSpaceId", HomeRes->MaxVrSpaceId);
            InJsonObject->TryGetStringField("background", HomeRes->FacsimileUrl);

            const TSharedPtr<FJsonObject>* ResourceObj = nullptr;
            if (InJsonObject->TryGetObjectField("resource", ResourceObj))
            {
                const TSharedPtr<FJsonObject>* ResourceDataObj = nullptr;
                if ((*ResourceObj)->TryGetObjectField("data", ResourceDataObj))
                {
                    (*ResourceDataObj)->TryGetStringField("md5", HomeRes->FileMD5);
                    (*ResourceDataObj)->TryGetStringField("url", HomeRes->FileURL);
                    HomeRes->FileName = FString::Printf(TEXT("%d_%s.%s"), Id, *HomeRes->FileMD5, *GetFileExtension(HomeRes->FileURL));
                    HomeRes->FilePath = GetPathFromFileName(EResourceType::Home, HomeRes->FileName);
                }
            }

            ResPtr.Add(HomeRes);

            return MakeShareable(new FContentItem(EResourceType::Home, Id, "", "", ResPtr));
        }

		const TSharedPtr<FJsonObject> JObject = InJsonObject->GetObjectField("data");
		int32 Version = InJsonObject->GetIntegerField("version");
		bool bIsSale = false;
		if (JObject.IsValid())
		{
			int32 CategoryId = JObject->GetIntegerField("categoryId");
			int32 ID = JObject->GetIntegerField("id");
			FString Name = JObject->GetStringField("name");
			FString ThumbnailURL = JObject->GetStringField("thumbnailUrl");
			int32 ResourceType = JObject->GetIntegerField("resourceType");

			int32 Width = 0;
			int32 Length = 0;
			int32 Height = 0;

			const TSharedPtr<FJsonObject> ResourceObject = JObject->GetObjectField("resource");
			if (ResourceObject.IsValid())
			{
				FString FileURL = ResourceObject->GetStringField("fileUrl");
				FString FileMD5 = ResourceObject->GetStringField("fileMd5");
				FString FileName = FString::Printf(TEXT("%d_%s.%s"), ID, *FileMD5, *GetFileExtension(FileURL));
				FString FilePath = GetPathFromFileName(EResourceType(ResourceType), FileName);
				PlacePosition ePP = { false };
				ePP.bFloor = ResourceObject->GetBoolField("ground");
				ePP.bWall = ResourceObject->GetBoolField("metope");
				ePP.bCeiling = ResourceObject->GetBoolField("topSurface");
				ePP.bMesa = ResourceObject->GetBoolField("mesa");

				Length = ResourceObject->GetIntegerField("customLength");
				Width = ResourceObject->GetIntegerField("customWidth");
				Height = ResourceObject->GetIntegerField("customHeight");

				if (ResourceType == EResourceType::MoveableMesh || ResourceType == EResourceType::Material || ResourceType == EResourceType::BlueprintClass)
				{
					// 解析优化参数
					FString MaterialParameter, LightParameter;
					FString OptimizeParam = ResourceObject->GetStringField("optimizeParam");
					TSharedPtr<FJsonObject> JOptimizeParam;
					if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(OptimizeParam), JOptimizeParam))
					{
						MaterialParameter = JOptimizeParam->GetStringField("materialParameter");
						LightParameter = JOptimizeParam->GetStringField("lightParameter");
					}

					bIsSale = ResourceObject->GetBoolField("isSale");
					//创建ModelRes
					ResPtr.Add(MakeShareable(new FContentItemSpace::FModelRes(FileName, FilePath, FileURL, FileMD5, EResourceType(ResourceType), MaterialParameter, LightParameter, ePP)));
					//ResPtr->placePosition = ePP;
				}
				//else if (ResourceType == EResourceType::GreatPlan)//大师方案
				//{
				//	int32 DesignStyle = ResourceObject->GetIntegerField("DesignStyle");
				//	int32 HouseType = ResourceObject->GetIntegerField("HouseType");
				//	float Area = ResourceObject->GetNumberField("Area");
				//	int32 AreaType = ResourceObject->GetIntegerField("AreaType");
				//	FString PanoUrl = ResourceObject->GetStringField("PanoUrl");
				//	int32 DesignerID = ResourceObject->GetIntegerField("DesignerID");
				//	FString DesignerName = ResourceObject->GetStringField("DesignerName");
				//	FString Description = ResourceObject->GetStringField("Description");

				//	TArray<FString> ImageList;
				//	ResourceObject->TryGetStringArrayField("DesignPictureList", ImageList);

				//	// 创建resource
				//	ResPtr.Add(MakeShareable(new FContentItemSpace::FArmyGreatPlanRes(
				//		FileName,
				//		FilePath,
				//		FileURL,
				//		FileMD5,
				//		EResourceType(ResourceType),
				//		Name,
				//		DesignStyle,
				//		HouseType,
				//		Area,
				//		AreaType,
				//		PanoUrl,
				//		DesignerID,
				//		DesignerName,
				//		Description,
				//		ImageList
				//	)));
				//}
				else
				{
					ResPtr.Add(MakeShareable(new FContentItemSpace::FResObj(FileName, FilePath, FileURL, FileMD5, EResourceType(ResourceType))));
				}
			}
			//component
			TSharedPtr<FContentItemSpace::FComponentRes> ComponentPtr;
			const TSharedPtr<FJsonObject> ComponentObject = JObject->GetObjectField("component");
			if (ComponentObject.IsValid())
			{
				FString FileURL, FileMD5;
				uint32 TypeID = 0, ComponentID = 0;
				if (ComponentObject->TryGetStringField("drawingUrl", FileURL) && ComponentObject->TryGetStringField("drawingMd5", FileMD5) && ComponentObject->TryGetNumberField("typeId", TypeID) && ComponentObject->TryGetNumberField("id", ComponentID))
				{
					FString FileName = FString::Printf(TEXT("%d_%s.%s"), ID, *FileMD5, *GetFileExtension(FileURL));
					FString FilePath = GetPathFromFileName(ComponentDXF, FileName);

					ComponentPtr = MakeShareable(new FContentItemSpace::FComponentRes(FileName, FilePath, FileURL, FileMD5, EResourceType::ComponentDXF, TypeID, ComponentID));
					ComponentPtr->ProductLength = Length;
					ComponentPtr->ProductWidth = Width;
					ComponentPtr->ProductHeight = Height;
					//ComponentPtr->ResourceType = EResourceType::ComponentDXF;
					ResPtr.Add(ComponentPtr);
				}
			}

			//创建ContentItem
			TSharedPtr<FContentItemSpace::FContentItem> ReturnItem = MakeShareable(new FContentItemSpace::FContentItem(
				EResourceType(ResourceType), ID, Name, ThumbnailURL, ResPtr, bIsSale, NULL, PF_NoFlags)
			);
			ReturnItem->CategryID = CategoryId;
			return ReturnItem;
		}
	}
	return NULL;
}

TSharedPtr<FContentItemSpace::FContentItem> FArmyCommonTools::ParseMakterPlanContentItemFromJson(TSharedPtr<FJsonObject> InJsonObject)
{
	if (InJsonObject.IsValid())
	{
		int32 areaRangeId = InJsonObject->GetIntegerField("areaRangeId");// 面积范围ID
		FString areaRangeName = InJsonObject->GetStringField("areaRangeName");// 面积范围名称
		int32 bathroomCount = InJsonObject->GetIntegerField("bathroomCount");// 卫生间数量
		int32 bedroomCount = InJsonObject->GetIntegerField("bedroomCount"); // 卧室数量 
		//"colorIdList": [],  // 颜色ID列表  // 颜色ID列表 
	//	FString colorNameList = InJsonObject->GetStringField("colorNameList"); // 颜色名称列表字符串
		FString description = InJsonObject->GetStringField("description"); // 
		const TArray<TSharedPtr<FJsonValue>> designPictureList = InJsonObject->GetArrayField("designPictureList");
		TArray<FString> EffectPictureArr;
		FString PictureStr;
		for (TSharedPtr<FJsonValue> list: designPictureList)
		{
			//list->AsObject()->TryGetString(PictureStr);
			if (list->TryGetString(PictureStr))
				EffectPictureArr.Add(PictureStr);
		}
		//"designPictureList": [], // 效果图列表
		int32 designStyleId = InJsonObject->GetIntegerField("designStyleId");// 设计风格ID
		FString designStyleName = InJsonObject->GetStringField("designStyleName"); // 
		FString designerAvatar = InJsonObject->GetStringField("designerAvatar"); // 
		int32 designerId = InJsonObject->GetIntegerField("designerId");
		FString designerName = InJsonObject->GetStringField("designerName"); // 
		FString gmtCreate = InJsonObject->GetStringField("gmtCreate"); // 
		FString gmtModified = InJsonObject->GetStringField("gmtModified"); // 
		int32 id = InJsonObject->GetIntegerField("id");// 设计风格ID
		int32 kitchenCount = InJsonObject->GetIntegerField("kitchenCount");// 设计风格ID
		int32 livingRoomCount = InJsonObject->GetIntegerField("livingRoomCount");// 设计风格ID
		FString name = InJsonObject->GetStringField("name"); // 
		FString panoUrl = InJsonObject->GetStringField("panoUrl"); // 
		FString thumbnailUrl = /*TEXT("https://ali-image.dabanjia.com/image/20181213/MATERIAL_1544675187208_2801.JPG")*/ InJsonObject->GetStringField("thumbnailUrl"); // 
		// zhx add  get RoomId List;
		TArray<int32> RoomIdList;
		const TArray<TSharedPtr<FJsonValue>> * RoomJsonList = nullptr;
		if (InJsonObject->TryGetArrayField("spaceIdSet",RoomJsonList))
		{
			for (auto SpaceId : *RoomJsonList)
			{
				RoomIdList.Add(SpaceId->AsNumber());
			}
		}
		
		TArray<TSharedPtr<FContentItemSpace::FResObj>> Res;
		Res.Add(MakeShareable(new FContentItemSpace::FArmyGreatPlanRes("", "", "", "", EResourceType::GreatPlan,
			areaRangeId,
			areaRangeName,
			bathroomCount,
			bedroomCount,
			description,
			EffectPictureArr,
			designStyleId,
			designStyleName,
			designerAvatar,
			designerId,
			designerName,
			gmtCreate,
			gmtModified,
			id,
			kitchenCount,
			livingRoomCount,
			name,
			panoUrl,
			thumbnailUrl,
			RoomIdList
		)));

		TSharedPtr<FContentItemSpace::FContentItem> ContentItem = MakeShareable(new	FContentItemSpace::FContentItem(
			EResourceType::GreatPlan,
			id,
			name,
			thumbnailUrl,
			Res,
			false
		));

		return ContentItem;
	}

	return nullptr;
}

/// http://101.201.45.88:3000/project/39/wiki  对应code 详细说明
TSharedPtr<FContentItemSpace::FContentItem> FArmyCommonTools::ParseHardModeContemItemFromJson(TSharedPtr<FJsonObject> InJsonObject)
{
	FString	caterorycode = InJsonObject->GetStringField("categoryCode");
	int32 categoryId = InJsonObject->GetIntegerField("categoryId");
	FString code = InJsonObject->GetStringField("code");
	int32 goodsType = InJsonObject->GetIntegerField("goodsType");
	bool ground = InJsonObject->GetBoolField("ground");
	int32 id = InJsonObject->GetIntegerField("id");
	int32 height = InJsonObject->GetIntegerField("height");
	int32 length = InJsonObject->GetIntegerField("length");
	int32 width = InJsonObject->GetIntegerField("width");
	bool mesa = InJsonObject->GetBoolField("mesa");
	bool metope = InJsonObject->GetBoolField("metope");
	int32 modelType = InJsonObject->GetIntegerField("modelType");
	FString name = InJsonObject->GetStringField("name");
	int32 purchasePrice = InJsonObject->GetIntegerField("purchasePrice");
	bool scale = InJsonObject->GetBoolField("sale");
	int32 sellingPrice = InJsonObject->GetIntegerField("sellingPrice");
	FString thumbnail = InJsonObject->GetStringField("thumbnail");
	bool topSurface = InJsonObject->GetBoolField("topSurface");
	FString unit = InJsonObject->GetStringField("unit");
	int32 AiCode = InJsonObject->GetIntegerField("aiCode");
	//int32 AiCode2 = InJsonObject->GetIntegerField("aiCode2");
	//AiCode = FArmyDataTools::GetNewAiCode(AiCode, AiCode2);
	const TArray<TSharedPtr<FJsonValue>> JArray = InJsonObject->GetArrayField("resourceList");
	PlacePosition ePP;
	ePP.bCeiling = topSurface;
	ePP.bFloor = ground;
	ePP.bWall = metope;
	ePP.bMesa = mesa;
	TSharedPtr<FContentItemSpace::FProductObj> PdtPtr = MakeShareable(new FContentItemSpace::FArmyHardModeProduct());
	PdtPtr->Length = length;
	PdtPtr->Width = width;
	PdtPtr->Height = height;
	PdtPtr->purchasePrice = purchasePrice;
	PdtPtr->unit = unit;
	PdtPtr->brandName = name;
	TSharedPtr<FContentItemSpace::FContentItem> contentItem = nullptr;
	TArray<TSharedPtr<FArmyKeyValue> > PavingMethodArr;
	const TArray<TSharedPtr<FJsonValue>> JArrayPav = InJsonObject->GetArrayField("paveList");
	for (auto &ArrIt : JArrayPav)
	{
		int32 id = ArrIt->AsObject()->GetIntegerField("id");
		FString name = ArrIt->AsObject()->GetStringField("name");
		PavingMethodArr.Add(MakeShareable(new FArmyKeyValue(id, name)));
	}

	EResourceType CurrentResourceType = EResourceType::None;

	TArray<TSharedPtr<FContentItemSpace::FResObj> > ResPtr;//
	for (auto& iter : JArray)
	{
		TSharedPtr<FJsonObject> resourceObj = iter->AsObject();
		const TSharedPtr<FJsonObject> JDataIt = resourceObj->GetObjectField("data");

		int32 resourcType = resourceObj->GetIntegerField("type");

		FString md5Path = JDataIt->GetStringField("md5");
		FString pakUrlPath = JDataIt->GetStringField("url");
		FString NameAndExtensionMaterial = FString::Printf(TEXT("%d_%s.%s"), id, *md5Path, *FArmyCommonTools::GetFileExtension(pakUrlPath));
		if (resourcType == 2 || resourcType == 1 || resourcType == 3)
		{
			// 解析优化参数
			FString MaterialParameter, LightParameter;
			FString OptimizeParam = JDataIt->GetStringField("optimizeParam");
			TSharedPtr<FJsonObject> JOptimizeParam;
			try
			{
				if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(OptimizeParam), JOptimizeParam))
				{
					MaterialParameter = JOptimizeParam->GetStringField("materialParameter");
					LightParameter = JOptimizeParam->GetStringField("lightParameter");
				}
			}
			catch (const std::exception&)
			{
				UE_LOG(LogTexture, Warning, TEXT("PAK Incomplete optimization parameters. Param: %s"), *OptimizeParam);

			}
			if (resourcType == 2)
			{
				CurrentResourceType == EResourceType::None ? CurrentResourceType = EResourceType::Texture : true;
				FString FilePathMaterial = FArmyCommonTools::GetPathFromFileName(EResourceType::Texture, NameAndExtensionMaterial);
				ResPtr.Add(MakeShareable(new FContentItemSpace::FArmyHardModeRes(NameAndExtensionMaterial, FilePathMaterial, pakUrlPath, md5Path, EResourceType::Texture, MaterialParameter, LightParameter, ePP)));
			}
			else if (resourcType == 1)
			{
				CurrentResourceType = HardPAK;
				FString FilePathMaterial = FArmyCommonTools::GetPathFromFileName(EResourceType::HardPAK, NameAndExtensionMaterial);
				ResPtr.Add(MakeShareable(new FContentItemSpace::FArmyHardModeRes(NameAndExtensionMaterial, FilePathMaterial, pakUrlPath, md5Path, EResourceType::HardPAK, MaterialParameter, LightParameter, ePP)));
			}
			else if (resourcType == 3)
			{
				CurrentResourceType == EResourceType::None ? CurrentResourceType = EResourceType::HardDXF : true;
				FString FilePathMaterial = FArmyCommonTools::GetPathFromFileName(EResourceType::HardDXF, NameAndExtensionMaterial);
				ResPtr.Add(MakeShareable(new FContentItemSpace::FArmyHardModeRes(NameAndExtensionMaterial, FilePathMaterial, pakUrlPath, md5Path, EResourceType::HardDXF, MaterialParameter, LightParameter, ePP)));
			}
		}

	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	const TSharedPtr<FJsonObject> ComponentObject = InJsonObject->GetObjectField("legend");
	if (ComponentObject.IsValid())
	{
		int32 ComponentID = ComponentObject->GetNumberField("id");
		int32 ComTypeID = ComponentObject->GetNumberField("typeId");

		// 所有构件图例
		TMap<EItemDxfType, TSharedPtr<FContentItemSpace::FComponentRes>> MapComponentPtr;

		//TSharedPtr<FContentItemSpace::FComponentRes> ComponentPtr = MakeShareable(new FContentItemSpace::FComponentRes(ComFileName, ComFilePath, ComFileURL, ComFileMD5, EResourceType::ComponentDXF, ComTypeID, ComponentID));
		const TArray<TSharedPtr<FJsonValue>> ComponentResourceList = ComponentObject->GetArrayField("resourceList");
		for (auto Res : ComponentResourceList)
		{
			if (Res.IsValid())
			{
				const TSharedPtr<FJsonObject> ResObj = Res->AsObject();
				if (ResObj.IsValid())
				{
					int32 ResType = -1;
					if (ResObj->TryGetNumberField("type", ResType))
					{
						FString ComFileName, ComFilePath, ComFileURL, ComFileMD5;
						TSharedPtr<FContentItemSpace::FComponentRes> CurrentComponentPtr;
						const TSharedPtr<FJsonObject> ComResData = ResObj->GetObjectField("data");
						if (ComResData.IsValid())
						{
							ComResData->TryGetStringField("md5", ComFileMD5);
							ComResData->TryGetStringField("url", ComFileURL);

							if (!ComFileMD5.IsEmpty() && !ComFileURL.IsEmpty())
							{
								ComFileName = FString::Printf(TEXT("%d_%s.%s"), ComponentID, *ComFileMD5, *FArmyCommonTools::GetFileExtension(ComFileURL));
								ComFilePath = FArmyCommonTools::GetPathFromFileName(ComponentDXF, ComFileName);
								CurrentComponentPtr = MakeShareable(new FContentItemSpace::FComponentRes(ComFileName, ComFilePath, ComFileURL, ComFileMD5, EResourceType::ComponentDXF, ComTypeID, ComponentID));

								CurrentComponentPtr->ProductLength = length;
								CurrentComponentPtr->ProductWidth = width;
								CurrentComponentPtr->ProductHeight = height;

								double DefalutFloorHeight = 0;
								if (ComponentObject->TryGetNumberField("defaultFloorHeight", DefalutFloorHeight))
								{
									CurrentComponentPtr->Altitudes.AddUnique(DefalutFloorHeight);
								}

								const TArray<TSharedPtr<FJsonValue>> floorHeightListes = ComponentObject->GetArrayField("floorHeightList");
								for (int32 i = 0; i < floorHeightListes.Num(); i++)
								{
									int32 floorHeight = floorHeightListes[i]->AsNumber();
									CurrentComponentPtr->Altitudes.AddUnique(floorHeight);
								}

								// 4:俯视图 5：正视图 6：侧视图
								if (ResType == 4)
								{
									MapComponentPtr.Add(E_Dxf_Overlook, CurrentComponentPtr);
								}
								else if (ResType == 5)
								{
									MapComponentPtr.Add(E_Dxf_Front, CurrentComponentPtr);
								}
								else if (ResType == 6)
								{
									MapComponentPtr.Add(E_Dxf_Left, CurrentComponentPtr);
								}

							}
						}
					}
				}
			}
		}


		// 添加所有图例
		//for (auto It : MapComponentPtr)
		//{
		//	if (It.Value.IsValid())
		//	{
		//		ResPtr.Add(It.Value);
		//	}
		//}
		// 添加俯视图图例
		TSharedPtr<FContentItemSpace::FComponentRes> ComponentPtr;
		if (MapComponentPtr.Contains(E_Dxf_Overlook))
		{
			ComponentPtr = MapComponentPtr.FindRef(E_Dxf_Overlook);
		}
		for (auto It : MapComponentPtr)
		{
			if (It.Value.IsValid() && It.Key != E_Dxf_Overlook)
			{
				ComponentPtr->MapResObj.Add(It.Key, It.Value);
			}
		}
		if (ComponentPtr.IsValid())
		{
			ResPtr.Add(ComponentPtr);
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////

	contentItem = MakeShareable(new FContentItemSpace::FContentItem(CurrentResourceType, id, name, thumbnail, ResPtr, scale, PdtPtr, PF_NoFlags));
	contentItem->codeStrId = caterorycode;
	contentItem->CategryID = categoryId;
	if (AiCode == 0)
	{
		contentItem->AiCode = AI_None;
	}
	else
	{
		contentItem->AiCode = (EAIComponentCode)AiCode;
	}

	if (CurrentResourceType == EResourceType::Texture)
	{
		TSharedPtr<FArmyHardModeRes> tempHardModel = StaticCastSharedPtr<FArmyHardModeRes>(ResPtr[0]);
		tempHardModel->placePosition = ePP;
		if (PavingMethodArr.Num() > 0)
		{
			tempHardModel->PavingMethodArr = PavingMethodArr;
			int32 id = PavingMethodArr[0]->Key;
			CategryApplicationType cateryType = CAT_None;
			if (id == 1)
				cateryType = CAT_Continue;
			else if (id == 2)
				cateryType = CAT_Worker;
			else if (id == 3)
				cateryType = CAT_Wind;
			else if (id == 4)
				cateryType = CAT_People;
			else if (id == 5)
				cateryType = CAT_TrapeZoid;
			else if (id == 6)
				cateryType = CAT_AntiCornerBrickMethod;
			else if (id == 7)
				cateryType = CAT_SlopeContinue;
			tempHardModel->ResourceCategryType = cateryType;
		}
		else if(GlobalHardModelCodeMap.Contains(caterorycode))
		{
			tempHardModel->ResourceCategryType = GlobalHardModelCodeMap.FindRef(caterorycode);
		}
		else
		{
			tempHardModel->ResourceCategryType = CAT_Continue;
		}
	}
	
	if (caterorycode.Equals(TEXT("GC0400")) || 
		caterorycode.Equals(TEXT("GC0418")) || 
		caterorycode.Equals(TEXT("GC0416")) || 
		caterorycode.Equals(TEXT("GC0420")) || 
		caterorycode.Equals(TEXT("GC0393")))
	{
		contentItem->ItemFlag |= EContentItemFlag::PF_OrignalCenter;
	}

	return contentItem;
}



TSharedPtr<FContentItemSpace::FContentItem> FArmyCommonTools::ParseHydropowerContentItemFromJson(TSharedPtr<FJsonObject> InJsonObject)
{
	if (InJsonObject.IsValid())
	{
		PlacePosition ePP = { false };

		ePP.bFloor = InJsonObject->GetBoolField("ground");
		ePP.bWall = InJsonObject->GetBoolField("metope");
		ePP.bCeiling = InJsonObject->GetBoolField("topSurface");
		ePP.bMesa = InJsonObject->GetBoolField("mesa");

		float Raduis = 20;
		const TArray<TSharedPtr<FJsonValue>> ObjPropertyArray = InJsonObject->GetArrayField("property");
		for (auto PJson : ObjPropertyArray)
		{
			int32 ShowType = 0, Value = 0;
			TSharedPtr<FJsonObject> PropertyObject = PJson->AsObject();
			const TArray<TSharedPtr<FJsonValue>> Values = PropertyObject->GetArrayField("valueVOList");

			if (PropertyObject->GetStringField("code") == PT_PipeDiameter)
			{
				if (Values.Num())
				{
					TSharedPtr<FJsonObject> Value = Values[0]->AsObject();
					Raduis = Value->GetNumberField("value");
					break;
				}
			}
		}

		int32 ComponentID = -1;
		//TSharedPtr<FContentItemSpace::FComponentRes> ComponentPtr;
		// 所有构件图例
		TMap<EItemDxfType, TSharedPtr<FContentItemSpace::FComponentRes>> MapComponentPtr;

		const TSharedPtr<FJsonObject> ComponentObject = InJsonObject->GetObjectField("legend");
		if (ComponentObject.IsValid())
		{
			ComponentID = GlobalComponentCodeMap.FindRef(ComponentObject->GetStringField("code"));
			int32 ComTypeID = ComponentObject->GetNumberField("typeId");

			const TArray<TSharedPtr<FJsonValue>> ComponentResourceList = ComponentObject->GetArrayField("resourceList");
			for (auto Res : ComponentResourceList)
			{
				if (Res.IsValid())
				{
					const TSharedPtr<FJsonObject> ResObj = Res->AsObject();
					if (ResObj.IsValid())
					{
						int32 ResType = -1; // 4:俯视图 5：正视图 6：侧视图
						if (ResObj->TryGetNumberField("type", ResType))
						{
							TSharedPtr<FContentItemSpace::FComponentRes> CurrentComponentPtr;
							FString ComFileName, ComFilePath, ComFileURL, ComFileMD5;
							const TSharedPtr<FJsonObject> ComResData = ResObj->GetObjectField("data");
							if (ComResData.IsValid())
							{
								ComResData->TryGetStringField("md5", ComFileMD5);
								ComResData->TryGetStringField("url", ComFileURL);

								if (!ComFileMD5.IsEmpty() && !ComFileURL.IsEmpty())
								{
									ComFileName = FString::Printf(TEXT("%d_%s.%s"), ComponentID, *ComFileMD5, *FArmyCommonTools::GetFileExtension(ComFileURL));
									ComFilePath = FArmyCommonTools::GetPathFromFileName(ComponentDXF, ComFileName);
									CurrentComponentPtr = MakeShareable(new FContentItemSpace::FComponentRes(ComFileName, ComFilePath, ComFileURL, ComFileMD5, EResourceType::ComponentDXF, ComTypeID, ComponentID));
									CurrentComponentPtr->SwitchCoupletNum = InJsonObject->GetIntegerField("switchCoupletNum");

									double DefalutFloorHeight = 0;
									if (ComponentObject->TryGetNumberField("defaultFloorHeight", DefalutFloorHeight))
									{
										CurrentComponentPtr->Altitudes.AddUnique(DefalutFloorHeight);
									}

									const TArray<TSharedPtr<FJsonValue>> floorHeightListes = ComponentObject->GetArrayField("floorHeightList");
									for (int32 i = 0; i < floorHeightListes.Num(); i++)
									{
										int32 floorHeight = floorHeightListes[i]->AsNumber();
										CurrentComponentPtr->Altitudes.AddUnique(floorHeight);
									}

									// 4:俯视图 5：正视图 6：侧视图
									if (ResType == 4)
									{
										MapComponentPtr.Add(E_Dxf_Overlook, CurrentComponentPtr);
									}
									else if (ResType == 5)
									{
										MapComponentPtr.Add(E_Dxf_Front, CurrentComponentPtr);
									}
									else if (ResType == 6)
									{
										MapComponentPtr.Add(E_Dxf_Left, CurrentComponentPtr);
									}
								}

							}
						}
					}
				}
			}
		}

		FString FileURL, FileMD5, FileName, FilePath, MaterialParameter, LightParameter;

		FString	CateroryCode = InJsonObject->GetStringField("categoryCode");
		int32 ID = InJsonObject->GetIntegerField("id");
		int32 ResourceType = -1;
		const TArray<TSharedPtr<FJsonValue>> ResourceList = InJsonObject->GetArrayField("resourceList");
		for (auto Res : ResourceList)
		{
			const TSharedPtr<FJsonObject> Obj = Res->AsObject();
			ResourceType = Obj->GetIntegerField("type");

			const TSharedPtr<FJsonObject> DataObj = Obj->GetObjectField("data");
			if (DataObj.IsValid())
			{
				FileURL = DataObj->GetStringField("url");
				FileMD5 = DataObj->GetStringField("md5");

				if (!FileMD5.IsEmpty() && !FileURL.IsEmpty())
				{
					FileName = FString::Printf(TEXT("%d_%s.%s"), ID, *FileMD5, *FArmyCommonTools::GetFileExtension(FileURL));
					FilePath = FArmyCommonTools::GetPathFromFileName((EResourceType)ResourceType, FileName);
					//bResource = true;
				}

				// 解析优化参数
				FString OptimizeParam = DataObj->GetStringField("optimizeParam");
				TSharedPtr<FJsonObject> JOptimizeParam;

				try
				{
					if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(OptimizeParam), JOptimizeParam))
					{
						MaterialParameter = JOptimizeParam->GetStringField("materialParameter");
						LightParameter = JOptimizeParam->GetStringField("lightParameter");
					}
				}
				catch (const std::exception&)
				{
					UE_LOG(LogTexture, Warning, TEXT("PAK Incomplete optimization parameters. Param: %s"), *OptimizeParam);
				}
				break;
			}
		}
		TSharedPtr<FContentItemSpace::FHydropowerProductObj> ProductObjPtr = MakeShareable(new FContentItemSpace::FHydropowerProductObj());

		ProductObjPtr->brandName = InJsonObject->GetStringField("brandName");//品牌
		ProductObjPtr->modelType = InJsonObject->GetStringField("productModel");//型号
																				//ProductObjPtr->introduce = InJsonObject->GetStringField("introduce");//备注信息
		ProductObjPtr->purchasePrice = InJsonObject->GetIntegerField("purchasePrice");//采购价格
		ProductObjPtr->Price = InJsonObject->GetIntegerField("sellingPrice");//采购价格
		ProductObjPtr->Length = InJsonObject->GetIntegerField("length");
		ProductObjPtr->Width = InJsonObject->GetIntegerField("width");
		ProductObjPtr->Height = InJsonObject->GetIntegerField("height");
		ProductObjPtr->Radius = Raduis;
		ProductObjPtr->unit = InJsonObject->GetStringField("unit");

		bool bIsScale = InJsonObject->GetBoolField("sale");
		FString Name = InJsonObject->GetStringField("name");
		FString ThumbnailURL = InJsonObject->GetStringField("thumbnail");
		int32 CategoryID = InJsonObject->GetIntegerField("categoryId");
		FString Code = InJsonObject->GetStringField("code");
		int32 AiCode = InJsonObject->GetIntegerField("aiCode");
		//int32 AiCode2 = InJsonObject->GetIntegerField("aiCode2");
		//AiCode = FArmyDataTools::GetNewAiCode(AiCode, AiCode2);
		
		
		TSharedPtr<FContentItemSpace::FArmyPipeRes> ResPtr = MakeShareable(new FContentItemSpace::FArmyPipeRes(FileName, FilePath, FileURL, FileMD5, (EResourceType)ResourceType, MaterialParameter, LightParameter, "", Name));
		ResPtr->MaterialParameter = MaterialParameter;
		ResPtr->LightParameter = LightParameter;
		ResPtr->ComponentID = ComponentID;
		ResPtr->Raduis = Raduis;

		if (ResPtr.IsValid())
		{
			uint32 Type, Category0;
			FString ClassName;
			if (!ParseCategoryToObjectType(CateroryCode, Type))
				ParseComponentToObjectType(ResPtr->ComponentID, Type);

			ParseCategoryToClassName(CateroryCode, ClassName);
			ParseCategoryToCategory0(CateroryCode, ComponentID, Category0);
			FColor NormalColor, RefromColor, PointColor, PointReformColor;
			ParseCategoryToColor(CateroryCode, NormalColor);
			ParseCategoryToReformColor(CateroryCode, RefromColor);
			ParseCategoryToPointColor(CateroryCode, PointColor);
			ParseCategoryToPointReformColor(CateroryCode, PointReformColor);

			if (ResPtr->ComponentID == EC_Drain_Point)
			{
				ResPtr->Raduis = 110.f;
			}
			ResPtr->ID = ID;
			ResPtr->placePosition = ePP;
			ResPtr->Color = NormalColor;
			ResPtr->RefromColor = RefromColor;
			ResPtr->PointColor = PointColor;
			ResPtr->PointReformColor = PointReformColor;
			ResPtr->ObjectType = Type;
			ResPtr->Categry0Type = Category0;
			ResPtr->ClassName = ClassName;


			TArray<TSharedPtr<FContentItemSpace::FResObj> > res;
			res.Add(ResPtr);
			
			// 添加所有图例
			/*for (auto It : MapComponentPtr)
			{
				if (It.Value.IsValid())
				{
					res.Add(It.Value);
				}
			}*/
			// 添加俯视图图例
			TSharedPtr<FContentItemSpace::FComponentRes> ComponentPtr;
			if (MapComponentPtr.Contains(E_Dxf_Overlook))
			{
				ComponentPtr = MapComponentPtr.FindRef(E_Dxf_Overlook);
			}

			for (auto It : MapComponentPtr)
			{
				if (It.Value.IsValid() && It.Key != E_Dxf_Overlook)
				{
					ComponentPtr->MapResObj.Add(It.Key, It.Value);
				}
			}

			ComponentPtr.IsValid() ? res.Add(ComponentPtr) : 0;

			TSharedPtr<FContentItemSpace::FContentItem> contentItem = MakeShareable(new FContentItemSpace::FContentItem(
				EResourceType::Hydropower, ID, Name, ThumbnailURL, res, bIsScale, ProductObjPtr, EContentItemFlag::PF_NoFlags)
			);
			contentItem->CategryID = CategoryID;
			contentItem->codeStrId = CateroryCode;
			contentItem->ModeIdent = EModeIdent::MI_HydropowerMode;
			if (AiCode == 0)
			{
				contentItem->AiCode = AI_None;
			}
			else
			{
				contentItem->AiCode = (EAIComponentCode)AiCode;
			}
			
			return contentItem;
		}
	}
	return nullptr;
}

//TSharedPtr<FContentItemSpace::FModelRes> FArmyCommonTools::ParsePakFromJson(TSharedPtr<FJsonObject> InObj, int32 InID, int32 InResourceType, int32 InModelType)
//{
//	FString FileURL, FileMD5, FileName, FilePath, MaterialParameter, LightParameter;
//	int32 ResourceType = InObj->GetIntegerField("type");
//
//	if (ResourceType == InResourceType)
//	{
//		const TSharedPtr<FJsonObject> DataObj = InObj->GetObjectField("data");
//		if (DataObj.IsValid())
//		{
//			FileURL = DataObj->GetStringField("url");
//			FileMD5 = DataObj->GetStringField("md5");
//
//			if (!FileMD5.IsEmpty() && !FileURL.IsEmpty())
//			{
//				FileName = FString::Printf(TEXT("%d_%s.%s"), InID, *FileMD5, *FArmyCommonTools::GetFileExtension(FileURL));
//				FilePath = FArmyCommonTools::GetPathFromFileName((EResourceType)ResourceType, FileName);
//				//bResource = true;
//			}
//
//			// 解析优化参数
//			FString OptimizeParam = DataObj->GetStringField("optimizeParam");
//			TSharedPtr<FJsonObject> JOptimizeParam;
//
//			try
//			{
//				if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(OptimizeParam), JOptimizeParam))
//				{
//					MaterialParameter = JOptimizeParam->GetStringField("materialParameter");
//					LightParameter = JOptimizeParam->GetStringField("lightParameter");
//				}
//			}
//			catch (const std::exception&)
//			{
//				UE_LOG(LogTexture, Warning, TEXT("PAK Incomplete optimization parameters. Param: %s"), *OptimizeParam);
//			}
//			//break;
//		}
//		if (InModelType == 2)
//		{
//			return MakeShareable(new FContentItemSpace::FArmyPipeRes(FileName, FilePath, FileURL, FileMD5, (EResourceType)ResourceType, MaterialParameter, LightParameter, "", ""));
//		}
//		else if (InModelType == 1)
//		{
//
//		}
//	}
//
//	return nullptr;
//}

bool FArmyCommonTools::ParseComponentToObjectType(uint32 _identity, uint32 &OutObjectType)
{
	switch (_identity)
	{

	case EC_EleBoxL_Point:
	{
		OutObjectType = OT_EleBoxL_Point;
		return true;
	}
	case EC_EleBoxH_Point:
	{
		OutObjectType = OT_EleBoxH_Point;
		return true;
	}
	//case	EC_Socket_Point://插座
	//{
	//	OutObjectType = OT_Socket_Point;
	//	return true;
	//}
	case EC_Socket_Five_Point://五孔插座
	{
		OutObjectType = OT_Socket_Five_Point;
		return true;
	}
	case EC_Socket_Five_On_Point://五孔开关插座
	{
		OutObjectType = OT_Socket_Five_On_Point;
		return true;
	}
	case EC_Socket_Five_On_Dislocation://五孔错位插座
	{
		OutObjectType = OT_Socket_Five_On_Dislocation;
		return true;
	}
	case EC_Socket_Five_Point_Dislocation://五孔错位开关插座
	{
		OutObjectType = OT_Socket_Five_Point_Dislocation;
		return true;
	}
	case EC_Socket_Four_Point: //四孔插座
	{
		OutObjectType = OT_Socket_Four_Point;
		return true;
	}
	case EC_Socket_Four_On_Point://四孔开关插座
	{
		OutObjectType = OT_Socket_Four_On_Point;
		return true;
	}
	case EC_Socket_Three_On_Point: //三孔开关插座
	{
		OutObjectType = OT_Socket_Three_On_Point;
		return true;
	}
	case EC_Socket_Three: //三孔插座
	{
		OutObjectType = OT_Socket_Three;
		return true;
	}
	case EC_Socket_Three_On_Point_3A: //16A三孔开关插座
	{
		OutObjectType = OT_Socket_Three_On_Point_3A;
		return true;
	}
	case EC_Socket_Three_3A: //16A三孔插座
	{
		OutObjectType = OT_Socket_Three_3A;
		return true;
	}
	case EC_Socket_White://白板
	{
		OutObjectType = OT_Socket_White;
		return true;
	}
	case EC_Socket_Spatter://防溅盒
	{
		OutObjectType = OT_Socket_Spatter;
		return true;
	}
	case EC_Socket_IT://网线插座
	{
		OutObjectType = OT_Socket_IT;
		return true;
	}
	case EC_Socket_IT_TV: //电视电脑插座
	{
		OutObjectType = OT_Socket_IT_TV;
		return true;
	}
	case EC_Socket_TV: //电视插座
	{
		OutObjectType = OT_Socket_TV;
		return true;
	}
	case EC_Socket_Phone: //电话插座
	{
		OutObjectType = OT_Socket_Phone;
		return true;
	}
	case EC_Socket_IT_Phone: //电脑电话插座
	{
		OutObjectType = OT_Socket_IT_Phone;
		return true;
	}
	case EC_Socket_TP: //厨房橱柜专用联排插座
	{
		OutObjectType = OT_Socket_TP;
		return true;
	}
	case EC_Socket_TV_Couplet: //电视墙专用联排插座
	{
		OutObjectType = OT_Socket_TV_Couplet;
		return true;
	}
	case EC_Switch_BathHeater: //浴霸开关
	{
		OutObjectType = OT_Switch_BathHeater;
		return true;
	}
	case EC_Switch_Double: //双键无线开关
	{
		OutObjectType = OT_Switch_Double;
		return true;
	}
	case EC_Switch_Single: //单键无线开关
	{
		OutObjectType = OT_Switch_Single;
		return true;
	}
	case EC_Switch4O2_Point: //四联双控开关
	{
		OutObjectType = OT_Switch4O2_Point;
		return true;
	}
	case EC_Switch4O1_Point: //四联单控开关
	{
		OutObjectType = OT_Switch4O1_Point;
		return true;
	}
	case EC_Switch3O2_Point: //三联双控开关
	{
		OutObjectType = OT_Switch3O2_Point;
		return true;
	}
	case EC_Switch3O1_Point: //三联单控开关
	{
		OutObjectType = OT_Switch3O1_Point;
		return true;
	}
	case EC_Switch2O2_Point: //双联双控开关
	{
		OutObjectType = OT_Switch2O2_Point;
		return true;
	}
	case EC_Switch2O1_Point://双联单控开关
	{
		OutObjectType = OT_Switch2O1_Point;
		return true;
	}
	case EC_Switch1O2_Point://单连双控开关
	{
		OutObjectType = OT_Switch1O2_Point;
		return true;
	}
	case EC_Switch1O1_Point: //单连单控开关
	{
		OutObjectType = OT_Switch1O1_Point;
		return true;
	}
	case EC_Water_Hot_Point: //热水点位
	{
		OutObjectType = OT_Water_Hot_Point;
		return true;
	}
	case EC_Water_Chilled_Point: //冷水点位
	{
		OutObjectType = OT_Water_Chilled_Point;
		return true;
	}
	case EC_Water_Hot_Chilled_Point: //冷熱水
	{
		OutObjectType = OT_Water_Hot_Chilled_Point;
		return true;
	}
	case EC_Dewatering_Point: //排水点位
	{
		OutObjectType = OT_Dewatering_Point;
		return true;
	}
	case EC_Closestool: //马桶下水
	{
		OutObjectType = OT_Closestool;
		return true;
	}
	case EC_Basin: //地漏下水
	{
		OutObjectType = OT_Basin;
		return true;
	}
	case EC_Drain_Point: //下水主管道
	{
		OutObjectType = OT_Drain_Point;
		return true;
	}
	case EC_Water_Supply: // 给水点位,
	{
		OutObjectType = OT_Water_Supply;
		return true;
	}
	case  EC_Water_Basin:
	{
		OutObjectType = OT_Water_Basin;
		return true;
	}
	default:
		return false;
	}
}

TSharedPtr<FArmyKeyValue> FArmyCommonTools::ParseSearchFilterFromJson(TSharedPtr<FJsonObject> InJsonObject)
{
	if (InJsonObject.IsValid())
	{
		int32 id = InJsonObject->GetIntegerField("id");
		FString title = InJsonObject->GetStringField("title");
		TSharedPtr<FArmyKeyValue> filterResult = MakeShareable(new FArmyKeyValue(id, title));
		return filterResult;
	}
	return nullptr;
}

bool FArmyCommonTools::ParseCategoryToObjectType(FString& InCode, uint32 &OutObjectType)
{
	if (InCode.Contains(CTStrong_25h))
	{
		OutObjectType = OT_StrongElectricity_25;
		return true;
	}
	else if (InCode.Contains(CTStrong_4))
	{
		OutObjectType = OT_StrongElectricity_4;
		return true;
	}
	else if (InCode.Contains(CTStrong_6))
	{
		OutObjectType = OT_StrongElectricity_6;
		return true;
	}
	else if (InCode.Contains(CTStrong_Single))
	{
		OutObjectType = OT_StrongElectricity_Single;
		return true;
	}
	else if (InCode.Contains(CTStrong_Double))
	{
		OutObjectType = OT_StrongElectricity_Double;
		return true;
	}
	else if (InCode.Contains(CTWeakElectricity_TV))
	{
		OutObjectType = OT_WeakElectricity_TV;
		return true;
	}
	else if (InCode.Contains(CTWeakElectricity_Net))
	{
		OutObjectType = OT_WeakElectricity_Net;
		return true;
	}
	else if (InCode.Contains(CTWeakElectricity_Phone))
	{
		OutObjectType = OT_WeakElectricity_Phone;
		return true;
	}
	else if (InCode.Contains(CTColdWaterTube))
	{
		OutObjectType = OT_ColdWaterTube;
		return true;
	}
	else if (InCode.Contains(CTHotWaterTube))
	{
		OutObjectType = OT_HotWaterTube;
		return true;
	}
	else if (InCode.Contains(CTDrain))
	{
		OutObjectType = OT_Drain;
		return true;
	}
	else if (InCode.Contains(CTLight_Wire))
	{
		OutObjectType = OT_LightElectricity;
		return true;
	}
	return false;
}
bool FArmyCommonTools::ParseCategoryToClassName(FString& InCode, FString& OutClassName)
{
	if (InCode.Contains(CTStrong_25h))
	{
		OutClassName = TEXT("2.5平方强电插座");
		return true;
	}
	else if (InCode.Contains(CTStrong_4))
	{
		OutClassName = TEXT("4平方强电插座");
		return true;
	}
	else if (InCode.Contains(CTStrong_6))
	{
		OutClassName = TEXT("6平方强电插座");
		return true;
	}
	else if (InCode.Contains(CTStrong_Single))
	{
		OutClassName = TEXT("单控强电插座");
		return true;
	}
	else if (InCode.Contains(CTStrong_Double))
	{
		OutClassName = TEXT("双控强电线路");
		return true;
	}
	else if (InCode.Contains(CTWeakElectricity_TV))
	{
		OutClassName = TEXT("电视线");
		return true;
	}
	else if (InCode.Contains(CTWeakElectricity_Net))
	{
		OutClassName = TEXT("网线");
		return true;
	}
	else if (InCode.Contains(CTWeakElectricity_Phone))
	{
		OutClassName = TEXT("电话线");
		return true;
	}
	else if (InCode.Contains(CTColdWaterTube))
	{
		OutClassName = TEXT("冷水管");
		return true;
	}
	else if (InCode.Contains(CTHotWaterTube))
	{
		OutClassName = TEXT("热水管");
		return true;
	}
	else if (InCode.Contains(CTDrain))
	{
		OutClassName = TEXT("排水管");
		return true;
	}
	else if (InCode.Contains(CTPreload))
	{
		OutClassName = TEXT("预装点位");
		return true;
	}
	else if (InCode.Contains(CTEleBox))
	{
		OutClassName = TEXT("配电箱");
		return true;
	}
	else if (InCode.Contains(CTSWITCH))
	{
		OutClassName = TEXT("开关");
		return true;
	}
	else if (InCode.Contains(CTSocketLian))
	{
		OutClassName = TEXT("联排插座");
		return true;
	}
	else if (InCode.Contains(CTSocketSupply))
	{
		OutClassName = TEXT("电源插座");
		return true;
	}
	else if (InCode.Contains(CTSocketWeak))
	{
		OutClassName = TEXT("弱电插座");
		return true;
	}
	else if (InCode.Contains(CTWater_Point))
	{
		OutClassName = TEXT("水路");
		return true;
	}


	else if (InCode.Contains(CTSocketNet))
	{
		OutClassName = TEXT("电脑插座");
		return true;
	}
	else if (InCode.Contains(CTSocketMusic))
	{
		OutClassName = TEXT("音频插座");
		return true;
	}
	else if (InCode.Contains(CTSocketTV))
	{
		OutClassName = TEXT("电视插座");
		return true;
	}
	else if (InCode.Contains(CTSocketPhone))
	{
		OutClassName = TEXT("电话插座");
		return true;
	}
	else if (InCode.Contains(CTSocketNetPhone))
	{
		OutClassName = TEXT("电脑+电话插座");
		return true;
	}
	else if (InCode.Contains(CTSocketNetTV))
	{
		OutClassName = TEXT("电脑+电视插座");
		return true;
	}
	else if (InCode.Contains(CTSocketPhoneTV))
	{
		OutClassName = TEXT("电话+电视插座");
		return true;
	}
	return false;

}

bool FArmyCommonTools::ParseCategoryToCategory0(FString& InCode, int32 _componentID, uint32 &OutCatory0)
{
	if (InCode.Contains(CTStrong_25h) ||
		InCode.Contains(CTLight_Wire)||
		InCode.Contains(CTStrong_4) ||
		InCode.Contains(CTStrong_6) ||
		InCode.Contains(CTStrong_Single) ||
		InCode.Contains(CTStrong_Double) ||
		InCode.Contains(CTWeakElectricity_TV) ||
		InCode.Contains(CTWeakElectricity_Phone) ||
		InCode.Contains(CTWeakElectricity_Net) ||
		InCode.Contains(CTColdWaterTube) ||
		InCode.Contains(CTHotWaterTube) ||
		InCode.Contains(CTDrain))
	{
		OutCatory0 = 5;
		return true;
	}
	else if (InCode.Contains(CTEleBox) ||
		InCode.Contains(CTSWITCH) ||
		InCode.Contains(CTSocketStrong) ||
		InCode.Contains(CTSocketWeak) ||
		InCode.Contains(CTWater_Point) ||
		InCode.Contains(CTSocketNet) ||
		InCode.Contains(CTSocketMusic) ||
		InCode.Contains(CTSocketTV) ||
		InCode.Contains(CTSocketPhone) ||
		InCode.Contains(CTSocketNetPhone) ||
		InCode.Contains(CTSocketNetTV) ||
		InCode.Contains(CTSocketPhoneTV) ||
		InCode.Contains(CTSocketLian) ||
		InCode.Contains(CTSocketSupply)
		)
	{
		OutCatory0 = 4;
		return true;
	}
	else if (InCode.Contains(CTPreload))
	{
		if (_componentID == EC_Drain_Point)
			OutCatory0 = 5;
		else
			OutCatory0 = 4;
		return true;
	}
	else
	{
		OutCatory0 = 4;
		return true;
	}
	return false;
}


bool FArmyCommonTools::ParseCategoryToColor(FString& InCode, FColor &OutColor)
{
	FColor Color = FColor::White;

	if (InCode.Contains(CTStrong_25h) ||
		InCode.Contains(CTStrong_4)  ||
		InCode.Contains(CTStrong_6))
	{
		OutColor = FColor(0XFE60000);
	}
	else if (InCode.Contains(CTStrong_Single) ||
		InCode.Contains(CTStrong_Double))
	{
		OutColor = FColor(0XF00A8F1);
	}
	else if (InCode.Contains(CTWeakElectricity_TV) ||
		InCode.Contains(CTWeakElectricity_Phone) ||
		InCode.Contains(CTWeakElectricity_Net))
	{
		OutColor = FColor(0XFE6E6E6);
	}
	else if (InCode.Contains(CTColdWaterTube))
	{
		OutColor = FColor(0XF00F547);
	}
	else if (InCode.Contains(CTHotWaterTube))
	{
		OutColor = FColor(0XFFF5E00);
	}
	else if (InCode.Contains(CTDrain))
	{
		OutColor = FColor(0XFE6E6E6);
	}
	else
	{
		OutColor = FColor(0XFE6E6E6);
	}

	return true;
}

bool FArmyCommonTools::ParseCategoryToReformColor(FString& InCode, FColor &OutColor)
{
	FColor Color = FColor::White;
	if (InCode.Equals(CTStrong_25h) ||
		InCode.Equals(CTStrong_4) ||
		InCode.Equals(CTStrong_6))
	{
		OutColor = FColor(0XFB30000);
	}
	else if (InCode.Contains(CTStrong_Single) ||
		InCode.Contains(CTStrong_Double))
	{
		OutColor = FColor(0XF0086BF);
	}
	else if (InCode.Equals(CTWeakElectricity_TV) ||
		InCode.Equals(CTWeakElectricity_Phone) ||
		InCode.Equals(CTWeakElectricity_Net))
	{
		OutColor = FColor(0XFA8A8A8);
	}
	else if (InCode.Equals(CTColdWaterTube))
	{
		OutColor = FColor(0XF008F28);
	}
	else if (InCode.Equals(CTHotWaterTube))
	{
		OutColor = FColor(0XF8A1E00);
	}
	else if (InCode.Equals(CTDrain))
	{
		OutColor = FColor(0XF8F8F8F);
		return true;
	}
	else
	{
		OutColor = FColor(0XF0086BF);
	}
	return true;
}

bool FArmyCommonTools::ParseCategoryToPointColor(FString& InCode, FColor &OutColor)
{
	FColor Color = FColor::White;
	if (InCode.Equals(CTStrong_25h) ||
		InCode.Equals(CTStrong_4) ||
		InCode.Equals(CTStrong_6))
	{
		FColor color(247, 179, 170);
		OutColor = color;
	}
	else if (InCode.Contains(CTStrong_Single) ||
		InCode.Contains(CTStrong_Double))
	{
		FColor color(145, 197, 245);
		OutColor = color;
	}
	else if (InCode.Equals(CTWeakElectricity_TV) ||
		InCode.Equals(CTWeakElectricity_Phone) ||
		InCode.Equals(CTWeakElectricity_Net))
	{
		FColor color(145, 197, 245);
		OutColor = color;
	}
	else if (InCode.Equals(CTColdWaterTube))
	{
		OutColor = FColor(0XF00C237);
	}
	else if (InCode.Equals(CTHotWaterTube))
	{
		OutColor = FColor(0XFBD2900);
	}
	else if (InCode.Equals(CTDrain))
	{
		OutColor = FColor(0XFC2C2C2);
	}
	else
	{
		OutColor = Color;
	}
	return true;

}

bool FArmyCommonTools::ParseCategoryToPointReformColor(FString& InCode, FColor &OutColor)
{
	FColor Color = FColor::White;
	if (InCode.Equals(CTStrong_25h) ||
		InCode.Equals(CTStrong_4) ||
		InCode.Equals(CTStrong_6))
	{
		FColor color(245, 120, 103);
		OutColor = color;
	}
	else if (InCode.Contains(CTStrong_Single) ||
		InCode.Contains(CTStrong_Double))
	{
		FColor color(96, 134, 168);
		OutColor = color;
	}
	else if (InCode.Equals(CTWeakElectricity_TV) ||
		InCode.Equals(CTWeakElectricity_Phone) ||
		InCode.Equals(CTWeakElectricity_Net))
	{
		FColor color(96, 134, 168);
		OutColor = color;
	}
	else if (InCode.Equals(CTColdWaterTube))
	{
		OutColor = FColor(0XF007521);
	}
	else if (InCode.Equals(CTHotWaterTube))
	{
		OutColor = FColor(0XF701800);
	}
	else if (InCode.Equals(CTDrain))
	{
		OutColor = FColor(0XF757575);
		return true;
	}
	else
	{
		OutColor = Color;
	}
	return true;
}

TSharedPtr<FContentItemSpace::FContentItem> FArmyCommonTools::ParsePakFromDxfJson(TSharedPtr<FJsonObject> InJsonObject)
{
	if (InJsonObject.IsValid())
	{
		PlacePosition ePP = { false };

		ePP.bFloor = InJsonObject->GetBoolField("ground");
		ePP.bWall = InJsonObject->GetBoolField("metope");
		ePP.bCeiling = InJsonObject->GetBoolField("topSurface");
		ePP.bMesa = InJsonObject->GetBoolField("mesa");

		float Raduis = 20;
		//const TArray<TSharedPtr<FJsonValue>> ObjPropertyArray = InJsonObject->GetArrayField("property");
		//for (auto PJson : ObjPropertyArray)
		//{
		//	int32 ShowType = 0, Value = 0;
		//	TSharedPtr<FJsonObject> PropertyObject = PJson->AsObject();
		//	const TArray<TSharedPtr<FJsonValue>> Values = PropertyObject->GetArrayField("valueVOList");

		//	if (PropertyObject->GetStringField("code") == PT_PipeDiameter)
		//	{
		//		if (Values.Num())
		//		{
		//			TSharedPtr<FJsonObject> Value = Values[0]->AsObject();
		//			Raduis = Value->GetNumberField("value");
		//			break;
		//		}
		//	}
		//}

		int32 ComponentID = -1;
		TSharedPtr<FContentItemSpace::FComponentRes> ComponentPtr;
		/*const TSharedPtr<FJsonObject> ComponentObject = InJsonObject->GetObjectField("member");
		if (ComponentObject.IsValid())
		{*/
			ComponentID = GlobalComponentCodeMap.FindRef(InJsonObject->GetStringField("code"));
			int32 ComTypeID = InJsonObject->GetNumberField("typeId");

			FString ComFileName, ComFilePath, ComFileURL, ComFileMD5;

			FString FileURL, FileMD5, FileName, FilePath, MaterialParameter, LightParameter;

			FString	CateroryCode = InJsonObject->GetStringField("categoryCode");
			int32 ID = InJsonObject->GetIntegerField("id");
			int32 ResourceType = -1;
			const TArray<TSharedPtr<FJsonValue>> ComponentResourceList = InJsonObject->GetArrayField("resourceList");
			for (auto Res : ComponentResourceList)
			{
				if (Res.IsValid())
				{
					const TSharedPtr<FJsonObject> ResObj = Res->AsObject();
					if (ResObj.IsValid())
					{
						int32 ResType = -1;
						if (ResObj->TryGetNumberField("type", ResType))
						{
							if (ResType == 4)
							{
								const TSharedPtr<FJsonObject> ComResData = ResObj->GetObjectField("data");
								if (ComResData.IsValid())
								{
									ComResData->TryGetStringField("md5", ComFileMD5);
									ComResData->TryGetStringField("url", ComFileURL);

									if (!ComFileMD5.IsEmpty() && !ComFileURL.IsEmpty())
									{
										ComFileName = FString::Printf(TEXT("%d_%s.%s"), ComponentID, *ComFileMD5, *FArmyCommonTools::GetFileExtension(ComFileURL));
										ComFilePath = FArmyCommonTools::GetPathFromFileName(ComponentDXF, ComFileName);
										ComponentPtr = MakeShareable(new FContentItemSpace::FComponentRes(ComFileName, ComFilePath, ComFileURL, ComFileMD5, EResourceType::ComponentDXF, ComTypeID, ComponentID));
										ComponentPtr->SwitchCoupletNum = InJsonObject->GetIntegerField("switchCoupletNum");

										double DefalutFloorHeight = 0;
										if (InJsonObject->TryGetNumberField("defaultFloorHeight", DefalutFloorHeight))
										{
											ComponentPtr->Altitudes.AddUnique(DefalutFloorHeight);
										}

										const TArray<TSharedPtr<FJsonValue>> floorHeightListes = InJsonObject->GetArrayField("floorHeightList");
										for (int32 i = 0; i < floorHeightListes.Num(); i++)
										{
											int32 floorHeight = floorHeightListes[i]->AsNumber();
											ComponentPtr->Altitudes.AddUnique(floorHeight);
										}
									}
									else
									{
										//没有构件的数据，视为无效数据
										return nullptr;
									}
									//break;
								}
							}
							if (ResType == 7)
							{
								ResourceType = 1;
								const TSharedPtr<FJsonObject> DataObj = ResObj->GetObjectField("data");
								if (DataObj.IsValid())
								{
									FileURL = DataObj->GetStringField("url");
									FileMD5 = DataObj->GetStringField("md5");

									if (!FileMD5.IsEmpty() && !FileURL.IsEmpty())
									{
										FileName = FString::Printf(TEXT("%d_%s.%s"), ID, *FileMD5, *FArmyCommonTools::GetFileExtension(FileURL));
										FilePath = FArmyCommonTools::GetPathFromFileName((EResourceType)ResourceType, FileName);
										//bResource = true;
									}
									else
									{
										//没有模型数据，视为无效数据
										return nullptr;
									}

									// 解析优化参数
									FString OptimizeParam = DataObj->GetStringField("optimizeParam");
									TSharedPtr<FJsonObject> JOptimizeParam;

									try
									{
										if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(OptimizeParam), JOptimizeParam))
										{
											MaterialParameter = JOptimizeParam->GetStringField("materialParameter");
											LightParameter = JOptimizeParam->GetStringField("lightParameter");
										}
									}
									catch (const std::exception&)
									{
										UE_LOG(LogTexture, Warning, TEXT("PAK Incomplete optimization parameters. Param: %s"), *OptimizeParam);
									}
									//break;
								}
							}
						}
					}
				}
			}
		TSharedPtr<FContentItemSpace::FHydropowerProductObj> ProductObjPtr = MakeShareable(new FContentItemSpace::FHydropowerProductObj());

		ProductObjPtr->brandName = InJsonObject->GetStringField("brandName");//品牌
		ProductObjPtr->modelType = InJsonObject->GetStringField("productModel");//型号
																				//ProductObjPtr->introduce = InJsonObject->GetStringField("introduce");//备注信息
		ProductObjPtr->purchasePrice = InJsonObject->GetIntegerField("purchasePrice");//采购价格
		ProductObjPtr->Price = InJsonObject->GetIntegerField("sellingPrice");//采购价格
		ProductObjPtr->Length = InJsonObject->GetIntegerField("length");
		ProductObjPtr->Width = InJsonObject->GetIntegerField("width");
		ProductObjPtr->Height = InJsonObject->GetIntegerField("height");
		ProductObjPtr->Radius = Raduis;
		ProductObjPtr->unit = InJsonObject->GetStringField("unit");

		bool bIsScale = InJsonObject->GetBoolField("sale");
		FString Name = InJsonObject->GetStringField("name");
		FString ThumbnailURL = InJsonObject->GetStringField("thumbnail");
		int32 CategoryID = InJsonObject->GetIntegerField("categoryId");
		FString Code = InJsonObject->GetStringField("code");
		int32 AiCode = InJsonObject->GetIntegerField("aiCode");
		//int32 AiCode2 = InJsonObject->GetIntegerField("aiCode2");
		//AiCode = FArmyDataTools::GetNewAiCode(AiCode, AiCode2);



		TSharedPtr<FContentItemSpace::FArmyPipeRes> ResPtr = MakeShareable(new FContentItemSpace::FArmyPipeRes(FileName, FilePath, FileURL, FileMD5, (EResourceType)ResourceType, MaterialParameter, LightParameter, "", Name));
		ResPtr->MaterialParameter = MaterialParameter;
		ResPtr->LightParameter = LightParameter;
		ResPtr->ComponentID = ComponentID;
		ResPtr->Raduis = Raduis;

		if (ResPtr.IsValid())
		{
			uint32 Type, Category0;
			FString ClassName;
			if (!ParseCategoryToObjectType(CateroryCode, Type))
				ParseComponentToObjectType(ResPtr->ComponentID, Type);

			ParseCategoryToClassName(CateroryCode, ClassName);
			ParseCategoryToCategory0(CateroryCode, ComponentID, Category0);
			FColor NormalColor, RefromColor, PointColor, PointReformColor;
			ParseCategoryToColor(CateroryCode, NormalColor);
			ParseCategoryToReformColor(CateroryCode, RefromColor);
			ParseCategoryToPointColor(CateroryCode, PointColor);
			ParseCategoryToPointReformColor(CateroryCode, PointReformColor);

			if (ResPtr->ComponentID == EC_Drain_Point)
			{
				ResPtr->Raduis = 110.f;
			}
			ResPtr->ID = ID;
			ResPtr->placePosition = ePP;
			ResPtr->Color = NormalColor;
			ResPtr->RefromColor = RefromColor;
			ResPtr->PointColor = PointColor;
			ResPtr->PointReformColor = PointReformColor;
			ResPtr->ObjectType = Type;
			ResPtr->Categry0Type = Category0;
			ResPtr->ClassName = ClassName;


			TArray<TSharedPtr<FContentItemSpace::FResObj> > res;
			res.Add(ResPtr);
			ComponentPtr.IsValid() ? res.Add(ComponentPtr) : 0;
			TSharedPtr<FContentItemSpace::FContentItem> contentItem = MakeShareable(new FContentItemSpace::FContentItem(
				EResourceType::Hydropower, ID, Name, ThumbnailURL, res, bIsScale, ProductObjPtr, EContentItemFlag::PF_NoFlags)
			);
			contentItem->CategryID = CategoryID;
			if (AiCode == 0)
			{
				contentItem->AiCode = AI_None;
			}
			else
			{
				contentItem->AiCode = (EAIComponentCode)AiCode;
			}

			return contentItem;
		}
	}
	return nullptr;
}
