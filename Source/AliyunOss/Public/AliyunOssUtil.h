/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File AliyunOssUtil.h
 * @Description 阿里云oss工具类
 *
 * @Author 欧石楠
 * @Date 2018年11月21日
 * @Version 1.0
 */

#pragma once

class FAliyunOssUtil
{
public:
    /** 加载Content-Type对应表 */
    static void LoadContentTypeMap();

    /**
     * 生成二进制流的Content-MD5
     * @param Content - const TArray<uint8> & - 二进制流
     * @return const FString - Content-MD5
     */
    static const FString GenerateContentMd5(const TArray<uint8>& Content);

    /**
     * 通过文件名后缀获取Content-Type
     * @param FileName - FString - 文件名
     * @return const FString - Content-Type
     */
    static const FString GetContentType(FString FileName);

    /** 生成签名 */
    static const FString GenerateAuthorization(
        const FString& ContentMd5, 
        const FString& ContentType, 
        const FString& Date, 
        const FString& CanonicalizedOssHeaders, 
        const FString& CanonicalizedResource, 
        const FString& AccessKeyId, 
        const FString& AccessKeySecret);

public:
    static TMap<FString, FString> ContentTypeMap;
};