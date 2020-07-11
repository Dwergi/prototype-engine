#pragma once

#include "IDebugPanel.h"

namespace dd
{
	struct HandleManagerBase;
	
	struct AssetManager : dd::IDebugPanel
	{
		AssetManager();
		~AssetManager();

		AssetManager(const AssetManager&) = delete;

		void Register(HandleManagerBase& manager);

		void Update();

		void Shutdown();

	private:

		virtual void DrawDebugInternal() override;
		virtual const char* GetDebugTitle() const override { return "Assets"; }

		std::vector<HandleManagerBase*> m_managers;
	};
}