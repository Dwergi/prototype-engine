//
// JobQueue.cpp
// Copyright (C) Sebastian Nordgren 
// August 27th 2019
// Based on: https://blog.molecular-matters.com/2015/08/24/job-system-2-0-lock-free-work-stealing-part-1-basics/
// Also influenced by: https://manu343726.github.io/2017-03-13-lock-free-job-stealing-task-system-with-modern-c/
//

#include "PCH.h"
#include "JobQueue.h"

#include "JobSystem.h"
#include "Job.h"

namespace dd
{
	static constexpr int MAX_JOBS = 4 * 1024;

	thread_local Job g_jobs[MAX_JOBS];
	thread_local int g_currentJob = 0;

	JobQueue::JobQueue(JobSystem& system, std::thread::id tid, uint index)
	{
		m_system = &system;
		m_ownerThread = tid;
		m_index = index;

		Clear();
	}

	void JobQueue::Clear()
	{
		DD_ASSERT(std::this_thread::get_id() == m_ownerThread);

		m_bottom = 0;
		m_top = 0;

		std::memset(g_jobs, 0, sizeof(Job) * MAX_JOBS);
		std::memset(m_pending, 0, sizeof(Job*) * MAX_PENDING);
		g_currentJob = 0;

		m_shouldClear = false;
	}

	Job* JobQueue::Allocate()
	{
		DD_ASSERT(std::this_thread::get_id() == m_ownerThread);
		DD_ASSERT(g_currentJob < MAX_JOBS);

		Job* job = &g_jobs[g_currentJob];
		++g_currentJob;

		job->Clear();
		return job;
	}

	Job* JobQueue::GetJob()
	{
		Job* job = Pop();
		if (job == nullptr)
		{
			JobQueue* queue = m_system->GetRandomQueue();
			if (queue != nullptr && queue != this)
			{
				job = queue->Steal();
			}
		}

		return job;
	}

	void JobQueue::Push(Job& job)
	{
		DD_ASSERT(std::this_thread::get_id() == m_ownerThread);

		int bottom = m_bottom.load(std::memory_order_acquire);

		DD_ASSERT(bottom < MAX_PENDING);

		m_pending[bottom] = &job;
		m_bottom.store(bottom + 1, std::memory_order_release);

		DD_DIAGNOSTIC("PUSH: Job %d, Queue %d.\n", bottom, m_index);
	}

	Job* JobQueue::Pop()
	{
		DD_ASSERT(std::this_thread::get_id() == m_ownerThread);

		int bottom = m_bottom.load(std::memory_order_acquire);
		
		if (bottom > 0)
		{
			bottom = bottom - 1;
			m_bottom.store(bottom, std::memory_order_release);
		}

		std::atomic_thread_fence(std::memory_order_release);

		int top = m_top.load(std::memory_order_acquire);

		if (top > bottom)
		{
			// empty queue
			m_bottom.store(top, std::memory_order_release);
			return nullptr;
		}

		Job* job = m_pending[bottom];
		if (top != bottom)
		{
			// more than one job left
			DD_DIAGNOSTIC("POP: Queue %d, Job %d.\n", m_index, bottom);
			return job;
		}

		// last item in queue
		if (!m_top.compare_exchange_strong(top, top + 1, std::memory_order_acq_rel))
		{
			// failed race
			job = nullptr;
		}

		m_bottom.store(top + 1, std::memory_order_release);

		DD_DIAGNOSTIC("POP: Queue %d, Job %d.\n", m_index, bottom);
		return job;
	}

	Job* JobQueue::Steal()
	{
		DD_ASSERT(std::this_thread::get_id() != m_ownerThread);

		int top = m_top.load(std::memory_order_acquire);
		DD_ASSERT(top < MAX_PENDING);

		std::atomic_thread_fence(std::memory_order_acquire);

		int bottom = m_bottom.load(std::memory_order_acquire);

		if (top < bottom)
		{
			Job* job = m_pending[top];
			if (!m_top.compare_exchange_weak(top, top + 1, std::memory_order_acq_rel))
			{
				return nullptr;
			}

			DD_DIAGNOSTIC("STEAL: Queue %d, Job %d.\n", m_index, top);
			return job;
		}

		return nullptr;
	}
}