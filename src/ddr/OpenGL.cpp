//
// OpenGL.cpp - Wrappers for OpenGL draw calls to allow for better tracking of draw calls.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#include "PCH.h"
#include "ddr/OpenGL.h"

#include "ddr/GLError.h"
#include "ddr/VAO.h"

#include "Profiler.h"

#include "fmt/format.h"

static dd::ProfilerValueRef s_drawCallProfiler("Render/Draw Calls");

namespace OpenGL
{
	void GLErrorCallback(GLenum src, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* msg, void const* user_param)
	{
		std::string src_str = [src]()
		{
			switch (src)
			{
				case GL_DEBUG_SOURCE_API: return "API";
				case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window";
				case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
				case GL_DEBUG_SOURCE_THIRD_PARTY: return "Third Party";
				case GL_DEBUG_SOURCE_APPLICATION: return "Application";
				case GL_DEBUG_SOURCE_OTHER: return "Other";
				default: return "<none>";
			}
		}();

		std::string type_str = [type]()
		{
			switch (type)
			{
				case GL_DEBUG_TYPE_ERROR: return "Error";
				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated";
				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "Undefined Behavior";
				case GL_DEBUG_TYPE_PORTABILITY: return "Portability";
				case GL_DEBUG_TYPE_PERFORMANCE: return "Performance";
				case GL_DEBUG_TYPE_MARKER: return "Marker";
				case GL_DEBUG_TYPE_OTHER: return "Other";
				default: return "<none>";
			}
		}();

		std::string severity_str = [severity]()
		{
			switch (severity)
			{
				case GL_DEBUG_SEVERITY_NOTIFICATION: return "Notification";
				case GL_DEBUG_SEVERITY_LOW: return "Low";
				case GL_DEBUG_SEVERITY_MEDIUM: return "Medium";
				case GL_DEBUG_SEVERITY_HIGH: return "High";
				default: return "<none>";
			}
		}();

		std::string message = fmt::format("{} - {} ({}) (#{}): {}", src_str, type_str, severity_str, id, msg);
		DD_ASSERT(false, message.c_str());
	}

	bool Initialize()
	{
		bool success = gl3wInit() == 0;

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		glDebugMessageCallback(&GLErrorCallback, nullptr);
		return success;
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

		glDrawElementsInstanced(GetGLPrimitive(primitive), (GLsizei) indices, GL_UNSIGNED_INT, 0, (GLsizei) instances);
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
