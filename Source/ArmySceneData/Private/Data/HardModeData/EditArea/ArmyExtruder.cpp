#include "ArmyExtruder.h"
#include "ArmyExtrusionActor.h"
#include "ArmySceneData.h"
#include "ArmyProceduralMeshComponent.h"
#include "ArmyResourceModule.h"
#include "ArmyViewportClient.h"
#include "ArmyMath.h"
#include "ArmyRoomSpaceArea.h"
#include "ArmyConstructionManager.h"

void FArmyExtruder::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    JsonWriter->WriteArrayStart("paths");
    for (auto& It : Paths)
    {
        JsonWriter->WriteObjectStart();
        It.SerializeToJson(JsonWriter);
        JsonWriter->WriteObjectEnd();
    }
    JsonWriter->WriteArrayEnd();
}

void FArmyExtruder::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
    const TArray< TSharedPtr<FJsonValue> > PathsArray = InJsonData->GetArrayField("paths");
    for (auto& It : PathsArray)
    {
        FArmyExtrusionPath Path;
        Path.Deserialization(It->AsObject());
        Paths.Add(Path);
    }
}

void FArmyExtruder::Generate()
{
    if (Paths.Num() == 0)
    {
        return;
    }

    // @欧石楠 生成放样模型前先销毁之前创建的
    for (auto& It : ActorMap)
    {
        if (It.Key && It.Key->IsValidLowLevel())
        {
            TArray<AActor*> AttachedActors;
            It.Key->GetAttachedActors(AttachedActors);
            for (auto It2 : AttachedActors)
            {
                It2->Destroy();
            }

            FArmySceneData::Get()->DeleteExtrusionConstructionItemData(It.Key->UniqueCodeExtrusion);
            It.Key->Destroy();
        }
    }
    ActorMap.Reset();

    TArray<FArmyExtrusionPath> SplitedPaths;
    SplitPaths(Paths, SplitedPaths);

    for (int32 i = 0; i < SplitedPaths.Num(); ++i)
    {
        SplitedPaths[i].Index = i;
        GenerateActor(SplitedPaths[i]);
    }
}

void FArmyExtruder::Clear()
{
    for (auto& It : ActorMap)
    {
        if (It.Key && It.Key->IsValidLowLevel())
        {
            FArmySceneData::Get()->DeleteExtrusionConstructionItemData(It.Key->UniqueCodeExtrusion);
            It.Key->Destroy();
        }
    }

    Paths.Reset();
    ActorMap.Reset();
}

void FArmyExtruder::DeleteActor(AArmyExtrusionActor* InActor)
{
    if (InActor && InActor->IsValidLowLevel())
    {
        FArmyExtrusionPath* DeletedPath = ActorMap.Find(InActor);
        if (DeletedPath)
        {
            FVector Vertex = DeletedPath->Vertexes[1];

            int32 i, j;
            for (i = 0; i < Paths.Num(); ++i)
            {
                TArray<FVector> Vertexes = Paths[i].Vertexes;
                bool bClosed = Paths[i].bClosed;

                for (j = 0; j < Vertexes.Num(); ++j)
                {
                    if (Vertexes[j] == Vertex)
                    {
                        TArray<FVector> Left, Right;
                        TSharedPtr<FContentItem> LeftContentItem, RightContentItem;
                        int32 LeftIndex = 0;
                        int32 RightIndex = 0;
                        if (SurfaceArea.IsValid())
                        {
                            int32 VertexNum = SurfaceArea->OutArea->Vertices.Num();
                            LeftIndex = DeletedPath->Index - 1 >= 0 ? DeletedPath->Index - 1 : DeletedPath->Index - 1 + VertexNum;
                            RightIndex = DeletedPath->Index + 1 < VertexNum ? DeletedPath->Index + 1 : DeletedPath->Index + 1 - VertexNum;
                        }

                        // @欧石楠 因为删除某一段踢脚线会对相邻的踢脚线造成影响，所以也需要销毁掉
                        if (j - 1 >= 0)
                        {
                            LeftContentItem = DeleteActorByVertex(Vertexes[j - 1]);
                        }
                        else if (bClosed)
                        {
                            LeftContentItem = DeleteActorByVertex(Vertexes.Last(0));
                        }

                        if (j + 1 < Vertexes.Num())
                        {
                            RightContentItem = DeleteActorByVertex(Vertexes[j + 1]);
                        }
                        else if (bClosed)
                        {
                            RightContentItem = DeleteActorByVertex(Vertexes[0]);
                        }

                        Paths.RemoveAt(i);

                        // @欧石楠 根据删除的踢脚线，将整段踢脚线打断成左右两截
                        if (bClosed)
                        {
                            Left = Vertexes;
                            while (j + 1 < Left.Num())
                            {
                                FVector Last = Left.Last(0);
                                Left.Remove(Last);
                                Left.Insert(Last, 0);
                                ++j;
                            }

                            Right = Left;

                            if (Left.Num() > 1)
                            {
                                Paths.Add(FArmyExtrusionPath(Left, false, LeftContentItem));
                            }
                        }
                        else
                        {
                            for (int32 k = 0; k < Vertexes.Num(); ++k)
                            {
                                if (k <= j)
                                {
                                    Left.Add(Vertexes[k]);
                                }
                                else
                                {
                                    Right.Add(Vertexes[k]);
                                }
                            }

                            if (Left.Num() > 1)
                            {
                                Paths.Add(FArmyExtrusionPath(Left, false, LeftContentItem));
                            }
                            if (Right.Num() > 1)
                            {
                                Paths.Add(FArmyExtrusionPath(Right, false, RightContentItem));
                            }
                        }
                        
                        // @欧石楠 重新生成相邻的两端踢脚线
                        while (Left.Num() > 3)
                        {
                            Left.RemoveAt(0);
                        }
                        while (Right.Num() > 3)
                        {
                            Right.RemoveAt(Right.Num() - 1);
                        }

                        if (Left.Num() > 1)
                        {
                            TArray<FArmyExtrusionPath> LeftPaths;
                            LeftPaths.Add(FArmyExtrusionPath(Left, false, LeftContentItem));
                            TArray<FArmyExtrusionPath> NewPaths;
                            SplitPaths(LeftPaths, NewPaths, true, true);
                            NewPaths[0].Index = LeftIndex;
                            GenerateRelatedActor(NewPaths[0], true);
                        }

                        if (Right.Num() > 1)
                        {
                            TArray<FArmyExtrusionPath> RightPaths;
                            RightPaths.Add(FArmyExtrusionPath(Right, false, RightContentItem));
                            TArray<FArmyExtrusionPath> NewPaths;
                            SplitPaths(RightPaths, NewPaths, true, false);
                            NewPaths[0].Index = RightIndex;
                            GenerateRelatedActor(NewPaths[0], false);
                        }

                        ActorMap.Remove(InActor);
                        InActor->Destroy();

                        return;
                    }
                }
            }
        }
    }
}

TSharedPtr<FContentItem> FArmyExtruder::DeleteActorByVertex(const FVector& InVertex)
{
    TSharedPtr<FContentItem> Result = nullptr;

    for (auto& It : ActorMap)
    {
        if (It.Value.Vertexes[1] == InVertex)
        {
            Result = It.Value.ContentItem;
            It.Key->Destroy();
            ActorMap.Remove(It.Key);
            break;
        }
    }

    return Result;
}

void FArmyExtruder::SplitPaths(TArray<FArmyExtrusionPath>& InPaths, TArray<FArmyExtrusionPath>& OutPaths, bool bOneSegment /*= false*/, bool bLeft /*= true*/)
{
    // @欧石楠 生成单段踢脚线的逻辑
    // @欧石楠 1. 计算房屋的轮廓（需考虑柱子、烟道、门窗等物体的轮廓）
    // @欧石楠 2. 门窗会将整体的房屋轮廓打断成几截，这种情况视为非闭合，没有被打断的情况视为闭合
    // @欧石楠 3. 针对一条或多条踢脚线路径进行打断，逐条计算
    // @欧石楠 4. 计算的逻辑是：一个4位的顶点数组Vertexes[4]，Vertexes[1] - Vertexes[2]是踢脚线的起点和终点，Vertexes[0]和Vertexes[3]是为了计算相接部位的踢脚线模型
    for (auto& It : InPaths)
    {
        TArray<FVector>& PathVertexes = It.Vertexes;

        if (It.bClosed)
        {
            for (int32 i = 0; i < PathVertexes.Num(); ++i)
            {
                FArmyExtrusionPath NewPath;
                NewPath.ContentItem = It.ContentItem;
                NewPath.bClosed = false;

                if (i == 0)
                {
                    NewPath.Vertexes.Add(PathVertexes.Last(0));
                    NewPath.Vertexes.Add(PathVertexes[0]);
                    NewPath.Vertexes.Add(PathVertexes[1]);
                    NewPath.Vertexes.Add(PathVertexes[2]);
                }
                else if (i == PathVertexes.Num() - 2)
                {
                    NewPath.Vertexes.Add(PathVertexes[i - 1]);
                    NewPath.Vertexes.Add(PathVertexes[i]);
                    NewPath.Vertexes.Add(PathVertexes[i + 1]);
                    NewPath.Vertexes.Add(PathVertexes[0]);
                }
                else if (i == PathVertexes.Num() - 1)
                {
                    NewPath.Vertexes.Add(PathVertexes[i - 1]);
                    NewPath.Vertexes.Add(PathVertexes[i]);
                    NewPath.Vertexes.Add(PathVertexes[0]);
                    NewPath.Vertexes.Add(PathVertexes[1]);
                }
                else
                {
                    NewPath.Vertexes.Add(PathVertexes[i - 1]);
                    NewPath.Vertexes.Add(PathVertexes[i]);
                    NewPath.Vertexes.Add(PathVertexes[i + 1]);
                    NewPath.Vertexes.Add(PathVertexes[i + 2]);
                }

                OutPaths.Add(NewPath);
            }
        }
        else
        {
            for (int32 i = 0; i < PathVertexes.Num() - 1; ++i)
            {
                FArmyExtrusionPath NewPath;
                NewPath.ContentItem = It.ContentItem;
                NewPath.bClosed = false;

                if (PathVertexes.Num() == 2)
                {
                    NewPath.Vertexes.Add(PathVertexes[0]);
                    NewPath.Vertexes.Add(PathVertexes[0]);
                    NewPath.Vertexes.Add(PathVertexes[1]);
                    NewPath.Vertexes.Add(PathVertexes[1]);
                }
                else if (bOneSegment) // @欧石楠 只生成一段的情况
                {
                    if (bLeft)
                    {
                        NewPath.Vertexes.Add(PathVertexes[0]);
                        NewPath.Vertexes.Add(PathVertexes[1]);
                        NewPath.Vertexes.Add(PathVertexes[2]);
                        NewPath.Vertexes.Add(PathVertexes[2]);
                    }
                    else
                    {
                        NewPath.Vertexes.Add(PathVertexes[0]);
                        NewPath.Vertexes.Add(PathVertexes[0]);
                        NewPath.Vertexes.Add(PathVertexes[1]);
                        NewPath.Vertexes.Add(PathVertexes[2]);
                    }

                    OutPaths.Add(NewPath);
                    break;
                }
                else
                {
                    if (i == 0)
                    {
                        NewPath.Vertexes.Add(PathVertexes[0]);
                        NewPath.Vertexes.Add(PathVertexes[0]);
                        NewPath.Vertexes.Add(PathVertexes[1]);
                        NewPath.Vertexes.Add(PathVertexes[2]);
                    }
                    else if (i == PathVertexes.Num() - 2)
                    {
                        NewPath.Vertexes.Add(PathVertexes[i - 1]);
                        NewPath.Vertexes.Add(PathVertexes[i]);
                        NewPath.Vertexes.Add(PathVertexes.Last(0));
                        NewPath.Vertexes.Add(PathVertexes.Last(0));
                    }
                    else
                    {
                        NewPath.Vertexes.Add(PathVertexes[i - 1]);
                        NewPath.Vertexes.Add(PathVertexes[i]);
                        NewPath.Vertexes.Add(PathVertexes[i + 1]);
                        NewPath.Vertexes.Add(PathVertexes[i + 2]);
                    }
                }

                OutPaths.Add(NewPath);
            }
        }
    }
}

AArmyExtrusionActor* FArmyExtruder::GenerateActor(const FArmyExtrusionPath& Path)
{
    FString DXFFilePath = GetDXFFilePath(Path.ContentItem);
    if (DXFFilePath.Len() == 0)
    {
        return nullptr;
    }




    AArmyExtrusionActor* ResultActor = World->SpawnActor<AArmyExtrusionActor>(AArmyExtrusionActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
    ResultActor->MeshComponent->Rename(TEXT("SkirtLineComponent"));
    ResultActor->MeshComponent->bBuildStaticLighting = true;
    ResultActor->UniqueCodeExtrusion = GetUniqueCode();
    ResultActor->SetFolderPath(GetFolderPath());
    ResultActor->Tags.Add(XRActorTag::Immovable);
    ResultActor->Tags.Add(XRActorTag::CanNotDelete);
    ResultActor->dxfName = Path.ContentItem->Name;
    ResultActor->SetActorLabel(Path.ContentItem->Name);
    ResultActor->dxfLength = Path.ContentItem->ProObj->Length;
    ResultActor->dxfWidth = Path.ContentItem->ProObj->Width;
    ResultActor->dxfHeight = Path.ContentItem->ProObj->Height;
    ResultActor->GenerateSkitLineMeshFromFilePath(DXFFilePath, Path.Vertexes, NeedReverse(), Path.bClosed, true);
    ResultActor->SetSynID(Path.ContentItem->ID);

    if (SurfaceArea.IsValid())
    {
        ResultActor->AttachSurfaceArea = SurfaceArea;
    }
    if (RoomSpaceArea.IsValid())
    {
        ResultActor->AttachRoomSpaceArea = RoomSpaceArea;
    }

    UMaterialInterface* MI = FArmyResourceModule::Get().GetResourceManager()->CreateContentItemMaterial(Path.ContentItem);
    if (MI)
    {
        ResultActor->SetMaterial(MI);
    }

    ActorMap.Add(ResultActor, Path);


	//@郭子阳
	//所依赖面的ID
	FGuid SurfaceId;
	if (SurfaceArea.IsValid())
		{
		SurfaceId = SurfaceArea->GetUniqueID();
		}
		else if (RoomSpaceArea.IsValid())
		{
			SurfaceId = RoomSpaceArea->GetUniqueID();
		}
	//@郭子阳 转换老方案(V1.9之前)施工项
	TSharedPtr<FArmyConstructionItemInterface> temp = FArmySceneData::Get()->GetConstructionItemDataByExtrusionUniqueCode(GetUniqueCode());//加载
	if (temp.IsValid() && SurfaceId.IsValid())
	{
		if (SurfaceId.IsValid())
		{
			XRConstructionManager::Get()->SaveConstructionData(SurfaceId
				, ResultActor->GetConstructionParameter(), temp);
		}
		FArmySceneData::Get()->DeleteExtrusionConstructionItemData(GetUniqueCode());
	}
	//@郭子阳 获取施工项
	XRConstructionManager::Get()->TryToFindConstructionData(SurfaceId, ResultActor->GetConstructionParameter(), nullptr);


    return ResultActor;
}

AArmyExtrusionActor* FArmyExtruder::GenerateRelatedActor(FArmyExtrusionPath& Path, bool bLeft)
{
    return GenerateActor(Path);
}

FString FArmyExtruder::GetDXFFilePath(TSharedPtr<FContentItem> InContentItem)
{
    // @欧石楠 获取放样dxf文件路径
    FString Result = TEXT("");

    if (!InContentItem.IsValid())
    {
        return Result;
    }

    TArray< TSharedPtr<FResObj> > ResArr = InContentItem->GetResObjNoComponent();
    if (ResArr.Num() < 1)
    {
        return Result;
    }

    for (auto& It : ResArr)
    {
        if (It->ResourceType == HardDXF)
        {
            Result = It->FilePath;
            break;
        }
    }

    return Result;
}

const int32 FArmyExtruder::GetSaleID() const
{
    if (Paths.Num() > 0)
    {
        return Paths[0].ContentItem->ID;
    }

    return -1;
}

float FArmyExtruder::GetCircumference()
{
    float Result = 0.f;

    for (const auto& Path : Paths)
    {
        Result += FArmyMath::CircumferenceOfPointList(Path.Vertexes, Path.bClosed);
    }

    return Result;
}

FContentItemPtr FArmyExtruder::GetContentItem()
{
    FContentItemPtr Result = nullptr;

    if (Paths.Num() > 0)
    {
        Result = Paths[0].ContentItem;
    }

    return Result;
}

void FArmyExtruder::ApplyOffset(const FVector& InOffset)
{
    for (auto& Path : Paths)
    {
        for (auto& Vertex : Path.Vertexes)
        {
            Vertex += InOffset;
        }
    }

    Generate();
}

void FArmyExtruder::SetDistanceToFloor(const float InDistanceToFloor)
{
    for (auto& Path : Paths)
    {
        for (auto& Vertex : Path.Vertexes)
        {
            Vertex.Z = InDistanceToFloor;
        }
    }

    Generate();
}

void FArmyExtrusionPath::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    JsonWriter->WriteArrayStart("vertexes");
    for (auto& It : Vertexes)
    {
        JsonWriter->WriteValue(It.ToString());
    }
    JsonWriter->WriteArrayEnd();
    JsonWriter->WriteValue("bClosed", bClosed);

    ContentItem->SerializeToJson(JsonWriter);

	JsonWriter->WriteValue("ID", ID.ToString());

}

void FArmyExtrusionPath::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
    const TArray<TSharedPtr<FJsonValue>>& VertexesArray = InJsonData->GetArrayField("vertexes");
    for (auto& It : VertexesArray)
    {
        FVector Vertex;
        Vertex.InitFromString(It->AsString());
        Vertexes.Add(Vertex);
    }
    bClosed = InJsonData->GetBoolField("bClosed");

    ContentItem = MakeShareable(new FContentItem());
    ContentItem->Deserialization(InJsonData);

	FString IDstr;
	if (InJsonData->TryGetStringField("ID", IDstr))
	{
		FGuid::Parse(IDstr, ID);
	}
	else
	{
		ID = FGuid::NewGuid();
	}
}
