#include "ArmyLampSlotExtruder.h"
#include "Classes/Engine/PointLight.h"
#include "ArmyMath.h"

const float ExtrusionLength = 13.f;

 FArmyLampSlotExtruder::FArmyLampSlotExtruder(UWorld * InWorld, TSharedPtr<class FArmyBaseArea> InSurfaceArea)
	: FArmyExtruder(InWorld, InSurfaceArea, nullptr)
{}

void FArmyLampSlotExtruder::DeleteActor(class AArmyExtrusionActor* InActor)
{
    // @欧石楠 实现灯槽单段删除后吊顶的位置重新计算
    if (InActor && InActor->IsValidLowLevel())
    {
        FArmyExtrusionPath* DeletedPath = ActorMap.Find(InActor);
        if (DeletedPath && SurfaceArea.IsValid())
        {
            UpdateSuspendedCeiling(DeletedPath->Index);

            // @欧石楠 删除对应的灯带施工图数据
            LightConstructionInfo.RemoveAt(DeletedPath->Index);
        }

        // @欧石楠 删除灯槽附着的光源物体
        RemoveLightModelByActor(InActor);
    }

    FArmyExtruder::DeleteActor(InActor);
}

void FArmyLampSlotExtruder::Generate()
{
    if (Paths.Num() == 0)
    {
        return;
    }

    LightConstructionInfo.Reset();
    if (LightConstructionInfo.Num() == 0)
    {
        const float XOffset = 5.f;
        TArray<FVector> LightVertexes = FArmyMath::Extrude3D(Paths[0].Vertexes, XOffset, false);

        int32 Number = LightVertexes.Num();
        for (int32 i = 0; i < Number; ++i)
        {
            LightConstructionInfo.Add(FArmyLightPath(LightVertexes[i], LightVertexes[(i + 1) % Number]));
        }
    }
    
    // @欧石楠 生成放样模型前先销毁之前创建的
    for (auto& It : ActorMap)
    {
        if (It.Key && It.Key->IsValidLowLevel())
        {
            RemoveLightModelByActor(It.Key);
            FArmySceneData::Get()->DeleteExtrusionConstructionItemData(It.Key->UniqueCodeExtrusion);
            It.Key->Destroy();
        }
    }
    ActorMap.Reset();
    LightMap.Reset();

    TArray<FArmyExtrusionPath> SplitedPaths;
    SplitPaths(Paths, SplitedPaths);

    for (int32 i = 0; i < SplitedPaths.Num(); ++i)
    {
        SplitedPaths[i].Index = i;
        GenerateActor(SplitedPaths[i]);
    }
    
    if (AreaVertexes.Num() == 0)
    {
        AreaVertexes = SurfaceArea->OutArea->Vertices;
        AreaVertexes = FArmyMath::ExturdePolygon(AreaVertexes, SurfaceArea->GetLampSlotWidth(), false);
        if (!FArmyMath::IsClockWise(AreaVertexes))
        {
            FArmyMath::ReversePointList(AreaVertexes);
        }
    }
}

void FArmyLampSlotExtruder::Clear()
{
    for (auto& It : ActorMap)
    {
        if (It.Key && It.Key->IsValidLowLevel())
        {
            RemoveLightModelByActor(It.Key);
            FArmySceneData::Get()->DeleteExtrusionConstructionItemData(It.Key->UniqueCodeExtrusion);
            It.Key->Destroy();
        }
    }

    Paths.Reset();
    ActorMap.Reset();
    LightMap.Reset();
    AreaVertexes.Reset();
    LightConstructionInfo.Reset();
}

void FArmyLampSlotExtruder::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    FArmyExtruder::SerializeToJson(JsonWriter);

    JsonWriter->WriteArrayStart("areaVertexes");
    for (const auto& It : AreaVertexes)
    {
        JsonWriter->WriteValue(It.ToString());
    }
    JsonWriter->WriteArrayEnd();

    JsonWriter->WriteArrayStart("lightConstructionInfo");
    for (auto& It : LightConstructionInfo)
    {
        JsonWriter->WriteObjectStart();
        It.SerializeToJson(JsonWriter);
        JsonWriter->WriteObjectEnd();
    }
    JsonWriter->WriteArrayEnd();
}

void FArmyLampSlotExtruder::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
    FArmyExtruder::Deserialization(InJsonData);

    const TArray< TSharedPtr<FJsonValue> > AreaVertexesArray = InJsonData->GetArrayField("areaVertexes");
    for (const auto& It : AreaVertexesArray)
    {
        FVector AreaVertex;
        AreaVertex.InitFromString(It->AsString());
        AreaVertexes.Add(AreaVertex);
    }

    const TArray< TSharedPtr<FJsonValue> > LightConstructionInfoArray = InJsonData->GetArrayField("lightConstructionInfo");
    for (const auto& It : LightConstructionInfoArray)
    {
        FArmyLightPath LightPath;
        LightPath.Deserialization(It->AsObject());
        LightConstructionInfo.Add(LightPath);
    }
}

void FArmyLampSlotExtruder::ApplyOffset(const FVector& InOffset)
{
    FArmyExtruder::ApplyOffset(InOffset);

    for (auto& It : AreaVertexes)
    {
        It.X -= InOffset.X;
        It.Y += InOffset.Y;
    }
    if (SurfaceArea.IsValid())
    {
        SurfaceArea->UpdateWallArea(AreaVertexes);
    }
}

FName FArmyLampSlotExtruder::GetFolderPath()
{
    return FArmyActorPath::GetRoofPath();
}

AArmyExtrusionActor* FArmyLampSlotExtruder::GenerateActor(const FArmyExtrusionPath& Path)
{
    FString DXFFilePath = GetDXFFilePath(Path.ContentItem);
    if (DXFFilePath.Len() == 0)
    {
        return nullptr;
    }

    TArray<FVector> LampSlotVertexes = ExtrusionVertexes(Path.Vertexes, SurfaceArea->GetLampSlotWidth());

    AArmyExtrusionActor* ResultActor = World->SpawnActor<AArmyExtrusionActor>(AArmyExtrusionActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
    ResultActor->MeshComponent->bBuildStaticLighting = true;
    //@马云龙 灯槽特殊处理，给一个较大的分辨率
    ResultActor->MeshComponent->SetLightmapResolution(256);
    ResultActor->SetFolderPath(GetFolderPath());
    ResultActor->Tags.Add(XRActorTag::Immovable);
    ResultActor->Tags.Add(XRActorTag::CanNotDelete);
    ResultActor->Tags.Add("LampSlotActor");
    ResultActor->SetActorLabel(FArmyActorLabel::MakeLampSlotLabel());
    ResultActor->SetSynID(Path.ContentItem->ID);
    ResultActor->GenerateSkitLineMeshFromFilePath(DXFFilePath, LampSlotVertexes, NeedReverse(), Path.bClosed);

    if (SurfaceArea.IsValid())
    {
        ResultActor->AttachSurfaceArea = SurfaceArea;
    }

    UMaterialInterface* MI = FArmyResourceModule::Get().GetResourceManager()->CreateContentItemMaterial(Path.ContentItem);
    if (MI)
    {
        ResultActor->SetMaterial(MI);
    }

    ActorMap.Add(ResultActor, Path);

    TArray<FVector> LightVertexes = ExtrusionVertexes(Path.Vertexes, SurfaceArea->GetLampSlotWidth() * 0.75f);
    for (auto& It : LightVertexes)
    {
        It.Z += 7.f;
    }

    // @欧石楠 生成灯管
    AArmyExtrusionActor* LightModel = GenerateLightModel(LightVertexes[1], LightVertexes[2], Path.Index);
    LightModel->SetSynID(ResultActor->GetSynID());

	//@郭子阳
	GetAllLightModel().Add(Path.ID, LightModel);

    LightMap.Add(ResultActor, LightModel);

    // @欧石楠 生成灯光
    APointLight* PointLight = GenerateLight(LightVertexes[1], LightVertexes[2], Path.Index);
    FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, false);
    PointLight->AttachToActor(LightModel, Rules);

    return ResultActor;
}

AArmyExtrusionActor* FArmyLampSlotExtruder::GenerateRelatedActor(FArmyExtrusionPath& Path, bool bLeft)
{
    if (Path.Vertexes.Num() >= 4)
    {
        FVector LastStart = Path.Vertexes[1];
        FVector LastEnd = Path.Vertexes[2];
        FVector& Start = Path.Vertexes[1];
        FVector& End = Path.Vertexes[2];

        if (bLeft)
        {
            if (Path.Vertexes[3] == LastEnd)
            {
                Path.Vertexes[3] = End;
            }

            for (auto& It : Paths)
            {
                int32 Index = -1;
                if (It.Vertexes.Find(LastEnd, Index))
                {
                    It.Vertexes[Index] = End;
                    break;
                }
                
            }
        }
        else
        {
            if (Path.Vertexes[0] == LastStart)
            {
                Path.Vertexes[0] = Start;
            }

            for (auto& It : Paths)
            {
                int32 Index = -1;
                if (It.Vertexes.Find(LastStart, Index))
                {
                    It.Vertexes[Index] = Start;
                    break;
                }
            }
        }
    }
    
    return GenerateActor(Path);
}

TSharedPtr<FContentItem> FArmyLampSlotExtruder::DeleteActorByVertex(const FVector& InVertex)
{
    TSharedPtr<FContentItem> Result = nullptr;

    for (auto& It : ActorMap)
    {
        if (It.Value.Vertexes[1] == InVertex)
        {
            RemoveLightModelByActor(It.Key);
            Result = It.Value.ContentItem;
            It.Key->Destroy();
            ActorMap.Remove(It.Key);
            break;
        }
    }

    return Result;
}

APointLight* FArmyLampSlotExtruder::GenerateLight(FVector StartPos, FVector EndPos, const int32 Index)
{
    FVector Dir = (EndPos - StartPos).GetSafeNormal();

    // @打扮家 XRLightmass 抬高等待灯的位置 防止被遮挡
    FQuat quat = FQuat::FindBetweenNormals(Dir, FVector(1, 0, 0));
    APointLight* pointLight = GVC->GetWorld()->SpawnActor<APointLight>(APointLight::StaticClass(), (StartPos + EndPos) / 2, quat.Rotator());
    pointLight->PointLightComponent->Rename(TEXT("LampSlotPointLightComponent"));
    pointLight->PointLightComponent->SetSourceLength((EndPos - StartPos).Size());
    pointLight->PointLightComponent->SetSourceRadius(2.0f);
    pointLight->PointLightComponent->SetSoftSourceRadius(5.0f);
    pointLight->PointLightComponent->SetAttenuationRadius(300.0f);
    pointLight->PointLightComponent->Intensity = 600.f;
    pointLight->SetMobility(EComponentMobility::Movable);
    pointLight->SetLightColor(FLinearColor(1.0f, 0.8f, 0.5f, 1.0f));
    pointLight->SetCastShadows(false);
    pointLight->SetActorLabel(FArmyActorLabel::MakePointlightLabel());
    pointLight->SetFolderPath(FArmyActorPath::GetLightSourcePath());

    //@打扮家 XRLightmass 序列化 为LightCOM设置Json反序列化来的GUID，可以失败
    if (LampLightIDs.IsValidIndex(Index) && LampLightIDs[Index].IsValid())
    {
        pointLight->PointLightComponent->LightGuid = LampLightIDs[Index];
    }

    return pointLight;
}

AArmyExtrusionActor* FArmyLampSlotExtruder::GenerateLightModel(FVector StartPos, FVector EndPos, const int32 Index)
{
    AArmyExtrusionActor* ResultActor = World->SpawnActor<AArmyExtrusionActor>(AArmyExtrusionActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
    ResultActor->Tags.Add(XRActorTag::Immovable);
    ResultActor->SetFolderPath(FArmyActorPath::GetLightSourcePath());
    ResultActor->SetActorLabel(FArmyActorLabel::MakeLampLabel());
    RecaculateLightModelPostion(ResultActor, StartPos, EndPos, Index);

    return ResultActor;
}

void FArmyLampSlotExtruder::RecaculateLightModelPostion(AArmyExtrusionActor* LightModel, FVector StartPos, FVector EndPos, const int32 Index)
{
    FVector Dir = (EndPos - StartPos).GetSafeNormal();

    float Radius = 2.0f;
    FVector CircleStartPos = FVector(-1, 0, 0) * Radius;
    TArray<FVector2D> TrackProfileVerts;
    int CircleNumber = 64;
    float DeltaAngle = 360.0f / CircleNumber;
    float StartAngle = 0.0f;
    for (int32 i = 0; i < CircleNumber; i++)
    {
        FVector TempPoint = FRotator(0, StartAngle, 0).RotateVector(CircleStartPos);
        StartAngle += DeltaAngle;
        TrackProfileVerts.Push(FVector2D(TempPoint));
    }
    TrackProfileVerts.Push(FVector2D(CircleStartPos));

    // @欧石楠 必须是Num为4的数组，详情请参考XRExtruder的SplitPaths方法注释
    TArray<FVector> NewPos;
    NewPos.Add(StartPos);
    NewPos.Add(StartPos);
    NewPos.Add(EndPos);
    NewPos.Add(EndPos);

    LightModel->GenerateSkitLineMeshWithOutlineData(TrackProfileVerts, NewPos, true);
}

void FArmyLampSlotExtruder::UpdateSuspendedCeiling(const int32& InIndex)
{
    // @欧石楠 灯带生成的顺序和计算吊顶侧面顶点的顺序是反的，需要倒序取
    int32 StartIndex = AreaVertexes.Num() - 1 - InIndex;
    int32 EndIndex = AreaVertexes.Num() - 1 - InIndex - 1;
    EndIndex = EndIndex < 0 ? EndIndex + AreaVertexes.Num() : EndIndex;

    FVector StartPos = AreaVertexes[StartIndex];
    FVector EndPos = AreaVertexes[EndIndex];
    FVector Dir = (StartPos - EndPos).GetSafeNormal();
    Dir = Dir.RotateAngleAxis(90.f, FVector(0.f, 0.f, 1.f));
    StartPos += Dir * ExtrusionLength;
    EndPos += Dir * ExtrusionLength;
    AreaVertexes[StartIndex] = StartPos;
    AreaVertexes[EndIndex] = EndPos;

    SurfaceArea->UpdateWallArea(AreaVertexes);
}

void FArmyLampSlotExtruder::RemoveLightModelByActor(AArmyExtrusionActor* InActor)
{
    AArmyExtrusionActor* LightModel = *(LightMap.Find(InActor));
    if (LightModel && LightModel->IsValidLowLevel())
    {
        TArray<AActor*> AttachedActors;
        LightModel->GetAttachedActors(AttachedActors);
        for (auto Actor : AttachedActors)
        {
            Actor->Destroy();
        }

        LightModel->Destroy();

        //@郭子阳
		//删除
        auto LightIDPtr = GetAllLightModel().FindKey(LightModel);
        if (LightIDPtr)
        {
            GetAllLightModel().Remove(*LightIDPtr);
        }
    }
}

TArray<FVector> FArmyLampSlotExtruder::ExtrusionVertexes(const TArray<FVector>& InVertexes, const float InExtrusionDistance)
{
    TArray<FVector> ResultVertexes = InVertexes;
    FVector LeftExtrusionDir = (ResultVertexes[1] - ResultVertexes[0]).GetSafeNormal();
    LeftExtrusionDir = LeftExtrusionDir.RotateAngleAxis(-90.f, FVector(0.f, 0.f, 1.f));
    FVector LeftStart = ResultVertexes[0];
    FVector LeftEnd = ResultVertexes[1];

    FVector RightExtrusionDir = (ResultVertexes[3] - ResultVertexes[2]).GetSafeNormal();
    RightExtrusionDir = RightExtrusionDir.RotateAngleAxis(-90.f, FVector(0.f, 0.f, 1.f));
    FVector RightStart = ResultVertexes[2];
    FVector RightEnd = ResultVertexes[3];

    FVector ExtrusionDir = (ResultVertexes[2] - ResultVertexes[1]).GetSafeNormal();
    ExtrusionDir = ExtrusionDir.RotateAngleAxis(-90.f, FVector(0.f, 0.f, 1.f));
    FVector Start = ResultVertexes[1] + ExtrusionDir * InExtrusionDistance;
    FVector End = ResultVertexes[2] + ExtrusionDir * InExtrusionDistance;

    FVector2D NewStart = FVector2D(Start);
    if (LeftStart != LeftEnd)
    {
        LeftStart += LeftExtrusionDir * InExtrusionDistance;
        LeftEnd += LeftExtrusionDir * InExtrusionDistance;
        FArmyMath::Line2DIntersection(FVector2D(Start), FVector2D(End), FVector2D(LeftStart), FVector2D(LeftEnd), NewStart);
    }

    FVector2D NewEnd = FVector2D(End);
    if (RightStart != RightEnd)
    {
        RightStart += RightExtrusionDir * InExtrusionDistance;
        RightEnd += RightExtrusionDir * InExtrusionDistance;
        FArmyMath::Line2DIntersection(FVector2D(Start), FVector2D(End), FVector2D(RightStart), FVector2D(RightEnd), NewEnd);
    }

    ResultVertexes[1] = FVector(NewStart, ResultVertexes[1].Z);
    ResultVertexes[2] = FVector(NewEnd, ResultVertexes[2].Z);
    ResultVertexes[0] = (LeftStart != LeftEnd) ? LeftStart : ResultVertexes[1];
    ResultVertexes[3] = (RightStart != RightEnd) ? RightEnd : ResultVertexes[2];

    return ResultVertexes;
}

TMap<FGuid, AArmyExtrusionActor*>& FArmyLampSlotExtruder::GetAllLightModel()
{
	static TMap< FGuid, AArmyExtrusionActor* > AllLightModel;
	return AllLightModel;
}

void FArmyLightPath::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    JsonWriter->WriteValue("startPos", StartPos.ToString());
    JsonWriter->WriteValue("endPos", EndPos.ToString());
}

void FArmyLightPath::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
    FString StartPosStr, EndPosStr;
    if (InJsonData->TryGetStringField("startPos", StartPosStr))
    {
        StartPos.InitFromString(StartPosStr);
    }
    if (InJsonData->TryGetStringField("endPos", EndPosStr))
    {
        EndPos.InitFromString(EndPosStr);
    }
}
