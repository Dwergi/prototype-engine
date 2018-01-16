//
// TrenchSystem.cpp - Procedural infinite trench system.
// Copyright (C) Sebastian Nordgren 
// January 12th 2017
//

#include "PrecompiledHeader.h"
#include "TrenchSystem.h"

#include "EntityManager.h"
#include "ICamera.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "PlayerComponent.h"
#include "Shader.h"
#include "ShaderProgram.h"

#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace dd
{
	const float TRENCH_CHUNK_LENGTH = 10.0f;

	static const glm::vec3 s_trenchChunkPositions[] =
	{
		//  X  Y  Z       
		// bottom
		glm::vec3( 0.5f,0.0f,0.0f ),   
		glm::vec3( -0.5f,0.0f,0.0f ),  
		glm::vec3( -0.5f,0.0f,1.0f ),  
		glm::vec3( 0.5f,0.0f,1.0f ),   
		glm::vec3( 0.5f,0.0f,0.0f ),   
		glm::vec3( -0.5f,0.0f,1.0f ),  

		// left
		glm::vec3( -0.5f,1.0f,0.0f ),
		glm::vec3( -0.5f,0.0f,1.0f ),
		glm::vec3( -0.5f,0.0f,0.0f ),
		glm::vec3( -0.5f,1.0f,1.0f ),
		glm::vec3( -0.5f,0.0f,1.0f ),
		glm::vec3( -0.5f,1.0f,0.0f ),

		// right
		glm::vec3( 0.5f,0.0f,0.0f ),
		glm::vec3( 0.5f,0.0f,1.0f ),
		glm::vec3( 0.5f,1.0f,0.0f ),
		glm::vec3( 0.5f,1.0f,0.0f ),
		glm::vec3( 0.5f,0.0f,1.0f ),
		glm::vec3( 0.5f,1.0f,1.0f ),
	};

	static const ConstBuffer<glm::vec3> s_trenchChunkPositionBuffer( s_trenchChunkPositions, sizeof( s_trenchChunkPositions ) / sizeof( glm::vec3 ) );

	static const glm::vec3 s_trenchChunkNormals[] =
	{
		// X	Y     Z
		// bottom
		glm::vec3( 0.0f, 1.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ),

		// left
		glm::vec3( 1.0f, 0.0f, 0.0f ),
		glm::vec3( 1.0f, 0.0f, 0.0f ),
		glm::vec3( 1.0f, 0.0f, 0.0f ),
		glm::vec3( 1.0f, 0.0f, 0.0f ),
		glm::vec3( 1.0f, 0.0f, 0.0f ),
		glm::vec3( 1.0f, 0.0f, 0.0f ),

		// right
		glm::vec3( -1.0f, 0.0f, 0.0f ),
		glm::vec3( -1.0f, 0.0f, 0.0f ),
		glm::vec3( -1.0f, 0.0f, 0.0f ),
		glm::vec3( -1.0f, 0.0f, 0.0f ),
		glm::vec3( -1.0f, 0.0f, 0.0f ),
		glm::vec3( -1.0f, 0.0f, 0.0f ),
	};

	static const ConstBuffer<glm::vec3> s_trenchChunkNormalBuffer( s_trenchChunkNormals, sizeof( s_trenchChunkNormals ) / sizeof( glm::vec3 ) );

	/*
	DATA:

	Each TrenchComponent:
	- Represents a segment of trench TRENCH_CHUNK_LENGTH long.
	- Is a cube, with 2 sides open
		- 4 of the sides are made up of meshes and potentially some gameplay component (turret, target, etc.)
		- 2 of the sides are open
		- In the case where there is a corner, the player is forced to turn

	LOGIC:

	On each update:
	- Check if a chunk is too far (TRENCH_CHUNK_DESTRUCTION_DISTANCE) behind the player and destroy the entity if it is
	- Create new chunks ahead of the player if necessary
	*/

	TrenchSystem::TrenchSystem( const ICamera& camera ) :
		m_trenchDirection( 0.0f, 0.0f, 1.0f ),
		m_trenchOrigin( 0.0f, 0.0f, 0.0f ),
		m_camera( camera )
	{

	}

	TrenchSystem::~TrenchSystem()
	{

	}

	namespace
	{
		ShaderHandle CreateShaders( const char* name )
		{
			Vector<Shader*> shaders;

			Shader* vert = Shader::Create( String8( "shaders\\standard.vertex" ), Shader::Type::Vertex );
			DD_ASSERT( vert != nullptr );
			shaders.Add( vert );

			Shader* geom = Shader::Create( String8( "shaders\\standard.geometry" ), Shader::Type::Geometry );
			DD_ASSERT( geom != nullptr );
			shaders.Add( geom );

			Shader* pixel = Shader::Create( String8( "shaders\\standard.pixel" ), Shader::Type::Pixel );
			DD_ASSERT( pixel != nullptr );
			shaders.Add( pixel );

			ShaderHandle handle = ShaderProgram::Create( String8( name ), shaders );
			return handle;
		}
	}

	void TrenchSystem::CreateRenderResources()
	{
		m_shader = CreateShaders( "trench_chunk" );
		m_chunkMesh = Mesh::Create( "trench_chunk", m_shader );

		ShaderProgram& shader = *m_shader.Get();
		shader.Use( true );
		shader.SetPositionsName( "Position" );
		shader.SetNormalsName( "Normal" );

		Mesh* mesh = m_chunkMesh.Get();
		mesh->SetPositions( s_trenchChunkPositionBuffer );
		mesh->SetNormals( s_trenchChunkNormalBuffer );

		shader.Use( false );

		AABB bounds;
		bounds.Expand( glm::vec3( -0.5, 0, 0 ) );
		bounds.Expand( glm::vec3( 0.5, 1, 1 ) );
		mesh->SetBounds( bounds );
	}

	EntityHandle TrenchSystem::CreateTrenchChunk( glm::vec3 position, EntityManager& entity_manager )
	{
		EntityHandle handle = entity_manager.CreateEntity<TransformComponent, MeshComponent>();

		glm::mat4 scale = glm::scale( glm::vec3( TRENCH_CHUNK_LENGTH, TRENCH_CHUNK_LENGTH, TRENCH_CHUNK_LENGTH ) );
		glm::mat4 translate = glm::translate( position );
		glm::mat4 transform = translate * scale;

		TransformComponent* transform_cmp = entity_manager.GetWritable<TransformComponent>( handle );
		transform_cmp->SetLocalTransform( transform );

		MeshComponent* mesh_cmp = entity_manager.GetWritable<MeshComponent>( handle );
		mesh_cmp->Mesh = m_chunkMesh;
		mesh_cmp->Colour = glm::vec4( 0.5f, 0.5f, 0.7f, 1.0f );
		mesh_cmp->Hidden = false;

		return handle;
	}

	void TrenchSystem::Update( EntityManager& entity_manager, float delta_t )
	{
		// cache these here to avoid recalculating for all components
		glm::vec3 player_pos = m_camera.GetPosition();

		int chunk_index = (int) (player_pos / (m_trenchDirection * TRENCH_CHUNK_LENGTH)).z;
		
		const int chunk_count = (int) (m_camera.GetFar() / TRENCH_CHUNK_LENGTH);

		for( int i = 0; i < chunk_count; ++i )
		{
			glm::vec3 position = ((float) chunk_index + i) * (m_trenchDirection * TRENCH_CHUNK_LENGTH);

			if( !m_chunks.Contains( position ) )
			{
				m_chunks.Add( position, CreateTrenchChunk( position, entity_manager ) );
			}
		}
	}
}