/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File SCTShape.h
 * @Description 型录基类
 *
 * @Author 赵志强
 * @Date 2018年5月15日
 * @Version 1.0
 */
#pragma once
#include "SCTShapeData.h"
#include "Materials/MaterialInterface.h"
#include "Json.h"

class FShapeAttribute;
class FAny;
class FFormula;
class ASCTShapeActor;
class ASCTBoundingBoxActor;
class ASCTWireframeActor;

// 型录基类
class SCTSHAPEMODULE_API FSCTShape
{
public:
	FSCTShape();
	virtual ~FSCTShape();
public:
	struct FTag
	{
		FGuid Guid;
		FString Name;
		FString Data;
		FString Description;
	};
	
public:
	/** 解析JSon文件创建型录 */
	virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject);
	virtual void ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject);
	virtual void ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject);
	virtual void ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject);

	/** 将型录转存为JSon文件 */
	virtual void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
	virtual void SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
	virtual void SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
	virtual void SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);

	/** 获取型录中的所有资源Url */
	virtual void GetResourceUrls(TArray<FString> &OutResourceUrls);
	/** 获取型录中的所有资源路径 */
	virtual void GetFileCachePaths(TArray<FString> &OutFileCachePaths);

	/** 检查资源url与缓冲路径 -- 规避重复资源*/
	virtual void CheckResourceUrlsAndCachePaths(TArray<FString> &OutResourceUrls, TArray<FString> &OutFileCachePaths);

	/** 拷贝创建新的型录 */
	// TODO: Add & or not
	virtual void CopyTo(FSCTShape* OutShape);

	/** 创建型录Actor */
	virtual ASCTShapeActor* SpawnShapeActor();
	virtual void DestroyShapeActor();
	/** 设置ProfileName */
	virtual void SetCollisionProfileName(FName InProfileName);

public:
	/** 获取型录类型 */
	FORCEINLINE EShapeType GetShapeType() const { return ShapeType; }
	FORCEINLINE void SetShapeType(EShapeType InShapeType) { ShapeType = InShapeType; }

	FORCEINLINE int32 GetShapeCategory() const { return ShapeCategory; }
	FORCEINLINE void SetShapeCategory(int32 InCategory) { ShapeCategory = InCategory; }

	//型录基础信息相关操作
	FORCEINLINE void SetShapeId(int64 Id) { ShapeId = Id; }
	FORCEINLINE int64 GetShapeId() const { return ShapeId; }

	FORCEINLINE void SetShapeName(const FString& InName) { ShapeName = InName; }
	FORCEINLINE const FString& GetShapeName() const { return ShapeName; }

	FORCEINLINE void SetShapeCode(const FString &Code) { ShapeCode = Code; }
	FORCEINLINE const FString& GetShapeCode() const { return ShapeCode; }

	FORCEINLINE void SetThumbnailUrl(const FString &Url) { ThumbnailUrl = Url; }
	FORCEINLINE const FString& GetThumbnailUrl() const { return ThumbnailUrl; }

	FORCEINLINE void SetSystemName(const FString &InName) { SystemName = InName; }
	FORCEINLINE const FString& GetSystemName() const { return SystemName; }

	FORCEINLINE void SetProductStatus(EProductStatus InStatus) { ProductStatus = InStatus; }
	FORCEINLINE EProductStatus GetProductStatus() const { return ProductStatus; }

	FORCEINLINE void SetProductAbnormal(bool InTag) { bAbnormal = InTag; }
	FORCEINLINE bool GetProductAbnormal() const { return bAbnormal; }

	FORCEINLINE void SetJsonFileUrl(const FString &InUrl) { JsonFileUrl = InUrl; }
	FORCEINLINE const FString& GetJsonFileUrl() const { return JsonFileUrl; }

	FORCEINLINE void SetUpdataTime(const FString &InTime) { UpdateTime = InTime; }
	FORCEINLINE const FString& GetUpdateTime() const { return UpdateTime; }

	//系统属性信息操作
	bool SetShowCondition(const FString &InStrValue);
	bool SetShowCondition(bool InbValue);
	bool GetShowCondition();
	const TSharedPtr<FShapeAttribute>& GetShowCondAttri() const;

	void ChangeShapeWidthAttriType(EShapeAttributeType InType);
	virtual bool SetShapeWidth(const FString &InStrValue);
	virtual bool SetShapeWidth(float InIntValue);
	float GetShapeWidth();
	const TSharedPtr<FShapeAttribute>& GetShapeWidthAttri() const;

	void ChangeShapeDepthAttriType(EShapeAttributeType InType);
	virtual bool SetShapeDepth(const FString &InStrValue);
	virtual bool SetShapeDepth(float InIntValue);
	float GetShapeDepth();
	const TSharedPtr<FShapeAttribute>& GetShapeDepthAttri() const;

	void ChangeShapeHeightAttriType(EShapeAttributeType InType);
	virtual bool SetShapeHeight(const FString &InStrValue);
	virtual bool SetShapeHeight(float InIntValue);
	float GetShapeHeight();
	const TSharedPtr<FShapeAttribute>& GetShapeHeightAttri() const;

	void ChangeShapeposXAttriType(EShapeAttributeType InType);
	virtual bool SetShapePosX(const FString &InStrValue);
	virtual bool SetShapePosX(float InIntValue);
	float GetShapePosX();
	const TSharedPtr<FShapeAttribute>& GetShapePosXAttri() const;
	/** 获取型录沿X轴左边的位置，以mm为单位 */
	virtual float GetShapePosLeftSide();
	/** 获取型录沿X轴右边的位置，以mm为单位 */
	virtual float GetShapePosRightSide();
	/** 设置型录沿X轴左边的位置，以mm为单位 */
	virtual bool SetShapePosLeftSide(float InValue);
	/** 设置型录沿X轴右边的位置，以mm为单位 */
	virtual bool SetShapePosRightSide(float InValue);

	void ChangeShapePosYAttriType(EShapeAttributeType InType);
	virtual bool SetShapePosY(const FString &InStrValue);
	virtual bool SetShapePosY(float InIntValue);
	float GetShapePosY();
	const TSharedPtr<FShapeAttribute>& GetShapePosYAttri() const;
	/** 获取型录沿Y轴背后的位置，以mm为单位 */
	virtual float GetShapePosBackSide();
	/** 获取型录沿Y轴面前的位置，以mm为单位 */
	virtual float GetShapePosFrontSide();
	/** 设置型录沿Y轴背后的位置，以mm为单位 */
	virtual bool SetShapePosBackSide(float InValue);
	/** 设置型录沿Y轴面前的位置，以mm为单位 */
	virtual bool SetShapePosFrontSide(float InValue);

	void ChangeShapePosZAttriType(EShapeAttributeType InType);
	virtual bool SetShapePosZ(const FString &InStrValue);
	virtual bool SetShapePosZ(float InIntValue);
	float GetShapePosZ();
	const TSharedPtr<FShapeAttribute>& GetShapePosZAttri() const;
	/** 获取型录沿Z轴底部的位置，以mm为单位 */
	virtual float GetShapePosBottomSide();
	/** 获取型录沿Z轴顶部的位置，以mm为单位 */
	virtual float GetShapePosTopSide();
	/** 设置型录沿Z轴底部的位置，以mm为单位 */
	virtual bool SetShapePosBottomSide(float InValue);
	/** 设置型录沿Z轴顶部的位置，以mm为单位 */
	virtual bool SetShapePosTopSide(float InValue);

	void ChangeShapeRotXAttriType(EShapeAttributeType InType);
	bool SetShapeRotX(const FString &InStrValue);
	bool SetShapeRotX(float InIntValue);
	float GetShapeRotX();
	const TSharedPtr<FShapeAttribute>& GetShapeRotXAttri() const;

	void ChangeShapeRotYAttriType(EShapeAttributeType InType);
	bool SetShapeRotY(const FString &InStrValue);
	bool SetShapeRotY(float InIntValue);
	float GetShapeRotY();
	const TSharedPtr<FShapeAttribute>& GetShapeRotYAttri() const;

	void ChangeShapeRotZAttriType(EShapeAttributeType InType);
	bool SetShapeRotZ(const FString &InStrValue);
	bool SetShapeRotZ(float InIntValue);
	float GetShapeRotZ();
	const TSharedPtr<FShapeAttribute>& GetShapeRotZAttri() const;


	/** 调整型录尺寸偏移量 */
	virtual bool ModifyShapeDimentionDelta(const FVector &InDelta);
	/** 调整型录位置偏移量 */
	bool ModifyShapePositionDelta(const FVector &InDelta);

	/** 父级型录 */
	FORCEINLINE void SetParentShape(FSCTShape* InShape) { ParentShape = InShape; }
	FORCEINLINE FSCTShape* GetParentShape() const { return ParentShape; }
	/** 子级型录 */
	int32 GetChildShapeIndex(FSCTShape* CurShape);
	TSharedPtr<FSCTShape> GetChildShape(FSCTShape* CurShape);
	void AddChildShape(const TSharedPtr<FSCTShape> &ChildShape);
	void InsertChildShape(FSCTShape* CurShape, const TSharedPtr<FSCTShape> &ChildShape);
	void RemoveChildShape(const TSharedPtr<FSCTShape> &ChildShape);
	const TArray<TSharedPtr<FSCTShape>>& GetChildrenShapes() const;

public:
	/** 获取型录Actor */
	ASCTShapeActor* GetShapeActor() const;
	/** 获取型录外包框Actor */
	ASCTBoundingBoxActor* GetBoundingBoxActor() const;
	ASCTWireframeActor* GetWireframeActor() const;

	/** 型录外包框 */
	ASCTBoundingBoxActor* SpawnBoundingBoxActor();
	ASCTWireframeActor* SpawnWireFrameActor();
	void SetBoundingBoxActorHidden(bool bHidden);
	void SetWireFrameActorHidden(bool bHidden);

	/** 设置型录显示状态 */
	bool SetShapeHoverFlag(bool bFlag);
	bool SetShapeSelectFlag(bool bFlag);
	bool SetShapeAbsorbFlag(bool bFlag);
	bool SetShapeConflictFlag(bool bFlag);
	void SetShapeShowStatus(int32 InStatus);
	int32 GetShapeShowStatus();
	void SetWireFrameActorColor(const FLinearColor &InColor);

	/** 更新型录尺寸和位置 */
	void UpdateShapeDimentionPosition();
	/** 更新相关联的所有Actor信息 */
	//InType: 1-Dimension;2-Position;3-Rotation;4-edgeMaterial
	void UpdateAssociatedActors(int32 InType);
	/** 更新子级型录信息 */
	void UpdateChileShapes(int32 InType);

	/** 型录的FileJson是否已经下载解析 */
	bool IsFileJsonDownloadParsed() const;

public:
	/** 获取父级型录属性值 */
	FAny* OnPValFunc(const FString& ValName);
	/** 获取兄弟型录属性值 */
	FAny* OnFindEntValue(int32 ShapeType, int64 ShapeId, const FString& ValName);
	/** 获取自己的属性值 */
	FAny* OnFindOwnValFunc(const FString& ValName);
public:
	/** 获取GUID */
	const FGuid & GetShapeGuid() const { return ShapeGuid; }
	FGuid & GetShapeGuid()  { return ShapeGuid; }
	const FGuid & GetShapeCopyFromGuid() const { return ShapeCopyFromGuid; }
	FGuid & GetShapeCopyFromGuid() { return ShapeCopyFromGuid; }
	void ModifyShapeGuid(const FGuid & InGuid) { ShapeGuid = InGuid; }

public:
	const TMap<FGuid, FTag> & GetShapeTagPools() const { return TagPools; }
	TMap<FGuid, FTag> & GetShapeTagPools() { return TagPools; }			
protected:
	EShapeType ShapeType;         //型录类型
	EProductStatus ProductStatus; //产品状态
	bool bAbnormal = false;       //产品异常标识
	int32 ShapeCategory;		  //型录内置子类型
	int64 ShapeId;                //型录ID(数据库标识，并由后台赋值)
	FString ShapeName;            //型录名称
	FString ShapeCode;            //型录编码
	FString ThumbnailUrl;         //缩略图URL
	FString SystemName;           //系统名称
	FString JsonFileUrl;          //Json文件URL
	FString UpdateTime;           //最后修改时间

	//属性信息
	TSharedPtr<FShapeAttribute> ShowCondition;

	TSharedPtr<FShapeAttribute> Width;
	TSharedPtr<FShapeAttribute> Depth;
	TSharedPtr<FShapeAttribute> Height;

	TSharedPtr<FShapeAttribute> PositionX;
	TSharedPtr<FShapeAttribute> PositionY;
	TSharedPtr<FShapeAttribute> PositionZ;

	TSharedPtr<FShapeAttribute> RotationX;
	TSharedPtr<FShapeAttribute> RotationY;
	TSharedPtr<FShapeAttribute> RotationZ;
    
	//父级型录
	FSCTShape* ParentShape;
	//子级型录
	TArray<TSharedPtr<FSCTShape>> ChildrenShapes;

	//型录Actor(用于显示)
	ASCTShapeActor* ShapeActor = nullptr;
	//型录外包框Actor
	ASCTBoundingBoxActor* BoundingBoxActor = nullptr;
	ASCTWireframeActor* WireFrameActor = nullptr;

	//型录显示状态 0:null; 1:Alignment; 2：hover; 3:select; 4:Conflict
	//注意：型录状态有优先级，值越大优先级越高
	int32 CurrentShowStatus = 0;

	//标识型录是否已经解析
	bool bJsonFileParsed;
	FGuid ShapeGuid = FGuid::NewGuid();
	FGuid ShapeCopyFromGuid;
	TMap<FGuid, FTag> TagPools;
};

