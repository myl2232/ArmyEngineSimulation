#include "ArmyDrawGridHelper.h"
#include "EngineDefines.h"
#include "ArmyEditorEngine.h"

static void GetAxisColors(FLinearColor Out[3], bool b3D)
{
	if (b3D)
	{
		Out[0] = FLinearColor::Red;
		Out[1] = FLinearColor::Green;
		Out[2] = FLinearColor::Blue;
	}
	else
	{
		Out[0] = FLinearColor(FColor(150,31,0));
		Out[1] = FLinearColor(FColor(0, 150, 63));
		Out[2] = FLinearColor::Blue;
	}

	// less prominent axis lines
	for (int i = 0; i < 3; ++i)
	{
		// darker
		if (b3D)
		{
			Out[i] += FLinearColor(0.2f, 0.2f, 0.2f, 0);
			Out[i] *= 0.1f;
		}
		else
		{
			//			Out[i] += FLinearColor(0.5f, 0.5f, 0.5f, 0);
		//	Out[i] *= 0.5f;
			//Out[i] *= 0.8f;
		}
	}
}
FArmyDrawGridHelper::FArmyDrawGridHelper()
	:bDrawGrid(true)
	, bDrawPivot(false)
	, bDrawBaseInfo(true)
	, bDrawWorldBox(false)
	, bDrawKillZ(false)
	, AxesLineThickness(0.0f)
	, GridColorAxis(70, 70, 70)
    , GridColorMajor(FColor(0X343A4B))
    , GridColorMinor(FColor(0X262D37))
	, PerspectiveGridSize(HALF_WORLD_MAX1)
	, PivotColor(FColor::Red)
	, PivotSize(0.02f)
	, NumCells(64)
	, BaseBoxColor(FColor::Green)
	, DepthPriorityGroup(SDPG_World)
	, GridDepthBias(0.000001f)
{

}


FArmyDrawGridHelper::~FArmyDrawGridHelper()
{
}
void FArmyDrawGridHelper::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	if (bDrawGrid)
	{
		DrawOldGrid(View, PDI);
	}
}

void FArmyDrawGridHelper::DrawBaseInfo(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{

}
void FArmyDrawGridHelper::DrawOldGrid(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	ESceneDepthPriorityGroup eDPG = (ESceneDepthPriorityGroup)DepthPriorityGroup;

	bool bIsPerspective = (View->ViewMatrices.GetProjectionMatrix().M[3][3] < 1.0f);

	// Don't attempt to draw the grid lines from the maximum half world extent as it may be clipped due to floating point truncation errors
	const float HalfWorldExtent = HALF_WORLD_MAX - 1000.0f;

	// Draw 3D perspective grid
	if (bIsPerspective)
	{
		// @todo: Persp grid should be changed to be adaptive and use same settings as ortho grid, including grid interval!
		const int32 RangeInCells = NumCells / 2;
		const int32 MajorLineInterval = NumCells / 8;

		const int32 NumLines = NumCells + 1;
		const int32 AxesIndex = NumCells / 2;
		for (int32 LineIndex = 0; LineIndex < NumLines; ++LineIndex)
		{
			bool bIsMajorLine = ((LineIndex - RangeInCells) % MajorLineInterval) == 0;

			FVector A, B;
			A.X = (PerspectiveGridSize / 4.f)*(-1.0 + 2.0*LineIndex / NumCells);	B.X = A.X;

			A.Y = (PerspectiveGridSize / 4.f);		B.Y = -(PerspectiveGridSize / 4.f);
			A.Z = 0.0;							B.Z = 0.0;

			FColor LineColor;
			float LineThickness = 0.f;

			if (LineIndex == AxesIndex)
			{
				LineColor = GridColorAxis;
				LineThickness = AxesLineThickness;
			}
			else if (bIsMajorLine)
			{
				LineColor = GridColorMajor;
			}
			else
			{
				LineColor = GridColorMinor;
			}

			PDI->DrawLine(A, B, LineColor, eDPG, LineThickness, GridDepthBias);

			A.Y = A.X;							B.Y = B.X;
			A.X = (PerspectiveGridSize / 4.f);		B.X = -(PerspectiveGridSize / 4.f);
			PDI->DrawLine(A, B, LineColor, eDPG, LineThickness, GridDepthBias);
		}
	}
	// Draw ortho grid.
	else
	{
		const bool bIsOrthoXY = (FMath::Abs(View->ViewMatrices.GetViewMatrix().M[2][2]) > 0.0f);
		const bool bIsOrthoXZ = (FMath::Abs(View->ViewMatrices.GetViewMatrix().M[1][2]) > 0.0f);
		const bool bIsOrthoYZ = (FMath::Abs(View->ViewMatrices.GetViewMatrix().M[0][2]) > 0.0f);

		FLinearColor AxisColors[3];
		GetAxisColors(AxisColors, false);

		if (bIsOrthoXY)
		{
			const bool bNegative = View->ViewMatrices.GetViewMatrix().M[2][2] > 0.0f;

			FVector StartY(0.0f, +HalfWorldExtent, bNegative ? HalfWorldExtent : -HalfWorldExtent);
			FVector EndY(0.0f, -HalfWorldExtent, bNegative ? HalfWorldExtent : -HalfWorldExtent);
			FVector StartX(+HalfWorldExtent, 0.0f, bNegative ? HalfWorldExtent : -HalfWorldExtent);
			FVector EndX(-HalfWorldExtent, 0.0f, bNegative ? HalfWorldExtent : -HalfWorldExtent);

			DrawGridSection(GArmyEditor->GetGridSize(), &StartY, &EndY, &StartY.X, &EndY.X, 0, View, PDI);
			DrawGridSection(GArmyEditor->GetGridSize(), &StartX, &EndX, &StartX.Y, &EndX.Y, 1, View, PDI);
			DrawOriginAxisLine(&StartY, &EndY, &StartY.X, &EndY.X, View, PDI, AxisColors[1]);
			DrawOriginAxisLine(&StartX, &EndX, &StartX.Y, &EndX.Y, View, PDI, AxisColors[0]);
			/*DrawOriginAxisLine(&StartY, &EndY, &StartY.X, &EndY.X, View, PDI, FColor(240, 63, 41, 255));
			DrawOriginAxisLine(&StartX, &EndX, &StartX.Y, &EndX.Y, View, PDI, FColor(65, 160, 246, 62));*/
		}
		else if (bIsOrthoXZ)
		{
			const bool bNegative = View->ViewMatrices.GetViewMatrix().M[1][2] > 0.0f;

			FVector StartZ(0.0f, bNegative ? HalfWorldExtent : -HalfWorldExtent, +HalfWorldExtent);
			FVector EndZ(0.0f, bNegative ? HalfWorldExtent : -HalfWorldExtent, -HalfWorldExtent);
			FVector StartX(+HalfWorldExtent, bNegative ? HalfWorldExtent : -HalfWorldExtent, 0.0f);
			FVector EndX(-HalfWorldExtent, bNegative ? HalfWorldExtent : -HalfWorldExtent, 0.0f);

			DrawGridSection(GArmyEditor->GetGridSize(), &StartZ, &EndZ, &StartZ.X, &EndZ.X, 0, View, PDI);
			DrawGridSection(GArmyEditor->GetGridSize(), &StartX, &EndX, &StartX.Z, &EndX.Z, 2, View, PDI);
			DrawOriginAxisLine(&StartZ, &EndZ, &StartZ.X, &EndZ.X, View, PDI, AxisColors[2]);
			DrawOriginAxisLine(&StartX, &EndX, &StartX.Z, &EndX.Z, View, PDI, AxisColors[0]);
		}
		else if (bIsOrthoYZ)
		{
			const bool bNegative = View->ViewMatrices.GetViewMatrix().M[0][2] < 0.0f;

			FVector StartZ(bNegative ? -HalfWorldExtent : HalfWorldExtent, 0.0f, +HalfWorldExtent);
			FVector EndZ(bNegative ? -HalfWorldExtent : HalfWorldExtent, 0.0f, -HalfWorldExtent);
			FVector StartY(bNegative ? -HalfWorldExtent : HalfWorldExtent, +HalfWorldExtent, 0.0f);
			FVector EndY(bNegative ? -HalfWorldExtent : HalfWorldExtent, -HalfWorldExtent, 0.0f);

			DrawGridSection(GArmyEditor->GetGridSize(), &StartZ, &EndZ, &StartZ.Y, &EndZ.Y, 1, View, PDI);
			DrawGridSection(GArmyEditor->GetGridSize(), &StartY, &EndY, &StartY.Z, &EndY.Z, 2, View, PDI);
			DrawOriginAxisLine(&StartZ, &EndZ, &StartZ.Y, &EndZ.Y, View, PDI, AxisColors[2]);
			DrawOriginAxisLine(&StartY, &EndY, &StartY.Z, &EndY.Z, View, PDI, AxisColors[1]);
		}

		if (bDrawKillZ && (bIsOrthoXZ || bIsOrthoYZ) && GWorld->GetWorldSettings()->bEnableWorldBoundsChecks)
		{
			float KillZ = GWorld->GetWorldSettings()->KillZ;

			PDI->DrawLine(FVector(-HalfWorldExtent, 0, KillZ), FVector(HalfWorldExtent, 0, KillZ), FColor::Red, SDPG_Foreground);
			PDI->DrawLine(FVector(0, -HalfWorldExtent, KillZ), FVector(0, HalfWorldExtent, KillZ), FColor::Red, SDPG_Foreground);
		}
	}

	// Draw orthogonal worldframe.
	if (bDrawWorldBox)
	{
		DrawWireBox(PDI, FBox(FVector(-HalfWorldExtent, -HalfWorldExtent, -HalfWorldExtent), FVector(HalfWorldExtent, HalfWorldExtent, HalfWorldExtent)), GEngine->C_WorldBox, eDPG);
	}
}

void FArmyDrawGridHelper::DrawGridSection(float ViewportGridY, FVector* A, FVector* B, float* AX, float* BX, int32 Axis, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	if (ViewportGridY == 0)
	{
		// Don't draw zero-size grid
		return;
	}

	// todo
//	int32 Exponent = GEditor->IsGridSizePowerOfTwo() ? 8 : 10;
	int32 Exponent = 10;
	const float SizeX = View->ViewRect.Width();
	const float Zoom = (1.0f / View->ViewMatrices.GetProjectionMatrix().M[0][0]) * 2.0f / SizeX;
	const float Dist = SizeX * Zoom / ViewportGridY;

	// when the grid fades
	static float Tweak = 8.0f;

	float IncValue = FMath::LogX(Exponent, Dist / (float)(SizeX / Tweak));
	int32 IncScale = 1;

	for (float x = 0; x < IncValue; ++x)
	{
		IncScale *= Exponent;
	}

	if (IncScale == 0)
	{
		// Prevent divide by zero
		return;
	}

	// 0 excluded for hard transitions .. 0.5f for very soft transitions
	const float TransitionRegion = 0.5f;

	const float InvTransitionRegion = 1.0f / TransitionRegion;
	float Fract = IncValue - floorf(IncValue);
	float AlphaA = FMath::Clamp(Fract * InvTransitionRegion, 0.0f, 1.0f);
	float AlphaB = FMath::Clamp(InvTransitionRegion - Fract * InvTransitionRegion, 0.0f, 1.0f);

	if (IncValue < -0.5f)
	{
		// no fade in magnification case
		AlphaA = 1.0f;
		AlphaB = 1.0f;
	}
	//	const int32 MajorLineInterval = FMath::TruncToInt(GEditor->GetGridInterval());
	const int32 MajorLineInterval = FMath::TruncToInt(5);

	const FLinearColor Background = View->BackgroundColor;
	// 0.05  and 0.02
	//FLinearColor MajorColor = FMath::Lerp(Background, FLinearColor::White, 0.1f);
	//FLinearColor MinorColor = FMath::Lerp(Background, FLinearColor::White, 0.04f);
	FLinearColor MajorColor = FMath::Lerp(Background, FLinearColor::White, 0.03f);
	FLinearColor MinorColor(FColor(30, 36, 43));

	const FMatrix InvViewProjMatrix = View->ViewMatrices.GetInvViewProjectionMatrix();
	int32 FirstLine = FMath::TruncToInt(InvViewProjMatrix.TransformPosition(FVector(-1, -1, 0.5f)).Component(Axis) / ViewportGridY);
	int32 LastLine = FMath::TruncToInt(InvViewProjMatrix.TransformPosition(FVector(+1, +1, 0.5f)).Component(Axis) / ViewportGridY);
	if (FirstLine > LastLine)
	{
		Exchange(FirstLine, LastLine);
	}

	// Draw major and minor grid lines
	const int32 FirstLineClamped = FMath::Max<int32>(FirstLine - 1, -HALF_WORLD_MAX / ViewportGridY) / IncScale;
	const int32 LastLineClamped = FMath::Min<int32>(LastLine + 1, +HALF_WORLD_MAX / ViewportGridY) / IncScale;
	for (int32 LineIndex = FirstLineClamped; LineIndex <= LastLineClamped; ++LineIndex)
	{
		*AX = FPlatformMath::TruncToFloat(LineIndex * ViewportGridY) * IncScale;
		*BX = FPlatformMath::TruncToFloat(LineIndex * ViewportGridY) * IncScale;

		// Only minor lines fade out with ortho zoom distance.  Origin lines and major lines are drawn
		// at 100% opacity, but with a brighter value
		const bool bIsMajorLine = (MajorLineInterval == 0) || ((LineIndex % MajorLineInterval) == 0);

		{
			// Don't bother drawing the world origin line.  We'll do that later.
			const bool bIsOriginLine = (LineIndex == 0);
			if (!bIsOriginLine)
			{
				FLinearColor Color;
				if (bIsMajorLine)
				{
					Color = FMath::Lerp(Background, MajorColor, AlphaA);
					PDI->DrawLine(*A, *B, Color, SDPG_World);
				}
				else
				{
					Color = FMath::Lerp(Background, MinorColor, AlphaB);
					PDI->DrawLine(*A, *B, Color, SDPG_World, 0.0f, -0.1f);
				}


			}
		}
	}
}

void FArmyDrawGridHelper::DrawOriginAxisLine(FVector* A, FVector* B, float* AX, float* BX, const FSceneView* View, FPrimitiveDrawInterface* PDI, const FLinearColor& Color)
{
	// Draw world origin lines.  We draw these last so they appear on top of the other lines.
	*AX = 0;
	*BX = 0;

	PDI->DrawLine(*A, *B, FLinearColor(Color.Quantize()), SDPG_World);
}

void FArmyDrawGridHelper::DrawPivot(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	const FMatrix CameraToWorld = View->ViewMatrices.GetInvViewMatrix();

	//const FVector PivLoc = GLevelEditorModeTools().SnappedLocation;
	const FVector PivLoc = FVector(0, 0, 0);
	const float ZoomFactor = FMath::Min<float>(View->ViewMatrices.GetProjectionMatrix().M[0][0], View->ViewMatrices.GetProjectionMatrix().M[1][1]);
	const float WidgetRadius = View->ViewMatrices.GetViewProjectionMatrix().TransformPosition(PivLoc).W * (PivotSize / ZoomFactor);

	const FVector CamX = CameraToWorld.TransformVector(FVector(1, 0, 0));
	const FVector CamY = CameraToWorld.TransformVector(FVector(0, 1, 0));


	PDI->DrawLine(PivLoc - (WidgetRadius*CamX), PivLoc + (WidgetRadius*CamX), PivotColor, SDPG_Foreground);
	PDI->DrawLine(PivLoc - (WidgetRadius*CamY), PivLoc + (WidgetRadius*CamY), PivotColor, SDPG_Foreground);
}
