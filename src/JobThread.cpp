//
// JobThread.cpp - A worker thread that requests tasks and executes them continuously.
// Copyright (C) Sebastian Nordgren 
// October 29th 2015
//

#include "PrecompiledHeader.h"
#include "JobThread.h"

#include "JobSystem.h"
#include "Timer.h"

namespace dd
{
	JobThread::JobThread( JobThread&& other )
		: m_killed( other.m_killed ),
		m_owner( std::move( other.m_owner ) ),
		m_name( other.m_name )
	{
		DD_PROFILE_THREAD_NAME( m_name.c_str() );

		std::swap( m_pendingJobs, other.m_pendingJobs );
	}

	JobThread::JobThread( JobSystem& owner, const char* name )
		: m_killed( false ),
		m_owner( owner ),
		m_name( name )
	{
		DD_PROFILE_THREAD_NAME( m_name.c_str() );
	}

	JobThread::~JobThread()
	{

	}

	void JobThread::Kill()
	{
		m_killed = true;
	}

	void JobThread::ProcessJob()
	{
		JobHandle handle;
		if( m_owner.GetPendingJob( handle ) )
		{
			m_pendingJobs.Add( handle );

			// we have a job, let's do it!
			JobSystem::Job* job = handle.GetJob();
			job->Status = JobSystem::JobStatus::Running;

			job->Func();

			job->Status = JobSystem::JobStatus::Done;

			m_pendingJobs.Pop();
		}
	}

	void JobThread::Run()
	{
		while( !m_killed )
		{
			ProcessJob();

			// let someone else have a go at the CPU
			::Sleep( 0 );
		}
	}

	void JobThread::WaitForCategory( const char* category, uint timeout_ms )
	{
		Timer timer;
		timer.Start();
		
		DD_ASSERT( m_pendingJobs.Size() > 0 );

		JobHandle current = m_pendingJobs[m_pendingJobs.Size() - 1];
		current.GetJob()->Status = JobSystem::JobStatus::Waiting;
		current.GetJob()->WaitingOn = String128( category );

		do
		{
			m_owner.UpdateJobs();

			if( current.GetJob()->Status == JobSystem::JobStatus::DoneWaiting )
				return;

			// find more work to process in the meantime
			ProcessJob();

			::Sleep( 0 );
		} while( timeout_ms == 0 || timer.Time() < timeout_ms );
	}
}
