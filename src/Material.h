//
// Material.h - A material.
// Copyright (C) Sebastian Nordgren 
// July 12th 2018
//

#pragma once

#include "HandleManager.h"
#include "RenderState.h"
#include "Shader.h"

#include <mutex>
#include <unordered_map>

namespace ddr
{
	struct UniformStorage;

	struct Material : dd::HandleTarget
	{
		//
		// Set the current values of the material's properties to the uniform storage.
		//
		void UpdateUniforms( UniformStorage& uniforms ) const;

		//
		// Material properties - Phong shading model.
		// 
		float Shininess { 32 };
		float Specular { 1 };
		float Diffuse { 1 };
		float Ambient { 1 };

		RenderState State;
		ShaderHandle Shader;

		Material();
		~Material();

		Material& operator=( const Material& ) = delete;
		Material& operator=( Material&& ) = delete;
		Material( const Material& ) = delete;
		Material( Material&& ) = delete;
	};

	using MaterialHandle = dd::Handle<ddr::Material>;
	using MaterialManager = dd::HandleManager<ddr::Material>;
}