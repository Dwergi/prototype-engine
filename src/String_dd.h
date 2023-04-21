//
// String_dd.h - String implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "Iteration.h"
#include "Hash.h"

namespace dd
{
	struct TypeInfo;

	struct String
	{
	protected:

		String(uint32 capacity);

	public:

		static const uint64 Invalid { ~0ull };

		String(const String& other) = delete;
		virtual ~String();

		String& operator=(const char* other);
		String& operator=(const String& other);

		String& operator+=(const String& other);
		String& operator+=(const char* other);
		String& operator+=(char other);
		String& operator+=(const std::string& other);

		void Append(const char* other, uint64 length);

		bool operator==(const char* other) const;
		bool operator==(const String& other) const;

		bool operator!=(const char* other) const;
		bool operator!=(const String& other) const;

		bool EqualsCaseless(const char* other) const;
		bool EqualsCaseless(const String& other) const;

		uint64 Find(const char* other, uint64 offset = 0) const;
		uint64 Find(const String& other, uint64 offset = 0) const;

		void ReplaceAll(char src, char target);

		void Clear() { m_length = 0; }

		char& operator[](int index) { DD_ASSERT(index >= 0 && (uint) index < m_length); return m_buffer[index]; }
		const char& operator[](int index) const { DD_ASSERT(index >= 0 && (uint) index < m_length); return m_buffer[index]; }

		char& operator[](uint64 index) { DD_ASSERT(index < m_length); return m_buffer[index]; }
		const char& operator[](uint64 index) const { DD_ASSERT(index < m_length); return m_buffer[index]; }

		const char* c_str() const;
		char* data();

		// Get a wide copy of this string.
		bool w_str(const Buffer<wchar_t>& buffer) const;

		uint64 Length() const { return m_length; }
		uint64 Capacity() const { return m_capacity - 1; /* null terminator */ }
		bool IsEmpty() const { return m_length == 0; }

		void ShrinkToFit();

		bool IsOnHeap() const { return m_buffer != StackAddress(); }
		char* StackAddress() const { return ((char*) this) + sizeof(dd::String); }

		bool StartsWith(const char* other) const;
		bool StartsWith(const String& other) const;

		DD_BASIC_TYPE(String)

		DD_DEFINE_ITERATORS(char, m_buffer, m_length)

	protected:

		char* m_buffer { nullptr };
		uint32 m_length { 0 };
		uint32 m_capacity { 0 };

		void Resize(uint64 length);
		void SetString(const char* data, uint64 length);
		bool Equals(const char* other, uint64 length, bool caseless) const;

		void NullTerminate();

		static const float GrowthFactor;
	};

	//
	// An in-place implementation of string which always allocates a fixed size buffer. 
	//
	template <uint64 Size = 32>
	class InplaceString
		: public String
	{
	public:

		InplaceString()
			: String(Size)
		{
		}

		InplaceString(const char* str)
			: String(Size)
		{
			uint64 length = str != nullptr ? strlen(str) : 0;
			SetString(str, length);
		}

		InplaceString(const char* other, uint64 length)
			: String(Size)
		{
			SetString(other, length);
		}

		InplaceString(const String& other)
			: String(Size)
		{
			SetString(other.c_str(), other.Length());
		}

		InplaceString(const InplaceString<Size>& other)
			: String(Size)
		{
			SetString(other.c_str(), other.Length());
		}

		template <uint64 OtherSize>
		InplaceString(const InplaceString<OtherSize>& other)
			: String(Size)
		{
			SetString(other.c_str(), other.Length());
		}
		
		InplaceString(InplaceString<Size>&& other) noexcept
			: String(Size)
		{
			if (other.IsOnHeap())
			{
				m_buffer = other.m_buffer;
				other.m_buffer = nullptr;

				m_length = other.m_length;
				m_capacity = other.m_capacity;
			}
			else
			{
				*this = other;
			}
		}

		InplaceString<Size>& operator=(const InplaceString<Size>& other)
		{
			SetString(other.c_str(), other.Length());
			return *this;
		}

		InplaceString<Size>& operator=(const char* other)
		{
			SetString(other, strlen(other));
			return *this;
		}

		virtual ~InplaceString()
		{

		}

		InplaceString<Size> Substring(uint64 start, uint64 count = INT_MAX) const;

	private:

		char m_stackData[Size] { 0 };
	};

	typedef InplaceString<8> String8;
	typedef InplaceString<16> String16;
	typedef InplaceString<32> String32;
	typedef InplaceString<64> String64;
	typedef InplaceString<128> String128;
	typedef InplaceString<256> String256;

	template <uint64 Size>
	InplaceString<Size> InplaceString<Size>::Substring(uint64 start, uint64 count) const
	{
		DD_ASSERT(start <= m_length);
		DD_ASSERT(count >= 0);

		count = ddm::min(m_length - start, count);

		InplaceString<Size> substring;
		substring.SetString(m_buffer + start, count);

		return substring;
	}

	template<> inline uint64 Hash<String>(const String& value) { return HashString(value.c_str(), value.Length()); }
	template<> inline uint64 Hash<String8>(const String8& value) { return HashString(value.c_str(), value.Length()); }
	template<> inline uint64 Hash<String16>(const String16& value) { return HashString(value.c_str(), value.Length()); }
	template<> inline uint64 Hash<String32>(const String32& value) { return HashString(value.c_str(), value.Length()); }
	template<> inline uint64 Hash<String64>(const String64& value) { return HashString(value.c_str(), value.Length()); }
	template<> inline uint64 Hash<String128>(const String128& value) { return HashString(value.c_str(), value.Length()); }
	template<> inline uint64 Hash<String256>(const String256& value) { return HashString(value.c_str(), value.Length()); }
}

namespace std
{
	template <typename T>
	struct string_hash
	{
		std::size_t operator()(const T& str) const { return dd::HashString(str.c_str(), str.Length()); }
	};

	template<> struct hash<dd::String> : string_hash<dd::String> {};
	template<> struct hash<dd::String8> : string_hash<dd::String8> {};
	template<> struct hash<dd::String16> : string_hash<dd::String16> {};
	template<> struct hash<dd::String32> : string_hash<dd::String32> {};
	template<> struct hash<dd::String64> : string_hash<dd::String64> {};
	template<> struct hash<dd::String128> : string_hash<dd::String128> {};
	template<> struct hash<dd::String256> : string_hash<dd::String256> {};
}
