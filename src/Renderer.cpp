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
#include "TerrainSystem.h"
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
		m_terrain( nullptr ),
		m_defaultShader( nullptr )
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

	void Renderer::DrawDebugUI() const
	{
		bool open = true;
		if( !ImGui::Begin( "Renderer", &open, ImVec2( 0, 0 ), 0.4f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings ) )
		{
			ImGui::End();
			return;
		}

		ImGui::SetWindowPos( ImVec2( ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x - 2, ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y - 2 ) );

		ImGui::Text( "Meshes: %d", m_meshCount );
		ImGui::End();
	}

	TerrainChunk* chunk;

	void Renderer::Initialize( Window& window )
	{
		m_camera = new Camera( window );
		m_camera->SetPosition( glm::vec3( 10, 0, 10 ) );
		m_camera->SetDirection( glm::vec3( -1, 0, -1 ) );

		// depth test
		glEnable( GL_DEPTH_TEST );
		
		// backface culling
		glEnable( GL_CULL_FACE );
		glFrontFace( GL_CCW );
		glCullFace( GL_BACK );

		// alpha blending
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		m_shaders.Add( CreateShaders( "mesh" ) );
		m_defaultShader = m_shaders[0].Get();

		m_shaders.Add( CreateShaders( "frustum" ) );
		m_shaders.Add( CreateShaders( "terrain" ) );

		CreateMeshEntity( "cube", *m_defaultShader, glm::vec4( 1, 0, 1, 1 ), glm::mat4() );

		CreateMeshEntity( "x_axis", *m_defaultShader, glm::vec4( 1, 0, 0, 1 ), glm::scale( glm::vec3( 100, 0.05f, 0.05f ) ) );
		CreateMeshEntity( "y_axis", *m_defaultShader, glm::vec4( 0, 1, 0, 1 ), glm::scale( glm::vec3( 0.05f, 100, 0.05f ) ) );
		CreateMeshEntity( "z_axis", *m_defaultShader, glm::vec4( 0, 0, 1, 1 ), glm::scale( glm::vec3( 0.05f, 0.05f, 100 ) ) );

		// TODO: Does not belong here.
		Services::GetDoubleBuffer<TransformComponent>().Swap();
		Services::GetDoubleBuffer<MeshComponent>().Swap();
		Services::GetDoubleBuffer<TransformComponent>().Duplicate();
		Services::GetDoubleBuffer<MeshComponent>().Duplicate();
	}

	void Renderer::CreateMeshEntity( const char* meshName, ShaderProgram& shader, glm::vec4& colour, const glm::mat4& transform )
	{
		MeshHandle mesh_h = Mesh::Create( meshName, shader );

		EntityHandle handle = Services::Get<EntityManager>().CreateEntity<TransformComponent, MeshComponent>();

		TransformComponent* transform_cmp = Services::GetWritePool<TransformComponent>().Find( handle );
		transform_cmp->Transform = transform;

		MeshComponent* mesh_cmp = Services::GetWritePool<MeshComponent>().Find( handle );
		mesh_cmp->Mesh = mesh_h;
		mesh_cmp->Colour = colour;
	}

	void Renderer::Render( float delta_t )
	{
		m_meshCount = 0;

		/*Camera cam;
		cam.SetPosition( glm::vec3( 0, 0, 0 ) );
		cam.SetDirection( m_camera->GetDirection() );
		cam.SetVerticalFOV( m_camera->GetVerticalFOV() );
		cam.SetAspectRatio( 16, 9 );
		cam.SetNear( 0.1f );
		cam.SetFar( 5.0f );

		Frustum frustum( cam );
		frustum.CreateRenderResources();
		frustum.Render( *m_camera, *m_shaders[1].Get() );*/

		if( m_terrain != nullptr )
			m_terrain->Render( *m_camera, *m_shaders[2].Get() );	

		const MeshComponent::Pool& meshes = Services::GetReadPool<MeshComponent>();
		const TransformComponent::Pool& transforms = Services::GetReadPool<TransformComponent>();

		for( MeshComponent& mesh_cmp : meshes )
		{
			Mesh* mesh = mesh_cmp.Mesh.Get();
			if( mesh != nullptr )
			{
				TransformComponent* transform_cmp = transforms.Find( mesh_cmp.Entity );
				if( transform_cmp != nullptr )
				{
					mesh->SetColourMultiplier( mesh_cmp.Colour	);
					mesh->Render( *m_camera, transform_cmp->Transform );

					++m_meshCount;
				}
			}
		}
	}

	Camera& Renderer::GetCamera() const
	{
		return *m_camera;
	}
}