#include "ArmyDimensions.h"
#include "CoreMinimal.h"
#include "MaterialShared.h"
#include "ArmyStyle.h"
#include "ArmyMath.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ArmyEngineModule.h"

FArmyDimensions::FArmyDimensions() :CArrowsType(Default), SeriesDir(FVector(0, 0, 0)), CurrentState(-1), AreaType(-1), DimensionValue(0)
{
    SetName(TEXT("标注") + GetUniqueID().ToString());

	//if (LocalViewPortClient)
	//{
	//	DimensionText = LocalViewPortClient->GetWorld()->SpawnActor<ATextRenderActor>(ATextRenderActor::StaticClass(), FVector(0, 0, 0), FRotator(90, 0, -90));
	//	UTextRenderComponent* UserScaleIndicatorText = DimensionText->GetTextRender();
	//	UserScaleIndicatorText->SetText(FText::FromName(""));
	//	UserScaleIndicatorText->SetHorizontalAlignment(EHTA_Center);
	//	UserScaleIndicatorText->bSelectable = false;
	//}
	DimensionCanvasText.SetColor(FLinearColor::White);

	#define BASE_COLOR FLinearColor(FColor(0xFF999999))

	MainLine = MakeShareable(new FArmyLine);
	MainLine->SetLineColor(BASE_COLOR);
	LeftLine = MakeShareable(new FArmyLine);
	LeftLine->SetLineColor(BASE_COLOR);
	RightLine = MakeShareable(new FArmyLine);
	RightLine->SetLineColor(BASE_COLOR);

	InnerOutLines.Add(MainLine);
	InnerOutLines.Add(LeftLine);
	InnerOutLines.Add(RightLine);

	LeftArrows = MakeShareable(new FArmyPolygon);
	RightArrows = MakeShareable(new FArmyPolygon);

	//获取自定义材质
	UMaterial* M_BaseColor = FArmyEngineModule::Get().GetEngineResource()->GetBaseColorMat();
	FColoredMaterialRenderProxy *SolidMaterialInstance;
	if (M_BaseColor)
	{
		SolidMaterialInstance = new FColoredMaterialRenderProxy(M_BaseColor->GetRenderProxy(false), FLinearColor(0.4f, 0.4f, 0.4f), FName("SelectionColor"));
	}
	else
	{
		SolidMaterialInstance = new FColoredMaterialRenderProxy(UMaterial::GetDefaultMaterial(MD_UI)->GetRenderProxy(false), FLinearColor(1,1,1), FName("SelectionColor"));
	}
	LeftArrows->MaterialRenderProxy = SolidMaterialInstance;
	RightArrows->MaterialRenderProxy = SolidMaterialInstance;

	LeftStaticPoint = MakeShareable(new FArmyPoint);
	RightStaticPoint = MakeShareable(new FArmyPoint);

	LeftExtentPoint = MakeShareable(new FArmyPoint);
	RightExtentPoint = MakeShareable(new FArmyPoint);

	TextControlPoint = MakeShareable(new FArmyPoint);

	LeadEditPoint = MakeShareable(new FArmyEditPoint());
	EndEditPoint = MakeShareable(new FArmyEditPoint());
	LeadEditLine = MakeShareable(new FArmyLine);
	LeadEditLine->SetLineColor(BASE_COLOR);
	MainEditLine = MakeShareable(new FArmyLine);
	MainEditLine->SetLineColor(BASE_COLOR);
	BDefaultLeadLine = true;
}


FArmyDimensions::~FArmyDimensions()
{
	//if (DimensionText->IsValidLowLevel())
	//{
	//    DimensionText->Destroy();
	//    DimensionText = NULL;
	//}
}
void FArmyDimensions::DrawHUD(UArmyEditorViewportClient* InViewPortClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		DimensionCanvasText.DrawHUD(InViewPortClient, View, Canvas);
	}
}
void FArmyDimensions::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		//设置标尺的深度值
		ESceneDepthPriorityGroup Depth = ::SDPG_MAX;
		LeftLine->Draw(PDI, View);
		LeftLine->DepthPriority = Depth;
		RightLine->Draw(PDI, View);
		RightLine->DepthPriority = Depth;
		MainLine->Draw(PDI, View);
		MainLine->DepthPriority = Depth;
		LeftArrows->Draw(PDI, View);
		LeftArrows->DepthPriority = Depth;
		RightArrows->Draw(PDI, View);
		RightArrows->DepthPriority = Depth;
		if (NeedLeadLine)
		{
			LeadEditLine->Draw(PDI, View);
			LeadEditLine->DepthPriority = Depth;
			MainEditLine->Draw(PDI, View);
			LeadEditLine->DepthPriority = Depth;
			if (State == OS_Selected)
			{
				/*梁晓菲 绘制*/
				LeadEditPoint->Draw(PDI, View);
				EndEditPoint->Draw(PDI, View);
			}
		}
	}
	//else
	//{
	//	DimensionText->SetActorHiddenInGame(true);
	//}
}
void FArmyDimensions::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
	if (ObjectType == OT_InSideWallDimensions)
	{
		JsonWriter->WriteValue("Type", TEXT("OT_InSideWallDimensions"));
	}
	else if (ObjectType == OT_OutSideWallDimensions)
	{
		JsonWriter->WriteValue("Type", TEXT("OT_OutSideWallDimensions"));
	}
	else if (ObjectType == OT_AddWallDimensions)
	{
		JsonWriter->WriteValue("Type", TEXT("OT_AddWallDimensions"));
	}
	else if (ObjectType == OT_DeleteWallDimensions)
	{
		JsonWriter->WriteValue("Type", TEXT("OT_DeleteWallDimensions"));
	}
	else if (ObjectType == OT_Dimensions)
	{
		JsonWriter->WriteValue("Type", TEXT("OT_Dimensions"));
	}
	JsonWriter->WriteValue("ClassType", ClassType);
	JsonWriter->WriteValue("AreaType", AreaType);
	JsonWriter->WriteValue("arrowType", int32(CArrowsType));
	JsonWriter->WriteValue("ExtentMaxV", ExtentMaxV);
	JsonWriter->WriteValue("TextSize", DimensionCanvasText.GetScaleTextSize());

	JsonWriter->WriteArrayStart("LeftStaticPoint");
	JsonWriter->WriteValue(LeftStaticPoint->Pos.X);
	JsonWriter->WriteValue(LeftStaticPoint->Pos.Y);
	JsonWriter->WriteValue(LeftStaticPoint->Pos.Z);
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteArrayStart("RightStaticPoint");
	JsonWriter->WriteValue(RightStaticPoint->Pos.X);
	JsonWriter->WriteValue(RightStaticPoint->Pos.Y);
	JsonWriter->WriteValue(RightStaticPoint->Pos.Z);
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteArrayStart("LeftExtentPoint");
	JsonWriter->WriteValue(LeftExtentPoint->Pos.X);
	JsonWriter->WriteValue(LeftExtentPoint->Pos.Y);
	JsonWriter->WriteValue(LeftExtentPoint->Pos.Z);
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteArrayStart("RightExtentPoint");
	JsonWriter->WriteValue(RightExtentPoint->Pos.X);
	JsonWriter->WriteValue(RightExtentPoint->Pos.Y);
	JsonWriter->WriteValue(RightExtentPoint->Pos.Z);
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteValue("EndEditPoint", EndEditPoint->GetPos().ToString());
	JsonWriter->WriteValue("LeadEditPoint", LeadEditPoint->GetPos().ToString());
	JsonWriter->WriteValue("ForceUseLeadLine", ForceUseLeadLine);
	JsonWriter->WriteValue("UseLeadLine", UseLeadLine);

	SERIALIZEREGISTERCLASS(JsonWriter, FArmyDimensions)
}

void FArmyDimensions::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);

	if (InJsonData->GetStringField("Type") == TEXT("OT_InSideWallDimensions"))
	{
		ObjectType = OT_InSideWallDimensions;
	}
	else if (InJsonData->GetStringField("Type") == TEXT("OT_OutSideWallDimensions"))
	{
		ObjectType = OT_OutSideWallDimensions;
	}
	else if (InJsonData->GetStringField("Type") == TEXT("OT_AddWallDimensions"))
	{
		ObjectType = OT_AddWallDimensions;
	}
	else if (InJsonData->GetStringField("Type") == TEXT("OT_DeleteWallDimensions"))
	{
		ObjectType = OT_DeleteWallDimensions;
	}
	else if (InJsonData->GetStringField("Type") == TEXT("OT_Dimensions"))
	{
		ObjectType = OT_Dimensions;
	}

	InJsonData->TryGetNumberField("AreaType", AreaType);
	InJsonData->TryGetNumberField("ClassType", ClassType);
	CArrowsType = ArrowsType(InJsonData->GetIntegerField("arrowType"));
	ExtentMaxV = InJsonData->GetNumberField("ExtentMaxV");
	SetTextSize(InJsonData->GetIntegerField("TextSize"));

	int32 VIndex = 0;
	for (auto& PIt : InJsonData->GetArrayField("LeftStaticPoint"))
	{
		if (VIndex == 0)
		{
			LeftStaticPoint->Pos.X = PIt->AsNumber();
		}
		else if (VIndex == 1)
		{
			LeftStaticPoint->Pos.Y = PIt->AsNumber();
		}
		else if (VIndex == 2)
		{
			LeftStaticPoint->Pos.Z = PIt->AsNumber();
		}
		++VIndex;
	}
	VIndex = 0;
	for (auto& PIt : InJsonData->GetArrayField("RightStaticPoint"))
	{
		if (VIndex == 0)
		{
			RightStaticPoint->Pos.X = PIt->AsNumber();
		}
		else if (VIndex == 1)
		{
			RightStaticPoint->Pos.Y = PIt->AsNumber();
		}
		else if (VIndex == 2)
		{
			RightStaticPoint->Pos.Z = PIt->AsNumber();
		}
		++VIndex;
	}
	VIndex = 0;
	for (auto& PIt : InJsonData->GetArrayField("LeftExtentPoint"))
	{
		if (VIndex == 0)
		{
			LeftExtentPoint->Pos.X = PIt->AsNumber();
		}
		else if (VIndex == 1)
		{
			LeftExtentPoint->Pos.Y = PIt->AsNumber();
		}
		else if (VIndex == 2)
		{
			LeftExtentPoint->Pos.Z = PIt->AsNumber();
		}
		++VIndex;
	}
	VIndex = 0;
	for (auto& PIt : InJsonData->GetArrayField("RightExtentPoint"))
	{
		if (VIndex == 0)
		{
			RightExtentPoint->Pos.X = PIt->AsNumber();
		}
		else if (VIndex == 1)
		{
			RightExtentPoint->Pos.Y = PIt->AsNumber();
		}
		else if (VIndex == 2)
		{
			RightExtentPoint->Pos.Z = PIt->AsNumber();
		}
		++VIndex;
	}

	InJsonData->TryGetBoolField("ForceUseLeadLine", ForceUseLeadLine);
	InJsonData->TryGetBoolField("UseLeadLine", UseLeadLine);

	UpdateLines();
	UpdateArrows();

	FString LeadEditPStr(""), EndEditPStr("");
	if (InJsonData->TryGetStringField("LeadEditPoint", LeadEditPStr))
	{
		FVector P;
		P.InitFromString(LeadEditPStr);
		SetLeadPoint(P);
	}
	if (InJsonData->TryGetStringField("EndEditPoint", EndEditPStr))
	{
		FVector P;
		P.InitFromString(EndEditPStr);
		SetEndPoint(P);
	}
	if (ForceUseLeadLine)
	{
		SetForceUseLeadLine();
	}
	else
	{
		UpdateDownLeadLine();
		UpdateText();
	}
}
void FArmyDimensions::SetState(EObjectState InState)
{
	State = InState;
	FLinearColor LinearColor = FLinearColor::White;

	switch (InState)
	{
	case OS_Normal:
	case OS_Disable:
		LinearColor = FLinearColor(BASE_COLOR);
		DimensionCanvasText.SetColor(FLinearColor::White);
		break;

	case OS_Hovered:
	case OS_Selected:
		LinearColor = FLinearColor(FColor(0xFFFF9800));
		DimensionCanvasText.SetColor(FLinearColor(FColor(0xFFEA8D03)));
		break;

	default:
		break;
	}
	if (MainLine.IsValid())
	{
		MainLine->SetLineColor(LinearColor);
	}
	if (LeadEditLine.IsValid())
	{
		LeadEditLine->SetLineColor(LinearColor);
	}
	if (MainEditLine.IsValid())
	{
		MainEditLine->SetLineColor(LinearColor);
	}
}
void FArmyDimensions::SetForceVisible(bool InV)
{
	//if (ForceVisible == InV)
	//{
	//	return;
	//}
	//FArmyObject::SetForceVisible(InV);
	//DimensionText->SetActorHiddenInGame(!InV);
}
bool FArmyDimensions::IsSelected(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		//FBox2D TempBox(ForceInitToZero);
		//TempBox += FVector2D(DimensionCanvasText.GetPosition());
		//TempBox += (FVector2D(DimensionCanvasText.GetPosition()) + DimensionCanvasText.DrawnSize);

		//bool PickText = TempBox.IsInside(FVector2D(Pos));

		if (MainLine->IsSelected(Pos, InViewportClient) || LeftLine->IsSelected(Pos, InViewportClient) || RightLine->IsSelected(Pos, InViewportClient)
			 || LeadEditLine->IsSelected(Pos, InViewportClient) || MainEditLine->IsSelected(Pos, InViewportClient)/*|| PickText*/)
		{
			return true;
		}
	}
	return  false;
}

bool FArmyDimensions::Hover(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (MainLine->IsSelected(Pos, InViewportClient)  ||
			LeftLine->IsSelected(Pos, InViewportClient)  || 
			RightLine->IsSelected(Pos, InViewportClient) ||
			LeadEditLine->IsSelected(Pos,InViewportClient)||
			MainEditLine->IsSelected(Pos, InViewportClient))
		{
			return true;
		}
	}
	return  false;
}

void FArmyDimensions::GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs)
{
	if (CurrentState == 0)
	{
		OutLines = InnerOutLines;
	}
}

void FArmyDimensions::ApplyTransform(const FTransform& Trans)
{
	if (RightStaticPoint.IsValid())
	{
		RightStaticPoint->Pos = Trans.TransformPosition(RightStaticPoint->Pos);
	}
	if (RightExtentPoint.IsValid())
	{
		RightExtentPoint->Pos = Trans.TransformPosition(RightExtentPoint->Pos);
	}
	if (LeftStaticPoint.IsValid())
	{
		LeftStaticPoint->Pos = Trans.TransformPosition(LeftStaticPoint->Pos);
	}
	if (LeftExtentPoint.IsValid())
	{
		LeftExtentPoint->Pos = Trans.TransformPosition(LeftExtentPoint->Pos);
	}
	if (LeadEditLine.IsValid())
	{
		LeadEditLine->SetStart(Trans.TransformPosition(LeadEditLine->GetStart()));
		LeadEditLine->SetEnd(Trans.TransformPosition(LeadEditLine->GetEnd()));
	}
	if (MainEditLine.IsValid())
	{
		MainEditLine->SetStart(Trans.TransformPosition(MainEditLine->GetStart()));
		MainEditLine->SetEnd(Trans.TransformPosition(MainEditLine->GetEnd()));
	}
	if (LeadEditPoint.IsValid())
	{
		LeadEditPoint->SetPos(Trans.TransformPosition(LeadEditPoint->GetPos()));
	}
	if (EndEditPoint.IsValid())
	{
		EndEditPoint->SetPos(Trans.TransformPosition(EndEditPoint->GetPos()));
	}

	UpdateLines();
	UpdateArrows();
	UpdateDownLeadLine();
	UpdateText();
}

const FBox FArmyDimensions::GetBounds()
{
	//return FBox(ForceInitToZero);
	return FBox(TArray<FVector>{LeftLine->GetStart(), LeftLine->GetEnd(), RightLine->GetEnd(), RightLine->GetStart()});
}
void FArmyDimensions::UpdateMouse(FVector InMouseWorldPos)
{
	switch (CurrentState)
	{
	case 1:
	{
		AreaType = 0;

		RightStaticPoint->Pos = InMouseWorldPos;
		RightExtentPoint->Pos = InMouseWorldPos;

		UpdateLines();
		UpdateArrows();
		UpdateDownLeadLine();
		UpdateText();
	}
	break;
	case 2:
	{
		if (LeftStaticPoint->Pos.X == RightStaticPoint->Pos.X)
		{
			//1 x dir
			AreaType = 2;

			LeftExtentPoint->Pos = LeftStaticPoint->Pos;
			LeftExtentPoint->Pos.X = InMouseWorldPos.X;

			RightExtentPoint->Pos = RightStaticPoint->Pos;
			RightExtentPoint->Pos.X = InMouseWorldPos.X;
		}
		else if (LeftStaticPoint->Pos.Y == RightStaticPoint->Pos.Y)
		{
			//1 y dir
			AreaType = 1;

			LeftExtentPoint->Pos = LeftStaticPoint->Pos;
			LeftExtentPoint->Pos.Y = InMouseWorldPos.Y;

			RightExtentPoint->Pos = RightStaticPoint->Pos;
			RightExtentPoint->Pos.Y = InMouseWorldPos.Y;
		}
		else
		{
			//3
			//main dir area
			float XDis = FMath::Abs(LeftStaticPoint->Pos.X - RightStaticPoint->Pos.X);
			float YDis = FMath::Abs(LeftStaticPoint->Pos.Y - RightStaticPoint->Pos.Y);
			FVector BaseLineDir = LeftStaticPoint->Pos - RightStaticPoint->Pos;
			FVector LinePoint = FMath::ClosestPointOnLine(LeftStaticPoint->Pos, RightStaticPoint->Pos, InMouseWorldPos);
			if ((LinePoint - LeftStaticPoint->Pos).Size() < BaseLineDir.Size() && (LinePoint - RightStaticPoint->Pos).Size() < BaseLineDir.Size())
			{
				//in
				AreaType = 0;
				FVector MainDir = InMouseWorldPos - LinePoint;
				LeftExtentPoint->Pos = LeftStaticPoint->Pos + MainDir;
				RightExtentPoint->Pos = RightStaticPoint->Pos + MainDir;
			}
			else if (FMath::Abs(InMouseWorldPos.X - LeftStaticPoint->Pos.X) < XDis && FMath::Abs(InMouseWorldPos.X - RightStaticPoint->Pos.X) < XDis)
			{
				//y dir
				AreaType = 1;

				LeftExtentPoint->Pos = LeftStaticPoint->Pos;
				LeftExtentPoint->Pos.Y = InMouseWorldPos.Y;

				RightExtentPoint->Pos = RightStaticPoint->Pos;
				RightExtentPoint->Pos.Y = InMouseWorldPos.Y;
			}
			else if (FMath::Abs(InMouseWorldPos.Y - LeftStaticPoint->Pos.Y) < YDis && FMath::Abs(InMouseWorldPos.Y - RightStaticPoint->Pos.Y) < YDis)
			{
				//x dir
				AreaType = 2;

				LeftExtentPoint->Pos = LeftStaticPoint->Pos;
				LeftExtentPoint->Pos.X = InMouseWorldPos.X;

				RightExtentPoint->Pos = RightStaticPoint->Pos;
				RightExtentPoint->Pos.X = InMouseWorldPos.X;
			}
			else
			{
				FVector MainLinePoint = FMath::ClosestPointOnInfiniteLine(LeftStaticPoint->Pos, RightStaticPoint->Pos, InMouseWorldPos);
				FVector XLinePoint = FMath::ClosestPointOnInfiniteLine(LeftStaticPoint->Pos, LeftStaticPoint->Pos + FVector(100, 0, 0), InMouseWorldPos);
				FVector YLinePoint = FMath::ClosestPointOnInfiniteLine(LeftStaticPoint->Pos, LeftStaticPoint->Pos + FVector(0, 100, 0), InMouseWorldPos);

				//main dir
				AreaType = 0;
				float MainDis1 = (MainLinePoint - LeftStaticPoint->Pos).Size();
				float MainDis2 = (MainLinePoint - RightStaticPoint->Pos).Size();

				float MinV = FMath::Min(MainDis1, MainDis2);

				float YDirDis1 = FMath::Abs(XLinePoint.X - LeftStaticPoint->Pos.X);
				float YDirDis2 = FMath::Abs(XLinePoint.X - RightStaticPoint->Pos.X);
				if (FMath::Min(YDirDis1, YDirDis2) < MinV)
				{
					AreaType = 1;
					//y dir
				}

				float XDirDis1 = FMath::Abs(XLinePoint.Y - LeftStaticPoint->Pos.Y);
				float XDirDis2 = FMath::Abs(XLinePoint.Y - RightStaticPoint->Pos.Y);
				if (FMath::Min(XDirDis1, XDirDis2) < MinV)
				{
					AreaType = 2;
					//x dir
				}

				if (AreaType == 0)
				{
					FVector MainDir = InMouseWorldPos - MainLinePoint;
					LeftExtentPoint->Pos = LeftStaticPoint->Pos + MainDir;
					RightExtentPoint->Pos = RightStaticPoint->Pos + MainDir;
				}
				else if (AreaType == 1)
				{
					LeftExtentPoint->Pos = LeftStaticPoint->Pos;
					LeftExtentPoint->Pos.Y = InMouseWorldPos.Y;

					RightExtentPoint->Pos = RightStaticPoint->Pos;
					RightExtentPoint->Pos.Y = InMouseWorldPos.Y;
				}
				else if (AreaType == 2)
				{
					LeftExtentPoint->Pos = LeftStaticPoint->Pos;
					LeftExtentPoint->Pos.X = InMouseWorldPos.X;

					RightExtentPoint->Pos = RightStaticPoint->Pos;
					RightExtentPoint->Pos.X = InMouseWorldPos.X;
				}
			}
		}
		UpdateLines();
		UpdateArrows();
		UpdateDownLeadLine();
		UpdateText();
	}
	break;
	case 3:
	{
		FVector NearPoint = FMath::ClosestPointOnInfiniteLine(LeftExtentPoint->Pos, LeftExtentPoint->Pos + SeriesDir * 10, InMouseWorldPos);

		RightExtentPoint->Pos = NearPoint;
		RightStaticPoint->Pos = NearPoint + LeftStaticPoint->Pos - LeftExtentPoint->Pos;

		UpdateLines();
		UpdateArrows();
		UpdateDownLeadLine();
		UpdateText();
	}
	break;
	default:
		break;
	}
}
void FArmyDimensions::UpdateArrows()
{
	switch (CArrowsType)
	{
	case FArmyDimensions::Default:
	{
		FVector MainLineDir = MainLine->GetEnd() - MainLine->GetStart();
		FRotator TempRot(0, 0, 0);

		if (MainLineDir.Size() > 0)
		{
			MainLineDir.Normalize();
			float Rad = MainLineDir | (FVector(1, 0, 0));
			int32 flag = Rad < 0 ? -1 : 1;
			if (Rad < 0 && MainLine->GetEnd().Y > MainLine->GetStart().Y)
			{
				flag = 1;
			}
			else if (Rad > 0 && MainLine->GetEnd().Y < MainLine->GetStart().Y)
			{
				flag = -1;
			}
			if (Rad == 0)
			{
				MainLineDir.Y > 0 ? flag = 1 : flag = -1;
			}
			Rad = FMath::Acos(Rad) * flag;
			float Deg = Rad * 180 / PI;
			TempRot.Yaw = Deg;
		}

		TArray<FVector> Vertexes;
		FVector p1(-1, -0.17, 0);
		FVector p2(-1, 0.17, 0);
		FVector p3(0, 0, 0);
		FTransform TransLeft(FRotator(0, 180, 0) + TempRot, FVector(0, 0, 0) + MainLine->GetStart(), FVector(5, 5, 1));

		Vertexes.Add(TransLeft.TransformPosition(p1));
		Vertexes.Add(TransLeft.TransformPosition(p2));
		Vertexes.Add(TransLeft.TransformPosition(p3));
		LeftArrows->SetVertices(Vertexes);

		Vertexes.Empty();
		FTransform TransRight(FRotator(0, 0, 0) + TempRot, FVector(0, 0, 0) + MainLine->GetEnd(), FVector(5, 5, 1));

		Vertexes.Add(TransRight.TransformPosition(p1));
		Vertexes.Add(TransRight.TransformPosition(p2));
		Vertexes.Add(TransRight.TransformPosition(p3));
		RightArrows->SetVertices(Vertexes);
		RightArrows->FillColor = FLinearColor(1, 1, 1, 1);
	}
	break;
	case FArmyDimensions::Diagonal:
	{
		FVector MainLineDir = MainLine->GetEnd() - MainLine->GetStart();
		FRotator TempRot(0, 0, 0);

		if (MainLineDir.Size() > 0)
		{
			MainLineDir.Normalize();
			float Rad = MainLineDir | (FVector(1, 0, 0));
			int32 flag = Rad < 0 ? -1 : 1;
			if (Rad < 0 && MainLine->GetEnd().Y > MainLine->GetStart().Y)
			{
				flag = 1;
			}
			else if (Rad > 0 && MainLine->GetEnd().Y < MainLine->GetStart().Y)
			{
				flag = -1;
			}
			if (Rad == 0)
			{
				MainLineDir.Y > 0 ? flag = 1 : flag = -1;
			}
			Rad = FMath::Acos(Rad) * flag;
			float Deg = Rad * 180 / PI;
			TempRot.Yaw = Deg;
		}

		TArray<FVector> Vertexes;
		FVector p1(0.7, 0.08, 0);
		FVector p2(0.7, -0.08, 0);
		FVector p3(-0.7, -0.08, 0);
		FVector p4(-0.7, 0.08, 0);
		FTransform TransLeft(FRotator(0, 45, 0) + TempRot, FVector(0, 0, 0) + MainLine->GetStart(), FVector(5, 5, 1));

		Vertexes.Add(TransLeft.TransformPosition(p1));
		Vertexes.Add(TransLeft.TransformPosition(p2));
		Vertexes.Add(TransLeft.TransformPosition(p3));
		Vertexes.Add(TransLeft.TransformPosition(p4));
		LeftArrows->SetVertices(Vertexes);

		Vertexes.Empty();
		FTransform TransRight(FRotator(0, 45, 0) + TempRot, FVector(0, 0, 0) + MainLine->GetEnd(), FVector(5, 5, 1));

		Vertexes.Add(TransRight.TransformPosition(p1));
		Vertexes.Add(TransRight.TransformPosition(p2));
		Vertexes.Add(TransRight.TransformPosition(p3));
		Vertexes.Add(TransRight.TransformPosition(p4));
		RightArrows->SetVertices(Vertexes);
		RightArrows->FillColor = FLinearColor(1, 1, 1, 1);
	}
	break;
	default:
		break;
	}
}
void FArmyDimensions::UpdateText()
{
	//UTextRenderComponent* UserScaleIndicatorText = DimensionText->GetTextRender();

	FString TextValue = FString::FromInt(FMath::RoundToInt(DimensionValue * 10));
	DimensionCanvasText.Text = FText::FromString(TextValue);

	int32 TextLenth = DimensionCanvasText.GetScaleTextSize() * TextValue.Len();
	
	FRotator BaseRot(0, 0, 0);
	FVector MainLineDir = MainLine->GetEnd() - MainLine->GetStart();
	FVector RightDir = FRotator(0, -90, 0).RotateVector(MainLineDir.GetSafeNormal());

#ifdef _FREEDIR_ //along mainline dir free
	if (MainLineDir.Size() > 0)
	{
		MainLineDir.Normalize();
		float Rad = MainLineDir | (FVector(1, 0, 0));
		int32 flag = Rad < 0 ? 1 : -1;
		if (Rad < 0 && MainLine->GetEnd().Y > MainLine->GetStart().Y)
		{
			flag = -1;
		}
		else if (Rad > 0 && MainLine->GetEnd().Y < MainLine->GetStart().Y)
		{
			flag = 1;
		}
		if (Rad == 0)
		{
			MainLineDir.Y > 0 ? flag = -1 : flag = 1;
		}
		Rad = FMath::Acos(Rad) * flag;
		float Deg = Rad * 180 / PI;
		BaseRot.Yaw += Deg;
	}
	FVector OffSetDir = LeftExtentPoint->Pos - LeftStaticPoint->Pos;

	if (OffSetDir.Size() < 0.0001)
	{
		OffSetDir = RightDir;
	}

#else
	FVector OffSetDir(0,-1,0);
	if (MainLineDir.Size() > 0)
	{
		MainLineDir.Normalize();
		float Rad = MainLineDir | (FVector(1, 0, 0));
		int32 flag = MainLineDir.Y < 0 ? 1 : -1;

		Rad = FMath::Acos(Rad) * flag;
		float Deg = Rad * 180 / PI;

		int32 OffSetFlag = 1;
		if (MainLineDir.X < 0 ||(MainLineDir.X == 0 && MainLineDir.Y > 0))
		{
			Deg = 180 + Deg;
			OffSetFlag = -1;
		}

		BaseRot.Yaw += Deg;

		OffSetDir = (MainLineDir^FVector(0, 0, 1)) * OffSetFlag;
	}
#endif

	if ((ForceUseLeadLine || (UseLeadLine && DimensionValue <= (TextLenth / 2))))
	{
		NeedLeadLine = true;
		if (BDefaultLeadLine)
		{
			float leadAngle = 15;
			if ((FRotator(0, -90, 0).RotateVector(MainLineDir).GetSafeNormal() + OffSetDir.GetSafeNormal()).Size() < 0.0001)
			{
				leadAngle = -15;
			}

			FVector LeadDir = FRotator(0, leadAngle, 0).RotateVector(OffSetDir);

			float Offset = DimensionCanvasText.GetScaleTextSize() + 5;

			FVector TextPos = (leadAngle < 0 ? TextControlPoint->Pos + LeadDir.GetSafeNormal() * 20 + MainLineDir.GetSafeNormal() * (TextLenth - 10) : TextControlPoint->Pos + LeadDir.GetSafeNormal() * 20) + MainLineDir.GetSafeNormal() + OffSetDir.GetSafeNormal() * Offset;
			DimensionCanvasText.SetPosition(TextPos);
			DimensionCanvasText.SetRotator(BaseRot);
		}
		else
		{
			FVector FontDir = (BaseRot * -1).RotateVector(FVector(1, 0, 0));
			FVector UpDir = FVector(FontDir.Y, -FontDir.X, 0);

			if (MainLineDir.Size() == 0)
			{
				FontDir = FVector(0, 0, 0);
			}

			float Offset = DimensionCanvasText.GetScaleTextSize() + 5;

			FVector MainLineDir = (EndEditPoint->GetPos() - LeadEditPoint->GetPos()).GetSafeNormal();

			if ((FontDir + MainLineDir).Size() > 1)
			{
				DimensionCanvasText.SetPosition(EndEditPoint->GetPos() + UpDir * Offset - FontDir * (TextLenth - 10));
			}
			else
			{
				DimensionCanvasText.SetPosition(EndEditPoint->GetPos() + UpDir * Offset);
			}
			DimensionCanvasText.SetRotator(BaseRot);
		}
	}
	else
	{
		NeedLeadLine = false;

		float Offset = DimensionCanvasText.GetScaleTextSize() + 5;
#ifdef _FREEDIR_ //along mainline dir free
		if ((OffSetDir.GetSafeNormal() + RightDir.GetSafeNormal()).Size() < 0.0001)
		{
			Offset = 10;
		}
#endif
		FVector FontDir = (BaseRot * -1).RotateVector(FVector(1,0,0));
		if (MainLineDir.Size() > 0)
		{
			if ((MainLineDir + FontDir).Size() > 1)
			{
				FontDir = -MainLineDir;
			}
			else
			{
				FontDir = MainLineDir;
			}
		}
		else
		{
			FontDir = FVector(0,0,0);
		}
		FVector TextPos = TextControlPoint->Pos + OffSetDir.GetSafeNormal() * Offset + FontDir * (TextLenth /4);
		DimensionCanvasText.SetPosition(TextPos);
		DimensionCanvasText.SetRotator(BaseRot);
	}
	//DimensionText->SetActorLocationAndRotation(TextPos,BaseRot);
}
void FArmyDimensions::UpdateLines()
{
	FVector ExtentDir = LeftExtentPoint->Pos - LeftStaticPoint->Pos;
	FVector LeftBasePos = LeftStaticPoint->Pos;
	FVector RightBasePos = RightStaticPoint->Pos;

	if ((RightExtentPoint->Pos - RightStaticPoint->Pos).Size() > ExtentMaxV)
	{
		RightBasePos = RightExtentPoint->Pos - ExtentDir.GetSafeNormal() * ExtentMaxV;
	}
	if (ExtentDir.Size() > ExtentMaxV)
	{
		LeftBasePos = LeftExtentPoint->Pos - ExtentDir.GetSafeNormal() * ExtentMaxV;
	}
	if (ExtentDir.Size() > 0)
	{
		ExtentDir.Normalize();
		ExtentDir *= 5;
	}
	else
	{
		ExtentDir = FVector(0, 0, 0);
	}

	LeftLine->SetStart(LeftBasePos);
	LeftLine->SetEnd(LeftExtentPoint->Pos + ExtentDir);

	RightLine->SetStart(RightBasePos);
	RightLine->SetEnd(RightExtentPoint->Pos + ExtentDir);

	MainLine->SetStart(LeftExtentPoint->Pos);
	MainLine->SetEnd(RightExtentPoint->Pos);

	TextControlPoint->Pos = (MainLine->GetEnd() + MainLine->GetStart()) / 2;

	if (AreaType == 1)
	{
		DimensionValue = FMath::Abs(LeftStaticPoint->Pos.X - RightStaticPoint->Pos.X);
	}
	else if (AreaType == 2)
	{
		DimensionValue = FMath::Abs(LeftStaticPoint->Pos.Y - RightStaticPoint->Pos.Y);
	}
	else
	{
		DimensionValue = (LeftStaticPoint->Pos - RightStaticPoint->Pos).Size();
	}
}

void FArmyDimensions::CreateText()
{
	//  if (!DimensionText)
	//  {
	//      DimensionText = LocalViewPortClient->GetWorld()->SpawnActor<ATextRenderActor>(ATextRenderActor::StaticClass(), FVector(0, 0, 0), FRotator(90, 0, -90));
	//      UTextRenderComponent* UserScaleIndicatorText = DimensionText->GetTextRender();
	//      UserScaleIndicatorText->SetText(FText::FromName(""));
		  //UserScaleIndicatorText->bSelectable = false;
	//      UpdateText();
	//  }
}

void FArmyDimensions::DeleteText()
{
	//DimensionText->Destroy();
	//DimensionText = NULL;
}

void FArmyDimensions::SetForceUseLeadLine()
{
	ForceUseLeadLine = true;
	BDefaultLeadLine = false;
	UpdateDownLeadLine();
	UpdateText();
}

void FArmyDimensions::StopUseLeadLine()
{
	UseLeadLine = false;
	ForceUseLeadLine = false;
	NeedLeadLine = false;
	UpdateDownLeadLine();
	UpdateText();
}

void FArmyDimensions::SetDefaulePos()
{
	FVector MainLineDir = MainLine->GetEnd() - MainLine->GetStart();

	int32 OffSetFlag = 1;
	if (MainLineDir.X < 0 || (MainLineDir.X == 0 && MainLineDir.Y > 0))
	{
		OffSetFlag = -1;
	}
	FVector OffSetDir(0, -1, 0);
	OffSetDir = (MainLineDir^FVector(0, 0, 1)) * OffSetFlag;

	float leadAngle = 15;
	if ((FRotator(0, -90, 0).RotateVector(MainLineDir).GetSafeNormal() + OffSetDir.GetSafeNormal()).Size() < 0.0001)
	{
		leadAngle = -15;
	}
	FVector LeadDir = FRotator(0, leadAngle, 0).RotateVector(OffSetDir);

	LeadEditPoint->SetPos(TextControlPoint->Pos + LeadDir.GetSafeNormal() * 20);
	EndEditPoint->SetPos(LeadEditPoint->GetPos() + MainLineDir.GetSafeNormal() * 20);
}

void FArmyDimensions::UpdateDownLeadLine()
{
	if ((ForceUseLeadLine || (UseLeadLine && DimensionValue <= 30)))
	{
		NeedLeadLine = true;
		/* 更新引线 */
		if (BDefaultLeadLine || !LeadEditLine.IsValid())
		{
			SetDefaulePos();
			LeadEditLine->SetStart(TextControlPoint->Pos);
			LeadEditLine->SetEnd(LeadEditPoint->GetPos());
		}
		/* @梁晓菲 如果不是默认状态，位置跟随鼠标*/
		else
		{
			if (LeadEditLine->GetEnd() != LeadEditPoint->GetPos())
			{
				SetLeadPoint(LeadEditPoint->GetPos());
			}
		}

		if (BDefaultLeadLine || !MainEditLine.IsValid())
		{
			MainEditLine->SetStart(LeadEditLine->GetEnd());
			MainEditLine->SetEnd(EndEditPoint->GetPos());
		}
		else
		{
			if (MainEditLine->GetEnd() != EndEditPoint->GetPos())
			{
				SetEndPoint(EndEditPoint->GetPos());
			}
		}
	}
	/* @梁晓菲 默认位置*/
	else
	{
		SetDefaulePos();

		LeadEditLine->SetStart(TextControlPoint->Pos);
		LeadEditLine->SetEnd(LeadEditPoint->GetPos());

		MainEditLine->SetStart(LeadEditLine->GetEnd());
		MainEditLine->SetEnd(EndEditPoint->GetPos());
	}
}

TSharedPtr<FArmyEditPoint> FArmyDimensions::HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (GetState() == OS_Selected)
		{
			if (LeadEditPoint->IsSelected(Pos, InViewportClient))
				return LeadEditPoint;
			else if (EndEditPoint->IsSelected(Pos, InViewportClient))
				return EndEditPoint;
		}
	}
	return NULL;
}

TSharedPtr<FArmyEditPoint> FArmyDimensions::SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (GetState() == OS_Selected)
		{
			if (LeadEditPoint->IsSelected(Pos, InViewportClient))
				return LeadEditPoint;
			else if (EndEditPoint->IsSelected(Pos, InViewportClient))
				return EndEditPoint;
		}
	}
	return NULL;
}

void FArmyDimensions::Refresh()
{
	if (GetState() == OS_Selected)
	{
		UpdateDownLeadLine();
		UpdateText();
	}
}

void FArmyDimensions::SetLeadPoint(const FVector& InPos)
{
	if (LeadEditLine.IsValid())
	{
		if (LeadEditLine->GetEnd() == InPos)
		{
			return;
		}
		LeadEditLine->SetStart(TextControlPoint->Pos);
		LeadEditLine->SetEnd(InPos);
		LeadEditPoint->SetPos(InPos);
		if (MainEditLine.IsValid())
		{
			FVector LineOffSet = MainEditLine->GetEnd() - MainEditLine->GetStart();
			MainEditLine->SetStart(InPos);
			MainEditLine->SetEnd(InPos + LineOffSet);
			EndEditPoint->SetPos(MainEditLine->GetEnd());
		}
		else
		{
			MainEditLine = MakeShareable(new FArmyLine);
			MainEditLine->SetStart(InPos);
			MainEditLine->SetEnd(InPos);
			EndEditPoint->SetPos(InPos);
		}
	}
}

void FArmyDimensions::SetEndPoint(const FVector& InPos)
{
	if (LeadEditLine.IsValid() && MainEditLine.IsValid())
	{
		if (MainEditLine->GetEnd() == InPos)
		{
			return;
		}
		
		FVector DirOffset = MainLine->GetEnd() - MainLine->GetStart();

		FVector OnLinePos = FMath::ClosestPointOnInfiniteLine(MainEditLine->GetStart(), MainEditLine->GetStart() + DirOffset, InPos);

		MainEditLine->SetEnd(OnLinePos);
		EndEditPoint->SetPos(OnLinePos);
	}
}

void FArmyDimensions::SetBaseColor(const FLinearColor& InColor)
{
	BaseColor = InColor;

	DimensionCanvasText.SetColor(BaseColor);

	MainLine->SetBaseColor(BaseColor);
	LeftLine->SetBaseColor(BaseColor);
	RightLine->SetBaseColor(BaseColor);

	/* @梁晓菲 颜色*/
	LeadEditLine->SetBaseColor(BaseColor);
	MainEditLine->SetBaseColor(BaseColor);

	auto SolidMaterialInstance = new FColoredMaterialRenderProxy(UMaterial::GetDefaultMaterial(MD_Surface)->GetRenderProxy(false), BaseColor, FName("SelectionColor"));

	LeftArrows->MaterialRenderProxy = SolidMaterialInstance;
	RightArrows->MaterialRenderProxy = SolidMaterialInstance;
}
const FLinearColor& FArmyDimensions::GetBaseColor() const
{
	return BaseColor;
}
void FArmyDimensions::SetTextSize(int32 InSize)
{
	DimensionCanvasText.SetTextSize(InSize);
	UpdateText();
}
int32 FArmyDimensions::GetTextSize()
{
	return DimensionCanvasText.GetScaleTextSize();
}
void FArmyDimensions::SetArrowsType(ArrowsType InType)
{
	if (CArrowsType != InType)
	{
		CArrowsType = InType;
		UpdateArrows();
	}
}
void FArmyDimensions::SetBaseStart(FVector InStartPoint)
{
	if (CurrentState == -1)
	{
		CurrentState = 1;
	}

	LeftStaticPoint->Pos = InStartPoint;
	LeftExtentPoint->Pos = InStartPoint;
	UpdateLines();
	UpdateArrows();
	UpdateDownLeadLine();
	UpdateText();
}
void FArmyDimensions::SetBaseEnd(FVector InEndPoint)
{
	if (CurrentState == -1)
	{
		return;
	}
	else if (CurrentState == 1)
	{
		CurrentState = 2;
	}

	RightStaticPoint->Pos = InEndPoint;
	RightExtentPoint->Pos = InEndPoint;
	UpdateLines();
	UpdateArrows();
	UpdateDownLeadLine();
	UpdateText();
}
void FArmyDimensions::SetDimensionsStart(FVector InStartPoint)
{
	if (CurrentState == -1 || CurrentState == 1)
	{
		return;
	}
	else if (CurrentState == 2)
	{
		CurrentState = 0;
	}

	LeftExtentPoint->Pos = InStartPoint;
	UpdateLines();
	UpdateArrows();
	UpdateDownLeadLine();
	UpdateText();
}
void FArmyDimensions::SetDimensionsEnd(FVector InEndPoint)
{
	if (CurrentState == -1 || CurrentState == 1)
	{
		return;
	}
	else if (CurrentState == 2)
	{
		CurrentState = 0;
	}

	RightExtentPoint->Pos = InEndPoint;
	UpdateLines();
	UpdateArrows();
	UpdateDownLeadLine();
	UpdateText();
}
void FArmyDimensions::SetSeries(FVector InStartPoint, FVector InExtentPoint, FVector InDir)
{
	SetBaseStart(InStartPoint);
	SetBaseEnd(InStartPoint);
	SetDimensionsStart(InExtentPoint);
	SetDimensionsEnd(InExtentPoint);
	SeriesDir = InDir;
	SeriesDir.Normalize();
	CurrentState = 3;
}