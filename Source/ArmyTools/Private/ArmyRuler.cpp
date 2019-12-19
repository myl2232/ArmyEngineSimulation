#include "ArmyRuler.h"
#include "ArmySceneData.h"
#include "ArmyRoom.h"
#include "ArmyMath.h"
#include "EngineUtils.h"
#include "ArmyViewportClient.h"
#include "ArmyEditorViewportClient.h"


void "FArmyRuler::Update(HAdsorbDetect CaptureInfo, bool In2DModle, bool IgnoreOverlappedWall, FRotator rotator)
{
	this->CaptureInfo = CaptureInfo;
	b2DModle = In2DModle;
	Rotator = rotator;
	this->IgnoreOverlappedWall = IgnoreOverlappedWall;
	DetectAxises();
}

void "FArmyRuler::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (!bVisible)
	{
		return;
	}

	for (auto axis : Axises)
	{
		if (axis->ShouldShow())
		{
			axis->Draw(PDI, View);
			axis->UpdateTextWidgetLocation(View);
			axis->UpdateLineWidth(View);
		}
	}
}

void "FArmyRuler::DetectAxis(TSharedPtr<RulerAxis> Axis)
{
	////过滤轴,仅供测试
	//if (Axis->Type != EAxisType::ZFront)
	//{
	//	return;
	//}

	Axis->Direction.Normalize();

	FCollisionQueryParams TraceParams(TEXT("Pick"), true);
	TraceParams.bTraceAsyncScene = true;

	for (TActorIterator<AActor> ActorItr(GEngine->GameViewport->GetWorld()); ActorItr; ++ActorItr)
	{
		bool isWall = ActorItr->Tags.Num() > 0 && ActorItr->Tags.Contains(TEXT("Wall"));
		if (!isWall || (isWall && ActorItr->bHidden))
		{
			TraceParams.AddIgnoredActor(*ActorItr);
		}

	/*	if (ActorItr->Tags.Num() > 0 && ( ActorItr->Tags.Contains(TEXT("MoveableMeshActor")) 
			|| ActorItr->Tags.Contains(TEXT("HydropowerPipeActor"))
			|| ActorItr->bHidden
			))
		{
			TraceParams.AddIgnoredActor(*ActorItr);
		}*/
	}
	
	auto LineTrace = [&](FVector OffSetDirection)
	{
		if (IgnoreOverlappedWall)
		{
			FHitResult Hit;
			FVector start = Axis->GetStart() + OffSetDirection- Axis->Direction;
			FVector end = start + MaxDistance*Axis->Direction ;

			int32 MaxOverlaptimes = 100; //最多重叠次数
			FVector CurrentPoint=start;
			int32 i = 0;
			while (i<MaxOverlaptimes)
			{
				++i;
				bool FirstLineHit;
				GEngine->GameViewport->GetWorld()->LineTraceSingleByChannel(Hit, CurrentPoint, end, ECC_Visibility, TraceParams);
				FirstLineHit = Hit.bBlockingHit;

				CurrentPoint = Hit.ImpactPoint;
				//往回打一条，检测是否发生重叠
				if (FirstLineHit)
				{
					CurrentPoint = Hit.ImpactPoint;
					GEngine->GameViewport->GetWorld()->LineTraceSingleByChannel(Hit, CurrentPoint + Axis->Direction*MaxOverlapDistance, CurrentPoint - Axis->Direction*MaxOverlapDistance, ECC_Visibility, TraceParams);
					if (Hit.bBlockingHit)
					{
						//发生重叠
						CurrentPoint = CurrentPoint + Axis->Direction*MaxOverlapDistance;
						continue;
						//return CurrentPoint;
					}
				}
				else
				{
					CurrentPoint = end;
				}

				GEngine->GameViewport->GetWorld()->LineTraceSingleByChannel(Hit, CurrentPoint + Axis->Direction*MaxOverlapDistance, start, ECC_Visibility, TraceParams);

				if (!Hit.bBlockingHit)
				{
					return CurrentPoint - OffSetDirection;
				}
				else
				{
					if ((Hit.ImpactPoint - start).IsNearlyZero(0.01f))
					{
						 return CurrentPoint - OffSetDirection;
					}
					return (FVector)Hit.ImpactPoint - OffSetDirection;
				}
			}
			return end - OffSetDirection;
		}
		else
		{
			FHitResult Hit;
			FVector start = Axis->GetStart() - OffSetDirection;
			FVector end = start + MaxDistance*Axis->Direction;
			GEngine->GameViewport->GetWorld()->LineTraceSingleByChannel(Hit, start, end, ECC_Visibility, TraceParams);
			if (!Hit.bBlockingHit)
			{
				return end - OffSetDirection;
			}

			FVector FirstImpacpoint = Hit.ImpactPoint;
			GEngine->GameViewport->GetWorld()->LineTraceSingleByChannel(Hit, FirstImpacpoint, start, ECC_Visibility, TraceParams);
			if (!Hit.bBlockingHit)
			{
				return FirstImpacpoint - OffSetDirection;
			}
			return (FVector)Hit.ImpactPoint - OffSetDirection;
		}
	};
	FQuat rotate(Axis->Direction, 45.0 / 180 * PI);
	FQuat rotate2(Axis->Direction, -45.0 / 180 * PI);
	FQuat rotate3(Axis->Direction, (1- 45.0 / 180) * PI);
	FQuat rotate4(Axis->Direction, -(1 - 45.0 / 180) * PI);

	TArray<FVector> Points; //射线打到的点
	Points.Add(LineTrace(rotate.RotateVector(Axis->OffsetDirection)*Offset));
	Points.Add(LineTrace(rotate2.RotateVector(Axis->OffsetDirection)*Offset));
	Points.Add(LineTrace(rotate3.RotateVector(Axis->OffsetDirection)*Offset));
	Points.Add(LineTrace(rotate4.RotateVector(Axis->OffsetDirection)*Offset));

	TArray<float> Lengthes;
	for (auto Point : Points)
	{
		float Length= (Point - Axis->GetStart()).Size();
		Lengthes.Add( Length<0.1f? MaxDistance : Length);
	}
	float AxisLength = FMath::Min(Lengthes);

	if (HideInfinateAxis && AxisLength >= MaxDistance - 100)
	{
		Axis->SetShouldDraw(false);  
		return;
	}
	Axis->SetEnd(Axis->GetStart() + Axis->Direction*AxisLength);
}
void "FArmyRuler::DetectAxises()
{
	if (!bVisible)
	{
		for (auto Axis : Axises)
		{
			if (Axis->ShouldShow())
			{	
				Axis->SetShouldDraw(false);
			}
		}
		return;
	}
	//重置轴部分数据
	TArray<bool> AxisShow;
	for (auto Axis : Axises)
	{
		AxisShow.Add(true);
		Axis->SetStart(CaptureInfo.AdsPoint);
	}

	FVector XDirection = FVector::ForwardVector;
	FVector YDirection = FVector::RightVector;
	FVector ZDirection = FVector::UpVector;

	FVector XOffsetDirection = FVector::RightVector;
	FVector YOffsetDirection = FVector::ForwardVector;
	FVector ZOffsetDirection = FVector::FVector(1, 1, 0);
	ZOffsetDirection.Normalize();

	//点的位置，这四个变量不完全互斥
	bool OnWall = CaptureInfo.bAdsorbDetect;
	bool AtCorner = CaptureInfo.bAdsorbDetect && (CaptureInfo.OnTop || CaptureInfo.OnFloor || CaptureInfo.bCornerDetect);
	bool OnTop = CaptureInfo.OnTop;
	bool OnFloor = CaptureInfo.OnFloor;
	DrawWithOffset = true;
	if (OnWall && !AtCorner)
	{
		//在2D墙面上
		XDirection = CaptureInfo.WallDirection;
		XOffsetDirection = CaptureInfo.WallNormal;
		ZOffsetDirection = CaptureInfo.WallNormal;

		AxisShow[EAxisType::YFront]=false;
		AxisShow[EAxisType::YBack] = false;
	}
	else if (AtCorner && (OnFloor || OnTop))
	{
		//在墙与地面或天花板的共线上
		if (OnTop)
		{
			AxisShow[EAxisType::ZFront]=false;
			XOffsetDirection = CaptureInfo.WallNormal - FVector::UpVector;//(-FVector::UpVector) ^ CaptureInfo.WallDirection;
			XOffsetDirection.Normalize();
		}
		if (OnFloor)
		{
			AxisShow[EAxisType::ZBack] = false;
			XOffsetDirection = FVector::UpVector ^ CaptureInfo.WallDirection;
		}

		XDirection = CaptureInfo.WallDirection;
		YDirection = XDirection ^ZDirection;

		ZOffsetDirection = CaptureInfo.WallNormal;
		YOffsetDirection = FVector::UpVector;
	}
	else if (AtCorner)
	{
		//两个墙面的夹缝处
		XDirection = CaptureInfo.WallDirection;
		YDirection = CaptureInfo.WallDirection2;

		XOffsetDirection = CaptureInfo.WallNormal;
		YOffsetDirection = CaptureInfo.WallNormal2;
		ZOffsetDirection = XOffsetDirection + YOffsetDirection;
		ZOffsetDirection.Normalize();

	}
	else if (OnTop || OnFloor)
	{
		AxisShow[EAxisType::YFront] = false;
		AxisShow[EAxisType::YBack] = false;



		if (CaptureInfo.bAdsorbDetect)
		{
			XDirection = CaptureInfo.WallDirection;
		}
		ZDirection = XDirection ^ZDirection;

		if (OnTop)
		{
			XOffsetDirection = -FVector::UpVector;
			ZOffsetDirection = -FVector::UpVector;
		}
		else
		{
			XOffsetDirection = FVector::UpVector;
			ZOffsetDirection = FVector::UpVector;
		}
	}
	else
	{
		if(b2DModle)
		{
			AxisShow[EAxisType::ZFront] = false;
			AxisShow[EAxisType::ZBack] = false;
		}

		//在3d空间中
		DrawWithOffset = false;
		XOffsetDirection = FVector::UpVector;
		YOffsetDirection = FVector::UpVector;

		XDirection = Rotator.RotateVector(XDirection);
		YDirection = Rotator.RotateVector(YDirection);
		ZDirection = Rotator.RotateVector(ZDirection);
	}

	for (int32 i=0; i<AxisShow.Num();i++)
	{
		Axises[i]->SetShouldDraw(AxisShow[i]);
	}

	Axises[EAxisType::XFront]->Direction = XDirection;
	Axises[EAxisType::XBack]->Direction = -XDirection;
	Axises[EAxisType::YFront]->Direction = YDirection;
	Axises[EAxisType::YBack]->Direction = -YDirection;
	Axises[EAxisType::ZFront]->Direction = ZDirection;
	Axises[EAxisType::ZBack]->Direction = -ZDirection;

	Axises[EAxisType::XFront]->OffsetDirection = XOffsetDirection;
	Axises[EAxisType::XBack]->OffsetDirection = XOffsetDirection;
	Axises[EAxisType::YFront]->OffsetDirection = YOffsetDirection;
	Axises[EAxisType::YBack]->OffsetDirection = YOffsetDirection;
	Axises[EAxisType::ZFront]->OffsetDirection = ZOffsetDirection;
	Axises[EAxisType::ZBack]->OffsetDirection = ZOffsetDirection;

	for (auto axis : Axises)
	{
		if (axis->ShouldShow())
		{
			DetectAxis(axis);
		}
	}
}

"FArmyRuler::"FArmyRuler()
{
	for (int32 i = 0; i < 6; i++)
	{	
		Axises.Add(MakeShareable(new RulerAxis(EAxisType(i))));
	}
}
"FArmyRuler::~"FArmyRuler() {}

TSharedPtr<"FArmyRuler>&	"FArmyRuler::Get()
{
	static 	TSharedPtr<"FArmyRuler> Me = MakeShareable(new "FArmyRuler);
	return Me;
}

 RulerAxis::RulerAxis(EAxisType type ) :Type(type)
{
	switch (type) {
	case EAxisType::XFront:
	case EAxisType::XBack:
		SetLineColor(FLinearColor::Red);
		OffsetDirection = FVector::RightVector;
		break;
	case EAxisType::YFront:
	case EAxisType::YBack:
		SetLineColor(FLinearColor::Green);
		OffsetDirection = FVector::ForwardVector;
		break;
	case EAxisType::ZFront:
	case EAxisType::ZBack:
		SetLineColor(FLinearColor::Blue);
		OffsetDirection = FVector(1, 1, 0);
		OffsetDirection.Normalize();
		break;
	}

	//线宽
	LineWidth = 2.5f;

	TSharedPtr<SSArmyEditableLabel> EditLabel = SNew(SSArmyEditableLabel).IsIntegerOnly(true);
	LengthLabel = EditLabel;
	LengthLabel.Pin()->OnInputBoxCommitted.BindRaw(this, &RulerAxis::OnTextCommitted);

	//包裹一下LengthLabel，防止LengthLabel太大
	TSharedPtr<SBox> sBox = SNew(SBox)
		.HAlign(EHorizontalAlignment::HAlign_Left)
		.VAlign(EVerticalAlignment::VAlign_Top)
		[
			LengthLabel.Pin().ToSharedRef()
		];
	LengthLabelWrapper = sBox;
	GEngine->GameViewport->AddViewportWidgetContent(LengthLabelWrapper.Pin().ToSharedRef());
	SetShouldDraw(false);
}

 bool RulerAxis::ShouldShow() { return bShouldDraw; }

 void RulerAxis::SetShouldDraw(bool bDraw)
 {
	 bShouldDraw = bDraw;
	 "FArmyLine::SetShouldDraw(bDraw);
	 LengthLabelWrapper.Pin()->SetVisibility(bDraw ? EVisibility::SelfHitTestInvisible : EVisibility::Hidden);
 }

 void RulerAxis::UpdateTextWidgetLocation(const FSceneView * InView)
{
	float DistanceToOrigin=100.0f;
	if (AdjustDistanceToStartPostion)
	{
		if( "FArmyRuler::Get()->Is2DModule() )
		{
		//   
		const float Zoom = GVC->GetOrthoZoom();
		 //                                                    距离系数，越大，距离原点越远
		DistanceToOrigin = FMath::Clamp(Zoom* 0.1f,1.0f, 999999999.0f);
		}
		else
		{
		//                                                                         距离系数，越大，距离原点越远
		DistanceToOrigin = FMath::Clamp((InView->ViewLocation - GetStart()).Size()* 0.3f,1.0f, 10000.0f);
		}
	}

	FVector2D PixelLocation;
	InView->WorldToPixel(GetStart() + DistanceToOrigin * Direction, PixelLocation);
	LengthLabel.Pin()->SetRenderTransform(FSlateRenderTransform(PixelLocation));
}
 void RulerAxis::UpdateLineWidth(const FSceneView*  InView)
 {
	 if (AdjustLinewidth)
	 {   //                                                                  线宽系数，越大线越宽
		 LineWidth = FMath::Clamp((InView->ViewLocation - GetStart()).Size()* 0.03f,2.5f, 10.0f);
	 }
 }
//输入完文字发生的事件
 void RulerAxis::OnTextCommitted(const FText & test, const SWidget * widget)
{
	 float CurrentLength = FMath::GridSnap((GetEnd() - GetStart()).Size(), 0.01f)*10; //mm
	 int32 NewLength = FCString::Atoi(*test.ToString()); //mm

	 if ("FArmyRuler::Get()->AxisOperationDelegate.IsBound())
	 {
		 "FArmyRuler::Get()->AxisOperationDelegate.Execute(Direction*(-NewLength+ CurrentLength)/10);
	 }

	 //手动刷新label
	 if (NewLength == CurrentLength)
	 {
		 UpdateText(true);
	 }
}
 void RulerAxis::SetStart(const FVector& value)
 {
	 StartPostion = value;	
 }
 void RulerAxis::SetEnd(const FVector& value)
 {
	EndPostion = value;
	 UpdateText();
 }

 void RulerAxis::UpdateText(bool ForceUpdate)
 {
	 int32 NewLength = FMath::CeilToInt(FMath::GridSnap((GetEnd() - GetStart()).Size(), 0.01f) *10);
	 if (ForceUpdate || PreLength != NewLength  )
	 {
		 LengthLabel.Pin()->SetInputText(FText::FromString(FString::Printf(TEXT("%d mm"), NewLength)));
		 PreLength = NewLength;
	 }
 }

 bool RulerAxis::IsZ()
 {
	 return (Type == EAxisType::ZFront || Type == EAxisType::ZBack);
 }

 void "FArmyRuler::Show(bool Visible)
 { 
	 bVisible = Visible;
	 if (!bVisible)
	 {
		 for (auto Axis : Axises)
		 {
			 Axis->SetShouldDraw(false);
		 }
	 }
 };

 FVector RulerAxis::GetStart() const
 {
	 return StartPostion;
 }
 FVector RulerAxis::GetEnd()  const
 {
	 return EndPostion;
 }

 void RulerAxis::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
 {
	 float OffSetDistance = "FArmyRuler::Get()->DrawWithOffset? 0.1f: .0f;
	 "FArmyLine::SetStart(StartPostion + OffsetDirection*OffSetDistance);
	 "FArmyLine::SetEnd(EndPostion+ OffsetDirection*OffSetDistance);
	 "FArmyLine::Draw(PDI, View);
 }

