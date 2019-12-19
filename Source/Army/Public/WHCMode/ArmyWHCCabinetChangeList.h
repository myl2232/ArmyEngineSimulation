#pragma once
#include "ArmyWHCShapeChangeList.h"
#include "Misc/Guid.h"

class FArmyWHCCabinetMtlChange final : public IArmyWHCModeShapeChange
{
public:
    FArmyWHCCabinetMtlChange(class FSCTShape *InShape);
    virtual void Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
    virtual void Deserialize(const TSharedPtr<FJsonObject>& JsonObject) override;
    virtual void ProcessChange() override;
    virtual bool IsSameType(const FString &InType) const override { return InType == SHAPE_CHANGE_CABINET_MTL; }

    void SetCabinetMtlId(int32 InId) { Id = InId; }
    int32 GetCabinetMtlId() const { return Id; }

private:
    int32 Id;
};

class FArmyWHCCabinetInsertionBoardMtlChange final : public IArmyWHCModeShapeChange
{
public:
    FArmyWHCCabinetInsertionBoardMtlChange(class FSCTShape *InShape);
    virtual void Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
    virtual void Deserialize(const TSharedPtr<FJsonObject>& JsonObject) override;
    virtual void ProcessChange() override;
    virtual bool IsSameType(const FString &InType) const override { return InType == SHAPE_CHANGE_CABINET_INSERTION_BOARD_MTL; }

    void SetInsertionBoardMtlId(int32 InId) { Id = InId; }
    int32 GetInsertionBoardMtlId() const { return Id; }

private:
    int32 Id;
};

class FArmyWHCFrontBoardCabinetChange final : public IArmyWHCModeShapeChange
{
public:
    FArmyWHCFrontBoardCabinetChange(class FSCTShape *InShape);
    virtual void Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
    virtual void Deserialize(const TSharedPtr<FJsonObject>& JsonObject) override;
    virtual void ProcessChange() override;
    virtual bool IsSameType(const FString &InType) const override { return InType == SHAPE_CHANGE_FRONT_BOARD_CAB; }

    void SetNumDoors(int32 InNumDoors) { NumDoors = InNumDoors; }
    int32 GetNumDoors() const { return NumDoors; }
    void SetDoorPoints(float InStartPoint, float InEndPoint) { StartPoint = InStartPoint; EndPoint = InEndPoint; }
    void GetDoorPoints(float &OutStartPoint, float &OutEndPoint) const { OutStartPoint = StartPoint; OutEndPoint = EndPoint; }

private:
    int32 NumDoors;
    float StartPoint, EndPoint;
};

class FArmyWHCEmbbedElecDevChange final : public IArmyWHCModeShapeChange
{
public:
    FArmyWHCEmbbedElecDevChange(class FSCTShape *InShape);
    virtual void Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
    virtual void Deserialize(const TSharedPtr<FJsonObject>& JsonObject) override;
    virtual void ProcessChange() override;
    virtual bool IsSameType(const FString &InType) const override { return InType == SHAPE_CHANGE_EMBBED_ELEC_DEV; }

    void SetElecDevGroupGuid(const FGuid &InGuid) { ElecDevGroupGuid = InGuid; }
    const FGuid& GetElecDevGroupGuid() const { return ElecDevGroupGuid; }
    void SetElecDevId(int32 InId) { ElecDevId = InId; }
    int32 GetElecDevId() const { return ElecDevId; }

private:
    FGuid ElecDevGroupGuid;
    int32 ElecDevId;
};

class FArmyWHCDelEmbbedElecDevChange final : public IArmyWHCModeShapeChange
{
public:
    FArmyWHCDelEmbbedElecDevChange(class FSCTShape *InShape);
    virtual void Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
    virtual void Deserialize(const TSharedPtr<FJsonObject>& JsonObject) override;
    virtual void ProcessChange() override;
    virtual bool IsSameType(const FString &InType) const override { return InType == SHAPE_CHANGE_EMBBED_ELEC_DEV; }
};

class FArmyWHCSlidingDoorWardrobeBBoardVisibilityChange final : public IArmyWHCModeShapeChange
{
public:
    FArmyWHCSlidingDoorWardrobeBBoardVisibilityChange(class FSCTShape *InShape);
    virtual void Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
    virtual void Deserialize(const TSharedPtr<FJsonObject>& JsonObject) override;
    virtual void ProcessChange() override;
    virtual bool IsSameType(const FString &InType) const override { return InType == SHAPE_CHANGE_SWARDROBE_BOTTOM_BOARD_VIS; }

    void SetVisibility(bool bVis) { Visibility = bVis; }
    bool GetVisibility() const { return Visibility; }
    
private:
    bool Visibility = true;
};

class FArmyWHCSlidingwayChange final : public IArmyWHCModeShapeChange
{
public:
    FArmyWHCSlidingwayChange(class FSCTShape *InShape);
    virtual void Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
    virtual void Deserialize(const TSharedPtr<FJsonObject>& JsonObject) override;
    virtual void ProcessChange() override;
    virtual bool IsSameType(const FString &InType) const override { return InType == SHAPE_CHANGE_SLIDINGWAY; }

    void SetSlidingwayId(int32 Id) { SlidingwayId = Id; }
    int32 GetSlidingwayId() const { return SlidingwayId; }

private:
    int32 SlidingwayId = 0;
};