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

	void VBO::SetData(const dd::IBuffer& buffer)
	{
		DD_ASSERT(IsValid());
		DD_ASSERT(buffer.IsValid() && buffer.SizeBytes() > 0);
		
		// if the sizes match we can just write the data now
		if (m_dataSize == buffer.SizeBytes())
		{
			OpenGL::BufferSubData(*this, buffer);
			return;
		}

		// haven't allocated any storage yet
		if (m_dataSize == 0)
		{
			OpenGL::BufferStorage(*this, buffer);
			m_dataSize = buffer.SizeBytes();
			return;
		}
		
		// if we need more data, we need to recreate it
		if (m_dataSize < buffer.SizeBytes())
		{
			Recreate(buffer);
		}
	}

	void VBO::Recreate(const dd::IBuffer& buffer)
	{
		DD_ASSERT(IsValid());
		
		uint old_id = m_id;

		OpenGL::DeleteBuffer(*this);

		m_id = OpenGL::CreateBuffer();
		glObjectLabel(GL_BUFFER, m_id, (GLsizei) m_name.length(), m_name.c_str());

		OpenGL::BufferStorage(*this, buffer);

		m_dataSize = buffer.SizeBytes();

		s_vboManager->OnRenamed(*this, old_id);
	}

	void VBO::Create(std::string_view name)
	{
		DD_ASSERT(!IsValid());

		m_name = name;

		m_id = OpenGL::CreateBuffer();
		glObjectLabel(GL_BUFFER, m_id, (GLsizei) m_name.length(), m_name.c_str());
	}

	void VBO::Destroy()
	{
		if (IsValid())
		{
			s_vboManager->OnDestroyed(*this);

			OpenGL::DeleteBuffer(*this);
		}

		m_id = OpenGL::InvalidID;
		m_dataSize = 0;
	}

	void VBOManager::OnRenamed(const VBO& vbo, uint old_id)
	{
		auto it = m_listeners.find(old_id);
		if (it != m_listeners.end())
		{
			for (IVBOListener* listener : it->second)
			{
				listener->OnVBORenamed(vbo, old_id);
			}

			// move listeners over to the new ID
			m_listeners.insert({ vbo.ID(), it->second});

			// remove the old ID
			m_listeners.erase(it);
		}
	}

	void VBOManager::OnDestroyed(const VBO& vbo)
	{
		auto it = m_listeners.find(vbo.ID());
		if (it != m_listeners.end())
		{
			for (IVBOListener* listener : it->second)
			{
				listener->OnVBODestroyed(vbo);
			}

			m_listeners.erase(it);
		}
	}

	void VBOManager::RegisterListenerFor(const VBO& vbo, IVBOListener* listener)
	{
		auto it = m_listeners.find(vbo.ID());
		if (it == m_listeners.end())
		{
			auto it_pair = m_listeners.insert({ vbo.ID(), std::vector<IVBOListener*>() });
			it = it_pair.first;
		}

		it->second.push_back(listener);
	}

	void VBOManager::UnregisterListenerFor(const VBO& vbo, IVBOListener* listener)
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

	void VBOManager::UnregisterListenerFromAll(IVBOListener* listener)
	{
		for (auto& pair : m_listeners)
		{
			std::vector<IVBOListener*>& listener_list = pair.second;
			listener_list.erase(
				std::remove(listener_list.begin(), listener_list.end(), listener),
				listener_list.end()
			);
		}
	}
}