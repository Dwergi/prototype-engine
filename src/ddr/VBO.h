//
// VBO.h - A wrapper around OpenGL VBOs.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#pragma once

#include "ddr/OpenGL.h"

namespace ddr
{
	struct VBO;

	struct IVBOListener
	{
		virtual void OnVBODestroyed(const VBO& vbo) = 0;
		virtual void OnVBORenamed(const VBO& vbo, uint old_id) = 0;
	};

	struct VBOManager
	{
		void RegisterListenerFor(const VBO& vbo, IVBOListener* listener);
		void UnregisterListenerFor(const VBO& vbo, IVBOListener* listener);

		void UnregisterListenerFromAll(IVBOListener* listener);

	private:
		friend VBO;

		std::unordered_map<uint, std::vector<IVBOListener*>> m_listeners;

		void OnRenamed(const VBO& vbo, uint old_id);
		void OnDestroyed(const VBO& vbo);
	};

	struct VBO
	{
	public:

		VBO();
		VBO(const VBO& other);
		~VBO();

		//
		// Set the given data to this VBO.
		// This will do everything - if the sizes do not match, it will resize and recreate the buffer.
		// The return value indicates if you need to rebind to the VAO.
		//
		template <typename T, size_t Size>
		void SetData(T(&arr)[Size]) { SetData(dd::ConstBuffer<T>(arr, Size)); }
		template <typename T>
		void SetData(const std::vector<T>& buffer) { SetData(dd::ConstBuffer<T>(buffer.data(), buffer.size())); }
		void SetData(const dd::IBuffer& data);
		
		//
		// Create a new buffer.
		// This has no storage yet, just a valid ID.
		//
		void Create(std::string_view name);
		void Destroy();

		bool IsValid() const { return m_id != OpenGL::InvalidID; }

		uint ID() const { return m_id; }

		uint64 SizeBytes() const { return m_dataSize; }

		VBO& operator=(const VBO& other);

	private:
		friend struct VAO;

		void Recreate(const dd::IBuffer& buffer);

		std::string m_name;
		uint m_id = OpenGL::InvalidID;
		uint64 m_dataSize = 0;
	};
}

namespace std
{
	template <>
	struct hash<ddr::VBO>
	{
		size_t operator()(const ddr::VBO& vbo)
		{
			return vbo.ID();
		}
	};
}
