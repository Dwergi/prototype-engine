//
// Material.cpp - A material.
// Copyright (C) Sebastian Nordgren 
// July 12th 2018
//

#include "PCH.h"
#include "Material.h"

#include "Uniforms.h"

DD_HANDLE_MANAGER( ddr::Material );

namespace ddr
{
	Material::Material()
	{
		
	}

	Material::~Material()
	{
		
	}

	void Material::UpdateUniforms( UniformStorage& uniforms ) const
	{
		uniforms.Set( "Material.Shininess", Shininess );
		uniforms.Set( "Material.Specular", Specular );
		uniforms.Set( "Material.Diffuse", Diffuse );
		uniforms.Set( "Material.Ambient", Ambient );
	}
}