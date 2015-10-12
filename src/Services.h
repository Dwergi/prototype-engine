//
// Services.h - A service registry to keep track of global instances of certain types.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include <typeinfo>
#include <typeindex>

namespace dd
{
	class Services
	{
	public:

		Services() {}
		~Services() {}

		template<typename T>
		void Register( T& service );

		template<typename T>
		bool Exists() const;

		template<typename T>
		T* GetPtr() const;

		template<typename T>
		T& Get() const;

	private:

		DenseMap<const char*, void*> m_services;

		template<typename T>
		const char* GetKey() const;
	};

	template <typename T>
	void dd::Services::Register( T& service )
	{
		const char* key = GetKey<T>();
		m_services.Add( key, &service );
	}

	template <typename T>
	T* dd::Services::GetPtr() const
	{
		const char* key = GetKey<T>();
		void** ptr = m_services.Find( key );

		if( ptr == nullptr )
			return nullptr;

		return reinterpret_cast<T*>(*ptr);
	}

	template <typename T>
	T& dd::Services::Get() const
	{
		T* ptr = GetPtr<T>();

		// just crash
		if( ptr == nullptr )
			(*(int*) nullptr) = 0;

		return *ptr;
	}

	template <typename T>
	const char* dd::Services::GetKey() const
	{
		return std::type_index( typeid(RemovePtr< RemoveQualifiers<T>::type >::type) ).name();
	}

	template <typename T>
	bool dd::Services::Exists() const
	{
		return GetPtr<T>() != nullptr;
	}
}