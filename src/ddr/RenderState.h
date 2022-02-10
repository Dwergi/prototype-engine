#pragma once

namespace ddr
{
	struct RenderState;

	struct ScopedRenderState
	{
		ScopedRenderState( RenderState& state );
		ScopedRenderState( ScopedRenderState&& state );
		~ScopedRenderState();

	private:

		RenderState* m_state;
	};

	struct RenderState
	{
		union
		{
			struct
			{
				uint Depth : 1;
				uint Blending : 1;
				uint BackfaceCulling : 1;
				uint DepthWrite : 1;
			};

			uint Flags { 0 };
		};

		RenderState();

		void Use( bool use );
		
		[[nodiscard]]
		ScopedRenderState UseScoped();

	private:

		bool m_inUse { false };
		const RenderState* m_previous { nullptr };

		static const RenderState* m_current;

		static void Apply( const RenderState& state );

		static void ApplyDepth( const RenderState& state );
		static void ApplyBackfaceCulling( const RenderState& state );
		static void ApplyBlending( const RenderState& state );
		static void ApplyDepthWrite( const RenderState& state );
	};
}