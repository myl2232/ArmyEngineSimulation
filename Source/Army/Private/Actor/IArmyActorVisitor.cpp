#include "IArmyActorVisitor.h"
#include "EngineUtils.h"
#include "ArmyViewportClient.h"

void IArmyActorVisitor::Show(bool bShow)
{
    for (auto It : Actors)
    {
        It->SetActorHiddenInGame(!bShow);
    }
}

void IArmyActorVisitor::Lock(bool bLock)
{
    for (auto It : Actors)
    {
        It->bIsSelectable = !bLock;
    }
}

void IArmyActorVisitor::Clear()
{
    while (Actors.Num() > 0)
    {
        Actors.Last()->Destroy();
    }
}

void IArmyActorVisitor::Refresh()
{
    for (FActorIterator ActorIt(GVC->GetWorld()); ActorIt; ++ActorIt)
    {
        if (IsActorTagsMatched(*ActorIt))
        {
            Actors.Add(*ActorIt);
        }
    }
}
