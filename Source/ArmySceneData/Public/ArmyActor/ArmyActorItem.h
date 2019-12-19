#pragma once

#include "ArmyObject.h"
#include "ArmyCommonTypes.h"

class ARMYSCENEDATA_API FArmyActorItem : public FArmyObject
{
public:
    FArmyActorItem()
        : ContentItem(NULL)
        , Actor(NULL)
        , Location(FVector::ZeroVector)
        , Rotation(FRotator::ZeroRotator)
        , Scale(FVector(1))
    {
        ObjectType = OT_ActorItem;
    }

    FArmyActorItem(AActor* InActor)
        : ContentItem(NULL)
        , Actor(InActor)
    {
        ObjectType = OT_ActorItem;
    }

    FArmyActorItem(FContentItemPtr InContentItem, AActor* InActor)
        : ContentItem(InContentItem)
        , Actor(InActor)
    {
        ObjectType = OT_ActorItem;
    }

    virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
    virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);

    virtual void PostDelete() override;

public:
    FContentItemPtr ContentItem;

    AActor* Actor;

    FVector Location;

    FRotator Rotation;

    FVector Scale;

};

REGISTERCLASS(FArmyActorItem)