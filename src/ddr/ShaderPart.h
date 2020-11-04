//
// Shader.h - Wrapper around an OpenGL shader.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#pragma once

#include "OpenGL.h"
#include "Uniforms.h"

namespace ddr
{
	struct ShaderPart
	{
	public:

		enum class Type
		{
			Vertex,
			Pixel,
			Geometry
		};

		//
		// Create a shader with the given name and type.
		// Load the contents from the given path.
		//
		static ShaderPart* Create(std::string_view path, Type type);

		//
		// Reload the shader from file.
		//
		bool Reload();

		~ShaderPart();

		ShaderPart(const ShaderPart& other) = delete;
		ShaderPart(ShaderPart&& other) = delete;
		ShaderPart& operator=(const ShaderPart& other) = delete;
		ShaderPart& operator=(ShaderPart&& other) = delete;

	private:

		friend struct Shader;

		Type m_type;
		GLuint m_id { OpenGL::InvalidID };

		UniformStorage m_uniforms;

		std::string m_path;
		std::string m_source;

		static std::unordered_map<std::string, ShaderPart*> sm_shaderCache;

		static bool LoadFile(std::string_view path, std::string& outSource);
		static std::string ProcessIncludes(std::string_view path, std::string_view source);
		static void GatherUniforms(std::string_view src, UniformStorage& outUniforms);

		ShaderPart(std::string_view path, Type type);

		std::string Compile(const std::string& source);
	};
}