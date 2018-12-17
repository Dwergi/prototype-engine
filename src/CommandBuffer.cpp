//
// CommandBuffer.h - A buffer of pending commands. 
// Copyright (C) Sebastian Nordgren 
// December 17th 2018
//

#include "PCH.h"
#include "CommandBuffer.h"

#include "MeshRenderCommand.h"

#include <algorithm>

namespace ddr
{
	void CommandBuffer::Clear()
	{
		m_storage.clear();
		m_commands.clear();
	}

	void CommandBuffer::Sort()
	{
		std::sort( m_commands.begin(), m_commands.end(), 
			[]( const RenderCommand* a, const RenderCommand* b )
		{
			return a->Key.Key < b->Key.Key;
		} );
	}

	void CommandBuffer::Dispatch()
	{
		for( const RenderCommand* cmd : m_commands )
		{
			switch( cmd->Type )
			{
			case CommandType::Mesh:
				static_cast<const MeshRenderCommand*>( cmd )->Dispatch();
				break;
			}
		}
	}
}