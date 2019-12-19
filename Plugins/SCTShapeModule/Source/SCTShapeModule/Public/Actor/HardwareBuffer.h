/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File HardwareBuffer.h
 * @Description 硬件缓冲
 *
 * @Author 曾援
 * @Date 2018年5月4日
 * @Version 1.0
 */
#pragma once

#include "RHI.h"
#include "RenderResource.h"
#include "LocalVertexFactory.h"
#include "PrimitiveSceneProxy.h"
#include "DynamicMeshBuilder.h"

class FDynVertexBuffer : public FVertexBuffer 
{
public:
	TArray<FDynamicMeshVertex> Vertices;

	virtual void InitRHI() override;
};

class FUint16IndexBuffer : public FIndexBuffer
{
public:
	TArray<uint16> Indices;

	virtual void InitRHI() override;
};



template <class VertexType>
class TTypedVertexFactory : public FLocalVertexFactory 
{
public:
	void Init(const FDynVertexBuffer *InVertexBuffer) {}
};



