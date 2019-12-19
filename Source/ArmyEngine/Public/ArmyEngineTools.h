#pragma once

class ARMYENGINE_API FArmyEngineTools
{
public:
	static FString CalcMD5FromFile(FString _FilePath);
	static FString CalcMD5FromFileData(TArray<uint8>& _FileData);

	static FString GetTimeStampString();
	static FDateTime GetDateFromTimeStamp(FString _TimeStamp);
	static FDateTime GetDateFromTimeStamp(int64 _TimeStamp);

	/** Time 0.f-1.f 对应00:00-24：00 */
	static FString Get24HoursStr(float _Time);

	/** 20170405121212333-yyyyMMddHHmmssSSS */
	static FString GetYMDSTimeString();

    /** 20180718-yyyyMMdd */
    static FString GetYMTimeString();

    /**
     * 跳转并打开指定的网页
     * @param Url - FString - 网页地址
     */
    static void RedirectHttp(FString Url);
};