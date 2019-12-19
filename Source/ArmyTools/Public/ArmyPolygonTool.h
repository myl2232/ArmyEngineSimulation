/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File ArmyPolygonTool.h
* @Description 多边形工具类
*
* @Author 刘克祥
* @Date 2018年12月19日
* @Version 1.0
*/
#pragma once
#include "CoreMinimal.h"

// 节点类
struct "FArmyNode
{
	int index;
	bool bExclude;
	TSharedPtr<"FArmyNode> parentNode;
	TArray<TSharedPtr<"FArmyNode>> connectNodes;
	FVector point;

	"FArmyNode(FVector point,int index)
	{ 
		this->index = index; 
		this->point = point;
	}
};

// 
class ARMYTOOLS_API "FArmyPolygonTool
{
public:
	"FArmyPolygonTool();
	~"FArmyPolygonTool();
	// 添加节点
	void AddNode(FVector point);
	// 添加边
	bool AddEdge(FVector startPoint, FVector endPoint);
	// 检索单个多边形
	TArray<FVector> SearchSinglePolygon();

protected:
	// 获得边对应的key值
	FString GetEdgeKey(int startNodeIndex, int endNodeIndex);
	// 获得节点的索引
	int GetNodeIndex(FVector point);
	// 基于某个节点开始裁剪叶子节点
	void CropLeafNodes(TSharedPtr<"FArmyNode> node);
	// 检索单个多边形
	TArray<FVector> SearchSinglePolygon(TSharedPtr<"FArmyNode> startNode);


private:
	// 节点
	TArray<TSharedPtr<"FArmyNode>> nodes;
	// 边对应的map
	TMap<FString, FString> edgesMap;

};
