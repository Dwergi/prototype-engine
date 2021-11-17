//
// Shader.cpp - Wrapper around an OpenGL shader.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#include "PCH.h"
#include "ShaderPart.h"

#include "File.h"
#include "OpenGL.h"
#include "Uniforms.h"

#include "fmt/format.h"

#include <charconv>

namespace ddr
{
	std::unordered_map<std::string, ShaderPart*> ShaderPart::sm_shaderCache;

	GLenum GetOpenGLShaderType(ShaderPart::Type type)
	{
		switch (type)
		{
			case ShaderPart::Type::Vertex:
				return GL_VERTEX_SHADER;

			case ShaderPart::Type::Pixel:
				return GL_FRAGMENT_SHADER;

			case ShaderPart::Type::Geometry:
				return GL_GEOMETRY_SHADER;
		}

		return GL_INVALID_INDEX;
	}

	ShaderPart::ShaderPart(std::string_view path, Type type) :
		m_path(path),
		m_type(type)
	{
		m_id = glCreateShader(GetOpenGLShaderType(type));

		DD_ASSERT_ERROR(m_id != OpenGL::InvalidID, "Failed to create shader!");
	}

	ShaderPart::~ShaderPart()
	{
		if (m_id != OpenGL::InvalidID)
		{
			glDeleteShader(m_id);
		}
	}

	bool ShaderPart::Reload()
	{
		m_uniforms.Clear();

		std::string oldSource = m_source;

		std::string source;
		if (!LoadFile(m_path, source))
		{
			DD_ASSERT_ERROR(false, "'%s': Failed to load.", m_path.c_str());
			return false;
		}

		std::string message = Compile(source);
		if (!message.empty())
		{
			DD_ASSERT_ERROR(false, "'%s': Compiling failed, message:\n %s", m_path.c_str(), message.c_str());

			m_source = oldSource;
			Compile(m_source);

			GatherUniforms(m_source, m_uniforms);
			return false;
		}

		m_source = source;

		GatherUniforms(m_source, m_uniforms);
		return true;
	}

	std::string ShaderPart::Compile(const std::string& source)
	{
		DD_PROFILE_SCOPED(Shader_Compile);

		std::string msg;

		const char* src = source.c_str();
		glShaderSource(m_id, 1, (const GLchar**) &src, NULL);

		glCompileShader(m_id);

		GLint status;
		glGetShaderiv(m_id, GL_COMPILE_STATUS, &status);

		if (status == GL_FALSE)
		{
			GLint infoLogLength;
			glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);

			char* strInfoLog = new char[(size_t) infoLogLength + 1];
			glGetShaderInfoLog(m_id, infoLogLength, NULL, strInfoLog);

			msg += strInfoLog;
			delete[] strInfoLog;
		}

		return msg;
	}

	size_t SkipWhitespace(std::string_view str, size_t start)
	{
		return str.find_first_not_of("\r\n\t ", start);
	}

	size_t EndOfWord(std::string_view str, size_t start)
	{
		return str.find_first_of("\r\n\t .,;[]()", start);
	}

	std::string_view BlockContents(std::string_view str, size_t& offset)
	{
		size_t block_start = str.find('{', offset);
		offset = str.find('}', block_start);

		std::string_view contents = str.substr(block_start + 1, offset - block_start - 1);
		++offset;
		return contents;
	}

	std::string_view NextWord(std::string_view str, size_t& offset)
	{
		size_t start = SkipWhitespace(str, offset);
		offset = EndOfWord(str, start);

		return str.substr(start, offset - start);
	}

	size_t NextLine(std::string_view str, size_t start)
	{
		size_t offset = str.find("\n", start);
		return SkipWhitespace(str, offset);
	}

	std::string ShaderPart::ProcessIncludes(std::string_view path, std::string_view src)
	{
		const char* const INCLUDE = "#include \"";

		std::string_view base_path = path.substr(0, path.rfind("\\"));

		std::string final_source(src);

		size_t include_start = 0;
		while ((include_start = final_source.find(INCLUDE, include_start)) != std::string::npos)
		{
			size_t file_start = include_start + strlen(INCLUDE);
			size_t file_end = final_source.find("\"", file_start);

			DD_ASSERT(file_end != std::string::npos, "'%s': Failed to find include name.");

			std::string include_name = final_source.substr(file_start, file_end - file_start);
			std::string include_path = base_path + "\\" + include_name;

			dd::File file(include_path);
			std::string include_source;
			if (!file.Read(include_source))
			{
				DD_ASSERT_ERROR(false, "'%s': Failed to load include path '%s'.", file.Path().c_str(), include_path.c_str());
			}

			include_source = ProcessIncludes(path, include_source);
			if (dd::last(include_source) != '\n')
			{
				include_source += '\n';
			}

			final_source.reserve(final_source.size() + include_source.size());
			final_source.erase(include_start, (file_end + 1) - include_start);
			final_source.insert(include_start, include_source);

			include_start += include_source.size();
		}

		return final_source;
	}

	void ParseStruct(std::string_view src, size_t head, UniformStorage& outUniforms)
	{
		std::string_view struct_contents = BlockContents(src, head);

		std::string_view struct_name = NextWord(src, head);

		size_t index_start = src.find('[', head);
		size_t line_end = src.find(';', head);

		int array_count = -1;

		if (index_start < line_end)
		{
			size_t index_end = index_start + 1;
			std::string_view index = NextWord(src, index_end);

			int parsed = -1;
			std::from_chars(index.data(), index.data() + index.size(), parsed);

			if (parsed > 0)
			{
				array_count = parsed;
			}
		}

		size_t struct_head = 0;
		struct_head = SkipWhitespace(struct_contents, struct_head);
		while (struct_head != std::string::npos)
		{
			std::string_view var_type = NextWord(struct_contents, struct_head);

			UniformType type = GetUniformTypeFromName(var_type);
			DD_ASSERT(type != UniformType::Invalid);

			std::string_view var_name = NextWord(struct_contents, struct_head);

			if (array_count == -1)
			{
				std::string full_name(var_name);
				if (!struct_name.empty())
				{
					full_name = std::string(struct_name) + "." + var_name;
				}

				outUniforms.Create(full_name.c_str(), type);
			}
			else
			{
				for (int i = 0; i < array_count; ++i)
				{
					std::string full_name = fmt::format("{0}[{1}].{2}", struct_name, i, var_name);
					outUniforms.Create(full_name.c_str(), type);
				}
			}

			struct_head = NextLine(struct_contents, struct_head);
		}
	}

	void ShaderPart::GatherUniforms(std::string_view src, UniformStorage& outUniforms)
	{
		const char* const UNIFORM = "uniform ";

		size_t uniform_start = 0;
		while ((uniform_start = src.find(UNIFORM, uniform_start)) != std::string::npos)
		{
			size_t head = uniform_start + strlen(UNIFORM);
			std::string_view type_name = NextWord(src, head);
			if (type_name == "struct")
			{
				ParseStruct(src, head, outUniforms);
			}
			else
			{
				UniformType type = GetUniformTypeFromName(type_name);
				if (type != UniformType::Invalid)
				{
					std::string_view var_name = NextWord(src, head);

					outUniforms.Create(var_name, type);
				}
				else
				{
					// uniform block
					ParseStruct(src, head, outUniforms);
				}
			}

			uniform_start = head;
		}
	}

	bool ShaderPart::LoadFile(std::string_view path, std::string& outSource)
	{
		dd::File file(path);
		std::string read;
		if (!file.Read(read))
		{
			return false;
		}

		outSource = ProcessIncludes(path, read);
		return true;
	}

	ShaderPart* ShaderPart::Create(std::string_view path_view, ShaderPart::Type type)
	{
		DD_PROFILE_SCOPED(Shader_Create);

		std::string path(path_view);

		auto it = sm_shaderCache.find(path);
		if (it != sm_shaderCache.end())
		{
			return it->second;
		}

		if (!dd::File::Exists(path))
		{
			return nullptr;
		}

		ShaderPart* shader = new ShaderPart(path, type);
		if (shader->m_id == OpenGL::InvalidID ||
			!shader->Reload())
		{
			DD_ASSERT(false, "Failed to load shader from '%s'!", path.c_str());
			delete shader;
			return nullptr;
		}

		sm_shaderCache.insert(std::make_pair(path, shader));
		return shader;
	}
}