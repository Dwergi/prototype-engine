#pragma once

#include "RenderCommand.h"

namespace ddr
{
	struct MeshRenderCommand : RenderCommand
	{
		glm::mat4 Transform;
		MeshHandle Mesh;
	};
}