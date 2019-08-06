//
// TerrainParameters.cpp - Parameters for procedural terrain.
// Copyright (C) Sebastian Nordgren 
// January 14th 2018
//

#include "PCH.h"
#include "neutrino/TerrainParameters.h"

#include "OpenGL.h"
#include "Uniforms.h"

namespace neut
{
	void TerrainParameters::UpdateUniforms( ddr::UniformStorage& uniforms ) const
	{
		for( int i = 0; i < HeightLevelCount; ++i )
		{
			uniforms.Set( ddr::GetArrayUniformName( "TerrainHeightLevels", i, "Colour" ).c_str(), HeightColours[i] );
			uniforms.Set( ddr::GetArrayUniformName( "TerrainHeightLevels", i, "Cutoff" ).c_str(), HeightCutoffs[i] );
		}

		uniforms.Set( "TerrainHeightCount", HeightLevelCount );
		uniforms.Set( "TerrainMaxHeight", HeightRange );
	}
}