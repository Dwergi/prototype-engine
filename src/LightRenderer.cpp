//
// LightRenderer.cpp - Renderer that ensures that lights get passed to other renderers.
// Copyright (C) Sebastian Nordgren 
// September 18th 2018
//

#include "PCH.h"
#include "LightRenderer.h"

#include "BoundBoxComponent.h"
#include "MeshUtils.h"
#include "LightComponent.h"
#include "Mesh.h"
#include "Services.h"
#include "Shader.h"
#include "TransformComponent.h"
#include "Uniforms.h"
#include "OpenGL.h"

namespace ddr
{
	LightRenderer::LightRenderer() :
		ddr::IRenderer( "Lights" )
	{
		Require<dd::TransformComponent>();
		Require<dd::LightComponent>();
		RequireTag( ddc::Tag::Visible );
	}

	ddc::Entity CreatePointLight( ddc::EntitySpace& entities )
	{
		ddc::Entity entity = entities.CreateEntity<dd::TransformComponent, dd::LightComponent>();
		entities.AddTag( entity, ddc::Tag::Visible );

		dd::LightComponent* light = entities.Access<dd::LightComponent>( entity );
		light->Ambient = 0.01f;
		light->Colour = glm::vec3( 1, 1, 1 );
		light->LightType = dd::LightType::Point;

		dd::TransformComponent* transform = entities.Access<dd::TransformComponent>( entity );
		transform->Position = glm::vec3( 0, 20, 0 );
		transform->Scale = glm::vec3( 0.4 );
		transform->Update();

		return entity;
	}

	void LightRenderer::UpdateDebugPointLights( ddc::EntitySpace& entities )
	{
		if( m_createLight )
		{
			CreatePointLight( entities );
			m_createLight = false;
		}

		if( m_deleteLight.IsValid() )
		{
			entities.DestroyEntity( m_deleteLight );
			m_deleteLight = ddc::Entity();
		}
	}

	void LightRenderer::RenderInit( ddc::EntitySpace& entities )
	{
		m_shader = ddr::ShaderHandle( "mesh" );
		m_mesh = ddr::MeshHandle( "sphere" );
	}

	void LightRenderer::RenderUpdate( ddc::EntitySpace& entities )
	{
		UpdateDebugPointLights( entities );
	}

	void LightRenderer::Render( const RenderData& data )
	{
		auto lights = data.Get<dd::LightComponent>();
		auto transforms = data.Get<dd::TransformComponent>();
		auto entities = data.Entities();

		ddr::UniformStorage& uniforms = data.Uniforms();

		size_t light_count = data.Size();
		if( light_count > 10 )
		{
			light_count = 10;
		}

		uniforms.Set( "LightCount", (int) light_count );

		m_debugLights.clear();

		for( size_t i = 0; i < light_count; ++i )
		{
			const dd::LightComponent& light = lights[ i ];
			const dd::TransformComponent& transform = transforms[ i ];

			m_debugLights.push_back( entities[ i ] );

			glm::vec4 position( transform.Position, 1 );
			uniforms.Set( GetArrayUniformName( "Lights", i, "Type" ).c_str(), (int) light.LightType );
			uniforms.Set( GetArrayUniformName( "Lights", i, "Position" ).c_str(), position );
			uniforms.Set( GetArrayUniformName( "Lights", i, "Colour" ).c_str(), light.Colour );
			uniforms.Set( GetArrayUniformName( "Lights", i, "Intensity" ).c_str(), light.Intensity );
			uniforms.Set( GetArrayUniformName( "Lights", i, "Attenuation" ).c_str(), light.Attenuation );
			uniforms.Set( GetArrayUniformName( "Lights", i, "AmbientStrength" ).c_str(), light.Ambient );
			uniforms.Set( GetArrayUniformName( "Lights", i, "SpecularStrength" ).c_str(), light.Specular );

			glm::vec4 direction = transform.Transform() * glm::vec4( glm::vec3( 0, 0, 1 ), 0 );
			uniforms.Set( GetArrayUniformName( "Lights", i, "Direction" ).c_str(), direction.xyz );
			uniforms.Set( GetArrayUniformName( "Lights", i, "CosInnerAngle" ).c_str(), glm::cos( light.InnerAngle ) );
			uniforms.Set( GetArrayUniformName( "Lights", i, "CosOuterAngle" ).c_str(), glm::cos( light.OuterAngle ) );
		}
	}

	void LightRenderer::DrawDebugInternal()
	{
		ImGui::SetWindowSize( ImVec2( 200, 400 ), ImGuiCond_FirstUseEver );

		for( size_t i = 0; i < m_debugLights.size(); ++i )
		{
			ddc::Entity entity = m_debugLights[i];
			dd::LightComponent* light = entity.Access<dd::LightComponent>();
			dd::TransformComponent* transform = entity.Access<dd::TransformComponent>();

			if( light == nullptr )
				continue;

			char buffer[ 16 ];
			sprintf_s( buffer, "Light %llu", i );

			if( ImGui::TreeNodeEx( buffer, ImGuiTreeNodeFlags_CollapsingHeader ) )
			{
				static const char* c_lightTypes = "Directional\0Point\0Spot\0";
				int lightType = (int) light->LightType;
					
				if( ImGui::Combo( "Type", &lightType, c_lightTypes ) )
				{
					light->LightType = (dd::LightType) lightType;
				}
				
				glm::vec3 light_colour = light->Colour;
				if( ImGui::ColorEdit3( "Colour", glm::value_ptr( light_colour ) ) )
				{
					light->Colour = light_colour;
				}

				ImGui::DragFloat( "Intensity", &light->Intensity, 0.01, 0, 100 );
				ImGui::DragFloat( "Attenuation", &light->Attenuation, 0.001, 0.001, 1 );
				ImGui::DragFloat( "Ambient", &light->Ambient, 0.001, 0, 1 );
				ImGui::DragFloat( "Specular", &light->Specular, 0.001, 0, 1 );

				if( light->LightType == dd::LightType::Directional )
				{
					glm::vec3 light_direction = glm::rotate( transform->Rotation, glm::vec4( 0, 0, 1, 1 ) ).xyz;
					if( ImGui::DragFloat3( "Direction", glm::value_ptr( light_direction ), 0.0025, -1, 1 ) )
					{
						transform->Rotation = glm::rotation( glm::vec3( 0, 0, 1 ), glm::normalize( light_direction ) );
					}
				}
				else
				{
					ImGui::DragFloat3( "Position", glm::value_ptr( transform->Position ), 0.1 );
				}

				float outer_angle = glm::degrees( light->OuterAngle );
				if( ImGui::SliderFloat( "Outer Angle", &outer_angle, 0, 89 ) )
				{
					light->OuterAngle = glm::radians( outer_angle );
				}

				float inner_angle = glm::degrees( glm::min( light->InnerAngle, light->OuterAngle ) );
				ImGui::SliderFloat( "Inner Angle", &inner_angle, 0, outer_angle );
				inner_angle = glm::radians( inner_angle );

				if( light->InnerAngle != inner_angle )
				{
					light->InnerAngle = inner_angle;
				}

				if( ImGui::Button( "Delete" ) )
				{
					m_deleteLight = entity;
				}

				ImGui::TreePop();
			}
		}

		if( m_debugLights.size() < 10 )
		{
			if( ImGui::Button( "Create Light" ) )
			{
				m_createLight = true;
			}
		}
	}
}