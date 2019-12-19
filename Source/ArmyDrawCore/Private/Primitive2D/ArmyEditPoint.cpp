#include "ArmyEditPoint.h"
#include "ArmyStyle.h"

FArmyEditPoint::FArmyEditPoint()
{
	bSprite = true;
	Size = 4.f;

    SetState(OPS_Normal);
}
FArmyEditPoint::FArmyEditPoint(const FVector& InV):FArmyPoint(InV)
{
	bSprite = true;
	Size = 4.f;

	SetState(OPS_Normal);
}
FArmyEditPoint::FArmyEditPoint(FArmyEditPoint* Copy):FArmyPoint(Copy)
{
	PriState = Copy->PriState;
	bSprite = Copy->bSprite;
	Size = Copy->Size;
	OperationType = Copy->OperationType;
}

bool FArmyEditPoint::Hover(const FVector& InPoint, class UArmyEditorViewportClient* InViewportClient)
{
    if (IsSelected(InPoint, InViewportClient))
    {
        SetState(OPS_Hovered);
        return true;
    }

    SetState(OPS_Normal);
	return false;
}

void FArmyEditPoint::AddReferenceLine(TSharedPtr<class FArmyLine> InLine)
{
	ReferenceLines.Push(InLine);
}

void FArmyEditPoint::MinusReferenceLine(TSharedPtr<class FArmyLine> InLine)
{
	ReferenceLines.Remove(InLine);
}

uint8 FArmyEditPoint::GetReferenceNum() const
{
	return ReferenceLines.Num();
}

const TArray<TSharedPtr<class FArmyLine>>& FArmyEditPoint::GetReferenceLines() const
{
	return ReferenceLines;
}

FBox FArmyEditPoint::GetBounds()
{
    return FBox(GetPos() - Size / 2.f, GetPos() + Size / 2.f);
}
