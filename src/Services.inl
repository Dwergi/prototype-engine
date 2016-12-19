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
		uint64 key = GetKey<T>();
		DD_ASSERT( key != 0 );

		if( key == 0 )
			return;

		m_instance->m_services.Add( key, &service );
	}

	template <typename T>
	T* Services::GetPtr()
	{
		uint64 key = GetKey<T>();

		if( key == 0 )
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
	uint64 Services::GetKey()
	{
		const TypeInfo* typeInfo = GET_TYPE( T );

		if( !typeInfo->IsRegistered() )
			return 0;

		return reinterpret_cast<uint64>(typeInfo);
	}

	template <typename T>
	bool Services::Exists()
	{
		return GetPtr<T>() != nullptr;
	}

	template <typename T>
	void Services::RegisterComponent()
	{
		const TypeInfo* typeInfo = GET_TYPE( T );
		String128 typeName( typeInfo->Name().c_str() );
		typeName += "Pool";

		TypeInfo::RegisterType<T::Pool>( typeName.c_str() );

		String128 doubleBufferName( "dd::DoubleBuffer<" );
		doubleBufferName += typeName;
		doubleBufferName += ">";
		TypeInfo::RegisterType<DoubleBuffer<typename T::Pool>>( doubleBufferName.c_str() );

		DoubleBuffer<typename T::Pool>* double_buffer = new DoubleBuffer<typename T::Pool>( new typename T::Pool(), new typename T::Pool() );
		Services::Register( *double_buffer );
	}

	template <typename T>
	typename T::Pool& Services::GetWritePool()
	{
		return Services::Get<DoubleBuffer<typename T::Pool>>().GetWrite();
	}

	template <typename T>
	typename const T::Pool& Services::GetReadPool()
	{
		return Services::Get<DoubleBuffer<typename T::Pool>>().GetRead();
	}

	template <typename T>
	DoubleBuffer<typename T::Pool>& Services::GetDoubleBuffer()
	{
		return Services::Get<DoubleBuffer<typename T::Pool>>();
	}
}