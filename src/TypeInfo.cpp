#include "PrecompiledHeader.h"
#include "TypeInfo.h"

TypeInfo::TypeInfo()
	: m_size( 0 )
{

}

TypeInfo::~TypeInfo()
{
	for( MemberBase* ptr : m_members )
		delete ptr;
}

TypeInfo* TypeInfo::GetType( const std::string& name )
{
	const TypeInfo* ptr = TypeInfo::Head();
	while( ptr != nullptr )
	{
		if( name == ptr->m_name )
			return const_cast<TypeInfo*>( ptr );

		ptr = ptr->Next();
	}

	return nullptr;
}