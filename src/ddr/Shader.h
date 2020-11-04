//
// ddr::ShaderProgram.h - Wrapper around OpenGL shader program.
// Copyright (C) Sebastian Nordgren 
// February 16th 2016
//

#pragma once

#include "HandleManager.h"
#include "OpenGL.h"

#include <string_view>

namespace ddr
{
	struct ShaderPart;
	struct Texture;
	struct UniformStorage;
	struct Shader;
	struct ShaderManager;

	struct ScopedShader
	{
		ScopedShader(Shader& shader);
		ScopedShader(ScopedShader&& other) noexcept : m_shader(other.m_shader) { other.m_shader = nullptr; }
		~ScopedShader();

	private:
		Shader* m_shader { nullptr };
	};

	enum class Normalized
	{
		Yes,
		No
	};

	enum class Instanced
	{
		Yes,
		No
	};

	struct Shader : dd::HandleTarget
	{
		bool Reload();

		ScopedShader UseScoped();

		void Use(bool use);
		bool InUse() const;

		bool IsValid() const { return m_valid; }

		void SetUniform(std::string_view name, const glm::mat3& mat);
		void SetUniform(std::string_view name, const glm::mat4& mat);
		void SetUniform(std::string_view name, const glm::vec2& vec);
		void SetUniform(std::string_view name, const glm::vec3& vec);
		void SetUniform(std::string_view name, const glm::vec4& vec);
		void SetUniform(std::string_view name, int i);
		void SetUniform(std::string_view name, bool b);
		void SetUniform(std::string_view name, float flt);
		void SetUniform(std::string_view name, const Texture& texture);

		bool BindPositions();
		bool BindNormals();
		bool BindUVs();
		bool BindVertexColours();

		bool BindAttributeFloat(std::string_view name, uint components, Normalized normalized = Normalized::No, Instanced instanced = Instanced::No);
		bool BindAttributeVec2(std::string_view name, Normalized normalized = Normalized::No, Instanced instanced = Instanced::No);
		bool BindAttributeVec3(std::string_view name, Normalized normalized = Normalized::No, Instanced instanced = Instanced::No);
		bool BindAttributeVec4(std::string_view name, Normalized normalized = Normalized::No, Instanced instanced = Instanced::No);

		bool BindAttributeMatrix(std::string_view name, uint size, Normalized normalized = Normalized::No, Instanced instanced = Instanced::No);
		bool BindAttributeMat2(std::string_view name, Normalized normalized = Normalized::No, Instanced instanced = Instanced::No);
		bool BindAttributeMat3(std::string_view name, Normalized normalized = Normalized::No, Instanced instanced = Instanced::No);
		bool BindAttributeMat4(std::string_view name, Normalized normalized = Normalized::No, Instanced instanced = Instanced::No);

		bool EnableAttribute(std::string_view name);
		bool DisableAttribute(std::string_view name);

		Shader();
		~Shader();

		Shader(const Shader& other) = delete;
		Shader(Shader&& other) = delete;
		Shader& operator=(const Shader& other) = delete;
		Shader& operator=(Shader&& other) = delete;

		static const char* TypeName() { return "Shader"; }

	private:

		friend struct ddr::UniformStorage;
		friend struct ShaderManager;

		bool m_valid { false };
		bool m_inUse { false };
		uint m_id { OpenGL::InvalidID };

		dd::Vector<ShaderPart*> m_shaders;
		std::unordered_map<std::string, ShaderLocation> m_uniforms;
		std::unordered_map<std::string, ShaderLocation> m_attributes;

		void SetShaders(const dd::Vector<ShaderPart*>& shaders);
		dd::String256 Link();

		ShaderLocation GetAttribute(std::string_view name);
		ShaderLocation GetUniform(std::string_view name);

		void AssertBeforeUse(const std::string& name) const;
	};

	using ShaderHandle = dd::Handle<Shader>;

	struct ShaderManager : dd::HandleManager<Shader>
	{
		// 
		// Load a shader of the given name, assuming that it uses standard extensions. 
		// For anything more advanced, use Create.
		//
		ShaderHandle Load(std::string_view name);

		//
		// Reload all shaders.
		//
		void ReloadAll();

	private:
		using super = HandleManager<Shader>;
	};
}