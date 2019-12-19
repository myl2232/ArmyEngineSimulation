/**
 * Copyright 2019 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File FArmyConstructionItemInterface.h
 * @Description 施工项数据接口
 *
 * @Author 欧石楠
 * @Date 2019年1月22日
 * @Version 1.0
 */

#pragma once
//#include "ArmyObject.h"
#include "Json.h"

class ARMYSCENEDATA_API FArmyConstructionItemInterface : public TSharedFromThis<FArmyConstructionItemInterface>{

public:
	FArmyConstructionItemInterface();
	virtual ~FArmyConstructionItemInterface(){}
public:	

	TMap<int32/*施工项ID*/, TMap<int32/*属性ID*/, int32/*属性值ID*/>> CheckedId;

	/**标记是否已经被设置过，来确定是否要使用默认值*/
	bool bHasSetted = false;

	/** 该对象所在空间ID*/
	int SpaceID = 0;

	virtual void SetConstructionItemCheckedId(TMap<int32/*施工项ID*/, TMap<int32/*属性ID*/, int32/*属性值ID*/>> &CheckedList);

	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);

	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);
};