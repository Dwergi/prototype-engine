//
// Services.h
// Copyright (C) Sebastian Nordgren 
// January 10th 2019
//

#pragma once

namespace ddc
{
	struct World;
}

namespace dd
{
	struct Services
	{
		template <typename T>
		static T& Register(T* instance)
		{
			DD_ASSERT(instance != nullptr);
			return Register(*instance);
		}

		template <typename T>
		static T& Register(T& instance)
		{
			static_assert(!std::is_same_v<T, ddc::World>);
			Service<T>::Register(instance);

			s_unregisterFuncs.push_back(&Service<T>::Unregister);
			return instance;
		}

		template <typename T>
		static void Unregister()
		{
			Service<T>::Unregister();
		}

		template <typename TInterface, typename TImpl>
		static TInterface& RegisterInterface(TImpl* instance)
		{
			DD_ASSERT(instance != nullptr);
			return RegisterInterface<TInterface>(*instance);
		}

		template <typename TInterface, typename TImpl>
		static TInterface& RegisterInterface(TImpl& instance)
		{
			static_assert(!std::is_same_v<TImpl, ddc::World>);
			static_assert(std::is_base_of_v<TInterface, TImpl>);
			Service<TInterface>::Register(instance);

			return (TInterface&) instance;
		}

		static void UnregisterAll()
		{
			for (void(*unregister_fn)() : s_unregisterFuncs)
			{
				unregister_fn();
			}

			s_unregisterFuncs.clear();
		}

	private:

		static std::vector<void(*)()> s_unregisterFuncs;
	};

	template <typename T>
	struct Service
	{
		static void Register( T& instance ) 
		{
			DD_ASSERT( s_instance == nullptr, "Instance already registered!" );
			s_instance = &instance;
		}

		static void Unregister()
		{
			DD_ASSERT(s_instance != nullptr, "Instance not registered!");
			delete s_instance;
			s_instance = nullptr;
		}

		static T& Get()
		{
			DD_ASSERT( s_instance != nullptr, "Instance not registered yet!" );
			return *s_instance;
		}

		operator T&() { return Get(); }
		operator T*() { return &Get(); }
		T& operator*() { return Get(); }
		T* operator->() { return &Get(); }

	private:
		static T* s_instance;
	};
}

template <typename T> T* dd::Service<T>::s_instance = nullptr;