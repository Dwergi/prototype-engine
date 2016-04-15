//
// Mesh.h - A mesh wrapper.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#pragma once

#include <atomic>

namespace dd
{
	class Camera;
	class ShaderProgram;

	struct MeshHandle;	 

	//
	// A ref-counted mesh asset.
	//
	class Mesh
	{
	public:

		static MeshHandle Create( const char* name, ShaderProgram& program );
		static Mesh* Get( MeshHandle handle );

		Mesh( const Mesh& other );
		~Mesh();

		//
		// Render this mesh in the given camera viewport.
		//
		void Render( const Camera& camera, const glm::vec3& position );

		Mesh& operator=( const Mesh& other );

		void AddRef();
		void RemoveRef();

	private:

		static DenseMap<uint64, Mesh> m_instances;
		
		uint m_vbo;
		uint m_vao;
		String128 m_name;
		ShaderProgram* m_shader;

		std::atomic<int>* m_refCount;
		void Retain();
		void Release();

		Mesh( const char* name, ShaderProgram& program );
	};

	struct MeshHandle
	{
	public:
	
		MeshHandle() : m_hash( 0 ) {}

		Mesh* Get()
		{
			return Mesh::Get( *this );
		}

		bool IsValid() const
		{
			return m_hash != 0;
		}

	private:

		friend class Mesh;

		uint64 m_hash;
	};
}