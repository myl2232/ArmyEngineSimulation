#include "ArmyObjAutoRuler.h"
#include "ArmyMath.h"
#include "ArmyStyle.h"
#include "ArmyEditorViewportClient.h"

FArmyObjAutoRuler::FArmyObjAutoRuler()
{
	BindObject = NULL;

	ObjectType = OT_ObjectRuler;

	for (int32 T = OT_TextureEditorBegin; T < OT_TextureEditorEnd;T++)
	{
		IgoneObjectTypeArray.AddUnique(EObjectType(T));
	}
	IgoneObjectTypeArray.AddUnique(OT_OutRoom);

    SetName(TEXT("¼ä¾à±ê×¢") + GetUniqueID().ToString());

	XFrontDimensionText.SetColor(FLinearColor::White);
	XBackDimensionText.SetColor(FLinearColor::White);
	YFrontDimensionText.SetColor(FLinearColor::White);
	YBackDimensionText.SetColor(FLinearColor::White);
	WidthDimensionText.SetColor(FLinearColor::White);
	LengthDimensionText.SetColor(FLinearColor::White);

	YFrontDimensionText.SetRotator(FRotator(0,90,0));
	YBackDimensionText.SetRotator(FRotator(0, 90, 0));
	LengthDimensionText.SetRotator(FRotator(0, 90, 0));

	LineVisible = XFRONT| XBACK| YFRONT| YBACK | WIDTH | LENGTH;

	XFrontLine = MakeShareable(new FArmyLine);
	XBackLine = MakeShareable(new FArmyLine);
	YFrontLine = MakeShareable(new FArmyLine);
	YBackLine = MakeShareable(new FArmyLine);

	WidthLine = MakeShareable(new FArmyLine);
	LengthLine = MakeShareable(new FArmyLine);
	WidthLine->DepthPriority = SDPG_Foreground;
	LengthLine->DepthPriority = SDPG_Foreground;

	XFrontLine->SetLineColor(FLinearColor(1, 1, 1, 1));
	XFrontLine->bIsDashLine = false;
	XFrontLine->DepthPriority = SDPG_Foreground;
	//XFrontLine->SetUseTrianlge();
	XBackLine->SetLineColor(FLinearColor(1, 1, 1, 1));
	XBackLine->bIsDashLine = false;
	XBackLine->DepthPriority = SDPG_Foreground;
	//XBackLine->SetUseTrianlge();

	YFrontLine->SetLineColor(FLinearColor(1, 1, 1, 1));
	YFrontLine->bIsDashLine = false;
	YFrontLine->DepthPriority = SDPG_Foreground;
	//YFrontLine->SetUseTrianlge();
	YBackLine->SetLineColor(FLinearColor(1, 1, 1, 1));
	YBackLine->bIsDashLine = false;
	YBackLine->DepthPriority = SDPG_Foreground;
	//YBackLine->SetUseTrianlge();

	/*@ ÁºÏþ·Æ ³õÊ¼»¯ÖÕÖ¹·ûºÅ*/
	XFrontStartArrows = MakeShareable(new FArmyPolygon);
	XFrontEndArrows = MakeShareable(new FArmyPolygon);
	XBackStartArrows = MakeShareable(new FArmyPolygon);
	XBackEndArrows = MakeShareable(new FArmyPolygon);
	YFrontStartArrows = MakeShareable(new FArmyPolygon);
	YFrontEndArrows = MakeShareable(new FArmyPolygon);
	YBackStartArrows = MakeShareable(new FArmyPolygon);
	YBackEndArrows = MakeShareable(new FArmyPolygon);
	auto SolidMaterialInstance = new FColoredMaterialRenderProxy(UMaterial::GetDefaultMaterial(MD_Surface)->GetRenderProxy(false), FLinearColor(1, 1, 1), FName("SelectionColor"));
	XFrontStartArrows->MaterialRenderProxy = SolidMaterialInstance;
	XFrontEndArrows->MaterialRenderProxy = SolidMaterialInstance;
	XBackStartArrows->MaterialRenderProxy = SolidMaterialInstance;
	XBackEndArrows->MaterialRenderProxy = SolidMaterialInstance;
	YFrontStartArrows->MaterialRenderProxy = SolidMaterialInstance;
	YFrontEndArrows->MaterialRenderProxy = SolidMaterialInstance;
	YBackStartArrows->MaterialRenderProxy = SolidMaterialInstance;
	YBackEndArrows->MaterialRenderProxy = SolidMaterialInstance;

	SetState(OS_Normal);
}
void FArmyObjAutoRuler::Update(TArray<FObjectPtr>& InObjects)
{
	if (!BindObject.IsValid()) return;
	if (!GetPropertyFlag(FArmyObject::FLAG_VISIBILITY)) return;

	FVector Trans(ForceInitToZero),Scale(1,1,1);
	FRotator Rot(ForceInitToZero);
	BindObject->GetTransForm(Rot,Trans,Scale);
	FTransform ObjTransFrom(Rot,Trans,Scale);

	const FBox PreBox = BindObject->GetPreBounds();
	FVector LastSize = PreBox.GetSize() * Scale;
	
	FVector WidthStart((PreBox.Min + FVector(0, 10, 0)));
	FVector WidthEnd(PreBox.Max.X, WidthStart.Y, 0);

	FVector LengthStart((PreBox.Min + FVector(10, 0, 0)));
	FVector LengthEnd(LengthStart.X, PreBox.Max.Y, 0);
	if (LastSize.Y < 20)
	{
		WidthStart = FVector(PreBox.Min + FVector(0, PreBox.GetSize().Y / 2, 0));
		WidthEnd = FVector(PreBox.Max.X, WidthStart.Y, 0);
	}
	if (LastSize.X < 20)
	{
		LengthStart = FVector((PreBox.Min + FVector(PreBox.GetSize().X / 2, 0, 0)));
		LengthEnd = FVector(LengthStart.X, PreBox.Max.Y, 0);
	}
	WidthStart = BindObject->TranfromVertex(WidthStart);
	WidthEnd = BindObject->TranfromVertex(WidthEnd);

	LengthStart = BindObject->TranfromVertex(LengthStart);
	LengthEnd = BindObject->TranfromVertex(LengthEnd);

	FBox SelfBox = BindObject->GetBounds();
	FVector SelfCenter = SelfBox.GetCenter();

	if (!BindObject->GetPropertyFlag(FArmyObject::FLAG_POINTOBJ))
	{
		WidthLine->SetStart(WidthStart);
		WidthLine->SetEnd(WidthEnd);
				
		LengthLine->SetStart(LengthStart);
		LengthLine->SetEnd(LengthEnd);
	}

	if (BindObject->GetPropertyFlag(FArmyObject::FLAG_POINTOBJ))
	{
		XFrontLine->SetStart(BindObject->GetBasePos());
		XFrontLine->SetEnd(XFrontLine->GetStart() - FVector(1, 0, 0));
		XBackLine->SetStart(BindObject->GetBasePos());
		XBackLine->SetEnd(XBackLine->GetStart() + FVector(1, 0, 0));
		YFrontLine->SetStart(BindObject->GetBasePos());
		YFrontLine->SetEnd(YFrontLine->GetStart() - FVector(0, 1, 0));
		YBackLine->SetStart(BindObject->GetBasePos());
		YBackLine->SetEnd(YBackLine->GetStart() + FVector(0, 1, 0));
	}
	else
	{
		XFrontLine->SetStart(SelfCenter + FVector(1, 0, 0) * SelfBox.GetSize().X / 2);
		XFrontLine->SetEnd(XFrontLine->GetStart() - FVector(1, 0, 0));
		XBackLine->SetStart(SelfCenter + FVector(-1, 0, 0) * SelfBox.GetSize().X / 2);
		XBackLine->SetEnd(XBackLine->GetStart() + FVector(1, 0, 0));
		YFrontLine->SetStart(SelfCenter + FVector(0, 1, 0) * SelfBox.GetSize().Y / 2);
		YFrontLine->SetEnd(YFrontLine->GetStart() - FVector(0, 1, 0));
		YBackLine->SetStart(SelfCenter + FVector(0, -1, 0) * SelfBox.GetSize().Y / 2);
		YBackLine->SetEnd(YBackLine->GetStart() + FVector(0, 1, 0));
	}

	FVector2D XLineStart = FVector2D(XFrontLine->GetStart());
	FVector2D XLineEnd = XLineStart + FVector2D(10, 0);

	FVector2D YLineStart = FVector2D(YFrontLine->GetStart());
	FVector2D YLineEnd = YLineStart + FVector2D(0, 10);

	for (auto ObjIt : InObjects)
	{
		if (ObjIt == BindObject) continue;
		if (IgoneObjectTypeArray.Contains(ObjIt->GetType())) continue;

		if (ObjIt->GetPropertyFlag(FArmyObject::FLAG_COLLISION))
		{
			if (ObjIt->bBoundingBox)
			{
				FBox ObjBox = ObjIt->GetBounds();

				if (ObjBox.Min.Y <= XFrontLine->GetStart().Y &&
					ObjBox.Max.Y >= XFrontLine->GetStart().Y &&
					(ObjBox.Min.X > XFrontLine->GetStart().X || XFrontLine->GetStart().X > XFrontLine->GetEnd().X))
				{
					if (ObjBox.Min.X < XFrontLine->GetEnd().X)
					{
						XFrontLine->SetEnd(FVector(ObjBox.Min.X, XFrontLine->GetStart().Y, SelfCenter.Z));
					}
				}
				else if (ObjBox.Min.Y <= XBackLine->GetStart().Y &&
					ObjBox.Max.Y >= XBackLine->GetStart().Y &&
					(ObjBox.Max.X < XBackLine->GetStart().X || XBackLine->GetStart().X < XBackLine->GetEnd().X))
				{
					if (ObjBox.Max.X > XBackLine->GetEnd().X)
					{
						XBackLine->SetEnd(FVector(ObjBox.Max.X, XBackLine->GetStart().Y, SelfCenter.Z));
					}
				}
				else if (ObjBox.Min.X <= YFrontLine->GetStart().X &&
					ObjBox.Max.X >= YFrontLine->GetStart().X &&
					(ObjBox.Min.Y > YFrontLine->GetStart().Y||YFrontLine->GetStart().Y > YFrontLine->GetEnd().Y))
				{
					if (ObjBox.Min.Y < YFrontLine->GetEnd().Y)
					{
						YFrontLine->SetEnd(FVector(YFrontLine->GetStart().X, ObjBox.Min.Y, SelfCenter.Z));
					}
				}
				else if (ObjBox.Min.X <= YBackLine->GetStart().X &&
					ObjBox.Max.X >= YBackLine->GetStart().X &&
					(ObjBox.Max.Y < YBackLine->GetStart().Y || YBackLine->GetStart().Y < XBackLine->GetEnd().Y))
				{
					if (ObjBox.Max.Y > YBackLine->GetEnd().Y)
					{
						YBackLine->SetEnd(FVector(YBackLine->GetStart().X, ObjBox.Max.Y, SelfCenter.Z));
					}
				}
			}
			else
			{
				TArray<TSharedPtr<FArmyLine>> Lines;
				ObjIt->GetLines(Lines);
				for (auto LineIt : Lines)
				{
					FVector2D InterPoint;
					//FVector InterPoint3D;
					//bool MatchPoint = false;
					if (FArmyMath::Line2DIntersection(FVector2D(LineIt->GetStart()), FVector2D(LineIt->GetEnd()), XLineStart, XLineEnd, InterPoint))
					{
						float LineMaxY = LineIt->GetStart().Y;
						float LineMinY = LineMaxY;
						LineIt->GetStart().Y < LineIt->GetEnd().Y ? LineMaxY = LineIt->GetEnd().Y : LineMinY = LineIt->GetEnd().Y;
						if (InterPoint.Y < LineMaxY && InterPoint.Y > LineMinY)
						{
							if ((XFrontLine->GetStart().X > XFrontLine->GetEnd().X && InterPoint.X > XFrontLine->GetStart().X) ||(XFrontLine->GetStart().X < XFrontLine->GetEnd().X && XFrontLine->GetStart().X < InterPoint.X && XFrontLine->GetEnd().X > InterPoint.X))
							{
								XFrontLine->SetEnd(FVector(InterPoint, 0));
							}

							if ((XBackLine->GetStart().X < XBackLine->GetEnd().X && InterPoint.X < XBackLine->GetStart().X) || (XBackLine->GetStart().X > XBackLine->GetEnd().X && XBackLine->GetStart().X > InterPoint.X && XBackLine->GetEnd().X < InterPoint.X))
							{
								XBackLine->SetEnd(FVector(InterPoint, 0));
							}
						}
					}
					if (FArmyMath::Line2DIntersection(FVector2D(LineIt->GetStart()), FVector2D(LineIt->GetEnd()), FVector2D(YFrontLine->GetStart()), FVector2D(YFrontLine->GetEnd()), InterPoint))
					{
						float LineMaxX = LineIt->GetStart().X;
						float LineMinX = LineMaxX;
						LineIt->GetStart().X < LineIt->GetEnd().X ? LineMaxX = LineIt->GetEnd().X : LineMinX = LineIt->GetEnd().X;
						if (InterPoint.X < LineMaxX && InterPoint.X > LineMinX)
						{
							if ((YFrontLine->GetStart().Y > YFrontLine->GetEnd().Y && InterPoint.Y > YFrontLine->GetStart().Y) || (YFrontLine->GetStart().Y < YFrontLine->GetEnd().Y && YFrontLine->GetStart().Y < InterPoint.Y && YFrontLine->GetEnd().Y > InterPoint.Y))
							{
								YFrontLine->SetEnd(FVector(InterPoint, 0));
							}

							if ((YBackLine->GetStart().Y < YBackLine->GetEnd().Y && InterPoint.Y < YBackLine->GetStart().Y) || (YBackLine->GetStart().Y > YBackLine->GetEnd().Y && YBackLine->GetStart().Y > InterPoint.Y && YBackLine->GetEnd().Y < InterPoint.Y))
							{
								YBackLine->SetEnd(FVector(InterPoint, 0));
							}
						}
					}
				}
				TArray<TSharedPtr<FArmyCircle>> CLines;
				ObjIt->GetCircleLines(CLines);
				for (auto C : CLines)
				{
					TArray<FVector> InterPoints;
					if (FArmyMath::ArcLineIntersection(C->GetStartPos(), C->GetEndPos(), C->GetArcMiddlePos(), XBackLine->GetEnd(), XFrontLine->GetEnd(), InterPoints))
					{
						FVector MNearPos = FMath::ClosestPointOnInfiniteLine(C->GetStartPos(), C->GetEndPos(), C->GetArcMiddlePos());

						for (auto& V : InterPoints)
						{
							FVector TempNearPos = FMath::ClosestPointOnInfiniteLine(C->GetStartPos(), C->GetEndPos(), V);
							if (((V - TempNearPos).GetSafeNormal() - (C->GetArcMiddlePos() - MNearPos).GetSafeNormal()).Size() < 0.0001)
							{
								if ((XFrontLine->GetStart().X > XFrontLine->GetEnd().X && XFrontLine->GetStart().X < V.X) || XFrontLine->GetStart().X <= V.X && XFrontLine->GetEnd().X >= V.X)
								{
									XFrontLine->SetEnd(V);
									//MatchPoint = true;
								}
								if ((XBackLine->GetStart().X < XBackLine->GetEnd().X && XBackLine->GetStart().X > V.X) || XBackLine->GetStart().X >= V.X && XBackLine->GetEnd().X <= V.X)
								{
									XBackLine->SetEnd(V);
									//MatchPoint = true;
								}
							}
						}
					}
					InterPoints.Empty();
					if (FArmyMath::ArcLineIntersection(C->GetStartPos(), C->GetEndPos(), C->GetArcMiddlePos(), YBackLine->GetEnd(), YFrontLine->GetEnd(), InterPoints))
					{
						FVector MNearPos = FMath::ClosestPointOnInfiniteLine(C->GetStartPos(), C->GetEndPos(), C->GetArcMiddlePos());

						for (auto& V : InterPoints)
						{
							FVector TempNearPos = FMath::ClosestPointOnInfiniteLine(C->GetStartPos(), C->GetEndPos(), V);
							FVector dir1 = (V - TempNearPos).GetSafeNormal();
							FVector dir2 = (C->GetArcMiddlePos() - MNearPos).GetSafeNormal();
							if (((V - TempNearPos).GetSafeNormal() - (C->GetArcMiddlePos() - MNearPos).GetSafeNormal()).Size() < 0.0001)
							{
								if ((YFrontLine->GetStart().Y > YFrontLine->GetEnd().Y && YFrontLine->GetStart().Y < V.Y) || YFrontLine->GetStart().Y <= V.Y && YFrontLine->GetEnd().Y >= V.Y)
								{
									YFrontLine->SetEnd(V);
									//MatchPoint = true;
								}
								if ((YBackLine->GetStart().Y < YBackLine->GetEnd().Y && YBackLine->GetStart().Y > V.Y) || YBackLine->GetStart().Y >= V.Y && YBackLine->GetEnd().Y <= V.Y)
								{
									YBackLine->SetEnd(V);
									//MatchPoint = true;
								}
							}
						}
					}
				}
			}
		}
	}

	

	XFrontDimensionText.SetPosition((XFrontLine->GetEnd() + XFrontLine->GetStart()) / 2);
	XBackDimensionText.SetPosition((XBackLine->GetEnd() + XBackLine->GetStart()) / 2);
	YFrontDimensionText.SetPosition((YFrontLine->GetEnd() + YFrontLine->GetStart()) / 2);
	YBackDimensionText.SetPosition((YBackLine->GetEnd() + YBackLine->GetStart()) / 2);

	WidthDimensionText.SetPosition((WidthLine->GetEnd() + WidthLine->GetStart()) / 2);
	LengthDimensionText.SetPosition((LengthLine->GetEnd() + LengthLine->GetStart()) / 2);

	WidthDimensionText.SetRotator(FQuat::FindBetweenVectors(WidthLine->GetEnd() - WidthLine->GetStart(), FVector(1, 0, 0)).Rotator());
	LengthDimensionText.SetRotator(FQuat::FindBetweenVectors(LengthLine->GetEnd() - LengthLine->GetStart(), FVector(1, 0, 0)).Rotator());

	int32 XFrontV = FMath::CeilToInt((XFrontLine->GetEnd() - XFrontLine->GetStart()).Size() * 10);
	XFrontDimensionText.Text = FText::FromString(FString::FromInt(XFrontV));
	int32 XBackV = FMath::CeilToInt((XBackLine->GetEnd() - XBackLine->GetStart()).Size() * 10);
	XBackDimensionText.Text = FText::FromString(FString::FromInt(XBackV));
	int32 YFrontV = FMath::CeilToInt((YFrontLine->GetEnd() - YFrontLine->GetStart()).Size() * 10);
	YFrontDimensionText.Text = FText::FromString(FString::FromInt(YFrontV));
	int32 YBackV = FMath::CeilToInt((YBackLine->GetEnd() - YBackLine->GetStart()).Size() * 10);
	YBackDimensionText.Text = FText::FromString(FString::FromInt(YBackV));

	if (!BindObject->GetPropertyFlag(FArmyObject::FLAG_POINTOBJ))
	{
		WidthDimensionText.Text = FText::FromString(FString::FromInt(FMath::CeilToInt((WidthLine->GetEnd() - WidthLine->GetStart()).Size() * 10)));

		LengthDimensionText.Text = FText::FromString(FString::FromInt(FMath::CeilToInt((LengthLine->GetEnd() - LengthLine->GetStart()).Size() * 10)));
	}

	if (bAutoCheck)
	{
		bool bX = XFrontV < XBackV;
		SetLineVisible(XFRONT, bX);
		SetLineVisible(XBACK, !bX);

		bool bY = YFrontV < YBackV;
		SetLineVisible(YFRONT, bY);
		SetLineVisible(YBACK, !bY);
	}
	/* @ÁºÏþ·Æ ¸üÐÂÖÕÖ¹·ûµÄÎ»ÖÃ*/
	UpdateArrows(XFrontLine, XFrontStartArrows, XFrontEndArrows);
	UpdateArrows(XBackLine, XBackStartArrows, XBackEndArrows);
	UpdateArrows(YFrontLine, YFrontStartArrows, YFrontEndArrows);
	UpdateArrows(YBackLine, YBackStartArrows, YBackEndArrows);
}
void FArmyObjAutoRuler::DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (BindObject.IsValid())
		{
			if (LineVisible & XFRONT && XFrontLine->GetStart().X < XFrontLine->GetEnd().X)
			{
				XFrontDimensionText.DrawHUD(InViewPortClient,View,Canvas);
			}
			if (LineVisible & XBACK && XBackLine->GetStart().X > XBackLine->GetEnd().X)
			{
				XBackDimensionText.DrawHUD(InViewPortClient, View, Canvas);
			}
			if (LineVisible & YFRONT && YFrontLine->GetStart().Y < YFrontLine->GetEnd().Y)
			{
				YFrontDimensionText.DrawHUD(InViewPortClient, View, Canvas);
			}
			if (LineVisible & YBACK && YBackLine->GetStart().Y > YBackLine->GetEnd().Y)
			{
				YBackDimensionText.DrawHUD(InViewPortClient, View, Canvas);
			}
			if (!BindObject->GetPropertyFlag(FArmyObject::FLAG_POINTOBJ))
			{
				if (LineVisible & WIDTH)
				{
					WidthDimensionText.DrawHUD(InViewPortClient, View, Canvas);
				}
				if (LineVisible & LENGTH)
				{
					LengthDimensionText.DrawHUD(InViewPortClient, View, Canvas);
				}
			}
		}
	}
}
void FArmyObjAutoRuler::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (BindObject.IsValid())
		{
			if (LineVisible & XFRONT && XFrontLine->GetStart().X < XFrontLine->GetEnd().X)
			{
				XFrontLine->Draw(PDI, View);
				/* @ÁºÏþ·Æ »æÖÆÖÕÖ¹·ûºÅ*/
				XFrontStartArrows->Draw(PDI, View);
				XFrontEndArrows->Draw(PDI, View);
			}
			if (LineVisible & XBACK && XBackLine->GetStart().X > XBackLine->GetEnd().X)
			{
				XBackLine->Draw(PDI, View);
				/* @ÁºÏþ·Æ »æÖÆÖÕÖ¹·ûºÅ*/
				XBackStartArrows->Draw(PDI, View);
				XBackEndArrows->Draw(PDI, View);
			}
			if (LineVisible & YFRONT && YFrontLine->GetStart().Y < YFrontLine->GetEnd().Y)
			{
				YFrontLine->Draw(PDI, View);
				/* @ÁºÏþ·Æ »æÖÆÖÕÖ¹·ûºÅ*/
				YFrontStartArrows->Draw(PDI, View);
				YFrontEndArrows->Draw(PDI, View);
			}
			if (LineVisible & YBACK && YBackLine->GetStart().Y > YBackLine->GetEnd().Y)
			{
				YBackLine->Draw(PDI, View);
				/* @ÁºÏþ·Æ »æÖÆÖÕÖ¹·ûºÅ*/
				YBackStartArrows->Draw(PDI, View);
				YBackEndArrows->Draw(PDI, View);
			}
			if (!BindObject->GetPropertyFlag(FArmyObject::FLAG_POINTOBJ))
			{
				if (LineVisible & WIDTH)
				{
					WidthLine->Draw(PDI, View);
				}
				if (LineVisible & LENGTH)
				{
					LengthLine->Draw(PDI, View);
				}
			}
		}	
	}
}
void FArmyObjAutoRuler::SetForceVisible(bool InV)
{
	//if (ForceVisible == InV)
	//{
	//	return;
	//}
	//FArmyObject::SetForceVisible(InV);
	//XFrontDimensionText->SetActorHiddenInGame(!(LineVisible & XFRONT) || !InV);
	//XBackDimensionText->SetActorHiddenInGame(!(LineVisible & XBACK)||!InV);
	//YFrontDimensionText->SetActorHiddenInGame(!(LineVisible & YFRONT)||!InV);
	//YBackDimensionText->SetActorHiddenInGame(!(LineVisible & YBACK)||!InV);

	//bool IsPoint = BindObject->GetPropertyFlag(FArmyObject::FLAG_POINTOBJ);
	//WidthDimensionText->SetActorHiddenInGame(!(LineVisible & WIDTH) || IsPoint || !InV);
	//LengthDimensionText->SetActorHiddenInGame(!(LineVisible & LENGTH)||IsPoint || !InV);
}
void FArmyObjAutoRuler::SetLineVisible(LineID InID, bool InV)
{
	if (InV)
	{
		LineVisible |= InID;
	}
	else
	{
		LineVisible &= ~InID;
	}

	bool IsPoint = BindObject->GetPropertyFlag(FArmyObject::FLAG_POINTOBJ);

	//switch (InID)
	//{
	//case FArmyObjAutoRuler::XFRONT:
	//	InV ? XFrontDimensionText->SetActorHiddenInGame(false) : XFrontDimensionText->SetActorHiddenInGame(true);
	//	break;
	//case FArmyObjAutoRuler::XBACK:
	//	InV ? XBackDimensionText->SetActorHiddenInGame(false) : XBackDimensionText->SetActorHiddenInGame(true);
	//	break;
	//case FArmyObjAutoRuler::YFRONT:
	//	InV ? YFrontDimensionText->SetActorHiddenInGame(false) : YFrontDimensionText->SetActorHiddenInGame(true);
	//	break;
	//case FArmyObjAutoRuler::YBACK:
	//	InV ? YBackDimensionText->SetActorHiddenInGame(false) : YBackDimensionText->SetActorHiddenInGame(true);
	//	break;
	//case FArmyObjAutoRuler::WIDTH:
	//	InV ? WidthDimensionText->SetActorHiddenInGame(IsPoint) : WidthDimensionText->SetActorHiddenInGame(true);
	//	break;
	//case FArmyObjAutoRuler::LENGTH:
	//	InV ? LengthDimensionText->SetActorHiddenInGame(IsPoint) : LengthDimensionText->SetActorHiddenInGame(true);
	//	break;
	//default:
	//	break;
	//}
}
void FArmyObjAutoRuler::SetBoundObj(FObjectPtr InObj)
{
	BindObject = InObj;
}

void FArmyObjAutoRuler::UpdateArrows(TSharedPtr<FArmyLine> MainLine, TSharedPtr<FArmyPolygon> LeftArrows, TSharedPtr<FArmyPolygon> RightArrows)
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
