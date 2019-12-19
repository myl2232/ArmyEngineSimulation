#pragma once

#include "ArmyObject.h"
#include "CoreMinimal.h"
#include "SBoxPanel.h"

class ARMYSCENEDATA_API FArmyDownLeadLabel :
	public FArmyObject
{
public:
	enum LabelType
	{
		LT_None,
		LT_Size,//规格
		LT_Material,//材质
		LT_Technology//工艺
	};
public:
	/**	构造函数*/
	FArmyDownLeadLabel();

	/**	析构函数*/
	virtual ~FArmyDownLeadLabel();

	virtual void SetState(EObjectState InState) override;

	/** 绘制 */
	virtual void DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas);

	/** 绘制 */
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	/** 选中 */
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

	/** 高亮 */
	virtual bool Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

	/** 高亮 */
	virtual TSharedPtr<FArmyEditPoint> HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;

	virtual TSharedPtr<FArmyEditPoint> SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

	/**	获取所有线*/
	virtual void GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs = false);

	virtual void Refresh();

	/**	获取包围盒*/
	virtual const FBox GetBounds();

	virtual void Destroy();

	virtual void ApplyTransform(const FTransform& Trans);

	void AttachDynamicWidget(TSharedPtr<SWidget> InWidget);

	void StartEditText();

	void EndEditText();

	void SetTextSize(int32 InValue);

	int32 GetTextSize();

	void SetTextColor(FLinearColor InColor) { TextColor = InColor; };

	void SetLabelType(LabelType InType);

	LabelType GetLabelType() { return CurrentLabelType; };

	void Init(TSharedPtr<SOverlay> ParentWidget);

	void EnableEditText(bool InV) { CanEditText = InV; };

	void SetLeadPoint(const FVector& InPos);

	void SetEndPoint(const FVector& InPos);

	void SetWorldPosition(const FVector& InPos);

	FVector GetWorldPosition() { return WorldPosition; };

	void SetLabelContent(FText InText) { LabelContent = InText; };

	const FText& GetLabelContent() { return LabelContent; };

	bool IsMoveOperationPoint(TSharedPtr<class FArmyEditPoint> InP) { return InP == EditPoint; };

	/**	设置关联对象标识*/
	void SetObjTag(FObjectPtr InObj,int32 InTag);

	/**	获取关联对象标识*/
	void GetObjTag(FObjectPtr& OutObj, int32& OutTag);

	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);

	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);

	TSharedPtr<FArmyLine> GetLeadLine() const { return LeadLine; }

	TSharedPtr<FArmyLine> GetMainLine() const { return MainLine; }

	FVector	GetTextLocation()const { return TextWorldPosition; }

	FVector	GetTextDirection()const { return TextDirection; }

private:
	void OnTextCommitted(const FText& InText, const ETextCommit::Type InTextAction);
	void UpdateInputBox(UArmyEditorViewportClient* InViewPortClient);
private:
	bool CanEditText = true;

	bool IsIputText = false;

	bool EditTextState = false;

	LabelType CurrentLabelType;

	FLinearColor TextColor = FLinearColor(FColor(0xFF999999));

	FVector2D LabelSize = FVector2D(0,0);

	int32 TextFontSize = 10;

	FText LabelContent;

	/**	关联对象*/
	TSharedPtr<FArmyObject> BindObject;

	/**	关联对象标识*/
	int32 ObjTag = -1;

	TSharedPtr<FArmyLine> LeadLine;

	TSharedPtr<FArmyLine> MainLine;

	TSharedPtr<class FArmyEditPoint> EditPoint;

	TSharedPtr<class FArmyEditPoint> LeadEditPoint;

	TSharedPtr<class FArmyEditPoint> EndEditPoint;

	TSharedPtr<SVerticalBox> InPutPanel;

	TSharedPtr<SOverlay> DynamicWidget;

	TSharedPtr<SOverlay> InputBoxParent;

	TSharedPtr<class SMultiLineEditableTextBox> TextInputBox;

	FScaleCanvasText FillTextItem;//@梁晓菲

	FVector TextWorldPosition = FVector(ForceInitToZero);

	FVector WorldPosition = FVector(ForceInitToZero);

	FVector TextDirection = FVector(ForceInitToZero);

	FVector2D PixPos;
};
REGISTERCLASS(FArmyDownLeadLabel)
