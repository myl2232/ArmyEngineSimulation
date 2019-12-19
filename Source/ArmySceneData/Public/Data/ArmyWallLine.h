#pragma once
#include "ArmyObject.h"

class ARMYSCENEDATA_API FArmyWallLine :public FArmyObject
{
public:

	FArmyWallLine();
	FArmyWallLine(FArmyWallLine* Copy);
	FArmyWallLine(class FArmyLine* Copy);
	FArmyWallLine(const TSharedPtr<class FArmyEditPoint> InStart, const TSharedPtr<class FArmyEditPoint> InEnd);
	FArmyWallLine(const FVector& InStart, const FVector& InEnd);
	~FArmyWallLine() {};

	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;

	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);

	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);

public:
	void SetColor(const FLinearColor& InColor);

	void SetWidth(const float InWidth);

	TSharedPtr<class FArmyLine> GetCoreLine() const;

	void GetAppendObjects(TArray<FObjectWeakPtr> & InObjects);

	void PushAppendObject(FObjectWeakPtr InObject);

	void RemoveAppendObject(FObjectWeakPtr InObject);

	//只允许在拆改后数据使用
	const FVector& GetNormal() const;

	FVector GetDirection() const;

	void SetRelateRoom(TSharedPtr<class FArmyRoom> InRoom);

	TWeakPtr<class FArmyRoom> GetRelatedRoom() const;

private:
	TSharedPtr<class FArmyLine> CoreLine;

	TArray<FObjectWeakPtr> AppendObjects;

	TWeakPtr<class FArmyRoom> RelatedRoom;
};
REGISTERCLASS(FArmyWallLine)