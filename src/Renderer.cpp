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
		m_camera->SetTransform( glm::lookAt( glm::vec3( 10, 0, 10 ), glm::vec3( 0, 0, 0 ), glm::vec3( 0, 1, 0 ) ) );

		glEnable( GL_DEPTH_TEST );
		glEnable( GL_CULL_FACE );
		glFrontFace( GL_CCW );
		glCullFace( GL_BACK );

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
					mesh->Render( *m_camera, transform_cmp->Position );
				}
			}
		}
	}

	Camera& Renderer::GetCamera() const
	{
		return *m_camera;
	}
}