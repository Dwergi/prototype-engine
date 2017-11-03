//
// Renderer.cpp - Master renderer class, coordinates all rendering.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#include "PrecompiledHeader.h"
#include "Renderer.h"

#include "AABB.h"
#include "Camera.h"
#include "EntityManager.h"
#include "Frustum.h"
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

#include "imgui/imgui.h"

#pragma optimize( "", off )

namespace dd
{
	Renderer::Renderer() :
		m_meshCount( 0 ),
		m_camera( nullptr ),
		m_window( nullptr ),
		m_frustumMeshCount( 0 )
	{
		m_debugWireframeColour = glm::vec3( 0, 1, 0 );
	}

	Renderer::~Renderer()
	{

	}

	namespace
	{
		// TODO: Don't do this.
		ShaderHandle CreateShaders( const char* name )
		{
			Vector<Shader> shaders;

			Shader vert = Shader::Create( String8( "vertex" ), String8( "shaders\\vertex.glsl" ), Shader::Type::Vertex );
			DD_ASSERT( vert.IsValid() );
			shaders.Add( vert );

			Shader geom = Shader::Create( String8( "geometry" ), String8( "shaders\\geometry.glsl" ), Shader::Type::Geometry );
			DD_ASSERT( geom.IsValid() );
			shaders.Add( geom );

			Shader pixel = Shader::Create( String8( "pixel" ), String8( "shaders\\pixel.glsl" ), Shader::Type::Pixel );
			DD_ASSERT( pixel.IsValid() );
			shaders.Add( pixel );

			ShaderHandle handle = ShaderProgram::Create( String8( name ), shaders );
			return handle;
		}
	}

	void Renderer::Initialize( Window& window, EntityManager& entityManager )
	{
		m_window = &window;
		m_camera = new Camera( *m_window );
		m_frustum = new Frustum();
		m_frustum->ResetFrustum( *m_camera );

		m_shaders.Add( CreateShaders( "mesh" ) );

		m_unitCube = Mesh::Create( "cube", m_shaders[0] );
		m_unitCube.Get()->MakeUnitCube();

		{
			EntityHandle directionalLight = entityManager.CreateEntity<LightComponent, TransformComponent>();
			ComponentHandle<LightComponent> light = directionalLight.Get<LightComponent>();
			light.Write()->IsDirectional = true;
			light.Write()->Colour = glm::vec3( 1, 1, 1 );
			light.Write()->Intensity = 0.5;

			ComponentHandle<TransformComponent> transform = directionalLight.Get<TransformComponent>();
			glm::vec3 direction( 0.5, 0.4, -0.3 );
			transform.Write()->SetLocalPosition( direction );
		}

		{
			EntityHandle pointLight = CreatePointLight( entityManager );
			ComponentHandle<LightComponent> light = pointLight.Get<LightComponent>();
			light.Write()->Colour = glm::vec3( 1 );
			light.Write()->Intensity = 10;

			ComponentHandle<TransformComponent> transform = pointLight.Get<TransformComponent>();
			transform.Write()->SetLocalPosition( glm::vec3( 0, 10, 10 ) );
		}

		m_xAxis = CreateMeshEntity( entityManager, m_unitCube, m_shaders[0], glm::vec4( 1, 0, 0, 1 ), glm::scale( glm::vec3( 100, 0.05f, 0.05f ) ) );
		m_yAxis = CreateMeshEntity( entityManager, m_unitCube, m_shaders[0], glm::vec4( 0, 1, 0, 1 ), glm::scale( glm::vec3( 0.05f, 100, 0.05f ) ) );
		m_zAxis = CreateMeshEntity( entityManager, m_unitCube, m_shaders[0], glm::vec4( 0, 0, 1, 1 ), glm::scale( glm::vec3( 0.05f, 0.05f, 100 ) ) );
	}

	void Renderer::Shutdown()
	{
		Mesh::Destroy( m_unitCube );

		delete m_frustum;
		m_frustum = nullptr;
		delete m_camera;
		m_camera = nullptr;

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
		mesh_cmp.Write()->UpdateBounds( transform );

		return handle;
	}

	void Renderer::DrawDebugInternal()
	{
		ImGui::Text( "Meshes: %d", m_meshCount );
		ImGui::Text( "Unculled Meshes: %d", m_frustumMeshCount );

		if( ImGui::Checkbox( "Draw Axes", &m_drawAxes ) )
		{
			m_xAxis.Get<MeshComponent>().Write()->Hidden = !m_drawAxes;
			m_yAxis.Get<MeshComponent>().Write()->Hidden = !m_drawAxes;
			m_zAxis.Get<MeshComponent>().Write()->Hidden = !m_drawAxes;
		}

		if( ImGui::TreeNodeEx( "Wireframe", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			ImGui::Checkbox( "Enabled?", &m_debugWireframe );

			ImGui::DragFloat( "Width", &m_debugWireframeWidth, 0.01f, 0.0f, 10.0f );

			float fltColour[3];
			fltColour[0] = m_debugWireframeColour.r; fltColour[1] = m_debugWireframeColour.g; fltColour[2] = m_debugWireframeColour.b;
			if( ImGui::ColorEdit3( "Colour", fltColour ) )
			{
				m_debugWireframeColour = glm::vec3( fltColour[0], fltColour[1], fltColour[2] );
			}

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

					glm::vec3 colour = light.Read()->Colour;
					float fltColour[3];
					fltColour[0] = colour.r; fltColour[1] = colour.g; fltColour[2] = colour.b;
					if( ImGui::ColorEdit3( "Colour", fltColour ) )
					{
						light.Write()->Colour = glm::vec3( fltColour[0], fltColour[1], fltColour[2] );
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

					glm::vec3 position = transform.Read()->GetLocalPosition();
					float fltPosition[3];
					fltPosition[0] = position.x; fltPosition[1] = position.y; fltPosition[2] = position.z;

					const char* positionLabel = light.Read()->IsDirectional ? "Direction" : "Position";
					if( ImGui::DragFloat3( positionLabel, fltPosition ) )
					{
						transform.Write()->SetLocalPosition( glm::vec3( fltPosition[0], fltPosition[1], fltPosition[2] ) );
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
			ImGui::Checkbox( "Highlight Meshes in Frustum", &m_debugHighlightFrustumMeshes );

			ImGui::TreePop();
		}

		if( !m_debugMeshGridCreated && ImGui::Button( "Create Mesh Grid" ) )
		{
			m_createDebugMeshGrid = true;
		}
	}

	void Renderer::SetRenderState()
	{
		if( !m_debugWireframe )
		{
			// depth test
			glEnable( GL_DEPTH_TEST );
			glDepthFunc( GL_LESS );

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

	String256 GetArrayUniformName( const char* arrayName, int index, const char* uniform )
	{
		String256 result;
		result += arrayName;
		result += "[";

		char buffer[32];
		_itoa_s( index, buffer, 10 );
		result += buffer;

		result += "].";
		result += uniform;
		return result;
	}

	void Renderer::RenderMesh( EntityHandle entity, ComponentHandle<MeshComponent> mesh_handle, ComponentHandle<TransformComponent> transform_handle, 
		const Vector<EntityHandle>& lights, const MousePicking* mousePicking )
	{
		const MeshComponent* mesh_cmp = mesh_handle.Read();
		Mesh* mesh = mesh_cmp->Mesh.Get();
		if( mesh != nullptr && !mesh_cmp->Hidden )
		{
			const glm::mat4& transform = transform_handle.Read()->GetWorldTransform();

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
			if( m_frustum->Intersects( mesh_cmp->Bounds ) )
			{
				if( m_debugHighlightFrustumMeshes )
				{
					debugMultiplier.x = 1.5f;
				}

				ShaderProgram* shader = mesh->GetShader().Get();
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

				shader->SetUniform( "UseWireframe", m_debugWireframe );
				shader->SetUniform( "WireframeColour", m_debugWireframeColour );
				shader->SetUniform( "WireframeWidth", m_debugWireframeWidth );

				shader->Use( false );

				glm::vec4 colour = mesh_cmp->Colour * debugMultiplier;
				mesh->SetColourMultiplier( colour );
				mesh->Render( *m_camera, transform );

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

	void Renderer::UpdateDebugPointLights( EntityManager& entityManager )
	{
		if( m_createLight )
		{
			CreatePointLight( entityManager );
			m_createLight = false;
		}

		if( m_deleteLight.IsValid() )
		{
			entityManager.Destroy( m_deleteLight );
			m_deleteLight = EntityHandle();
		}

		entityManager.ForAllWithReadable<MeshComponent, LightComponent>( []( auto entity, auto mesh, auto light )
		{
			if( mesh.Write() != nullptr )
			{
				mesh.Write()->Colour = glm::vec4( light.Read()->Colour, 1);
			}
		} );
	}

	void Renderer::Render( EntityManager& entityManager, float delta_t )
	{
		DD_ASSERT( m_window->IsContextValid() );

		m_frustumMeshCount = 0;
		m_meshCount = 0;

		CreateDebugMeshGrid( entityManager );

		SetRenderState();

		m_frustum->ResetFrustum( *m_camera );

		UpdateDebugPointLights( entityManager );
		Vector<EntityHandle> lights = entityManager.FindAllWithReadable<LightComponent, TransformComponent>();

		entityManager.ForAllWithReadable<MeshComponent, TransformComponent>( [this, &lights]( auto entity, auto mesh, auto transform )
		{ 
			RenderMesh( entity, mesh, transform, lights, m_mousePicking );
		} );

		m_debugLights = entityManager.FindAllWithWritable<LightComponent, TransformComponent>();
	}

	Camera& Renderer::GetCamera() const
	{
		return *m_camera;
	}
}