#pragma once

namespace EArmyLevelViewportType
{
	UENUM()
	enum Type
	{
		/** Top */
		LVT_OrthoXY = 0,
		/** Front */
		LVT_OrthoXZ = 1,
		/** Left */
		LVT_OrthoYZ = 2,
		LVT_Perspective = 3,
		LVT_OrthoFreelook = 4,
		/** Bottom */
		LVT_OrthoNegativeXY = 5,
		/** Back */
		LVT_OrthoNegativeXZ = 6,
		/** Right */
		LVT_OrthoNegativeYZ = 7,
		LVT_OnlyCanvas = 8,
		LVT_MAX,

		LVT_None = 255,
	};
}
