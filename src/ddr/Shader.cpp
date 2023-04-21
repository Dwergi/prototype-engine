//
// ShaderProgram.cpp - Wrapper around OpenGL shader program.
// Copyright (C) Sebastian Nordgren 
// February 16th 2016
//

#include "PCH.h"
#include "Shader.h"

#include "ICamera.h"

#include "ddr/GLError.h"
#include "ddr/OpenGL.h"
#include "ddr/ShaderPart.h"
#include "ddr/Texture.h"
#include "ddr/VAO.h"
#include "ddr/VBO.h"

DD_POD_CPP(ddr::ShaderHandle);

static dd::ProfilerValueRef g_shaderChanged("Render/Shader Changes");

namespace ddr
{
	ScopedShader::ScopedShader(Shader& shader) :
		m_shader(&shader)
	{
		m_shader->Use(true);
	}

	ScopedShader::~ScopedShader()
	{
		if (m_shader != nullptr)
		{
			m_shader->Use(false);
		}
	}

	Shader::Shader()
	{
		m_id = glCreateProgram();
		CheckOGLError();

		DD_ASSERT_ERROR(m_id != OpenGL::InvalidID, "glCreateProgram failed!");
	}

	Shader::~Shader()
	{
		for (const ShaderPart* shader : m_shaders)
		{
			glDetachShader(m_id, shader->m_id);
		}

		if (m_id != OpenGL::InvalidID)
		{
			glDeleteProgram(m_id);
			m_id = OpenGL::InvalidID;
		}
	}

	void Shader::SetShaders(const dd::Vector<ShaderPart*>& shaders)
	{
		m_valid = true;

		if (shaders.Size() == 0)
		{
			DD_ASSERT_ERROR(shaders.Size() > 0, "ShaderProgram '%s': Failed to provide any shaders!", m_name.c_str());

			m_valid = false;
		}

		for (const ShaderPart* shader : shaders)
		{
			DD_ASSERT_ERROR(shader != nullptr, "ShaderProgram '%s': Invalid shader given to program!", m_name.c_str());

			glAttachShader(m_id, shader->m_id);
			CheckOGLError();
		}

		dd::String256 msg = Link();
		if (!msg.IsEmpty())
		{
			DD_ASSERT_ERROR(false, "ShaderProgram '%s': %s", m_name.c_str(), msg.c_str());

			m_valid = false;
		}

		m_shaders = shaders;
		glObjectLabel(GL_PROGRAM, m_id, (GLsizei) m_name.length(), m_name.c_str());
	}

	dd::String256 Shader::Link()
	{
		DD_PROFILE_SCOPED(ShaderProgram_Link);

		glLinkProgram(m_id);

		dd::String256 msg;

		GLint status;
		glGetProgramiv(m_id, GL_LINK_STATUS, &status);
		CheckOGLError();

		if (status == GL_FALSE)
		{
			msg = "\nProgram linking failure: \n";

			GLint infoLogLength;
			glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);
			CheckOGLError();

			char* strInfoLog = new char[infoLogLength + 1];
			glGetProgramInfoLog(m_id, infoLogLength, NULL, strInfoLog);
			CheckOGLError();

			strInfoLog[infoLogLength] = '\0';

			msg += strInfoLog;
			delete[] strInfoLog;

			m_valid = false;
		}

		return msg;
	}

	bool Shader::Reload()
	{
		for (ShaderPart* shader : m_shaders)
		{
			if (!shader->Reload())
			{
				break;
			}
		}

		dd::String256 msg = Link();
		if (!msg.IsEmpty())
		{
			DD_ASSERT_ERROR(false, "ShaderProgram '%s': %s", m_name.c_str(), msg.c_str());
			return false;
		}

		m_uniforms.clear();
		m_attributes.clear();

		return true;
	}

	bool Shader::InUse() const
	{
		return m_inUse;
	}

	void Shader::Use(bool use)
	{
		DD_ASSERT(use != m_inUse, "ShaderProgram '%s': Trying to use shader that is already in use!", m_name.c_str());

		g_shaderChanged.Increment();

		int current;
		glGetIntegerv(GL_CURRENT_PROGRAM, &current);
		CheckOGLError();

		if (use)
		{
			DD_ASSERT(current == 0, "ShaderProgram '%s': Shader not deactivated before trying to activate another shader!", m_name.c_str());
		}
		else
		{
			DD_ASSERT(current == m_id, "ShaderProgram '%s': Trying to deactivate a different shader than currently bound!", m_name.c_str());
		}

		m_inUse = use;

		glUseProgram(m_inUse ? m_id : 0);
		CheckOGLError();
	}

	ScopedShader Shader::UseScoped()
	{
		return ScopedShader(*this);
	}

	ShaderLocation Shader::GetAttribute(std::string_view name)
	{
		std::string name_str(name);

		auto it = m_attributes.find(name_str);
		if (it != m_attributes.end())
		{
			return it->second;
		}

		GLint location = glGetAttribLocation(m_id, (const GLchar*) name_str.c_str());
		CheckOGLError();

		m_attributes.insert(std::make_pair(name_str, location));

		return location;
	}

	bool Shader::BindPositions(VAO& vao, const VBO& vbo)
	{
		return CreateAttributeVec3(vao, vbo, "Position", Normalized::No, Instanced::No);
	}

	bool Shader::BindNormals(VAO& vao, const VBO& vbo)
	{
		return CreateAttributeVec3(vao, vbo, "Normal", Normalized::Yes, Instanced::No);
	}

	bool Shader::BindUVs(VAO& vao, const VBO& vbo)
	{
		return CreateAttributeVec2(vao, vbo, "UV", Normalized::No, Instanced::No);
	}

	bool Shader::BindVertexColours(VAO& vao, const VBO& vbo)
	{
		return CreateAttributeVec4(vao, vbo, "VertexColour", Normalized::No, Instanced::No);
	}

	bool Shader::CreateAttributeVec2(VAO& vao, const VBO& vbo, std::string_view name, Normalized normalized, Instanced instanced)
	{
		return CreateAttributeFloat(vao, vbo, name, 2, normalized, instanced);
	}

	bool Shader::CreateAttributeVec3(VAO& vao, const VBO& vbo, std::string_view name, Normalized normalized, Instanced instanced)
	{
		return CreateAttributeFloat(vao, vbo, name, 3, normalized, instanced);
	}

	bool Shader::CreateAttributeVec4(VAO& vao, const VBO& vbo, std::string_view name, Normalized normalized, Instanced instanced)
	{
		return CreateAttributeFloat(vao, vbo, name, 4, normalized, instanced);
	}

	bool Shader::CreateAttributeFloat(VAO& vao, const VBO& vbo, std::string_view name, uint components, Normalized normalized, Instanced instanced)
	{
		DD_ASSERT(components > 0 && components <= 4);

		ShaderLocation loc = GetAttribute(name);
		if (loc == InvalidLocation)
		{
			return false;
		}

		vao.CreateAttribute(name, loc, vbo, Format::Float, components, normalized, instanced, 0);
		return true;
	}

	bool Shader::CreateAttributeMatrix(VAO& vao, const VBO& vbo, std::string_view name, int components, Normalized normalized, Instanced instanced)
	{
		ShaderLocation loc = GetAttribute(name);
		if (loc == InvalidLocation)
		{
			return false;
		}

		size_t row_offset = sizeof(float) * components;

		for (int i = 0; i < components; ++i)
		{
			vao.CreateAttribute(name, loc + i, vbo, Format::Float, components, normalized, instanced, i * row_offset);
		}

		return true;
	}

	bool Shader::CreateAttributeMat2(VAO& vao, const VBO& vbo, std::string_view name, Normalized normalized, Instanced instanced)
	{
		return CreateAttributeMatrix(vao, vbo, name, 2, normalized, instanced);
	}
	
	bool Shader::CreateAttributeMat3(VAO& vao, const VBO& vbo, std::string_view name, Normalized normalized, Instanced instanced)
	{
		return CreateAttributeMatrix(vao, vbo, name, 3, normalized, instanced);
	}

	bool Shader::CreateAttributeMat4(VAO& vao, const VBO& vbo, std::string_view name, Normalized normalized, Instanced instanced)
	{
		return CreateAttributeMatrix(vao, vbo, name, 4, normalized, instanced);
	}

	ShaderLocation Shader::GetUniform(std::string_view name)
	{
		std::string name_str(name);

		auto it = m_uniforms.find(name_str);
		if (it != m_uniforms.end())
		{
			return it->second;
		}

		GLint location = glGetUniformLocation(m_id, (const GLchar*) name_str.c_str());
		CheckOGLError();

		m_uniforms.insert(std::make_pair(name_str, location));

		return location;
	}

	void Shader::SetUniform(std::string_view name, float f)
	{
		ShaderLocation uniform = GetUniform(name);
		if (uniform != InvalidLocation)
		{
			glProgramUniform1f(m_id, uniform, f);
			CheckOGLError();
		}
	}

	void Shader::SetUniform(std::string_view name, int i)
	{
		ShaderLocation uniform = GetUniform(name);
		if (uniform != InvalidLocation)
		{
			glProgramUniform1i(m_id, uniform, i);
			CheckOGLError();
		}
	}

	void Shader::SetUniform(std::string_view name, bool b)
	{
		ShaderLocation uniform = GetUniform(name);
		if (uniform != InvalidLocation)
		{
			glProgramUniform1i(m_id, uniform, b);
			CheckOGLError();
		}
	}

	void Shader::SetUniform(std::string_view name, const glm::vec2& vec)
	{
		ShaderLocation uniform = GetUniform(name);
		if (uniform != InvalidLocation)
		{
			glProgramUniform2fv(m_id, uniform, 1, glm::value_ptr(vec));
			CheckOGLError();
		}
	}

	void Shader::SetUniform(std::string_view name, const glm::vec3& vec)
	{
		ShaderLocation uniform = GetUniform(name);
		if (uniform != InvalidLocation)
		{
			glProgramUniform3fv(m_id, uniform, 1, glm::value_ptr(vec));
			CheckOGLError();
		}
	}

	void Shader::SetUniform(std::string_view name, const glm::vec4& vec)
	{
		ShaderLocation uniform = GetUniform(name);
		if (uniform != InvalidLocation)
		{
			glProgramUniform4fv(m_id, uniform, 1, glm::value_ptr(vec));
			CheckOGLError();
		}
	}

	void Shader::SetUniform(std::string_view name, const glm::mat3& mat)
	{
		ShaderLocation uniform = GetUniform(name);
		if (uniform != InvalidLocation)
		{
			glProgramUniformMatrix3fv(m_id, uniform, 1, false, glm::value_ptr(mat));
			CheckOGLError();
		}
	}

	void Shader::SetUniform(std::string_view name, const glm::mat4& mat)
	{
		ShaderLocation uniform = GetUniform(name);
		if (uniform != InvalidLocation)
		{
			glProgramUniformMatrix4fv(m_id, uniform, 1, false, glm::value_ptr(mat));
			CheckOGLError();
		}
	}

	void Shader::SetUniform(std::string_view name, const Texture& texture)
	{
		ShaderLocation uniform = GetUniform(name);
		if (uniform != InvalidLocation)
		{
			glProgramUniform1i(m_id, uniform, texture.GetTextureUnit());
			CheckOGLError();
		}
	}

	void Shader::AssertBeforeUse(const std::string& uniform) const
	{
		DD_ASSERT(InUse(), "ShaderProgram '%s': Need to use shader before trying to access it!", m_name.c_str());
		DD_ASSERT(IsValid(), "ShaderProgram '%s': Program is invalid!", m_name.c_str());
		DD_ASSERT(uniform.size() > 0, "ShaderProgram '%s': Empty uniform name given!", m_name.c_str());
	}

	ShaderHandle ShaderManager::Load(std::string_view name)
	{
		ShaderHandle shader_h = super::Find(name);
		if (!shader_h.IsValid())
		{
			std::string folder("shaders\\");

			dd::Vector<ShaderPart*> shaders;
			shaders.Reserve(3);

			{
				const std::string vertex_path = folder + name + ".vert";

				ShaderPart* vertex = ShaderPart::Create(vertex_path, ShaderPart::Type::Vertex);
				DD_ASSERT(vertex != nullptr);
				shaders.Add(vertex);
			}

			{
				const std::string geometry_path = folder + name + ".geom";

				ShaderPart* geom = ShaderPart::Create(geometry_path, ShaderPart::Type::Geometry);
				if (geom != nullptr)
				{
					shaders.Add(geom);
				}
			}

			{
				const std::string pixel_path = folder + name + ".frag";

				ShaderPart* pixel = ShaderPart::Create(pixel_path, ShaderPart::Type::Pixel);
				DD_ASSERT(pixel != nullptr);
				shaders.Add(pixel);
			}

			shader_h = super::Create(name);

			Shader* program = shader_h.Access();
			program->SetShaders(shaders);
		}

		DD_ASSERT(shader_h.IsValid());
		return shader_h;
	}

	void ShaderManager::ReloadAll()
	{
		for (size_t i = 0; i < super::LiveCount(); ++i)
		{
			Shader* program = super::AccessNth(i);
			program->Reload();
		}
	}

}