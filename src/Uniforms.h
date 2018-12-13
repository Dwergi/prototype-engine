//
// Uniforms.h - Container for setting uniforms.
// Copyright (C) Sebastian Nordgren 
// July 13th 2018
//

#pragma once

namespace ddr
{
	struct Shader;
	struct Texture;

	enum class UniformType
	{
		Invalid = 0,
		Boolean,
		Integer,
		Float,
		Vector2,
		Vector3,
		Vector4,
		Matrix3,
		Matrix4,
		Sampler2
	};

	UniformType GetUniformTypeFromName( std::string_view type_name );

	template <typename T> UniformType GetUniformType() { return UniformType::Invalid; }

	template <typename T>
	UniformType GetUniformTypeFor( const T& value )
	{
		return GetUniformType<T>();
	}

	constexpr int MAX_UNIFORM_NAME = 64;

	struct IUniform
	{
		UniformType Type;
		char Name[ MAX_UNIFORM_NAME ];
	};

	template <typename T>
	struct Uniform : IUniform
	{
		T Value;
	};

	struct UniformStorage
	{
	public:

		UniformStorage();
		~UniformStorage();

		void Create( const char* name, UniformType type );

		void Set( const char* name, bool value );
		void Set( const char* name, int value );
		void Set( const char* name, float value );
		void Set( const char* name, glm::vec2 value );
		void Set( const char* name, glm::vec3 value );
		void Set( const char* name, glm::vec4 value );
		void Set( const char* name, const glm::mat3& value );
		void Set( const char* name, const glm::mat4& value );
		void Set( const char* name, const ddr::Texture& value );

		void Bind( Shader& shader );
		void Unbind();

		IUniform* Find( const char* name );

	private:

		static const int MAX_UNIFORMS = 256;
		static const int UNIFORM_SIZE = sizeof( Uniform<glm::mat4> );

		int m_count { 0 };

		Shader* m_shader { nullptr };

		byte m_uniforms[MAX_UNIFORMS * UNIFORM_SIZE];

		template <typename T>
		void SetValue( IUniform* uniform, const T& value );

		template <typename T>
		void SetHelper( const char* name, const T& value );

		IUniform* Access( int index );
	};
}