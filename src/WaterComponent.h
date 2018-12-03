#pragma once

#include "Mesh.h"

namespace dd
{
	struct WaterComponent
	{
		ddr::MeshHandle Mesh;

		DD_CLASS( dd::WaterComponent )
		{
			DD_COMPONENT();
		}
	};
}