/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File AliyunOssHttp.h
 * @Description 阿里云oss Http管理
 *
 * @Author 欧石楠
 * @Date 2018年11月21日
 * @Version 1.0
 */

#pragma once

#include "Http.h"

DECLARE_DELEGATE_OneParam(FOnUploadComplete, TSharedPtr<class FAliyunOssResponse>);

/** 阿里云目录类型 */
enum EOssDir
{
    /** 资源 */
    OD_Resource = 1,

    /** 图片 */
    OD_Picture,

    /** 视频 */
    OD_Video,

    /** 全景图 */
    OD_Panorama,

    /** 临时文件 */
    OD_Temp
};

/**
 * 用户配置参数
 */
class FUserConfig
{
public:
    FUserConfig(FString InServerAddress, FString InLoginId, FString InToken)
        : ServerAddress(InServerAddress)
        , LoginId(InLoginId)
        , Token(InToken)
    {}

    void CheckValidation()
    {
        check(ServerAddress.Len() > 0);
        check(LoginId.Len() > 0);
        check(Token.Len() > 0);
    }

public:
    /** 服务器地址 */
    FString ServerAddress;

    /** 登录id */
    FString LoginId;

    /** 用户token */
    FString Token;
};

/**
 * 阿里云oss上传请求数据
 */
class FAliyunOSSRequest
{
public:
    /** 获得上传请求的url */
    const FString GetUrl() const;

    /** 获得下载的url */
    const FString GetCdnUrl() const;

    const FString GetResourcePath() const;

    const FString GetCanonicalizedOssHeaders() const;

    const FString GetCanonicalizedResource() const;

    /** 构造http请求 */
    TSharedPtr<IHttpRequest> Build();

public:
    FString AccessKeyId;

    FString AccessKeySecret;

    FString SecurityToken;

    FString Endpoint;

    FString BucketName;

    FString CdnUrl;

    /** 资源文件夹名 */
    FString Dir;

    /** 文件名 */
    FString FileName;

    /** 上传内容 */
    TArray<uint8> Content;

    /** 上传回调 */
    FOnUploadComplete OnUploadComplete;
};

/**
 * 阿里云oss上传返回数据
 */
class FAliyunOssResponse
{
public:
    /** 接收请求返回的数据 */
    void Receive(bool InbWasSuccessful, FHttpResponsePtr Response);

public:
    /** 资源下载地址 */
    FString CdnUrl;

    /** 是否上传成功 */
    bool bWasSuccessful;

    /** http code */
    int32 ResponseCode = -1;

    /** 提示信息 */
    FString Message;
};