//
// Uniforms.cpp - Container for setting uniforms.
// Copyright (C) Sebastian Nordgren 
// July 13th 2018
//

#include "PrecompiledHeader.h"
#include "Uniforms.h"

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
			Set( uniform, UniformType::Boolean, value );
		}
		else
		{
			Create( UniformType::Boolean, value );
		}
	}

	void UniformStorage::Set( const char* name, int value )
	{
		IUniform* uniform = Find( name );
		if( uniform != nullptr )
		{
			Set( uniform, UniformType::Integer, value );
		}
		else
		{
			Create( UniformType::Integer, value );
		}
	}

	void UniformStorage::Set( const char* name, float value )
	{
		IUniform* uniform = Find( name );
		if( uniform != nullptr )
		{
			Set( uniform, UniformType::Float, value );
		}
		else
		{
			Create( UniformType::Float, value );
		}
	}

	void UniformStorage::Set( const char* name, glm::vec2 value )
	{
		IUniform* uniform = Find( name );
		if( uniform != nullptr )
		{
			Set( uniform, UniformType::Vector2, value );
		}
		else
		{
			Create( UniformType::Vector2, value );
		}
	}

	void UniformStorage::Set( const char* name, glm::vec3 value )
	{
		IUniform* uniform = Find( name );
		if( uniform != nullptr )
		{
			Set( uniform, UniformType::Vector3, value );
		}
		else
		{
			Create( UniformType::Vector3, value );
		}
	}

	void UniformStorage::Set( const char* name, glm::vec4 value )
	{
		IUniform* uniform = Find( name );
		if( uniform != nullptr )
		{
			Set( uniform, UniformType::Vector4, value );
		}
		else
		{
			Create( UniformType::Vector4, value );
		}
	}

	void UniformStorage::Set( const char* name, glm::mat4 value )
	{
		IUniform* uniform = Find( name );
		if( uniform != nullptr )
		{
			Set( uniform, UniformType::Matrix4, value );
		}
		else
		{
			Create( UniformType::Matrix4, value );
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
					shader.SetUniform( u->Name, u->Value );
					break;
				}
				case UniformType::Integer:
				{
					Uniform<int>* u = (Uniform<int>*) uniform;
					shader.SetUniform( u->Name, u->Value );
					break;
				}
				case UniformType::Float:
				{
					Uniform<float>* u = (Uniform<float>*) uniform;
					shader.SetUniform( u->Name, u->Value );
					break;
				}
				case UniformType::Vector2:
				{
					Uniform<glm::vec2>* u = (Uniform<glm::vec2>*) uniform;
					shader.SetUniform( u->Name, u->Value );
					break;
				}
				case UniformType::Vector3:
				{
					Uniform<glm::vec3>* u = (Uniform<glm::vec3>*) uniform;
					shader.SetUniform( u->Name, u->Value );
					break;
				}
				case UniformType::Vector4:
				{
					Uniform<glm::vec4>* u = (Uniform<glm::vec4>*) uniform;
					shader.SetUniform( u->Name, u->Value );
					break;
				}
				case UniformType::Matrix4:
				{
					Uniform<glm::mat4>* u = (Uniform<glm::mat4>*) uniform;
					shader.SetUniform( u->Name, u->Value );
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