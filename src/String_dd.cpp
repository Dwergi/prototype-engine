//
// String_dd.cpp - String implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PCH.h"
#include "String_dd.h"

namespace dd
{
	const float String::GrowthFactor = 2.0f;

	String::String(uint capacity) : 
		m_buffer(StackAddress()),
		m_capacity(capacity),
		m_length(0)
	{
		DD_ASSERT(capacity > 0);

		NullTerminate();
	}

	String::~String()
	{
		// delete heap buffer
		if (IsOnHeap())
		{
			delete[] m_buffer;
		}
	}

	const char* String::c_str() const
	{
		return m_buffer;
	}

	char* String::data()
	{
		return m_buffer;
	}

	bool String::w_str(const Buffer<wchar_t>& buffer) const
	{
		if (buffer.Size() <= m_length)
			return false;

		size_t count;
		return mbstowcs_s(&count, buffer.Access(), buffer.Size() / 4, m_buffer, m_length) > 0;
	}

	bool String::Equals(const char* other, uint64 length, bool caseless) const
	{
		if (m_length != length)
			return false;

		for (uint64 i = 0; i < m_length; ++i)
		{
			if (other[i] == '\0')
				return false;

			if (caseless)
			{
				if (tolower(m_buffer[i]) != tolower(other[i]))
					return false;
			}
			else
			{
				if (m_buffer[i] != other[i])
					return false;
			}
		}

		return true;
	}

	bool String::EqualsCaseless(const String& other) const
	{
		return Equals(other.m_buffer, other.m_length, true);
	}

	bool String::EqualsCaseless(const char* other) const
	{
		uint64 length = strlen(other);
		return Equals(other, length, true);
	}

	bool String::operator==(const char* other) const
	{
		if (other == nullptr)
		{
			return false;
		}

		uint64 length = strlen(other);
		return Equals(other, length, false);
	}

	bool String::operator==(const String& other) const
	{
		return Equals(other.m_buffer, other.m_length, false);
	}

	bool String::operator!=(const char* other) const
	{
		return !(operator==(other));
	}

	bool String::operator!=(const String& other) const
	{
		return !(operator==(other));
	}

	String& String::operator=(const String& other)
	{
		SetString(other.m_buffer, other.m_length);

		return *this;
	}

	String& String::operator=(const char* other)
	{
		DD_ASSERT(other != nullptr);

		SetString(other, strlen(other));

		return *this;
	}

	String& String::operator+=(const String& other)
	{
		Append(other.m_buffer, other.m_length);

		return *this;
	}

	String& String::operator+=(const char* other)
	{
		Append(other, strlen(other));

		return *this;
	}

	String& String::operator+=(const std::string& other)
	{
		Append(other.c_str(), other.length());

		return *this;
	}

	uint64 String::Find(const char* other, uint64 offset) const
	{
		DD_ASSERT(m_buffer != nullptr);
		DD_ASSERT(other != nullptr);

		if (other == nullptr || offset >= m_length)
			return ~0ull;

		const char* ptr = strstr(m_buffer + offset, other);

		if (ptr == nullptr)
			return ~0ull;

		uint64 location = (uint64) (ptr - m_buffer);

		return location;
	}

	uint64 String::Find(const String& other, uint64 offset) const
	{
		DD_ASSERT(m_buffer != nullptr);
		DD_ASSERT(other.m_buffer != nullptr);

		if (offset >= m_length)
			return ~0ull;

		const char* ptr = strstr(m_buffer, other.m_buffer);

		if (ptr == nullptr)
			return ~0ull;

		uint64 location = (uint64) (ptr - m_buffer);

		return location;
	}

	void String::ReplaceAll(char src, char target)
	{
		DD_ASSERT(m_buffer != nullptr);

		for (uint64 i = 0; i < m_length; ++i)
		{
			if (m_buffer[i] == src)
			{
				m_buffer[i] = target;
			}
		}
	}

	String& String::operator+=(char other)
	{
		Resize(m_length + 1);

		m_buffer[m_length] = other;
		++m_length;

		NullTerminate();

		return *this;
	}

	bool String::StartsWith(const char* other) const
	{
		size_t len = strlen(other);

		for (size_t i = 0; i < len; ++i)
		{
			if (m_buffer[i] != other[i])
			{
				return false;
			}
		}

		return true;
	}

	bool String::StartsWith(const String& other) const
	{
		return StartsWith(other.c_str());
	}

	void String::Append(const char* buffer, uint64 other_length)
	{
		if (other_length == 0)
			return;

		uint64 new_length = m_length + other_length;
		Resize(new_length);

		memcpy(&m_buffer[m_length], buffer, other_length);

		m_length = (uint) new_length;
		NullTerminate();
	}

	void String::SetString(const char* data, uint64 length)
	{
		if (data == nullptr || length == 0)
		{
			if (IsOnHeap())
			{
				void* ptr = m_buffer;
				delete[] ptr;

				m_buffer = StackAddress();
			}

			m_length = 0;

			return;
		}

		Resize(length);

		memcpy(m_buffer, data, length);
		m_length = (uint) length;

		NullTerminate();
	}

	//
	// Resize to contain a string of the given length. May expand to be larger than the given size.
	// Never shrinks a string.
	//
	void String::Resize(uint64 length)
	{
		DD_ASSERT(m_capacity > 0);

		// keep growing string until we hit a size that fits
		uint new_capacity = m_capacity;
		while (length >= new_capacity)
		{
			new_capacity = (uint) (new_capacity * GrowthFactor);
		}

		if (m_capacity == new_capacity)
		{
			return;
		}

		char* old_buffer = m_buffer;

		m_buffer = new char[new_capacity];
		m_capacity = (uint) new_capacity;

		// copy old data over if required
		if (m_length > 0)
		{
			SetString(old_buffer, m_length);
		}

		// delete old buffer if not stack-allocated
		if (old_buffer != StackAddress())
		{
			delete[] old_buffer;
		}
	}

	void String::ShrinkToFit()
	{
		if (!IsOnHeap())
		{
			return;
		}

		void* old_buffer = m_buffer;

		m_buffer = new char[m_length + 1]; 
		m_capacity = m_length + 1;

		memcpy(m_buffer, old_buffer, m_length);

		delete[] old_buffer;

		NullTerminate();
	}

	void String::NullTerminate()
	{
		DD_ASSERT(m_length < m_capacity);
		m_buffer[m_length] = '\0';
	}
}