#pragma once

class IXRSceneOutliner;
class IXRSceneOutlinerColumn;

struct FArmyInitializationOptions;
struct FArmySharedOutlinerData;

struct FArmyTreeItemID;
struct IXRTreeItem;

struct FArmyActorTreeItem;
struct FArmyWorldTreeItem;
struct FArmyFolderTreeItem;

typedef TSharedPtr<IXRTreeItem> FArmyTreeItemPtr;
typedef TSharedRef<IXRTreeItem> FArmyTreeItemRef;

typedef TMap<FArmyTreeItemID, FArmyTreeItemPtr> FArmyTreeItemMap;

/** A delegate used to factory a new column type */
DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<IXRSceneOutlinerColumn>, FArmyCreateSceneOutlinerColumn, IXRSceneOutliner&);