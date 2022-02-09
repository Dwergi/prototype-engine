//
// UndoStack.h - Undo stack implementation for a given property.
// Copyright (C) Sebastian Nordgren 
// November 17th 2021
//

#pragma once

#include "Array.h"
#include "RingBuffer.h"
#include "Scope.h"
#include "Variant.h"

namespace dd
{
	struct Property;

	struct UndoStack
	{
	public:
		static constexpr uint64 MaxVariableSize = 64;

		explicit UndoStack(dd::Property& property);
		
		void CopyFrom(const UndoStack& other);

		void OnSet(const Variable& new_value);
		bool Undo(Variable& out_value);
		bool Redo(Variable& out_value);

		uint64 GetUndoSize() const { return m_undo.size(); }
		uint64 GetRedoSize() const { return m_redo.size(); }

	private:
		const TypeInfo* m_type { nullptr };
		Variant64 m_current;
		std::vector<Variant<MaxVariableSize>> m_undo;
		std::vector<Variant<MaxVariableSize>> m_redo;
		Scope m_scope;
	};
}
