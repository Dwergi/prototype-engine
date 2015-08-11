#pragma once

#define DEFINE_ITERATORS( Type ) \
class const_iterator { \
public: \
	const Type* Pointer; \
	const_iterator() : Pointer( nullptr ) {} \
	const_iterator( const Type* ptr ) : Pointer( ptr ) {} \
	inline const Type& operator*() const { return *Pointer; } \
	inline const_iterator& operator++() { ++Pointer; return *this; } \
	inline const_iterator& operator+( size_t count ) { Pointer += count; return *this; } \
	inline bool operator!=( const const_iterator& other ) const { return Pointer != other.Pointer; } \
}; \
\
class iterator : public const_iterator { \
public: \
	iterator() : const_iterator() {} \
	iterator( Type* ptr ) : const_iterator( ptr ) {} \
	inline Type& operator*() const { return *const_cast<char*>( Pointer ); } \
}; \
inline const_iterator begin() const { return const_iterator( m_buffer ); } \
inline const_iterator end() const { return const_iterator( m_buffer + m_length ); } \
inline iterator begin() { return iterator( m_buffer ); } \
inline iterator end() { return iterator( m_buffer + m_length ); }

