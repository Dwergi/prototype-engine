//
// RenderCommandBuffer.h - A buffer of pending render commands. 
// Copyright (C) Sebastian Nordgren 
// December 17th 2018
//

#pragma once

namespace ddr
{
	struct ICamera;
	struct RenderCommand;
	struct UniformStorage;

	template <typename T>
	struct RenderCommandBuffer
	{
		static_assert(std::is_base_of_v<RenderCommand, T>);

		RenderCommandBuffer() {}
		RenderCommandBuffer( const RenderCommandBuffer& other ) = delete;

		void Clear();

		T& Allocate();

		void Sort();
		void Dispatch( UniformStorage& uniforms );

		int Size() const { return (int) m_offsets.size(); }
		const T& Get(int index) const;
		T& Access(int index);

	private:

		friend struct Iterator;

		std::vector<byte> m_storage;
		std::vector<size_t> m_offsets;
	};

	template <typename T>
	void RenderCommandBuffer<T>::Clear()
	{
		m_storage.clear();
		m_offsets.clear();
	}

	template <typename T>
	void RenderCommandBuffer<T>::Sort()
	{
		std::sort(m_offsets.begin(), m_offsets.end(),
			[this](size_t a, size_t b)
			{
				const T* ptr_a = reinterpret_cast<const T*>(&m_storage[a]);
				const T* ptr_b = reinterpret_cast<const T*>(&m_storage[b]);

				return ptr_a->Key > ptr_b->Key;
			});
	}

	template <typename T>
	T& RenderCommandBuffer<T>::Allocate()
	{
		size_t offset = m_storage.size();
		m_storage.resize( offset + sizeof( T ) );

		T* out_ptr = new (&m_storage[ offset ]) T();
		m_offsets.push_back( offset );

		return *out_ptr;
	}

	template <typename T>
	const T& RenderCommandBuffer<T>::Get(int index) const
	{
		DD_ASSERT(index >= 0 && index < m_offsets.size());
		const void* ptr = &m_storage[m_offsets[index]];
		return *reinterpret_cast<const T*>(ptr);
	}

	template <typename T>
	T& RenderCommandBuffer<T>::Access(int index)
	{
		DD_ASSERT(index >= 0 && index < m_offsets.size());
		void* ptr = &m_storage[m_offsets[index]];
		return *reinterpret_cast<T*>(ptr);
	}
}