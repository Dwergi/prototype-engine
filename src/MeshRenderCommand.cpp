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
		const ddr::Material* material = mesh->GetMaterial().Get();

		Key.Opaque = material->State.Blending;

		float depth = glm::distance( Transform[3].xyz(), camera.GetPosition() );
		Key.Depth = ddr::DistanceToDepth( depth, Key.Opaque );
		Key.Material = mesh->GetMaterial().GetID() & 0x00FFFFFF;
	}

	void MeshRenderCommand::Dispatch( UniformStorage& uniforms ) const
	{
		ddr::Mesh* mesh = Mesh.Access();

		const ddr::Material* material = mesh->GetMaterial().Get();
		material->UpdateUniforms( uniforms );

		uniforms.Set( "Model", Transform );
		uniforms.Set( "ObjectColour", Colour );

		Shader* shader = material->Shader.Access();
		ScopedShader scoped_shader = shader->UseScoped();
		uniforms.Bind( *shader );

		mesh->Render();

		uniforms.Unbind();
	}
}