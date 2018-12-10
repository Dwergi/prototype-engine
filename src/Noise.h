//
// Noise.h - Parameters for noise generation.
// Copyright (C) Sebastian Nordgren 
// December 3rd 2018
//

#pragma once

namespace ddm
{
	struct NoiseParameters
	{
		//
		// The maximum number of octaves of noise.
		//
		static const int MaxOctaves = 10;

		//
		// The current number of octaves to use.
		//
		int Octaves = 6;

		//
		// The wavelength of the noise. 
		// Smaller numbers lead to higher frequency noise and more jagged terrain.
		// Larger numbers lead to smooth rolling hills.
		//
		float Wavelength { 96.0f };

		//
		// A seed for noise generation to introduce some variation.
		//
		float Seed { 1.01f };

		//
		// The amplitudes of noise to apply at each octave.
		//
		float Amplitudes[MaxOctaves] = { 0.5f, 0.25f, 0.125f, 0.0625f, 0.03f, 0.015f, 0, 0, 0, 0 };
	};

	void GenerateNoise( std::vector<float>& data, const glm::ivec2 dimensions, const glm::ivec2 stride, 
		const glm::vec2 offset, const glm::vec2 increment, const NoiseParameters& params );
}