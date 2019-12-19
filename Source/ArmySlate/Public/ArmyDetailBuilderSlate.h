#pragma once
#include "CoreMinimal.h"
#include "SlateBasics.h"
#include "ArmyStyle.h"
#include "Widgets/SToolTip.h"

/************************************************************************/
/*具体请参考SArmyDetailBuilderTest类的示例代码
*/
/************************************************************************/

//测试用的结构体
struct FTreeItem : public TSharedFromThis<FTreeItem>
{
public:
	FString ItemName;
	TSharedPtr<SWidget> ItemWidget;
	TArray<TSharedPtr<FTreeItem>> Children;
	TWeakPtr<FTreeItem> Parent;

	FTreeItem(FString _ItemName)
	{
		ItemName = _ItemName;
	}

	~FTreeItem() {}
};

class FArmyDetailNode;

//主要用来重载[]符号以及设置对其属性
class FArmyDetailLayout
{
public:
	FArmyDetailLayout(FArmyDetailNode& _ParentNode, float InMinWidth, float InMaxWidth, EHorizontalAlignment InHAlign, EVerticalAlignment InVAlign, FMargin InMargin)
		: ParentNode(_ParentNode)
		, HorizontalAlignment(InHAlign)
		, VerticalAlignment(InVAlign)
		, MinWidth(InMinWidth)
		, MaxWidth(InMaxWidth)
		, Widget(nullptr)
		, Margin(InMargin)
	{
		int a = 1;
	}

	~FArmyDetailLayout() {}

	FArmyDetailNode& operator[](TSharedPtr<SWidget> InWidget)
	{
		Widget = InWidget;
		return ParentNode;
	}

	FArmyDetailLayout& VAlign(EVerticalAlignment InAlignment)
	{
		VerticalAlignment = InAlignment;
		return *this;
	}

	FArmyDetailLayout& HAlign(EHorizontalAlignment InAlignment)
	{
		HorizontalAlignment = InAlignment;
		return *this;
	}

	FArmyDetailLayout& MinDesiredWidth(TOptional<float> InMinWidth)
	{
		MinWidth = InMinWidth;
		return *this;
	}

	FArmyDetailLayout& MaxDesiredWidth(TOptional<float> InMaxWidth)
	{
		MaxWidth = InMaxWidth;
		return *this;
	}

	FArmyDetailLayout& Padding(FMargin InMargin)
	{
		Margin = InMargin;
		return *this;
	}

	//@ 设置节点可见性
	void SetVisibility(TAttribute<EVisibility> _Visibility)
	{
		if (Widget.IsValid())
		{
			Widget->SetVisibility(_Visibility);
		}
	}

	TSharedPtr<SWidget> GetWidget() { return Widget; }
	void SetWidget(TSharedPtr<SWidget> NewWidget) { Widget = NewWidget; }

public:
	TSharedPtr<SWidget> Widget;
	EHorizontalAlignment HorizontalAlignment;
	EVerticalAlignment VerticalAlignment;
	TOptional<float> MinWidth;
	TOptional<float> MaxWidth;
	FMargin Margin;
private:
	FArmyDetailNode& ParentNode;
};

//树状展开结构的单位组建，负责构造左右标准结构的属性组建
class ARMYSLATE_API FArmyDetailNode : public TSharedFromThis<FArmyDetailNode>
{

public:
	FArmyDetailNode(FName _NodeName, FString _NodeDisplayName, TSharedPtr<SWidget> _NodeWidget)
		: NameWidget(*this, 0.0f, 0.0f, HAlign_Fill, VAlign_Top, FMargin(0,0,0,0))
		, ValueWidget(*this, 125.0f, 125.0f, HAlign_Left, VAlign_Fill, FMargin(0,0,0,0))
		, WholeRowWidget(*this, 0.0f, 0.0f, HAlign_Fill, VAlign_Fill, FMargin(0,0,0,0))
		, IfVisibility(EVisibility::Visible)
	{
		NodeName = _NodeName;
		NodeDisplayName = _NodeDisplayName;
		ValueWidget.SetWidget(_NodeWidget);
		bUseBackgroundBorder = false;
		bUseSplitter = true;
	}
	virtual ~FArmyDetailNode() {}

	/**@欧石楠 属性组件的显隐控制*/
	TAttribute<EVisibility> IfVisibility;

	//节点标识符，保证同一层级节点名称唯一
	FName NodeName;
	//显示名称，用在NameWidget
	FString NodeDisplayName;
	//左侧名称组件
	FArmyDetailLayout NameWidget;
	//右侧内容组件
	FArmyDetailLayout ValueWidget;
	//整行内容组件
	FArmyDetailLayout WholeRowWidget;
	//最外侧是否启用黑色背景，一般用在Category类型上
	bool bUseBackgroundBorder;
	//是否启用中间分隔线，默认需要，如果某个节点只是显示一个ValueWidget，不需要左侧的NameWidget占位，该值为false
	bool bUseSplitter;

	/** @欧石楠 是否使用醒目的颜色框*/
	bool bUseBorder = false;

    /** Category的左侧需要有8px的padding */
    bool bIsCategory = false;

	//根据节点Name获取对应节点
	TSharedPtr<FArmyDetailNode> FindChildNode(FName _NodeName)
	{
		TSharedPtr<FArmyDetailNode>* result = ChildrenNodes.Find(_NodeName);
		if (result)
			return (*result);

		return nullptr;
	}

	//添加一个已有节点到子节点
 	TSharedPtr<FArmyDetailNode> AddChildNode(TSharedPtr<FArmyDetailNode> _NewNode)
 	{
 		return ChildrenNodes.Add(_NewNode->NodeName, _NewNode);
 	}

	//根据节点Name获取对应节点
	int32 RemoveChildNode(FName _NodeName)
	{
		return ChildrenNodes.Remove(_NodeName);
	}

	FArmyDetailNode& AddChildNode(FName _NodeName, FString _NodeDisplayName = "NodeDisplayName", TSharedPtr<SWidget> _NodeWidget = nullptr)
	{
		TSharedRef<FArmyDetailNode> NewNode = MakeShareable(new FArmyDetailNode(_NodeName, _NodeDisplayName, _NodeWidget));
		ChildrenNodes.Shrink();
		ChildrenNodes.Add(NewNode->NodeName, NewNode);
		return *NewNode;
	}

	void ClearChildren()
	{
		ChildrenNodes.Reset();
	}

	FArmyDetailLayout& NameContentOverride()
	{
		bUseSplitter = true;
		return NameWidget;
	}

	FArmyDetailLayout& ValueContentOverride()
	{
		bUseSplitter = true;
		return ValueWidget;
	}

	FArmyDetailLayout& WholeContentOverride()
	{
		bUseSplitter = false;
		return WholeRowWidget;
	}

	//@ 修改左侧显示的文本内容
	FText GetNodeDisplayName() const{ return FText::FromString(NodeDisplayName); }
	void SetNodeDisplayName(FText _NodeDisplayName) { NodeDisplayName = _NodeDisplayName.ToString(); }

	TSharedPtr<SWidget> CreateTextNameWidget()
	{		
		// @欧石楠
		TSharedPtr<SBox> NameWidgetBox;
		if (bUseBorder)
		{
			NameWidgetBox =
			SNew(SBox)
            .HeightOverride(24)
			.VAlign(VAlign_Center)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(EHorizontalAlignment::HAlign_Left)
				.VAlign(EVerticalAlignment::VAlign_Center)
				.Padding(0, 0, 2, 0)
				[
					SNew(SImage)
					.Image(FArmyStyle::Get().GetBrush("Category.Construction"))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(EHorizontalAlignment::HAlign_Left)
				.VAlign(EVerticalAlignment::VAlign_Center)
				[
					SNew(STextBlock)
					.Text(this,&FArmyDetailNode::GetNodeDisplayName)
					.TextStyle(FArmyStyle::Get(), "ArmyText_12")
					.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 1)))
				]
			];
		}
		else
		{
			NameWidgetBox =
			SNew(SBox)
			.HeightOverride(24)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(this, &FArmyDetailNode::GetNodeDisplayName)
				.TextStyle(FArmyStyle::Get(), "ArmyText_12")
				.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 1)))
			];
		}
		if (!NodeDisplayName.IsEmpty())
		{
			NameWidgetBox->SetToolTip(
				SNew(SToolTip)
            	.Text(this,&FArmyDetailNode::GetNodeDisplayName)
            	.Font(FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12").Font)
            	.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
            	.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
			);
		}
		return NameWidgetBox;
	}

	//生成左侧名称组件
 	TSharedPtr<SWidget> GenerateNameWidget()
 	{
		TSharedPtr<SBox> BoxContainer = SNew(SBox)
            .HeightOverride(24)
			.HAlign(NameWidget.HorizontalAlignment)
			.Padding(ValueWidget.Margin)
			.VAlign(NameWidget.VerticalAlignment)
			.Visibility(IfVisibility);
		if (NameWidget.GetWidget().IsValid())
		{
			BoxContainer->SetContent(NameWidget.GetWidget().ToSharedRef());
		}
		else
		{
			NameWidget.SetWidget(CreateTextNameWidget());
			BoxContainer->SetContent(NameWidget.GetWidget().ToSharedRef());
		}
		return BoxContainer;
	}

	//生成右侧内容组件
	TSharedPtr<SWidget> GenerateValueWidget()
	{
		TSharedPtr<SBox> BoxContainer = SNew(SBox)
			.HAlign(ValueWidget.HorizontalAlignment)
			.Padding(ValueWidget.Margin)
			.VAlign(ValueWidget.VerticalAlignment)
			.Visibility(IfVisibility);
		if (ValueWidget.GetWidget().IsValid())
		{
			BoxContainer->SetContent(ValueWidget.GetWidget().ToSharedRef());
		}
		else
		{
			ValueWidget.SetWidget(SNullWidget::NullWidget);
			BoxContainer->SetContent(ValueWidget.GetWidget().ToSharedRef());
		}
		return BoxContainer;
	}
	//@  设置整行节点的可见性
	void SetNodeVisibility(TAttribute<EVisibility> _node) {
		ValueWidget.SetVisibility(_node);
		NameWidget.SetVisibility(_node);
		WholeRowWidget.SetVisibility(_node);
	}

	//生成右侧内容组件
	TSharedPtr<SWidget> GenerateWholeRowWidget()
	{
		TSharedPtr<SBox> BoxContainer = SNew(SBox)
			.HAlign(WholeRowWidget.HorizontalAlignment)
			.Padding(ValueWidget.Margin)
			.VAlign(WholeRowWidget.VerticalAlignment);
		if (WholeRowWidget.GetWidget().IsValid())
		{
			BoxContainer->SetContent(WholeRowWidget.GetWidget().ToSharedRef());
		}
		else
		{
			WholeRowWidget.SetWidget(SNullWidget::NullWidget);
			BoxContainer->SetContent(WholeRowWidget.GetWidget().ToSharedRef());
		}
		return BoxContainer;
	}

	TMap<FName, TSharedPtr<FArmyDetailNode>> ChildrenNodes;
};

//构件树状展开结构的管理器，具体使用方法可以参考里面的CreateTestDetailWidget函数，列举了所有常用类型组建的创建方法
class ARMYSLATE_API FArmyDetailBuilder : public TSharedFromThis<FArmyDetailBuilder>
{
public:
	FArmyDetailBuilder() {}
	~FArmyDetailBuilder() {}
	/**@欧石楠，右侧可添加一个widget*/
	FArmyDetailNode& AddCategory(FName _CategoryName, FString _DisplayCategoryName, TSharedPtr<SWidget> _NodeWidget = nullptr, bool bUseBorder = false);
	
	//@欧石楠 
	//可能返回空
	TSharedPtr<FArmyDetailNode> GetCategory(FName _CategoryName);

	//@欧石楠
	//根据名称排序
	/*
	* Order 按顺序排列的_CategoryName
	*/
	void SortCatergory(TArray<FName> Order);

	void RemoveCategory(FName _CategoryName);

	TSharedPtr<FArmyDetailNode> EditCategory(FName _CategoryName);
	TSharedPtr<SWidget> BuildDetail();
	void RefreshTree();
	void UpdateListTest();
	void UpdateDetail();
	TMap<FName, TSharedPtr<FArmyDetailNode>>& GetChildrenNodes() { return ChildrenNodes; }

	/**@欧石楠获取树状结构*/
	TSharedPtr<STreeView<TSharedPtr<FArmyDetailNode>>> GetTreeView()const { return TreeView; }

	/**@欧石楠获取树状数据源*/
	TArray<TSharedPtr<FArmyDetailNode>> GetTreeSource()const { return TreeSource; }

	/**@欧石楠 是否展开*/
	void SetTreeItemExpansion(bool bOpen);

private:
	TAttribute<float> LeftSize;
	TAttribute<float> RightSize;

	float ActualLeftSize;
	float GetLeftSize() const { return ActualLeftSize; }
	float GetRightSize() const { return 1.f - ActualLeftSize; }

	void OnLeftSizeChanged(float NewSize) { ActualLeftSize = NewSize; }
	void OnRightSizeChanged(float NewSize) {}

	TSharedRef<ITableRow> OnGenerateRowForTree(TSharedPtr<FArmyDetailNode> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	void GetChildrenForTree(TSharedPtr<FArmyDetailNode> TreeItem, TArray< TSharedPtr<FArmyDetailNode> >& OutChildren);

	TArray<TSharedPtr<FArmyDetailNode>> TreeSource;
	TSharedPtr<STreeView<TSharedPtr<FArmyDetailNode>>> TreeView;
	TMap<FName, TSharedPtr<FArmyDetailNode>> ChildrenNodes;

};