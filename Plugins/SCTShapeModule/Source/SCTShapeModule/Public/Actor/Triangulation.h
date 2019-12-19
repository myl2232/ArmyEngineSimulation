/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File Triangulation.h
 * @Description 耳切法三角剖分
 *
 * @Author 曾援
 * @Date 2018年5月4日
 * @Version 1.0
 */
#pragma once
#include "CoreMinimal.h"

namespace SCT
{
	bool TriangulatePoly(TArray<uint16>& OutIndexList, const TArray<FVector>& InPointList, const FVector& InFaceNormal, bool bKeepColinearVertices = false);
}