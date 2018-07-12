//
// ShaderHandle.h
// A very simple handle to be used to reference a single global instance of a shader in a semi-safe way.
// Use ddr::ShaderProgram::Create to get a handle to a shader.
//
// Copyright (C) Sebastian Nordgren 
// January 7th 2018
//

#pragma once

namespace ddr
{
	class ShaderProgram;

	class ShaderHandle
	{
	public:
		ShaderHandle() : m_hash( 0 ) {}

		bool Valid() const { return m_hash != 0; }

	private:
		friend class ShaderProgram;

		uint64 m_hash;
	};
}