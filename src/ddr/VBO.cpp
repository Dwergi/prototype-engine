//
// VBO.h - A wrapper around OpenGL VBOs.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#include "PCH.h"
#include "ddr/VBO.h"

#include "ddr/GLError.h"

namespace ddr
{
	static dd::Service<ddr::VBOManager> s_vboManager;

	VBO::VBO()
	{

	}

	VBO::VBO(const VBO& other) :
		m_id(other.m_id)
	{

	}

	VBO::~VBO()
	{
		m_id = OpenGL::InvalidID;
	}

	VBO& VBO::operator=(const VBO& other)
	{
		m_id = other.m_id;
		return *this;
	}

	void VBO::Create(const dd::IBuffer& buffer)
	{
		DD_ASSERT(!IsValid());

		glCreateBuffers(1, &m_id);
		CheckOGLError();

		glNamedBufferStorage(m_id, buffer.SizeBytes(), buffer.GetVoid(), GL_DYNAMIC_STORAGE_BIT);
		CheckOGLError();

		m_dataSize = buffer.SizeBytes();
	}

	void VBO::Destroy()
	{
		if (IsValid())
		{
			s_vboManager->OnDestroyed(*this);

			glDeleteBuffers(1, &m_id);
			CheckOGLError();
		}

		m_id = OpenGL::InvalidID;
		m_dataSize = 0;
	}

	void VBOManager::OnDestroyed(const VBO& vbo)
	{
		auto it = m_listeners.find(vbo.ID());
		if (it != m_listeners.end())
		{
			for (IVBODestroyedListener* listener : it->second)
			{
				listener->OnVBODestroyed(vbo);
			}

			m_listeners.erase(it);
		}
	}

	void VBOManager::RegisterListenerFor(const VBO& vbo, IVBODestroyedListener* listener)
	{
		auto it = m_listeners.find(vbo.ID());
		if (it == m_listeners.end())
		{
			auto it_pair = m_listeners.insert({ vbo.ID(), std::vector<IVBODestroyedListener*>() });
			it = it_pair.first;
		}

		it->second.push_back(listener);
	}

	void VBOManager::UnregisterListenerFor(const VBO& vbo, IVBODestroyedListener* listener)
	{
		auto it = m_listeners.find(vbo.ID());
		if (it != m_listeners.end())
		{
			auto to_erase = std::find(it->second.begin(), it->second.end(), listener);
			if (to_erase != it->second.end())
			{
				it->second.erase(to_erase);
			}
		}
	}

	void VBOManager::UnregisterListenerFromAll(IVBODestroyedListener* listener)
	{
		for (auto& pair : m_listeners)
		{
			std::vector<IVBODestroyedListener*>& listener_list = pair.second;
			listener_list.erase(
				std::remove(listener_list.begin(), listener_list.end(), listener),
				listener_list.end()
			);
		}
	}
}