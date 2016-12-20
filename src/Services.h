//
// Services.h - A service registry to keep track of global instances of certain types.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "DoubleBuffer.h"

namespace dd
{
	class Services
	{
	public:

		~Services();

		static void Initialize();

		template <typename T>
		static void Register( T& service );

		template <typename T>
		static bool Exists();

		template <typename T>
		static T* GetPtr();

		template <typename T>
		static T& Get();

		template <typename T>
		static void RegisterComponent();

	private:

		static Services* m_instance;
		DenseMap<uint64, void*> m_services;

		Services();

		template <typename T>
		static uint64 GetKey();
	};
}

#include "Services.inl"