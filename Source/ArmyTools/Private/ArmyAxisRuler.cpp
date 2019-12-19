#include "ArmyAxisRuler.h"
#include "ArmyEditableLabelSlate.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyMath.h"

"FArmyAxisRuler::"FArmyAxisRuler() :CurrentObj(NULL), SceneModel(0)
{
	SetRefCoordinateSystem(FVector(ForceInitToZero), FVector(1, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1));

	XFrontLine = MakeShareable(new "FArmyLine);
	XBackLine = MakeShareable(new "FArmyLine);
	YFrontLine = MakeShareable(new "FArmyLine);
	YBackLine = MakeShareable(new "FArmyLine);
	ZFrontLine = MakeShareable(new "FArmyLine);
	ZBackLine = MakeShareable(new "FArmyLine);

	XDrawFrontLine = MakeShareable(new "FArmyLine);
	XDrawBackLine = MakeShareable(new "FArmyLine);
	YDrawFrontLine = MakeShareable(new "FArmyLine);
	YDrawBackLine = MakeShareable(new "FArmyLine);
	ZDrawFrontLine = MakeShareable(new "FArmyLine);
	ZDrawBackLine = MakeShareable(new "FArmyLine);

	XDrawFrontLine->SetLineColor(FLinearColor(1, 0.78, 0.05, 1));
	XDrawFrontLine->bIsDashLine = true;
	XDrawFrontLine->DepthPriority = 2;
	XDrawBackLine->SetLineColor(FLinearColor(1, 0.78, 0.05, 1));
	XDrawBackLine->bIsDashLine = true;
	XDrawBackLine->DepthPriority = 2;

	YDrawFrontLine->SetLineColor(FLinearColor(1, 0.78, 0.05, 1));
	YDrawFrontLine->bIsDashLine = true;
	YDrawFrontLine->DepthPriority = 2;
	YDrawBackLine->SetLineColor(FLinearColor(1, 0.78, 0.05, 1));
	YDrawBackLine->bIsDashLine = true;
	YDrawBackLine->DepthPriority = 2;

	ZDrawFrontLine->SetLineColor(FLinearColor(1, 0.78, 0.05, 1));
	ZDrawFrontLine->bIsDashLine = true;
	ZDrawFrontLine->DepthPriority = 2;
	ZDrawBackLine->SetLineColor(FLinearColor(1, 0.78, 0.05, 1));
	ZDrawBackLine->bIsDashLine = true;
	ZDrawBackLine->DepthPriority = 2;
}

"FArmyAxisRuler::~"FArmyAxisRuler()
{

}
void "FArmyAxisRuler::Init(TSharedPtr<SOverlay> ParentWidget)
{
	SAssignNew(XFrontInputBox, SSArmyEditableLabel)
		.Visibility(EVisibility::Collapsed);
	XFrontInputBox->OnInputBoxCommitted = FOnInPutValueCommited::CreateRaw(this, &"FArmyAxisRuler::OnTextCommitted);
	SAssignNew(XBackInputBox, SSArmyEditableLabel)
		.Visibility(EVisibility::Collapsed);
	XBackInputBox->OnInputBoxCommitted = FOnInPutValueCommited::CreateRaw(this, &"FArmyAxisRuler::OnTextCommitted);
	SAssignNew(YFrontInputBox, SSArmyEditableLabel)
		.Visibility(EVisibility::Collapsed);
	YFrontInputBox->OnInputBoxCommitted = FOnInPutValueCommited::CreateRaw(this, &"FArmyAxisRuler::OnTextCommitted);
	SAssignNew(YBackInputBox, SSArmyEditableLabel)
		.Visibility(EVisibility::Collapsed);
	YBackInputBox->OnInputBoxCommitted = FOnInPutValueCommited::CreateRaw(this, &"FArmyAxisRuler::OnTextCommitted);
	SAssignNew(ZFrontInputBox, SSArmyEditableLabel)
		.Visibility(EVisibility::Collapsed);
	ZFrontInputBox->OnInputBoxCommitted = FOnInPutValueCommited::CreateRaw(this, &"FArmyAxisRuler::OnTextCommitted);
	SAssignNew(ZBackInputBox, SSArmyEditableLabel)
		.Visibility(EVisibility::Collapsed);
	ZBackInputBox->OnInputBoxCommitted = FOnInPutValueCommited::CreateRaw(this, &"FArmyAxisRuler::OnTextCommitted);

	ParentWidget->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			XFrontInputBox.ToSharedRef()
		];
	ParentWidget->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			XBackInputBox.ToSharedRef()
		];
	ParentWidget->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			YFrontInputBox.ToSharedRef()
		];
	ParentWidget->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			YBackInputBox.ToSharedRef()
		];

	ParentWidget->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			ZFrontInputBox.ToSharedRef()
		];
	ParentWidget->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			ZBackInputBox.ToSharedRef()
		];


}
void "FArmyAxisRuler::SetBoundObj(FObjectPtr InObj)
{
	CurrentObj = InObj;

	XFrontInputBox->SetVisibility(CurrentObj.IsValid() ? EVisibility::Visible : EVisibility::Collapsed);
	XBackInputBox->SetVisibility(CurrentObj.IsValid() ? EVisibility::Visible : EVisibility::Collapsed);
	YFrontInputBox->SetVisibility(CurrentObj.IsValid() ? EVisibility::Visible : EVisibility::Collapsed);
	YBackInputBox->SetVisibility(CurrentObj.IsValid() ? EVisibility::Visible : EVisibility::Collapsed);
	ZFrontInputBox->SetVisibility((CurrentObj.IsValid() && SceneModel == 1) ? EVisibility::Visible : EVisibility::Collapsed);
	ZBackInputBox->SetVisibility((CurrentObj.IsValid() && SceneModel == 1) ? EVisibility::Visible : EVisibility::Collapsed);

	if (!CurrentObj.IsValid())
	{
		SetEnableAutoAttach(false);
	}
}

void "FArmyAxisRuler::SetSceneModel(int32 InSceneModel)
{
	if (SceneModel == 0 && InSceneModel == 1)
	{
		XDrawFrontLine->bIsDashLine = false;
		XDrawBackLine->bIsDashLine = false;
		YDrawFrontLine->bIsDashLine = false;
		YDrawBackLine->bIsDashLine = false;
		ZDrawFrontLine->bIsDashLine = false;
		ZDrawBackLine->bIsDashLine = false;
		ZFrontInputBox->SetVisibility(EVisibility::Collapsed);
		ZBackInputBox->SetVisibility(EVisibility::Collapsed);
	}
	else if (SceneModel == 1 && InSceneModel == 0)
	{
		XDrawFrontLine->bIsDashLine = true;
		XDrawBackLine->bIsDashLine = true;
		YDrawFrontLine->bIsDashLine = true;
		YDrawBackLine->bIsDashLine = true;
		ZDrawFrontLine->bIsDashLine = true;
		ZDrawBackLine->bIsDashLine = true;
		ZFrontInputBox->SetVisibility(EVisibility::Visible);
		ZBackInputBox->SetVisibility(EVisibility::Visible);
	}
	SceneModel = InSceneModel;
}

void "FArmyAxisRuler::AutoAttach()
{
	if (bEnableAutoAttach)
	{
		float TempValue;
		TempValue = FCString::Atof(*XFrontInputBox->GetText());
		if (TempValue < 100 && TempValue != 10)
		{
			OnTextCommitted(FText::AsNumber(0), XFrontInputBox.Get());
		}
		TempValue = FCString::Atof(*XBackInputBox->GetText());
		if (TempValue < 100 && TempValue != 10)
		{
			OnTextCommitted(FText::AsNumber(0), XBackInputBox.Get());
		}
		TempValue = FCString::Atof(*YFrontInputBox->GetText());
		if (TempValue < 100 && TempValue != 10)
		{
			OnTextCommitted(FText::AsNumber(0), YFrontInputBox.Get());
		}
		TempValue = FCString::Atof(*YBackInputBox->GetText());
		if (TempValue < 100 && TempValue != 10)
		{
			OnTextCommitted(FText::AsNumber(0), YBackInputBox.Get());
		}
	}
}

void "FArmyAxisRuler::SetUseInDrawArea(bool Value)
{
	UseInDrawArea = Value;
}

//检查输入是否有效
bool "FArmyAxisRuler::CheckStringIsVaild(const FString& str)
{
	FString Reg = TEXT("^[0-9]*$");
	FRegexPattern Pattern(Reg);
	FRegexMatcher regMatcher(Pattern, str);
	regMatcher.SetLimits(0, str.Len());
	return regMatcher.FindNext();
}

void "FArmyAxisRuler::OnTextCommitted(const FText& InText, const SWidget* InWidget)
{
	//只能输入数字
	//static const SWidget* PreWidget = NULL;
	//static FText PreText;
	//if (PreWidget == InWidget && PreText.CompareTo(InText) == 0)
	//{
	//	return;
	//}
	//else
	//{
	//	PreText = InText;
	//	PreWidget = InWidget;
	//}

	float V = FCString::Atof(*InText.ToString());
	if (CurrentObj.IsValid())
	{
		if (InWidget == XFrontInputBox.Get() && XFrontInputBox->HasTextChangedFromOriginal())
		{
			FTransform Trans(FVector(XFrontV - V / 10.f, 0, 0));
			if (AxisOperationDelegate.IsBound())
			{
				if (AxisOperationDelegate.Execute(CurrentObj, Trans, 1))
				{
					XFrontInputBox->Revert();
					return;
				}
				else
				{
					CurrentObj->SetTransformFlag(1);//x正方向
					CurrentObj->ApplyTransform(Trans);
					CurrentObj->SetTransformFlag(0);//应用变换后恢复默认值
				}
			}
			else
			{
				CurrentObj->SetTransformFlag(1);//x正方向
				CurrentObj->ApplyTransform(Trans);
				CurrentObj->SetTransformFlag(0);//应用变换后恢复默认值
			}
			//XFrontV = V/10.f;
			XMain = 1;
			//FText LenText = FText::FromString(FString::Printf(TEXT("%d"), FMath::CeilToInt(V)));
			//XFrontInputBox->SetInputText(LenText);
		}
		else if (InWidget == XBackInputBox.Get() && XBackInputBox->HasTextChangedFromOriginal())
		{
			FTransform Trans(FVector(V / 10.f - XBackV, 0, 0));

			if (AxisOperationDelegate.IsBound())
			{
				if (AxisOperationDelegate.Execute(CurrentObj, Trans, 2))
				{
					XBackInputBox->Revert();
					return;
				}
				else
				{
					CurrentObj->SetTransformFlag(2);//x负方向
					CurrentObj->ApplyTransform(Trans);
					CurrentObj->SetTransformFlag(0);//应用变换后恢复默认值
				}
			}
			else
			{
				CurrentObj->SetTransformFlag(2);//x负方向
				CurrentObj->ApplyTransform(Trans);
				CurrentObj->SetTransformFlag(0);//应用变换后恢复默认值
			}
			//XBackV = V/10.f;
			XMain = -1;
			//FText LenText = FText::FromString(FString::Printf(TEXT("%d"), FMath::CeilToInt(V)));
			//XBackInputBox->SetInputText(LenText);
		}
		else if (InWidget == YFrontInputBox.Get() && YFrontInputBox->HasTextChangedFromOriginal())
		{
			FTransform Trans(FVector(0, YFrontV - V / 10.f, 0));
			if (AxisOperationDelegate.IsBound())
			{
				if (AxisOperationDelegate.Execute(CurrentObj, Trans, 3))
				{
					YFrontInputBox->Revert();
					return;
				}
				else
				{
					CurrentObj->SetTransformFlag(3);//y正方向
					CurrentObj->ApplyTransform(Trans);
					CurrentObj->SetTransformFlag(0);//应用变换后恢复默认值
				}
			}
			else
			{
				CurrentObj->SetTransformFlag(3);//y正方向
				CurrentObj->ApplyTransform(Trans);
				CurrentObj->SetTransformFlag(0);//应用变换后恢复默认值
			}

			//YFrontV = V/10.f;
			YMain = 1;
			FText LenText = FText::FromString(FString::Printf(TEXT("%d"), FMath::CeilToInt(V)));
			//YFrontInputBox->SetInputText(LenText);
		}
		else if (InWidget == YBackInputBox.Get() && YBackInputBox->HasTextChangedFromOriginal())
		{
			FTransform Trans(FVector(0, V / 10.f - YBackV, 0));
			if (AxisOperationDelegate.IsBound())
			{
				if (AxisOperationDelegate.Execute(CurrentObj, Trans, 4))
				{
					YBackInputBox->Revert();
					return;
				}
				else
				{
					CurrentObj->SetTransformFlag(4);//y负方向
					CurrentObj->ApplyTransform(Trans);
					CurrentObj->SetTransformFlag(0);//应用变换后恢复默认值
				}
			}
			else
			{
				CurrentObj->SetTransformFlag(4);//y负方向
				CurrentObj->ApplyTransform(Trans);
				CurrentObj->SetTransformFlag(0);//应用变换后恢复默认值
			}
			//YBackV = V/10.f;
			YMain = -1;
			//FText LenText = FText::FromString(FString::Printf(TEXT("%d"), FMath::CeilToInt(V)));
			//YBackInputBox->SetInputText(LenText);
		}
		else if (InWidget == ZFrontInputBox.Get() && ZFrontInputBox->HasTextChangedFromOriginal())
		{
			ZFrontV = V;
		}
		else if (InWidget == ZBackInputBox.Get() && ZBackInputBox->HasTextChangedFromOriginal())
		{
			ZBackV = V;
		}
	}
}
void "FArmyAxisRuler::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (CurrentObj.IsValid())
	{
		XDrawFrontLine->Draw(PDI, View);
		XDrawBackLine->Draw(PDI, View);
		YDrawFrontLine->Draw(PDI, View);
		YDrawBackLine->Draw(PDI, View);
		ZDrawFrontLine->Draw(PDI, View);
		ZDrawBackLine->Draw(PDI, View);
	}
}
void "FArmyAxisRuler::UpdateTransform()
{
	XDrawFrontLine->SetStart(FromBasePlane(XFrontLine->GetStart()));
	XDrawFrontLine->SetEnd(FromBasePlane(XFrontLine->GetEnd()));

	XDrawBackLine->SetStart(FromBasePlane(XBackLine->GetStart()));
	XDrawBackLine->SetEnd(FromBasePlane(XBackLine->GetEnd()));

	YDrawFrontLine->SetStart(FromBasePlane(YFrontLine->GetStart()));
	YDrawFrontLine->SetEnd(FromBasePlane(YFrontLine->GetEnd()));

	YDrawBackLine->SetStart(FromBasePlane(YBackLine->GetStart()));
	YDrawBackLine->SetEnd(FromBasePlane(YBackLine->GetEnd()));

	ZDrawFrontLine->SetStart(FromBasePlane(ZFrontLine->GetStart()));
	ZDrawFrontLine->SetEnd(FromBasePlane(ZFrontLine->GetEnd()));

	ZDrawBackLine->SetStart(FromBasePlane(ZBackLine->GetStart()));
	ZDrawBackLine->SetEnd(FromBasePlane(ZBackLine->GetEnd()));
}
void "FArmyAxisRuler::Update(UArmyEditorViewportClient* InViwportClient, const TArray<FObjectWeakPtr>& InObjects)
{
	if (!CurrentObj.IsValid()) return;

	SetSceneModel(0);
	Update2D(InViwportClient, InObjects);

	/**@刘克祥 自动吸附*/
	AutoAttach();
	//if(InViwportClient->IsPerspective ())
	//{
	//	SetSceneModel(1);
	//	Update3D(InViwportClient, InObjects);
	//}
	//else if(InViwportClient->IsOrtho ())
	//{
	//	SetSceneModel(0);
	//	Update2D(InViwportClient, InObjects);
	//}
}

void "FArmyAxisRuler::Update2D(UArmyEditorViewportClient* InViwportClient, const TArray<FObjectWeakPtr>& InObjects)
{
	FBox SelfBox = CurrentObj->GetBounds();
	FVector SelfCenter = SelfBox.GetCenter();
	SelfCenter.Z = 0;

	if (CurrentObj->GetPropertyFlag("FArmyObject::FLAG_POINTOBJ))
	{
		XFrontLine->SetStart(FVector(FVector2D(CurrentObj->GetBasePos()), 0));
		XFrontLine->SetEnd(XFrontLine->GetStart() - FVector(1, 0, 0));
		XBackLine->SetStart(FVector(FVector2D(CurrentObj->GetBasePos()), 0));
		XBackLine->SetEnd(XBackLine->GetStart() + FVector(1, 0, 0));
		YFrontLine->SetStart(FVector(FVector2D(CurrentObj->GetBasePos()), 0));
		YFrontLine->SetEnd(YFrontLine->GetStart() - FVector(0, 1, 0));
		YBackLine->SetStart(FVector(FVector2D(CurrentObj->GetBasePos()), 0));
		YBackLine->SetEnd(YBackLine->GetStart() + FVector(0, 1, 0));
	}
	else
	{
		XFrontLine->SetStart(FVector(SelfBox.Max.X, SelfCenter.Y, SelfCenter.Z));
		XFrontLine->SetEnd(XFrontLine->GetStart() - FVector(1, 0, 0));
		XBackLine->SetStart(FVector(SelfBox.Min.X, SelfCenter.Y, SelfCenter.Z));
		XBackLine->SetEnd(XBackLine->GetStart() + FVector(1, 0, 0));
		YFrontLine->SetStart(FVector(SelfCenter.X, SelfBox.Max.Y, SelfCenter.Z));
		YFrontLine->SetEnd(YFrontLine->GetStart() - FVector(0, 1, 0));
		YBackLine->SetStart(FVector(SelfCenter.X, SelfBox.Min.Y, SelfCenter.Z));
		YBackLine->SetEnd(YBackLine->GetStart() + FVector(0, 1, 0));
	}

	FVector2D XLineStart = FVector2D(XFrontLine->GetStart());
	FVector2D XLineEnd = XLineStart + FVector2D(10, 0);

	FVector2D YLineStart = FVector2D(YFrontLine->GetStart());
	FVector2D YLineEnd = YLineStart + FVector2D(0, 10);

	bool XFrontDefault = true, XBackDefault = true, YFrontDefault = true, YBackDefault = true;


	for (auto ObjIt : InObjects)
	{
		//if (UseInDrawArea)
		//{
		//	break;
		//}
		if (ObjIt == CurrentObj) continue;


		else if (ObjIt.Pin()->GetPropertyFlag("FArmyObject::FLAG_COLLISION))
		{
			if (ObjIt.Pin()->bBoundingBox)
			{
				FBox ObjBox = ObjIt.Pin()->GetBounds();

				if (ObjBox.Min.Y <= XFrontLine->GetStart().Y &&
					ObjBox.Max.Y >= XFrontLine->GetStart().Y &&
					(ObjBox.Min.X >= XFrontLine->GetStart().X || XFrontLine->GetStart().X > XFrontLine->GetEnd().X))
				{
					if (ObjBox.Min.X < XFrontLine->GetEnd().X)
					{
						XFrontLine->SetEnd(FVector(ObjBox.Min.X, XFrontLine->GetStart().Y, SelfCenter.Z));
						XFrontDefault = false;
					}
				}
				else if (ObjBox.Min.Y <= XBackLine->GetStart().Y &&
					ObjBox.Max.Y >= XBackLine->GetStart().Y &&
					(ObjBox.Max.X <= XBackLine->GetStart().X || XBackLine->GetStart().X < XBackLine->GetEnd().X))
				{
					if (ObjBox.Max.X > XBackLine->GetEnd().X)
					{
						XBackLine->SetEnd(FVector(ObjBox.Max.X, XBackLine->GetStart().Y, SelfCenter.Z));
						XBackDefault = false;
					}
				}
				else if (ObjBox.Min.X <= YFrontLine->GetStart().X &&
					ObjBox.Max.X >= YFrontLine->GetStart().X &&
					(ObjBox.Min.Y >= YFrontLine->GetStart().Y || YFrontLine->GetStart().Y > YFrontLine->GetEnd().Y))
				{
					if (ObjBox.Min.Y < YFrontLine->GetEnd().Y)
					{
						YFrontLine->SetEnd(FVector(YFrontLine->GetStart().X, ObjBox.Min.Y, SelfCenter.Z));
						YFrontDefault = false;
					}
				}
				else if (ObjBox.Min.X <= YBackLine->GetStart().X &&
					ObjBox.Max.X >= YBackLine->GetStart().X &&
					(ObjBox.Max.Y <= YBackLine->GetStart().Y || YBackLine->GetStart().Y < YBackLine->GetEnd().Y))
				{
					if (ObjBox.Max.Y > YBackLine->GetEnd().Y)
					{
						YBackLine->SetEnd(FVector(YBackLine->GetStart().X, ObjBox.Max.Y, SelfCenter.Z));
						YBackDefault = false;
					}
				}
			}
			else
			{
				TArray<TSharedPtr<"FArmyLine>> Lines;
				ObjIt.Pin()->GetLines(Lines);
				for (auto LineIt : Lines)
				{
					FVector2D InterPoint;
					if ("FArmyMath::Line2DIntersection(FVector2D(LineIt->GetStart()), FVector2D(LineIt->GetEnd()), XLineStart, XLineEnd, InterPoint))
					{
						// @刘克祥 如果射线碰撞点和原点相近，则不作处理
						if (InterPoint.Equals(XLineStart, 0.1f) || InterPoint.Equals(XLineEnd, 0.1f))
						{
							continue;
						}

						float LineMaxY = LineIt->GetStart().Y;
						float LineMinY = LineMaxY;
						LineIt->GetStart().Y < LineIt->GetEnd().Y ? LineMaxY = LineIt->GetEnd().Y : LineMinY = LineIt->GetEnd().Y;
						if (InterPoint.Y <= LineMaxY && InterPoint.Y >= LineMinY)
						{
							if ((XFrontLine->GetStart().X > XFrontLine->GetEnd().X && InterPoint.X - XFrontLine->GetStart().X > -0.01) ||
								(XFrontLine->GetStart().X < XFrontLine->GetEnd().X && XFrontLine->GetStart().X - InterPoint.X < 0.01 && XFrontLine->GetEnd().X - InterPoint.X > -0.01))
							{
								XFrontLine->SetEnd(FVector(InterPoint, 0));
								XFrontDefault = false;
							}

							if ((XBackLine->GetStart().X < XBackLine->GetEnd().X && InterPoint.X - XBackLine->GetStart().X < 0.01) ||
								(XBackLine->GetStart().X >= XBackLine->GetEnd().X && XBackLine->GetStart().X - InterPoint.X > -0.01 && XBackLine->GetEnd().X - InterPoint.X < 0.01))
							{
								XBackLine->SetEnd(FVector(InterPoint, 0));
								XBackDefault = false;
							}
						}
					}

					if ("FArmyMath::Line2DIntersection(FVector2D(LineIt->GetStart()), FVector2D(LineIt->GetEnd()), YLineStart, YLineEnd, InterPoint))
					{
						// @刘克祥 如果射线碰撞点和原点相近，则不作处理
						if (InterPoint.Equals(YLineStart, 0.1f) || InterPoint.Equals(YLineEnd, 0.1f))
						{
							continue;
						}

						float LineMaxX = LineIt->GetStart().X;
						float LineMinX = LineMaxX;
						LineIt->GetStart().X < LineIt->GetEnd().X ? LineMaxX = LineIt->GetEnd().X : LineMinX = LineIt->GetEnd().X;
						if (InterPoint.X <= LineMaxX && InterPoint.X >= LineMinX)
						{
							if ((YFrontLine->GetStart().Y > YFrontLine->GetEnd().Y && InterPoint.Y - YFrontLine->GetStart().Y > -0.01) ||
								(YFrontLine->GetStart().Y < YFrontLine->GetEnd().Y && YFrontLine->GetStart().Y - InterPoint.Y < 0.01 && YFrontLine->GetEnd().Y - InterPoint.Y > -0.01))
							{
								YFrontLine->SetEnd(FVector(InterPoint, 0));
								YFrontDefault = false;
							}

							if ((YBackLine->GetStart().Y < YBackLine->GetEnd().Y && InterPoint.Y - YBackLine->GetStart().Y < 0.01) ||
								(YBackLine->GetStart().Y > YBackLine->GetEnd().Y && YBackLine->GetStart().Y - InterPoint.Y > -0.01 && YBackLine->GetEnd().Y - InterPoint.Y < 0.01))
							{
								YBackLine->SetEnd(FVector(InterPoint, 0));
								YBackDefault = false;
							}
						}
					}
				}
				TArray<TSharedPtr<"FArmyCircle>> CLines;
				ObjIt.Pin()->GetCircleLines(CLines);
				for (auto C : CLines)
				{
					FVector NearPoint = FMath::ClosestPointOnInfiniteLine(XBackLine->GetEnd(), XFrontLine->GetEnd(), C->GetPosition());

					TArray<FVector> InterPoints;
					if ("FArmyMath::ArcLineIntersection(C->GetStartPos(), C->GetEndPos(), C->GetArcMiddlePos(), XBackLine->GetEnd(), XFrontLine->GetEnd(), InterPoints))
					{
						FVector MNearPos = FMath::ClosestPointOnInfiniteLine(C->GetStartPos(), C->GetEndPos(), C->GetArcMiddlePos());

						for (auto& V : InterPoints)
						{
							FVector TempNearPos = FMath::ClosestPointOnInfiniteLine(C->GetStartPos(), C->GetEndPos(), V);
							if (((V - TempNearPos).GetSafeNormal() - (C->GetArcMiddlePos() - MNearPos).GetSafeNormal()).Size() < 0.0001)
							{
								if ((XFrontLine->GetStart().X > XFrontLine->GetEnd().X && XFrontLine->GetStart().X <= V.X) || XFrontLine->GetStart().X <= V.X && XFrontLine->GetEnd().X >= V.X)
								{
									XFrontLine->SetEnd(V);
									XFrontDefault = false;
									//MatchPoint = true;
								}
								if ((XBackLine->GetStart().X < XBackLine->GetEnd().X && XBackLine->GetStart().X >= V.X) || XBackLine->GetStart().X >= V.X && XBackLine->GetEnd().X <= V.X)
								{
									XBackLine->SetEnd(V);
									XBackDefault = false;
									//MatchPoint = true;
								}
							}
						}
					}
					InterPoints.Empty();
					if ("FArmyMath::ArcLineIntersection(C->GetStartPos(), C->GetEndPos(), C->GetArcMiddlePos(), YBackLine->GetEnd(), YFrontLine->GetEnd(), InterPoints))
					{
						FVector MNearPos = FMath::ClosestPointOnInfiniteLine(C->GetStartPos(), C->GetEndPos(), C->GetArcMiddlePos());

						for (auto& V : InterPoints)
						{
							FVector TempNearPos = FMath::ClosestPointOnInfiniteLine(C->GetStartPos(), C->GetEndPos(), V);
							FVector dir1 = (V - TempNearPos).GetSafeNormal();
							FVector dir2 = (C->GetArcMiddlePos() - MNearPos).GetSafeNormal();
							if (((V - TempNearPos).GetSafeNormal() - (C->GetArcMiddlePos() - MNearPos).GetSafeNormal()).Size() < 0.0001)
							{
								if ((YFrontLine->GetStart().Y > YFrontLine->GetEnd().Y && YFrontLine->GetStart().Y <= V.Y) || YFrontLine->GetStart().Y <= V.Y && YFrontLine->GetEnd().Y >= V.Y)
								{
									YFrontLine->SetEnd(V);
									YFrontDefault = false;
									//MatchPoint = true;
								}
								if ((YBackLine->GetStart().Y < YBackLine->GetEnd().Y && YBackLine->GetStart().Y >= V.Y) || YBackLine->GetStart().Y >= V.Y && YBackLine->GetEnd().Y <= V.Y)
								{
									YBackLine->SetEnd(V);
									YBackDefault = false;
									//MatchPoint = true;
								}
							}
						}
					}
				}
				//TArray<TSharedPtr<"FArmyCircle>> CLines;
				//ObjIt.Pin()->GetCircleLines(CLines);
				//for (auto C : CLines)
				//{
				//	TArray<FVector> InterPoints;
				//	if ("FArmyMath::ArcLineIntersection(C->GetStartPos(), C->GetEndPos(), C->GetArcMiddlePos(), XBackLine->GetEnd(), XFrontLine->GetEnd(), InterPoints))
				//	{
				//		FVector MNearPos = FMath::ClosestPointOnInfiniteLine(C->GetStartPos(), C->GetEndPos(), C->GetArcMiddlePos());

				//		for (auto& V : InterPoints)
				//		{
				//			FVector TempNearPos = FMath::ClosestPointOnInfiniteLine(C->GetStartPos(), C->GetEndPos(), V);
				//			if (((V - TempNearPos).GetSafeNormal() - (C->GetArcMiddlePos() - MNearPos).GetSafeNormal()).Size() < 0.0001)
				//			{
				//				if ((XFrontLine->GetStart().X > XFrontLine->GetEnd().X && XFrontLine->GetStart().X <= V.X) || XFrontLine->GetStart().X <= V.X && XFrontLine->GetEnd().X >= V.X)
				//				{
				//					XFrontLine->SetEnd(V);
				//					//MatchPoint = true;
				//				}
				//				if ((XBackLine->GetStart().X < XBackLine->GetEnd().X && XBackLine->GetStart().X >= V.X) || XBackLine->GetStart().X >= V.X && XBackLine->GetEnd().X <= V.X)
				//				{
				//					XBackLine->SetEnd(V);
				//					//MatchPoint = true;
				//				}
				//			}
				//		}
				//	}
				//	InterPoints.Empty();
				//	if ("FArmyMath::ArcLineIntersection(C->GetStartPos(), C->GetEndPos(), C->GetArcMiddlePos(), YBackLine->GetEnd(), YFrontLine->GetEnd(), InterPoints))
				//	{
				//		FVector MNearPos = FMath::ClosestPointOnInfiniteLine(C->GetStartPos(), C->GetEndPos(), C->GetArcMiddlePos());

				//		for (auto& V : InterPoints)
				//		{
				//			FVector TempNearPos = FMath::ClosestPointOnInfiniteLine(C->GetStartPos(), C->GetEndPos(), V);
				//			FVector dir1 = (V - TempNearPos).GetSafeNormal();
				//			FVector dir2 = (C->GetArcMiddlePos() - MNearPos).GetSafeNormal();
				//			if (((V - TempNearPos).GetSafeNormal() - (C->GetArcMiddlePos() - MNearPos).GetSafeNormal()).Size() < 0.0001)
				//			{
				//				if ((YFrontLine->GetStart().Y > YFrontLine->GetEnd().Y && YFrontLine->GetStart().Y <= V.Y) || YFrontLine->GetStart().Y <= V.Y && YFrontLine->GetEnd().Y >= V.Y)
				//				{
				//					YFrontLine->SetEnd(V);
				//					//MatchPoint = true;
				//				}
				//				if ((YBackLine->GetStart().Y < YBackLine->GetEnd().Y && YBackLine->GetStart().Y >= V.Y) || YBackLine->GetStart().Y >= V.Y && YBackLine->GetEnd().Y <= V.Y)
				//				{
				//					YBackLine->SetEnd(V);
				//					//MatchPoint = true;
				//				}
				//			}
				//		}
				//	}
				//}
			}
		}
	}

	//@刘克祥
	//用多边形边界限制标尺的长度
	if (OutLine.Num() > 2)
	{
		auto LimitLine = [this](TSharedPtr<"FArmyLine> Axis) {

			FVector TempEndPoint = Axis->GetEnd();
			FVector TempStartPoint = Axis->GetStart();
			TempStartPoint.Z = 0;
			TempEndPoint.Z = 0;
			if(!"FArmyMath::IsPointInOrOnPolygon2D(TempEndPoint, OutLine))
			{
				if (!"FArmyMath::IsPointInOrOnPolygon2D(TempStartPoint, OutLine))
				{
					Axis->SetEnd(Axis->GetStart());
				}
				else
				{
					FVector Intersection;
					for (int32 i = 0; i < OutLine.Num(); i++)
					{
						FVector StartPoint = OutLine[i];
						FVector EndPoint = i + 1 < OutLine.Num() ? OutLine[i + 1] : OutLine[0];

						if (FMath::SegmentIntersection2D(StartPoint
							, EndPoint
							, TempEndPoint
							, TempStartPoint
							, Intersection))
						{
							Intersection.Z = Axis->GetStart().Z;
							Axis->SetEnd(Intersection);
							break;
						}
					}
				}
			}


		};

		LimitLine(XFrontLine);
		LimitLine(XBackLine);
		LimitLine(YFrontLine);
		LimitLine(YBackLine);

		////隐藏选定轴 用于测试
		//auto HideLine = [](TSharedPtr<"FArmyLine> Axis)
		//{
		//	Axis->SetEnd(Axis->GetStart());
		//};

		//HideLine(XFrontLine);
		//HideLine(XBackLine);
		//HideLine(YFrontLine);
	}

	//@刘克祥
	//用圆形边界限制标尺的长度
	if (UseCircleBound)
	{

		//利用圆形方程判断标尺与圆的交点 (x-a)^2 + (y - b) ^ 2=R ^ 2
		float XX = CircleRadius *CircleRadius - (XFrontLine->GetEnd().Y - CircleCenter.Y) * (XFrontLine->GetEnd().Y - CircleCenter.Y);
		//求得两个交点
		float X1 = CircleCenter.X +FMath::Sqrt(XX);
		float X2 = CircleCenter.X- FMath::Sqrt(XX);

		//判断终点在不在圆外
		if ((XFrontLine->GetEnd() - CircleCenter).Size2D() > CircleRadius)
		{
				FVector NewEnd = XFrontLine->GetEnd();
				NewEnd.X = X1;
				XFrontLine->SetEnd(NewEnd);
		}

		//判断终点在不在圆外
		if ((XBackLine->GetEnd() - CircleCenter).Size2D() > CircleRadius)
		{
				FVector NewEnd = XBackLine->GetEnd();
				NewEnd.X = X2;
				XBackLine->SetEnd(NewEnd);
		}

		//利用圆形方程判断标尺与圆的交点 (x-a)^2 + (y - b) ^ 2=R ^ 2
		float YY = CircleRadius *CircleRadius - (YFrontLine->GetEnd().X - CircleCenter.X) * (YFrontLine->GetEnd().X - CircleCenter.X);
		//求得两个交点
		float Y1 = CircleCenter.Y + FMath::Sqrt(YY);
		float Y2 = CircleCenter.Y - FMath::Sqrt(YY);

		//判断终点在不在圆外
		if ((YFrontLine->GetEnd() - CircleCenter).Size2D() > CircleRadius)
		{
			FVector NewEnd = YFrontLine->GetEnd();
			NewEnd.Y = Y1;
			YFrontLine->SetEnd(NewEnd);
		}

		//判断终点在不在圆外
		if ((YBackLine->GetEnd() - CircleCenter).Size2D() > CircleRadius)
		{
			FVector NewEnd = YBackLine->GetEnd();
			NewEnd.Y = Y2;
			YBackLine->SetEnd(NewEnd);
		}

	}



	FVector2D ViewportSize;
	InViwportClient->GetViewportSize(ViewportSize);
	//int32 XObj = FMath::CeilToInt ((XFrontLine->GetStart()-XBackLine->GetStart()).Size ()*10);
	//int32 YObj = FMath::CeilToInt ((YFrontLine->GetStart()-YBackLine->GetStart()).Size ()*10);

	//float TempV = FMath::FloorToFloat((XFrontLine->GetEnd() - XFrontLine->GetStart()).Size() * 100)/100;
	float TempV = 0;
	if (XFrontDefault)
	{
		XFrontLine->SetEnd(XFrontLine->GetStart());
	}
	else
	{
		TempV = (XFrontLine->GetEnd() - XFrontLine->GetStart()).Size();
	}

	if (FMath::IsNearlyEqual(TempV, 0.f, 0.01f))
	{
		XFrontInputBox->SetVisibility(EVisibility::Collapsed);
	}
	else
	{
		XFrontInputBox->SetVisibility(EVisibility::Visible);
	}

	if (TempV != XFrontV)
	{
		XFrontV = TempV;
		float AdjustV = FMath::FloorToFloat(XFrontV * 100) / 100;
		int32 V = FMath::CeilToInt(AdjustV * 10);
		FText LenText = FText::FromString(FString::Printf(TEXT("%d"), V));
		XFrontInputBox->SetInputText(LenText);
	}
	//TempV = FMath::FloorToFloat((XBackLine->GetEnd() - XBackLine->GetStart()).Size()*100)/100;
	TempV = 0;
	if (XBackDefault)
	{
		XBackLine->SetEnd(XBackLine->GetStart());
	}
	else
	{
		TempV = (XBackLine->GetEnd() - XBackLine->GetStart()).Size();
	}

	if (FMath::IsNearlyEqual(TempV, 0.f, 0.01f))
	{
		XBackInputBox->SetVisibility(EVisibility::Collapsed);
	}
	else
	{
		XBackInputBox->SetVisibility(EVisibility::Visible);
	}

	if (TempV != XBackV)
	{
		XBackV = TempV;
		float AdjustV = FMath::FloorToFloat(XBackV * 100) / 100;
		int32 V = FMath::CeilToInt(AdjustV * 10);
		FText LenText = FText::FromString(FString::Printf(TEXT("%d"), V));
		XBackInputBox->SetInputText(LenText);
	}

	//TempV = FMath::FloorToFloat((YFrontLine->GetEnd() - YFrontLine->GetStart()).Size()*100)/100;
	TempV = 0;
	if (YFrontDefault)
	{
		YFrontLine->SetEnd(YFrontLine->GetStart());
	}
	else
	{
		TempV = (YFrontLine->GetEnd() - YFrontLine->GetStart()).Size();
	}

	if (FMath::IsNearlyEqual(TempV, 0.f, 0.01f))
	{
		YFrontInputBox->SetVisibility(EVisibility::Collapsed);
	}
	else
	{
		YFrontInputBox->SetVisibility(EVisibility::Visible);
	}

	if (TempV != YFrontV)
	{
		YFrontV = TempV;
		float AdjustV = FMath::FloorToFloat(YFrontV * 100) / 100;
		int32 V = FMath::CeilToInt(AdjustV * 10);
		FText LenText = FText::FromString(FString::Printf(TEXT("%d"), V));
		YFrontInputBox->SetInputText(LenText);
	}

	//TempV = FMath::FloorToFloat((YBackLine->GetEnd() - YBackLine->GetStart()).Size()*100)/100;
	TempV = 0;
	if (YBackDefault)
	{
		YBackLine->SetEnd(YBackLine->GetStart());
	}
	else
	{
		TempV = (YBackLine->GetEnd() - YBackLine->GetStart()).Size();
	}

	if (FMath::IsNearlyEqual(TempV, 0.f, 0.01f))
	{
		YBackInputBox->SetVisibility(EVisibility::Collapsed);
	}
	else
	{
		YBackInputBox->SetVisibility(EVisibility::Visible);
	}

	if (TempV != YBackV)
	{
		YBackV = TempV;
		float AdjustV = FMath::FloorToFloat(YBackV * 100) / 100;
		int32 V = FMath::CeilToInt(AdjustV * 10);
		FText LenText = FText::FromString(FString::Printf(TEXT("%d"), V));
		YBackInputBox->SetInputText(LenText);
	}

	/*if(XMain > 0)
	{
		int32 XFullV = FMath::CeilToInt ((XFrontLine->GetEnd()-XBackLine->GetEnd()).Size ()*10);
		float TempV = (XFrontLine->GetEnd()-XFrontLine->GetStart()).Size ();
		if(TempV!=XFrontV)
		{
			XFrontV = TempV;
			int32 V = FMath::CeilToInt (XFrontV*10);
			FText LenText = FText::FromString (FString::Printf (TEXT ("%d"),V));
			XFrontInputBox->SetInputText (LenText);

			XBackV = (XBackLine->GetEnd()-XBackLine->GetStart()).Size ();
			LenText = FText::FromString (FString::Printf (TEXT ("%d"),XFullV-XObj-V));
			XBackInputBox->SetInputText (LenText);
		}
	}
	else
	{
		int32 XFullV = FMath::CeilToInt ((XFrontLine->GetEnd()-XBackLine->GetEnd()).Size ()*10);
		float TempV = (XBackLine->GetEnd()-XBackLine->GetStart()).Size ();
		if(TempV!=XBackV)
		{
			XBackV = TempV;
			int32 V = FMath::CeilToInt (XBackV*10);
			FText LenText = FText::FromString (FString::Printf (TEXT ("%d"),V));
			XBackInputBox->SetInputText (LenText);

			XFrontV = (XFrontLine->GetEnd()-XFrontLine->GetStart()).Size ();
			LenText = FText::FromString (FString::Printf (TEXT ("%d"),XFullV-XObj-V));
			XFrontInputBox->SetInputText (LenText);
		}
	}

	if(YMain > 0)
	{
		int32 YFullV = FMath::CeilToInt ((YFrontLine->GetEnd()-YBackLine->GetEnd()).Size ()*10);
		float TempV = (YFrontLine->GetEnd()-YFrontLine->GetStart()).Size ();
		if(TempV!=YFrontV)
		{
			YFrontV = TempV;
			int32 V = FMath::CeilToInt (YFrontV*10);
			FText LenText = FText::FromString (FString::Printf (TEXT ("%d"),V));
			YFrontInputBox->SetInputText (LenText);

			YBackV = (YBackLine->GetEnd()-YBackLine->GetStart()).Size ();
			LenText = FText::FromString (FString::Printf (TEXT ("%d"),YFullV-YObj-V));
			YBackInputBox->SetInputText (LenText);
		}
	}
	else
	{
		int32 YFullV = FMath::CeilToInt ((YFrontLine->GetEnd()-YBackLine->GetEnd()).Size ()*10);
		float TempV = (YBackLine->GetEnd()-YBackLine->GetStart()).Size ();
		if(TempV!=YBackV)
		{
			YBackV = TempV;
			int32 V = FMath::CeilToInt (YBackV*10);
			FText LenText = FText::FromString (FString::Printf (TEXT ("%d"),V));
			YBackInputBox->SetInputText (LenText);

			YFrontV = (YFrontLine->GetEnd()-YFrontLine->GetStart()).Size ();
			LenText = FText::FromString (FString::Printf (TEXT ("%d"),YFullV-YObj-V));
			YFrontInputBox->SetInputText (LenText);
		}
	}*/
	UpdateTransform();
	FVector2D PixPos, TempStartPixPos, TempEndPixPos;
	InViwportClient->WorldToPixel((XDrawFrontLine->GetStart() + XDrawFrontLine->GetEnd()) / 2, PixPos);
	PixPos -= ViewportSize / 2;

	InViwportClient->WorldToPixel(XDrawFrontLine->GetStart(), TempStartPixPos);
	InViwportClient->WorldToPixel(XDrawFrontLine->GetEnd(), TempEndPixPos);
	if ((TempEndPixPos - TempStartPixPos).Size() < 80)
	{
		PixPos += (TempEndPixPos - TempStartPixPos).GetSafeNormal() * 80;
	}
	XFrontInputBox->SetRenderTransform(FSlateRenderTransform(PixPos));

	InViwportClient->WorldToPixel((XDrawBackLine->GetStart() + XDrawBackLine->GetEnd()) / 2, PixPos);
	PixPos -= ViewportSize / 2;

	InViwportClient->WorldToPixel(XDrawBackLine->GetStart(), TempStartPixPos);
	InViwportClient->WorldToPixel(XDrawBackLine->GetEnd(), TempEndPixPos);
	if ((TempEndPixPos - TempStartPixPos).Size() < 80)
	{
		PixPos += (TempEndPixPos - TempStartPixPos).GetSafeNormal() * 80;
	}
	XBackInputBox->SetRenderTransform(FSlateRenderTransform(PixPos));

	InViwportClient->WorldToPixel((YDrawFrontLine->GetStart() + YDrawFrontLine->GetEnd()) / 2, PixPos);
	PixPos -= ViewportSize / 2;

	InViwportClient->WorldToPixel(YDrawFrontLine->GetStart(), TempStartPixPos);
	InViwportClient->WorldToPixel(YDrawFrontLine->GetEnd(), TempEndPixPos);
	if ((TempEndPixPos - TempStartPixPos).Size() < 50)
	{
		PixPos += (TempEndPixPos - TempStartPixPos).GetSafeNormal() * 50;
	}
	YFrontInputBox->SetRenderTransform(FSlateRenderTransform(PixPos));

	InViwportClient->WorldToPixel((YDrawBackLine->GetStart() + YDrawBackLine->GetEnd()) / 2, PixPos);
	PixPos -= ViewportSize / 2;

	InViwportClient->WorldToPixel(YDrawBackLine->GetStart(), TempStartPixPos);
	InViwportClient->WorldToPixel(YDrawBackLine->GetEnd(), TempEndPixPos);
	if ((TempEndPixPos - TempStartPixPos).Size() < 50)
	{
		PixPos += (TempEndPixPos - TempStartPixPos).GetSafeNormal() * 50;
	}
	YBackInputBox->SetRenderTransform(FSlateRenderTransform(PixPos));

	/**@刘克祥 计算X,Y方向总长度*/
	XProjectionDistance = FVector::Distance(XFrontLine->GetEnd(), XBackLine->GetEnd());
	YProjectionDistance = FVector::Distance(YFrontLine->GetEnd(), YBackLine->GetEnd());
}

void "FArmyAxisRuler::Update3D(UArmyEditorViewportClient* InViwportClient, const TArray<FObjectWeakPtr>& InObjects)
{
	FBox SelfBox = CurrentObj->GetBounds();
	FVector SelfCenter = SelfBox.GetCenter();
	FVector Position = CurrentObj->GetBasePos();

	float Hight = Position.Z;

	float Length = 1000000.f;
	XFrontLine->SetStart(Position);
	XFrontLine->SetEnd(Position + FVector::ForwardVector * Length);
	XBackLine->SetStart(Position);
	XBackLine->SetEnd(Position - FVector::ForwardVector * Length);
	YFrontLine->SetStart(Position);
	YFrontLine->SetEnd(Position + FVector::RightVector * Length);
	YBackLine->SetStart(Position);
	YBackLine->SetEnd(Position - FVector::RightVector * Length);

	ZFrontLine->SetStart(Position);
	ZFrontLine->SetEnd(Position + FVector::UpVector *Length);

	ZBackLine->SetStart(Position);
	ZBackLine->SetEnd(Position - FVector::UpVector* FMath::Abs(Hight));


	for (auto ObjIt : InObjects)
	{
		if (ObjIt.Pin() == CurrentObj)
		{
			continue;
		}

		TArray<TSharedPtr<"FArmyLine>> Lines;
		ObjIt.Pin()->GetLines(Lines);
		for (auto LineIt : Lines)
		{
			FVector2D InterPoint;
			FVector InterPoint3D;
			//bool MatchPoint = false;
			FVector XBackEnd = XBackLine->GetEnd();
			FVector XFrontEnd = XFrontLine->GetEnd();
			FVector YBackEnd = YBackLine->GetEnd();
			FVector YFrontEnd = YFrontLine->GetEnd();

			if ("FArmyMath::CalculateLinesIntersection3DTo2D(LineIt->GetStart(), LineIt->GetEnd(), XBackEnd, XFrontEnd, InterPoint3D))
			{
				InterPoint = FVector2D(InterPoint3D);
				if (XFrontLine->GetStart().X <= InterPoint.X && XFrontLine->GetEnd().X >= InterPoint.X &&
					((InterPoint.X >= FMath::Min(LineIt->GetStart().X, LineIt->GetEnd().X) && InterPoint.X <= FMath::Max(LineIt->GetStart().X, LineIt->GetEnd().X)) ||
					(InterPoint.Y >= FMath::Min(LineIt->GetStart().Y, LineIt->GetEnd().Y) && InterPoint.Y <= FMath::Max(LineIt->GetStart().Y, LineIt->GetEnd().Y))))
				{
					XFrontLine->SetEnd(FVector(InterPoint, Hight));
				}
				if (XBackLine->GetStart().X >= InterPoint.X && XBackLine->GetEnd().X <= InterPoint.X &&
					((InterPoint.X >= FMath::Min(LineIt->GetStart().X, LineIt->GetEnd().X) && InterPoint.X <= FMath::Max(LineIt->GetStart().X, LineIt->GetEnd().X)) ||
					(InterPoint.Y >= FMath::Min(LineIt->GetStart().Y, LineIt->GetEnd().Y) && InterPoint.Y <= FMath::Max(LineIt->GetStart().Y, LineIt->GetEnd().Y))))
				{
					XBackLine->SetEnd(FVector(InterPoint, Hight));
					//MatchPoint = true;
				}
			}
			if ("FArmyMath::CalculateLinesIntersection3DTo2D(LineIt->GetStart(), LineIt->GetEnd(), YBackEnd, YFrontEnd, InterPoint3D))
			{
				InterPoint = FVector2D(InterPoint3D);
				if (YFrontLine->GetStart().Y <= InterPoint.Y && YFrontLine->GetEnd().Y >= InterPoint.Y &&
					((InterPoint.X >= FMath::Min(LineIt->GetStart().X, LineIt->GetEnd().X) && InterPoint.X <= FMath::Max(LineIt->GetStart().X, LineIt->GetEnd().X)) ||
					(InterPoint.Y >= FMath::Min(LineIt->GetStart().Y, LineIt->GetEnd().Y) && InterPoint.Y <= FMath::Max(LineIt->GetStart().Y, LineIt->GetEnd().Y))))
				{
					YFrontLine->SetEnd(FVector(InterPoint, Hight));
					//MatchPoint = true;
				}
				if (YBackLine->GetStart().Y >= InterPoint.Y && YBackLine->GetEnd().Y <= InterPoint.Y &&
					((InterPoint.X >= FMath::Min(LineIt->GetStart().X, LineIt->GetEnd().X) && InterPoint.X <= FMath::Max(LineIt->GetStart().X, LineIt->GetEnd().X)) ||
					(InterPoint.Y >= FMath::Min(LineIt->GetStart().Y, LineIt->GetEnd().Y) && InterPoint.Y <= FMath::Max(LineIt->GetStart().Y, LineIt->GetEnd().Y))))
				{
					YBackLine->SetEnd(FVector(InterPoint, Hight));
					//MatchPoint = true;
				}
			}
		}
	}

	{

		FVector2D ViewportSize;
		InViwportClient->GetViewportSize(ViewportSize);
		int32 XObj = FMath::CeilToInt((XFrontLine->GetStart() - XBackLine->GetStart()).Size() * 10);
		int32 YObj = FMath::CeilToInt((YFrontLine->GetStart() - YBackLine->GetStart()).Size() * 10);

		int Lenght = FMath::Abs(XFrontLine->GetStart().X - XFrontLine->GetEnd().X) * 10;
		FText LenText = FText::FromString(FString::Printf(TEXT("%d"), Lenght));
		XFrontInputBox->SetInputText(LenText);
		XFrontInputBox->SetVisibility(Lenght != 0 && Lenght != 1000000.f ? EVisibility::Visible : EVisibility::Collapsed);



		Lenght = FMath::Abs(XBackLine->GetStart().X * 10 - XBackLine->GetEnd().X * 10);
		LenText = FText::FromString(FString::Printf(TEXT("%d"), Lenght));
		XBackInputBox->SetInputText(LenText);
		XBackInputBox->SetVisibility(Lenght != 0 && Lenght != 1000000.f ? EVisibility::Visible : EVisibility::Collapsed);


		Lenght = FMath::Abs(YFrontLine->GetStart().Y - YFrontLine->GetEnd().Y) * 10;
		LenText = FText::FromString(FString::Printf(TEXT("%d"), Lenght));
		YFrontInputBox->SetInputText(LenText);
		YFrontInputBox->SetVisibility(Lenght != 0 && Lenght != 1000000.f ? EVisibility::Visible : EVisibility::Collapsed);


		Lenght = FMath::Abs(YBackLine->GetStart().Y - YBackLine->GetEnd().Y) * 10;
		LenText = FText::FromString(FString::Printf(TEXT("%d"), Lenght));
		YBackInputBox->SetInputText(LenText);
		YBackInputBox->SetVisibility(Lenght != 0 && Lenght != 1000000.f ? EVisibility::Visible : EVisibility::Collapsed);


		if (SceneModel)
		{
			Lenght = FMath::Abs(ZFrontLine->GetStart().Z - ZFrontLine->GetEnd().Z) * 10;
			if (Lenght != 0)
				LenText = FText::FromString(FString::Printf(TEXT("%d"), Lenght));
			ZFrontInputBox->SetInputText(LenText);
			ZFrontInputBox->SetVisibility(Lenght != 0 && Lenght != 1000000.f ? EVisibility::Visible : EVisibility::Collapsed);

			Lenght = FMath::Abs(ZBackLine->GetStart().Z - ZBackLine->GetEnd().Z) * 10;
			LenText = FText::FromString(FString::Printf(TEXT("%d"), Lenght));
			ZBackInputBox->SetInputText(LenText);
			ZBackInputBox->SetVisibility(Lenght != 0 && Lenght != 1000000.f ? EVisibility::Visible : EVisibility::Collapsed);
		}




		FVector2D PixPos;

		InViwportClient->WorldToPixel(Position + FVector::ForwardVector * 50, PixPos);
		PixPos -= ViewportSize / 2;
		XFrontInputBox->SetRenderTransform(FSlateRenderTransform(PixPos));

		InViwportClient->WorldToPixel(Position + FVector::ForwardVector * (-50), PixPos);
		PixPos -= ViewportSize / 2;
		XBackInputBox->SetRenderTransform(FSlateRenderTransform(PixPos));

		InViwportClient->WorldToPixel(Position + FVector::RightVector * 50, PixPos);
		PixPos -= ViewportSize / 2;
		YFrontInputBox->SetRenderTransform(FSlateRenderTransform(PixPos));

		InViwportClient->WorldToPixel(Position + FVector::RightVector*(-50), PixPos);
		PixPos -= ViewportSize / 2;
		YBackInputBox->SetRenderTransform(FSlateRenderTransform(PixPos));

		InViwportClient->WorldToPixel(Position + FVector::UpVector * 50, PixPos);
		PixPos -= ViewportSize / 2;
		ZFrontInputBox->SetRenderTransform(FSlateRenderTransform(PixPos));

		InViwportClient->WorldToPixel(Position + FVector::UpVector*(-50), PixPos);
		PixPos -= ViewportSize / 2;
		ZBackInputBox->SetRenderTransform(FSlateRenderTransform(PixPos));
	}
}

void "FArmyAxisRuler::SetRefCoordinateSystem(const FVector& InBasePos, const FVector& InXDir, const FVector& InYDir, const FVector& InNormal)
{
	BasePoint = InBasePos;
	BaseXDir = InXDir;
	BaseYDir = InYDir;
	BaseNormal = InNormal;
	BasePlane = FPlane(BasePoint, BaseNormal);
}
const FPlane& "FArmyAxisRuler::GetPlane() const
{
	return BasePlane;
}
FVector "FArmyAxisRuler::ToBasePlane(const FVector& InV)
{
	float x = FVector::DotProduct((InV - BasePoint), BaseXDir.GetSafeNormal());
	float y = FVector::DotProduct((InV - BasePoint), BaseYDir.GetSafeNormal());
	float z = FVector::DotProduct((InV - BasePoint), BaseNormal.GetSafeNormal());

	return FVector(x, y, z);
}
FVector "FArmyAxisRuler::FromBasePlane(const FVector& InV)
{
	return BasePoint + InV.X * BaseXDir.GetSafeNormal() + InV.Y * BaseYDir.GetSafeNormal() + InV.Z * BaseNormal.GetSafeNormal();
}