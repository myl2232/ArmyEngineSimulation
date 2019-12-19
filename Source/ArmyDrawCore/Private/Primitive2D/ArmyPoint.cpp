#include "ArmyPoint.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyEngineModule.h"

FArmyPoint::FArmyPoint()
{
	DepthPriority = SDPG_Foreground;
	Pos = FVector(0, 0, 0);
	Size = 1.0f;
	bSprite = false;
	PivotPosition = FVector::ZeroVector;
}

FArmyPoint::FArmyPoint(FArmyPoint* Copy)
{
	SetBaseColor(Copy->GetBaseColor());
	DepthPriority = Copy->DepthPriority;
	DepthBias = Copy->DepthBias;
	Size = Copy->Size;
	Pos = Copy->Pos;
	bSprite = Copy->bSprite;
	PivotPosition = Copy->PivotPosition;
	PropertyFlag = Copy->PropertyFlag;
}

FArmyPoint::FArmyPoint(FVector Position)
{
	Pos = Position;
	Size = 1.0f;
	bSprite = false;
	PivotPosition = FVector::ZeroVector;
}

void FArmyPoint::SetState(EOPState InState)
{
	PriState = InState;

	switch (InState)
	{
	case OPS_Default:
		BaseColor = DefaultColor;
		break;

	case OPS_Normal:
		BaseColor = FLinearColor::White;
		break;

	case OPS_Hovered:
		BaseColor = FLinearColor(FColor(0xFFFF5E00));
		break;

	case OPS_Selected:
		BaseColor = FLinearColor(FColor(0xFFFF5E00));
		break;

	case OPS_Disable:
		break;

	default:
		break;
	}
}

bool FArmyPoint::IsSelected(const FVector& Point, class UArmyEditorViewportClient* InViewportClient)
{
    FVector2D PixPoint = FVector2D(Point);
    FVector2D PixPos = FVector2D(Pos + PivotPosition);
	FBox2D BoundingBox;
	BoundingBox.Min = FVector2D(PixPos.X - 8, PixPos.Y - 8);
	BoundingBox.Max = FVector2D(PixPos.X + 8, PixPos.Y + 8);

	return BoundingBox.IsInside(PixPoint);
}
bool FArmyPoint::IsSelected(const FBox2D& box)
{
	return box.IsInside(FVector2D(Pos.X, Pos.Y));
}

void FArmyPoint::Transform(const FVector& PostAdd)
{
	PivotPosition += PostAdd;
	Pos += PostAdd;
}

void FArmyPoint::Rotate(const FRotator& Rotation)
{
	const FRotationMatrix RotMatrix(Rotation);
	Pos = RotMatrix.TransformVector(Pos);
	PivotPosition = RotMatrix.TransformVector(PivotPosition);
}

void FArmyPoint::Scale(const FVector& Scale)
{
	if (Scale.X == 1.0f&&Scale.Y == 1.0f&&
		Scale.Z == 1.0f)
	{
		return;
	}
	PivotPosition *= Scale;
	Pos *= Scale;
}

void FArmyPoint::SetBasePoint(const FVector& InBasePoint)
{
	PivotPosition = InBasePoint;
}

void  FArmyPoint::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetState() == FArmyPrimitive::OPS_Disable)
		return;

	float UniformScale = View->WorldToScreen(Pos + PivotPosition).W * (4.0f / View->ViewRect.Width() / View->ViewMatrices.GetProjectionMatrix().M[0][0]);
	float Scale = UniformScale / 2.0f;

	if (bSprite)
	{
		PDI->DrawSprite(Pos + PivotPosition, Size * Scale, Size * Scale, GEngine->DefaultDiffuseTexture->Resource, GetBaseColor(), DepthPriority, 0.0, 1.0, 1.0, 0.0, SE_BLEND_MaskedDistanceField);
		DrawWireBox(PDI, FBox::BuildAABB(Pos + PivotPosition, FVector(Size * Scale, Size * Scale, 0)), FLinearColor::White, 0, 1, 0,true);
	}
	else
	{
		PDI->DrawPoint(Pos + PivotPosition, GetBaseColor(), Size, DepthPriority);
	}
}
