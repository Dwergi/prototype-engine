//
// TerrainChunk.h - A chunk of procedural terrain.
// Copyright (C) Sebastian Nordgren 
// April 21st 2016
//

namespace dd
{
	template <int Width, int Height>
	TerrainChunk::IndexBuffer<Width, Height>::IndexBuffer()
		: m_created( false )
	{
		uint index = 0;

		for( uint16 y = 0; y < Height - 1; ++y )
		{
			for( uint16 x = 0; x < Width; ++x )
			{
				bool first = x == 0;
				bool last = x == (Width - 1);

				uint16 next_row = (y + 1) * Width + x;

				if( !last )
				{
					Indices[index] = y * Width + x;
					Indices[index + 1] = next_row;
					Indices[index + 2] = y * Width + x + 1;

					index += 3;
				}

				if( !first )
				{
					Indices[index] = y * Width + x;
					Indices[index + 1] = next_row - 1;
					Indices[index + 2] = next_row;

					index += 3;
				}
			}
		}
	}

	template <int Width, int Height>
	TerrainChunk::IndexBuffer<Width, Height>::~IndexBuffer()
	{
		m_vbo.Destroy();
	}

	template <int Width, int Height>
	void TerrainChunk::IndexBuffer<Width, Height>::Create()
	{
		if( m_created )
			return;

		m_created = true;
		/*m_vbo.Create( 0x8893 );
		m_vbo.Bind();
		m_vbo.SetData( &Indices[0], sizeof( Indices[0] ) * Count );*/
	}

	template <int Width, int Height>
	void TerrainChunk::IndexBuffer<Width, Height>::Bind()
	{
		if( !m_created )
			return;

		m_vbo.Bind();
	}
}