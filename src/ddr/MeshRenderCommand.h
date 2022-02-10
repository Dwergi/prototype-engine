//
// MeshRenderCommand.h
// Copyright (C) Sebastian Nordgren 
// December 17th 2018
//

#pragma once

#include "ddr/Mesh.h"
#include "ddr/RenderCommand.h"
#include "ddr/RenderCommandBuffer.h"

namespace ddr
{
	struct MeshRenderCommand : RenderCommand
	{
		MeshHandle Mesh;
		glm::mat4 Transform;
		glm::vec4 Colour;

		MeshRenderCommand();

		void InitializeKey(const ICamera& camera);
	};

	using MeshRenderCommandBuffer = RenderCommandBuffer<MeshRenderCommand>;
}