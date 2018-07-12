//
// MaterialHandle.h
// A very simple handle to be used to reference a single global instance of a material in a semi-safe way.
// Use Material::Create to get a handle to a shader.
//
// Copyright (C) Sebastian Nordgren 
// July 12th 2018
//

#pragma once

namespace ddr
{
	class Material;

	class MaterialHandle
	{
	public:
		MaterialHandle() : m_hash( 0 ) {}

		bool IsValid() const { return m_hash != 0; }

	private:
		friend class Material;

		uint64 m_hash;
	};
}