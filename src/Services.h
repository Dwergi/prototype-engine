#pragma once

#include <unordered_map>
#include <cassert>
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
		void Register( T* service  )
		{
			ASSERT( service != nullptr );

			m_services.insert( std::make_pair( std::type_index( typeid( T ) ), service ) );
		}

		template<typename T>
		bool Exists()
		{
			return GetPtr() != nullptr;
		}

		template<typename T>
		T* GetPtr() const
		{
			auto it = m_services.find( std::type_index( typeid( T ) ) );

			if( it == m_services.end() )
				return nullptr;

			return reinterpret_cast<T*>( it->second );
		}

		template<typename T>
		T& Get() const
		{
			T* ret = GetPtr<T>();

			// just crash
			if( ret == nullptr )
				(*(int*) ret) = 0;

			return *ret;
		}

	private:

		std::unordered_map<std::type_index, void*> m_services;
	};
}