#include "PCH.h"
#include "OpenGL.h"

namespace OpenGL
{
	bool Initialize()
	{
		return gl3wInit() == 0;
	}
}

namespace ddr
{
	dd::String128 GetArrayUniformName( const char* arrayName, size_t index, const char* uniform )
	{
		dd::String256 result;
		result += arrayName;
		result += "[";

		char buffer[ 32 ];
		_itoa_s( (int) index, buffer, 10 );
		result += buffer;

		result += "]";

		if( uniform != nullptr )
		{
			result += ".";
			result += uniform;
		}

		return result;
	}
}