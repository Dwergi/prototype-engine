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

#define DD_TRACK_JOBS 1

#if DD_TRACK_JOBS
	enum class JobOp
	{
		None = 0,
		Push,
		Pop,
		Steal
	};

	struct JobChange
	{
		int Queue { 0 };
		int Job { 0 };
		JobOp Op { JobOp::None };
	};

	static JobChange g_jobTracking[MAX_JOBS];
	static std::atomic<int> g_jobTrackingHead = 0;

	static void OnPush(int queue, int job)
	{
		int head = g_jobTrackingHead++;
		g_jobTracking[head].Queue = queue;
		g_jobTracking[head].Job = job;
		g_jobTracking[head].Op = JobOp::Push;

		DD_DIAGNOSTIC("PUSH: Queue %d, Job %d.\n", queue, job);
	}

	static void OnPop(int queue, int job)
	{
		int head = g_jobTrackingHead++;
		g_jobTracking[head].Queue = queue;
		g_jobTracking[head].Job = job;
		g_jobTracking[head].Op = JobOp::Pop;

		DD_DIAGNOSTIC("POP: Queue %d, Job %d.\n", queue, job);
	}

	static void OnSteal(int queue, int job)
	{
		int head = g_jobTrackingHead++;
		g_jobTracking[head].Queue = queue;
		g_jobTracking[head].Job = job;
		g_jobTracking[head].Op = JobOp::Steal;

		DD_DIAGNOSTIC("STEAL: Queue %d, Job %d.\n", queue, job);
	}
	
#else
	static void OnPush(int queue, int job) {}
	static void OnPop(int queue, int job) {}
	static void OnSteal(int queue, int job) {}
#endif

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

#if DD_TRACK_JOBS
		std::memset(g_jobTracking, 0, sizeof(JobChange) * MAX_JOBS);
		g_jobTrackingHead = 0;
#endif
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

#if DD_LOCK_FREE_JOBQUEUE == 1
	
	void JobQueue::Push(Job* job)
	{
		DD_ASSERT(std::this_thread::get_id() == m_ownerThread);

		int bottom = m_bottom.load(std::memory_order_acquire);
		m_pending[bottom] = job;

		m_bottom.store(bottom + 1, std::memory_order_release);

		OnPush(m_index, bottom);
	}

	Job* JobQueue::Pop()
	{
		DD_ASSERT(std::this_thread::get_id() == m_ownerThread);

		int bottom = m_bottom.fetch(std::memory_order_acquire);
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
			OnPop(m_index, bottom);

			return job;
		}

		// last item in queue
		if (!m_top.compare_exchange_strong(top, top + 1, std::memory_order_acq_rel))
		{
			// failed race
			job = nullptr;
		}

		m_bottom.store(top + 1, std::memory_order_release);

		OnPop(m_index, bottom);
		return job;
	}

	Job* JobQueue::Steal()
	{
		DD_ASSERT(std::this_thread::get_id() != m_ownerThread);

		int top = m_top.load(std::memory_order_acquire);
		int bottom = m_bottom.load(std::memory_order_acquire);

		if (top >= bottom)
		{
			return nullptr;
		}

		Job* job = m_pending[top];
		if (m_top.compare_exchange_strong(top, top + 1, std::memory_order_acq_rel))
		{
			return nullptr;
		}

		OnSteal(m_index, top);
		return job;
	}

#else

	void JobQueue::Push(Job* job)
	{
		DD_ASSERT(std::this_thread::get_id() == m_ownerThread);
		DD_ASSERT(m_bottom < MAX_PENDING);

		std::lock_guard<std::mutex> lock(m_mutex);

		m_pending[m_bottom] = job;
		m_bottom++;

		OnPush(m_index, m_bottom - 1);
	}

	Job* JobQueue::Pop()
	{
		DD_ASSERT(std::this_thread::get_id() == m_ownerThread);

		std::lock_guard<std::mutex> lock(m_mutex);

		if (m_bottom <= m_top)
		{
			return nullptr;
		}

		m_bottom--;
		OnPop(m_index, m_bottom);
		return m_pending[m_bottom];
	}

	Job* JobQueue::Steal()
	{
		DD_ASSERT(std::this_thread::get_id() != m_ownerThread);
		std::lock_guard<std::mutex> lock(m_mutex);
		DD_ASSERT(m_top < MAX_PENDING);

		if (m_bottom <= m_top)
		{
			return nullptr;
		}

		m_top++;
		OnPop(m_index, m_top - 1);
		return m_pending[m_top - 1];
	}
#endif
}