/**
* Copyright 2019 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File IArmyConstructionHelper.h
* @Description 含有施工项对象的接口
*
* @Author 郭子阳
* @Date 2019年4月4日
* @Version 1.1
*/
#pragma once

#include "Runtime/Core/Public/Templates/SharedPointer.h"
class FArmyRoom;

class  IArmyConstructionHelper
{
public:
	//获取所在的房间
	virtual TSharedPtr<FArmyRoom> GetRoom()=0;

	//获取空间ID
	ARMYSCENEDATA_API int32 GetRoomSpaceID();

	virtual void OnRoomSpaceIDChanged(int32 NewSpaceID)=0;
};