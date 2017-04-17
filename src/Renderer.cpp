#include "PrecompiledHeader.h"
#include "Renderer.h"

#include "AABB.h"
#include "Camera.h"
#include "EntityManager.h"
#include "Frustum.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "TransformComponent.h"
#include "Window.h"

#include "GL/gl3w.h"

#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"

namespace dd
{
	Renderer::Renderer() :
		m_meshCount( 0 ),
		m_camera( nullptr ),
		m_window( nullptr ),
		m_entityManager( nullptr ),
		m_drawAxes( true ),
		m_debugHighlightMeshes( false ),
		m_debugHitTestMeshes( false ),
		m_frustumMeshCount( 0 ),
		m_debugFocusedMeshDistance( FLT_MAX ),
		m_debugMeshGridCreated( false )
	{

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

			Shader vert = Shader::Create( String8( "vertex" ), String8( "" ), Shader::Type::Vertex );
			DD_ASSERT( vert.IsValid() );
			shaders.Add( vert );

			Shader pixel = Shader::Create( String8( "pixel" ), String8( "" ), Shader::Type::Pixel );
			DD_ASSERT( pixel.IsValid() );
			shaders.Add( pixel );

			ShaderHandle handle = ShaderProgram::Create( String8( name ), shaders );
			return handle;
		}
	}

	void Renderer::Initialize( Window& window, EntityManager& entity_manager )
	{
		m_camera = new Camera( window );
		m_camera->SetPosition( glm::vec3( 10, 0, 10 ) );
		m_camera->SetDirection( glm::vec3( -1, 0, -1 ) );

		m_frustum = new Frustum( *m_camera );
		m_frustum->ResetFrustum( *m_camera );

		m_shaders.Add( CreateShaders( "mesh" ) );

		m_entityManager = &entity_manager;

		m_xAxis = CreateMeshEntity( *m_entityManager, "x_axis", m_shaders[0], glm::vec4( 1, 0, 0, 1 ), glm::scale( glm::vec3( 100, 0.05f, 0.05f ) ) );
		m_yAxis = CreateMeshEntity( *m_entityManager, "y_axis", m_shaders[0], glm::vec4( 0, 1, 0, 1 ), glm::scale( glm::vec3( 0.05f, 100, 0.05f ) ) );
		m_zAxis = CreateMeshEntity( *m_entityManager, "z_axis", m_shaders[0], glm::vec4( 0, 0, 1, 1 ), glm::scale( glm::vec3( 0.05f, 0.05f, 100 ) ) );
	}

	EntityHandle Renderer::CreateMeshEntity( EntityManager& entity_manager, const char* meshName, ShaderHandle shader, glm::vec4& colour, const glm::mat4& transform )
	{
		MeshHandle mesh_h = Mesh::Create( meshName, shader );
		mesh_h.Get()->MakeUnitCube();

		EntityHandle handle = entity_manager.CreateEntity<TransformComponent, MeshComponent>();

		TransformComponent* transform_cmp = entity_manager.GetWritable<TransformComponent>( handle );
		transform_cmp->Transform = transform;

		MeshComponent* mesh_cmp = entity_manager.GetWritable<MeshComponent>( handle );
		mesh_cmp->Mesh = mesh_h;
		mesh_cmp->Colour = colour;
		mesh_cmp->Hidden = false;
		mesh_cmp->UpdateBounds( transform );

		return handle;
	}

	void Renderer::DrawDebugUI()
	{
		bool open = true;
		if( !ImGui::Begin( "Renderer", &open, ImVec2( 0, 0 ), 0.4f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings ) )
		{
			ImGui::End();
			return;
		}

		ImGui::SetWindowPos( ImVec2( ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x - 2, ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y - 2 ) );

		ImGui::Text( "Meshes: %d", m_meshCount );
		ImGui::Text( "Unculled Meshes: %d", m_frustumMeshCount );

		if( ImGui::Checkbox( "Draw Axes", &m_drawAxes ) )
		{
			m_xAxis.Get<MeshComponent>().Write()->Hidden = !m_drawAxes;
			m_yAxis.Get<MeshComponent>().Write()->Hidden = !m_drawAxes;
			m_zAxis.Get<MeshComponent>().Write()->Hidden = !m_drawAxes;
		}

		ImGui::Checkbox( "Hit Test Meshes", &m_debugHitTestMeshes );

		if( m_debugHitTestMeshes )
		{
			if( m_debugFocusedMesh.IsValid() )
			{
				glm::vec3 focusedMesh = m_debugFocusedMesh.Get<TransformComponent>().Read()->GetPosition();
				ImGui::Text( "Focused Mesh: %.2f %.2f %.2f", focusedMesh.x, focusedMesh.y, focusedMesh.z );
			}
			else
			{
				ImGui::Text( "Focused Mesh: <none>" );
			}
		}

		if( ImGui::TreeNodeEx( "Frustum", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			ImGui::Checkbox( "Highlight Meshes in Frustum", &m_debugHighlightMeshes );

			ImGui::TreePop();
		}

		if( !m_debugMeshGridCreated && ImGui::Button( "Create Mesh Grid" ) )
		{
			for( int x = -5; x < 5; ++x )
			{
				for( int y = -5; y < 5; ++y )
				{
					for( int z = -5; z < 5; ++z )
					{
						CreateMeshEntity( *m_entityManager, "cube", m_shaders[0], glm::vec4( 0.5, 0.5, 0.5, 1 ), glm::translate( glm::vec3( 10.f * x, 10.f * y, 10.f * z ) ) );
					}
				}
			}

			m_debugMeshGridCreated = true;
		}

		ImGui::End();
	}

	void Renderer::SetRenderState()
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
	
	void Renderer::HitTestMesh( EntityHandle entity, ComponentHandle<MeshComponent> mesh_handle )
	{
		if( entity == m_xAxis || entity == m_yAxis || entity == m_zAxis )
			return;

		const MeshComponent* mesh_cmp = mesh_handle.Read();
		Mesh* mesh = mesh_cmp->Mesh.Get();
		if( mesh != nullptr && !mesh_cmp->Hidden )
		{
			const AABB& bounds = mesh_cmp->Bounds;

			float distance;
			if( bounds.IntersectsRay( m_camera->GetPosition(), m_camera->GetDirection(), distance ) )
			{
				if( distance < m_debugFocusedMeshDistance )
				{
					m_debugFocusedMeshDistance = distance;
					m_debugFocusedMesh = entity;
				}
			}
		}
	}

	void Renderer::RenderMesh( EntityHandle entity, ComponentHandle<MeshComponent> mesh_handle, ComponentHandle<TransformComponent> transform_handle )
	{
		const MeshComponent* mesh_cmp = mesh_handle.Read();
		Mesh* mesh = mesh_cmp->Mesh.Get();
		if( mesh != nullptr && !mesh_cmp->Hidden )
		{
			const glm::mat4& transform = transform_handle.Read()->Transform;

			glm::vec4 debugMultiplier( 1, 1, 1, 1 );

			if( entity == m_debugFocusedMesh )
			{
				debugMultiplier.y = 1.5f;
			}

			// check if it intersects with the frustum
			if( m_frustum->Intersects( mesh_cmp->Bounds ) )
			{
				if( m_debugHighlightMeshes )
				{
					debugMultiplier.x = 1.5f;
				}

				glm::vec4 colour = mesh_cmp->Colour * debugMultiplier;
				mesh->SetColourMultiplier( colour );
				mesh->Render( *m_camera, transform );

				++m_frustumMeshCount;
			}
		}

		++m_meshCount;
	}

	void Renderer::Render( float delta_t )
	{
		DD_ASSERT( m_entityManager != nullptr );

		m_debugFocusedMeshDistance = 0;
		m_debugFocusedMesh = EntityHandle();
		m_debugFocusedMeshDistance = FLT_MAX;
		m_frustumMeshCount = 0;
		m_meshCount = 0;

		SetRenderState();

		m_frustum->ResetFrustum( *m_camera );

		if( m_debugHitTestMeshes )
		{
			m_entityManager->ForAllWithReadable<MeshComponent>( [this]( auto entity, auto mesh ) { HitTestMesh( entity, mesh ); } );
		}

		m_entityManager->ForAllWithReadable<MeshComponent, TransformComponent>( [this]( auto entity, auto mesh, auto transform ) { RenderMesh( entity, mesh, transform ); } );

		DrawDebugUI();
	}

	Camera& Renderer::GetCamera() const
	{
		return *m_camera;
	}
}