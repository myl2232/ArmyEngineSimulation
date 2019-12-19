/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File BoardMaterialManager.h
* @Description 型录数据Shape管理
*
* @Author 赵志强
* @Date 2018年5月15日
* @Version 1.0
*/
#pragma once

#include "Job.h"
#include "SCTShape.h"

class FSCTShape;
class FBoardShape;
class FAccessoryShape;
//class FMaterialManager;
class FMeshManager;

// 当前型录管理类
class SCTSHAPEMODULE_API FSCTShapeManager
{
private:
	FSCTShapeManager();

public:
	static const TSharedRef<FSCTShapeManager>& Get();

public:
	/** 拷贝复制创建新的型录 */
	TSharedPtr<FSCTShape> CopyShapeToNew(const TSharedPtr<FSCTShape>& InShape);

	/** 解析JSon文件创建型录 */
	TSharedPtr<FSCTShape> ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject);

	/** 保存型录为Json字符串 */
	FString SaveShapeToJson(FSCTShape* InShape);

public:
	/** 解析子级型录 */
	void ParseChildrenShapes(const TSharedPtr<FJsonObject>& InJsonObject);
	/** 添加子级型录 */
	void AddChildShape(TSharedPtr<FSCTShape> InShape);
	/** 统计指定型录的所有嵌套层级的子型录 */
	void CountInheritChildShapes(FSCTShape* InShape);
	/** 获取指定类型子级型录 */
	TMap<int64, TSharedPtr<FSCTShape>>& GetChildrenShapesByType(int32 InType);
	/** 获取指定类型和ID的子级型录 */
	TSharedPtr<FSCTShape> GetChildShapeByTypeAndID(int32 InType, int64 InID);
	/** 查询指定类型和ID的子级型录 */
	bool FindChildShapeByTypeAndID(int32 InType, int64 InID);
	/** 清空本地缓存子级型录 */
	void ClearAllChildrenShapes();
		

public:
	TSharedPtr<FMeshManager> GetStaticMeshManager();

private:
	//板件材质缓存管理
	//TSharedPtr<FMaterialManager> MaterialManager;
	//模型网格缓存管理
	TSharedPtr<FMeshManager> StaticMeshManager;

	//本地缓存子级型录
	//打开或保存顶层型录使用的临时变量，用完后需要清空
	TArray<TMap<int64, TSharedPtr<FSCTShape>>> ChildrenShapeLists;
};

