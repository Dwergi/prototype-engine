//
// OpenGL.cpp - Wrappers for OpenGL draw calls to allow for better tracking of draw calls.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#include "PCH.h"
#include "OpenGL.h"

#include "GLError.h"
#include "Profiler.h"

static dd::ProfilerValueRef s_drawCallProfiler("Draw Calls");

namespace OpenGL
{
	bool Initialize()
	{
		return gl3wInit() == 0;
	}

	void DrawArrays(int verts)
	{
		DD_ASSERT(verts > 0);

		s_drawCallProfiler.Increment();

		glDrawArrays(GL_TRIANGLES, 0, verts);
		CheckOGLError();
	}

	void DrawArraysInstanced(int verts, int instances)
	{
		DD_ASSERT(verts > 0);
		DD_ASSERT(instances > 0);

		s_drawCallProfiler.Increment();

		glDrawArraysInstanced(GL_TRIANGLES, 0, verts, instances);
		CheckOGLError();
	}

	void DrawElementsInstanced(int indices, int instances)
	{
		DD_ASSERT(indices > 0);
		DD_ASSERT(instances > 0);

		s_drawCallProfiler.Increment();

		glDrawElementsInstanced(GL_TRIANGLES, indices, GL_UNSIGNED_INT, 0, instances);
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