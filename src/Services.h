//
// Services.h - A service registry to keep track of global instances of certain types.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

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
	void Services::Register( T& service )
	{
		const char* key = GetKey<T>();
		DD_ASSERT( key != nullptr );

		if( key == nullptr )
			return;

		m_services.Add( key, &service );
	}

	template <typename T>
	T* Services::GetPtr() const
	{
		const char* key = GetKey<T>();

		if( key == nullptr )
			return nullptr;

		void** ptr = m_services.Find( key );

		if( ptr == nullptr )
			return nullptr;

		return reinterpret_cast<T*>(*ptr);
	}

	template <typename T>
	T& Services::Get() const
	{
		T* ptr = GetPtr<T>();

		// just crash
		if( ptr == nullptr )
			(*(int*) nullptr) = 0;

		return *ptr;
	}

	template <typename T>
	const char* Services::GetKey() const
	{
		const TypeInfo* typeInfo = GET_TYPE( dd::RemovePtr<T>::type );

		DD_ASSERT( typeInfo != nullptr );

		return typeInfo->Name().c_str();
	}

	template <typename T>
	bool Services::Exists() const
	{
		return GetPtr<T>() != nullptr;
	}
}