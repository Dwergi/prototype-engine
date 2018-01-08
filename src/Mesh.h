//
// Mesh.h - A mesh wrapper.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#pragma once

#include "AABB.h"
#include "ShaderProgram.h"
#include "VAO.h"
#include "VBO.h"

#include <atomic>
#include <memory>
#include <unordered_map>

namespace dd
{
	class ICamera;
	class ShaderProgram;

	class MeshHandle;

	//
	// A ref-counted mesh asset.
	//
	class Mesh
	{
	public:

		//
		// Create (or retrieve) a handle to a mesh with the given name using the given shader.
		//
		static MeshHandle Create( const char* name, ShaderHandle program );

		//
		// Destroy the mesh associated with the given handle. All handles will become invalidated.
		//
		static void Destroy( MeshHandle handle );

		//
		// Render this mesh in the given camera viewport.
		//
		void Render( const ICamera& camera, ShaderProgram& shader, const glm::mat4& transform );

		//
		// Retrieve the axis-aligned bounds of this mesh.
		//
		const AABB& Bounds() const { return m_bounds; }

		//
		// Set the bounds of this mesh.
		//
		void SetBounds( const AABB& bounds ) { m_bounds = bounds; }

		//
		// Set the positions that the mesh will use.
		// The mesh does *NOT* take ownership of this.
		// The pointer in the buffer must remain valid for the lifetime of this mesh.
		//
		void SetPositions( const ConstBuffer<glm::vec3>& positions );

		//
		// Enable indexed drawing.
		// SetIndices can be called before or after this, and the old buffer will be stored (but not used) between calls.
		//
		void EnableIndices( bool enabled ) { m_useIndex = enabled; }

		//
		// Set the index buffer that the mesh will use.
		// The mesh does *NOT* take ownership of this.
		//
		void SetIndices( const ConstBuffer<uint>& indices );

		//
		// Enable/disable normals.
		// SetNormals can be called before or after this, and the old buffer will be stored (but not used) between calls.
		//
		void EnableNormals( bool enabled ) { m_useNormal = enabled; }

		//
		// Set the normal buffer that the mesh will use.
		// The mesh does *NOT* take ownership of this.
		//
		void SetNormals( const ConstBuffer<glm::vec3>& normals );

		//
		// Enable/disable vertex colours.
		// SetNormals can be called before or after this, and the old buffer will be stored (but not used) between calls.
		//
		void EnableVertexColours( bool enabled ) { m_useVertexColour = enabled; }
		//
		// Set the vertex colour buffer that the mesh will use.
		// The mesh does *NOT* take ownership of this.
		//
		void SetVertexColours( const ConstBuffer<glm::vec4>& colours );

		//
		// Enable/disable UV coordinates.
		// SetUVs can be called before or after this, and the old buffer will be stored (but not used) between calls.
		//
		void EnableUVs( bool enabled ) { m_useUV = enabled; }

		//
		// Set the UV buffer that the mesh will use.
		// The mesh does *NOT* take ownership of this.
		//
		void SetUVs( const ConstBuffer<glm::vec2>& uvs );

		//
		// Send updated mesh data to the GPU from the same place it currently is.
		//
		void UpdateBuffers();

		//
		// Set a colour multiplier that is applied to all vertices of the mesh.
		//
		void SetColourMultiplier( const glm::vec4& colour ) { m_colourMultiplier = colour; }

		//
		// Get this mesh's shader.
		//
		ShaderProgram* GetShader() { return m_shader.Get(); }

		//
		// Use or release this mesh's shader.
		//
		void UseShader( bool use );

		//
		// Get the name of this mesh.
		//
		const String& GetName() const { return m_name; }

		void MakeUnitCube();

		Mesh& operator=( const Mesh& other );
		Mesh( const Mesh& other );
		~Mesh();

	private:

		friend class MeshHandle;

		static std::mutex m_instanceMutex;
		static std::unordered_map<uint64, Mesh*> m_instances;
		
		VBO m_vboPosition;
		ConstBuffer<glm::vec3> m_bufferPosition;

		bool m_useNormal { false };
		VBO m_vboNormal;
		ConstBuffer<glm::vec3> m_bufferNormal;

		bool m_useIndex { false };
		VBO m_vboIndex;
		ConstBuffer<uint> m_bufferIndex;

		bool m_useUV { false };
		VBO m_vboUV;
		ConstBuffer<glm::vec2> m_bufferUV;

		bool m_useVertexColour { false };
		VBO m_vboVertexColour;
		ConstBuffer<glm::vec4> m_bufferVertexColour;
		
		VAO m_vao;

		String128 m_name;
		ShaderHandle m_shader;
		AABB m_bounds;

		glm::vec4 m_colourMultiplier;

		std::atomic<int>* m_refCount;

		void Retain();
		void Release();

		Mesh( const char* name, ShaderHandle program );

		void Assign( const Mesh& other );

		//
		// Get the mesh instance associated with the given handle.
		// Returns null if the handle does not reference a mesh that still exists.
		//
		static Mesh* Get( MeshHandle handle );
	};

	//
	// A very simple handle to be used to reference a single global instance of a mesh in a semi-safe way.
	// Use Mesh::Create to get a handle to a given mesh.
	//
	class MeshHandle
	{
	public:
		MeshHandle() : m_hash( 0 ) {}

		Mesh* Get() const { return Mesh::Get( *this ); }

		bool IsValid() const { return m_hash != 0; }

		BASIC_TYPE( MeshHandle )
		
	private:
		friend class Mesh;
		uint64 m_hash;
	};
}