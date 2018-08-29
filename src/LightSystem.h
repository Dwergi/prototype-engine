#pragma once

#include "System.h"

namespace ddr
{
	struct UniformStorage;

	struct LightSystem : ddc::System
	{
		LightSystem( ddr::UniformStorage& uniforms );

		void Update( const ddc::UpdateData& data, dd::seconds delta_t );

	private:

		UniformStorage& m_uniforms;
	};
}