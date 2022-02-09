//
// Property.h - A wrapper around a member that can be used to create a property editor.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#include "PCH.h"
#include "Property.h"

namespace dd
{
	Property::Property()
	{

	}

	Property::~Property()
	{
		if (m_undo != nullptr)
		{
			delete m_undo;
		}
	}

	Property::Property(const Variable& variable, std::string_view name) :
		m_variable(variable),
		m_name(name)
	{
		if (m_variable.Type()->Size() < UndoStack::MaxVariableSize)
		{
			m_undo = new UndoStack(*this);
		}
	}

	Property::Property(const Member& member, void* object) :
		m_variable(member, object),
		m_name(member.Name().c_str())
	{
		if (m_variable.Type()->Size() < UndoStack::MaxVariableSize)
		{
			m_undo = new UndoStack(*this);
		}
	}

	Property::Property(const Property& other)
	{
		*this = other;
	}

	Property::Property(Property&& other) noexcept
	{
		*this = std::move(other);
	}

	Property& Property::operator=(const Property& other)
	{
		m_variable = other.m_variable;
		m_name = other.m_name;

		if (other.m_undo != nullptr)
		{
			m_undo = new UndoStack(*this);
			m_undo->CopyFrom(*other.m_undo);
		}

		return *this;
	}

	Property& Property::operator=(Property&& other) noexcept
	{
		std::swap(m_variable, other.m_variable);
		std::swap(m_name, other.m_name);
		std::swap(m_undo, other.m_undo);

		return *this;
	}

	void Property::SetFromVariable(const Variable& var)
	{
		m_variable.SetFromVariable(var);

		m_undo->OnSet(var);
	}

	void Property::Undo()
	{
		m_undo->Undo(m_variable);
	}

	void Property::Redo()
	{
		m_undo->Redo(m_variable);
	}
}