#pragma once

#include "ArmyHardware.h"
#include "ArmyPolyline.h"

class ARMYSCENEDATA_API FArmyAirLouver : public FArmyHardware
{
public:
	FArmyAirLouver();
	FArmyAirLouver(FArmyAirLouver* Copy);
	virtual ~FArmyAirLouver();

	//~ Begin FArmyObject Interface
	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	virtual void SetState(EObjectState InState) override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual bool IsSelected(const FVector& Pos, UArmyEditorViewportClient* InViewportClient) override;
	virtual bool Hover(const FVector& Pos, UArmyEditorViewportClient* InViewportClient) override;
	virtual const FBox GetBounds() override;
	virtual void Destroy() override;
	//~ End FArmyObject Interface

	/**	设置属性标识*/
	virtual void SetPropertyFlag(PropertyType InType, bool InUse) override;

	//~ Begin FArmyHardware Interface
	virtual void Update() override;
	//~ End FArmyHardware Interface	

private:
	//代表空调孔的四条线
	TSharedPtr<FArmyLine> LeftLengthLine;
	TSharedPtr<FArmyLine> RightLengthLine;
	TSharedPtr<FArmyLine> LeftWidthLine;
	TSharedPtr<FArmyLine> RightWidthLine;

private:
	//两侧突出距离
	const float widthOffset = 3.f;
	//默认离地高度
	const float defaultHeight = 230.f;
	//默认直径
	const float defaultDiameter = 6.f;
	//默认宽度
	const float defaultWidth = 22.f;
};
REGISTERCLASS(FArmyAirLouver)