//
// TerrainParameters.h - Parameters for procedural terrain.
// Copyright (C) Sebastian Nordgren 
// January 14th 2018
//

#pragma once

namespace ddr
{
	struct UniformStorage;
}

namespace dd
{
	struct TerrainParameters
	{
		//
		// The number of octaves of noise.
		//
		static const int Octaves = 6;

		//
		// Distance between vertices on the chunk.
		//
		float VertexDistance { 1.0f };

		//
		// The maximum height range of vertices. Heights will vary between 0 and this.
		//
		float HeightRange { 64.0f };

		//
		// The wavelength of the noise. 
		// Smaller numbers lead to higher frequency noise and more jagged terrain.
		// Larger numbers lead to smooth rolling hills.
		//
		float Wavelength { 96.0f };

		//
		// A seed for noise generation to introduce some variation.
		//
		float Seed { 1.0f };

		//
		// The amplitudes of noise to apply at each octave.
		//
		float Amplitudes[Octaves] = { 0.5f, 0.25f, 0.125f, 0.0625f, 0.03f, 0.015f };

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

		//
		// Should the terrain chunks be visualized by their ID?
		//
		bool UseDebugColours { false };

		void UpdateUniforms( ddr::UniformStorage& uniforms );
	};
}