//
// HitTestSystem.h - A system to do asynchronous hit tests.
// Copyright (C) Sebastian Nordgren 
// September 17th 2017
//

#include "IAsyncHitTest.h"
#include "IDebugPanel.h"

#include "ddc/System.h"

namespace dd
{
	struct HitTestSystem : ddc::System, IAsyncHitTest, IDebugPanel
	{
	public:

		HitTestSystem();

		virtual void Initialize( ddc::EntityLayer& entities ) override;
		virtual void Update( ddc::UpdateData& data ) override;

		virtual HitHandle ScheduleHitTest( const ddm::Ray& ray ) override;
		virtual bool FetchResult( HitHandle handle, HitResult& result ) override;
		virtual void ReleaseResult( HitHandle handle ) override;

	private:

		static const uint MAX_HITS = 8 * 1024;

		struct HitEntry
		{
			HitHandle Handle;
			HitResult Result;

			bool IsPending() const { return Handle.Valid && !Handle.Completed; }
			bool IsCompleted() const { return Handle.Valid && Handle.Completed; }
		};

		HitEntry m_hits[MAX_HITS];
		std::vector<uint> m_free;
		uint m_last { 0 };

		uint m_scheduled { 0 };
		uint m_executed { 0 };
		uint m_released { 0 };
		int m_hitTests { 0 };

		// Inherited via IDebugPanel
		virtual const char* GetDebugTitle() const override { return "Hit Testing"; }
		virtual void DrawDebugInternal() override;
	};
}