#pragma once
#include "Widgets/SWidget.h"

enum class EWHCAttrUIType {
    EWAUT_AboveGround,
    EWAUT_WidthEditing,
    EWAUT_DepthEditing,
    EWAUT_HeightEditing,
    EWAUT_VentilatorWidthEditing,
    EWAUT_Width,
    EWAUT_Depth,
    EWAUT_Height,
    EWAUT_Cab,
    EWAUT_CabMtl,
    EWAUT_Door,
    EWAUT_DoorMtl,
    EWAUT_DoorHandle,
    EWAUT_Slidingway,
    EWAUT_CabAcc,
    EWAUT_SlidingDoorWardrobeBBoard,
    EWAUT_DoorVisibility,
    EWAUT_ModificationRange,
    EWAUT_Extrusion,
    EWAUT_ExtrusionMtl,
    EWAUT_DecorationBoard,
    EWAUT_Platform,
	EWAUT_CabDrawerDoorVisibility,
    EWAUT_Num
};

class IArmyWHCAttrUIComponent 
{
public:
    IArmyWHCAttrUIComponent(class SArmyWHCModeAttrPanelContainer *InContainer) : Container(InContainer) {}
    virtual ~IArmyWHCAttrUIComponent() {}
    virtual TSharedPtr<SWidget> MakeWidget() = 0;
    EWHCAttrUIType GetType() const { return Type; }

protected:
    EWHCAttrUIType Type;
    SArmyWHCModeAttrPanelContainer *Container;
};

template <class T>
class TXRWHCAttrUIComponent : public IArmyWHCAttrUIComponent
{
public:  
    TXRWHCAttrUIComponent(class SArmyWHCModeAttrPanelContainer *InContainer) : IArmyWHCAttrUIComponent(InContainer) {}
    template <class ...Args>
    static T* MakeComponentInstance(class SArmyWHCModeAttrPanelContainer *InContainer, Args && ...args)
    {
        return new T(InContainer, Forward<Args>(args)...);
    }
};

template <class T, class ...Args>
inline T* MakeAttrUIComponent(class SArmyWHCModeAttrPanelContainer *InContainer, TArray<TSharedPtr<IArmyWHCAttrUIComponent>> &InOutComponentArr, Args && ...args)
{
    T *Component = (T*)TXRWHCAttrUIComponent<T>::MakeComponentInstance(InContainer, Forward<Args>(args)...);
    InOutComponentArr.Emplace(MakeShareable(Component));
    return Component;
}

inline TArray<IArmyWHCAttrUIComponent*> FindAttrUIComponents(EWHCAttrUIType InType, const TArray<TSharedPtr<IArmyWHCAttrUIComponent>> &InComponentArr)
{
    TArray<IArmyWHCAttrUIComponent*> RetComponentArr;
    for (const auto &Component : InComponentArr)
    {
        if (Component->GetType() == InType)
            RetComponentArr.Emplace(Component.Get());
    }
    return MoveTemp(RetComponentArr);
}