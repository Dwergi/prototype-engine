//
// Variant.h - A variant is an inline stored Variable that can be of any reflectable type.
// Copyright (C) Sebastian Nordgren 
// November 17th 2021
//

#pragma once

#include "TypeInfo.h"
#include "Variable.h"

namespace dd
{
	template <uint64 Capacity>
	struct Variant
	{
		Variant() {}
		Variant(const TypeInfo* type, const void* value)
		{
			SetVoid(type, value);
		}

		explicit Variant(const Variable& var)
		{
			if (var.Type() != nullptr)
			{
				SetVoid(var.Type(), var.Data());
			}
		}

		Variant(const Variant& other)
		{
			SetVoid(other.m_variable.Type(), (void*) other.m_data);
		}

		Variant& operator=(const Variable& variable)
		{
			SetVoid(variable.Type(), variable.Data());
			return *this;
		}

		template <typename T>
		Variant& operator=(const T& value)
		{
			Set(value);
			return *this;
		}

		template <typename T>
		const T& Get() const
		{
			DD_ASSERT(TypeInfo::GetType<T>() == m_type);
			return *reinterpret_cast<const T*>(m_data)
		}

		template <typename T>
		void Set(const T& value)
		{
			SetVoid(TypeInfo::GetType<T>(), &value);
		}

		void* GetVoid()
		{
			return m_data;
		}

		const void* GetVoid() const
		{
			return m_data;
		}

		void SetVoid(const TypeInfo* type, void* value)
		{
			DD_ASSERT(type != nullptr);
			DD_ASSERT(type->Size() <= Capacity);

			if (m_variable.IsValid())
			{
				m_variable.Type()->PlacementDelete(m_data);
			}

			type->PlacementNew(m_data);
			type->Copy(m_data, value);

			m_variable = Variable(type, m_data);
		}

		const TypeInfo* Type() const
		{
			return m_type;
		}

		Variable ToVariable() const
		{
			return m_variable;
		}

	private:
		Variable m_variable;
		byte m_data[Capacity] { 0 };
	};

	using Variant8 = Variant<8>;
	using Variant16 = Variant<16>;
	using Variant32 = Variant<32>;
	using Variant64 = Variant<64>;
	using Variant128 = Variant<128>;
	using Variant256 = Variant<256>;
}