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
	}

	JobThread::JobThread( JobSystem& owner, const char* name )
		: m_killed( false ),
		m_owner( owner ),
		m_name( name )
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
		DD_PROFILE_SCOPED( JobThread_ProcessJob );

		JobSystem::Job* job;
		if( m_owner.WaitForJob( job ) )
		{
			DD_PROFILE_SCOPED( JobThread_RunJob );

			job->Func();

			m_owner.MarkDone( *job );
		}
	}

	void JobThread::Run()
	{
		DD_PROFILE_THREAD_NAME( m_name.c_str() );

		while( !m_killed )
		{
			ProcessJob();
		}
	}
}
