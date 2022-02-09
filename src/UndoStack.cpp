//
// UndoStack.h - Undo stack implementation for a given property.
// Copyright (C) Sebastian Nordgren 
// November 17th 2021
//

#include "PCH.h"
#include "UndoStack.h"
#include "Property.h"

namespace dd
{
	UndoStack::UndoStack(Property& property)
	{
		m_type = property.Type();
		m_current = property.GetVariable();
	}

	void UndoStack::CopyFrom(const UndoStack& other)
	{
		DD_ASSERT(other.m_type == m_type);

		m_undo = other.m_undo;
		m_redo = other.m_redo;
	}

	void UndoStack::OnSet(const Variable& new_value)
	{
		DD_ASSERT(new_value.Type() == m_type);

		m_undo.push_back(m_current);
		m_current = new_value;
	}

	bool UndoStack::Undo(Variable& out_value)
	{
		if (m_undo.size() == 0)
		{
			return false;
		}

		Inside inside = m_scope.Enter();
		if (!inside)
		{
			return false;
		}

		m_redo.push_back(m_current);
		
		m_current = m_undo.back();
		m_undo.pop_back();
		out_value = m_current.ToVariable();
		return true;
	}

	bool UndoStack::Redo(Variable& out_value)
	{
		if (m_redo.size() == 0)
		{
			return false;
		}

		Inside inside = m_scope.Enter();
		if (!inside)
		{
			return false;
		}

		m_undo.push_back(m_current);

		m_current = m_redo.back();
		m_redo.pop_back();
		out_value = m_current.ToVariable();

		return true;
	}
}