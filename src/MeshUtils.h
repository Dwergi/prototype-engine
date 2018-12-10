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
	void MakeQuad( ddr::Mesh& mesh );
	
	void MakeUnitCube( ddr::Mesh& mesh );

	void MakeIcosphere( ddr::Mesh& mesh, int iterations );
	void MakeIcosphere( ddr::VBO& positions, ddr::VBO& indices, ddr::VBO& normals, int iterations );

	void MakeIcosphereLines( ddr::VBO& positions, ddr::VBO& indices, int iterations );

	void GetLineIndicesFromTriangles( const std::vector<uint>& src, std::vector<uint>& dest );

	//
	// Fetch indices for a grid mesh with the given maximum vertex count 
	// per side (ie. at LOD 0) for the given LOD.
	//
	const std::vector<uint>& GetGridIndices( uint vertex_count, uint lod );
}
