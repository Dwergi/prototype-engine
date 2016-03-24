#pragma once

namespace dd
{
	template <typename FnType>
	void WrenEngine::RegisterMethod( const char* name, const Function& method, FnType ptr )
	{
		
	}

	template <typename ObjType>
	void WrenEngine::RegisterObject( bool byValue )
	{
		const TypeInfo* typeInfo = GET_TYPE( ObjType );
	}
}