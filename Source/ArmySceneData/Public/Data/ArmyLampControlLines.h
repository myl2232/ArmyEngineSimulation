#pragma once

#include "ArmyObject.h"

/**
 * 灯控线路
 */
class FArmyEditPoint;
class ARMYSCENEDATA_API FArmyLampControlLines : public FArmyObject
{
public:
	struct FControlLine
	{
		int32 SwitchIndex = -1;//多联开关中每一联的ID
		TArray<FObjectWeakPtr> ContrlLampArray;//每一联控制的所有灯
		TArray<TSharedPtr<FArmyArcLine>> ControlLines;//每一联与所有灯之间的所有连线

		FControlLine()
		{

		}
		FControlLine(int32 Index) :SwitchIndex(Index)
		{

		}
		bool operator==(const FControlLine& InOther) const
		{
			return SwitchIndex == InOther.SwitchIndex;
		}
		void SerializeToJson(TSharedRef<TJsonWriter<TCHAR,TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
		void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);
	};
	struct LampControlInfo
	{
		FObjectWeakPtr SwitchObj;//开关
		TArray<FControlLine> ControlLampLineArray;//开关控制信息数组
		LampControlInfo(){}
		LampControlInfo(FObjectWeakPtr InSwitch) :SwitchObj(InSwitch)
		{

		}
		bool operator==(const LampControlInfo& InOther) const
		{
			return SwitchObj == InOther.SwitchObj;
		}
		void SerializeToJson(TSharedRef<TJsonWriter<TCHAR,TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
		void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);
	};
public:
	FArmyLampControlLines();

	virtual ~FArmyLampControlLines();

	/** 绘制 */
	virtual void Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View);

	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;

	virtual void SetState(EObjectState InState) override;

	/** 移动位置 */
	virtual void ApplyTransform(const FTransform& Trans) override;

	virtual void Refresh();

	/** 选中 */
	virtual bool IsSelected(const FVector& Pos,class UArmyEditorViewportClient* InViewportClient);

	/** 高亮 */
	virtual bool Hover(const FVector& Pos,class UArmyEditorViewportClient* InViewportClient);

	virtual TSharedPtr<FArmyEditPoint> SelectPoint(const FVector& Pos,class UArmyEditorViewportClient* InViewportClient);

	virtual TSharedPtr<FArmyEditPoint> HoverPoint(const FVector& Pos,class UArmyEditorViewportClient* InViewportClient);

	LampControlInfo& GetLampControlInfo(FObjectWeakPtr InSwitch,bool InCreate = true);

	TSharedPtr<FArmyArcLine>& GetSelectArcLine(){ return SelectArcLine; }
	TSharedPtr<FArmyEditPoint> GetEditPoint(){return EditPoint;}

	/**	获取弧形线*/
	virtual void GetArcLines(TArray<TSharedPtr<FArmyArcLine>>& OutArcLines);
private:
	void Reset();

	void Sort(TArray<FVector>& Pointes);

	void FindEnvelopes(TArray<FVector>& InPoints, FVector InStartPoint, TArray<FVector>& OutEnvelope, int32 AttachDistance);
private:
	TArray<LampControlInfo> LampControlInfoArray;
	TArray<TArray<FVector>> TempPointes;
	TSharedPtr<class FArmyEditPoint> EditPoint;
	TSharedPtr<FArmyArcLine> SelectArcLine;
	FVector WorldPosition = FVector(ForceInitToZero);
};
REGISTERCLASS(FArmyLampControlLines)