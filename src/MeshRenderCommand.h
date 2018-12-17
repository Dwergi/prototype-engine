//
// MeshRenderCommand.h
// Copyright (C) Sebastian Nordgren 
// December 17th 2018
//

#pragma once

#include "RenderCommand.h"

namespace ddr
{
	struct MeshRenderCommand : RenderCommand
	{
		glm::mat4 Transform;
		MeshHandle Mesh;

		MeshRenderCommand() : Type( CommandType.Mesh ) {}

		void Dispatch() const;
	};
}