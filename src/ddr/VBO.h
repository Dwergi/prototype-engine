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

	struct IVBODestroyedListener
	{
		virtual void OnVBODestroyed(const VBO& vbo) = 0;
	};

	struct VBOManager
	{
		void RegisterListenerFor(const VBO& vbo, IVBODestroyedListener* listener);
		void UnregisterListenerFor(const VBO& vbo, IVBODestroyedListener* listener);

		void UnregisterListenerFromAll(IVBODestroyedListener* listener);

	private:
		friend VBO;

		std::unordered_map<uint, std::vector<IVBODestroyedListener*>> m_listeners;

		void OnDestroyed(const VBO& vbo);
	};

	struct VBO
	{
	public:

		VBO();
		VBO(const VBO& other);
		~VBO();

		//
		// Create a buffer.
		//
		void Create(const dd::IBuffer& buffer);
		
		template <typename T>
		void Create(const std::vector<T>& buffer) { Create(dd::ConstBuffer<T>(buffer.data(), buffer.size())); }

		template <typename T, size_t Size>
		void Create(T(&arr)[Size]) { Create(dd::ConstBuffer<T>(arr, Size)); }

		void Destroy();

		bool IsValid() const { return m_id != OpenGL::InvalidID; }

		uint ID() const { return m_id; }

		uint64 SizeBytes() const { return m_dataSize; }

		VBO& operator=(const VBO& other);

	private:
		friend struct VAO;

		uint m_id { OpenGL::InvalidID };
		uint64 m_dataSize { 0 };
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
