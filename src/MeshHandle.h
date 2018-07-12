//
// A very simple handle to be used to reference a single global instance of a mesh in a semi-safe way.
// Use Mesh::Create to get a handle to a given mesh.
// Copyright (C) Sebastian Nordgren 
// July 12th 2018
//

#pragma once

namespace ddr
{
	class Mesh;

	class MeshHandle
	{
	public:
		MeshHandle() : m_hash( 0 ) {}

		bool IsValid() const { return m_hash != 0; }

		BASIC_TYPE( MeshHandle )

	private:
		friend class Mesh;
		uint64 m_hash;
	};
}