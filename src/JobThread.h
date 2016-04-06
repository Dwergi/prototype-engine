//
// JobThread.h - A worker thread that requests tasks and executes them continuously.
// Copyright (C) Sebastian Nordgren 
// October 29th 2015
//

#pragma once

#include "JobSystem.h"

namespace dd
{
	class JobThread
	{
	public:

		JobThread( JobThread&& other );
		JobThread( JobSystem& m_owner );
		~JobThread();

		JobThread( const JobThread& ) = delete;

		void Kill();

	private:

		bool m_killed;
		JobSystem& m_owner;
		Vector<JobHandle> m_pendingJobs;

		void Run();
		void ProcessJob();

		void WaitForCategory( const char* category, uint timeout_ms );
		
		friend class JobSystem;
	};
}
