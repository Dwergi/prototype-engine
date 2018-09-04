
#include "PrecompiledHeader.h"
#include "LightRenderer.h"

#include "LightComponent.h"
#include "TransformComponent.h"
#include "Uniforms.h"
#include "OpenGL.h"

namespace ddr
{
	LightRenderer::LightRenderer() : 
		ddr::Renderer( "Lights" )
	{
		Require<dd::TransformComponent>();
		Require<ddr::LightComponent>();
	}

	void LightRenderer::Render( const RenderData& data )
	{
		ddr::RenderBuffer<ddr::LightComponent> lights = data.Get<ddr::LightComponent>();
		ddr::RenderBuffer<dd::TransformComponent> transforms = data.Get<dd::TransformComponent>();

		ddr::UniformStorage& uniforms = data.Uniforms();

		size_t lightCount = lights.Size();
		DD_ASSERT( lightCount <= 10 );

		uniforms.Set( "LightCount", (int) lightCount );

		for( size_t i = 0; i < lights.Size(); ++i )
		{
			const ddr::LightComponent& light = lights[ i ];
			const dd::TransformComponent& transform = transforms[ i ];

			glm::vec4 position( transform.GetWorldPosition(), 1 );
			if( light.IsDirectional )
			{
				position.w = 0;
			}

			uniforms.Set( GetArrayUniformName( "Lights", i, "Position" ).c_str(), position );
			uniforms.Set( GetArrayUniformName( "Lights", i, "Colour" ).c_str(), light.Colour );
			uniforms.Set( GetArrayUniformName( "Lights", i, "Intensity" ).c_str(), light.Intensity );
			uniforms.Set( GetArrayUniformName( "Lights", i, "Attenuation" ).c_str(), light.Attenuation );
			uniforms.Set( GetArrayUniformName( "Lights", i, "AmbientStrength" ).c_str(), light.Ambient );
			uniforms.Set( GetArrayUniformName( "Lights", i, "SpecularStrength" ).c_str(), light.Specular );
		}
	}
}