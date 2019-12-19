// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "STableRow.h"
#include "Widgets/SWidget.h"
#include "STableRow.h"

typedef TSharedPtr<class SArmyObjectTreeItem> SArmyObjectTreeItemPtr;
typedef TSharedRef<class SArmyObjectTreeItem> SArmyObjectTreeItemRef;

class SArmyObjectOutlinerTreeRow;
class ARMYSLATE_API SArmyObjectTreeItem: public TSharedFromThis <SArmyObjectTreeItem>
{
public:
	/** Default constructor */
	SArmyObjectTreeItem():Parent(nullptr), RowUI(nullptr), Visibility(true), EditState(false){}
	virtual ~SArmyObjectTreeItem() {}

	/** This item's parent, if any. */
	mutable TWeakPtr<SArmyObjectTreeItem> Parent;

	/** Array of children contained underneath this item */
    mutable TArray< TSharedPtr<SArmyObjectTreeItem> > Children;

    STableRow<SArmyObjectTreeItemPtr>* RowUI;

public:
	void SetVisible(bool InVisible)
	{
		Visibility = InVisible;
		for (auto ItemIt : Children)
		{
			ItemIt->SetVisible(InVisible);
		}
	}
	/** Get this item's parent. Can be nullptr. */
	SArmyObjectTreeItemPtr GetParent() const
	{
		return Parent.Pin();
	}

	/** Add a child to this item */
	void AddChild(SArmyObjectTreeItemPtr Child)
	{
		Child->Parent = AsShared();
		Children.Add(Child);
	}

	/** Remove a child from this item */
	void RemoveChild(const SArmyObjectTreeItemPtr Child)
	{
		if (Children.Remove(Child))
		{
			Child->Parent = nullptr;
		}
	}
	void ClearChildren()
	{
		for (auto Child : Children)
		{
			Child->Parent = nullptr;
		}
		Children.Empty();
	}
	/** Get this item's children, if any. Although we store as weak pointers, they are guaranteed to be valid. */
	FORCEINLINE const TArray<TSharedPtr<SArmyObjectTreeItem>>& GetChildren() const
	{
		return Children;
	}

	/** Module name */
	FName ModuleName;

	FGuid ItemID;

	bool EditState;

	bool Visibility;
};
