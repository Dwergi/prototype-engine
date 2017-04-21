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
#include <chrono>

//#define NO_JOB_SCHEDULING // Uncomment to run all jobs on the scheduling thread.

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
	JobSystem::Job::Job( const Job& other ) :
		Func( other.Func ),
		Category( other.Category ),
		Status( other.Status ),
		ID( other.ID )
	{

	}

	JobSystem::Job::Job( const std::function<void()>& fn, JobSystem::Category* category, uint id ) :
		Func( fn ),
		Category( category ),
		Status( JobStatus::Pending ),
		ID( id )
	{

	}

	JobSystem::Category::Category() :
		Pending( 0 )
	{

	}

	JobSystem::Category::Category( const char* name ) :
		Pending( 0 ),
		Name( name )
	{

	}

	JobSystem::Job::~Job()
	{
		ID = 0;
		Status = JobStatus::Free;
		Category = nullptr;
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
		}

		m_jobsPending.notify_all(); // signal workers to wake up

		for( uint i = 0; i < m_workers.Size(); ++i )
		{
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

	void JobSystem::Schedule( const Function& fn, const char* category_name )
	{
		DD_ASSERT( fn.Signature()->ArgCount() == 0 );
		DD_ASSERT( !fn.IsMethod() || fn.Context().IsValid() ); // must be free or already be bound

		Schedule( [fn]() { fn(); }, category_name );
	}

	void JobSystem::Schedule( const std::function<void()>& fn, const char* category_name )
	{
		DD_PROFILE_SCOPED( JobSystem_Schedule );

#ifndef NO_JOB_SCHEDULING
		std::unique_lock<std::mutex> lock( m_jobsMutex );

		Category* category = FindCategory( category_name );
		if( category == nullptr )
		{
			DD_ASSERT( m_categories.Size() < m_categories.Capacity(), "JobSystem categories list out of space, allocate more!" );

			if( m_categories.Size() < m_categories.Capacity() )
			{
				category = m_categories.Data() + m_categories.Size();
				new (category) Category( category_name );
				m_categories.SetSize( m_categories.Size() + 1 );
			}
		}

		{
			std::lock_guard<std::mutex> lock( category->Mutex );
			++category->Pending;
		}

		// find a free slot
		for( uint i = 0; i < m_jobs.Size(); ++i )
		{
			if( m_jobs[i].Status == JobStatus::Free )
			{
				new (&m_jobs[i]) Job( fn, category, i );
				m_pendingJobs.Add( i );

				lock.unlock();
				m_jobsPending.notify_one();
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

			lock.unlock();
			m_jobsPending.notify_one();
			return;
		}
#else
		fn();
#endif
	}

	void JobSystem::MarkDone( const Job& job )
	{
		std::lock_guard<std::mutex> lock( m_jobsMutex );

		Category* category = job.Category;
		int id = job.ID;
		m_jobs[id].~Job();

		if( category != nullptr )
		{
			std::unique_lock<std::mutex> lock( category->Mutex );
			if( --category->Pending == 0 )
			{
				lock.unlock();
				category->Condition.notify_one();
			}

			DD_ASSERT( category->Pending >= 0, "Should never go negative in pending jobs!" );
		}
	}

	bool JobSystem::WaitForJob( Job*& out_job )
	{
		out_job = nullptr;

		std::unique_lock<std::mutex> lock( m_jobsMutex );
		m_jobsPending.wait( lock, [this] { return true; } );

		if( m_pendingJobs.Size() > 0 )
		{
			out_job = &m_jobs[m_pendingJobs[0]];
			m_pendingJobs.RemoveOrdered( 0 );

			return true;
		}

		// woken up for some other reason, go find out why
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

	JobSystem::Category* JobSystem::FindCategory( const char* category_name ) const
	{
		for( Category& category : m_categories )
		{
			if( category.Name == category_name )
				return &category;
		}

		return nullptr;
	}

	void JobSystem::WaitForCategory( const char* category_name, uint timeout_ms )
	{
		JobThread* thread = FindCurrentWorker();
		DD_ASSERT_ERROR( thread == nullptr, "Cannot wait for categories inside jobs!" );
		
		Category* category = FindCategory( category_name );
		if( category != nullptr )
		{
			std::unique_lock<std::mutex> lock( category->Mutex );

			DD_PROFILE_START( JobSystem_WaitForCategory );

			if( timeout_ms > 0 )
			{
				category->Condition.wait_for( lock, std::chrono::milliseconds( timeout_ms ), [category] { return category->Pending == 0; } );
			}
			else
			{
				category->Condition.wait( lock, [category] { return category->Pending == 0; } );
			}

			DD_PROFILE_END();
			DD_ASSERT_ERROR( category->Pending == 0, "Category still has pending jobs!" );
		}
	}
}