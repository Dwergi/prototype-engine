#include "PCH.h"
#include "JobSystemStealing.h"

#include "DDAssertHelpers.h"

#define COMPILER_BARRIER() _ReadWriteBarrier()

namespace dd
{
	static constexpr int MAX_JOBS = 4096;
	static constexpr uint MASK = MAX_JOBS - 1;
	
	// should be thread_local?
	thread_local Job g_jobs[MAX_JOBS];
	thread_local int g_currentJob = 0;

	JobSystemStealing::JobSystemStealing(uint threads)
	{
		DD_ASSERT(dd::IsMainThread());

		m_queues.resize(threads);
		m_queues[0] = new JobQueue(*this, std::this_thread::get_id());
		
		m_rng = Random32(0, threads - 1);

		for (uint i = 1; i < threads; ++i)
		{
			m_workers.emplace_back([this, i]() { WorkerThread(i); });
		}
	}

	void JobSystemStealing::WorkerThread(uint this_index)
	{
		m_queues[this_index] = new JobQueue(*this, std::this_thread::get_id());

		while (!m_stop)
		{
			Job* job = m_queues[this_index]->GetJob();
			if (job != nullptr)
			{
				job->Run();
			}
			else
			{
				std::this_thread::yield();
			}
		}
	}

	void JobSystemStealing::Wait(const Job* job)
	{
		JobQueue* queue = FindQueue(std::this_thread::get_id());

		while (!job->IsFinished())
		{
			Job* work = queue->GetJob();
			if (work != nullptr)
			{
				work->Run();
			}
			else
			{
				std::this_thread::yield();
			}
		}
	}
	
	JobQueue* JobSystemStealing::FindQueue(std::thread::id tid) const
	{
		for (JobQueue* queue : m_queues)
		{
			if (queue->Owner() == tid)
			{
				return queue;
			}
		}

		return nullptr;
	}

	JobQueue* JobSystemStealing::GetRandomQueue()
	{
		return m_queues[m_rng.Next()];
	}

	JobQueue::JobQueue(JobSystemStealing& system, std::thread::id tid)
	{
		m_system = &system;
		m_ownerThread = tid;
		m_tail = 0;
		m_head = 0;
		Clear();
	}

	void JobQueue::Clear()
	{
		std::memset(g_jobs, 0, sizeof(Job) * MAX_JOBS);
		g_currentJob = 0;
	}

	Job* JobQueue::Allocate()
	{
		DD_ASSERT(std::this_thread::get_id() == m_ownerThread);

		Job* job = &g_jobs[g_currentJob];
		std::memset(job, 0, sizeof(Job));
		++g_currentJob;
		return job;
	}

	Job* JobQueue::GetJob()
	{
		Job* job = Pop();
		if (job == nullptr)
		{
			JobQueue* queue = m_system->GetRandomQueue();
			if (queue != this)
			{
				job = queue->Steal();
			}
		}

		return job;
	}


	Job* JobQueue::Pop()
	{
		DD_ASSERT(std::this_thread::get_id() == m_ownerThread);
		
		int tail = m_tail - 1;
		m_tail.exchange(tail);

		int head = m_head;
		if (head >= tail)
		{
			m_tail = head;
			return nullptr;
		}

		if (tail > (head + 1))
		{
			// more than one item
			Job* job = m_pending[tail & PENDING_MASK];
		}

		return nullptr;
	}

	void JobQueue::Push(Job& job)
	{
		int tail = m_tail;
		m_pending[tail & PENDING_MASK] = &job;
		m_tail = tail + 1;
	}

	Job* JobQueue::Steal()
	{
		DD_ASSERT(std::this_thread::get_id() != m_ownerThread);
		DD_ASSERT(m_head < MAX_PENDING);
		
		int head = m_head;

		COMPILER_BARRIER();

		int tail = m_tail;

		if (head == tail)
		{
			return nullptr;
		}

		if (!m_head.compare_exchange_strong(head, head + 1))
		{
			return nullptr;
		}

		Job* job = m_pending[head & MASK];
		return job;
	}

	void Job::Run()
	{
		DD_ASSERT(m_function != nullptr);

		if (m_function != nullptr)
		{
			m_function(this);
		}

		Finish();
	}

	void Job::Finish()
	{
		int pending = --m_pendingJobs;
		if (pending == 0 && m_parent != nullptr)
		{
			m_parent->Finish();
		}
	}

	bool Job::IsFinished() const
	{
		return m_pendingJobs == 0;
	}
}