//
// ShaderHandle.cpp
// A very simple handle to be used to reference a single global instance of a shader in a semi-safe way.
// Use ShaderProgram::Create to get a handle to a shader.
//
// Copyright (C) Sebastian Nordgren 
// January 7th 2018
//

#include "PrecompiledHeader.h"
#include "ShaderHandle.h"

#include "ShaderProgram.h"

namespace dd
{
	ShaderProgram* ShaderHandle::Get() const
	{
		return ShaderProgram::Get( *this );
	}
}