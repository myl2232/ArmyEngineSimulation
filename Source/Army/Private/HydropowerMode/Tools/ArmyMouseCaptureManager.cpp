#include "ArmyMouseCaptureManager.h"
#include "SceneView.h"
#include "Engine/Canvas.h"
#include "Engine/GameEngine.h"
#include "ArmyEditorViewportClient.h"
#include "Math/XRMath.h"
#include "ArmySceneData.h"
#include "ArmyRoom.h"
#include "ArmyPipeline.h"
#include "ArmyPipePoint.h"
#include "ArmyFurniture.h"
#include "ArmyViewportClient.h"
#include "ArmyEngineModule.h"
#include "ArmyPlayerController.h"
#include "ArmyEditorEngine.h"
#include "EngineUtils.h"
#include "ArmyFurnitureActor.h"
#include "Army3DManager.h"
#include "Runtime/Engine/Classes/Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"


IMPLEMENT_HIT_PROXY (HComponentProxy,HXRObjectVisProxy);
#define  CaputureLenght 2.f

FArmyMouseCaptureManager* FArmyMouseCaptureManager::Singleton = nullptr;

FArmyMouseCaptureManager::FArmyMouseCaptureManager()
	:MouseCapturePoint(FVector::ZeroVector)
	,StartPoint(FVector::ZeroVector)
	,ObjectType(OT_None)
	,bEnableCaptureCoordinate(false)
	,bCaptureAttachment (false)
	,bTopView(false)
	,bAdsorptionPoint(false)
{
	int32 i = 0;
}

FArmyMouseCaptureManager::~FArmyMouseCaptureManager()
{

}

FVector FArmyMouseCaptureManager::GetCapturePoint()
{
	
	return MouseCapturePoint;
}
FVector FArmyMouseCaptureManager::GetMovePoint()
{
	return FVector::ZeroVector;
}

FVector FArmyMouseCaptureManager::GetCaptureNormal ()
{
	return MouseCaptureNormal;
}

FVector FArmyMouseCaptureManager::GetCaptureNormal(FVector& InLocation)
{
	HAdsorbDetect AdsorbDetect = G3DM->CaptureWallInfoRelace(InLocation);
	if (AdsorbDetect.bAdsorbDetect)
	{
		return AdsorbDetect.WallNormal;
	}
	else
	{
		return FVector::UpVector;
	}
}

FVector FArmyMouseCaptureManager::GetCaptureWallDirection(FVector& InLocation)
{
	HAdsorbDetect AdsorbDetect = G3DM->CaptureWallInfoRelace(InLocation);
	if (AdsorbDetect.bAdsorbDetect)
	{
		return AdsorbDetect.WallDirection;
	}
	return FVector::ForwardVector;
	
}
HAdsorbDetect FArmyMouseCaptureManager::GetCaptureInfo(FVector& InLocation)
{
	auto info= G3DM->CaptureWallInfoRelace(InLocation,0.1f);
	return info;
}

HAdsorbDetect FArmyMouseCaptureManager::GetCaptureInfo()
{
	IsOnCorner(MouseCapturePoint);
	CurrentInfo.OnTop = IsOnTop(CurrentInfo.AdsPoint);
	CurrentInfo.OnFloor= IsOnFloor(CurrentInfo.AdsPoint);
	return CurrentInfo;
}

void FArmyMouseCaptureManager::Draw(UArmyEditorViewportClient* InViewPortClient,FViewport* InViewport, FCanvas* SceneCanvas)
{
	
}

void FArmyMouseCaptureManager::Draw (const FSceneView* View,FPrimitiveDrawInterface* PDI)
{
	if (!bEnable)
		return;

	for(TActorIterator<AXRFurnitureActor> ActorItr (GVC->GetWorld ()); ActorItr; ++ActorItr)
	{
		AXRFurnitureActor* Actor = *ActorItr;
		TSharedPtr<FArmyFurniture> ComponentItem = StaticCastSharedPtr<FArmyFurniture>(Actor->GetRelevanceObject().Pin());
		if (IsShowEditPoint(ComponentItem))
		{
			TMap<uint32,FVector> EditPoint3D,EditPoint2D;
			for(int32 i = 0;i<Actor->Children.Num ();i++)
			{
				AActor* child=Actor->Children[i];
				if(child->Tags.Contains(FName("HydropowerActor"))||child->Tags.Contains(FName("OriginalPoint")))
				{
					ComponentItem->GetPointMapes(EditPoint3D,EditPoint2D);
				}
			}
			TMap<uint32,FVector> ShowPointes = bTopView ? EditPoint2D:EditPoint3D;
			for(auto& Postion:ShowPointes)
			{
				FVector Location = Postion.Value;
				PDI->DrawPoint (Location,FLinearColor::Blue,10,SDPG_World);
			}
		}
		
	}
}

bool FArmyMouseCaptureManager::MouseMove (UArmyEditorViewportClient* InViewPortClient,FViewport* ViewPort,int32 X,int32 Y)
{
	//HHitProxy* HoveredHitProxy = ViewPort->GetHitProxy(X,Y);
	//bCaptureAttachment = false;
	//if(HoveredHitProxy)
	//{ 
	//	if(HoveredHitProxy->IsA (HComponentProxy::StaticGetType ()))
	//	{
	//		bCaptureAttachment = true;
	//		HComponentProxy* KeyProxy = (HComponentProxy*)HoveredHitProxy;
	//		FArmyFurniture* item = (FArmyFurniture*)(KeyProxy->Component.Pin ().Get ());
	//		if(item && item->GetEditPoints3D ().Num()!=0)
	//		{
	//			MouseCapturePoint = *(item->GetEditPoints3D ().Find (KeyProxy->KeyIndex));
	//			return true;
	//		}
	//		else
	//			return false;
	//			
	//	}
	//}
	return false;
}

void FArmyMouseCaptureManager::Reset()
{
	SelectObject = nullptr;
}

void FArmyMouseCaptureManager::Capture(class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y)
{

	AActor* SelectActor = GXREditor->GetSelectedActors()->GetBottom<AActor>();
	bool bDropActor = InViewPortClient->IsDroppingObjects();
	if (SelectActor && SelectActor->IsValidLowLevel() && !bDropActor&&bCaputureActor)
	{
		AActor* SelectActor = GXREditor->GetSelectedActors()->GetBottom<AActor>();
		if (SelectActor && SelectActor->IsValidLowLevel())
		{
			FVector mMouseCapturePoint = GVC->GetWidgetLocation();
			MouseCaptureNormal = GetCaptureNormal(mMouseCapturePoint);
			MouseCapturePoint = mMouseCapturePoint;
		}
	}
	else
	{
		/**@欧石楠顶视图模式下获取的点使用屏幕坐标转世界*/
		if (GXRPC->GetXRViewMode()==EXRView_TOP) {
			FVector2D MousePos;
			GVC->GetMousePosition(MousePos);
			FVector4 TempVector4 = GVC->PixelToWorld(MousePos.X,MousePos.Y,0);
			MouseCapturePoint.X = TempVector4.X;
			MouseCapturePoint.Y = TempVector4.Y;
			MouseCapturePoint.Z = 0.f;
			MouseCaptureNormal = FVector::ZeroVector;
		}
		else {
			MouseCapturePoint = GVC->GetCurrentMouseTraceToWorldPos();
			MouseCaptureNormal = GetCaptureNormal();
		}
	}
	

	if(!bEnable)
		return;
	//吸附点位附着点
	if (CaptureAdsorptionPoint(InViewPortClient,x,y))
	{
		return ;
	}
	// 吸附线上点
	if (CaptureLinePoint(InViewPortClient,x,y))
	{
		return;
	}
	//吸附线
	if (CaptureLine(InViewPortClient,x,y))
	{
		return ;
	}

	//吸附坐标轴
	CaptureCoordinatePoint(InViewPortClient,x,y);
	//吸附墙角
	CaptureCorner(InViewPortClient,x,y);
	//顶视图吸附墙面
	if (GXRPC->GetXRViewMode()==EXRView_TOP)
	{
		if (CaptureWallInTopView (InViewPortClient,x,y))
		{
			return;
		}
	}
	CaptureAngle45Coordinate(InViewPortClient,x,y);

	////吸附墙面
	//CapturePlane(InViewPortClient,x,y); 
}

bool FArmyMouseCaptureManager::CaptureCorner (class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y)
{
	float MousePointZ = MouseCapturePoint.Z;
	FVector TempMousePoint = MouseCapturePoint;
	TempMousePoint.Z = 0;
	TArray<TWeakPtr<FArmyObject>> InObjects;
	FArmySceneData::Get ()->GetObjects (E_LayoutModel,OT_InternalRoom,InObjects);
	if(InObjects.Num ()==0)
		return false;

	CurrentInfo.bAdsorbDetect = false;
	CurrentInfo.bCornerDetect = false;
	for(int32 i = 0;i<InObjects.Num ();i++)
	{
		FObjectPtr object = InObjects[i].Pin ();
		if(object->GetType ()!=OT_InternalRoom)
			continue;
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom> (object);
		FBox bounds = Room->GetBounds3D ();
		bool bCaputure = false;
		if(FMath::PointBoxIntersection (TempMousePoint,bounds))
		{
			TArray< TSharedPtr<FArmyLine> > Lines;
			Room->GetLines (Lines);



			for(auto LineIt:Lines)
			{
				FVector Start = LineIt->GetStart ();
				FVector End = LineIt->GetEnd ();
				FVector WStart = Start+FVector (0,0, FArmySceneData::WallHeight);
				FVector WEnd = End+FVector (0,0, FArmySceneData::WallHeight);
				FVector CaputurePoint;
				if(FArmyMath::NearlyPoint(TempMousePoint,Start,End,CaputurePoint,CaputureLenght))
				{
					if (!CurrentInfo.bAdsorbDetect)
					{
						bCaputure = true;
						TempMousePoint = CaputurePoint;

						CurrentInfo.bAdsorbDetect = true;
						CurrentInfo.WallDirection = End - Start;
						CurrentInfo.WallDirection.Normalize();
						CurrentInfo.WallNormal = CurrentInfo.WallDirection ^FVector::UpVector;
					}
					else
					{
						CurrentInfo.bCornerDetect = true;
						CurrentInfo.WallDirection2 = End - Start;
						CurrentInfo.WallDirection2.Normalize();
						CurrentInfo.WallNormal2 = CurrentInfo.WallDirection2 ^FVector::UpVector;
					}

					continue;
					//break;
				}
				if(FArmyMath::NearlyPoint (TempMousePoint,WStart,WEnd,CaputurePoint,CaputureLenght))
				{
					if (!CurrentInfo.bAdsorbDetect)
					{
						bCaputure = true;
						TempMousePoint = CaputurePoint;

						CurrentInfo.bAdsorbDetect = true;
						CurrentInfo.WallDirection = End - Start;
						CurrentInfo.WallDirection.Normalize();
						CurrentInfo.WallNormal = CurrentInfo.WallDirection ^FVector::UpVector;
					}
					else
					{
						CurrentInfo.bCornerDetect = true;
						CurrentInfo.WallDirection2 = End - Start;
						CurrentInfo.WallDirection2.Normalize();
						CurrentInfo.WallNormal2 = CurrentInfo.WallDirection2 ^FVector::UpVector;
					}
					//break;
				}
			}
		}

	
		if (bCaputure)
		{
			MouseCapturePoint = TempMousePoint;
			MouseCapturePoint.Z = MousePointZ;
			CurrentInfo.AdsPoint = MouseCapturePoint;
			return true;
		}

	}
	return false;
}

bool FArmyMouseCaptureManager::CapturePlane(class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y)
{
	if (!bEnableCaptureCoordinate)
		return false;

	FVector Direction = (MouseCapturePoint-StartPoint).GetSafeNormal();
	TArray<TWeakPtr<FArmyObject>> InObjects;
	FArmySceneData::Get ()->GetObjects (E_LayoutModel,OT_InternalRoom,InObjects);
	if (InObjects.Num ()==0)
		return false;
	for (int32 i = 0;i<InObjects.Num ();i++)
	{
		FObjectPtr object = InObjects[i].Pin ();
		if (object->GetType ()!=OT_InternalRoom)
			continue;
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom> (object);
		FBox bounds = Room->GetBounds3D ();
		bool bCaputure = false;
		if (Room->IsPointInRoom(MouseCapturePoint))
		{
			TArray<FVector> Pointes = Room->GetWorldPoints(true);
			int32 Lenght = Pointes.Num();
			for (int32 i = 0 ;i<Lenght;i++)
			{
				int32 NextIndex = (i == Lenght-1)? i % Lenght : i+1;
				FVector Pos (Pointes[i].X,Pointes[i].Y,280.f);
				
				//FVector ProjectPoint = FVector::PointPlaneProject(MouseCapturePoint,Pointes[i],Pointes[NextIndex],Pos);
				//float Lenght = (ProjectPoint - MouseCapturePoint).Size();
				//if (Lenght<CaputureLenght*10)
				//{
				//	MouseCapturePoint = ProjectPoint;
				//	bCaputure = true;
				//}
			}
			//TArray< TSharedPtr<FArmyLine> > Lines;
			//Room->GetLines (Lines);
			//for (auto LineIt:Lines)
			//{
			//	FPlane plane;
			//	/*FVector Start = LineIt->GetStart ();
			//	FVector End = LineIt->GetEnd ();
			//	FVector WStart = Start+FVector (0,0,FArmySceneData::WallHeight);
			//	FVector WEnd = End+FVector (0,0,FArmySceneData::WallHeight);
			//	FVector CaputurePoint;
			//	if (FArmyMath::NearlyPoint(MouseCapturePoint,Start,End,CaputurePoint,CaputureLenght))
			//	{
			//		bCaputure = true;
			//		MouseCapturePoint = CaputurePoint;
			//		break;
			//	}
			//	if (FArmyMath::NearlyPoint (MouseCapturePoint,WStart,WEnd,CaputurePoint,CaputureLenght))
			//	{
			//		MouseCapturePoint = CaputurePoint;
			//		bCaputure = true;
			//		break;
			//	}*/
			//}
		}
		if (bCaputure)
			return true;
	}
	return false;
}

bool FArmyMouseCaptureManager::CaptureLinePoint (class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y)
{
	EObjectType PointType = FArmyObject::GetPipeLinkerType(ObjectType);
	/*	EObjectType(ObjectType - OT_Pipe_Begin + OT_Linker_Begin);
	if (ObjectType == OT_StrongElectricity_6)
	{
		PointType = OT_StrongElectricity_6_Linker;
	}*/

	TArray<TWeakPtr<FArmyObject>> InObjects;
	FArmySceneData::Get()->GetObjects(E_HydropowerModel,PointType,InObjects);
	if(InObjects.Num()==0)
		return false;
	

	TArray<TWeakPtr<FArmyPipePoint>> TempPoints;
	for (int32 i = 0 ;i<InObjects.Num();i++)
	{
		TSharedPtr<FArmyPipePoint> Point = StaticCastSharedPtr<FArmyPipePoint>(InObjects[i].Pin());
		bool canLink=Point->CanLink();
		if (SelectObject.IsValid())
		{
			if(SelectObject->GetType()==ObjectType)
			{
				TSharedPtr<FArmyPipeline> Line=StaticCastSharedPtr<FArmyPipeline>(SelectObject);
				canLink=Point!=Line->StartPoint && Point!=Line->EndPoint;
			}
			if(SelectObject->GetType()==PointType)
			{
				TSharedPtr<FArmyPipePoint> Line=StaticCastSharedPtr<FArmyPipePoint>(SelectObject);
				canLink=Point!=Line;
			} 
		}
		if (canLink)
			TempPoints.Add(Point);
	}
	TMap<TWeakPtr<FArmyPipePoint>,FVector2D> TempMap;

	for(int32 i = 0 ;i<TempPoints.Num();i++)
	{
		FVector2D PointLocation2D;
		TSharedPtr<FArmyPipePoint> Point=TempPoints[i].Pin();
		GVC->WorldToPixel(Point->GetLocation(),PointLocation2D);
		TempMap.Add(TempPoints[i],PointLocation2D);
	}

	float TempLenght3D=10000.f;
	FVector TempPoint;
	bool bCapture = false;
	FVector MWorldStart,MWorldDir;
	InViewPortClient->DeprojectFVector2D(FVector2D(x,y),MWorldStart,MWorldDir);
	
	for (auto&Elem:TempMap)
	{
		TWeakPtr<FArmyPipePoint> Point = Elem.Key;
		FVector2D PointLocation = Elem.Value;
		FVector2D InDirection=PointLocation-FVector2D(x,y);
		FVector ScreenDirection = MWorldStart - Point.Pin()->GetLocation();
		if (InDirection.Size()<=CaputureLenght+10)
		{
			if (InDirection.Size() <= TempLenght3D)
			{
				bCapture = true;
				TempPoint = Point.Pin()->GetLocation();
			}
		}
	}
	if (bCapture)
	{
		MouseCapturePoint= TempPoint;
		return true;
	}
	return false;
}

bool FArmyMouseCaptureManager::CaptureLine (class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y)
{
	EObjectType PointType = ObjectType;
	if (!(ObjectType == OT_HotWaterTube || ObjectType == OT_ColdWaterTube || ObjectType == OT_Drain ||ObjectType == OT_Drain_Point))
		return false;
	TArray<TWeakPtr<FArmyObject>> InObjects;
	FArmySceneData::Get ()->GetObjects (E_HydropowerModel,PointType,InObjects);
	if (ObjectType == OT_Drain)
		FArmySceneData::Get ()->GetObjects (E_HydropowerModel,OT_Drain_Point,InObjects);
	if(InObjects.Num ()==0)
		return false;

	FVector TempPoint;
	float TempLenght3D=10000.f;
	bool bCapture=false;
	FVector MWorldStart,MWorldDir;
	InViewPortClient->DeprojectFVector2D(FVector2D(x,y),MWorldStart,MWorldDir);
	for(int32 i = 0;i<InObjects.Num ();i++)
	{
		TSharedPtr<FArmyPipeline> LineIt = StaticCastSharedPtr<FArmyPipeline> (InObjects[i].Pin ());
		FVector Start = LineIt->GetStart ();
		FVector End = LineIt->GetEnd ();
		if(FArmyMath::NearlyPoint (TempPoint,Start,End,MouseCapturePoint,CaputureLenght))
		{
			return true;
		}
			
	}
	return false;
}

bool FArmyMouseCaptureManager::CaptureAdsorptionPoint (class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y)
{

	if(!bEnable)
		return false;
	bool bCapture = false;
	for(TActorIterator<AXRFurnitureActor> ActorItr (GVC->GetWorld ()); ActorItr; ++ActorItr)
	{
		AXRFurnitureActor* Actor=*ActorItr;
		TSharedPtr<FArmyFurniture> ComponentItem=StaticCastSharedPtr<FArmyFurniture>(Actor->GetRelevanceObject().Pin());
		if(IsShowEditPoint(ComponentItem))
		{
			TMap<uint32,FVector> EditPoint3D,EditPoint2D;
			for(int32 i=0;i<Actor->Children.Num ();i++)
			{
				AActor* child=Actor->Children[i];
				if(child->Tags.Contains(FName("HydropowerActor"))||child->Tags.Contains(FName("OriginalPoint")))
				{
					ComponentItem->GetPointMapes(EditPoint3D,EditPoint2D);
				}
			}
			TMap<uint32,FVector> ShowPointes=bTopView?EditPoint2D:EditPoint3D;
			for(auto& Postion:ShowPointes)
			{
				FVector Location=Postion.Value;
				uint32 key = Postion.Key;
				FVector2D Adsortption;
				InViewPortClient->WorldToPixel(Location,Adsortption);
				if ( Adsortption.Equals(FVector2D(x,y),5.f ))
				{
					MouseCapturePoint = *EditPoint3D.Find(key);//Location;
					bCapture = true;
					break;
				}
			}
			if (bCapture)
			{
				break;
			}
		}

	};
	bAdsorptionPoint = bCapture;
	return bCapture;
}

bool FArmyMouseCaptureManager::CaptureWallInTopView (class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y)
{
	if(GXRPC->GetXRViewMode ()!=EXRView_TOP)
		return false;
	TArray<TWeakPtr<FArmyObject>> InObjects;
	FArmySceneData::Get ()->GetObjects (E_LayoutModel,OT_InternalRoom,InObjects);
	if(InObjects.Num ()==0)
		return false;
	for(int32 i = 0;i<InObjects.Num ();i++)
	{
		FObjectPtr object = InObjects[i].Pin ();
		if(object->GetType ()!=OT_InternalRoom)
			continue;
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom> (object);
		FBox bounds = Room->GetBounds3D ();
		bool bCaputure = false;
		if(FMath::PointBoxIntersection (MouseCapturePoint,bounds))
		{
			TArray< TSharedPtr<FArmyLine> > Lines;
			Room->GetLines (Lines);
			const float MaxDistance = 3.f;
			float MinDistance = MaxDistance;
			FVector SnapedPos = MouseCapturePoint;
			FVector TempPos = MouseCapturePoint;
			TSharedPtr<FArmyLine> SnapedLine;
			for(auto LineIt:Lines)
			{
				FVector P1 = LineIt->GetStart();
				FVector P0 = LineIt->GetEnd();
				FVector Projection = FArmyMath::GetProjectionPoint (TempPos,P0,P1);
				bool BCapture = FArmyMath::IsPointProjectionOnLineSegment2D (FVector2D (Projection),FVector2D (P0),FVector2D (P1));
				float Distance = FVector::Distance (TempPos,Projection);
				if(Distance>0.f && Distance<MaxDistance && Distance<MinDistance)
				{
					SnapedPos = Projection;
					SnapedLine = LineIt;
					MinDistance = Distance;
				}
			}
			if (SnapedLine.IsValid())
			{
				MouseCapturePoint = SnapedPos;
				MouseCaptureNormal = FArmyMath::GetLineDirection(TempPos, SnapedPos);// .GetSafeNormal();
				return true;
			}
		}
	}
	return false;
}

bool FArmyMouseCaptureManager::CaptureCoordinatePoint (class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y)
{
	if (!bEnableCaptureCoordinate)
		return false;
	ForwardVector = FVector::ForwardVector;
	RightVector = FVector::RightVector;
	UpVector = FVector::UpVector;

	bool bCapture = false;

	FVector OutLocation;
	if (GXRPC->GetXRViewMode()==EXRView_TOP)
	{
		if (CaptureXYCoordinatePoint(InViewPortClient,x,y,OutLocation))
		{
			MouseCapturePoint = OutLocation;
			bCapture = true;
		}
	}
	else
	{
		if (IsOnWall(StartPoint)&&!IsOnFloorOrTop(MouseCapturePoint))
		{
			RightVector = MouseCaptureNormal;
			ForwardVector = FVector::CrossProduct(RightVector,UpVector);
		}
		//优先捕捉z轴
		if (CaptureZCoordinatePoint(InViewPortClient, x, y, OutLocation))
			bCapture = true;
		else if (CaptureXCoordinatePoint(InViewPortClient,x,y,OutLocation))
			bCapture = true;
		else if (CaptureYCoordinatePoint(InViewPortClient,x,y,OutLocation))
			bCapture = true;
		if (bCapture)
		{
			bool bInRoom = InSameRoom(StartPoint,OutLocation);

			if (bInRoom)
			{
				MouseCapturePoint = OutLocation;
				return bCapture;
			}


			static FName TraceTag = FName(TEXT("PickPos"));
			FCollisionQueryParams TraceParams(TraceTag,false);
			TraceParams.bTraceAsyncScene = true;

			FVector Location = GVC->GetMouseTraceToWorldPos(StartPoint,MouseCapturePoint);
			bInRoom = InSameRoom(StartPoint,Location);
			if (!bInRoom)
				return false;
			if (!Location.IsNearlyZero())
				MouseCapturePoint = Location;
			if (MouseCapturePoint.Z<0.f)
				MouseCapturePoint.Z = 0;
		}
	}

	
	return bCapture;
}

bool FArmyMouseCaptureManager::CaptureXCoordinatePoint(class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y,FVector& OutLocation)
{

	FPlane XYBasePlane(StartPoint,UpVector);
	FPlane XZBasePlane(StartPoint,RightVector);

	FVector XZWorldPoint,XYWorldPoint;
	

	float Min = 0.98,Max = 1;
	{
		bool bXZ = GetPointPlane(InViewPortClient,XZBasePlane,x,y,XZWorldPoint);
		FVector WorldPoint = XZWorldPoint;
		FVector Direction = WorldPoint-StartPoint;
		FVector DNormal = Direction.GetSafeNormal();


		float XDot = FVector::DotProduct (DNormal,ForwardVector);
		if ((XDot>=Min && XDot<=Max)||(XDot<=-Min && XDot>=-Max))
		{
			OutLocation = FArmyMath::GetProjectionPoint(WorldPoint,StartPoint,StartPoint+ForwardVector);
			return true;
		}

	}
	{
		bool bXY = GetPointPlane(InViewPortClient,XYBasePlane,x,y,XYWorldPoint);
		FVector WorldPoint = XYWorldPoint;
		FVector Direction = WorldPoint-StartPoint;
		FVector DNormal = Direction.GetSafeNormal();
		float XDot = FVector::DotProduct (DNormal,ForwardVector);
		if ((XDot>=Min && XDot<=Max)||(XDot<=-Min && XDot>=-Max))
		{
			OutLocation = FArmyMath::GetProjectionPoint(WorldPoint,StartPoint,StartPoint+ForwardVector);
			return true;
		}
	}



	
	return false;


}

bool FArmyMouseCaptureManager::CaptureYCoordinatePoint(class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y,FVector& OutLocation)
{
	FPlane XYBasePlane(StartPoint,UpVector);
	FPlane YZBasePlane(StartPoint,ForwardVector);

	FVector YZWorldPoint,XYWorldPoint,WorldPoint;
	float Min = 0.98,Max = 1;
	{

		bool bXY = GetPointPlane(InViewPortClient,XYBasePlane,x,y,XYWorldPoint);
		WorldPoint = XYWorldPoint;
		FVector Direction = WorldPoint-StartPoint;
		FVector DNormal = Direction.GetSafeNormal ();

		float YDot = FVector::DotProduct (DNormal,RightVector);
		if ((YDot>=Min && YDot<=Max)||(YDot<=-Min && YDot>=-Max))
		{
			OutLocation = FArmyMath::GetProjectionPoint(WorldPoint,StartPoint,StartPoint+RightVector);
			return true;
		}
	}
	{
		bool bYZ = GetPointPlane(InViewPortClient,YZBasePlane,x,y,YZWorldPoint);
		WorldPoint = YZWorldPoint;
		FVector Direction = WorldPoint-StartPoint;
		FVector DNormal = Direction.GetSafeNormal ();

		float YDot = FVector::DotProduct (DNormal,RightVector);
		if ((YDot>=Min && YDot<=Max)||(YDot<=-Min && YDot>=-Max))
		{
			OutLocation = FArmyMath::GetProjectionPoint(WorldPoint,StartPoint,StartPoint+RightVector);
			return true;
		}
	}

	

	return false;
}

bool FArmyMouseCaptureManager::CaptureZCoordinatePoint(class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y,FVector& OutLocation)
{
	FPlane XZBasePlane(StartPoint,RightVector);
	FPlane YZBasePlane(StartPoint,ForwardVector);

	FVector XZWorldPoint;
	FVector YZWorldPoint;
	FVector CaptureLocation = MouseCapturePoint;
	bool bCapture = false;
	{
		bool bXZ = GetPointPlane(InViewPortClient,XZBasePlane,x,y,XZWorldPoint);
		if (bXZ)
		{
			FVector TempInLinePos = FMath::ClosestPointOnInfiniteLine(StartPoint, StartPoint + FVector::UpVector, XZWorldPoint);
			FVector OnWallPoint = TempInLinePos;
			OnWallPoint.Z = MouseCapturePoint.Z;
			FVector2D InLinePix;
			InViewPortClient->WorldToPixel(OnWallPoint, InLinePix);
			if ((InLinePix - FVector2D(x, y)).Size() < 10)
			{
				CaptureLocation = OnWallPoint;
				bCapture = true;
			}
			else
			{
				InViewPortClient->WorldToPixel(TempInLinePos, InLinePix);
				if ((InLinePix - FVector2D(x, y)).Size() < 10)
				{
					CaptureLocation = TempInLinePos;
					bCapture = true;
				}
			}
		}
	}
	if (!bCapture)
	{
		CaptureLocation = MouseCapturePoint;
		bool bYZ=GetPointPlane(InViewPortClient,YZBasePlane,x,y,YZWorldPoint);
		if (bYZ)
		{
			FVector TempInLinePos = FMath::ClosestPointOnInfiniteLine(StartPoint, StartPoint + FVector::UpVector, YZWorldPoint);
			FVector2D InLinePix;
			InViewPortClient->WorldToPixel(TempInLinePos, InLinePix);
			if ((InLinePix - FVector2D(x, y)).Size() < 10)
			{
				CaptureLocation = TempInLinePos;
				bCapture = true;
			}
		}
	}

	if (bCapture)
	{
		OutLocation = CaptureLocation;
		return true;
	}
	return false;
}

bool FArmyMouseCaptureManager::CaptureXYCoordinatePoint(class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y,FVector& OutLocation)
{
	FVector TempStartPoint = StartPoint;
	TempStartPoint.Z = 0;
	FVector CurrentPoint = MouseCapturePoint;
	CurrentPoint.Z = 0 ;
	FVector Direction = CurrentPoint-TempStartPoint;
	FVector DNormal = Direction.GetSafeNormal ();

	float Min = 0.98,Max = 1;

	float ZDot = FVector::DotProduct (DNormal,FVector::UpVector);
	if ((ZDot>=Min && ZDot<=Max)||(ZDot<=-Min && ZDot>=-Max))
	{
		OutLocation = FArmyMath::GetProjectionPoint (CurrentPoint,TempStartPoint,TempStartPoint+FVector::UpVector);
		return true;
	}


	float YDot = FVector::DotProduct (DNormal,FVector::RightVector);
	if ((YDot>=Min && YDot<=Max)||(YDot<=-Min && YDot>=-Max))
	{
		OutLocation = FArmyMath::GetProjectionPoint (CurrentPoint,TempStartPoint,TempStartPoint+FVector::RightVector);
		return true;
	}



	float XDot = FVector::DotProduct (DNormal,FVector::ForwardVector);
	if ((XDot>=Min && XDot<=Max)||(XDot<=-Min && XDot>=-Max))
	{
		OutLocation = FArmyMath::GetProjectionPoint (CurrentPoint,TempStartPoint,TempStartPoint+FVector::ForwardVector);
		return true;
	}
	MouseCapturePoint = CurrentPoint;
	MouseCapturePoint.Z = StartPoint.Z;
	return false;
}

bool FArmyMouseCaptureManager::CaptureAngle45Coordinate(class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y)
{
	if (!bEnableCaptureCoordinate)
		return false;
	if (IsOnFloorOrTop(StartPoint)||IsOnWall(StartPoint))
	{
		FVector WallDirection = GetCaptureWallDirection(StartPoint);
		FVector WallNormal = GetCaptureNormal(StartPoint);
		FVector XDir = WallDirection;
		FVector YDir = FVector::CrossProduct(WallDirection,WallNormal).GetSafeNormal();
		FVector MouseDirection = MouseCapturePoint - StartPoint;

		FVector MDir = ((XDir+YDir)/2).GetSafeNormal();
		FVector NMDir = ((YDir-XDir)/2).GetSafeNormal();
		{

			FVector NPoint = FArmyMath::GetProjectionPoint(MouseCapturePoint,StartPoint,StartPoint+MDir);
			FVector NMPoint = FArmyMath::GetProjectionPoint(MouseCapturePoint,StartPoint,StartPoint+NMDir);

			FVector NDirection = (NPoint-MouseCapturePoint);

			float Lenght = MouseDirection.Size() * 0.15f ;//< 40 ? 20*MouseDirection.Size()/40.f : 20;
			if (NDirection.Size()<CaputureLenght+Lenght)
			{
				MouseCapturePoint = NPoint;
				return true;
			}

			FVector NMDirection = (NMPoint-MouseCapturePoint);
			if (NMDirection.Size()<CaputureLenght+Lenght)
			{
				MouseCapturePoint = NMPoint;
				return true;
			}

		}

		
		return false;
	}
	return false;
}

bool FArmyMouseCaptureManager::CanCaptureCoordinatePoint (class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y)
{
	TArray<TWeakPtr<FArmyObject>> InObjects;
	FArmySceneData::Get ()->GetObjects (E_LayoutModel,OT_InternalRoom,InObjects);
	if(InObjects.Num ()==0)
		return false;
	for(int32 i = 0;i<InObjects.Num ();i++)
	{
		FObjectPtr object = InObjects[i].Pin ();
		if(object->GetType ()!=OT_InternalRoom)
			continue;
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom> (object);
		FBox bounds = Room->GetBounds3D ();
		bool bCaputure = false;
		if(FMath::PointBoxIntersection (MouseCapturePoint,bounds))
		{
			FPoly;
			TArray< TSharedPtr<FArmyLine> > Lines;
			Room->GetLines (Lines);
			for(auto LineIt:Lines)
			{
				FVector Start = LineIt->GetStart ();
				FVector End = LineIt->GetEnd ();
				FVector WStart = Start+FVector (0,0, FArmySceneData::WallHeight);
				FVector WEnd = End+FVector (0,0, FArmySceneData::WallHeight);
				FVector CaputurePoint;
				if(FArmyMath::NearlyPoint (MouseCapturePoint,Start,End,MouseCapturePoint,CaputureLenght))
				{
					bCaputure = true;
					break;
				}
				if(FArmyMath::NearlyPoint (MouseCapturePoint,WStart,WEnd,MouseCapturePoint,CaputureLenght))
				{
					bCaputure = true;
					break;
				}
			}
		}
		if(bCaputure)
			return true;
	}
	return true;
}

bool FArmyMouseCaptureManager::GetPointPlane(class UArmyEditorViewportClient* InViewPortClient,FPlane& BasePlane,int32 x,int32 y,FVector& OutPoint)
{
	FVector MWorldStart,MWorldDir;
	InViewPortClient->DeprojectFVector2D(FVector2D(x,y),MWorldStart,MWorldDir);
	FVector Start = MWorldStart;
	FVector End = MouseCapturePoint;// MWorldStart+MWorldDir * ((InViewPortClient->GetViewLocation()-FVector::PointPlaneProject(InViewPortClient->GetViewLocation(),BasePlane)).Size()+10);
	{
		FVector TempPoint=FMath::LinePlaneIntersection(Start,End,BasePlane);
		if (OutPoint.ContainsNaN())
			return false;

		OutPoint = TempPoint;
		return true;
	}
	return false;
}

void FArmyMouseCaptureManager::SetObjectType (EObjectType InObjectType)
{
	ObjectType = InObjectType;	
}

void FArmyMouseCaptureManager::SetEnable (bool InEnable)
{
	bEnable = InEnable;
}

void FArmyMouseCaptureManager::GetFurnitures (TArray<TSharedPtr< class FArmyFurniture> > & FunitureObjects)
{
	FunitureObjects.Empty ();
	TArray<FObjectWeakPtr> Objects;
	FArmySceneData::Get ()->GetObjects (E_HydropowerModel,OT_ComponentBase,Objects);
	switch(ObjectType)
	{
		case OT_StrongElectricity_25:
		case OT_StrongElectricity_4:
		case OT_StrongElectricity_Single:
		case OT_StrongElectricity_Double:
		{
			for(int32 i = 0;i<Objects.Num ();i++)
			{
				FObjectWeakPtr Object = Objects[i];
				TSharedPtr<FArmyFurniture> Item = StaticCastSharedPtr<FArmyFurniture> (Object.Pin ());
				if(Item->IsStrongElectirc ())
					FunitureObjects.Add (Item);
			}
		}
		break;
		case OT_WeakElectricity_TV:
		case OT_WeakElectricity_Net:
		case OT_WeakElectricity_Phone:
		{
			for(int32 i = 0;i<Objects.Num ();i++)
			{
				FObjectWeakPtr Object = Objects[i];
				TSharedPtr<FArmyFurniture> Item = StaticCastSharedPtr<FArmyFurniture> (Object.Pin ());
				if(Item->IsWeakElectirc ())
					FunitureObjects.Add (Item);
			}
		}
		break;
		case OT_ColdWaterTube:
		{
			for(int32 i = 0;i<Objects.Num ();i++)
			{
				FObjectWeakPtr Object = Objects[i];
				TSharedPtr<FArmyFurniture> Item = StaticCastSharedPtr<FArmyFurniture> (Object.Pin ());
				if(Item->IsColdWater ())
					FunitureObjects.Add (Item);
			}
		}
		break;
		case OT_HotWaterTube:
		{
			for(int32 i = 0;i<Objects.Num ();i++)
			{
				FObjectWeakPtr Object = Objects[i];
				TSharedPtr<FArmyFurniture> Item = StaticCastSharedPtr<FArmyFurniture> (Object.Pin ());
				if(Item->IsHotWater ())
					FunitureObjects.Add (Item);
			}
		}
		break;
		case OT_Drain:
		{
			for(int32 i = 0;i<Objects.Num ();i++)
			{
				FObjectWeakPtr Object = Objects[i];
				TSharedPtr<FArmyFurniture> Item = StaticCastSharedPtr<FArmyFurniture> (Object.Pin ());
				if(Item->IsDrain ())
					FunitureObjects.Add (Item);
			}
		}
		break;
		default:
			break;
	}
}

bool FArmyMouseCaptureManager::IsShowEditPoint (TSharedPtr< class FArmyFurniture> Item)
{
	if (!Item.IsValid())
		return false;
	switch(ObjectType)
	{
		case OT_StrongElectricity_25:
		case OT_StrongElectricity_4:
		case OT_StrongElectricity_Single:
		case OT_StrongElectricity_Double:
		{
			return Item->IsStrongElectirc();
		}
		break;
		case OT_WeakElectricity_TV:
		case OT_WeakElectricity_Net:
		case OT_WeakElectricity_Phone:
		{
			return Item->IsWeakElectirc();
		}
		break;
		case OT_ColdWaterTube:
		{
			switch(Item->ComponentType)
			{
				case EC_Water_Supply: // 给水点位,
					return true;
				default:
					break;
			}
			return Item->IsColdWater();
		}
		break;
		case OT_HotWaterTube:
		{
			switch(Item->ComponentType)
			{
				case EC_Water_Hot_Point: //排水点位
					return true;
				default:
					break;
			}
			return Item->IsHotWater();
		}
		break;
		case OT_Drain:
		{
			switch(Item->ComponentType)
			{
				case EC_Drain_Point:
				case EC_Basin:
				case EC_Water_Basin:
				case EC_Closestool:
					return true;
				default:
					break;
			}
			return Item->IsDrain();
		}
		break;
		default:
			return false;
			break;
	}
}

bool FArmyMouseCaptureManager::InSameRoom(FVector& InStartPoint,FVector InEndPoint)
{
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get ()->GetObjects (E_LayoutModel,OT_InternalRoom,RoomList);
	if (RoomList.Num ()==0)
		return false;
	TSharedPtr<FArmyRoom> StartRoom = nullptr;
	for (int32 i = 0;i<RoomList.Num ();i++)
	{
		FObjectPtr object = RoomList[i].Pin ();
		if (object->GetType ()!=OT_InternalRoom)
			continue;
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(object);
		FBox bounds = Room->GetBounds3D ();
		if (Room->IsPointInRoom(InStartPoint))
			StartRoom = Room;
		if (Room->IsPointInRoom(InEndPoint))
			return StartRoom == Room;
	}
	return false;
}

FArmyMouseCaptureManager* FArmyMouseCaptureManager::Get ()
{
	if(Singleton == nullptr)
	{
		check (IsInGameThread ());
		/*TSharedPtr<FArmyMouseCaptureManager> New = MakeShareable(new FArmyMouseCaptureManager);
		Singleton =  New.Get();*/
		Singleton = new FArmyMouseCaptureManager;
	}
	//check (Singleton != nullptr);

	return Singleton;
}

void FArmyMouseCaptureManager::SetStartPoint (FVector InStartPoint,bool InEnableCaptureCoordinate /*= false*/)
{
	StartPoint=InStartPoint;
	bEnableCaptureCoordinate=InEnableCaptureCoordinate;
}


bool FArmyMouseCaptureManager::IsOnWall(FVector& InPoint)
{
	bool bOnWall = false;
	FVector InMousePoint2D = FVector(InPoint.X, InPoint.Y, 0);
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);
	if (RoomList.Num() == 0)
		return bOnWall;
	for (int32 i = 0; i<RoomList.Num(); i++)
	{
		FObjectPtr object = RoomList[i].Pin();
		if (object->GetType() != OT_InternalRoom)
			continue;
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(object);
		TArray<TSharedPtr<FArmyLine>> OutLines;
		Room->GetLines(OutLines);
		if (OutLines.Num())
		{
			for (auto& Line : OutLines)
			{
				float Distance = FArmyMath::Distance(InMousePoint2D, Line->GetStart(), Line->GetEnd());
				bOnWall = (Distance <= 2.0f);
				if (bOnWall)
				{
					return bOnWall;
				}

			}
		}

	}
	return false;
}

bool FArmyMouseCaptureManager::IsOnWall(FVector& InPoint, FVector&WallDirection , FVector&WallNormal )
{
	bool bOnWall = false;
	FVector InMousePoint2D = FVector(InPoint.X, InPoint.Y,0);
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get ()->GetObjects (E_LayoutModel,OT_InternalRoom,RoomList);
	if (RoomList.Num ()==0)
		return bOnWall;
	for (int32 i = 0;i<RoomList.Num ();i++)
	{
		FObjectPtr object = RoomList[i].Pin ();
		if (object->GetType ()!=OT_InternalRoom)
			continue;
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(object);
		TArray<TSharedPtr<FArmyLine>> OutLines;
		Room->GetLines(OutLines);
		if (OutLines.Num())
		{
			for (auto& Line:OutLines)
			{
				float Distance = FArmyMath::Distance(InMousePoint2D,Line->GetStart(),Line->GetEnd());
				bOnWall = (Distance<=2.0f);
				if (bOnWall)
				{

					WallDirection = -Line->GetStart() + Line->GetEnd();
					WallDirection.Normalize();
					WallNormal = WallDirection ^ FVector::UpVector;
					return bOnWall;
				}

			}
		}

	}
	return false;
}

bool FArmyMouseCaptureManager::IsOnFloor(FVector& InPoint)
{
	return FMath::IsNearlyZero(InPoint.Z,0.3f);
}

bool FArmyMouseCaptureManager::IsOnTop(FVector& InPoint)
{
	return FMath::IsNearlyEqual(InPoint.Z, FArmySceneData::WallHeight,0.3f);
}

bool FArmyMouseCaptureManager::IsOnFloorOrTop(FVector& InPoint)
{
	return IsOnFloor(InPoint)||IsOnTop(InPoint);
}

bool FArmyMouseCaptureManager::IsOnCorner(FVector & InPoint)
{
	float MousePointZ = MouseCapturePoint.Z;
	FVector TempMousePoint = MouseCapturePoint;
	TempMousePoint.Z = 0;
	TArray<TWeakPtr<FArmyObject>> InObjects;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, InObjects);
	if (InObjects.Num() == 0)
		return false;

	CurrentInfo.bAdsorbDetect = false;
	CurrentInfo.bCornerDetect = false;
	for (int32 i = 0; i<InObjects.Num(); i++)
	{
		FObjectPtr object = InObjects[i].Pin();
		if (object->GetType() != OT_InternalRoom)
			continue;
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(object);
		FBox bounds = Room->GetBounds3D();
		bool bCaputure = false;
		if (FMath::PointBoxIntersection(TempMousePoint, bounds))
		{
			TArray< TSharedPtr<FArmyLine> > Lines;
			Room->GetLines(Lines);



			for (auto LineIt : Lines)
			{
				FVector Start = LineIt->GetStart();
				FVector End = LineIt->GetEnd();
				FVector WStart = Start + FVector(0, 0, FArmySceneData::WallHeight);
				FVector WEnd = End + FVector(0, 0, FArmySceneData::WallHeight);
				FVector CaputurePoint;
				if (FArmyMath::NearlyPoint(TempMousePoint, Start, End, CaputurePoint, CaputureLenght))
				{
					if (!CurrentInfo.bAdsorbDetect)
					{
						bCaputure = true;
						TempMousePoint = CaputurePoint;

						CurrentInfo.bAdsorbDetect = true;
						CurrentInfo.WallDirection = End - Start;
						CurrentInfo.WallDirection.Normalize();
						CurrentInfo.WallNormal = CurrentInfo.WallDirection ^FVector::UpVector;
					}
					else
					{
						CurrentInfo.bCornerDetect = true;
						CurrentInfo.WallDirection2 = End - Start;
						CurrentInfo.WallDirection2.Normalize();
						CurrentInfo.WallNormal2 = CurrentInfo.WallDirection2 ^FVector::UpVector;
					}

					continue;
					//break;
				}
				if (FArmyMath::NearlyPoint(TempMousePoint, WStart, WEnd, CaputurePoint, CaputureLenght))
				{
					if (!CurrentInfo.bAdsorbDetect)
					{
						bCaputure = true;
						TempMousePoint = CaputurePoint;

						CurrentInfo.bAdsorbDetect = true;
						CurrentInfo.WallDirection = End - Start;
						CurrentInfo.WallDirection.Normalize();
						CurrentInfo.WallNormal = CurrentInfo.WallDirection ^FVector::UpVector;
					}
					else
					{
						CurrentInfo.bCornerDetect = true;
						CurrentInfo.WallDirection2 = End - Start;
						CurrentInfo.WallDirection2.Normalize();
						CurrentInfo.WallNormal2 = CurrentInfo.WallDirection2 ^FVector::UpVector;
					}
					//break;
				}
			}
		}


		if (bCaputure)
		{
			MouseCapturePoint = TempMousePoint;
			MouseCapturePoint.Z = MousePointZ;
			CurrentInfo.AdsPoint = MouseCapturePoint;
			return true;
		}

	}
	return false;
}
