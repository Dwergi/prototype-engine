//
// ComponentViews.h
// Copyright (C) Sebastian Nordgren 
// August 23rd 2019
//

#pragma once

#include "ComponentBuffer.h"

namespace ddc
{
	//
	// A read view provide a const view over a range of components.
	//
	template <typename TComponent>
	struct ReadView
	{
		ReadView(const ComponentBuffer& buffer, size_t start, size_t count) :
			m_buffer(buffer),
			m_data(reinterpret_cast<const TComponent*>(m_buffer.Data() + start)),
			m_start(start),
			m_count(count)
		{
			DD_ASSERT(m_start + m_count <= m_buffer.Size());
			DD_ASSERT(m_buffer.Usage() == DataUsage::Read);
			DD_ASSERT(m_buffer.Component() == dd::TypeInfo::GetType<TComponent>());
		}

		bool Optional() const { return m_buffer.Optional(); }
		bool Has(size_t i) const { return m_buffer.Has(m_start + i); }
		size_t Size() const { return m_count; }

		const TComponent* Get(size_t i) const
		{
			DD_ASSERT(i < Size());
			if (Has(i))
			{
				return At(i);
			}
			return nullptr;
		}

		const TComponent& operator[](size_t i) const
		{
			DD_ASSERT(i < Size());
			DD_ASSERT(Has(i));

			return *At(i);
		}

		const TComponent* begin() const
		{
			DD_ASSERT(!Optional(), "Cannot iterate over optional buffers!");
			return At(0);
		}

		const TComponent* end() const
		{
			DD_ASSERT(!Optional(), "Cannot iterate over optional buffers!");
			return At(Size());
		}

	private:

		const ComponentBuffer& m_buffer;
		const TComponent* const m_data { nullptr };
		size_t m_start { 0 };
		size_t m_count { 0 };

		const TComponent* At(size_t i) const
		{
			return m_data + i;
		}
	};

	template <typename TComponent>
	struct WriteView
	{
		WriteView(const ComponentBuffer& buffer, size_t start, size_t count) :
			m_buffer(buffer),
			m_data(reinterpret_cast<TComponent*>(m_buffer.Data() + start)),
			m_start(start),
			m_count(count)
		{
			DD_ASSERT(m_start + m_count <= m_buffer.Size());
			DD_ASSERT(m_buffer.Usage() == DataUsage::Write);
			DD_ASSERT(m_buffer.Component() == dd::TypeInfo::GetType<TComponent>());
		}

		bool Optional() const { return m_buffer.Optional(); }
		bool Has(size_t i) const { return m_buffer.Has(m_start + i); }
		size_t Size() const { return m_count; }

		TComponent* Access(size_t i) const
		{
			DD_ASSERT(i < Size());
			if (Has(i))
			{
				return At(i);
			}
			return nullptr;
		}

		TComponent& operator[](size_t i) const
		{
			DD_ASSERT(i < Size());
			DD_ASSERT(Has(i));
			return *At(i);
		}

		TComponent* begin() const
		{
			DD_ASSERT(!Optional(), "Cannot iterate over optional buffers!");
			return At(0);
		}

		TComponent* end() const
		{
			DD_ASSERT(!Optional(), "Cannot iterate over optional buffers!");
			return At(Size());
		}

	private:

		const ComponentBuffer& m_buffer;
		TComponent* const m_data { nullptr };
		size_t m_start { 0 };
		size_t m_count { 0 };

		TComponent* At(size_t i) const
		{
			return m_data + i;
		}
	};
}