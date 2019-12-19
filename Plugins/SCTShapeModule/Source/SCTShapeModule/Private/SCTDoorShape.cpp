//#include "SCTDoorShape.h"
//#include "JsonObject.h"
//#include "Serialization/JsonWriter.h"
//#include "SCTShapeManager.h"
//#include "SCTModelShape.h"
//#include "SCTShapeActor.h"
//#include <SCTModelActor.h>
//#include "SCTShape.h"
//#include "SCTAttribute.h"
//#include "SCTShapeBoxActor.h"
//#include "Components/MeshComponent.h"
//#include "MaterialManager.h"
//#include "SCTStaticMeshManager.h"
//#include "Engine/StaticMesh.h"
//#include "Actor/SCTDoorActor.h"
//#include "SCTBoardShape.h"
//#include "SCTShapeData.h"
////#include "CSGInterface.h"
//#include "ProceduralMeshComponent.h"
//#include "KismetProceduralMeshLibrary.h"
//
//
//
//void FDoorShape::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
//{
//	//1、基类
//	FSCTShape::ParseFromJson(InJsonObject);
//
//	// 材质
//	if (InJsonObject->HasField(TEXT("material")))
//	{
//		const TSharedPtr<FJsonObject> TempMaterialObj = InJsonObject->GetObjectField(TEXT("material"));
//		FDoorPakMetaData & PakRef = GetMaterial();
//		PakRef.ID = TempMaterialObj->GetIntegerField(TEXT("id"));
//		PakRef.Name = TempMaterialObj->GetStringField(TEXT("name"));
//		PakRef.Url = TempMaterialObj->GetStringField(TEXT("pakUrl"));
//		PakRef.MD5 = TempMaterialObj->GetStringField(TEXT("pakMd5"));
//		PakRef.ThumbnailUrl = TempMaterialObj->GetStringField(TEXT("thumbnailUrl"));
//	}
//
//	// 基材
//	if (InJsonObject->HasField(TEXT("substrate")))
//	{
//		const TSharedPtr<FJsonObject> TempSubstrateObj = InJsonObject->GetObjectField(TEXT("substrate"));
//		SetSubstrateID(TempSubstrateObj->GetIntegerField(TEXT("id")));
//		SetSubstrateName(TempSubstrateObj->GetStringField(TEXT("name")));
//	}	
//	// 门缝
//	if (InJsonObject->HasField(TEXT("doorSlotValue")))
//	{
//		TArray<int32> & DoorSlotValuesRef = GetDoorSlotValues();
//		const TArray<TSharedPtr<FJsonValue>> TempDoorSlotArray = InJsonObject->GetArrayField(TEXT("doorSlotValue"));
//		for (const auto & IterRef : TempDoorSlotArray)
//		{
//			DoorSlotValuesRef.Emplace(IterRef->AsNumber());
//		}
//	}
//	// 延伸方式
//	if (InJsonObject->HasField(TEXT("extensionValue")))
//	{
//		TArray<int32> & ExtensionValuesRef = GetExtensionValues();
//		const TArray<TSharedPtr<FJsonValue>> TempExtensionArray = InJsonObject->GetArrayField(TEXT("extensionValue"));
//		for (const auto & IterRef : TempExtensionArray)
//		{
//			ExtensionValuesRef.Emplace(IterRef->AsNumber());
//		}
//	}
//	TArray<int32> ChildrenIds;
//	// Shapes
//	if (InJsonObject->HasField(TEXT("shapes")))
//	{
//		const TArray<TSharedPtr<FJsonValue>> & TempShapes = InJsonObject->GetArrayField(TEXT("shapes"));
//		for (const auto & Ref : TempShapes)
//		{
//			TSharedPtr<FSCTShape> Shape = FSCTShapeManager::Get()->ParseShapeFromJson(Ref->AsObject());
//			FSCTShapeManager::Get()->AddChildShape(Shape);
//			int32 ID = Ref->AsObject()->GetIntegerField(TEXT("id"));
//			OpetionsMetalsShapesList.Add(ID, Shape);
//			if (StaticCast<EShapeType>(Ref->AsObject()->GetIntegerField(TEXT("type"))) != ST_Accessory) continue;									
//			if (ChildrenIds.FindByKey(ID) == nullptr)
//			{
//				FAccessoryToDownload TempAccessoryToDownload;
//				TempAccessoryToDownload.ModelID = Ref->AsObject()->GetIntegerField(TEXT("modelId"));
//				TempAccessoryToDownload.Md5 = Ref->AsObject()->GetStringField(TEXT("fileMd5"));
//				TempAccessoryToDownload.Url = Ref->AsObject()->GetStringField(TEXT("fileUrl"));
//				TempAccessoryToDownload.UpdateTime = Ref->AsObject()->GetNumberField(TEXT("updateTime"));
//				ToDownloadAccessoryList.Emplace(MoveTemp(TempAccessoryToDownload));
//			}			
//		}
//	}
//	ProcessOptionsMetalType();
//}
//
//void FDoorShape::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
//{
//	FSCTShape::ParseAttributesFromJson(InJsonObject);
//	// 基材
//	if (InJsonObject->HasField(TEXT("substrate")))
//	{
//		const TSharedPtr<FJsonObject> TempSubstrateObj = InJsonObject->GetObjectField(TEXT("substrate"));
//		SetSubstrateID(TempSubstrateObj->GetIntegerField(TEXT("id")));
//		SetSubstrateName(TempSubstrateObj->GetStringField(TEXT("name")));
//	}	
//	// 材质
//	if (InJsonObject->HasField(TEXT("material")))
//	{
//		const TSharedPtr<FJsonObject> TempMaterialObj = InJsonObject->GetObjectField(TEXT("material"));
//		FDoorPakMetaData & PakRef = GetMaterial();
//		PakRef.ID = TempMaterialObj->GetIntegerField(TEXT("id"));
//		PakRef.Name = TempMaterialObj->GetStringField(TEXT("name"));
//		PakRef.Url = TempMaterialObj->GetStringField(TEXT("pakUrl"));
//		PakRef.MD5 = TempMaterialObj->GetStringField(TEXT("pakMd5"));
//		PakRef.ThumbnailUrl = TempMaterialObj->GetStringField(TEXT("thumbnailUrl"));
//	}
//
//	// 门缝
//	if (InJsonObject->HasField(TEXT("doorSlotValue")))
//	{
//		TArray<int32> & DoorSlotValuesRef = GetDoorSlotValues();
//		const TArray<TSharedPtr<FJsonValue>> TempDoorSlotArray = InJsonObject->GetArrayField(TEXT("doorSlotValue"));
//		for (const auto & IterRef : TempDoorSlotArray)
//		{
//			DoorSlotValuesRef.Emplace(IterRef->AsNumber());
//		}
//	}
//	// 延伸方式
//	if (InJsonObject->HasField(TEXT("extensionValue")))
//	{
//		TArray<int32> & ExtensionValuesRef = GetExtensionValues();
//		const TArray<TSharedPtr<FJsonValue>> TempExtensionArray = InJsonObject->GetArrayField(TEXT("extensionValue"));
//		for (const auto & IterRef : TempExtensionArray)
//		{
//			ExtensionValuesRef.Emplace(IterRef->AsNumber());
//		}
//	}
//}
//
//void FDoorShape::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
//{
//	FSCTShape::ParseShapeFromJson(InJsonObject);
//	if (InJsonObject->HasField(TEXT("shapes")))
//	{
//		const TArray<TSharedPtr<FJsonValue>> & TempShapes = InJsonObject->GetArrayField(TEXT("shapes"));
//		for (const auto & Ref : TempShapes)
//		{
//			TSharedPtr<FSCTShape> Shape = FSCTShapeManager::Get()->ParseShapeFromJson(Ref->AsObject());
//			FSCTShapeManager::Get()->AddChildShape(Shape);
//			OpetionsMetalsShapesList.Add(Ref->AsObject()->GetIntegerField(TEXT("id")), Shape);
//		}
//	}
//}
//
//void FDoorShape::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
//{
//	FSCTShape::ParseContentFromJson(InJsonObject);
//	//0、类型
//	int32 ShapeType = InJsonObject->GetIntegerField(TEXT("type"));
//	SetShapeType((EShapeType)ShapeType);
//	int32 ShapeCategory = InJsonObject->GetIntegerField(TEXT("category"));
//	SetShapeCategory(ShapeCategory);
//	//1、名称
//	FString ShapeName = InJsonObject->GetStringField(TEXT("name"));
//	SetShapeName(ShapeName);
//	//3、Id
//	int32 ShapeId = InJsonObject->GetIntegerField(TEXT("id"));
//	SetShapeId(ShapeId);
//	//4、编码
//	FString ShapeCode = InJsonObject->GetStringField(TEXT("code"));
//	SetShapeCode(ShapeCode);
//	//5、缩略图URL
//	FString ThumbnailUrl = InJsonObject->GetStringField(TEXT("thumbnailUrl"));
//	SetThumbnailUrl(ThumbnailUrl);
//		
//}
//
//void FDoorShape::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
//{
//	
//	FSCTShape::SaveToJson(JsonWriter);
//	// 基材
//	{
//		JsonWriter->WriteObjectStart(TEXT("substrate"));
//		JsonWriter->WriteValue(TEXT("id"), GetSubstrateID());
//		JsonWriter->WriteValue(TEXT("name"), GetSubstrateName());
//		JsonWriter->WriteObjectEnd();
//	}
//
//	// 材质
//	{
//		JsonWriter->WriteObjectStart(TEXT("material"));
//		const FDoorPakMetaData & PakRef = GetMaterial();
//		JsonWriter->WriteValue(TEXT("id"), PakRef.ID);
//		JsonWriter->WriteValue(TEXT("name"), PakRef.Name);
//		JsonWriter->WriteValue(TEXT("pakUrl"), PakRef.Url);
//		JsonWriter->WriteValue(TEXT("pakMd5"), PakRef.MD5);
//		JsonWriter->WriteValue(TEXT("thumbnailUrl"), PakRef.ThumbnailUrl);
//		JsonWriter->WriteObjectEnd();
//	}
//
//	// 门缝
//	{
//		JsonWriter->WriteArrayStart(TEXT("doorSlotValue"));
//		const TArray<int32> & DoorSlotValuesRef = GetDoorSlotValues();
//		for (const auto & Ref : DoorSlotValuesRef)
//		{
//			JsonWriter->WriteValue(Ref);
//		}
//		JsonWriter->WriteArrayEnd();
//	}
//
//	// 延伸方式
//	{
//		JsonWriter->WriteArrayStart(TEXT("extensionValue"));
//		const TArray<int32> & ExtensionValues = GetExtensionValues();
//		for (const auto & Ref : ExtensionValues)
//		{
//			JsonWriter->WriteValue(Ref);
//		}
//		JsonWriter->WriteArrayEnd();
//	}
//
//}
//
//void FDoorShape::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
//{
//	FSCTShape::SaveAttriToJson(JsonWriter);
//	// 基材
//	{
//		JsonWriter->WriteObjectStart(TEXT("substrate"));
//		JsonWriter->WriteValue(TEXT("id"), GetSubstrateID());
//		JsonWriter->WriteValue(TEXT("name"), GetSubstrateName());
//		JsonWriter->WriteObjectEnd();
//	}
//	// 材质
//	{
//		JsonWriter->WriteObjectStart(TEXT("material"));
//		const FDoorPakMetaData & PakRef = GetMaterial();
//		JsonWriter->WriteValue(TEXT("id"), PakRef.ID);
//		JsonWriter->WriteValue(TEXT("name"), PakRef.Name);
//		JsonWriter->WriteValue(TEXT("pakUrl"), PakRef.Url);
//		JsonWriter->WriteValue(TEXT("pakMd5"), PakRef.MD5);
//		JsonWriter->WriteValue(TEXT("thumbnailUrl"), PakRef.ThumbnailUrl);
//		JsonWriter->WriteObjectEnd();
//	}
//	// 门缝
//	{
//		JsonWriter->WriteArrayStart(TEXT("doorSlotValue"));
//		const TArray<int32> & DoorSlotValuesRef = GetDoorSlotValues();
//		for (const auto & Ref : DoorSlotValuesRef)
//		{
//			JsonWriter->WriteValue(Ref);
//		}
//		JsonWriter->WriteArrayEnd();
//	}
//
//	// 延伸方式
//	{
//		JsonWriter->WriteArrayStart(TEXT("extensionValue"));
//		const TArray<int32> & ExtensionValues = GetExtensionValues();
//		for (const auto & Ref : ExtensionValues)
//		{
//			JsonWriter->WriteValue(Ref);
//		}
//		JsonWriter->WriteArrayEnd();
//	}
//}
//
//void FDoorShape::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
//{
//	FSCTShape::SaveShapeToJson(JsonWriter);
//}
//
//void FDoorShape::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
//{
//	FSCTShape::SaveContentToJson(JsonWriter);
//}
//
//void FDoorShape::CopyTo(FSCTShape* OutShape)
//{
//	FSCTShape::CopyTo(OutShape);
//	//基础组件类型
//	FDoorShape* OutBaseShape = StaticCast<FDoorShape*>(OutShape);
//	//OutBaseShape->BaseShapeType = BaseShapeType;
//	// 基材
//	{
//		OutBaseShape->SetSubstrateID(GetSubstrateID());
//		OutBaseShape->SetSubstrateName(GetSubstrateName());
//	}
//	// 门缝
//	{
//		OutBaseShape->SetDoorSlotValues(GetDoorSlotValues());
//	}
//
//	// 延伸方式
//	{
//		OutBaseShape->SetExtensionValues(GetExtensionValues());
//	}
//	OutBaseShape->OpetionsMetalsShapesList = OpetionsMetalsShapesList;
//}
//
//ASCTShapeActor* FDoorShape::SpawnShapeActor()
//{
//	//创建生成型录Actor
//	UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();
//	ASCTDoorActor* NewShapeActor = World->SpawnActor<ASCTDoorActor>(
//		ASCTDoorActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
//
//	//设置型录位置和旋转
//	FVector ActorPos(GetShapePosX(), GetShapePosY(), GetShapePosZ());
//	NewShapeActor->SetActorPosition(ActorPos);
//	FRotator ActorRot(GetShapeRotY(), GetShapeRotZ(), GetShapeRotX());
//	NewShapeActor->SetActorRotator(ActorRot);
//
//	//设置型录可见性
//	bool bParentVisible = NewShapeActor->GetVisibleStateFromParent();
//	NewShapeActor->SetActorHiddenInGame(!GetShowCondition() || !bParentVisible);
//
//	//设置关联的Shape
//	NewShapeActor->SetShape(this);
//
//	ShapeActor = NewShapeActor;
//	return NewShapeActor;
//}
//
//void FDoorShape::SetCollisionProfileName(FName InProfileName)
//{
//	FSCTShape::SetCollisionProfileName(InProfileName);
//}
//
//void FDoorShape::GetResourceUrls(TArray<FString>& OutResourceUrls)
//{
//	FSCTShape::GetResourceUrls(OutResourceUrls);
//	for (const auto & Ref : ToDownloadAccessoryList)
//	{
//		if (Ref.Url.IsEmpty()) continue;
//		OutResourceUrls.Add(Ref.Url);
//	}
//}
//
//void FDoorShape::GetFileCachePaths(TArray<FString>& OutFileCachePaths)
//{	
//	FSCTShape::GetFileCachePaths(OutFileCachePaths);	
//	for (const auto & Ref : ToDownloadAccessoryList)
//	{
//		if (Ref.Url.IsEmpty()) continue;
//		OutFileCachePaths.Add(FDecorationShape::GetCacheFilePathFromIDAndMd5AndUpdateTime(Ref.ModelID, Ref.Md5, Ref.UpdateTime));
//	}
//}
//
//void FDoorShape::CheckResourceUrlsAndCachePaths(TArray<FString>& OutResourceUrls, TArray<FString>& OutFileCachePaths)
//{
//	class FindRepeatResourceIndex
//	{
//	public:
//		FindRepeatResourceIndex(const TArray<FString>& InFileCachePaths):CachePaths(InFileCachePaths) {}
//	public:
//		void operator()(const FString & InStr)
//		{			
//			++Index;
//			bool IsAlreadyExist = false;
//			UniqueStrings.Add(InStr, &IsAlreadyExist);
//			if (IsAlreadyExist == false)
//			{
//				int32 FindIndex = CachePaths.Contains(InStr);
//				check(FindIndex != -1);	
//				SaveIndices.Add(Index);
//			}
//		}
//	public:
//		TArray<int32> SaveIndices;
//		TSet<FString> UniqueStrings;
//		int32 Index = -1;
//		const TArray<FString> & CachePaths;
//	};
//	FindRepeatResourceIndex FindRepeatResourceIndex(OutFileCachePaths);
//	for (const auto & Ref : OutFileCachePaths)
//	{
//		FindRepeatResourceIndex(Ref);
//	}
//	
//	TArray<FString> RetCachePaths;
//	TArray<FString> RetUrls;
//	for (const auto & Ref : FindRepeatResourceIndex.UniqueStrings)
//	{
//		RetCachePaths.Add(Ref);
//	}
//	for (const auto & Ref : FindRepeatResourceIndex.SaveIndices)
//	{
//		RetUrls.Add(OutResourceUrls[Ref]);
//	}
//	OutResourceUrls = RetUrls;
//	OutFileCachePaths = RetCachePaths;
//}
//
//FDrawerShape::FDrawerShape()
//{
//	SetShapeType(ST_DrawerDoor);
//	HandleAttriX = MakeShareable(new FNumberRangeAttri(this));
//	HandleAttriZ = MakeShareable(new FNumberRangeAttri(this));
//	LockAttriX = MakeShareable(new FNumberRangeAttri(this));
//	LockAttriZ = MakeShareable(new FNumberRangeAttri(this));
//
//	HandlePosHSelectXAtt = MakeShareable(new FTextSelectAttri(this));
//	HandlePosHSelectXAtt->AddSelectValue(TEXT("1"), TEXT("距左边"));
//	HandlePosHSelectXAtt->AddSelectValue(TEXT("2"), TEXT("距右边"));
//	HandlePosHSelectXAtt->AddSelectValue(TEXT("3"), TEXT("水平居中"));
//	HandlePosHSelectXAtt->AddSelectValue(TEXT("4"), TEXT("公式"));
//	HandlePosHSelectXAtt->SetAttributeValue(TEXT("3"));	
//
//	HanleRotateDirection = MakeShareable(new FTextSelectAttri(this));
//	HanleRotateDirection->AddSelectValue(TEXT("0"),TEXT("保持不变"));
//	HanleRotateDirection->AddSelectValue(TEXT("1"), TEXT("左旋90度"));
//	HanleRotateDirection->AddSelectValue(TEXT("2"), TEXT("右旋90度"));
//	HanleRotateDirection->SetAttributeValue(TEXT("0"));
//
//	HandlePosVSelectXAtt = MakeShareable(new FTextSelectAttri(this));
//	HandlePosVSelectXAtt->AddSelectValue(TEXT("1"), TEXT("距上边"));
//	HandlePosVSelectXAtt->AddSelectValue(TEXT("2"), TEXT("距下边"));
//	HandlePosVSelectXAtt->AddSelectValue(TEXT("3"), TEXT("垂直居中"));
//	HandlePosVSelectXAtt->AddSelectValue(TEXT("4"), TEXT("公式"));
//	HandlePosVSelectXAtt->SetAttributeValue(TEXT("3"));
//	
//
//	LockRotateDirection = MakeShareable(new FTextSelectAttri(this));
//	LockRotateDirection->AddSelectValue(TEXT("0"), TEXT("保持不变"));
//	LockRotateDirection->AddSelectValue(TEXT("1"), TEXT("左旋90度"));
//	LockRotateDirection->AddSelectValue(TEXT("2"), TEXT("右旋90度"));
//	LockRotateDirection->SetAttributeValue(TEXT("0"));
//
//	LockPosHSelectXAtt = MakeShareable(new FTextSelectAttri(this));
//	LockPosHSelectXAtt->AddSelectValue(TEXT("1"), TEXT("距左边"));
//	LockPosHSelectXAtt->AddSelectValue(TEXT("2"), TEXT("距右边"));
//	LockPosHSelectXAtt->AddSelectValue(TEXT("3"), TEXT("水平居中"));
//	LockPosHSelectXAtt->AddSelectValue(TEXT("4"), TEXT("公式"));
//	LockPosHSelectXAtt->SetAttributeValue(TEXT("3"));
//	
//
//	LockPosVSelectXAtt = MakeShareable(new FTextSelectAttri(this));
//	LockPosVSelectXAtt->AddSelectValue(TEXT("1"), TEXT("距上边"));
//	LockPosVSelectXAtt->AddSelectValue(TEXT("2"), TEXT("距下边"));
//	LockPosVSelectXAtt->AddSelectValue(TEXT("3"), TEXT("垂直居中"));
//	LockPosVSelectXAtt->AddSelectValue(TEXT("4"), TEXT("公式"));
//	LockPosVSelectXAtt->SetAttributeValue(TEXT("3"));
//
//	HandleListSelectAtt = MakeShareable(new FTextSelectAttri(this));
//	LockListSelectAtt = MakeShareable(new FTextSelectAttri(this));
//
//}
//
//FDrawerShape::~FDrawerShape()
//{	
//}
//
//void FDrawerShape::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
//{
//	FDoorShape::ParseFromJson(InJsonObject);	
//	// 封边
//	if (InJsonObject->HasField(TEXT("edgeBanding")))
//	{
//		const TSharedPtr<FJsonObject> TempEdgeBandingObj = InJsonObject->GetObjectField(TEXT("edgeBanding"));
//		FDoorPakMetaData & PakRef = GetEdgeBanding();
//		PakRef.ID = TempEdgeBandingObj->GetIntegerField(TEXT("id"));
//		PakRef.Name = TempEdgeBandingObj->GetStringField(TEXT("name"));
//		PakRef.Url = TempEdgeBandingObj->GetStringField(TEXT("pakUrl"));
//		PakRef.MD5 = TempEdgeBandingObj->GetStringField(TEXT("pakMd5"));
//		PakRef.ThumbnailUrl = TempEdgeBandingObj->GetStringField(TEXT("thumbnailUrl"));
//	}
//	// 连接模式
//	if (InJsonObject->HasField(TEXT("connectionMode")))
//	{
//		TArray<int32> & ConnectModelRef = GetConnectMode();
//		const TArray<TSharedPtr<FJsonValue>> TempConnectModeArray = InJsonObject->GetArrayField(TEXT("connectionMode"));
//		for (const auto & IterRef : TempConnectModeArray)
//		{
//			ConnectModelRef.Emplace(IterRef->AsNumber());
//		}
//	}
//	// 开门方向
//	if (InJsonObject->HasField(TEXT("openDoorDirection")))
//	{
//		SetOpenDirection(InJsonObject->GetIntegerField(TEXT("openDoorDirection")));
//	}
//	// 覆盖方式
//	if (InJsonObject->HasField(TEXT("coverUpWay")))
//	{
//		SetConverUpWay(InJsonObject->GetIntegerField(TEXT("coverUpWay")));
//	}
//	for (const auto Ref : GetOptionShapeMap())
//	{
//		EMetalsType MetalType = StaticCast<EMetalsType>(Ref.Value->GetShapeCategory());
//		FAuxiliaryMetalInfo Info;
//		Info.ID = Ref.Value->GetShapeId();
//		Info.Name = Ref.Value->GetShapeName();
//		if (MetalType == EMetalsType::MT_SHAKE_HANDS)
//		{
//			HandleList.Add(MoveTemp(Info));
//		}
//		else if (MetalType == EMetalsType::MT_LOCK)
//		{
//			LockList.Add(MoveTemp(Info));
//		}
//	}
//	
//	auto ParseDividBlockShape = [this](TSharedPtr<FDecorationShape> InShape, const TSharedPtr<FJsonObject>& InShapeJsonObject,const FString & InFileName)
//	{		
//		InShape->ParseFromJson(InShapeJsonObject);
//		auto ConvertFormula = [](const FString & InStr)->FString
//		{
//			FString RetStr = InStr;
//			static TArray<TPair<FString, FString>> ToFixPairs =
//			{
//				TPair<FString,FString>(TEXT("w"),TEXT("pval(\"W\")")),
//				TPair<FString,FString>(TEXT("h"),TEXT("pval(\"H\")")),
//			};
//			for (const auto & Ref : ToFixPairs)
//			{
//				RetStr = RetStr.Replace(*(Ref.Key), *(Ref.Value), ESearchCase::Type::CaseSensitive);
//			}			
//			return RetStr;
//		};
//		auto ParseCurrentValue = [ConvertFormula,InShape](const FString & InAttriName, const TSharedPtr<FJsonObject>& InShapeJsonObject)->FString
//		{
//			const TSharedPtr<FJsonObject> & JsonObj = InShapeJsonObject->GetObjectField(InAttriName);
//			return ConvertFormula(JsonObj->GetStringField(TEXT("current")));
//		};		
//		InShape->SetShapeWidth(ParseCurrentValue(TEXT("width"), InShapeJsonObject));
//		InShape->SetShapeHeight(ParseCurrentValue(TEXT("height"), InShapeJsonObject));									
//		InShape->SetShapePosX(ParseCurrentValue(TEXT("posx"), InShapeJsonObject));		
//		InShape->SetShapePosZ(ParseCurrentValue(TEXT("posz"), InShapeJsonObject));
//		InShape->SetFileUrl(InShapeJsonObject->GetStringField(TEXT("pakUrl")));												
//		InShape->SetShapeType(ST_Decoration);				
//		InShape->SetFileName(InFileName);	
//	};
//	
//	// 掩门切块	
//	if (InJsonObject->HasField(TEXT("divideBlocks")))
//	{
//		const TArray<TSharedPtr<FJsonValue>> & TempDividBlocks = InJsonObject->GetArrayField(TEXT("divideBlocks"));	
//		int Index = 0;
//		for (const auto & Ref : TempDividBlocks)
//		{
//			const TSharedPtr<FJsonObject> & Obj = Ref->AsObject();						
//			TSharedPtr<FDecorationShape> ShapePtr = MakeShareable(new FDecorationShape());	
//			AddBaseChildShape(ShapePtr);	
//			const FString  CombineFileName = FString::Printf(TEXT("%s_%d_%d"), TEXT("divideBlocks"), GetShapeId(), ++Index);
//			ParseDividBlockShape(ShapePtr, Obj, CombineFileName);			
//		}
//	}
//
//	// 五金 
//	if (InJsonObject->HasField(TEXT("children")))
//	{
//		const TArray<TSharedPtr<FJsonValue>> & TempChildren = InJsonObject->GetArrayField(TEXT("children"));
//		for (const auto & Ref : TempChildren)
//		{
//			const TSharedPtr<FJsonObject> & Obj = Ref->AsObject();
//			int32 ShapeId = Obj->GetIntegerField(TEXT("id"));		
//			TSharedPtr<FSCTShape> NewShape = FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ST_Accessory, ShapeId);
//			TSharedPtr<FSCTShape> CopyShape = FSCTShapeManager::Get()->CopyShapeToNew(NewShape);			
//			AddBaseChildShape(CopyShape);
//			CopyShape->ParseAttributesFromJson(Obj);
//		}
//	}	
//	
//}
//
//void FDrawerShape::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
//{	
//	FDoorShape::ParseAttributesFromJson(InJsonObject);	
//	// 封边
//	if (InJsonObject->HasField(TEXT("edgeBanding")))
//	{
//		const TSharedPtr<FJsonObject> TempEdgeBandingObj = InJsonObject->GetObjectField(TEXT("edgeBanding"));
//		FDoorPakMetaData & PakRef = GetEdgeBanding();
//		PakRef.ID = TempEdgeBandingObj->GetIntegerField(TEXT("id"));
//		PakRef.Name = TempEdgeBandingObj->GetStringField(TEXT("name"));
//		PakRef.Url = TempEdgeBandingObj->GetStringField(TEXT("pakUrl"));
//		PakRef.MD5 = TempEdgeBandingObj->GetStringField(TEXT("pakMd5"));
//		PakRef.ThumbnailUrl = TempEdgeBandingObj->GetStringField(TEXT("thumbnailUrl"));
//	}
//	// 连接模式
//	if (InJsonObject->HasField(TEXT("connectionMode")))
//	{
//		TArray<int32> & ConnectModelRef = GetConnectMode();
//		const TArray<TSharedPtr<FJsonValue>> TempConnectModeArray = InJsonObject->GetArrayField(TEXT("connectionMode"));
//		for (const auto & IterRef : TempConnectModeArray)
//		{
//			ConnectModelRef.Emplace(IterRef->AsNumber());
//		}
//	}
//	// 开门方向
//	if (InJsonObject->HasField(TEXT("openDoorDirection")))
//	{
//		SetOpenDirection(InJsonObject->GetIntegerField(TEXT("openDoorDirection")));
//	}
//	// 覆盖方式
//	if (InJsonObject->HasField(TEXT("coverUpWay")))
//	{
//		SetConverUpWay(InJsonObject->GetIntegerField(TEXT("coverUpWay")));
//	}
//	auto ParseDividBlockShape = [](TSharedPtr<FDecorationShape> InShape, const TSharedPtr<FJsonObject>& InShapeJsonObject, const FString & InFileName)
//	{
//		InShape->ParseFromJson(InShapeJsonObject);
//		auto ConvertFormula = [](const FString & InStr)->FString
//		{
//			FString RetStr = InStr;
//			static TArray<TPair<FString, FString>> ToFixPairs =
//			{
//				TPair<FString,FString>(TEXT("w"),TEXT("pval(\"W\")")),
//				TPair<FString,FString>(TEXT("h"),TEXT("pval(\"H\")")),
//			};
//			for (const auto & Ref : ToFixPairs)
//			{
//				RetStr = RetStr.Replace(*(Ref.Key), *(Ref.Value), ESearchCase::Type::CaseSensitive);
//			}
//			return RetStr;
//		};
//		auto ParseCurrentValue = [ConvertFormula, InShape](const FString & InAttriName, const TSharedPtr<FJsonObject>& InShapeJsonObject)->FString
//		{
//			const TSharedPtr<FJsonObject> & JsonObj = InShapeJsonObject->GetObjectField(InAttriName);
//			return ConvertFormula(JsonObj->GetStringField(TEXT("current")));
//		};
//		InShape->SetShapeWidth(ParseCurrentValue(TEXT("width"), InShapeJsonObject));
//		InShape->SetShapeHeight(ParseCurrentValue(TEXT("height"), InShapeJsonObject));
//		InShape->SetShapePosX(ParseCurrentValue(TEXT("posx"), InShapeJsonObject));
//		InShape->SetShapePosZ(ParseCurrentValue(TEXT("posz"), InShapeJsonObject));
//		InShape->SetFileUrl(InShapeJsonObject->GetStringField(TEXT("pakUrl")));
//		InShape->SetShapeType(ST_Decoration);
//		InShape->SetFileName(InFileName);
//	};
//
//	// 掩门切块	
//	if (InJsonObject->HasField(TEXT("divideBlocks")))
//	{
//		const TArray<TSharedPtr<FJsonValue>> & TempDividBlocks = InJsonObject->GetArrayField(TEXT("divideBlocks"));		
//		int Index = 0;
//		for (const auto & Ref : TempDividBlocks)
//		{
//			const TSharedPtr<FJsonObject> & Obj = Ref->AsObject();
//			TSharedPtr<FDecorationShape> ShapePtr = MakeShareable(new FDecorationShape());
//			AddBaseChildShape(ShapePtr);
//			const FString  CombineFileName = FString::Printf(TEXT("%s_%d_%d"), TEXT("divideBlocks"), GetShapeId(), ++Index);
//			ParseDividBlockShape(ShapePtr, Obj, CombineFileName);
//		}
//	}
//	// 五金 
//	if (InJsonObject->HasField(TEXT("children")))
//	{
//		const TArray<TSharedPtr<FJsonValue>> & TempMetals = InJsonObject->GetArrayField(TEXT("children"));
//		for (const auto & Ref : TempMetals)
//		{
//			const TSharedPtr<FJsonObject> & Obj = Ref->AsObject();
//			int64 ShapeId = Obj->GetNumberField(TEXT("id"));
//			TSharedPtr<FSCTShape> NewShape = FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ST_Accessory, ShapeId);
//			if (!NewShape.IsValid()) continue;
//			TSharedPtr<FSCTShape> CopyShape = FSCTShapeManager::Get()->CopyShapeToNew(NewShape);
//			AddBaseChildShape(CopyShape);
//			CopyShape->ParseAttributesFromJson(Obj);
//		}
//	}
//	auto SetDefaultAuxiliaryMetalInfo = [InJsonObject](FAuxiliaryMetalInfo & OutInfo, const FString & InJsonObjName)
//	{
//		if (!InJsonObject->HasField(InJsonObjName)) return;
//		{
//			const TSharedPtr<FJsonObject> & PosJsonObj = InJsonObject->GetObjectField(InJsonObjName);
//			OutInfo.ID = PosJsonObj->GetIntegerField(TEXT("id"));
//			OutInfo.Name = PosJsonObj->GetStringField(TEXT("name"));
//			OutInfo.Location.Hpos = PosJsonObj->GetIntegerField(TEXT("hpos"));
//			OutInfo.Location.Vpos = PosJsonObj->GetIntegerField(TEXT("vpos"));
//			OutInfo.Location.RotateDirection = PosJsonObj->GetIntegerField(TEXT("rotationalDirection"));
//			OutInfo.Location.HposVal = PosJsonObj->GetNumberField(TEXT("hposVal"));
//			OutInfo.Location.VposVAl = PosJsonObj->GetNumberField(TEXT("vposVal"));
//		}						
//	};
//	SetDefaultAuxiliaryMetalInfo(DefaultHandleLocationInfo, TEXT("handleInfo"));
//	SetDefaultAuxiliaryMetalInfo(DefaultLockLocationInfo, TEXT("lockInfo"));
//
//	// 存储拉手列表 
//	auto ParseHandleOrLockList = [InJsonObject](const FString & InName, TArray<FAuxiliaryMetalInfo> & InList)
//	{
//		if (!InJsonObject->HasField(InName)) return;
//		const TArray<TSharedPtr<FJsonValue>> & Arrays = InJsonObject->GetArrayField(InName);
//		for (const auto & Ref : Arrays)
//		{
//			FAuxiliaryMetalInfo Temp;
//			Temp.ID = Ref->AsObject()->GetIntegerField(TEXT("id"));
//			Temp.Name = Ref->AsObject()->GetStringField(TEXT("name"));			
//			InList.Emplace(MoveTemp(Temp));
//		}		
//	};
//	ParseHandleOrLockList(TEXT("hanleList"), HandleList);
//	ParseHandleOrLockList(TEXT("lockList"), LockList);
//
//}
//
//void FDrawerShape::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
//{	
//	FDoorShape::ParseShapeFromJson(InJsonObject);		
//}
//
//void FDrawerShape::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
//{
//	FDoorShape::ParseContentFromJson(InJsonObject);
//}
//
//void FDrawerShape::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
//{
//	FDoorShape::SaveToJson(JsonWriter);	
//	// 封边
//	{
//		JsonWriter->WriteObjectStart(TEXT("edgeBanding"));
//		const FDoorPakMetaData & PakRef = GetEdgeBanding();
//		JsonWriter->WriteValue(TEXT("id"), PakRef.ID);
//		JsonWriter->WriteValue(TEXT("name"), PakRef.Name);
//		JsonWriter->WriteValue(TEXT("pakUrl"), PakRef.Url);
//		JsonWriter->WriteValue(TEXT("pakMd5"), PakRef.MD5);
//		JsonWriter->WriteValue(TEXT("thumbnailUrl"), PakRef.ThumbnailUrl);
//		JsonWriter->WriteObjectEnd();
//	}
//	// 连接模式	
//	{
//		JsonWriter->WriteArrayStart(TEXT("connectionMode"));
//		const TArray<int32> & ConnectModelRef = GetConnectMode();
//		for (const auto & Ref : ConnectModelRef)
//		{
//			JsonWriter->WriteValue(Ref);
//		}
//		JsonWriter->WriteArrayEnd();
//	}
//	// 开门方向
//	JsonWriter->WriteValue(TEXT("openDoorDirection"), GetOpenDirection());
//
//	// 覆盖方式	
//	JsonWriter->WriteValue(TEXT("coverUpWay"), GetConverUpWay());
//
//	// 存储门切块
//	JsonWriter->WriteArrayStart(TEXT("divideBlocks"));
//	for (const auto & Ref : DividBlocks)
//	{
//		JsonWriter->WriteObjectStart();
//		Ref->SaveToJson(JsonWriter);
//		JsonWriter->WriteObjectEnd();
//	}
//	JsonWriter->WriteArrayEnd();
//
//	// 存储门所用到的五金
//	JsonWriter->WriteArrayStart(TEXT("children"));
//	for (const auto & Ref : Metals)
//	{
//		JsonWriter->WriteObjectStart();
//		Ref->SaveToJson(JsonWriter);
//		JsonWriter->WriteObjectEnd();
//	}
//	JsonWriter->WriteArrayEnd();		
//}
//
//void FDrawerShape::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
//{
//	FDoorShape::SaveAttriToJson(JsonWriter);	
//
//	// 封边
//	{
//		JsonWriter->WriteObjectStart(TEXT("edgeBanding"));
//		const FDoorPakMetaData & PakRef = GetEdgeBanding();
//		JsonWriter->WriteValue(TEXT("id"), PakRef.ID);
//		JsonWriter->WriteValue(TEXT("name"), PakRef.Name);
//		JsonWriter->WriteValue(TEXT("pakUrl"), PakRef.Url);
//		JsonWriter->WriteValue(TEXT("pakMd5"), PakRef.MD5);
//		JsonWriter->WriteValue(TEXT("thumbnailUrl"), PakRef.ThumbnailUrl);
//		JsonWriter->WriteObjectEnd();
//	}
//
//	// 连接模式	
//	{
//		JsonWriter->WriteArrayStart(TEXT("connectionMode"));
//		const TArray<int32> & ConnectModelRef = GetConnectMode();
//		for (const auto & Ref : ConnectModelRef)
//		{
//			JsonWriter->WriteValue(Ref);
//		}
//		JsonWriter->WriteArrayEnd();
//	}
//
//	// 开门方向
//	JsonWriter->WriteValue(TEXT("openDoorDirection"), GetOpenDirection());
//
//	// 覆盖方式	
//	JsonWriter->WriteValue(TEXT("coverUpWay"), GetConverUpWay());
//
//	// 存储门切块
//	JsonWriter->WriteArrayStart(TEXT("divideBlocks"));
//
//	auto WriteAttribute = [](const FString & InName,const FString & InRefName,const TSharedPtr<FShapeAttribute> & InAttri, TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
//	{
//		TSharedPtr<FNumberRangeAttri> RangeAttri = StaticCastSharedPtr<FNumberRangeAttri>(InAttri);
//		JsonWriter->WriteObjectStart(InName);
//		JsonWriter->WriteValue(TEXT("id"), RangeAttri->GetAttributeId());
//		JsonWriter->WriteValue(TEXT("type"), (int32)RangeAttri->GetAttributeType());
//		JsonWriter->WriteValue(TEXT("refName"), InRefName);
//		JsonWriter->WriteObjectStart(TEXT("value"));
//		JsonWriter->WriteValue(TEXT("min"), RangeAttri->GetMinValue());
//		JsonWriter->WriteValue(TEXT("max"), RangeAttri->GetMaxValue());
//		JsonWriter->WriteObjectEnd();
//		JsonWriter->WriteValue(TEXT("current"), RangeAttri->GetAttributeStr());
//		JsonWriter->WriteObjectEnd();
//	};
//
//	for (const auto & Ref : DividBlocks)
//	{
//		JsonWriter->WriteObjectStart();
//		WriteAttribute(TEXT("width"),TEXT("WIDTH"), Ref->GetShapeWidthAttri(), JsonWriter);
//		WriteAttribute(TEXT("depth"), TEXT("DEPTH"), Ref->GetShapeDepthAttri(), JsonWriter);
//		WriteAttribute(TEXT("height"), TEXT("HEIGHT"), Ref->GetShapeHeightAttri(), JsonWriter);
//		WriteAttribute(TEXT("posx"), TEXT("POSX"), Ref->GetShapePosXAttri(), JsonWriter);
//		WriteAttribute(TEXT("posy"), TEXT("POSY"), Ref->GetShapePosYAttri(), JsonWriter);
//		WriteAttribute(TEXT("posz"), TEXT("POSZ"), Ref->GetShapePosZAttri(), JsonWriter);
//		WriteAttribute(TEXT("rotx"), TEXT("ROTX"), Ref->GetShapeRotXAttri(), JsonWriter);
//		WriteAttribute(TEXT("roty"), TEXT("ROTY"), Ref->GetShapeRotYAttri(), JsonWriter);
//		WriteAttribute(TEXT("rotz"), TEXT("ROTZ"), Ref->GetShapeRotZAttri(), JsonWriter);
//		JsonWriter->WriteValue(TEXT("pakUrl"), Ref->GetFileUrl());
//		JsonWriter->WriteValue(TEXT("fileName"), Ref->GetFileName());
//		JsonWriter->WriteValue(TEXT("mwassetUrl"), TEXT(""));
//		JsonWriter->WriteValue(TEXT("optimizeParam"), TEXT(""));
//		JsonWriter->WriteObjectEnd();
//	}
//	JsonWriter->WriteArrayEnd();	
//	auto SetDefaultAuxiliaryMetalInfo = [JsonWriter](const FAuxiliaryMetalInfo & InInfo,const FString & InJsonObjName,const int32 InType)
//	{
//		if (InInfo.ID != -1)
//		{
//			JsonWriter->WriteObjectStart(InJsonObjName);
//			JsonWriter->WriteValue(TEXT("id"), InInfo.ID);
//			JsonWriter->WriteValue(TEXT("name"), InInfo.Name);
//			JsonWriter->WriteValue(TEXT("hpos"), InInfo.Location.Hpos);
//			JsonWriter->WriteValue(TEXT("metalsType"), InType);
//			JsonWriter->WriteValue(TEXT("rotationalDirection"), InInfo.Location.RotateDirection);
//			JsonWriter->WriteValue(TEXT("hposVal"), InInfo.Location.HposVal);			
//			JsonWriter->WriteValue(TEXT("vpos"), InInfo.Location.Vpos);
//			JsonWriter->WriteValue(TEXT("vposVal"), InInfo.Location.VposVAl);			
//			JsonWriter->WriteObjectEnd();
//		}
//	};
//	FAuxiliaryMetalInfo HandleInfo = GetDefaultHandleInfo();
//	FAuxiliaryMetalInfo LockInfo = GetDefaultLockInfo();
//	SetDefaultAuxiliaryMetalInfo(HandleInfo, TEXT("handleInfo"), MT_handle);
//	SetDefaultAuxiliaryMetalInfo(LockInfo, TEXT("lockInfo"), MT_Lock);
//	
//	// 存储拉手列表 
//	auto WriteHandleOrLockList = [&JsonWriter](const FString & InName, const TArray<FAuxiliaryMetalInfo> & InList)
//	{
//		JsonWriter->WriteArrayStart(InName);
//		for (const auto & Ref : InList)
//		{
//			JsonWriter->WriteObjectStart();
//			JsonWriter->WriteValue(TEXT("name"), Ref.Name);
//			JsonWriter->WriteValue(TEXT("id"), Ref.ID);
//			JsonWriter->WriteObjectEnd();
//		}
//		JsonWriter->WriteArrayEnd();
//	};
//	WriteHandleOrLockList(TEXT("hanleList"), HandleList);
//	WriteHandleOrLockList(TEXT("lockList"), LockList);
//
//	// 存储门所用到的五金
//	JsonWriter->WriteArrayStart(TEXT("children"));
//	const TArray<TSharedPtr<FAccessoryShape>> & MetalsRef = GetMetals();
//	for (const auto & Ref : MetalsRef)
//	{
//		JsonWriter->WriteObjectStart();
//		Ref->SaveAttriToJson(JsonWriter);
//		JsonWriter->WriteObjectEnd();
//	}
//	JsonWriter->WriteArrayEnd();
//
//}
//
//void FDrawerShape::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
//{	
//	FDoorShape::SaveShapeToJson(JsonWriter);
//	JsonWriter->WriteArrayStart(TEXT("shapes"));
//	auto SaveAccessoryShapes = [this,&JsonWriter](const TArray<FAuxiliaryMetalInfo> & InRef)
//	{
//		for (const auto Ref : InRef)
//		{
//			JsonWriter->WriteObjectStart();
//			TSharedPtr<FSCTShape> Shape = *(GetOptionShapeMap().Find(Ref.ID));
//			Shape->SaveShapeToJson(JsonWriter);
//			JsonWriter->WriteObjectEnd();
//		}
//	};
//	SaveAccessoryShapes(HandleList);
//	SaveAccessoryShapes(LockList);	
//	JsonWriter->WriteArrayEnd();
//}
//
//void FDrawerShape::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
//{
//	FDoorShape::SaveContentToJson(JsonWriter);
//}
//
//void FDrawerShape::CopyTo(FSCTShape * OutShape)
//{
//	//基类操作
//	FDoorShape::CopyTo(OutShape);
//
//	//基础组件类型
//	SideHungDoor* OutBaseShape = StaticCast<SideHungDoor*>(OutShape);
//	//OutBaseShape->BaseShapeType = BaseShapeType;
//	// 材质
//	{
//		OutBaseShape->SetMaterial(GetMaterial());
//	}
//
//	// 封边
//	{
//		OutBaseShape->SetEdgeBanding(GetEdgeBanding());
//	}
//
//	// 连接模式	
//	{
//		OutBaseShape->SetConnectMode(GetConnectMode());
//	}
//
//	// 开门方向	
//	OutBaseShape->SetOpenDirection(GetOpenDirection());
//
//	// 覆盖方式		
//	OutBaseShape->SetConverUpWay(GetConverUpWay());
//
//	//子级型录
//	for (int32 i = 0; i< DividBlocks.Num(); ++i)
//	{
//		TSharedPtr<FSCTShape> DoorDividBlockShape = FSCTShapeManager::Get()->CopyShapeToNew(DividBlocks[i]);
//		OutBaseShape->AddBaseChildShape(DoorDividBlockShape);
//	}
//	for (int32 i = 0; i< Metals.Num(); ++i)
//	{
//		TSharedPtr<FSCTShape> AccessoryShape = FSCTShapeManager::Get()->CopyShapeToNew(Metals[i]);
//		OutBaseShape->AddBaseChildShape(AccessoryShape);
//	}
//	OutBaseShape->LockList = LockList;
//	OutBaseShape->HandleList = HandleList;
//	OutBaseShape->DefaultHandleLocationInfo = DefaultHandleLocationInfo;	
//	OutBaseShape->DefaultLockLocationInfo = DefaultLockLocationInfo;	
//	
//}
//
//ASCTShapeActor * FDrawerShape::SpawnShapeActor()
//{
//	//创建基础组件型录
//	ASCTShapeActor* NewBaseActor = FDoorShape::SpawnShapeActor();
//
//	//创建组件子型录
//	for (auto it = DividBlocks.CreateConstIterator(); it; ++it)
//	{
//		(*it)->LoadModelMesh();
//		ASCTShapeActor* NewBoardActor = (*it)->SpawnShapeActor();
//		NewBoardActor->AttachToActorOverride(NewBaseActor, FAttachmentTransformRules::KeepRelativeTransform);
//
//		//创建型录外包框
//		ASCTWireframeActor* WireFrameActor = (*it)->SpawnWireFrameActor();
//		WireFrameActor->UpdateActorDimension();
//		WireFrameActor->SetActorHiddenInGame(true);
//		WireFrameActor->AttachToActor(NewBoardActor, FAttachmentTransformRules::KeepRelativeTransform);
//	}
//	for (auto it = Metals.CreateConstIterator(); it; ++it)
//	{
//		(*it)->LoadModelMesh();
//		ASCTShapeActor* NewAccessoryActor = (*it)->SpawnShapeActor();
//		NewAccessoryActor->AttachToActorOverride(NewBaseActor, FAttachmentTransformRules::KeepRelativeTransform);
//		
//		//创建型录外包框
//		ASCTWireframeActor* WireFrameActor = (*it)->SpawnWireFrameActor();
//		WireFrameActor->UpdateActorDimension();
//		WireFrameActor->SetActorHiddenInGame(true);
//		WireFrameActor->AttachToActor(NewAccessoryActor, FAttachmentTransformRules::KeepRelativeTransform);		
//	}
//	ReCalDividBlocksPositionAndSize();
//	// 更新材质
//	{		
//		int32 MaterialID = GetMaterial().ID;
//		FString PakUrl = GetMaterial().Url;
//		FString PakMd5 = GetMaterial().MD5;
//		auto ApplayMaterialWithActor = [this](UMaterialInterface * InMaterialInstance, const FString & InErrorMsg)
//		{
//			if (InMaterialInstance)
//			{
//				UpdateMaterial(InMaterialInstance);
//			}
//		};
//		FMaterialLoadCallback MCallback;
//		MCallback.BindLambda(ApplayMaterialWithActor);
//		FMaterialManagerInstatnce::GetIns().GetMaterialManagerPtr()->GetOrCreateMaterialInterface(
//			EMaterialType::Board_Material, PakUrl, FString::FromInt(MaterialID), PakMd5, MCallback);
//		
//	}	
//	return NewBaseActor;
//}
//
//void FDrawerShape::SetCollisionProfileName(FName InProfileName)
//{
//	for (auto it = DividBlocks.CreateConstIterator(); it; ++it)
//	{
//		(*it)->SetCollisionProfileName(InProfileName);
//	}
//	for (auto it = Metals.CreateConstIterator(); it; ++it)
//	{
//		(*it)->SetCollisionProfileName(InProfileName);
//	}
//}
//
//void FDrawerShape::GetResourceUrls(TArray<FString>& OutResourceUrls)
//{
//	FDoorShape::GetResourceUrls(OutResourceUrls);
//	if (!GetEdgeBanding().Url.IsEmpty())
//	{
//		OutResourceUrls.Add(GetEdgeBanding().Url);
//	}
//	if (!GetMaterial().Url.IsEmpty())
//	{
//		OutResourceUrls.Add(GetMaterial().Url);
//	}
//	
//}
//
//void FDrawerShape::GetFileCachePaths(TArray<FString>& OutFileCachePaths)
//{
//	FDoorShape::GetFileCachePaths(OutFileCachePaths);	
//	if (!GetEdgeBanding().Url.IsEmpty())
//	{
//		OutFileCachePaths.Emplace(
//			FPaths::ProjectDir() +
//			TEXT("Content/Material/Edge/") + TEXT("EdgeMaterial_") +
//			FString::Printf(TEXT("%d.pak"), GetEdgeBanding().ID));
//	}
//	if (!GetMaterial().Url.IsEmpty())
//	{
//		OutFileCachePaths.Emplace(
//			FPaths::ProjectDir() +
//			TEXT("Content/Material/Board/") + TEXT("BoardMaterial_") +
//			FString::Printf(TEXT("%d.pak"), GetMaterial().ID));
//	}	
//}
//
//void FDrawerShape::UpdateMaterial(UMaterialInterface * InMaterialInterface)
//{
//	for (const auto Ref : DividBlocks)
//	{
//		ASCTModelActor * ModelActor = StaticCast<ASCTModelActor *>(Ref->GetShapeActor());
//		if (!ModelActor) return;
//		ModelActor->ChangeMaterial(InMaterialInterface);		
//	}
//	ReCalDividBlocksPositionAndSize();
//}
//
//FAuxiliaryMetalInfo FDrawerShape::GetDefaultLockInfo() const
//{	
//	FAuxiliaryMetalInfo RetValue;
//	if (DefaultLockLocationInfo.ID != -1)
//	{
//		RetValue = DefaultLockLocationInfo;
//	}
//	else
//	{
//		for (const auto & Ref : GetMetals())
//		{
//			if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_LOCK)
//			{
//				RetValue.ID = Ref->GetShapeId();
//				RetValue.Name = Ref->GetShapeName();
//				break;
//			}
//		}
//	}
//	return RetValue;
//}
//
//FAuxiliaryMetalInfo FDrawerShape::GetDefaultHandleInfo() const
//{
//	FAuxiliaryMetalInfo RetValue;
//	if (DefaultHandleLocationInfo.ID != -1)
//	{
//		RetValue = DefaultHandleLocationInfo;
//	}
//	else
//	{
//		for (const auto & Ref : GetMetals())
//		{
//			if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_SHAKE_HANDS)
//			{
//				RetValue.ID = Ref->GetShapeId();
//				RetValue.Name = Ref->GetShapeName();
//				break;
//			}
//		}
//	}
//	return RetValue;
//}
//
//bool FDrawerShape::SetHandlePosX(const FString & InStrValue)
//{
//	bool bResult = true;
//	if (HandleAttriX->GetAttributeStr() != InStrValue)
//	{
//		bResult = HandleAttriX->SetAttributeValue(InStrValue);		
//		if (bResult)
//		{
//			DefaultHandleLocationInfo.Location.HposVal = FCString::Atof(*InStrValue);
//			RecalHandlePosition();
//		}
//	}
//	return bResult;
//}
//
//bool FDrawerShape::SetHandlePosX(float InIntValue)
//{	
//	return SetHandlePosX(FString::Printf(TEXT("%f"), InIntValue));
//}
//
//bool FDrawerShape::SetHandlePosZ(const FString & InStrValue)
//{
//	bool bResult = true;
//	if (HandleAttriZ->GetAttributeStr() != InStrValue)
//	{
//		bResult = HandleAttriZ->SetAttributeValue(InStrValue);		
//		if (bResult)
//		{
//			DefaultHandleLocationInfo.Location.VposVAl = FCString::Atof(*InStrValue);
//			RecalHandlePosition();
//		}
//	}
//	return bResult;
//}
//
//bool FDrawerShape::SetHandlePosZ(float InIntValue)
//{
//	return SetHandlePosZ(FString::Printf(TEXT("%f"), InIntValue));
//}
//
//bool FDrawerShape::SetLockPosX(const FString & InStrValue)
//{
//	bool bResult = true;
//	if (LockAttriX->GetAttributeStr() != InStrValue)
//	{
//		bResult = LockAttriX->SetAttributeValue(InStrValue);
//		if (bResult)
//		{						
//			RecalLockPosition();
//		}
//	}
//	return bResult;
//}
//
//bool FDrawerShape::SetLockPosX(float InIntValue)
//{
//	return SetLockPosX(FString::Printf(TEXT("%f"), InIntValue));
//}
//
//bool FDrawerShape::SetLockPosZ(const FString & InStrValue)
//{
//	bool bResult = true;
//	if (LockAttriZ->GetAttributeStr() != InStrValue)
//	{
//		bResult = LockAttriZ->SetAttributeValue(InStrValue);
//		if (bResult)
//		{
//			RecalLockPosition();
//		}
//	}
//	return bResult;
//}
//
//bool FDrawerShape::SetLockPosZ(float InIntValue)
//{
//	return SetLockPosZ(FString::Printf(TEXT("%f"), InIntValue));
//}
//
//void FDrawerShape::ReBuildHandleOrLock(const bool InIsHanle /*= true*/)
//{
//	TSharedPtr<FAccessoryShape> CurHandle;
//	EMetalsType DesType = EMetalsType::MT_SHAKE_HANDS;
//	if(!InIsHanle)
//	{
//		DesType = EMetalsType::MT_LOCK;
//	}
//	for (const auto & Ref : Metals)
//	{
//		if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == DesType)
//		{
//			CurHandle = Ref;
//			break;
//		}
//	}		
//	if (CurHandle.IsValid())
//	{
//		FString ShapeID;
//		FString ShapeName;
//		if (InIsHanle)
//		{
//			const TMap<FString, FString> & MapRef = HandleListSelectAtt->GetSelectedValues();
//			for (const auto & Ref : MapRef)
//			{
//				if (Ref.Value == HandleListSelectAtt->GetStringValue())
//				{
//					ShapeID = Ref.Key;
//					break;
//				}
//			}
//		}
//		else
//		{
//			const TMap<FString, FString> & MapRef = LockListSelectAtt->GetSelectedValues();
//			for (const auto & Ref : MapRef)
//			{
//				if (Ref.Value == LockListSelectAtt->GetStringValue())
//				{
//					ShapeID = Ref.Key;
//					ShapeName = Ref.Value;
//					break;
//				}
//			}
//		}
//		TSharedPtr<FAccessoryShape> AccessShape = StaticCastSharedPtr<FAccessoryShape>(*(GetOptionShapeMap().Find(FCString::Atoi(*ShapeID))));		
//		TSharedPtr<FAccessoryShape> NewShape = StaticCastSharedPtr<FAccessoryShape>(FSCTShapeManager::Get()->CopyShapeToNew(CurHandle));
//		NewShape->SetFileUrl(AccessShape->GetFileUrl());		
//		NewShape->SetFileMd5(AccessShape->GetFileMd5());
//		NewShape->SetModelID(AccessShape->GetModelID());
//		NewShape->SetFileName(AccessShape->GetFileName());
//		NewShape->SetShapeId(AccessShape->GetShapeId());
//		NewShape->SetShapeCategory(AccessShape->GetShapeCategory());
//		NewShape->LoadModelMesh(true);
//		AddChildShape(NewShape);
//		Metals.Add(NewShape);
//		FAuxiliaryMetalInfo CurHandleInfo = GetDefaultHandleInfo();
//		CurHandleInfo.ID = FCString::Atoi(*ShapeID);
//		CurHandleInfo.Name = ShapeName;
//		SetDefaultHandleInfo(CurHandleInfo);
//		NewShape->SpawnShapeActor();		
//		NewShape->GetShapeActor()->AttachToActor(ShapeActor, FAttachmentTransformRules::KeepRelativeTransform);
//		Metals.Remove(CurHandle);
//		RemoveChildShape(CurHandle);
//		CurHandle->GetShapeActor()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
//		CurHandle->GetShapeActor()->Destroy();
//	}
//}
//
//void FDrawerShape::ReCalDividBlocksPositionAndSize()
//{	
//#if 0
//	if (DividBlocks.Num() != 9) return;
//	const float DoorWidth = GetShapeWidth();
//	const float DoorHeight = GetShapeHeight();
//	auto ReCalZ = [&DoorHeight](TSharedPtr<FDecorationShape> & Shape)
//	{		
//		const float NewZ = DoorHeight - Shape->GetShapeHeight();		
//		Shape->SetShapePosZ(NewZ);
//	};
//	auto ReCalX = [&DoorWidth](TSharedPtr<FDecorationShape> & Shape)
//	{
//		const float NewX = DoorWidth - Shape->GetShapeWidth();		
//		Shape->SetShapePosX(NewX);
//	};
//
//	// 九宫格 左上，中上，右上会发生Z坐标的改变
//	{
//		ReCalZ(DividBlocks[0]);
//		ReCalZ(DividBlocks[1]);
//		ReCalZ(DividBlocks[2]);		
//	}
//	// 九宫格，右上，右中，右下会发生X坐标的改变
//	{
//		ReCalX(DividBlocks[2]);
//		ReCalX(DividBlocks[5]);
//		ReCalX(DividBlocks[8]);
//	}
//	auto SetActorMaterialOffsetAndRepeat = [](TSharedPtr<FDecorationShape> & InShape,const float & InOffsetU,const float & InOffsetV,
//		const float & InRepeatU,const float & InRepeatV)
//	{
//		const float OffsetScale = 100.0f;
//		ASCTModelActor * ModelActor = Cast<ASCTModelActor>(InShape->GetShapeActor());
//		if (ModelActor)
//		{
//			ModelActor->SetSingleMaterialOffsetAndRepeatParameter(InOffsetU * OffsetScale, InOffsetV * OffsetScale, InRepeatU, InRepeatV);			
//		}
//	};
//
//	// 重新计算纹理 
//	const float DefaultRepeatValue = 200.0f;
//	// 左上
//	{
//		TSharedPtr<FDecorationShape> ShapePtr = DividBlocks[0];
//		const float OffsetU = 0.0f;
//		const float OffsetV = 0.0f;
//		const float RepeatU = ShapePtr->GetShapeWidth() / DoorWidth;
//		const float RepeatV = ShapePtr->GetShapeHeight() / DoorHeight;
//		SetActorMaterialOffsetAndRepeat(ShapePtr, OffsetU, OffsetV, RepeatU, RepeatV);
//	}
//	// 中上
//	{
//		TSharedPtr<FDecorationShape> ShapePtr = DividBlocks[1];
//		const float OffsetU = DividBlocks[0]->GetShapeWidth() / DoorWidth;
//		const float OffsetV = 0.0f;
//		const float RepeatU = ShapePtr->GetShapeWidth() / DoorWidth;
//		const float RepeatV = ShapePtr->GetShapeHeight() / DoorHeight;
//		SetActorMaterialOffsetAndRepeat(ShapePtr, OffsetU, OffsetV, RepeatU, RepeatV);
//	}
//	// 右上
//	{
//		TSharedPtr<FDecorationShape> ShapePtr = DividBlocks[2];
//		const float OffsetU = 1.0f - ShapePtr->GetShapeWidth() / DoorWidth;
//		const float OffsetV = 0.0f;
//		const float RepeatU = ShapePtr->GetShapeWidth() / DoorWidth;
//		const float RepeatV = ShapePtr->GetShapeHeight() / DoorHeight;
//		SetActorMaterialOffsetAndRepeat(ShapePtr, OffsetU, OffsetV, RepeatU, RepeatV);
//	}
//	// 左中
//	{
//		TSharedPtr<FDecorationShape> ShapePtr = DividBlocks[3];
//		const float OffsetU = 0.0f;
//		const float OffsetV = DividBlocks[0]->GetShapeHeight() / DoorHeight;
//		const float RepeatU = ShapePtr->GetShapeWidth() / DoorWidth;
//		const float RepeatV = ShapePtr->GetShapeHeight() / DoorHeight;
//		SetActorMaterialOffsetAndRepeat(ShapePtr, OffsetU, OffsetV, RepeatU, RepeatV);
//	}
//	// 中中
//	{
//		TSharedPtr<FDecorationShape> ShapePtr = DividBlocks[4];
//		const float OffsetU = DividBlocks[3]->GetShapeWidth() / DoorWidth;
//		const float OffsetV = DividBlocks[1]->GetShapeHeight() / DoorHeight;
//		const float RepeatU = ShapePtr->GetShapeWidth() / DoorWidth;
//		const float RepeatV = ShapePtr->GetShapeHeight() / DoorHeight;
//		SetActorMaterialOffsetAndRepeat(ShapePtr, OffsetU, OffsetV, RepeatU, RepeatV);
//	}
//	// 右中
//	{
//		TSharedPtr<FDecorationShape> ShapePtr = DividBlocks[5];
//		const float OffsetU = 1.0f - ShapePtr->GetShapeWidth() / DoorWidth;
//		const float OffsetV = DividBlocks[2]->GetShapeHeight() / DoorHeight;
//		const float RepeatU = ShapePtr->GetShapeWidth() / DoorWidth;
//		const float RepeatV = ShapePtr->GetShapeHeight() / DoorHeight;
//		SetActorMaterialOffsetAndRepeat(ShapePtr, OffsetU, OffsetV, RepeatU, RepeatV);
//	}
//
//	// 左下
//	{
//		TSharedPtr<FDecorationShape> ShapePtr = DividBlocks[6];
//		const float OffsetU = 0.0f;
//		const float OffsetV = 1.0f - ShapePtr->GetShapeHeight() / DoorHeight;
//		const float RepeatU = ShapePtr->GetShapeWidth() / DoorWidth;
//		const float RepeatV = ShapePtr->GetShapeHeight() / DoorHeight;
//		SetActorMaterialOffsetAndRepeat(ShapePtr, OffsetU, OffsetV, RepeatU, RepeatV);
//	}
//	// 中下
//	{
//		TSharedPtr<FDecorationShape> ShapePtr = DividBlocks[7];
//		const float OffsetU = DividBlocks[0]->GetShapeWidth() / DoorWidth;
//		const float OffsetV = 1.0f - ShapePtr->GetShapeHeight() / DoorHeight;
//		const float RepeatU = ShapePtr->GetShapeWidth() / DoorWidth;
//		const float RepeatV = ShapePtr->GetShapeHeight() / DoorHeight;
//		SetActorMaterialOffsetAndRepeat(ShapePtr, OffsetU, OffsetV, RepeatU, RepeatV);
//	}
//	// 右下
//	{
//		TSharedPtr<FDecorationShape> ShapePtr = DividBlocks[8];
//		const float OffsetU = 1.0f - ShapePtr->GetShapeWidth() / DoorWidth;
//		const float OffsetV = 1.0f - ShapePtr->GetShapeHeight() / DoorHeight;
//		const float RepeatU = ShapePtr->GetShapeWidth() / DoorWidth;
//		const float RepeatV = ShapePtr->GetShapeHeight() / DoorHeight;
//		SetActorMaterialOffsetAndRepeat(ShapePtr, OffsetU, OffsetV, RepeatU, RepeatV);
//	}
//#else
//	const float DoorWidth = GetShapeWidth();
//	const float DoorHeight = GetShapeHeight();		
//	auto SetActorMaterialOffsetAndRepeat = [](TSharedPtr<FDecorationShape> & InShape, const float & InOffsetU, const float & InOffsetV,
//	const float & InRepeatU, const float & InRepeatV)
//	{
//		const float OffsetScale = 100.0f;
//		ASCTModelActor * ModelActor = Cast<ASCTModelActor>(InShape->GetShapeActor());
//		if (ModelActor)
//		{
//			ModelActor->SetSingleMaterialOffsetAndRepeatParameter(InOffsetU * OffsetScale, InOffsetV * OffsetScale, InRepeatU, InRepeatV);
//		}
//	};
//
//	// 重新计算纹理 
//	const float DefaultRepeatValue = 200.0f;
//	for (auto & Ref : DividBlocks)
//	{
//
//		const float OffsetU = Ref->GetShapePosX() / DoorWidth;
//		const float OffsetV = Ref->GetShapePosZ() / DoorHeight;
//		const float RepeatU = Ref->GetShapeWidth() / DoorWidth;
//		const float RepeatV = Ref->GetShapeHeight() / DoorHeight;
//		SetActorMaterialOffsetAndRepeat(Ref, OffsetU, OffsetV, RepeatU, RepeatV);
//	}
//#endif
//	RecalHandlePosition();
//	RecalLockPosition();
//}
//
//void FDrawerShape::RecalHandlePosition()
//{
//	if (DefaultHandleLocationInfo.ID == -1)
//	{
//		return;
//	}
//	// 计算水平方向
//	switch (DefaultHandleLocationInfo.Location.Hpos)
//	{
//	case 1:
//	{
//		for (const auto Ref : Metals)
//		{
//			if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_SHAKE_HANDS)
//			{
//				Ref->SetShapePosX(DefaultHandleLocationInfo.Location.HposVal);
//			}
//		}
//	}break;
//	case 2:
//	{
//		for (const auto Ref : Metals)
//		{
//			if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_SHAKE_HANDS)
//			{
//				Ref->SetShapePosX(GetShapeWidth() - DefaultHandleLocationInfo.Location.HposVal);
//			}
//		}
//
//	}break;
//	case 3:
//	{
//		for (const auto Ref : Metals)
//		{
//			if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_SHAKE_HANDS)
//			{
//				Ref->SetShapePosX(GetShapeWidth() / 2.0f);
//			}
//		}
//	}break;
//	default:
//		break;
//	}
//
//	// 计算垂直方向
//	switch (DefaultHandleLocationInfo.Location.Vpos)
//	{
//	case 1:
//	{
//		for (const auto Ref : Metals)
//		{
//			if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_SHAKE_HANDS)
//			{
//				Ref->SetShapePosZ(GetShapeHeight() - DefaultHandleLocationInfo.Location.VposVAl);
//			}
//		}
//	}break;
//	case 2:
//	{
//		for (const auto Ref : Metals)
//		{
//			if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_SHAKE_HANDS)
//			{
//				Ref->SetShapePosZ(DefaultHandleLocationInfo.Location.VposVAl);
//			}
//		}
//
//	}break;
//	case 3:
//	{
//		for (const auto Ref : Metals)
//		{
//			if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_SHAKE_HANDS)
//			{
//				Ref->SetShapePosZ(GetShapeHeight() / 2.0f);
//			}
//		}
//	}break;
//	default:
//		break;
//	}
//
//	auto RotateSpecialDegree = [this](const float InDegree)
//	{
//		for (const auto Ref : Metals)
//		{
//			if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_SHAKE_HANDS)
//			{
//				Ref->SetShapeRotY(InDegree);
//			}
//		}
//	};
//
//	// 旋转
//	switch (DefaultHandleLocationInfo.Location.RotateDirection)
//	{
//	case 0:
//		RotateSpecialDegree(0.0f);
//		break;				
//		// 左旋转90
//	case 1:
//		RotateSpecialDegree(90.0f);
//		break;
//	case 2:
//		RotateSpecialDegree(-90.0f);
//		break;
//	default:
//		break;
//	}
//
//	for (const auto Ref : Metals)
//	{
//		if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_SHAKE_HANDS)
//		{
//			Ref->UpdateAssociatedActors(2);
//		}
//	}
//}
//
//void FDrawerShape::RecalLockPosition()
//{
//	if (DefaultLockLocationInfo.ID == -1)
//	{
//		return;
//	}
//	// 计算水平方向
//	switch (DefaultLockLocationInfo.Location.Hpos)
//	{
//	case 1:
//	{
//		for (const auto Ref : Metals)
//		{
//			if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_LOCK)
//			{
//				Ref->SetShapePosX(DefaultLockLocationInfo.Location.HposVal);
//			}
//		}
//	}break;
//	case 2:
//	{
//		for (const auto Ref : Metals)
//		{
//			if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_LOCK)
//			{
//				Ref->SetShapePosX(GetShapeWidth() - DefaultLockLocationInfo.Location.HposVal);
//			}
//		}
//
//	}break;
//	case 3:
//	{
//		for (const auto Ref : Metals)
//		{
//			if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_LOCK)
//			{
//				Ref->SetShapePosX(GetShapeWidth() / 2.0f);
//			}
//		}
//	}break;
//	default:
//		break;
//	}
//
//	// 计算垂直方向
//	switch (DefaultLockLocationInfo.Location.Vpos)
//	{
//	case 1:
//	{
//		for (const auto Ref : Metals)
//		{
//			if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_LOCK)
//			{
//				Ref->SetShapePosZ(GetShapeHeight() - DefaultLockLocationInfo.Location.VposVAl);
//			}
//		}
//	}break;
//	case 2:
//	{
//		for (const auto Ref : Metals)
//		{
//			if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_LOCK)
//			{
//				Ref->SetShapePosZ(DefaultLockLocationInfo.Location.VposVAl);
//			}
//		}
//
//	}break;
//	case 3:
//	{
//		for (const auto Ref : Metals)
//		{
//			if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_LOCK)
//			{
//				Ref->SetShapePosZ(GetShapeHeight() / 2.0f);
//			}
//		}
//	}break;
//	default:
//		break;
//	}
//
//	auto RotateSpecialDegree = [this](const float InDegree)
//	{
//		for (const auto Ref : Metals)
//		{
//			if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_LOCK)
//			{
//				Ref->SetShapeRotY(InDegree);
//			}
//		}
//	};
//
//	// 旋转
//	switch (DefaultLockLocationInfo.Location.RotateDirection)
//	{
//	case 0:
//		RotateSpecialDegree(0.0f);
//		break;
//		// 左旋转90
//	case 1:
//		RotateSpecialDegree(90.0f);
//		break;
//	case 2:
//		RotateSpecialDegree(-90.0f);
//		break;
//	default:
//		break;
//	}
//
//	for (const auto Ref : Metals)
//	{
//		if (StaticCast<EMetalsType>(Ref->GetShapeCategory()) == EMetalsType::MT_LOCK)
//		{
//			Ref->UpdateAssociatedActors(2);
//		}
//	}
//}
//
//void FDrawerShape::ProcessOptionsMetalType()
//{
//	for (const auto & Ref : OpetionsMetalsShapesList)
//	{
//		if(Ref.Value->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SHAKE_HANDS))
//		{
//			FAuxiliaryMetalInfo TempMetalInfo;
//			TempMetalInfo.ID = Ref.Key;
//			TempMetalInfo.Name = Ref.Value->GetShapeName();
//			HandleList.Add(TempMetalInfo);
//		}
//		else if (Ref.Value->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_LOCK))
//		{
//			FAuxiliaryMetalInfo TempMetalInfo;
//			TempMetalInfo.ID = Ref.Key;
//			TempMetalInfo.Name = Ref.Value->GetShapeName();
//			LockList.Add(TempMetalInfo);
//		}
//	}
//}
//
//void FDrawerShape::AddBaseChildShape(const TSharedPtr<FSCTShape>& InShape)
//{	
//	ChildrenShapes.Add(InShape);
//	InShape->SetParentShape(this);
//	switch (InShape->GetShapeType())
//	{
//		case ST_Decoration:
//		{
//			TSharedPtr<FDecorationShape> CurrentBoard = StaticCastSharedPtr<FDecorationShape>(InShape);				
//			DividBlocks.Add(CurrentBoard);
//			break;
//		}
//		case ST_Accessory:
//		{
//			TSharedPtr<FAccessoryShape> CurrentAccessory = StaticCastSharedPtr<FAccessoryShape>(InShape);
//			//下载/加载Pak文件				
//			Metals.Add(CurrentAccessory);
//			break;
//		}
//		default:
//		check(false);
//		break;
//	}
//
//}
//
//SlidingDoor::~SlidingDoor()
//{
//	DestoryActor();
//}
//
//void SlidingDoor::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
//{
//	FDoorShape::ParseFromJson(InJsonObject);
//	SetFirstDoorPosition(InJsonObject->GetIntegerField(TEXT("firstDoorPosition")));
//	SetDoorInterleavintMode(InJsonObject->GetIntegerField(TEXT("doorInterleavingMode")));
//	SetOverlapWidth(InJsonObject->GetIntegerField(TEXT("overlapWidth")));
//	SetIntroskipValue(InJsonObject->GetIntegerField(TEXT("introskipValue")));
//	SetConverUpWay(InJsonObject->GetIntegerField(TEXT("coverUpWay")));
//	
//	const TArray<TSharedPtr<FJsonValue>> & TempChildren = InJsonObject->GetArrayField(TEXT("children"));
//	for (const auto & Ref : TempChildren)
//	{
//		const TSharedPtr<FJsonObject> & Obj = Ref->AsObject();
//		int32 ShapeId = Obj->GetIntegerField(TEXT("id"));
//		TSharedPtr<FSCTShape> NewShape = FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ST_Accessory, ShapeId);
//		TSharedPtr<FSCTShape> CopyShape = FSCTShapeManager::Get()->CopyShapeToNew(NewShape);
//		CopyShape->ParseAttributesFromJson(Obj);
//		SlideWays.Emplace(StaticCastSharedPtr<FAccessoryShape>(CopyShape));			
//	}	
//
//	const TArray<TSharedPtr<FJsonValue>> & DoorsRef = InJsonObject->GetArrayField(TEXT("doorGoops"));
//	for (const auto & Ref : DoorsRef)
//	{
//		const int32 Index = Doors.Add(MakeShareable(new FDoor));
//		Doors[Index]->ParseInfoFromJson(Ref->AsObject());
//	}
//	OriginDoorCount = DoorsRef.Num();
//
//	const TArray<TSharedPtr<FJsonValue>> & PlateSettingsRef = InJsonObject->GetArrayField(TEXT("plateSettings"));
//	for (const auto & Ref : PlateSettingsRef)
//	{
//		const int32 Index = PlateBoards.Add(MakeShareable(new FPlateBoard));
//		PlateBoards[Index]->ParseInfoFromJson(Ref->AsObject(),this);
//	}	
//	{
//		check(PlateBoards.Num() == 2);
//		PlateBoards[0]->IsTopBoard = true;
//		PlateBoards[1]->IsTopBoard = false;
//	}
//
//}
//
//void SlidingDoor::ParseAttributesFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
//{
//	FDoorShape::ParseAttributesFromJson(InJsonObject);
//	SetFirstDoorPosition(InJsonObject->GetIntegerField(TEXT("firstDoorPosition")));
//	SetDoorInterleavintMode(InJsonObject->GetIntegerField(TEXT("doorInterleavingMode")));
//	SetOverlapWidth(InJsonObject->GetIntegerField(TEXT("overlapWidth")));
//	SetIntroskipValue(InJsonObject->GetIntegerField(TEXT("introskipValue")));	
//	SetConverUpWay(InJsonObject->GetIntegerField(TEXT("coverUpWay")));
//}
//
//void SlidingDoor::ParseShapeFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
//{
//	FDoorShape::ParseShapeFromJson(InJsonObject);
//	SetFirstDoorPosition(InJsonObject->GetIntegerField(TEXT("firstDoorPosition")));
//	SetDoorInterleavintMode(InJsonObject->GetIntegerField(TEXT("doorInterleavingMode")));
//	SetOverlapWidth(InJsonObject->GetIntegerField(TEXT("overlapWidth")));
//	SetIntroskipValue(InJsonObject->GetIntegerField(TEXT("introskipValue")));
//	SetConverUpWay(InJsonObject->GetIntegerField(TEXT("coverUpWay")));
//
//	const TArray<TSharedPtr<FJsonValue>> & DoorsRef = InJsonObject->GetArrayField(TEXT("doorGoops"));
//	for (const auto & Ref : DoorsRef)
//	{
//		const int32 Index = Doors.Add(MakeShareable(new FDoor));
//		Doors[Index]->ParseInfoFromJson(Ref->AsObject());
//	}
//	OriginDoorCount = DoorsRef.Num();
//
//	const TArray<TSharedPtr<FJsonValue>> & PlateSettingsRef = InJsonObject->GetArrayField(TEXT("plateSettings"));
//	for (const auto & Ref : PlateSettingsRef)
//	{
//		const int32 Index = PlateBoards.Add(MakeShareable(new FPlateBoard));
//		PlateBoards[Index]->ParseInfoFromJson(Ref->AsObject(), this);
//	}
//	{
//		check(PlateBoards.Num() == 2);
//		PlateBoards[0]->IsTopBoard = true;
//		PlateBoards[1]->IsTopBoard = false;
//	}
//
//	const TArray<TSharedPtr<FJsonValue>> & TempChildren = InJsonObject->GetArrayField(TEXT("children"));
//	for (const auto & Ref : TempChildren)
//	{
//		const TSharedPtr<FJsonObject> & Obj = Ref->AsObject();
//		int32 ShapeId = Obj->GetIntegerField(TEXT("id"));
//		TSharedPtr<FSCTShape> NewShape = FSCTShapeManager::Get()->GetChildShapeByTypeAndID(ST_Accessory, ShapeId);
//		TSharedPtr<FSCTShape> CopyShape = FSCTShapeManager::Get()->CopyShapeToNew(NewShape);
//		CopyShape->ParseAttributesFromJson(Obj);
//		SlideWays.Emplace(StaticCastSharedPtr<FAccessoryShape>(CopyShape));
//	}
//}
//
//void SlidingDoor::ParseContentFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
//{
//	FDoorShape::ParseContentFromJson(InJsonObject);
//}
//
//void SlidingDoor::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
//{
//	FDoorShape::SaveToJson(JsonWriter);
//	JsonWriter->WriteValue(TEXT("firstDoorPosition"), GetFirstDoorPosition());
//	JsonWriter->WriteValue(TEXT("doorInterleavingMode"),GetDoorInterleavingMode());
//	JsonWriter->WriteValue(TEXT("overlapWidth"),GetOverlapWidth());
//	JsonWriter->WriteValue(TEXT("introskipValue"),GetIntroskipValue());	
//	JsonWriter->WriteValue(TEXT("coverUpWay"), GetConverUpWay());	
//
//	// 不保存经过拷贝的门
//	JsonWriter->WriteArrayStart(TEXT("doorGoops"));	
//	for(int32 Index = 0; Index < OriginDoorCount; ++Index)
//	{		
//		JsonWriter->WriteObjectStart();
//		Doors[Index]->SaveInfoToJson(JsonWriter);
//		JsonWriter->WriteObjectEnd();
//	}
//	JsonWriter->WriteArrayEnd();
//	JsonWriter->WriteArrayStart(TEXT("plateSettings"));
//	for (const auto & Ref : PlateBoards)
//	{
//		JsonWriter->WriteObjectStart();
//		Ref->SaveInfoToJson(JsonWriter);
//		JsonWriter->WriteObjectEnd();
//	}
//	JsonWriter->WriteArrayEnd();
//}
//
//void SlidingDoor::SaveAttriToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
//{
//	FDoorShape::SaveAttriToJson(JsonWriter);
//	JsonWriter->WriteValue(TEXT("firstDoorPosition"), GetFirstDoorPosition());
//	JsonWriter->WriteValue(TEXT("doorInterleavingMode"), GetDoorInterleavingMode());
//	JsonWriter->WriteValue(TEXT("overlapWidth"), GetOverlapWidth());
//	JsonWriter->WriteValue(TEXT("introskipValue"), GetIntroskipValue());
//	JsonWriter->WriteValue(TEXT("coverUpWay"), GetConverUpWay());
//}
//
//void SlidingDoor::SaveShapeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
//{
//	FDoorShape::SaveShapeToJson(JsonWriter);
//	JsonWriter->WriteValue(TEXT("firstDoorPosition"), GetFirstDoorPosition());
//	JsonWriter->WriteValue(TEXT("doorInterleavingMode"), GetDoorInterleavingMode());
//	JsonWriter->WriteValue(TEXT("overlapWidth"), GetOverlapWidth());
//	JsonWriter->WriteValue(TEXT("introskipValue"), GetIntroskipValue());
//	// 不保存经过拷贝的门
//	JsonWriter->WriteArrayStart(TEXT("doorGoops"));
//	for (int32 Index = 0; Index < OriginDoorCount; ++Index)
//	{
//		JsonWriter->WriteObjectStart();
//		Doors[Index]->SaveInfoToJson(JsonWriter);
//		JsonWriter->WriteObjectEnd();
//	}
//	JsonWriter->WriteArrayEnd();
//	JsonWriter->WriteArrayStart(TEXT("plateSettings"));
//	for (const auto & Ref : PlateBoards)
//	{
//		JsonWriter->WriteObjectStart();
//		Ref->SaveInfoToJson(JsonWriter);
//		JsonWriter->WriteObjectEnd();
//	}
//	JsonWriter->WriteArrayEnd();
//}
//
//void SlidingDoor::SaveContentToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
//{
//	FDoorShape::SaveContentToJson(JsonWriter);
//}
//	
//void SlidingDoor::CopyTo(FSCTShape * OutShape)
//{
//	FDoorShape::CopyTo(OutShape);
//	SlidingDoor * OutBaseShape = StaticCast<SlidingDoor*>(OutShape);
//	OutBaseShape->SetFirstDoorPosition(GetFirstDoorPosition());
//	OutBaseShape->SetDoorInterleavintMode(GetDoorInterleavingMode());
//	OutBaseShape->SetOverlapWidth(GetOverlapWidth());
//	OutBaseShape->SetIntroskipValue(GetIntroskipValue());	
//	OutBaseShape->OriginDoorCount = OriginDoorCount;		
//	for(const auto & Ref : Doors)
//	{
//		OutBaseShape->Doors.Add(Ref->CloneDoor());
//	}
//	for (const auto & Ref : PlateBoards)
//	{
//		OutBaseShape->PlateBoards.Add(Ref->ClonePlateBoard(OutBaseShape));
//	}
//	for (const auto & Ref : SlideWays)
//	{
//		OutBaseShape->SlideWays.Emplace(StaticCastSharedPtr<FAccessoryShape>(FSCTShapeManager::Get()->CopyShapeToNew(Ref)));
//	}
//}
//
//ASCTShapeActor * SlidingDoor::SpawnShapeActor()
//{
//	//创建基础组件型录
//	ASCTShapeActor* NewBaseActor = FDoorShape::SpawnShapeActor();
//	//创建组件子型录
//	for (const auto Ref : Doors)
//	{	
//		Ref->SpawnActor(NewBaseActor);		
//	}	
//	for (auto & Ref : PlateBoards)
//	{
//		Ref->SpawnActor(NewBaseActor);
//	}
//	// 放置滑轨
//	for (auto & Ref : SlideWays)
//	{
//		Ref->LoadModelMesh();
//		AActor * TempActor = Ref->SpawnShapeActor();
//		TempActor->AttachToActor(NewBaseActor, FAttachmentTransformRules::KeepRelativeTransform);
//	}
//	ReCalDividBlocksPositionAndSize();
//	return NewBaseActor;
//}
//
//void SlidingDoor::SetCollisionProfileName(FName InProfileName)
//{
//	if (!ShapeActor) return;	
//	TArray<AActor*> AttachedActors;
//	ShapeActor->GetAttachedActors(AttachedActors);
//	for (auto& Actor : AttachedActors)
//	{
//		int32 NumChilds = Actor->GetRootComponent()->GetNumChildrenComponents();
//		for (int32 i = 0; i < NumChilds; ++i)
//		{
//			UMeshComponent *ChildMeshComp = Cast<UMeshComponent>(Actor->GetRootComponent()->GetChildComponent(i));
//			if (ChildMeshComp != nullptr)
//			{
//				ChildMeshComp->SetCollisionProfileName(InProfileName);
//			}
//		}
//	}	
//}
//
//void SlidingDoor::GetResourceUrls(TArray<FString>& OutResourceUrls)
//{
//	FDoorShape::GetResourceUrls(OutResourceUrls);	
//	for (const auto & Ref : Doors)
//	{
//		Ref->GetResourceUrls(OutResourceUrls);
//	}
//	for (const auto & Ref : PlateBoards)
//	{
//		Ref->GetResourceUrls(OutResourceUrls);
//	}
//}
//
//void SlidingDoor::GetFileCachePaths(TArray<FString>& OutFileCachePaths)
//{
//	FDoorShape::GetFileCachePaths(OutFileCachePaths);
//	for (const auto & Ref : Doors)
//	{
//		Ref->GetFileCachePaths(OutFileCachePaths);
//	}
//	for (const auto & Ref : PlateBoards)
//	{
//		Ref->GetFileCachePaths(OutFileCachePaths);
//	}
//}
//
//void SlidingDoor::ReCalDividBlocksPositionAndSize()
//{	
//	// 先清空复制的门
//	if (Doors.Num() > OriginDoorCount)
//	{
//		Doors.RemoveAt(OriginDoorCount, Doors.Num() - OriginDoorCount);
//	}
//	// 重置门板切块信息
//	for (auto & Ref : Doors)
//	{
//		Ref->ResetDividPosition();
//	}
//
//	// 根据移门尺寸，计算需要多少块来合成
//	const float SlidingDoorWidht =  GetShapeWidth();
//	const float SlidingDoorHeight = GetShapeHeight();
//	// 检验是否可组合
//	struct SizeReange
//	{
//		float MinWitht;
//		float MaxWidth;
//		float MinHeight;
//		float MaxHeight;
//	};
//
//	// 先收集所有移门的可变范围
//	TArray<SizeReange> AllDoorRanges;
//	for (const auto & Ref : Doors)
//	{
//		SizeReange Size;
//		Size.MinWitht = Ref->WidhtRange.Min;
//		Size.MaxWidth = Ref->WidhtRange.Max;
//		Size.MinHeight = Ref->HeightRange.Min;
//		Size.MaxHeight = Ref->HeightRange.Max;
//		AllDoorRanges.Add(MoveTemp(Size));
//	}	
//	// 由于滑轨以及上下垫板引起的门板高度的减少量
//	float ZCullSize = 0.0f;
//	// 由于下滑轨以及下垫板引起的门Z方向整体向上的偏移量 
//	float ZIncraseValue = 0.0f;	
//	float YIncreaseValue = 0.0f;
//	// 滑轨与上下垫板 
//	{
//		// 由于上垫板的存在引起的上滑轨Z方向的下移量
//		float ZDecreaseValue = 0.0f;
//		float TopYIncreaseValue = 0.0f;
//		float BottomYIncreaseValue = 0.0f;
//		// 上下垫板
//		{
//			for (auto & Ref : PlateBoards)
//			{
//				Ref->ReCalPlateBoardPositionAndSize(SlidingDoorWidht, SlidingDoorHeight, ZCullSize, 
//					ZIncraseValue, ZDecreaseValue, YIncreaseValue, TopYIncreaseValue, BottomYIncreaseValue);
//			}
//		}
//
//		//  上下滑轨
//		{
//			FVector TopSlidWaySize = FVector::ZeroVector, BottomSlidWaySize = FVector::ZeroVector;
//			bool SlidWayIsOk = SlideWays.Num() == 2;			
//			// 如果存在上下滑轨
//			if (SlidWayIsOk)
//			{
//				TSharedPtr<FAccessoryShape> TopSlidWay, BottomSlidWay;
//				for (const auto & Ref : SlideWays)
//				{
//					if (Ref->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SD_TOP_RAILS))
//					{
//						TopSlidWay = Ref;
//					}
//					else if (Ref->GetShapeCategory() == StaticCast<int32>(EMetalsType::MT_SD_BOTTOM_RAILS))
//					{
//						BottomSlidWay = Ref;
//					}
//				}
//				check(TopSlidWay.IsValid() && BottomSlidWay.IsValid());
//				TopSlidWaySize = StaticCast<ASCTModelActor*>(TopSlidWay->GetShapeActor())->GetModelMeshSize();
//				BottomSlidWaySize = StaticCast<ASCTModelActor*>(BottomSlidWay->GetShapeActor())->GetModelMeshSize();
//				// 上滑轨
//				{		
//					float TempValue = TopSlidWay->GetShapeHeight();
//					TopSlidWay->SetShapePosX(0.0f);
//					TopSlidWay->SetShapePosY(TopYIncreaseValue - TopSlidWay->GetShapeDepth() / 2.0f);
//					TopSlidWay->SetShapePosZ(SlidingDoorHeight - ZDecreaseValue - TempValue);
//					TopSlidWay->SetShapeWidth(FString::Printf(TEXT("%f"), SlidingDoorWidht));
//					//TopSlidWay->SetShapeDepth(FString::Printf(TEXT("%f"), TopSlidWaySize.Y));					
//					ZCullSize += TempValue;					
//				}
//				// 下滑轨
//				{
//					BottomSlidWay->SetShapePosX(0.0f);
//					BottomSlidWay->SetShapePosY(BottomYIncreaseValue - BottomSlidWay->GetShapeDepth() / 2.0f);
//					BottomSlidWay->SetShapePosZ(ZIncraseValue);
//					BottomSlidWay->SetShapeWidth(FString::Printf(TEXT("%f"), SlidingDoorWidht));
//					//BottomSlidWay->SetShapeDepth(FString::Printf(TEXT("%f"), BottomSlidWaySize.Y));
//					ZCullSize += BottomSlidWay->GetShapeHeight();
//					ZIncraseValue += BottomSlidWay->GetShapeHeight();					
//				}
//			}
//		}		
//	}
//	float DoorObtainDepth = 0.0f;
//	bool IsInner = FirstDoorPosition == 1;
//	switch (AllDoorRanges.Num())
//	{		
//		case 1:
//		{
//			const int32 WCountMax = FMath::FloorToInt(SlidingDoorWidht / AllDoorRanges[0].MinWitht);
//			const int32 WCountMin = FMath::CeilToInt(SlidingDoorWidht / AllDoorRanges[0].MaxWidth);
//			check((WCountMax >= WCountMin ) && (SlidingDoorHeight >= AllDoorRanges[0].MinHeight)  && (SlidingDoorHeight <= AllDoorRanges[0].MaxHeight));
//			int32 DesCount = WCountMin;			
//			float DesDoorWidth = SlidingDoorWidht / DesCount;	
//			float DesDoorHeight = SlidingDoorHeight - ZCullSize;
//			int TempDoorInterleavingMode = DoorInterleavingMode;
//			if (DesCount < 4)
//			{
//				TempDoorInterleavingMode = 1;
//			}
//			// 内外交错
//			if (TempDoorInterleavingMode == 1 && DesCount >= 1)
//			{
//				DesDoorWidth += (DesCount - 1) * OverlapWidth  / DesCount;
//			}
//			// 对称交错
//			else if(TempDoorInterleavingMode == 2)
//			{
//				DesDoorWidth += (DesCount - 1 - 1) * OverlapWidth / DesCount;
//			}			
//			Doors[0]->WidhtRange.Current = DesDoorWidth;
//			Doors[0]->HeightRange.Current = DesDoorHeight;						
//			if (IsInner)
//			{
//				Doors[0]->RootBlock->PosY = YIncreaseValue / 10.0f - Doors[0]->GetTrueDepth();
//			}
//			else
//			{
//				Doors[0]->RootBlock->PosY = YIncreaseValue / 10.0f;
//			}
//			Doors[0]->RootBlock->PosZ = ZIncraseValue / 10.0f;
//			float DoorIncreaseX = Doors[0]->WidhtRange.Current / 10.0f;
//			// 进行门的复制
//			for (int32 Index = 1; Index < DesCount; ++Index)
//			{				
//				int32 NewIndex = Doors.Add(Doors[0]->CloneDoor());		
//				float TempOverlapWidth = OverlapWidth / 10.0f;
//				// 内外交错
//				if (TempDoorInterleavingMode == 1)
//				{
//					IsInner = !IsInner;
//				}
//				// 对称交错
//				else if(TempDoorInterleavingMode == 2)
//				{
//					if (Index == FMath::FloorToInt(DesCount / 2.0f))
//					{
//						IsInner = !(FirstDoorPosition == 1);
//						TempOverlapWidth = 0.0f;
//					}
//					else
//					{
//						IsInner = !IsInner;
//					}					
//				}
//				Doors[NewIndex]->IsInner = IsInner;
//				Doors[NewIndex]->RootBlock->PosX += DoorIncreaseX - TempOverlapWidth;												
//				if (IsInner)
//				{
//					Doors[NewIndex]->RootBlock->PosY = YIncreaseValue / 10.0f - Doors[NewIndex]->GetTrueDepth();
//				}
//				else
//				{
//					Doors[NewIndex]->RootBlock->PosY = YIncreaseValue / 10.0f;
//				}
//				if (Doors[NewIndex]->GetTrueDepth() > DoorObtainDepth)
//				{
//					DoorObtainDepth = Doors[NewIndex]->GetTrueDepth();
//				}				
//				DoorIncreaseX += Doors[NewIndex]->WidhtRange.Current / 10.0f - TempOverlapWidth;
//				check(GetShapeActor())
//				{
//					Doors[NewIndex]->SpawnActor(GetShapeActor());
//				}
//			}
//
//		}break;
//		case 2:
//		case 3:
//		case 4:
//		case 5:
//		case 6:
//		case 7:
//		{
//			int32 DesCount = AllDoorRanges.Num();
//			float DesDoorWidth = SlidingDoorWidht / DesCount;
//			float DesDoorHeight = SlidingDoorHeight - ZCullSize;
//			int TempDoorInterleavingMode = DoorInterleavingMode;
//			if (DesCount < 4)
//			{
//				TempDoorInterleavingMode = 1;
//			}
//			// 内外交错
//			if (TempDoorInterleavingMode == 1 && DesCount >= 1)
//			{
//				DesDoorWidth += (DesCount - 1) * OverlapWidth / DesCount;
//			}
//			// 对称交错
//			else if (TempDoorInterleavingMode == 2)
//			{
//				DesDoorWidth += (DesCount - 1 - 1) * OverlapWidth / DesCount;
//			}
//			Doors[0]->WidhtRange.Current = DesDoorWidth;
//			Doors[0]->HeightRange.Current = DesDoorHeight;	
//			Doors[0]->RootBlock->PosZ = ZIncraseValue / 10.0f;
//			if (IsInner)
//			{
//				Doors[0]->RootBlock->PosY = YIncreaseValue / 10.0f - Doors[0]->GetTrueDepth();
//			}
//			else
//			{
//				Doors[0]->RootBlock->PosY = YIncreaseValue / 10.0f;
//			}
//			float DoorIncreaseX = Doors[0]->WidhtRange.Current / 10.0f;		
//			for (int32 Index = 1; Index < DesCount; ++Index)
//			{				
//				Doors[Index]->WidhtRange.Current = DesDoorWidth;
//				Doors[Index]->HeightRange.Current = DesDoorHeight;
//				float TempOverlapWidth = OverlapWidth / 10.0f;
//				// 内外交错
//				if (TempDoorInterleavingMode == 1)
//				{
//					IsInner = !IsInner;
//				}
//				// 对称交错
//				else if (TempDoorInterleavingMode == 2)
//				{
//					if (Index == FMath::FloorToInt(DesCount / 2.0f))
//					{
//						IsInner = !(FirstDoorPosition == 1);
//						TempOverlapWidth = 0.0f;
//					}
//					else
//					{
//						IsInner = !IsInner;
//					}
//				}
//				Doors[Index]->IsInner = IsInner;
//				Doors[Index]->RootBlock->PosX += DoorIncreaseX - TempOverlapWidth;
//				Doors[Index]->RootBlock->PosZ = ZIncraseValue / 10.0f;
//				if (IsInner)
//				{
//					Doors[Index]->RootBlock->PosY = YIncreaseValue / 10.0f - Doors[Index]->GetTrueDepth();
//				}	
//				else
//				{
//					Doors[Index]->RootBlock->PosY = YIncreaseValue / 10.0f;
//				}
//				if (Doors[Index]->GetTrueDepth() > DoorObtainDepth)
//				{
//					DoorObtainDepth = Doors[Index]->GetTrueDepth();
//				}
//				DoorIncreaseX += Doors[Index]->WidhtRange.Current / 10.0f - TempOverlapWidth;			
//			}
//		}break;
//		default:
//			break;
//	}
//	for (auto & Ref : Doors)
//	{
//		Ref->RecalSizeAndPosition();		
//	}		
//}
//
//bool SlidingDoor::HasBottomPlateBoard() const
//{	
//	return const_cast<SlidingDoor*>(this)->GetBottomPlateBoard().IsValid();
//}
//
//bool SlidingDoor::HasTopPlateBoard() const
//{
//	return const_cast<SlidingDoor*>(this)->GetTopPlateBoard().IsValid();
//}
//
//void SlidingDoor::GetTopPlateBoardShowInfo(FString & OutSubstrateName, FString & OutMaterialName, FString & OutEdgeBaingingName, float & OutW, float & OutDepth, TArray<float>& HegithList, float & OutCurHeight)
//{
//	TSharedPtr<FPlateBoard> SPtr = GetTopPlateBoard();
//	if (SPtr.IsValid())
//	{
//		OutSubstrateName = SPtr->Substrate.Name;
//		OutMaterialName = SPtr->Material.Name;
//		OutEdgeBaingingName = SPtr->EdgeBanding.Name;
//		OutW = SPtr->BoardShape->GetShapeWidth();
//		OutDepth = SPtr->BoardShape->GetShapeDepth();
//		OutCurHeight = SPtr->BoardShape->GetShapeHeight();
//	}	
//}
//
//void SlidingDoor::GetBottomPlateBoardShowInfo(FString & OutSubstrateName, FString & OutMaterialName, FString & OutEdgeBaingingName, float & OutW, float & OutDepth, TArray<float>& HegithList, float & OutCurHeight)
//{
//	TSharedPtr<FPlateBoard> SPtr = GetBottomPlateBoard();
//	if (SPtr.IsValid())
//	{	
//		OutSubstrateName = SPtr->Substrate.Name;
//		OutMaterialName = SPtr->Material.Name;
//		OutEdgeBaingingName = SPtr->EdgeBanding.Name;
//		OutW = SPtr->BoardShape->GetShapeWidth();
//		OutDepth = SPtr->BoardShape->GetShapeDepth();
//		OutCurHeight = SPtr->BoardShape->GetShapeHeight();
//	}
//}
//
//float SlidingDoor::GetTopPlateBoardHeight() const
//{
//	float RetValue = 0.0f;
//	TSharedPtr<FPlateBoard> SPtr = const_cast<SlidingDoor*>(this)->GetTopPlateBoard();
//	if (SPtr.IsValid())
//	{		
//		RetValue = SPtr->BoardShape->GetShapeHeight();
//	}
//	return 	RetValue;
//}
//
//float SlidingDoor::GetBottomPlateBoardHeight() const
//{
//	float RetValue = 0.0f;
//	TSharedPtr<FPlateBoard> SPtr = const_cast<SlidingDoor*>(this)->GetBottomPlateBoard();
//	if (SPtr.IsValid())
//	{
//		RetValue = SPtr->BoardShape->GetShapeHeight();
//	}
//	return 	RetValue;
//}
//
//void SlidingDoor::SetTopPlateBoardVisiable(const bool bInShow)
//{
//	TSharedPtr<FPlateBoard> SPtr = GetTopPlateBoard();
//	if (SPtr.IsValid())
//	{
//		if (SPtr->GetVisiable() != bInShow)
//		{
//			SPtr->SetVisiable(bInShow);
//			ReCalDividBlocksPositionAndSize();
//		}
//	}
//}
//
//void SlidingDoor::SetBottomPlateBoardVisiable(const bool bInShow)
//{
//	TSharedPtr<FPlateBoard> SPtr = GetBottomPlateBoard();
//	if (SPtr->GetVisiable() != bInShow)
//	{
//		SPtr->SetVisiable(bInShow);
//		ReCalDividBlocksPositionAndSize();
//	}
//}
//
//void SlidingDoor::DestoryActor()
//{
//	for (auto & Ref : Doors)
//	{
//		Ref->DestoryActor();
//	}
//}
//
//TSharedPtr<SlidingDoor::FPlateBoard> SlidingDoor::GetTopPlateBoard()
//{
//	TSharedPtr<FPlateBoard> RetPtr;
//	for (auto & Ref : PlateBoards)
//	{
//		if (Ref->IsTopBoard)
//		{
//			RetPtr = Ref;
//		}
//	}
//	return RetPtr;
//}
//
//TSharedPtr<SlidingDoor::FPlateBoard> SlidingDoor::GetBottomPlateBoard()
//{
//	TSharedPtr<FPlateBoard> RetPtr;
//	for (auto & Ref : PlateBoards)
//	{
//		if (Ref->IsTopBoard == false)
//		{
//			RetPtr = Ref;
//		}
//	}
//	return RetPtr;
//}
//
//void SlidingDoor::FDoor::SpawnActor(AActor * InParentActor)
//{
//	// 计算所有型材的尺寸
//	for (auto & Ref : SectionalMaterials)
//	{						
//		FSCTShapeManager::Get()->GetStaticMeshManager()->UpLoadStaticMesh(Ref.Value->PakUrl, Ref.Value->CacheFilePath,Ref.Value->OptimizeParm);
//		const FMeshDataEntry * MeshData = FSCTShapeManager::Get()->GetStaticMeshManager()->FindMesh(Ref.Value->CacheFilePath);
//		check(MeshData);
//		Ref.Value->Width = MeshData->MeshData.Mesh->GetBoundingBox().GetSize().X;
//		Ref.Value->Height = MeshData->MeshData.Mesh->GetBoundingBox().GetSize().Z;
//		Ref.Value->Depth = MeshData->MeshData.Mesh->GetBoundingBox().GetSize().Y;
//	}
//
//	auto SpawnStaticMeshActor = [InParentActor](const TSharedPtr<FSectionalMaterial> & InOuterSectionMaterial, FDividBlock & InBlock)
//	{			
//		UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();
//		ASCTModelActor * NewModelActor = World->SpawnActor<ASCTModelActor>(
//			ASCTModelActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
//		InBlock.StaticMeshActors.Add(NewModelActor);
//		check(InOuterSectionMaterial->CacheFilePath.IsEmpty() == false);
//		const FMeshDataEntry * MeshData = FSCTShapeManager::Get()->GetStaticMeshManager()->FindMesh(InOuterSectionMaterial->CacheFilePath);
//		check(MeshData && MeshData->Type == 0);		
//		NewModelActor->SetStaticMesh(MeshData->MeshData.Mesh, MeshData->MaterialList);
//		NewModelActor->AttachToActor(InParentActor, FAttachmentTransformRules::KeepRelativeTransform);				
//	};
//
//	// 使用边框搭建外轮廓
//	// 左竖
//	const TSharedPtr<FSectionalMaterial> LeftV = GetLeftV();
//	SpawnStaticMeshActor(LeftV, *RootBlock);
//	// 右竖
//	const TSharedPtr<FSectionalMaterial> RightV = GetRightV();
//	SpawnStaticMeshActor(RightV, *RootBlock);	
//	// 上横
//	const TSharedPtr<FSectionalMaterial> TopH = GetTopH();
//	SpawnStaticMeshActor(TopH, *RootBlock);
//	// 下横
//	const TSharedPtr<FSectionalMaterial> BottomH = GetBottomH();
//	SpawnStaticMeshActor(BottomH, *RootBlock);	
//	// 动态生成子块
//	RootBlock->SpawnActor(InParentActor, *this);	
//}
//
//void SlidingDoor::FDoor::ParseInfoFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
//{
//	Id = InJsonObject->GetIntegerField(TEXT("id"));
//	Name = InJsonObject->GetStringField(TEXT("name"));
//	Code = InJsonObject->GetStringField(TEXT("code"));
//	Type = InJsonObject->GetIntegerField(TEXT("type"));
//	
//
//	auto ParseRangeSpace = [&InJsonObject](const FString & InName, FSpaceRange & OuterSpace)
//	{
//		const TSharedPtr<FJsonObject> & Ref = InJsonObject->GetObjectField(InName);
//		const TSharedPtr<FJsonObject> & ValueRef = Ref->GetObjectField(TEXT("value"));
//		OuterSpace.Max = StaticCast<float>(ValueRef->GetNumberField(TEXT("max")));
//		OuterSpace.Min = StaticCast<float>(ValueRef->GetNumberField(TEXT("min")));
//		OuterSpace.Current = FCString::Atof(*(Ref->GetStringField(TEXT("current"))));
//	};
//	ParseRangeSpace(TEXT("width"), WidhtRange);
//	ParseRangeSpace(TEXT("depth"), DepthRange);
//	ParseRangeSpace(TEXT("height"), HeightRange);
//	const TArray<TSharedPtr<FJsonValue>> & SectionMaterialRef = InJsonObject->GetArrayField(TEXT("sectionalMaterials"));
//	for (const auto & Ref : SectionMaterialRef)
//	{
//		TSharedPtr<FSectionalMaterial> SectionalMaterial = MakeShareable(new FSectionalMaterial);
//		SectionalMaterial->Id =  Ref->AsObject()->GetIntegerField(TEXT("id"));
//		SectionalMaterial->Type = Ref->AsObject()->GetIntegerField(TEXT("type"));
//		SectionalMaterial->DisplayMode = Ref->AsObject()->GetIntegerField(TEXT("displayMode"));
//		SectionalMaterial->PakUrl =  Ref->AsObject()->GetStringField(TEXT("pakUrl"));
//		SectionalMaterial->Md5 = Ref->AsObject()->GetStringField(TEXT("pakMd5"));
//		SectionalMaterial->OptimizeParm = Ref->AsObject()->GetStringField(TEXT("optimizeParam"));
//		SectionalMaterial->IsShutter = (SectionalMaterial->Type == StaticCast<int32>(ESectionalMaterialType::Louver_Core));
//		SectionalMaterials.Add(TPair<int32,int32>(SectionalMaterial->Id, SectionalMaterial->Type), SectionalMaterial);		
//		TSharedPtr<FMeshManager> StaticMeshMrg = FSCTShapeManager::Get()->GetStaticMeshManager();
//		const FString CachePath = GetCachePath(SectionalMaterial->Id, SectionalMaterial->Md5);
//		SectionalMaterial->CacheFilePath = CachePath;
//	}	
//	RootBlock = MakeShareable(new FDividBlock);
//	RootBlock->ParseInfoFromJson(InJsonObject->GetObjectField(TEXT("divideBlocks")));
//}
//
//void SlidingDoor::FDoor::SaveInfoToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
//{
//	JsonWriter->WriteValue(TEXT("id"),Id);
//	JsonWriter->WriteValue(TEXT("name"),Name);
//	JsonWriter->WriteValue(TEXT("code"),Code);
//	JsonWriter->WriteValue(TEXT("type"),Type);
//
//	auto SaveRangeSpace = [&JsonWriter](const FString & InName,const FSpaceRange & InSpace,const FString & InRefName,const int32 InId = 0,const int32 InType = 3)
//	{
//		JsonWriter->WriteObjectStart(InName);
//		JsonWriter->WriteValue(TEXT("id"), InId);
//		JsonWriter->WriteValue(TEXT("type"), InType);
//		JsonWriter->WriteValue(TEXT("refName"), InRefName);
//		JsonWriter->WriteObjectStart(TEXT("value"));
//		JsonWriter->WriteValue("min", InSpace.Min);
//		JsonWriter->WriteValue("max", InSpace.Max);
//		JsonWriter->WriteObjectEnd();
//		JsonWriter->WriteValue(TEXT("current"), FString::Printf(TEXT("%f"), InSpace.Current));
//		JsonWriter->WriteObjectEnd();
//	};
//	SaveRangeSpace(TEXT("width"), WidhtRange,TEXT("WIDTH"));
//	SaveRangeSpace(TEXT("depth"), DepthRange,TEXT("DEPTH"));
//	SaveRangeSpace(TEXT("height"), HeightRange,TEXT("HEIGHT"));
//
//	JsonWriter->WriteArrayStart(TEXT("sectionalMaterials"));	
//	for (const auto & Ref : SectionalMaterials)
//	{
//		JsonWriter->WriteObjectStart();		
//		JsonWriter->WriteValue(TEXT("id"), Ref.Value->Id);		
//		JsonWriter->WriteValue(TEXT("type"),Ref.Value->Type);		
//		JsonWriter->WriteValue(TEXT("displayMode"), Ref.Value->DisplayMode);		
//		JsonWriter->WriteValue(TEXT("pakUrl"), Ref.Value->PakUrl);		
//		JsonWriter->WriteValue(TEXT("pakMd5"), Ref.Value->Md5);
//		JsonWriter->WriteValue(TEXT("optimizeParam"), Ref.Value->OptimizeParm);
//		JsonWriter->WriteObjectEnd();			
//	}
//	JsonWriter->WriteArrayEnd();
//
//	JsonWriter->WriteObjectStart(TEXT("divideBlocks"));
//	RootBlock->SaveInfoToJson(JsonWriter);
//	JsonWriter->WriteObjectEnd();
//}
//
//void SlidingDoor::FDoor::GetResourceUrls(TArray<FString>& OutResourceUrls)
//{
//	for (const auto & Ref : SectionalMaterials)
//	{		
//		OutResourceUrls.Add(Ref.Value->PakUrl);		
//	}
//}
//
//void SlidingDoor::FDoor::GetFileCachePaths(TArray<FString>& OutFileCachePaths)
//{
//	for (const auto & Ref : SectionalMaterials)
//	{		
//		OutFileCachePaths.Add(Ref.Value->CacheFilePath);
//	}	
//}
//
//void SlidingDoor::FDoor::DestoryActor()
//{
//	RootBlock->DestoryActor();
//}
//
//TSharedPtr<SlidingDoor::FDoor> SlidingDoor::FDoor::CloneDoor() const
//{
//	TSharedPtr<SlidingDoor::FDoor> NewDoor = MakeShareable(new SlidingDoor::FDoor);
//	NewDoor->Id = this->Id;
//	NewDoor->Name = this->Name;
//	NewDoor->Code = this->Code;
//	NewDoor->Type = this->Type;
//	NewDoor->WidhtRange = this->WidhtRange;
//	NewDoor->HeightRange = this->HeightRange;
//	NewDoor->DepthRange = this->DepthRange;
//	NewDoor->IsInner = this->IsInner;	
//	NewDoor->SectionalMaterials = this->SectionalMaterials;	
//	NewDoor->RootBlock = this->RootBlock->CloneDividBlock();
//	return NewDoor;
//}
//
//float SlidingDoor::FDoor::GetTrueDepth() const
//{
//	static float TrueDepth = 0.0f;
//	if (FMath::IsNearlyZero(TrueDepth))
//	{
//		for (const auto & Ref : SectionalMaterials)
//		{
//			if (Ref.Value->Depth < TrueDepth) continue;			
//			TrueDepth = Ref.Value->Depth;			
//		}
//	}
//	return TrueDepth;
//}
//
//const TSharedPtr<SlidingDoor::FDoor::FSectionalMaterial>& SlidingDoor::FDoor::GetCore(const int32 Index) const
//{
//	for (const auto & Ref : SectionalMaterials)
//	{
//		if (Ref.Key.Key == Index)
//		{
//			return Ref.Value;
//		}
//	}
//	check(false);
//	return SectionalMaterials[TPair<int32, int32>(0, 0)];
//}
//
//void SlidingDoor::FDoor::RecalSizeAndPosition()
//{	
//
//	const FVector Offset = { RootBlock->PosX,RootBlock->PosY,RootBlock->PosZ };
//	// 外包围框
//	check(RootBlock->StaticMeshActors.Num() > 4);
//	// 左竖
//	const TSharedPtr<FSectionalMaterial> & LeftV = GetLeftV();
//	RootBlock->StaticMeshActors[0]->SetActorRelativeScale3D(FVector(1.0f, 1.0f, HeightRange.Current / 10.0f / LeftV->Height ));
//	RootBlock->StaticMeshActors[0]->SetActorRelativeLocation(FVector::ZeroVector + Offset);
//	// 右竖
//	const TSharedPtr<FSectionalMaterial> & RightV = GetRightV();	
//#if 0
//	RootBlock->StaticMeshActors[1]->SetActorRelativeScale3D(FVector(0.1f, 0.1f, HeightRange.Current / RightV->Height / 10.0f));
//	RootBlock->StaticMeshActors[1]->SetActorRelativeLocation(FVector((WidhtRange.Current - RightV->GetCareSpace()) / 10.0f,0.0f,0.0f));
//#else
//	RootBlock->StaticMeshActors[1]->SetActorRelativeScale3D(FVector(-1.0f, 1.0f, HeightRange.Current / 10.0f / RightV->Height ));
//	RootBlock->StaticMeshActors[1]->SetActorRelativeLocation(FVector(WidhtRange.Current / 10.0f, 0.0f, 0.0f) + Offset);
//#endif
//	// 上横
//	const TSharedPtr<FSectionalMaterial> & TopH = GetTopH();
//	RootBlock->StaticMeshActors[2]->SetActorRelativeScale3D(FVector(((WidhtRange.Current / 10.0f) - LeftV->GetCareSpace() - RightV->GetCareSpace() ) / TopH->Width , 1.0f, 1.0f));
//	RootBlock->StaticMeshActors[2]->SetActorRelativeLocation(FVector(LeftV->GetCareSpace(), 0.0f, (HeightRange.Current / 10.0f - TopH->Height) ) + Offset);
//	// 下横
//	const TSharedPtr<FSectionalMaterial> BottomH = GetBottomH();
//	RootBlock->StaticMeshActors[3]->SetActorRelativeScale3D(FVector(( (WidhtRange.Current / 10.0f) - LeftV->GetCareSpace() - RightV->GetCareSpace() ) / BottomH->Width, 1.0f, 1.0f));
//	RootBlock->StaticMeshActors[3]->SetActorRelativeLocation(FVector(LeftV->GetCareSpace(),0.0f,0.0f) + Offset);
//
//	RootBlock->PosX = RootBlock->PosX + LeftV->GetCareSpace();
//	RootBlock->PosZ = RootBlock->PosZ + BottomH->GetCareSpace();
//	RootBlock->Width = (WidhtRange.Current / 10.0f) - GetLeftV()->GetCareSpace() - GetRightV()->GetCareSpace();
//	RootBlock->Height = (HeightRange.Current / 10.0f) - GetTopH()->GetCareSpace() - GetBottomH()->GetCareSpace();
//	RootBlock->RecalSizeAndPosition(*this);
//}
//
//void SlidingDoor::FDoor::ResetDividPosition()
//{
//	RootBlock->ResetDividPosition();
//}
//
//TPair<int32,int32> SlidingDoor::FDoor::GetIndexByType(const ESectionalMaterialType InType) const
//{
//	TPair<int32, int32> RetIndex = TPair<int32, int32>(-1, -1);
//	for (const auto & Ref : SectionalMaterials)
//	{
//		if (Ref.Value->Type == StaticCast<int32>(InType))
//		{
//			RetIndex = Ref.Key;
//			break;
//		}
//	}	
//	check(RetIndex.Key != -1 && RetIndex.Value != -1);
//	return RetIndex;
//}
//
//const FString SlidingDoor::FDoor::GetCachePath(const int32 InSectionMaterialId, const FString & InMd5) const
//{
//   const FString CombineId = FString::Printf(TEXT("%s_%d_%d_%d"), TEXT("SlidingDoorBlocks"), Id, InSectionMaterialId,Type);
//   return FAccessoryShape::GetCacheFilePathFromIDAndMd5AndUpdateTime(CombineId,InMd5,0.0f);
//}
//
//void SlidingDoor::FDoor::FDividBlock::ParseInfoFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
//{	
//	DividRation = InJsonObject->GetStringField(TEXT("divideRatio"));
//	if (DividRation.IsEmpty())
//	{
//		bIsRoot = true;
//		DividRation = TEXT("1x");
//	}
//	DividDirection = InJsonObject->GetIntegerField(TEXT("divideDirection"));
//	if (InJsonObject->HasField(TEXT("doorCoreId")))
//	{
//		SectionalMaterialId = InJsonObject->GetIntegerField(TEXT("doorCoreId"));
//	}
//	const TArray<TSharedPtr<FJsonValue>> & ChildrenRef = InJsonObject->GetArrayField(TEXT("children"));
//	for (const auto & Ref : ChildrenRef)
//	{
//		int32 Index = ChildrenBlocks.Emplace(MakeShareable(new FDividBlock));
//		ChildrenBlocks[Index]->ParseInfoFromJson(Ref->AsObject());
//	}
//}
//
//void SlidingDoor::FDoor::FDividBlock::SaveInfoToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
//{
//	if (bIsRoot)
//	{
//		JsonWriter->WriteValue(TEXT("divideRatio"), TEXT(""));
//	}
//	else
//	{
//		JsonWriter->WriteValue(TEXT("divideRatio"), DividRation);
//	}
//	JsonWriter->WriteValue(TEXT("divideDirection"), DividDirection);
//	if (SectionalMaterialId != -1)
//	{
//		JsonWriter->WriteValue(TEXT("doorCoreId"),SectionalMaterialId);
//	}
//	JsonWriter->WriteArrayStart(TEXT("children"));
//	for (const auto & Ref : ChildrenBlocks)
//	{
//		JsonWriter->WriteObjectStart();
//		Ref->SaveInfoToJson(JsonWriter);
//		JsonWriter->WriteObjectEnd();		
//	}
//	JsonWriter->WriteArrayEnd();
//}
//
//void SlidingDoor::FDoor::FDividBlock::SpawnActor(AActor * InParentActor, const FDoor & InParentDoor)
//{
//	auto SpawnStaticMeshActor = [InParentActor,this](const TSharedPtr<FSectionalMaterial> & InOuterSectionMaterial)
//	{
//		UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();
//		if (InOuterSectionMaterial->Type == StaticCast<int32>(ESectionalMaterialType::Louver_Core) && InOuterSectionMaterial->IsShutter)
//		{
//			AActor * TempParentActor = World->SpawnActor<ASCTCompActor>(ASCTCompActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
//			TempParentActor->AttachToActor(InParentActor, FAttachmentTransformRules::KeepRelativeTransform);
//			StaticMeshActors.Add(TempParentActor);
//		}
//		else
//		{
//			ASCTModelActor * NewModelActor = World->SpawnActor<ASCTModelActor>(
//				ASCTModelActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
//			StaticMeshActors.Add(NewModelActor);
//			check(InOuterSectionMaterial->CacheFilePath.IsEmpty() == false);
//			const FMeshDataEntry * MeshData = FSCTShapeManager::Get()->GetStaticMeshManager()->FindMesh(InOuterSectionMaterial->CacheFilePath);
//			check(MeshData && MeshData->Type == 0);
//			NewModelActor->SetStaticMesh(MeshData->MeshData.Mesh, MeshData->MaterialList);
//			NewModelActor->AttachToActor(InParentActor, FAttachmentTransformRules::KeepRelativeTransform);
//		}
//	};
//	// 不切分
//	if (DividDirection == 0)
//	{
//		const TSharedPtr<FSectionalMaterial> & SectionalMaterial = InParentDoor.GetCore(SectionalMaterialId);
//		SpawnStaticMeshActor(SectionalMaterial);
//	}
//	// 竖向切分
//	else if (DividDirection == 1)
//	{		
//		const int32 ExcCount = ChildrenBlocks.Num() - 1;
//		const TSharedPtr<FSectionalMaterial> & SectionalMaterial = InParentDoor.GetMiddleV();
//		for (int Index = 0; Index < ExcCount; ++Index)
//		{
//			SpawnStaticMeshActor(SectionalMaterial);
//		}
//	}
//	// 横向切分
//	else if (DividDirection == 2)
//	{
//		const int32 ExcCount = ChildrenBlocks.Num() - 1;
//		const TSharedPtr<FSectionalMaterial> & SectionalMaterial = InParentDoor.GetMiddleH();
//		for (int Index = 0; Index < ExcCount; ++Index)
//		{
//			SpawnStaticMeshActor(SectionalMaterial);
//		}
//	}
//
//	for (auto & Ref : ChildrenBlocks)
//	{
//		Ref->SpawnActor(InParentActor, InParentDoor);
//	}
//
//}
//
//void SlidingDoor::FDoor::FDividBlock::RecalSizeAndPosition(const FDoor & InParentDoor)
//{
//	// 计算门芯尺寸
//	{
//		auto ParseDividRation = [](const FString & InStr, float & OuterValue)->bool
//		{
//			bool RetIsFormula = false;
//			const FString & TempDividRation = InStr;
//			check(TempDividRation.IsEmpty() == false);
//			RetIsFormula = TempDividRation.Len() > 1 && TempDividRation.EndsWith(TEXT("x"));
//			if (RetIsFormula)
//			{
//				OuterValue = FCString::Atof(*(TempDividRation.LeftChop(TempDividRation.Len() - 1)));
//			}
//			else
//			{
//				OuterValue = FCString::Atof(*TempDividRation);
//			}
//			return RetIsFormula;			
//		};
//		// 纵向切割
//		if (DividDirection == 1)
//		{			
//			const int32 MiddleVCount = ChildrenBlocks.Num() - 1;
//			const float AvailableWidth = Width - InParentDoor.GetMiddleV()->GetCareSpace() * MiddleVCount;
//			const float AvaliableHeight = Height;
//
//			float TotalNoneFormulaWith = 0.0f;			
//			// 先计算所有非比例
//			for (auto & Ref : ChildrenBlocks)
//			{				
//				Ref->IsRation = ParseDividRation(Ref->DividRation, Ref->DividValue);
//				if (Ref->IsRation) continue;
//				Ref->Width = Ref->DividValue / 10.0f;
//				Ref->Height = AvaliableHeight;
//				TotalNoneFormulaWith += Ref->Width;
//			}
//			const float AvailableWidthWithoutRations = AvailableWidth - TotalNoneFormulaWith;
//
//			float TotalRations = 0.0f;
//			// 统计所有比例
//			for (auto & Ref : ChildrenBlocks)
//			{
//				if (Ref->IsRation == false) continue;
//				TotalRations += Ref->DividValue;
//			}
//			// 计算比例所占空间
//			for (auto & Ref : ChildrenBlocks)
//			{
//				if (Ref->IsRation == false) continue;
//				Ref->Width = AvailableWidthWithoutRations * (Ref->DividValue / TotalRations);
//				Ref->Height = AvaliableHeight;
//			}
//		}
//		// 横向切割
//		else if (DividDirection == 2)
//		{
//			const int32 MiddleHCount = ChildrenBlocks.Num() - 1;
//			const float AvailableHeight = Height - InParentDoor.GetMiddleV()->GetCareSpace() * MiddleHCount;
//			const float AvaliableWidth = Width;
//
//			float TotalNoneFormulaHeight = 0.0f;
//			// 先计算所有非比例
//			for (auto & Ref : ChildrenBlocks)
//			{
//				Ref->IsRation = ParseDividRation(Ref->DividRation, Ref->DividValue);
//				if (Ref->IsRation) continue;
//				Ref->Width = AvaliableWidth;
//				Ref->Height = Ref->DividValue / 10.0;
//				TotalNoneFormulaHeight += Ref->Height;
//			}
//			const float AvailableHeightWithoutRations = AvailableHeight - TotalNoneFormulaHeight;
//
//			float TotalRations = 0.0f;
//			// 统计所有比例
//			for (auto & Ref : ChildrenBlocks)
//			{
//				if (Ref->IsRation == false) continue;
//				TotalRations += Ref->DividValue;
//			}
//			// 计算比例所占空间
//			for (auto & Ref : ChildrenBlocks)
//			{
//				if (Ref->IsRation == false) continue;
//				Ref->Width = AvaliableWidth;
//				Ref->Height = AvailableHeightWithoutRations * (Ref->DividValue / TotalRations);
//			}
//		}
//	}
//
//	// 计算门芯坐标
//	{
//		// 纵向分割
//		if (DividDirection == 1)
//		{
//			const int32 ChildrenCount = ChildrenBlocks.Num();
//			const TSharedPtr<FSectionalMaterial>  & InfoRef = InParentDoor.GetMiddleV();
//			float IncreaseX = PosX;
//			int32 ExtIndex = 0;
//			if (bIsRoot)
//			{
//				ExtIndex = 4;
//				check(StaticMeshActors.Num() > 4);
//			}
//			for (int32 Index = 0; Index < ChildrenCount; ++Index)
//			{		
//				ChildrenBlocks[Index]->PosZ = PosZ;	
//				ChildrenBlocks[Index]->PosY = PosY;
//				ChildrenBlocks[Index]->PosX = IncreaseX + Index * InfoRef->GetCareSpace();
//				IncreaseX += ChildrenBlocks[Index]->Width;
//				if (Index == 0) continue;
//				const TSharedPtr<FSectionalMaterial>  & InfoRef = InParentDoor.GetMiddleV();
//#if 0
//				StaticMeshActors[Index + ExtIndex - 1]->SetActorRelativeScale3D(FVector(0.1f, 0.1f, Height / InfoRef->Height / 10.0f));
//				StaticMeshActors[Index + ExtIndex - 1]->SetActorRelativeLocation(FVector((ChildrenBlocks[Index]->PosX - InfoRef->GetCareSpace()) / 10.0f, PosY / 10.0f, PosZ / 10.0f));
//
//#else
//				StaticMeshActors[Index + ExtIndex - 1]->SetActorRelativeScale3D(FVector(Height / InfoRef->Width,1.0f, 1.0f));
//				StaticMeshActors[Index + ExtIndex - 1]->SetActorRelativeRotation(FQuat::MakeFromEuler(FVector(0.0f, -90.0f, 0.0f)));						
//				StaticMeshActors[Index + ExtIndex - 1]->SetActorRelativeLocation(FVector((ChildrenBlocks[Index]->PosX) - InfoRef->GetCareSpace(), PosY , (PosZ + Height)));
//#endif
//			}
//		}
//		// 横向切割
//		else if (DividDirection == 2)
//		{
//			const int32 ChildrenCount = ChildrenBlocks.Num();
//			const TSharedPtr<FSectionalMaterial>  & InfoRef = InParentDoor.GetMiddleH();
//			float IncreaseZ = PosZ;
//			int32 ExtIndex = 0;
//			if (bIsRoot)
//			{
//				ExtIndex = 4;
//				check(StaticMeshActors.Num() > 4);
//			}
//			for (int32 Index = ChildrenCount - 1; Index >= 0; --Index)
//			{
//				ChildrenBlocks[Index]->PosX = PosX;		
//				ChildrenBlocks[Index]->PosY = PosY;
//				ChildrenBlocks[Index]->PosZ = IncreaseZ + (ChildrenCount - 1 - Index) * InfoRef->GetCareSpace();
//				IncreaseZ += ChildrenBlocks[Index]->Height;
//				if (Index == 0) continue;
//				const TSharedPtr<FSectionalMaterial>  & InfoRef = InParentDoor.GetMiddleV();
//				StaticMeshActors[ChildrenCount - 1 - Index + ExtIndex]->SetActorRelativeScale3D(FVector(Width / InfoRef->Width , 1.0f, 1.0f));
//				StaticMeshActors[ChildrenCount - 1 - Index + ExtIndex]->SetActorRelativeLocation(FVector(PosX , PosY, (ChildrenBlocks[Index]->PosZ + ChildrenBlocks[Index]->Height)));			
//			}
//		}
//	}
//
//	// 更新门芯Actor的坐标与尺寸
//	if(DividDirection == 0)
//	{		
//		int32 Index = 0;
//		if (bIsRoot)
//		{
//			Index = 4;
//			check(StaticMeshActors.Num() > 4);
//		}
//		for (; Index <  StaticMeshActors.Num(); ++Index)
//		{			
//			const TSharedPtr<FSectionalMaterial>  & InfoRef = InParentDoor.GetCore(SectionalMaterialId);			
//			// 非百叶 平铺模式下，需要在U方向重复贴图
//			if (InfoRef->DisplayMode == 1 && InfoRef->IsShutter == false)
//			{
//				StaticMeshActors[Index]->SetActorRelativeScale3D(FVector(Width / InfoRef->Width, 1.0f, Height / InfoRef->Height));
//				StaticMeshActors[Index]->SetActorRelativeLocation(FVector(PosX, PosY, PosZ));
//				ASCTModelActor * SCTModelActor = Cast<ASCTModelActor>(StaticMeshActors[Index]);
//				if (SCTModelActor)
//				{
//#if 0
//					Cast<ASCTModelActor>(StaticMeshActors[Index])->SetSingleMaterialOffsetAndRepeatParameter(0.0f, 0.0f, Width / InfoRef->Width, Height / InfoRef->Height);
//#else
//					Cast<ASCTModelActor>(StaticMeshActors[Index])->SetSingleMaterialOffsetAndRepeatParameter(0.0f, 0.0f, Width / InfoRef->Width, 1.0f);
//#endif
//				}
//			}
//			// 如果是百叶，则需要根据尺寸对百叶模型，在垂直方向进行切割
//			if (InfoRef->IsShutter)
//			{
//				// 计算当前高度，需要多少个完整的百叶进行拼接
//				int IntCount = FMath::FloorToInt(Height / InfoRef->Height);
//				ASCTCompActor * SCTCompActor = Cast<ASCTCompActor>(StaticMeshActors[Index]);
//				check(SCTCompActor);
//				// 清除当前已有的百叶子Actor
//				auto ClearCurrentActors = [this](ASCTCompActor * InCompActor)
//				{					
//					TArray<USceneComponent *> SceneCompoents = InCompActor->GetRootComponent()->GetAttachChildren();
//					for (auto & InRef : SceneCompoents)
//					{
//						AActor * TempActor = Cast<AActor>(InRef->GetOwner());
//						if (TempActor)
//						{
//							TempActor->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
//							TempActor->Destroy();
//							InRef->RemoveFromRoot();
//						}
//						
//					}
//					
//				};
//				ClearCurrentActors(SCTCompActor);
//				// 摆放完整的百叶子Actor，只需要横向拉伸，不需要再垂直方向上面进行切割的
//				auto SpawnFullActor = [this,&InfoRef](ASCTCompActor * InCompActor)->AActor*
//				{
//					UWorld* World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();
//					AActor * NewModelActor = World->SpawnActor<AActor>(
//						AActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
//					check(InfoRef->CacheFilePath.IsEmpty() == false);
//					UStaticMeshComponent * StaticMeshCompoent = NewObject<UStaticMeshComponent>();
//					const FMeshDataEntry * MeshData = FSCTShapeManager::Get()->GetStaticMeshManager()->FindMesh(InfoRef->CacheFilePath);
//					check(MeshData && MeshData->Type == 0);				
//					StaticMeshCompoent->SetStaticMesh(MeshData->MeshData.Mesh);
//					for (const auto & Ref : MeshData->MaterialList)
//					{
//						StaticMeshCompoent->SetMaterialByName(FName(*Ref.Key), Ref.Value);
//					}					
//					UProceduralMeshComponent * ProcedureMeshCompoent = NewObject<UProceduralMeshComponent>(NewModelActor);
//					NewModelActor->AddOwnedComponent(ProcedureMeshCompoent);
//					NewModelActor->SetRootComponent(ProcedureMeshCompoent);					
//					ProcedureMeshCompoent->RegisterComponent();
//					UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(StaticMeshCompoent, 0, ProcedureMeshCompoent, true);
//					NewModelActor->AttachToActor(InCompActor, FAttachmentTransformRules::KeepRelativeTransform);					
//					return NewModelActor;
//				};
//				float ToClipSize = (Height / InfoRef->Height) - IntCount;
//				for (int32 Index = 0; Index < IntCount; ++Index)
//				{
//					const float ZOffset = (ToClipSize + Index) * InfoRef->Height;
//					AActor * TempActor = SpawnFullActor(SCTCompActor);
//					TempActor->SetActorScale3D(FVector(Width / InfoRef->Width, 1.0f, 1.0f));
//					TempActor->SetActorRelativeLocation(FVector(PosX, PosY, PosZ + ZOffset));
//				}
//				if (ToClipSize *  InfoRef->Height > 1.0f)
//				{
//					AActor * RunTimeActor = SpawnFullActor(SCTCompActor);
//					check(RunTimeActor);
//					UProceduralMeshComponent * Compoent = Cast<UProceduralMeshComponent>(RunTimeActor->GetRootComponent());										
//					UProceduralMeshComponent * OuterHalfCompoent = nullptr;
//					const float D = (1.0f - ToClipSize) *  InfoRef->Height;					
//					FVector PlanePos = FVector(0.0f, 0.0f, D) + SCTCompActor->GetActorLocation();
//					UKismetProceduralMeshLibrary::SliceProceduralMesh(Compoent, PlanePos,FVector::UpVector,false, OuterHalfCompoent, EProcMeshSliceCapOption::NoCap,nullptr);
//					RunTimeActor->SetActorScale3D(FVector(Width / InfoRef->Width, 1.0f, 1.0f));
//					RunTimeActor->SetActorRelativeLocation(FVector(PosX, PosY, PosZ - D));
//					
//				}
//			}
//
//		}
//	}
//
//	// 更新子块
//	for (auto & Ref : ChildrenBlocks)
//	{
//		Ref->RecalSizeAndPosition(InParentDoor);
//	}
//}
//
//void SlidingDoor::FDoor::FDividBlock::ResetDividPosition()
//{
//	 PosX = 0.0f;
//	 PosY = 0.0f;
//	 PosZ = 0.0f;
//	 for (auto & Ref : ChildrenBlocks)
//	 {
//		 Ref->ResetDividPosition();
//	 }
//}
//
//TSharedPtr<SlidingDoor::FDoor::FDividBlock> SlidingDoor::FDoor::FDividBlock::CloneDividBlock() const
//{
//	TSharedPtr<FDividBlock> NewBlock = MakeShareable(new FDividBlock);
//	NewBlock->bIsRoot = this->bIsRoot;
//	NewBlock->DividDirection = this->DividDirection;
//	NewBlock->DividValue = this->DividValue;
//	NewBlock->IsRation = this->IsRation;
//	NewBlock->DividRation = this->DividRation;
//	NewBlock->Width = this->Width;
//	NewBlock->Height = this->Height;
//	NewBlock->PosX = this->PosX;
//	NewBlock->PosY = this->PosY;
//	NewBlock->PosZ = this->PosZ;
//	NewBlock->SectionalMaterialId = this->SectionalMaterialId;
//	for (const auto & Ref : ChildrenBlocks)
//	{
//		NewBlock->ChildrenBlocks.Add(Ref->CloneDividBlock());
//	}
//	return NewBlock;
//}
//
//void SlidingDoor::FDoor::FDividBlock::DestoryActor()
//{
//	for (auto & Ref : StaticMeshActors)
//	{		
//		if (Cast<ASCTModelActor>(Ref))
//		{
//			Ref->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
//			Ref->Destroy();
//		}
//		else if(Cast<ASCTCompActor>(Ref))
//		{														
//			TArray<USceneComponent *> SceneCompoents = Ref->GetRootComponent()->GetAttachChildren();			
//			for (auto & InRef : SceneCompoents)
//			{
//				AActor * TempActor = Cast<AActor>(InRef->GetOwner());
//				if (TempActor)
//				{
//					TempActor->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
//					TempActor->Destroy();
//				}
//			}			
//		}			
//	}
//	StaticMeshActors.Empty();
//	for (auto & Ref : ChildrenBlocks)
//	{
//		Ref->DestoryActor();
//	}	
//}
//
//float SlidingDoor::FDoor::FSectionalMaterial::GetCareSpace() const
//{
//	float RetValue = 0.0f;
//	switch (StaticCast<ESectionalMaterialType>(Type))
//	{
//	case ESectionalMaterialType::Left_V:	
//	case ESectionalMaterialType::Right_V:
//	{
//		RetValue = Width;
//	}
//	break;
//	case ESectionalMaterialType::Middle_V:
//	case ESectionalMaterialType::Top_H:
//	case ESectionalMaterialType::Middle_H:
//	case ESectionalMaterialType::Bottom_H:
//	{
//		RetValue = Height;
//	}
//	break;
//	default:
//		break;
//	}
//	return RetValue;
//}
//
//void SlidingDoor::FPlateBoard::SpawnActor(AActor * InParentActor)
//{	
//	ASCTShapeActor* ShapeActor = BoardShape->SpawnShapeActor();
//	ShapeActor->AttachToActor(InParentActor, FAttachmentTransformRules::KeepRelativeTransform);
//}
//
//void SlidingDoor::FPlateBoard::ParseInfoFromJson(const TSharedPtr<FJsonObject>& InJsonObject,FSCTShape * InParentShape)
//{
//	Depth = StaticCast<float>(InJsonObject->GetNumberField(TEXT("depth")));
//	SubstrateId = InJsonObject->GetIntegerField(TEXT("substrateId"));
//	Height = StaticCast<float>(InJsonObject->GetNumberField(TEXT("height")));
//
//	BoardShape = MakeShareable(new FBoardShape);
//	FString FileContent = TEXT(
//		R"(
//			{
//				"id": 54,
//				"name" : "",
//				"code" : "",
//				"type" : 1,							
//				"width" : {
//					"id": 0, 
//					"type" : 3, 
//					"refName" : "WIDTH", 
//					"value" : {
//					"min": -10000, 
//					"max" : 10000
//					}, 
//					"current" : "500", 
//					"currentValue" : ""
//				}, 
//				"depth": {
//					"id": 0, 
//					"type" : 3, 
//					"refName" : "DEPTH", 
//					"value" : {
//					"min": -10000, 
//					"max" : 10000
//					}, 
//					"current" : "20", 
//					"currentValue" : ""
//				}, 
//				"height": {
//					"id": 0, 
//					"type" : 3, 
//					"refName" : "HEIGHT", 
//					"value" : {
//					"min": -10000, 
//					"max" : 10000
//					}, 
//					"current" : "800", 
//					"currentValue" : ""
//				}, 
//				"posx": {
//					"id": 0, 
//					"type" : 3, 
//					"refName" : "POSX", 
//					"value" : {
//					"min": -100000, 
//					"max" : 100000
//					}, 
//					"current" : "0", 
//					"currentValue" : ""
//				}, 
//				"posy": {
//					"id": 0, 
//					"type" : 3, 
//					"refName" : "POSY", 
//					"value" : {
//					"min": -10000, 
//					"max" : 10000
//					}, 
//					"current" : "0", 
//					"currentValue" : ""
//				}, 
//				"posz": {
//					"id": 0, 
//					"type" : 3, 
//					"refName" : "POSZ", 
//					"value" : {
//					"min": -10000, 
//					"max" : 10000
//					}, 
//					"current" : "0", 
//					"currentValue" : ""
//				},
//			   "rotx": {
//						"id": 0,
//						"type": 3,
//						"refName": "ROTX",
//						"value": {
//							"min": -180,
//							"max": 180
//						},
//						"current": "0",
//						"currentValue": ""
//					},
//				"roty": {
//						"id": 0,
//						"type": 3,
//						"refName": "ROTY",
//						"value": {
//							"min": -180,
//							"max": 180
//						},
//						"current": "0",
//						"currentValue": ""
//				  },
//				"rotz": {
//						"id": 0,
//						"type": 3,
//						"refName": "ROTZ",
//						"value": {
//							"min": -180,
//							"max": 180
//						},
//						"current": "0",
//						"currentValue": ""
//				}
//			}
//		)");
//	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(FileContent);
//	TSharedPtr<FJsonObject> Data;
//	FJsonSerializer::Deserialize(JsonReader, Data);
//	BoardShape->ParseFromJson(Data);
//	BoardShape->SetBoardShapeType(BoST_Rectangle);			
//	BoardShape->SetParentShape(InParentShape);	
//	BoardShape->SetShapeDepth(Depth);
//	BoardShape->SetShapeHeight(Height);	
//
//	// 材质信息
//	{
//		const TSharedPtr<FJsonObject> & ObjRef = InJsonObject->GetObjectField(TEXT("material"));
//		Material.ID = ObjRef->GetIntegerField(TEXT("id"));
//		Material.Url = ObjRef->GetStringField(TEXT("pakUrl"));
//		Material.MD5 = ObjRef->GetStringField(TEXT("pakMd5"));
//		Material.Name = ObjRef->GetStringField(TEXT("name"));
//		FPakMetaData TempInfo;
//		TempInfo.ID = Material.ID;
//		TempInfo.Url = Material.Url;
//		TempInfo.MD5 = Material.MD5;
//		TempInfo.Name = Material.Name;
//		BoardShape->SetMaterialData(TempInfo);
//	}
//
//	// 封边信息
//	{
//		const TSharedPtr<FJsonObject> & ObjRef = InJsonObject->GetObjectField(TEXT("edgeBanding"));
//		EdgeBanding.ID = ObjRef->GetIntegerField(TEXT("id"));
//		EdgeBanding.Url = ObjRef->GetStringField(TEXT("pakUrl"));
//		EdgeBanding.MD5 = ObjRef->GetStringField(TEXT("pakMd5"));
//		EdgeBanding.Name = ObjRef->GetStringField(TEXT("name"));
//		FPakMetaData TempInfo;
//		TempInfo.ID = EdgeBanding.ID;
//		TempInfo.Url = EdgeBanding.Url;
//		TempInfo.MD5 = EdgeBanding.MD5;
//		TempInfo.Name = EdgeBanding.Name;
//		BoardShape->SetEdgeBandingData(TempInfo);		
//	}
//
//	// 基材信息
//	{
//		const TSharedPtr<FJsonObject> & ObjRef = InJsonObject->GetObjectField(TEXT("substrate"));
//		Substrate.ID = ObjRef->GetIntegerField(TEXT("id"));				
//		Substrate.Name = ObjRef->GetStringField(TEXT("name"));
//		BoardShape->SetSubstrateID(Substrate.ID);
//		BoardShape->SetSubstrateName(Substrate.Name);
//		BoardShape->SetSubstrateType(StaticCast<ESubstrateType>(ObjRef->GetIntegerField(TEXT("type"))));
//	}
//
//}
//
//void SlidingDoor::FPlateBoard::SaveInfoToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
//{
//	JsonWriter->WriteValue(TEXT("depth"), Depth);
//	JsonWriter->WriteValue(TEXT("substrateId"), SubstrateId);
//	JsonWriter->WriteValue(TEXT("height"), Height);
//	JsonWriter->WriteObjectStart(TEXT("material"));
//	JsonWriter->WriteValue(TEXT("id"), Material.ID);
//	JsonWriter->WriteValue(TEXT("pakUrl"), Material.Url);
//	JsonWriter->WriteValue(TEXT("pakMd5"), Material.MD5);
//	JsonWriter->WriteObjectEnd();		
//}
//
//void SlidingDoor::FPlateBoard::GetResourceUrls(TArray<FString>& OutResourceUrls)
//{
//	BoardShape->GetResourceUrls(OutResourceUrls);
//}
//
//void SlidingDoor::FPlateBoard::GetFileCachePaths(TArray<FString>& OutFileCachePaths)
//{
//	BoardShape->GetFileCachePaths(OutFileCachePaths);
//}
//
//TSharedPtr<SlidingDoor::FPlateBoard> SlidingDoor::FPlateBoard::ClonePlateBoard(FSCTShape * InParentShape)
//{
//	TSharedPtr<FPlateBoard> NewPlateBoard = MakeShareable(new FPlateBoard);
//	NewPlateBoard->Depth = this->Depth;
//	NewPlateBoard->SubstrateId = this->SubstrateId;
//	NewPlateBoard->Height = this->Height;
//	NewPlateBoard->Material = this->Material;
//	NewPlateBoard->EdgeBanding = this->EdgeBanding;
//	NewPlateBoard->IsTopBoard = this->IsTopBoard;
//	NewPlateBoard->Substrate = this->Substrate;
//	NewPlateBoard->bShow = this->bShow;
//	NewPlateBoard->BoardShape = MakeShareable(new FBoardShape);	
//	this->BoardShape->CopyTo(NewPlateBoard->BoardShape.Get());
//	NewPlateBoard->BoardShape->SetParentShape(InParentShape);
//	return NewPlateBoard;
//}
//
//void SlidingDoor::FPlateBoard::ReCalPlateBoardPositionAndSize(const float InDoorWidth, const float InDoorHeight, float & OutZCullValue, float & OutZIncreaseValue,
//	float & OutZDecreaseValue, float & OutYIncreaseValue, float & OutTopYIncreaseVlue, float & OutBottomIncreaseValue)
//{
//	if (bShow == false) return;
//	BoardShape->SetShapeWidth(InDoorWidth );
//	BoardShape->SetShapePosX(0.0f);
//	BoardShape->SetShapePosY(0.0f);
//	OutZCullValue += BoardShape->GetShapeHeight();
//	if (IsTopBoard)
//	{
//		BoardShape->SetShapePosZ(InDoorHeight - BoardShape->GetShapeHeight());	
//		OutZDecreaseValue += BoardShape->GetShapeHeight();
//		OutTopYIncreaseVlue += BoardShape->GetShapeDepth() / 2.0f;
//	}
//	else
//	{
//		BoardShape->SetShapePosZ(0.0f);	
//		OutZIncreaseValue += BoardShape->GetShapeHeight();
//		OutYIncreaseValue += BoardShape->GetShapeDepth() / 2.0f;
//		OutBottomIncreaseValue += BoardShape->GetShapeDepth() / 2.0f;
//	}
//}
//
//void SlidingDoor::FPlateBoard::SetVisiable(const bool bInVisiable)
//{
//	if (GetVisiable() == bInVisiable) return;
//	bShow = bInVisiable;
//	BoardShape->GetShapeActor()->SetActorHiddenInGame(!bShow);
//}
