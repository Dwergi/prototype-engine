//
// CommandBuffer.h - A buffer of pending commands. 
// Copyright (C) Sebastian Nordgren 
// December 17th 2018
//

#pragma once

namespace ddr
{
	struct ICamera;
	struct RenderCommand;
	struct UniformStorage;

	struct CommandBuffer
	{
		CommandBuffer() {}
		CommandBuffer( const CommandBuffer& other ) = delete;

		void Clear();

		template <typename T>
		void Allocate( T*& out_ptr );

		void Sort( const ICamera& camera );
		void Dispatch( UniformStorage& uniforms );

	private:

		friend struct Iterator;

		std::vector<byte> m_storage;
		std::vector<size_t> m_offsets;

		RenderCommand* Access( int index );
	};

	template <typename T>
	void CommandBuffer::Allocate( T*& out_ptr )
	{
		static_assert(std::is_base_of<RenderCommand, T>::value);

		size_t offset = m_storage.size();
		m_storage.resize( offset + sizeof( T ) );

		out_ptr = new (&m_storage[ offset ]) T();
		m_offsets.push_back( offset );
	}
}