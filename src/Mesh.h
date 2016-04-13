//
// Mesh.h - A mesh wrapper.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#pragma once

#include "ShaderProgram.h"

namespace dd
{
	class Camera;

	class Mesh
	{
	public:

		Mesh();
		~Mesh();

		void Create( ShaderProgram& program );

		void Render( Camera& camera );

	private:
		
		Vector<glm::vec3> m_vertices;
		Vector<uint> m_indices;

		uint m_vbo;
		uint m_vao;

		ShaderProgram* m_shader;
	};
}