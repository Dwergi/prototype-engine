//
// EntityVisualizer.cpp - Visualizes the data of the currently selected entity in ImGui.
// Copyright (C) Sebastian Nordgren 
// October 7th 2018
//

#include "PCH.h"
#include "EntityVisualizer.h"

#include "Input.h"

#include <imgui/imgui.h>

namespace dd
{
	static void AddValue(const String& name, Variable& var)
	{
		if (var.Type() == DD_FIND_TYPE(float))
		{
			ImGui::DragFloat(name.c_str(), &var.AccessValue<float>(), 0.001f);
		}
		else if (var.Type() == DD_FIND_TYPE(int))
		{
			ImGui::DragInt(name.c_str(), &var.AccessValue<int>());
		}
		else if (var.Type() == DD_FIND_TYPE(bool))
		{
			ImGui::Checkbox(name.c_str(), &var.AccessValue<bool>());
		}
	}

	static void AddEnum(const String& name, Variable& var)
	{
		int& current = var.AccessValue<int>();

		const Vector<EnumOption>& options = var.Type()->GetEnumOptions();

		String32 selected;
		for (const EnumOption& o : options)
		{
			if (current == o.Value)
			{
				selected = o.Name;
			}
		}

		if (ImGui::BeginCombo(name.c_str(), selected.c_str()))
		{
			for (const EnumOption& o : options)
			{
				bool is_selected = (o.Value == current);
				if (ImGui::Selectable(o.Name.c_str(), &is_selected))
				{
					current = o.Value;
				}
			}

			ImGui::EndCombo();
		}
	}

	static void AddVariable(const String& name, Variable& var);

	static void AddContainer(const String& name, Variable& var)
	{
		if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			const TypeInfo* typeInfo = var.Type();

			uint size = typeInfo->ContainerSize(var.Data());
			for (uint i = 0; i < size; ++i)
			{
				void* item = typeInfo->ElementAt(var.Data(), i);

				Variable item_var(typeInfo->ContainedType(), item);

				char buffer[8];
				_itoa_s(i, buffer, 10);

				String16 item_name;
				item_name += "[";
				item_name += buffer;
				item_name += "]";

				AddVariable(name, item_var);
			}
		}
	}

	static void AddClass(const String& name, Variable& var)
	{
		if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (const dd::Member& member : var.Type()->Members())
			{
				AddVariable(member.Name(), Variable(var, member));
			}
		}
	}

	static void AddVariable(const String& name, Variable& var)
	{
		switch (var.Type()->GetTypeKind())
		{
			case dd::TypeKind::POD:
				AddValue(name, var);
				break;

			case dd::TypeKind::Class:
				AddClass(name, var);
				break;

			case dd::TypeKind::Enum:
				AddEnum(name, var);
				break;

			case dd::TypeKind::Container:
				AddContainer(name, var);
				break;
		}
	}

	static dd::Service<dd::Input> s_input;

	EntityVisualizer::EntityVisualizer() :
		ddc::System("Entity Visualizer")
	{
		s_input->AddHandler(InputAction::TOGGLE_ENTITY_DATA, InputType::Release,
			[this]()
			{
				SetDebugPanelOpen(!IsDebugPanelOpen());
			});

		RequireTag(ddc::Tag::Selected);
	}

	void EntityVisualizer::Update(ddc::UpdateData& update_data)
	{
		if (update_data.Data().Entities().Size() > 0)
		{
			m_selected = update_data.Data().Entities()[0];
		}
	}

	void EntityVisualizer::DrawDebugInternal()
	{
		ImGui::SetWindowSize(ImVec2(300, 500), ImGuiCond_Once);

		if (m_selected.IsValid())
		{
			ImGui::Text("Entity - ID: %d, Version: %d", m_selected.ID, m_selected.Version);
			ImGui::Text("Components: %d", m_selected.Components());

			const ddc::EntityLayer* layer = m_selected.Layer();

			for (int i = 0; i < m_selected.Components(); ++i)
			{
				dd::ComponentID cmp_id = layer->GetNthComponentID(m_selected, i);

				void* cmp_data = layer->AccessComponent(m_selected, cmp_id);
				DD_ASSERT(cmp_data != nullptr);

				const dd::TypeInfo* typeInfo = dd::TypeInfo::GetComponent(cmp_id);

				AddClass(typeInfo->Name(), Variable(typeInfo, cmp_data));
			}
		}
	}
}