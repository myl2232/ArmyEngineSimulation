/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File AliyunOss.h
 * @Description 阿里云oss管理器
 *
 * @Author 欧石楠
 * @Date 2018年11月21日
 * @Version 1.0
 */

#pragma once

#include "AliyunOssHttp.h"

class FAliyunOss
{
public:
    ALIYUNOSS_API static FAliyunOss& Get();

    /**
     * 初始化用户相关参数
     * @param InUserConfig - TSharedPtr<FUserConfig> - 用户信息
     */
    ALIYUNOSS_API void InitUserConfig(TSharedPtr<FUserConfig> InUserConfig);

    /**
     * 上传文件内容到阿里云oss服务器
     * @param OssDir - EOssDir - 阿里云oss资源路径类型
     * @param FileName - const FString & - 文件名
     * @param Content - const TArray<uint8> & - 文件二进制流内容
     * @param OnUploadComplete - FOnUploadComplete - 上传完成的回调
     */
    ALIYUNOSS_API void PutObject(EOssDir OssDir, const FString& FileName, const TArray<uint8>& Content, FOnUploadComplete OnUploadComplete);

    /**
     * 上传文件内容到阿里云oss服务器
     * @param OssDir - EOssDir - 阿里云oss资源路径类型
     * @param Dir - const FString & - 自定义文件夹路径
     * @param FileName - const FString & - 文件名
     * @param Content - const TArray<uint8> & - 文件二进制流内容
     * @param OnUploadComplete - FOnUploadComplete - 上传完成的回调
     */
    ALIYUNOSS_API void PutObject(EOssDir OssDir, const FString& Dir, const FString& FileName, const TArray<uint8>& Content, FOnUploadComplete OnUploadComplete);

private:
    /** 获取oss配置 */
    void ReqOssConfig(EOssDir OssDir, const FString& Dir, FString FileName);
    void ResOssConfig(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString FileName);

    /** 请求上传阿里云oss */
    void ResUploadOss(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString FileName);

private:
    static TSharedPtr<FAliyunOss> Singleton;

    TSharedPtr<FUserConfig> UserConfig;

    TMap<FString, TSharedPtr<FAliyunOSSRequest> > OssRequestMap;
};