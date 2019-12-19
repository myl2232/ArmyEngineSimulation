#pragma once
#include "../XRWHCAttrPanelMixin.h"
#include "Widgets/Layout/SBox.h"

class FArmyWHCCabWidthUI final : public TXRWHCAttrUIComponent<FArmyWHCCabWidthUI>
{
public:
    FArmyWHCCabWidthUI(class SArmyWHCModeAttrPanelContainer *InContainer)
    : TXRWHCAttrUIComponent<FArmyWHCCabWidthUI>(InContainer)
    { Type = EWHCAttrUIType::EWAUT_WidthEditing; }
    virtual TSharedPtr<SWidget> MakeWidget() override;

private:
    void ModifyWidth(const FString &InStr);
    FText GetWidth() const;

private:
    TSharedPtr<class SArmyCabDimensionComboBox> CabWidthCombo;
    TSharedPtr<SBox> CabWidthEditContainer;
};

class FArmyWHCCabDepthUI final : public TXRWHCAttrUIComponent<FArmyWHCCabDepthUI>
{
public:
    FArmyWHCCabDepthUI(class SArmyWHCModeAttrPanelContainer *InContainer)
    : TXRWHCAttrUIComponent<FArmyWHCCabDepthUI>(InContainer)
    { Type = EWHCAttrUIType::EWAUT_DepthEditing; }
    virtual TSharedPtr<SWidget> MakeWidget() override;

private:
    void ModifyDepth(const FString &InStr);
    FText GetDepth() const;

private:
    TSharedPtr<class SArmyCabDimensionComboBox> CabDepthCombo;
    TSharedPtr<SBox> CabDepthEditContainer;
};

class FArmyWHCCabHeightUI final : public TXRWHCAttrUIComponent<FArmyWHCCabHeightUI>
{
public:
    FArmyWHCCabHeightUI(class SArmyWHCModeAttrPanelContainer *InContainer)
    : TXRWHCAttrUIComponent<FArmyWHCCabHeightUI>(InContainer)
    { Type = EWHCAttrUIType::EWAUT_HeightEditing; }
    virtual TSharedPtr<SWidget> MakeWidget() override;

private:
    void ModifyHeight(const FString &InStr);
    FText GetHeight() const;

private:
    TSharedPtr<class SArmyCabDimensionComboBox> CabHeightCombo;
    TSharedPtr<SBox> CabHeightEditContainer;
};

class FArmyWHCCabVentilatorWidthUI final : public TXRWHCAttrUIComponent<FArmyWHCCabVentilatorWidthUI>
{
public:
    FArmyWHCCabVentilatorWidthUI(class SArmyWHCModeAttrPanelContainer *InContainer)
    : TXRWHCAttrUIComponent<FArmyWHCCabVentilatorWidthUI>(InContainer)
    { Type = EWHCAttrUIType::EWAUT_VentilatorWidthEditing; }
    virtual TSharedPtr<SWidget> MakeWidget() override;

private:
    void ModifyVentilatorWidth(const FString &InStr);
    FText GetVentilatorWidth() const;

private:
    TSharedPtr<class SArmyCabDimensionComboBox> VentilatorWidthCombo;
    TSharedPtr<SBox> VentilatorWidthContainer;
};

class FArmyWHCCabWidthStaticUIBase : public TXRWHCAttrUIComponent<FArmyWHCCabWidthStaticUIBase>
{
public:
    FArmyWHCCabWidthStaticUIBase(class SArmyWHCModeAttrPanelContainer *InContainer)
    : TXRWHCAttrUIComponent<FArmyWHCCabWidthStaticUIBase>(InContainer)
    { Type = EWHCAttrUIType::EWAUT_Width; }
    virtual TSharedPtr<SWidget> MakeWidget() override;

protected:
    virtual bool IsValidSelection() const = 0;
    virtual float GetWidthValue() = 0;
};

class FArmyWHCCabWidthStaticUI final : public FArmyWHCCabWidthStaticUIBase
{
public:
    FArmyWHCCabWidthStaticUI(class SArmyWHCModeAttrPanelContainer *InContainer)
    : FArmyWHCCabWidthStaticUIBase(InContainer) {}

protected:
    virtual bool IsValidSelection() const override;
    virtual float GetWidthValue() override;   
};

class FArmyWHCAccWidthStaticUI final : public FArmyWHCCabWidthStaticUIBase
{
public:
    FArmyWHCAccWidthStaticUI(class SArmyWHCModeAttrPanelContainer *InContainer)
    : FArmyWHCCabWidthStaticUIBase(InContainer) {}   

protected:
    virtual bool IsValidSelection() const override;
    virtual float GetWidthValue() override;
};

class FArmyWHCDrawerWidthStaticUI final : public FArmyWHCCabWidthStaticUIBase
{
public:
    FArmyWHCDrawerWidthStaticUI(class SArmyWHCModeAttrPanelContainer *InContainer)
    : FArmyWHCCabWidthStaticUIBase(InContainer) {}   

protected:
    virtual bool IsValidSelection() const override;
    virtual float GetWidthValue() override;
};

class FArmyWHCCabDepthStaticUIBase : public TXRWHCAttrUIComponent<FArmyWHCCabDepthStaticUIBase>
{
public:
    FArmyWHCCabDepthStaticUIBase(class SArmyWHCModeAttrPanelContainer *InContainer)
    : TXRWHCAttrUIComponent<FArmyWHCCabDepthStaticUIBase>(InContainer)
    { Type = EWHCAttrUIType::EWAUT_Depth; }
    virtual TSharedPtr<SWidget> MakeWidget() override;

protected:
    virtual bool IsValidSelection() const = 0;
    virtual float GetDepthValue() = 0;
};

class FArmyWHCCabDepthStaticUI final : public FArmyWHCCabDepthStaticUIBase
{
public:
    FArmyWHCCabDepthStaticUI(class SArmyWHCModeAttrPanelContainer *InContainer)
    : FArmyWHCCabDepthStaticUIBase(InContainer) {}

protected:
    virtual bool IsValidSelection() const override;
    virtual float GetDepthValue() override;
};

class FArmyWHCAccDepthStaticUI final : public FArmyWHCCabDepthStaticUIBase
{
public:
    FArmyWHCAccDepthStaticUI(class SArmyWHCModeAttrPanelContainer *InContainer)
    : FArmyWHCCabDepthStaticUIBase(InContainer) {}

protected:
    virtual bool IsValidSelection() const override;
    virtual float GetDepthValue() override;
};

class FArmyWHCDrawerDepthStaticUI final : public FArmyWHCCabDepthStaticUIBase
{
public:
    FArmyWHCDrawerDepthStaticUI(class SArmyWHCModeAttrPanelContainer *InContainer)
    : FArmyWHCCabDepthStaticUIBase(InContainer) {}

protected:
    virtual bool IsValidSelection() const override;
    virtual float GetDepthValue() override;
};

class FArmyWHCCabHeightStaticUIBase : public TXRWHCAttrUIComponent<FArmyWHCCabHeightStaticUIBase>
{
public:
    FArmyWHCCabHeightStaticUIBase(class SArmyWHCModeAttrPanelContainer *InContainer)
    : TXRWHCAttrUIComponent<FArmyWHCCabHeightStaticUIBase>(InContainer)
    { Type = EWHCAttrUIType::EWAUT_Height; }
    virtual TSharedPtr<SWidget> MakeWidget() override;

protected:
    virtual bool IsValidSelection() const = 0;
    virtual float GetHeightValue() = 0;
};

class FArmyWHCCabHeightStaticUI final : public FArmyWHCCabHeightStaticUIBase
{
public:
    FArmyWHCCabHeightStaticUI(class SArmyWHCModeAttrPanelContainer *InContainer)
    : FArmyWHCCabHeightStaticUIBase(InContainer) {}

protected:
    virtual bool IsValidSelection() const override;
    virtual float GetHeightValue() override;
};

class FArmyWHCAccHeightStaticUI final : public FArmyWHCCabHeightStaticUIBase
{
public:
    FArmyWHCAccHeightStaticUI(class SArmyWHCModeAttrPanelContainer *InContainer)
    : FArmyWHCCabHeightStaticUIBase(InContainer) {}

protected:
    virtual bool IsValidSelection() const override;
    virtual float GetHeightValue() override;
};

class FArmyWHCDrawerHeightStaticUI final : public FArmyWHCCabHeightStaticUIBase
{
public:
    FArmyWHCDrawerHeightStaticUI(class SArmyWHCModeAttrPanelContainer *InContainer)
    : FArmyWHCCabHeightStaticUIBase(InContainer) {}

protected:
    virtual bool IsValidSelection() const override;
    virtual float GetHeightValue() override;
};