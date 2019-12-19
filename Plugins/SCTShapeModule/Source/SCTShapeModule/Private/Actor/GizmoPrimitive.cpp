#include "GizmoPrimitive.h"
#include "Engine/CollisionProfile.h"

class FGizmoSceneProxy : public FPrimitiveSceneProxy
{
public:
    FGizmoSceneProxy(const UPrimitiveComponent* InComponent)
        : FPrimitiveSceneProxy(InComponent)
    {}

    virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, 
        const FSceneViewFamily& ViewFamily, 
        uint32 VisibilityMap, 
        class FMeshElementCollector& Collector) const override
    {
        // auto WireframeMaterial = new FColoredMaterialRenderProxy(
		// 	GEngine->LevelColorationUnlitMaterial->GetRenderProxy(IsSelected(), IsHovered()),
		// 	FLinearColor::Red)
		// );
        // Collector.RegisterOneFrameMaterialProxy(WireframeMaterial);

        for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
        {
            if (VisibilityMap & (1 << ViewIndex))
            {
                const FSceneView* View = Views[ViewIndex];
                FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

                const FMatrix& LocalToWorld = GetLocalToWorld();

                PDI->DrawLine(FVector(1.0f, 1.0f, 20.0f), 
                    FVector(7.0f, 1.0f, 20.0f), 
                    FLinearColor::Blue, SDPG_Foreground, 
                    1.0f, 
                    0.0f,
                    false);
            }
        }
    }

    virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
    {
        FPrimitiveViewRelevance Result;
        Result.bDrawRelevance = IsShown(View);
        //Result.bDrawRelevance = bDrawDebug && !IsSelected() && IsShown(View) && View->Family->EngineShowFlags.Splines;
        Result.bDynamicRelevance = true;
        //Result.bShadowRelevance = IsShadowCast(View);
        Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
        return Result;
    }

    virtual bool CanBeOccluded() const override
	{
		return false;
	}

	virtual uint32 GetMemoryFootprint() const override
	{
		return sizeof(*this) + FPrimitiveSceneProxy::GetAllocatedSize();
	}
};

UGizmoPrimitive::UGizmoPrimitive(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
    bUseEditorCompositing = 1;

	PrimaryComponentTick.bCanEverTick = false;
	SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
}

FPrimitiveSceneProxy* UGizmoPrimitive::CreateSceneProxy()
{
    return new FGizmoSceneProxy(this);
}

FBoxSphereBounds UGizmoPrimitive::CalcBounds(const FTransform& LocalToWorld) const
{
    return FBoxSphereBounds(FBox(FVector(0.0f, -0.5f, -0.5f), FVector(6.0f, 0.5f, 0.5f)));
}