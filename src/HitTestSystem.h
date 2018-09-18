//
// HitTestSystem.h - A system to do asynchronous hit tests.
// Copyright (C) Sebastian Nordgren 
// September 17th 2017
//

#include "IAsyncHitTest.h"
#include "IDebugPanel.h"
#include "System.h"

namespace dd
{
	class HitTestSystem : public ddc::System, public IAsyncHitTest, public IDebugPanel
	{
	public:
		
		HitTestSystem();

		virtual void Initialize( ddc::World& world ) override;
		virtual void Update( const ddc::UpdateData& data ) override;

		virtual const HitState& ScheduleHitTest( const Ray& ray, float length ) override;

	private:

		static const uint MAX_HITS = 8 * 1024;

		HitState m_hits[MAX_HITS];
		std::vector<uint> m_free;
		uint m_last { 0 };

		uint m_scheduled { 0 };
		uint m_executed { 0 };
		uint m_cleared { 0 };
		int m_hitTests { 0 };

		// Inherited via IDebugPanel
		virtual const char* GetDebugTitle() const override { return "Hit Testing"; }
		virtual void DrawDebugInternal( const ddc::World & world ) override;
	};
}