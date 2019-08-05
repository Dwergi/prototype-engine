#include "PCH.h"
#include "SystemManager.h"

#include "JobSystem.h"
#include "System.h"
#include "SystemsSorting.h"
#include "UpdateData.h"

namespace ddc
{
	static dd::Service<dd::JobSystem> s_jobsystem;

	static void WaitForAllFutures(std::vector<std::shared_future<void>>& futures)
	{
		DD_TODO("Add a way to check for asserts here.");

		size_t ready = 0;
		while (ready < futures.size())
		{
			ready = 0;
			for (std::shared_future<void>& f : futures)
			{
				std::future_status s = f.wait_for(std::chrono::microseconds(1));
				if (s == std::future_status::ready)
				{
					++ready;
				}
			}
		}
	}

	void SystemManager::Initialize()
	{
		DD_ASSERT(m_systems.size() == m_orderedSystems.size(), "System mismatch, Initialize not called!");

		for (System* system : m_systems)
		{
			system->Initialize(*this);
		}

		ddc::OrderSystemsByDependencies(dd::Span<System*>(m_systems), m_orderedSystems);
	}

	void SystemManager::Shutdown()
	{
		for (System* system : m_systems)
		{
			system->Shutdown(*this);
		}
	}

	void SystemManager::Update(float delta_t)
	{
		UpdateSystemsWithTreeScheduling(delta_t);

		std::vector<std::shared_future<void>> updates;
		updates.reserve(m_orderedSystems.size());

		for (SystemNode& s : m_orderedSystems)
		{
			updates.push_back(s.m_update);
		}

		WaitForAllFutures(updates);

		for (SystemNode& s : m_orderedSystems)
		{
			s.m_update = std::shared_future<void>();
		}
	}

	void SystemManager::RegisterSystem(System& system)
	{
		m_systems.push_back(&system);
	}

	void SystemManager::UpdateSystem(System* system, std::vector<std::shared_future<void>> dependencies, float delta_t)
	{
		DD_ASSERT(system != nullptr);

		WaitForAllFutures(dependencies);

		ddc::UpdateData update(*this, delta_t);

		// get names
		dd::Array<dd::String16, 8> names;
		for (const DataRequest* req : system->GetRequests())
		{
			bool found = false;
			for (const dd::String& name : names)
			{
				if (name == req->Name())
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				names.Add(req->Name());
			}
		}

		// for each name filter requests
		for (const dd::String& name : names)
		{
			dd::Array<dd::ComponentID, MAX_COMPONENTS> required;
			dd::Array<const DataRequest*, MAX_COMPONENTS> requests;
			for (const DataRequest* req : system->GetRequests())
			{
				if (name == req->Name())
				{
					if (!req->Optional())
					{
						required.Add(req->Component().ComponentID());
					}

					requests.Add(req);
				}
			}

			TagBits tags = system->GetRequiredTags(name.c_str());

			// find entities with requirements
			std::vector<Entity> entities;
			FindAllWith(required, tags, entities);

			update.AddData(entities, requests, name.c_str());
		}

		system->Update(update);

		update.Commit();
	}

	std::vector<std::shared_future<void>> GetFutures(SystemNode& s, std::vector<SystemNode>& systems)
	{
		std::vector<std::shared_future<void>> futures;

		for (SystemNode::Edge& e : s.m_in)
		{
			SystemNode& dep = systems[e.m_from];
			futures.push_back(dep.m_update);
		}

		return futures;
	}

	void SystemManager::UpdateSystemsWithTreeScheduling(float delta_t)
	{
		for (SystemNode& s : m_orderedSystems)
		{
			DD_ASSERT(s.m_system != nullptr);

			System* system = s.m_system;
			auto futures = GetFutures(s, m_orderedSystems);

			s.m_update = s_jobsystem->Schedule([this, system, futures, delta_t]()
				{
					UpdateSystem(system, futures, delta_t);
				}).share();
		}
	}

	namespace
	{
		const float NODE_WIDTH = 100;
		const float NODE_HEIGHT = 25;
		const float NODE_WIDTH_OFFSET = 30;
		const float NODE_HEIGHT_OFFSET = 20;

		ImVec2 GetNodePosition(int column, int row)
		{
			ImVec2 window_pos = ImGui::GetWindowPos();
			return ImVec2(window_pos.x + column * (NODE_WIDTH + NODE_WIDTH_OFFSET) + NODE_WIDTH_OFFSET,
				window_pos.y + ImGui::GetFrameHeightWithSpacing() + row * (NODE_HEIGHT + NODE_HEIGHT_OFFSET) + NODE_HEIGHT_OFFSET);
		}

		struct NodeEntry
		{
			SystemNode* Node { nullptr };
			int Column { -1 };
			int Row { -1 };
		};

		const NodeEntry* FindNodeEntry(const std::vector<NodeEntry>& entries, const SystemNode& node)
		{
			for (const NodeEntry& entry : entries)
			{
				if (entry.Node == &node)
				{
					return &entry;
				}
			}

			return nullptr;
		}
	}

	void SystemManager::DrawDebugInternal(ddc::EntitySpace& entities)
	{
		ImGui::Checkbox("Systems Graph", &m_drawSystemsGraph);

		if (!m_drawSystemsGraph)
			return;

		ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);
		ImGui::Begin("Systems", &m_drawSystemsGraph);

		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		// systems are ordered so that all systems with 0 dependencies come before all systems with any dependencies

		std::vector<NodeEntry> entries;

		int row = 0;

		for (SystemNode& node : m_orderedSystems)
		{
			NodeEntry entry;
			entry.Node = &node;

			if (node.m_in.empty())
			{
				entry.Column = 0;
				entry.Row = row;

				++row;
			}

			entries.push_back(entry);
		}

		// assign columns to entries with dependencies
		bool did_work;
		do
		{
			did_work = false;

			for (NodeEntry& entry : entries)
			{
				if (entry.Column != -1)
					continue;

				int column = -1;
				for (const SystemNode::Edge& edge : entry.Node->m_in)
				{
					const NodeEntry* entry = FindNodeEntry(entries, m_orderedSystems[edge.m_from]);
					if (entry->Column == -1)
					{
						column = -1;
						break;
					}

					column = std::max(column, entry->Column);
				}

				if (column != -1)
				{
					entry.Column = column + 1;
					did_work = true;
				}
			}
		} while (did_work);

		// assign rows to entries with dependencies
		for (NodeEntry& entry : entries)
		{
			if (entry.Row != -1)
				continue;

			int row = -1;
			for (const NodeEntry& e : entries)
			{
				if (e.Node == entry.Node)
					continue;

				if (e.Column != entry.Column)
					continue;

				row = std::max(row, e.Row);
			}

			entry.Row = row + 1;
		}

		// and finally draw nodes
		for (NodeEntry& entry : entries)
		{
			ImVec2 a = GetNodePosition(entry.Column, entry.Row);
			ImVec2 b(a.x + NODE_WIDTH, a.y + NODE_HEIGHT);
			draw_list->AddRectFilled(a, b, ImColor(0, 0, 0, 128), 3.0f);
			draw_list->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(a.x + 5, a.y + 5), ImColor(255, 255, 255, 255),
				entry.Node->m_system->GetName());

			// draw connections
			for (const SystemNode::Edge& edge : entry.Node->m_in)
			{
				const NodeEntry* dep = FindNodeEntry(entries, m_orderedSystems[edge.m_from]);

				ImVec2 out_pos = GetNodePosition(dep->Column, dep->Row);
				out_pos.x += NODE_WIDTH;
				out_pos.y += NODE_HEIGHT / 2;

				ImVec2 in_pos = GetNodePosition(entry.Column, entry.Row);
				in_pos.y += NODE_HEIGHT / 2;

				ImVec2 cp1 = out_pos;
				cp1.x += NODE_WIDTH_OFFSET / 3;

				ImVec2 cp2 = in_pos;
				cp2.x -= NODE_WIDTH_OFFSET / 3;

				draw_list->AddBezierCurve(out_pos, cp1, cp2, in_pos, ImColor(255, 255, 255, 255), 1);
			}
		}

		ImGui::End();
	}
}