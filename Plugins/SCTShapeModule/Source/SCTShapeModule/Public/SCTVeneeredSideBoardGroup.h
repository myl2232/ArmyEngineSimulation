/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SCTVeneeredSideBoardGroup.h
* @Description 减光板组
*
* @Author 欧石楠
* @Date 2019年5月9日
* @Version 1.0
*/
#pragma once


#include "SCTShape.h"
#include "SCTShapeData.h"
#include "SCTBoardShape.h"


/** 饰条 */
class SCTSHAPEMODULE_API FVeneeredSudeBoardBase : public FSCTShape
{
public:
	struct FVeneeredSudeBoardMaterial : public FCommonPakData {};
	enum class EVeneeredSudeBoardShape : uint8
	{
		E_None_Model = 0,		 /*!< 无造型,使用板件来构造*/
		E_Mosaic_Model = 1 << 0,	 /*!< 拼接造型，使用模型拼接的方法来实现*/
	};
	enum class EPosition : uint8
	{
		E_Left = 0,		/*!< 左侧*/
		E_Right = 1 << 0,	/*!< 右侧*/
		E_Back = 1 << 1	/*!< 后面*/
	};
public:
	FVeneeredSudeBoardBase() { SetShapeType(ST_None); }	
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
	virtual void SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	/** 拷贝创建新的型录 */
	virtual void CopyTo(FSCTShape* OutShape) override;

	/** 创建型录Actor */
	virtual ASCTShapeActor* SpawnShapeActor() override;

	virtual void SpawnActorsForSelected(FName InSelectProfileName);

	/** 设置ProfileName */
	virtual void SetCollisionProfileName(FName InProfileName) override;

	/** 获取型录中的所有资源Url */
	virtual void GetResourceUrls(TArray<FString> &OutResourceUrls) override;

	/** 获取型录中的所有资源路径 */
	virtual void GetFileCachePaths(TArray<FString> &OutFileCachePaths) override;
public:
	void HiddenVeneeredSudeBoard(bool bHidden);

	/** 获取造型 */
	EVeneeredSudeBoardShape GetModelShape() const { return ModelShape; }

	/** 材质 */
	const FVeneeredSudeBoardMaterial & GetMaterial() const { return Material; }
	void SetMaterial(const FVeneeredSudeBoardMaterial & InMaterial) { Material = InMaterial; }

	/**  见光板位置 */
	virtual EPosition GetPosition() const { return Position; }
	virtual void SetPosition(const EPosition InPosition) { Position = InPosition; }

	/** 更新见光板 */
	virtual void UpdateVeneeredSudeBoard() = 0;

	/** 封边 */
	const FVeneeredSudeBoardMaterial & GetEdgeMaterial() const { return EdgeBanding; }
	FVeneeredSudeBoardMaterial & GetEdgeMaterial() { return EdgeBanding; }
	void SetEdgeMaterial(const FVeneeredSudeBoardMaterial & InMaterial) { EdgeBanding = InMaterial; }

	/** 基材类型 */
	void SetSubstarteType(const int32 InType) { SubstrateType = InType; }
	int32 GetSubstrateType() const { return SubstrateType; }

	/** 基材ID */
	void SetSubstateId(const int32 InSubstrateId) { SubstrateID = InSubstrateId; }
	int32 GetSubstrateId() const { return SubstrateID; }

	/** 基材名称 */
	void SetSubstrateName(const FString & InName) { SubstrateName = InName; }
	const FString & GetSubstrateName() const { return SubstrateName; }

	bool GetSpawnActorVallid() const { return bToSpawnActor; }
	void SetSpawnActorValid(const bool bInValid) { bToSpawnActor = bInValid; }

protected:
	EVeneeredSudeBoardShape ModelShape = EVeneeredSudeBoardShape::E_None_Model;	
	EPosition Position = EPosition::E_Left;
	FVeneeredSudeBoardMaterial EdgeBanding;     /*!< 封边*/
	FVeneeredSudeBoardMaterial Material;	    /*!< 材质*/
	int32 SubstrateType = -1;					/*!< 基材类型*/
	int32 SubstrateID = -1;						/*!< 基材ID */
	FString SubstrateName;						/*!< 基材名称 */
	bool bToSpawnActor = true;					/*!< 是否创建Actor*/
};


class SCTSHAPEMODULE_API FNoneModelVeneeredSudeBoard final : public FVeneeredSudeBoardBase
{	
public:
	FNoneModelVeneeredSudeBoard() { ShapeType = ST_NoneModelVeneeredBoard;  Position = EPosition::E_Left; ModelShape = EVeneeredSudeBoardShape::E_None_Model; ShapeName = TEXT("非造型见光板");  }
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
	virtual void SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	/** 拷贝创建新的型录 */
	virtual void CopyTo(FSCTShape* OutShape) override;

	/** 创建型录Actor */
	virtual ASCTShapeActor* SpawnShapeActor() override;

	virtual void SpawnActorsForSelected(FName InSelectProfileName) override;

	/** 设置ProfileName */
	virtual void SetCollisionProfileName(FName InProfileName) override;

	/** 获取型录中的所有资源Url */
	virtual void GetResourceUrls(TArray<FString> &OutResourceUrls) override;

	/** 获取型录中的所有资源路径 */
	virtual void GetFileCachePaths(TArray<FString> &OutFileCachePaths) override;

	virtual void UpdateVeneeredSudeBoard() override;

	TSharedPtr<FBoardShape> GetBaordShape() const { return BoardShape; }

public:
	void SetIsToUse(const bool InValue) { bIsToUse = InValue; }
	bool GetIsToUse() const { return bIsToUse; }

private:
	void CreateBoardShape();
private:
	TSharedPtr<FBoardShape> BoardShape;			/*!< 使用板件构建见光板*/	
	bool  bIsToUse = false;						/*!< 是否被使用*/
};


class SCTSHAPEMODULE_API FVeneerdSudeBoardGroup : public FSCTShape
{

public:
	struct FDefaultSettingInfo
	{
		float substrateHeight = 0.0f; /**< 默认基材厚度 */
		FVeneeredSudeBoardBase::FVeneeredSudeBoardMaterial EdgeBanding; /**< 默认封边信息 */
		FVeneeredSudeBoardBase::FVeneeredSudeBoardMaterial Material;    /**< 默认材质 */
		int32 SubstrateID = 0;
		int32 SubstrateType = 0;
		FString SubstrateName;
	};	
public:
	FVeneerdSudeBoardGroup() { ShapeType = ST_VeneerdBoardGroup; ShapeName = TEXT("见光板组");}	
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
	virtual void SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	/** 拷贝创建新的型录 */
	virtual void CopyTo(FSCTShape* OutShape) override;

	/** 创建型录Actor */
	virtual ASCTShapeActor* SpawnShapeActor() override;

	void SpawnActorsForSelected(FName InSelectProfileName);

	/** 设置ProfileName */
	virtual void SetCollisionProfileName(FName InProfileName) override;

	/** 获取型录中的所有资源Url */
	virtual void GetResourceUrls(TArray<FString> &OutResourceUrls) override;

	/** 获取型录中的所有资源路径 */
	virtual void GetFileCachePaths(TArray<FString> &OutFileCachePaths) override;

public:

	void HiddenVeneerdGroup(bool bHidden);

	TSharedPtr<FVeneeredSudeBoardBase> GetSpecialPositionBoard(const FVeneeredSudeBoardBase::EPosition InPosition);

	/** 添加见光板 */
	void AddVeneeredSudeBoard(TSharedPtr<FVeneeredSudeBoardBase> InBoard);

	/** 删除见光板 */
	void DeleteVeneeredSudeBoard(FVeneeredSudeBoardBase * InBoard);

	/**
	 * @brief 设置指定位置的见光板是否生成Actor
	 * @param[in] InPosition 见光板位置
	 * @param[in] bInValid	 是否需要生成Actor,如果已经生成，则销毁
	 * @return 如果存储，且设置成功，则返回true,否则返回false
	 */
	bool SetSpecialBoardActorValid(const FVeneeredSudeBoardBase::EPosition InPosition,const bool bInValid);

	bool GetSpecialBoardActorValid(const FVeneeredSudeBoardBase::EPosition InPosition);

	/** 获取默认见光板设置信息 */
	const FDefaultSettingInfo & GetDefaultVeneerdSudeBoardInof() const {
		return VeneerdSudeBoardDefaultInfo;
	}
	FDefaultSettingInfo & GetDefaultVeneerdSudeBoardInof() {
		return VeneerdSudeBoardDefaultInfo;
	}

public:

	void UpdateGroup();
private:
	TArray<TSharedPtr<FVeneeredSudeBoardBase>> VeneeredSudeBoards;
	FDefaultSettingInfo VeneerdSudeBoardDefaultInfo;
};




