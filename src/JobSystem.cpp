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

	void JobSystem::RunSingleTask()
	{
		Task task;
		{
			std::unique_lock<std::mutex> lock(m_queueMutex);
			m_condition.wait(lock, [this] { return m_stop || !m_tasks.empty(); });

			if (m_stop && m_tasks.empty())
				return;

			task = dd::pop_front(m_tasks);
		}
		task.Function();
	}

	void JobSystem::WorkerFunction()
	{
		while( true )
		{
			RunSingleTask();
		}
	}


	template <typename T>
	void WaitForAllInternal(const std::vector<T>& futures)
	{
		size_t valid = std::count_if(futures.begin(), futures.end(), [](const auto& f) { return f.valid(); });
		size_t ready = 0;

		while (ready < valid)
		{
			ready = 0;

			for (auto& f : futures)
			{
				if (f.valid())
				{
					std::future_status s = f.wait_for(std::chrono::microseconds(0));
					if (s == std::future_status::ready)
					{
						++ready;
					}
				}
			}

			if (ready < valid)
			{
				std::this_thread::yield();
			}
		}
	}

	void JobSystem::WaitForAll(const std::vector<std::future<void>>& futures)
	{
		WaitForAllInternal(futures);
	}

	void JobSystem::WaitForAll(const std::vector<std::shared_future<void>>& futures)
	{
		WaitForAllInternal(futures);
	}

	void JobSystem::WaitForCategory(uint64 category)
	{
		bool found_any = true;

		while (found_any)
		{
			found_any = false;

			{
				std::unique_lock<std::mutex> lock(m_queueMutex);
				for (const Task& task : m_tasks)
				{
					if (task.Category == category)
					{
						found_any = true;
						break;
					}
				}
			}

			RunSingleTask();
		}
	}
}