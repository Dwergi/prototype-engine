//
// Handle.h - A generic, simple handle system that stores instances and uses indices to reference them.
// Copyright (C) Sebastian Nordgren 
// November 22nd 2018
//

#pragma once

namespace dd
{
#define DD_HANDLE_MANAGER( TypeName ) dd::HandleManager<TypeName>* dd::HandleManager<TypeName>::s_singleton;

	template <typename T> struct Handle;

	struct HandleTarget
	{
		//
		// The name of this instance.
		//
		const dd::String& Name() const { return m_name; }

		//
		// Set the name of this instance.
		//
		void SetName( const char* name ) { m_name = name; }

	protected:
		dd::String64 m_name;
	};

	//
	// Handle container that cares rather little about the internals of your type, as long as it derives from this.
	// Stick DD_HANDLE_TARGET( YourType ) into your .cpp as well.
	//
	template <typename T>
	struct HandleManager
	{
		static_assert(std::is_base_of<HandleTarget, T>::value);

		//
		// Find a handle to a mesh with the given name.
		//
		Handle<T> Find( const char* name ) const;
		
		//
		// Create (or retrieve) a handle to a mesh with the given name.
		//
		Handle<T> Create( const char* name );

		//
		// Destroy the mesh associated with the given handle. 
		// Returns true if something was destroyed, false if nothing of the given name was found.
		//
		void Destroy( Handle<T> handle );
		
		//
		// Get the mesh instance associated with the given handle.
		// Returns null if the handle does not reference a mesh that still exists.
		//
		const T* Get( Handle<T> handle ) const;

		//
		// Access the mesh instance associated with the given handle.
		// Returns null if the handle does not reference a mesh that still exists.
		//
		T* Access( Handle<T> handle ) const;

		//
		// Is the given handle pointing a valid, live object.
		//
		bool IsAlive( Handle<T> h ) const;

		//
		// Get the number of valid handles.
		//
		size_t Count() const;

		//
		// Access the handle at index i.
		//
		T* AccessAt( size_t i ) const;

		//
		// Update the handle manager, creating/destroying things that were created last frame.
		//
		void Update();

		static void RegisterSingleton( HandleManager<T>* singleton ) { s_singleton = singleton; }
		static HandleManager<T>* Instance() { return s_singleton; }

	protected:

		struct HandleEntry
		{
			union
			{
				struct
				{
					byte Alive : 1;
					byte Create : 1;
					byte Destroy : 1;
				};

				byte Flags { 0 };
			};

			Handle<T> Handle;

			T* Instance { nullptr };
		};

		std::vector<size_t> m_free;
		std::vector<HandleEntry> m_entries;
		std::mutex m_mutex;
		static HandleManager<T>* s_singleton;

		Handle<T> FindInternal( const char* name ) const;
	};

	// 
	// Generic handle type that can reference anything derived from a HandleManager.
	//
	template <typename T>
	struct Handle
	{
		static_assert(std::is_base_of<HandleTarget, T>::value);

		Handle() {}
		Handle( const Handle<T>& other ) : m_handle( other.m_handle ) {}

		bool IsValid() const { return m_handle != ~0u; }

		const T* Get() const { return HandleManager<T>::Instance()->Get( *this ); }
		T* Access() const { return HandleManager<T>::Instance()->Access( *this ); }

	private:
		friend struct HandleManager<T>;

		union
		{
			struct
			{
				uint ID : 22;
				uint Version : 10;
			};

			uint m_handle { ~0u };
		};
	};

	//
	// Implementations
	//

	template <typename T>
	Handle<T> HandleManager<T>::FindInternal( const char* name ) const
	{
		DD_ASSERT( name != nullptr );
		DD_ASSERT( strlen( name ) > 0 );

		for( size_t i = 0; i < m_entries.size(); ++i )
		{
			if( (m_entries[ i ].Alive || m_entries[ i ].Create) &&
				m_entries[ i ].Instance->Name() == name )
			{
				return m_entries[ i ].Handle;
			}
		}

		return Handle<T>();
	}

	template <typename T>
	Handle<T> HandleManager<T>::Find( const char* name ) const
	{
		return FindInternal( name );
	}

	template <typename T>
	Handle<T> HandleManager<T>::Create( const char* name )
	{
		std::scoped_lock<std::mutex> lock( m_mutex );
		Handle<T> h = FindInternal( name );
		if( h.IsValid() )
		{
			return h;
		}

		if( m_free.empty() )
		{
			m_free.push_back( m_entries.size() );

			HandleEntry& entry = m_entries.emplace_back();
			entry.Handle.ID = m_entries.size() - 1;
			entry.Handle.Version = -1;
		}

		size_t idx = dd::pop_front( m_free );

		HandleEntry& entry = m_entries[ idx ];
		entry.Handle.Version++;
		entry.Create = true;

		DD_ASSERT( entry.Instance == nullptr );

		entry.Instance = new T();
		entry.Instance->SetName( name );

		return entry.Handle;
	}

	template <typename T>
	bool HandleManager<T>::IsAlive( Handle<T> h ) const
	{
		DD_ASSERT( h.ID >= 0 && h.ID < m_entries.size() );

		const HandleEntry& entry = m_entries[ h.ID ];

		return entry.Handle.Version == h.Version && 
			(entry.Alive || entry.Create);
	}

	template <typename T>
	void HandleManager<T>::Destroy( Handle<T> h )
	{
		DD_ASSERT( IsAlive( h ) );
		m_entries[ h.ID ].Destroy = true;
	}

	template <typename T>
	T* HandleManager<T>::Access( Handle<T> handle ) const
	{
		DD_ASSERT( IsAlive( handle ) );

		if( handle.ID < m_entries.size() )
		{
			return m_entries[ handle.ID ].Instance;
		}

		return nullptr;
	}

	template <typename T>
	const T* HandleManager<T>::Get( Handle<T> handle ) const
	{
		DD_ASSERT( IsAlive( handle ) );

		if( handle.ID < m_entries.size() )
		{
			return m_entries[ handle.ID ].Instance;
		}

		return nullptr;
	}

	template <typename T>
	size_t HandleManager<T>::Count() const
	{
		size_t count = 0;

		for( const HandleEntry& e : m_entries )
		{
			if( e.Alive )
			{
				++count;
			}
		}

		return count;
	}

	template <typename T>
	T* HandleManager<T>::AccessAt( size_t i ) const
	{
		size_t current = 0;
		for( const HandleEntry& e : m_entries )
		{
			if( e.Alive )
			{
				if( current == i )
				{
					return e.Instance;
				}

				++current;
			}
		}

		return nullptr;
	}

	template <typename T>
	void HandleManager<T>::Update()
	{
		for( HandleEntry& entry : m_entries )
		{
			if( entry.Destroy )
			{
				entry.Alive = false;
				entry.Create = false;
				entry.Destroy = false;

				DD_ASSERT( entry.Instance != nullptr );

				delete entry.Instance;
				entry.Instance = nullptr;
			}

			if( entry.Create )
			{
				entry.Alive = true;
				entry.Create = false;
				entry.Destroy = false;
			}
		}
	}
}