#pragma once

#include "ArmyObjectModel.h"

void XRObjectModel::Add(FObjectPtr Object)
{
	Objects.AddUnique(Object);
}

void XRObjectModel::Delete(FObjectPtr Object)
{
	Object->OnDelete();
	Objects.Remove(Object);
}

void XRObjectModel::Empty()
{
    for (auto It : Objects)
    {
        It->Destroy();
    }

	Objects.Empty();
}

FObjectPtr XRObjectModel::Select(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	for (FObjectPtr Object : Objects)
	{
		if (Object->IsSelected(Pos, InViewportClient))
		{
			Object->SetState(OS_Selected);
			return Object;
		}
	}

	return NULL;
}

FObjectPtr XRObjectModel::Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	for (FObjectPtr Object : Objects)
	{
		if (Object->GetState() == OS_Selected)
		{
		/*	FObjectPtr OperationPoint = StaticCastSharedPtr<FArmyObject>(Object->HoverPoint(Pos, InViewportClient));
			if (Object->Hover(Pos, InViewportClient))
			{
				return Object;
			}
			if (OperationPoint.IsValid())
			{
				return OperationPoint;
			}*/
		}
		else if (Object->Hover(Pos, InViewportClient))
		{
			Object->SetState(OS_Hovered);
			return Object;
		}
	}

	return NULL;
}

const TArray<FObjectPtr>& XRObjectModel::GetAllObjects() const
{
	return Objects;
}