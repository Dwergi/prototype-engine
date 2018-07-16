//
// Material.cpp - A material.
// Copyright (C) Sebastian Nordgren 
// July 12th 2018
//

#include "PrecompiledHeader.h"
#include "Material.h"

#include "Uniforms.h"

namespace ddr
{
	std::unordered_map<uint64, Material*> Material::s_instances;
	std::mutex Material::s_instancesMutex;

	Material::Material()
	{
		
	}

	Material::~Material()
	{
		
	}

	MaterialHandle Material::Create( const char* name )
	{
		std::lock_guard lock( s_instancesMutex );

		uint64 hash = dd::HashString( name, strlen( name ) );

		auto it = s_instances.find( hash );
		if( it == s_instances.end() )
		{
			it = s_instances.insert( std::make_pair( hash, new Material() ) ).first;
		}

		MaterialHandle handle;
		handle.m_hash = hash;
		return handle;
	}

	Material* Material::Get( MaterialHandle material_h )
	{
		std::lock_guard lock( s_instancesMutex );

		auto it = s_instances.find( material_h.m_hash );
		if( it == s_instances.end() )
		{
			return nullptr;
		}

		return it->second;
	}

	void Material::Destroy( MaterialHandle material_h )
	{
		std::lock_guard lock( s_instancesMutex );

		auto it = s_instances.find( material_h.m_hash );
		if( it != s_instances.end() )
		{
			s_instances.erase( it );
		}
	}

	void Material::UpdateUniforms( UniformStorage& uniforms ) const
	{
		uniforms.Set( "Material.Shininess", m_shininess );
		uniforms.Set( "Material.Specular", m_specular );
		uniforms.Set( "Material.Diffuse", m_diffuse );
		uniforms.Set( "Material.Ambient", m_ambient );
	}
}