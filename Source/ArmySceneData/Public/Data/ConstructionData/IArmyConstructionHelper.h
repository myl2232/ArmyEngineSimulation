/**
* Copyright 2019 ������Ƽ����޹�˾.
* All Rights Reserved.
* 
*
* @File IArmyConstructionHelper.h
* @Description ����ʩ�������Ľӿ�
*
* @Author ������
* @Date 2019��4��4��
* @Version 1.1
*/
#pragma once

#include "Runtime/Core/Public/Templates/SharedPointer.h"
class FArmyRoom;

class  IArmyConstructionHelper
{
public:
	//��ȡ���ڵķ���
	virtual TSharedPtr<FArmyRoom> GetRoom()=0;

	//��ȡ�ռ�ID
	ARMYSCENEDATA_API int32 GetRoomSpaceID();

	virtual void OnRoomSpaceIDChanged(int32 NewSpaceID)=0;
};