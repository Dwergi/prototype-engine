//
// JobThread.cpp - A worker thread that requests tasks and executes them continuously.
// Copyright (C) Sebastian Nordgren 
// October 29th 2015
//

#include "PrecompiledHeader.h"
#include "JobThread.h"

#include "JobSystem.h"

namespace dd
{
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

	void JobThread::Run()
	{
		while( !m_killed )
		{
			Function task;
			FunctionArgs args;
			if( m_owner.GetJob( task, args ) )
			{
				// we have a job, let's do it!
				Variable ret;
				task.Bind( args.Context );
				task( ret, args.Arguments.Data(), args.Arguments.Size() );
			}

			// let someone else have a go at the CPU
			::Sleep( 0 );
		}
	}
}
