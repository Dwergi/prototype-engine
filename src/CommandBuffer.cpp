//
// CommandBuffer.h - A buffer of pending commands. 
// Copyright (C) Sebastian Nordgren 
// December 17th 2018
//

#include "PCH.h"
#include "CommandBuffer.h"

#include "MeshRenderCommand.h"
#include "Uniforms.h"

namespace ddr
{
	RenderCommand* CommandBuffer::Access( int index )
	{
		if( index < 0 || index >= m_offsets.size() )
		{
			return nullptr;
		}

		void* ptr = &m_storage[m_offsets[index]];
		return reinterpret_cast<RenderCommand*>(ptr);
	}

	void CommandBuffer::Clear()
	{
		m_storage.clear();
		m_offsets.clear();
	}

	void CommandBuffer::Sort( const ICamera& camera )
	{
		for( int i = 0; i < m_offsets.size(); ++i )
		{
			RenderCommand* cmd = Access( i );
			switch( cmd->Type )
			{
			case CommandType::Mesh:
				static_cast<MeshRenderCommand*>( cmd )->InitializeKey( camera );
				break;
			}
		}

		std::sort( m_offsets.begin(), m_offsets.end(), 
			[this]( size_t a, size_t b )
		{
			auto cmd_a = reinterpret_cast<const RenderCommand*>(&m_storage[a]);
			auto cmd_b = reinterpret_cast<const RenderCommand*>(&m_storage[b]);

			return cmd_a->Key.Key > cmd_b->Key.Key;
		} );
	}

	void CommandBuffer::Dispatch( UniformStorage& uniforms )
	{
		for( int i = 0; i < m_offsets.size(); ++i )
		{
			RenderCommand* cmd = Access( i );
			ScopedRenderState render_state = cmd->RenderState.UseScoped();

			switch( cmd->Type )
			{
			case CommandType::Mesh:
				DD_TODO( "Add material batching here to avoid rebinding uniforms." );
				static_cast<MeshRenderCommand*>( cmd )->Dispatch( uniforms );
				break;
			}
		}
	}
}