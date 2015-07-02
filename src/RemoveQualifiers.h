#pragma once

template <typename T>
struct StripMemberness;

template <typename T, typename U>
struct StripMemberness<T U::*>
{
	typedef T type;
};

template <typename T>
struct RemoveQualifiers
{
	typedef T type;
};

template <typename T>
struct RemoveQualifiers<const T>
{
	typedef typename RemoveQualifiers<T>::type type;
};

template <typename T>
struct RemoveQualifiers<T&>
{
	typedef typename RemoveQualifiers<T>::type type;
};

template <typename T>
struct RemoveQualifiers<const T&>
{
	typedef typename RemoveQualifiers<T>::type type;
};

template <typename T>
struct RemoveQualifiers<T&&>
{
	typedef typename RemoveQualifiers<T>::type type;
};

template <typename T>
struct RemoveQualifiers<const T*>
{
	typedef typename RemoveQualifiers<T*>::type type;
};

template <typename T>
struct RemovePtr
{
	typedef T type;
};

template <typename T>
struct RemovePtr<T*>
{
	typedef typename RemovePtr<T>::type type;
};