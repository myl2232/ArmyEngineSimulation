// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ArmyPipelineVisualizer.h"
#include "ArmyPipeline.h"
#include "ArmyPipelineActor.h"
//#include "ArmyHydropowerEditor.h"
#include "EngineUtils.h"

IMPLEMENT_HIT_PROXY (HPipelineKeyProxy,HXRObjectVisProxy);

FArmyPipelineVisualizer::FArmyPipelineVisualizer ()
	:FArmyObjectVisualizer()
	,KeyIndexSelected (INDEX_NONE)
{

}

FArmyPipelineVisualizer::~FArmyPipelineVisualizer ()
{

}

void FArmyPipelineVisualizer::OnRegister ()
{

}

void FArmyPipelineVisualizer::DrawVisualization (const FArmyObject* Component,const FSceneView* View,FPrimitiveDrawInterface* PDI)
{
	FArmyPipeline* Pipeline = (FArmyPipeline*)Component;
	if (Pipeline == nullptr)
		return;
	TSharedPtr<FArmyPipePoint> Start= Pipeline->StartPoint;
	TSharedPtr<FArmyPipePoint> End = Pipeline->EndPoint;
	PDI->SetHitProxy (NULL);
	TArray<TWeakPtr<FArmyPipeline>> lines;
	Start->GetPipeLines(lines);
	End->GetPipeLines(lines);

	for (TWeakPtr<FArmyPipeline> line : lines)
	{
		TSharedPtr<FArmyPipeline> linePtr = line.Pin();
		if (linePtr.IsValid())
			DrawDashedLine(PDI,linePtr->GetStart(),linePtr->GetEnd(),FLinearColor::Blue,10,SDPG_Foreground);
	}
	//if (Start->GetPPType() == EPPT_None ||Start->GetPPType () == EPPT_DirectE)
	{ 
		PDI->SetHitProxy (new HPipelineKeyProxy (Component,0));
		PDI->DrawPoint(Start->Location,FLinearColor::Red,20,SDPG_MAX);

	}
	//if(End->GetPPType ()==EPPT_None||Start->GetPPType ()==EPPT_DirectE)
	{
		PDI->SetHitProxy (new HPipelineKeyProxy (Component,1));
		PDI->DrawPoint(End->Location,FLinearColor::Red,20,SDPG_MAX);
	}
	PDI->SetHitProxy (NULL);
}

bool FArmyPipelineVisualizer::VisProxyHandleClick (UArmyEditorViewportClient* InViewportClient,HXRObjectVisProxy* VisProxy,const FArmyViewportClick& Click)
{
	if (VisProxy &&VisProxy->Component.IsValid())
	{
		TWeakPtr <const FArmyObject> OldOwingObject = OwningObject;
		OwningObject = VisProxy->Component;
		if (VisProxy->IsA(HPipelineKeyProxy::StaticGetType()))
		{
			HPipelineKeyProxy* KeyProxy = (HPipelineKeyProxy*)VisProxy;
			if(OldOwingObject!=OwningObject)
			{
				KeyIndexSelected = INDEX_NONE;
			}
			KeyIndexSelected = KeyProxy->KeyIndex;
		}
		
		return true;
	}

	EndEditing();
	return false;
}

bool FArmyPipelineVisualizer::ProxyHandleClick (UArmyEditorViewportClient* InViewportClient,HHitProxy* ActorHit,const FArmyViewportClick& Click)
{
	if(ActorHit && ActorHit->IsA(HActor::StaticGetType()))
	{
		HActor* hActor = static_cast<HActor*>(ActorHit);

		AXRActor* hitActor = Cast<AXRActor>(hActor->Actor);
		if (hitActor && hitActor->IsValidLowLevel())
		{
			TWeakPtr <const FArmyObject> OldOwingObject = OwningObject;
			OwningObject = hitActor->GetRelevanceObject();
			KeyIndexSelected = INDEX_NONE;
			return true;
		}
		

	}
	return false;
}

bool FArmyPipelineVisualizer::OnLevelSelectionChanged(UObject* Obj)
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

void FArmyPipelineVisualizer::EndEditing ()
{
	KeyIndexSelected = INDEX_NONE;
}

bool FArmyPipelineVisualizer::GetWidgetLocation (const UArmyEditorViewportClient* ViewportClient,FVector& OutLocation) const
{
	FArmyPipeline* PipeLine =  (FArmyPipeline*)(OwningObject.Pin().Get());
	if (PipeLine)
	{
		switch(KeyIndexSelected)
		{
			case 0:
				OutLocation = PipeLine->GetStart(); 
				break;
			case 1:
				OutLocation = PipeLine->GetEnd();
				break;
			default:
				OutLocation = (PipeLine->GetStart () + PipeLine->GetEnd() )/2;
				break;
		}

	}
	
	return true;
}

bool FArmyPipelineVisualizer::HandleInputDelta (UArmyEditorViewportClient* ViewportClient,FViewport* Viewport,FVector& DeltaTranslate,FRotator& DeltalRotate,FVector& DeltaScale)
{
	if (OwningObject.IsValid())
	{
		FArmyPipeline* PipeLine = (FArmyPipeline*)(OwningObject.Pin ().Get ());
		bool bEnable = PipeLine->GetType()!=OT_ColdWaterTube && PipeLine->GetType() != OT_HotWaterTube&& PipeLine->GetType() != OT_Drain;
		if(!bEnable)
			bEnable = KeyIndexSelected == 0 ? PipeLine->StartPoint->GetPPType() == EPPT_None : PipeLine->EndPoint->GetPPType ()==EPPT_None;
		
		{
			switch(KeyIndexSelected)
			{
				case 0:
					PipeLine->StartPoint->Location += DeltaTranslate;
					break;
				case 1:
					PipeLine->EndPoint->Location += DeltaTranslate;
					break;
				default:
				{
					PipeLine->StartPoint->Location += DeltaTranslate;
					PipeLine->EndPoint->Location += DeltaTranslate;
				}
				break;
			}
			if (DeltaTranslate.Size() > 0)
			{
				PipeLine->IsChanged = true;
			}
		}
		
		return true;
	}
	return false;
}

bool FArmyPipelineVisualizer::HandleInputLocation(UArmyEditorViewportClient* ViewportClient,FViewport* Viewport,FVector& Location)
{
	if(OwningObject.IsValid())
	{
		FArmyPipeline* PipeLine=(FArmyPipeline*)(OwningObject.Pin ().Get ());
		bool bEnable=PipeLine->GetType()!=OT_ColdWaterTube && PipeLine->GetType()!=OT_HotWaterTube&& PipeLine->GetType()!=OT_Drain;
		if(!bEnable)
			bEnable=KeyIndexSelected==0?PipeLine->StartPoint->GetPPType()==EPPT_None:PipeLine->EndPoint->GetPPType ()==EPPT_None;

		{
			switch(KeyIndexSelected)
			{
				case 0:
					PipeLine->StartPoint->Location = Location;
					break;
				case 1:
					PipeLine->EndPoint->Location=Location;
					break;
				default:
				{
					FVector OutLocation = (PipeLine->GetStart () + PipeLine->GetEnd() )/2;
					FVector DeltaTranslate = Location - OutLocation; 
					PipeLine->StartPoint->Location+=DeltaTranslate;
					PipeLine->EndPoint->Location+=DeltaTranslate;
				}
				break;
			}
			PipeLine->IsChanged = true;
		}

		return true;
	}
	return false;
}
