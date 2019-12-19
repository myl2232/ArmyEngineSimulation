#include "ArmyEngineTools.h"

#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"

FString FArmyEngineTools::CalcMD5FromFile(FString _FilePath)
{
	TArray<uint8> Result;
	FArchive* Ar = IFileManager::Get().CreateFileReader(*_FilePath);
	if (Ar == NULL)
		return TEXT("");
	Result.Reset();
	Result.AddUninitialized(Ar->TotalSize());
	Ar->Serialize(Result.GetData(), Result.Num());
	delete Ar;

	FMD5 Md5Gen;
	Md5Gen.Update(Result.GetData(), Result.Num());

	uint8 Digest[16];
	Md5Gen.Final(Digest);

	FString MD5;
	for (int32 i = 0; i < 16; i++)
	{
		MD5 += FString::Printf(TEXT("%02x"), Digest[i]);
	}

	return MD5;
}

FString FArmyEngineTools::CalcMD5FromFileData(TArray<uint8>& _FileData)
{
	FMD5 Md5Gen;
	Md5Gen.Update(_FileData.GetData(), _FileData.Num());

	uint8 Digest[16];
	Md5Gen.Final(Digest);

	FString MD5;
	for (int32 i = 0; i < 16; i++)
	{
		MD5 += FString::Printf(TEXT("%02x"), Digest[i]);
	}

	return MD5;
}

FString FArmyEngineTools::GetTimeStampString()
{
	//int64 ts = (FDateTime::Now().GetTicks() - 621355968000000000) / 10000;
	int64 ts = (FDateTime::Now().GetTicks() - FDateTime(1970, 1, 1, 8).GetTicks()) / 10000;
	char buf[64];
	_i64toa_s(ts, buf, 64, 10);
	FString str = UTF8_TO_TCHAR(buf);
	return str;
}

FDateTime FArmyEngineTools::GetDateFromTimeStamp(FString _TimeStamp)
{
	int64 tick = FCString::Atoi64(*_TimeStamp);
	tick = tick * 10000 + FDateTime(1970, 1, 1, 8).GetTicks();
	FDateTime result = FDateTime(tick);
	return result;
}

FDateTime FArmyEngineTools::GetDateFromTimeStamp(int64 _TimeStamp)
{
	int64 tick = _TimeStamp;
	tick = tick * 10000 + FDateTime(1970, 1, 1, 8).GetTicks();
	FDateTime result = FDateTime(tick);
	return result;
}

FString FArmyEngineTools::Get24HoursStr(float _Time)
{
	int32 CurMinutes = 1440 * _Time;
	int32 CurHours = CurMinutes / 60;
	CurMinutes = CurMinutes - CurHours * 60;

	FString MinStr;
	if (CurMinutes < 10)
	{
		MinStr = FString::Printf(TEXT("0%d"), CurMinutes);
	}
	else
	{
		MinStr = FString::Printf(TEXT("%d"), CurMinutes);
	}

	FString HourStr;
	if (CurHours < 10)
	{
		HourStr = FString::Printf(TEXT("0%d"), CurHours);
	}
	else
	{
		HourStr = FString::Printf(TEXT("%d"), CurHours);
	}

	return FString::Printf(TEXT("%s:%s"), *HourStr, *MinStr);
}

FString FArmyEngineTools::GetYMDSTimeString()
{
	int32 Year = FDateTime::Now().GetYear();
	int32 Month = FDateTime::Now().GetMonth();
	int32 Day = FDateTime::Now().GetDay();
	int32 Hour = FDateTime::Now().GetHour();
	int32 Minute = FDateTime::Now().GetMinute();
	int32 Second = FDateTime::Now().GetSecond();
	int32 MSecond = FDateTime::Now().GetMillisecond();

	FString YearStr = FString::FromInt(Year);
	FString MonthStr = Month < 10 ? TEXT("0") + FString::FromInt(Month) : FString::FromInt(Month);
	FString DayStr = Day < 10 ? TEXT("0") + FString::FromInt(Day) : FString::FromInt(Day);
	FString HourStr = Hour < 10 ? TEXT("0") + FString::FromInt(Hour) : FString::FromInt(Hour);
	FString MinuteStr = Minute < 10 ? TEXT("0") + FString::FromInt(Minute) : FString::FromInt(Minute);
	FString SecondStr = Second < 10 ? TEXT("0") + FString::FromInt(Second) : FString::FromInt(Second);
	FString MSecondStr;

	if (MSecond < 10)
		MSecondStr = TEXT("00") + FString::FromInt(MSecond);
	else if (MSecond < 100)
		MSecondStr = TEXT("0") + FString::FromInt(MSecond);
	else
		MSecondStr = FString::FromInt(MSecond);

	FString TimeStr = YearStr + MonthStr + DayStr + HourStr + MinuteStr + SecondStr + MSecondStr;

	return TimeStr;
}

FString FArmyEngineTools::GetYMTimeString()
{
    int32 Year = FDateTime::Now().GetYear();
    int32 Month = FDateTime::Now().GetMonth();
    int32 Day = FDateTime::Now().GetDay();

    FString YearStr = FString::FromInt(Year);
    FString MonthStr = Month < 10 ? TEXT("0") + FString::FromInt(Month) : FString::FromInt(Month);
    FString DayStr = Day < 10 ? TEXT("0") + FString::FromInt(Day) : FString::FromInt(Day);

    return YearStr + MonthStr + DayStr;
}

void FArmyEngineTools::RedirectHttp(FString Url)
{
    if (Url != "")
    {
        FPlatformMisc::OsExecute(TEXT("open"), *Url);
    }
}