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
		m_defaultShader( nullptr ),
		m_entityManager( nullptr ),
		m_drawAxes( true ),
		m_debugFrustumEnabled( false ),
		m_debugHighlightMeshes( true ),
		m_debugFrustum( nullptr ),
		m_debugCamera( nullptr ),
		m_debugFrustumNear( 0.1f ),
		m_debugFrustumFar( 1.0f ),
		m_debugFrustumMeshCount( 0 ),
		m_debugFocusedMeshDistance( FLT_MAX )
	{

	}

	Renderer::~Renderer()
	{

	}

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

		if( ImGui::Checkbox( "Draw Axes", &m_drawAxes ) )
		{
			m_xAxis.Get<MeshComponent>().Write()->Hidden = !m_drawAxes;
			m_yAxis.Get<MeshComponent>().Write()->Hidden = !m_drawAxes;
			m_zAxis.Get<MeshComponent>().Write()->Hidden = !m_drawAxes;
		}

		if( m_debugFocusedMesh.IsValid() )
		{
			glm::vec3 focusedMesh = m_debugFocusedMesh.Get<TransformComponent>().Read()->GetPosition();
			ImGui::Text( "Focused Mesh: %.2f %.2f %.2f", focusedMesh.x, focusedMesh.y, focusedMesh.z );
		}
		else
		{
			ImGui::Text( "Focused Mesh: <none>" );
		}

		if( ImGui::CollapsingHeader( "Frustum" ) )
		{
			ImGui::Checkbox( "Highlight Meshes in Frustum", &m_debugHighlightMeshes );
			ImGui::Checkbox( "Draw Debug Frustum", &m_debugFrustumEnabled );

			ImGui::SliderFloat( "Frustum Near", &m_debugFrustumNear, 0.01f, m_debugFrustumFar - 0.01f );
			ImGui::SliderFloat( "Frustum Far", &m_debugFrustumFar, m_debugFrustumNear, 100.f );

			ImGui::Text( "Meshes in Frustum: %d", m_debugFrustumMeshCount );
		}

		ImGui::End();
	}

	void Renderer::CreateDebugFrustum( Window& window )
	{
		m_debugCamera = new Camera( window );
		m_debugCamera->SetPosition( glm::vec3( 0, 0, 0 ) );
		m_debugCamera->SetDirection( glm::vec3( 1, 0, 0 ) );
		m_debugCamera->SetVerticalFOV( m_camera->GetVerticalFOV() );
		m_debugCamera->SetAspectRatio( 16, 9 );
		m_debugCamera->SetNear( m_debugFrustumNear );
		m_debugCamera->SetFar( m_debugFrustumFar );

		m_debugFrustum = new Frustum( *m_debugCamera );
	}

	void Renderer::Initialize( Window& window, EntityManager& entity_manager )
	{
		m_camera = new Camera( window );
		m_camera->SetPosition( glm::vec3( 10, 0, 10 ) );
		m_camera->SetDirection( glm::vec3( -1, 0, -1 ) );

		m_debugFrustumNear = m_camera->GetNear();
		m_debugFrustumFar = m_camera->GetFar();

		m_shaders.Add( CreateShaders( "mesh" ) );
		m_defaultShader = m_shaders[0].Get();

		m_shaders.Add( CreateShaders( "frustum" ) );
		m_shaders.Add( CreateShaders( "terrain" ) );

		CreateDebugFrustum( window );

		m_entityManager = &entity_manager;

		for( int x = -5; x < 5; ++x )
		{
			for( int y = -5; y < 5; ++y )
			{
				for( int z = -5; z < 5; ++z )
				{
					CreateMeshEntity( *m_entityManager, "cube", *m_defaultShader, glm::vec4( 0.5, 0.5, 0.5, 1 ), glm::translate( glm::vec3( 10.f * x, 10.f * y, 10.f * z ) ) );
				}
			}
		}

		m_xAxis = CreateMeshEntity( *m_entityManager, "x_axis", *m_defaultShader, glm::vec4( 1, 0, 0, 1 ), glm::scale( glm::vec3( 100, 0.05f, 0.05f ) ) );
		m_yAxis = CreateMeshEntity( *m_entityManager, "y_axis", *m_defaultShader, glm::vec4( 0, 1, 0, 1 ), glm::scale( glm::vec3( 0.05f, 100, 0.05f ) ) );
		m_zAxis = CreateMeshEntity( *m_entityManager, "z_axis", *m_defaultShader, glm::vec4( 0, 0, 1, 1 ), glm::scale( glm::vec3( 0.05f, 0.05f, 100 ) ) );
	}

	EntityHandle Renderer::CreateMeshEntity( EntityManager& entity_manager, const char* meshName, ShaderProgram& shader, glm::vec4& colour, const glm::mat4& transform )
	{
		MeshHandle mesh_h = Mesh::Create( meshName, shader );

		EntityHandle handle = entity_manager.CreateEntity<TransformComponent, MeshComponent>();

		TransformComponent* transform_cmp = entity_manager.GetWritable<TransformComponent>( handle );
		transform_cmp->Transform = transform;

		MeshComponent* mesh_cmp = entity_manager.GetWritable<MeshComponent>( handle );
		mesh_cmp->Mesh = mesh_h;
		mesh_cmp->Colour = colour;
		mesh_cmp->Hidden = false;

		return handle;
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

		// alpha blending
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}

	void Renderer::HitTestMesh( EntityHandle entity, ComponentHandle<MeshComponent> mesh_cmp, ComponentHandle<TransformComponent> transform_cmp )
	{
		if( entity == m_xAxis || entity == m_yAxis || entity == m_zAxis )
			return;

		Mesh* mesh = mesh_cmp.Read()->Mesh.Get();
		if( mesh != nullptr && !mesh_cmp.Read()->Hidden )
		{
			const glm::mat4& transform = transform_cmp.Read()->Transform;
			AABB bounds = mesh->Bounds().GetTransformed( transform );

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

	void Renderer::RenderMesh( EntityHandle entity, ComponentHandle<MeshComponent> mesh_cmp, ComponentHandle<TransformComponent> transform_cmp )
	{
		Mesh* mesh = mesh_cmp.Read()->Mesh.Get();
		if( mesh != nullptr && !mesh_cmp.Read()->Hidden )
		{
			const glm::mat4& transform = transform_cmp.Read()->Transform;
			AABB bounds = mesh->Bounds().GetTransformed( transform );

			glm::vec4 debugMultiplier( 1, 1, 1, 1 );

			bool inFrustum = true;
			
			if( entity == m_debugFocusedMesh )
			{
				debugMultiplier.y = 1.5f;
			}

			// check if it intersects with the debug frustum and tint it if it does
			if( m_debugHighlightMeshes )
			{
				inFrustum = m_debugFrustum->Intersects( bounds );
				if( inFrustum )
				{
					debugMultiplier.x = 1.5f;
					++m_debugFrustumMeshCount;
				}
			}

			if( inFrustum )
			{
				glm::vec4 colour = mesh_cmp.Read()->Colour * debugMultiplier;
				mesh->SetColourMultiplier( colour );
				mesh->Render( *m_camera, transform );

				++m_meshCount;
			}
		}
	}

	void Renderer::RenderDebugFrustum()
	{
		m_debugCamera->SetPosition( m_camera->GetPosition() );
		m_debugCamera->SetDirection( m_camera->GetDirection() );
		m_debugCamera->SetVerticalFOV( m_camera->GetVerticalFOV() );
		m_debugCamera->SetNear( m_debugFrustumNear );
		m_debugCamera->SetFar( m_debugFrustumFar );

		m_debugFrustum->ResetFrustum( *m_debugCamera );

		if( m_debugFrustumEnabled )
		{
			m_debugFrustum->Render( *m_camera, *m_shaders[1].Get() );
		}
	}

	void Renderer::Render( float delta_t )
	{
		DD_ASSERT( m_entityManager != nullptr );

		m_debugFocusedMeshDistance = 0;
		m_debugFrustumMeshCount = 0;
		m_debugFocusedMesh = EntityHandle();
		m_debugFocusedMeshDistance = FLT_MAX;
		m_meshCount = 0;

		SetRenderState();

		m_entityManager->ForAllWithReadable<MeshComponent, TransformComponent>( [this]( auto entity, auto mesh, auto transform ) { HitTestMesh( entity, mesh, transform ); } );
		m_entityManager->ForAllWithReadable<MeshComponent, TransformComponent>( [this]( auto entity, auto mesh, auto transform ) { RenderMesh( entity, mesh, transform ); } );

		RenderDebugFrustum();

		DrawDebugUI();
	}

	Camera& Renderer::GetCamera() const
	{
		return *m_camera;
	}
}