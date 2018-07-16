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

#include "GL/gl3w.h"

namespace dd
{
	MousePicking::MousePicking( const Window& window, const Input& input ) : 
		m_window( window ),
		m_input( input )
	{
	}

	void MousePicking::BindActions( InputBindings& bindings )
	{
		bindings.RegisterHandler( InputAction::SELECT_MESH, [this]( InputAction action, InputType type ) { HandleInput( action, type ); } );
		bindings.RegisterHandler( InputAction::TOGGLE_PICKING, [this]( InputAction action, InputType type ) { HandleInput( action, type ); } );
	}

	void MousePicking::HandleInput( InputAction action, InputType type )
	{
		if( action == InputAction::SELECT_MESH && type == InputType::RELEASED )
		{
			m_select = true;
			m_selectedMesh = EntityHandle();
		}

		if( action == InputAction::TOGGLE_PICKING && type == InputType::RELEASED )
		{
			m_enabled = !m_enabled;
			IDebugPanel::SetDebugOpen( m_enabled );
		}
	}

	int MousePicking::GetEntityHandleAt( glm::vec2 mouse_pos ) const
	{
		glm::ivec2 flipped = glm::ivec2( (int) mouse_pos.x, m_window.GetHeight() - (int) mouse_pos.y );

		glm::ivec2 clamped = glm::clamp( flipped, glm::ivec2( 0, 0 ), glm::ivec2( m_window.GetWidth() - 1, m_window.GetHeight() - 1 ) );
		int index = (clamped.y / DownScalingFactor) * (m_window.GetWidth() / DownScalingFactor) + (clamped.x / DownScalingFactor);

		DD_ASSERT( (index * DownScalingFactor * DownScalingFactor) < m_lastIDBuffer.SizeBytes(), "Index out of range!" );

		const int* base = reinterpret_cast<const int*>( m_lastIDBuffer.GetVoid() );
		return base[index];
	}

	float MousePicking::GetDepthAt( glm::vec2 mouse_pos ) const
	{
		glm::ivec2 flipped = glm::ivec2( (int) mouse_pos.x, m_window.GetHeight() - (int) mouse_pos.y );

		glm::ivec2 clamped = glm::clamp( flipped, glm::ivec2( 0, 0 ), glm::ivec2( m_window.GetWidth() - 1, m_window.GetHeight() - 1 ) );
		int index = (clamped.y / DownScalingFactor) * (m_window.GetWidth() / DownScalingFactor) + (clamped.x / DownScalingFactor);

		DD_ASSERT( (index * DownScalingFactor * DownScalingFactor) < m_lastDepthBuffer.SizeBytes(), "Index out of range!" );

		const float* base = reinterpret_cast<const float*>( m_lastDepthBuffer.GetVoid() );
		return base[index];
	}

	void MousePicking::Update( EntityManager& entity_manager, float dt )
	{
		if( m_enabled )
		{
			m_focusedMesh = EntityHandle();

			m_position = m_input.GetMousePosition().Absolute;
			m_handle = GetEntityHandleAt( m_input.GetMousePosition().Absolute );
			m_depth = GetDepthAt( m_input.GetMousePosition().Absolute );

			EntityHandle entity = EntityHandle( m_handle, entity_manager );
			if( entity.IsValid() )
			{
				m_focusedMesh = entity;
			}
			else
			{
				m_handle = -1;
			}
			
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

	void MousePicking::RenderInit()
	{
		Vector<ddr::Shader*> shaders;
		shaders.Add( ddr::Shader::Create( String8( "shaders\\picking.vertex" ), ddr::Shader::Type::Vertex ) );
		shaders.Add( ddr::Shader::Create( String8( "shaders\\picking.pixel" ), ddr::Shader::Type::Pixel ) );

		m_shader = ddr::ShaderProgram::Create( String8( "picking" ), shaders );

		CreateFrameBuffer( m_window.GetSize() );
		m_previousSize = m_window.GetSize();
	}

	void MousePicking::CreateFrameBuffer( glm::ivec2 window_size )
	{
		glm::ivec2 size = glm::ivec2( m_window.GetWidth() / DownScalingFactor, m_window.GetHeight() / DownScalingFactor );
		int buffer_size = size.x * size.y * 4;

		m_lastIDBuffer.Set( new byte[buffer_size], buffer_size );
		m_idTexture.Create( size, GL_R32UI, 1 );

		m_lastDepthBuffer.Set( new byte[buffer_size], buffer_size );
		m_depthTexture.Create( size, GL_DEPTH_COMPONENT32F, 1 );

		m_framebuffer.SetClearDepth( 0.0f );
		m_framebuffer.SetClearColour( glm::vec4( 1 ) );
		m_framebuffer.Create( m_idTexture, &m_depthTexture );
		m_framebuffer.RenderInit();
	}

	void MousePicking::Render( const EntityManager& entity_manager, const ICamera& camera, ddr::UniformStorage& uniforms )
	{
		if( m_enabled )
		{
			if( m_previousSize != m_window.GetSize() )
			{
				m_framebuffer.Destroy();

				m_lastIDBuffer.Delete();
				m_idTexture.Destroy();

				m_depthTexture.Destroy();
				m_lastDepthBuffer.Delete();

				CreateFrameBuffer( m_window.GetSize() );
			}

			m_framebuffer.BindDraw();
			m_framebuffer.BindRead();

			m_framebuffer.Clear();

			ddr::ShaderProgram* shader = ddr::ShaderProgram::Get( m_shader );
			shader->Use( true );

			entity_manager.ForAllWithReadable<MeshComponent, TransformComponent>( [this, &camera, shader]( auto entity, auto mesh, auto transform )
			{
				RenderMesh( camera, *shader, entity, mesh.Read(), transform.Read() );
			} );

			shader->Use( false );

			m_framebuffer.UnbindRead();
			m_framebuffer.UnbindDraw();

			m_idTexture.GetData( m_lastIDBuffer, 0, GL_RED_INTEGER, GL_INT );
			m_depthTexture.GetData( m_lastDepthBuffer, 0, GL_DEPTH_COMPONENT, GL_FLOAT );
		}
	}

	void MousePicking::RenderDebug()
	{
		m_framebuffer.BindRead();

		m_framebuffer.Render();

		m_framebuffer.UnbindRead();
	}

	void MousePicking::RenderMesh( const dd::ICamera& camera, ddr::ShaderProgram& shader, EntityHandle entity, const MeshComponent* mesh_cmp, const TransformComponent* transform_cmp )
	{
		shader.SetUniform( "ID", entity.Handle );

		ddr::Mesh* mesh = ddr::Mesh::Get( mesh_cmp->Mesh );
		mesh->Render( shader, transform_cmp->GetWorldTransform() );
	}

	void MousePicking::DrawDebugInternal()
	{
		ImGui::SetWindowPos( ImVec2( 2.0f, ImGui::GetIO().DisplaySize.y - 100 ), ImGuiSetCond_FirstUseEver );

		ImGui::Checkbox( "Enabled", &m_enabled );
		ImGui::Checkbox( "Render Debug", &m_renderDebug );

		ImGui::Value( "Handle", m_handle );
		ImGui::Value( "Position", m_position, "%.1f" );
		ImGui::Value( "Depth", 1.0f / m_depth, "%.3f" );

		if( ImGui::TreeNodeEx( "Focused", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen ) )
		{
			if( m_focusedMesh.IsValid() )
			{
				ddr::MeshHandle mesh_h = m_focusedMesh.Get<MeshComponent>().Read()->Mesh;
				
				const String& name = ddr::Mesh::Get( mesh_h )->GetName();
				ImGui::Text( "Name: %s", name.c_str() );

				glm::vec3 focused_mesh_pos = m_focusedMesh.Get<TransformComponent>().Read()->GetWorldPosition();
				ImGui::Value( "Position", focused_mesh_pos, "%.2f" );
			}
			else
			{
				ImGui::Text( "Name: <none>" );
				ImGui::Text( "Position: <none>" );
			}

			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Selected", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen ) )
		{
			if( m_selectedMesh.IsValid() )
			{
				ddr::MeshHandle mesh_h = m_selectedMesh.Get<MeshComponent>().Read()->Mesh;

				const String& name = ddr::Mesh::Get( mesh_h )->GetName();
				ImGui::Text( "Name: %s", name.c_str() );

				glm::vec3 selected_mesh_pos = m_selectedMesh.Get<TransformComponent>().Read()->GetWorldPosition();
				ImGui::Value( "Position", selected_mesh_pos, "%.2f" );
			}
			else
			{
				ImGui::Text( "Name: <none>" );
				ImGui::Text( "Position: <none>" );
			}

			ImGui::TreePop();
		}
	}

	void MousePicking::HitTestMesh( EntityHandle entity, const MeshComponent* mesh_cmp, const Ray& mouse_ray, float& nearest_distance )
	{
		ddr::Mesh* mesh = ddr::Mesh::Get( mesh_cmp->Mesh );
		if( mesh != nullptr && !mesh_cmp->Hidden )
		{
			const AABB& bounds = mesh_cmp->Bounds;

			float distance;
			if( bounds.IntersectsRay( mouse_ray, distance ) )
			{
				if( distance < nearest_distance )
				{
					nearest_distance = distance;
					m_focusedMesh = entity;
				}
			}
		}
	}

	Ray MousePicking::GetScreenRay( const ICamera& camera, const MousePosition& pos ) const
	{
		glm::vec3 camera_dir( camera.GetDirection() );
		glm::vec3 dir( camera_dir );
		glm::vec3 up( 0, 1, 0 );

		{
			float width = (float) m_window.GetWidth();
			float x_percent = (pos.Absolute.x - (width / 2)) / width;
			float hfov = camera.GetVerticalFOV() * camera.GetAspectRatio();
			float x_angle = hfov * x_percent;

			dir = glm::rotate( camera_dir, -x_angle, up );
		}

		{
			float height = (float) m_window.GetHeight();
			float y_percent = (pos.Absolute.x - (height / 2)) / height;
			float vfov = camera.GetVerticalFOV();
			float y_angle = vfov * y_percent;

			glm::vec3 right = glm::normalize( glm::cross( camera.GetDirection(), up ) );

			dir = glm::rotate( dir, -y_angle, right );
		}

		return Ray( camera.GetPosition(), dir );
	}
}
