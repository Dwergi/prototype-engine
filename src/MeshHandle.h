//
// A very simple handle to be used to reference a single global instance of a mesh in a semi-safe way.
// Use Mesh::Create to get a handle to a given mesh.
// Copyright (C) Sebastian Nordgren 
// July 12th 2018
//

#pragma once

namespace ddr
{
	struct Mesh;

	struct MeshHandle
	{
	public:
		MeshHandle() {}

		bool IsValid() const { return m_id != ~0u; }

		DD_BASIC_TYPE( MeshHandle );

	private:
		friend struct Mesh;
		size_t m_id { ~0u };
	};
}