//
// MeshRenderCommand.cpp
// Copyright (C) Sebastian Nordgren 
// December 17th 2018
//

#include "PCH.h"
#include "MeshRenderCommand.h"

namespace ddr
{
	void MeshRenderCommand::Dispatch() const
	{
		ddr::Mesh* mesh = Mesh.Access();

		UniformStorage uniforms;

		mesh->Render();
	}
}