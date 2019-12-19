#include "ArmyNewDoorHole.h"
#include "ArmyPolyline.h"
#include "ArmyRect.h"
#include "ArmyShapeBoardActor.h"
#include "ArmyStyle.h"
#include "ArmyEngineModule.h"
#include "ArmySingleDoor.h"
#include "ArmySlidingDoor.h"
#include "ArmyExtrusion/Public/XRExtrusionActor.h"
#include "ArmyViewportClient.h"
#include "ArmyMath.h"
#include "ArmyGameInstance.h"

FArmyNewDoorHole::FArmyNewDoorHole()
	: FArmyHardware()
{
}

FArmyNewDoorHole::FArmyNewDoorHole(FArmyNewDoorHole * Copy)
{
}

FArmyNewDoorHole::~FArmyNewDoorHole()
{
}

void FArmyNewDoorHole::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
}

void FArmyNewDoorHole::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
}

void FArmyNewDoorHole::SetState(EObjectState InState)
{
}

void FArmyNewDoorHole::Draw(FPrimitiveDrawInterface * PDI, const FSceneView * View)
{
}

bool FArmyNewDoorHole::IsSelected(const FVector & Pos, UArmyEditorViewportClient * InViewportClient)
{
	return false;
}

bool FArmyNewDoorHole::Hover(const FVector & Pos, UArmyEditorViewportClient * InViewportClient)
{
	return false;
}

const FBox FArmyNewDoorHole::GetBounds()
{
	return FBox();
}

void FArmyNewDoorHole::Destroy()
{
}

void FArmyNewDoorHole::Update()
{
}

void FArmyNewDoorHole::Generate(UWorld * InWorld)
{
}

TArray<FVector> FArmyNewDoorHole::GetClipingBox()
{
	return TArray<FVector>();
}

void FArmyNewDoorHole::UpdatePostWall()
{
}

void FArmyNewDoorHole::SetThickness(const float InThickness)
{
}

void FArmyNewDoorHole::SetOuterWidth(const float InOuterWidth)
{
}

const void FArmyNewDoorHole::GetAlonePoints(TArray<TSharedPtr<FArmyPoint>>& OutPoints)
{
	return void();
}

void FArmyNewDoorHole::SetIfGeneratePassModel(bool bGenerate)
{
}
