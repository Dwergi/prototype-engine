//
// Mesh.h - A mesh wrapper.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#pragma once

#include "AABB.h"
#include "ShaderProgram.h"
#include "VAO.h"

#include <atomic>
#include <memory>
#include <unordered_map>

namespace dd
{
	class Camera;
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
		void Render( const Camera& camera, const glm::mat4& transform );

		//
		// Retrieve the axis-aligned bounds of this mesh.
		//
		const AABB& Bounds() const;

		//
		// Set the bounds of this mesh.
		//
		void SetBounds( const AABB& bounds );

		//
		// Set the vertex buffer that the mesh will use.
		// The mesh does *NOT* take ownership of this.
		//
		void SetData( float* data, int count, int stride );

		//
		// Send updated mesh data to the GPU from the same place it currently is.
		//
		void UpdateData();

		//
		// Set the index buffer that the mesh will use.
		//
		void SetIndices( uint* data, int count );

		//
		// Bind the attribute of the given name and type.
		//
		void BindAttribute( const char* shaderAttribute, uint count, uint first, bool normalized );

		//
		// Set a colour multiplier that is applied to all vertices of the mesh.
		//
		void SetColourMultiplier( const glm::vec4& colour ) { m_colour = colour; }

		//
		// Get the shader used for this mesh.
		//
		ShaderHandle GetShader() const { return m_shader; }

		void MakeUnitCube();

		Mesh& operator=( const Mesh& other );
		Mesh( const Mesh& other );
		~Mesh();

	private:

		friend class MeshHandle;

		static std::mutex m_instanceMutex;
		static std::unordered_map<uint64, Mesh> m_instances;
		
		uint m_indexVBO;
		uint m_vbo;
		VAO m_vao;

		Buffer<uint> m_indices;
		Buffer<float> m_data;
		uint m_stride;

		String128 m_name;
		ShaderHandle m_shader;
		AABB m_bounds;
		glm::vec4 m_colour;

		std::atomic<int>* m_refCount;
		void Retain();
		void Release();

		Mesh( const char* name, ShaderHandle program );

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