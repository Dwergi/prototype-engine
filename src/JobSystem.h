//
// JobSystem.h - A manager for creating job threads which will process tasks in parallel over a number of threads.
// Copyright (C) Sebastian Nordgren 
// October 29th 2015
//

#pragma once

#include <mutex>

namespace std
{
	class thread;
}

namespace dd
{
	class Function;
	struct FunctionArgs;

	class JobThread;

	class JobSystem
	{
	public:

		explicit JobSystem( uint thread_count );
		~JobSystem();

		void Schedule( const Function& fn, const FunctionArgs& args );

		bool GetJob( Function& out_task, FunctionArgs& out_args );

	private:

		static const uint MAX_THREADS = 8;

		struct Job
		{
			Job( const Function& fn, const FunctionArgs& args );
			const Function& Function;
			const FunctionArgs& Args;
		};

		std::thread m_threads[MAX_THREADS];
		Vector<JobThread> m_workers;
		Vector<Job> m_jobs;

		std::mutex m_jobsMutex;

		void CreateWorkers( uint thread_count );
	};
}