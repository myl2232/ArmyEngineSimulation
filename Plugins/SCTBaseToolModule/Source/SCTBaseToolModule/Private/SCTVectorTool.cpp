#pragma once

#include "SCTVectorTool.h"



bool  LineBoxIntersectionWithHitPoint(
	const FBox& Box,
	const FVector& Start,
	const FVector& End,
	const FVector& Direction,
	FVector& OutHit)
{
	FVector OneOverDirection = Direction.Reciprocal();

	FVector	Time;
	bool bStartIsOutside = false;

	if (Start.X < Box.Min.X)
	{
		bStartIsOutside = true;
		if (End.X >= Box.Min.X)
		{
			Time.X = (Box.Min.X - Start.X) * OneOverDirection.X;
		}
		else
		{
			return false;
		}
	}
	else if (Start.X > Box.Max.X)
	{
		bStartIsOutside = true;
		if (End.X <= Box.Max.X)
		{
			Time.X = (Box.Max.X - Start.X) * OneOverDirection.X;
		}
		else
		{
			return false;
		}
	}
	else
	{
		Time.X = 0.0f;
	}

	if (Start.Y < Box.Min.Y)
	{
		bStartIsOutside = true;
		if (End.Y >= Box.Min.Y)
		{
			Time.Y = (Box.Min.Y - Start.Y) * OneOverDirection.Y;
		}
		else
		{
			return false;
		}
	}
	else if (Start.Y > Box.Max.Y)
	{
		bStartIsOutside = true;
		if (End.Y <= Box.Max.Y)
		{
			Time.Y = (Box.Max.Y - Start.Y) * OneOverDirection.Y;
		}
		else
		{
			return false;
		}
	}
	else
	{
		Time.Y = 0.0f;
	}

	if (Start.Z < Box.Min.Z)
	{
		bStartIsOutside = true;
		if (End.Z >= Box.Min.Z)
		{
			Time.Z = (Box.Min.Z - Start.Z) * OneOverDirection.Z;
		}
		else
		{
			return false;
		}
	}
	else if (Start.Z > Box.Max.Z)
	{
		bStartIsOutside = true;
		if (End.Z <= Box.Max.Z)
		{
			Time.Z = (Box.Max.Z - Start.Z) * OneOverDirection.Z;
		}
		else
		{
			return false;
		}
	}
	else
	{
		Time.Z = 0.0f;
	}

	if (bStartIsOutside)
	{
		const float	MaxTime = FMath::Max3(Time.X, Time.Y, Time.Z);

		if (MaxTime >= 0.0f && MaxTime <= 1.0f)
		{
			OutHit = Start + Direction * MaxTime;
			const float BOX_SIDE_THRESHOLD = 0.1f;
			if (OutHit.X > Box.Min.X - BOX_SIDE_THRESHOLD && OutHit.X < Box.Max.X + BOX_SIDE_THRESHOLD &&
				OutHit.Y > Box.Min.Y - BOX_SIDE_THRESHOLD && OutHit.Y < Box.Max.Y + BOX_SIDE_THRESHOLD &&
				OutHit.Z > Box.Min.Z - BOX_SIDE_THRESHOLD && OutHit.Z < Box.Max.Z + BOX_SIDE_THRESHOLD)
			{
				return true;
			}
		}

		return false;
	}
	else
	{
		return true;
	}
}

bool  GetPositionOnBBoxFromScreenPos(const FVector2D &ScreenLocation, float InBBoxWidth, float InBBoxDepth, float InBBoxHeight, FVector &OutHit)
{
	FVector Start, End, Dir;
	UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(GWorld, 0), ScreenLocation, Start, Dir);
	End = Start + Dir * 10000.0f;

	enum EBoxDirection
	{
		BD_Bottom = 0,
		BD_Top,
		BD_Left,
		BD_Right,
		BD_Back,
		BD_Front
	};

	FBox Boxes[] = {
		{ FVector(0.0f, 0.0f, 0.0f), FVector(InBBoxWidth, InBBoxDepth, 1.0f) / 10.0f }, // Bottom
		{ FVector(0.0f, 0.0f, InBBoxHeight - 1.0f) / 10.0f, FVector(InBBoxWidth, InBBoxDepth, InBBoxHeight) / 10.0f }, // Top
		{ FVector(0.0f, 0.0f, 0.0f), FVector(1.0f, InBBoxDepth, InBBoxHeight) / 10.0f }, // Left
		{ FVector(InBBoxWidth - 1.0f, 0.0f, 0.0f) / 10.0f, FVector(InBBoxWidth, InBBoxDepth, InBBoxHeight) / 10.0f }, // Right
		{ FVector(0.0f, 0.0f, 0.0f), FVector(InBBoxWidth, 1.0f, InBBoxHeight) / 10.0f }, // Back
		{ FVector(0.0f, InBBoxDepth - 1.0f, 0.0f) / 10.0f, FVector(InBBoxWidth, InBBoxDepth, InBBoxHeight) / 10.0f } // Front
	};

	float HitPointDistanceFromEye = MAX_FLT;
	bool bHit = false;
	for (int32 i = BD_Back; i >= 0; --i)
	{
		FVector HitPoint;
		if (LineBoxIntersectionWithHitPoint(Boxes[i], Start, End, End - Start, HitPoint))
		{
			bHit = true;
			float Distance = (Start - HitPoint).Size();
			if (HitPointDistanceFromEye > Distance)
			{
				HitPointDistanceFromEye = Distance;
				OutHit = HitPoint;
			}
		}
	}

	return bHit;
}

int32  GetCameraMostlyFacingPlane(float InBBoxWidth, float InBBoxDepth, float InBBoxHeight, FPlane &OutPlane)
{
	FRotator CameraRotation = UGameplayStatics::GetPlayerController(GWorld, 0)->PlayerCameraManager->GetCameraRotation();
	FVector CameraLookDirection = CameraRotation.RotateVector(FVector::ForwardVector);
	FVector SixStandardDirection[] = {
		FVector::ForwardVector,
		-FVector::ForwardVector,
		FVector::RightVector,
		-FVector::RightVector,
		FVector::UpVector,
		-FVector::UpVector
	};
	float SixPlaneDistance[] = {
		InBBoxWidth / 10.0f,
		0.0f,
		InBBoxDepth / 10.0f,
		0.0f,
		InBBoxHeight / 10.0f,
		0.0f
	};
	int32 Result = -1;
	float MaxAngle = -MAX_FLT;
	for (int32 i = 0; i < 6; ++i)
	{
		float Angle = FMath::Abs(FMath::Acos(FVector::DotProduct(CameraLookDirection, SixStandardDirection[i])));
		if (Angle > MaxAngle)
		{
			MaxAngle = Angle;
			Result = i;
		}
	}
	OutPlane = FPlane(SixStandardDirection[Result], SixPlaneDistance[Result]);
	return Result;
}