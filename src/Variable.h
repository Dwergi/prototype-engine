/*
Copyright (c) 2013-2014 Randy Gaul http://RandyGaul.net

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:
1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

Creator(s)    : Randy Gaul
Creation Date : Wed Oct 09 23:58:31 2013
File Name     : SELVariable.h
Purpose       : 
*/

#pragma once

namespace dd
{
	struct TypeInfo;
	class Member;

	struct Variable
	{
		Variable();
		Variable(const TypeInfo* typeInfo, void* data);
		Variable(const Member& member, void* object);
		Variable(const Variable& rhs);

		template <typename T>
		explicit Variable( const T& rhs );
		template <typename T>
		explicit Variable( const T* rhs );
		template <typename T>
		explicit Variable( T* rhs );

		// Reset this Variable to point at the values in the given variable
		void Reset(const Variable& other);

		Variable& operator=(const Variable& rhs);

		void* Data() const;
		const TypeInfo* Type() const;
		bool IsValid() const;

		template <typename T>
		T& Access() const;
		template <typename T>
		const T& Get() const;

		template <typename T>
		void Get(const T* out) const;
		template <typename T>
		void Set(const T& new_value) const;

		void CopyTo(void* dst) const;
		void SetFromVariable(const Variable& other) const;

		void PlacementNew() const;
		void PlacementDelete() const;
		void Delete() const;

	private:
		void* m_data { nullptr };
		const TypeInfo* m_typeInfo { nullptr };

	};
	
	template <typename T>
	Variable::Variable( const T& rhs )
		: m_data( (T*) &rhs ),
		m_typeInfo( DD_FIND_TYPE( T ) )
	{
	}

	template <typename T>
	Variable::Variable( const T* rhs )
		: m_data( (T*) rhs ),
		m_typeInfo( DD_FIND_TYPE( T* )  )
	{
	}

	template <typename T>
	Variable::Variable( T* rhs )
		: m_data( rhs ),
		m_typeInfo( DD_FIND_TYPE( T* )  )
	{
	}

	// Cast helper exists to properly handle pointer types
	template <typename T>
	struct CastHelper
	{
		static T& Cast(void* const& data)
		{
			return *(T*&) (data);
		}
	};

	template <typename T>
	struct CastHelper<T*>
	{
		static T*& Cast(void* const& data)
		{
			return (T*&) data;
		}
	};

	template <typename T>
	struct CastHelper<const T*>
	{
		static const T*& Cast(void* const& data)
		{
			return (const T*&) data;
		}
	};

	template <typename T>
	struct CastHelper<const T&>
	{
		static const T& Cast(void* const& data)
		{
			return *(const T*&) data;
		}
	};

	template <typename T>
	void Variable::Set(const T& new_value) const
	{
		DD_ASSERT(m_typeInfo == DD_FIND_TYPE(T));

		T& value = CastHelper<T>::Cast(m_data);
		value = new_value;
	}

	template <typename T>
	T& Variable::Access() const
	{
		// allow casting only up the inheritance tree, eg. a TransformComponent can be retrieved as a Component
		DD_ASSERT(m_typeInfo->IsDerivedFrom(DD_FIND_TYPE(T)));
		return CastHelper<T>::Cast(m_data);
	}

	template <typename T>
	void Variable::Get(const T* out) const
	{
		DD_ASSERT(out != nullptr);
		// allow casting only up the inheritance tree, eg. a TransformComponent can be retrieved as a Component
		DD_ASSERT(m_typeInfo->IsDerivedFrom(DD_FIND_TYPE(T)));

		out = CastHelper<const T>::Cast(m_data);
	}

	template <typename T>
	const T& Variable::Get() const
	{
		DD_ASSERT(m_typeInfo->IsDerivedFrom(DD_FIND_TYPE(T)));

		return CastHelper<const T>::Cast(m_data);
	}
}
