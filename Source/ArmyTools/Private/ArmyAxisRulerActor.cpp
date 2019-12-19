#include "ArmyAxisRulerActor.h"
#include "SSArmyEditableLabel.h"
#include "ArmyEditorEngine.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyMath.h"
#include "Engine.h"

"FArmyAxisRulerActor::"FArmyAxisRulerActor()
{
}

"FArmyAxisRulerActor::~"FArmyAxisRulerActor()
{

}
void "FArmyAxisRulerActor::Init(TSharedPtr<SOverlay> ParentWidget)
{
	SAssignNew(XFrontInputBox, SSArmyEditableLabel)
		.Visibility(EVisibility::Collapsed);
	XFrontInputBox->OnInputBoxCommitted = FOnInPutValueCommited::CreateRaw(this, &"FArmyAxisRulerActor::OnTextCommitted);
	SAssignNew(XBackInputBox, SSArmyEditableLabel)
		.Visibility(EVisibility::Collapsed);
	XBackInputBox->OnInputBoxCommitted = FOnInPutValueCommited::CreateRaw(this, &"FArmyAxisRulerActor::OnTextCommitted);
	SAssignNew(YFrontInputBox, SSArmyEditableLabel)
		.Visibility(EVisibility::Collapsed);
	YFrontInputBox->OnInputBoxCommitted = FOnInPutValueCommited::CreateRaw(this, &"FArmyAxisRulerActor::OnTextCommitted);
	SAssignNew(YBackInputBox, SSArmyEditableLabel)
		.Visibility(EVisibility::Collapsed);
	YBackInputBox->OnInputBoxCommitted = FOnInPutValueCommited::CreateRaw(this, &"FArmyAxisRulerActor::OnTextCommitted);
	SAssignNew(ZFrontInputBox, SSArmyEditableLabel)
		.Visibility(EVisibility::Collapsed);
	ZFrontInputBox->OnInputBoxCommitted = FOnInPutValueCommited::CreateRaw(this, &"FArmyAxisRulerActor::OnTextCommitted);
	SAssignNew(ZBackInputBox, SSArmyEditableLabel)
		.Visibility(EVisibility::Collapsed);
	ZBackInputBox->OnInputBoxCommitted = FOnInPutValueCommited::CreateRaw(this, &"FArmyAxisRulerActor::OnTextCommitted);

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
	
		ParentWidget->AddSlot ()
			.HAlign (HAlign_Center)
			.VAlign (VAlign_Center)
			[
				ZFrontInputBox.ToSharedRef ()
			];
		ParentWidget->AddSlot ()
			.HAlign (HAlign_Center)
			.VAlign (VAlign_Center)
			[
				ZBackInputBox.ToSharedRef ()
			];
}
//检查输入是否有效
bool "FArmyAxisRulerActor::CheckStringIsVaild(const FString& str)
{
	FString Reg = TEXT("^[0-9]*$");
	FRegexPattern Pattern(Reg);
	FRegexMatcher regMatcher(Pattern, str);
	regMatcher.SetLimits(0, str.Len());
	return regMatcher.FindNext();
}
void "FArmyAxisRulerActor::OnViewTypeChanged(int32 InViewType)
{
	CurrentViewType = InViewType;
}
void "FArmyAxisRulerActor::OnTextCommitted(const FText& InText, const SWidget* InWidget)
{
	float V = FCString::Atof(*InText.ToString());

	TArray<AActor*> SelectedActors;
	GArmyEditor->GetSelectedActors(SelectedActors);

	if (SelectedActors.Num() > 0)
	{
		AActor* SelectedActor = SelectedActors[0];
		if (InWidget == XFrontInputBox.Get() && XFrontInputBox->HasTextChangedFromOriginal())
		{
			FVector Move(XFrontV - V / 10.f, 0, 0);
			FRotator Rot(ForceInitToZero);
			FVector Scale(1,1,1);
			GArmyEditor->ApplyDeltaToActor(SelectedActor,true,&Move,NULL,NULL);
		}
		else if (InWidget == XBackInputBox.Get() && XBackInputBox->HasTextChangedFromOriginal())
		{
			FVector Move(V / 10.f - XBackV, 0, 0);
			FRotator Rot(ForceInitToZero);
			FVector Scale(1, 1, 1);
			GArmyEditor->ApplyDeltaToActor(SelectedActor, true, &Move, NULL,NULL);
		}
		else if (InWidget == YFrontInputBox.Get() && YFrontInputBox->HasTextChangedFromOriginal())
		{
			FVector Move(0, YFrontV - V / 10.f, 0);
			FRotator Rot(ForceInitToZero);
			FVector Scale(1, 1, 1);
			GArmyEditor->ApplyDeltaToActor(SelectedActor, true, &Move, NULL,NULL);
		}
		else if (InWidget == YBackInputBox.Get() && YBackInputBox->HasTextChangedFromOriginal())
		{
			FVector Move(0, V / 10.f - YBackV, 0);
			FRotator Rot(ForceInitToZero);
			FVector Scale(1, 1, 1);
			GArmyEditor->ApplyDeltaToActor(SelectedActor, true, &Move, NULL,NULL);
		}
		else if (InWidget == ZFrontInputBox.Get() && ZFrontInputBox->HasTextChangedFromOriginal())
		{
			FVector Move(0, 0, ZFrontV - V / 10.f);
			FRotator Rot(ForceInitToZero);
			FVector Scale(1, 1, 1);
			GArmyEditor->ApplyDeltaToActor(SelectedActor, true, &Move, NULL,NULL);
		}
		else if (InWidget == ZBackInputBox.Get() && ZBackInputBox->HasTextChangedFromOriginal())
		{
			FVector Move(0, 0, V / 10.f - ZBackV);
			FRotator Rot(ForceInitToZero);
			FVector Scale(1, 1, 1);
			GArmyEditor->ApplyDeltaToActor(SelectedActor, true, &Move, NULL,NULL);
		}
	}
}
void "FArmyAxisRulerActor::SetAxisVisible(AxisType InType, bool InVisible)
{
	switch (InType)
	{
	case AXIS_XFRONT:
		XFrontAxis.Visible = InVisible;
		XFrontInputBox->SetVisibility(InVisible ? EVisibility::Visible : EVisibility::Collapsed);
		break;
	case AXIS_XBACK:
		XBackAxis.Visible = InVisible;
		XBackInputBox->SetVisibility(InVisible ? EVisibility::Visible : EVisibility::Collapsed);
		break;
	case AXIS_YFRONT:
		YFrontAxis.Visible = InVisible;
		YFrontInputBox->SetVisibility(InVisible ? EVisibility::Visible : EVisibility::Collapsed);
		break;
	case AXIS_YBACK:
		YBackAxis.Visible = InVisible;
		YBackInputBox->SetVisibility(InVisible ? EVisibility::Visible : EVisibility::Collapsed);
		break;
	case AXIS_ZFRONT:
		ZFrontAxis.Visible = InVisible;
		ZFrontInputBox->SetVisibility(InVisible ? EVisibility::Visible : EVisibility::Collapsed);
		break;
	case AXIS_ZBACK:
		ZBackAxis.Visible = InVisible;
		ZBackInputBox->SetVisibility(InVisible ? EVisibility::Visible : EVisibility::Collapsed);
		break;
	case AXIS_ALL:
	{
		ShowAxis = InVisible;

		XFrontAxis.Visible = InVisible;
		XBackAxis.Visible = InVisible;
		YFrontAxis.Visible = InVisible;
		YBackAxis.Visible = InVisible;
		ZFrontAxis.Visible = InVisible;
		ZBackAxis.Visible = InVisible;

		XFrontInputBox->SetVisibility(InVisible ? EVisibility::Visible : EVisibility::Collapsed);
		XBackInputBox->SetVisibility(InVisible ? EVisibility::Visible : EVisibility::Collapsed);
		YFrontInputBox->SetVisibility(InVisible ? EVisibility::Visible : EVisibility::Collapsed);
		YBackInputBox->SetVisibility(InVisible ? EVisibility::Visible : EVisibility::Collapsed);
		ZFrontInputBox->SetVisibility(InVisible ? EVisibility::Visible : EVisibility::Collapsed);
		ZBackInputBox->SetVisibility(InVisible ? EVisibility::Visible : EVisibility::Collapsed);
	}
		break;
	default:
		break;
	}
}
void "FArmyAxisRulerActor::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (ShowAxis)
	{
		if (ZBackAxis.TheWorld && ZBackAxis.Visible)DrawMeasureLine(ZBackAxis.TheWorld, ZBackAxis.TraceStart, ZBackAxis.TraceEnd,FColor::Blue);
		if (ZFrontAxis.TheWorld && ZFrontAxis.Visible)DrawMeasureLine(ZFrontAxis.TheWorld, ZFrontAxis.TraceStart, ZFrontAxis.TraceEnd, FColor::Blue);
		if (XBackAxis.TheWorld && XBackAxis.Visible)DrawMeasureLine(XBackAxis.TheWorld, XBackAxis.TraceStart, XBackAxis.TraceEnd, FColor::Red);
		if (XFrontAxis.TheWorld && XFrontAxis.Visible)DrawMeasureLine(XFrontAxis.TheWorld, XFrontAxis.TraceStart, XFrontAxis.TraceEnd, FColor::Red);
		if (YBackAxis.TheWorld && YBackAxis.Visible)DrawMeasureLine(YBackAxis.TheWorld, YBackAxis.TraceStart, YBackAxis.TraceEnd, FColor::Green);
		if (YFrontAxis.TheWorld && YFrontAxis.Visible)DrawMeasureLine(YFrontAxis.TheWorld, YFrontAxis.TraceStart, YFrontAxis.TraceEnd, FColor::Green);
	}
}
void "FArmyAxisRulerActor::DrawMeasureLine(UWorld* InWorld,FVector InStart, FVector InEnd, FColor InColor)
{
	DrawDebugLine(InWorld, InStart, InEnd, InColor, false, -1.f, 1.f, 1.f);
}
bool "FArmyAxisRulerActor::OnCollisionLocation(UWorld* InWorld,const FVector& InTraceStart,const FVector& InTraceEnd, FHitResult& OutHit,const TArray<AActor*>& InIgnoredArray)
{
	static FName TraceTag = FName(TEXT("Pick"));
	FCollisionQueryParams TraceParams(TraceTag, true);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.AddIgnoredActors(InIgnoredArray);

	return InWorld->LineTraceSingleByChannel(OutHit, InTraceStart, InTraceEnd, ECC_Visibility, TraceParams);
}
void "FArmyAxisRulerActor::Update(UArmyEditorViewportClient* InViewportClient)
{
	TArray<AActor*> SelectedActors;
	FBox BoudingBox(ForceInitToZero);
	FVector AloneActorLocation(ForceInitToZero);
	bool IsPointObject = false;

	for (FSelectionIterator It(GArmyEditor->GetSelectedActorIterator()); It; ++It)
	{
		if (It->GetName().Contains(TEXT("NOLIST-")))
		{
			continue;
		}
		if (AActor* Actor = Cast<AActor>(*It))
		{
			SelectedActors.Add(Actor);
			if (Actor->Tags.Contains("POINTOBJECT") && SelectedActors.Num() == 1)
			{
				IsPointObject = true;
				AloneActorLocation = Actor->GetActorLocation();
			}
			else
			{
				IsPointObject = false;
			}
			FBox TempBox = InViewportClient->GetActorCollisionBox(Actor);
			BoudingBox += TempBox;
		}
	}

	if (SelectedActors.Num() == 0 || BoudingBox.GetSize().Size() == 0)
	{
		if (ShowAxis)
		{
			ShowAxis = false;
			SetAxisVisible(AXIS_ALL, ShowAxis);
		}
		return;
	}
	else
	{
		if (!ShowAxis)
		{
			ShowAxis = true;
			SetAxisVisible(AXIS_ALL, ShowAxis);
			if (CurrentViewType == EArmyLevelViewportType::LVT_OrthoXY)
			{
				SetAxisVisible(AXIS_ZFRONT, false);
				SetAxisVisible(AXIS_ZBACK, false);
			}
		}
	}

	
	///////////////////////////////////////////////////////
	FVector BasePoint = BoudingBox.GetCenter();
	//down
	FVector TraceStart = IsPointObject ? AloneActorLocation : FVector(BasePoint.X, BasePoint.Y, BoudingBox.Min.Z);
	FVector TraceEnd = TraceStart;
	TraceEnd.Z -= 10000;

	FHitResult Hit(ForceInit);
	if (OnCollisionLocation(InViewportClient->GetWorld(),TraceStart, TraceEnd, Hit, SelectedActors))
	{
		TraceEnd = Hit.ImpactPoint;
	}
	ZBackAxis.TheWorld = InViewportClient->GetWorld();
	ZBackAxis.TraceStart = TraceStart;
	ZBackAxis.TraceEnd = TraceEnd;
	ZBackAxis.AxisColor = FColor::Blue;
	//up
	TraceStart = IsPointObject ? AloneActorLocation : FVector(BasePoint.X, BasePoint.Y, BoudingBox.Max.Z);
	TraceEnd = TraceStart;
	TraceEnd.Z += 10000;

	Hit.Init();
	if (OnCollisionLocation(InViewportClient->GetWorld(), TraceStart, TraceEnd, Hit, SelectedActors))
	{
		TraceEnd = Hit.ImpactPoint;
	}
	ZFrontAxis.TheWorld = InViewportClient->GetWorld();
	ZFrontAxis.TraceStart = TraceStart;
	ZFrontAxis.TraceEnd = TraceEnd;
	ZFrontAxis.AxisColor = FColor::Blue;
	//left
	TraceStart = IsPointObject ? AloneActorLocation : FVector(BoudingBox.Min.X, BasePoint.Y, BasePoint.Z);
	TraceEnd = TraceStart;
	TraceEnd.X -= 10000;

	Hit.Init();
	if (OnCollisionLocation(InViewportClient->GetWorld(), TraceStart, TraceEnd, Hit, SelectedActors))
	{
		TraceEnd = Hit.ImpactPoint;
	}
	XBackAxis.TheWorld = InViewportClient->GetWorld();
	XBackAxis.TraceStart = TraceStart;
	XBackAxis.TraceEnd = TraceEnd;
	XBackAxis.AxisColor = FColor::Red;
	//right
	TraceStart = IsPointObject ? AloneActorLocation : FVector(BoudingBox.Max.X, BasePoint.Y, BasePoint.Z);
	TraceEnd = TraceStart;
	TraceEnd.X += 10000;

	Hit.Init();
	if (OnCollisionLocation(InViewportClient->GetWorld(), TraceStart, TraceEnd, Hit, SelectedActors))
	{
		TraceEnd = Hit.ImpactPoint;
	}
	XFrontAxis.TheWorld = InViewportClient->GetWorld();
	XFrontAxis.TraceStart = TraceStart;
	XFrontAxis.TraceEnd = TraceEnd;
	XFrontAxis.AxisColor = FColor::Red;
	//back
	TraceStart = IsPointObject ? AloneActorLocation : FVector(BasePoint.X, BoudingBox.Min.Y, BasePoint.Z);
	TraceEnd = TraceStart;
	TraceEnd.Y -= 10000;

	Hit.Init();
	if (OnCollisionLocation(InViewportClient->GetWorld(), TraceStart, TraceEnd, Hit, SelectedActors))
	{
		TraceEnd = Hit.ImpactPoint;
	}
	YBackAxis.TheWorld = InViewportClient->GetWorld();
	YBackAxis.TraceStart = TraceStart;
	YBackAxis.TraceEnd = TraceEnd;
	YBackAxis.AxisColor = FColor::Green;
	//front
	TraceStart = IsPointObject ? AloneActorLocation : FVector(BasePoint.X, BoudingBox.Max.Y, BasePoint.Z);
	TraceEnd = TraceStart;
	TraceEnd.Y += 10000;

	Hit.Init();
	if (OnCollisionLocation(InViewportClient->GetWorld(), TraceStart, TraceEnd, Hit, SelectedActors))
	{
		TraceEnd = Hit.ImpactPoint;
	}
	YFrontAxis.TheWorld = InViewportClient->GetWorld();
	YFrontAxis.TraceStart = TraceStart;
	YFrontAxis.TraceEnd = TraceEnd;
	YFrontAxis.AxisColor = FColor::Green;
	//////////////////////////////////////////////////////////
	{
		FVector2D ViewportSize;
		InViewportClient->GetViewportSize(ViewportSize);

		float TempV = FMath::FloorToFloat((XFrontAxis.TraceEnd - XFrontAxis.TraceStart).Size() * 100) / 100;
		if (TempV != XFrontV)
		{
			XFrontV = TempV;
			int32 V = FMath::CeilToInt(XFrontV * 10);
			FText LenText = FText::FromString(FString::Printf(TEXT("%d"), V));
			XFrontInputBox->SetInputText(LenText);
		}
		TempV = FMath::FloorToFloat((XBackAxis.TraceEnd - XBackAxis.TraceStart).Size() * 100) / 100;
		if (TempV != XBackV)
		{
			XBackV = TempV;
			int32 V = FMath::CeilToInt(XBackV * 10);
			FText LenText = FText::FromString(FString::Printf(TEXT("%d"), V));
			XBackInputBox->SetInputText(LenText);
		}

		TempV = FMath::FloorToFloat((YFrontAxis.TraceEnd - YFrontAxis.TraceStart).Size() * 100) / 100;
		if (TempV != YFrontV)
		{
			YFrontV = TempV;
			int32 V = FMath::CeilToInt(YFrontV * 10);
			FText LenText = FText::FromString(FString::Printf(TEXT("%d"), V));
			YFrontInputBox->SetInputText(LenText);
		}

		TempV = FMath::FloorToFloat((YBackAxis.TraceEnd - YBackAxis.TraceStart).Size() * 100) / 100;
		if (TempV != YBackV)
		{
			YBackV = TempV;
			int32 V = FMath::CeilToInt(YBackV * 10);
			FText LenText = FText::FromString(FString::Printf(TEXT("%d"), V));
			YBackInputBox->SetInputText(LenText);
		}

		TempV = FMath::FloorToFloat((ZFrontAxis.TraceEnd - ZFrontAxis.TraceStart).Size() * 100) / 100;
		if (TempV != ZFrontV)
		{
			ZFrontV = TempV;
			int32 V = FMath::CeilToInt(ZFrontV * 10);
			FText LenText = FText::FromString(FString::Printf(TEXT("%d"), V));
			ZFrontInputBox->SetInputText(LenText);
		}

		TempV = FMath::FloorToFloat((ZBackAxis.TraceEnd - ZBackAxis.TraceStart).Size() * 100) / 100;
		if (TempV != ZBackV)
		{
			ZBackV = TempV;
			int32 V = FMath::CeilToInt(ZBackV * 10);
			FText LenText = FText::FromString(FString::Printf(TEXT("%d"), V));
			ZBackInputBox->SetInputText(LenText);
		}

		//UpdateTransform();
		FVector2D PixPos, TempStartPixPos, TempEndPixPos;
		InViewportClient->WorldToPixel((XFrontAxis.TraceStart + XFrontAxis.TraceEnd) / 2, PixPos);
		PixPos -= ViewportSize / 2;

		InViewportClient->WorldToPixel(XFrontAxis.TraceStart, TempStartPixPos);
		InViewportClient->WorldToPixel(XFrontAxis.TraceEnd, TempEndPixPos);
		if ((TempEndPixPos - TempStartPixPos).Size() < 80)
		{
			PixPos += FVector2D(80, 0);
		}
		XFrontInputBox->SetRenderTransform(FSlateRenderTransform(PixPos));

		InViewportClient->WorldToPixel((XBackAxis.TraceStart + XBackAxis.TraceEnd) / 2, PixPos);
		PixPos -= ViewportSize / 2;

		InViewportClient->WorldToPixel(XBackAxis.TraceStart, TempStartPixPos);
		InViewportClient->WorldToPixel(XBackAxis.TraceEnd, TempEndPixPos);
		if ((TempEndPixPos - TempStartPixPos).Size() < 80)
		{
			PixPos -= FVector2D(80, 0);
		}
		XBackInputBox->SetRenderTransform(FSlateRenderTransform(PixPos));

		InViewportClient->WorldToPixel((YFrontAxis.TraceStart + YFrontAxis.TraceEnd) / 2, PixPos);
		PixPos -= ViewportSize / 2;

		InViewportClient->WorldToPixel(YFrontAxis.TraceStart, TempStartPixPos);
		InViewportClient->WorldToPixel(YFrontAxis.TraceEnd, TempEndPixPos);
		if ((TempEndPixPos - TempStartPixPos).Size() < 50)
		{
			PixPos += FVector2D(0, 50);
		}
		YFrontInputBox->SetRenderTransform(FSlateRenderTransform(PixPos));

		InViewportClient->WorldToPixel((YBackAxis.TraceStart + YBackAxis.TraceEnd) / 2, PixPos);
		PixPos -= ViewportSize / 2;

		InViewportClient->WorldToPixel(YBackAxis.TraceStart, TempStartPixPos);
		InViewportClient->WorldToPixel(YBackAxis.TraceEnd, TempEndPixPos);
		if ((TempEndPixPos - TempStartPixPos).Size() < 50)
		{
			PixPos -= FVector2D(0, 50);
		}
		YBackInputBox->SetRenderTransform(FSlateRenderTransform(PixPos));

		InViewportClient->WorldToPixel((ZFrontAxis.TraceStart + ZFrontAxis.TraceEnd) / 2, PixPos);
		PixPos -= ViewportSize / 2;

		InViewportClient->WorldToPixel(ZFrontAxis.TraceStart, TempStartPixPos);
		InViewportClient->WorldToPixel(ZFrontAxis.TraceEnd, TempEndPixPos);
		if ((TempEndPixPos - TempStartPixPos).Size() < 50)
		{
			PixPos += FVector2D(0, 50);
		}
		ZFrontInputBox->SetRenderTransform(FSlateRenderTransform(PixPos));

		InViewportClient->WorldToPixel((ZBackAxis.TraceStart + ZBackAxis.TraceEnd) / 2, PixPos);
		PixPos -= ViewportSize / 2;

		InViewportClient->WorldToPixel(ZBackAxis.TraceStart, TempStartPixPos);
		InViewportClient->WorldToPixel(ZBackAxis.TraceEnd, TempEndPixPos);
		if ((TempEndPixPos - TempStartPixPos).Size() < 50)
		{
			PixPos -= FVector2D(0, 50);
		}
		ZBackInputBox->SetRenderTransform(FSlateRenderTransform(PixPos));
	}
}