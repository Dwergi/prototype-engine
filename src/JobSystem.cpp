//
// JobSystem.cpp - A manager for creating job threads which will process tasks in parallel over a number of threads.
// Copyright (C) Sebastian Nordgren 
// 7th April 2016
//

#include "PrecompiledHeader.h"
#include "JobSystem.h"

#include "Function.h"
#include "JobThread.h"
#include "Timer.h"

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
	JobSystem::Job::Job( const Job& other )
		: Func( other.Func ),
		Category( other.Category ),
		Status( other.Status ),
		ID( other.ID )
	{

	}

	JobSystem::Job::Job( const std::function<void()>& fn, const char* category, uint id )
		: Func( fn ),
		Category( category ),
		Status( JobStatus::Pending ),
		ID( id )
	{

	}

	JobSystem::Job::~Job()
	{
		ID = 0;
		Status = JobStatus::Free;
	}

	JobSystem::JobSystem( uint thread_count )
	{
		m_jobs.Reserve( JOB_QUEUE_SIZE );

		DD_ASSERT( thread_count <= MAX_THREADS );

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
			m_workers[i]->Kill();

			m_threads[i].join();
		}
	}

	void JobSystem::CreateWorkers( uint thread_count )
	{
		for( uint i = 0; i < thread_count; ++i )
		{
			char name[32] = "DD Jobs ";
			char count[8];
			_itoa_s( i + 1, count, 10 );
			count[7] = '\0';

			strcat_s( name, count );

			m_workers.Add( new JobThread( *this, name ) );

			m_threads[i] = std::thread( &JobThread::Run, m_workers[i] );

			DWORD threadId = ::GetThreadId( static_cast<HANDLE>( m_threads[i].native_handle() ) );
			SetThreadName( threadId, name );

			SetThreadName( ::GetCurrentThreadId(), "Main" );
		}
	}

	void JobSystem::Schedule( const Function& fn, const char* category )
	{
		DD_ASSERT( fn.Signature()->ArgCount() == 0 );
		DD_ASSERT( !fn.IsMethod() || fn.Context().IsValid() ); // must be free or already be bound

		std::lock_guard<std::mutex> lock( m_jobsMutex );

		// find a free slot
		for( uint i = 0; i < m_jobs.Size(); ++i )
		{
			if( m_jobs[i].Status == JobStatus::Free )
			{
				new (&m_jobs[i]) Job( [fn]() { fn(); }, category, i );
				m_pendingJobs.Add( i );
				return;
			}
		}

		DD_ASSERT( m_jobs.Size() < m_jobs.Capacity(), "JobSystem out of space, allocate more!" );

		// add if we won't cause a reallocate
		if( m_jobs.Size() < m_jobs.Capacity() )
		{
			int id = m_jobs.Size();
			m_jobs.Add( Job( [fn]() { fn(); }, category, id ) );
			m_pendingJobs.Add( id );
		}
	}

	void JobSystem::Schedule( const std::function<void ()>& fn, const char* category )
	{
		std::lock_guard<std::mutex> lock( m_jobsMutex );

		// find a free slot
		for( uint i = 0; i < m_jobs.Size(); ++i )
		{
			if( m_jobs[i].Status == JobStatus::Free )
			{
				new (&m_jobs[i]) Job( fn, category, i );
				m_pendingJobs.Add( i );
				return;
			}
		}

		DD_ASSERT( m_jobs.Size() < m_jobs.Capacity(), "JobSystem out of space, allocate more!" );

		// add if we won't cause a reallocate
		if( m_jobs.Size() < m_jobs.Capacity() )
		{
			int id = m_jobs.Size();
			m_jobs.Add( Job( fn, category, id ) );
			m_pendingJobs.Add( id );
		}
	}

	bool JobSystem::HasPendingJobs( const char* category )
	{
		std::lock_guard<std::mutex> lock( m_jobsMutex );

		for( int job : m_pendingJobs )
		{
			if( m_jobs[job].Category == category )
				return true;
		}

		return false;
	}

	void JobSystem::MarkDone( const Job& job )
	{
		int id = job.ID;
		m_jobs[id].~Job();
		m_jobs[id].Status = JobStatus::Free;
	}

	bool JobSystem::GetPendingJob( Job*& out_job )
	{
		out_job = nullptr;

		std::lock_guard<std::mutex> lock( m_jobsMutex );
		
		if( m_pendingJobs.Size() == 0 )
			return false;	

		out_job = &m_jobs[m_pendingJobs[0]];
		m_pendingJobs.RemoveOrdered( 0 );

		return true;
	}

	JobThread* JobSystem::FindCurrentWorker() const
	{
		std::thread::id current_id = std::this_thread::get_id();

		for( uint i = 0; i < m_workers.Size(); ++i )
		{
			if( m_threads[i].get_id() == current_id )
			{
				return m_workers[i];
			}
		}

		return nullptr;
	}

	void JobSystem::WaitForCategory( const char* category, uint timeout_ms )
	{
		Timer timer;
		timer.Start();

		JobThread* thread = FindCurrentWorker();
		DD_ASSERT_ERROR( thread == nullptr, "Cannot wait for categories inside jobs!" );
		
		do
		{
			if( !HasPendingJobs( category ) )
				return;
			
			::Sleep( 1 );
		} 
		while( timeout_ms == 0 || timer.Time() < timeout_ms );
	}
}