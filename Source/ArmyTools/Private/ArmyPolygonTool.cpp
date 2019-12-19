/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File ArmyPolygonTool.cpp
* @Description 多边形工具类
*
* @Author 刘克祥
* @Date 2018年12月19日
* @Version 1.0
*/
#include "ArmyPolygonTool.h"
#include "ArmyMath.h"

"FArmyPolygonTool::"FArmyPolygonTool()
{
}

"FArmyPolygonTool::~"FArmyPolygonTool()
{
}

// 添加节点
void "FArmyPolygonTool::AddNode(FVector point)
{
	"FArmyNode *test = new "FArmyNode(point, nodes.Num());
	TSharedPtr<"FArmyNode> node = MakeShareable(new "FArmyNode(point, nodes.Num()));
	nodes.Push(node);
}

// 添加边
bool "FArmyPolygonTool::AddEdge(FVector startPoint, FVector endPoint)
{
	int startIndex = GetNodeIndex(startPoint);
	int endIndex = GetNodeIndex(endPoint);

	if (startIndex == -1 || endIndex == -1 || endIndex == startIndex)
	{
		return false;
	}

	// 正向添加
	FString keyValue = GetEdgeKey(startIndex, endIndex);
	if (!edgesMap.Contains(keyValue))
	{
		TSharedPtr<"FArmyNode> tmpNode = nodes[startIndex];
		tmpNode->connectNodes.Add(nodes[endIndex]);
		edgesMap.Add(keyValue, "1");
	}

	// 反向添加
	keyValue = GetEdgeKey(endIndex, startIndex);
	if (!edgesMap.Contains(keyValue))
	{
		TSharedPtr<"FArmyNode> tmpNode = nodes[endIndex];
		tmpNode->connectNodes.Add(nodes[startIndex]);
		edgesMap.Add(keyValue, "1");
	}

	return true;
}

// 检索单个多边形
TArray<FVector> "FArmyPolygonTool::SearchSinglePolygon()
{
	TArray<FVector> polygonPoints;
	if (nodes.Num() < 1)
	{
		return polygonPoints;
	}
	else
	{
		return SearchSinglePolygon(nodes[0]);
	}
}

// 获得边对应的key值
FString "FArmyPolygonTool::GetEdgeKey(int startNodeIndex, int endNodeIndex)
{
	return FString::FromInt(startNodeIndex) + "-" + FString::FromInt(endNodeIndex);
}
// 获得节点的索引
int "FArmyPolygonTool::GetNodeIndex(FVector point)
{
	for (auto node : nodes)
	{
		if (node->point == point)
		{
			return node->index;
		}
	}

	return -1;
}

// 检索单个多边形
TArray<FVector> "FArmyPolygonTool::SearchSinglePolygon(TSharedPtr<"FArmyNode> startNode)
{
	TArray<FVector> resultVectors;
	TSharedPtr<"FArmyNode> currentNode;
	TSharedPtr<"FArmyNode> nextNode;

	for (int i = 0; i < startNode->connectNodes.Num(); i++)
	{
		currentNode = startNode->connectNodes[i];
		currentNode->parentNode = startNode;
		while (true)
		{
			FString key = GetEdgeKey(currentNode->parentNode->index, currentNode->index);
			if (!edgesMap.Contains(key))
			{
				break;
			}

			if (currentNode->index == startNode->index)
			{
				break;
			}

			nextNode = nullptr;
			FVector parentToCurrent = currentNode->point - currentNode->parentNode->point;
			float minAngle = 9999;
			for (int j = 0; j < currentNode->connectNodes.Num(); j++)
			{
				TSharedPtr<"FArmyNode> tmpNode1 = currentNode->connectNodes[j];
				TSharedPtr<"FArmyNode> tmpNode2 = currentNode->parentNode;
				if (tmpNode1->index == tmpNode2->index)
				{
					continue;
				}

				FVector currentToNext = tmpNode1->point - currentNode->point;
				float angle = parentToCurrent.CosineAngle2D(currentToNext);
				if (angle < 0)
				{
					angle = angle + PI * 2;
				}
				if (angle < minAngle)
				{
					minAngle = angle;
					nextNode = currentNode->connectNodes[j];
				}
			}
			
			if (!nextNode.IsValid())
			{
				break;
			}

			// 如果nextNode在此轮中已经遍历过则退出
			if (nextNode->parentNode.IsValid())
			{
				break;
			}

			// 进行下一轮查找
			nextNode->parentNode = currentNode;
			currentNode = nextNode;
		}

		// 找到了多边形
		if (currentNode->index == startNode->index)
		{
			TSharedPtr<"FArmyNode> tmpNode = currentNode;
			while (true)
			{
				resultVectors.Add(tmpNode->point);
				if (tmpNode->parentNode->index == startNode->index)
				{
					break;
				}
				if (!(tmpNode->parentNode.IsValid()))
				{
					break;
				}
				tmpNode = tmpNode->parentNode;
			}

			return resultVectors;
		}
	}
	return resultVectors;
}