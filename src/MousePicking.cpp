//
// MousePicking.cpp - Class that handles mouse picking.
// Copyright (C) Sebastian Nordgren 
// April 26th 2017
//

#include "PrecompiledHeader.h"
#include "MousePicking.h"

#include "BoundsComponent.h"
#include "ICamera.h"
#include "Input.h"
#include "InputBindings.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "OpenGL.h"
#include "ParticleSystemComponent.h"
#include "RayComponent.h"
#include "RenderData.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "TransformComponent.h"
#include "Uniforms.h"
#include "UpdateData.h"
#include "Window.h"
#include "World.h"

#include "HitTest.h"

#include "glm/gtx/rotate_vector.hpp"

#include "imgui/imgui.h"

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
		Require<dd::BoundsComponent>();
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
		}

		if( action == InputAction::TOGGLE_PICKING && type == InputType::RELEASED )
		{
			m_enabled = !m_enabled;
			IDebugPanel::SetDebugOpen( m_enabled );
		}
	}

	int MousePicking::GetEntityIDAt( glm::vec2 mouse_pos ) const
	{
		glm::ivec2 flipped = glm::ivec2( (int) mouse_pos.x, m_window.GetHeight() - (int) mouse_pos.y );

		glm::ivec2 clamped = glm::clamp( flipped, glm::ivec2( 0, 0 ), glm::ivec2( m_window.GetWidth() - 1, m_window.GetHeight() - 1 ) );
		int index = (clamped.y / DownScalingFactor) * (m_window.GetWidth() / DownScalingFactor) + (clamped.x / DownScalingFactor);

		DD_ASSERT( (index * DownScalingFactor * DownScalingFactor) < m_lastIDBuffer.SizeBytes(), "Index out of range!" );

		const int* base = reinterpret_cast<const int*>(m_lastIDBuffer.GetVoid());
		return base[index];
	}

	float MousePicking::GetDepthAt( glm::vec2 mouse_pos ) const
	{
		glm::ivec2 flipped = glm::ivec2( (int) mouse_pos.x, m_window.GetHeight() - (int) mouse_pos.y );

		glm::ivec2 clamped = glm::clamp( flipped, glm::ivec2( 0, 0 ), glm::ivec2( m_window.GetWidth() - 1, m_window.GetHeight() - 1 ) );
		int index = (clamped.y / DownScalingFactor) * (m_window.GetWidth() / DownScalingFactor) + (clamped.x / DownScalingFactor);

		DD_ASSERT( (index * DownScalingFactor * DownScalingFactor) < m_lastDepthBuffer.SizeBytes(), "Index out of range!" );

		const float* base = reinterpret_cast<const float*>(m_lastDepthBuffer.GetVoid());
		return base[index];
	}

	void MousePicking::RenderInit( ddc::World& world )
	{
		m_shader = ddr::ShaderProgram::Load( "picking" );

		CreateFrameBuffer( m_window.GetSize() );
		m_previousSize = m_window.GetSize();

		m_previousRay = world.CreateEntity();
		world.Add<dd::RayComponent>( m_previousRay );
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
		if( !m_enabled )
			return;

		m_position = m_input.GetMousePosition().Absolute;

		// do hit
		ddc::Entity hit;
		if( m_rayTest )
		{
			hit = HitTestRay( data );
		}
		else
		{
			hit = HitTestRender( data );
		}

		ddc::World& world = data.World();

		// set focused
		if( m_focused.IsValid() )
		{
			world.RemoveTag( m_focused, ddc::Tag::Focused );
		}

		m_focused = hit;

		if( m_focused.IsValid() )
		{
			world.AddTag( m_focused, ddc::Tag::Focused );
		}

		// select
		if( m_select )
		{
			if( m_selected.IsValid() )
			{
				world.RemoveTag( m_selected, ddc::Tag::Selected );
			}

			m_selected = hit;
			
			if( m_selected.IsValid() )
			{
				world.AddTag( m_focused, ddc::Tag::Selected );
			}
		}

		m_select = false;
	}

	ddc::Entity MousePicking::HitTestRender( const ddr::RenderData& data )
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
		ddr::RenderBuffer<dd::BoundsComponent> bounds = data.Get<dd::BoundsComponent>();
		dd::Span<ddc::Entity> entities = data.Entities();

		for( size_t i = 0; i < data.Size(); ++i )
		{
			ddr::Mesh* mesh = ddr::Mesh::Get( meshes[i].Mesh );
			if( mesh == nullptr )
				continue;

			uniforms.Set( "ID", (int) entities[i].ID );
			mesh->Render( uniforms, shader, transforms[i].World );
		}

		shader.Use( false );

		m_framebuffer.UnbindRead();
		m_framebuffer.UnbindDraw();

		m_idTexture.GetData( m_lastIDBuffer, 0, GL_RED_INTEGER, GL_INT );
		m_depthTexture.GetData( m_lastDepthBuffer, 0, GL_DEPTH_COMPONENT, GL_FLOAT );

		int id = GetEntityIDAt( m_position );
		m_depth = (1.0f / GetDepthAt( m_position )) / 100.f;

		ddc::Entity entity = data.World().GetEntity( id );
		return entity;
	}

	ddc::Entity MousePicking::HitTestRay( const ddr::RenderData& data )
	{
		ddc::World& world = data.World();

		if( m_visualizeRay )
		{
			world.AddTag( m_previousRay, ddc::Tag::Visible );
		}
		else
		{
			world.RemoveTag( m_previousRay, ddc::Tag::Visible );
		}

		dd::Ray screen_ray = GetScreenRay( data.Camera() );

		ddc::Entity entity;
		m_depth = FLT_MAX;

		ddr::RenderBuffer<dd::MeshComponent> meshes = data.Get<dd::MeshComponent>();
		ddr::RenderBuffer<dd::TransformComponent> transforms = data.Get<dd::TransformComponent>();
		ddr::RenderBuffer<dd::BoundsComponent> bounds = data.Get<dd::BoundsComponent>();
		dd::Span<ddc::Entity> entities = data.Entities();

		for( size_t i = 0; i < data.Size(); ++i )
		{
			ddr::Mesh* mesh = ddr::Mesh::Get( meshes[i].Mesh );
			if( mesh == nullptr )
				continue;

			float distance;
			if( dd::HitTestMesh( screen_ray, transforms[i].World, bounds[i].WorldSphere, bounds[i].WorldBox, *mesh, distance ) )
			{
				if( distance < m_depth )
				{
					entity = entities[i];
					m_depth = distance;
				}
			}
		}

		if( m_select )
		{
			dd::RayComponent* ray = world.Access<dd::RayComponent>( m_previousRay );
			ray->Ray = screen_ray;
			ray->Length = m_depth;
		}

		return entity;
	}

	void MousePicking::RenderDebug( const ddr::RenderData& data )
	{
		m_framebuffer.BindRead();

		m_framebuffer.Render( data.Uniforms() );

		m_framebuffer.UnbindRead();
	}

	void MousePicking::DrawDebugInternal( const ddc::World& world )
	{
		ImGui::SetWindowPos( ImVec2( 2.0f, ImGui::GetIO().DisplaySize.y - 100 ), ImGuiSetCond_FirstUseEver );

		ImGui::Checkbox( "Enabled", &m_enabled );
		ImGui::Checkbox( "Use Ray", &m_rayTest );

		ImGui::Checkbox( "Render Debug", &m_renderDebug );

		ImGui::Value( "Handle", m_focused.ID );
		ImGui::Value( "Position", m_position, "%.1f" );
		ImGui::Value( "Depth", m_depth, "%.3f" );

		if( ImGui::TreeNodeEx( "Ray", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen ) )
		{
			ImGui::Checkbox( "Visualize Ray", &m_visualizeRay );

			const dd::RayComponent* ray_cmp = world.Get<dd::RayComponent>( m_previousRay );

			ImGui::Value( "Ray Origin", ray_cmp->Ray.Origin() );
			ImGui::Value( "Ray Dir", ray_cmp->Ray.Direction() );

			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Focused", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen ) )
		{
			if( m_focused.IsValid() )
			{
				ddr::MeshHandle mesh_h = world.Get<MeshComponent>( m_focused )->Mesh;

				const String& name = ddr::Mesh::Get( mesh_h )->GetName();
				ImGui::Text( "Name: %s", name.c_str() );

				glm::vec3 mesh_pos = world.Get<TransformComponent>( m_focused )->GetLocalPosition();
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
			if( m_selected.IsValid() )
			{
				ddr::MeshHandle mesh_h = world.Get<MeshComponent>( m_selected )->Mesh;

				const String& name = ddr::Mesh::Get( mesh_h )->GetName();
				ImGui::Value( "Name", name.c_str() );

				glm::vec3 mesh_pos = world.Get<TransformComponent>( m_selected )->GetLocalPosition();
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

	//
	// Unproject a point at the mouse coordinates at the near plane and at the far plane to get a world-space ray.
	//
	Ray MousePicking::GetScreenRay( const ddr::ICamera& camera ) const
	{
		glm::vec4 viewport( 0, 0, m_window.GetWidth(), m_window.GetHeight() );

		glm::vec3 win_near( m_position.x, m_window.GetHeight() - m_position.y, 0 );
		glm::vec3 win_far( m_position.x, m_window.GetHeight() - m_position.y, 1 );

		glm::vec3 world_near = glm::unProject( win_near, camera.GetViewMatrix(), camera.GetProjectionMatrix(), viewport );
		glm::vec3 world_far = glm::unProject( win_far, camera.GetViewMatrix(), camera.GetProjectionMatrix(), viewport );

		return Ray( world_near, glm::normalize( world_far - world_near ) );
	}
}
