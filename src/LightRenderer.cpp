//
// LightRenderer.cpp - Renderer that ensures that lights get passed to other renderers.
// Copyright (C) Sebastian Nordgren 
// September 18th 2018
//

#include "PrecompiledHeader.h"
#include "LightRenderer.h"

#include "BoundsComponent.h"
#include "Icosphere.h"
#include "LightComponent.h"
#include "ShaderProgram.h"
#include "TransformComponent.h"
#include "Uniforms.h"
#include "OpenGL.h"

#include "imgui/imgui.h"

namespace ddr
{
	LightRenderer::LightRenderer() :
		ddr::Renderer( "Lights" )
	{
		Require<dd::TransformComponent>();
		Require<ddr::LightComponent>();
		RequireTag( ddc::Tag::Visible );
	}

	ddc::Entity CreatePointLight( ddc::World& world )
	{
		ddc::Entity entity = world.CreateEntity<dd::TransformComponent, ddr::LightComponent>();
		world.AddTag( entity, ddc::Tag::Visible );

		ddr::LightComponent* light = world.Access<ddr::LightComponent>( entity );
		light->Ambient = 0.01f;
		light->Colour = glm::vec3( 1, 1, 1 );
		light->IsDirectional = false;

		dd::TransformComponent* transform = world.Access<dd::TransformComponent>( entity );
		transform->Local = glm::translate( glm::vec3( 0, 20, 0 ) ) * glm::scale( glm::vec3( 0.4 ) );

		return entity;
	}

	void LightRenderer::UpdateDebugPointLights( ddc::World& world )
	{
		if( m_createLight )
		{
			CreatePointLight( world );
			m_createLight = false;
		}

		if( m_deleteLight.IsValid() )
		{
			world.DestroyEntity( m_deleteLight );
			m_deleteLight = ddc::Entity();
		}
	}

	void LightRenderer::RenderInit( ddc::World& world )
	{
		m_shader = ShaderProgram::Load( "line" );
		DD_ASSERT( m_shader.Valid() );

		ShaderProgram* shader = ShaderProgram::Get( m_shader );
		DD_ASSERT( shader != nullptr );

		shader->Use( true );

		m_vao.Create();
		m_vao.Bind();

		m_vboPosition.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );

		m_vboPosition.Bind();
		shader->BindPositions();
		m_vboPosition.Unbind();

		m_vboIndex.Create( GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW );

		dd::MakeIcosphereLines( m_vboPosition, m_vboIndex, 2 );

		m_vao.Unbind();

		shader->Use( false );
	}

	void LightRenderer::Render( const RenderData& data )
	{
		auto lights = data.Get<ddr::LightComponent>();
		auto transforms = data.Get<dd::TransformComponent>();
		auto entities = data.Entities();

		ddr::UniformStorage& uniforms = data.Uniforms();

		size_t light_count = entities.Size();
		if( light_count > 10 )
		{
			DD_ASSERT( light_count <= 10 );
			light_count = 10;
		}

		uniforms.Set( "LightCount", (int) light_count );

		m_debugLights.clear();

		glm::mat4 view_projection = data.Camera().GetProjectionMatrix() * data.Camera().GetViewMatrix();
		ShaderProgram* shader = nullptr;
		if( m_drawBounds )
		{
			glEnable( GL_BLEND );
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

			shader = ShaderProgram::Get( m_shader );
			shader->Use( true );

			m_vao.Bind();

			m_vboPosition.Bind();
			m_vboIndex.Bind();
		}

		for( size_t i = 0; i < light_count; ++i )
		{
			const ddr::LightComponent& light = lights[ i ];
			const dd::TransformComponent& transform = transforms[ i ];

			m_debugLights.push_back( entities[ i ] );

			glm::vec4 position( transform.GetLocalPosition(), 1 );
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

			if( m_drawBounds )
			{
				shader->SetUniform( "Colour", glm::vec4( light.Colour, 1 ) );
				shader->SetUniform( "ModelViewProjection", view_projection * transform.Local );

				glDrawElements( GL_LINES, m_vboIndex.GetDataSize(), GL_UNSIGNED_INT, 0 );
			}
		}

		if( m_drawBounds )
		{
			m_vboIndex.Unbind();
			m_vboPosition.Unbind();

			m_vao.Unbind();

			shader->Use( false );

			glDisable( GL_BLEND );
		}

		UpdateDebugPointLights( data.World() );
	}

	void LightRenderer::DrawDebugInternal( const ddc::World& world )
	{
		ImGui::SetWindowSize( ImVec2( 200, 400 ), ImGuiCond_FirstUseEver );

		ImGui::Checkbox( "Draw Bounds", &m_drawBounds );

		for( size_t i = 0; i < m_debugLights.size(); ++i )
		{
			char buffer[ 16 ];
			sprintf_s( buffer, "Light %llu", i );

			ddc::Entity entity = m_debugLights[ i ];
			ddr::LightComponent* light = world.Access<ddr::LightComponent>( entity );
			dd::TransformComponent* transform = world.Access<dd::TransformComponent>( entity );

			if( ImGui::TreeNodeEx( buffer, ImGuiTreeNodeFlags_CollapsingHeader ) )
			{
				bool directional = light->IsDirectional;
				if( ImGui::Checkbox( "Directional?", &directional ) )
				{
					light->IsDirectional = directional;
				}

				glm::vec3 light_colour = light->Colour;
				if( ImGui::ColorEdit3( "Colour", glm::value_ptr( light_colour ) ) )
				{
					light->Colour = light_colour;
				}

				float intensity = light->Intensity;
				if( ImGui::DragFloat( "Intensity", &intensity, 0.01, 0, 100 ) )
				{
					light->Intensity = intensity;
				}

				float attenuation = light->Attenuation;
				if( ImGui::DragFloat( "Attenuation", &attenuation, 0.001, 0.001, 1 ) )
				{
					light->Attenuation = attenuation;
				}

				glm::vec3 light_position = transform->GetLocalPosition();

				const char* positionLabel = light->IsDirectional ? "Direction" : "Position";
				if( ImGui::DragFloat3( positionLabel, glm::value_ptr( light_position ) ) )
				{
					transform->SetLocalPosition( light_position );
				}

				float ambient = light->Ambient;
				if( ImGui::SliderFloat( "Ambient", &ambient, 0.0f, 1.0f ) )
				{
					light->Ambient = ambient;
				}

				float specular = light->Specular;
				if( ImGui::SliderFloat( "Specular", &specular, 0.0f, 1.0f ) )
				{
					light->Specular = specular;
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