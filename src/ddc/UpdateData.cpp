//
// UpdateData.cpp
// Copyright (C) Sebastian Nordgren 
// September 19th 2018
//

#include "PCH.h"
#include "UpdateData.h"

#include "ScratchEntity.h"

namespace ddc
{
	const UpdateBufferView& UpdateData::CreateView(UpdateBuffer& buffer, size_t start, size_t count)
	{
		for (const UpdateBufferView& v : m_views)
		{
			if (v.Name() == buffer.Name())
			{
				return v;
			}
		}

		return m_views.Add(UpdateBufferView(buffer, start, count));
	}

	const UpdateBufferView& UpdateData::Data(const char* name) const
	{
		dd::String32 str_name(name);

		for (const UpdateBufferView& v : m_views)
		{
			if (v.Name() == str_name)
			{
				return v;
			}
		}
		throw new std::exception("No UpdateBufferView found of the given name!");
	}

	void UpdateData::CreateEntity(ddc::ScratchEntity&& scratch)
	{
		m_createdEntities.push_back(new ddc::ScratchEntity(std::move(scratch)));
	}

	void UpdateData::DestroyEntity(ddc::Entity entity)
	{
		m_destroyedEntities.push_back(entity);
	}

	void UpdateData::CommitChanges()
	{
		DD_ASSERT(m_layer != nullptr);

		for (ddc::Entity e : m_destroyedEntities)
		{
			DD_ASSERT(e.Layer() == m_layer);
			e.Destroy();
		}
		m_destroyedEntities.clear();

		for (ddc::ScratchEntity* scratch : m_createdEntities)
		{
			scratch->Instantiate(*m_layer);
			delete scratch;
		}
		m_createdEntities.clear();
	}
}
