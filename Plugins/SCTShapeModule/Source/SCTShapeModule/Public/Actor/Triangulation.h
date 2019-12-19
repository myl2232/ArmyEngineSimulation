/**
 * Copyright 2018 ������Ƽ����޹�˾.
 * All Rights Reserved.
 * 
 *
 * @File Triangulation.h
 * @Description ���з������ʷ�
 *
 * @Author ��Ԯ
 * @Date 2018��5��4��
 * @Version 1.0
 */
#pragma once
#include "CoreMinimal.h"

namespace SCT
{
	bool TriangulatePoly(TArray<uint16>& OutIndexList, const TArray<FVector>& InPointList, const FVector& InFaceNormal, bool bKeepColinearVertices = false);
}