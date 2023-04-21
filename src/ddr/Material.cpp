//
// Material.cpp - A material.
// Copyright (C) Sebastian Nordgren 
// July 12th 2018
//

#include "PCH.h"
#include "Material.h"

#include "Uniforms.h"

DD_POD_CPP(ddr::MaterialHandle);

namespace ddr
{
	static dd::ProfilerValueRef g_materialChanged("Material Changed");

	Material::Material()
	{
	}

	Material::~Material()
	{

	}

	void Material::Bind(UniformStorage& uniforms)
	{
		DD_ASSERT(!m_inUse, "Material already in use when bound!");

		g_materialChanged.Increment();

		m_inUse = true;

		uniforms.Set("Material.Shininess", Shininess);
		uniforms.Set("Material.Specular", Specular);
		uniforms.Set("Material.Diffuse", Diffuse);
		uniforms.Set("Material.Ambient", Ambient);

		uniforms.Upload(*Shader);

		Shader->Use(true);
		
		State.Use(true);
	}

	void Material::Unbind(UniformStorage& uniforms)
	{
		DD_ASSERT(m_inUse, "Material not in use when unbound!");

		Shader->Use(false);

		State.Use(false);

		m_inUse = false;
	}
}
