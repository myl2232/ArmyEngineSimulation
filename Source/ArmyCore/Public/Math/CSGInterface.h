/*
    几何体布尔运算
    author: 欧石楠
    date: 2019-3-27
*/
#pragma once
#include "CoreMinimal.h"

namespace CSG
{
    struct FCSGVertex
    {
        FVector P;
        FVector2D UV;
        FCSGVertex() {}
        FCSGVertex(const FVector &InP) : P(InP) { UV.Set(0.0f, 0.0f); }
        FCSGVertex(const FVector &InP, const FVector2D &InUV) : P(InP), UV(InUV) {}
    };

    struct FCSGPolygon
    {
        TArray<FCSGVertex> Vertices;
        FPlane Plane;
        // 传入顶点数组为顺时针，目前只支持每个多边形为三个点或四个点
        FCSGPolygon(const TArray<FCSGVertex> &InVertices)
         : Vertices(InVertices)
         , Plane(FPlane(InVertices[0].P, InVertices[1].P, InVertices[2].P)) {}
        void Flip();
    };

    class FCSGNode
    {
    public:
        FCSGNode() : Front(nullptr), Back(nullptr), SplitPlane(nullptr) {}
        void Build(const TArray<FCSGPolygon> &InPolygons);
        void ClipTo(FCSGNode *InOtherNode);
        void Invert();
        ARMYCORE_API TArray<FCSGPolygon> GetFlattenedPolygons();

    private:
        void SplitPolygon(const FCSGPolygon &InPolygon, 
            TArray<FCSGPolygon> &OutCoplanarFrontPolygons, 
            TArray<FCSGPolygon> &OutCoplanarBackPolygons, 
            TArray<FCSGPolygon> &OutFrontPolygons, 
            TArray<FCSGPolygon> &OutBackPolygons);
        TArray<FCSGPolygon> ClipPolygons(const TArray<FCSGPolygon> &InPolygonsToClip);

    private:
        FCSGNode *Front;
        FCSGNode *Back;
        FPlane *SplitPlane;
        TArray<FCSGPolygon> MyPolygons;
    };

    // 创建CSGNode
    ARMYCORE_API FCSGNode* MakeCSGNode(const TArray<FCSGPolygon> &InPolygons);
    // 销毁CSGNode
    ARMYCORE_API void DestroyCSGNode(FCSGNode *InNodeToDestroy);
    // 几何体减法运算，Src - Dest，运算结果保存在InNodeSrc中
    ARMYCORE_API void Subtract(FCSGNode *InNodeSrc, FCSGNode *InNodeDest);
    // 几何体相交运算, Src intersect Dest，运算结果保存在InNodeSrc中
    ARMYCORE_API void Intersect(FCSGNode *InNodeSrc, FCSGNode *InNodeDest);
    // 转换为三角网格的顶点和索引
    ARMYCORE_API void ToCSGVertices(FCSGNode *InNode, TArray<FCSGVertex> &OutVertices, TArray<uint16> &OutIndices);
}