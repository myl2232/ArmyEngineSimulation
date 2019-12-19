#pragma once

#include "ArmyOperation.h"
#include "ArmyBaseArea.h"
#include "ArmyToolsModule.h"

FArmyOperation::FArmyOperation(EModelType InBelongModel) :BelongModel(InBelongModel)
{
	ModifyOperationState = false;
	ModifyOperationTime = 0;
	OperationLine = MakeShareable(new FArmyLine());
	hitTime = 1;
}

FVector FArmyOperation::GetWorldFromLocal(FVector InputPosition, TSharedPtr<FArmyBaseArea> InputaseArea)
{
	if (InputaseArea.IsValid())
	{
		FVector PlaneNormal = InputaseArea->GetPlaneNormal();
		FVector Center = InputaseArea->GetPlaneCenter();
		FVector XDir = InputaseArea->GetXDir();
		FVector YDir = InputaseArea->GetYDir();
		float tempOffset = InputaseArea->SurfaceType == 2 ? -2 : 2;
		FVector WorldPos = InputPosition.X* XDir + InputPosition.Y * YDir + PlaneNormal * tempOffset + Center;
		return WorldPos;
	}
	return FVector(0, 0, 0);

}

FVector FArmyOperation::GetIntersectPointWithPlane(const FVector2D& InMousePos, TSharedPtr<class FArmyBaseArea> CurrentAttachedArea)
{
	FVector MWorldStart, MWorldDir;

	GVC->DeprojectFVector2D(FVector2D(InMousePos.X, InMousePos.Y), MWorldStart, MWorldDir);

	FPlane BasePlane(CurrentAttachedArea->GetPlaneCenter() + CurrentAttachedArea->GetPlaneNormal()*CurrentAttachedArea->GetExtrusionHeight(), CurrentAttachedArea->GetPlaneNormal());
	FVector MouseWorldPoint = FMath::LinePlaneIntersection(MWorldStart, MWorldStart + MWorldDir * ((GVC->GetViewLocation() - FVector::PointPlaneProject(GVC->GetViewLocation(), BasePlane)).Size() + 100000), BasePlane);
	float x = FVector::DotProduct((MouseWorldPoint - CurrentAttachedArea->GetPlaneCenter()), CurrentAttachedArea->GetXDir());
	float y = FVector::DotProduct((MouseWorldPoint - CurrentAttachedArea->GetPlaneCenter()), CurrentAttachedArea->GetYDir());
	return FVector(x, y, 0.0);
}

bool FArmyOperation::ShouldTick()
{
    GVC->GetViewportSize(ViewportSize);
    if (ViewportSize.X == 0 || ViewportSize.Y == 0)
    {
        return false;
    }

	ViewportMousePos = FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint();

    if (LastViewportMousePos == ViewportMousePos || ViewportMousePos < -ViewportSize)
    {
		LastViewportMousePos = ViewportMousePos;
        return false;
    }

    LastViewportMousePos = ViewportMousePos;

    return true;
}