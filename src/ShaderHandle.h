//
// ShaderHandle.h
// A very simple handle to be used to reference a single global instance of a shader in a semi-safe way.
// Use ShaderProgram::Create to get a handle to a shader.
//
// Copyright (C) Sebastian Nordgren 
// January 7th 2018
//

#pragma once

namespace dd
{
	class ShaderProgram;

	class ShaderHandle
	{
	public:
		ShaderHandle() : m_hash( 0 ) {}

		ShaderProgram* Get() const;

		bool IsValid() const { return m_hash != 0; }

	private:
		friend class ShaderProgram;
		friend class Mesh;

		uint64 m_hash;
	};
}