//
// System.h - Base class for systems.
// Copyright (C) Sebastian Nordgren 
// September 18th 2018
//

#pragma once

#include "ddc/DataRequest.h"
#include "ddc/UpdateData.h"

namespace ddc
{
	struct EntityLayer;

	struct System
	{
		System(const char* name);

		virtual void Initialize(ddc::EntityLayer& layer) {}
		virtual void Update(ddc::UpdateData& update_data) = 0;
		virtual void Shutdown(ddc::EntityLayer& layer) {}

		//
		// Allow the system to be updated for the given layer. May be called multiple times. 
		// If never called, then the system will allow all layers.
		//
		void EnableForLayer(const ddc::EntityLayer& layer);
		bool IsEnabledForLayer(const ddc::EntityLayer& layer) const;

		// 
		// Master flag for enabling a system. (default = true)
		// 
		void SetEnabled(bool enabled) { m_enabled = enabled; }
		bool IsEnabled() const { return m_enabled; }

		void DependsOn(const System& system) { DD_ASSERT(&system != this); m_dependencies.Add(&system); }

		const dd::IArray<const System*>& GetDependencies() const { return m_dependencies; }
		const char* GetName() const { return m_name.c_str(); }

		int MaxPartitions() const { return m_partitions; }

	protected:

		template <typename T>
		void RequireType(const char* name = nullptr)
		{
			static_assert(std::is_trivially_constructible_v<T>, "Must pass in ddc::EntityType to RequireType.");
			RequireTypeHelper(T {}, name);
		}

		template <typename T>
		void RequireRead(const char* name = nullptr)
		{
			dd::ComponentRegistration<T>::Register();

			ddc::UpdateBuffer& buffer = CreateBuffer(name);
			buffer.RequestData(new ReadRequired<T>());
		}

		template <typename T>
		void RequireWrite(const char* name = nullptr)
		{
			dd::ComponentRegistration<T>::Register();

			ddc::UpdateBuffer& buffer = CreateBuffer(name);
			buffer.RequestData(new WriteRequired<T>());
		}

		template <typename T>
		void OptionalRead(const char* name = nullptr)
		{
			dd::ComponentRegistration<T>::Register();

			ddc::UpdateBuffer& buffer = CreateBuffer(name);
			buffer.RequestData(new ReadOptional<T>());
		}

		template <typename T>
		void OptionalWrite(const char* name = nullptr)
		{
			dd::ComponentRegistration<T>::Register();

			ddc::UpdateBuffer& buffer = CreateBuffer(name);
			buffer.RequestData(new WriteOptional<T>());
		}

		void RequireTag(Tag tag, const char* name = nullptr);

		void SetPartitions(int count)
		{
			DD_ASSERT(count > 0 && count <= MAX_PARTITIONS);
			m_partitions = count;
		}

	private:
		friend struct SystemsManager;

		dd::Array<const System*, 8> m_dependencies;
		dd::String32 m_name;

		bool m_enabled { true };
		std::vector<const ddc::EntityLayer*> m_layers;

		const static int MAX_PARTITIONS = 8;
		int m_partitions { MAX_PARTITIONS };

		void CommitChanges(ddc::UpdateData& data);

		ddc::UpdateBuffer& CreateBuffer(const char* name);

		ddc::UpdateData CreateUpdateData(ddc::EntityLayer& layer, float delta_t) const;

		dd::Array<UpdateBuffer, 8> m_updateBuffers;

		template <typename... TComponents>
		void RequireTypeHelper(const ddc::EntityType<TComponents...>& type, const char* name)
		{
			(RequireSingleType<TComponents>(name), ...);
		}

		template <typename TComponent>
		void RequireSingleType(const char* name)
		{
			if constexpr (std::is_const_v<TComponent>)
			{
				RequireRead<std::remove_const_t<TComponent>>(name);
			}
			else
			{
				RequireWrite<TComponent>(name);
			}
		}
	};
}