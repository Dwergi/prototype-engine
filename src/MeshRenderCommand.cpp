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
		RenderCommand( ddr::CommandType::Mesh )
	{
	}

	void MeshRenderCommand::InitializeKey( const ICamera& camera )
	{
		const ddr::Mesh* mesh = Mesh.Get();
		const ddr::Material* material = Material.Get();

		Key.Opaque = material->State.Blending;

		float depth = glm::distance( Transform[3].xyz(), camera.GetPosition() );
		Key.Depth = ddr::DistanceToDepth(depth, Key.Opaque);
		Key.Mesh = Mesh.GetID() & 0xFFF;
		Key.Material = Material.GetID() & 0xFFF;
	}
}