/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SCTEmbededElectricalGroup.h
* @Description 电器组类
*
* @Author 欧石楠
* @Date 2019年1月17日
* @Version 1.0
*/
#pragma once


#include "SCTShape.h"
#include "SCTGlobalDataForShape.h"
#include "SCTShapeData.h"

/** 饰条 */
class SCTSHAPEMODULE_API FInsertionBoard : public FSCTShape
{
public:
	struct FInsertBoardMaterial : public FCommonPakData {};
	enum class EInserBoardPositionType : uint8
	{
		E_None,  /*!< 无*/
		E_Up,	 /*!< 上*/
		E_Bottom /*!< 下*/
	};
public:
	FInsertionBoard() { SetShapeType(ST_InsertionBoard); }
	~FInsertionBoard() = default;
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

	/** 用于创建子型录用于点选的Actor */
	void SpawnActorsForSelected(FName InProfileName);

	/** 设置ProfileName */
	virtual void SetCollisionProfileName(FName InProfileName) override;	

	/** 获取型录中的所有资源Url */
	virtual void GetResourceUrls(TArray<FString> &OutResourceUrls) override;
	/** 获取型录中的所有资源路径 */
	virtual void GetFileCachePaths(TArray<FString> &OutFileCachePaths) override;

public:
	/** 外盖引起的上延 */
	float GetUpExtension() const { return UpExtension; }
	void SetUpExtension(const float InExtension) { UpExtension = InExtension; }

	/** 上延修正 */
	float GetUpFixedExtension() const { return UpFixedExtension; }
	void SetUpFixedExtension(const float InExtension) { UpFixedExtension = InExtension; }

	/** 下延*/
	float GetDownExtension() const { return DownExtension; }
	void SetDownExtension(const float InExtension) { DownExtension = InExtension; }

	/** 下延修正 */
	float GetDownFixedExtension() const { return DownFixedExtension; }
	void SetDownFixedExtension(const float InExtension) { DownFixedExtension = InExtension; }

	/** 左延*/
	float GetLeftExtension() const { return LeftExtension; }
	void SetLeftExtension(const float InExtension) { LeftExtension = InExtension; }

	/** 左延修正 */
	float GetLeftFixedExtension() const { return LeftFixedExtension; }
	void SetLeftFixedExtension(const float InExtension) { LeftFixedExtension = InExtension; }

	/** 右延 */
	float GetRightExtension() const { return RightExtension; }
	void SetRightExtension(const float InExtension) { RightExtension = InExtension; }

	/** 右延修正 */
	float GetRightFixedExtension() const { return RightFixedExtension; }
	void SetRightFixedExtension(const float InExtension) { RightFixedExtension = InExtension; }

	/** 内缩 */
	float GetInnerExtension() const { return InnerExtension; }
	void SetInnerExtension(const float InExtension) { InnerExtension = InExtension; }

	/** 内缩 */
	float GetInnerFixedExtension() const { return InnerFixedExtension; }
	void SetInnerFixedExtension(const float InExtension) { InnerFixedExtension = InExtension; }

	/** 材质 */
	const FInsertBoardMaterial & GetMaterial() const { return Material; }
	FInsertBoardMaterial & GetMaterial() { return Material; }
	void SetMaterial(const FInsertBoardMaterial & InMaterial) { Material = InMaterial; }

	/** 封边 */
	const FInsertBoardMaterial & GetEdgeMaterial() const { return EdgeBanding; }
	FInsertBoardMaterial & GetEdgeMaterial() { return EdgeBanding; }
	void SetEdgeMaterial(const FInsertBoardMaterial & InMaterial) { EdgeBanding = InMaterial; }

	/** 基材类型 */
	void SetSubstarteType(const int32 InType) { SubstrateType = InType; }
	int32 GetSubstrateType() const { return SubstrateType; }

	/** 基材ID */
	void SetSubstateId(const int32 InSubstrateId) { SubstrateID = InSubstrateId; }
	int32 GetSubstrateId() const { return SubstrateID; }

	/** 基材名称 */
	void SetSubstrateName(const FString & InName) { SubstrateName = InName; }
	const FString & GetSubstrateName() const { return SubstrateName; }

	/** 饰条位置 */
	EInserBoardPositionType GetInserBoardPositionType() const { return InserBoardPositionType; }
	void SetInserBoardPositionType(const EInserBoardPositionType InType) { InserBoardPositionType = InType; }

	/** 箱体拉手 */
	void SetLateralBoxHanleShape(const TSharedPtr<FAccessoryShape> InShape);
	TSharedPtr<FAccessoryShape> GetLateralBoxHanleShape() { return LateralBoxShape; }
	void RemoveLateralBoxHandleShape();
	bool IsLaterBoxHanleValid() const { return LateralBoxShape.IsValid(); }

	/** 箱体拉手距离门板 */
	float GetLateralBoxLenToInsertionBoard() const { return LateralBoxLenToInsertionBoard; }
	void SetLateralBoxHanleLenToInsertionBoard(float InValue) { LateralBoxLenToInsertionBoard = InValue; }

public:
	/**  更新板件饰条 */
	void UpdateBoardShape();
private:
	/** 创建板件饰条 */
	bool CreateBoardShape();	
private:
	float UpExtension = 0.0f;		/*!< 上延*/
	float DownExtension = 0.0f;		/*!< 下延*/
	float LeftExtension = 0.0f;		/*!< 左延*/
	float RightExtension = 0.0f;	/*!< 右延*/
	float InnerExtension = 0.0f;	/*!< 内缩*/

	float UpFixedExtension = 0.0f;		/*!< 上延修正*/
	float DownFixedExtension = 0.0f;	/*!< 下延修正*/
	float LeftFixedExtension = 0.0f;	/*!< 左延修正*/
	float RightFixedExtension = 0.0f;	/*!< 右延修正*/
	float InnerFixedExtension = 0.0f;	/*!< 内缩修正*/

	FInsertBoardMaterial EdgeBanding;  /*!< 封边*/
	FInsertBoardMaterial Material;	   /*!< 材质*/
	int32 SubstrateType = -1;		   /*!< 基材类型*/
	int32 SubstrateID = -1;			   /*!< 基材ID */
	FString SubstrateName;	           /*!< 基材名称 */
	TSharedPtr<FBoardShape> BoardShape;/*!< 饰条板件*/	
	EInserBoardPositionType InserBoardPositionType = EInserBoardPositionType::E_None; /*!< 饰条位置 */
	TSharedPtr<FAccessoryShape> LateralBoxShape;	 /** 箱体拉手 */
	float LateralBoxLenToInsertionBoard = 0.0f;	/** 箱体拉手距离门板 */
};


/** 电器组基类 */
class SCTSHAPEMODULE_API FEmbededElectricalGroup : public FSCTShape
{

public:
	FEmbededElectricalGroup() { SetShapeType(ST_EmbeddedElectricalGroup); SetShapeName(TEXT("电器饰条组")); }
	~FEmbededElectricalGroup() = default;
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
	/** 用于创建子型录用于点选的Actor */
	void SpawnActorsForSelected(FName InProfileName);
	/** 设置ProfileName */
	virtual void SetCollisionProfileName(FName InProfileName) override;

	/** 获取型录中的所有资源Url */
	virtual void GetResourceUrls(TArray<FString> &OutResourceUrls) override;
	/** 获取型录中的所有资源路径 */
	virtual void GetFileCachePaths(TArray<FString> &OutFileCachePaths) override;


public:
	/** 更新电器饰条组 */
	void UpdateEmbededEletricalShape();

	/** 左侧相关板件的厚度 */
	void SetLeftRelativeBoardWidth(const float InValue) { LeftRelativeBoardWidth = InValue; }
	float GetLeftRelativeBoardWidth() const { return LeftRelativeBoardWidth; }

	/** 右侧相关板件的厚度 */
	void SetRightRelativeBoardWidth(const float InValue) { RightRelativeBoardWidth = InValue; }
	float GetRightRelativeBoardWidth() const { return RightRelativeBoardWidth; }
public:
	/** 饰条 */
	void SetInsertionBoardShape(TSharedPtr<FInsertionBoard> InInsertionBoardShape);
	TSharedPtr<FInsertionBoard>	GetInsertionBoardShape() { return InsertionBoardShape; }
	void RemoveInserionBoardShpae();

	/** 电器 */
	void SetEmbededEletricalShape(TSharedPtr<FAccessoryShape> InShape);
	TSharedPtr<FAccessoryShape> GetEmbededEletricalShape() { return EmbededEletricalShape; }
private:
	TSharedPtr<FAccessoryShape> EmbededEletricalShape; /*!< 嵌入式电器*/
	TSharedPtr<FInsertionBoard> InsertionBoardShape;   /*!< 饰条板件 */
	FName ProfileName;
	float LeftRelativeBoardWidth = 0.0f;
	float RightRelativeBoardWidth = 0.0f;	
};

