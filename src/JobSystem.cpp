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
		WaitingOn( other.WaitingOn ),
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

	JobSystem::JobSystem( uint thread_count )
	{
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
			m_workers.Add( new JobThread( *this ) );

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

	void JobSystem::Schedule( const Function& fn, const char* category )
	{
		DD_ASSERT( fn.Signature()->ArgCount() == 0 );
		DD_ASSERT( !fn.IsMethod() || fn.Context().IsValid() ); // must be free or already be bound

		std::lock_guard<std::mutex> lock( m_jobsMutex );

		m_jobs.Add( Job( [fn]() { fn(); }, category, m_jobID++ ) );
	}

	void JobSystem::Schedule( const std::function<void ()>& fn, const char* category )
	{
		std::lock_guard<std::mutex> lock( m_jobsMutex );

		m_jobs.Add( Job( fn, category, m_jobID++ ) );
	}

	bool JobSystem::HasPendingJobs( const char* category )
	{
		std::lock_guard<std::mutex> lock( m_jobsMutex );

		for( Job& job : m_jobs )
		{
			if( job.Category == category )
				return true;
		}

		return false;
	}

	void JobSystem::UpdateJobs()
	{
		std::lock_guard<std::mutex> lock( m_jobsMutex );

		// clear finished jobs
		for( int i = 0; i < (int) m_jobs.Size(); ++i )
		{
			if( m_jobs[i].Status == JobStatus::Done )
			{
				m_jobs.Remove( i );
				--i;
			}
		}

		DenseMap<String128, int> categories;
		categories.Reserve( m_jobs.Size() );

		// hash all the categories and keep track of them
		for( Job& job : m_jobs )
		{
			int* count = categories.Find( job.Category );
			if( count == nullptr )
			{
				categories.Add( job.Category, 1 );
			}
			else
			{
				++(*count);
			}
		}

		// if the category a job is waiting on isn't in the above list, bump the status of the job
		for( Job& job : m_jobs )
		{
			if( job.Status == JobStatus::Waiting )
			{
				int* count = categories.Find( job.WaitingOn );
				if( count == nullptr )
				{
					job.Status = JobStatus::DoneWaiting;
				}
			}
		}
	}

	JobHandle::JobHandle()
		: m_system( nullptr ),
		m_id( 0 )
	{

	}

	JobHandle::JobHandle( JobSystem& system, JobSystem::Job& job )
		: m_system( &system ),
		m_id( job.ID )
	{

	}
	
	JobSystem::Job* JobHandle::GetJob() const
	{
		DD_ASSERT( m_system != nullptr, "Invalid handle used!" );

		std::lock_guard<std::mutex> lock( m_system->m_jobsMutex );

		for( JobSystem::Job& job : m_system->m_jobs )
		{
			if( job.ID == m_id )
				return &job;
		}

		return nullptr;
	}

	bool JobSystem::GetPendingJob( JobHandle& out_handle )
	{
		UpdateJobs();

		std::lock_guard<std::mutex> lock( m_jobsMutex );
		
		if( m_jobs.Size() == 0 )
			return false;	

		for( Job& job : m_jobs )
		{
			if( job.Status == JobStatus::Pending )
			{
				job.Status = JobStatus::Assigned;
				out_handle = JobHandle( *this, job );
				return true;
			}
		}

		return false;
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
		if( thread != nullptr )
		{
			// delegate to thread if coming from inside another job
			thread->WaitForCategory( category, timeout_ms );
			return;
		}

		do
		{
			if( !HasPendingJobs( category ) )
				return;

			UpdateJobs();
			
			::Sleep( 0 );
		} 
		while( timeout_ms == 0 || timer.Time() < timeout_ms );
	}
}