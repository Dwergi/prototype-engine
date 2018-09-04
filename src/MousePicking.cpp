//
// MousePicking.cpp - Class that handles mouse picking.
// Copyright (C) Sebastian Nordgren 
// April 26th 2017
//

#include "PrecompiledHeader.h"
#include "MousePicking.h"

#include "ICamera.h"
#include "Input.h"
#include "InputBindings.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "ParticleSystemComponent.h"
#include "RenderData.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "TransformComponent.h"
#include "Uniforms.h"
#include "UpdateData.h"
#include "Window.h"
#include "World.h"

#include "glm/gtx/rotate_vector.hpp"

#include "imgui/imgui.h"

#include "GL/gl3w.h"

namespace dd
{
	MousePicking::MousePicking( const Window& window, const Input& input ) : 
		ddr::Renderer( "Mouse Picking" ),
		m_window( window ),
		m_input( input )
	{
		ddr::Renderer::RequireTag( ddc::Tag::Visible );
		Require<dd::MeshComponent>();
		Require<dd::TransformComponent>();
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
			m_selectedMesh = ddc::Entity();
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

	void MousePicking::RenderInit()
	{
		m_shader = ddr::ShaderProgram::Load( "picking" );

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

	void MousePicking::Render( const ddr::RenderData& data )
	{
		m_focusedMesh = ddc::Entity();

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

			ddr::ShaderProgram& shader = *ddr::ShaderProgram::Get( m_shader );
			shader.Use( true );

			ddr::UniformStorage& uniforms = data.Uniforms();
			uniforms.Bind( shader );

			ddr::RenderBuffer<dd::MeshComponent> meshes = data.Get<dd::MeshComponent>();
			ddr::RenderBuffer<dd::TransformComponent> transforms = data.Get<dd::TransformComponent>();
			dd::Span<ddc::Entity> entities = data.Entities();

			for( size_t i = 0; i < data.Size(); ++i )
			{
				RenderMesh( uniforms, shader, entities[ i ], meshes[ i ], transforms[ i ] );
			}

			shader.Use( false );

			m_framebuffer.UnbindRead();
			m_framebuffer.UnbindDraw();

			m_idTexture.GetData( m_lastIDBuffer, 0, GL_RED_INTEGER, GL_INT );
			m_depthTexture.GetData( m_lastDepthBuffer, 0, GL_DEPTH_COMPONENT, GL_FLOAT );

			m_position = m_input.GetMousePosition().Absolute;
			m_handle = GetEntityHandleAt( m_input.GetMousePosition().Absolute );
			m_depth = GetDepthAt( m_input.GetMousePosition().Absolute );

			ddc::Entity entity = data.World().GetEntity( m_handle );

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

			m_select = false;
		}
	}

	void MousePicking::RenderDebug( const ddr::RenderData& data )
	{
		m_framebuffer.BindRead();

		m_framebuffer.Render( data.Uniforms() );

		m_framebuffer.UnbindRead();
	}

	void MousePicking::RenderMesh( ddr::UniformStorage& uniforms, ddr::ShaderProgram& shader, ddc::Entity entity, const MeshComponent& mesh_cmp, const TransformComponent& transform_cmp )
	{
		uniforms.Set( "ID", (int) entity.ID );

		ddr::Mesh* mesh = ddr::Mesh::Get( mesh_cmp.Mesh );
		mesh->Render( uniforms, shader, transform_cmp.World );
	}

	void MousePicking::DrawDebugInternal( const ddc::World& world )
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
				ddr::MeshHandle mesh_h = world.Get<MeshComponent>( m_focusedMesh )->Mesh;
				
				const String& name = ddr::Mesh::Get( mesh_h )->GetName();
				ImGui::Text( "Name: %s", name.c_str() );

				glm::vec3 mesh_pos = world.Get<TransformComponent>( m_focusedMesh )->GetLocalPosition();
				ImGui::Value( "Position", mesh_pos, "%.2f" );
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
				ddr::MeshHandle mesh_h = world.Get<MeshComponent>( m_selectedMesh )->Mesh;

				const String& name = ddr::Mesh::Get( mesh_h )->GetName();
				ImGui::Text( "Name: %s", name.c_str() );

				glm::vec3 mesh_pos = world.Get<TransformComponent>( m_selectedMesh )->GetLocalPosition();
				ImGui::Value( "Position", mesh_pos, "%.2f" );
			}
			else
			{
				ImGui::Text( "Name: <none>" );
				ImGui::Text( "Position: <none>" );
			}

			ImGui::TreePop();
		}
	}

	void MousePicking::HitTestBounds( ddc::Entity entity, const AABB& bounds, const Ray& mouse_ray, float& nearest_distance )
	{
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

	Ray MousePicking::GetScreenRay( const ddr::ICamera& camera, const MousePosition& pos ) const
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
