#include "ArmyMouseCapture.h"
#include "SceneView.h"
#include "Engine/Canvas.h"
#include "Engine/GameEngine.h"
#include "ArmyEditorViewportClient.h"
#include "Math/ArmyMath.h"
#include "ArmyStyle.h"

#define SEMI_ORTHO //正交捕捉设置成半正交

FArmyMouseCapture::FArmyMouseCapture()
    : CurrentFixPoint(ForceInitToZero)
    , IsCaptrueChange(true)
    , CurrentFixPointInit(false)
    , CaptureModel(0)
    , LineColor("FArmyStyle::Get().GetColor("Color.Capture"))
{
	SetRefCoordinateSystem(FVector(ForceInitToZero), FVector(1, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1));
}
FArmyMouseCapture::~FArmyMouseCapture()
{

}
const TSharedPtr<"FArmyEditPoint> FArmyMouseCapture::GetCapturePointPtr() const
{
	return CurrentCaptruePoint.CapturePointPtr;
}
FVector2D FArmyMouseCapture::GetCapturePoint()
{
	return CurrentCaptruePoint.CapturePoint;
}
FVector2D FArmyMouseCapture::GetMovePoint()
{
	return CurrentMovePoint;
}
void DrawBrokenLine(const FVector2D& InStart, const FVector2D& InEnd, const FLinearColor& InColor, FCanvas* InCanvas)
{
	FVector2D LineDir = (InEnd - InStart).GetSafeNormal();
	if (LineDir.Size() < 0.0001) return;
	float LineLen = (InEnd - InStart).Size();
	int32 BrokenNum = FMath::CeilToInt(LineLen / 5.f);

	FBatchedElements* BatchedElements = InCanvas->GetBatchedElements(FCanvas::ET_Line);
	FHitProxyId HitProxyId = InCanvas->GetHitProxyId();

	for (int32 i = 0; i < BrokenNum; i+=2)
	{
		BatchedElements->AddLine(FVector(InStart + LineDir * i * 5, 0.f), FVector(InStart + LineDir * (i + 1) * 5, 0.f), InColor, HitProxyId, 0);
	}
}
void FArmyMouseCapture::Draw(UArmyEditorViewportClient* InViewPortClient,FViewport* InViewport, FCanvas* SceneCanvas)
{
	if(!EnableCapture) return;

	if (CurrentCaptruePoint.PointType != PT_None)
	{
		FVector2D PixPos;
		InViewPortClient->WorldToPixel(FromBasePlane(FVector(CurrentCaptruePoint.CapturePoint, 0)), PixPos);

		switch (CurrentCaptruePoint.PointType)
		{
		case FArmyMouseCapture::PT_VertexCaptrue:
		{
			FCanvasBoxItem BoxItem(PixPos - FVector2D(5.f, 5.f), FVector2D(10.f, 10.f));
			BoxItem.SetColor(LineColor);
			BoxItem.Draw(SceneCanvas);
		}
			break;
		case FArmyMouseCapture::PT_Perpendicular:
		{
			FCanvasLineItem LineItem1(PixPos - FVector2D(5.f,5.f), PixPos + FVector2D(-5.f, 5.f));
			FCanvasLineItem LineItem2(PixPos + FVector2D(-5.f, 5.f), PixPos + FVector2D(5.f, 5.f));

			FCanvasLineItem LineItem3(PixPos - FVector2D(5.f, 0.f), PixPos);
			FCanvasLineItem LineItem4(PixPos, PixPos + FVector2D(0.f, 5.f));

			LineItem1.SetColor(LineColor);
			LineItem2.SetColor(LineColor);
			LineItem3.SetColor(LineColor);
			LineItem4.SetColor(LineColor);
			LineItem1.Draw(SceneCanvas);
			LineItem2.Draw(SceneCanvas);
			LineItem3.Draw(SceneCanvas);
			LineItem4.Draw(SceneCanvas);
		}
			break;
		case FArmyMouseCapture::PT_Closest:
		{
			FVector2D PixPos;
			InViewPortClient->WorldToPixel(FromBasePlane(FVector(CurrentCaptruePoint.CapturePoint, 0)), PixPos);
			FCanvasLineItem TL(FVector2D(PixPos.X - 5, PixPos.Y + 7), FVector2D(PixPos.X + 5, PixPos.Y + 7));
			FCanvasLineItem BL(FVector2D(PixPos.X - 5, PixPos.Y - 7), FVector2D(PixPos.X + 5, PixPos.Y - 7));
			FCanvasLineItem LL(FVector2D(PixPos.X + 5, PixPos.Y + 7), FVector2D(PixPos.X - 5, PixPos.Y - 7));
			FCanvasLineItem RL(FVector2D(PixPos.X - 5, PixPos.Y + 7), FVector2D(PixPos.X + 5, PixPos.Y - 7));

			TL.SetColor(LineColor);
			TL.LineThickness = 1;
			TL.Draw(SceneCanvas);

			BL.SetColor(LineColor);
			BL.LineThickness = 1;
			BL.Draw(SceneCanvas);

			LL.SetColor(LineColor);
			LL.Draw(SceneCanvas);

			RL.SetColor(LineColor);
			RL.Draw(SceneCanvas);
		}
			break;
		case PT_Midpoint:
		{
			FCanvasLineItem LineItem1(PixPos + FVector2D(-5.f, 5.f), PixPos + FVector2D(0.f, -5.f));
			FCanvasLineItem LineItem2(PixPos + FVector2D(0.f, -5.f), PixPos + FVector2D(5.f, 5.f));
			FCanvasLineItem LineItem3(PixPos + FVector2D(-5.f, 5.f), PixPos + FVector2D(5.f, 5.f));

			LineItem1.SetColor(LineColor);
			LineItem2.SetColor(LineColor);
			LineItem3.SetColor(LineColor);
			LineItem1.Draw(SceneCanvas);
			LineItem2.Draw(SceneCanvas);
			LineItem3.Draw(SceneCanvas);
		}
			break;
		case FArmyMouseCapture::PT_Intersection:
		{
			FVector2D PixPos;
			InViewPortClient->WorldToPixel(FromBasePlane(FVector(CurrentCaptruePoint.CapturePoint, 0)), PixPos);

			FCanvasLineItem X1(FVector2D(PixPos.X - 5, PixPos.Y + 5), FVector2D(PixPos.X + 5, PixPos.Y - 5));
			FCanvasLineItem X2(FVector2D(PixPos.X - 5, PixPos.Y - 5), FVector2D(PixPos.X + 5, PixPos.Y + 5));

			X1.SetColor(LineColor);
			X1.Draw(SceneCanvas);

			X2.SetColor(LineColor);
			X2.Draw(SceneCanvas);
		}
			break;
		default:
			break;
		}
	}
	if (CaptureModel & Model_Auxiliary)
	{
#if 0
		for (auto AuxIt : AuxiliaryPoints)
		{
			FVector2D PixPos;
			InViewPortClient->WorldToPixel(FromBasePlane(FVector(AuxIt.CapturePoint, 0)), PixPos);

			FCanvasLineItem HL(FVector2D(PixPos.X - 5, PixPos.Y), FVector2D(PixPos.X + 5, PixPos.Y));
			FCanvasLineItem VL(FVector2D(PixPos.X, PixPos.Y - 5), FVector2D(PixPos.X, PixPos.Y + 5));

			HL.SetColor(LineColor);
			HL.Draw(SceneCanvas);

			VL.SetColor(LineColor);
			VL.Draw(SceneCanvas);
		}
#endif
		for (auto AuxIt : AuxiliaryLines)
		{
			FVector2D PixStartPos,PixEndPos;
			InViewPortClient->WorldToPixel(FromBasePlane(FVector(AuxIt.StartPoint, 0)), PixStartPos);

			if (AuxIt.IsFullScreen)
			{
				FVector2D LineDir = AuxIt.EndPoint - AuxIt.StartPoint;
				LineDir.Normalize();
				FVector2D ViewPortSize;
				InViewPortClient->GetViewportSize(ViewPortSize);
				FVector2D AuxPixPoint;
				InViewPortClient->WorldToPixel(FromBasePlane(FVector(AuxIt.AuxiliaryPoint, 0)), AuxPixPoint);

				PixStartPos = AuxPixPoint - LineDir * ViewPortSize.Size();
				PixEndPos = AuxPixPoint + LineDir * ViewPortSize.Size();
			}
			else
			{
				InViewPortClient->WorldToPixel(FromBasePlane(FVector(AuxIt.EndPoint, 0)), PixEndPos);
			}

			//FCanvasLineItem LineItem(PixStartPos, PixEndPos);
			//LineItem.SetColor(FLinearColor::Green);
			//LineItem.Draw(SceneCanvas);
			DrawBrokenLine(PixStartPos, PixEndPos, LineColor, SceneCanvas);
			if (AuxIt.AuxiliaryDraw)
			{
				FVector2D PixAuxiliaryPos;
				InViewPortClient->WorldToPixel(FromBasePlane(FVector(AuxIt.AuxiliaryPoint, 0)), PixAuxiliaryPos);
				FCanvasBoxItem BoxItem(PixAuxiliaryPos - FVector2D(5.f, 5.f), FVector2D(10.f, 10.f));
				BoxItem.SetColor(LineColor);
				BoxItem.Draw(SceneCanvas);
			}
		}
		//if (AuxiliaryLines.Num() > 0)
		//{

		//}
	}
}
void FArmyMouseCapture::SetRefCoordinateSystem(const FVector& InBasePos, const FVector& InXDir, const FVector& InYDir, const FVector& InNormal)
{
	BasePoint = InBasePos;
	BaseXDir = InXDir;
	BaseYDir = InYDir;
	BaseNormal = InNormal;
	BasePlane = FPlane(BasePoint, BaseNormal);
}
const FPlane& FArmyMouseCapture::GetPlane() const
{
	return BasePlane;
}
void FArmyMouseCapture::Reset()
{
	AuxiliaryPoints.Empty();
	AuxiliaryLines.Empty();
	CurrentCaptrueLines.Empty();
	CurrentCaptruePoint.Reset();

	IsCaptrueChange = true;
	CurrentFixPointInit = false;
}
void FArmyMouseCapture::SetCurrentPoint(FVector2D InCurrent,bool IsContinuous)
{
	/* @刘克祥 用于右键后继续画线*/
	if (CurrentCaptruePoint.CapturePointPtr.IsValid())//如果捕捉到的点有效
	{
		FVector temp = CurrentCaptruePoint.CapturePointPtr->GetPos();
		if (FVector2D(temp.X, temp.Y) == InCurrent && CurrentCaptrueLines.Num() != 0)//如果捕捉到的点就是当前点击的点  并且捕捉到了线
		{
			for (auto It: CurrentCaptrueLines)//查找当前捕捉到的线们
			{
				if (!It.IsValid()) continue;
				TSharedPtr<"FArmyEditPoint> TempPointStart = It.Pin()->GetStartPointer();
				TSharedPtr<"FArmyEditPoint> TempPointEnd = It.Pin()->GetEndPointer();
				if (CurrentCaptruePoint.CapturePointPtr == TempPointStart)//如果捕捉到的线的编辑点和捕捉到的点的编辑点一致，那么该点就是要做垂线的辅助点
				{
					FVector2D StartPoint = FVector2D(It.Pin()->GetStart().X, It.Pin()->GetStart().Y);
					FVector2D EndPoint = FVector2D(It.Pin()->GetEnd().X, It.Pin()->GetEnd().Y);
					AddAuxiliaryPerpendicular(EndPoint, StartPoint);
					break;
				}
				else if (CurrentCaptruePoint.CapturePointPtr == TempPointEnd)
				{
					FVector2D StartPoint = FVector2D(It.Pin()->GetStart().X, It.Pin()->GetStart().Y);
					FVector2D EndPoint = FVector2D(It.Pin()->GetEnd().X, It.Pin()->GetEnd().Y);
					AddAuxiliaryPerpendicular(StartPoint, EndPoint);
					break;
				}
			}
		}
	}
	/* @刘克祥 用于连续画线*/
	else if(IsContinuous)
	{
		if (CurrentFixPointInit && CurrentFixPoint != InCurrent)
			AddAuxiliaryPerpendicular(CurrentFixPoint, InCurrent);
	}

	//Reset();
	/* @刘克祥 以当前点为终止点，上一个点为起始点的线段的垂线 */
	CurrentFixPoint = InCurrent;
	CurrentFixPointInit = true;
}
void FArmyMouseCapture::AddAuxiliaryPoint(TSharedPtr<"FArmyEditPoint> InPoint)
{
	CapturePointInfo TempCapturePoint;
	TempCapturePoint.PointType = PT_VertexCaptrue;
	TempCapturePoint.CapturePoint = FVector2D(InPoint->Pos);
	TempCapturePoint.CapturePointPtr = InPoint;

	TempCapturePoint.RelatedLines.Add(AuxiliaryLineInfo(TempCapturePoint.CapturePoint, FVector2D(TempCapturePoint.CapturePoint.X, TempCapturePoint.CapturePoint.Y + 100), TempCapturePoint.CapturePoint, true, true));//垂直辅助线
	TempCapturePoint.RelatedLines.Add(AuxiliaryLineInfo(TempCapturePoint.CapturePoint, FVector2D(TempCapturePoint.CapturePoint.X + 100, TempCapturePoint.CapturePoint.Y), TempCapturePoint.CapturePoint, true, true));//水平辅助线

	AuxiliaryPoints.AddUnique(TempCapturePoint);
}
void FArmyMouseCapture::ClearAuxiliaryPoints()
{
	AuxiliaryPoints.Empty();
}
//void FArmyMouseCapture::SetCapture(bool InCapture)
//{
//	if (!InCapture)
//	{
//		IsCaptrueChange = true;
//	}
//	else
//	{
//		AuxiliaryLines.Empty();
//	}
//
//	IsCaptrue = InCapture;
//}

bool FArmyMouseCapture::Capture(UArmyEditorViewportClient* InViewPortClient,int32 x, int32 y, const TArray< TSharedPtr<class "FArmyLine> >& InData, const TArray< TSharedPtr<class "FArmyPoint> >& InPointData)
{
    if (!EnableCapture)
    {
        return false;
    }

	//优先级 垂足-中点-交点-端点-辅助线交点-最近点
	int32 CaptureDistance = 10;//相交判断的像素距离
	int32 AuxiliaryPointDistance = 7;//辅助点判断像素距离

	bool IsCapture = false, OrthoCapture = false;

	int32 TempX = x, TempY = y;

	//FVector MouseWorldPoint(InViewPortClient->PixelToWorld(TempX, TempY, 0));
	//MouseWorldPoint.Z = 0;

	FVector MWorldStart, MWorldDir;
	InViewPortClient->DeprojectFVector2D(FVector2D(TempX, TempY), MWorldStart, MWorldDir);
	FVector MouseWorldPoint = FMath::LinePlaneIntersection(MWorldStart, MWorldStart + MWorldDir * ((InViewPortClient->GetViewLocation() - FVector::PointPlaneProject(InViewPortClient->GetViewLocation(),BasePlane)).Size() + 100000),BasePlane);

	MouseWorldPoint = ToBasePlane(MouseWorldPoint);

	CurrentMovePoint = FVector2D(MouseWorldPoint);

	if (CaptureModel & Model_Ortho)
	{
		OrthoCapture = OrthoAmend(InViewPortClient, TempX, TempY);
	}

	CurrentCaptrueLines.Empty();
	if (CaptureModel & (Model_VertexCaptrue | Model_Perpendicular | Model_Closest | Model_Midpoint | Model_IntersectionPoint))
	{
		CapturePointInfo TempCapturePoint;

		//float WorldToScreenScale = InViewPortClient->GetUniformScale(MouseWorldPoint);

		float NearestToPoint = -1;

		TArray<TSharedPtr<class "FArmyLine>> NearestLines, RelatedLines/*捕捉点的关联线，用来计算范围线*/;
		for (auto LIt : InData)
		{
			FVector NearPoint = FMath::ClosestPointOnLine(LIt->GetStart(), LIt->GetEnd(), MouseWorldPoint);
			FVector2D PixPos, TempPixPos;;
			InViewPortClient->WorldToPixel(FromBasePlane(NearPoint), PixPos);
			float TempPixDis = (PixPos - FVector2D(x, y)).Size();

			if (TempPixDis < CaptureDistance)
			{
				if (CaptureModel & Model_Closest && TempCapturePoint.PointType == PT_None)
				{
					(NearestToPoint > CaptureDistance || NearestToPoint < 0) ? NearestToPoint = CaptureDistance : NearestToPoint;

					RelatedLines.Add(LIt);
					TempCapturePoint.PointType = PT_Closest;
					TempCapturePoint.CapturePoint = FVector2D(NearPoint);
					TempCapturePoint.CapturePointPtr = NULL;
					IsCapture = true;
				}
				if (CaptureModel & Model_IntersectionPoint)
				{
					for (auto& L : NearestLines)
					{
						FVector InterPos;
						if ("FArmyMath::CalculateLinesIntersection(L->GetStart(), L->GetEnd(), LIt->GetStart(), LIt->GetEnd(), InterPos))
						{
							if (L->GetStart() == InterPos || L->GetEnd() == InterPos || LIt->GetStart() == InterPos || LIt->GetEnd() == InterPos)
							{
								continue;
							}

							InViewPortClient->WorldToPixel(FromBasePlane(InterPos), TempPixPos);
							float TempDisToPoint = (FVector2D(x, y) - TempPixPos).Size();
							if (NearestToPoint < 0 || TempDisToPoint < NearestToPoint)
							{
								NearestToPoint = TempDisToPoint;

								TempCapturePoint.PointType = PT_Intersection;
								TempCapturePoint.CapturePoint = FVector2D(InterPos);
								TempCapturePoint.CapturePointPtr = NULL;
								RelatedLines.Empty();
								IsCapture = true;
							}
						}
					}
				}
				if (CaptureModel & Model_VertexCaptrue)
				{
					InViewPortClient->WorldToPixel(FromBasePlane(LIt->GetStart()), TempPixPos);
					float TempDisToPoint = (FVector2D(x, y) - TempPixPos).Size();
					if (NearestToPoint < 0 || TempDisToPoint <= NearestToPoint)
					{
						if (TempCapturePoint.CapturePoint == FVector2D(LIt->GetStart()))
						{
							RelatedLines.Add(LIt);
						}
						else
						{
							RelatedLines.Empty();
							RelatedLines.Add(LIt);
						}
						NearestToPoint = TempDisToPoint;

						TempCapturePoint.PointType = PT_VertexCaptrue;
						TempCapturePoint.CapturePoint = FVector2D(LIt->GetStart());
						TempCapturePoint.CapturePointPtr = LIt->GetStartPointer();
						IsCapture = true;
					}
					InViewPortClient->WorldToPixel(FromBasePlane(LIt->GetEnd()), TempPixPos);
					TempDisToPoint = (FVector2D(x, y) - TempPixPos).Size();
					if (NearestToPoint < 0 || TempDisToPoint < NearestToPoint)
					{
						NearestToPoint = TempDisToPoint;

						TempCapturePoint.PointType = PT_VertexCaptrue;
						TempCapturePoint.CapturePoint = FVector2D(LIt->GetEnd());
						TempCapturePoint.CapturePointPtr = LIt->GetEndPointer();
						IsCapture = true;
					}
				}
				if (CaptureModel & Model_Midpoint)
				{
					FVector MidPoint = (LIt->GetStart() + LIt->GetEnd()) / 2;
					InViewPortClient->WorldToPixel(FromBasePlane(MidPoint), TempPixPos);
					float TempDisToPoint = (FVector2D(x, y) - TempPixPos).Size();
					if (NearestToPoint < 0 || TempDisToPoint < NearestToPoint)
					{
						if (TempCapturePoint.CapturePoint == FVector2D(LIt->GetStart()))
						{
							RelatedLines.Add(LIt);
						}
						else
						{
							RelatedLines.Empty();
							RelatedLines.Add(LIt);
						}
						NearestToPoint = TempDisToPoint;

						TempCapturePoint.PointType = PT_Midpoint;
						TempCapturePoint.CapturePoint = FVector2D(MidPoint);
						TempCapturePoint.CapturePointPtr = NULL;
						IsCapture = true;
					}
				}
				if (CaptureModel & Model_Perpendicular && CurrentFixPointInit)
				{
					FVector FootPoint = FMath::ClosestPointOnInfiniteLine(LIt->GetStart(), LIt->GetEnd(), FVector(CurrentFixPoint, 0));
					if (FVector2D(FootPoint) != CurrentFixPoint)
					{
						InViewPortClient->WorldToPixel(FromBasePlane(FootPoint), TempPixPos);
						float TempDisToPoint = (FVector2D(x, y) - TempPixPos).Size();
						if (NearestToPoint < 0 || TempDisToPoint < NearestToPoint)
						{
							if (TempCapturePoint.CapturePoint == FVector2D(LIt->GetStart()))
							{
								RelatedLines.Add(LIt);
							}
							else
							{
								RelatedLines.Empty();
								RelatedLines.Add(LIt);
							}
							NearestToPoint = TempDisToPoint;

							TempCapturePoint.PointType = PT_Perpendicular;
							TempCapturePoint.CapturePoint = FVector2D(FootPoint);
							TempCapturePoint.CapturePointPtr = NULL;
							IsCapture = true;
						}
					}
				}
				NearestLines.Add(LIt);
			}
		}
		for (auto L : RelatedLines)
		{
			CurrentCaptrueLines.Add(L.ToSharedRef());
		}
		if (CaptureModel & Model_VertexCaptrue)
		{
			for (auto PIt : InPointData)
			{
				FVector2D PixPos;
				InViewPortClient->WorldToPixel(FromBasePlane(PIt->Pos), PixPos);
				float TempPixDis = (PixPos - FVector2D(x, y)).Size();
				if (TempPixDis < CaptureDistance && (NearestToPoint < 0 || TempPixDis < NearestToPoint))
				{
					NearestToPoint = TempPixDis;
					TempCapturePoint.PointType = PT_VertexCaptrue;
					TempCapturePoint.CapturePoint = FVector2D(PIt->Pos);
					TempCapturePoint.CapturePointPtr = NULL;
					IsCapture = true;
				}
			}
		}
		
		if (IsCapture && TempCapturePoint != CurrentCaptruePoint)
		{
			IsCaptrueChange = true;
			CurrentCaptruePoint = TempCapturePoint;
		}
	
		if (NearestToPoint > 0 && NearestToPoint < AuxiliaryPointDistance)
		{
			int32 AuxPointIndex = AuxiliaryPoints.Find(CurrentCaptruePoint);
			if (IsCaptrueChange && AuxPointIndex >= 0)
			{
				IsCaptrueChange = false;
#if 0
				CapturePointInfo* AuxPoint = &(AuxiliaryPoints[AuxPointIndex]);
				AuxiliaryPoints.RemoveAt(AuxPointIndex);
#endif
				/*else
				{
					AuxPoint->RelatedLines.Add(AuxiliaryLineInfo(FVector2D(LIt->GetStart()), FVector2D(LIt->GetEnd()), CurrentCaptruePoint));
				}*/
			}
			else if(IsCaptrueChange)
			{
				IsCaptrueChange = false;

#if 0
				if (AuxiliaryPoints.Num() > 6)
					AuxiliaryPoints.HeapPopDiscard();
#endif

				if (CurrentCaptruePoint.PointType == PT_Perpendicular)
				{
					CurrentCaptruePoint.RelatedLines.Add(AuxiliaryLineInfo(CurrentFixPoint, CurrentCaptruePoint.CapturePoint, CurrentCaptruePoint.CapturePoint, true, true));
				}

				if (CaptureModel & Model_LineRange)
				{
					for (auto RL : RelatedLines)
					{
						CurrentCaptruePoint.RelatedLines.Add(AuxiliaryLineInfo(AuxiliaryLineInfo(FVector2D(RL->GetStart()), FVector2D(RL->GetEnd()), CurrentCaptruePoint.CapturePoint)));
					}
				}
				if (CaptureModel & Model_Auxiliary)
				{
					CurrentCaptruePoint.RelatedLines.Add(AuxiliaryLineInfo(CurrentCaptruePoint.CapturePoint, FVector2D(CurrentCaptruePoint.CapturePoint.X, CurrentCaptruePoint.CapturePoint.Y + 100), CurrentCaptruePoint.CapturePoint, true, true));//垂直辅助线
					CurrentCaptruePoint.RelatedLines.Add(AuxiliaryLineInfo(CurrentCaptruePoint.CapturePoint, FVector2D(CurrentCaptruePoint.CapturePoint.X + 100, CurrentCaptruePoint.CapturePoint.Y), CurrentCaptruePoint.CapturePoint, true, true));//水平辅助线
				}
				AuxiliaryPoints.Push(CurrentCaptruePoint);
			}
		}
	}
    if (IsCapture)
    {
        return IsCapture;
    }

	//////////////////////////////////////////////////////////////////////////
	if (CaptureModel & Model_LineRange || CaptureModel & Model_Auxiliary)
	{
		AuxiliaryLines.Empty();
		for (auto AuxPIt : AuxiliaryPoints)
		{
			for (auto AuxLineIt : AuxPIt.RelatedLines)
			{
				FVector2D ALineDir = AuxLineIt.EndPoint - AuxLineIt.StartPoint;
				FVector NearPoint(ForceInitToZero);
				if (ALineDir.Size() > 0 && ALineDir.X * ALineDir.Y == 0)
				{
					if (ALineDir.X == 0)
					{
						NearPoint.X = AuxLineIt.StartPoint.X;
						NearPoint.Y = CurrentCaptruePoint.CapturePoint.Y;
					}
					else
					{
						NearPoint.X = CurrentCaptruePoint.CapturePoint.X;
						NearPoint.Y = AuxLineIt.StartPoint.Y;
					}
				}
				else
				{
					NearPoint = FMath::ClosestPointOnInfiniteLine(FVector(AuxLineIt.StartPoint, 0), FVector(AuxLineIt.EndPoint, 0), FVector(CurrentCaptruePoint.CapturePoint,0));
				}

				FVector2D TempPix;
				InViewPortClient->WorldToPixel(FromBasePlane(NearPoint), TempPix);
				if ((TempPix - FVector2D(TempX, TempY)).Size() < CaptureDistance)//10个像素
				{
					bool AuxiliaryCaptrue = false;
					if (AuxiliaryLines.Num() > 0)
					{
						for (int32 i = 0; i < AuxiliaryLines.Num(); ++i)
						{
							AuxiliaryLineInfo& ExistAuxLine = AuxiliaryLines[i];
							FVector2D InterPoint;
							if ("FArmyMath::Line2DIntersection(ExistAuxLine.StartPoint, ExistAuxLine.EndPoint, AuxLineIt.AuxiliaryPoint, FVector2D(NearPoint), InterPoint))
							{
								FVector2D PixPos0, PixPos1, PixPos2;
								InViewPortClient->WorldToPixel(FromBasePlane(FVector(InterPoint, 0)), PixPos0);
								InViewPortClient->WorldToPixel(FromBasePlane(FVector(AuxLineIt.AuxiliaryPoint, 0)), PixPos1);
								InViewPortClient->WorldToPixel(FromBasePlane(FVector(ExistAuxLine.AuxiliaryPoint, 0)), PixPos2);
								float dis1 = (PixPos1 - PixPos0).Size();
								float dis2 = (PixPos2 - PixPos0).Size();
								if (dis1 < 10 || dis2 < 10)
								{
									continue;
								}
								AuxiliaryCaptrue = true;
								IsCapture = true;
								CurrentCaptruePoint.CapturePoint = InterPoint;
								CurrentCaptruePoint.PointType = PT_Intersection;
								CurrentCaptruePoint.CapturePointPtr = NULL;
								ExistAuxLine.EndPoint = InterPoint;
								AuxiliaryLines.Add(AuxiliaryLineInfo(AuxLineIt.AuxiliaryPoint, InterPoint, AuxLineIt.AuxiliaryPoint, AuxLineIt.AuxiliaryDraw, AuxLineIt.IsFullScreen));
								return IsCapture;//发生两线相交则直接返回，不做再多线求交
							}
						}
					}
					else
					{
						AuxiliaryCaptrue = true;
						CurrentCaptruePoint.CapturePoint = FVector2D(NearPoint);
						CurrentCaptruePoint.PointType = PT_Intersection;
						CurrentCaptruePoint.CapturePointPtr = NULL;
						IsCapture = true;
						AuxiliaryLines.Add(AuxiliaryLineInfo(AuxLineIt.AuxiliaryPoint, CurrentCaptruePoint.CapturePoint, AuxLineIt.AuxiliaryPoint, AuxLineIt.AuxiliaryDraw, AuxLineIt.IsFullScreen));
					}
					if (AuxiliaryCaptrue)
					{
						break;
					}
				}
			}
		}
	}
	if (!IsCapture)
	{
		CurrentCaptruePoint.CapturePoint = CurrentMovePoint;
		CurrentCaptruePoint.PointType = PT_None;
		CurrentCaptruePoint.CapturePointPtr = NULL;
	}

    return IsCapture;
}
bool FArmyMouseCapture::OrthoAmend(UArmyEditorViewportClient* InViewPortClient,int32& x, int32& y)
{
	if (!CurrentFixPointInit) return false;
	FVector2D PixPos;
	InViewPortClient->WorldToPixel(FromBasePlane(FVector(CurrentFixPoint, 0)), PixPos);

	if ((PixPos - FVector2D(x,y)).Size() < 10)
	{
		CurrentMovePoint = CurrentFixPoint;
	}
	else if (FMath::Abs(x - PixPos.X) > FMath::Abs(y - PixPos.Y))
	{
#ifdef SEMI_ORTHO
		if (FMath::Abs(y - PixPos.Y) < 50)
		{
			FVector MWorldStart, MWorldDir;
			InViewPortClient->DeprojectFVector2D(FVector2D(x, y), MWorldStart, MWorldDir);
			FVector WorldPoint = FMath::LinePlaneIntersection(MWorldStart, MWorldStart + MWorldDir * ((InViewPortClient->GetViewLocation() - FVector::PointPlaneProject(InViewPortClient->GetViewLocation(), BasePlane)).Size() + 10), BasePlane);
			WorldPoint = ToBasePlane(WorldPoint);
			//FVector4 WorldPoint = InViewPortClient->PixelToWorld(x, y, 0);
			CurrentMovePoint.X = WorldPoint.X;
			CurrentMovePoint.Y = CurrentFixPoint.Y;
		}
#else
		//FVector4 WorldPoint = InViewPortClient->PixelToWorld(x, y, 0);
		FVector MWorldStart, MWorldDir;
		InViewPortClient->DeprojectFVector2D(FVector2D(x, y), MWorldStart, MWorldDir);
		FVector WorldPoint = FMath::LinePlaneIntersection(MWorldStart, MWorldStart + MWorldDir * ((InViewPortClient->GetViewLocation() - FVector::PointPlaneProject(InViewPortClient->GetViewLocation(), BasePlane)).Size() + 10), BasePlane);
		CurrentMovePoint.X = WorldPoint.X;
		CurrentMovePoint.Y = CurrentFixPoint.Y;
#endif // SEMI_ORTHO
	}
	else
	{
#ifdef SEMI_ORTHO
		if (FMath::Abs(x - PixPos.X) < 50)
		{
			FVector MWorldStart, MWorldDir;
			InViewPortClient->DeprojectFVector2D(FVector2D(x, y), MWorldStart, MWorldDir);
			FVector WorldPoint = FMath::LinePlaneIntersection(MWorldStart, MWorldStart + MWorldDir * ((InViewPortClient->GetViewLocation() - FVector::PointPlaneProject(InViewPortClient->GetViewLocation(), BasePlane)).Size() + 10), BasePlane);
			WorldPoint = ToBasePlane(WorldPoint);
			//FVector4 WorldPoint = InViewPortClient->PixelToWorld(x, y, 0);
			CurrentMovePoint.Y = WorldPoint.Y;
			CurrentMovePoint.X = CurrentFixPoint.X;
		}
#else
		FVector4 WorldPoint = InViewPortClient->PixelToWorld(x, y, 0);
		CurrentMovePoint.Y = WorldPoint.Y;
		CurrentMovePoint.X = CurrentFixPoint.X;
#endif
	}
	InViewPortClient->WorldToPixel(FromBasePlane(FVector(CurrentMovePoint, 0)), PixPos);
	x = PixPos.X;
	y = PixPos.Y;
    return true;
}
FVector FArmyMouseCapture::ToBasePlane(const FVector& InV)
{
	float x = FVector::DotProduct((InV - BasePoint), BaseXDir.GetSafeNormal());
	float y = FVector::DotProduct((InV - BasePoint), BaseYDir.GetSafeNormal());
	float z = FVector::DotProduct((InV - BasePoint), BaseNormal.GetSafeNormal());

	return FVector(x,y,z);
}
FVector FArmyMouseCapture::FromBasePlane(const FVector& InV)
{
	return BasePoint + InV.X * BaseXDir.GetSafeNormal() + InV.Y * BaseYDir.GetSafeNormal() + InV.Z * BaseNormal.GetSafeNormal();
}

void FArmyMouseCapture::AddAuxiliaryPerpendicular(FVector2D Start, FVector2D End)
{
	FVector2D PerpendicularPoint;//垂线终点
	PerpendicularPoint = FVector2D(-(End - Start).Y, (End - Start).X);

	CapturePointInfo TempCapturePoint;
	TempCapturePoint.PointType = PT_Perpendicular;
	TempCapturePoint.CapturePoint = End;

	int32 Index = AuxiliaryPoints.Find(TempCapturePoint);

	if (Index == INDEX_NONE)
	{
		TempCapturePoint.CapturePointPtr = nullptr;
		TempCapturePoint.AuxiliaryLineOK = true;
		TempCapturePoint.RelatedLines.Add(AuxiliaryLineInfo(TempCapturePoint.CapturePoint, TempCapturePoint.CapturePoint + PerpendicularPoint, TempCapturePoint.CapturePoint, true, true));
		AuxiliaryPoints.AddUnique(TempCapturePoint);
	}
	else
	{
		AuxiliaryPoints[Index].RelatedLines.Add(AuxiliaryLineInfo(TempCapturePoint.CapturePoint, TempCapturePoint.CapturePoint + PerpendicularPoint, TempCapturePoint.CapturePoint, true, true));
	}
}

