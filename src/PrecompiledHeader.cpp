//
// PrecompiledHeader.cpp
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "imgui/imgui.h"

namespace dd
{
	void* PointerAdd( void* base, uint64 offset )
	{
		return (void*) ((size_t) base + offset);
	}

	const void* PointerAdd( const void* base, uint64 offset )
	{
		return (const void*) ((size_t) base + offset);
	}
}

namespace ImGui
{
	void Value( const char* prefix, const glm::ivec2& value )
	{
		ImGui::Text( "%s: %.3f, %.3f", prefix, value.x, value.y );
	}

	void Value( const char* prefix, const glm::vec2& value, const char* float_format )
	{
		if( float_format != nullptr )
		{
			char fmt[64];
			snprintf( fmt, 64, "%%s: %s, %s", float_format, float_format );
			ImGui::Text( fmt, prefix, value.x, value.y );
		}
		else
		{
			ImGui::Text( "%s: %.3f, %.3f", prefix, value.x, value.y );
		}
	}

	void Value( const char* prefix, const glm::vec3& value, const char* float_format )
	{
		if( float_format != nullptr )
		{
			char fmt[64];
			snprintf( fmt, 64, "%%s: %s, %s, %s", float_format, float_format, float_format );
			ImGui::Text( fmt, prefix, value.x, value.y, value.z );
		}
		else
		{
			ImGui::Text( "%s: %.3f, %.3f, %.3f", prefix, value.x, value.y, value.z );
		}
	}

	void Value( const char* prefix, const glm::vec4& value, const char* float_format )
	{
		if( float_format != nullptr )
		{
			char fmt[64];
			snprintf( fmt, 64, "%%s: %s, %s, %s, %s", float_format, float_format, float_format, float_format );
			ImGui::Text( fmt, prefix, value.x, value.y, value.z, value.w );
		}
		else
		{
			ImGui::Text( "%s: %.3f, %.3f, %.3f, %.3f", prefix, value.x, value.y, value.z, value.w );
		}
	}
}