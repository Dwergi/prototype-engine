//
// Mesh.h - A mesh wrapper.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#pragma once

#include "AABB.h"

#include <atomic>
#include <memory>

namespace dd
{
	class Camera;
	class ShaderProgram;

	struct MeshHandle;

	enum class MeshAttribute
	{
		Position,
		Normal,
		UV,
		Other
	};

	//
	// A ref-counted mesh asset.
	//
	class Mesh
	{
	public:

		//
		// Create (or retrieve) a handle to a mesh with the given name using the given shader.
		//
		static MeshHandle Create( const char* name, ShaderProgram& program );

		//
		// Get the mesh instance associated with the given handle.
		// Returns null if the handle does not reference a mesh that still exists.
		//
		static Mesh* Get( MeshHandle handle );

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
		void SetBounds( const AABB& bounds );

		void SetData( float* data, uint count, uint stride );
		void BindAttribute( const char* shaderAttribute, MeshAttribute type, uint count, bool normalized );

		void SetColourMultiplier( const glm::vec4& colour ) { m_colour = colour; }

		Mesh& operator=( const Mesh& other );
		Mesh( const Mesh& other );
		~Mesh();

	private:

		static std::mutex m_instanceMutex;
		static DenseMap<uint64, Mesh> m_instances;
		
		uint m_vbo;
		uint m_vao;
		String128 m_name;
		ShaderProgram* m_shader;
		AABB m_bounds;
		Buffer<float> m_data;
		uint m_stride;
		glm::vec4 m_colour;

		std::atomic<int>* m_refCount;
		void Retain();
		void Release();

		Mesh( const char* name, ShaderProgram& program );
	};

	//
	// A very simple handle to be used to reference a single global instance of a mesh in a semi-safe way.
	// Use Mesh::Create to get a handle to a given mesh.
	//
	struct MeshHandle
	{
	public:
		MeshHandle() : m_hash( 0 ) {}
		Mesh* Get() { return Mesh::Get( *this ); }

		BASIC_TYPE( MeshHandle )
		
	private:
		friend class Mesh;
		uint64 m_hash;
	};
}