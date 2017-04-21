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
		static const uint JOB_QUEUE_SIZE = 1024;

		enum class JobStatus
		{
			Pending,
			Assigned,
			Done,
			Free
		};

		struct Category
		{
			Category();
			Category( const char* name );
			Category( const Category& ) = delete;

			String128 Name;
			int Pending;
			std::condition_variable Condition;
			std::mutex Mutex;
		};

		//
		// A job structure.
		//
		struct Job
		{
			Job( const Job& other );
			Job( const std::function<void()>& fn, Category* category, uint id );
			~Job();

			JobStatus Status;
			Category* Category;
			uint ID;
			std::function<void()> Func;
		};

		friend class JobThread;

		std::thread m_threads[MAX_THREADS];

		Array<Category, 32> m_categories;
		Vector<JobThread*> m_workers;
		Vector<Job> m_jobs;
		Vector<int> m_pendingJobs;

		std::mutex m_jobsMutex;
		std::condition_variable m_jobsPending;

		void CreateWorkers( uint thread_count );
		JobThread* FindCurrentWorker() const;

		Category* FindCategory( const char* category_name ) const;

		// JobThread interface
		bool WaitForJob( Job*& out_job );
		void MarkDone( const Job& job );
	};
}