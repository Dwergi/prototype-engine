//
// Triangulator.h - A mesh triangulator helper for meshes with indices and without.
// Copyright (C) Sebastian Nordgren 
// September 25th 2018
//

#pragma once

namespace ddr
{
	struct Mesh;
}

namespace dd
{
	struct ConstTriangle
	{
		ConstTriangle( const glm::vec3& a, const glm::vec3& b, const glm::vec3& c ) :
			p0( a ), p1( b ), p2( c ) 
		{
		}

		ConstTriangle( const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, uint ia, uint ib, uint ic ) :
			p0( a ), p1( b ), p2( c ),
			i0( ia ), i1( ib ), i2( ic )
		{
		}

		ConstTriangle( const ConstTriangle& other ) :
			p0( other.p0 ), p1( other.p1 ), p2( other.p2 ),
			i0( other.i0 ), i1( other.i1 ), i2( other.i2 )
		{
		}

		const uint i0 { ~0u };
		const uint i1 { ~0u };
		const uint i2 { ~0u };

		const glm::vec3& p0;
		const glm::vec3& p1;
		const glm::vec3& p2;
	};

	struct Triangulator
	{
		Triangulator( const ddr::Mesh& mesh );

		Triangulator( const dd::ConstBuffer<glm::vec3>& pos );
		Triangulator( const dd::ConstBuffer<glm::vec3>& pos, const dd::ConstBuffer<uint>& idx );

		Triangulator( const std::vector<glm::vec3>& pos );
		Triangulator( const std::vector<glm::vec3>& pos, const std::vector<uint>& idx );

		size_t Size() const { return m_size; }

		ConstTriangle operator[]( size_t i ) const;

	private:

		const glm::vec3* m_positions { nullptr };
		const uint* m_indices { nullptr };
		size_t m_size { 0 };
	};
}
