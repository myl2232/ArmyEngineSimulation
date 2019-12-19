/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRSecurityDoor.h
* @Description 防盗门
*
* @Author 欧石楠
* @Date 2018年10月29日
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