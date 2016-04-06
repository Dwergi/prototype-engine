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
		m_owner( std::move( other.m_owner ) )
	{
		std::swap( m_pendingJobs, other.m_pendingJobs );
	}

	JobThread::JobThread( JobSystem& owner )
		: m_killed( false ),
		m_owner( owner )
	{
		
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

			Variable ret;
			Function func( job->Func );

			func.Bind( job->Args.Context );

			Vector<Variable> vars = job->Args.GetArguments();
			func( ret, vars.Data(), job->Args.ArgCount() );

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
			m_owner.UpdateWaitingJobs();

			if( current.GetJob()->Status == JobSystem::JobStatus::DoneWaiting )
				return;

			// find more work to process in the meantime
			ProcessJob();

			::Sleep( 0 );
		} while( timeout_ms == 0 || timer.Time() < timeout_ms );
	}
}
