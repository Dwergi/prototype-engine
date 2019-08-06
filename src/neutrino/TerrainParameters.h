//
// TerrainParameters.h - Parameters for procedural terrain.
// Copyright (C) Sebastian Nordgren 
// January 14th 2018
//

#pragma once

#include "Noise.h"

namespace ddr
{
	struct UniformStorage;
}

namespace neut
{
	struct TerrainParameters
	{
		ddm::NoiseParameters Noise;
		
		//
		// Size of a chunk in metres in one dimension.
		//
		float ChunkSize { 64.0f };

		//
		// The maximum height range of vertices. Heights will vary between 0 and this.
		//
		float HeightRange { 64.0f };

		//
		// The number of height levels for colouring.
		//
		static const int HeightLevelCount = 5;

		//
		// The colours to display at each height level.
		//
		glm::vec3 HeightColours[HeightLevelCount] =
		{
			glm::vec3( 0.25f, 0.8f, 0.25f ), // green
			glm::vec3( 0.25f, 0.5f, 0.25f ), // darker green
			glm::vec3( 0.6f, 0.4f, 0.1f ), // brown
			glm::vec3( 0.5f, 0.5f, 0.5f ), // grey
			glm::vec3( 1.0f, 1.0f, 1.0f ) // white
		};

		//
		// The cutoff points for the heights, expressed as a fraction of the maximum height range.
		//
		float HeightCutoffs[HeightLevelCount] =
		{
			0.0f,
			0.2f,
			0.4f,
			0.6f,
			1.0f
		};

		static const int LODs = 6;

		float LODSwitchDistances[ LODs ] =
		{
			64.0f,
			128.0f,
			256.0f,
			512.0f,
			1024.0f,
			2048.0f
		};

		//
		// Should the terrain chunks be visualized by their ID?
		//
		bool UseDebugColours { false };

		//
		// Update the uniforms that map to the terrain parameters.
		//
		void UpdateUniforms( ddr::UniformStorage& uniforms ) const;
	};
}