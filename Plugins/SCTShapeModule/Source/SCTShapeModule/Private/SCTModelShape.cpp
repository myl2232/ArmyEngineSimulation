#include "SCTModelShape.h"
#include "JsonObject.h"
#include "Paths.h"
#include "Math/UnrealMathUtility.h"
#include "SCTShapeManager.h"
#include "SCTStaticMeshManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "SCTModelActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "FileManager.h"
#include "SCTShapeBoxActor.h"
#include "SCTAttribute.h"
#include "SCTCompActor.h"
#include "SCTGlobalDataForShape.h"
#include "PhysicsEngine/BodySetup.h"


FString FDecorationShape::FileCacheDir = FPaths::Combine(FGlobalDataForShape::GetInsPtr()->GetRootDir(), TEXT("Model"));

FDecorationShape::FDecorationShape()
{
	SetShapeType(ST_Decoration);
	ModelID = 0;
	FileCacheDir = FPaths::Combine(FGlobalDataForShape::GetInsPtr()->GetRootDir(), TEXT("Model"));   
}

FDecorationShape::~FDecorationShape()
{

}

FString FDecorationShape::GetCacheFilePathFromIDAndMd5AndUpdateTime(const int32 & InModelId, const FString & InMd5, const float & InUpdateTime)
{
	const FString UpdateTimeStr = FString::Printf(TEXT("%f"), InUpdateTime);
	return GetCacheFilePathFromIDAndMd5AndUpdateTime(InModelId, InMd5, UpdateTimeStr);
}

FString FDecorationShape::GetCacheFilePathFromIDAndMd5AndUpdateTime(const int32 & InModelId, const FString & InMd5, const FString & InUpdateTime)
{
	const FString ModelIdStr = FString::Printf(TEXT("%d"), InModelId);
	return GetCacheFilePathFromIDAndMd5AndUpdateTime(ModelIdStr, InMd5, InUpdateTime);
}

FString FDecorationShape::GetCacheFilePathFromIDAndMd5AndUpdateTime(const FString & InModelId, const FString & InMd5, const FString & InUpdateTime)
{	
	return FPaths::Combine(FileCacheDir, GetCacheFileName(InModelId, InMd5, InUpdateTime));
}

FString FDecorationShape::GetCacheFilePathFromIDAndMd5AndUpdateTime(const FString & InModelId, const FString & InMd5, const float & InUpdateTime)
{
	const FString UpdateTimeStr = FString::Printf(TEXT("%f"), InUpdateTime);
	return GetCacheFilePathFromIDAndMd5AndUpdateTime(InModelId, InMd5, UpdateTimeStr);
}

FString FDecorationShape::GetCacheFileName(const FString & InModelId, const FString & InMd5, const FString & InUpdateTime)
{
	return FString::Printf(TEXT("%s_%s.pak"), *GetCacheFileNameWithoutUpdateTime(InModelId, InMd5), *InUpdateTime);
}

FString FDecorationShape::GetCacheFileNameWithoutUpdateTime(const FString & InModelId, const FString & InMd5)
{
	return FString::Printf(TEXT("%s_%s"), *InModelId, *InMd5);
}

void FDecorationShape::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//解析基本信息
	FSCTShape::ParseFromJson(InJsonObject);

	//更新时间
	float UpdateTimeFloat = InJsonObject->GetNumberField(TEXT("updateTime"));
	UpdateTimeStr = FString::Printf(TEXT("%f"), UpdateTimeFloat);

	//模型文件信息
	SetModelID(InJsonObject->GetIntegerField(TEXT("modelId")));
	SetFileName(InJsonObject->GetStringField(TEXT("fileName")));
	SetFileUrl(InJsonObject->GetStringField(TEXT("fileUrl")));
	SetFileMd5(InJsonObject->GetStringField(TEXT("fileMd5")));
	SetoptimizeParam(InJsonObject->GetStringField(TEXT("optimizeParam")));
}

void FDecorationShape::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//型录类型和ID
	int32 CurrentType = InJsonObject->GetIntegerField(TEXT("type"));
	int32 CurrentId = InJsonObject->GetIntegerField(TEXT("id"));
	check(CurrentType == ShapeType && CurrentId == ShapeId);

	//型录基本信息
	FSCTShape::ParseAttributesFromJson(InJsonObject);
}

void FDecorationShape::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//解析基本信息
	FSCTShape::ParseFromJson(InJsonObject);

	//更新时间
	float UpdateTimeFloat = InJsonObject->GetNumberField(TEXT("updateTime"));
	UpdateTimeStr = FString::Printf(TEXT("%f"), UpdateTimeFloat);

	//模型文件信息
	SetModelID(InJsonObject->GetIntegerField(TEXT("modelId")));
	SetFileName(InJsonObject->GetStringField(TEXT("fileName")));
	if (InJsonObject->HasField(TEXT("fileUrl")))
	{
		SetFileUrl(InJsonObject->GetStringField(TEXT("fileUrl")));
	}
	else if(InJsonObject->HasField(TEXT("pakUrl")))
	{
		SetFileUrl(InJsonObject->GetStringField(TEXT("pakUrl")));
	}
	SetFileMd5(InJsonObject->GetStringField(TEXT("fileMd5")));
	SetoptimizeParam(InJsonObject->GetStringField(TEXT("optimizeParam")));
}

void FDecorationShape::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//解析基本信息
	FSCTShape::ParseFromJson(InJsonObject);

	//更新时间
	float UpdateTimeFloat = InJsonObject->GetNumberField(TEXT("updateTime"));
	UpdateTimeStr = FString::Printf(TEXT("%f"), UpdateTimeFloat);

	//模型文件信息
	SetModelID(InJsonObject->GetIntegerField(TEXT("modelId")));
	SetFileName(InJsonObject->GetStringField(TEXT("fileName")));
	if (InJsonObject->HasField(TEXT("fileUrl")))
	{
		SetFileUrl(InJsonObject->GetStringField(TEXT("fileUrl")));
	}
	else if (InJsonObject->HasField(TEXT("pakUrl")))
	{
		SetFileUrl(InJsonObject->GetStringField(TEXT("pakUrl")));
	}
	SetFileMd5(InJsonObject->GetStringField(TEXT("fileMd5")));
	SetoptimizeParam(InJsonObject->GetStringField(TEXT("optimizeParam")));
}

void FDecorationShape::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//保存基本信息
	FSCTShape::SaveToJson(JsonWriter);

	//更新时间
	JsonWriter->WriteValue(TEXT("updateTime"), GetUpdateTime());

	//模型文件信息
	JsonWriter->WriteValue(TEXT("modelId"), GetModelID());
	JsonWriter->WriteValue(TEXT("fileName"), GetFileName());
	JsonWriter->WriteValue(TEXT("fileUrl"), GetFileUrl());
	JsonWriter->WriteValue(TEXT("fileMd5"), GetFileMd5());
	JsonWriter->WriteValue(TEXT("optimizeParam"), GetoptimizeParam());
}

void FDecorationShape::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//保存型录基本信息
	FSCTShape::SaveAttriToJson(JsonWriter);
}

void FDecorationShape::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveShapeToJson(JsonWriter);

	//更新时间
	JsonWriter->WriteValue(TEXT("updateTime"), GetUpdateTime());

	//模型文件信息
	JsonWriter->WriteValue(TEXT("modelId"), GetModelID());
	JsonWriter->WriteValue(TEXT("fileName"), GetFileName());
	JsonWriter->WriteValue(TEXT("fileUrl"), GetFileUrl());
	JsonWriter->WriteValue(TEXT("fileMd5"), GetFileMd5());
	JsonWriter->WriteValue(TEXT("optimizeParam"), GetoptimizeParam());
}

void FDecorationShape::GetResourceUrls(TArray<FString> &OutResourceUrls)
{
	OutResourceUrls.Push(FileUrl);
}

void FDecorationShape::GetFileCachePaths(TArray<FString> &OutFileCachePaths)
{
	// int64 ModelID = CurAccessory->GetModelID();
	// FString FileMd5 = CurAccessory->GetFileMd5();
	// FString FileName = FString::FromInt((int32)ModelID) + TEXT("_") + FileMd5 + TEXT("_") + UpdateTimeStr + TEXT(".pak");
	// FString FileDirectory = FPaths::ProjectDir() + TEXT("Content/Model/");
	// FString FileCachePath = FileDirectory + FileName;
	const FString TempModelId = GetModelIdStr();
	OutFileCachePaths.Emplace(GetCacheFilePathFromIDAndMd5AndUpdateTime(TempModelId,FileMd5,UpdateTimeStr));	
}

void FDecorationShape::CopyTo(FSCTShape* OutShape)
{
	//基类信息
	FSCTShape::CopyTo(OutShape);
	
	FDecorationShape* ModelShape = StaticCast<FDecorationShape*>(OutShape);
	//模型类型
	//ModelShape->ModelType = ModelType;
	//更新时间
	ModelShape->UpdateTimeStr = UpdateTimeStr;

	//模型文件信息
	ModelShape->ModelID = ModelID;       //数据库ID
	ModelShape->FileName = FileName;    //文件名称
	ModelShape->FileUrl = FileUrl;     //文件URL
	ModelShape->FileMd5 = FileMd5;     //文件MD5码
	ModelShape->OptimizeParam = OptimizeParam;

	//模型网格
	ModelShape->ModelMesh = ModelMesh;
}

ASCTShapeActor* FDecorationShape::SpawnShapeActor()
{
	//下载/加载Pak文件
	bool bLoadMesh = LoadModelMesh();
	if (!bLoadMesh)
	{
		return nullptr;
	}

	//创建生成型录Actor
	UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();
	ASCTModelActor* NewShapeActor = World->SpawnActor<ASCTModelActor>(
		ASCTModelActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);

	//设置关联的Shape
	ShapeActor = NewShapeActor;
	NewShapeActor->SetShape(this);

	//设置模型网格
	check(ModelMesh.Type != -1);
	if (ModelMesh.Type == 0)
	{
		NewShapeActor->SetStaticMesh(ModelMesh.MeshData.Mesh, ModelMesh.MaterialList);
	}
	else if (ModelMesh.Type == 1)
	{
		NewShapeActor->SetSkeletalMesh(ModelMesh.MeshData.SkeletalMesh, ModelMesh.AnimationAsset, ModelMesh.MaterialList);
		NewShapeActor->SetFixedModelMeshCenterSize(ModelMesh.Center * 10.0f,ModelMesh.Size * 10.0f);
	}

	//设置型录尺寸、位置和旋转
	NewShapeActor->UpdateActorDimension();
	NewShapeActor->UpdateActorPosition();
	NewShapeActor->UpdateActorRotation();

	//设置型录可见性
	bool bParentVisible = NewShapeActor->GetVisibleStateFromParent();
	NewShapeActor->SetActorHiddenInGame(!GetShowCondition() || !bParentVisible);

	return NewShapeActor;
}

void FDecorationShape::SpawnActorsForSelected(FName InProfileName)
{
	ASCTShapeActor* CurShapeActor = GetShapeActor();
	if (!CurShapeActor)
	{
		return;
	}

	//Model Mesh Size
	ASCTModelActor* CurModelActor = StaticCast<ASCTModelActor*>(CurShapeActor);
	//FVector MeshSize = CurModelActor->GetModelMeshSize();
	//生成模型的包围盒，用于鼠标点选
	ASCTBoundingBoxActor* BoundingBoxActor = SpawnBoundingBoxActor();
	BoundingBoxActor->SetCollisionProfileName(InProfileName);
	BoundingBoxActor->SetLineAlpha(0.0f);
	BoundingBoxActor->SetRegionAlpha(0.0f);
	BoundingBoxActor->SetIsNeedGetShapeSize(false);
	FBoxSphereBounds SrcBound = GetBoxSphereBounds();
	if (CurModelActor->IsSkeletalMesh())
	{
		const FVector SkeletalExt = CurModelActor->GetModelMeshSize() / 10.0f / 2.0f;
		const FVector SkeletaCenter = CurModelActor->GetModelCenter() / 10.0f;
		SrcBound = FBoxSphereBounds(FBox(SkeletaCenter - SkeletalExt, SkeletaCenter + SkeletalExt));
	}
	//FTransform Transform = FTransform::Identity;	
	//const FVector SacleVector = FVector{ GetShapeWidth() / MeshSize.X , GetShapeDepth() / MeshSize.Y  ,GetShapeHeight() / MeshSize.Z };
	//Transform.SetScale3D(SacleVector);
	//FBoxSphereBounds DesBound = SrcBound.TransformBy(Transform);
	//BoundingBoxActor->SetActorDimension(MeshSize.X, MeshSize.Y, MeshSize.Z);
	BoundingBoxActor->SetActorDimension(SrcBound.GetBox().GetCenter(), SrcBound.GetBox().Min, SrcBound.GetBox().Max);
	BoundingBoxActor->SetActorHiddenInGame(false);
	BoundingBoxActor->AttachToActor(CurShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
	//创建型录外包框
	ASCTWireframeActor* WireFrameActor = SpawnWireFrameActor();	
	//WireFrameActor->SetActorDimension(MeshSize.X, MeshSize.Y, MeshSize.Z);
	WireFrameActor->SetActorDimension(SrcBound.GetBox().GetCenter(), SrcBound.GetBox().Min, SrcBound.GetBox().Max);
	WireFrameActor->SetActorHiddenInGame(true);
	WireFrameActor->SetIsNeedGetShapeSize(false);
	WireFrameActor->AttachToActor(CurShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
}

void FDecorationShape::SetCollisionProfileName(FName InProfileName)
{
	if (ShapeActor)
	{
		UMeshComponent *ActorPrimComp = Cast<UMeshComponent>(ShapeActor->GetRootComponent());
		ActorPrimComp->SetCollisionProfileName(InProfileName);

		TArray<AActor*> AttachedActors;
		ShapeActor->GetAttachedActors(AttachedActors);
		for (auto& Actor : AttachedActors)
		{
			int32 NumChilds = Actor->GetRootComponent()->GetNumChildrenComponents();
			for (int32 i = 0; i < NumChilds; ++i)
			{
				UMeshComponent *ChildMeshComp = Cast<UMeshComponent>(Actor->GetRootComponent()->GetChildComponent(i));
				if (ChildMeshComp != nullptr)
					ChildMeshComp->SetCollisionProfileName(InProfileName);
			}
		}
	}
}

bool FDecorationShape::SetShapeWidth(float InValue)
{
	return FDecorationShape::SetShapeWidth(FString::Printf(TEXT("%f"), InValue));
}

bool FDecorationShape::SetShapeWidth(const FString &InStrValue)
{
	bool bResult = true;
	if (Width->GetAttributeStr() != InStrValue)
	{
		bResult = Width->SetAttributeValue(InStrValue);
		//若Actor已生成，同时更新Actor
		if (bResult)
		{
			//UpdateAssociatedActors(1);
			if (ShapeActor)
			{
				ShapeActor->UpdateActorDimension();
			}
			////更新外包框和包围盒(不再需要更新，通过比例系数实现)
			//FBoxSphereBounds Bound = GetBoxSphereBounds();
			//if (WireFrameActor)
			//{				
			//	WireFrameActor->SetActorDimension(Bound.GetBox().GetCenter(), Bound.GetBox().Min, Bound.GetBox().Max);
			//	WireFrameActor->UpdateActorDimension();
			//}
			//if (BoundingBoxActor)
			//{
			//	BoundingBoxActor->SetActorDimension(Bound.GetBox().GetCenter(), Bound.GetBox().Min, Bound.GetBox().Max);
			//	BoundingBoxActor->UpdateActorDimension();
			//}
		}
	}
	return bResult;
}

bool FDecorationShape::SetShapeDepth(float InValue)
{
	return FDecorationShape::SetShapeDepth(FString::Printf(TEXT("%f"), InValue));
}

bool FDecorationShape::SetShapeDepth(const FString &InStrValue)
{
	bool bResult = true;
	if (Depth->GetAttributeStr() != InStrValue)
	{
		bResult = Depth->SetAttributeValue(InStrValue);
		//若Actor已生成，同时更新Actor
		if (bResult)
		{
			//UpdateAssociatedActors(1);
			if (ShapeActor)
			{
				ShapeActor->UpdateActorDimension();
			}
			//更新外包框和包围盒(不再需要更新，通过比例系数实现)
			/*FBoxSphereBounds Bound = GetBoxSphereBounds();
			if (WireFrameActor)
			{
				WireFrameActor->SetActorDimension(Bound.GetBox().GetCenter(), Bound.GetBox().Min, Bound.GetBox().Max);
				WireFrameActor->UpdateActorDimension();
			}
			if (BoundingBoxActor)
			{
				BoundingBoxActor->SetActorDimension(Bound.GetBox().GetCenter(), Bound.GetBox().Min, Bound.GetBox().Max);
				BoundingBoxActor->UpdateActorDimension();
			}*/
		}
	}
	return bResult;
}

bool FDecorationShape::SetShapeHeight(float InValue)
{
	return FDecorationShape::SetShapeHeight(FString::Printf(TEXT("%f"), InValue));
}

bool FDecorationShape::SetShapeHeight(const FString &InStrValue)
{
	bool bResult = true;
	if (Height->GetAttributeStr() != InStrValue)
	{
		bResult = Height->SetAttributeValue(InStrValue);
		//若Actor已生成，同时更新Actor
		if (bResult)
		{
			//UpdateAssociatedActors(1);
			if (ShapeActor)
			{
				ShapeActor->UpdateActorDimension();
			}
			//更新外包框和包围盒(不再需要更新，通过比例系数实现)
			/*FBoxSphereBounds Bound = GetBoxSphereBounds();
			if (WireFrameActor)
			{
				WireFrameActor->SetActorDimension(Bound.GetBox().GetCenter(), Bound.GetBox().Min, Bound.GetBox().Max);
				WireFrameActor->UpdateActorDimension();
			}
			if (BoundingBoxActor)
			{
				BoundingBoxActor->SetActorDimension(Bound.GetBox().GetCenter(), Bound.GetBox().Min, Bound.GetBox().Max);
				BoundingBoxActor->UpdateActorDimension();
			}*/
		}
	}
	return bResult;
}

bool FDecorationShape::SetShapePosX(const FString &InStrValue)
{
	bool bResult = true;
	if (PositionX->GetAttributeStr() != InStrValue)
	{
		bResult = PositionX->SetAttributeValue(InStrValue);
		//若Actor已生成，同时更新Actor
		if (bResult)
		{
			//UpdateAssociatedActors(2);
			if (ShapeActor)
			{
				ShapeActor->UpdateActorPosition();
			}
		}
	}
	return bResult;
}

bool FDecorationShape::SetShapePosY(const FString &InStrValue)
{
	bool bResult = true;
	if (PositionY->GetAttributeStr() != InStrValue)
	{
		bResult = PositionY->SetAttributeValue(InStrValue);
		//若Actor已生成，同时更新Actor
		if (bResult)
		{
			//UpdateAssociatedActors(2);
			if (ShapeActor)
			{
				ShapeActor->UpdateActorPosition();
			}
		}
	}
	return bResult;
}

bool FDecorationShape::SetShapePosZ(const FString &InStrValue)
{
	bool bResult = true;
	if (PositionZ->GetAttributeStr() != InStrValue)
	{
		bResult = PositionZ->SetAttributeValue(InStrValue);
		//若Actor已生成，同时更新Actor
		if (bResult)
		{
			//UpdateAssociatedActors(2);
			if (ShapeActor)
			{
				ShapeActor->UpdateActorPosition();
			}
		}
	}
	return bResult;
}

bool FDecorationShape::SetShapePosX(float InValue)
{
	return FDecorationShape::SetShapePosX(FString::Printf(TEXT("%f"), InValue));
}

bool FDecorationShape::SetShapePosY(float InValue)
{
	return FDecorationShape::SetShapePosY(FString::Printf(TEXT("%f"), InValue));
}

bool FDecorationShape::SetShapePosZ(float InValue)
{
	return FDecorationShape::SetShapePosZ(FString::Printf(TEXT("%f"), InValue));
}

//void FDecorationShape::ReBuildBoxSphereBounds(const FBoxSphereBounds & InBoundBox)
//{		
//	if (!BoundingBoxActor || !WireFrameActor) return;
//	BoundingBoxActor->SetActorDimension(InBoundBox.GetBox().GetCenter(), InBoundBox.GetBox().Min, InBoundBox.GetBox().Max);
//	WireFrameActor->SetActorDimension(InBoundBox.GetBox().GetCenter(), InBoundBox.GetBox().Min, InBoundBox.GetBox().Max);	
//	check(ShapeActor && ShapeActor->IsA<ASCTModelActor>());
//	//Cast<ASCTModelActor>(ShapeActor)->SetIsDelayReBuildBoundBox(false);
//}

// void FDecorationShape::SetModelType(int32 InType)
// {
// 	ModelType = (EModelType)InType;
// }
// int32 FDecorationShape::GetModelType()
// {
// 	return ModelType;
// }

void FDecorationShape::SetUpdateTime(FString InTime)
{
	UpdateTimeStr = InTime;
}
FString FDecorationShape::GetUpdateTime()
{
	return UpdateTimeStr;
}

void FDecorationShape::SetModelID(int64 InID)
{
	ModelID = InID;
}
int64 FDecorationShape::GetModelID()
{
	return ModelID;
}

void FDecorationShape::SetFileName(FString InName)
{
	FileName = InName;
}
FString FDecorationShape::GetFileName()
{
	return FileName;
}

void FDecorationShape::SetFileUrl(FString InUrl)
{
	FileUrl = InUrl;
}
FString FDecorationShape::GetFileUrl()
{
	return FileUrl;
}

void FDecorationShape::SetFileMd5(FString inMd5)
{
	FileMd5 = inMd5;
}
FString FDecorationShape::GetFileMd5()
{
	return FileMd5;
}

bool FDecorationShape::LoadModelMesh(const bool bForceReload /*= false*/)
{
	if (ModelMesh.Type != -1 && !bForceReload)
		return true;

	//处理本地缓存文件:删除同名但不同时间的缓存文件	
	FString TempModelId = GetModelIdStr();
	
	//本地缓存名称(Id_Time)
	FString FileCacheName = GetCacheFileName(TempModelId,FileMd5,UpdateTimeStr);

	if (ModelID != 0)
	{
		//处理本地缓存文件:删除同名但不同时间的缓存文件
		TArray<FString> FindedFiles;
		FString SearchFileName = FPaths::Combine(GetFileCacheDir(), GetCacheFileNameWithoutUpdateTime(TempModelId,FileMd5) + TEXT("*.pak"));
		IFileManager::Get().FindFiles(FindedFiles, *SearchFileName, true, false);
		for (int32 i = 0; i < FindedFiles.Num(); ++i)
		{
			if (FindedFiles[i] != FileCacheName)
			{
				FString DeleteFileName = GetFileCacheDir() + FindedFiles[i];
				IFileManager::Get().Delete(*DeleteFileName);
			}
		}
	}

	//下载并加载模型资源
	FString FileCachePath = GetCacheFilePathFromIDAndMd5AndUpdateTime(TempModelId, FileMd5,UpdateTimeStr);
	TSharedPtr<FMeshManager> StaticMeshMrg = FSCTShapeManager::Get()->GetStaticMeshManager();
	// 此处不应该进行下载，loadMesh的时候要确保资源已经下载完成
	//StaticMeshMrg->DownLoadPakFile(FileUrl, ""/*FileMd5*/, FileCachePath);
	StaticMeshMrg->UpLoadStaticMesh(FileUrl, FileCachePath,OptimizeParam);
	const FMeshDataEntry * TemoMeshDataEntry = StaticMeshMrg->FindMesh(FileCachePath);
	if (TemoMeshDataEntry)
	{
		ModelMesh = *TemoMeshDataEntry;
	}

	//模型下载失败
	if (!ModelMesh.MeshData.Mesh && !ModelMesh.MeshData.SkeletalMesh)
	{
		ModelMesh.Type = -1;
		return false;
	}	
	return ModelMesh.Type != -1; //ModelMesh.IsValid();
}

const FBoxSphereBounds FDecorationShape::GetBoxSphereBounds(const bool InTryGetCollisionBox /*= false */) const
{		
	FBoxSphereBounds Bound = FBox{ FVector::ZeroVector,FVector{const_cast<FDecorationShape*>(this)->GetShapeWidth(),const_cast<FDecorationShape*>(this)->GetShapeDepth(),const_cast<FDecorationShape*>(this)->GetShapeHeight()} };
	if (ModelMesh.Type == 0)
	{
		if (InTryGetCollisionBox && ModelMesh.MeshData.Mesh->BodySetup)
		{
			if (ModelMesh.MeshData.Mesh->BodySetup->AggGeom.BoxElems.Num() > 0)
			{
				const FKBoxElem & BoxRef = ModelMesh.MeshData.Mesh->BodySetup->AggGeom.BoxElems[0];
				FVector Sacle = BoxRef.GetTransform().GetScale3D();
				if (GetShapeActor())
				{
					Sacle = GetShapeActor()->GetActorRelativeScale3D();
				}
				Bound = ModelMesh.MeshData.Mesh->BodySetup->AggGeom.CalcAABB(BoxRef.GetTransform().GetRelativeTransformReverse(BoxRef.GetTransform()));	
				Bound = FBox{ Bound.GetBox().Min * Sacle,Bound.GetBox().Max * Sacle };
			}
		}
		else
		{
			Bound = ModelMesh.MeshData.Mesh->GetBounds();
		}
	}
	else if (ModelMesh.Type == 1)
	{
		Bound = ModelMesh.MeshData.SkeletalMesh->GetBounds();
		if (GetShapeActor())
		{
			ASCTModelActor * SCTShapeActor = Cast<ASCTModelActor>(GetShapeActor());			
			if (SCTShapeActor->IsSkeletalMesh())
			{
				const FVector SkeletalExt = SCTShapeActor->GetModelMeshSize() / 10.0f / 2.0f;
				const FVector SkeletaCenter = SCTShapeActor->GetModelCenter() / 10.0f;
				Bound = FBoxSphereBounds(FBox(SkeletaCenter - SkeletalExt, SkeletaCenter + SkeletalExt));
			}
		}		
	}	
	return Bound;
}

const FString FDecorationShape::GetModelIdStr()
{	
	return  FString::FromInt(ModelID);
}


FAccessoryShape::FAccessoryShape()
	:FDecorationShape()
{
	SetShapeType(ST_Accessory);
}

FAccessoryShape::~FAccessoryShape()
{

}

void FAccessoryShape::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FDecorationShape::ParseFromJson(InJsonObject);
	//产品编码
	SetProduceCode(InJsonObject->GetStringField(TEXT("ProduceCode")));
	// 品牌民称
	if (InJsonObject->HasField(TEXT("brandName")))
	{
		SetBrandName(InJsonObject->GetStringField(TEXT("brandName")));
	}
	// 产品型号
	if (InJsonObject->HasField(TEXT("version")))
	{
		SetProductVersion(InJsonObject->GetStringField(TEXT("version")));
	}

	if (InJsonObject->HasField(TEXT("metalsPropertyList")))
	{
		const TArray<TSharedPtr<FJsonValue>> & ArrayRef = InJsonObject->GetArrayField(TEXT("metalsPropertyList"));
		for(const auto & Ref : ArrayRef)
		{
			FMetalsProperty Pro;
			Pro.MetallId = Ref->AsObject()->GetStringField(TEXT("metalsId"));
			Pro.propertyId = Ref->AsObject()->GetStringField(TEXT("propertyId"));
			Pro.PropertyValueIdentity = Ref->AsObject()->GetStringField(TEXT("propertyValueIdentity"));
			Pro.propertyValue = Ref->AsObject()->GetStringField(TEXT("propertyValue"));
			MetalsPropertyList.Emplace(Pro);
		}
	}
}

void FAccessoryShape::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FDecorationShape::ParseAttributesFromJson(InJsonObject);
}

void FAccessoryShape::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	//解析基本信息
	FDecorationShape::ParseShapeFromJson(InJsonObject);

	//产品编码
	SetProduceCode(InJsonObject->GetStringField(TEXT("ProduceCode")));

	// 品牌民称
	if (InJsonObject->HasField(TEXT("brandName")))
	{
		SetBrandName(InJsonObject->GetStringField(TEXT("brandName")));
	}
	// 产品型号
	if (InJsonObject->HasField(TEXT("version")))
	{
		SetProductVersion(InJsonObject->GetStringField(TEXT("version")));
	}

	if (InJsonObject->HasField(TEXT("metalsPropertyList")))
	{
		const TArray<TSharedPtr<FJsonValue>> & ArrayRef = InJsonObject->GetArrayField(TEXT("metalsPropertyList"));
		for (const auto & Ref : ArrayRef)
		{
			FMetalsProperty Pro;
			Pro.MetallId = Ref->AsObject()->GetStringField(TEXT("metalsId"));
			Pro.propertyId = Ref->AsObject()->GetStringField(TEXT("propertyId"));
			Pro.PropertyValueIdentity = Ref->AsObject()->GetStringField(TEXT("propertyValueIdentity"));
			Pro.propertyValue = Ref->AsObject()->GetStringField(TEXT("propertyValue"));
			MetalsPropertyList.Emplace(Pro);
		}
	}
}

void FAccessoryShape::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FDecorationShape::ParseContentFromJson(InJsonObject);
}

void FAccessoryShape::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//保存型录基本信息
	FDecorationShape::SaveToJson(JsonWriter);
	//产品编码
	JsonWriter->WriteValue(TEXT("ProduceCode"), GetProduceCode());
	// 品牌民称
	JsonWriter->WriteValue(TEXT("brandName"), GetBrandName());
	// 产品型号
	JsonWriter->WriteValue(TEXT("version"), GetProductVersion());
	if (MetalsPropertyList.Num() > 0)
	{
		JsonWriter->WriteArrayStart(TEXT("metalsPropertyList"));
		for (const auto & Ref : MetalsPropertyList)
		{
			JsonWriter->WriteObjectStart();
			JsonWriter->WriteValue(TEXT("metalsId"), Ref.MetallId);
			JsonWriter->WriteValue(TEXT("propertyId"), Ref.propertyId);
			JsonWriter->WriteValue(TEXT("propertyValueIdentity"), Ref.PropertyValueIdentity);
			JsonWriter->WriteValue(TEXT("propertyValue"), Ref.propertyValue);
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();
	}
	
}

void FAccessoryShape::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FDecorationShape::SaveAttriToJson(JsonWriter);
}

void FAccessoryShape::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FDecorationShape::SaveShapeToJson(JsonWriter);
	//产品编码	
	JsonWriter->WriteValue(TEXT("ProduceCode"), GetProduceCode());
	// 品牌民称
	JsonWriter->WriteValue(TEXT("brandName"), GetBrandName());
	// 产品型号
	JsonWriter->WriteValue(TEXT("version"), GetProductVersion());
	if (MetalsPropertyList.Num() > 0)
	{
		JsonWriter->WriteArrayStart(TEXT("metalsPropertyList"));
		for (const auto & Ref : MetalsPropertyList)
		{
			JsonWriter->WriteObjectStart();
			JsonWriter->WriteValue(TEXT("metalsId"), Ref.MetallId);
			JsonWriter->WriteValue(TEXT("propertyId"), Ref.propertyId);
			JsonWriter->WriteValue(TEXT("propertyValueIdentity"), Ref.PropertyValueIdentity);
			JsonWriter->WriteValue(TEXT("propertyValue"), Ref.propertyValue);
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();
	}
}

void FAccessoryShape::CopyTo(FSCTShape* OutShape)
{
	FDecorationShape::CopyTo(OutShape);
	
	FAccessoryShape* CurrentShahpe = StaticCast<FAccessoryShape*>(OutShape);
	CurrentShahpe->ProduceCode = ProduceCode;
	CurrentShahpe->BrandName = BrandName;
	CurrentShahpe->ProductVersion = ProductVersion;
	CurrentShahpe->MetalsPropertyList = MetalsPropertyList;
}

ASCTShapeActor* FAccessoryShape::SpawnShapeActor()
{
	return FDecorationShape::SpawnShapeActor();
}

void FAccessoryShape::SetCollisionProfileName(FName InProfileName)
{
	FDecorationShape::SetCollisionProfileName(InProfileName);
}

bool FAccessoryShape::SetShapeWidth(float InValue)
{
	return FDecorationShape::SetShapeWidth(InValue);
}
bool FAccessoryShape::SetShapeWidth(const FString &InStrValue)
{
	return FDecorationShape::SetShapeWidth(InStrValue);
}

bool FAccessoryShape::SetShapeDepth(float InValue)
{
	return FDecorationShape::SetShapeDepth(InValue);
}
bool FAccessoryShape::SetShapeDepth(const FString &InStrValue)
{
	return FDecorationShape::SetShapeDepth(InStrValue);
}

bool FAccessoryShape::SetShapeHeight(float InValue)
{
	return FDecorationShape::SetShapeHeight(InValue);
}
bool FAccessoryShape::SetShapeHeight(const FString &InStrValue)
{
	return FDecorationShape::SetShapeHeight(InStrValue);
}

bool FAccessoryShape::SetShapePosX(const FString &InStrValue)
{
	return FDecorationShape::SetShapePosX(InStrValue);
}

bool FAccessoryShape::SetShapePosY(const FString &InStrValue)
{
	return FDecorationShape::SetShapePosY(InStrValue);
}

bool FAccessoryShape::SetShapePosZ(const FString &InStrValue)
{
	return FDecorationShape::SetShapePosZ(InStrValue);
}

bool FAccessoryShape::SetShapePosX(float InValue)
{
	return FDecorationShape::SetShapePosX(InValue);
}

bool FAccessoryShape::SetShapePosY(float InValue)
{
	return FDecorationShape::SetShapePosY(InValue);
}

bool FAccessoryShape::SetShapePosZ(float InValue)
{
	return FDecorationShape::SetShapePosZ(InValue);
}

void FAccessoryShape::SetProduceCode(const FString& InCode)
{
	ProduceCode = InCode;
}
FString FAccessoryShape::GetProduceCode()
{
	return ProduceCode;
}

const FString FDividBlockShape::GetModelIdStr()
{
	FString TempModelId = FString::FromInt(GetModelID());
	if (GetModelID() == 0)
	{
		TempModelId = GetFileName();
	}
	return 	TempModelId;
}

ASCTShapeActor * FDividBlockShape::SpawnShapeActor()
{
	ASCTModelActor * ModelActor = StaticCast<ASCTModelActor*>(FDecorationShape::SpawnShapeActor());
	ModelActor->SetScaleFilter(ModelSacleFilter::SCALE_XZ);
	ModelActor->UpdateActorDimension();
	return ModelActor;
}

