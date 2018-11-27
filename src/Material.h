//
// Material.h - A material.
// Copyright (C) Sebastian Nordgren 
// July 12th 2018
//

#pragma once

#include "HandleManager.h"
#include "Shader.h"

#include <mutex>
#include <unordered_map>

namespace ddr
{
	struct UniformStorage;

	struct Material : dd::HandleTarget
	{
		//
		// Set the shader to use when rendering this material.
		//
		void SetShader( ShaderHandle shader_h ) { m_shader = shader_h; }

		//
		// Get the shader associated with this material.
		//
		ShaderHandle GetShader() const { return m_shader; }

		//
		// Set the current values of the material's properties to the uniform storage.
		//
		void UpdateUniforms( UniformStorage& uniforms ) const;

		//
		// Material properties - Phong shading model.
		// 
		float GetShininess() const { return m_shininess; }
		float GetSpecular() const { return m_specular; }
		float GetDiffuse() const { return m_diffuse; }
		float GetAmbient() const { return m_ambient; }

		Material();
		~Material();

		Material& operator=( const Material& ) = delete;
		Material& operator=( Material&& ) = delete;
		Material( const Material& ) = delete;
		Material( Material&& ) = delete;

	private:

		ShaderHandle m_shader;

		float m_shininess { 32.0f };
		float m_specular { 1.0f };
		float m_diffuse { 1.0f };
		float m_ambient { 1.0f };
	};

	using MaterialHandle = dd::Handle<ddr::Material>;
	using MaterialManager = dd::HandleManager<ddr::Material>;
}