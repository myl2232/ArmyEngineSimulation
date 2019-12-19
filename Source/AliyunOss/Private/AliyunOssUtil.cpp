#include "AliyunOssUtil.h"
#include "Base64.h"
#include "SecureHash.h"

TMap<FString, FString> FAliyunOssUtil::ContentTypeMap;

void FAliyunOssUtil::LoadContentTypeMap()
{
    const FString MapFilePath = FPaths::ProjectContentDir() + "DBJCache/Common/ContentType.ct";
    FString FileContent;
    FFileHelper::LoadFileToString(FileContent, *MapFilePath);

    FString DelimiterSpace;
    DelimiterSpace.AppendChar(' ');
    FString DelimiterTab;
    DelimiterTab.AppendChar('\t');

    TArray<FString> StrLines;
    FileContent.ParseIntoArrayLines(StrLines);

    for (FString& It : StrLines)
    {
        FString Key, Value;
        It.Split(DelimiterSpace, &Key, &Value);
        if (Key.IsEmpty() && Value.IsEmpty())
        {
            It.Split(DelimiterTab, &Key, &Value);
        }

        check(!Key.IsEmpty() && !Value.IsEmpty());

        ContentTypeMap.Add(Key.TrimEnd(), Value.TrimStart());
    }
}

const FString FAliyunOssUtil::GenerateContentMd5(const TArray<uint8>& Content)
{
    FMD5 Md5Gen;
    Md5Gen.Update(Content.GetData(), Content.Num());
    uint8 Digest[16];
    Md5Gen.Final(Digest);
    
    return FBase64::Encode(Digest, 16);
}

const FString FAliyunOssUtil::GetContentType(FString FileName)
{
    FString Name, Suffix;
    FileName.Split(".", &Name, &Suffix);

    check(!Suffix.IsEmpty());
    return *ContentTypeMap.Find(Suffix);
}

const FString FAliyunOssUtil::GenerateAuthorization(
    const FString& ContentMd5, 
    const FString& ContentType, 
    const FString& Date, 
    const FString& CanonicalizedOssHeaders, 
    const FString& CanonicalizedResource, 
    const FString& AccessKeyId, 
    const FString& AccessKeySecret)
{
    FString MsgStr = "PUT\n" + ContentMd5 + "\n" + ContentType + "\n" + Date + "\n" + CanonicalizedOssHeaders + CanonicalizedResource;

    TArray<uint8> Msg;
    FTCHARToUTF8 MsgConverter(*MsgStr);
    Msg.SetNum(MsgConverter.Length());
    FMemory::Memcpy(Msg.GetData(), MsgConverter.Get(), MsgConverter.Length());
    
    TArray<uint8> Key;
    FTCHARToUTF8 KeyConverter(*AccessKeySecret);
    Key.SetNum(KeyConverter.Length());
    FMemory::Memcpy(Key.GetData(), KeyConverter.Get(), KeyConverter.Length());
    
    FSHAHash MsgHash;
    FSHA1::HMACBuffer(Key.GetData(), Key.Num(), Msg.GetData(), Msg.Num(), MsgHash.Hash);
    
    FString Signature = FBase64::Encode(MsgHash.Hash, sizeof(MsgHash.Hash));

    return "OSS " + AccessKeyId + ":" + Signature;
}
