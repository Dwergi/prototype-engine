//
// JobSystem.h - A manager for creating job threads which will process tasks in parallel over a number of threads.
// Copyright (C) Sebastian Nordgren 
// October 29th 2015
//

#pragma once

#include <mutex>
#include <functional>

namespace std
{
	class thread;
}

namespace dd
{
	class JobHandle;

	class JobSystem
	{
	public:

		explicit JobSystem( uint thread_count );
		~JobSystem();

		JobSystem() = delete;
		JobSystem( const JobSystem& ) = delete;
		JobSystem& operator=( const JobSystem& ) = delete;

		//
		// Schedule a function to be run at some point.
		// Category allows jobs to be grouped and waited on as logical units.
		//
		void Schedule( const Function& fn, const char* category );
		void Schedule( const std::function<void()>& fn, const char* category );
		void WaitForCategory( const char* category, uint timeout_ms = 0 );

		BASIC_TYPE( JobSystem )

	private:

		static const uint MAX_THREADS = 8;

		enum class JobStatus
		{
			Pending,
			Assigned,
			Running,
			Waiting,
			DoneWaiting,
			Done,
			Invalid
		};

		//
		// A job structure.
		//
		struct Job
		{
			Job( const Job& other );
			Job( const std::function<void()>& fn, const char* category, uint id );

			std::function<void()> Func;
			String128 Category;
			String128 WaitingOn;
			JobStatus Status;
			uint ID;
		};

		friend class JobThread;
		friend class JobHandle;

		std::thread m_threads[MAX_THREADS];
		Vector<JobThread*> m_workers;
		Vector<Job> m_jobs;
		uint m_jobID;

		std::mutex m_jobsMutex;

		void CreateWorkers( uint thread_count );

		bool GetPendingJob( JobHandle& out_job );

		JobThread* FindCurrentWorker() const;

		bool HasPendingJobs( const char* category );
		void UpdateJobs();
	};

	//
	// A handle to refer to a single pending job.
	//
	class JobHandle
	{
	public:
		JobHandle();
		JobHandle( JobSystem& system, JobSystem::Job& job );

		JobSystem::Job* GetJob() const;
		void SetStatus( JobSystem::JobStatus status ) const;
		
	private:
		JobSystem* m_system;
		uint m_id;
	};
}