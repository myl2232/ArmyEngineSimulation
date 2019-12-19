#include "Primitive2D/ArmyPrimitive.h"
#include "ArmyEditPoint.h"

FArmyPrimitive::FArmyPrimitive()
{
	DepthPriority = SDPG_World;
	DepthBias = 0.0f;	
	NormalColor = FLinearColor::White;
	HoverColor = FLinearColor(FColor(0xFFFF9800));
	BaseColor = NormalColor;
	ShowEditPoint = false;
	PriState = OPS_Normal;
}
FArmyPrimitive::~FArmyPrimitive()
{

}
void FArmyPrimitive::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (ShowEditPoint)
	{
		for (auto EditPoint : EditPointMap)
		{
			EditPoint.Value->Draw(PDI, View);
		}
	}
}
int32 FArmyPrimitive::GetEditPointIndex(TSharedPtr<FArmyEditPoint> InEditPoint)
{
	for (auto P : EditPointMap)
	{
		if (P.Value == InEditPoint)
		{
			return P.Key;
		}
	}
	return -1;
}
TSharedPtr<FArmyEditPoint> FArmyPrimitive::SelectEditPoint(FVector InPos, UArmyEditorViewportClient* InViewportClient)
{
	if (ShowEditPoint)
	{
		for (auto P : EditPointMap)
		{
			if (P.Value->IsSelected(InPos, InViewportClient))
			{
				return P.Value;
			}
		}
	}
	return NULL;
}

void FArmyPrimitive::SetState(EOPState InState)
{
	PriState = InState;

	switch (InState)
	{
	case OPS_Default:
		BaseColor = DefaultColor;
		break;

	case OPS_Normal:
		BaseColor = NormalColor;
		break;

	case OPS_Hovered:
		BaseColor = HoverColor;
		break;

	case OPS_Selected:
		BaseColor = HoverColor;
		break;

	case OPS_Disable:
		break;

	default:
		break;
	}
}

void FArmyPrimitive::SetBaseColor(const FLinearColor& InColor)
{
	BaseColor = InColor;
	DefaultColor = BaseColor;
}

void FArmyPrimitive::SetHoverColor(const FLinearColor & InColor)
{
	HoverColor = InColor;
}

void FArmyPrimitive::SetNormalColor(const FLinearColor & InColor)
{
	NormalColor = InColor;
}

const FLinearColor& FArmyPrimitive::GetBaseColor() const
{
	return BaseColor;
}

const FLinearColor & FArmyPrimitive::GetHoverColor() const
{
	return HoverColor;
}

const FLinearColor & FArmyPrimitive::GetNormalColor() const
{
	return NormalColor;
}
