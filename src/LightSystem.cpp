
#include "PrecompiledHeader.h"
#include "LightSystem.h"

#include "LightComponent.h"
#include "OpenGL.h"
#include "TransformComponent.h"
#include "Uniforms.h"

namespace ddr
{
	LightSystem::LightSystem( ddr::UniformStorage& uniforms ) :
		ddc::System( "Lights" ),
		m_uniforms( uniforms )
	{
		RequireRead<dd::LightComponent>();
		RequireRead<dd::TransformComponent>();
	}

	void LightSystem::Update( const ddc::UpdateData& data, dd::seconds delta_t )
	{
		ddc::ReadBuffer<dd::LightComponent> lights = data.Read<dd::LightComponent>();
		ddc::ReadBuffer<dd::TransformComponent> transforms = data.Read<dd::TransformComponent>();

		size_t lightCount = lights.Size();
		DD_ASSERT( lightCount <= 10 );

		m_uniforms.Set( "LightCount", (int) lightCount );

		for( size_t i = 0; i < lights.Size(); ++i )
		{
			const dd::LightComponent& light = lights[i];

			glm::vec4 position( transforms[i].GetWorldPosition(), 1 );
			if( light.IsDirectional )
			{
				position.w = 0;
			}

			m_uniforms.Set( GetArrayUniformName( "Lights", i, "Position" ).c_str(), position );
			m_uniforms.Set( GetArrayUniformName( "Lights", i, "Colour" ).c_str(), light.Colour );
			m_uniforms.Set( GetArrayUniformName( "Lights", i, "Intensity" ).c_str(), light.Intensity );
			m_uniforms.Set( GetArrayUniformName( "Lights", i, "Attenuation" ).c_str(), light.Attenuation );
			m_uniforms.Set( GetArrayUniformName( "Lights", i, "AmbientStrength" ).c_str(), light.Ambient );
			m_uniforms.Set( GetArrayUniformName( "Lights", i, "SpecularStrength" ).c_str(), light.Specular );
		}
	}
}