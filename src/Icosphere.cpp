#include "PrecompiledHeader.h"
#include "Icosphere.h"

namespace dd
{
	// reference: http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
	// https://github.com/caosdoar/spheres/blob/master/src/spheres.cpp
	static const float T = (1.0f + sqrt( 5.0f )) / 2.0f;

	static glm::vec3 s_basePosition[ 12 ] =
	{
		glm::vec3( -1, T, 0 ),
		glm::vec3( 1, T, 0 ),
		glm::vec3( -1, -T, 0 ),
		glm::vec3( 1, -T, 0 ),

		glm::vec3( 0, -1, T ),
		glm::vec3( 0, 1, T ),
		glm::vec3( 0, -1, -T ),
		glm::vec3( 0, 1, -T ),

		glm::vec3( T, 0, -1 ),
		glm::vec3( T, 0, 1 ),
		glm::vec3( -T, 0, -1 ),
		glm::vec3( -T, 0, 1 )
	};

	static uint s_baseIndex[ 20 * 3 ] =
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

	static glm::vec3 s_icosphere1Position[ 12 * 4 ];
	static uint s_icosphere1Index[ 20 * 4 * 3 ];
	static bool s_icosphere1Generated { false };

	static uint s_icosphere2Position[ 12 * 4 * 4 ];
	static uint s_icosphere2Index[ 20 * 4 * 4 * 3 ];
	static bool s_icosphere2Generated { false };

	template <size_t Size>
	void NormalizePositions( glm::vec3 (&arr)[ Size ] )
	{
		for( glm::vec3& v : arr )
		{
			v = glm::normalize( v );
		}
	}

	void Subdivide( const glm::vec3* src_pos, const uint* src_index, uint src_triangles,
		glm::vec3* dst_pos, uint* dst_index, uint dst_triangles )
	{
		std::unordered_map<int64, uint>

	}

	void MakeIcosphere( ddr::Mesh& mesh, int iterations )
	{
		if( iterations == 0 )
		{
			NormalizePositions( s_basePosition );
		}

		if( iterations == 1 )
		{
			Subdivide( s_basePosition, s_baseIndex, 20,
				s_icosphere1Position, s_icosphere1Index, 20 * 4 );

			NormalizePositions( s_icosphere1Position );

			s_icosphere1Generated = true;
		}
	}
}