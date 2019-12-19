#pragma once
#include "GameFramework/Actor.h"
#include "ArmyShapeFrameActor.generated.h"

UCLASS()
class AXRShapeFrame : public AActor
{
    GENERATED_BODY()

public:
    AXRShapeFrame();
    void RefreshFrame(float InWidth, float InDepth, float InHeight);
	void RefreshVer();
    struct FShapeInRoom *ShapeInRoomRef = nullptr;

private:
    class UPNTPrimitive *FramePrimitive;
};