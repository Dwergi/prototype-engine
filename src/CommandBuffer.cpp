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

	static void SwitchMaterial( MaterialHandle handle, UniformStorage& uniforms )
	{

	}

	void CommandBuffer::Dispatch( UniformStorage& uniforms )
	{
		MaterialHandle current_material;

		for( int i = 0; i < m_offsets.size(); ++i )
		{
			RenderCommand* cmd = Access( i );

			if( cmd->Material != current_material )
			{
				if( current_material.IsValid() )
				{
					Material* old_material = current_material.Access();
					old_material->Unbind( uniforms );
				}

				Material* new_material = cmd->Material.Access();
				new_material->Bind( uniforms );

				current_material = cmd->Material;
			}

			switch( cmd->Type )
			{
			case CommandType::Mesh:
				
				static_cast<MeshRenderCommand*>( cmd )->Dispatch( uniforms );
				break;
			}
		}

		if( current_material.IsValid() )
		{
			Material* old_material = current_material.Access();
			old_material->Unbind( uniforms );
		}
	}
}