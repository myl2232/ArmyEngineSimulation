#pragma once
/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File SArmyHardModeContentBrowser.h
 * @Description 水电模式开关控制图
 *
 * @Author 朱同宽
 * @Date 2018年10月08日
 * @Version 1.0
 */
#include "ArmySceneOutlinerFwd.h"
#include "CoreMinimal.h"
#include "SlateBasics.h"
#include "ArmyStyle.h"
#include "SArmyObjectTreeItem.h"
#include "SArmyObjectOutliner.h"
#include "ArmyEditorEngine.h"

template<class ItemType> class STreeView;
class FArmyObject;
typedef TSharedPtr<class SArmySpaceTreeItem> SArmySpaceTreeItemPtr;
typedef TSharedRef<class SArmySpaceTreeItem> SArmySpaceTreeItemRef;

//郭子阳 1.15
typedef STreeView<SArmySpaceTreeItemPtr> SArmySwitchConnectTreeView;

enum class SwitchConnectLightType
{
	Normal,
	Extrusion //灯带
};


class SArmySpaceTreeItem : public TSharedFromThis <SArmySpaceTreeItem>
{
public:
	/**
	* 灯控设置Item类型
	*/
	enum ItemType
	{
	  	IT_None = 0,
		IT_SPACE = 1,//空间类型
		IT_SWITCH = 2, //开关类型
		IT_LIAN = 3, //开关的开关个数类型
		IT_LIGHT = 4,//灯具类型
		IT_Max
	};
public:
	/** Default constructor */
	SArmySpaceTreeItem() : Parent(nullptr),RowUI(nullptr),itemType(IT_None),SwtichIndex(-1){}
	virtual ~SArmySpaceTreeItem() {}

	/** This item's parent, if any. */
	mutable TWeakPtr<SArmySpaceTreeItem> Parent;

	/** Array of children contained underneath this item */
	mutable TArray<TSharedPtr<SArmySpaceTreeItem>> Children;

	STableRow< SArmySpaceTreeItemPtr >* RowUI;

	ItemType itemType;

	//灯的类型  @郭子阳 
	SwitchConnectLightType LightType= SwitchConnectLightType::Normal;

public:

	/** Get this item's parent. Can be nullptr. */
	SArmySpaceTreeItemPtr GetParent() const
	{
		return Parent.Pin();
	}

	/** Add a child to this item */
	void AddChild(SArmySpaceTreeItemPtr Child)
	{
		Child->Parent = AsShared();
		Children.Add(Child);
	}

	/** Remove a child from this item */
	void RemoveChild(const SArmySpaceTreeItemPtr Child)
	{
		if (Children.Remove(Child))
		{
			Child->Parent = nullptr;
		}
	}
	void ClearChildren()
	{
		for (auto Child:Children)
		{
			Child->Parent = nullptr;
		}
		Children.Empty();
	}
	/** Get this item's children, if any. Although we store as weak pointers, they are guaranteed to be valid. */
	FORCEINLINE const TArray<TSharedPtr<SArmySpaceTreeItem>>& GetChildren() const
	{
		return Children;
	}

	/** Module name */
	FName SpaceName;

	FGuid ItemID;

	AActor* Actor;

	TWeakPtr<FArmyObject> XRObject;

	int32 SwtichIndex;

};

/*房间行 开关行 注释：郭子阳 */
class SArmySwitchOutlinerTreeRow : public SMultiColumnTableRow<SArmySpaceTreeItemPtr>
{
public:
	SLATE_BEGIN_ARGS(SArmySwitchOutlinerTreeRow) {}

	//郭子阳 2018.1.14
	//添加单击响应事件
	/** Called when the button is clicked */
	SLATE_EVENT(FOnClicked, OnClicked)

	/** The list item for this row */
	SLATE_ARGUMENT(SArmySpaceTreeItemPtr,Item)

		SLATE_END_ARGS()

		/** Construct function for this widget *///TSharedPtr<SArmyObjectTreeView>
		void Construct(const FArguments& InArgs,const TSharedRef<SArmySwitchConnectTreeView>& OutlinerTreeView);

	/** Overridden from SMultiColumnTableRow.  Generates a widget for this column of the tree row. */
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;

public:
	//郭子阳 2018.1.14
	//添加单击响应事件
	/** The delegate to execute when the button is clicked */
	FOnClicked OnClicked;

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:

	/** The item associated with this row of data */
	SArmySpaceTreeItemPtr Item;
};


/*开关的1联/2联 注释：郭子阳 */ 
class SArmySwitchItemOutlinerTreeRow : public SMultiColumnTableRow<SArmySpaceTreeItemPtr>
{
public:
	SLATE_BEGIN_ARGS(SArmySwitchItemOutlinerTreeRow) {}

	/** The list item for this row */
	SLATE_ARGUMENT(SArmySpaceTreeItemPtr,Item)

		//郭子阳 2018.1.15
		//添加单击响应事件
		/** Called when the button is clicked */
	SLATE_EVENT(FOnClicked, OnClicked)

	/** Called when the checked state has changed */
	SLATE_EVENT(FOnCheckStateChanged,OnCheckStateChanged)

	/** Whether the check box is currently in a checked state */
	SLATE_ATTRIBUTE(ECheckBoxState,IsChecked)

	SLATE_END_ARGS()

		/** Construct function for this widget *///TSharedPtr<SArmyObjectTreeView>
	void Construct(const FArguments& InArgs,const TSharedRef<SArmySwitchConnectTreeView>& OutlinerTreeView);

	/** Overridden from SMultiColumnTableRow.  Generates a widget for this column of the tree row. */
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;


	//郭子阳 2018.1.14
	//添加单击响应事件
	/** The delegate to execute when the button is clicked */
	FOnClicked OnClicked;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:

	/** The item associated with this row of data */
	SArmySpaceTreeItemPtr Item;
	TSharedPtr<class SArmyImageTextButton> SArmyImageTextButton;
};


/*灯具行 注释：郭子阳 */
class SArmyLightItemOutlinerTreeRow : public SMultiColumnTableRow<SArmySpaceTreeItemPtr>
{
public:
	SLATE_BEGIN_ARGS(SArmyLightItemOutlinerTreeRow) {}

	/** The list item for this row */
	SLATE_ARGUMENT(SArmySpaceTreeItemPtr,Item)
	//删除按钮click
	SLATE_EVENT(FOnClicked,OnClicked)
	//灯具click
	SLATE_EVENT(FOnClicked, OnItemClicked)

	SLATE_END_ARGS()

	/** Construct function for this widget *///TSharedPtr<SArmyObjectTreeView>
	void Construct(const FArguments& InArgs,const TSharedRef<SArmySwitchConnectTreeView>& OutlinerTreeView);


	//郭子阳 2018.1.14
	//添加单击响应事件
	/** The delegate to execute when the button is clicked */
	FOnClicked OnItemClicked;

	//将鼠标单击转换为  OnItemClicked广播
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;
private:

	/** The item associated with this row of data */
	SArmySpaceTreeItemPtr Item;
};


class FArmyHydropowerSwitchConnect : public TSharedFromThis<FArmyHydropowerSwitchConnect>
{
public:
	FArmyHydropowerSwitchConnect();
	TSharedRef<SWidget> CreateDetailWidget();

	void ClearRootTree();

	void AddItemToTree(SArmySpaceTreeItemPtr Item);

	void Refresh();
private:
	TSharedRef<ITableRow> OnGenerateRowForOutlinerTree(SArmySpaceTreeItemPtr Item,const TSharedRef<STableViewBase>& OwnerTable);
	/** Called by STreeView to get child items for the specified parent item */
	void OnGetChildrenForOutlinerTree(SArmySpaceTreeItemPtr InParent,TArray<SArmySpaceTreeItemPtr>& OutChildren);
	void OnCheckStateChanged(ECheckBoxState InCheck,SArmySpaceTreeItemPtr CurrentItem);
	ECheckBoxState IsChecked(SArmySpaceTreeItemPtr CurrentItem)const;
	FReply DeleteItem(SArmySpaceTreeItemPtr Item);
public:
	DECLARE_DELEGATE_TwoParams(FOnCheckStateChangedDelegate,ECheckBoxState,SArmySpaceTreeItemPtr);
	FOnCheckStateChangedDelegate CheckStateChanged;
	DECLARE_DELEGATE_OneParam(FOnClickDelegate,SArmySpaceTreeItemPtr);
	FOnClickDelegate OnDelete;
	FOnClickDelegate OnItemClicked;
private:
	TSharedPtr<SArmySwitchConnectTreeView> OutlinerTreeView;

	SArmySpaceTreeItemPtr CurrentItem;

	TSharedPtr<class SArmyObjectOutliner> ObjectOutLiner;

public:
	/** Root level tree items */
	TArray<SArmySpaceTreeItemPtr> RootTreeItems;

	//郭子阳 2018.1.14
	//在UI中单击开关响应时间
	//添加单击响应事件
	FReply OnSwitchClicked(SArmySpaceTreeItemPtr Item);

	//高亮Actor对应的UI
	void HighLightCorrespondingUI(AActor * actor);

	//找到对应AActor的SArmySpaceTreeItemPtr
	bool FindCorrespondingItemPtr(SArmySpaceTreeItemPtr& InItem, AActor *Inactor, SArmySpaceTreeItemPtr& out);



};