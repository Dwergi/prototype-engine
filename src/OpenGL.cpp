#include "PrecompiledHeader.h"
#include "OpenGL.h"

namespace dd
{
	String128 GetArrayUniformName( const char* arrayName, int index, const char* uniform )
	{
		String256 result;
		result += arrayName;
		result += "[";

		char buffer[ 32 ];
		_itoa_s( index, buffer, 10 );
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