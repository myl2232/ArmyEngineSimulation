#pragma once
#include "ArmyActor.h"
#include "ArmyObject.h"

void AXRActor::SetRelevanceObject(TWeakPtr<FArmyObject> InObj)
{
	RelevanceObject = InObj;
}
TWeakPtr<FArmyObject> AXRActor::GetRelevanceObject()
{
	return RelevanceObject;
}