// JobSystem.cpp - Modified from https://github.com/progschj/ThreadPool/blob/master/ThreadPool.h

#include "PrecompiledHeader.h"
#include "JobSystem.h"

namespace dd
{

	// the constructor just launches some amount of workers
	JobSystem::JobSystem( size_t threads )
	{
#ifndef JOBSYSTEM_NO_THREADS
		for( size_t i = 0; i < threads; ++i )
		{
			m_workers.emplace_back( [this]() { WorkerFunction(); } ); 
		}
#endif
	}


	// the destructor joins all threads
	JobSystem::~JobSystem()
	{
#ifndef JOBSYSTEM_NO_THREADS
		{
			std::unique_lock<std::mutex> lock( m_queueMutex );
			m_stop = true;
		}

		m_condition.notify_all();

		for( std::thread& worker : m_workers )
		{
			worker.join();
		}
#endif
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