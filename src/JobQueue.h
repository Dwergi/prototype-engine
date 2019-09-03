//
// JobQueue.h
// Copyright (C) Sebastian Nordgren 
// August 27th 2019
// Based on: https://blog.molecular-matters.com/2015/08/24/job-system-2-0-lock-free-work-stealing-part-1-basics/
// Also influenced by https://manu343726.github.io/2017-03-13-lock-free-job-stealing-task-system-with-modern-c/
//

#pragma once

#include <atomic>

namespace dd
{
	struct Job;
	struct JobSystem;

#define DD_LOCK_FREE_JOBQUEUE 0

	struct JobQueue
	{
		JobQueue(JobSystem& system, std::thread::id tid, uint index);

		// Create a new job.
		Job* Allocate();

		// Get a new job to work on.
		Job* GetJob();

		// The owner thread of this queue.
		std::thread::id Owner() const { return m_ownerThread; }

		// Push is always called from the owning thread.
		void Push(Job* job);

		// Clear the queue in the worker thread.
		void ScheduleClear() { m_shouldClear = true; }

		// Should we clear the queue now?
		bool ShouldClear() const { return m_shouldClear; }

		// Clear the queue.
		void Clear();

	private:
		std::thread::id m_ownerThread;

#if DD_LOCK_FREE_JOBQUEUE
		std::atomic<int> m_top;
		std::atomic<int> m_bottom;
#else
		std::mutex m_mutex;
		int m_top;
		int m_bottom;
#endif

		bool m_shouldClear { false };
		JobSystem* m_system { nullptr };
		uint m_index { 0 };

		static constexpr int MAX_PENDING = 2 * 1024;
		Job* m_pending[MAX_PENDING];

		// Pop is always called from the owning thread.
		Job* Pop();

		// Steal is always called from another thread.
		Job* Steal();
	};
}