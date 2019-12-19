#pragma once

#include "ArmySceneOutlinerFwd.h"

/**
 *	Interface for a scene outliner column
 */
class IXRSceneOutlinerColumn : public TSharedFromThis< IXRSceneOutlinerColumn >
{
public:
    virtual ~IXRSceneOutlinerColumn() {}

    virtual FName GetColumnID() = 0;

    virtual SHeaderRow::FColumn::FArguments ConstructHeaderRowColumn() = 0;

    virtual const TSharedRef<SWidget> ConstructRowWidget(FArmyTreeItemRef TreeItem, const STableRow<FArmyTreeItemPtr>& Row) = 0;

    virtual void Tick(double InCurrentTime, float InDeltaTime) {}
};