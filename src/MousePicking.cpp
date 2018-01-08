//
// MousePicking.cpp - Class that handles mouse picking.
// Copyright (C) Sebastian Nordgren 
// April 26th 2017
//

#include "PrecompiledHeader.h"
#include "MousePicking.h"

#include "ICamera.h"
#include "EntityManager.h"
#include "Input.h"
#include "InputBindings.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "TransformComponent.h"
#include "Window.h"

#include "glm/gtx/rotate_vector.hpp"

#include "imgui/imgui.h"

namespace dd
{
	MousePicking::MousePicking( const Window& window, const ICamera& camera, const Input& input ) : 
		m_window( window ),
		m_camera( camera ),
		m_input( input ),
		m_rtt( window )
	{
	}

	void MousePicking::BindActions( InputBindings& bindings )
	{
		auto handle_input = std::bind( &MousePicking::HandleInput, std::ref( *this ), std::placeholders::_1, std::placeholders::_2 );
		bindings.RegisterHandler( InputAction::SELECT_MESH, handle_input );
	}

	void MousePicking::HandleInput( InputAction action, InputType type )
	{
		if( action == InputAction::SELECT_MESH && type == InputType::PRESSED )
		{
			m_select = true;
			m_selectedMesh = EntityHandle();
		}
	}

	void MousePicking::Update( EntityManager& entity_manager, float dt )
	{
		if( m_enabled )
		{
			//Ray mouse_ray = GetScreenRay( m_input.GetMousePosition() );

			m_focusedMesh = EntityHandle();

			float distance = FLT_MAX;

			MousePosition pos = m_input.GetMousePosition();

			int clampedX = (int) glm::clamp( pos.X, 0.0f, m_window.GetWidth() - 1.0f );
			int clampedY = (int) glm::clamp( pos.Y, 0.0f, m_window.GetHeight() - 1.0f );
			int index = clampedY * m_window.GetWidth() + clampedX;

			int* handle = ((int*) m_textureData.Get()) + index;

			m_position = glm::ivec2( clampedX, clampedY );
			m_handle = *handle;
			
			EntityHandle entity( *handle, entity_manager );
			if( entity.IsValid() && entity.Has<MeshComponent>() )
			{
				m_focusedMesh = entity;
			}
			
			//entity_manager.ForAllWithReadable<MeshComponent>( [this, &mouse_ray, &distance]( auto entity, auto mesh ) { HitTestMesh( entity, mesh, mouse_ray, distance ); } );

			if( m_focusedMesh.IsValid() )
			{
				if( m_select )
				{
					m_selectedMesh = m_focusedMesh;
				}
			}
		}
		else
		{
			m_focusedMesh = EntityHandle();
		}

		m_select = false;
	}

	void MousePicking::RenderInit( const EntityManager& entity_manager, const ICamera& camera )
	{
		Vector<Shader> shaders;
		shaders.Add( Shader::Create( String8( "shaders\\picking.vertex" ), Shader::Type::Vertex ) );
		shaders.Add( Shader::Create( String8( "shaders\\picking.pixel" ), Shader::Type::Pixel ) );

		m_shader = ShaderProgram::Create( String8( "picking" ), shaders );

		ShaderProgram& shader = *m_shader.Get();
		shader.Use( true );

		shader.SetPositionsName( "Position" );

		shader.Use( false );

		int width = m_window.GetWidth() / 2;
		int height = m_window.GetHeight() / 2;
		int buffer_size = width * height * 4;

		m_textureData.Set( new byte[ buffer_size ], buffer_size );
		memset( m_textureData.Get(), 0, m_textureData.SizeBytes() );

		m_texture.Create( width, height, 4 );

		m_rtt.Create( m_texture, true );
		m_rtt.PreRender();
	}

	void MousePicking::Render( const EntityManager& entity_manager, const ICamera& camera )
	{
		ShaderProgram& shader = *m_shader.Get();

		m_rtt.Bind();

		shader.Use( true );

		entity_manager.ForAllWithReadable<MeshComponent, TransformComponent>( [this, &camera, &shader]( auto entity, auto mesh, auto transform )
		{
			RenderMesh( camera, shader, entity, mesh.Read(), transform.Read() );
		} );

		shader.Use( false );

		m_rtt.Unbind();

		m_texture.GetData( m_textureData, 0 );
	}

	void MousePicking::RenderMesh( const ICamera& camera, ShaderProgram& shader, EntityHandle entity, const MeshComponent* mesh_cmp, const TransformComponent* transform_cmp )
	{
		shader.SetUniform( "ID", entity.Handle );

		Mesh* mesh = mesh_cmp->Mesh.Get();
		mesh->Render( camera, *m_shader.Get(), transform_cmp->GetWorldTransform() );
	}

	void MousePicking::DrawDebugInternal()
	{
		ImGui::SetWindowPos( ImVec2( 2.0f, ImGui::GetIO().DisplaySize.y - 100 ), ImGuiSetCond_FirstUseEver );

		ImGui::Checkbox( "Enabled", &m_enabled );

		ImGui::Text( "Handle: %d", m_handle );
		ImGui::Text( "Position: %d %d", m_position.x, m_position.y );

		if( m_focusedMesh.IsValid() )
		{
			ImGui::CollapsingHeader( "Focused" );

			const String& name = m_focusedMesh.Get<MeshComponent>().Read()->Mesh.Get()->GetName();
			ImGui::Text( "Name: %s", name.c_str() );

			glm::vec3 focused_mesh_pos = m_focusedMesh.Get<TransformComponent>().Read()->GetWorldPosition();
			ImGui::Text( "Position: %.2f %.2f %.2f", focused_mesh_pos.x, focused_mesh_pos.y, focused_mesh_pos.z );
		}
		else
		{
			ImGui::CollapsingHeader( "Focused" );

			ImGui::Text( "Name: <none>" );
			ImGui::Text( "Position: <none>" );
		}

		if( m_selectedMesh.IsValid() )
		{
			ImGui::CollapsingHeader( "Selected" );

			const String& name = m_selectedMesh.Get<MeshComponent>().Read()->Mesh.Get()->GetName();
			ImGui::Text( "Name: %s", name.c_str() );

			glm::vec3 selected_mesh_pos = m_selectedMesh.Get<TransformComponent>().Read()->GetWorldPosition();
			ImGui::Text( "Position: %.2f %.2f %.2f", selected_mesh_pos.x, selected_mesh_pos.y, selected_mesh_pos.z );
		}
		else
		{
			ImGui::CollapsingHeader( "Selected" );

			ImGui::Text( "Name: <none>" );
			ImGui::Text( "Position: <none>" );
		}
	}

	void MousePicking::HitTestMesh( EntityHandle entity, ComponentHandle<MeshComponent> mesh_handle, const Ray& mouse_ray, float& nearest_distance )
	{
		const MeshComponent* mesh_cmp = mesh_handle.Read();
		Mesh* mesh = mesh_cmp->Mesh.Get();
		if( mesh != nullptr && !mesh_cmp->Hidden )
		{
			const AABB& bounds = mesh_cmp->Bounds;

			float distance;
			if( bounds.IntersectsRay( mouse_ray.GetOrigin(), mouse_ray.GetDirection(), distance ) )
			{
				if( distance < nearest_distance )
				{
					nearest_distance = distance;
					m_focusedMesh = entity;
				}
			}
		}
	}

	Ray MousePicking::GetScreenRay( const MousePosition& pos ) const
	{
		glm::vec3 camera_dir( m_camera.GetDirection() );
		glm::vec3 dir( camera_dir );
		glm::vec3 up( 0, 1, 0 );

		{
			float width = (float) m_window.GetWidth();
			float x_percent = (pos.X - (width / 2)) / width;
			float hfov = m_camera.GetVerticalFOV() * m_camera.GetAspectRatio();
			float x_angle = hfov * x_percent;

			dir = glm::rotate( camera_dir, -x_angle, up );
		}

		{
			float height = (float) m_window.GetHeight();
			float y_percent = (pos.Y - (height / 2)) / height;
			float vfov = m_camera.GetVerticalFOV();
			float y_angle = vfov * y_percent;

			glm::vec3 right = glm::normalize( glm::cross( m_camera.GetDirection(), up ) );

			dir = glm::rotate( dir, -y_angle, right );
		}

		return Ray( m_camera.GetPosition(), dir );
	}
}
