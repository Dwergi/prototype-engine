//
// GLError.cpp - Helpers for pinning down OpenGL errors.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#include "PrecompiledHeader.h"
#include "GLError.h"

#include "GL/gl3w.h"

namespace dd
{
	void _CheckGLError( const char* file, int line )
	{
		GLenum err( glGetError() );

		while( err != GL_NO_ERROR )
		{
			dd::String32 error;
			switch( err )
			{
			case GL_INVALID_OPERATION:
				error = "INVALID_OPERATION";
				break;
			case GL_INVALID_ENUM:
				error = "INVALID_ENUM";
				break;
			case GL_INVALID_VALUE:
				error = "INVALID_VALUE";
				break;
			case GL_OUT_OF_MEMORY:
				error = "OUT_OF_MEMORY";
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				error = "INVALID_FRAMEBUFFER_OPERATION";
				break;
			}

			printf( "GL_%s - %s:%d\n", error.c_str(), file, line );

			err = glGetError();
		}
	}
}