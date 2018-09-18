#include "PrecompiledHeader.h"
#include "Icosphere.h"

#include "Mesh.h"
#include "VBO.h"

namespace dd
{
	// reference: http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
	// https://github.com/caosdoar/spheres/blob/master/src/spheres.cpp
	static const float T = (1.0f + sqrt( 5.0f )) / 2.0f;

	static const std::vector<glm::vec3> s_basePosition =
	{
		glm::normalize( glm::vec3( -1,  T, 0 ) ),
		glm::normalize( glm::vec3(  1,  T, 0 ) ),
		glm::normalize( glm::vec3( -1, -T, 0 ) ),
		glm::normalize( glm::vec3(  1, -T, 0 ) ),

		glm::normalize( glm::vec3( 0, -1,  T ) ),
		glm::normalize( glm::vec3( 0,  1,  T ) ),
		glm::normalize( glm::vec3( 0, -1, -T ) ),
		glm::normalize( glm::vec3( 0,  1, -T ) ),

		glm::normalize( glm::vec3(  T, 0, -1 ) ),
		glm::normalize( glm::vec3(  T, 0,  1 ) ),
		glm::normalize( glm::vec3( -T, 0, -1 ) ),
		glm::normalize( glm::vec3( -T, 0,  1 ) )
	};

	static const std::vector<uint> s_baseIndex =
	{
		0, 11, 5,
		0, 5, 1,
		0, 1, 7,
		0, 7, 10,
		0, 10, 11,

		1, 5, 9,
		5, 11, 4,
		11, 10, 2,
		10, 7, 6,
		7, 1, 8,

		3, 9, 4,
		3, 4, 2,
		3, 2, 6,
		3, 6, 8,
		3, 8, 9,

		4, 9, 5,
		2, 4, 11,
		6, 2, 10,
		8, 6, 7,
		9, 8, 1
	};

	static std::vector<std::vector<glm::vec3>> s_positionLODs = { s_basePosition };
	static std::vector<std::vector<uint>> s_indexLODs = { s_baseIndex };

	void NormalizePositions( std::vector<glm::vec3>& vec )
	{
		for( glm::vec3& v : vec )
		{
			v = glm::normalize( v );
		}
	}

	uint64 GetKey( uint a, uint b )
	{
		uint64 low = min( a, b );
		uint64 high = max( a, b );

		uint64 key = (low << 32) + high;
		return key;
	}

	uint GetMidpoint( uint i0, uint i1, std::vector<glm::vec3>& positions, std::unordered_map<uint64, uint>& vert_cache )
	{
		uint64 key = GetKey( i0, i1 );

		auto it = vert_cache.find( key );
		if( it != vert_cache.end() )
		{
			return it->second;
		}

		glm::vec3 p0 = positions[i0];
		glm::vec3 p1 = positions[i1];

		glm::vec3 midpoint = (p0 + p1) / 2.0f;

		uint index = (uint) positions.size();
		positions.push_back( midpoint );

		vert_cache.insert( std::make_pair( key, index ) );

		return index;
	}

	void Subdivide( const std::vector<glm::vec3>& src_pos, const std::vector<uint>& src_idx,
		std::vector<glm::vec3>& dst_pos, std::vector<uint>& dst_idx )
	{
		DD_ASSERT( dst_pos.size() == 0 );
		DD_ASSERT( dst_idx.size() == 0 );

		dst_pos.reserve( src_pos.size() * 4 );
		dst_idx.reserve( src_idx.size() * 4 );

		// copy source verts to destination
		for( glm::vec3 v : src_pos )
		{
			dst_pos.push_back( v );
		}

		std::unordered_map<uint64, uint> vert_cache;

		for( size_t i = 0; i < src_idx.size(); i += 3 )
		{
			uint i0 = src_idx[i];
			uint i1 = src_idx[i + 1];
			uint i2 = src_idx[i + 2];

			uint i0i1 = GetMidpoint( i0, i1, dst_pos, vert_cache );
			uint i1i2 = GetMidpoint( i1, i2, dst_pos, vert_cache );
			uint i2i0 = GetMidpoint( i2, i0, dst_pos, vert_cache );

			dst_idx.push_back( i0 ); dst_idx.push_back( i0i1 ); dst_idx.push_back( i2i0 );
			dst_idx.push_back( i1 ); dst_idx.push_back( i1i2 ); dst_idx.push_back( i0i1 );
			dst_idx.push_back( i2 ); dst_idx.push_back( i2i0 ); dst_idx.push_back( i1i2 );
			dst_idx.push_back( i0i1 ); dst_idx.push_back( i1i2 ); dst_idx.push_back( i2i0 );
		}
	}

	void CalculateIcosphere( std::vector<glm::vec3>*& out_pos, std::vector<uint>*& out_idx, int iterations )
	{
		DD_ASSERT( iterations <= 6, "Too many iterations! Danger, Will Robinson!" );

		if( s_positionLODs.size() > iterations )
		{
			out_pos = &s_positionLODs[iterations];
			out_idx = &s_indexLODs[iterations];
			return;
		}

		std::vector<glm::vec3> src_pos;
		std::vector<uint> src_idx;

		size_t start = s_positionLODs.size() - 1;
		for( size_t i = start; i < iterations; ++i )
		{
			src_pos = s_positionLODs.back();
			src_idx = s_indexLODs.back();

			std::vector<glm::vec3>& dst_pos = s_positionLODs.emplace_back();
			std::vector<uint>& dst_idx = s_indexLODs.emplace_back();

			Subdivide( src_pos, src_idx, dst_pos, dst_idx );
			NormalizePositions( dst_pos );
		}

		out_pos = &s_positionLODs[iterations];
		out_idx = &s_indexLODs[iterations];
	}

	void MakeIcosphere( ddr::Mesh& mesh, int iterations )
	{
		std::vector<glm::vec3>* pos = nullptr;
		std::vector<uint>* idx = nullptr;
		CalculateIcosphere( pos, idx, iterations );

		dd::ConstBuffer<glm::vec3> positions(pos->data(),pos->size() );
		mesh.SetPositions( positions );

		dd::ConstBuffer<uint> indices( idx->data(), idx->size() );
		mesh.SetIndices( indices );
	}

	void MakeIcosphere( ddr::VBO& positions, ddr::VBO& indices, int iterations )
	{
		std::vector<glm::vec3>* pos = nullptr;
		std::vector<uint>* idx = nullptr;
		CalculateIcosphere( pos, idx, iterations );

		positions.SetData(pos->data(),pos->size() );
		indices.SetData( idx->data(), idx->size() );
	}

	void GetLineIndicesFromTriangles( const std::vector<uint>& src, std::vector<uint>& dest )
	{
		DD_ASSERT( dest.empty() );

		dest.reserve( src.size() * 2 );

		for( int i = 0; i < src.size(); i += 3 )
		{
			uint i0 = src[i];
			uint i1 = src[i + 1];
			uint i2 = src[i + 2];

			dest.push_back( i0  );
			dest.push_back( i1 );

			dest.push_back( i1 );
			dest.push_back( i2 );

			dest.push_back( i2 );
			dest.push_back( i0 );
		}
	}

	void MakeIcosphereLines( ddr::VBO& positions, ddr::VBO& indices, int iterations )
	{
		std::vector<glm::vec3>* pos = nullptr;
		std::vector<uint>* idx = nullptr;
		CalculateIcosphere( pos, idx, iterations );

		std::vector<uint> line_indices;
		GetLineIndicesFromTriangles( *idx, line_indices );

		positions.Bind();
		positions.SetData( pos->data(), pos->size() );
		positions.CommitData();
		positions.Unbind();

		indices.Bind();
		indices.SetData( line_indices.data(), line_indices.size() );
		indices.CommitData();
		indices.Unbind();
	}
}