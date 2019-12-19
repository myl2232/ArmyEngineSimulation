// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Tickable.h"

class FSCTShape;
class SCTSHAPEMODULE_API FAnimation : public FTickableGameObject
{
public:
	FAnimation();
	virtual ~FAnimation();

	virtual void Tick(float DeltaTime) override;
	virtual void AnimationTick(float DeltaTime);

	FSCTShape* ShapeRef;
	
	void Stop();
	void Start();
	void SetShapePos(const FVector& InPos);
	void SetShapeRotator(const FRotator& InRotator);
	
	void InitDefault(FSCTShape*);
	void ReinitDefault();

	bool IsInitState() const;

	virtual bool IsTickable() const override
	{
		return State == 1;
	}

	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(OSVREntryPoint, STATGROUP_Tickables);
	}

	int32		State;
	int32		IsReverse;
	
	float		Duration;
	float		CurTime;
	

	FVector OriginalPos;
	FVector AnimationStartPos;
	FRotator AnimationStartRot;

	FVector AnimationInitPos;
	FRotator AnimationInitRot;

	
};

class SCTSHAPEMODULE_API FLeftOpenDoorAnimation : public FAnimation
{
public:
	FLeftOpenDoorAnimation();

	void SetAnimationParams(float InAngle);
	virtual void AnimationTick(float DeltaTime) override;

private:
	float 		Angle;
};

class SCTSHAPEMODULE_API FRightOpenDoorAnimation : public FAnimation
{
public:
	FRightOpenDoorAnimation();

	void SetAnimationParams(float InAngle);
	virtual void AnimationTick(float DeltaTime) override;

private:
	float 		Angle;
};

class SCTSHAPEMODULE_API FTopOpenDoorAnimation : public FAnimation
{
public:
	FTopOpenDoorAnimation();

	void SetAnimationParams(float InAngle);
	virtual void AnimationTick(float DeltaTime) override;

private:
	float 		Angle;
};

class SCTSHAPEMODULE_API FDrawerAnimation : public FAnimation
{
public:
	FDrawerAnimation();

	virtual void AnimationTick(float DeltaTime) override;
};
