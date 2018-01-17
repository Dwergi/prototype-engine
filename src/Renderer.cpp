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
#include "Mesh.h"
#include "MeshComponent.h"
#include "MousePicking.h"
#include "LightComponent.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "TransformComponent.h"
#include "Window.h"

#include "GL/gl3w.h"

#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "imgui/imgui.h"

namespace dd
{
	static Fog s_fog;

	Fog* Fog::Get()
	{
		if( s_fog.Enabled )
		{
			return &s_fog;
		}

		return nullptr;
	}

	Renderer::Renderer( const Window& window ) :
		m_meshCount( 0 ),
		m_window( window ),
		m_frustumMeshCount( 0 )
	{
		m_debugWireframeColour = glm::vec3( 0, 1, 0 );
		m_debugWireframeEdgeColour = glm::vec3( 0, 0, 0 );
	}

	Renderer::~Renderer()
	{

	}

	// TODO: Don't do this.
	static ShaderHandle CreateShaders( const char* name )
	{
		Vector<Shader*> shaders;

		Shader* vert = Shader::Create( String8( "shaders\\standard.vertex" ), Shader::Type::Vertex );
		DD_ASSERT( vert != nullptr );
		shaders.Add( vert );

		Shader* geom = Shader::Create( String8( "shaders\\standard.geometry" ), Shader::Type::Geometry );
		DD_ASSERT( geom != nullptr );
		shaders.Add( geom );

		Shader* pixel = Shader::Create( String8( "shaders\\standard.pixel" ), Shader::Type::Pixel );
		DD_ASSERT( pixel != nullptr );
		shaders.Add( pixel );

		ShaderHandle handle = ShaderProgram::Create( String8( name ), shaders );

		ShaderProgram& shader = *handle.Get();
		shader.Use( true );

		shader.SetPositionsName( "Position" );
		shader.SetNormalsName( "Normal" );

		shader.Use( false );

		return handle;
	}

	void Renderer::Initialize( EntityManager& entity_manager )
	{
		{
			EntityHandle directionalLight = entity_manager.CreateEntity<LightComponent, TransformComponent>();
			ComponentHandle<LightComponent> light = directionalLight.Get<LightComponent>();
			light.Write()->IsDirectional = true;
			light.Write()->Colour = glm::vec3( 1, 1, 1 );
			light.Write()->Intensity = 0.5;

			ComponentHandle<TransformComponent> transform = directionalLight.Get<TransformComponent>();
			glm::vec3 direction( 0.5, 0.4, -0.3 );
			transform.Write()->SetLocalPosition( direction );
		}

		m_createLight = true;
	}

	void Renderer::RenderInit( const EntityManager& entity_manager, const ICamera& camera )
	{
		m_shaders.Add( CreateShaders( "mesh" ) );

		m_frustum = new Frustum();
		m_frustum->CreateRenderData( m_shaders[0] );
		m_frustum->Update( camera );

		m_unitCube = Mesh::Create( "cube", m_shaders[ 0 ] );
		m_unitCube.Get()->MakeUnitCube();

		CreateFrameBuffer( m_window.GetSize() );

		m_previousSize = m_window.GetSize();
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

		delete m_frustum;
		m_frustum = nullptr;

		m_shaders.Clear();
	}

	EntityHandle Renderer::CreateMeshEntity( EntityManager& entityManager, MeshHandle mesh_h, ShaderHandle shader, glm::vec4 colour, const glm::mat4& transform )
	{
		EntityHandle handle = entityManager.CreateEntity<TransformComponent, MeshComponent>();

		ComponentHandle<TransformComponent> transform_cmp = handle.Get<TransformComponent>();
		transform_cmp.Write()->SetLocalTransform( transform );

		ComponentHandle<MeshComponent> mesh_cmp = handle.Get<MeshComponent>();
		mesh_cmp.Write()->Mesh = mesh_h;
		mesh_cmp.Write()->Colour = colour;
		mesh_cmp.Write()->Hidden = false;

		return handle;
	}

	void Renderer::DrawDebugInternal()
	{
		ImGui::Value( "Meshes", m_meshCount );
		ImGui::Value( "Unculled Meshes", m_frustumMeshCount );

		ImGui::Checkbox( "Draw Depth", &m_debugDrawDepth );
		ImGui::Checkbox( "Draw Bounds", &m_debugDrawBounds );
		ImGui::Checkbox( "Draw Standard", &m_debugDrawStandard );

		if( ImGui::Checkbox( "Draw Axes", &m_debugDrawAxes ) )
		{
			m_xAxis.Get<MeshComponent>().Write()->Hidden = !m_debugDrawAxes;
			m_yAxis.Get<MeshComponent>().Write()->Hidden = !m_debugDrawAxes;
			m_zAxis.Get<MeshComponent>().Write()->Hidden = !m_debugDrawAxes;
		}	

		if( ImGui::TreeNodeEx( "Wireframe", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			ImGui::Checkbox( "Enabled", &m_debugWireframe );

			ImGui::DragFloat( "Width", &m_debugWireframeWidth, 0.01f, 0.0f, 10.0f );

			ImGui::ColorEdit3( "Colour", glm::value_ptr( m_debugWireframeColour ) );

			ImGui::DragFloat( "Edge Width", &m_debugWireframeEdgeWidth, 0.01f, 0.0f, m_debugWireframeWidth );

			ImGui::ColorEdit3( "Edge Colour", glm::value_ptr( m_debugWireframeEdgeColour ) );

			ImGui::DragFloat( "Max Distance", &m_debugWireframeMaxDistance, 1.0f, 0.0f, 1000.0f );

			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Lighting", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			for( int i = 0; i < m_debugLights.Size(); ++i )
			{
				String64 lightLabel( "Light " );
				char buffer[16];
				_itoa_s( i, buffer, 10 );
				lightLabel += buffer;

				EntityHandle entity = m_debugLights[ i ];
				ComponentHandle<LightComponent> light = entity.Get<LightComponent>();
				ComponentHandle<TransformComponent> transform = entity.Get<TransformComponent>();

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


			if( m_debugLights.Size() < 10 )
			{
				if( ImGui::Button( "Create Light" ) )
				{
					m_createLight = true;
				}
			}

			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Frustum", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			if( ImGui::Checkbox( "Freeze", &m_debugFreezeFrustum ) && m_debugFreezeFrustum )
			{
				m_forceUpdateFrustum = true;
			}

			ImGui::Checkbox( "Enable Culling", &m_frustumCulling );
			ImGui::Checkbox( "Highlight Meshes in Frustum", &m_debugHighlightFrustumMeshes );

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

	void Renderer::RenderMesh( EntityHandle entity, const MeshComponent* mesh_cmp, const TransformComponent* transform_cmp, 
		const Vector<EntityHandle>& lights, const ICamera& camera, const MousePicking* mousePicking )
	{
		glm::mat4 transform = transform_cmp->GetWorldTransform();

		Mesh* mesh = mesh_cmp->Mesh.Get();
		if( m_debugDrawBounds )
		{
			mesh = m_unitCube.Get();

			glm::vec3 scale = mesh_cmp->Bounds.Max - mesh_cmp->Bounds.Min;
			transform = glm::translate( mesh_cmp->Bounds.Center() ) * glm::scale( scale / 2.0f );
		}

		if( mesh != nullptr && !mesh_cmp->Hidden )
		{
			glm::vec4 debugMultiplier( 1, 1, 1, 1 );

			if( mousePicking != nullptr )
			{
				if( entity == mousePicking->GetFocusedMesh() )
				{
					debugMultiplier.z = 1.5f;
				}

				if( entity == mousePicking->GetSelectedMesh() )
				{
					debugMultiplier.y = 1.5f;
				}
			}

			// check if it intersects with the frustum
			if( !m_frustumCulling || m_frustum->Intersects( mesh_cmp->Bounds ) )
			{
				if( m_debugHighlightFrustumMeshes )
				{
					debugMultiplier.x = 1.5f;
				}

				ShaderProgram* shader = mesh->GetShader();
				shader->Use( true );

				int lightCount = lights.Size();
				DD_ASSERT( lightCount <= 10 );
				shader->SetUniform( "LightCount", lightCount );

				int index = 0;
				for( EntityHandle light : lights )
				{
					ComponentHandle<TransformComponent> transformCmp = light.Get<TransformComponent>();
					ComponentHandle<LightComponent> lightCmp = light.Get<LightComponent>();

					glm::vec4 position( transformCmp.Read()->GetWorldPosition(), 1 );
					if( lightCmp.Read()->IsDirectional )
					{
						position.w = 0;
					}

					shader->SetUniform( GetArrayUniformName( "Lights", index, "Position" ).c_str(), position );
					shader->SetUniform( GetArrayUniformName( "Lights", index, "Colour" ).c_str(), lightCmp.Read()->Colour );
					shader->SetUniform( GetArrayUniformName( "Lights", index, "Intensity" ).c_str(), lightCmp.Read()->Intensity );
					shader->SetUniform( GetArrayUniformName( "Lights", index, "Attenuation" ).c_str(), lightCmp.Read()->Attenuation );
					shader->SetUniform( GetArrayUniformName( "Lights", index, "AmbientStrength" ).c_str(), lightCmp.Read()->Ambient );
					shader->SetUniform( GetArrayUniformName( "Lights", index, "SpecularStrength" ).c_str(), lightCmp.Read()->Specular );

					++index;
				}

				shader->SetUniform( "DrawStandard", m_debugDrawStandard );
				shader->SetUniform( "Wireframe.Enabled", m_debugWireframe );
				shader->SetUniform( "Wireframe.Colour", m_debugWireframeColour );
				shader->SetUniform( "Wireframe.Width", m_debugWireframeWidth );

				shader->SetUniform( "Wireframe.EdgeColour", m_debugWireframeEdgeColour );
				shader->SetUniform( "Wireframe.EdgeWidth", m_debugWireframeEdgeWidth );
				shader->SetUniform( "Wireframe.MaxDistance", m_debugWireframeMaxDistance );			

				glm::vec4 colour = mesh_cmp->Colour * debugMultiplier;
				mesh->SetColourMultiplier( colour );
				mesh->Render( camera, *shader, transform );

				shader->Use( false );

				++m_frustumMeshCount;
			}
		}

		++m_meshCount;
	}

	void Renderer::CreateDebugMeshGrid( EntityManager& entityManager )
	{
		if( m_debugMeshGridCreated || !m_createDebugMeshGrid )
			return;

		for( int x = -5; x < 5; ++x )
		{
			for( int y = -5; y < 5; ++y )
			{
				for( int z = -5; z < 5; ++z )
				{
					CreateMeshEntity( entityManager, m_unitCube, m_shaders[0], glm::vec4( 0.5, 0.5, 0.5, 1 ), glm::translate( glm::vec3( 10.f * x, 10.f * y, 10.f * z ) ) );
				}
			}
		}

		m_debugMeshGridCreated = true;
	}

	EntityHandle Renderer::CreatePointLight( EntityManager& entityManager )
	{
		glm::mat4 transform = glm::translate( glm::vec3( 0 ) ) * glm::scale( glm::vec3( 0.1f ) );
		EntityHandle entity = CreateMeshEntity( entityManager, m_unitCube, m_shaders[ 0 ], glm::vec4( 1 ), transform );
		ComponentHandle<LightComponent> light = entityManager.AddComponent<LightComponent>( entity );
		light.Write()->Ambient = 0.01f;

		return entity;
	}

	void Renderer::UpdateDebugPointLights( EntityManager& entity_manager )
	{
		if( m_createLight )
		{
			CreatePointLight( entity_manager );
			m_createLight = false;
		}

		if( m_deleteLight.IsValid() )
		{
			entity_manager.Destroy( m_deleteLight );
			m_deleteLight = EntityHandle();
		}

		entity_manager.ForAllWithReadable<MeshComponent, LightComponent>( []( auto entity, auto mesh, auto light )
		{
			if( mesh.Write() != nullptr )
			{
				mesh.Write()->Colour = glm::vec4( light.Read()->Colour, 1);
			}
		} );
	}

	void Renderer::Update( EntityManager& entity_manager, float delta_t )
	{
		if( !m_xAxis.IsValid() )
		{
			m_xAxis = CreateMeshEntity( entity_manager, m_unitCube, m_shaders[ 0 ], glm::vec4( 1, 0, 0, 1 ), glm::scale( glm::vec3( 100, 0.05f, 0.05f ) ) );
			m_yAxis = CreateMeshEntity( entity_manager, m_unitCube, m_shaders[ 0 ], glm::vec4( 0, 1, 0, 1 ), glm::scale( glm::vec3( 0.05f, 100, 0.05f ) ) );
			m_zAxis = CreateMeshEntity( entity_manager, m_unitCube, m_shaders[ 0 ], glm::vec4( 0, 0, 1, 1 ), glm::scale( glm::vec3( 0.05f, 0.05f, 100 ) ) );
		}

		CreateDebugMeshGrid( entity_manager );
		UpdateDebugPointLights( entity_manager );

		m_debugLights = entity_manager.FindAllWithWritable<LightComponent, TransformComponent>();
	}

	void Renderer::RenderDebug( IRenderer& debug_render )
	{
		m_framebuffer.BindDraw();

		debug_render.RenderDebug();

		m_framebuffer.UnbindDraw();
	}

	void Renderer::BeginRender( const ICamera& camera )
	{
		if( m_reloadShaders )
		{
			ShaderProgram::ReloadAllShaders();
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
	}

	void Renderer::Render( const EntityManager& entity_manager, const ICamera& camera )
	{
		DD_ASSERT( m_window.IsContextValid() );

		m_framebuffer.BindDraw();

		m_frustumMeshCount = 0;
		m_meshCount = 0;

		SetRenderState();

		if( !m_debugFreezeFrustum && camera.IsDirty() || m_forceUpdateFrustum )
		{
			m_frustum->Update( camera );

			m_forceUpdateFrustum = false;
		}

		Vector<EntityHandle> lights = entity_manager.FindAllWithReadable<LightComponent, TransformComponent>();

		entity_manager.ForAllWithReadable<MeshComponent, TransformComponent>( [this, &lights, &camera]( auto entity, auto mesh, auto transform )
		{ 
			RenderMesh( entity, mesh.Read(), transform.Read(), lights, camera, m_mousePicking );
		} );

		if( m_debugFreezeFrustum )
		{
			m_frustum->Render( camera );
		}

		m_framebuffer.UnbindDraw();
	}

	void Renderer::EndRender( const ICamera& camera )
	{
		m_framebuffer.BindRead();

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
		glViewport( 0, 0, m_window.GetWidth(), m_window.GetHeight() );
		CheckGLError();

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