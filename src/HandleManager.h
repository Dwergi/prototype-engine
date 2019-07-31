//
// HandleManager.h - A generic, simple handle system that stores instances and uses indices to reference them.
// Copyright (C) Sebastian Nordgren 
// November 22nd 2018
//

#pragma once

namespace dd
{
	template <typename T> struct Handle;

	struct HandleTarget
	{
		//
		// The name of this instance.
		//
		const std::string& Name() const { return m_name; }

		//
		// Set the name of this instance.
		//
		void SetName( std::string name ) { m_name = name; }

	protected:
		std::string m_name;
	};

	//
	// Handle container that cares rather little about the internals of your type, as long as it derives from this.
	// Stick DD_HANDLE_MANAGER( YourType ) into your .cpp as well.
	//
	template <typename T>
	struct HandleManager
	{
		static_assert(std::is_base_of<HandleTarget, T>::value);

		HandleManager();

		//
		// Find a handle to an object with the given name.
		//
		Handle<T> Find( std::string name ) const;
		
		//
		// Create (or retrieve) a handle to an object with the given name.
		//
		Handle<T> Create( std::string name );

		//
		// Destroy the object associated with the given handle. 
		// Returns true if something was destroyed, false if nothing of the given name was found.
		//
		void Destroy( Handle<T> handle );
		
		//
		// Get the object instance associated with the given handle.
		// Returns null if the handle does not reference an object that still exists.
		//
		const T* Get( Handle<T> handle ) const;

		//
		// Access the object instance associated with the given handle.
		// Returns null if the handle does not reference an object that still exists.
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
		virtual void Update();

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

		Handle<T> FindInternal( std::string name ) const;

		virtual void OnCreate( T& instance ) const {}
		virtual void OnDestroy( T& instance ) const {}
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

		bool operator==( const Handle<T>& other ) const { return m_handle == other.m_handle; }
		bool operator!=( const Handle<T>& other ) const { return m_handle != other.m_handle; }

		uint GetID() const { return m_handle; }
		bool IsValid() const { return m_handle != ~0u; }

		const T* Get() const { return m_manager->Get( *this ); }
		T* Access() const { return m_manager->Access( *this ); }

	private:
		friend struct HandleManager<T>;
		static const HandleManager<T>* m_manager;

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
}

template <typename T> const dd::HandleManager<T>* dd::Handle<T>::m_manager = nullptr;

#include "HandleManager.inl"