
#include "SCTDoorSheet.h"
#include "Engine/StaticMesh.h"
#include "Components/MeshComponent.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"

#include "SCTModelShape.h"
#include "MaterialManager.h"
#include "SCTStaticMeshManager.h"
#include "Actor/SCTModelActor.h"
#include "Actor/SCTCompActor.h"
#include "Actor/SCTDoorSheetActor.h"
#include "SCTShapeManager.h"
#include "SCTAttribute.h"
#include "SCTGlobalDataForShape.h"
#include "Materials/MaterialInstanceDynamic.h"

void FDoorSheetShapeBase::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseFromJson(InJsonObject);

	// 封边
	if (InJsonObject->HasField(TEXT("edgeBanding")))
	{
		const TSharedPtr<FJsonObject> TempEdgeBandingObj = InJsonObject->GetObjectField(TEXT("edgeBanding"));
		FDoorPakMetaData & PakRef = GetEdgeBanding();
		PakRef.ID = TempEdgeBandingObj->GetIntegerField(TEXT("id"));
		PakRef.Name = TempEdgeBandingObj->GetStringField(TEXT("name"));
		PakRef.Url = TempEdgeBandingObj->GetStringField(TEXT("pakUrl"));
		PakRef.MD5 = TempEdgeBandingObj->GetStringField(TEXT("pakMd5"));
		PakRef.ThumbnailUrl = TempEdgeBandingObj->GetStringField(TEXT("thumbnailUrl"));
	}
	
	// 材质
	if (InJsonObject->HasField(TEXT("material")))
	{
		const TSharedPtr<FJsonObject> TempMaterialObj = InJsonObject->GetObjectField(TEXT("material"));
		FDoorPakMetaData & PakRef = GetMaterial();
		PakRef.ID = TempMaterialObj->GetIntegerField(TEXT("id"));
		PakRef.Name = TempMaterialObj->GetStringField(TEXT("name"));
		PakRef.Url = TempMaterialObj->GetStringField(TEXT("pakUrl"));
		PakRef.MD5 = TempMaterialObj->GetStringField(TEXT("pakMd5"));
		PakRef.ThumbnailUrl = TempMaterialObj->GetStringField(TEXT("thumbnailUrl"));
		PakRef.OptimizeParam = TempMaterialObj->GetStringField(TEXT("optimizeParam"));
	}
	if (InJsonObject->HasField(TEXT("hasMaterialReplace")))
	{
		bHasOptionalMaterial = InJsonObject->GetBoolField(TEXT("hasMaterialReplace"));
	}
	
	// 基材
	if (InJsonObject->HasField(TEXT("substrate")))
	{
		const TSharedPtr<FJsonObject> TempSubstrateObj = InJsonObject->GetObjectField(TEXT("substrate"));
		SetSubstrateID(TempSubstrateObj->GetIntegerField(TEXT("id")));
		SetSubstrateName(TempSubstrateObj->GetStringField(TEXT("name")));
	}

	TArray<int32> ChildrenIds;
	if (InJsonObject->HasField(TEXT("shapes")))
	{
		const TArray<TSharedPtr<FJsonValue>> & TempShapes = InJsonObject->GetArrayField(TEXT("shapes"));
		for (const auto & Ref : TempShapes)
		{
			const int32 FindIndex = FilterParseMetalType.Find(StaticCast<EMetalsType>(Ref->AsObject()->GetIntegerField(TEXT("category"))));
			if (FindIndex == INDEX_NONE) continue;
			TSharedPtr<FSCTShape> Shape = FSCTShapeManager::Get()->ParseShapeFromJson(Ref->AsObject());
			FSCTShapeManager::Get()->AddChildShape(Shape);
			int32 ID = Ref->AsObject()->GetIntegerField(TEXT("id"));
			OpetionsMetalsShapesList.Add(ID, Shape);
			if (StaticCast<EShapeType>(Ref->AsObject()->GetIntegerField(TEXT("type"))) != ST_Accessory) continue;
			if (ChildrenIds.FindByKey(ID) == nullptr)
			{
				FAccessoryToDownload TempAccessoryToDownload;
				TempAccessoryToDownload.ModelID = Ref->AsObject()->GetIntegerField(TEXT("modelId"));
				TempAccessoryToDownload.Md5 = Ref->AsObject()->GetStringField(TEXT("fileMd5"));
				TempAccessoryToDownload.Url = Ref->AsObject()->GetStringField(TEXT("fileUrl"));
				TempAccessoryToDownload.UpdateTime = Ref->AsObject()->GetNumberField(TEXT("updateTime"));
				TempAccessoryToDownload.OptimizeParam = InJsonObject->GetStringField(TEXT("optimizeParam"));
				ToDownloadAccessoryList.Emplace(MoveTemp(TempAccessoryToDownload));
			}
		}
	}

	// 五金 
	if (InJsonObject->HasField(TEXT("children")))
	{
		const TArray<TSharedPtr<FJsonValue>> & TempChildren = InJsonObject->GetArrayField(TEXT("children"));
		for (const auto & Ref : TempChildren)
		{
			const TSharedPtr<FJsonObject> & Obj = Ref->AsObject();			
			const int32 ShapeId = Obj->GetIntegerField(TEXT("id"));	
			if (OpetionsMetalsShapesList.Find(ShapeId) == nullptr)
			{
				continue;
			}
			TSharedPtr<FSCTShape> NewShape = FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ST_Accessory, ShapeId);
			TSharedPtr<FAccessoryShape> CopyShape = StaticCastSharedPtr<FAccessoryShape>(FSCTShapeManager::Get()->CopyShapeToNew(NewShape));
			AddMetal(CopyShape);		
			CopyShape->ParseAttributesFromJson(Obj);
		}
	}
	ProcessOptionsMetalType();
}

void FDoorSheetShapeBase::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseAttributesFromJson(InJsonObject);
	// 封边
	if (InJsonObject->HasField(TEXT("edgeBanding")))
	{
		const TSharedPtr<FJsonObject> TempEdgeBandingObj = InJsonObject->GetObjectField(TEXT("edgeBanding"));
		FDoorPakMetaData & PakRef = GetEdgeBanding();
		PakRef.ID = TempEdgeBandingObj->GetIntegerField(TEXT("id"));
		PakRef.Name = TempEdgeBandingObj->GetStringField(TEXT("name"));
		PakRef.Url = TempEdgeBandingObj->GetStringField(TEXT("pakUrl"));
		PakRef.MD5 = TempEdgeBandingObj->GetStringField(TEXT("pakMd5"));
		PakRef.ThumbnailUrl = TempEdgeBandingObj->GetStringField(TEXT("thumbnailUrl"));
	}

	// 材质
	if (InJsonObject->HasField(TEXT("material")))
	{
		const TSharedPtr<FJsonObject> TempMaterialObj = InJsonObject->GetObjectField(TEXT("material"));
		FDoorPakMetaData & PakRef = GetMaterial();
		PakRef.ID = TempMaterialObj->GetIntegerField(TEXT("id"));
		PakRef.Name = TempMaterialObj->GetStringField(TEXT("name"));
		PakRef.Url = TempMaterialObj->GetStringField(TEXT("pakUrl"));
		PakRef.MD5 = TempMaterialObj->GetStringField(TEXT("pakMd5"));
		PakRef.ThumbnailUrl = TempMaterialObj->GetStringField(TEXT("thumbnailUrl"));
		PakRef.OptimizeParam = TempMaterialObj->GetStringField(TEXT("optimizeParam"));
	}

	// 基材
	if (InJsonObject->HasField(TEXT("substrate")))
	{
		const TSharedPtr<FJsonObject> TempSubstrateObj = InJsonObject->GetObjectField(TEXT("substrate"));
		SetSubstrateID(TempSubstrateObj->GetIntegerField(TEXT("id")));
		SetSubstrateName(TempSubstrateObj->GetStringField(TEXT("name")));
	}
	if (InJsonObject->HasField(TEXT("hasMaterialReplace")))
	{
		bHasOptionalMaterial = InJsonObject->GetBoolField(TEXT("hasMaterialReplace"));
	}
	// 五金 
	if (InJsonObject->HasField(TEXT("children")))
	{
		const TArray<TSharedPtr<FJsonValue>> & TempChildren = InJsonObject->GetArrayField(TEXT("children"));
		for (const auto & Ref : TempChildren)
		{
			const TSharedPtr<FJsonObject> & Obj = Ref->AsObject();			
			const int32 ShapeId = Obj->GetIntegerField(TEXT("id"));
			if (OpetionsMetalsShapesList.Find(ShapeId) == nullptr || Obj->GetIntegerField(TEXT("type")) != ST_Accessory)
			{
				continue;
			}
			TSharedPtr<FSCTShape> NewShape = FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ST_Accessory, ShapeId);
			TSharedPtr<FAccessoryShape> CopyShape = StaticCastSharedPtr<FAccessoryShape>(FSCTShapeManager::Get()->CopyShapeToNew(NewShape));
			AddMetal(CopyShape);
			CopyShape->ParseAttributesFromJson(Obj);
		}
	}
	ProcessOptionsMetalType();
}

void FDoorSheetShapeBase::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseShapeFromJson(InJsonObject);	
	TArray<int32> ChildrenIds;
	
	if (InJsonObject->HasField(TEXT("shapes")))
	{
		const TArray<TSharedPtr<FJsonValue>> & TempShapes = InJsonObject->GetArrayField(TEXT("shapes"));
		for (const auto & Ref : TempShapes)
		{
			const int32 FindIndex = FilterParseMetalType.Find(StaticCast<EMetalsType>(Ref->AsObject()->GetIntegerField(TEXT("category"))));
			if (FindIndex == INDEX_NONE) continue;
			TSharedPtr<FSCTShape> Shape = FSCTShapeManager::Get()->ParseShapeFromJson(Ref->AsObject());
			FSCTShapeManager::Get()->AddChildShape(Shape);
			OpetionsMetalsShapesList.Add(Ref->AsObject()->GetIntegerField(TEXT("id")), Shape);
			if (StaticCast<EShapeType>(Ref->AsObject()->GetIntegerField(TEXT("type"))) != ST_Accessory) continue;
			const int32 ID = Ref->AsObject()->GetIntegerField(TEXT("id"));
			if (ChildrenIds.FindByKey(ID) == nullptr)
			{
				FAccessoryToDownload TempAccessoryToDownload;
				TempAccessoryToDownload.ModelID = Ref->AsObject()->GetIntegerField(TEXT("modelId"));
				TempAccessoryToDownload.Md5 = Ref->AsObject()->GetStringField(TEXT("fileMd5"));
				TempAccessoryToDownload.Url = Ref->AsObject()->GetStringField(TEXT("fileUrl"));
				TempAccessoryToDownload.UpdateTime = Ref->AsObject()->GetNumberField(TEXT("updateTime"));
				TempAccessoryToDownload.OptimizeParam = InJsonObject->GetStringField(TEXT("optimizeParam"));
				ToDownloadAccessoryList.Emplace(MoveTemp(TempAccessoryToDownload));
			}
		}
	}
}

void FDoorSheetShapeBase::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FSCTShape::ParseContentFromJson(InJsonObject);
}

void FDoorSheetShapeBase::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveToJson(JsonWriter);
	// 封边
	{
		JsonWriter->WriteObjectStart(TEXT("edgeBanding"));
		const FDoorPakMetaData & PakRef = GetEdgeBanding();
		JsonWriter->WriteValue(TEXT("id"), PakRef.ID);
		JsonWriter->WriteValue(TEXT("name"), PakRef.Name);
		JsonWriter->WriteValue(TEXT("pakUrl"), PakRef.Url);
		JsonWriter->WriteValue(TEXT("pakMd5"), PakRef.MD5);
		JsonWriter->WriteValue(TEXT("thumbnailUrl"), PakRef.ThumbnailUrl);
		JsonWriter->WriteObjectEnd();
	}
	// 基材
	{
		JsonWriter->WriteObjectStart(TEXT("substrate"));
		JsonWriter->WriteValue(TEXT("id"), GetSubstrateID());
		JsonWriter->WriteValue(TEXT("name"), GetSubstrateName());
		JsonWriter->WriteObjectEnd();
	}
	// 材质
	{
		JsonWriter->WriteObjectStart(TEXT("material"));
		const FDoorPakMetaData & PakRef = GetMaterial();
		JsonWriter->WriteValue(TEXT("id"), PakRef.ID);
		JsonWriter->WriteValue(TEXT("name"), PakRef.Name);
		JsonWriter->WriteValue(TEXT("pakUrl"), PakRef.Url);
		JsonWriter->WriteValue(TEXT("pakMd5"), PakRef.MD5);
		JsonWriter->WriteValue(TEXT("thumbnailUrl"), PakRef.ThumbnailUrl);
		JsonWriter->WriteValue(TEXT("optimizeParam"), PakRef.OptimizeParam);		
		JsonWriter->WriteObjectEnd();
	}	
	JsonWriter->WriteValue(TEXT("hasMaterialReplace"),bHasOptionalMaterial);
	// 存储所有的Shapes
	JsonWriter->WriteArrayStart(TEXT("shapes"));
	for (const auto & Ref : OpetionsMetalsShapesList)
	{				
		JsonWriter->WriteObjectStart();
		Ref.Value->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	// 存储门所用到的五金
	JsonWriter->WriteArrayStart(TEXT("children"));
	for (const auto & Ref : Metals)
	{
		JsonWriter->WriteObjectStart();
		Ref->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
}

void FDoorSheetShapeBase::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveAttriToJson(JsonWriter);
	// 封边
	{
		JsonWriter->WriteObjectStart(TEXT("edgeBanding"));
		const FDoorPakMetaData & PakRef = GetEdgeBanding();
		JsonWriter->WriteValue(TEXT("id"), PakRef.ID);
		JsonWriter->WriteValue(TEXT("name"), PakRef.Name);
		JsonWriter->WriteValue(TEXT("pakUrl"), PakRef.Url);
		JsonWriter->WriteValue(TEXT("pakMd5"), PakRef.MD5);
		JsonWriter->WriteValue(TEXT("thumbnailUrl"), PakRef.ThumbnailUrl);
		JsonWriter->WriteObjectEnd();
	}
	// 基材
	{
		JsonWriter->WriteObjectStart(TEXT("substrate"));
		JsonWriter->WriteValue(TEXT("id"), GetSubstrateID());
		JsonWriter->WriteValue(TEXT("name"), GetSubstrateName());
		JsonWriter->WriteObjectEnd();
	}
	// 材质
	{
		JsonWriter->WriteObjectStart(TEXT("material"));
		const FDoorPakMetaData & PakRef = GetMaterial();
		JsonWriter->WriteValue(TEXT("id"), PakRef.ID);
		JsonWriter->WriteValue(TEXT("name"), PakRef.Name);
		JsonWriter->WriteValue(TEXT("pakUrl"), PakRef.Url);
		JsonWriter->WriteValue(TEXT("pakMd5"), PakRef.MD5);
		JsonWriter->WriteValue(TEXT("thumbnailUrl"), PakRef.ThumbnailUrl);
		JsonWriter->WriteValue(TEXT("optimizeParam"), PakRef.OptimizeParam);
		JsonWriter->WriteObjectEnd();
	}	
	JsonWriter->WriteValue(TEXT("hasMaterialReplace"), bHasOptionalMaterial);
	// 存储门所用到的五金
	JsonWriter->WriteArrayStart(TEXT("children"));
	for (const auto & Ref : Metals)
	{		
		JsonWriter->WriteObjectStart();
		Ref->SaveAttriToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
}

void FDoorSheetShapeBase::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveShapeToJson(JsonWriter);
	// 存储所有的Shapes
	JsonWriter->WriteArrayStart(TEXT("shapes"));
	for (const auto & Ref : OpetionsMetalsShapesList)
	{
		JsonWriter->WriteObjectStart();
		Ref.Value->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

}

void FDoorSheetShapeBase::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FSCTShape::SaveContentToJson(JsonWriter);
}

void FDoorSheetShapeBase::CopyTo(FSCTShape * OutShape)
{
	FSCTShape::CopyTo(OutShape);
	FDoorSheetShapeBase* OutBaseShape = StaticCast<FDoorSheetShapeBase*>(OutShape);
	OutBaseShape->ToDownloadAccessoryList = ToDownloadAccessoryList;
	OutBaseShape->OpetionsMetalsShapesList = OpetionsMetalsShapesList;
	for (const auto & Ref : Metals)
	{
		TSharedPtr<FAccessoryShape> NewShape = MakeShareable(new FAccessoryShape);
		Ref->CopyTo(NewShape.Get());
		OutBaseShape->AddMetal(NewShape);
	}
	OutBaseShape->SetMaterial(GetMaterial());
	OutBaseShape->SetSubstrateID(this->GetSubstrateID());
	OutBaseShape->SetSubstrateName(this->GetSubstrateName());
	OutBaseShape->SetEdgeBanding(this->GetEdgeBanding());
	OutBaseShape->bHasOptionalMaterial = this->bHasOptionalMaterial;
}

ASCTShapeActor * FDoorSheetShapeBase::SpawnShapeActor()
{
	//创建生成型录Actor
	UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();
	ASCTDoorSheetActor* NewShapeActor = World->SpawnActor<ASCTDoorSheetActor>(
		ASCTDoorSheetActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);

	//设置型录位置和旋转
	FVector ActorPos(GetShapePosX(), GetShapePosY(), GetShapePosZ());
	NewShapeActor->SetActorPosition(ActorPos);
	FRotator ActorRot(GetShapeRotY(), GetShapeRotZ(), GetShapeRotX());
	NewShapeActor->SetActorRotator(ActorRot);

	//设置型录可见性
	bool bParentVisible = NewShapeActor->GetVisibleStateFromParent();
	NewShapeActor->SetActorHiddenInGame(!GetShowCondition() || !bParentVisible);

	//设置关联的Shape
	NewShapeActor->SetShape(this);

	ShapeActor = NewShapeActor;

	// 放置所有当前正在使用的五金
	for (auto & Ref : Metals)
	{		
		const int32 FindIndex = FilterSpawnMetalType.Find(StaticCast<EMetalsType>(Ref->GetShapeCategory()));
		if (FindIndex == INDEX_NONE) continue;
		Ref->LoadModelMesh();
		ASCTShapeActor * Actor = Ref->SpawnShapeActor();
		Actor->AttachToActorOverride(NewShapeActor,FAttachmentTransformRules::KeepRelativeTransform);
	}

	return NewShapeActor;
}

void FDoorSheetShapeBase::GetResourceUrls(TArray<FString>& OutResourceUrls)
{
	FSCTShape::GetResourceUrls(OutResourceUrls);
	for (const auto & Ref : ToDownloadAccessoryList)
	{
		if (Ref.Url.IsEmpty()) continue;
		OutResourceUrls.Add(Ref.Url);
	}
	if (!GetMaterial().Url.IsEmpty())
	{
		OutResourceUrls.Add(GetMaterial().Url);
	}
}

void FDoorSheetShapeBase::GetFileCachePaths(TArray<FString>& OutFileCachePaths)
{
	FSCTShape::GetFileCachePaths(OutFileCachePaths);
	for (const auto & Ref : ToDownloadAccessoryList)
	{
		if (Ref.Url.IsEmpty()) continue;
		OutFileCachePaths.Add(FDividBlockShape::GetCacheFilePathFromIDAndMd5AndUpdateTime(Ref.ModelID, Ref.Md5, Ref.UpdateTime));
	}
	// 材质
	if (!GetMaterial().Url.IsEmpty())
	{
		const FString  RelativePath = FString::Printf(TEXT("Material/Board/BoardMaterial_%d.pak"), GetMaterial().ID);
		OutFileCachePaths.Emplace(
			FPaths::Combine(FGlobalDataForShape::GetInsPtr()->GetRootDir(), RelativePath)
		);
	}
}

void FDoorSheetShapeBase::SetCollisionProfileName(FName InProfileName)
{
	for (auto & Ref : Metals)
	{
		Ref->SetCollisionProfileName(InProfileName);
    }
}

int32 FDoorSheetShapeBase::GetOptionMetalsIdByNameAndType(const FString & InName, const EMetalsType InType)
{
	int32 RetValue = INDEX_NONE;
	for (const auto & Ref : OpetionsMetalsShapesList)
	{
		if (Ref.Value->GetShapeCategory() == StaticCast<int32>(InType) && Ref.Value->GetShapeName() == InName)
		{
			RetValue = Ref.Key;
			break;
		}
	}
	return 	RetValue;
}

EMetalsType FDoorSheetShapeBase::GetMetalTypeByID(const int32 InId)
{
	EMetalsType RetValue = EMetalsType::MT_None;
	for (const auto & Ref : OpetionsMetalsShapesList)
	{
		if (Ref.Value->GetShapeId() == InId)
		{
			RetValue = StaticCast<EMetalsType>(Ref.Value->GetShapeCategory());
			break;
		}
	}
	return 	RetValue;
}

FString FDoorSheetShapeBase::GetMetalNameByID(const int32 InId)
{
	FString  RetValue;
	for (const auto & Ref : OpetionsMetalsShapesList)
	{
		if (Ref.Value->GetShapeId() == InId)
		{
			RetValue = Ref.Value->GetShapeName();
			break;
		}
	}
	return 	RetValue;
}

void FDoorSheetShapeBase::AddMetal(const TSharedPtr<FAccessoryShape>& InShape)
{
	Metals.Add(InShape);
	AddChildShape(InShape);
	InShape->SetParentShape(this);
}

FSpaceDividDoorSheet::FSpaceDividDoorSheet()
{
	SetShapeType(ST_SpaceDividDoorSheet);
}

void FSpaceDividDoorSheet::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FDoorSheetShapeBase::ParseFromJson(InJsonObject);
	// 类型修订
	SetShapeType(ST_SpaceDividDoorSheet);
	// 弥补缺少的范围限定	
	{
		auto FixNumberRangeAttr = [](TSharedPtr<FShapeAttribute> & InAttri)
		{
			TSharedPtr<FNumberRangeAttri>  TempAttr = StaticCastSharedPtr<FNumberRangeAttri>(InAttri);
			TempAttr->SetMinValue(-10000.0f);
			TempAttr->SetMaxValue(10000.0f);
			TempAttr->SetAttributeValue(TEXT("0.0"));
		};
		FixNumberRangeAttr(PositionX);
		FixNumberRangeAttr(PositionY);
		FixNumberRangeAttr(PositionZ);
	}
	const TArray<TSharedPtr<FJsonValue>> & SectionMaterialRef = InJsonObject->GetArrayField(TEXT("sectionalMaterials"));
	for (const auto & Ref : SectionMaterialRef)
	{
		TSharedPtr<FSectionalMaterial> SectionalMaterial = MakeShareable(new FSectionalMaterial);
		SectionalMaterial->Id = Ref->AsObject()->GetIntegerField(TEXT("id"));
		SectionalMaterial->Type = StaticCast<FSectionalMaterial::ESectionalMaterialType>(Ref->AsObject()->GetIntegerField(TEXT("type")));
		SectionalMaterial->DisplayMode = StaticCast<FSectionalMaterial::EDisplayModel>(Ref->AsObject()->GetIntegerField(TEXT("displayMode")));
		SectionalMaterial->PakUrl = Ref->AsObject()->GetStringField(TEXT("pakUrl"));
		SectionalMaterial->Md5 = Ref->AsObject()->GetStringField(TEXT("pakMd5"));
		SectionalMaterial->OptimizeParm = Ref->AsObject()->GetStringField(TEXT("optimizeParam"));
		SectionalMaterial->IsShutter = (SectionalMaterial->Type == FSectionalMaterial::ESectionalMaterialType::Louver_Core);
		if (Ref->AsObject()->HasTypedField<EJson::Object>(TEXT("propertyValue")))
		{
			const TSharedPtr<FJsonObject> & propertyValuesJsonObjRef = Ref->AsObject()->GetObjectField(TEXT("propertyValue"));
			const FSectionalMaterial::EProsType SectionMaterialProType = SectionalMaterial->GetProsType();
			if (SectionMaterialProType == FSectionalMaterial::EProsType::ETopAndBottomH)
			{
				SectionalMaterial->TopAndBottomSectionSizePros.Height = propertyValuesJsonObjRef->GetNumberField(TEXT("height")) / 10.0f;
				SectionalMaterial->TopAndBottomSectionSizePros.Depth = propertyValuesJsonObjRef->GetNumberField(TEXT("depth")) / 10.0f;
				SectionalMaterial->TopAndBottomSectionSizePros.BayonetHeight = propertyValuesJsonObjRef->GetNumberField(TEXT("bayonetHeight")) / 10.0f;
				SectionalMaterial->TopAndBottomSectionSizePros.BayonetDepth = propertyValuesJsonObjRef->GetNumberField(TEXT("bayonetDepth")) / 10.0f;
			}
			else if (SectionMaterialProType == FSectionalMaterial::EProsType::ELeftAndRightV)
			{
				SectionalMaterial->LeftAndRightSectionSizePros.Width = propertyValuesJsonObjRef->GetNumberField(TEXT("width")) / 10.0f;
				SectionalMaterial->LeftAndRightSectionSizePros.Depth = propertyValuesJsonObjRef->GetNumberField(TEXT("depth")) / 10.0f;
				SectionalMaterial->LeftAndRightSectionSizePros.BayonetHeight = propertyValuesJsonObjRef->GetNumberField(TEXT("bayonetHeight")) / 10.0f;
				SectionalMaterial->LeftAndRightSectionSizePros.BayonetDepth = propertyValuesJsonObjRef->GetNumberField(TEXT("bayonetDepth")) / 10.0f;
			}
			else if(SectionMaterialProType == FSectionalMaterial::EProsType::EInside)
			{
				SectionalMaterial->InSideSectionSizePros.Height = propertyValuesJsonObjRef->GetNumberField(TEXT("height")) / 10.0f;
				SectionalMaterial->InSideSectionSizePros.Depth = propertyValuesJsonObjRef->GetNumberField(TEXT("depth")) / 10.0f;
				SectionalMaterial->InSideSectionSizePros.BayonetDepth = propertyValuesJsonObjRef->GetNumberField(TEXT("bayonetDepth")) / 10.0f;
				SectionalMaterial->InSideSectionSizePros.UpBayonetHeight = propertyValuesJsonObjRef->GetNumberField(TEXT("upBayonetHeight")) / 10.0f;
				SectionalMaterial->InSideSectionSizePros.DownBayonetHeight = propertyValuesJsonObjRef->GetNumberField(TEXT("downBayonetHeight")) / 10.0f;
			}
		}
		SectionalMaterials.Add(TPair<int32, int32>(SectionalMaterial->Id, StaticCast<int32>(SectionalMaterial->Type)), SectionalMaterial);
		TSharedPtr<FMeshManager> StaticMeshMrg = FSCTShapeManager::Get()->GetStaticMeshManager();
		const FString CachePath = GetCachePath(SectionalMaterial->Id, SectionalMaterial->Md5);
		SectionalMaterial->CacheFilePath = CachePath;
	}
	RootBlock = MakeShareable(new FSpaceDivid);
	RootBlock->ParseInfoFromJson(InJsonObject->GetObjectField(TEXT("divideBlocks")));
}

void FSpaceDividDoorSheet::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FDoorSheetShapeBase::ParseAttributesFromJson(InJsonObject);
}

void FSpaceDividDoorSheet::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FDoorSheetShapeBase::ParseFromJson(InJsonObject);
	const TArray<TSharedPtr<FJsonValue>> & SectionMaterialRef = InJsonObject->GetArrayField(TEXT("sectionalMaterials"));
	for (const auto & Ref : SectionMaterialRef)
	{
		TSharedPtr<FSectionalMaterial> SectionalMaterial = MakeShareable(new FSectionalMaterial);
		SectionalMaterial->Id = Ref->AsObject()->GetIntegerField(TEXT("id"));
		SectionalMaterial->Type = StaticCast<FSectionalMaterial::ESectionalMaterialType>(Ref->AsObject()->GetIntegerField(TEXT("type")));
		SectionalMaterial->DisplayMode = StaticCast<FSectionalMaterial::EDisplayModel>(Ref->AsObject()->GetIntegerField(TEXT("displayMode")));
		SectionalMaterial->PakUrl = Ref->AsObject()->GetStringField(TEXT("pakUrl"));
		SectionalMaterial->Md5 = Ref->AsObject()->GetStringField(TEXT("pakMd5"));
		SectionalMaterial->OptimizeParm = Ref->AsObject()->GetStringField(TEXT("optimizeParam"));
		SectionalMaterial->IsShutter = (SectionalMaterial->Type == FSectionalMaterial::ESectionalMaterialType::Louver_Core);
		if (Ref->AsObject()->HasTypedField<EJson::Object>(TEXT("propertyValue")))
		{
			const TSharedPtr<FJsonObject> & propertyValuesJsonObjRef = Ref->AsObject()->GetObjectField(TEXT("propertyValue"));
			const FSectionalMaterial::EProsType SectionMaterialProType = SectionalMaterial->GetProsType();
			if (SectionMaterialProType == FSectionalMaterial::EProsType::ETopAndBottomH)
			{
				SectionalMaterial->TopAndBottomSectionSizePros.Height = propertyValuesJsonObjRef->GetNumberField(TEXT("height")) / 10.0f;
				SectionalMaterial->TopAndBottomSectionSizePros.Depth = propertyValuesJsonObjRef->GetNumberField(TEXT("depth")) / 10.0f;
				SectionalMaterial->TopAndBottomSectionSizePros.BayonetHeight = propertyValuesJsonObjRef->GetNumberField(TEXT("bayonetHeight")) / 10.0f;
				SectionalMaterial->TopAndBottomSectionSizePros.BayonetDepth = propertyValuesJsonObjRef->GetNumberField(TEXT("bayonetDepth")) / 10.0f;
			}
			else if (SectionMaterialProType == FSectionalMaterial::EProsType::ELeftAndRightV)
			{
				SectionalMaterial->LeftAndRightSectionSizePros.Width = propertyValuesJsonObjRef->GetNumberField(TEXT("width")) / 10.0f;
				SectionalMaterial->LeftAndRightSectionSizePros.Depth = propertyValuesJsonObjRef->GetNumberField(TEXT("depth")) / 10.0f;
				SectionalMaterial->LeftAndRightSectionSizePros.BayonetHeight = propertyValuesJsonObjRef->GetNumberField(TEXT("bayonetHeight")) / 10.0f;
				SectionalMaterial->LeftAndRightSectionSizePros.BayonetDepth = propertyValuesJsonObjRef->GetNumberField(TEXT("bayonetDepth")) / 10.0f;
			}
			else if (SectionMaterialProType == FSectionalMaterial::EProsType::EInside)
			{
				SectionalMaterial->InSideSectionSizePros.Height = propertyValuesJsonObjRef->GetNumberField(TEXT("height")) / 10.0f;
				SectionalMaterial->InSideSectionSizePros.Depth = propertyValuesJsonObjRef->GetNumberField(TEXT("depth")) / 10.0f;
				SectionalMaterial->InSideSectionSizePros.BayonetDepth = propertyValuesJsonObjRef->GetNumberField(TEXT("bayonetDepth")) / 10.0f;
				SectionalMaterial->InSideSectionSizePros.UpBayonetHeight = propertyValuesJsonObjRef->GetNumberField(TEXT("upBayonetHeight")) / 10.0f;
				SectionalMaterial->InSideSectionSizePros.DownBayonetHeight = propertyValuesJsonObjRef->GetNumberField(TEXT("downBayonetHeight")) / 10.0f;
			}
		}
		SectionalMaterials.Add(TPair<int32, int32>(SectionalMaterial->Id, StaticCast<int32>(SectionalMaterial->Type)), SectionalMaterial);
		TSharedPtr<FMeshManager> StaticMeshMrg = FSCTShapeManager::Get()->GetStaticMeshManager();
		const FString CachePath = GetCachePath(SectionalMaterial->Id, SectionalMaterial->Md5);
		SectionalMaterial->CacheFilePath = CachePath;
	}
	RootBlock = MakeShareable(new FSpaceDivid);
	RootBlock->ParseInfoFromJson(InJsonObject->GetObjectField(TEXT("divideBlocks")));
}

void FSpaceDividDoorSheet::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FDoorSheetShapeBase::ParseContentFromJson(InJsonObject);
}

void FSpaceDividDoorSheet::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FDoorSheetShapeBase::SaveToJson(JsonWriter);

	JsonWriter->WriteArrayStart(TEXT("sectionalMaterials"));
	for (const auto & Ref : SectionalMaterials)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue(TEXT("id"), Ref.Value->Id);
		JsonWriter->WriteValue(TEXT("type"), StaticCast<int32>(Ref.Value->Type));
		JsonWriter->WriteValue(TEXT("displayMode"), StaticCast<int32>(Ref.Value->DisplayMode));
		JsonWriter->WriteValue(TEXT("pakUrl"), Ref.Value->PakUrl);
		JsonWriter->WriteValue(TEXT("pakMd5"), Ref.Value->Md5);
		JsonWriter->WriteValue(TEXT("optimizeParam"), Ref.Value->OptimizeParm);
		const FSectionalMaterial::EProsType SectionMaterialProType = Ref.Value->GetProsType();
		if (SectionMaterialProType == FSectionalMaterial::EProsType::ETopAndBottomH &&
			Ref.Value->TopAndBottomSectionSizePros.IsValid())
		{
			JsonWriter->WriteObjectStart(TEXT("propertyValue"));
			JsonWriter->WriteValue(TEXT("height"), Ref.Value->TopAndBottomSectionSizePros.Height * 10.0f);
			JsonWriter->WriteValue(TEXT("depth"), Ref.Value->TopAndBottomSectionSizePros.Depth * 10.0f);
			JsonWriter->WriteValue(TEXT("bayonetHeight"), Ref.Value->TopAndBottomSectionSizePros.BayonetHeight * 10.0f);
			JsonWriter->WriteValue(TEXT("bayonetDepth"), Ref.Value->TopAndBottomSectionSizePros.BayonetDepth * 10.0f);
			JsonWriter->WriteObjectEnd();
		}
		else if(SectionMaterialProType == FSectionalMaterial::EProsType::ELeftAndRightV &&
			Ref.Value->LeftAndRightSectionSizePros.IsValid())		
		{
			JsonWriter->WriteObjectStart(TEXT("propertyValue"));			
			JsonWriter->WriteValue(TEXT("width"), Ref.Value->LeftAndRightSectionSizePros.Width * 10.0f);			
			JsonWriter->WriteValue(TEXT("depth"), Ref.Value->LeftAndRightSectionSizePros.Depth * 10.0f);
			JsonWriter->WriteValue(TEXT("bayonetDepth"), Ref.Value->LeftAndRightSectionSizePros.BayonetDepth * 10.0f);
			JsonWriter->WriteValue(TEXT("bayonetHeight"), Ref.Value->LeftAndRightSectionSizePros.BayonetHeight * 10.0f);			
			JsonWriter->WriteObjectEnd();
		}	
		else if (SectionMaterialProType == FSectionalMaterial::EProsType::EInside &&
			Ref.Value->InSideSectionSizePros.IsValid())
		{
			JsonWriter->WriteObjectStart(TEXT("propertyValue"));
			JsonWriter->WriteValue(TEXT("height"), Ref.Value->InSideSectionSizePros.Height * 10.0f);
			JsonWriter->WriteValue(TEXT("depth"), Ref.Value->InSideSectionSizePros.Depth * 10.0f);
			JsonWriter->WriteValue(TEXT("bayonetDepth"), Ref.Value->InSideSectionSizePros.BayonetDepth * 10.0f);
			JsonWriter->WriteValue(TEXT("upBayonetHeight"), Ref.Value->InSideSectionSizePros.UpBayonetHeight * 10.0f);
			JsonWriter->WriteValue(TEXT("downBayonetHeight"), Ref.Value->InSideSectionSizePros.DownBayonetHeight * 10.0f);
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteObjectStart(TEXT("divideBlocks"));
	RootBlock->SaveInfoToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();
}

void FSpaceDividDoorSheet::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FDoorSheetShapeBase::SaveAttriToJson(JsonWriter);
}

void FSpaceDividDoorSheet::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	SaveToJson(JsonWriter);
}

void FSpaceDividDoorSheet::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FDoorSheetShapeBase::SaveContentToJson(JsonWriter);
}

ASCTShapeActor * FSpaceDividDoorSheet::SpawnShapeActor()
{	
	ASCTShapeActor * RetActor = FDoorSheetShapeBase::SpawnShapeActor();
	// 计算所有型材的尺寸
	for (auto & Ref : SectionalMaterials)
	{
		FSCTShapeManager::Get()->GetStaticMeshManager()->UpLoadStaticMesh(Ref.Value->PakUrl, Ref.Value->CacheFilePath, Ref.Value->OptimizeParm);
		const FMeshDataEntry * MeshData = FSCTShapeManager::Get()->GetStaticMeshManager()->FindMesh(Ref.Value->CacheFilePath);
		check(MeshData);
		Ref.Value->Width = MeshData->MeshData.Mesh->GetBoundingBox().GetSize().X;
		Ref.Value->Height = MeshData->MeshData.Mesh->GetBoundingBox().GetSize().Z;
		Ref.Value->Depth = MeshData->MeshData.Mesh->GetBoundingBox().GetSize().Y;
	}

	auto SpawnStaticMeshActor = [RetActor](const TSharedPtr<FSectionalMaterial> & InOuterSectionMaterial, FSpaceDivid & InBlock)
	{
		UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();
		ASCTModelActor * NewModelActor = World->SpawnActor<ASCTModelActor>(
			ASCTModelActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
		InBlock.StaticMeshActors.Add(NewModelActor);
		check(InOuterSectionMaterial->CacheFilePath.IsEmpty() == false);
		const FMeshDataEntry * MeshData = FSCTShapeManager::Get()->GetStaticMeshManager()->FindMesh(InOuterSectionMaterial->CacheFilePath);
		check(MeshData && MeshData->Type == 0);
		NewModelActor->SetStaticMesh(MeshData->MeshData.Mesh, MeshData->MaterialList);
		NewModelActor->AttachToActor(RetActor, FAttachmentTransformRules::KeepRelativeTransform);
	};

	// 使用边框搭建外轮廓
	// 左竖
	const TSharedPtr<FSectionalMaterial> LeftV = GetLeftV();
	SpawnStaticMeshActor(LeftV, *RootBlock);
	// 右竖
	const TSharedPtr<FSectionalMaterial> RightV = GetRightV();
	SpawnStaticMeshActor(RightV, *RootBlock);
	// 上横
	const TSharedPtr<FSectionalMaterial> TopH = GetTopH();
	SpawnStaticMeshActor(TopH, *RootBlock);
	// 下横
	const TSharedPtr<FSectionalMaterial> BottomH = GetBottomH();
	SpawnStaticMeshActor(BottomH, *RootBlock);
	// 动态生成子块
	RootBlock->SpawnActor(RetActor, *this);
	ReCalDividBlocksPositionAndSize();
	return 	RetActor;
}

void FSpaceDividDoorSheet::GetResourceUrls(TArray<FString>& OutResourceUrls)
{
	FDoorSheetShapeBase::GetResourceUrls(OutResourceUrls);
	for (const auto & Ref : SectionalMaterials)
	{
		OutResourceUrls.Add(Ref.Value->PakUrl);
	}
}

void FSpaceDividDoorSheet::GetFileCachePaths(TArray<FString>& OutFileCachePaths)
{
	FDoorSheetShapeBase::GetFileCachePaths(OutFileCachePaths);
	for (const auto & Ref : SectionalMaterials)
	{
		OutFileCachePaths.Add(Ref.Value->CacheFilePath);
	}
}

void FSpaceDividDoorSheet::SetCollisionProfileName(FName InProfileName)
{
	FDoorSheetShapeBase::SetCollisionProfileName(InProfileName);
	RootBlock->SetCollisionProfileName(InProfileName);
}

void FSpaceDividDoorSheet::CopyTo(FSCTShape * OutShape)
{
	check(OutShape->GetShapeType() == ST_SpaceDividDoorSheet);
	FDoorSheetShapeBase::CopyTo(OutShape);
	FSpaceDividDoorSheet * Shape = StaticCast<FSpaceDividDoorSheet*>(OutShape);
	Shape->IsInner = this->IsInner;
	Shape->SectionalMaterials = this->SectionalMaterials;
	Shape->ModelDepth = this->ModelDepth;
	Shape->RootBlock = this->RootBlock->CloneDividBlock();
}

void FSpaceDividDoorSheet::ReCalDividBlocksPositionAndSize()
{
	RootBlock->ResetDividPosition();
	const FVector Offset = { RootBlock->PosX,RootBlock->PosY,RootBlock->PosZ };
	// 外包围框
	check(RootBlock->StaticMeshActors.Num() > 4);

	// 左竖
	const TSharedPtr<FSectionalMaterial> & LeftV = GetLeftV();
	RootBlock->StaticMeshActors[0]->SetActorRelativeScale3D(FVector(1.0f, 1.0f, GetShapeHeight() / 10.0f / LeftV->Height));
	RootBlock->StaticMeshActors[0]->SetActorRelativeLocation(FVector::ZeroVector + Offset);

	// 右竖
	const TSharedPtr<FSectionalMaterial> & RightV = GetRightV();
	RootBlock->StaticMeshActors[1]->SetActorRelativeScale3D(FVector(-1.0f, 1.0f, GetShapeHeight() / 10.0f / RightV->Height));
	RootBlock->StaticMeshActors[1]->SetActorRelativeLocation(FVector(GetShapeWidth() / 10.0f,0.0f, 0.0f) + Offset);

	// 上横
	const TSharedPtr<FSectionalMaterial> & TopH = GetTopH();
	RootBlock->StaticMeshActors[2]->SetActorRelativeScale3D(FVector(((GetShapeWidth() / 10.0f) - LeftV->GetCareSpace() - RightV->GetCareSpace()) / TopH->Width, 1.0f, 1.0f));
	const float TopAndDownHDepthOffset = LeftV->LeftAndRightSectionSizePros.IsValid() ? (LeftV->LeftAndRightSectionSizePros.Depth - TopH->Depth) / 2.0f : (LeftV->Depth - TopH->Depth) / 2.0f;
	RootBlock->StaticMeshActors[2]->SetActorRelativeLocation(FVector(LeftV->GetCareSpace(), TopAndDownHDepthOffset, (GetShapeHeight() / 10.0f - TopH->Height)) + Offset);

	// 下横
	const TSharedPtr<FSectionalMaterial> BottomH = GetBottomH();
	RootBlock->StaticMeshActors[3]->SetActorRelativeScale3D(FVector(((GetShapeWidth() / 10.0f) - LeftV->GetCareSpace() - RightV->GetCareSpace()) / BottomH->Width, 1.0f, 1.0f));	
	RootBlock->StaticMeshActors[3]->SetActorRelativeLocation(FVector(LeftV->GetCareSpace(), TopAndDownHDepthOffset, 0.0f) + Offset);

	RootBlock->PosX = RootBlock->PosX + LeftV->GetCareSpace();
	RootBlock->PosZ = RootBlock->PosZ + BottomH->GetCareSpace();
	RootBlock->Width = (GetShapeWidth() / 10.0f) - GetLeftV()->GetCareSpace() - GetRightV()->GetCareSpace();
	RootBlock->Height = (GetShapeHeight() / 10.0f) - GetTopH()->GetCareSpace() - GetBottomH()->GetCareSpace();
	RootBlock->RecalSizeAndPosition(*this);
}

float FSpaceDividDoorSheet::GetOrCalDoorTrueDepth()
{
	if (FMath::IsNearlyZero(ModelDepth))
	{
		for (const auto & Ref : SectionalMaterials)
		{
			if (Ref.Value->Depth * 10.0f < ModelDepth)
			{
				continue;
			}
			ModelDepth = Ref.Value->Depth * 10.0f;
		}
	}
	return ModelDepth;
}

const TSharedPtr<FSpaceDividDoorSheet::FSectionalMaterial>& FSpaceDividDoorSheet::GetCore(const int32 Index) const
{
	for (const auto & Ref : SectionalMaterials)
	{
		if (Ref.Key.Key == Index)
		{
			return Ref.Value;
		}
	}
	check(false);
	return SectionalMaterials[TPair<int32, int32>(0, 0)];
}

TTuple<TPair<float, float>, TPair<float, float>, TPair<float, float>, TPair<float, float>> 
FSpaceDividDoorSheet::GetSloatAndDepthOffset(const FSpaceDivid * InDivid) const
{	
	struct FGetExpectValue : public FNoncopyable
	{
	public:
		FGetExpectValue(const FSpaceDividDoorSheet & InParentDoor, const FSpaceDividDoorSheet::FSpaceDivid * InFindBase,			
			TTuple<TPair<float, float>, TPair<float, float>, TPair<float, float>, TPair<float, float>> & OutValues)
			: FindBase(InFindBase)
			, ExpectValues(OutValues)			
			, ParentDoor(InParentDoor)
			{}
		void GetSpaceDividParent(FSpaceDivid * InParentDivid,const FSpaceDivid * & InDesDivid, FSpaceDivid * & OutDivid)
		{
			for (const auto & Ref : InParentDivid->ChildrenBlocks)
			{				
				if (Ref.Get() == InDesDivid)
				{
					OutDivid = InParentDivid;
					return;
				}
				else if (Ref->DividDirection != 0)
				{
					GetSpaceDividParent(Ref.Get(), InDesDivid, OutDivid);
				}				
			}
		}
		void operator()()
		{
			auto SetSideSectionMaterialValue = [](const TSharedPtr<FSectionalMaterial> & InMaterial)->TPair<float,float>
			{
				const FSectionalMaterial::EProsType ProsType = InMaterial->GetProsType();
				float depthOffset = 0.0f;
				float slotDepth = 0.0f;
				if (ProsType == FSectionalMaterial::EProsType::ETopAndBottomH)
				{
					depthOffset = InMaterial->TopAndBottomSectionSizePros.BayonetDepth;
					slotDepth = InMaterial->TopAndBottomSectionSizePros.BayonetHeight;
				}
				else if (ProsType == FSectionalMaterial::EProsType::ELeftAndRightV)
				{
					depthOffset = InMaterial->LeftAndRightSectionSizePros.BayonetDepth;
					slotDepth = InMaterial->LeftAndRightSectionSizePros.BayonetHeight;
				}							
				return TPair<float, float>(depthOffset, slotDepth);
			};
			if (FindBase->DividDirection == 0)
			{		
				{
					const TSharedPtr<FSectionalMaterial> & TopH = ParentDoor.GetTopH();
					if (TopH->TopAndBottomSectionSizePros.IsValid())
					{
						ExpectValues.Get<0>() = SetSideSectionMaterialValue(TopH);
					}
				}
				{
					const TSharedPtr<FSectionalMaterial> & BottomH = ParentDoor.GetBottomH();
					if (BottomH->TopAndBottomSectionSizePros.IsValid())
					{
						ExpectValues.Get<1>() = SetSideSectionMaterialValue(BottomH);
					}
				}
				{
					const TSharedPtr<FSectionalMaterial> & LeftV = ParentDoor.GetLeftV();
					if (LeftV->LeftAndRightSectionSizePros.IsValid())
					{
						ExpectValues.Get<2>() = SetSideSectionMaterialValue(LeftV);
					}
				}
				{
					const TSharedPtr<FSectionalMaterial> & RightV = ParentDoor.GetRightV();
					if (RightV->LeftAndRightSectionSizePros.IsValid())
					{
						ExpectValues.Get<3>() = SetSideSectionMaterialValue(RightV);
					}
				}				
			}
			
			// 查找父节点
			FSpaceDivid * ParentDivid = nullptr;
			GetSpaceDividParent(ParentDoor.GetRootBlock().Get(), FindBase, ParentDivid);
			
			const bool bFirst = ParentDoor.GetRootBlock()->ChildrenBlocks.Num() == 0 ||
				ParentDivid->ChildrenBlocks[0].Get() == FindBase;
			const bool bLast = ParentDoor.GetRootBlock()->ChildrenBlocks.Num() == 0 ||
				ParentDivid->ChildrenBlocks[ParentDoor.GetRootBlock()->ChildrenBlocks.Num() - 1].Get() == FindBase;
			const bool bMiddle = ParentDoor.GetRootBlock()->ChildrenBlocks.Num() > 1 && !bFirst && !bLast;
			// 横向分割
			if (ParentDivid->DividDirection == 1)
			{				
				// 如果是第一个且非最后一个
				if (bFirst && !bLast)
				{			
					const TSharedPtr<FSectionalMaterial> & MiddleV = ParentDoor.GetMiddleV();
					if (MiddleV->InSideSectionSizePros.IsValid())
					{
						const float depthOffset = MiddleV->InSideSectionSizePros.BayonetDepth;
						const float slotDepth = MiddleV->InSideSectionSizePros.UpBayonetHeight;
						ExpectValues.Get<3>() = TPair<float, float>(depthOffset, slotDepth);
					}					
				}
				// 如果是中间
				if (bMiddle)
				{
					const TSharedPtr<FSectionalMaterial> & MiddleV = ParentDoor.GetMiddleV();
					if (MiddleV->InSideSectionSizePros.IsValid())
					{
						const float depthOffset = MiddleV->InSideSectionSizePros.BayonetDepth;
						const float slotUpDepth = MiddleV->InSideSectionSizePros.DownBayonetHeight;
						const float SlotDownDepth = MiddleV->InSideSectionSizePros.UpBayonetHeight;
						ExpectValues.Get<2>() = TPair<float, float>(depthOffset, slotUpDepth);
						ExpectValues.Get<3>() = TPair<float, float>(depthOffset, SlotDownDepth);
					}
				}

				// 如果是最后一个且非第一个
				if (bLast && !bFirst)
				{
					const TSharedPtr<FSectionalMaterial> & MiddleV = ParentDoor.GetMiddleV();
					if (MiddleV->InSideSectionSizePros.IsValid())
					{
						const float depthOffset = MiddleV->InSideSectionSizePros.BayonetDepth;
						const float slotDepth = MiddleV->InSideSectionSizePros.DownBayonetHeight;
						ExpectValues.Get<2>() = TPair<float, float>(depthOffset, slotDepth);
					}
				}
			}
			// 纵向分割
			else if (ParentDivid->DividDirection == 2)
			{
				// 如果是第一个且非最后一个
				if (bFirst && !bLast)
				{
					const TSharedPtr<FSectionalMaterial> & MiddleH = ParentDoor.GetMiddleH();
					if (MiddleH->InSideSectionSizePros.IsValid())
					{
						const float depthOffset = MiddleH->InSideSectionSizePros.BayonetDepth;
						const float slotDepth = MiddleH->InSideSectionSizePros.UpBayonetHeight;
						ExpectValues.Get<1>() = TPair<float, float>(depthOffset, slotDepth);
					}
				}
				// 如果是中间
				if (bMiddle)
				{
					const TSharedPtr<FSectionalMaterial> & MiddleH = ParentDoor.GetMiddleH();
					if (MiddleH->InSideSectionSizePros.IsValid())
					{
						const float depthOffset = MiddleH->InSideSectionSizePros.BayonetDepth;
						const float slotUpDepth = MiddleH->InSideSectionSizePros.DownBayonetHeight;
						const float SlotDownDepth = MiddleH->InSideSectionSizePros.UpBayonetHeight;
						ExpectValues.Get<0>() = TPair<float, float>(depthOffset, slotUpDepth);
						ExpectValues.Get<1>() = TPair<float, float>(depthOffset, SlotDownDepth);
					}
				}

				// 如果是最后一个且非第一个
				if (bLast && !bFirst)
				{
					const TSharedPtr<FSectionalMaterial> & MiddleH = ParentDoor.GetMiddleH();
					if (MiddleH->InSideSectionSizePros.IsValid())
					{
						const float depthOffset = MiddleH->InSideSectionSizePros.BayonetDepth;
						const float slotDepth = MiddleH->InSideSectionSizePros.DownBayonetHeight;
						ExpectValues.Get<0>() = TPair<float, float>(depthOffset, slotDepth);
					}
				}
			}

			if (ParentDivid->bIsRoot)
			{
				return;
			}
			
			FGetExpectValue GetExpectValue(ParentDoor, ParentDivid, ExpectValues);
			GetExpectValue();
		}
	private:
		const FSpaceDividDoorSheet::FSpaceDivid * & FindBase;
		TTuple<TPair<float, float>, TPair<float, float>, TPair<float, float>, TPair<float, float>> & ExpectValues;
		const FSpaceDividDoorSheet & ParentDoor;		
	};
	
	TTuple<TPair<float, float>, TPair<float, float>, TPair<float, float>, TPair<float, float>> RetValue =
	MakeTuple(TPair<float,float>(0.0f,0.0f), TPair<float, float>(0.0f, 0.0f), TPair<float, float>(0.0f, 0.0f), TPair<float, float>(0.0f, 0.0f));
	FGetExpectValue GetExpectValue(*this, InDivid, RetValue);
	GetExpectValue();
	return RetValue;
}

float FSpaceDividDoorSheet::FSectionalMaterial::GetCareSpace() const
{
	float RetValue = 0.0f;
	switch (Type)
	{
		case ESectionalMaterialType::Left_V:
		case ESectionalMaterialType::Right_V:
		{
			RetValue = Width;
		}
		break;
		case ESectionalMaterialType::Middle_V:
		case ESectionalMaterialType::Top_H:
		case ESectionalMaterialType::Middle_H:
		case ESectionalMaterialType::Bottom_H:
		{
			RetValue = Height;
		}
		break;
		default:
			break;
	}
	return RetValue;
}

FSpaceDividDoorSheet::FSectionalMaterial::EProsType FSpaceDividDoorSheet::FSectionalMaterial::GetProsType() const
{
	EProsType RetType = EProsType::ENone;
	switch (Type)
	{
		case FSpaceDividDoorSheet::FSectionalMaterial::ESectionalMaterialType::Top_H:
		case FSpaceDividDoorSheet::FSectionalMaterial::ESectionalMaterialType::Bottom_H:
		{
			RetType = EProsType::ETopAndBottomH;		
		}break;
		case FSpaceDividDoorSheet::FSectionalMaterial::ESectionalMaterialType::Middle_H:
		case FSpaceDividDoorSheet::FSectionalMaterial::ESectionalMaterialType::Middle_V:
		{
			RetType = EProsType::EInside;
		}break;		
		case FSpaceDividDoorSheet::FSectionalMaterial::ESectionalMaterialType::Left_V:
		case FSpaceDividDoorSheet::FSectionalMaterial::ESectionalMaterialType::Right_V:
		{
			RetType = EProsType::ELeftAndRightV;
		}break;
		default:
			break;
	}	
	return RetType;
}

TPair<int32, int32> FSpaceDividDoorSheet::GetIndexByType(const FSectionalMaterial::ESectionalMaterialType InType) const
{
	TPair<int32, int32> RetIndex = TPair<int32, int32>(-1, -1);
	for (const auto & Ref : SectionalMaterials)
	{
		if (Ref.Value->Type == InType)
		{
			RetIndex = Ref.Key;
			break;
		}
	}
	check(RetIndex.Key != -1 && RetIndex.Value != -1);
	return RetIndex;
}

const FString FSpaceDividDoorSheet::GetCachePath(const int32 InSectionMaterialId, const FString & InMd5) const
{
	const FString CombineId = FString::Printf(TEXT("%s_%d_%d_%d"), TEXT("SlidingDoorBlocks"), GetShapeId(), InSectionMaterialId, StaticCast<int32>(GetShapeType()));
	return FAccessoryShape::GetCacheFilePathFromIDAndMd5AndUpdateTime(CombineId, InMd5, 0.0f);
}

void FSpaceDividDoorSheet::FSpaceDivid::ParseInfoFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	DividRation = InJsonObject->GetStringField(TEXT("divideRatio"));
	if (DividRation.IsEmpty())
	{
		bIsRoot = true;
		DividRation = TEXT("1x");
	}
	DividDirection = InJsonObject->GetIntegerField(TEXT("divideDirection"));
	if (InJsonObject->HasField(TEXT("doorCoreId")))
	{
		SectionalMaterialId = InJsonObject->GetIntegerField(TEXT("doorCoreId"));
	}
	const TArray<TSharedPtr<FJsonValue>> & ChildrenRef = InJsonObject->GetArrayField(TEXT("children"));
	for (const auto & Ref : ChildrenRef)
	{
		int32 Index = ChildrenBlocks.Emplace(MakeShareable(new FSpaceDivid));
		ChildrenBlocks[Index]->ParseInfoFromJson(Ref->AsObject());
	}
}

void FSpaceDividDoorSheet::FSpaceDivid::SaveInfoToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	if (bIsRoot)
	{
		JsonWriter->WriteValue(TEXT("divideRatio"), TEXT(""));
	}
	else
	{
		JsonWriter->WriteValue(TEXT("divideRatio"), DividRation);
	}
	JsonWriter->WriteValue(TEXT("divideDirection"), DividDirection);
	if (SectionalMaterialId != -1)
	{
		JsonWriter->WriteValue(TEXT("doorCoreId"), SectionalMaterialId);
	}
	JsonWriter->WriteArrayStart(TEXT("children"));
	for (const auto & Ref : ChildrenBlocks)
	{
		JsonWriter->WriteObjectStart();
		Ref->SaveInfoToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
}

void FSpaceDividDoorSheet::FSpaceDivid::SpawnActor(AActor * InParentActor, const FSpaceDividDoorSheet & InParentDoor)
{
	auto SpawnStaticMeshActor = [InParentActor, this](const TSharedPtr<FSectionalMaterial> & InOuterSectionMaterial)
	{
		UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();
		if (InOuterSectionMaterial->Type == FSectionalMaterial::ESectionalMaterialType::Louver_Core && InOuterSectionMaterial->IsShutter)
		{
			AActor * TempParentActor = World->SpawnActor<ASCTCompActor>(ASCTCompActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
			TempParentActor->AttachToActor(InParentActor, FAttachmentTransformRules::KeepRelativeTransform);
			StaticMeshActors.Add(TempParentActor);
		}
		else
		{
			ASCTModelActor * NewModelActor = World->SpawnActor<ASCTModelActor>(
				ASCTModelActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
			StaticMeshActors.Add(NewModelActor);
			check(InOuterSectionMaterial->CacheFilePath.IsEmpty() == false);
			const FMeshDataEntry * MeshData = FSCTShapeManager::Get()->GetStaticMeshManager()->FindMesh(InOuterSectionMaterial->CacheFilePath);
			check(MeshData && MeshData->Type == 0);
			NewModelActor->SetStaticMesh(MeshData->MeshData.Mesh, MeshData->MaterialList);
			NewModelActor->AttachToActor(InParentActor, FAttachmentTransformRules::KeepRelativeTransform);
		}
	};
	// 不切分
	if (DividDirection == 0)
	{
		const TSharedPtr<FSectionalMaterial> & SectionalMaterial = InParentDoor.GetCore(SectionalMaterialId);
		SpawnStaticMeshActor(SectionalMaterial);
	}
	// 竖向切分
	else if (DividDirection == 1)
	{
		const int32 ExcCount = ChildrenBlocks.Num() - 1;
		const TSharedPtr<FSectionalMaterial> & SectionalMaterial = InParentDoor.GetMiddleV();
		for (int Index = 0; Index < ExcCount; ++Index)
		{
			SpawnStaticMeshActor(SectionalMaterial);
		}
	}
	// 横向切分
	else if (DividDirection == 2)
	{
		const int32 ExcCount = ChildrenBlocks.Num() - 1;
		const TSharedPtr<FSectionalMaterial> & SectionalMaterial = InParentDoor.GetMiddleH();
		for (int Index = 0; Index < ExcCount; ++Index)
		{
			SpawnStaticMeshActor(SectionalMaterial);
		}
	}

	for (auto & Ref : ChildrenBlocks)
	{		
		Ref->SpawnActor(InParentActor, InParentDoor);
	}

}

void FSpaceDividDoorSheet::FSpaceDivid::RecalSizeAndPosition(const FSpaceDividDoorSheet & InParentDoor)
{
	// 计算门芯尺寸
	{
		auto ParseDividRation = [](const FString & InStr, float & OuterValue)->bool
		{
			bool RetIsFormula = false;
			const FString & TempDividRation = InStr;
			check(TempDividRation.IsEmpty() == false);
			RetIsFormula = TempDividRation.Len() > 1 && TempDividRation.EndsWith(TEXT("x"));
			if (RetIsFormula)
			{
				OuterValue = FCString::Atof(*(TempDividRation.LeftChop(TempDividRation.Len() - 1)));
			}
			else
			{
				OuterValue = FCString::Atof(*TempDividRation);
			}
			return RetIsFormula;
		};
		// 纵向切割
		if (DividDirection == 1)
		{
			const int32 MiddleVCount = ChildrenBlocks.Num() - 1;
			const float AvailableWidth = Width - InParentDoor.GetMiddleV()->GetCareSpace() * MiddleVCount;
			const float AvaliableHeight = Height;

			float TotalNoneFormulaWith = 0.0f;
			// 先计算所有非比例
			for (auto & Ref : ChildrenBlocks)
			{
				Ref->IsRation = ParseDividRation(Ref->DividRation, Ref->DividValue);
				if (Ref->IsRation) continue;
				Ref->Width = Ref->DividValue / 10.0f;
				Ref->Height = AvaliableHeight;
				TotalNoneFormulaWith += Ref->Width;
			}
			const float AvailableWidthWithoutRations = AvailableWidth - TotalNoneFormulaWith;

			float TotalRations = 0.0f;
			// 统计所有比例
			for (auto & Ref : ChildrenBlocks)
			{
				if (Ref->IsRation == false) continue;
				TotalRations += Ref->DividValue;
			}
			// 计算比例所占空间
			for (auto & Ref : ChildrenBlocks)
			{
				if (Ref->IsRation == false) continue;
				Ref->Width = AvailableWidthWithoutRations * (Ref->DividValue / TotalRations);
				Ref->Height = AvaliableHeight;
			}
		}
		// 横向切割
		else if (DividDirection == 2)
		{
			const int32 MiddleHCount = ChildrenBlocks.Num() - 1;
			const float AvailableHeight = Height - InParentDoor.GetMiddleH()->GetCareSpace() * MiddleHCount;
			const float AvaliableWidth = Width;

			float TotalNoneFormulaHeight = 0.0f;
			// 先计算所有非比例
			for (auto & Ref : ChildrenBlocks)
			{
				Ref->IsRation = ParseDividRation(Ref->DividRation, Ref->DividValue);
				if (Ref->IsRation) continue;
				Ref->Width = AvaliableWidth;
				Ref->Height = Ref->DividValue / 10.0;
				TotalNoneFormulaHeight += Ref->Height;
			}
			const float AvailableHeightWithoutRations = AvailableHeight - TotalNoneFormulaHeight;

			float TotalRations = 0.0f;
			// 统计所有比例
			for (auto & Ref : ChildrenBlocks)
			{
				if (Ref->IsRation == false) continue;
				TotalRations += Ref->DividValue;
			}
			// 计算比例所占空间
			for (auto & Ref : ChildrenBlocks)
			{
				if (Ref->IsRation == false) continue;
				Ref->Width = AvaliableWidth;
				Ref->Height = AvailableHeightWithoutRations * (Ref->DividValue / TotalRations);
			}
		}
	}
	// 对移门进行X方向偏移，用于单独测试移门门板拼接
	const float TestOffset = 0.0f;
	// 计算门芯坐标
	{
		// 纵向分割
		if (DividDirection == 1)
		{
			const int32 ChildrenCount = ChildrenBlocks.Num();
			const TSharedPtr<FSectionalMaterial>  & InfoRef = InParentDoor.GetMiddleV();
			float IncreaseX = PosX;
			int32 ExtIndex = 0;
			if (bIsRoot)
			{
				ExtIndex = 4;
				check(StaticMeshActors.Num() > 4);
			}
			for (int32 Index = 0; Index < ChildrenCount; ++Index)
			{
				ChildrenBlocks[Index]->PosZ = PosZ;
				ChildrenBlocks[Index]->PosY = PosY;
				ChildrenBlocks[Index]->PosX = IncreaseX + Index * InfoRef->GetCareSpace();
				IncreaseX += ChildrenBlocks[Index]->Width;
				if (Index == 0) continue;
				const TSharedPtr<FSectionalMaterial>  & InfoRef = InParentDoor.GetMiddleV();
#if 0
				StaticMeshActors[Index + ExtIndex - 1]->SetActorRelativeScale3D(FVector(0.1f, 0.1f, Height / InfoRef->Height / 10.0f));
				StaticMeshActors[Index + ExtIndex - 1]->SetActorRelativeLocation(FVector((ChildrenBlocks[Index]->PosX - InfoRef->GetCareSpace()) / 10.0f, PosY / 10.0f, PosZ / 10.0f));

#else
				const float DepthOffset =  InParentDoor.GetLeftV()->LeftAndRightSectionSizePros.IsValid() ? InParentDoor.GetLeftV()->Depth - InfoRef->Depth : 0.0f;
				StaticMeshActors[Index + ExtIndex - 1]->SetActorRelativeScale3D(FVector(Height / InfoRef->Width, 1.0f, 1.0f));
				StaticMeshActors[Index + ExtIndex - 1]->SetActorRelativeRotation(FQuat::MakeFromEuler(FVector(0.0f, -90.0f, 0.0f)));
				StaticMeshActors[Index + ExtIndex - 1]->SetActorRelativeLocation(FVector((ChildrenBlocks[Index]->PosX) - InfoRef->GetCareSpace() + TestOffset, PosY + DepthOffset, (PosZ + Height)));
#endif
			}
		}
		// 横向切割
		else if (DividDirection == 2)
		{
			const int32 ChildrenCount = ChildrenBlocks.Num();
			const TSharedPtr<FSectionalMaterial>  & InfoRef = InParentDoor.GetMiddleH();
			float IncreaseZ = PosZ;
			int32 ExtIndex = 0;
			if (bIsRoot)
			{
				ExtIndex = 4;
				check(StaticMeshActors.Num() > 4);
			}
			for (int32 Index = ChildrenCount - 1; Index >= 0; --Index)
			{
				ChildrenBlocks[Index]->PosX = PosX;
				ChildrenBlocks[Index]->PosY = PosY;
				ChildrenBlocks[Index]->PosZ = IncreaseZ + (ChildrenCount - 1 - Index) * InfoRef->GetCareSpace();
				IncreaseZ += ChildrenBlocks[Index]->Height;
				if (Index == 0) continue;
				const float DepthOffset = InParentDoor.GetLeftV()->LeftAndRightSectionSizePros.IsValid() ? InParentDoor.GetLeftV()->Depth - InfoRef->Depth : 0.0f;
				const TSharedPtr<FSectionalMaterial>  & InfoRef = InParentDoor.GetMiddleV();
				StaticMeshActors[ChildrenCount - 1 - Index + ExtIndex]->SetActorRelativeScale3D(FVector(Width / InfoRef->Width, 1.0f, 1.0f));
				StaticMeshActors[ChildrenCount - 1 - Index + ExtIndex]->SetActorRelativeLocation(FVector(PosX + TestOffset, PosY + DepthOffset, (ChildrenBlocks[Index]->PosZ + ChildrenBlocks[Index]->Height)));				
			}
		}
	}

	// 更新门芯Actor的坐标与尺寸
	if (DividDirection == 0)
	{
		int32 Index = 0;
		if (bIsRoot)
		{
			Index = 4;
			check(StaticMeshActors.Num() > 4);
		}	
		TTuple<TPair<float,float>, TPair<float, float>, TPair<float, float>, TPair<float, float>>
		SloatOffsets = InParentDoor.GetSloatAndDepthOffset(this);
		for (; Index < StaticMeshActors.Num(); ++Index)
		{
			const TSharedPtr<FSectionalMaterial>  & InfoRef = InParentDoor.GetCore(SectionalMaterialId);
			// 非百叶 平铺模式下，需要在U方向重复贴图
			if (InfoRef->DisplayMode == FSectionalMaterial::EDisplayModel::Tiled && InfoRef->IsShutter == false)
			{
				
				const float LeftOffset = SloatOffsets.Get<2>().Value;
				const float DownOffset = SloatOffsets.Get<1>().Value;
				const float DesWidth = Width + SloatOffsets.Get<2>().Value + SloatOffsets.Get<3>().Value;
				const float DesDepthOffset = FMath::IsNearlyZero(SloatOffsets.Get<2>().Key) ? 0.0f : (SloatOffsets.Get<2>().Key - InfoRef->Depth);
				const float DesHeight = Height + SloatOffsets.Get<0>().Value + SloatOffsets.Get<1>().Value;
				StaticMeshActors[Index]->SetActorRelativeScale3D(FVector(DesWidth / InfoRef->Width, 1.0f, DesHeight / InfoRef->Height));
				StaticMeshActors[Index]->SetActorRelativeLocation(FVector(PosX + TestOffset - LeftOffset, PosY + DesDepthOffset, PosZ - DownOffset));
				ASCTModelActor * SCTModelActor = Cast<ASCTModelActor>(StaticMeshActors[Index]);
				if (SCTModelActor)
				{
					Cast<ASCTModelActor>(StaticMeshActors[Index])->SetSingleMaterialOffsetAndRepeatParameter(0.0f, 0.0f, Width / InfoRef->Width, 1.0f);
				}
			}
			// 如果是百叶，则需要根据尺寸对百叶模型，在垂直方向进行切割
			if (InfoRef->IsShutter)
			{
				// 计算当前高度，需要多少个完整的百叶进行拼接
				int IntCount = FMath::FloorToInt(Height / InfoRef->Height);
				ASCTCompActor * SCTCompActor = Cast<ASCTCompActor>(StaticMeshActors[Index]);
				check(SCTCompActor);
				// 清除当前已有的百叶子Actor
				auto ClearCurrentActors = [this](ASCTCompActor * InCompActor)
				{
					TArray<USceneComponent *> SceneCompoents = InCompActor->GetRootComponent()->GetAttachChildren();
					for (auto & InRef : SceneCompoents)
					{
						AActor * TempActor = Cast<AActor>(InRef->GetOwner());
						if (TempActor)
						{
							TempActor->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
							TempActor->Destroy();
							InRef->RemoveFromRoot();
						}

					}

				};
				ClearCurrentActors(SCTCompActor);
				// 摆放完整的百叶子Actor，只需要横向拉伸，不需要再垂直方向上面进行切割的
				auto SpawnFullActor = [this, &InfoRef](ASCTCompActor * InCompActor)->AActor*
				{
					UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();
					AActor * NewModelActor = World->SpawnActor<AActor>(
						AActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
					check(InfoRef->CacheFilePath.IsEmpty() == false);
					UStaticMeshComponent * StaticMeshCompoent = NewObject<UStaticMeshComponent>();
					const FMeshDataEntry * MeshData = FSCTShapeManager::Get()->GetStaticMeshManager()->FindMesh(InfoRef->CacheFilePath);
					check(MeshData && MeshData->Type == 0);
					StaticMeshCompoent->SetStaticMesh(MeshData->MeshData.Mesh);
					for (const auto & Ref : MeshData->MaterialList)
					{
						StaticMeshCompoent->SetMaterialByName(FName(*Ref.Key), Ref.Value);
					}
					UProceduralMeshComponent * ProcedureMeshCompoent = NewObject<UProceduralMeshComponent>(NewModelActor);
					NewModelActor->AddOwnedComponent(ProcedureMeshCompoent);
					NewModelActor->SetRootComponent(ProcedureMeshCompoent);
					ProcedureMeshCompoent->RegisterComponent();
					UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(StaticMeshCompoent, 0, ProcedureMeshCompoent, true);
					NewModelActor->AttachToActor(InCompActor, FAttachmentTransformRules::KeepRelativeTransform);
					return NewModelActor;
				};
				float ToClipSize = (Height / InfoRef->Height) - IntCount;
				for (int32 Index = 0; Index < IntCount; ++Index)
				{
					const float ZOffset = (ToClipSize + Index) * InfoRef->Height;
					AActor * TempActor = SpawnFullActor(SCTCompActor);
					TempActor->SetActorScale3D(FVector(Width / InfoRef->Width, 1.0f, 1.0f));
					TempActor->SetActorRelativeLocation(FVector(PosX, PosY, PosZ + ZOffset));
				}
				if (ToClipSize *  InfoRef->Height > 1.0f)
				{
					AActor * RunTimeActor = SpawnFullActor(SCTCompActor);
					check(RunTimeActor);
					UProceduralMeshComponent * Compoent = Cast<UProceduralMeshComponent>(RunTimeActor->GetRootComponent());
					UProceduralMeshComponent * OuterHalfCompoent = nullptr;
					const float D = (1.0f - ToClipSize) *  InfoRef->Height;
					FVector PlanePos = FVector(0.0f, 0.0f, D) + SCTCompActor->GetActorLocation();
					UKismetProceduralMeshLibrary::SliceProceduralMesh(Compoent, PlanePos, FVector::UpVector, false, OuterHalfCompoent, EProcMeshSliceCapOption::NoCap, nullptr);
					RunTimeActor->SetActorScale3D(FVector(Width / InfoRef->Width, 1.0f, 1.0f));
					RunTimeActor->SetActorRelativeLocation(FVector(PosX, PosY, PosZ - D));

				}
			}

		}
	}

	// 更新子块
	for (auto & Ref : ChildrenBlocks)
	{
		Ref->RecalSizeAndPosition(InParentDoor);
	}
}

void FSpaceDividDoorSheet::FSpaceDivid::SetCollisionProfileName(const FName & InProfileName)
{
	auto SetCollisionProfilleName = [](AActor * InActor,const FName & InProfileName)
	{
		UMeshComponent *ActorPrimComp = Cast<UMeshComponent>(InActor->GetRootComponent());
		ActorPrimComp->SetCollisionProfileName(InProfileName);
	};
	for (auto & Ref : StaticMeshActors)
	{
		SetCollisionProfilleName(Ref, InProfileName);
	}
	for (auto & Ref : ChildrenBlocks)
	{
		Ref->SetCollisionProfileName(InProfileName);
	}
}

void FSpaceDividDoorSheet::FSpaceDivid::ResetDividPosition()
{
	PosX = 0.0f;
	PosY = 0.0f;
	PosZ = 0.0f;
	for (auto & Ref : ChildrenBlocks)
	{
		Ref->ResetDividPosition();
	}
}

TSharedPtr<FSpaceDividDoorSheet::FSpaceDivid> FSpaceDividDoorSheet::FSpaceDivid::CloneDividBlock() const
{
	TSharedPtr<FSpaceDivid> NewBlock = MakeShareable(new FSpaceDivid);
	NewBlock->bIsRoot = this->bIsRoot;
	NewBlock->DividDirection = this->DividDirection;
	NewBlock->DividValue = this->DividValue;
	NewBlock->IsRation = this->IsRation;
	NewBlock->DividRation = this->DividRation;
	NewBlock->Width = this->Width;
	NewBlock->Height = this->Height;
	NewBlock->PosX = this->PosX;
	NewBlock->PosY = this->PosY;
	NewBlock->PosZ = this->PosZ;
	NewBlock->SectionalMaterialId = this->SectionalMaterialId;
	for (const auto & Ref : ChildrenBlocks)
	{
		NewBlock->ChildrenBlocks.Add(Ref->CloneDividBlock());
	}
	return NewBlock;
}

void FSpaceDividDoorSheet::FSpaceDivid::DestoryActor()
{
	for (auto & Ref : StaticMeshActors)
	{
		if (Cast<ASCTModelActor>(Ref))
		{
			Ref->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
			Ref->Destroy();
		}
		else if (Cast<ASCTCompActor>(Ref))
		{
			TArray<USceneComponent *> SceneCompoents = Ref->GetRootComponent()->GetAttachChildren();
			for (auto & InRef : SceneCompoents)
			{
				AActor * TempActor = Cast<AActor>(InRef->GetOwner());
				if (TempActor)
				{
					TempActor->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
					TempActor->Destroy();
				}
			}
		}
	}
	StaticMeshActors.Empty();
	for (auto & Ref : ChildrenBlocks)
	{
		Ref->DestoryActor();
	}

}

FVariableAreaDoorSheet::FVariableAreaDoorSheet()
{
	SetShapeType(ST_VariableAreaDoorSheet);
}

void FVariableAreaDoorSheet::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{	
	FDoorSheetShapeBase::ParseFromJson(InJsonObject);
	SetShapeType(ST_VariableAreaDoorSheet);
	//暂时不从后台读取开门方向，默认全部为左开门
	const TArray<TSharedPtr<FJsonValue>> & OpenDoorDirectionsJsonArray = InJsonObject->GetArrayField(TEXT("openDoorDirections"));
	AvailableOpenDirections.Empty();
	for (const auto & Ref : OpenDoorDirectionsJsonArray)
	{
		AvailableOpenDirections.Emplace(StaticCast<EDoorOpenDirection>(StaticCast<uint8>(Ref->AsNumber())));
	}
	OpenDoorDirectionIndex = InJsonObject->GetIntegerField(TEXT("defaultOpenDoorIndex"));
	//SetOpenDoorDirection(1/*InJsonObject->GetIntegerField(TEXT("openDoorDirection"))*/);
	auto ParseDividBlockShape = [this](TSharedPtr<FDividBlockShape> InShape, const TSharedPtr<FJsonObject>& InShapeJsonObject, const FString & InFileName)
	{
		InShape->ParseFromJson(InShapeJsonObject);
		auto ConvertFormula = [](const FString & InStr)->FString
		{
			FString RetStr = InStr;
			static TArray<TPair<FString, FString>> ToFixPairs =
			{
				TPair<FString,FString>(TEXT("w"),TEXT("pval(\"W\")")),
				TPair<FString,FString>(TEXT("h"),TEXT("pval(\"H\")")),
			};
			for (const auto & Ref : ToFixPairs)
			{
				RetStr = RetStr.Replace(*(Ref.Key), *(Ref.Value), ESearchCase::Type::CaseSensitive);
			}
			return RetStr;
		};
		auto ParseCurrentValue = [ConvertFormula, InShape](const FString & InAttriName, const TSharedPtr<FJsonObject>& InShapeJsonObject)->FString
		{
			const TSharedPtr<FJsonObject> & JsonObj = InShapeJsonObject->GetObjectField(InAttriName);
			return ConvertFormula(JsonObj->GetStringField(TEXT("current")));
		};
		InShape->SetShapeWidth(ParseCurrentValue(TEXT("width"), InShapeJsonObject));
		InShape->SetShapeHeight(ParseCurrentValue(TEXT("height"), InShapeJsonObject));
		InShape->SetShapePosX(ParseCurrentValue(TEXT("posx"), InShapeJsonObject));
		InShape->SetShapePosZ(ParseCurrentValue(TEXT("posz"), InShapeJsonObject));
		InShape->SetFileUrl(InShapeJsonObject->GetStringField(TEXT("pakUrl")));
		InShape->SetFileMd5(InShapeJsonObject->GetStringField(TEXT("pakMd5")));
		InShape->SetoptimizeParam(InShapeJsonObject->GetStringField(TEXT("optimizeParam")));
		InShape->SetShapeType(ST_Decoration);
		InShape->SetFileName(InFileName);
	};

	// 掩门切块	
	if (InJsonObject->HasField(TEXT("divideBlocks")))
	{
		const TArray<TSharedPtr<FJsonValue>> & TempDividBlocks = InJsonObject->GetArrayField(TEXT("divideBlocks"));
		int Index = 0;
		for (const auto & Ref : TempDividBlocks)
		{
			const TSharedPtr<FJsonObject> & Obj = Ref->AsObject();
			TSharedPtr<FDividBlockShape> ShapePtr = MakeShareable(new FDividBlockShape());
			AddChildShape(ShapePtr);
			ShapePtr->SetParentShape(this);
			AddDividBlock(ShapePtr);
			const FString  CombineFileName = FString::Printf(TEXT("%s_%d_%d"), TEXT("divideBlocks"), GetShapeId(), ++Index);
			ParseDividBlockShape(ShapePtr, Obj, CombineFileName);
		}
	}
	
}

void FVariableAreaDoorSheet::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FDoorSheetShapeBase::ParseAttributesFromJson(InJsonObject);
	OpenDoorDirectionIndex = InJsonObject->GetIntegerField(TEXT("defaultOpenDoorIndex"));
	auto ParseDividBlockShape = [](TSharedPtr<FDividBlockShape> InShape, const TSharedPtr<FJsonObject>& InShapeJsonObject, const FString & InFileName)
	{
		InShape->ParseFromJson(InShapeJsonObject);
		auto ConvertFormula = [](const FString & InStr)->FString
		{
			FString RetStr = InStr;
			static TArray<TPair<FString, FString>> ToFixPairs =
			{
				TPair<FString,FString>(TEXT("w"),TEXT("pval(\"W\")")),
				TPair<FString,FString>(TEXT("h"),TEXT("pval(\"H\")")),
			};
			for (const auto & Ref : ToFixPairs)
			{
				RetStr = RetStr.Replace(*(Ref.Key), *(Ref.Value), ESearchCase::Type::CaseSensitive);
			}
			return RetStr;
		};
		auto ParseCurrentValue = [ConvertFormula, InShape](const FString & InAttriName, const TSharedPtr<FJsonObject>& InShapeJsonObject)->FString
		{
			const TSharedPtr<FJsonObject> & JsonObj = InShapeJsonObject->GetObjectField(InAttriName);
			return ConvertFormula(JsonObj->GetStringField(TEXT("current")));
		};
		InShape->SetShapeWidth(ParseCurrentValue(TEXT("width"), InShapeJsonObject));
		InShape->SetShapeHeight(ParseCurrentValue(TEXT("height"), InShapeJsonObject));
		InShape->SetShapePosX(ParseCurrentValue(TEXT("posx"), InShapeJsonObject));
		InShape->SetShapePosZ(ParseCurrentValue(TEXT("posz"), InShapeJsonObject));
		InShape->SetFileUrl(InShapeJsonObject->GetStringField(TEXT("pakUrl")));
		InShape->SetFileMd5(InShapeJsonObject->GetStringField(TEXT("pakMd5")));
		InShape->SetoptimizeParam(InShapeJsonObject->GetStringField(TEXT("optimizeParam")));
		InShape->SetShapeType(ST_Decoration);
		InShape->SetFileName(InFileName);
	};

	// 掩门切块	
	if (InJsonObject->HasField(TEXT("divideBlocks")))
	{
		const TArray<TSharedPtr<FJsonValue>> & TempDividBlocks = InJsonObject->GetArrayField(TEXT("divideBlocks"));
		int Index = 0;
		for (const auto & Ref : TempDividBlocks)
		{
			const TSharedPtr<FJsonObject> & Obj = Ref->AsObject();
			TSharedPtr<FDividBlockShape> ShapePtr = MakeShareable(new FDividBlockShape());
			AddChildShape(ShapePtr);
			ShapePtr->SetParentShape(this);
			AddDividBlock(ShapePtr);
			const FString  CombineFileName = FString::Printf(TEXT("%s_%d_%d"), TEXT("divideBlocks"), GetShapeId(), ++Index);
			ParseDividBlockShape(ShapePtr, Obj, CombineFileName);
		}
	}


	// 把手类型位置
	if (InJsonObject->HasField(TEXT("MetalTypePosition")))
	{
		const TSharedPtr<FJsonObject> & MetalTypeObjeRef = InJsonObject->GetObjectField(TEXT("MetalTypePosition"));
		// 明装拉手
		if (MetalTypeObjeRef->HasField(TEXT("shakeHanleTypePosition")))
		{
			const TSharedPtr<FJsonObject> & shakeHanleTypePositionObjRef = MetalTypeObjeRef->GetObjectField(TEXT("shakeHanleTypePosition"));			
			ShakeHanleTypePosition.HPositionType = shakeHanleTypePositionObjRef->GetIntegerField(TEXT("hpos"));
			ShakeHanleTypePosition.HPositionValue = shakeHanleTypePositionObjRef->GetNumberField(TEXT("hposVal"));
			ShakeHanleTypePosition.VPositionType = shakeHanleTypePositionObjRef->GetIntegerField(TEXT("vpos"));
			ShakeHanleTypePosition.VPositionValue = shakeHanleTypePositionObjRef->GetNumberField(TEXT("vposVal"));
			ShakeHanleTypePosition.RotateDirection = shakeHanleTypePositionObjRef->GetIntegerField(TEXT("rotationalDirection"));
		}
		// 封边拉手
		if (MetalTypeObjeRef->HasField(TEXT("sealingSideHandleTypePosition")))
		{			;
			const TSharedPtr<FJsonObject> & SealingSideHanleOnjRef = MetalTypeObjeRef->GetObjectField(TEXT("sealingSideHandleTypePosition"));			
			SealingSideHandleTypePosition.VPostionType = SealingSideHanleOnjRef->GetIntegerField(TEXT("vposVal"));
		}		
	}
}

void FVariableAreaDoorSheet::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FDoorSheetShapeBase::ParseShapeFromJson(InJsonObject);
	/*if (InJsonObject->HasField(TEXT("openDoorDirections")))
	{
	
	}*/
	const TArray<TSharedPtr<FJsonValue>> & OpenDoorDirectionsJsonArray = InJsonObject->GetArrayField(TEXT("openDoorDirections"));
	if (OpenDoorDirectionsJsonArray.Num() > 0)
	{
		AvailableOpenDirections.Empty();
	}
	for (const auto & Ref : OpenDoorDirectionsJsonArray)
	{
		AvailableOpenDirections.Emplace(StaticCast<EDoorOpenDirection>(StaticCast<uint8>(Ref->AsNumber())));
	}
	OpenDoorDirectionIndex = InJsonObject->GetIntegerField(TEXT("defaultOpenDoorIndex"));
	SetShapeType(ST_VariableAreaDoorSheet);
}

void FVariableAreaDoorSheet::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{
	FDoorSheetShapeBase::ParseContentFromJson(InJsonObject);
}

void FVariableAreaDoorSheet::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FDoorSheetShapeBase::SaveToJson(JsonWriter);

	JsonWriter->WriteArrayStart(TEXT("openDoorDirections"));
	for(const auto & Ref : AvailableOpenDirections)
	{
		JsonWriter->WriteValue(StaticCast<int32>(Ref));
	}
	JsonWriter->WriteArrayEnd();	
	JsonWriter->WriteValue(TEXT("defaultOpenDoorIndex"), StaticCast<int32>(OpenDoorDirectionIndex));
	// 存储门切块
	JsonWriter->WriteArrayStart(TEXT("divideBlocks"));
	for (const auto & Ref : DividBlocks)
	{
		JsonWriter->WriteObjectStart();
		Ref->SaveToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();	
}

void FVariableAreaDoorSheet::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FDoorSheetShapeBase::SaveAttriToJson(JsonWriter);
	JsonWriter->WriteValue(TEXT("defaultOpenDoorIndex"),OpenDoorDirectionIndex);
	
	// 存储门切块
	JsonWriter->WriteArrayStart(TEXT("divideBlocks"));

	auto WriteAttribute = [](const FString & InName, const FString & InRefName, const TSharedPtr<FShapeAttribute> & InAttri, TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
	{
		TSharedPtr<FNumberRangeAttri> RangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(InAttri);
		JsonWriter->WriteObjectStart(InName);
		JsonWriter->WriteValue(TEXT("id"), RangeAttri->GetAttributeId());
		JsonWriter->WriteValue(TEXT("type"), (int32)RangeAttri->GetAttributeType());
		JsonWriter->WriteValue(TEXT("refName"), InRefName);
		JsonWriter->WriteObjectStart(TEXT("value"));
		JsonWriter->WriteValue(TEXT("min"), RangeAttri->GetMinValue());
		JsonWriter->WriteValue(TEXT("max"), RangeAttri->GetMaxValue());
		JsonWriter->WriteObjectEnd();
		JsonWriter->WriteValue(TEXT("current"), RangeAttri->GetAttributeStr());
		JsonWriter->WriteObjectEnd();
	};

	for (const auto & Ref : DividBlocks)
	{
		JsonWriter->WriteObjectStart();
		WriteAttribute(TEXT("width"), TEXT("WIDTH"), Ref->GetShapeWidthAttri(), JsonWriter);
		WriteAttribute(TEXT("depth"), TEXT("DEPTH"), Ref->GetShapeDepthAttri(), JsonWriter);
		WriteAttribute(TEXT("height"), TEXT("HEIGHT"), Ref->GetShapeHeightAttri(), JsonWriter);
		WriteAttribute(TEXT("posx"), TEXT("POSX"), Ref->GetShapePosXAttri(), JsonWriter);
		WriteAttribute(TEXT("posy"), TEXT("POSY"), Ref->GetShapePosYAttri(), JsonWriter);
		WriteAttribute(TEXT("posz"), TEXT("POSZ"), Ref->GetShapePosZAttri(), JsonWriter);
		WriteAttribute(TEXT("rotx"), TEXT("ROTX"), Ref->GetShapeRotXAttri(), JsonWriter);
		WriteAttribute(TEXT("roty"), TEXT("ROTY"), Ref->GetShapeRotYAttri(), JsonWriter);
		WriteAttribute(TEXT("rotz"), TEXT("ROTZ"), Ref->GetShapeRotZAttri(), JsonWriter);
		JsonWriter->WriteValue(TEXT("pakUrl"), Ref->GetFileUrl());
		JsonWriter->WriteValue(TEXT("pakMd5"), Ref->GetFileMd5());		
		JsonWriter->WriteValue(TEXT("fileName"), Ref->GetFileName());
		JsonWriter->WriteValue(TEXT("mwassetUrl"), TEXT(""));
		JsonWriter->WriteValue(TEXT("optimizeParam"), Ref->GetoptimizeParam());
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	// 存储默认把手位置
	if (IsShakeHanlePositionValid() || IsSealingSideHandleValid())
	{
		JsonWriter->WriteObjectStart(TEXT("MetalTypePosition"));
		{
			// 明装拉手
			if (IsShakeHanlePositionValid())
			{
				JsonWriter->WriteObjectStart(TEXT("shakeHanleTypePosition"));
				JsonWriter->WriteValue(TEXT("hpos"), ShakeHanleTypePosition.HPositionType);
				JsonWriter->WriteValue(TEXT("rotationalDirection"), ShakeHanleTypePosition.RotateDirection);
				JsonWriter->WriteValue(TEXT("hposVal"), ShakeHanleTypePosition.HPositionValue);
				JsonWriter->WriteValue(TEXT("vpos"), ShakeHanleTypePosition.VPositionType);
				JsonWriter->WriteValue(TEXT("vposVal"), ShakeHanleTypePosition.VPositionValue);
				JsonWriter->WriteObjectEnd();
			}
			// 封边拉手
			if (IsSealingSideHandleValid())
			{
				JsonWriter->WriteObjectStart(TEXT("sealingSideHandleTypePosition"));
				JsonWriter->WriteValue(TEXT("vposVal"), SealingSideHandleTypePosition.VPostionType);
				JsonWriter->WriteObjectEnd();
			}
		}
		JsonWriter->WriteObjectEnd();
	}
}

void FVariableAreaDoorSheet::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FDoorSheetShapeBase::SaveShapeToJson(JsonWriter);
	JsonWriter->WriteArrayStart(TEXT("openDoorDirections"));
	for (const auto & Ref : AvailableOpenDirections)
	{
		JsonWriter->WriteValue(StaticCast<int32>(Ref));
	}
	JsonWriter->WriteArrayEnd();
	JsonWriter->WriteValue(TEXT("defaultOpenDoorIndex"), StaticCast<int32>(OpenDoorDirectionIndex));
}

void FVariableAreaDoorSheet::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FDoorSheetShapeBase::SaveContentToJson(JsonWriter);
}

ASCTShapeActor * FVariableAreaDoorSheet::SpawnShapeActor()
{	
	//创建基础组件型录
	ASCTShapeActor* NewBaseActor = FDoorSheetShapeBase::SpawnShapeActor();

	//创建组件子型录
	for (auto it = DividBlocks.CreateConstIterator(); it; ++it)
	{
		(*it)->LoadModelMesh();
		ASCTShapeActor* NewBoardActor = (*it)->SpawnShapeActor();
		NewBoardActor->AttachToActorOverride(NewBaseActor, FAttachmentTransformRules::KeepRelativeTransform);		
	}		
	if(bHasOptionalMaterial)
	{
		const int32 MaterialID = GetMaterial().ID;
		const FString PakUrl = GetMaterial().Url;
		const FString PakMd5 = GetMaterial().MD5;
		const FString OptimizeParam = GetMaterial().OptimizeParam;
		auto ApplayMaterialWithActor = [this](UMaterialInterface * InMaterialInstance, const FString & InErrorMsg)
		{
			if (InMaterialInstance)
			{
				UpdateMaterial(InMaterialInstance);
			}
		};
		FMaterialLoadCallback MCallback;
		MCallback.BindLambda(ApplayMaterialWithActor);
		FMaterialManagerInstatnce::GetIns().GetMaterialManagerPtr()->GetOrCreateMaterialInterfaceWithParameter(
			EMaterialType::Board_Material, PakUrl, FString::FromInt(MaterialID), PakMd5, OptimizeParam, MCallback);

	}
	ReCalDividBlocksPositionAndSize();
	return NewBaseActor;
}

void FVariableAreaDoorSheet::GetResourceUrls(TArray<FString>& OutResourceUrls)
{
	FDoorSheetShapeBase::GetResourceUrls(OutResourceUrls);		
}

void FVariableAreaDoorSheet::GetFileCachePaths(TArray<FString>& OutFileCachePaths)
{
	FDoorSheetShapeBase::GetFileCachePaths(OutFileCachePaths);
}

void FVariableAreaDoorSheet::CopyTo(FSCTShape * OutShape)
{
	FDoorSheetShapeBase::CopyTo(OutShape);
	FVariableAreaDoorSheet* OutBaseShape = StaticCast<FVariableAreaDoorSheet*>(OutShape);
	
	OutBaseShape->OpenDoorDirectionIndex = OpenDoorDirectionIndex;
	OutBaseShape->AvailableOpenDirections = AvailableOpenDirections;

	//子级型录
	for (int32 i = 0; i< DividBlocks.Num(); ++i)
	{
		FDividBlockShape* NewBlock = new FDividBlockShape;
		DividBlocks[i]->CopyTo(NewBlock);
		OutBaseShape->AddDividBlock(MakeShareable(NewBlock));
	}
	OutBaseShape->ShakeHanleTypePosition = ShakeHanleTypePosition;
	OutBaseShape->SealingSideHandleTypePosition = SealingSideHandleTypePosition;
}

void FVariableAreaDoorSheet::SetCollisionProfileName(FName InProfileName)
{
	FDoorSheetShapeBase::SetCollisionProfileName(InProfileName);
	for (auto & Ref : DividBlocks)
	{
		Ref->SetCollisionProfileName(InProfileName);
	}
}

void FVariableAreaDoorSheet::ReCalDividBlocksPositionAndSize()
{
	const float DoorWidth = GetShapeWidth();
	const float DoorHeight = GetShapeHeight();
	auto SetActorMaterialOffsetAndRepeat = [](TSharedPtr<FDividBlockShape> & InShape, const float & InOffsetU, const float & InOffsetV,
		const float & InRepeatU, const float & InRepeatV)
	{
		const float OffsetScale = 100.0f;
		ASCTModelActor * ModelActor = Cast<ASCTModelActor>(InShape->GetShapeActor());
		if (ModelActor)
		{
			ModelActor->SetSingleMaterialOffsetAndRepeatParameter(InOffsetU * OffsetScale, InOffsetV * OffsetScale, InRepeatU, InRepeatV);
		}
	};

	// 重新计算纹理 
	constexpr float DefaultRepeatValue = 200.0f;
	for (auto & Ref : DividBlocks)
	{
		const float OffsetU = Ref->GetShapePosX() / DoorWidth;
		const float OffsetV = 1.0f - (Ref->GetShapePosZ() + Ref->GetShapeHeight()) / DoorHeight;
		const float RepeatU = Ref->GetShapeWidth() / DoorWidth;
		const float RepeatV = Ref->GetShapeHeight() / DoorHeight;
		SetActorMaterialOffsetAndRepeat(Ref, OffsetU, OffsetV, RepeatU, RepeatV);

#if 0
		static int32 ModIndex = 1;
		static float StepIncreaseX = 0.0f;
		static float StepIncreaseZ = 5.0f;
		Ref->SetShapePosX(Ref->GetShapePosX() + StepIncreaseX);
		Ref->SetShapePosZ(Ref->GetShapePosZ() + StepIncreaseZ);
		if (ModIndex % 5 == 0)
		{
			StepIncreaseX += 0.0f;
			StepIncreaseZ -= 5.0f;
		}
		++ModIndex;
#endif
	}	
}

FVariableAreaDoorSheet::EDoorOpenDirection FVariableAreaDoorSheet::GetOpenDoorDirection() const
{
	EDoorOpenDirection RetDircetion = FVariableAreaDoorSheet::EDoorOpenDirection::E_None;
	if (AvailableOpenDirections.IsValidIndex(OpenDoorDirectionIndex))
	{
		RetDircetion = AvailableOpenDirections[OpenDoorDirectionIndex];
	}
	return 	RetDircetion;
}

void FVariableAreaDoorSheet::SetOpenDoorDirection(const EDoorOpenDirection InDoorDirection)
{
	const int32 FindIndex = AvailableOpenDirections.Find(InDoorDirection);
	check(FindIndex != INDEX_NONE);
	SetOpenDoorDirectionIndex(FindIndex);
}

void FVariableAreaDoorSheet::AddDividBlock(TSharedPtr<FDividBlockShape> InBlock)
{
	DividBlocks.Add(InBlock);
	AddChildShape(InBlock);
	InBlock->SetParentShape(this);
}

void FVariableAreaDoorSheet::UpdateMaterial(UMaterialInterface * InMaterialInterface)
{
	bHasOptionalMaterial = true;
	for (const auto Ref : DividBlocks)
	{
		ASCTModelActor * ModelActor = StaticCast<ASCTModelActor *>(Ref->GetShapeActor());
		if (!ModelActor) return;
		// 对门板的 材质进行旋转		
		UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(InMaterialInterface);
		UMaterialInstanceDynamic* NewMID = UMaterialInstanceDynamic::Create(MID->Parent, nullptr);
		NewMID->CopyParameterOverrides(MID);
		ModelActor->ChangeMaterial(NewMID);
		ModelActor->SetSingleMaterialRotateParameter(90.0f);
	}
	ReCalDividBlocksPositionAndSize();	
}
