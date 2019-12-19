#include "ArmyLampControlLines.h" 
#include "ArmyEditorViewportClient.h"
#include "ArmyMath.h"
#include "ArmyEditPoint.h"
#include "ArmyRectArea.h"
#include "ArmyPolygon.h"



void FArmyLampControlLines::FControlLine::SerializeToJson(TSharedRef<TJsonWriter<TCHAR,TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	JsonWriter->WriteArrayStart("arcLines");
	for (auto& ArcLine : ControlLines)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("start",ArcLine->GetStartPos().ToString());
		JsonWriter->WriteValue("middle",ArcLine->GetArcMiddlePos().ToString());
		JsonWriter->WriteValue("end",ArcLine->GetEndPos().ToString());
		JsonWriter->WriteValue("color",ArcLine->GetBaseColor().ToString());
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
}

void FArmyLampControlLines::FControlLine::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	const TArray< TSharedPtr<FJsonValue> > controlInfoes = InJsonData->GetArrayField("arcLines");
	for (auto& Info:controlInfoes)
	{
		TSharedPtr<FJsonObject> JsonObject = Info->AsObject();
		TSharedPtr<FArmyArcLine> ArcLine = MakeShareable(new FArmyArcLine);
		FString start = JsonObject->GetStringField("start");
		FString end = JsonObject->GetStringField("end");
		FString middle = JsonObject->GetStringField("middle");
		FString color = JsonObject->GetStringField("color");
		FVector startpos,endpos,middlepos;
		FLinearColor Color;
		Color.InitFromString(color);
		startpos.InitFromString(start);
		endpos.InitFromString(end);
		middlepos.InitFromString(middle);
		ArcLine->SetStartPos(startpos);
		ArcLine->SetArcMiddlePos(middlepos);
		ArcLine->SetEndPos(endpos);
		ArcLine->SetBaseColor(Color);
		ControlLines.Add(ArcLine);
	}
}

void FArmyLampControlLines::LampControlInfo::SerializeToJson(TSharedRef<TJsonWriter<TCHAR,TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	JsonWriter->WriteArrayStart("controllInfoes");
	for (auto& ControlInfo : ControlLampLineArray)
	{
		JsonWriter->WriteObjectStart();
		ControlInfo.SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
}

void FArmyLampControlLines::LampControlInfo::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	const TArray< TSharedPtr<FJsonValue> > controlInfoes = InJsonData->GetArrayField("controllInfoes");
	for (auto& Info : controlInfoes)
	{
		TSharedPtr<FJsonObject> JsonObject =  Info->AsObject();
		FControlLine Line ;
		Line.Deserialization(JsonObject);
		ControlLampLineArray.Add(Line);
	}
}


FArmyLampControlLines::FArmyLampControlLines():
	SelectArcLine(nullptr)
{
	ObjectType = OT_LampControlLine;//OT_LampControlLine 待添加
	EditPoint = MakeShareable(new FArmyEditPoint());
	EditPoint->OperationType = XROP_MOVE;
}
FArmyLampControlLines::~FArmyLampControlLines() 
{

}
void FArmyLampControlLines::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
	JsonWriter->WriteArrayStart("ControlLines");
	for (auto& ControlInfo : LampControlInfoArray)
	{
		JsonWriter->WriteObjectStart();
		ControlInfo.SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
	SERIALIZEREGISTERCLASS(JsonWriter, FArmyLampControlLines)
}

void FArmyLampControlLines::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);
	const TArray< TSharedPtr<FJsonValue> > controlInfoes = InJsonData->GetArrayField("ControlLines");
	for (auto& Info:controlInfoes)
	{
		TSharedPtr<FJsonObject> JsonObject = Info->AsObject();
		LampControlInfo Line;
		Line.Deserialization(JsonObject);
		LampControlInfoArray.Add(Line);
	}
}

void FArmyLampControlLines::SetState(EObjectState InState)
{
	State = InState;

	switch (InState)
	{
	case OS_Normal:
		EditPoint->SetState(FArmyEditPoint::OPS_Normal);
		break;

	case OS_Hovered:
		break;

	case OS_Selected:
		EditPoint->SetState(FArmyEditPoint::OPS_Normal);
		break;

	case OS_Disable:
		EditPoint->SetState(FArmyEditPoint::OPS_Normal);
		break;

	default:
		break;
	}
}

void FArmyLampControlLines::ApplyTransform(const FTransform& Trans)
{
	if (SelectArcLine.IsValid())
	{
		
	}
}

void FArmyLampControlLines::Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View) 
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		for (auto& LLInfo : LampControlInfoArray)
		{
			for (auto & LInfo : LLInfo.ControlLampLineArray)
			{
				for (auto L : LInfo.ControlLines)
				{
					L->Draw(PDI, View);
				}
			}
		}
		if (GetState()==OS_Selected)
		{
			EditPoint->Draw(PDI,View);
		}
	}
}
FArmyLampControlLines::LampControlInfo& FArmyLampControlLines::GetLampControlInfo(FObjectWeakPtr InSwitch, bool InCreate)
{
	if (!InCreate)
	{
		int32 Index = LampControlInfoArray.Find(LampControlInfo(InSwitch));
		return LampControlInfoArray[Index];
	}
	else
	{
		int32 Index = LampControlInfoArray.AddUnique(LampControlInfo(InSwitch));
		return LampControlInfoArray[Index];
	}
}
void FArmyLampControlLines::Refresh()
{
	Reset();
	//灯控线的算法  通过LampControlInfoArray 内的关联关系生成 FControlLine结构内的ControlLines控制线信息
	TempPointes.Empty();
	for (LampControlInfo& nLampControlInfo:LampControlInfoArray)
	{
		FObjectWeakPtr SwitchObjectPtr = nLampControlInfo.SwitchObj;
		if (SwitchObjectPtr.IsValid())
		{
			AActor* SwitchActor = SwitchObjectPtr.Pin()->GetRelevanceActor();
			if (SwitchActor&&SwitchActor->IsValidLowLevel())
			{
				FVector Location = SwitchActor->GetActorLocation();
				Location.Z = 0;
				FVector2D Location2D(Location);
				for (auto& ControlLineItem:nLampControlInfo.ControlLampLineArray)
				{
					TArray<FVector> Pointes;
					for (auto& LightObjct:ControlLineItem.ContrlLampArray)
					{
						//@郭子阳 6.13临时解决空指针问题 ，花恩欠我一顿饭
						if (!LightObjct.IsValid())
						{
							continue;
						}
						FVector End(ForceInit);
						if (LightObjct.Pin()->GetType()==OT_ComponentBase)
						{
							End = LightObjct.Pin()->GetBasePos();
							//TSharedPtr<FArmyFurniture> LightFurture = StaticCastSharedPtr<FArmyFurniture>(LightObjct.Pin());
							//End = LightFurture->GetRelevanceActor()->GetActorLocation();
						}
						else if (LightObjct.Pin()->GetType()==OT_RectArea)
						{
							TSharedPtr<FArmyRectArea> LightFurture = StaticCastSharedPtr<FArmyRectArea>(LightObjct.Pin());

							FVector MinVector = LightFurture->GetLampLightDashLines()[0];
							float Distance = (Location-MinVector).Size();
							for (int32 i = 0;i<LightFurture->GetLampLightDashLines().Num();i++)
							{
								FVector CurPoint = LightFurture->GetLampLightDashLines()[i];
								float Lenght = (CurPoint-Location).Size();
								if (Distance>Lenght)
								{
									Distance = Lenght;
									MinVector = CurPoint;
								}
							}
							End = MinVector;
						}
						else if (LightObjct.Pin()->GetType()==OT_CircleArea)
						{
							TSharedPtr<FArmyCircleArea> LightFurture = StaticCastSharedPtr<FArmyCircleArea>(LightObjct.Pin());
							FVector Center = LightFurture->GetPosition3D();
							Center.Z = 0;
							float Radiuse = LightFurture->GetRadius();
							FVector Direction = (FVector(Location2D,0)-Center).GetSafeNormal();
							End = Center+Direction*Radiuse;
						}
						else if (LightObjct.Pin()->GetType()==OT_PolygonArea||LightObjct.Pin()->GetType()==OT_FreePolygonArea)
						{
							TSharedPtr<FArmyBaseArea> LightFurture = StaticCastSharedPtr<FArmyBaseArea>(LightObjct.Pin());
							int number = LightFurture->GetOutArea()->Vertices.Num();

							TArray<FVector> Positiones;
							for (int i = 0; i<number; ++i)
							{
								FVector start = LightFurture->GetOutArea()->Vertices[i%number].X* LightFurture->GetXDir()+LightFurture->GetOutArea()->Vertices[i%number].Y * LightFurture->GetYDir()+LightFurture->GetPlaneCenter();
								Positiones.Add(start);
							}
							FVector MinVector = Positiones[0];
							float Distance = (Location-MinVector).Size();
							for (int32 i = 0;i<Positiones.Num();i++)
							{
								FVector CurPoint = Positiones[i];
								float Lenght = (CurPoint-Location).Size();
								if (Distance>Lenght)
								{
									Distance = Lenght;
									MinVector = CurPoint;
								}
							}
							End = MinVector;
						}
						Pointes.Add(FVector(End.X, End.Y, 0));
					}
					if (Pointes.Num() == 0) continue;
#if 0
					TArray<float> XArray,YArray;
					int32 XFront = 0,XBack = 0,YFront = 0,YBack = 0;
					FVector MinPoint = Pointes[0];
					for (auto& V : Pointes)
					{
						XArray.AddUnique(V.X);
						YArray.AddUnique(V.Y);

						FVector Offset(V - Location);
						Offset.X > 0 ? XFront++ : XBack++;
						Offset.Y > 0 ? YFront++ : YBack++;

						if ((V - Location).Size() < (MinPoint - Location).Size())
						{
							MinPoint = V;
						}
					}
					FVector XDir = XFront > XBack ? FVector(1,0,0) : FVector(-1,0,0);
					FVector YDir = YFront > YBack ? FVector(0, 1, 0) : FVector(0, -1, 0);

					XArray.Sort([&](const float& A,const float& B) {
						return XDir.X > 0 ? A < B : A > B;
					});
					YArray.Sort([&](const float& A, const float& B) {
						return YDir.Y > 0 ? A < B : A > B;
					});

					float SplitDistanceX = XArray.Num() > 1 ? 0 : 100, SplitDistanceY = YArray.Num() > 1 ? 0 : 100;
					for (int32 i = 0;i < XArray.Num() - 1;++i)
					{
						float Offset = FMath::Abs(XArray[i] - XArray[i + 1]);
						SplitDistanceX += Offset;
					}
					SplitDistanceX /= XArray.Num() > 1 ? (XArray.Num() - 1) : 1;
					SplitDistanceX > 100 ? SplitDistanceX = 100 : true;
					for (int32 i = 0; i < YArray.Num() - 1; ++i)
					{
						float Offset = FMath::Abs(YArray[i] - YArray[i + 1]);
						SplitDistanceY += Offset;
					}
					SplitDistanceY /= YArray.Num() > 1 ? (YArray.Num() - 1) : 1;
					SplitDistanceY > 100 ? SplitDistanceY = 100 : true;
					TArray<float> XSplitArray, YSplitArray;
					for (int32 i = 0;i < XArray.Num() - 1;++i)
					{
						float V1 = XArray[i];
						float V2 = XArray[i + 1];

						if (FMath::Abs(V2 - V1) > SplitDistanceX)
						{
							XSplitArray.Add(V1);
						}
					}
					XSplitArray.Add(XArray.Last());

					for (int32 i = 0; i < YArray.Num() - 1; ++i)
					{
						float V1 = YArray[i];
						float V2 = YArray[i + 1];

						if (FMath::Abs(V2 - V1) > SplitDistanceY)
						{
							YSplitArray.Add(V1);
						}
					}
					YSplitArray.Add(YArray.Last());

					TArray<FVector> PointArray;
					if (XSplitArray.Num() > YSplitArray.Num() || (XSplitArray.Num() == YSplitArray.Num() && XArray.Num() > YArray.Num()))
					{
						for (int32 i = 0; i < YSplitArray.Num(); ++i)
						{
							float SplitY1 = i == 0 ? YArray.HeapTop() - YDir.Y : YSplitArray[i - 1];
							float SplitY2 = YSplitArray[i];

							TArray<FVector> XPointArray;
							for (int32 j = 0; j < XArray.Num(); ++j)
							{
								for (int32 k = 0; k < YArray.Num(); ++k)
								{
									float Y = YArray[k];

									if ((YDir.Y > 0 && Y <= SplitY2 && Y > SplitY1) || (YDir.Y < 0 && Y >= SplitY2 && Y < SplitY1))
									{
										FVector V(XArray[j], Y, 0);
										if (Pointes.Contains(V))
										{
											if (i % 2 == 0)
											{
												XPointArray.Add(V);
											}
											else
											{
												XPointArray.Insert(V, 0);
											}
										}
									}
								}
							}
							PointArray.Append(XPointArray);
						}
					}
					else
					{
						for (int32 i = 0; i < XSplitArray.Num(); ++i)
						{
							float SplitX1 = i == 0 ? XArray.HeapTop() - XDir.X : XSplitArray[i - 1];
							float SplitX2 = XSplitArray[i];

							TArray<FVector> YPointArray;

							for (int32 j = 0; j < YArray.Num(); ++j)
							{
								for (int32 k = 0; k < XArray.Num(); ++k)
								{
									float X = XArray[k];

									if ((XDir.X > 0 && X <= SplitX2 && X > SplitX1) || (XDir.X < 0 && X >= SplitX2 && X < SplitX1))
									{
										FVector V(X, YArray[j], 0);
										if (Pointes.Contains(V))
										{
											if (i % 2 == 0)
											{
												YPointArray.Add(V);
											}
											else
											{
												YPointArray.Insert(V, 0);
												//XPointArray.HeapPush();
											}
										}
									}
								}
							}
							PointArray.Append(YPointArray);
						}
					}

					int32 StartIndex = PointArray.Find(MinPoint);

					PointArray.Insert(Location, StartIndex);
					for (int32 i = StartIndex; i<PointArray.Num() - 1; i++)
					{
						FVector Start = PointArray[i];
						FVector End = PointArray[i + 1];

						FVector Middle = (Start + End) / 2;
						FVector Dir1 = (End - Start).GetSafeNormal();
						FVector Normal = FVector::CrossProduct(Dir1, FVector::UpVector);
						TSharedPtr<FArmyArcLine> Line = MakeShareable(new FArmyArcLine());
						Line->SetTreePointPos(Start, End, Middle + 10 * Normal);
						Line->SetBaseColor(FLinearColor::Red);
						ControlLineItem.ControlLines.Add(Line);
					}
					for (int32 i = -1; i < StartIndex - 1; i++)
					{
						FVector Start = i == -1 ? PointArray.Last() : PointArray[i];
						FVector End = PointArray[i + 1];

						FVector Middle = (Start + End) / 2;
						FVector Dir1 = (End - Start).GetSafeNormal();
						FVector Normal = FVector::CrossProduct(Dir1, FVector::UpVector);
						TSharedPtr<FArmyArcLine> Line = MakeShareable(new FArmyArcLine());
						Line->SetTreePointPos(Start, End, Middle + 10 * Normal);
						Line->SetBaseColor(FLinearColor::Red);
						ControlLineItem.ControlLines.Add(Line);
					}
#endif
#if 1
					Pointes.Sort([&](const FVector& A,const FVector& B)
					{
						return (A - Location).Size2D() < (B - Location).Size2D();
					});
					FVector ClosePoint = Pointes.HeapTop();
					Pointes.Remove(ClosePoint);
					
					TArray<FVector> PointArray;
					for (int32 Tolerance = 0; Tolerance < 200; Tolerance += 5)
					{
						TArray<FVector> Out{ ClosePoint };
						TArray<FVector> TempPoints = Pointes;

						FindEnvelopes(TempPoints, ClosePoint, Out, Tolerance);

						if (PointArray.Num() == 0)
						{
							PointArray.Append(Out);
						}
						else
						{
							auto CountlinesLength = [](TArray<FVector> InPointArray) {

								float Len = 0;
								for (int Index = 0; Index < InPointArray.Num() - 1; Index++)
								{
									Len += FVector::Dist2D(InPointArray[Index], InPointArray[Index + 1]);
								}
								return Len;
							};

							float Distance1 = CountlinesLength(PointArray);
							float Distance2 = CountlinesLength(Out);
							if (Distance2 < Distance1)
							{
								PointArray.Empty();
								PointArray.Append(Out);
							}
						}
					}
					PointArray.Insert(Location, 0);

					for (int32 i = 0; i<PointArray.Num() - 1; i++)
					{
						FVector Start = PointArray[i];
						FVector End = PointArray[i + 1];

						FVector Middle = (Start + End) / 2;
						FVector Dir1 = (End - Start).GetSafeNormal();
						FVector Normal = FVector::CrossProduct(Dir1, FVector::UpVector);
						TSharedPtr<FArmyArcLine> Line = MakeShareable(new FArmyArcLine());
						Line->SetTreePointPos(Start, End, Middle + 10 * Normal);
						Line->SetBaseColor(FLinearColor::Red);
						ControlLineItem.ControlLines.Add(Line);
					}
#endif // 2
				}
			}
		}
	}

}
void FArmyLampControlLines::FindEnvelopes(TArray<FVector>& InPoints, FVector InStartPoint, TArray<FVector>& OutEnvelope, int32 AttachDistance)
{
	for (auto Slectedpoint : InPoints)
	{
		bool HasPointOnOneside = false;
		bool HasPointOnAnotherside = false;

		FVector NewLineVector = Slectedpoint - InStartPoint;
		for (auto Otherpoint : InPoints)
		{
			if (Otherpoint == Slectedpoint)
			{
				continue;
			}

			FVector OtherVector = Otherpoint - InStartPoint;

			float CrossMultipleResult = (OtherVector ^ NewLineVector).Z;

			HasPointOnAnotherside = CrossMultipleResult > 0;
			if (HasPointOnAnotherside)
			{
				//这条边不能当做包络线
				break;
			}
		}

		if (HasPointOnAnotherside)
		{
			//这条边不能当做包络线
			continue;
		}
		else
		{
			//这条边可以当作包络线
			FVector NewPointToAdd = Slectedpoint;

			//插入过于靠近这条边的点
			TArray<FVector> MiddlePoints;
			for (auto Otherpoint : InPoints)
			{
				if (Otherpoint == Slectedpoint)
				{
					continue;
				}

				FVector OtherVector = Otherpoint - InStartPoint;
				float DistanceToLine = (OtherVector ^ NewLineVector).Size() / NewLineVector.Size();


				if (DistanceToLine < AttachDistance)
				{
					//确保起点-中间点-终点形成钝角，以保证中间点在线段的投影点不在线段的延长线上
					if (OtherVector.Size() < NewLineVector.Size()
						&& (Slectedpoint - Otherpoint).Size() < NewLineVector.Size())
					{

						MiddlePoints.Add(Otherpoint);
					}
				}
			}

			//对中间点按距离起点的远近进行排序
			//if (MiddlePoints.Num() > 1)
			//{
			//	for (int32 i = 0; i < MiddlePoints.Num() - 1; i++)
			//	{
			//		FVector OtherVector = MiddlePoints[i] - InStartPoint;

			//		//点在边上投影点到起点的距离
			//		float ProjectionPointToStartPoint = (OtherVector | NewLineVector) / NewLineVector.Size();


			//		for (int32 j = i + 1; j < MiddlePoints.Num(); j++)
			//		{

			//			//点在边上投影点到起点的距离
			//			float ProjectionPointToStartPoint2 = ((MiddlePoints[j] - InStartPoint) | NewLineVector) / NewLineVector.Size();
			//			if (ProjectionPointToStartPoint2 < ProjectionPointToStartPoint)
			//			{
			//				MiddlePoints.Swap(i, j);
			//			}


			//			if (ProjectionPointToStartPoint2 == ProjectionPointToStartPoint)
			//			{
			//				//按点到边的距离排序
			//				float DistanceToLine = (OtherVector ^ NewLineVector).Size() / NewLineVector.Size();
			//				float DistanceToLine2 = ((MiddlePoints[j] - InStartPoint) ^ NewLineVector).Size() / NewLineVector.Size();
			//				if (DistanceToLine > DistanceToLine2)
			//				{
			//					MiddlePoints.Swap(i, j);
			//				}
			//			}
			//		}

			//	}
			//}
			MiddlePoints.Sort([&] (const FVector& A,const FVector& B)
			{
				return (A - InStartPoint).Size2D() < (B - InStartPoint).Size2D();
			});

			for (auto MiddlePoint : MiddlePoints)
			{
				OutEnvelope.Add(MiddlePoint);
				InPoints.Remove(MiddlePoint);
			}

			OutEnvelope.Add(Slectedpoint);
			InPoints.Remove(Slectedpoint);

			if (InPoints.Num()>0)
			{
				FindEnvelopes(InPoints, Slectedpoint, OutEnvelope, AttachDistance);
			}
			return;
		}

	}
}

bool FArmyLampControlLines::IsSelected(const FVector& Pos,class UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		for (auto& LLInfo:LampControlInfoArray)
		{
			for (auto & LInfo:LLInfo.ControlLampLineArray)
			{
				for (auto& L:LInfo.ControlLines)
				{
					FVector InterPoint;
					if (FArmyMath::ArcPointIntersection(L->GetStartPos(),L->GetEndPos(),L->GetArcMiddlePos(),Pos,10.f,InterPoint))
					{
						SelectArcLine = L;
						EditPoint->SetPos(SelectArcLine->GetArcMiddlePos());
						return true;
					}
				}
			}
		}
	}
	return  false;
}

bool FArmyLampControlLines::Hover(const FVector& Pos,class UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{

	}
	return  false;
}

TSharedPtr<FArmyEditPoint> FArmyLampControlLines::SelectPoint(const FVector& Pos,class UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (GetState()==OS_Selected)
		{
			if (EditPoint->IsSelected(Pos,InViewportClient)) return EditPoint;
		}
	}
	return NULL;
}

TSharedPtr<FArmyEditPoint> FArmyLampControlLines::HoverPoint(const FVector& Pos,class UArmyEditorViewportClient* InViewportClient)
{
	return SelectPoint(Pos,InViewportClient);
}

void FArmyLampControlLines::Reset()
{
	for (auto& LLInfo : LampControlInfoArray)
	{
		for (auto & LInfo : LLInfo.ControlLampLineArray)
		{
			LInfo.ControlLines.Empty();
		}
	}
}

void FArmyLampControlLines::Sort(TArray<FVector>& Pointes)
{
	for (int32 i = 0;i<Pointes.Num();i++)
	{
		for (int32 j = i;j<Pointes.Num();j++)
		{
			bool bTemp = false;
			if ((Pointes[i].X>Pointes[j].X))
			{
				bTemp = true;
			}
			else if (Pointes[i].Y>Pointes[j].Y)
			{
				bTemp = true;
			}
			if (bTemp)
			{
				FVector tmp = Pointes[j];
				Pointes[j] = Pointes[i];
				Pointes[i] = tmp;
			}
		}
	}
}

/**	获取弧形线*/
void FArmyLampControlLines::GetArcLines(TArray<TSharedPtr<FArmyArcLine>>& OutArcLines)
{
	for (auto& LLInfo : LampControlInfoArray)
	{
		for (auto & LInfo : LLInfo.ControlLampLineArray)
		{
			for (auto L : LInfo.ControlLines)
			{
				OutArcLines.Add(L);
			}
		}
	}
}