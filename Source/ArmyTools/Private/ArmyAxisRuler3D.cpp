#include "ArmyAxisRuler3D.h"
#include "SSArmyEditableLabel.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyMath.h"
#include "ArmyRoom.h"
#include "ArmyLine.h"
#include "SBorder.h"
#include "Engine/Engine.h"
#include "CanvasTypes.h"
#include "ArmySceneData.h"
#include "Runtime/Engine/Classes/Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/BodySetup.h"
#include "ArmyRoomSpaceArea.h"
#include "ArmyPlayerController.h"
#include "ArmyEditorViewportClient.h"
#include "Engine.h"

#define LOCTEXT_NAMESPACE ""FArmyAxisRuler3D" 

"FArmyAxisRuler3D::"FArmyAxisRuler3D():bInRoom(false)
, Pos(FVector2D::ZeroVector)
, bShowXAxis(true)
, bShowYAxis(true)
, bShowZAxis(true)
, bShowX(true)
, bShowY(true)
, bShowZ(true)
,bShowXView(true)
,bShowYView(true)
,bShowZView(true)
,bActor(false)
,mCurrentActor(nullptr)
{
	XFrontLine = MakeShareable(new "FArmyLine);
	XBackLine = MakeShareable(new "FArmyLine);
	YFrontLine = MakeShareable(new "FArmyLine);
	YBackLine = MakeShareable(new "FArmyLine);
	ZFrontLine = MakeShareable(new "FArmyLine);
	ZBackLine = MakeShareable(new "FArmyLine);

	XFrontLine->SetLineWidth(2.f);
	XBackLine->SetLineWidth(2.f);
	YFrontLine->SetLineWidth(2.f);
	YBackLine->SetLineWidth(2.f);
	ZFrontLine->SetLineWidth(2.f);
	ZBackLine->SetLineWidth(2.f);

	XFrontLine->SetLineColor(FLinearColor(0.594f,0.0197f,0.0f));
	XBackLine->SetLineColor(FLinearColor(0.594f,0.0197f,0.0f));
	YFrontLine->SetLineColor(FLinearColor(0.1349f,0.3959f,0.0f));
	YBackLine->SetLineColor (FLinearColor(0.1349f,0.3959f,0.0f));
	ZFrontLine->SetLineColor( FLinearColor(0.0251f,0.207f,0.85f));
	ZBackLine->SetLineColor (FLinearColor(0.0251f,0.207f,0.85f));

	XFrontLine->DepthPriority = SDPG_World;
	XBackLine->DepthPriority = SDPG_World;
	YFrontLine->DepthPriority = SDPG_World;
	YBackLine->DepthPriority = SDPG_World;
	ZFrontLine->DepthPriority = SDPG_World;
	ZBackLine->DepthPriority = SDPG_World;

	//XFrontLine->SetLineColor(FLinearColor::Gray);
	//XBackLine->SetLineColor(FLinearColor::Gray);
	//YFrontLine->SetLineColor(FLinearColor::Gray);
	//YBackLine->SetLineColor(FLinearColor::Gray);
	//ZFrontLine->SetLineColor(FLinearColor::Gray);
	//ZBackLine->SetLineColor(FLinearColor::Gray);
}

"FArmyAxisRuler3D::~"FArmyAxisRuler3D()
{

}
void "FArmyAxisRuler3D::Init(TSharedPtr<SOverlay> ParentWidget)
{
	auto ShowFuncX = [&]()
	{
		return XShow; 
	};
	auto ShowFuncY = [&]()
	{
		return YShow;
	};
	auto ShowFuncZ = [&]()
	{
		return ZShow;
	};
	SAssignNew (XFrontInputBox,SSArmyEditableLabel)
		.Visibility_Lambda(ShowFuncX);
	XFrontInputBox->OnInputBoxCommitted = FOnInPutValueCommited::CreateRaw (this,&"FArmyAxisRuler3D::OnTextCommitted);
	SAssignNew(XBackInputBox,SSArmyEditableLabel)
		.Visibility_Lambda(ShowFuncX);
	XBackInputBox->OnInputBoxCommitted = FOnInPutValueCommited::CreateRaw (this,&"FArmyAxisRuler3D::OnTextCommitted);
	SAssignNew (YFrontInputBox,SSArmyEditableLabel)
		.Visibility_Lambda(ShowFuncY);
	YFrontInputBox->OnInputBoxCommitted = FOnInPutValueCommited::CreateRaw (this,&"FArmyAxisRuler3D::OnTextCommitted);
	SAssignNew (YBackInputBox,SSArmyEditableLabel)
		.Visibility_Lambda(ShowFuncY);
	YBackInputBox->OnInputBoxCommitted = FOnInPutValueCommited::CreateRaw (this,&"FArmyAxisRuler3D::OnTextCommitted);
	SAssignNew (ZFrontInputBox,SSArmyEditableLabel)
		.Visibility_Lambda(ShowFuncZ);
	ZFrontInputBox->OnInputBoxCommitted = FOnInPutValueCommited::CreateRaw (this,&"FArmyAxisRuler3D::OnTextCommitted);
	SAssignNew (ZBackInputBox,SSArmyEditableLabel)
		.Visibility_Lambda(ShowFuncZ);
	ZBackInputBox->OnInputBoxCommitted = FOnInPutValueCommited::CreateRaw (this,&"FArmyAxisRuler3D::OnTextCommitted);
	
	ParentWidget->AddSlot()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	[
		XFrontInputBox.ToSharedRef()
	];
	ParentWidget->AddSlot()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	[
		XBackInputBox.ToSharedRef()
	];
	ParentWidget->AddSlot()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	[
		YFrontInputBox.ToSharedRef()
	];
	ParentWidget->AddSlot()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	[
		YBackInputBox.ToSharedRef()
	];
	ParentWidget->AddSlot()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	[
		ZFrontInputBox.ToSharedRef()
	];
	ParentWidget->AddSlot()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	[
		ZBackInputBox.ToSharedRef()
	];
}

//检查输入是否有效
bool "FArmyAxisRuler3D::CheckStringIsVaild (const FString& str)
{
	FString Reg = TEXT ("^[0-9]*$");
	FRegexPattern Pattern (Reg);
	FRegexMatcher regMatcher (Pattern,str);
	regMatcher.SetLimits (0,str.Len ());
	return regMatcher.FindNext ();
}

void "FArmyAxisRuler3D::OnTextCommitted (const FText& InText,const SWidget* InWidget)
{
	//只能输入数字
	static const SWidget* PreWidget = NULL;
	static FText PreText;
	//if(PreWidget==InWidget && PreText.CompareTo (InText)==0)
	//{
	//	return;
	//}
	//else
	{
		PreText = InText;
		PreWidget = InWidget;
	}
	float V = FCString::Atof (*InText.ToString());// / 10.f;
	if(bInRoom)
	{
		FVector Direction = FVector::ZeroVector;
		if(InWidget==XFrontInputBox.Get ())
		{
			FText LenText = FText::FromString (FString::Printf (TEXT ("%d mm"),FMath::CeilToInt (V)));
			XFrontInputBox->SetInputText(LenText);
			PXFrontV = V;
			Direction = ForwardVector*(XFrontV-PXFrontV)/10.f;
		}
		else if(InWidget==XBackInputBox.Get ())
		{
			FText LenText = FText::FromString (FString::Printf (TEXT ("%d mm"),FMath::CeilToInt (V)));
			XBackInputBox->SetInputText(LenText);
			PXBackV=V;
			Direction = ForwardVector*(PXBackV - XBackV)/10.f;
		}
		else if(InWidget==YFrontInputBox.Get ())
		{
			
			FText LenText = FText::FromString (FString::Printf (TEXT ("%d mm"),FMath::CeilToInt (V)));
			YFrontInputBox->SetInputText(LenText);
			PYFrontV=V;
			Direction=RightVector*(YFrontV-PYFrontV)/10.f;
		}
		else if(InWidget==YBackInputBox.Get ())
		{
			FText LenText = FText::FromString (FString::Printf (TEXT ("%d mm"),FMath::CeilToInt (V)));
			YBackInputBox->SetInputText(LenText);
			PYBackV=V;
			Direction=-RightVector*(YBackV - PYBackV)/10.f;
		}
		else if(InWidget==ZFrontInputBox.Get ())
		{
			FText LenText=FText::FromString (FString::Printf (TEXT ("%d mm"),FMath::CeilToInt (V)));
			ZFrontInputBox->SetInputText(LenText);
			PZFrontV=V;
			Direction=UpVector*( ZFrontV - PZFrontV)/10.f;
		}
		else if(InWidget==ZBackInputBox.Get())
		{
			FText LenText=FText::FromString (FString::Printf (TEXT ("%d mm"),FMath::CeilToInt (V)));
			ZBackInputBox->SetInputText(LenText);
			PZBackV=V;
			Direction = UpVector*(PZBackV - ZBackV)/10.f;
		}
		if(AxisOperationDelegate.IsBound())
		{
			AxisOperationDelegate.Execute(Direction);
		}
	}
}

void "FArmyAxisRuler3D::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	//if (bInRoom)
	{
		/**@刘克祥控制各个轴标尺显示*/
		if (bShowXAxis&&bShowXView&&(bShowX)) {
			XFrontLine->Draw(PDI, View);
			XBackLine->Draw(PDI, View);
			if (XBackAxis.TheWorld && XBackAxis.Visible)DrawMeasureLine(XBackAxis.TheWorld,XBackAxis.TraceStart,XBackAxis.TraceEnd,FColor::Red);
			if (XFrontAxis.TheWorld && XFrontAxis.Visible)DrawMeasureLine(XFrontAxis.TheWorld,XFrontAxis.TraceStart,XFrontAxis.TraceEnd,FColor::Red);
		}
		if (bShowYAxis&&bShowYView&&(bShowY)) {
			YFrontLine->Draw(PDI, View);
			YBackLine->Draw(PDI, View);
			if (YBackAxis.TheWorld && YBackAxis.Visible)DrawMeasureLine(YBackAxis.TheWorld,YBackAxis.TraceStart,YBackAxis.TraceEnd,FColor::Green);
			if (YFrontAxis.TheWorld && YFrontAxis.Visible)DrawMeasureLine(YFrontAxis.TheWorld,YFrontAxis.TraceStart,YFrontAxis.TraceEnd,FColor::Green);

		}
		if (bShowZAxis&&bShowZView&&(bShowZ)) {
			ZFrontLine->Draw(PDI, View);
			ZBackLine->Draw(PDI, View);
			if (ZBackAxis.TheWorld && ZBackAxis.Visible)DrawMeasureLine(ZBackAxis.TheWorld,ZBackAxis.TraceStart,ZBackAxis.TraceEnd,FColor::Blue);
			if (ZFrontAxis.TheWorld && ZFrontAxis.Visible)DrawMeasureLine(ZFrontAxis.TheWorld,ZFrontAxis.TraceStart,ZFrontAxis.TraceEnd,FColor::Blue);

		}		

	}
	bInView = View->ViewFrustum.IntersectBox(mCurrentPosition,FVector::ZeroVector);
	
	
}

void "FArmyAxisRuler3D::Draw (const FSceneView* InView,FCanvas* SceneCanvas)
{	
	
}

void "FArmyAxisRuler3D::Update(UArmyEditorViewportClient* InViwportClient,const FSceneView* InView,const TArray<FObjectWeakPtr>& InObjects,FVector Position,FVector InNormal,FCanvas* SceneCanvas,FVector InWallDiection)
{

	mCurrentPosition = Position;
	bool bOnWall = IsOnWall(Position,InNormal);
	bool bOnFloor = IsOnFloorOrTop(Position,InNormal);

	if (bInView)
	{
		if (mCurrentActor && mCurrentActor->IsValidLowLevel())
		{
			AStaticMeshActor* StaticActor = Cast<AStaticMeshActor>(mCurrentActor);
			bInView = mCurrentActor->WasRecentlyRendered();
		}
	}

	
	if (!bInView)
	{
		XFrontInputBox->SetVisibility(EVisibility::Collapsed);
		XBackInputBox->SetVisibility(EVisibility::Collapsed);
		YFrontInputBox->SetVisibility(EVisibility::Collapsed);
		YBackInputBox->SetVisibility(EVisibility::Collapsed);
		ZFrontInputBox->SetVisibility(EVisibility::Collapsed);
		ZBackInputBox->SetVisibility(EVisibility::Collapsed);
		return;
	}
	
	UpdateInfo(InViwportClient,InView,InObjects,Position,InNormal,SceneCanvas,InWallDiection);
	//GVC->SetViewportType(EArmyLevelViewportType::LVT_OrthoXY);
	if ((bOnWall || bOnFloor))
	{
		Axis3DCollision(InViwportClient);
		Update2D(InViwportClient, InView, InObjects, Position, InNormal, SceneCanvas);
	}
	else
		Update3D(InViwportClient,InView,InObjects,Position,InNormal,SceneCanvas);
}

void "FArmyAxisRuler3D::UpdateInfo(UArmyEditorViewportClient* InViwportClient,const FSceneView* InView,const TArray<FObjectWeakPtr>& InObjects,FVector Position,FVector InNormal,FCanvas* SceneCanvas, FVector InWallDiection)
{
	{
		TArray<TWeakPtr<"FArmyObject>> OutObjects;
		"FArmySceneData::Get ()->GetObjects (E_LayoutModel,OT_InternalRoom,OutObjects);
		for (int32 i = 0;i<OutObjects.Num ();i++)
		{
			FObjectPtr object = OutObjects[i].Pin ();
			if (object->GetType ()!=OT_InternalRoom)
				continue;
			TSharedPtr<"FArmyRoom> Room = StaticCastSharedPtr<"FArmyRoom> (object);
			bInRoom = Room->IsPointInRoom(Position);
			if (bInRoom)break;
		}


		EArmyLevelViewportType::Type ViewportType = InViwportClient->GetViewportType();

		bool bOnWall = IsOnWall(Position,InNormal);
		bool bOnFloor = IsOnFloorOrTop(Position,InNormal);


		UpVector = FVector::UpVector;
		RightVector = FVector::RightVector;
		ForwardVector = FVector::ForwardVector;
		if (bOnWall)
		{
			RightVector = InNormal;
			ForwardVector = InWallDiection;
			//FVector::CrossProduct(RightVector, UpVector);
		}

		float Hight = Position.Z;
		float Length = 100000.f;
		XFrontStart = Position;
		XBackStart = Position;

		YFrontStart = Position;
		YBackStart = Position;

		ZFrontStart = Position;
		ZBackStart = Position;

		bActor = mCurrentActor&&mCurrentActor->IsValidLowLevel()&&mCurrentActor->IsA(AStaticMeshActor::StaticClass());
		if(bActor)
		{
			AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(mCurrentActor);
			UStaticMeshComponent* StaticMeshComp = StaticMeshActor->GetStaticMeshComponent();
			FBox Box = StaticMeshComp->GetBodySetup()->AggGeom.CalcAABB(FTransform(FVector(0,0,0)));
			FVector MeshPos,Extent;
			Box.GetCenterAndExtents(MeshPos,Extent);
			float Y = Extent.Y;
			float X = Extent.X;
			float Z = Extent.Z;

			FVector ForwardV (X,0,Z);
			FVector BackV(-X,0,Z);

			FVector UpV(0,0,2*Z);
			FVector BottmV(0,0,0);

			FVector LeftV(0,0,Z);
			FVector RightV(0,Y+MeshPos.Y,Z);

			FVector V[6];
			ConversionLocation(StaticMeshComp,LeftV,V[0]);
			ConversionLocation(StaticMeshComp,RightV,V[1]);
			ConversionLocation(StaticMeshComp,UpV,V[2]);
			ConversionLocation(StaticMeshComp,BottmV,V[3]);
			ConversionLocation(StaticMeshComp,ForwardV,V[4]);
			ConversionLocation(StaticMeshComp,BackV,V[5]);

			RightVector = (V[1]-V[0]).GetSafeNormal();
			ForwardVector = (V[4]-V[5]).GetSafeNormal();

			/*XFrontStart = V[4];
			XBackStart = V[5];

			YFrontStart = V[0];
			YBackStart = V[1];

			ZFrontStart = V[2];
			ZBackStart = V[3];*/

			
		}

		bShowZ = true;
		bShowY = true;
		bShowX = true;

		if ( !bOnFloor && !bOnWall)
		{
		}
		else if ("FArmyMath::AreLinesParallel(FVector::UpVector,InNormal))
		{
     		bShowZ = false;
		}
		else
		{
			bShowY = false;
		}



		if (bOnWall)
		{
			//将坐标向外轻微移动，避免标尺在墙面上，能够防止坐标轴闪烁以及显示异常问题
			FVector offset = RightVector.GetSafeNormal()*0.05;
			XFrontStart += offset;
			XBackStart+= offset;

		}

		bShowZView = ViewportType != EArmyLevelViewportType::LVT_OrthoXY;
		XFrontLine->SetStart(XFrontStart);
		XFrontLine->SetEnd(XFrontStart+ForwardVector * Length);
		XBackLine->SetStart(XBackStart);
		XBackLine->SetEnd(XBackStart-ForwardVector * Length);
		YFrontLine->SetStart(YFrontStart);
		YFrontLine->SetEnd(YFrontStart+RightVector * Length);
		YBackLine->SetStart(YBackStart);
		YBackLine->SetEnd(YBackStart-RightVector * Length);

		ZFrontLine->SetStart (ZFrontStart);
		ZFrontLine->SetEnd (FVector (ZFrontStart.X,ZFrontStart.Y,"FArmySceneData::WallHeight));

		ZBackLine->SetStart (ZBackStart);
		ZBackLine->SetEnd (ZBackStart-FVector::UpVector* FMath::Abs(Hight));
	}
}
void "FArmyAxisRuler3D::Axis3DCollision(UArmyEditorViewportClient* InViwportClient)
{
	FHitResult Hit(ForceInit);
	if (OnCollisionLocation(InViwportClient->GetWorld(), XFrontLine->GetStart(), XFrontLine->GetEnd(), Hit))
	{
		XFrontLine->SetEnd(Hit.ImpactPoint);
	}

	Hit.Init();
	if (OnCollisionLocation(InViwportClient->GetWorld(), XBackLine->GetStart(), XBackLine->GetEnd(), Hit))
	{
		XBackLine->SetEnd(Hit.ImpactPoint);
	}

	Hit.Init();
	if (OnCollisionLocation(InViwportClient->GetWorld(), YFrontLine->GetStart(), YFrontLine->GetEnd(), Hit))
	{
		YFrontLine->SetEnd(Hit.ImpactPoint);
	}

	Hit.Init();
	if (OnCollisionLocation(InViwportClient->GetWorld(), YBackLine->GetStart(), YBackLine->GetEnd(), Hit))
	{
		YBackLine->SetEnd(Hit.ImpactPoint);
	}

	Hit.Init();
	if (OnCollisionLocation(InViwportClient->GetWorld(), ZFrontLine->GetStart(), ZFrontLine->GetEnd(), Hit))
	{
		ZFrontLine->SetEnd(Hit.ImpactPoint);
	}

	Hit.Init();
	if (OnCollisionLocation(InViwportClient->GetWorld(), ZBackLine->GetStart(), ZBackLine->GetEnd(), Hit))
	{
		ZBackLine->SetEnd(Hit.ImpactPoint);
	}
}
void "FArmyAxisRuler3D::Update3D(UArmyEditorViewportClient* InViwportClient,const FSceneView* InView,const TArray<FObjectWeakPtr>& InObjects,FVector Position,FVector InNormal,FCanvas* SceneCanvas)
{
	FVector2D ViewportSize;
	InViwportClient->GetViewportSize (ViewportSize);

	if (Position.ContainsNaN())
		return;


	float Hight = mCurrentPosition.Z;
	float Length = 10000000.f;

	Axis3DCollision(InViwportClient);
	
	FText LenText;
	FVector Center = Position;
	int32 X = 0,Y = 0;
	FVector2D PixPos1,PixPos2,PixPos3,PixPos4,PixPos5,PixPos6;
	InView->WorldToPixel((ZFrontStart+UpVector*50),PixPos5);
	InView->WorldToPixel((ZBackStart-UpVector*50),PixPos6);
	InView->WorldToPixel(XFrontStart+ForwardVector*50,PixPos1);
	InView->WorldToPixel((XBackStart-ForwardVector*50),PixPos2);
	InView->WorldToPixel((YFrontStart+RightVector*50),PixPos3);
	InView->WorldToPixel((YBackStart-RightVector*50),PixPos4);
	PixPos1 -= ViewportSize/2;
	PixPos2 -= ViewportSize/2;
	PixPos3 -= ViewportSize/2;
	PixPos4 -= ViewportSize/2;
	PixPos5 -= ViewportSize/2;
	PixPos6 -= ViewportSize/2;


	/**@刘克祥控制各轴标尺长度的显示*/
	float TempV = 0.f;
	if (bShowXAxis)
	{

		XFrontInputBox->SetRenderTransform(FSlateRenderTransform(PixPos1));
		int32 TempV =FMath::RoundToInt(   XFrontLine->Size()*10);
		bShowX &= TempV!=0;
		if (TempV!=XFrontV)
		{
			XFrontV = FMath::IsNearlyEqual(TempV,Length) ? 0.f : TempV;
			LenText = FText::FromString (FString::Printf (TEXT("%d mm"),XFrontV));
			XFrontInputBox->SetInputText(LenText);
		}

		TempV = FMath::RoundToInt((XFrontLine->GetEnd() - XBackLine->GetEnd()).Size2D() * 10) - TempV;
		//TempV = FMath::RoundToInt(XBackLine->Size()*10);
		bShowX &= TempV!=0;
		if (TempV!=XBackV)
		{
			XBackV = FMath::IsNearlyEqual(TempV,Length) ? 0.f : TempV;
			LenText = FText::FromString (FString::Printf (TEXT ("%d mm"),XBackV));
			XBackInputBox->SetInputText(LenText);
		}
		XBackInputBox->SetRenderTransform(FSlateRenderTransform(PixPos2));


	}

	//if(bShowYAxis)
	{
		int32 TempV = FMath::RoundToInt(YFrontLine->Size()*10);
		bShowY &= TempV!=0;
		if (TempV!=YFrontV)
		{
			YFrontV = FMath::IsNearlyEqual(TempV,Length) ? 0.f : TempV;
			LenText = FText::FromString (FString::Printf (TEXT ("%d mm"),YFrontV));
			YFrontInputBox->SetInputText(LenText);
		}
		YFrontInputBox->SetRenderTransform(FSlateRenderTransform(PixPos3));

		TempV = FMath::RoundToInt((YFrontLine->GetEnd() - YBackLine->GetEnd()).Size2D() * 10) - TempV;
		//TempV = FMath::RoundToInt(YBackLine->Size()*10);
		bShowY &= TempV!=0;
		if (TempV!=YBackV)
		{
			YBackV = FMath::IsNearlyEqual(TempV,Length) ? 0.f : TempV;
			LenText = FText::FromString (FString::Printf (TEXT ("%d mm"),YBackV));
			YBackInputBox->SetInputText(LenText);
		}
		YBackInputBox->SetRenderTransform(FSlateRenderTransform(PixPos4));


	}
	//if(bShowZAxis)
	{

		FQuat2D quat2D(PI/2);
		FMatrix2x2 matrix(quat2D);
		FTransform2D trans(matrix,PixPos5);

		int32 TempV = FMath::RoundToInt(ZFrontLine->Size()*10);
		bShowZ &= TempV!=0;
		if (TempV!=ZFrontV)
		{
			ZFrontV = TempV;
			LenText = FText::FromString (FString::Printf (TEXT ("%d mm"),ZFrontV));
			ZFrontInputBox->SetInputText(LenText);
		}

		ZFrontInputBox->SetRenderTransformPivot(FVector2D(0.5,0.5));
		ZFrontInputBox->SetRenderTransform(trans);

		TempV = FMath::RoundToInt((ZFrontLine->GetEnd() - ZBackLine->GetEnd()).Size2D()*10)- TempV;
		bShowZ &= TempV!=0;
		if (TempV!=ZBackV)
		{
			ZBackV = TempV;
			LenText = FText::FromString (FString::Printf (TEXT ("%d mm"),ZBackV));
			ZBackInputBox->SetInputText(LenText);
		}


		//trans(matrix,PixPos6);
		FTransform2D trans1(matrix,PixPos6);
		ZBackInputBox->SetRenderTransformPivot(FVector2D(0.5,0.5));
		ZBackInputBox->SetRenderTransform(trans1);

	}


	XShow = bShowXAxis&&bInRoom ? EVisibility::Visible : EVisibility::Collapsed;
	XFrontInputBox->SetVisibility(XShow);
	XBackInputBox->SetVisibility(XShow);

	YShow = bShowYAxis&&bInRoom ? EVisibility::Visible : EVisibility::Collapsed;
	YFrontInputBox->SetVisibility(YShow);
	YBackInputBox->SetVisibility(YShow);

	ZShow = bShowZAxis&&bInRoom ? EVisibility::Visible : EVisibility::Collapsed;
	ZFrontInputBox->SetVisibility(ZShow);
	ZBackInputBox->SetVisibility(ZShow);
}

void "FArmyAxisRuler3D::Update2D(UArmyEditorViewportClient* InViwportClient,const FSceneView* InView,const TArray<FObjectWeakPtr>& InObjects,FVector Position,FVector InNormal,FCanvas* SceneCanvas)
{
	FVector2D ViewportSize;
	InViwportClient->GetViewportSize (ViewportSize);

	if (Position.ContainsNaN())
		return;

	float Hight = mCurrentPosition.Z;
	float Length = 10000000.f;

	bInRoom = false;
	FVector Position2D = FVector (Position.X,Position.Y,0);
	for (int32 i = 0;i<InObjects.Num ();i++)
	{
		FObjectPtr object = InObjects[i].Pin ();
		TSharedPtr<"FArmyRoomSpaceArea> room = StaticCastSharedPtr<"FArmyRoomSpaceArea>(object);
		{

			FVector xDir = room->GetXDir();
			FVector yDir = room->GetYDir();

			TArray< TSharedPtr<"FArmyLine> > Lines;
			object->GetLines(Lines,false);
			bInRoom = true;
			for (auto LineIt:Lines)
			{
				FVector StartWorldPos = LineIt->GetStart().X * xDir+LineIt->GetStart().Y * yDir+room->GetPlaneCenter();
				FVector EndWorldPos = LineIt->GetEnd().X * xDir+LineIt->GetEnd().Y * yDir+room->GetPlaneCenter();

				/*	{
						FVector2D SP2D,EP2D;
						InViwportClient->WorldToPixel(StartWorldPos,SP2D);
						InViwportClient->WorldToPixel(EndWorldPos,EP2D);
						SceneCanvas->DrawNGon(SP2D,FColor::Blue,5,5);
						SceneCanvas->DrawNGon(EP2D,FColor::Blue,5,5);
					}*/


				FVector InterPoint;
				if ("FArmyMath::Line3DIntersectionDirection(StartWorldPos,EndWorldPos,XFrontLine->GetStart(),ForwardVector,InterPoint))
				{
					float PreLenght = XFrontLine->Size();
					float TempLenght = (InterPoint - XFrontLine->GetStart()).Size();
					if (PreLenght == 0)
						XFrontLine->SetEnd(InterPoint);
					else
					{
						float CurrentSize = XFrontLine->Size();
						if (TempLenght <= CurrentSize)
						{
							XFrontLine->SetEnd(InterPoint);
							/*if (!FMath::IsNearlyEqual(TempLenght,CurrentSize,0.001f))
							{
								XFrontLine->SetEnd(InterPoint);
							}*/
						}
						
					}
				}
				if ("FArmyMath::Line3DIntersectionDirection(StartWorldPos,EndWorldPos,XBackLine->GetStart(),-ForwardVector,InterPoint))
				{
					float PreLenght = XBackLine->Size();
					float TempLenght = (InterPoint-XBackLine->GetStart()).Size();
					if (PreLenght==0)
						XBackLine->SetEnd(InterPoint);
					else
					{
						float CurrentSize = XBackLine->Size();
						if (TempLenght<CurrentSize)
						{
							XBackLine->SetEnd(InterPoint);
							/*if (!FMath::IsNearlyEqual(TempLenght,CurrentSize,0.001f))
							{
								XBackLine->SetEnd(InterPoint);
							}*/
						}

					}
				}
				


				if ("FArmyMath::Line3DIntersectionDirection(StartWorldPos,EndWorldPos,YFrontLine->GetStart(),RightVector,InterPoint))
				{
					float PreLenght = YFrontLine->Size();
					float TempLenght = (InterPoint-YFrontLine->GetStart()).Size();
					if (PreLenght==0)
						YFrontLine->SetEnd(InterPoint);
					else
					{
						float CurrentSize = YFrontLine->Size();
						if (TempLenght<=CurrentSize)
						{
							YFrontLine->SetEnd(InterPoint);
							/*if (!FMath::IsNearlyEqual(TempLenght,CurrentSize,0.001f))
							{
								YFrontLine->SetEnd(InterPoint);
							}*/
						}

					}
				}
				if ("FArmyMath::Line3DIntersectionDirection(StartWorldPos,EndWorldPos,YBackLine->GetStart(),-RightVector,InterPoint))
				{
					float PreLenght = YBackLine->Size();
					float TempLenght = (InterPoint-YBackLine->GetStart()).Size();
					if (PreLenght==0)
						YBackLine->SetEnd(InterPoint);
					else
					{
						float CurrentSize = YBackLine->Size();
						if (TempLenght<=CurrentSize)
						{
							YBackLine->SetEnd(InterPoint);
							/*if (!FMath::IsNearlyEqual(TempLenght,CurrentSize,0.001f))
							{
								YBackLine->SetEnd(InterPoint);
							}*/
						}

					}
				}

				if ("FArmyMath::Line3DIntersectionDirection(StartWorldPos,EndWorldPos,ZFrontLine->GetStart(),UpVector,InterPoint))
				{
					float PreLenght = ZFrontLine->Size();
					float TempLenght = (InterPoint-ZFrontLine->GetStart()).Size();
					if (PreLenght==0)
						ZFrontLine->SetEnd(InterPoint);
					else
					{
						float CurrentSize = ZFrontLine->Size();
						if (TempLenght<=CurrentSize)
						{
							ZFrontLine->SetEnd(InterPoint);
						}

					}
				}
				if ("FArmyMath::Line3DIntersectionDirection(StartWorldPos,EndWorldPos,ZBackLine->GetStart(),-UpVector,InterPoint))
				{
					float PreLenght = ZBackLine->Size();
					float TempLenght = (InterPoint-ZBackLine->GetStart()).Size();
					if (PreLenght==0)
						ZBackLine->SetEnd(InterPoint);
					else
					{
						float CurrentSize = ZBackLine->Size();
						if (TempLenght<=CurrentSize)
						{
							ZBackLine->SetEnd(InterPoint);
							/*if (!FMath::IsNearlyEqual(TempLenght,CurrentSize,0.001f))
							{
								
							}*/
						}

					}
				}

			}



			FText LenText;
			FVector Center = Position;
			int32 X = 0,Y = 0;
			FVector2D PixPos1,PixPos2,PixPos3,PixPos4,PixPos5,PixPos6;
			InView->WorldToPixel((ZFrontStart+UpVector*50),PixPos5);
			InView->WorldToPixel((ZBackStart-UpVector*50),PixPos6);
			InView->WorldToPixel(XFrontStart+ForwardVector*50,PixPos1);
			InView->WorldToPixel((XBackStart-ForwardVector*50),PixPos2);
			InView->WorldToPixel((YFrontStart+RightVector*50),PixPos3);
			InView->WorldToPixel((YBackStart-RightVector*50),PixPos4);
			PixPos1 -= ViewportSize/2;
			PixPos2 -= ViewportSize/2;
			PixPos3 -= ViewportSize/2;
			PixPos4 -= ViewportSize/2;
			PixPos5 -= ViewportSize/2;
			PixPos6 -= ViewportSize/2;
			

			/**@刘克祥控制各轴标尺长度的显示*/
			float TempV = 0.f;
			if (bShowXAxis)
			{

				XFrontInputBox->SetRenderTransform(FSlateRenderTransform(PixPos1));
				int32 TempV = FMath::RoundToInt(XFrontLine->Size() * 10);
				
				//int32 TempV = XFrontLine->Size()*10;
				if (TempV >= Length*10.f)
					TempV = 0.f;
				bShowX &=  TempV!=0;
				if (TempV!=XFrontV)
				{
					XFrontV = FMath::IsNearlyEqual(TempV,Length) ? 0.f : TempV;
					LenText = FText::FromString (FString::Printf (TEXT("%d mm"),XFrontV));
					XFrontInputBox->SetInputText(LenText);
				}


				TempV = FMath::RoundToInt((XFrontLine->GetEnd() - XBackLine->GetEnd()).Size2D() * 10) - TempV;
				if (TempV>=Length*10.f)
					TempV = 0.f;
				bShowX &= TempV!=0;
				if (TempV!=XBackV)
				{
					XBackV = FMath::IsNearlyEqual(TempV,Length) ? 0.f : TempV;
					LenText = FText::FromString (FString::Printf (TEXT ("%d mm"),XBackV));
					XBackInputBox->SetInputText(LenText);
				}
				XBackInputBox->SetRenderTransform(FSlateRenderTransform(PixPos2));


			}

			//if(bShowYAxis)
			{
				int32 TempV = FMath::RoundToInt(YFrontLine->Size() * 10);
				
				if (TempV>=Length*10.f)
					TempV = 0.f;
				bShowY &= TempV!=0;
				if (TempV!=YFrontV)
				{
					YFrontV = FMath::IsNearlyEqual(TempV,Length) ? 0.f : TempV;
					LenText = FText::FromString (FString::Printf (TEXT ("%d mm"),YFrontV));
					YFrontInputBox->SetInputText(LenText);
				}
				YFrontInputBox->SetRenderTransform(FSlateRenderTransform(PixPos3));


				TempV = FMath::RoundToInt((YFrontLine->GetEnd() - YBackLine->GetEnd()).Size2D() * 10) - TempV;
				bShowY &= TempV!=0;
				if (TempV>=Length*10.f)
					TempV = 0.f;
				if (TempV!=YBackV)
				{
					YBackV = FMath::IsNearlyEqual(TempV,Length) ? 0.f : TempV;
					LenText = FText::FromString (FString::Printf (TEXT ("%d mm"),YBackV));
					YBackInputBox->SetInputText(LenText);
				}
				YBackInputBox->SetRenderTransform(FSlateRenderTransform(PixPos4));


			}
			//if(bShowZAxis)
			{

				FQuat2D quat2D(PI/2);
				FMatrix2x2 matrix(quat2D);
				FTransform2D trans(matrix,PixPos5);

				int32 TempV = FMath::RoundToInt(ZFrontLine->Size() * 10);
				bShowZ &= TempV!=0;
				if (TempV!=ZFrontV)
				{
					ZFrontV = TempV;
					LenText = FText::FromString (FString::Printf (TEXT ("%d mm"),ZFrontV));
					ZFrontInputBox->SetInputText(LenText);
				}

				ZFrontInputBox->SetRenderTransformPivot(FVector2D(0.5,0.5));
				ZFrontInputBox->SetRenderTransform(trans);

				TempV = FMath::RoundToInt(ZFrontLine->Size() + ZBackLine->Size()) * 10 - TempV;
				bShowZ &= TempV!=0;
				if (TempV!=ZBackV)
				{
					ZBackV = TempV;
					LenText = FText::FromString (FString::Printf (TEXT ("%d mm"),ZBackV));
					ZBackInputBox->SetInputText(LenText);
				}


				//trans(matrix,PixPos6);
				FTransform2D trans1(matrix,PixPos6);
				ZBackInputBox->SetRenderTransformPivot(FVector2D(0.5,0.5));
				ZBackInputBox->SetRenderTransform(trans1);

			}

		}
	}

	//UpdateTransform();
	bool bOnWall = IsOnWall(Position,InNormal);
	bool bOnFloor = IsOnFloorOrTop(Position,InNormal);

	XShow = bShowXView&&bShowXAxis&&bInRoom&&bShowX ? EVisibility::Visible : EVisibility::Collapsed;
	XFrontInputBox->SetVisibility(XShow);
	XBackInputBox->SetVisibility(XShow);

	YShow = bShowYView&&bShowYAxis&&bInRoom&& !bOnWall &&bShowY ? EVisibility::Visible : EVisibility::Collapsed;
	YFrontInputBox->SetVisibility(YShow);
	YBackInputBox->SetVisibility(YShow);

	ZShow = bShowZView&&bShowZAxis&&bInRoom && bOnWall&&bShowZ ? EVisibility::Visible : EVisibility::Collapsed;
	ZFrontInputBox->SetVisibility(ZShow);
	ZBackInputBox->SetVisibility(ZShow);
}

void "FArmyAxisRuler3D::WorldToPixel (UArmyEditorViewportClient* InViwportClient,const FVector& WorldPoint,FVector2D& OutPixelLocation)
{
	InViwportClient->WorldToPixel (WorldPoint,OutPixelLocation);
	FVector2D Size (1280,720);
	FVector2D ViewportSize;
	InViwportClient->GetViewportSize (ViewportSize);
	//OutPixelLocation -= ViewportSize / 2;
}

void "FArmyAxisRuler3D::GetLenght (TSharedPtr<"FArmyLine> Line,FText& LenText)
{
	int32 Lenght = Line->Size()*10;
	LenText = FText::FromString (FString::Printf (TEXT ("%d"),Lenght));
}

bool "FArmyAxisRuler3D::OnCollisionLocation(UWorld* InWorld,const FVector& InTraceStart,const FVector& InTraceEnd,FHitResult& OutHit)
{
	static FName TraceTag = FName(TEXT("Pick"));
	FCollisionQueryParams TraceParams(TraceTag,true);
	TraceParams.bTraceAsyncScene = true;
	for (TActorIterator<AActor> ActorItr(InWorld); ActorItr; ++ActorItr)
	{
		if (ActorItr->Tags.Num()>0&&ActorItr->Tags[0]=="MoveableMeshActor")
		{
			TraceParams.AddIgnoredActor(*ActorItr);
		}
	}
	return InWorld->LineTraceSingleByChannel(OutHit,InTraceStart,InTraceEnd,ECC_Visibility,TraceParams);
}

void "FArmyAxisRuler3D::SetAxisRulerDisplay(bool InShowXAxis, bool InShowYAxis, bool InShowZAxis)
{
	bShowXAxis = InShowXAxis;
	bShowYAxis = InShowYAxis;
	bShowZAxis = InShowZAxis;
}

bool "FArmyAxisRuler3D::IsOnWall(FVector& InMousePoint,FVector& InNormal)
{
	bool bOnWall = false;
	FVector MousePoint = InMousePoint;
	FVector InMousePoint2D = FVector(MousePoint.X,MousePoint.Y,0);
	TArray<TWeakPtr<"FArmyObject>> RoomList;
	"FArmySceneData::Get ()->GetObjects (E_LayoutModel,OT_InternalRoom,RoomList);
	"FArmySceneData::Get ()->GetObjects (E_LayoutModel, OT_IndependentWall,RoomList);
	"FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_PackPipe, RoomList);
	"FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Pillar, RoomList);
	"FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_AirFlue, RoomList);

	if (RoomList.Num ()==0)
		return bOnWall;
	for (int32 i = 0;i<RoomList.Num ();i++)
	{
		FObjectPtr object = RoomList[i].Pin ();
		//if (object->GetType ()!=OT_InternalRoom)
		//	continue;
		//TSharedPtr<"FArmyRoom> Room = StaticCastSharedPtr<"FArmyRoom>(object);
		TArray<TSharedPtr<"FArmyLine>> OutLines;
		object->GetLines(OutLines);
		if (OutLines.Num())
		{
			for (auto& Line : OutLines)
			{
				float Distance = "FArmyMath::Distance(InMousePoint2D,Line->GetStart(),Line->GetEnd());
				bOnWall = (Distance<=0.2f && Distance >= 0.f);
				if (bOnWall)
				{
					return bOnWall;
				}
					
			}
		}

	}
	return false;
}

bool "FArmyAxisRuler3D::IsOnFloor(FVector& InMousePoint,FVector& InNormal)
{
	FVector MousePoint = InMousePoint;
	bool bZero = FMath::IsNearlyZero(MousePoint.Z,0.001f);
	if ( bZero)
		return true;
	return false;
}

bool "FArmyAxisRuler3D::IsOnTop(FVector& InMousePoint,FVector& InNormal)
{
	FVector MousePoint = InMousePoint;
	bool bEquls = FMath::IsNearlyZero(MousePoint.Z-"FArmySceneData::WallHeight,0.001f);
	if (bEquls)
		return true;
	return false;
}

bool "FArmyAxisRuler3D::IsOnFloorOrTop(FVector& InMousePoint,FVector& InNormal)
{
	return IsOnTop(InMousePoint,InNormal)||IsOnFloor(InMousePoint,InNormal);
}

void "FArmyAxisRuler3D::SetActor(AActor* InActor)
{
	mCurrentActor = InActor; 
}

void "FArmyAxisRuler3D::Show(bool inEnable)
{
	XFrontInputBox->SetVisibility(inEnable?EVisibility::Visible:EVisibility::Collapsed);
	XFrontInputBox->SetVisibility(inEnable?EVisibility::Visible:EVisibility::Collapsed);
	XBackInputBox->SetVisibility(inEnable?EVisibility::Visible:EVisibility::Collapsed);
	YFrontInputBox->SetVisibility(inEnable?EVisibility::Visible:EVisibility::Collapsed);
	YBackInputBox->SetVisibility(inEnable?EVisibility::Visible:EVisibility::Collapsed);
	ZFrontInputBox->SetVisibility (inEnable?EVisibility::Visible:EVisibility::Collapsed);
	ZBackInputBox->SetVisibility (inEnable?EVisibility::Visible:EVisibility::Collapsed);

}


FBox "FArmyAxisRuler3D::GetBox()
{
	FBox box(ForceInit);
	if (mCurrentActor&&mCurrentActor->IsValidLowLevel())
	{
		if (mCurrentActor->IsA(AStaticMeshActor::StaticClass()))
		{
			AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(mCurrentActor);
			UStaticMeshComponent* StaticMeshComp = StaticMeshActor->GetStaticMeshComponent();
			box = StaticMeshComp->GetBodySetup()->AggGeom.CalcAABB(FTransform(FVector(0,0,0)));
		}
	}
	return box;
}

void "FArmyAxisRuler3D::ConversionLocation(class UStaticMeshComponent* InComponent,FVector& InRelativeLocation,FVector& OutLocation)
{
	AActor* InActor = InComponent->GetOwner();
	FTransform transForm = InComponent->GetRelativeTransform();
	TArray<USceneComponent*> Parents;
	InComponent->GetParentComponents(Parents);
	bool bRoot = Parents.Num()==0;
	const FMatrix CompLS = transForm.ToMatrixNoScale();
	FRotator DefRotator(0,0,0);
	//本地空间到世界空间
	const FMatrix LocalToWorld = InActor->ActorToWorld().ToMatrixWithScale();
	//构造附着点相对于actor的本地空间矩阵 
	const FMatrix DefMeshLS = FRotationTranslationMatrix(DefRotator,InRelativeLocation);
	FMatrix NowLocationLS = bRoot ? DefMeshLS * CompLS : DefMeshLS * CompLS * LocalToWorld;

	OutLocation = NowLocationLS.GetOrigin();
}


void "FArmyAxisRuler3D::SetRefCoordinateSystem(const FVector& InBasePos,const FVector& InXDir,const FVector& InYDir,const FVector& InNormal)
{
	BasePoint = InBasePos;
	BaseXDir = InXDir;
	BaseYDir = InYDir;
	BaseNormal = InNormal;
	BasePlane = FPlane(BasePoint,BaseNormal);
}
const FPlane& "FArmyAxisRuler3D::GetPlane() const
{
	return BasePlane;
}

void "FArmyAxisRuler3D::DrawMeasureLine(UWorld* InWorld,FVector InStart,FVector InEnd,FColor InColor)
{
	DrawDebugLine(InWorld,InStart,InEnd,InColor,false,-1.f,1.f,0.5f);
}

#undef LOCTEXT_NAMESPACE 