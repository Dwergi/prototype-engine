//
// Mesh.h - A mesh wrapper.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#pragma once

#include "AABB.h"
#include "MaterialHandle.h"
#include "VAO.h"
#include "VBO.h"

#include <memory>
#include <unordered_map>

namespace ddr
{
	class ICamera;
	struct MeshHandle;
	struct ShaderProgram;
	struct UniformStorage;

	//
	// A mesh asset.
	//
	class Mesh
	{
	public:

		//
		// Create (or retrieve) a handle to a mesh with the given name.
		//
		static MeshHandle Create( const char* name );

		//
		// Find a handle to a mesh with the given name.
		//
		static MeshHandle Find( const char* name );

		//
		// Get the mesh instance associated with the given handle.
		// Returns null if the handle does not reference a mesh that still exists.
		//
		static Mesh* Get( MeshHandle handle );

		//
		// Destroy the mesh associated with the given handle. 
		//
		static void Destroy( MeshHandle handle );

		//
		// Render this mesh in the given camera viewport.
		//
		void Render( UniformStorage& uniforms, ShaderProgram& shader, const glm::mat4& transform );

		//
		// Retrieve the axis-aligned bounds of this mesh.
		//
		const dd::AABB& Bounds() const { return m_bounds; }

		//
		// Set the bounds of this mesh.
		//
		void SetBounds( const dd::AABB& bounds ) { m_bounds = bounds; }

		//
		// Set the positions that the mesh will use.
		// The mesh does *NOT* take ownership of this.
		// The pointer in the buffer must remain valid for the lifetime of this mesh.
		//
		void SetPositions( const dd::ConstBuffer<glm::vec3>& positions );

		//
		// Set the index buffer that the mesh will use.
		// The mesh does *NOT* take ownership of this.
		//
		void SetIndices( const dd::ConstBuffer<uint>& indices );

		//
		// Set the normal buffer that the mesh will use.
		// The mesh does *NOT* take ownership of this.
		//
		void SetNormals( const dd::ConstBuffer<glm::vec3>& normals );

		//
		// Set the vertex colour buffer that the mesh will use.
		// The mesh does *NOT* take ownership of this.
		//
		void SetVertexColours( const dd::ConstBuffer<glm::vec4>& colours );

		//
		// Set the UV buffer that the mesh will use.
		// The mesh does *NOT* take ownership of this.
		//
		void SetUVs( const dd::ConstBuffer<glm::vec2>& uvs );

		//
		// Set the material that this mesh uses.
		//
		void SetMaterial( MaterialHandle material );

		//
		// Get the material this mesh uses.
		//
		MaterialHandle GetMaterial() const { return m_material; }

		//
		// Send updated mesh data to the GPU from the same place it currently is.
		//
		void UpdateBuffers();

		//
		// Get the name of this mesh.
		//
		const dd::String& GetName() const { return m_name; }

		bool IsDirty() const { return m_dirty; }
		void SetDirty() { m_dirty = true; }

		void MakeUnitCube();

		~Mesh();

		Mesh& operator=( const Mesh& ) = delete;
		Mesh& operator=( Mesh&& ) = delete;
		Mesh( const Mesh& ) = delete;
		Mesh( Mesh&& ) = delete;

	private:

		static std::mutex m_instanceMutex;
		static std::unordered_map<uint64, Mesh*> m_instances;

		bool m_dirty { false };
		
		VBO m_vboPosition;
		VBO m_vboNormal;
		VBO m_vboIndex;
		VBO m_vboUV;
		VBO m_vboVertexColour;

		MaterialHandle m_material;
		
		VAO m_vao;

		dd::String128 m_name;
		dd::AABB m_bounds;

		Mesh( const char* name );

		void BindToShader( ShaderProgram& shader );
	};
}