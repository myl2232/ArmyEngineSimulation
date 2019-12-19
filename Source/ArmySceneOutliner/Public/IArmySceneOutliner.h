#pragma once

#include "ArmySceneOutlinerFwd.h"
#include "Widgets/SCompoundWidget.h"

template<typename ItemType> class STreeView;

/**
 * The public interface for the Scene Outliner widget
 */
class IXRSceneOutliner : public SCompoundWidget
{
public:
	virtual void ShowOrHideLevelActor(AActor* InActor, bool NewHiden) = 0;
    /** Sends a requests to the Scene Outliner to refresh itself the next chance it gets */
    virtual void Refresh(bool bForce = false) = 0;
	/**	close and lock Expander */
	virtual void CloseExpander(const FName& InName, bool InClose, bool InVisible = false, bool InSelectable = true) = 0;
    /** Get a const reference to the actual tree hierarchy */
    virtual const STreeView<FArmyTreeItemPtr>& GetTree() const = 0;
};