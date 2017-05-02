//
// MousePicking.cpp - Class that handles mouse picking.
// Copyright (C) Sebastian Nordgren 
// April 26th 2017
//

#include "PrecompiledHeader.h"
#include "MousePicking.h"

#include "Camera.h"
#include "EntityManager.h"
#include "Input.h"
#include "InputBindings.h"
#include "MeshComponent.h"
#include "TransformComponent.h"
#include "Window.h"

#include "glm/gtx/rotate_vector.hpp"

#include "imgui/imgui.h"

namespace dd
{
	MousePicking::MousePicking( Window& window, Camera& camera, Input& input )
	{
		m_camera = &camera;
		m_input = &input;
		m_window = &window;
		m_enabled = true;
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
			Ray mouse_ray = GetScreenRay( m_input->GetMousePosition() );

			m_focusedMesh = EntityHandle();

			float distance = FLT_MAX;
			entity_manager.ForAllWithReadable<MeshComponent>( [this, &mouse_ray, &distance]( auto entity, auto mesh ) { HitTestMesh( entity, mesh, mouse_ray, distance ); } );

			if( m_focusedMesh.IsValid() )
			{
				if( m_select )
				{
					m_selectedMesh = m_focusedMesh;
				}
			}
		}

		m_select = false;
	}

	void MousePicking::DrawDebugInternal()
	{
		ImGui::SetWindowPos( ImVec2( 2.0f, ImGui::GetIO().DisplaySize.y - 100 ), ImGuiSetCond_FirstUseEver );

		ImGui::Checkbox( "Enabled", &m_enabled );

		if( m_focusedMesh.IsValid() )
		{
			glm::vec3 focused_mesh_pos = m_focusedMesh.Get<TransformComponent>().Read()->GetWorldPosition();
			ImGui::Text( "Focused Mesh: %.2f %.2f %.2f", focused_mesh_pos.x, focused_mesh_pos.y, focused_mesh_pos.z );
		}
		else
		{
			ImGui::Text( "Focused Mesh: <none>" );
		}

		if( m_selectedMesh.IsValid() )
		{
			glm::vec3 selected_mesh_pos = m_selectedMesh.Get<TransformComponent>().Read()->GetWorldPosition();
			ImGui::Text( "Selected Mesh: %.2f %.2f %.2f", selected_mesh_pos.x, selected_mesh_pos.y, selected_mesh_pos.z );
		}
		else
		{
			ImGui::Text( "Selected Mesh: <none>" );
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
		glm::vec3 camera_dir( m_camera->GetDirection() );
		glm::vec3 dir( camera_dir );
		glm::vec3 up( 0, 1, 0 );

		{
			float width = (float) m_window->GetWidth();
			float x_percent = (pos.X - (width / 2)) / width;
			float hfov = m_camera->GetHorizontalFOV();
			float x_angle = hfov * x_percent;

			dir = glm::rotate( camera_dir, -x_angle, up );
		}

		{
			float height = (float) m_window->GetHeight();
			float y_percent = (pos.Y - (height / 2)) / height;
			float vfov = m_camera->GetVerticalFOV();
			float y_angle = vfov * y_percent;

			glm::vec3 right = glm::normalize( glm::cross( m_camera->GetDirection(), up ) );

			dir = glm::rotate( dir, -y_angle, right );
		}

		return Ray( m_camera->GetPosition(), dir );
	}
}
