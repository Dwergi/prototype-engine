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
		void SetName(std::string_view name) { m_name = name; }

	protected:
		std::string m_name;
	};

	struct HandleManagerBase
	{
		//
		// Update this handle manager.
		//
		virtual void Update() = 0;

		//
		// Get the type name of the type this manager is handling.
		//
		virtual const char* TypeName() const = 0;

		//
		// Get the number of valid handles.
		//
		virtual size_t LiveCount() const = 0;

		//
		// Get the names of all live instances of the type.
		//
		virtual void GetLiveNames(std::vector<std::string>& names) const = 0;
	};

	//
	// Handle container that cares rather little about the internals of your type, as long as it derives from this.
	//
	template <typename T>
	struct HandleManager : HandleManagerBase
	{
		static_assert(std::is_base_of<HandleTarget, T>::value);

		HandleManager();

		//
		// Find a handle to an object with the given name.
		//
		[[nodiscard]]
		Handle<T> Find(std::string_view name) const;

		//
		// Create (or retrieve) a handle to an object with the given name.
		//
		[[nodiscard]]
		Handle<T> Create(std::string_view name);

		//
		// Destroy the object associated with the given handle. 
		// Returns true if something was destroyed, false if nothing of the given name was found.
		//
		bool Destroy(Handle<T> handle);

		//
		// Get the object instance associated with the given handle.
		// Returns null if the handle does not reference an object that still exists.
		//
		[[nodiscard]]
		const T* Get(Handle<T> handle) const;

		//
		// Access the object instance associated with the given handle.
		// Returns null if the handle does not reference an object that still exists.
		//
		[[nodiscard]]
		T* Access(Handle<T> handle) const;

		//
		// Is the given handle pointing a valid, live object.
		//
		[[nodiscard]]
		bool IsAlive(Handle<T> h) const;

		//
		// Get the number of valid handles.
		//
		[[nodiscard]]
		virtual size_t LiveCount() const override;

		//
		// Get the names of all live instances of the handle.
		//
		virtual void GetLiveNames(std::vector<std::string>& names) const override;

		//
		// Get the name of the type.
		//
		virtual const char* TypeName() const override { return T::TypeName(); }

		//
		// Access the handle at index i.
		//
		[[nodiscard]]
		T* AccessNth(size_t i) const;

		//
		// Update the handle manager, creating/destroying things that were created last frame.
		//
		virtual void Update() override;

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

		Handle<T> FindInternal(std::string_view name) const;

		virtual void OnCreate(T& instance) const {}
		virtual void OnDestroy(T& instance) const {}
	};

	// 
	// Generic handle type that can reference anything derived from a HandleManager.
	//
	template <typename T>
	struct Handle
	{
		static_assert(std::is_base_of<HandleTarget, T>::value);

		Handle() {}
		Handle(std::string_view name);
		Handle(const Handle<T>& other) : m_handle(other.m_handle) {}

		[[nodiscard]]
		uint GetID() const { return m_handle; }
		[[nodiscard]]
		const std::string& GetName() const;

		[[nodiscard]]
		bool IsValid() const { return m_handle != ~0u; }
		[[nodiscard]]
		bool IsAlive() const { return m_manager->IsAlive(*this); }

		[[nodiscard]]
		const T* Get() const { return m_manager->Get(*this); }
		[[nodiscard]]
		T* Access() const { return m_manager->Access(*this); }

		void Destroy() const { m_manager->Destroy(*this); }

		// comparisons
		bool operator==(const Handle<T>& other) const { return m_handle == other.m_handle; }
		bool operator!=(const Handle<T>& other) const { return m_handle != other.m_handle; }
		bool operator<(const Handle<T>& other) const { return m_handle < other.m_handle; }
		bool operator>(const Handle<T>& other) const { return m_handle > other.m_handle; }

		const T* operator->() const { return Get(); }
		T* operator->() { return Access(); }
		
		const T& operator*() const { return *Get(); }
		T& operator*() { return *Access(); }

	private:
		friend struct HandleManager<T>;
		static HandleManager<T>* m_manager;

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

template <typename T> dd::HandleManager<T>* dd::Handle<T>::m_manager = nullptr;

namespace std
{
	template <typename T>
	struct hash<dd::Handle<T>>
	{
		size_t operator()(const dd::Handle<T>& handle) const
		{
			return handle.GetID();
		}
	};
};


#include "HandleManager.inl"