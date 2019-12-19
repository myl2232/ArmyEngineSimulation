/**
* Copyright 2018 ������Ƽ����޹�˾.
* All Rights Reserved.
* 
*
* @File XRSecurityDoor.h
* @Description ������
*
* @Author ŷʯ�
* @Date 2018��10��29��
* @Version 1.0
*/

#pragma once

#include "ArmySingleDoor.h"

class ARMYSCENEDATA_API FArmySecurityDoor : public FArmySingleDoor
{
public:
	FArmySecurityDoor();
	FArmySecurityDoor(FArmySecurityDoor* Copy);
	virtual ~FArmySecurityDoor();

    virtual void Generate(UWorld* InWorld) override;
	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
};

REGISTERCLASS(FArmySecurityDoor)