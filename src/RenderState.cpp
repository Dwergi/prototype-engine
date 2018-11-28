#include "PCH.h"
#include "RenderState.h"

#include "OpenGL.h"

namespace ddr
{
	const RenderState* RenderState::m_current = nullptr;

	ScopedRenderState::ScopedRenderState( RenderState& state ) :
		m_state( &state )
	{
		m_state->Use( true );
	}

	ScopedRenderState::ScopedRenderState( ScopedRenderState&& other ) :
		m_state( other.m_state )
	{
		
	}

	ScopedRenderState::~ScopedRenderState()
	{
		m_state->Use( false );
	}


	ScopedRenderState RenderState::UseScoped()
	{
		return ScopedRenderState( *this );
	}

	void RenderState::Use( bool use )
	{
		if( use )
		{
			DD_ASSERT( !m_inUse, "Render state already in use!" );

			m_previous = m_current;
			RenderState::Apply( *this );

			m_inUse = true;
		}
		else
		{
			DD_ASSERT( m_inUse, "Render state not in use!" );

			if( m_previous != nullptr )
			{
				RenderState::Apply( *m_previous );
			}
			else
			{
				m_current = nullptr;
			}

			m_inUse = false;
		}
	}

	void RenderState::Apply( const RenderState& state )
	{
		if( m_current == nullptr )
		{
			ApplyBackfaceCulling( state );
			ApplyBlending( state );
			ApplyDepth( state );
		}
		else
		{
			if( m_current->BackfaceCulling != state.BackfaceCulling )
			{
				ApplyBackfaceCulling( state );
			}

			if( m_current->Blending != state.Blending )
			{
				ApplyBlending( state );
			}

			if( m_current->Depth != state.Depth )
			{
				ApplyDepth( state );
			}
		}

		m_current = &state;
	}

	void RenderState::ApplyDepth( const RenderState& state )
	{
		if( state.Depth )
		{
			glEnable( GL_DEPTH_TEST );
			glDepthFunc( GL_GREATER );
			glClipControl( GL_LOWER_LEFT, GL_ZERO_TO_ONE );
		}
		else
		{
			glDisable( GL_DEPTH_TEST );
		}
	}

	void RenderState::ApplyBackfaceCulling( const RenderState& state )
	{
		if( state.BackfaceCulling )
		{
			glEnable( GL_CULL_FACE );
			glFrontFace( GL_CCW );
			glCullFace( GL_BACK );
		}
		else
		{
			glDisable( GL_CULL_FACE );
		}
	}

	void RenderState::ApplyBlending( const RenderState& state )
	{
		if( state.Blending )
		{
			glEnable( GL_BLEND );
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		}
	}
}