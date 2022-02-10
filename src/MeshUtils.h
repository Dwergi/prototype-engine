//
// MeshUtils.h - Various utilities to make meshes of various shapes.
// Copyright (C) Sebastian Nordgren 
// September 25th 2018
//

#pragma once

#include "ddr/Material.h"
#include "ddr/Mesh.h"

namespace ddr
{
	struct Mesh;
	struct VBO;
}

namespace dd
{
	struct MeshUtils
	{
		static void CreateDefaultMaterial();
		static ddr::MaterialHandle GetDefaultMaterial();

		static ddr::MeshHandle CreateUnitCube();
		static ddr::MeshHandle CreateUnitSphere();
		static ddr::MeshHandle CreateQuad();

		static void MakeQuad(ddr::Mesh& mesh);

		static void MakeUnitCube(ddr::Mesh& mesh);

		static void MakeIcosphere(ddr::Mesh& mesh, int iterations);
		static void MakeIcosphere(ddr::VBO& positions, ddr::VBO& indices, ddr::VBO& normals, int iterations);

		static void MakeIcosphereLines(ddr::VBO& positions, ddr::VBO& indices, int iterations);

		static void GetLineIndicesFromTriangles(const std::vector<uint>& src, std::vector<uint>& dest);

		//
		// Fetch indices for a grid mesh with the given maximum vertex count 
		// per side (ie. at LOD 0) for the given LOD.
		//
		static const std::vector<uint>& GetGridIndices(uint vertex_count, uint lod);
	};
}
