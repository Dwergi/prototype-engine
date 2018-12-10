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
		Boolean,
		Integer,
		Float,
		Vector2,
		Vector3,
		Vector4,
		Matrix3,
		Matrix4,
		Texture
	};

	struct IUniform
	{
		UniformType Type;
		char Name[256];
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
		void Create( const char* name, UniformType type, T value );

		template <typename T>
		void SetValue( IUniform* uniform, UniformType type, T value );

		IUniform* Access( int index );
	};

	template <typename T>
	void UniformStorage::SetValue( IUniform* uniform, UniformType type, T value )
	{
		DD_ASSERT( uniform->Type == type );

		((Uniform<T>*) uniform)->Value = value;
	}
}