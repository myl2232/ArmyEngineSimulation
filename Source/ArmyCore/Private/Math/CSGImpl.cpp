#include "CSGInterface.h"

namespace CSG
{
    enum class EPolygonType
    {
        PT_COPLANAR = 0,
        PT_FRONT,
        PT_BACK,
        PT_SPANNING
    };

    void FCSGPolygon::Flip()
    {
        int32 NumVertices = Vertices.Num();

        TArray<FCSGVertex> ReverseVertices;
        ReverseVertices.SetNumUninitialized(NumVertices);
        for (int32 i = 0; i < NumVertices; ++i)
        {
            //Vertices[i].Flip();
            ReverseVertices[NumVertices - i - 1] = MoveTemp(Vertices[i]);
        }
        Vertices = MoveTemp(ReverseVertices);
    }

    void FCSGNode::Build(const TArray<FCSGPolygon> &InPolygons)
    {
        // check(InPolygons.Num() > 0);
        if (InPolygons.Num() == 0)
            return;

        if (SplitPlane == nullptr)
        {
            SplitPlane = new FPlane;
            *SplitPlane = InPolygons[0].Plane;
        }
        TArray<FCSGPolygon> FrontPolygons, BackPolygons;
        for (const auto &Polygon : InPolygons)
            SplitPolygon(Polygon, MyPolygons, MyPolygons, FrontPolygons, BackPolygons);
        if (FrontPolygons.Num() > 0)
        {
            if (Front == nullptr)
                Front = new FCSGNode;
            Front->Build(FrontPolygons);
        }
        if (BackPolygons.Num() > 0)
        {
            if (Back == nullptr)
                Back = new FCSGNode;
            Back->Build(BackPolygons);
        }
    }

    void FCSGNode::ClipTo(FCSGNode *InOtherNode)
    {
        MyPolygons = InOtherNode->ClipPolygons(MyPolygons);
        if (Front != nullptr)
            Front->ClipTo(InOtherNode);
        if (Back != nullptr)
            Back->ClipTo(InOtherNode);
    }

    void FCSGNode::Invert()
    {
        for (auto &MyPolygon : MyPolygons)
            MyPolygon.Flip();
        *SplitPlane = SplitPlane->Flip();
        if (Front != nullptr)
            Front->Invert();
        if (Back != nullptr)
            Back->Invert();
        FCSGNode *Temp = Front;
        Front = Back;
        Back = Temp;
    }

    TArray<FCSGPolygon> FCSGNode::GetFlattenedPolygons()
    {
        TArray<FCSGPolygon> FlattenedPolygons(MyPolygons);
        if (Front != nullptr)
            FlattenedPolygons.Append(Front->GetFlattenedPolygons());
        if (Back != nullptr)
            FlattenedPolygons.Append(Back->GetFlattenedPolygons());
        return MoveTemp(FlattenedPolygons);
    }

    void FCSGNode::SplitPolygon(const FCSGPolygon &InPolygon, 
        TArray<FCSGPolygon> &OutCoplanarFrontPolygons, 
        TArray<FCSGPolygon> &OutCoplanarBackPolygons, 
        TArray<FCSGPolygon> &OutFrontPolygons, 
        TArray<FCSGPolygon> &OutBackPolygons)
    {
        int32 NumVertices = InPolygon.Vertices.Num();
        int32 PolygonType = 0;
        TArray<EPolygonType> Types;
        for (int32 i = 0; i < NumVertices; ++i)
        {
            float D = SplitPlane->PlaneDot(InPolygon.Vertices[i].P);
            EPolygonType Type = D < -KINDA_SMALL_NUMBER ? 
                EPolygonType::PT_BACK : 
                (D > KINDA_SMALL_NUMBER ? EPolygonType::PT_FRONT : EPolygonType::PT_COPLANAR);
            PolygonType |= (int32)Type;
            Types.Emplace(Type);
        }

        switch (PolygonType)
        {
            case EPolygonType::PT_COPLANAR:
                (SplitPlane->operator | (InPolygon.Plane) > 0.0f ? OutCoplanarFrontPolygons : OutCoplanarBackPolygons).Emplace(InPolygon);
                break;
            case EPolygonType::PT_FRONT:
                OutFrontPolygons.Emplace(InPolygon);
                break;
            case EPolygonType::PT_BACK:
                OutBackPolygons.Emplace(InPolygon);
                break;
            case EPolygonType::PT_SPANNING:
            {
                TArray<FCSGVertex> FrontVertices, BackVertices;
                for (int32 i = 0; i < NumVertices; ++i)
                {
                    int32 j = (i + 1) % NumVertices;
                    EPolygonType CurrentType = Types[i];
                    EPolygonType NextType = Types[j];
                    if (CurrentType != EPolygonType::PT_BACK) // 如果点不在平面背侧，就推入到正侧列表中
                        FrontVertices.Emplace(InPolygon.Vertices[i]);
                    if (CurrentType != EPolygonType::PT_FRONT) // 如果点不在平面正侧，就推入到背侧列表中，如果是平面上的点正侧背侧列表都会有
                        BackVertices.Emplace(InPolygon.Vertices[i]);
                    if (((int32)CurrentType | (int32)NextType) == (int32)EPolygonType::PT_SPANNING) // 前后两点跨在平面两侧
                    {
                        // 找到两点连线和平面的交点
                        const FPlane &Plane = *SplitPlane;
                        const FVector &Point1 = InPolygon.Vertices[i].P;
                        const FVector &Point2 = InPolygon.Vertices[j].P;
                        const FVector2D &Point1UV = InPolygon.Vertices[i].UV;
                        const FVector2D &Point2UV = InPolygon.Vertices[j].UV;
                        //FVector Intersection = FMath::LinePlaneIntersection(InPolygon.Vertices[i].P, InPolygon.Vertices[j].P, *SplitPlane);
                        float t = (Plane.W - (Point1|Plane)) / ((Point2 - Point1)|Plane);
                        FVector Intersection = Point1 + (Point2 - Point1) * t;
                        FVector2D IntersectionUV = Point1UV + (Point2UV - Point1UV) * t;
                        FrontVertices.Emplace(FCSGVertex(Intersection, IntersectionUV));
                        BackVertices.Push(FCSGVertex(Intersection, IntersectionUV));
                    }
                }
                if (FrontVertices.Num() >= 3)
                    OutFrontPolygons.Emplace(FCSGPolygon(FrontVertices));
                if (BackVertices.Num() >= 3)
                    OutBackPolygons.Emplace(FCSGPolygon(BackVertices));
                break;
            }
        }
    }

    TArray<FCSGPolygon> FCSGNode::ClipPolygons(const TArray<FCSGPolygon> &InPolygonsToClip)
    {
        if (SplitPlane == nullptr)
            return MyPolygons;
        TArray<FCSGPolygon> FrontPolygons, BackPolygons;
        for (const auto &PolygonToClip : InPolygonsToClip)
            SplitPolygon(PolygonToClip, FrontPolygons, BackPolygons, FrontPolygons, BackPolygons);
        if (Front != nullptr)
            FrontPolygons = Front->ClipPolygons(FrontPolygons);
        if (Back != nullptr)
            BackPolygons = Back->ClipPolygons(BackPolygons);
        else   
            BackPolygons.Empty();
        FrontPolygons.Append(BackPolygons);
        return MoveTemp(FrontPolygons);
    }

    FCSGNode* MakeCSGNode(const TArray<FCSGPolygon> &InPolygons)
    {
        FCSGNode *Node = new FCSGNode;
        Node->Build(InPolygons);
        return Node;
    }

    void DestroyCSGNode(FCSGNode *InNodeToDestroy)
    {
        if (InNodeToDestroy != nullptr)
            delete InNodeToDestroy;
    }

    void Subtract(FCSGNode *InNodeSrc, FCSGNode *InNodeDest)
    {
        // A - B = ~(~A | B)
        // ~A
        InNodeSrc->Invert();
        // ~A | B
        InNodeSrc->ClipTo(InNodeDest);
        InNodeDest->ClipTo(InNodeSrc);
        InNodeDest->Invert();
        InNodeDest->ClipTo(InNodeSrc);
        InNodeDest->Invert();
        InNodeSrc->Build(InNodeDest->GetFlattenedPolygons());
        // ~(~A | B)
        InNodeSrc->Invert();
    }

    void Intersect(FCSGNode *InNodeSrc, FCSGNode *InNodeDest)
    {
        InNodeSrc->Invert();
        InNodeDest->ClipTo(InNodeSrc);
        InNodeDest->Invert();
        InNodeSrc->ClipTo(InNodeDest);
        InNodeDest->ClipTo(InNodeSrc);
        InNodeSrc->Build(InNodeDest->GetFlattenedPolygons());
        InNodeSrc->Invert();
    }

    void ToCSGVertices(FCSGNode *InNode, TArray<FCSGVertex> &OutVertices, TArray<uint16> &OutIndices)
    {
        OutVertices.Empty();
        OutIndices.Empty();

        TArray<FCSGPolygon> AllPolygons = InNode->GetFlattenedPolygons();
        int32 VertexIndexOffset = 0;
        for (const auto &Polygon : AllPolygons)
        {
            OutVertices.Append(Polygon.Vertices);
            int32 NumVerticesInPolygon = Polygon.Vertices.Num();
            switch (NumVerticesInPolygon)
            {
                case 3:
                {
                    OutIndices.Emplace(VertexIndexOffset);
                    OutIndices.Emplace(VertexIndexOffset + 2);
                    OutIndices.Emplace(VertexIndexOffset + 1);
                    break;
                }
                case 4:
                {
                    OutIndices.Emplace(VertexIndexOffset);
                    OutIndices.Emplace(VertexIndexOffset + 2);
                    OutIndices.Emplace(VertexIndexOffset + 1);
                    OutIndices.Emplace(VertexIndexOffset);
                    OutIndices.Emplace(VertexIndexOffset + 3);
                    OutIndices.Emplace(VertexIndexOffset + 2);
                    break;
                }
            }
            VertexIndexOffset += NumVerticesInPolygon;
        }
    }
}