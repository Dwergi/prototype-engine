// JobSystem.h - Modified from https://github.com/progschj/ThreadPool/blob/master/ThreadPool.h

#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

//#define JOBSYSTEM_NO_THREADS

namespace dd
{
	class JobSystem {
	public:
		JobSystem( size_t threads );
		~JobSystem();

		template<class F, class... Args>
		auto Schedule( F&& f, Args&&... args )
			-> std::future<typename std::result_of<F( Args... )>::type>;
	private:
		
		std::vector<std::thread> m_workers;
		std::queue<std::function<void()>> m_tasks;

		std::mutex m_queueMutex;
		std::condition_variable m_condition;
		bool m_stop { false };

		void WorkerFunction();
	};

	// add new work item to the pool
	template<class F, class... Args>
	auto JobSystem::Schedule( F&& f, Args&&... args )
		-> std::future<typename std::result_of<F( Args... )>::type>
	{
		using return_type = typename std::result_of<F( Args... )>::type;

		auto task = std::make_shared< std::packaged_task<return_type()> >(
			std::bind( std::forward<F>( f ), std::forward<Args>( args )... )
			);

		std::future<return_type> res = task->get_future();
#ifndef JOBSYSTEM_NO_THREADS
		{
			std::unique_lock<std::mutex> lock( m_queueMutex );

			// don't allow enqueueing after stopping the pool
			if( m_stop )
				DD_ASSERT( "Scheduled job on stopped JobSystem" );

			m_tasks.emplace( [task]() { (*task)(); } );
		}
		m_condition.notify_one();
#else
		(*task)();
#endif
		return res;
	}
}