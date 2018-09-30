// JobSystem.cpp - Modified from https://github.com/progschj/ThreadPool/blob/master/ThreadPool.h

#include "PCH.h"
#include "JobSystem.h"

#define JOBSYSTEM_NO_THREADS

namespace dd
{
	// the constructor just launches some amount of workers
	JobSystem::JobSystem( size_t threads )
	{
		if( threads == 0 )
		{
			m_useThreads = false;
			return;
		}

		for( size_t i = 0; i < threads; ++i )
		{
			m_workers.emplace_back( [this]() { WorkerFunction(); } ); 
		}
	}


	// the destructor joins all threads
	JobSystem::~JobSystem()
	{
		if( m_useThreads )
		{
			{
				std::unique_lock<std::mutex> lock( m_queueMutex );
				m_stop = true;
			}

			m_condition.notify_all();

			for( std::thread& worker : m_workers )
			{
				worker.join();
			}
		}
	}

	void JobSystem::WorkerFunction()
	{
		while( true )
		{
			std::function<void()> task;
			{
				std::unique_lock<std::mutex> lock( m_queueMutex );
				m_condition.wait( lock, [this] { return m_stop || !m_tasks.empty(); } );

				if( m_stop && m_tasks.empty() )
					return;

				task = std::move( m_tasks.front() );
				m_tasks.pop();
			}

			task();
		}
	}
}