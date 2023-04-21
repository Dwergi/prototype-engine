//
// OpenGL.cpp - Wrappers for OpenGL draw calls to allow for better tracking of draw calls.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#include "PCH.h"
#include "ddr/OpenGL.h"

#include "ddr/GLError.h"
#include "ddr/VAO.h"
#include "ddr/VBO.h"

#include "Profiler.h"

#include "fmt/format.h"

static dd::ProfilerValueRef s_drawCallProfiler("Render/Draw Calls");

namespace OpenGL
{
	void OutputCommand(const char* commandName)
	{
		static bool s_outputDebug = false;
		if (s_outputDebug)
		{
			OutputDebugStringA(fmt::format("OpenGL: {}\n", commandName).c_str());
		}
	}

	void GLErrorCallback(GLenum src, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* msg, void const* user_param)
	{
		if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		{
		return;
		}
		
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
		
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		// filter out notification messages
		//glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		glDebugMessageCallback(&GLErrorCallback, nullptr);

		return success;
	}

	GLenum GetGLPrimitive(ddr::Primitive primitive)
	{
		switch (primitive)
		{
			case ddr::Primitive::Triangles: return GL_TRIANGLES;
			case ddr::Primitive::Lines: return GL_LINES;
			default: DD_ASSERT(false, "Unknown primitive."); return GL_NONE;
		};
	}

	GLenum GetGLFormat(ddr::Format format)
	{
		switch (format)
		{
			case ddr::Format::Float: return GL_FLOAT;
			default: DD_ASSERT(false, "Unknown format."); return GL_NONE;
		}
	}

	void DrawArrays(ddr::Primitive primitive, uint64 verts)
	{
		DD_ASSERT(verts > 0);

		OutputCommand("glDrawArrays");
		s_drawCallProfiler.Increment();

		glDrawArrays(GetGLPrimitive(primitive), 0, (GLsizei) verts);
		CheckOGLError();
	}

	void DrawElements(ddr::Primitive primitive, uint64 indices)
	{
		DD_ASSERT(indices > 0);
		
		OutputCommand("glDrawElements");
		s_drawCallProfiler.Increment();

		glDrawElements(GetGLPrimitive(primitive), (GLsizei) indices, GL_UNSIGNED_INT, 0);
		CheckOGLError();
	}

	void DrawArraysInstanced(ddr::Primitive primitive, uint64 verts, uint64 instances)
	{
		DD_ASSERT(verts > 0);
		DD_ASSERT(instances > 0);

		OutputCommand("glDrawArraysInstanced");
		s_drawCallProfiler.Increment();

		glDrawArraysInstanced(GetGLPrimitive(primitive), 0, (GLsizei) verts, (GLsizei) instances);
		CheckOGLError();
	}

	void DrawElementsInstanced(ddr::Primitive primitive, uint64 indices, uint64 instances)
	{
		DD_ASSERT(indices > 0);
		DD_ASSERT(instances > 0);

		OutputCommand("glDrawElementsInstanced");
		s_drawCallProfiler.Increment();

		glDrawElementsInstanced(GetGLPrimitive(primitive), (GLsizei) indices, GL_UNSIGNED_INT, 0, (GLsizei) instances);
 		CheckOGLError();
	}

	uint CreateVertexArray()
	{
		OutputCommand("glCreateVertexArrays");
		
		uint out_id;
		glCreateVertexArrays(1, &out_id);
		CheckOGLError();

		return out_id;
	}

	void DeleteVertexArray(const ddr::VAO& vao)
	{
		OutputCommand("glDeleteVertexArrays");

		const uint id = vao.ID();

		glDeleteVertexArrays(1, &id);
		CheckOGLError();
	}

	void BindVertexArray(const ddr::VAO& vao)
	{
		OutputCommand("glBindVertexArray");

		glBindVertexArray(vao.ID());
		CheckOGLError();
	}

	void UnbindVertexArray()
	{
		OutputCommand("glBindVertexArray");

		glBindVertexArray(0);
		CheckOGLError();
	}

	void EnableVertexArrayAttribute(const ddr::VAO& vao, ddr::ShaderLocation loc)
	{
		OutputCommand("glEnableVertexArrayAttrib");

		glEnableVertexArrayAttrib(vao.ID(), loc);
		CheckOGLError();
	}

	void DisableVertexArrayAttribute(const ddr::VAO& vao, ddr::ShaderLocation loc)
	{
		OutputCommand("glDisableVertexArrayAttrib");

		glDisableVertexArrayAttrib(vao.ID(), loc);
		CheckOGLError();
	}

	void SetVertexArrayAttributeFormat(const ddr::VAO& vao, ddr::ShaderLocation loc, ddr::Format format, uint components, ddr::Normalized normalized, uint64 offset)
	{
		OutputCommand("glVertexArrayAttribFormat");

		glVertexArrayAttribFormat(vao.ID(), loc, components, GetGLFormat(format), normalized == ddr::Normalized::Yes ? GL_TRUE : GL_FALSE, (GLuint) offset);
		CheckOGLError();
	}

	void SetVertexArrayBindingDivisor(const ddr::VAO& vao, uint vbo_index, uint divisor)
	{
		OutputCommand("glVertexArrayBindingDivisor");

		glVertexArrayBindingDivisor(vao.ID(), vbo_index, divisor);
		CheckOGLError();
	}

	void BindVertexArrayAttribute(const ddr::VAO& vao, ddr::ShaderLocation loc, uint vbo_index)
	{
		OutputCommand("glVertexArrayAttribBinding");

		glVertexArrayAttribBinding(vao.ID(), loc, vbo_index);
		CheckOGLError();
	}

	void SetVertexArrayBuffer(const ddr::VAO& vao, uint index, const ddr::VBO& vbo, uint offset, uint stride)
	{
		OutputCommand("glVertexArrayVertexBuffer");

		glVertexArrayVertexBuffer(vao.ID(), index, vbo.ID(), offset, stride);
		CheckOGLError();
	}

	void ClearVertexArrayBuffer(const ddr::VAO& vao, uint index)
	{
		OutputCommand("glVertexArrayVertexBuffer");

		glVertexArrayVertexBuffer(vao.ID(), index, 0, 0, 0);
		CheckOGLError();
	}

	void SetVertexArrayIndices(const ddr::VAO& vao, const ddr::VBO& vbo)
	{
		OutputCommand("glVertexArrayElementBuffer");

		glVertexArrayElementBuffer(vao.ID(), vbo.ID());
		CheckOGLError();
	}

	void ClearVertexArrayIndices(const ddr::VAO& vao)
	{
		OutputCommand("glVertexArrayElementBuffer");

		glVertexArrayElementBuffer(vao.ID(), 0);
		CheckOGLError();
	}

	uint CreateBuffer()
	{
		OutputCommand("glCreateBuffers");

		uint out_id;
		glCreateBuffers(1, &out_id);
		CheckOGLError();

		return out_id;
	}

	void DeleteBuffer(const ddr::VBO& vbo)
	{
		OutputCommand("glDeleteBuffers");

		const uint id = vbo.ID();
		glDeleteBuffers(1, &id);
		CheckOGLError();
	}

	void BufferStorage(const ddr::VBO& vbo, const dd::IBuffer& buffer)
	{
		DD_ASSERT(buffer.IsValid());
		DD_ASSERT(buffer.SizeBytes() > 0);
		DD_ASSERT(vbo.IsValid());

		OutputCommand("glNamedBufferStorage");

		glNamedBufferStorage(vbo.ID(), buffer.SizeBytes(), buffer.GetVoid(), GL_DYNAMIC_STORAGE_BIT);
		CheckOGLError();
	}

	void BufferSubData(const ddr::VBO& vbo, const dd::IBuffer& buffer)
	{
		DD_ASSERT(buffer.IsValid());
		DD_ASSERT(buffer.SizeBytes() > 0);
		DD_ASSERT(vbo.IsValid());

		OutputCommand("glNamedBufferSubData");

		glNamedBufferSubData(vbo.ID(), 0, buffer.SizeBytes(), buffer.GetVoid());
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
