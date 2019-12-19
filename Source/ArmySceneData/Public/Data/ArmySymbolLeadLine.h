#pragma once

#include "ArmyObject.h"

/**
 * 索引符号引线包括剖切点
 */
class ARMYSCENEDATA_API FArmySymbolLeadLine : public FArmyObject
{
public:
	enum ESymbolType
	{
		EST_NONE = 0,
		EST_LEFT = 1,
		EST_RIGHT = 1<<1,
		EST_TOP = 1<<2,
		EST_BOTTOM = 1<<3
	};
	FArmySymbolLeadLine();

	virtual ~FArmySymbolLeadLine();

	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);

	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);

	/** 刷新 */
	virtual void Refresh();

	/** 绘制 */
	virtual void Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View);

	/** HUD绘制 */
	virtual void DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas);

	/**	获取基点*/
	virtual const FVector GetBasePos();

	/** 获取包围盒*/
	virtual const FBox GetBounds();

	void SetBasePos(const FVector& InPos);

	void SetTargetPos(const FVector& InPos);

	const FVector GetTargetPos() const;

	void SetTargetType(uint8 InType, bool InUse);

	bool GetTargetType(uint8 InType) const;

	void SetColor(const FLinearColor& InColor);

	bool Pick(const FVector& InPos, FArmySymbolLeadLine::ESymbolType& OutType);

	void SetEditorModel(bool InState) {
		EditorModel = InState; 
		Update();
	};

	void SetCurrentRoomBoundBox(const FBox RoomBox);
private:
	void Update();
private:
	bool EditorModel = true;

	uint8 SymbolType = 0;

	float Offset = 30;
	float CheckBoxSize = 6.4;
	float SectionPointSize = 9; 

	FVector BasePos = FVector(ForceInitToZero);
	FVector TargetPos = FVector(ForceInitToZero);

	FLinearColor BaseColor = FLinearColor::White;
	TArray<FVector> LineVertexArray;

	FBox m_BCurrentRoomBound;
};
REGISTERCLASS(FArmySymbolLeadLine)