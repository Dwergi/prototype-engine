//
// WrenArgument.h - A wrapper around Wren slots, allowing you to get/set arguments relatively simply.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

namespace dd
{
	namespace WrenInternal
	{
		template <typename T>
		struct WrenArgument
		{
			static T get( WrenVM* vm, int slot )
			{
				return *static_cast<T*>(wrenGetSlotForeign( vm, slot ));
			}
		};

		template <typename T>
		struct WrenArgument<T&>
		{
			static T& get( WrenVM* vm, int slot )
			{
				return *static_cast<T*>(wrenGetSlotForeign( vm, slot ));
			}
		};

		template <typename T>
		struct WrenArgument<const T&>
		{
			static const T& get( WrenVM* vm, int slot )
			{
				return *static_cast<const T*>(wrenGetSlotForeign( vm, slot ));
			}
		};

		template <typename T>
		struct WrenArgument<T*>
		{
			static T* get( WrenVM* vm, int slot )
			{
				return static_cast<T*>(wrenGetSlotForeign( vm, slot ));
			}
		};

		template <typename T>
		struct WrenArgument<const T*>
		{
			static const T* get( WrenVM* vm, int slot )
			{
				return static_cast<const T*>(wrenGetSlotForeign( vm, slot ));
			}
		};

		template <>
		struct WrenArgument<bool>
		{
			static bool get( WrenVM* vm, int slot )
			{
				return wrenGetSlotBool( vm, slot );
			}

			static void set( WrenVM* vm, int slot, bool val )
			{
				wrenSetSlotBool( vm, slot, val );
			}
		};

		template <>
		struct WrenArgument<const char*>
		{
			static const char* get( WrenVM* vm, int slot )
			{
				return wrenGetSlotString( vm, slot );
			}

			static void set( WrenVM* vm, int slot, const char* val )
			{
				wrenSetSlotString( vm, slot, val );
			}
		};

		template <>
		struct WrenArgument<double>
		{
			static double get( WrenVM* vm, int slot )
			{
				return wrenGetSlotDouble( vm, slot );
			}

			static void set( WrenVM* vm, int slot, double val )
			{
				wrenSetSlotDouble( vm, slot, val );
			}
		};

		template <>
		struct WrenArgument<int>
		{
			static int get( WrenVM* vm, int slot )
			{
				return (int) wrenGetSlotDouble( vm, slot );
			}

			static void set( WrenVM* vm, int slot, int val )
			{
				wrenSetSlotDouble( vm, slot, (double) val );
			}
		};

		template <>
		struct WrenArgument<uint>
		{
			static uint get( WrenVM* vm, int slot )
			{
				return (uint) wrenGetSlotDouble( vm, slot );
			}

			static void set( WrenVM* vm, int slot, uint val )
			{
				wrenSetSlotDouble( vm, slot, (double) val );
			}
		};

		template <>
		struct WrenArgument<float>
		{
			static float get( WrenVM* vm, int slot )
			{
				return (float) wrenGetSlotDouble( vm, slot );
			}

			static void set( WrenVM* vm, int slot, float val )
			{
				wrenSetSlotDouble( vm, slot, (double) val );
			}
		};
	}
}