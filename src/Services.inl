//
// Services.inl - A service registry to keep track of global instances of certain types.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

namespace dd
{
	template <typename T>
	void Services::Register( T& service )
	{
		const char* key = GetKey<T>();
		DD_ASSERT( key != nullptr );

		if( key == nullptr )
			return;

		m_instance->m_services.Add( key, &service );
	}

	template <typename T>
	T* Services::GetPtr()
	{
		const char* key = GetKey<T>();

		if( key == nullptr )
			return nullptr;

		void** ptr = m_instance->m_services.Find( key );

		if( ptr == nullptr )
			return nullptr;

		return reinterpret_cast<T*>(*ptr);
	}

	template <typename T>
	T& Services::Get()
	{
		T* ptr = GetPtr<T>();

		// just crash
		if( ptr == nullptr )
			(*(int*) nullptr) = 0;

		return *ptr;
	}

	template <typename T>
	const char* Services::GetKey()
	{
		const TypeInfo* typeInfo = GET_TYPE( dd::RemovePtr<T>::type );

		DD_ASSERT( typeInfo != nullptr );

		return typeInfo->Name().c_str();
	}

	template <typename T>
	bool Services::Exists()
	{
		return GetPtr<T>() != nullptr;
	}

	template<typename T>
	void Services::RegisterComponent()
	{
		DoubleBuffer<typename T::Pool>* double_buffer = new DoubleBuffer<typename T::Pool>( new typename T::Pool(), new typename T::Pool() );
		Services::Register( *double_buffer );
	}

	template<typename T>
	typename T::Pool& Services::GetWritePool()
	{
		return Services::Get<DoubleBuffer<typename T::Pool>>().GetWrite();
	}

	template<typename T>
	typename const T::Pool& Services::GetReadPool()
	{
		return Services::Get<DoubleBuffer<typename T::Pool>>().GetRead();
	}

	template<typename T>
	DoubleBuffer<typename T::Pool>& Services::GetDoubleBuffer()
	{
		DoubleBuffer<typename T::Pool>& buffer = Services::Get<DoubleBuffer<typename T::Pool>>();

		return buffer;
	}
}