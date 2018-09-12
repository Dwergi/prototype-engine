#pragma once

#include "ComponentType.h"
#include "Ray.h"

namespace dd
{
	struct RayComponent
	{
		Ray Ray;

		DD_COMPONENT;
	};
}
