//
// OpenGL.cpp - Wrappers for OpenGL draw calls to allow for better tracking of draw calls.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#include "PCH.h"
#include "OpenGL.h"

#include "GLError.h"
#include "Profiler.h"

static dd::ProfilerValueRef s_drawCallProfiler("Render/Draw Calls");

namespace OpenGL
{
	bool Initialize()
	{
		return gl3wInit() == 0;
	}

	GLenum GetGLPrimitive(Primitive primitive)
	{
		switch (primitive)
		{
			case Primitive::Triangles: return GL_TRIANGLES;
			case Primitive::Lines: return GL_LINES;
			default: DD_ASSERT(false, "Unknown primitive."); return GL_NONE;
		};
	}

	void DrawArrays(Primitive primitive, uint64 verts)
	{
		DD_ASSERT(verts > 0);

		s_drawCallProfiler.Increment();

		glDrawArrays(GetGLPrimitive(primitive), 0, (GLsizei) verts);
		CheckOGLError();
	}

	void DrawElements(Primitive primitive, uint64 indices)
	{
		DD_ASSERT(indices > 0);

		s_drawCallProfiler.Increment();

		glDrawElements(GetGLPrimitive(primitive), (GLsizei) indices, GL_UNSIGNED_INT, 0);
		CheckOGLError();
	}

	void DrawArraysInstanced(Primitive primitive, uint64 verts, uint64 instances)
	{
		DD_ASSERT(verts > 0);
		DD_ASSERT(instances > 0);

		s_drawCallProfiler.Increment();

		glDrawArraysInstanced(GetGLPrimitive(primitive), 0, (GLsizei) verts, (GLsizei) instances);
		CheckOGLError();
	}

	void DrawElementsInstanced(Primitive primitive, uint64 indices, uint64 instances)
	{
		DD_ASSERT(indices > 0);
		DD_ASSERT(instances > 0);

		s_drawCallProfiler.Increment();

		glDrawElementsInstanced(GetGLPrimitive(primitive), (GLsizei) indices, GL_UNSIGNED_INT, 0, (GLsizei)instances);
		CheckOGLError();
	}
}

namespace ddr
{
	dd::String64 GetArrayUniformName( const char* arrayName, size_t index, const char* uniform )
	{
		dd::String64 result;
		result += arrayName;
		result += "[";

		char buffer[ 32 ];
		_itoa_s( (int) index, buffer, 10 );
		result += buffer;

		result += "]";

		if( uniform != nullptr )
		{
			result += ".";
			result += uniform;
		}

		return result;
	}
}