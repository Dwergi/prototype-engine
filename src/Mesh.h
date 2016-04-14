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

	//
	// A ref-counted mesh asset.
	//
	class Mesh
	{
	public:

		Mesh( const char* name, ShaderProgram& program );
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
		
		uint m_vbo;
		uint m_vao;

		ShaderProgram* m_shader;

		std::atomic<int>* m_refCount;

		String128 m_name;

		void Retain();
		void Release();
	};
}