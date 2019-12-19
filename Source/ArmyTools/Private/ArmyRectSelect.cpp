#include "ArmyRectSelect.h"
#include "ArmyEditorViewportClient.h"
#include "Math/ArmyMath.h"
#include "Style/ArmyStyle.h"
#include "Engine/Canvas.h"
#include "ArmySceneData.h"

FArmyRectSelect::FArmyRectSelect() :CurrentState(0)
{
	SetRefCoordinateSystem(FVector(ForceInitToZero), FVector(1, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1));
	BIsConstruction = false;
}

FArmyRectSelect::~FArmyRectSelect()
{
}
void FArmyRectSelect::Draw(UArmyEditorViewportClient* InViewPortClient, FViewport* InViewport, FCanvas* SceneCanvas)
{
	if (CurrentState == 2)
	{
		float MinX = FMath::Min(RectFirstPoint.X, RectSecondPoint.X);
		float MinY = FMath::Min(RectFirstPoint.Y, RectSecondPoint.Y);

		float MaxX = FMath::Max(RectFirstPoint.X, RectSecondPoint.X);
		float MaxY = FMath::Max(RectFirstPoint.Y, RectSecondPoint.Y);

		FVector2D(FMath::Min(RectFirstPoint.X, RectSecondPoint.X), FMath::Min(RectFirstPoint.Y, RectSecondPoint.Y));


		FVector2D TempSize = RectSecondPoint - RectFirstPoint;

		FCanvasBoxItem RectBox(FVector2D(MinX, MinY), FVector2D(MaxX - MinX, MaxY - MinY));
		if (IntersectRect)
		{
			RectBox.SetColor(FLinearColor::Green);
		}
		else
		{
			RectBox.SetColor(FLinearColor(FColor(98, 152, 242)));
		}
		RectBox.Draw(SceneCanvas);
	}
}
void FArmyRectSelect::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (SelectedDrawPoints.Num() > 0)
	{
		for (auto V : SelectedDrawPoints)
		{
			V.Value->Draw(PDI, View);
		}
	}
	if (CurrnetHoverPoint.IsValid())
	{
		CurrnetHoverPoint->Draw(PDI, View);
	}
}
void FArmyRectSelect::AddToSelectedObject(FObjectWeakPtr InObj)
{
	if (PickOne)Clear();//temp
	InObj.Pin()->SetState(OS_Selected);
	SelectedObjects.AddUnique(InObj);
	TArray<FObjectPtr> ObjList;
	ObjList.Add(InObj.Pin());
	SelectedObjectsDelegate.ExecuteIfBound(ObjList);
}
void FArmyRectSelect::SetEnableGlobalHover(bool bEnable)
{
	bEnableGlobalHover = bEnable;
}
//void FArmyRectSelect::AddCanHoverObjects(ObjectInfo InObjInfo)
//{
//	AllCanHoverObjects.HeapPush(InObjInfo);
//}
//void FArmyRectSelect::RemoveCanHoverObjects(ObjectInfo InObjInfo)
//{
//	AllCanHoverObjects.Remove(InObjInfo);
//}
bool FArmyRectSelect::PickSelect(int32 X, int32 Y, UArmyEditorViewportClient* InViewPortClient, const TArray<FObjectWeakPtr>& InObjectArray, bool Subtract)
{
	if (!Subtract && CurrentOperationInfo.IsValid())
	{
		if (CurrentOperationInfo.CurrentOperationLine.IsValid())
		{
			FPrimitiveInfo Primitive(CurrentOperationInfo.CurrentOperationObject, CurrentOperationInfo.CurrentOperationLine);
			if (SelectedPrimitives.Find(Primitive) == INDEX_NONE)
			{
				TSharedPtr<"FArmyLine> SelectedLine = CurrentOperationInfo.CurrentOperationLine;
				if (PickOne)
					Clear();
				SelectedLine->SetState("FArmyPrimitive::OPS_Selected);
				SelectedLine->ShowEditPoint = true;
				SelectedPrimitives.AddUnique(Primitive);
			}
		}
		return true;
	}
	switch (Unit)
	{
	case FArmyRectSelect::Unit_Vertex:
	{
		for (auto Obj : InObjectArray)
		{
			if (FiltArray.Contains(Obj.Pin()->GetType()))
			{
				TArray<FVector> VertexArray;
				Obj.Pin()->GetVertexes(VertexArray);

				for (auto V : VertexArray)
				{
					FVector2D PixV;
					InViewPortClient->WorldToPixel(FromBasePlane(V), PixV);
					if ((PixV - FVector2D(X, Y)).Size() < 10)
					{
						if (Subtract)
						{
							SelectedVertexs.Remove(V);
						}
						else if (SelectedVertexs.Find(V) != INDEX_NONE)
						{
							continue;
						}
						else if (SelectedVertexs.Find(V) == INDEX_NONE)
						{
							if (PickOne)Clear();//temp
							SelectedVertexs.AddUnique(V);
						}
						return true;
					}
				}
			}
		}
	}
	break;
	case FArmyRectSelect::Unit_Primitive:
	{
		for (auto Obj : InObjectArray)
		{
			if (FiltArray.Contains(Obj.Pin()->GetType()))
			{
				TArray<TSharedPtr<"FArmyLine>> CurrentModelLines;
				Obj.Pin()->GetLines(CurrentModelLines);
				for (auto L : CurrentModelLines)
				{
					FVector2D StartPix, EndPix, CurrentPix(X, Y);
					InViewPortClient->WorldToPixel(FromBasePlane(L->GetStart()), StartPix);
					InViewPortClient->WorldToPixel(FromBasePlane(L->GetEnd()), EndPix);
					FVector NearPos = FMath::ClosestPointOnLine(FVector(StartPix, 0), FVector(EndPix, 0), FVector(CurrentPix, 0));
					if ((FVector2D(NearPos) - CurrentPix).Size() < 10)
					{
						FPrimitiveInfo CurrentPrimitive(Obj,L);
						if (Subtract)
						{
							SelectedPrimitives.Remove(CurrentPrimitive);
						}
						else if (SelectedPrimitives.Find(CurrentPrimitive) != INDEX_NONE)
						{
							continue;
						}
						else if (SelectedPrimitives.Find(CurrentPrimitive) == INDEX_NONE)
						{
							if (PickOne)Clear();//temp
							SelectedPrimitives.AddUnique(CurrentPrimitive);
						}
						return true;
					}
				}
				TArray<TSharedPtr<"FArmyCircle>> CurrentModelCircles;
				Obj.Pin()->GetCircleLines(CurrentModelCircles);
				for (auto C : CurrentModelCircles)
				{
					FVector2D CStartPix, CEndPix, CMidPix;
					InViewPortClient->WorldToPixel(FromBasePlane(C->GetStartPos()), CStartPix);
					InViewPortClient->WorldToPixel(FromBasePlane(C->GetEndPos()), CEndPix);
					InViewPortClient->WorldToPixel(FromBasePlane(C->GetArcMiddlePos()), CMidPix);
					TArray<FVector> InterPoints;
					FBox2D Box(FVector2D(X, Y) - FVector2D(10, 10), FVector2D(X, Y) + FVector2D(10, 10));
					if ("FArmyMath::ArcBoxIntersection(FVector(CStartPix, 0), FVector(CEndPix, 0), FVector(CMidPix, 0), Box, InterPoints))
					{
						FPrimitiveInfo CurrentPrimitive(Obj, C);
						if (Subtract)
						{
							SelectedPrimitives.Remove(CurrentPrimitive);
						}
						else if (SelectedPrimitives.Find(CurrentPrimitive) != INDEX_NONE)
						{
							continue;
						}
						else if (SelectedPrimitives.Find(CurrentPrimitive) == INDEX_NONE)
						{
							if (PickOne)Clear();//temp
							SelectedPrimitives.AddUnique(CurrentPrimitive);
						}
						return true;
					}
				}
			}
		}
	}
	break;
	case FArmyRectSelect::Unit_Object:
	{
		bool IsSelected = false;
		TArray<TSharedPtr<"FArmyObject>> SelectedObjectsPtr;
		for (auto Obj : InObjectArray)
		{
			if (FiltArray.Contains(Obj.Pin()->GetType()))
			{
				///////////////////////////////拣选Object///////////////////////////////////////////
				FVector CurrentWorldP = InViewPortClient->PixelToWorld(X, Y, 0);
				CurrentWorldP.Z = 0.0f;
				if (Obj.Pin()->IsSelected(CurrentWorldP, InViewPortClient))
				{
					IsSelected = true;
					//if (Subtract)
					//{
					//	Obj.Pin()->SetState(OS_Normal);
					//	SelectedObjects.Remove(Obj.Pin());
					//}
					//else
					{
                        if (PickOne&&!Subtract)
                        {
                            SelectedObjectsPtr.Reset();
                            Clear();
                        }
						SelectedObjectsPtr.Add(Obj.Pin());

						if (SelectedObjects.Find(Obj) != INDEX_NONE)//已选中
						{
							Obj.Pin()->SetState(OS_Normal);
							SelectedObjects.Remove(Obj.Pin());
							continue;
						}
						else//未选中
						{
							Obj.Pin()->SetState(OS_Selected);
							SelectedObjects.AddUnique(Obj);
						}

                        if (PickOne)
                        {
                            break;
                        }
					}
					break;
				}
			}
		}

		//将选中的Object用代理发出去，之所以用数组是为了后期扩展用
		SelectedObjectsDelegate.ExecuteIfBound(SelectedObjectsPtr);
		if (!IsSelected && !Subtract)
			Clear();
		return IsSelected;
	}
	break;
	default:
		break;
	}
	return false;
}
void FArmyRectSelect::LeftSelect(const FBox2D& InBox, UArmyEditorViewportClient* InViewPortClient, const TArray<FObjectWeakPtr>& InObjectArray, bool Subtract)
{
	TempSelectedVertexs.Empty();
	TempSelectedPrimitives.Empty();
	TempSelectedObjects.Empty();

	switch (Unit)
	{
	case FArmyRectSelect::Unit_Vertex:
	{
		for (auto Obj : InObjectArray)
		{
			if (FiltArray.Contains(Obj.Pin()->GetType()))
			{
				TArray<FVector> VertexArray;
				Obj.Pin()->GetVertexes(VertexArray);

				for (auto V : VertexArray)
				{
					FVector2D PixV;
					InViewPortClient->WorldToPixel(FromBasePlane(V), PixV);
					if (InBox.IsInside(PixV))
					{
						if (Subtract)
						{
							SelectedVertexs.Remove(V);
						}
						else if (SelectedVertexs.Find(V) == INDEX_NONE)
						{
							TempSelectedVertexs.AddUnique(V);
						}
					}
				}
			}
		}
	}
	break;
	case FArmyRectSelect::Unit_Primitive:
	{
		for (auto Obj : InObjectArray)
		{
			if (FiltArray.Contains(Obj.Pin()->GetType()))
			{
				TArray<TSharedPtr<"FArmyLine>> CurrentModelLines;
				Obj.Pin()->GetLines(CurrentModelLines);
				for (auto L : CurrentModelLines)
				{
					FVector2D StartPix, EndPix;
					InViewPortClient->WorldToPixel(FromBasePlane(L->GetStart()), StartPix);
					InViewPortClient->WorldToPixel(FromBasePlane(L->GetEnd()), EndPix);
					if ("FArmyMath::LineBox2DIntersection(StartPix, EndPix, InBox))
					{
						FPrimitiveInfo CurrentPrimitive(Obj, L);
						if (Subtract)
						{
							SelectedPrimitives.Remove(CurrentPrimitive);
						}
						else if (SelectedPrimitives.Find(CurrentPrimitive) == INDEX_NONE)
						{
							TempSelectedPrimitives.AddUnique(CurrentPrimitive);
						}
					}
				}
				TArray<TSharedPtr<"FArmyArcLine>> CurrentModelArcLines;
				Obj.Pin()->GetArcLines(CurrentModelArcLines);
				for (auto C : CurrentModelArcLines)
				{
					FVector2D CStartPix, CEndPix, CMidPix;
					InViewPortClient->WorldToPixel(FromBasePlane(C->GetStartPos()), CStartPix);
					InViewPortClient->WorldToPixel(FromBasePlane(C->GetEndPos()), CEndPix);
					InViewPortClient->WorldToPixel(FromBasePlane(C->GetArcMiddlePos()), CMidPix);
					TArray<FVector> InterPoints;
					if ("FArmyMath::ArcBoxIntersection(FVector(CStartPix, 0), FVector(CEndPix, 0), FVector(CMidPix, 0), InBox, InterPoints))
					{
						FPrimitiveInfo CurrentPrimitive(Obj, C);
						if (Subtract)
						{
							SelectedPrimitives.Remove(CurrentPrimitive);
						}
						else if (SelectedPrimitives.Find(CurrentPrimitive) == INDEX_NONE)
						{
							TempSelectedPrimitives.AddUnique(CurrentPrimitive);
						}
					}
				}
			}
		}
	}
	break;
	case FArmyRectSelect::Unit_Object:
	{
		for (auto Obj : InObjectArray)
		{
			if (FiltArray.Contains(Obj.Pin()->GetType()))
			{
				TArray< TSharedPtr<"FArmyLine> > ObjLines;
				TArray< TSharedPtr<"FArmyArcLine> > ObjArcLines;
				Obj.Pin()->GetLines(ObjLines);
				Obj.Pin()->GetArcLines(ObjArcLines);

				bool UseBondBox = true;
				if (ObjLines.Num() > 0)
				{
					UseBondBox = false;
					bool IsSelected = false;
					for (auto L : ObjLines)
					{
						FVector2D StartPix, EndPix;
						InViewPortClient->WorldToPixel(FromBasePlane(L->GetStart()), StartPix);
						InViewPortClient->WorldToPixel(FromBasePlane(L->GetEnd()), EndPix);

						if ("FArmyMath::LineBox2DIntersection(StartPix, EndPix, InBox))
						{
							if (SelectedObjects.Find(Obj) == INDEX_NONE)
							{
								TempSelectedObjects.AddUnique(Obj);
							}
							else//如果框选且已经选中，则取消选中
							{
								if (Subtract && Obj.Pin().IsValid())
								{
									Obj.Pin()->SetState(OS_Normal);
									SelectedObjects.Remove(Obj.Pin());
								}
							}
							IsSelected = true;
							break;
						}
					}
					if (IsSelected)
					{
						continue;
					}
				}
				if (ObjArcLines.Num() > 0)
				{
					UseBondBox = false;

					FVector2D CStartPix, CEndPix, CMidPix;
					bool IsSelected = false;
					for (auto C : ObjArcLines)
					{
						InViewPortClient->WorldToPixel(FromBasePlane(C->GetStartPos()), CStartPix);
						InViewPortClient->WorldToPixel(FromBasePlane(C->GetEndPos()), CEndPix);
						InViewPortClient->WorldToPixel(FromBasePlane(C->GetArcMiddlePos()), CMidPix);
						TArray<FVector> InterPoints;
						if ("FArmyMath::ArcBoxIntersection(FVector(CStartPix, 0), FVector(CEndPix, 0), FVector(CMidPix, 0), InBox, InterPoints))
						{
							if (Subtract)
							{
								Obj.Pin()->SetState(OS_Normal);
								SelectedObjects.Remove(Obj);
							}
							else if (SelectedObjects.Find(Obj) == INDEX_NONE)
							{
								TempSelectedObjects.AddUnique(Obj);
							}
							IsSelected = true;
							break;
						}
					}
					if (IsSelected)
					{
						continue;
					}
				}
				//////////////////////////////////////////////////////////////////////////
				if(UseBondBox)
				{
					FBox Box3D = Obj.Pin()->GetBounds();
					FVector2D MinPix, MaxPix;
					InViewPortClient->WorldToPixel(FromBasePlane(Box3D.Min), MinPix);
					InViewPortClient->WorldToPixel(FromBasePlane(Box3D.Max), MaxPix);

					if (InBox.Intersect(FBox2D(MinPix, MaxPix)))
					{
						if (SelectedObjects.Find(Obj) == INDEX_NONE)
						{
							//Obj->SetState(OS_Selected);
							TempSelectedObjects.AddUnique(Obj);
						}
						else//如果框选且已经选中，则取消选中
						{
							if (Subtract && Obj.Pin().IsValid())
							{
								Obj.Pin()->SetState(OS_Normal);
								SelectedObjects.Remove(Obj.Pin());
							}
						}
					}
				}
			}
		}
	}
	break;
	default:
		break;
	}
}
void FArmyRectSelect::RightSelect(const FBox2D& InBox, UArmyEditorViewportClient* InViewPortClient, const TArray<FObjectWeakPtr>& InObjectArray, bool Subtract)
{
	TempSelectedVertexs.Empty();
	TempSelectedPrimitives.Empty();
	TempSelectedObjects.Empty();
	switch (Unit)
	{
	case FArmyRectSelect::Unit_Vertex:
	{
		for (auto Obj : InObjectArray)
		{
			if (FiltArray.Contains(Obj.Pin()->GetType()))
			{
				TArray<FVector> VertexArray;
				Obj.Pin()->GetVertexes(VertexArray);
				for (auto V : VertexArray)
				{
					FVector2D PixV;
					InViewPortClient->WorldToPixel(FromBasePlane(V), PixV);
					if (InBox.IsInside(PixV))
					{
						if (Subtract)
						{
							SelectedVertexs.Remove(V);
						}
						else if (SelectedVertexs.Find(V) == INDEX_NONE)
						{
							TempSelectedVertexs.AddUnique(V);
						}
					}
				}
			}
		}
	}
	break;
	case FArmyRectSelect::Unit_Primitive:
	{
		for (auto Obj : InObjectArray)
		{
			if (FiltArray.Contains(Obj.Pin()->GetType()))
			{
				TArray<TSharedPtr<"FArmyLine>> CurrentModelLines;
				Obj.Pin()->GetLines(CurrentModelLines);
				for (auto L : CurrentModelLines)
				{
					FVector2D StartPix, EndPix;
					InViewPortClient->WorldToPixel(FromBasePlane(L->GetStart()), StartPix);
					InViewPortClient->WorldToPixel(FromBasePlane(L->GetEnd()), EndPix);
					if (InBox.IsInside(StartPix) && InBox.IsInside(EndPix))
					{
						FPrimitiveInfo CurrentPrimitive(Obj, L);
						if (Subtract)
						{
							SelectedPrimitives.Remove(CurrentPrimitive);
						}
						else if (SelectedPrimitives.Find(CurrentPrimitive) == INDEX_NONE)
						{
							TempSelectedPrimitives.AddUnique(CurrentPrimitive);
						}
					}
				}
				TArray<TSharedPtr<"FArmyArcLine>> CurrentModelArcLines;
				Obj.Pin()->GetArcLines(CurrentModelArcLines);
				for (auto C : CurrentModelArcLines)
				{
					FVector2D CStartPix, CEndPix, CMidPix;
					InViewPortClient->WorldToPixel(FromBasePlane(C->GetStartPos()), CStartPix);
					InViewPortClient->WorldToPixel(FromBasePlane(C->GetEndPos()), CEndPix);
					InViewPortClient->WorldToPixel(FromBasePlane(C->GetArcMiddlePos()), CMidPix);
					TArray<FVector> InterPoints;
					if (InBox.IsInside(CStartPix) && InBox.IsInside(CEndPix) && InBox.IsInside(CMidPix))
					{
						FPrimitiveInfo CurrentPrimitive(Obj, C);
						if (Subtract)
						{
							SelectedPrimitives.Remove(CurrentPrimitive);
						}
						else if (SelectedPrimitives.Find(CurrentPrimitive) == INDEX_NONE)
						{
							TempSelectedPrimitives.AddUnique(CurrentPrimitive);
						}
					}
				}
			}
		}
	}
	break;
	case FArmyRectSelect::Unit_Object:
	{
		for (auto Obj : InObjectArray)
		{
			if (FiltArray.Contains(Obj.Pin()->GetType()))
			{
				FBox Box3D = Obj.Pin()->GetBounds();
				FVector2D MinPix, MaxPix;
				InViewPortClient->WorldToPixel(FromBasePlane(Box3D.Min), MinPix);
				InViewPortClient->WorldToPixel(FromBasePlane(Box3D.Max), MaxPix);

				if (InBox.IsInside(FBox2D(MinPix, MaxPix)))
				{
					if (SelectedObjects.Find(Obj) == INDEX_NONE)
					{
						//Obj->SetState(OS_Selected);
						TempSelectedObjects.AddUnique(Obj);
					}
					else//如果框选且已经选中，则取消选中
					{
						if (Subtract && Obj.Pin().IsValid())
						{
							Obj.Pin()->SetState(OS_Normal);
							SelectedObjects.Remove(Obj.Pin());
						}
					}
				}
			}
		}
	}
	break;
	default:
		break;
	}
}
bool FArmyRectSelect::MouseMove(UArmyEditorViewportClient* InViewPortClient, int32 X, int32 Y, const TArray<ObjectInfo>& InCanHoverObjects)
{
	if(CurrentState > 0 && SelectedObjects.Num() > 0)
	{
		CurrentOperationInfo.Reset();

		FVector MousePos(InViewPortClient->PixelToWorld(X, Y, 0));
		for (auto Obj : SelectedObjects)
		{
			if (!Obj.IsValid())
				return false;
			TSharedPtr<"FArmyEditPoint> HoverPoint = Obj.Pin()->HoverPoint(MousePos, InViewPortClient);
			if (HoverPoint.IsValid())
			{
				HoverPoint->SetState("FArmyPrimitive::OPS_Hovered);
				CurrentOperationInfo.CurrentOperationObject = Obj;
				CurrentOperationInfo.CurrentOperationPoint = HoverPoint;
				return true;
			}
			else
			{
				TSharedPtr<"FArmyLine> HoverLine = Obj.Pin()->HoverLine(MousePos, InViewPortClient);
				if (HoverLine.IsValid())
				{
					HoverLine->SetState("FArmyPrimitive::OPS_Hovered);
					CurrentOperationInfo.CurrentOperationObject = Obj;
					CurrentOperationInfo.CurrentOperationLine = HoverLine;
					return true;
				}
				else if(Obj.Pin()->Hover(MousePos,InViewPortClient))
				{
					//Obj.Pin()->SetState(OS_Hovered);
					CurrentOperationInfo.CurrentOperationObject = Obj;
					return true;
				}
			}
		}
	}
	else if(BIsConstruction)
	{
		return false;
	}
	else {
		CurrentOperationInfo.Reset();
		FVector MousePos(InViewPortClient->PixelToWorld(X, Y, 0));
		MousePos.Z = 0;

		if (SelectedPrimitives.Num() > 0)
		{
			for (auto& Primitive : SelectedPrimitives)
			{
				if (!Primitive.Primitive.IsValid())
					return false;
				TSharedPtr<"FArmyEditPoint> HoverPoint = Primitive.Primitive->SelectEditPoint(MousePos, InViewPortClient);
				if (HoverPoint.IsValid())
				{
					HoverPoint->SetState("FArmyPrimitive::OPS_Hovered);
					CurrentOperationInfo.CurrentOperationObject = Primitive.Obj;
					CurrentOperationInfo.CurrentOperationPoint = HoverPoint;
					if (dynamic_cast<"FArmyLine*>(Primitive.Primitive.Get()))
					{
						CurrentOperationInfo.CurrentOperationLine = StaticCastSharedPtr<"FArmyLine>(Primitive.Primitive);
					}
					return true;
				}
			}
		}
		if (!bEnableGlobalHover)
		{
			return false;
		}
		//@刘克祥 全局线捕捉			
		for (auto & It : InCanHoverObjects)
		{
			CurrentOperationInfo.Reset();

			FVector MousePos(InViewPortClient->PixelToWorld(X, Y, 0));
			MousePos.Z = 0;
			if (!It.Obj.IsValid())
				continue;

			if (!It.NeedHoverLine)
			{
				if (It.Obj.Pin()->IsSelected(MousePos, InViewPortClient))
				{
					return false;
				}
			}

			TSharedPtr<"FArmyLine> HoverLine = It.Obj.Pin()->HoverLine(MousePos, InViewPortClient);
			if (HoverLine.IsValid())
			{
				if (HoverLine->GetState() == "FArmyPrimitive::OPS_Selected && !"FArmySceneData::Get()->GetHasOutWall())
				{										
					/**@刘克祥捕捉线体上的操作点*/
					if (HoverLine->GetLineEditPoint()->IsSelected(MousePos, InViewPortClient))
					{
						CurrnetHoverPoint = HoverLine->GetLineEditPoint();
						CurrnetHoverPoint->SetState("FArmyPrimitive::OPS_Hovered);
						CurrentOperationInfo.CurrentOperationPoint = CurrnetHoverPoint;
						CurrentOperationInfo.CurrentOperationObject = It.Obj;
						CurrentOperationInfo.CurrentOperationLine = HoverLine;
						return true;
					}
					else
					{
						CurrnetHoverPoint = It.Obj.Pin()->HoverPoint(MousePos, InViewPortClient);
						if (CurrnetHoverPoint.IsValid())
						{
							CurrnetHoverPoint->SetState("FArmyPrimitive::OPS_Hovered);
							CurrentOperationInfo.CurrentOperationObject = It.Obj;
							CurrentOperationInfo.CurrentOperationPoint = CurrnetHoverPoint;
							CurrentOperationInfo.CurrentOperationLine = HoverLine;
							return true;
						}						
					}
					
				}
				if (HoverLine->GetState() == "FArmyPrimitive::OPS_Normal)
				{
					HoverLine->SetState("FArmyPrimitive::OPS_Hovered);
				}
				CurrentOperationInfo.CurrentOperationObject = It.Obj;
				CurrentOperationInfo.CurrentOperationLine = HoverLine;
				return true;
			}
			/*CurrnetHoverPoint = "FArmySceneData::Get()->GetHasOutWall() ? nullptr : It.Pin()->HoverPoint(MousePos, InViewPortClient);
			if (CurrnetHoverPoint.IsValid() && !"FArmySceneData::Get()->GetHasOutWall())
			{				
				CurrnetHoverPoint->SetState("FArmyPrimitive::OPS_Hovered);
				CurrentOperationInfo.CurrentOperationObject = It;
				CurrentOperationInfo.CurrentOperationPoint = CurrnetHoverPoint;
				return true;
			}*/							
		}		
	}
	return true;
}

bool FArmyRectSelect::MouseDrag(FViewport* Viewport, FKey Key)
{
	if (CurrentState == 2)
	{
		RectSecondPoint = FVector2D(Viewport->GetMouseX(), Viewport->GetMouseY());

		if (RectFirstPoint.X > RectSecondPoint.X)
		{
			IntersectRect = true;//左选
		}
		else
		{
			IntersectRect = false;
		}
		return true;
	}

	return false;
}

bool FArmyRectSelect::LeftMouseButtonPress(UArmyEditorViewportClient* InViewPortClient, int32 X, int32 Y, const TArray<FObjectWeakPtr>& InObjectArray, bool IsPressCtl)
{
	if (CurrentState == 1)
	{
		if (!PickSelect(X, Y, InViewPortClient, InObjectArray, IsPressCtl))
		{
			RectFirstPoint.X = X;
			RectFirstPoint.Y = Y;

			RectSecondPoint.X = X;
			RectSecondPoint.Y = Y;
			CurrentState = 2;
		}
	}
	else if (CurrentState == 2)
	{
		if (RectFirstPoint.X > RectSecondPoint.X)
		{
			IntersectRect = true;//左选
		}
		else
		{
			IntersectRect = false;
		}

		float MinX = RectFirstPoint.X;
		float MaxX = RectSecondPoint.X;
		float MinY = RectFirstPoint.Y;
		float MaxY = RectSecondPoint.Y;
		if (MinX > MaxX)
		{
			MinX = RectSecondPoint.X;
			MaxX = RectFirstPoint.X;
		}
		if (MinY > MaxY)
		{
			MinY = RectSecondPoint.Y;
			MaxY = RectFirstPoint.Y;
		}
		FBox2D RectBox(FVector2D(MinX, MinY), FVector2D(MaxX, MaxY));

		if (IntersectRect)
		{
			LeftSelect(RectBox, InViewPortClient, InObjectArray, IsPressCtl);
		}
		else
		{
			RightSelect(RectBox, InViewPortClient, InObjectArray, IsPressCtl);
		}

		SelectedVertexs.Append(TempSelectedVertexs);
		for (auto V : TempSelectedVertexs)
		{
			TSharedPtr<"FArmyPoint> P = MakeShareable(new "FArmyPoint);
			P->SetBaseColor(FLinearColor::Red);
			P->Size = 4;
			P->Pos = V;
			SelectedDrawPoints.Add(V, P);
		}
		SelectedPrimitives.Append(TempSelectedPrimitives);
		for (auto& PIt : SelectedPrimitives)
		{
			PIt.Primitive->UpdateEditPoint();
			PIt.Primitive->ShowEditPoint = true;
		}
		SelectedObjects.Append(TempSelectedObjects);
		for (auto ObjIt : TempSelectedObjects)
		{
			ObjIt.Pin()->SetState(OS_Selected);
		}

		if (MultipleSelect)
		{
			CurrentState = 1;//可多次选择
		}
		else
		{
			CurrentState = 0;
		}
	}
	return false;
}
void FArmyRectSelect::Start()
{
	CurrentState == 0 ? CurrentState = 1 : CurrentState;
}
void FArmyRectSelect::End()
{
	CurrentState = 0;
}
void FArmyRectSelect::Clear()
{
	for (auto ObjIt : SelectedObjects)
	{
		if (ObjIt.IsValid())
		{
			ObjIt.Pin()->SetState(OS_Normal);
		}
	}
	for (auto& PIt : SelectedPrimitives)
	{
		PIt.Primitive->SetState("FArmyPrimitive::OPS_Normal);
		PIt.Primitive->ShowEditPoint = false;
	}
	SelectedVertexs.Empty();
	SelectedPrimitives.Empty();
	SelectedObjects.Empty();
	SelectedDrawPoints.Empty();
	CurrentOperationInfo.Reset();	
	CurrnetHoverPoint = nullptr;
	//@刘克祥 发送一个空数组以便属性面板得知当前已被清空
	TArray<TSharedPtr<"FArmyObject>> SelectedObjectsPtr;
	SelectedObjectsDelegate.ExecuteIfBound(SelectedObjectsPtr);
}
void FArmyRectSelect::AddFiltType(EObjectType InObjType)
{
	FiltArray.Add(InObjType);
}
void FArmyRectSelect::SetSelectUnit(UnitType InUnit)
{
	Unit = InUnit;
}
const TArray<FVector>& FArmyRectSelect::GetSelectedVertexs() const
{
	return SelectedVertexs;
}
const TArray<FArmyRectSelect::FPrimitiveInfo>& FArmyRectSelect::GetSelectedPrimitives() const
{
	return SelectedPrimitives;
}
const TArray<FObjectWeakPtr>& FArmyRectSelect::GetSelectedObjects() const
{
	return SelectedObjects;
}
const FArmyRectSelect::OperationInfo& FArmyRectSelect::GetCurrentOperationInfo() const
{
	return CurrentOperationInfo;
}
void FArmyRectSelect::SetRefCoordinateSystem(const FVector& InBasePos, const FVector& InXDir, const FVector& InYDir, const FVector& InNormal)
{
	BasePoint = InBasePos;
	BaseXDir = InXDir;
	BaseYDir = InYDir;
	BaseNormal = InNormal;
	BasePlane = FPlane(BasePoint, BaseNormal);
}

FVector FArmyRectSelect::ToBasePlane(const FVector& InV)
{
	float x = FVector::DotProduct((InV - BasePoint), BaseXDir.GetSafeNormal());
	float y = FVector::DotProduct((InV - BasePoint), BaseYDir.GetSafeNormal());
	float z = FVector::DotProduct((InV - BasePoint), BaseNormal.GetSafeNormal());

	return FVector(x, y, z);
}
FVector FArmyRectSelect::FromBasePlane(const FVector& InV)
{
	return BasePoint + InV.X * BaseXDir.GetSafeNormal() + InV.Y * BaseYDir.GetSafeNormal() + InV.Z * BaseNormal.GetSafeNormal();
}
const FPlane& FArmyRectSelect::GetPlane() const
{
	return BasePlane;
}