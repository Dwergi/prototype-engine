//
// PlaneRenderer.cpp - Renderer for RayComponent.
// Copyright (C) Sebastian Nordgren 
// March 10th 2019
//

#include "PCH.h"
#include "PlaneRenderer.h"

#include "ColourComponent.h"
#include "Mesh.h"
#include "MeshRenderCommand.h"
#include "PlaneComponent.h"
#include "Shader.h"
#include "TransformComponent.h"

namespace ddr
{
	static const glm::vec3 s_lines[] =
	{
		// direction
		glm::vec3( 0, 0, 0 ),
		glm::vec3( 0, 0, 1 ),

		// bounds
		glm::vec3( -1, -1, 0 ),
		glm::vec3( 1, -1, 0 ),
		glm::vec3( -1, 1, 0 ),
		glm::vec3( 1, 1, 0 ),
		glm::vec3( -1, -1, 0 ),
		glm::vec3( -1, 1, 0 ),
		glm::vec3( 1, -1, 0 ),
		glm::vec3( 1, 1, 0 ),

		// arrowhead
		glm::vec3( 0, 0, 1 ),
		glm::vec3( 0, 0.05, 0.95 ),
		glm::vec3( 0, 0, 1 ),
		glm::vec3( 0, -0.05, 0.95 ),
		glm::vec3( 0, 0, 1 ),
		glm::vec3( 0.05, 0, 0.95 ),
		glm::vec3( 0, 0, 1 ),
		glm::vec3( -0.05, 0, 0.95 )
	};

	static const dd::ConstBuffer<glm::vec3> s_linesBuffer( s_lines, dd::ArrayLength( s_lines ) );

	PlaneRenderer::PlaneRenderer() :
		Renderer( "Plane" )
	{
		RequireTag( ddc::Tag::Visible );
		Require<dd::PlaneComponent>();
		Optional<dd::TransformComponent>();
		Optional<dd::ColourComponent>(); 

		m_lineState.BackfaceCulling = false;
		m_lineState.Blending = false;
		m_lineState.Depth = false;
		m_lineState.DepthWrite = false;
	}

	PlaneRenderer::~PlaneRenderer()
	{

	}

	void PlaneRenderer::RenderInit( ddc::World& world )
	{
		m_lineShader = ShaderManager::Instance()->Load( "line" );
		DD_ASSERT( m_lineShader.IsValid() );

		Shader* line_shader = m_lineShader.Access();
		DD_ASSERT( line_shader != nullptr );

		line_shader->Use( true );

		m_vao.Create();
		m_vao.Bind();

		m_vbo.Create( GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		m_vbo.Bind();
		m_vbo.SetData( s_linesBuffer );
		m_vbo.CommitData();

		line_shader->BindPositions();

		m_vbo.Unbind();
		m_vao.Unbind();

		line_shader->Use( false );

		ShaderHandle mesh_shader = ShaderManager::Instance()->Load( "mesh" );
		DD_ASSERT( mesh_shader.IsValid() );

		m_mesh = MeshManager::Instance()->Find( "quad" );

		m_meshMaterial = MaterialManager::Instance()->Create( "plane" );

		Material* material = m_meshMaterial.Access();
		material->Shader = mesh_shader;
		material->State.BackfaceCulling = false;
		material->State.Blending = true;
		material->State.Depth = true;
		material->State.DepthWrite = false;
	}

	void PlaneRenderer::Render( const ddr::RenderData& data )
	{
		Shader* line_shader = m_lineShader.Access();
		DD_ASSERT( line_shader != nullptr );

		m_lineState.Use( true );

		line_shader->Use( true );

		m_vao.Bind();
		m_vbo.Bind();

		glm::mat4 view = data.Camera().GetViewMatrix();
		glm::mat4 projection = data.Camera().GetViewMatrix();
		glm::mat4 view_projection = projection * view;

		auto planes = data.Get<dd::PlaneComponent>();
		auto colours = data.Get<dd::ColourComponent>();
		auto transforms = data.Get<dd::TransformComponent>();

		for( size_t i = 0; i < data.Size(); ++i )
		{
			const dd::PlaneComponent& plane = planes[i];

			const dd::TransformComponent* transform_cmp = transforms.Get( i );

			glm::mat4 transform( 1 );
			if( transform_cmp != nullptr )
			{
				transform = transform_cmp->Transform();
			}

			const dd::ColourComponent* colour_cmp = colours.Get( i );
			glm::vec4 colour( 1 );
			if( colour_cmp != nullptr )
			{
				colour = colour_cmp->Colour;
			}

			line_shader->SetUniform( "Colour", colour );

			glm::mat4 plane_transform( ddm::AlignToDir( glm::vec3( 0, 0, 1 ), plane.Plane.Normal() ) );
			plane_transform[3] = glm::vec4( plane.Plane.Origin(), 1 );

			glm::mat4 model = transform * plane_transform * glm::scale( glm::vec3( m_scale ) );

			line_shader->SetUniform( "ModelViewProjection", view_projection * model );

			glDrawArrays( GL_LINES, 0, m_vbo.GetDataSize() );

			colour.a = 0.3f;

			MeshRenderCommand* cmd;
			data.Commands().Allocate( cmd );

			cmd->Material = m_meshMaterial;
			cmd->Mesh = m_mesh;
			cmd->Colour = colour;
			cmd->Transform = model;
		}

		m_vbo.Unbind();
		m_vao.Unbind();

		line_shader->Use( false );

		m_lineState.Use( false );
	}
}