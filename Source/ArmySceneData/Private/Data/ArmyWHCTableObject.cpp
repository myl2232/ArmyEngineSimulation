#include "ArmyWHCTableObject.h"
#include "ArmyFurniture.h"


void FArmyWHCTableObject::FTableInfo::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	JsonWriter->WriteValue("ComboStyle", ComboStyle);
	JsonWriter->WriteValue("BoardMaterial", ComboStyle);
	JsonWriter->WriteValue("FrontEdgeName", FrontEdgeName);
	JsonWriter->WriteValue("BackEdgeName", BackEdgeName);

	JsonWriter->WriteValue("BoardHeight", BoardHeight);
	JsonWriter->WriteValue("FrontEdgeHeight", FrontEdgeHeight);
	JsonWriter->WriteValue("BackEdgeHeight", BackEdgeHeight);

	JsonWriter->WriteArrayStart("StoveTypeArray");
	for (auto& Str : StoveTypeArray)
	{
		JsonWriter->WriteValue(Str);
	}
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteArrayStart("SinkTypeArray");
	for (auto& Str : SinkTypeArray)
	{
		JsonWriter->WriteValue(Str);
	}
	JsonWriter->WriteArrayEnd();
}
void FArmyWHCTableObject::FTableInfo::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	InJsonData->TryGetStringField("ComboStyle", ComboStyle);
	InJsonData->TryGetStringField("BoardMaterial", BoardMaterial);
	InJsonData->TryGetStringField("FrontEdgeName", FrontEdgeName);
	InJsonData->TryGetStringField("BackEdgeName", BackEdgeName);

	BoardHeight = InJsonData->GetNumberField("BoardHeight");
	FrontEdgeHeight = InJsonData->GetNumberField("FrontEdgeHeight");
	BackEdgeHeight = InJsonData->GetNumberField("BackEdgeHeight");

	for (auto& JsonV : InJsonData->GetArrayField("StoveTypeArray"))
	{
		StoveTypeArray.Add(JsonV->AsString());
	}
	for (auto& JsonV : InJsonData->GetArrayField("SinkTypeArray"))
	{
		SinkTypeArray.Add(JsonV->AsString());
	}
}

FArmyWHCTableObject::FArmyWHCTableObject()
{
    SetName(TEXT("WHCTable"));
	DrawComponent = false;
	DrawFrontOutLine = true;
	DrawBackOutLine = true;
	BoundingBox.Init();
}

void FArmyWHCTableObject::SetTableOutline(const TArray<FArmyWHCTableObject::FWHCTableBatchData>& InBatchDataArray, float SplineWidth, const TArray<TWeakPtr<class FArmyFurniture>>& InComponentArray)
{
	struct FPosVertexInfo
	{
		TArray<FVector2D> OutlinePoints;
		TArray<FVector2D> FrontOutlinePoints;
		TArray<FVector2D> BackOutlinePoints;
		bool bClosed = false;
	};
	FBox2D LocalBoundingBox(ForceInitToZero);
	TArray<FPosVertexInfo> PosDataArray;
	for (auto& BatchData : InBatchDataArray)
	{
		int32 Index = PosDataArray.Add(FPosVertexInfo());
		FPosVertexInfo& CurrentPosData = PosDataArray[Index];

		CurrentPosData.bClosed = BatchData.bClosed;
		//CurrentPosData.FrontOutlinePoints = BatchData.VerFronts;
		float FrontShrink = 15.f;
		if (BatchData.bClosed)
		{
			int32 NumFrontPoints = BatchData.VerFronts.Num();
			FVector2D DirPrev, DirNext, RightPrev, RightNext;
			for (int32 i = 0; i < NumFrontPoints; ++i)
			{
				DirPrev = BatchData.VerFronts[i] - BatchData.VerFronts[(i - 1 + NumFrontPoints) % NumFrontPoints];
				DirPrev.Normalize();
				RightPrev.Set(-DirPrev.Y, DirPrev.X);

				DirNext = BatchData.VerFronts[(i + 1 + NumFrontPoints) % NumFrontPoints] - BatchData.VerFronts[i];
				DirNext.Normalize();
				RightNext.Set(-DirNext.Y, DirNext.X);

				CurrentPosData.OutlinePoints.Emplace(BatchData.VerFronts[i] + RightPrev * SplineWidth + RightNext * SplineWidth);
				CurrentPosData.FrontOutlinePoints.Emplace(BatchData.VerFronts[i] + RightPrev * (SplineWidth - FrontShrink) + RightNext * (SplineWidth - FrontShrink));
			}
		}
		else
		{
			FVector2D Dir = BatchData.VerFronts[1] - BatchData.VerFronts[0];
			Dir.Normalize();
			FVector2D Right(-Dir.Y, Dir.X);
			CurrentPosData.OutlinePoints.Emplace(BatchData.VerFronts[0] + Right * SplineWidth);
			CurrentPosData.FrontOutlinePoints.Emplace(BatchData.VerFronts[0] + Right * (SplineWidth - FrontShrink));

			FVector2D DirNext, RightNext;
			for (int32 i = 1; i < BatchData.VerFronts.Num() - 1; ++i)
			{
				DirNext = BatchData.VerFronts[i + 1] - BatchData.VerFronts[i];
				DirNext.Normalize();
				RightNext.Set(-DirNext.Y, DirNext.X);
				CurrentPosData.OutlinePoints.Emplace(BatchData.VerFronts[i] + Right * SplineWidth + RightNext * SplineWidth);
				CurrentPosData.FrontOutlinePoints.Emplace(BatchData.VerFronts[i] + Right * (SplineWidth - FrontShrink) + RightNext * (SplineWidth - FrontShrink));
				Right = RightNext;
			}

			CurrentPosData.OutlinePoints.Emplace(BatchData.VerFronts[BatchData.VerFronts.Num() - 1] + Right * SplineWidth);
			CurrentPosData.FrontOutlinePoints.Emplace(BatchData.VerFronts[BatchData.VerFronts.Num() - 1] + Right * (SplineWidth - FrontShrink));

			if (BatchData.VerBacks.Num() > 2)
			{
				for (int32 i = BatchData.VerBacks.Num() - 2; i > 0; --i)
					CurrentPosData.OutlinePoints.Emplace(BatchData.VerBacks[i]);
			}
		}

		int32 NumBackPoints = BatchData.VerBacks.Num();

		if (NumBackPoints > 1)
		{
			constexpr float BackShrink = 15.0f;

			FVector2D Dir = BatchData.VerBacks[1] - BatchData.VerBacks[0];
			Dir.Normalize();
			FVector2D Right(-Dir.Y, Dir.X);
			CurrentPosData.BackOutlinePoints.Emplace(BatchData.VerBacks[0] + Right * BackShrink);

			FVector2D DirNext, RightNext;
			for (int32 i = 1; i < BatchData.VerBacks.Num() - 1; ++i)
			{
				DirNext = BatchData.VerBacks[i + 1] - BatchData.VerBacks[i];
				DirNext.Normalize();
				RightNext.Set(-DirNext.Y, DirNext.X);
				CurrentPosData.BackOutlinePoints.Emplace(BatchData.VerBacks[i] + Right * BackShrink + RightNext * BackShrink);
				Right = RightNext;
			}

			CurrentPosData.BackOutlinePoints.Emplace(BatchData.VerBacks[BatchData.VerBacks.Num() - 1] + Right * BackShrink);
		}

		/////////////////////////////////////Reset//////////////////////////////////////////////
		LocalBoundingBox += FBox2D(CurrentPosData.OutlinePoints);
		LocalBoundingBox += FBox2D(CurrentPosData.FrontOutlinePoints);
		LocalBoundingBox += FBox2D(CurrentPosData.BackOutlinePoints);
	}
	FVector BasePos = FVector(LocalBoundingBox.Min.X * 0.1f, LocalBoundingBox.Max.Y * 0.1f, 0.0f);//以包围盒的左下角作为基点

	BoundingBox.Init();
	LocalTransform = FTransform::Identity;
	LocalTransform.SetLocation(BasePos);

	TableOutline.Empty();
	TableFrontOutline.Empty();
	TableBackOutline.Empty();
	/////////////////////////////////////////////////////////////////////////////////
	for(auto& PostData : PosDataArray)
	{
		int32 NumPoints = PostData.OutlinePoints.Num();
		for (int32 i = 0; i < NumPoints; ++i)
		{
			FArmyLine *NewLine = new FArmyLine(FVector(PostData.OutlinePoints[i] * 0.1f, 0.0f) - BasePos,
				FVector(PostData.OutlinePoints[(i + 1) % NumPoints] * 0.1f, 0.0f) - BasePos);
			NewLine->SetLineWidth(1.0f);
			NewLine->SetLineColor(FLinearColor::White);

			TableOutline.Emplace(FLineTransformInfo(MakeShareable(NewLine), MakeShareable(new FArmyLine(NewLine))));
			BoundingBox += NewLine->GetStart();
			BoundingBox += NewLine->GetEnd();
		}

		int32 NumFrontPoints = PostData.FrontOutlinePoints.Num();
		int32 IterCount = PostData.bClosed ? NumFrontPoints : NumFrontPoints - 1;
		for (int32 i = 0; i < IterCount; ++i)
		{
			FArmyLine *NewFrontLine = new FArmyLine(FVector(PostData.FrontOutlinePoints[i] * 0.1f, 0.0f) - BasePos,
				FVector(PostData.FrontOutlinePoints[(i + 1) % NumFrontPoints] * 0.1f, 0.0f) - BasePos);
			NewFrontLine->SetLineWidth(1.0f);
			NewFrontLine->SetLineColor(FLinearColor::White);
			NewFrontLine->bIsDashLine = true;
			TableFrontOutline.Emplace(FLineTransformInfo(MakeShareable(NewFrontLine), MakeShareable(new FArmyLine(NewFrontLine))));
			BoundingBox += NewFrontLine->GetStart();
			BoundingBox += NewFrontLine->GetEnd();
		}

		for (int32 i = 0; i < PostData.BackOutlinePoints.Num() - 1; ++i)
		{
			FArmyLine *NewBackLine = new FArmyLine(FVector(PostData.BackOutlinePoints[i] * 0.1f, 0.0f) - BasePos,
				FVector(PostData.BackOutlinePoints[i + 1] * 0.1f, 0.0f) - BasePos);
			NewBackLine->SetLineWidth(1.0f);
			NewBackLine->SetLineColor(FLinearColor::White);
			TableBackOutline.Emplace(FLineTransformInfo(MakeShareable(NewBackLine), MakeShareable(new FArmyLine(NewBackLine))));
			BoundingBox += NewBackLine->GetStart();
			BoundingBox += NewBackLine->GetEnd();
		}
	}
	//////
	for (auto ObjPtr : InComponentArray)
	{
		if (!ObjPtr.IsValid()) continue;
		FVector ObjPos = ObjPtr.Pin()->GetBasePos();
		ObjPos.Z = 0;
		FVector CenterPos = ObjPos - BasePos;
		FRotator Rot;
		FVector Position;
		FVector Scale;
		ObjPtr.Pin()->GetTransForm(Rot,Position,Scale);
		FVector TransDir = Rot.RotateVector(FVector(0,1,0));
		TransDir.Normalize();
		FVector FirstPoint = CenterPos - TransDir * BoundingBox.GetSize().Size();
		FVector SecondPoint = CenterPos + TransDir * BoundingBox.GetSize().Size();

		TArray<FVector> InterPosArray;
		for (auto& OutL : TableOutline)
		{
			FVector IterPos;
			if (FMath::SegmentIntersection2D(OutL.BaseLine->GetStart(), OutL.BaseLine->GetEnd(), FirstPoint, SecondPoint, IterPos))
			{
				InterPosArray.AddUnique(IterPos);
			}
		}

		if (InterPosArray.Num() > 1)
		{
			InterPosArray.Sort([&](const FVector& A, const FVector& B) {
				return (A - CenterPos).Size() < (B - CenterPos).Size();
			});

			FirstPoint = InterPosArray[0];
			FVector FirstDir = (FirstPoint - CenterPos).GetSafeNormal();
			FirstPoint = FirstPoint + FirstDir * 5;
			for (int32 i = 1;i < InterPosArray.Num();++i)
			{
				FVector& V = InterPosArray[i];
				if (((V - CenterPos).GetSafeNormal() - FirstDir).Size() > 1)
				{
					SecondPoint = V - FirstDir * 5;
					break;
				}
			}
		}

		int32 Index = OnTabelComponentArray.Add(FComponentInfo());
		FComponentInfo& VInfo = OnTabelComponentArray[Index];
		VInfo.CenterLine.BaseLine = MakeShareable(new FArmyLine(FirstPoint,SecondPoint));
		VInfo.CenterLine.DrawLine = MakeShareable(new FArmyLine(FirstPoint, SecondPoint));
		VInfo.CenterLine.DrawLine->bIsDashLine = true;
		VInfo.BaseArray = ObjPtr.Pin()->GetElementVertex();
		for (auto& V : VInfo.BaseArray)
		{
			V.Position = V.Position - BasePos;
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	Update();
}

void FArmyWHCTableObject::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
    if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
    {
		for (auto &LInfo : TableOutline)
			LInfo.DrawLine->Draw(PDI,View);
		
		if (DrawFrontOutLine)
		{
			for (auto &LInfo : TableFrontOutline)
				LInfo.DrawLine->Draw(PDI, View);
		}
		if (DrawBackOutLine)
		{
			for (auto &LInfo : TableBackOutline)
				LInfo.DrawLine->Draw(PDI, View);
		}
		if (DrawComponent)
		{
			for (auto& ObjInfo : OnTabelComponentArray)
			{
				PDI->DrawLines(ObjInfo.DrawArray, 1);
				ObjInfo.CenterLine.DrawLine->Draw(PDI,View);
			}
		}
    }
}
void FArmyWHCTableObject::Update()
{
	for (auto &LInfo : TableOutline)
	{
		LInfo.DrawLine->SetStart(LocalTransform.TransformPosition(LInfo.BaseLine->GetStart()));
		LInfo.DrawLine->SetEnd(LocalTransform.TransformPosition(LInfo.BaseLine->GetEnd()));
	}
	for (auto &LInfo : TableFrontOutline)
	{
		LInfo.DrawLine->SetStart(LocalTransform.TransformPosition(LInfo.BaseLine->GetStart()));
		LInfo.DrawLine->SetEnd(LocalTransform.TransformPosition(LInfo.BaseLine->GetEnd()));
	}
	for (auto &LInfo : TableBackOutline)
	{
		LInfo.DrawLine->SetStart(LocalTransform.TransformPosition(LInfo.BaseLine->GetStart()));
		LInfo.DrawLine->SetEnd(LocalTransform.TransformPosition(LInfo.BaseLine->GetEnd()));
	}
	for (auto& VInfo : OnTabelComponentArray)
	{
		VInfo.CenterLine.DrawLine->SetStart(LocalTransform.TransformPosition(VInfo.CenterLine.BaseLine->GetStart()));
		VInfo.CenterLine.DrawLine->SetEnd(LocalTransform.TransformPosition(VInfo.CenterLine.BaseLine->GetEnd()));
		VInfo.DrawArray.Empty();
		for (auto V : VInfo.BaseArray)
		{
			V.Position = LocalTransform.TransformPosition(FVector(V.Position));
			VInfo.DrawArray.Add(V);
		}
	}
}
void FArmyWHCTableObject::ApplyTransform(const FTransform& Trans)
{
	LocalTransform = Trans;
	Update();
}

void FArmyWHCTableObject::GetTransForm(FRotator& InRot, FVector& InTran, FVector& InScale) const
{
	InTran = LocalTransform.GetLocation();
	InRot = LocalTransform.Rotator();
	InScale = LocalTransform.GetScale3D();
}

const FBox FArmyWHCTableObject::GetBounds()
{
	FBox V(ForceInitToZero);
	V += LocalTransform.TransformPosition(BoundingBox.Min);
	V += LocalTransform.TransformPosition(BoundingBox.Max);
	V += LocalTransform.TransformPosition(FVector(BoundingBox.Min.X, BoundingBox.Max.Y, 0));
	V += LocalTransform.TransformPosition(FVector(BoundingBox.Max.X, BoundingBox.Min.Y, 0));
	return V;
}

const FBox FArmyWHCTableObject::GetPreBounds()
{
	return BoundingBox;
}

const FVector FArmyWHCTableObject::GetBasePos()
{
	return LocalTransform.GetTranslation();
}
void FArmyWHCTableObject::GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs)
{
	for (auto L : TableOutline)
	{
		OutLines.Add(L.DrawLine);
	}
	if (DrawComponent)
	{
		for (auto& ObjInfo : OnTabelComponentArray)
		{
			OutLines.Add(ObjInfo.CenterLine.DrawLine);
		}
	}
	//for (auto L : TableFrontOutline)
	//{
	//	OutLines.Add(L.DrawLine);
	//}
	//for (auto L : TableBackOutline)
	//{
	//	OutLines.Add(L.DrawLine);
	//}
}
void FArmyWHCTableObject::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
	JsonWriter->WriteValue("Type", TEXT("OT_CupboardTable"));

	JsonWriter->WriteValue("DrawComponent", DrawComponent);
	JsonWriter->WriteValue("LocalTransform", LocalTransform.ToString());

	JsonWriter->WriteObjectStart("TableInfo");
	TableInfo.SerializeToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteArrayStart("TableOutline");
	for (auto L : TableOutline)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("StartPoint", L.BaseLine->GetStart().ToString());
		JsonWriter->WriteValue("EndPoint", L.BaseLine->GetEnd().ToString());
		JsonWriter->WriteValue("Color", L.BaseLine->GetBaseColor().ToString());
		JsonWriter->WriteValue("bIsDashLine", L.BaseLine->bIsDashLine);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteArrayStart("TableFrontOutline");
	for (auto L : TableFrontOutline)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("StartPoint", L.BaseLine->GetStart().ToString());
		JsonWriter->WriteValue("EndPoint", L.BaseLine->GetEnd().ToString());
		JsonWriter->WriteValue("Color", L.BaseLine->GetBaseColor().ToString());
		JsonWriter->WriteValue("bIsDashLine", L.BaseLine->bIsDashLine);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteArrayStart("TableBackOutline");
	for (auto L : TableBackOutline)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("StartPoint", L.BaseLine->GetStart().ToString());
		JsonWriter->WriteValue("EndPoint", L.BaseLine->GetEnd().ToString());
		JsonWriter->WriteValue("Color", L.BaseLine->GetBaseColor().ToString());
		JsonWriter->WriteValue("bIsDashLine", L.BaseLine->bIsDashLine);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteArrayStart("OnTabelComponentArray");
	for (auto ObjInfo : OnTabelComponentArray)
	{
		JsonWriter->WriteObjectStart();

		JsonWriter->WriteObjectStart("CenterBaseLine");
		JsonWriter->WriteValue("StartPoint", ObjInfo.CenterLine.BaseLine->GetStart().ToString());
		JsonWriter->WriteValue("EndPoint", ObjInfo.CenterLine.BaseLine->GetEnd().ToString());
		JsonWriter->WriteValue("Color", ObjInfo.CenterLine.BaseLine->GetBaseColor().ToString());
		JsonWriter->WriteValue("bIsDashLine", ObjInfo.CenterLine.BaseLine->bIsDashLine);
		JsonWriter->WriteObjectEnd();
		JsonWriter->WriteObjectStart("CenterDrawLine");
		JsonWriter->WriteValue("StartPoint", ObjInfo.CenterLine.DrawLine->GetStart().ToString());
		JsonWriter->WriteValue("EndPoint", ObjInfo.CenterLine.DrawLine->GetEnd().ToString());
		JsonWriter->WriteValue("Color", ObjInfo.CenterLine.DrawLine->GetBaseColor().ToString());
		JsonWriter->WriteValue("bIsDashLine", ObjInfo.CenterLine.DrawLine->bIsDashLine);
		JsonWriter->WriteObjectEnd();
		JsonWriter->WriteArrayStart("ComponentVertexArray");
		for (auto& V : ObjInfo.BaseArray)
		{
			JsonWriter->WriteObjectStart();
			JsonWriter->WriteValue("Color", V.Color.ToString());
			JsonWriter->WriteValue("HitProxyIdColor", V.HitProxyIdColor.ToString());
			JsonWriter->WriteValue("Position", V.Position.ToString());
			JsonWriter->WriteValue("TextureCoordinate", V.TextureCoordinate.ToString());
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();

		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
	
	SERIALIZEREGISTERCLASS(JsonWriter, FArmyWHCTableObject)
}
void FArmyWHCTableObject::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	BoundingBox.Init();
	TableOutline.Empty();
	TableFrontOutline.Empty();
	TableBackOutline.Empty();

	FArmyObject::Deserialization(InJsonData);

	if (InJsonData->GetStringField("Type") == TEXT("OT_CupboardTable"))
	{
		ObjectType = OT_CupboardTable;
	}
	DrawComponent = InJsonData->GetBoolField("DrawComponent");
	LocalTransform.InitFromString(InJsonData->GetStringField("LocalTransform"));

	const TSharedPtr<FJsonObject> TableInfoJsonObj = InJsonData->GetObjectField("TableInfo");
	if (TableInfoJsonObj.IsValid())
	{
		TableInfo.Deserialization(TableInfoJsonObj);
	}

	const TArray< TSharedPtr<FJsonValue> >& JsonTableOutline = InJsonData->GetArrayField("TableOutline");
	for (auto JsonV : JsonTableOutline)
	{
		const TSharedPtr<FJsonObject>& JsonObj = JsonV->AsObject();
		if (JsonObj.IsValid())
		{
			TSharedPtr<FArmyLine> L = MakeShareable(new FArmyLine);
			FVector StartPoint, EndPoint;
			StartPoint.InitFromString(JsonObj->GetStringField("StartPoint"));
			EndPoint.InitFromString(JsonObj->GetStringField("EndPoint"));
			L->SetStart(StartPoint);
			L->SetEnd(EndPoint);
			FLinearColor BaseColor;
			BaseColor.InitFromString(JsonObj->GetStringField("Color"));
			L->SetBaseColor(BaseColor);
			L->bIsDashLine = JsonObj->GetBoolField("bIsDashLine");
			TableOutline.Add(FLineTransformInfo(L, MakeShareable(new FArmyLine(L.Get()))));
			BoundingBox += StartPoint;
			BoundingBox += EndPoint;
		}
	}

	const TArray< TSharedPtr<FJsonValue> >& JsonTableFrontOutline = InJsonData->GetArrayField("TableFrontOutline");
	for (auto JsonV : JsonTableFrontOutline)
	{
		const TSharedPtr<FJsonObject>& JsonObj = JsonV->AsObject();
		if (JsonObj.IsValid())
		{
			TSharedPtr<FArmyLine> L = MakeShareable(new FArmyLine);
			FVector StartPoint, EndPoint;
			StartPoint.InitFromString(JsonObj->GetStringField("StartPoint"));
			EndPoint.InitFromString(JsonObj->GetStringField("EndPoint"));
			L->SetStart(StartPoint);
			L->SetEnd(EndPoint);
			FLinearColor BaseColor;
			BaseColor.InitFromString(JsonObj->GetStringField("Color"));
			L->SetBaseColor(BaseColor);
			L->bIsDashLine = JsonObj->GetBoolField("bIsDashLine");
			TableFrontOutline.Add(FLineTransformInfo(L, MakeShareable(new FArmyLine(L.Get()))));
			BoundingBox += StartPoint;
			BoundingBox += EndPoint;
		}
	}

	const TArray< TSharedPtr<FJsonValue> >& JsonTableBackOutline = InJsonData->GetArrayField("TableBackOutline");
	for (auto JsonV : JsonTableBackOutline)
	{
		const TSharedPtr<FJsonObject>& JsonObj = JsonV->AsObject();
		if (JsonObj.IsValid())
		{
			TSharedPtr<FArmyLine> L = MakeShareable(new FArmyLine);
			FVector StartPoint, EndPoint;
			StartPoint.InitFromString(JsonObj->GetStringField("StartPoint"));
			EndPoint.InitFromString(JsonObj->GetStringField("EndPoint"));
			L->SetStart(StartPoint);
			L->SetEnd(EndPoint);
			FLinearColor BaseColor;
			BaseColor.InitFromString(JsonObj->GetStringField("Color"));
			L->SetBaseColor(BaseColor);
			L->bIsDashLine = JsonObj->GetBoolField("bIsDashLine");
			TableBackOutline.Add(FLineTransformInfo(L, MakeShareable(new FArmyLine(L.Get()))));
			BoundingBox += StartPoint;
			BoundingBox += EndPoint;
		}
	}

	const TArray< TSharedPtr<FJsonValue> >& JsonOnTabelComponentArray = InJsonData->GetArrayField("OnTabelComponentArray");
	for (auto JsonComponent : JsonOnTabelComponentArray)
	{
		int32 CIndex = OnTabelComponentArray.Add(FComponentInfo());
		FComponentInfo& ComponentInfo = OnTabelComponentArray[CIndex];
		const TSharedPtr<FJsonObject>& JsonObj = JsonComponent->AsObject();
		if (JsonObj.IsValid())
		{
			const TSharedPtr<FJsonObject>& JsonCenterBaseLine = JsonObj->GetObjectField("CenterBaseLine");
			if (JsonCenterBaseLine.IsValid())
			{
				TSharedPtr<FArmyLine> L = MakeShareable(new FArmyLine);
				FVector StartPoint, EndPoint;
				StartPoint.InitFromString(JsonCenterBaseLine->GetStringField("StartPoint"));
				EndPoint.InitFromString(JsonCenterBaseLine->GetStringField("EndPoint"));
				L->SetStart(StartPoint);
				L->SetEnd(EndPoint);
				FLinearColor BaseColor;
				BaseColor.InitFromString(JsonCenterBaseLine->GetStringField("Color"));
				L->SetBaseColor(BaseColor);
				L->bIsDashLine = JsonCenterBaseLine->GetBoolField("bIsDashLine");
				ComponentInfo.CenterLine.BaseLine = L;
			}
			const TSharedPtr<FJsonObject>& JsonCenterDrawLine = JsonObj->GetObjectField("CenterDrawLine");
			if (JsonCenterDrawLine.IsValid())
			{
				TSharedPtr<FArmyLine> L = MakeShareable(new FArmyLine);
				FVector StartPoint, EndPoint;
				StartPoint.InitFromString(JsonCenterDrawLine->GetStringField("StartPoint"));
				EndPoint.InitFromString(JsonCenterDrawLine->GetStringField("EndPoint"));
				L->SetStart(StartPoint);
				L->SetEnd(EndPoint);
				FLinearColor BaseColor;
				BaseColor.InitFromString(JsonCenterDrawLine->GetStringField("Color"));
				L->SetBaseColor(BaseColor);
				L->bIsDashLine = JsonCenterDrawLine->GetBoolField("bIsDashLine");
				ComponentInfo.CenterLine.DrawLine = L;
			}

			const TArray< TSharedPtr<FJsonValue> >& JsonComponentVertexArray = JsonObj->GetArrayField("ComponentVertexArray");
			for (auto JsonV : JsonComponentVertexArray)
			{
				const TSharedPtr<FJsonObject>& JsonVObj = JsonV->AsObject();
				if (JsonVObj.IsValid())
				{
					FSimpleElementVertex VObj;
					VObj.Color.InitFromString(JsonVObj->GetStringField("Color"));
					VObj.HitProxyIdColor.InitFromString(JsonVObj->GetStringField("HitProxyIdColor"));
					VObj.Position.InitFromString(JsonVObj->GetStringField("Position"));
					VObj.TextureCoordinate.InitFromString(JsonVObj->GetStringField("TextureCoordinate"));
					ComponentInfo.BaseArray.Add(VObj);
				}
			}
		}
	}
	Update();
}

// 获得绘制CAD的线段 0：Outline 1：FrontOutline 2：BackOutline 
void FArmyWHCTableObject::GetCadLines(TArray< TSharedPtr<FArmyLine> >& OutLines, int32 LineType)
{
	// 外围线段
	if (LineType == 0)
	{
		for (auto L : TableOutline)
		{
			OutLines.Add(L.DrawLine);
		}
	}
	else if (LineType == 1)
	{
		for (auto L : TableFrontOutline)
		{
			OutLines.Add(L.DrawLine);
		}
	}
	else if (LineType == 2)
	{
		for (auto L : TableBackOutline)
		{
			OutLines.Add(L.DrawLine);
		}
	}
}