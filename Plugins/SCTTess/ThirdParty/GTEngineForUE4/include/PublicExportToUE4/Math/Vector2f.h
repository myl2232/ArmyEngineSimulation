#pragma once

#include "Prerequisites/PrerequisitesUtil.h"

namespace GTForUE4
{
	class GTFORUE4_EXPORT Vector2f
	{		
	public:
		float x, y;

	public:
		Vector2f() = default;

		constexpr Vector2f(float x, float y)
			: x(x), y(y)
		{ }

	};

}
