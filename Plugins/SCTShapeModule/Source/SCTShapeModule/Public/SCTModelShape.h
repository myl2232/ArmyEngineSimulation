/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File SCTModelShape.h
 * @Description 模型(包括五金和饰品)
 *
 * @Author 赵志强
 * @Date 2018年5月15日
 * @Version 1.0
 */
#pragma once

#include "SCTShape.h"

class UStaticMesh;

// 饰品模型型录
class SCTSHAPEMODULE_API FDecorationShape : public FSCTShape
{
public:
	FDecorationShape();
	virtual ~FDecorationShape();
public:
	static FString GetCacheFilePathFromIDAndMd5AndUpdateTime(const int32 & InModelId, const FString & InMd5, const float & InUpdateTime);
	static FString GetCacheFilePathFromIDAndMd5AndUpdateTime(const int32 & InModelId, const FString & InMd5, const FString & InUpdateTime);
	static FString GetCacheFilePathFromIDAndMd5AndUpdateTime(const FString & InModelId, const FString & InMd5, const FString & InUpdateTime);
	static FString GetCacheFilePathFromIDAndMd5AndUpdateTime(const FString & InModelId, const FString & InMd5, const float & InUpdateTime);
	static FString GetCacheFileName(const FString & InModelId, const FString & InMd5, const FString & InUpdateTime);
	static FString GetCacheFileNameWithoutUpdateTime(const FString & InModelId, const FString & InMd5);	
public:
	/** 解析JSon文件创建型录 */
	virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;

	/** 将型录转存为JSon文件 */
	virtual void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	virtual void GetResourceUrls(TArray<FString> &OutResourceUrls) override;
	virtual void GetFileCachePaths(TArray<FString> &OutFileCachePaths) override;

	/** 拷贝创建新的型录 */
	virtual void CopyTo(FSCTShape* OutShape) override;

	/** 创建型录Actor */
	virtual ASCTShapeActor* SpawnShapeActor() override;
	/** 创建子型录用于点选的Actor */
	void SpawnActorsForSelected(FName InProfileName);
	/** 设置ProfileName */
	virtual void SetCollisionProfileName(FName InProfileName) override;

	/** 重载设置型录尺寸 */
	virtual bool SetShapeWidth(float InValue) override;
	virtual bool SetShapeWidth(const FString &InStrValue) override;
	virtual bool SetShapeDepth(float InValue) override;
	virtual bool SetShapeDepth(const FString &InStrValue) override;
	virtual bool SetShapeHeight(float InValue) override;
	virtual bool SetShapeHeight(const FString &InStrValue) override;
	/** 重载设置型录尺寸 */
	virtual bool SetShapePosX(const FString &InStrValue) override;
	virtual bool SetShapePosY(const FString &InStrValue) override;
	virtual bool SetShapePosZ(const FString &InStrValue) override;
	virtual bool SetShapePosX(float InValue) override;
	virtual bool SetShapePosY(float InValue) override;
	virtual bool SetShapePosZ(float InValue) override;

	/** 如果是动画模型的话，我们无法在设置模型资源的时候，获取正确的尺寸信息，必须再Actor Tick的时候才能获取正确尺寸信息*/
	//virtual void ReBuildBoxSphereBounds(const FBoxSphereBounds & InBoundBox);
public:
	//void SetModelType(int32 InType);
	//int32 GetModelType();

	void SetUpdateTime(FString InTime);
	FString GetUpdateTime();

	void SetModelID(int64 InID);
	int64 GetModelID();

	void SetFileName(FString InName);
	FString GetFileName();

	void SetoptimizeParam(FString InName) { OptimizeParam = InName; }
	FString GetoptimizeParam() { return OptimizeParam; }

	void SetFileUrl(FString InUrl);
	FString GetFileUrl();

	void SetFileMd5(FString inMd5);
	FString GetFileMd5();

	bool LoadModelMesh(const bool bForceReload = false);
	//TSharedPtr<UStaticMesh> GetModelMesh();

	const FBoxSphereBounds GetBoxSphereBounds(const bool InTryGetCollisionBox = false) const;

	const FString & GetFileCacheDir() const { return FileCacheDir; }

	bool IsMeshValid() const { return ModelMesh.Type != -1; }
protected:
	virtual const FString GetModelIdStr();
private:
	//模型类型
	//EModelType ModelType;
	//模型更新时间
	FString UpdateTimeStr;

	//模型文件信息
	int64 ModelID;       //数据库ID
	FString FileName;    //文件名称
	FString FileUrl;     //文件URL
	FString FileMd5;     //文件MD5码
	FString OptimizeParam; // Pak优化参数

	//模型网格
	FMeshDataEntry ModelMesh;
	static FString FileCacheDir;	
};

// 切块专用
class SCTSHAPEMODULE_API FDividBlockShape :public FDecorationShape
{
protected:
	virtual const FString GetModelIdStr() override;
public:
	virtual ASCTShapeActor* SpawnShapeActor() override;
};

// 五金配件模型型录
class SCTSHAPEMODULE_API FAccessoryShape : public FDecorationShape
{
public:
	struct FMetalsProperty
	{
		FString MetallId;
		FString propertyId;
		FString PropertyValueIdentity;
		FString propertyValue;
	};
public:
	FAccessoryShape();
	virtual ~FAccessoryShape();

public:
	/** 解析JSon文件创建型录 */
	virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;

	/** 将型录转存为JSon文件 */
	virtual void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	/** 拷贝创建新的型录 */
	virtual void CopyTo(FSCTShape* OutShape) override;

	/** 创建型录Actor */
	virtual ASCTShapeActor* SpawnShapeActor() override;
	/** 设置ProfileName */
	virtual void SetCollisionProfileName(FName InProfileName) override;

	/** 重载设置型录尺寸 */
	virtual bool SetShapeWidth(float InValue) override;
	virtual bool SetShapeWidth(const FString &InStrValue) override;
	virtual bool SetShapeDepth(float InValue) override;
	virtual bool SetShapeDepth(const FString &InStrValue) override;
	virtual bool SetShapeHeight(float InValue) override;
	virtual bool SetShapeHeight(const FString &InStrValue) override;
	/** 重载设置型录尺寸 */
	virtual bool SetShapePosX(const FString &InStrValue) override;
	virtual bool SetShapePosY(const FString &InStrValue) override;
	virtual bool SetShapePosZ(const FString &InStrValue) override;
	virtual bool SetShapePosX(float InValue) override;
	virtual bool SetShapePosY(float InValue) override;
	virtual bool SetShapePosZ(float InValue) override;

public:
	void SetProduceCode(const FString& InCode);
	FString GetProduceCode();
	/**  产品品牌 */
	FString GetBrandName() const { return BrandName; }
	void SetBrandName(const FString & InStr) { BrandName = InStr; }
	/**  产品型号 */
	FString GetProductVersion() const { return ProductVersion; }
	void SetProductVersion(const FString & InStr) { ProductVersion = InStr; }

	const TArray<FMetalsProperty> & GetMetalsPropertyList() const { return MetalsPropertyList; }
	void SetMetalsPropertyList(const TArray<FMetalsProperty> & InPropertyList) { MetalsPropertyList = InPropertyList; }
private:	
	FString ProduceCode; /*!< 生产编号 */
	FString BrandName;	 /*!< 品牌 */
	FString ProductVersion;  /*!< 产品型号 */

	TArray<FMetalsProperty> MetalsPropertyList;
};

