//
// Renderer.cpp - Master renderer class, coordinates all rendering.
// Copyright (C) Sebastian Nordgren 
// April 14th 2016
//

#include "PrecompiledHeader.h"
#include "WorldRenderer.h"

#include "AABB.h"
#include "GLError.h"
#include "Frustum.h"
#include "ICamera.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "MousePicking.h"
#include "LightComponent.h"
#include "ParticleSystem.h"
#include "OpenGL.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "TransformComponent.h"
#include "Uniforms.h"
#include "Window.h"
#include "World.h"

#include "imgui/imgui.h"

namespace ddr
{

	struct Fog
	{
		bool Enabled { true };
		float Distance { 1000.0f };
		glm::vec3 Colour { 0.6, 0.7, 0.8 };

		void UpdateUniforms( ddr::UniformStorage& uniforms ) const
		{
			uniforms.Set( "Fog.Enabled", Enabled );
			uniforms.Set( "Fog.Distance", Distance );
			uniforms.Set( "Fog.Colour", Colour );
		}
	};

	Fog s_fog;

	struct Wireframe
	{
		bool Enabled { false };

		glm::vec3 Colour { 0, 1.0f, 0 };
		float Width { 2.0f };

		glm::vec3 EdgeColour { 0, 0, 0 };
		float EdgeWidth { 0.5f };

		float MaxDistance { 250.0f };

		void UpdateUniforms( ddr::UniformStorage& uniforms ) const
		{
			uniforms.Set( "Wireframe.Enabled", Enabled );
			uniforms.Set( "Wireframe.Colour", Colour );
			uniforms.Set( "Wireframe.Width", Width );
			uniforms.Set( "Wireframe.EdgeColour", EdgeColour );
			uniforms.Set( "Wireframe.EdgeWidth", EdgeWidth );
			uniforms.Set( "Wireframe.MaxDistance", MaxDistance );
		}
	};

	Wireframe s_wireframe;

	WorldRenderer::WorldRenderer( const dd::Window& window ) :
		m_window( window )
	{
		m_uniforms = new ddr::UniformStorage();
	}

	WorldRenderer::~WorldRenderer()
	{
		delete m_uniforms;
	}

	void WorldRenderer::InitializeRenderers( ddc::World& world )
	{
		CreateFrameBuffer( m_window.GetSize() );
		m_previousSize = m_window.GetSize();

		for( ddr::Renderer* current : m_renderers )
		{
			current->RenderInit( world );
		}
	}

	void WorldRenderer::Register( ddr::Renderer& renderer )
	{
		m_renderers.push_back( &renderer );
	}

	void WorldRenderer::CreateFrameBuffer( glm::ivec2 size )
	{
		m_colourTexture.Create( size, GL_SRGB8_ALPHA8, 1 );
		m_depthTexture.Create( size, GL_DEPTH_COMPONENT32F, 1 );

		m_framebuffer.SetClearColour( glm::vec4( m_skyColour.xyz, 0.0 ) );
		m_framebuffer.SetClearDepth( 0.0f );
		m_framebuffer.Create( m_colourTexture, &m_depthTexture );
		m_framebuffer.RenderInit();
	}

	void WorldRenderer::DrawDebugInternal( const ddc::World& world )
	{
		ImGui::Checkbox( "Draw Depth", &m_debugDrawDepth );
		ImGui::Checkbox( "Draw Standard", &m_debugDrawStandard );

		if( ImGui::TreeNodeEx( "Wireframe", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			ImGui::Checkbox( "Enabled", &s_wireframe.Enabled );

			ImGui::DragFloat( "Width", &s_wireframe.Width, 0.01f, 0.0f, 10.0f );

			ImGui::ColorEdit3( "Colour", glm::value_ptr( s_wireframe.Colour ) );

			ImGui::DragFloat( "Edge Width", &s_wireframe.EdgeWidth, 0.01f, 0.0f, s_wireframe.Width );

			ImGui::ColorEdit3( "Edge Colour", glm::value_ptr( s_wireframe.EdgeColour ) );

			ImGui::DragFloat( "Max Distance", &s_wireframe.MaxDistance, 1.0f, 0.0f, 1000.0f );

			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Fog", ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			ImGui::Checkbox( "Enabled", &s_fog.Enabled );
			ImGui::SliderFloat( "Distance", &s_fog.Distance, 0, 10000, "%.1f" );
			ImGui::ColorEdit3( "Colour", glm::value_ptr( s_fog.Colour ) );

			ImGui::TreePop();
		}
		
		if( ImGui::Button( "Reload Shaders" ) )
		{
			m_reloadShaders = true;
		}

		if( !m_debugMeshGridCreated && ImGui::Button( "Create Mesh Grid" ) )
		{
			m_createDebugMeshGrid = true;
		}
	}

	void WorldRenderer::SetRenderState()
	{
		if( m_debugDrawStandard )
		{
			// depth test
			glEnable( GL_DEPTH_TEST );
			glDepthFunc( GL_GREATER );
			glClipControl( GL_LOWER_LEFT, GL_ZERO_TO_ONE );

			// backface culling
			glEnable( GL_CULL_FACE );
			glFrontFace( GL_CCW );
			glCullFace( GL_BACK );

			// blending
			glDisable( GL_BLEND );
		}
		else
		{
			glDisable( GL_CULL_FACE );
			glDisable( GL_DEPTH_TEST );
			glEnable( GL_BLEND );
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		}
	}

	/*void WorldRenderer::CreateDebugMeshGrid( ddc::World& world )
	{
		if( m_debugMeshGridCreated || !m_createDebugMeshGrid )
			return;

		for( int x = -5; x < 5; ++x )
		{
			for( int y = -5; y < 5; ++y )
			{
				for( int z = -5; z < 5; ++z )
				{
					CreateMeshEntity( world, m_unitCube, glm::vec4( 0.5, 0.5, 0.5, 1 ), glm::translate( glm::vec3( 10.f * x, 10.f * y, 10.f * z ) ) );
				}
			}
		}

		m_debugMeshGridCreated = true;
	} */

	void WorldRenderer::RenderDebug( const ddr::RenderData& data, ddr::Renderer& debug_render )
	{
		m_framebuffer.BindDraw();

		debug_render.RenderDebug( data );

		m_framebuffer.UnbindDraw();
	}

	void WorldRenderer::CallRenderer( ddr::Renderer& renderer, ddc::World& world, const ddr::ICamera& camera, std::function<void(Renderer&, const RenderData&)> fn )
	{
		dd::Array<ddc::TypeID, ddc::MAX_COMPONENTS> components;
		for( const ddc::DataRequest* req : renderer.GetRequirements() )
		{
			components.Add( req->Component().ID );
		}

		const std::bitset<ddc::MAX_TAGS>& tags = renderer.GetRequiredTags();

		std::vector<ddc::Entity> entities;
		world.FindAllWith( components, tags, entities );

		if( entities.size() == 0 )
		{
			return;
		}

		dd::Span<ddc::Entity> entity_span( entities );

		RenderData data( world, camera, *m_uniforms, entity_span, renderer.GetRequirements() );

		fn( renderer, data );
	}

	void WorldRenderer::Render( ddc::World& world, const ddr::ICamera& camera )
	{
		ddr::Renderer* debug_render = nullptr;

		BeginRender( world, camera );

		for( ddr::Renderer* r : m_renderers )
		{
			if( !r->UsesAlpha() )
			{
				CallRenderer( *r, world, camera,
					[]( Renderer& rend, const RenderData& data ) { rend.Render( data ); } );
			}

			if( r->ShouldRenderDebug() )
			{
				debug_render = r;
				break;
			}
		}

		if( debug_render != nullptr )
		{
			CallRenderer( *debug_render, world, camera, 
				[]( Renderer& rend, const RenderData& data ) { rend.RenderDebug( data ); } );
		}

		for( ddr::Renderer* r : m_renderers )
		{
			if( r->UsesAlpha() )
			{
				CallRenderer( *r, world, camera,
					[]( Renderer& rend, const RenderData& data ) { rend.Render( data ); } );
			}
		}

		EndRender( *m_uniforms, camera );
	}

	void WorldRenderer::BeginRender( const ddc::World& world, const ddr::ICamera& camera )
	{
		DD_ASSERT( m_window.IsContextValid() );

		if( m_reloadShaders )
		{
			ShaderProgram::ReloadAll();
			m_reloadShaders = false;
		}

		if( m_window.GetSize() != m_previousSize )
		{
			m_framebuffer.Destroy();
			m_colourTexture.Destroy();
			m_depthTexture.Destroy();

			CreateFrameBuffer( m_window.GetSize() );

			m_previousSize = m_window.GetSize();
		}

		m_framebuffer.BindRead();
		m_framebuffer.BindDraw();
		m_framebuffer.Clear();

		SetRenderState();

		s_wireframe.UpdateUniforms( *m_uniforms );
		s_fog.UpdateUniforms( *m_uniforms );

		m_uniforms->Set( "View", camera.GetViewMatrix() );
		m_uniforms->Set( "Projection", camera.GetProjectionMatrix() );
		m_uniforms->Set( "DrawStandard", m_debugDrawStandard );
	}

	void WorldRenderer::EndRender( ddr::UniformStorage& uniforms, const ddr::ICamera& camera )
	{
		m_framebuffer.BindRead();

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
		glViewport( 0, 0, m_window.GetWidth(), m_window.GetHeight() );
		CheckOGLError();

		if( m_debugDrawDepth )
		{
			m_framebuffer.RenderDepth( uniforms, camera );
		}
		else
		{
			m_framebuffer.Blit();
		}

		m_framebuffer.UnbindRead();
	}
}