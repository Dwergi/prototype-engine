//
// Noise.cpp - Parameters for noise generation.
// Copyright (C) Sebastian Nordgren 
// December 3rd 2018
//

#include "PCH.h"
#include "Noise.h"

#include "glm/gtc/noise.hpp"

namespace ddm
{
	float GetNoise( glm::vec2 pos, const NoiseParameters& params )
	{
		float height = 0;
		float wavelength = params.Wavelength;

		float total_amplitude = 0;

		for( int i = 0; i < params.Octaves; ++i )
		{
			float multiplier = 1.f / wavelength;
			glm::vec3 coord( pos.x * multiplier, pos.y * multiplier, params.Seed );

			float noise = glm::simplex( coord );
			height += noise * params.Amplitudes[i];

			total_amplitude += params.Amplitudes[i];
			wavelength /= 2;
		}

		float normalized = height / total_amplitude;
		return normalized;
	}

	void GenerateNoise( std::vector<float>& data, const glm::ivec2 dimensions, const glm::ivec2 stride,
		const glm::vec2 offset, const glm::vec2 increment, const NoiseParameters& params )
	{
		data.clear();
		data.resize( dimensions.x * dimensions.y );
		
		float previous = -100.0f;

		for( int y = 0; y < dimensions.y; y += stride.y )
		{
			for( int x = 0; x < dimensions.x; x += stride.x )
			{
				const glm::vec2 pos( offset.x + x * increment.x, offset.y + y * increment.y );
				
				float height = GetNoise( pos, params );

				// height is y
				float normalized_height = (1 + height) / 2;
				DD_ASSERT( normalized_height >= 0 && normalized_height <= 1 );

				const int current = y * dimensions.y + x;
				DD_ASSERT( current < data.size() );

				data[current] = normalized_height;
			}
		}
	}
}