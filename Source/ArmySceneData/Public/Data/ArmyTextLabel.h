#pragma once

#include "ArmyObject.h"
#include "CoreMinimal.h"

class ARMYSCENEDATA_API FArmyTextLabel :
	public FArmyObject
{
public:
	enum LabelType
	{
		LT_None,
		LT_SpaceHeight,
		LT_SpaceName,
		LT_SpaceArea,
		LT_SpacePerimeter,
		LT_CustomHeight,//手动标高
		LT_Switch,
		LT_Socket,
		LT_WaterPoint,
		LT_Beam,//梁
		LT_BaseGroundHeight,
		LT_PostGroundHeight
	};
public:
	/**	构造函数*/
	FArmyTextLabel();

	/**	析构函数*/
	virtual ~FArmyTextLabel();

	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);

	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);

	virtual void SetState(EObjectState InState) override;

	/** 绘制 */
	virtual void DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas);

	/** 绘制 */
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	/** 选中 */
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

	/** 高亮 */
	virtual bool Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

	virtual TSharedPtr<FArmyEditPoint> SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

	virtual TSharedPtr<FArmyEditPoint> HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient);

	/**	获取所有线*/
	virtual void GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs);

	virtual void Refresh();

	/**	获取包围盒*/
	virtual const FBox GetBounds();

	virtual void ApplyTransform(const FTransform& Trans) override;

	TSharedPtr<class FArmyEditPoint> GetEditPoint() const { return EditPoint; };

	void StartEditText();

	/**设置文本字体大小*/
	void SetTextSize(int32 InSize);

	/**	获取文本字体大小*/
	int32 GetTextSize();

	void SetTextColor(FLinearColor InColor) { TextColor = InColor; };

	void SetLabelType(LabelType InType);

	LabelType GetLabelType() { return CurrentLabelType;};

	void Init(TSharedPtr<SOverlay> ParentWidget);

	void EnableEditText(bool InV) { CanEditText = InV; };

	void SetWorldPosition(const FVector& InPos);

	FVector GetWorldPosition() { return WorldPosition; };

	FVector GetTextPosition() { return TextWorldPosition; }

	void SetLabelContent(FText InText) { LabelContent = InText; };

	const FText& GetLabelContent() const { return LabelContent; }

	bool IsMoveOperationPoint(TSharedPtr<class FArmyEditPoint> InP) { return InP == EditPoint; };

	/**	设置关联对象标识*/
	void SetObjTag(FObjectPtr InObj,int32 InTag);

	/**	获取关联对象标识*/
	void GetObjTag(FObjectPtr& OutObj, int32& OutTag);

	const FGuid& GetBindObjGuid() const { return BindObjectGuid; };

	//获取和设置是否使用编辑点
	bool GetbUseEditPos() const { return bUseEditPos; }
	void SetbUseEditPos(bool InbUseEditPos) { bUseEditPos = InbUseEditPos; }

	//@ 获取或设置是否绘制标签，true表示绘制，否则不绘制
	bool GetbDrawLabel() const { return bDrawLabel; }
	void SetbDrawLabel(bool InbDrawLabel) { bDrawLabel = InbDrawLabel; }

	TMap<uint32, TSharedPtr<FArmyLine>> GetDrawLines() const { return DrawLines; }

private:
	void OnTextCommitted(const FText&, const ETextCommit::Type InTextAction);
	void UpdateInputBox(UArmyEditorViewportClient* InViewPortClient);

	void RebuildDrawLines();
private:
	bool CanEditText = true;

	bool IsIputText = false;

	FGuid BindObjectGuid;

	int32 TextSize = 12;

	LabelType CurrentLabelType;

	FLinearColor TextColor = FLinearColor::White;

	FVector2D LabelSize = FVector2D(0,0);

	FText LabelContent;
	/**	关联对象*/
	TSharedPtr<FArmyObject> BindObject;
	/**	关联对象标识*/
	int32 ObjTag = -1;

	TMap<uint32, TSharedPtr<FArmyLine>> DrawLines;

	TSharedPtr<class FArmyEditPoint> EditPoint;

	TSharedPtr<class SMultiLineEditableTextBox> TextInputBox;

	FVector TextWorldPosition = FVector(ForceInitToZero);
	FVector WorldPosition = FVector(ForceInitToZero);

	bool EditTextState = false;

	bool bUseEditPos = true;

	//是否绘制便签，true表示绘制，否则不绘制
	bool bDrawLabel = true;
};
REGISTERCLASS(FArmyTextLabel)
