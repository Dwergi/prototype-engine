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
	struct MeshRenderCommand : RenderCommand
	{
		glm::mat4 Transform;
		MeshHandle Mesh;

		MeshRenderCommand();

		void Dispatch() const;
	};
}