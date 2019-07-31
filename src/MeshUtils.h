//
// MeshUtils.h - Various utilities to make meshes of various shapes.
// Copyright (C) Sebastian Nordgren 
// September 25th 2018
//

#pragma once

namespace ddr
{
	struct Mesh;
	class VBO;
}

namespace dd
{
	struct MeshUtils
	{
		static void CreateDefaultMaterial();
		static void CreateUnitCube();
		static void CreateUnitSphere();
		static void CreateQuad();

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
