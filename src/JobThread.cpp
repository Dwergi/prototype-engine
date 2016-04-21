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
		JobSystem::Job* job;
		if( m_owner.GetPendingJob( job ) )
		{
			job->Func();
			m_owner.MarkDone( *job );
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
}
