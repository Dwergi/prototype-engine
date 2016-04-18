#include "PrecompiledHeader.h"
#include "Renderer.h"

#include "AABB.h"
#include "Camera.h"
#include "EntitySystem.h"
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
		m_window( nullptr )
	{

	}

	Renderer::~Renderer()
	{

	}

	// TODO: Don't do this.
	ShaderHandle CreateShaders()
	{
		Vector<Shader> shaders;

		Shader vert = Shader::Create( String8( "vertex" ), String8( "" ), Shader::Type::Vertex );
		DD_ASSERT( vert.IsValid() );
		shaders.Add( vert );

		Shader pixel = Shader::Create( String8( "pixel" ), String8( "" ), Shader::Type::Pixel );
		DD_ASSERT( pixel.IsValid() );
		shaders.Add( pixel );

		ShaderHandle handle = ShaderProgram::Create( String8( "default" ), shaders );
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

		ImGui::SetWindowPos( ImVec2( ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x - 2, 2 ) );

		ImGui::Text( "Meshes: %d", m_meshCount );
		ImGui::End();
	}

	void Renderer::Init( Window& window )
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

		m_shaders.Add( CreateShaders() );
		ShaderProgram* shader = m_shaders[0].Get();

		MeshHandle mesh = Mesh::Create( "cube", *shader );
		m_meshes.Add( mesh );

		EntityHandle handle = Services::Get<EntitySystem>().CreateEntity<TransformComponent, MeshComponent>();
		
		TransformComponent* transform_cmp = Services::GetWritePool<TransformComponent>().Find( handle );
		transform_cmp->Position = glm::vec3( 0, 0, 0 );

		MeshComponent* mesh_cmp = Services::GetWritePool<MeshComponent>().Find( handle );
		mesh_cmp->SetMesh( mesh );

		// TODO: Does not belong here.
		Services::GetDoubleBuffer<TransformComponent>().Swap();
		Services::GetDoubleBuffer<MeshComponent>().Swap();
		Services::GetDoubleBuffer<TransformComponent>().Duplicate();
		Services::GetDoubleBuffer<MeshComponent>().Duplicate();
	}

	void RenderAxes( Camera& camera, ShaderProgram& program )
	{
		// Render axes
		{
			MeshHandle x_axis = Mesh::Create( "x_axis", program );
			Mesh* x_mesh = x_axis.Get();

			x_mesh->SetColourMultiplier( glm::vec4( 1, 0, 0, 1 ) );

			glm::mat4 x_transform = glm::scale( glm::vec3( 100, 0.05f, 0.05f ) );
			x_mesh->Render( camera, x_transform );
		}

		{
			MeshHandle y_axis = Mesh::Create( "y_axis", program );
			Mesh* y_mesh = y_axis.Get();

			y_mesh->SetColourMultiplier( glm::vec4( 0, 1, 0, 1 ) );

			glm::mat4 y_transform = glm::scale( glm::vec3( 0.05f, 100, 0.05f ) );
			y_mesh->Render( camera, y_transform );
		}

		{
			MeshHandle z_axis = Mesh::Create( "z_axis", program );
			Mesh* z_mesh = z_axis.Get();

			z_mesh->SetColourMultiplier( glm::vec4( 0, 0, 1, 1 ) );

			glm::mat4 z_transform = glm::scale( glm::vec3( 0.05f, 0.05f, 100 ) );
			z_mesh->Render( camera, z_transform );
		}

	}

	void Renderer::Render( float delta_t )
	{
		Frustum frustum( *m_camera );

		const MeshComponent::Pool& meshes = Services::GetReadPool<MeshComponent>();
		const TransformComponent::Pool& transforms = Services::GetReadPool<TransformComponent>();

		for( MeshComponent& mesh_cmp : meshes )
		{
			Mesh* mesh = mesh_cmp.GetMesh().Get();
			if( mesh != nullptr )
			{
				TransformComponent* transform_cmp = transforms.Find( mesh_cmp.Entity );
				if( transform_cmp != nullptr )
				{
					glm::vec4 multiplier( 0, 1, 0, 1 );
					if( !frustum.Intersects( mesh->Bounds() ) )
						multiplier = glm::vec4( 1, 0, 0, 1 );

					mesh->SetColourMultiplier( multiplier );
					mesh->Render( *m_camera, glm::translate( transform_cmp->Position ) );

					++m_meshCount;
				}
			}
		}

		RenderAxes( *m_camera, *m_shaders[0].Get() );

		// Render frustum
		MeshHandle test_h = Mesh::Create( "test", *m_shaders[0].Get() );
		Mesh* test_mesh = test_h.Get();

		test_mesh->SetColourMultiplier( glm::vec4( 1, 1, 0, 1.0f ) );

		glm::mat4 test_transform = glm::translate( glm::vec3( 5, 5, 5 ) );
		test_mesh->Render( *m_camera, test_transform );
	}

	Camera& Renderer::GetCamera() const
	{
		return *m_camera;
	}
}