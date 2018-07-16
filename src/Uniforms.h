//
// Uniforms.h - Container for setting uniforms.
// Copyright (C) Sebastian Nordgren 
// July 13th 2018
//

#pragma once

namespace ddr
{
	class ShaderProgram;

	enum class UniformType
	{
		Boolean,
		Integer,
		Float,
		Vector2,
		Vector3,
		Vector4,
		Matrix4
	};

	struct IUniform
	{
		UniformType Type;
		dd::String32 Name;
	};

	template <typename T>
	struct Uniform : IUniform
	{
		T Value;
	};

	class UniformStorage
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
		void Set( const char* name, glm::mat4 value );

		void Erase( const char* name );

		void Bind( ShaderProgram& shader );

		IUniform* Find( const char* name );

	private:

		std::vector<IUniform*> m_uniforms;

		template <typename T>
		void Create( const char* name, UniformType type, T value );

		template <typename T>
		void SetValue( IUniform* uniform, UniformType type, T value );
	};

	template <typename T>
	void UniformStorage::SetValue( IUniform* uniform, UniformType type, T value )
	{
		DD_ASSERT( uniform->Type == type );

		((Uniform<T>*) uniform)->Value = value;
	}
}