#include "ArmyPipelineOperation.h"
#include "ArmyEditPoint.h"
#include "Style/XRStyle.h"
#include "Math/XRMath.h"
#include "SArmyInputBox.h"
#include "ArmyShapeBoardActor.h"
#include "SceneManagement.h"
#include "Engine.h"
#include "CoreMinimal.h"
#include "Engine/World.h"
#include "ArmyCommonTypes.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyEditorEngine.h"
#include "ArmyMouseCaptureManager.h"
#include "ArmyPipelineActor.h"
#include "ArmyPipePointActor.h"
#include "ArmyPipeLine.h"
#include "SceneManagement.h"
#include "Army3DManager.h"
#include "ArmyRoom.h"
#include "ArmyPlayerController.h"
#include "ArmyGroupActor.h"
#include "ArmyEngineModule.h"
#include "ArmySceneData.h"
#include "ArmyGameInstance.h"
#include "ArmyPipeGraph.h"
#include "ArmyRulerLine.h"
#include "ArmyAuxiliary.h"
#include "ArmyHydropowerDataManager.h"
#include "ArmyHydropowerModeController.h"
using namespace FContentItemSpace;



bool FIntersectionStateInfo::IsIntersection()
{
	TWeakPtr<FArmyObject> Line1Obj1 = FArmySceneData::Get()->GetObjectByGuid(E_HydropowerModel,Intersection->Line1);
	TWeakPtr<FArmyObject> Line1Obj2 = FArmySceneData::Get()->GetObjectByGuid(E_HydropowerModel,Intersection->Line2);
	if (Line1Obj1.IsValid()&&Line1Obj2.IsValid())
	{
		TSharedPtr<FArmyPipeline> Line1 = StaticCastSharedPtr<FArmyPipeline>(Line1Obj1.Pin());
		TSharedPtr<FArmyPipeline> Line2 = StaticCastSharedPtr<FArmyPipeline>(Line1Obj2.Pin());
		return Line1->IsIntersect(Line2);
	}
	return false;
}

bool FIntersectionStateInfo::IsWaterPipeLineIntersection()
{
	bool bWater = false;
	if (IsWaterLine(Intersection->Line1)&&IsWaterLine(Intersection->Line2))
	{
		bWater = true;
	}
	return bWater;
}

bool FIntersectionStateInfo::IsWaterElecIntersection()
{
	bool bLineW1 = IsWaterLine(Intersection->Line1);
	bool bLineW2 = IsWaterLine(Intersection->Line2);
	return (bLineW1 != bLineW2);
}

bool FIntersectionStateInfo::IsElecIntersection()
{
	bool bLineW1 = IsWaterLine(Intersection->Line1);
	bool bLineW2 = IsWaterLine(Intersection->Line2);
	return (!bLineW1 && bLineW1==bLineW2);
}

bool FIntersectionStateInfo::IsLineBridge(FGuid& Line)
{
	if (!bState)
	{
		if (IsWaterPipeLineIntersection())
		{
			FGuid Line2 = Intersection->Line1==Line ? Intersection->Line2 : Intersection->Line1;
			TSharedPtr<FArmyPipeline> PipeLine1 = GetLinePtr(Line);
			TSharedPtr<FArmyPipeline> PipeLine2 = GetLinePtr(Line2);
			if ((PT_Drain == PipeLine1->PipeType ) &&(PipeLine1->PipeType == PipeLine2->PipeType ))
			{
				return PipeLine1->GetRadius() <= PipeLine2->GetRadius();
			}
			return PipeLine1->PipeType<=PipeLine2->PipeType;
		}
	}
	
	return false;
}

TSharedPtr<FArmyPipeline> FIntersectionStateInfo::GetLinePtr(FGuid& InLineID)
{
	TWeakPtr<FArmyObject> LineObj = FArmySceneData::Get()->GetObjectByGuid(E_HydropowerModel,InLineID);
	if (LineObj.IsValid())
	{
		TSharedPtr<FArmyPipeline> PipeLine1 = StaticCastSharedPtr<FArmyPipeline>(LineObj.Pin());
		return PipeLine1;
	}
	return nullptr;
}

TSharedPtr<FArmyPipeline> FIntersectionStateInfo::GetOtherLinePtr(FGuid& InLineID)
{
	FGuid LineID = GetOtherLineID(InLineID);
	TWeakPtr<FArmyObject> LineObj = FArmySceneData::Get()->GetObjectByGuid(E_HydropowerModel,LineID);
	if (LineObj.IsValid())
	{
		TSharedPtr<FArmyPipeline> PipeLine1 = StaticCastSharedPtr<FArmyPipeline>(LineObj.Pin());
		return PipeLine1;
	}
	return nullptr;
}


FGuid FIntersectionStateInfo::GetOtherLineID(FGuid& InLineID)
{
	FGuid LineID = Intersection->Line1==InLineID ? Intersection->Line2 : Intersection->Line1;
	return LineID;
}

bool FIntersectionStateInfo::IsWaterLine(FGuid& InLineID)
{
	TWeakPtr<FArmyObject> LineObj = FArmySceneData::Get()->GetObjectByGuid(E_HydropowerModel,InLineID);

	bool bWater = false;
	if (LineObj.IsValid())
	{
		if (LineObj.Pin()->GetType()==OT_ColdWaterTube||
			LineObj.Pin()->GetType()==OT_HotWaterTube||
			LineObj.Pin()->GetType()==OT_Drain)
		{
			bWater = true;
		}
	}
	return bWater;
}

void FLineIntersectionInfo::AddIntersertionInfo(TSharedPtr<FIntersectionStateInfo>& Info)
{
	TWeakPtr<FArmyObject> LineObj = FArmySceneData::Get()->GetObjectByGuid(E_HydropowerModel,LineID);
	if (LineObj.IsValid())
	{
		TSharedPtr<FArmyPipeline> Line = StaticCastSharedPtr<FArmyPipeline>(LineObj.Pin());
		FVector Start = Line->GetStart();
		Intersectiones.AddUnique(Info);
		Intersectiones.Sort([&](const TSharedPtr<FIntersectionStateInfo>& A,const TSharedPtr<FIntersectionStateInfo>& B){
			FVector DirectA = A->Intersection->Location -Start;
			FVector DirectB = B->Intersection->Location -Start;
			return DirectA.Size() > DirectB.Size();
		});
	}
	
}


bool FLineIntersectionInfo::IsWaterLine()
{
	TWeakPtr<FArmyObject> LineObj = FArmySceneData::Get()->GetObjectByGuid(E_HydropowerModel,LineID);
	if (LineObj.IsValid())
	{
		switch (LineObj.Pin()->GetType())
		{
		case OT_ColdWaterTube:
		case OT_HotWaterTube:
		case OT_Drain:
			return true;
		default:
			return false;
			break;
		}
	}
	return false;
}

void FLineIntersectionInfo::GetNearIntersectionstateInfoes(TSharedPtr<FIntersectionStateInfo>& PointInfo,TArray<TSharedPtr<FIntersectionStateInfo>>& OutArray)
{
	int32 Index = Intersectiones.IndexOfByPredicate([&](const TSharedPtr<FIntersectionStateInfo>& Item)
	{
		 return PointInfo->Intersection->Location.Equals(Item->Intersection->Location);
	});
	if (Index != INDEX_NONE)
	{
		for (int32 i = Index ; i<Intersectiones.Num()-1;i++)
		{
			TSharedPtr<FIntersectionStateInfo>& Info = Intersectiones[i];
			TSharedPtr<FIntersectionStateInfo>& InfoNext = Intersectiones[i+1];
			FVector Direction = InfoNext->Intersection->Location - Info->Intersection->Location;
			if (Direction.Size()<=50.f)
			{
				if (!Info->GetState())
					OutArray.AddUnique(Info);
				if (!InfoNext->GetState())
					OutArray.AddUnique(InfoNext);
			}
			else
			{
				break;
			}
		}
		for (int32 i = Index; i>0;i--)
		{
			TSharedPtr<FIntersectionStateInfo>& Info = Intersectiones[i];
			TSharedPtr<FIntersectionStateInfo>& InfoPre = Intersectiones[i-1];
			FVector Direction = InfoPre->Intersection->Location-Info->Intersection->Location;
			if (Direction.Size()<=50.f)
			{
				if (!Info->GetState())
					OutArray.AddUnique(Info);
				if (!InfoPre->GetState())
					OutArray.AddUnique(InfoPre);
			}
			else
			{
				break;
			}
		}
		//if (Intersectiones.Num() == 1)
		{
			OutArray.AddUnique(Intersectiones[Index]);
		}
	}
}

FArmyPipelineOperation::FArmyPipelineOperation (EModelType InBelongModel)
	:FArmyOperation(InBelongModel)
	,UpLeftDashLine (MakeShareable (new FArmyLine ()))
	,UpRightDashLine (MakeShareable (new FArmyLine ()))
	,UpDashLine (MakeShareable (new FArmyLine ()))
	,Start(FVector::ZeroVector)
	,End(FVector::ZeroVector)
	,MousePosition(FVector::ZeroVector)
	,bShowCash(false)
	,bPreIsInKitnes(-1)
{
	
	//UpDashLine->bIsDashLine = false;
	//UpDashLine->SetBaseColor( FLinearColor::Gray );
	//
	//UpRightDashLine ->bIsDashLine = false;
	//UpRightDashLine->SetBaseColor(FLinearColor::Gray );
	//
	//UpLeftDashLine->bIsDashLine = false;
	//UpLeftDashLine->SetBaseColor (FLinearColor::Gray);

	MaterialInsDy = UMaterialInstanceDynamic::Create (FArmyEngineModule::Get ().GetEngineResource ()->GetLineMaterialIns (),nullptr);
	if(MaterialInsDy->IsValidLowLevel ())
	{
		MaterialInsDy->AddToRoot ();
	}
	

}

FArmyPipelineOperation::FArmyPipelineOperation (EModelType InBelongModel,class UWorld* World)
	:FArmyOperation (InBelongModel)
	,UpLeftDashLine (MakeShareable (new FArmyLine ()))
	,UpRightDashLine (MakeShareable (new FArmyLine ()))
	,UpDashLine (MakeShareable (new FArmyLine ()))
	,Start (FVector::ZeroVector)
	,End (FVector::ZeroVector)
	,MousePosition (FVector::ZeroVector)
	,bShowCash (false)
	,bPreIsInKitnes(-1)
{
	OwningWorld = World;
	UpDashLine->bIsDashLine = false;
	UpDashLine->SetBaseColor (FLinearColor::Gray);

	UpRightDashLine->bIsDashLine = false;
	UpRightDashLine->SetBaseColor (FLinearColor::Gray);

	UpLeftDashLine->bIsDashLine = false;
	UpLeftDashLine->SetBaseColor (FLinearColor::Gray);
	MaterialInsDy = UMaterialInstanceDynamic::Create (FArmyEngineModule::Get().GetEngineResource()->GetLineMaterialIns(),nullptr);
	if(MaterialInsDy->IsValidLowLevel ())
	{
		MaterialInsDy->AddToRoot ();
	}
	/*TSharedPtr<FArmyPipeGraph> NewGraph=MakeShareable(new FArmyPipeGraph());
	FArmySceneData::Get()->Add(NewGraph,XRArgument(1).ArgUint32(E_HydropowerModel));
	PipeGraph=NewGraph;*/
}

FArmyPipelineOperation::~FArmyPipelineOperation()
{
	 FArmySceneData::Get()->PostOperationDelegate.RemoveAll(this);
	if(MaterialInsDy->IsValidLowLevel())
	{
		MaterialInsDy->RemoveFromRoot();
	}
}

void FArmyPipelineOperation::Init()
{
	RulerLine = MakeShareable(new FArmyRulerLine());
	RulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this,&FArmyPipelineOperation::OnInputBoxCommitted));
	LocalModelService=MakeShareable(new FArmyResourceService);
	LocalModelService->OnUpdateProductInfoDelegate.BindRaw(this,&FArmyPipelineOperation::UpdateProductInfoAfter);
	FArmySceneData::Get()->PostOperationDelegate.AddRaw(this, &FArmyPipelineOperation::OnObjectOperation);
	GXREditor->OnActorMoved().AddRaw(this,&FArmyPipelineOperation::OnActorMoved);

	Auxiliary = MakeShareable(new FArmyAuxiliary);
}

void FArmyPipelineOperation::InitWidget (TSharedPtr<SOverlay> InParentWidget /* = NULL */)
{
	if (Auxiliary.IsValid())
		Auxiliary->Init(InParentWidget);
}

void FArmyPipelineOperation::Draw (FPrimitiveDrawInterface* PDI,const FSceneView* View)
{
	if (IsDrawing())
	{
		FVector Normal = FArmyMouseCaptureManager::Get()->GetCaptureNormal();
		if (Normal.ContainsNaN())
			Normal = FVector::ZeroVector;
		FVector nStart = Start - Normal* Radius;
		FVector nEnd = End - Normal*Radius;
		//DrawCylinder(PDI,nStart,nEnd,Radius,128,MaterialInsDy->GetRenderProxy (false),SDPG_Foreground);
		PDI->DrawLine(Start,End,Color,SDPG_Foreground,Radius * 5,0.0f,true);
		PDI->DrawPoint(nStart,FLinearColor::White,10,SDPG_MAX);
		PDI->DrawPoint(End,FLinearColor::Red,10,SDPG_MAX);
		RulerLine->Draw(PDI,View);
		if (Auxiliary.IsValid())
			Auxiliary->Draw(PDI,View);
		
	}
}

void FArmyPipelineOperation::BeginOperation(XRArgument InArg /*= XRArgument ()*/)
{
	CurrentArg = InArg;
	//InputBoxWidget->SetEnabled(true);
	TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = ContentItem->GetResObjNoComponent();
	if (resArr.Num() < 1)
		return;
	TSharedPtr<FArmyPipeRes> Res = StaticCastSharedPtr<FArmyPipeRes> (resArr[0]);
	bPreIsInKitnes = bPreIsInKitnes == 1? 0 : bPreIsInKitnes;
	Radius = Res->Raduis / 20.f;
	Color = Res->Color;
	MaterialInsDy->SetVectorParameterValue(TEXT("MainColor"),FLinearColor::FromSRGBColor (Color));
	FArmyMouseCaptureManager::Get()->SetEnable (true);
	GXREditor->SelectNone(true, true);
	GVC->CancelDrop();
	FArmyMouseCaptureManager::Get()->SetObjectType((EObjectType)Res->ObjectType);
	FArmyMouseCaptureManager::Get()->SetCaputureActor(false);
	//InputBoxWidget->SetFocus(true);
}

void FArmyPipelineOperation::EndOperation()
{

	RulerLine->ShowInputBox(false);
	bPreIsInKitnes = -1;
	Start = End = MousePosition = FVector::ZeroVector;
	CachedPoints.Empty();
	FArmyMouseCaptureManager::Get()->SetEnable(false);
	FArmyMouseCaptureManager::Get()->SetObjectType (OT_None);
	FArmyMouseCaptureManager::Get()->SetStartPoint(FVector::ZeroVector);
	FArmyMouseCaptureManager::Get()->SetCaputureActor(true);
	FArmyOperation::EndOperation();
}

void FArmyPipelineOperation::Tick()
{
	
	
}

void FArmyPipelineOperation::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
{
    if (Key == EKeys::LeftMouseButton && Event == IE_Released)
    {
		if (HitProxy&&HitProxy->Priority>=0&&HitProxy->OrthoPriority>=0 && HitProxy->IsA(HActor::StaticGetType()))
			GXREditor->SelectNone(true,true,false);
        CachedPoints.Add(MousePosition);
		TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = ContentItem->GetResObjNoComponent();
		if (resArr.Num() < 1)
			return;
        TSharedPtr<FArmyPipeRes> Res = StaticCastSharedPtr<FArmyPipeRes>(resArr[0]);
        if (CachedPoints.Num() > 1 && IsDrawing() && Res->ObjectType != OT_Drain_Point)
        {
            End = CachedPoints.Last();
            Exec_AddPipeline(Start, End);
        }
        else if (Res->ObjectType == OT_Drain_Point)
        {
            End = MousePosition + FVector::UpVector * FArmySceneData::WallHeight;
            Exec_AddPipeline(MousePosition, End); 
        }

		ForceUpdate();
    }
}

void FArmyPipelineOperation::MouseMove(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y)
{
	static int32 OldX=X,OldY=Y;
	if(OldX==X && OldY==Y)
	{
		return;
	}
	else
	{
		OldX=X;
		OldY=Y;
	}
	Update(InViewPortClient,ViewPort);
	if (GVC->Viewport->KeyState(EKeys::MiddleMouseButton))
		RulerLine->SetInputBoxFocus(false);
	else
		RulerLine->SetInputBoxFocus(true);
	//RulerLine->GetWidget()->SetForegroundColor()
}

bool FArmyPipelineOperation::InputKey(class UArmyEditorViewportClient* InViewPortClient,class FViewport* InViewPort,FKey Key,EInputEvent Event)
{
	return false;
}

void FArmyPipelineOperation::Exec_AddPipeline (const FVector& Start,const FVector& End)
{
	EXRViewMode CurrentViewMode = GXRPC->GetXRViewMode();
	
	TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = ContentItem->GetResObjNoComponent();
	if (resArr.Num() < 1)
		return;
	TSharedPtr<FArmyPipeRes> Res = StaticCastSharedPtr<FArmyPipeRes>(resArr[0]);
	EObjectType LineType = (EObjectType)Res->ObjectType;
	FVector InStart = Start;
	FVector InEnd = End;
	if (CurrentViewMode == EXRView_TOP&&Res->ObjectType != OT_Drain_Point)
	{
		InStart.Z = 0;//暂时这样，之后智能布线改下面方式
		InEnd.Z = 0;
		float Height = FMath::IsNearlyZero(InStart.Z)? FArmySceneData::WallHeight : InStart.Z;
		FVector HeightVector =  Height * FVector::UpVector ;
		bool bStartInKitchen=IsInKitchenes(Start);
		bool bEndInKitchen=IsInKitchenes(End);
		if(LineType==OT_StrongElectricity_25||
			LineType==OT_StrongElectricity_4||            //强电4
			LineType==OT_WeakElectricity_TV|| //弱电 电视线
			LineType==OT_WeakElectricity_Net|| //弱电， 网线
			LineType==OT_WeakElectricity_Phone//弱电，电话线
			)
		{
			if(bStartInKitchen)
				InEnd+=HeightVector;
			//开始
			if (bPreIsInKitnes == -1 )
			{
				if (bStartInKitchen)
					InStart+=HeightVector;
			}
			//前一个点在在厨房
			else if(bPreIsInKitnes == 1)
			{
				if(bStartInKitchen!=bEndInKitchen) // 两点不在同一区域
				{
					if(bStartInKitchen)
					{
						InStart+=HeightVector;
					}
				}
				else// 两点在同一区域
				{
					InStart += HeightVector;
				}
			}
			//前一个点在不在厨房
			else
			{
			}
		}
		else if(LineType==OT_ColdWaterTube||//冷水管
			LineType==OT_HotWaterTube||//热水管
			LineType==OT_Drain || //排水管
			LineType==OT_StrongElectricity_Single||           //强电单控
			LineType==OT_StrongElectricity_Double           //强电双控
			)
		{
			InStart+=HeightVector;
			InEnd+=HeightVector;
		}

		bPreIsInKitnes=bStartInKitchen;
	}
	

	TSharedPtr<FArmyPipePoint> StartPoint = FArmyHydropowerDataManager::Get()->MakeShareablePoint(InStart,LineType,Res->PointColor,Res->PointReformColor);
	TSharedPtr<FArmyPipePoint> EndPoint = FArmyHydropowerDataManager::Get()->MakeShareablePoint(InEnd,LineType,Res->PointColor,Res->PointReformColor);
 
	TSharedPtr<FArmyPipeline> Result = FArmyHydropowerDataManager::Get()->CreatePipeline(StartPoint,EndPoint, Res);
	
	if (Result.IsValid())
	{//请求施工项
		AActor *SelectedActor = Cast<AActor>(Result->GetRelevanceActor());
		auto TempController = dynamic_cast<FArmyHydropowerModeController*>(GGI->DesignEditor->CurrentController.Get());
		TempController->RequestConstructionList(Result->GetRelevanceActor());
	}

	//if (ContentItem->GetComponent().IsValid())
	//{
	//	TArray<TSharedPtr<FArmyFurniture>> ObjectLayers = FArmyToolsModule::ParseDXF (ContentItem->GetComponent()->FilePath);
	//	if (ObjectLayers.Num())
	//	{
	//		TSharedPtr<FArmyFurniture> CurrentObj = ObjectLayers[0];
	//		CurrentObj->SetTransform (FTransform(Start));
	//		CurrentObj->SetType (OT_ComponentBase);
	//		CurrentObj->BelongClass = ContentItem->GetComponent()->TypeID;
	//		CurrentObj->ComponentType = ContentItem->GetComponent()->ComponentID;
	//		FArmySceneData::Get ()->Add(CurrentObj,XRArgument(1).ArgUint32 (E_HydropowerModel),Result);
	//	}
	//}
	//
}


void FArmyPipelineOperation::Update(UArmyEditorViewportClient* InViewPortClient,class FViewport* InViewPort)
{
	
	Auxiliary->Update(InViewPortClient,MousePosition);
	MousePosition = FArmyMouseCaptureManager::Get()->GetCapturePoint();
	FVector WallNormal = FArmyMouseCaptureManager::Get()->GetCaptureNormal();
	//UE_LOG(LogTemp, Warning, TEXT("捕获鼠标坐标 %s"), *MousePosition.ToString());
	if (!IsDrawing())
		return;

	//InputBoxWidget->Show(true);
	RulerLine->ShowInputBox(true);
	Start = CachedPoints.Last();
	FArmyMouseCaptureManager::Get()->SetStartPoint(Start,true);
	End = End.IsNearlyZero() ? (CachedPoints.Last()+FVector::UpVector) : MousePosition;

	if (!IsDrawing())
		return;

	FVector Direction = End-Start;
	FVector DirectionNormal = Direction.GetSafeNormal();

	FVector2D PixPos;
	{
		bool bXY = (GVC->GetViewportType()==EXRLevelViewportType::LVT_OrthoXY);
		FVector Direction = FVector(0,0,15);
		if (bXY)
		{
			Direction = FVector::CrossProduct(DirectionNormal,FVector::UpVector).GetSafeNormal();
		}
		else
		{
			Direction = FVector::CrossProduct(WallNormal,DirectionNormal).GetSafeNormal();
		}
		Direction  = 15*Direction;
		RulerLine->Update(Start,End,Direction,Direction,false);
	}
	
}

bool FArmyPipelineOperation::IsNumber ()
{
	
	return false;
}

bool FArmyPipelineOperation::IsDrawing ()
{
	return CachedPoints.Num() > 0;
}

void FArmyPipelineOperation::SetCurrentItem (TSharedPtr<FContentItemSpace::FContentItem> item)
{
	ContentItem = item;
}

TSharedPtr<class FArmyPipePoint> FArmyPipelineOperation::MakeShareablePoint(EObjectType InType,FColor InColor,FColor InReformColor,bool bTrans /*= false*/)
{
	EObjectType LineTypeObj = InType;
	EObjectType LinerType = FArmyObject::GetPipeLinkerType(InType);
	/*	
		(LineTypeObj==OT_Drain_Point ? OT_Drain_Linker : EObjectType((int32)(LineTypeObj-OT_Pipe_Begin)+OT_Linker_Begin));
	if (LineTypeObj == OT_StrongElectricity_6)
	{
		LinerType = OT_StrongElectricity_6_Linker;
	}*/
	
	TSharedPtr<FArmyPipePoint> New = MakeShareable(new FArmyPipePoint());
	New->SetLineType(LineTypeObj);
	New->SetColor(InColor);
	New->SetReformColor(InReformColor);
	New->SetType(LinerType);
	FArmySceneData::Get()->Add(New,XRArgument(1).ArgUint32(E_HydropowerModel),nullptr,bTrans);
	return New;
}

void FArmyPipelineOperation::OnInputBoxCommitted (const FText& InText,const ETextCommit::Type InTextAction)
{
	if(InTextAction==ETextCommit::OnEnter)
	{
		const int32 LineLength=FCString::Atoi(*InText.ToString());

		if (LineLength == 0)
		{
			return;
		}

		FVector LineDirection=FArmyMath::GetLineDirection(Start,End);
		FVector Offset=LineLength * 0.1f *LineDirection;
		End=Start+Offset;
		Exec_AddPipeline(Start,End);
		CachedPoints.Add(End);
		ForceUpdate();
		Tick();
	}
}


void FArmyPipelineOperation::OnInputBoxKeyDown(const FKeyEvent& InKeyEvent)
{
	//InputBoxWidget->SetInputText()
	int32 a = 0;
}

bool FArmyPipelineOperation::MergeTwoPoint (TArray< TSharedPtr<class FArmyPipePoint > > Pointes)
{
	bool bMerge = false;
	for(auto Point:Pointes)
	{
		EObjectType LinerType = Point->GetType ();


		TArray<FObjectWeakPtr> Objects;
		//PipeGraph.Pin()->GetObjects(LinerType,Objects); 
		FArmySceneData::Get ()->GetObjects(E_HydropowerModel,LinerType,Objects);
		bool bCanDelete = false;
		for(int32 i = 0;i<Objects.Num();i++)
		{
			TSharedPtr<FArmyPipePoint> PipePoint = StaticCastSharedPtr<FArmyPipePoint> (Objects[i].Pin ());
			if(PipePoint->IsBridgeBend())
				continue;
			if(PipePoint.ToSharedRef()==Point.ToSharedRef())
				continue;

			FVector Direction = PipePoint->GetLocation() - Point->GetLocation();
			float lenght = Direction.Size();
			float DistRaduis = (PipePoint->GetRadius()+ Point->GetRadius()) /10.f ;
			if( lenght <= DistRaduis )
			{
				if(!PipePoint->IsElectirc())
				{
					if(PipePoint->GetPPType()==EPPT_Tee)
						continue;
				}
				if(PipePoint->IsElectirc())
				{
					if(PipePoint->PipeLines.Num()>=2)
						continue;
				}

				if(CanPointesMerge(Point,PipePoint))
				{
					PipePoint->MergeOtherPoint(Point);
					bCanDelete = true;
				}
				break;
			}
		}
		if(bCanDelete)
		{
			bMerge = true;
			FArmySceneData::Get()->Delete(Point);
		}
	}
	return bMerge;
}

bool FArmyPipelineOperation::TruncationLineByPoint (TArray< TSharedPtr<class FArmyPipePoint > > Pointes)
{
	for(auto Point:Pointes)
	{
		EObjectType LineType = FArmyObject::GetPipeType(Point->GetType());// EObjectType((Point->GetType() - OT_Linker_Begin) + OT_Pipe_Begin);
	/*	if (Point->GetType() == OT_StrongElectricity_6_Linker)
		{
			LineType = OT_StrongElectricity_6;
		}*/
		
		TArray<FObjectWeakPtr> Objects;
		FArmySceneData::Get()->GetObjects(E_HydropowerModel,LineType,Objects);
		if(LineType==OT_Drain)
			FArmySceneData::Get()->GetObjects(E_HydropowerModel,OT_Drain_Point,Objects);

		if( !Point->IsElectirc() && Point->GetPPType()==EPPT_None)
		{
			for( int32 i = 0 ;i< Objects.Num();i++ )
			{
				TSharedPtr<FArmyPipeline> Pipeline = StaticCastSharedPtr<FArmyPipeline> ( Objects[i].Pin() );
				if(Pipeline.IsValid ())
				{
					float DistRaduis = (Pipeline->GetRadius ()*2)/10.f;
					if(Pipeline->GetStart ().Equals (Point->GetLocation (),DistRaduis)
						||Pipeline->GetEnd ().Equals (Point->GetLocation (),DistRaduis)
						)
						continue;
					FVector ProjectPoint;
					bool bStartInLine =  FArmyMath::NearlyPoint(Point->GetLocation(), Pipeline->GetStart(),Pipeline->GetEnd(),ProjectPoint,DistRaduis );
					if(bStartInLine)
					{
						Point->SetLocation(ProjectPoint);
						TSharedPtr<FArmyPipePoint> StartPoint = Point;
						TSharedPtr<FArmyPipePoint> OldStartPoint = Pipeline->StartPoint;
						TSharedPtr<FArmyPipePoint> OldEndPoint = Pipeline->EndPoint;

						Pipeline->ModifyEndPoint (StartPoint);

						TSharedPtr<FArmyPipeline> Result1 = Pipeline->CopyPipeLine();
						StartPoint->AddPipeLine (Result1);
						OldEndPoint->AddPipeLine (Result1);
						Result1->SetStartPoint (StartPoint);
						Result1->SetEndPoint (OldEndPoint);
						FArmySceneData::Get()->Add(Result1,XRArgument(1).ArgUint32(E_HydropowerModel));
						Result1->GeneratePipelineModel (GVC->GetWorld());
						break;
					}
				}
			}
		}
	}
	return false;
}

bool FArmyPipelineOperation::CreateBridgeBend(TArray<TSharedPtr<class FArmyPipePoint>>& Points)
{
	TArray<TWeakPtr<FArmyPipeline>> Lines;
	for (int32 i = 0 ;i<Points.Num();i++)
	{
		for (int32 j = 0 ;j<Points[i]->PipeLines.Num();j++)
		{
			Lines.AddUnique (Points[i]->GetLine(j));
		}
		
	}
	TArray<TSharedPtr<FArmyPipeline>>  ModifyLines;
	for(int32 i = 0;i<Lines.Num ();i++)
	{
		TSharedPtr<FArmyPipeline> Line = Lines[i].Pin();
		if (!Line.IsValid())
			continue;
		EObjectType PointType = FArmyObject::GetPipeLinkerType(Line->GetType());// EObjectType(Line->GetType() - OT_Pipe_Begin + OT_Linker_Begin);
		
		//if (Line->GetType() == OT_StrongElectricity_6)
		//{
		//	PointType = OT_StrongElectricity_6_Linker;
		//}

		TArray<FObjectWeakPtr> LineObjects;
		GetObjects(Line->PipeType,LineObjects);
		if(PointType==OT_Drain_Linker)
			FArmySceneData::Get()->GetObjects(E_HydropowerModel,OT_Drain_Point,LineObjects);

	
		TArray<TSharedPtr<FArmyPipePoint>> DeleteBridges;

		for(int32 k = 0;k<LineObjects.Num ();k++) 
		{
			TSharedPtr<FArmyPipeline> LineNext = StaticCastSharedPtr<FArmyPipeline> (LineObjects[k].Pin());
			if(!Line->IsIntersect(LineNext))
				continue;
			FVector LPoint,LPointNext;
			Line->Intersect(LineNext,LPoint,LPointNext);
			if (LPointNext.IsNearlyZero() && LPoint.IsNearlyZero())
				continue;
			TSharedPtr<FIntersectionStateInfo> IntersectionInfo;
			bool bHasBridgeLine = IsLineIntersection(Line->GetUniqueID(),LineNext->GetUniqueID(),IntersectionInfo);


			FVector Normal =   FArmyPipeline::CrossProduct(Line,LineNext);

			FVector TempPoint = LPoint + 5 * Normal;
			if (G3DM->IsPointInRoom(TempPoint))
			{
				Normal = -Normal;
			}
			if (bHasBridgeLine)
			{
				IntersectionInfo->UpdateLocation(LPoint,Normal);
			}
			else
			{
				TSharedPtr<FIntersectionStateInfo> IntersectionStateInfo = MakeShareable(new FIntersectionStateInfo);
				IntersectionStateInfo->AddInstersection(Line->GetUniqueID(),LineNext->GetUniqueID(),LPoint,Normal);
				AddIntersection(Line->GetUniqueID(),IntersectionStateInfo);
				AddIntersection(LineNext->GetUniqueID(),IntersectionStateInfo);
			}
		}
	}
	return false;
}

void FArmyPipelineOperation::UpdateBridgeBend (TArray<TSharedPtr<class FArmyPipePoint>>& Points)
{
	TArray<TSharedPtr<FArmyPipeline>> UpdateLines;
	for (int32 i = 0;i<Points.Num ();i++)
	{
		for (int32 j = 0;j<Points[i]->PipeLines.Num ();j++)
		{
			UpdateLines.AddUnique(Points[i]->GetLine(j).Pin());
		}
	}
	for (int32 i = 0 ;i<IntersectionInfoes.Num();i++)
	{
		FLineIntersectionInfo& LineIntersectionInfo = IntersectionInfoes[i];
		TWeakPtr<FArmyObject> Line1Obj = FArmySceneData::Get()->GetObjectByGuid(E_HydropowerModel,LineIntersectionInfo.LineID);
		if (Line1Obj.IsValid())
		{
			for (int32 j = 0;j<LineIntersectionInfo.Intersectiones.Num();j++)
			{
				TSharedPtr<FIntersectionStateInfo>& IntersectionStateInfo = LineIntersectionInfo.Intersectiones[j];
				//设置未被统计
				IntersectionStateInfo->SetState(false);
				if (!IntersectionStateInfo->IsIntersection())
				{
					LineIntersectionInfo.Intersectiones.RemoveAt(j);
					j--;
				}
			}
			TSharedPtr<FArmyPipeline> Line1 = StaticCastSharedPtr<FArmyPipeline>(Line1Obj.Pin());
			UpdateLines.AddUnique(Line1);
		}
		else
		{
			IntersectionInfoes.RemoveAt(i);
			i--;
		}
	}
	for (auto& Line:UpdateLines)
	{
		if (!Line.IsValid())
		{
			continue;
		}
		FGuid LineID = Line->GetUniqueID();
		for (int32 i = 0;i<Line->AttachmentPoints.Num();i++)
		{
			TSharedPtr<FArmyPipePoint> BridgePoint = Line->AttachmentPoints[i];
			Line->AttachmentPoints.Remove(BridgePoint);
			FArmySceneData::Get()->Delete(BridgePoint,true);
			i--;
		}

		TArray<TSharedPtr<FBridgePointInfo>> OutPointInfoes;
		GetBridgePointInfo(LineID,OutPointInfoes);
		for (int32 i = 0;i<OutPointInfoes.Num();i++)
		{
			TSharedPtr<FBridgePointInfo> PointInfo = OutPointInfoes[i];
			FVector BottomPoint = PointInfo->GetBottomLocation();
			TSharedPtr<FArmyPipePoint> BridgePoint = FArmyHydropowerDataManager::Get()->MakeShareablePoint(BottomPoint,(EObjectType)PointInfo->LinkerType,PointInfo->NormalColor,PointInfo->ReformColor,true);
			BridgePoint->SetBridgeBendRadius (PointInfo->OverlyLineRadius);
			BridgePoint->SetBridgeBottomPoint(BottomPoint);
			BridgePoint->PipePointType = BridgePoint->IsElectirc() ? EPPT_DirectBridgeBend : EPPT_BridgeBend;
			BridgePoint->GeneratePipePointModel(GVC->GetWorld ());
			BridgePoint->AddBridgeBendPoint(PointInfo->StartObjectID,PointInfo->GetNearlyStart());
			BridgePoint->AddBridgeBendPoint(PointInfo->EndObjectID,PointInfo->GetNearlyEnd());
			BridgePoint->ArcType = PointInfo->LineInfoes.Num() == 1 ? AT_Single :AT_Mutil;
			Line->AddBridgeBendPoint(BridgePoint);
		}
		Line->UpdateWithPoints();
	}
}

void FArmyPipelineOperation::GetObjects(EPipeType Type,TArray<FObjectWeakPtr>& Objects)
{
	{
		FArmySceneData::Get ()->GetObjects (E_HydropowerModel, OT_StrongElectricity_25,Objects);
		FArmySceneData::Get ()->GetObjects (E_HydropowerModel,OT_StrongElectricity_4,Objects);
		FArmySceneData::Get ()->GetObjects (E_HydropowerModel,OT_StrongElectricity_Single,Objects);
		FArmySceneData::Get ()->GetObjects (E_HydropowerModel,OT_StrongElectricity_Double,Objects);
		FArmySceneData::Get ()->GetObjects (E_HydropowerModel,OT_WeakElectricity_TV,Objects);
		FArmySceneData::Get ()->GetObjects (E_HydropowerModel,OT_WeakElectricity_Net,Objects);
		FArmySceneData::Get ()->GetObjects (E_HydropowerModel,OT_WeakElectricity_Phone,Objects);
		FArmySceneData::Get ()->GetObjects (E_HydropowerModel,OT_HotWaterTube,Objects);
		FArmySceneData::Get ()->GetObjects (E_HydropowerModel,OT_Drain,Objects);
		FArmySceneData::Get ()->GetObjects (E_HydropowerModel,OT_ColdWaterTube,Objects);
	}
}

bool FArmyPipelineOperation::CanPointesMerge (TSharedPtr<class FArmyPipePoint> P1,TSharedPtr<class FArmyPipePoint> P2)
{

	/**  	EPPT_None = 0, // 没有类型
	EPPT_Direct = 1,// 直通（算量）
	EPPT_Flexure = 2,//弯通（算量）*/
	if(!P1.IsValid ()||!P2.IsValid ())
		return false;
	if(P1->IsBridgeBend ()||
		P2->IsBridgeBend ())
		return false;
	if(P1->GetType ()!=P2->GetType ())
		return false;
	if(P1->LineType != P2->LineType)
		return false;
	if (P1->IsElectirc() && P2->IsElectirc())
	{
		return (P1->GetPPType ()==EPPT_None && P2->GetPPType() ==EPPT_None);
	}
	else
	{
		if (P1->GetPPType() == EPPT_Direct || P1->GetPPType() == EPPT_Flexure)
			return P2->GetPPType() == EPPT_None;
		else if ( P1->GetPPType() == EPPT_None)
			return P2->GetPPType()==EPPT_Direct||P2->GetPPType()==EPPT_Flexure||P2->GetPPType() == EPPT_None;
	}
	return false;
}

void FArmyPipelineOperation::OnDeleteSelectionActor (AActor* Actor)
{
	if (!Actor && !Actor->IsValidLowLevel())
		return;
	TArray<TWeakPtr<FArmyPipeline>> lines;

	if(Actor->IsA (AXRPipePointActor::StaticClass ()))
	{
		AXRPipePointActor* PointActor = Cast<AXRPipePointActor> (Actor);
		TSharedPtr<FArmyPipePoint> Point = StaticCastSharedPtr<FArmyPipePoint> (PointActor->GetRelevanceObject().Pin ());
		
		if (Point.IsValid())
		{
			for (auto Line : Point->PipeLines)
			{
				lines.Add(Point->GetLine(Line).Pin());
			}
		}
	}
	else if(Actor->IsA (AXRPipelineActor::StaticClass ()))
	{
		AXRPipelineActor* LineActor = Cast<AXRPipelineActor> (Actor);
		TSharedPtr<FArmyPipeline> Line = StaticCastSharedPtr<FArmyPipeline> (LineActor->GetRelevanceObject().Pin ());
		lines.Add(Line);
	}
	
	DeleteLines(lines);
	lines.Empty();

	int32 i = 0;
}


void FArmyPipelineOperation::DeleteLines(TArray<TWeakPtr<FArmyPipeline>>& Lines)
{
	TArray<TSharedPtr<FArmyPipePoint>> Points;
	for(auto& Line:Lines)
	{
		if (Line.IsValid())
		{
			TSharedPtr<FArmyPipeline> LinePtr = Line.Pin();
			for (auto It = LinePtr->OverlayLines.CreateIterator(); It; ++It)
			{
				FGuid OverLineID = *It;
				TSharedPtr<FArmyPipeline> InstentLine = StaticCastSharedPtr<FArmyPipeline>(FArmySceneData::Get()->GetObjectByGuid(E_HydropowerModel, OverLineID).Pin());
				Points.AddUnique(InstentLine->StartPoint);
				Points.AddUnique(InstentLine->EndPoint);
			}
			LinePtr->DeleteSelf(true);
			FArmySceneData::Get()->Delete(LinePtr);
		}
	}
	CreateBridgeBend(Points);
	UpdateBridgeBend(Points);
}

void FArmyPipelineOperation::PostEditPointes(TArray<TSharedPtr<FArmyObject>>& Objectes)
{
	
	TArray<TSharedPtr<FArmyPipePoint>> Pointes;
	
	for(auto Object:Objectes)
	{
		TSharedPtr<FArmyPipePoint>Point = StaticCastSharedPtr<FArmyPipePoint>(Object);
		if(!Point->IsBridgeBend())
		{
			Pointes.Add(Point);
		}

	}
	SCOPE_TRANSACTION(TEXT("修改水电位置"));
	MergeTwoPoint(Pointes);
	TruncationLineByPoint(Pointes);
	CreateBridgeBend(Pointes);
	UpdateBridgeBend(Pointes);
}

void FArmyPipelineOperation::PostModifyEditPointes(TArray<TSharedPtr<FArmyObject>>& Objectes,bool bTrans)
{
	TArray<TSharedPtr<FArmyPipePoint>> Pointes;
	TArray<TWeakPtr<FArmyPipeline>> Lines;
	for(auto Object:Objectes)
	{
		TSharedPtr<FArmyPipePoint>Point=StaticCastSharedPtr<FArmyPipePoint>(Object);
		if(!Point->IsBridgeBend())
		{
			Pointes.Add(Point);
			TArray<TWeakPtr<FArmyPipeline>> TempLines;
			Point->GetPipeLines(TempLines);
			for (auto& Item : TempLines)
			{
				Lines.AddUnique(Item);
			}

		}
		
	}
	if (bTrans)
	{
		for (int32 i = 0 ;i<Lines.Num();i++)
		{
			TSharedPtr<FArmyPipeline> Line = Lines[i].Pin();
			if (!Line.IsValid())
				continue;
			Line->UpdateWithPoints(true);
		}
	}
	else
	{
		MergeTwoPoint(Pointes);
		TruncationLineByPoint(Pointes);
	}

	CreateBridgeBend(Pointes);
	UpdateBridgeBend(Pointes);
	
}

void FArmyPipelineOperation::UpdateProductInfo(int32 ComponentID,FString PipeDiameter,FString ObjectIDStr)
{
	LocalModelService->UpdateAutoComponentProductInfo(ComponentID,PipeDiameter,ObjectIDStr);
}

void FArmyPipelineOperation::UpdateProductInfoAfter(FString guidStr,int32 productID)
{
	FGuid guid;
	FGuid::Parse(guidStr,guid);
	FObjectWeakPtr object = FArmySceneData::Get()->GetObjectByGuid(E_HydropowerModel,guid);
	if ( object.IsValid())
	{
		TSharedPtr<FArmyPipePoint> Point = StaticCastSharedPtr<FArmyPipePoint>(object.Pin());
		if ( Point.IsValid() )
		{
			Point->ProductID = productID;
			Point->ChangeColor();
		}
	}
}

FString FArmyPipelineOperation::GetMousePointSpaceName(const FVector& MousePoint)
{
	TArray<TWeakPtr<FArmyObject>> InObjects;
	FArmySceneData::Get ()->GetObjects(E_HomeModel,OT_InternalRoom,InObjects);
	if(InObjects.Num ()==0)
		return FString();
	for(int32 i=0;i<InObjects.Num ();i++)
	{
		FObjectPtr object=InObjects[i].Pin ();
		if(object->GetType ()!=OT_InternalRoom)
			continue;
		TSharedPtr<FArmyRoom> Room=StaticCastSharedPtr<FArmyRoom>(object);
		FBox bounds=Room->GetBounds3D();
		bool bCaputure=false;
		if(FMath::PointBoxIntersection(MousePoint,bounds))
		{
			return Room->GetSpaceName();
		}
	}

	return FString();
}

bool FArmyPipelineOperation::IsInKitchenes(const FVector& MousePoint)
{
	FString SpaceName=GetMousePointSpaceName(MousePoint);
	if(SpaceName.Equals( TEXT("主卫生间"))||SpaceName.Equals(TEXT("客卫生间"))||SpaceName.Equals(TEXT("厨房")))
	{
		return true;
	}
	return false;
}

bool FArmyPipelineOperation::IsLineIntersection(const FGuid& line1,const FGuid & line2,TSharedPtr<FIntersectionStateInfo>& IntersectionInfo)
{
	int32 Index1 = IntersectionInfoes.Find(FLineIntersectionInfo(line1));
	int32 Index2 = IntersectionInfoes.Find(FLineIntersectionInfo(line2));
	bool bHaseLine = Index1 !=INDEX_NONE && Index2 != INDEX_NONE;
	if (bHaseLine)
	{
		
		TArray<TSharedPtr<FIntersectionStateInfo>>& IntersectionStateInfoArr = IntersectionInfoes[Index1].Intersectiones;
		if (IntersectionStateInfoArr.Num())
		{
			for (int32 i = 0; i<IntersectionStateInfoArr.Num();i++)
			{
				TSharedPtr<FIntersectionStateInfo>& InfoItem = IntersectionStateInfoArr[i];
				if (InfoItem->Contains(line2))
				{
					IntersectionInfo = InfoItem;
					return true;
				}
			}
			return false;
		}
		return false;
	}
	return false;
}

void FArmyPipelineOperation::GetBridgePointInfo(FGuid& InLineID,TArray<TSharedPtr<FBridgePointInfo>>& OutPointInfoes)
{
	int32 Index = IntersectionInfoes.Find(FLineIntersectionInfo(InLineID));
	if (Index != INDEX_NONE)
	{
		FLineIntersectionInfo& LineIntersectionInfo = IntersectionInfoes[Index];
		if (LineIntersectionInfo.IsWaterLine())
		{
			for (int32 j = 0;j<LineIntersectionInfo.Intersectiones.Num();j++)
			{
				TSharedPtr<FIntersectionStateInfo>& IntersectionStateInfo = LineIntersectionInfo.Intersectiones[j];
				if (IntersectionStateInfo->IsLineBridge(InLineID))
				{
					TSharedPtr<FArmyPipeline> OtherLine = IntersectionStateInfo->GetOtherLinePtr(InLineID);
					TSharedPtr<FArmyPipeline> Line = IntersectionStateInfo->GetLinePtr(InLineID);
					EObjectType LinkerType = Line->GetType();
					TSharedPtr<FBridgePointInfo> PointInfo = MakeShareable(new FBridgePointInfo(
						Line->GetRealStart(),
						Line->GetRealEnd(),
						Line->StartPoint->GetUniqueID(),
						Line->EndPoint->GetUniqueID(),
						Line->Radius,
						Line->PointColor,
						Line->PointRefromColor,
						LinkerType));
					TSharedPtr<FBridgeOverlyLineInfo> OverlyLineInfo = MakeShareable(new FBridgeOverlyLineInfo(OtherLine->GetUniqueID(),
						IntersectionStateInfo->Intersection->Location,
						OtherLine->GetRadius(),
						IntersectionStateInfo->Intersection->Normal));
					PointInfo->AddBridgeOverlyLineInfo(OverlyLineInfo);
					OutPointInfoes.AddUnique(PointInfo);
					IntersectionStateInfo->SetState(true);
				}
			}
		}
		else
		{
			for (int32 j = 0;j<LineIntersectionInfo.Intersectiones.Num();j++)
			{
				TArray<TSharedPtr<FIntersectionStateInfo>> LineIntersectionStateList,OtherLineIntersectionStateList;
				TSharedPtr<FIntersectionStateInfo>& IntersectionStateInfo = LineIntersectionInfo.Intersectiones[j];

				if (IntersectionStateInfo->GetState())
				{
					continue;
				}

				LineIntersectionInfo.GetNearIntersectionstateInfoes(IntersectionStateInfo,LineIntersectionStateList);
				int32 OtherIndex = IntersectionInfoes.Find(FLineIntersectionInfo(IntersectionStateInfo->GetOtherLineID(InLineID)));
				if (OtherIndex != INDEX_NONE)
				{
					FLineIntersectionInfo& OtherLineIntersectionInfo = IntersectionInfoes[OtherIndex];
					OtherLineIntersectionInfo.GetNearIntersectionstateInfoes(IntersectionStateInfo,OtherLineIntersectionStateList);
				}
				if (LineIntersectionStateList.Num()>=OtherLineIntersectionStateList.Num())
				{
					bool bCreateBridge = true;
					if (LineIntersectionStateList.Num()==OtherLineIntersectionStateList.Num())
					{
						for (auto& Item :LineIntersectionStateList)
						{
							if (Item->GetState())
							{
								bCreateBridge = false;
								break;
							}
						}
					}
					if (!bCreateBridge)
						continue;
					TSharedPtr<FArmyPipeline> Line = IntersectionStateInfo->GetLinePtr(InLineID);
					EObjectType LinkerType = Line->GetType();
					TSharedPtr<FBridgePointInfo> PointInfo = MakeShareable(new FBridgePointInfo(
						Line->GetRealStart(),
						Line->GetRealEnd(),
						Line->StartPoint->GetUniqueID(),
						Line->EndPoint->GetUniqueID(),
						Line->Radius,
						Line->PointColor,
						Line->PointRefromColor,
						LinkerType));
					for (auto& LineStateInfo :LineIntersectionStateList)
					{
						TSharedPtr<FArmyPipeline> OtherLine = LineStateInfo->GetOtherLinePtr(InLineID);
						TSharedPtr<FBridgeOverlyLineInfo> OverlyLineInfo = MakeShareable(new FBridgeOverlyLineInfo(OtherLine->GetUniqueID(),
							LineStateInfo->Intersection->Location,
							OtherLine->GetRadius(),
							LineStateInfo->Intersection->Normal));
						PointInfo->AddBridgeOverlyLineInfo(OverlyLineInfo);
						LineStateInfo->SetState(true);
					}
					IntersectionStateInfo->SetState(true);
					OutPointInfoes.AddUnique(PointInfo);
				}
				if (LineIntersectionStateList.Num()<=OtherLineIntersectionStateList.Num())
				{
					bool bIsEleWater = IntersectionStateInfo->IsWaterElecIntersection();
					if (bIsEleWater)
					{

						TSharedPtr<FArmyPipeline> Line = IntersectionStateInfo->GetLinePtr(InLineID);
						TSharedPtr<FArmyPipeline> OtherLine = IntersectionStateInfo->GetOtherLinePtr(InLineID);
						EObjectType LinkerType = Line->GetType();
						TSharedPtr<FBridgePointInfo> PointInfo = MakeShareable(new FBridgePointInfo(
							Line->GetRealStart(),
							Line->GetRealEnd(),
							Line->StartPoint->GetUniqueID(),
							Line->EndPoint->GetUniqueID(),
							Line->Radius,
							Line->PointColor,
							Line->PointRefromColor,
							LinkerType));
						for (auto& StateInfo:LineIntersectionStateList)
						{
							if (!StateInfo->IsWaterElecIntersection())
								break;
							TSharedPtr<FBridgeOverlyLineInfo> OverlyLineInfo = MakeShareable(new FBridgeOverlyLineInfo(StateInfo->GetOtherLineID(InLineID),
								StateInfo->Intersection->Location,
								OtherLine->GetRadius(),
								StateInfo->Intersection->Normal));
							PointInfo->AddBridgeOverlyLineInfo(OverlyLineInfo);
							OutPointInfoes.AddUnique(PointInfo);
							StateInfo->SetState(true);
						}
					}
					
				}
			}
		}
	}
	
	
}

void FArmyPipelineOperation::AddIntersection(const FGuid& InLineID,TSharedPtr<FIntersectionStateInfo>& IntersectionInfo)
{
	int Index = IntersectionInfoes.Find(FLineIntersectionInfo(InLineID));
	if (Index ==INDEX_NONE)
	{
		Index = IntersectionInfoes.AddUnique(FLineIntersectionInfo(InLineID));
	}
	FLineIntersectionInfo& lineInfo = IntersectionInfoes[Index];
	lineInfo.AddIntersertionInfo(IntersectionInfo);
}

void FArmyPipelineOperation::OnObjectOperation(const XRArgument& InArg,FObjectPtr InObj,bool bTransaction/*=false*/)
{
	if (InArg._ArgUint32 != E_HydropowerModel)
		return;
	if (InArg._ArgInt32 == 1)
	{
		if(InObj->IsPipeLine())
		{
			TSharedPtr<FArmyPipeline> Line=StaticCastSharedPtr<FArmyPipeline>(InObj);
			if (!Line.IsValid())
				return;
			Line->GeneratePipelineModel(GVC->GetWorld());
			if (InArg._ArgBoolean)
			{
				TArray<TSharedPtr<FArmyPipePoint>> Points;
				Points.Add(Line->StartPoint);
				Points.Add(Line->EndPoint);

				if(!bTransaction)
				{
					MergeTwoPoint(Points);
					TruncationLineByPoint(Points);
				}
				CreateBridgeBend(Points);
				UpdateBridgeBend(Points);
			}
			Line->PostEditDelegate.BindRaw (this,&FArmyPipelineOperation::PostEditPointes);
			Line->ModifyDelegate.BindRaw(this,&FArmyPipelineOperation::PostModifyEditPointes);
		}
		if(InObj->IsPipeLineLinker())
		{
			TSharedPtr<FArmyPipePoint> Point=StaticCastSharedPtr<FArmyPipePoint>(InObj);
			Point->PostEditDelegate.BindRaw (this,&FArmyPipelineOperation::PostEditPointes);
			Point->UpdateInfoDelegate.BindRaw(this,&FArmyPipelineOperation::UpdateProductInfo);
		}
		if (InObj->GetType() == OT_Drain_Point)
		{
			TSharedPtr<FArmyPipeline> Line = StaticCastSharedPtr<FArmyPipeline>(InObj);
			Line->GeneratePipelineModel(GVC->GetWorld());
			Line->UpdateWithPoints();
		}
	}
	else if (InArg._ArgInt32 == 0)
	{
		if(InObj->IsPipeLine())
		{
			TArray<TSharedPtr<FArmyPipePoint>> Points;
			TSharedPtr<FArmyPipeline> Line = StaticCastSharedPtr<FArmyPipeline>(InObj);
			TArray<FGuid> LineIDes(Line->OverlayLines);
			for (auto LineID : LineIDes)
			{
				TSharedPtr<FArmyPipeline> InstentLine = StaticCastSharedPtr<FArmyPipeline>(FArmySceneData::Get()->GetObjectByGuid(E_HydropowerModel,LineID).Pin());
                if (InstentLine.IsValid())
                {
                    Points.Add(InstentLine->StartPoint);
                    Points.Add(InstentLine->EndPoint);
                }
			}
            if (Points.Num())
            {
                CreateBridgeBend(Points);
                UpdateBridgeBend(Points);
            }
		}
	}
}

void FArmyPipelineOperation::OnActorMoved(AActor* InActor)
{
	TArray<AActor*> Actores;
	if(InActor->IsA(AXRGroupActor::StaticClass()))
	{
		AXRGroupActor* GroupActor=Cast<AXRGroupActor>(InActor);
		GroupActor->GetGroupActors(Actores);
	}
	TArray<TSharedPtr<FArmyObject>> Pointes;
	for (auto& Actor :Actores)
	{
		if (Actor->IsA(AXRPipelineActor::StaticClass()))
		{
			AXRPipelineActor* LineActor = Cast<AXRPipelineActor>(Actor);
			TSharedPtr<FArmyPipeline> PipeLine=StaticCastSharedPtr<FArmyPipeline> (LineActor->GetRelevanceObject().Pin ());
			Pointes.AddUnique(PipeLine->StartPoint);
			Pointes.AddUnique(PipeLine->EndPoint);
		}
	}
	if (Pointes.Num())
	{
		PostEditPointes(Pointes);
	}
}

