#pragma once

namespace ddr
{
	class Mesh;
	class VBO;
}

namespace dd
{
	void MakeIcosphere( ddr::Mesh& mesh, int iterations );
	void MakeIcosphere( ddr::VBO& positions, ddr::VBO& indices, int iterations );

	void MakeIcosphereLines( ddr::VBO& positions, ddr::VBO& indices, int iterations );

	void GetLineIndicesFromTriangles( const std::vector<uint>& src, std::vector<uint>& dest );
}