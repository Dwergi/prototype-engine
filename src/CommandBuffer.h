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

	struct CommandBuffer
	{
		void Clear();

		template <typename T>
		void Allocate( T*& out_ptr );

		void Sort( const ICamera& camera );
		void Dispatch();

		std::vector<RenderCommand*>::const_iterator begin() const { return m_commands.begin(); }
		std::vector<RenderCommand*>::const_iterator end() const { return m_commands.end(); }

	private:

		std::vector<byte> m_storage;
		std::vector<RenderCommand*> m_commands;
	};

	template <typename T>
	void CommandBuffer::Allocate( T*& out_ptr )
	{
		static_assert(std::is_base_of<RenderCommand, T>::value);

		size_t offset = m_storage.size();
		m_storage.resize( offset + sizeof( T ) );

		out_ptr = new (&m_storage[ offset ]) T();
		m_commands.push_back( out_ptr );
	}
}