//
// GLError.cpp - Helpers for pinning down OpenGL errors.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#include "PCH.h"
#include "GLError.h"
#include "OpenGL.h"

namespace dd
{
	void _CheckOGLError( const char* file, int line, bool ignore )
	{
		GLenum err( glGetError() );

		while( err != GL_NO_ERROR )
		{
			if( !ignore )
			{
				const char* error = "";
				switch( err )
				{
				case GL_INVALID_OPERATION:
					error = "GL_INVALID_OPERATION";
					break;
				case GL_INVALID_ENUM:
					error = "GL_INVALID_ENUM";
					break;
				case GL_INVALID_VALUE:
					error = "GL_INVALID_VALUE";
					break;
				case GL_OUT_OF_MEMORY:
					error = "GL_OUT_OF_MEMORY";
					break;
				case GL_INVALID_FRAMEBUFFER_OPERATION:
					error = "GL_INVALID_FRAMEBUFFER_OPERATION";
					break;
				}

				char message[256];
				snprintf( message, 256, "%s - %s:%d\n", error, file, line );

				__debugbreak();
				DD_ASSERT( false, message );
			}

			err = glGetError();
		}
	}
}