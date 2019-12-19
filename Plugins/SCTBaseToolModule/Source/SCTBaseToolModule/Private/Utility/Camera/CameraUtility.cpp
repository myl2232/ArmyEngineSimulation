#pragma once

#include "Utility/Camera/CameraUtility.h"

namespace  SCTUtility
{
	void FCameraUtility::CalOrthoProjcetionClipWidthAndHeight(const float InW, const float InD, const float InH, const FTransform & InCameraTransform, float & OutW, float & OutH)
	{
		const TArray<FVector> SrcBoundBoxVertexArray = {
			{ 0.0f,0.0f,0.0f, }, // 左下后
			{ 0.0f,InD,0.0f },	 // 左下前
			{ InW,InD,0.0f },	 // 右下前
			{ InW,0.0f,0.0f },   // 右下后
			{ 0.0f,0.0f,InH },	 // 左上后
			{ 0.0f,InD,InH },	 // 左上前
			{ InW,InD,InH },	 // 右上前
			{ InW,0.0,InH }		 // 右上后
		};
		const FMatrix CameraInverseMatrix = InCameraTransform.Inverse().ToMatrixWithScale();
		TArray<FVector> CameraCoordVertexArray;
		for (const auto & Ref : SrcBoundBoxVertexArray)
		{
			CameraCoordVertexArray.Emplace(CameraInverseMatrix.TransformVector(Ref));
		}
		const FBox CameraCoordBox(CameraCoordVertexArray);
		OutW = CameraCoordBox.GetSize().X;
		OutH = CameraCoordBox.GetSize().Z;
	}
}