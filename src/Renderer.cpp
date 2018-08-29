//
// Renderer.cpp - Master renderer class, coordinates all rendering.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#include "PrecompiledHeader.h"
#include "Renderer.h"

#include "AABB.h"
#include "GLError.h"
#include "EntityManager.h"
#include "Frustum.h"
#include "ICamera.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "MousePicking.h"
#include "LightComponent.h"
#include "ParticleSystem.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "TransformComponent.h"
#include "Uniforms.h"
#include "Window.h"

#include "GL/gl3w.h"

#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "imgui/imgui.h"

namespace ddr
{

	struct Fog
	{
		bool Enabled { true };
		float Distance { 1000.0f };
		glm::vec3 Colour { 0.6, 0.7, 0.8 };

		void UpdateUniforms( ddr::UniformStorage& uniforms ) const
		{
			uniforms.Set( "Fog.Enabled", Enabled );
			uniforms.Set( "Fog.Distance", Distance );
			uniforms.Set( "Fog.Colour", Colour );
		}
	};

	Fog s_fog;

	struct Wireframe
	{
		bool Enabled { false };

		glm::vec3 Colour { 0, 1.0f, 0 };
		float Width { 2.0f };

		glm::vec3 EdgeColour { 0, 0, 0 };
		float EdgeWidth { 0.5f };

		float MaxDistance { 250.0f };

		void UpdateUniforms( ddr::UniformStorage& uniforms ) const
		{
			uniforms.Set( "Wireframe.Enabled", Enabled );
			uniforms.Set( "Wireframe.Colour", Colour );
			uniforms.Set( "Wireframe.Width", Width );
			uniforms.Set( "Wireframe.EdgeColour", EdgeColour );
			uniforms.Set( "Wireframe.EdgeWidth", EdgeWidth );
			uniforms.Set( "Wireframe.MaxDistance", MaxDistance );
		}
	};

	Wireframe s_wireframe;

	Renderer::Renderer( const dd::Window& window ) :
		m_window( window )
	{
		m_uniforms = new ddr::UniformStorage();
	}

	Renderer::~Renderer()
	{
		delete m_uniforms;
	}

	void Renderer::Initialize( dd::EntityManager& entity_manager )
	{
		{
			dd::EntityHandle directionalLight = entity_manager.CreateEntity<dd::LightComponent, dd::TransformComponent>();
			dd::ComponentHandle<dd::LightComponent> light = directionalLight.Get<dd::LightComponent>();
			light.Write()->IsDirectional = true;
			light.Write()->Colour = glm::vec3( 1, 1, 1 );
			light.Write()->Intensity = 0.5;

			dd::ComponentHandle<dd::TransformComponent> transform = directionalLight.Get<dd::TransformComponent>();
			glm::vec3 direction( 0.5, 0.4, -0.3 );
			transform.Write()->SetLocalPosition( direction );
		}

		m_createLight = true;
	}

	void Renderer::RenderInit()
	{
		m_unitCube = Mesh::Find( "unitcube" );
		if( !m_unitCube.IsValid() )
		{
			m_unitCube = Mesh::Create( "unitcube" );

			Mesh* mesh = Mesh::Get( m_unitCube );
			DD_ASSERT( mesh != nullptr );

			ShaderHandle shader_h = ShaderProgram::Load( "standard" );
			ShaderProgram* shader = ShaderProgram::Get( shader_h );
			DD_ASSERT( shader != nullptr );

			MaterialHandle material_h = Material::Create( "standard" );
			Material* material = Material::Get( material_h );
			DD_ASSERT( material != nullptr );

			material->SetShader( shader_h );
			mesh->SetMaterial( material_h );

			shader->Use( true );

			mesh->MakeUnitCube();

			shader->Use( false );
		}

		CreateFrameBuffer( m_window.GetSize() );
		m_previousSize = m_window.GetSize();

		for( ddr::IRenderer* current : m_renderers )
		{
			current->RenderInit();
		}
	}

	void Renderer::Register( ddr::IRenderer& renderer )
	{
		m_renderers.push_back( &renderer );
	}

	void Renderer::CreateFrameBuffer( glm::ivec2 size )
	{
		m_colourTexture.Create( size, GL_SRGB8_ALPHA8, 1 );
		m_depthTexture.Create( size, GL_DEPTH_COMPONENT32F, 1 );

		m_framebuffer.SetClearColour( glm::vec4( m_skyColour.xyz, 0.0 ) );
		m_framebuffer.SetClearDepth( 0.0f );
		m_framebuffer.Create( m_colourTexture, &m_depthTexture );
		m_framebuffer.RenderInit();
	}

	void Renderer::Shutdown()
	{
		Mesh::Destroy( m_unitCube );
	}

	dd::EntityHandle Renderer::CreateMeshEntity( dd::EntityManager& entityManager, MeshHandle mesh_h, glm::vec4 colour, const glm::mat4& transform )
	{
		dd::EntityHandle handle = entityManager.CreateEntity<dd::TransformComponent, dd::MeshComponent>();

		dd::ComponentHandle<dd::TransformComponent> transform_cmp = handle.Get<dd::TransformComponent>();
		transform_cmp.Write()->SetLocalTransform( transform );

		dd::ComponentHandle<dd::MeshComponent> mesh_cmp = handle.Get<dd::MeshComponent>();
		mesh_cmp.Write()->Mesh = mesh_h;
		mesh_cmp.Write()->Colour = colour;
		mesh_cmp.Write()->Hidden = false;

		return handle;
	}

	void Renderer::DrawDebugInternal()
	{
		ImGui::Checkbox( "Draw Depth", &m_debugDrawDepth );
		ImGui::Checkbox( "Draw Standard", &m_debugDrawStandard );

		if( ImGui::Checkbox( "Draw Axes", &m_debugDrawAxes ) )
		{
			m_xAxis.Get<dd::MeshComponent>().Write()->Hidden = !m_debugDrawAxes;
			m_yAxis.Get<dd::MeshComponent>().Write()->Hidden = !m_debugDrawAxes;
			m_zAxis.Get<dd::MeshComponent>().Write()->Hidden = !m_debugDrawAxes;
		}	

		if( ImGui::TreeNodeEx( "Wireframe", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			ImGui::Checkbox( "Enabled", &s_wireframe.Enabled );

			ImGui::DragFloat( "Width", &s_wireframe.Width, 0.01f, 0.0f, 10.0f );

			ImGui::ColorEdit3( "Colour", glm::value_ptr( s_wireframe.Colour ) );

			ImGui::DragFloat( "Edge Width", &s_wireframe.EdgeWidth, 0.01f, 0.0f, s_wireframe.Width );

			ImGui::ColorEdit3( "Edge Colour", glm::value_ptr( s_wireframe.EdgeColour ) );

			ImGui::DragFloat( "Max Distance", &s_wireframe.MaxDistance, 1.0f, 0.0f, 1000.0f );

			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Lighting", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			for( size_t i = 0; i < m_debugLights.size(); ++i )
			{
				dd::String64 lightLabel( "Light " );
				char buffer[16];
				_itoa_s( (int) i, buffer, 10 );
				lightLabel += buffer;

				dd::EntityHandle entity = m_debugLights[ i ];
				dd::ComponentHandle<dd::LightComponent> light = entity.Get<dd::LightComponent>();
				dd::ComponentHandle<dd::TransformComponent> transform = entity.Get<dd::TransformComponent>();

				if( ImGui::TreeNodeEx( lightLabel.c_str(), ImGuiTreeNodeFlags_CollapsingHeader ) )
				{
					bool directional = light.Read()->IsDirectional;
					if( ImGui::Checkbox( "Directional?", &directional ) )
					{
						light.Write()->IsDirectional = directional;
					}
					
					glm::vec3 light_colour = light.Read()->Colour;
					if( ImGui::ColorEdit3( "Colour", glm::value_ptr( light_colour ) ) )
					{
						light.Write()->Colour = light_colour;
					}

					float intensity = light.Read()->Intensity;
					if( ImGui::DragFloat( "Intensity", &intensity, 0.01, 0, 100 ) )
					{
						light.Write()->Intensity = intensity;
					}

					float attenuation = light.Read()->Attenuation;
					if( ImGui::DragFloat( "Attenuation", &attenuation, 0.01, 0, 1 ) )
					{
						light.Write()->Attenuation = attenuation;
					}

					glm::vec3 light_position = transform.Read()->GetLocalPosition();

					const char* positionLabel = light.Read()->IsDirectional ? "Direction" : "Position";
					if( ImGui::DragFloat3( positionLabel, glm::value_ptr( light_position ) ) )
					{
						transform.Write()->SetLocalPosition( light_position );
					}

					float ambient = light.Read()->Ambient;
					if( ImGui::SliderFloat( "Ambient", &ambient, 0.0f, 1.0f ) )
					{
						light.Write()->Ambient = ambient;
					}

					float specular = light.Read()->Specular;
					if( ImGui::SliderFloat( "Specular", &specular, 0.0f, 1.0f ) )
					{
						light.Write()->Specular = specular;
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

			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Fog", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			ImGui::Checkbox( "Enabled", &s_fog.Enabled );
			ImGui::SliderFloat( "Distance", &s_fog.Distance, 0, 10000, "%.1f" );
			ImGui::ColorEdit3( "Colour", glm::value_ptr( s_fog.Colour ) );

			ImGui::TreePop();
		}
		
		if( ImGui::Button( "Reload Shaders" ) )
		{
			m_reloadShaders = true;
		}

		if( !m_debugMeshGridCreated && ImGui::Button( "Create Mesh Grid" ) )
		{
			m_createDebugMeshGrid = true;
		}
	}

	void Renderer::SetRenderState()
	{
		if( m_debugDrawStandard )
		{
			// depth test
			glEnable( GL_DEPTH_TEST );
			glDepthFunc( GL_GREATER );
			glClipControl( GL_LOWER_LEFT, GL_ZERO_TO_ONE );

			// backface culling
			glEnable( GL_CULL_FACE );
			glFrontFace( GL_CCW );
			glCullFace( GL_BACK );

			// blending
			glDisable( GL_BLEND );
		}
		else
		{
			glDisable( GL_CULL_FACE );
			glDisable( GL_DEPTH_TEST );
			glEnable( GL_BLEND );
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		}
	}

	void Renderer::CreateDebugMeshGrid( dd::EntityManager& entityManager )
	{
		if( m_debugMeshGridCreated || !m_createDebugMeshGrid )
			return;

		for( int x = -5; x < 5; ++x )
		{
			for( int y = -5; y < 5; ++y )
			{
				for( int z = -5; z < 5; ++z )
				{
					CreateMeshEntity( entityManager, m_unitCube, glm::vec4( 0.5, 0.5, 0.5, 1 ), glm::translate( glm::vec3( 10.f * x, 10.f * y, 10.f * z ) ) );
				}
			}
		}

		m_debugMeshGridCreated = true;
	}

	dd::EntityHandle Renderer::CreatePointLight( dd::EntityManager& entityManager )
	{
		glm::mat4 transform = glm::translate( glm::vec3( 0 ) ) * glm::scale( glm::vec3( 0.1f ) );
		dd::EntityHandle entity = CreateMeshEntity( entityManager, m_unitCube, glm::vec4( 1 ), transform );
		dd::ComponentHandle<dd::LightComponent> light = entityManager.AddComponent<dd::LightComponent>( entity );
		light.Write()->Ambient = 0.01f;

		return entity;
	}

	void Renderer::UpdateDebugPointLights( dd::EntityManager& entity_manager )
	{
		if( m_createLight )
		{
			CreatePointLight( entity_manager );
			m_createLight = false;
		}

		if( m_deleteLight.IsValid() )
		{
			entity_manager.Destroy( m_deleteLight );
			m_deleteLight = dd::EntityHandle();
		}

		entity_manager.ForAllWithReadable<dd::MeshComponent, dd::LightComponent>( []( auto entity, auto mesh, auto light )
		{
			if( mesh.Write() != nullptr )
			{
				mesh.Write()->Colour = glm::vec4( light.Read()->Colour, 1);
			}
		} );
	}

	void Renderer::Update( dd::EntityManager& entity_manager, float delta_t )
	{
		if( !m_xAxis.IsValid() )
		{
			m_xAxis = CreateMeshEntity( entity_manager, m_unitCube, glm::vec4( 1, 0, 0, 1 ), glm::scale( glm::vec3( 100, 0.05f, 0.05f ) ) );
			m_yAxis = CreateMeshEntity( entity_manager, m_unitCube, glm::vec4( 0, 1, 0, 1 ), glm::scale( glm::vec3( 0.05f, 100, 0.05f ) ) );
			m_zAxis = CreateMeshEntity( entity_manager, m_unitCube, glm::vec4( 0, 0, 1, 1 ), glm::scale( glm::vec3( 0.05f, 0.05f, 100 ) ) );
		}

		CreateDebugMeshGrid( entity_manager );
		UpdateDebugPointLights( entity_manager );

		m_debugLights = entity_manager.FindAllWithWritable<dd::LightComponent, dd::TransformComponent>();
	}

	void Renderer::RenderDebug( ddr::IRenderer& debug_render )
	{
		m_framebuffer.BindDraw();

		debug_render.RenderDebug();

		m_framebuffer.UnbindDraw();
	}

	void Renderer::Render( const dd::EntityManager& entity_manager, const ddr::ICamera& camera )
	{
		ddr::IRenderer* debug_render = nullptr;

		BeginRender( entity_manager, camera );

		DD_TODO( "Uncomment" );

		/*for( ddr::IRenderer* r : m_renderers )
		{
			if( !r->UsesAlpha() )
			{
				r->Render( entity_manager, camera, *m_uniforms );
			}

			if( r->ShouldRenderDebug() )
			{
				debug_render = r;
				break;
			}
		}

		if( debug_render != nullptr )
		{
			RenderDebug( *debug_render );
		}

		for( ddr::IRenderer* r : m_renderers )
		{
			if( r->UsesAlpha() )
			{
				r->Render( entity_manager, camera, *m_uniforms );
			}
		}*/
	}

	void Renderer::BeginRender( const dd::EntityManager& entity_manager, const ddr::ICamera& camera )
	{
		DD_ASSERT( m_window.IsContextValid() );

		if( m_reloadShaders )
		{
			ShaderProgram::ReloadAll();
			m_reloadShaders = false;
		}

		if( m_window.GetSize() != m_previousSize )
		{
			m_framebuffer.Destroy();
			m_colourTexture.Destroy();
			m_depthTexture.Destroy();

			CreateFrameBuffer( m_window.GetSize() );

			m_previousSize = m_window.GetSize();
		}

		m_framebuffer.BindRead();
		m_framebuffer.BindDraw();
		m_framebuffer.Clear();

		SetRenderState();

		DD_TODO( "Move this to a light renderer.")
		std::vector<dd::EntityHandle> lights = entity_manager.FindAllWithReadable<dd::LightComponent, dd::TransformComponent>();

		size_t lightCount = lights.size();
		DD_ASSERT( lightCount <= 10 );
		m_uniforms->Set( "LightCount", (int) lightCount );

		for( size_t i = 0; i < lights.size(); ++i )
		{
			const dd::TransformComponent* transformCmp = lights[ i ].Get<dd::TransformComponent>().Read();
			const dd::LightComponent* lightCmp = lights[ i ].Get<dd::LightComponent>().Read();

			glm::vec4 position( transformCmp->GetWorldPosition(), 1 );
			if( lightCmp->IsDirectional )
			{
				position.w = 0;
			}

			m_uniforms->Set( GetArrayUniformName( "Lights", i, "Position" ).c_str(), position );
			m_uniforms->Set( GetArrayUniformName( "Lights", i, "Colour" ).c_str(), lightCmp->Colour );
			m_uniforms->Set( GetArrayUniformName( "Lights", i, "Intensity" ).c_str(), lightCmp->Intensity );
			m_uniforms->Set( GetArrayUniformName( "Lights", i, "Attenuation" ).c_str(), lightCmp->Attenuation );
			m_uniforms->Set( GetArrayUniformName( "Lights", i, "AmbientStrength" ).c_str(), lightCmp->Ambient );
			m_uniforms->Set( GetArrayUniformName( "Lights", i, "SpecularStrength" ).c_str(), lightCmp->Specular );
		}

		m_uniforms->Set( "View", camera.GetCameraMatrix() );
		m_uniforms->Set( "Projection", camera.GetProjectionMatrix() );

		s_wireframe.UpdateUniforms( *m_uniforms );
		s_fog.UpdateUniforms( *m_uniforms );

		m_uniforms->Set( "DrawStandard", m_debugDrawStandard );
	}

	void Renderer::EndRender( const ddr::ICamera& camera )
	{
		m_framebuffer.BindRead();

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
		glViewport( 0, 0, m_window.GetWidth(), m_window.GetHeight() );
		CheckOGLError();

		if( m_debugDrawDepth )
		{
			m_framebuffer.RenderDepth( camera );
		}
		else
		{
			m_framebuffer.Blit();
		}

		m_framebuffer.UnbindRead();
	}
}