#pragma once
#include "ArmyWHCShapeChangeList.h"
#include "Misc/Guid.h"

class FArmyWHCSingleDoorGroupChange : public IArmyWHCModeShapeChange
{
public:
    FArmyWHCSingleDoorGroupChange(class FSCTShape *InShape);
    virtual void Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
    virtual void Deserialize(const TSharedPtr<FJsonObject>& JsonObject) override;
    virtual void ProcessChange() override;
    virtual bool IsSameType(const FString &InType) const override { return InType == SHAPE_CHANGE_SINGLE_DOOR_GROUP; }

    void SetDoorGroupGuid(const FGuid &InGuid) { DoorGroupGuid = InGuid; }
    const FGuid& GetDoorGroupGuid() const { return DoorGroupGuid; }

    void SetDoorTypeTemplateId(int32 InId) { Id = InId; }
    int32 GetDoorTypeTemplateId() const { return Id; }

protected:
    FGuid DoorGroupGuid;
    int32 Id;
};

class FArmyWHCSingleDrawerGroupChange final : public FArmyWHCSingleDoorGroupChange
{
public:
    FArmyWHCSingleDrawerGroupChange(class FSCTShape *InShape) : FArmyWHCSingleDoorGroupChange(InShape) {}
    virtual void Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
    virtual void ProcessChange() override;
    virtual bool IsSameType(const FString &InType) const override { return InType == SHAPE_CHANGE_SINGLE_DRAWER_GROUP; }
};

class FArmyWHCSingleDoorGroupMtlChange : public IArmyWHCModeShapeChange
{
public:
    FArmyWHCSingleDoorGroupMtlChange(class FSCTShape *InShape);
    virtual void Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
    virtual void Deserialize(const TSharedPtr<FJsonObject>& JsonObject) override;
    virtual void ProcessChange() override;
    virtual bool IsSameType(const FString &InType) const override { return InType == SHAPE_CHANGE_SINGLE_DOOR_GROUP_MTL; }

    void SetDoorGroupGuid(const FGuid &InGuid) { DoorGroupGuid = InGuid; }
    const FGuid& GetDoorGroupGuid() const { return DoorGroupGuid; }

    void SetDoorMtlId(int32 InId) { Id = InId; }
    int32 GetDoorMtlId() const { return Id; }

protected:
    FGuid DoorGroupGuid;
    int32 Id;
};

class FArmyWHCSingleDrawerGroupMtlChange final : public FArmyWHCSingleDoorGroupMtlChange
{
public:
    FArmyWHCSingleDrawerGroupMtlChange(class FSCTShape *InShape) : FArmyWHCSingleDoorGroupMtlChange(InShape) {}
    virtual void Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
    virtual bool IsSameType(const FString &InType) const override { return InType == SHAPE_CHANGE_SINGLE_DRAWER_GROUP_MTL; }
};

class FArmyWHCSingleDoorGroupHandleChange : public IArmyWHCModeShapeChange
{
public:
    FArmyWHCSingleDoorGroupHandleChange(class FSCTShape *InShape);
    virtual void Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
    virtual void Deserialize(const TSharedPtr<FJsonObject>& JsonObject) override;
    virtual void ProcessChange() override;
    virtual bool IsSameType(const FString &InType) const override { return InType == SHAPE_CHANGE_SINGLE_DOOR_GROUP_HANDLE; }

    void SetDoorGroupGuid(const FGuid &InGuid) { DoorGroupGuid = InGuid; }
    const FGuid& GetDoorGroupGuid() const { return DoorGroupGuid; }

    void SetHandleId(int32 InHandleId) { HandleId = InHandleId; }
    int32 GetHandleId() const { return HandleId; }

protected:
    FGuid DoorGroupGuid;
    int32 HandleId;
};

class FArmyWHCSingleDrawerGroupHandleChange final : public FArmyWHCSingleDoorGroupHandleChange
{
public:
    FArmyWHCSingleDrawerGroupHandleChange(class FSCTShape *InShape) : FArmyWHCSingleDoorGroupHandleChange(InShape) {}
    virtual void Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
    virtual bool IsSameType(const FString &InType) const override { return InType == SHAPE_CHANGE_SINGLE_DRAWER_GROUP_HANDLE; }
};

class FArmyWHCDoorOpenDirChange final : public IArmyWHCModeShapeChange
{
public:
    FArmyWHCDoorOpenDirChange(class FSCTShape *InShape);
    virtual void Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
    virtual void Deserialize(const TSharedPtr<FJsonObject>& JsonObject) override;
    virtual void ProcessChange() override;
    virtual bool IsSameType(const FString &InType) const override { return InType == SHAPE_CHANGE_DOOR_OPEN_DIR; }

    void SetDoorSheetGuid(const FGuid &InGuid) { DoorSheetGuid = InGuid; }
    const FGuid& GetDoorSheetGuid() const { return DoorSheetGuid; }

    void SetDoorOpenDir(int32 InOpenDir) { OpenDir = InOpenDir; }
    int32 GetDoorOpenDir() const { return OpenDir; }

private:
    FGuid DoorSheetGuid;
    int32 OpenDir;
};