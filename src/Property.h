//
// Property.h - A wrapper around a member that can be used to create a property editor.
// Copyright (C) Sebastian Nordgren 
// September 2nd 2015
//

#pragma once

#include "Member.h"
#include "UndoStack.h"

namespace dd
{
	struct Property
	{
		Property();
		Property(const Variable& value, std::string_view name);
		Property(const Member& member, void* object);
		Property(const Property& other);
		Property(Property&& other) noexcept;
		~Property();

		Property& operator=(const Property& other);
		Property& operator=(Property&& other) noexcept;

		const Variable& GetVariable() const { return m_variable; }
		void SetFromVariable(const Variable& var);

		template <typename T>
		void GetPtr(const T* out) const;

		template <typename T>
		const T& Get() const;

		template <typename T>
		T& Access() const;
		
		template <typename T>
		void Set(const T& val);

		const dd::TypeInfo* Type() const { return m_variable.Type(); }
		std::string_view Name() const { return m_name; }

		void Undo();
		void Redo();

		uint64 UndoSize() const { return m_undo->GetUndoSize(); }
		uint64 RedoSize() const { return m_undo->GetRedoSize(); }

	private:
		Variable m_variable;
		std::string m_name;
		UndoStack* m_undo { nullptr };
	};

	template <typename T>
	void Property::GetPtr(const T* out) const
	{
		DD_ASSERT(out != nullptr);
		DD_ASSERT(m_variable.IsValid());
		m_variable.Get(out);
	}

	template <typename T>
	const T& Property::Get() const
	{
		DD_ASSERT(m_variable.IsValid());
		return m_variable.Get<T>();
	}

	template <typename T>
	T& Property::Access() const
	{
		DD_ASSERT(m_variable.IsValid());
		return m_variable.Access<T>();
	}

	template <typename T>
	void Property::Set(const T& val)
	{
		DD_ASSERT(m_variable.IsValid());
		m_variable.Set(val);
		m_undo->OnSet(m_variable);
	}
}