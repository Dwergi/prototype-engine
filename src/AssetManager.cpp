
#include "PCH.h"
#include "AssetManager.h"
#include "HandleManager.h"

#include "fmt/core.h"

namespace dd
{
	AssetManager::AssetManager()
	{

	}

	AssetManager::~AssetManager()
	{

	}

	void AssetManager::Register(HandleManagerBase& manager)
	{
		DD_ASSERT(std::find(m_managers.begin(), m_managers.end(), &manager) == m_managers.end(), "Asset manager already registered!");

		m_managers.push_back(&manager);
	}

	void AssetManager::Update()
	{
		for (HandleManagerBase* manager : m_managers)
		{
			manager->Update();
		}
	}

	void AssetManager::DrawDebugInternal()
	{
		std::vector<std::string> instances;

		for (HandleManagerBase* manager : m_managers)
		{
			size_t live_count = manager->LiveCount();

			std::string header = fmt::format("{} ({})", manager->TypeName(), live_count);
			if (ImGui::CollapsingHeader(header.c_str()))
			{
				instances.clear();
				instances.reserve(live_count);
				manager->GetLiveNames(instances);

				for (const std::string& instance : instances)
				{
					ImGui::Text(instance.c_str());
				}
			}
		}
	}
}