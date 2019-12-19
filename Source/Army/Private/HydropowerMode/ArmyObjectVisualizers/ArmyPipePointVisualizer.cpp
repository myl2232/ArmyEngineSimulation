// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ArmyPipePointVisualizer.h"
#include "ArmyPipeline.h"
#include "ArmyPipelineActor.h"
#include "EngineUtils.h"

FArmyPipePointVisualizer::FArmyPipePointVisualizer ()
	:FArmyObjectVisualizer()
{

}

FArmyPipePointVisualizer::~FArmyPipePointVisualizer ()
{

}

void FArmyPipePointVisualizer::OnRegister ()
{

}

void FArmyPipePointVisualizer::DrawVisualization (const FArmyObject* Component,const FSceneView* View,FPrimitiveDrawInterface* PDI)
{
	FArmyPipePoint* Point = (FArmyPipePoint*)Component;
	if (Point== nullptr)
		return;
	OwningObject = Component->AsShared();
	PDI->SetHitProxy (NULL);
	if (Point->GetPPType() != EPPT_BridgeBend)
	{
		TArray<TWeakPtr<FArmyPipeline>> lines;
		Point->GetPipeLines(lines);
		for(TWeakPtr<FArmyPipeline> line:lines)
		{
			TSharedPtr<FArmyPipeline> linePtr = line.Pin ();
			if (linePtr.IsValid())
				DrawDashedLine(PDI,linePtr->GetStart(),linePtr->GetEnd(),FLinearColor::Blue,10,SDPG_Foreground);
		}
	}
	
}


bool FArmyPipePointVisualizer::VisProxyHandleClick (UArmyEditorViewportClient* InViewportClient,HXRObjectVisProxy* VisProxy,const FArmyViewportClick& Click)
{
	return false;
}

bool FArmyPipePointVisualizer::ProxyHandleClick (UArmyEditorViewportClient* InViewportClient,class HHitProxy* ActorHit,const FArmyViewportClick& Click)
{
	if(ActorHit && ActorHit->IsA (HActor::StaticGetType ()))
	{
		HActor* hActor = static_cast<HActor*>(ActorHit);

		AXRActor* hitActor = Cast<AXRActor> (hActor->Actor);
		if(hitActor && hitActor->IsValidLowLevel ())
		{
			TWeakPtr <const FArmyObject> OldOwingObject = OwningObject;
			OwningObject = hitActor->GetRelevanceObject ();
			return true;
		}
	}
	return false;
}

bool FArmyPipePointVisualizer::OnLevelSelectionChanged(UObject* Obj)
{
	AXRActor * Actor=Cast<AXRActor>(Obj);
	if(Actor && Actor->IsValidLowLevel())
	{
		const FArmyObject* ClickedComponent=Actor->GetRelevanceObject().Pin().Get();
		if(ClickedComponent!=NULL)
		{
			TWeakPtr <const FArmyObject> OldOwingObject=OwningObject;
			OwningObject=Actor->GetRelevanceObject ();
			return true;
		}
	}
	return false;
}

bool FArmyPipePointVisualizer::GetWidgetLocation (const UArmyEditorViewportClient* ViewportClient,FVector& OutLocation) const
{
	if(OwningObject.IsValid ())
	{
		FArmyPipePoint* Point = (FArmyPipePoint*)(OwningObject.Pin ().Get ());
		OutLocation = Point->GetLocation();
	}
	
	return true;
}

bool FArmyPipePointVisualizer::HandleInputDelta (UArmyEditorViewportClient* ViewportClient,FViewport* Viewport,FVector& DeltaTranslate,FRotator& DeltalRotate,FVector& DeltaScale)
{
	return false;
}

bool FArmyPipePointVisualizer::HandleInputLocation(UArmyEditorViewportClient* ViewportClient,FViewport* Viewport,FVector& Location)
{
	return false;
}
