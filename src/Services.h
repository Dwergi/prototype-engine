//
// Services.h
// Copyright (C) Sebastian Nordgren 
// January 10th 2019
//

#pragma once

namespace dd
{
	struct Services
	{
		template <typename T>
		static void Register( T* instance )
		{
			Service<T>::Register( instance );
		}
	};

	template <typename T>
	struct Service
	{
		static void Register( T* instance ) 
		{
			DD_ASSERT( s_instance == nullptr, "Instance already registered!" );
			s_instance = instance;
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