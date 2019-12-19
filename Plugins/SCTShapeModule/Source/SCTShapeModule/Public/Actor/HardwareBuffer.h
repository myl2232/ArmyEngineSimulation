/**
 * Copyright 2018 ������Ƽ����޹�˾.
 * All Rights Reserved.
 * 
 *
 * @File HardwareBuffer.h
 * @Description Ӳ������
 *
 * @Author ��Ԯ
 * @Date 2018��5��4��
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



