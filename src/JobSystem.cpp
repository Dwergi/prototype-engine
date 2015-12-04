//
// JobSystem.cpp - A manager for creating job threads which will process tasks in parallel over a number of threads.
// Copyright (C) Sebastian Nordgren 
// Date
//

#include "PrecompiledHeader.h"
#include "JobSystem.h"

#include "Function.h"
#include "JobThread.h"

#include <thread>

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // must be 0x1000
	LPCSTR szName; // pointer to name (in user addr space)
	DWORD dwThreadID; // thread ID (-1=caller thread)
	DWORD dwFlags; // reserved for future use, must be zero
} THREADNAME_INFO;
#pragma pack(pop)

void SetThreadName( DWORD dwThreadID, LPCSTR szThreadName )
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = szThreadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try
	{
		RaiseException( 0x406D1388, 0, sizeof( info ) / sizeof( DWORD ), (ULONG_PTR*) &info );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}

namespace dd
{
	JobSystem::Job::Job( const dd::Function& fn, const dd::FunctionArgs& args )
		: Function( fn ),
		Args( args )
	{

	}

	JobSystem::JobSystem( uint thread_count )
	{
		ASSERT( thread_count <= MAX_THREADS );

		CreateWorkers( std::min( thread_count, MAX_THREADS ) );
	}

	JobSystem::~JobSystem()
	{
		{
			std::lock_guard<std::mutex> lock( m_jobsMutex );
			m_jobs.Clear();
		}

		for( uint i = 0; i < m_workers.Size(); ++i )
		{
			m_workers[i].Kill();

			m_threads[i].join();
		}
	}

	void JobSystem::CreateWorkers( uint thread_count )
	{
		for( uint i = 0; i < thread_count; ++i )
		{
			m_workers.Add( JobThread( *this ) );

			m_threads[i] = std::thread( &JobThread::Run, m_workers[i] );

			char name[32] = "DD Jobs ";
			char count[8];
			_itoa_s( i + 1, count, 10 );
			count[7] = '\0';

			strcat_s( name, count );			

			DWORD threadId = ::GetThreadId( static_cast<HANDLE>( m_threads[i].native_handle() ) );
			SetThreadName( threadId, name );
		}
	}

	void JobSystem::Schedule( const Function& fn, const FunctionArgs& args )
	{
		ASSERT( fn.Signature()->GetRet() == nullptr );
		ASSERT( fn.Signature()->ArgCount() == args.Arguments.Size() );

		std::lock_guard<std::mutex> lock( m_jobsMutex );

		m_jobs.Add( Job( fn, args ) );
	}

	bool JobSystem::GetJob( Function& out_task, FunctionArgs& out_args )
	{
		std::lock_guard<std::mutex> lock( m_jobsMutex );

		if( m_jobs.Size() == 0 )
			return false;

		out_task = m_jobs[0].Function;
		out_args = m_jobs[0].Args;

		m_jobs.RemoveOrdered( 0 );

		return true;
	}
}