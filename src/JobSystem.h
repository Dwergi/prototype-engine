// JobSystem.h

#pragma once

#include <condition_variable>

namespace dd
{
	struct JobSystem
	{
		JobSystem( size_t threads );
		~JobSystem();

		template<class F, class... Args>
		auto Schedule(F&& f, Args&& ... args)
			->std::future<std::invoke_result_t<F, Args...>>;

		template<class F, class... Args>
		auto ScheduleCategory(uint64 category, F&& f, Args&& ... args)
			->std::future<std::invoke_result_t<F, Args...>>;

		// Wait for all tasks of the given category to be ready.
		void WaitForCategory(uint64 category);

		// Wait for all given futures to be ready.
		static void WaitForAll(const std::vector<std::future<void>>& futures);
		static void WaitForAll(const std::vector<std::shared_future<void>>& futures);

	private:
		std::vector<std::thread> m_workers;

		struct Task
		{
			uint64 Category;
			std::function<void()> Function;
		};
		std::vector<Task> m_tasks;

		std::mutex m_queueMutex;
		std::condition_variable m_condition;
		bool m_stop { false };
		bool m_useThreads { true };

		void WorkerFunction();
		void RunSingleTask();
	};

	// add new work item to the pool
	template<class F, class... Args>
	auto JobSystem::ScheduleCategory(uint64 category, F&& f, Args&&... args)
		-> std::future<std::invoke_result_t<F, Args...>>
	{
		using return_type = std::invoke_result_t<F, Args...>;

		// don't allow enqueuing after stopping the pool
		if( m_stop )
		{
			DD_ASSERT( false, "Scheduled job on stopped JobSystem" );
		}

		auto packaged_task = std::make_shared<std::packaged_task<return_type()>>(
			std::bind( std::forward<F>( f ), std::forward<Args>( args )... )
			);

		std::future<return_type> res = packaged_task->get_future();

		if (m_useThreads)
		{
			std::unique_lock<std::mutex> lock(m_queueMutex);

			Task task;
			task.Category = category;
			task.Function = [packaged_task]() { (*packaged_task)(); };
			m_tasks.emplace_back(task);

			m_condition.notify_one();
		}
		else
		{
			f( args... );
		}

		return res;
	}

	template<class F, class... Args>
	auto JobSystem::Schedule(F&& f, Args&& ... args)
		-> std::future<std::invoke_result_t<F, Args...>>
	{
		return ScheduleCategory(0, std::forward<F>(f), std::forward<Args>(args)...);
	}
}