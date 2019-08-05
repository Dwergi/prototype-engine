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
	static void AddValue( const String& name, Variable& var )
	{
		if( var.Type() == DD_FIND_TYPE( float ) )
		{
			ImGui::DragFloat( name.c_str(), &var.GetValue<float>(), 0.001f );
		}
		else if( var.Type() == DD_FIND_TYPE( int ) )
		{
			ImGui::DragInt( name.c_str(), &var.GetValue<int>() );
		}
		else if( var.Type() == DD_FIND_TYPE( bool ) )
		{
			ImGui::Checkbox( name.c_str(), &var.GetValue<bool>() );
		}
	}

	static void AddEnum( const String& name, Variable& var )
	{
		int& current = var.GetValue<int>();

		const Vector<EnumOption>& options = var.Type()->GetEnumOptions();

		String32 selected;
		for( const EnumOption& o : options )
		{
			if( current == o.Value )
			{
				selected = o.Name;
			}
		}

		if( ImGui::BeginCombo( name.c_str(), selected.c_str() ) )
		{
			for( const EnumOption& o : options )
			{
				bool is_selected = (o.Value == current);
				if( ImGui::Selectable( o.Name.c_str(), &is_selected ) )
				{
					current = o.Value;
				}
			}

			ImGui::EndCombo();
		}
	}

	static void AddVariable( const String& name, Variable& var );

	static void AddContainer( const String& name, Variable& var )
	{
		if( ImGui::TreeNodeEx( name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			const TypeInfo* typeInfo = var.Type();

			uint size = typeInfo->ContainerSize( var.Data() );
			for( uint i = 0; i < size; ++i )
			{
				void* item = typeInfo->ElementAt( var.Data(), i );

				Variable item_var( typeInfo->ContainedType(), item );

				char buffer[ 8 ];
				_itoa_s( i, buffer, 10 );

				String16 item_name;
				item_name += "[";
				item_name += buffer;
				item_name += "]";

				AddVariable( name, item_var );
			}
			ImGui::TreePop();
		}
	}

	static void AddClass( const String& name, Variable& var )
	{
		if( ImGui::TreeNodeEx( name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_CollapsingHeader ) )
		{
			for( const dd::Member& member : var.Type()->Members() )
			{
				AddVariable( member.Name(), Variable( var, member ) );
			}

			ImGui::TreePop();
		}
	}

	static void AddVariable( const String& name, Variable& var )
	{
		switch( var.Type()->GetTypeKind() )
		{
		case dd::TypeKind::POD:
			AddValue( name, var );
			break;

		case dd::TypeKind::Class:
			AddClass( name, var );
			break;

		case dd::TypeKind::Enum:
			AddEnum( name, var );
			break;

		case dd::TypeKind::Container:
			AddContainer( name, var );
			break;
		}
	}
	
	static dd::Service<dd::Input> s_input;

	EntityVisualizer::EntityVisualizer()
	{
		s_input->RegisterActionHandler(InputAction::TOGGLE_ENTITY_DATA, [this]()
			{
				SetDebugPanelOpen(!IsDebugPanelOpen());
			});
	}

	void EntityVisualizer::DrawDebugInternal( ddc::EntitySpace& space )
	{
		ImGui::SetWindowSize( ImVec2( 300, 500 ), ImGuiCond_Once );

		dd::Array<dd::ComponentID, 1> components;
		ddc::TagBits tags;
		tags.set( (int) ddc::Tag::Selected, true );

		std::vector<ddc::Entity> entities;
		space.FindAllWith( components, tags, entities );

		if( entities.size() > 0 )
		{
			ddc::Entity entity = entities.front();

			for( dd::ComponentID id = 0; id < ddc::MAX_COMPONENTS; ++id )
			{
				void* cmp_data = space.AccessComponent( entity, id );
				if( cmp_data != nullptr )
				{
					const dd::TypeInfo* typeInfo = dd::TypeInfo::GetComponent( id );

					AddClass( typeInfo->Name(), Variable( typeInfo, cmp_data ) );
				}
			}
		}
	}
}