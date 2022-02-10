//
// MeshRenderCommand.cpp
// Copyright (C) Sebastian Nordgren 
// December 17th 2018
//

#include "PCH.h"
#include "MeshRenderCommand.h"

#include "ICamera.h"
#include "Mesh.h"
#include "Uniforms.h"

namespace ddr
{
	MeshRenderCommand::MeshRenderCommand() :
		RenderCommand(ddr::CommandType::Mesh)
	{
	}

	void MeshRenderCommand::InitializeKey(const ICamera& camera)
	{
		DD_ASSERT(Material.IsValid(), "Mesh has invalid material: %s", Mesh.GetName().c_str());
		DD_ASSERT(Material.IsAlive(), "Mesh has dead material: %s", Mesh.GetName().c_str());

		const ddr::Material* material = Material.Get();
		Key.Opaque = material->State.Blending;

		float depth = glm::distance(Transform[3].xyz(), camera.GetPosition());
		Key.Depth = ddr::DistanceToDepth(depth, Key.Opaque);
		Key.Mesh = dd::Hash(Mesh.GetID());
		Key.Material = dd::Hash(Material.GetID());
	}
}