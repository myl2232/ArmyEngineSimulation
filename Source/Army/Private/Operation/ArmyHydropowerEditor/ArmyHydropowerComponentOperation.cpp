#include "ArmyHydropowerComponentOperation.h"
#include "ArmyDxfModel.h"
#include "dxfFile.h"
#include "SceneManagement.h"
#include "Engine.h"
#include "CoreMinimal.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ArmyCommonTypes.h"
#include "ArmyFurnitureActor.h"
#include "ArmyMouseCaptureManager.h"

FArmyHydropowerComponentOperation::FArmyHydropowerComponentOperation(EModelType InBelongModel)
	:FArmyOperation (InBelongModel)
	,MousePosition (FVector::ZeroVector)
	,IsAdded(false)
	,CurrentActor(nullptr)
{
}

FArmyHydropowerComponentOperation::~FArmyHydropowerComponentOperation()
{
}

void FArmyHydropowerComponentOperation::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	
}

void FArmyHydropowerComponentOperation::Reset()
{
	if (!IsAdded && SelectedComponent.IsValid())
		SelectedComponent->Destroy();
}

void FArmyHydropowerComponentOperation::Tick()
{
	return;
}

bool FArmyHydropowerComponentOperation::InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event)
{
	//if (Event==IE_Pressed)
	{
		if (Key==EKeys::LeftMouseButton)
		{
			return false; 
		}
	}
	return false;
}

void FArmyHydropowerComponentOperation::BeginOperation (XRArgument InArg)
{
}

void FArmyHydropowerComponentOperation::EndOperation()
{
	Reset();
}

void FArmyHydropowerComponentOperation::SetCurrentItem (TSharedPtr<FContentItemSpace::FContentItem> item)
{
	ContentItem = item;
}

void FArmyHydropowerComponentOperation::SetCurrentActor (class AXRFurnitureActor* InCurActor)
{
	CurrentActor = InCurActor;
}
