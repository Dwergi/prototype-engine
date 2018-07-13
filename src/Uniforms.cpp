//
// Uniforms.cpp - Container for setting uniforms.
// Copyright (C) Sebastian Nordgren 
// July 13th 2018
//

#include "PrecompiledHeader.h"
#include "Uniforms.h"

#include "ShaderProgram.h"

namespace ddr
{

	UniformStorage::UniformStorage()
	{
	}

	UniformStorage::~UniformStorage()
	{
		for( IUniform* uniform : m_uniforms )
		{
			delete uniform;
		}

		m_uniforms.clear();
	}

	void UniformStorage::Set( const char* name, bool value )
	{
		IUniform* uniform = Find( name );
		if( uniform != nullptr )
		{
			SetValue( uniform, UniformType::Boolean, value );
		}
		else
		{
			Create( UniformType::Boolean, name, value );
		}
	}

	void UniformStorage::Set( const char* name, int value )
	{
		IUniform* uniform = Find( name );
		if( uniform != nullptr )
		{
			SetValue( uniform, UniformType::Integer, value );
		}
		else
		{
			Create( UniformType::Integer, name, value );
		}
	}

	void UniformStorage::Set( const char* name, float value )
	{
		IUniform* uniform = Find( name );
		if( uniform != nullptr )
		{
			SetValue( uniform, UniformType::Float, value );
		}
		else
		{
			Create( UniformType::Float, name, value );
		}
	}

	void UniformStorage::Set( const char* name, glm::vec2 value )
	{
		IUniform* uniform = Find( name );
		if( uniform != nullptr )
		{
			SetValue( uniform, UniformType::Vector2, value );
		}
		else
		{
			Create( UniformType::Vector2, name, value );
		}
	}

	void UniformStorage::Set( const char* name, glm::vec3 value )
	{
		IUniform* uniform = Find( name );
		if( uniform != nullptr )
		{
			SetValue( uniform, UniformType::Vector3, value );
		}
		else
		{
			Create( UniformType::Vector3, name, value );
		}
	}

	void UniformStorage::Set( const char* name, glm::vec4 value )
	{
		IUniform* uniform = Find( name );
		if( uniform != nullptr )
		{
			SetValue( uniform, UniformType::Vector4, value );
		}
		else
		{
			Create( UniformType::Vector4, name, value );
		}
	}

	void UniformStorage::Set( const char* name, glm::mat4 value )
	{
		IUniform* uniform = Find( name );
		if( uniform != nullptr )
		{
			SetValue( uniform, UniformType::Matrix4, value );
		}
		else
		{
			Create( UniformType::Matrix4, name, value );
		}
	}

	void UniformStorage::Erase( const char* name )
	{
		auto it = m_uniforms.begin();
		for( ; it != m_uniforms.end(); ++it )
		{
			if( (*it)->Name == name )
			{
				break;
			}
		}

		if( it != m_uniforms.end() )
		{
			delete *it;
			m_uniforms.erase( it );
		}
	}

	void UniformStorage::Bind( ShaderProgram& shader )
	{
		for( IUniform* uniform : m_uniforms )
		{
			switch( uniform->Type )
			{
				case UniformType::Boolean:
				{
					Uniform<bool>* u = (Uniform<bool>*) uniform;
					shader.SetUniform( u->Name.c_str(), u->Value );
					break;
				}
				case UniformType::Integer:
				{
					Uniform<int>* u = (Uniform<int>*) uniform;
					shader.SetUniform( u->Name.c_str(), u->Value );
					break;
				}
				case UniformType::Float:
				{
					Uniform<float>* u = (Uniform<float>*) uniform;
					shader.SetUniform( u->Name.c_str(), u->Value );
					break;
				}
				case UniformType::Vector2:
				{
					Uniform<glm::vec2>* u = (Uniform<glm::vec2>*) uniform;
					shader.SetUniform( u->Name.c_str(), u->Value );
					break;
				}
				case UniformType::Vector3:
				{
					Uniform<glm::vec3>* u = (Uniform<glm::vec3>*) uniform;
					shader.SetUniform( u->Name.c_str(), u->Value );
					break;
				}
				case UniformType::Vector4:
				{
					Uniform<glm::vec4>* u = (Uniform<glm::vec4>*) uniform;
					shader.SetUniform( u->Name.c_str(), u->Value );
					break;
				}
				case UniformType::Matrix4:
				{
					Uniform<glm::mat4>* u = (Uniform<glm::mat4>*) uniform;
					shader.SetUniform( u->Name.c_str(), u->Value );
					break;
				}
			}
		}
	}

	IUniform* UniformStorage::Find( const char* name )
	{
		for( IUniform* uniform : m_uniforms )
		{
			if( uniform->Name == name )
			{
				return uniform;
			}
		}

		return nullptr;
	}
}