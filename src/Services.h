#ifndef _SERVICES_H
#define _SERVICES_H

#include <unordered_map>
#include <cassert>
#include <typeinfo>
#include <typeindex>

class Services
{
public:

	Services()
	{

	}

	~Services()
	{
		
	}

	template<typename T>
	void Register( T* service  )
	{
		assert( service != nullptr );

		m_services.insert( std::make_pair( std::type_index( typeid( T ) ), service ) );
	}

	template<typename T>
	T* Get()
	{
		auto it = m_services.find( std::type_index( typeid( T ) ) );

		if( it == it.end() )
			return nullptr;

		return reinterpret_cast<T*>( it->second );
	}

private:

	std::unordered_map<std::type_index, void*> m_services;

};

#endif //_SERVICES_H