//
// MeshRenderCommand.h
// Copyright (C) Sebastian Nordgren 
// December 17th 2018
//

#pragma once

#include "Mesh.h"
#include "RenderCommand.h"

namespace ddr
{
	struct UniformStorage;

	struct MeshRenderCommand : RenderCommand
	{
		MeshHandle Mesh;
		glm::mat4 Transform;
		glm::vec4 Colour;

		MeshRenderCommand();

		void InitializeKey( const ICamera& camera );
	};
}