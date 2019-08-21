#include "PCH.h"
#include "JobSystem.h"

#include "DDAssertHelpers.h"

namespace dd
{
	static constexpr int MAX_JOBS = 4096;
	static constexpr uint MASK = MAX_JOBS - 1;
	
	thread_local Job g_jobs[MAX_JOBS];
	thread_local int g_currentJob = 0;

	struct JobQueue
	{
		JobQueue(JobSystem& system, std::thread::id tid);

		// Create a new job.
		Job* Allocate();

		// Get a new job to work on.
		Job* GetJob();

		std::thread::id Owner() const { return m_ownerThread; }

		// Push is always called from the owning thread.
		void Push(Job& job);

		// Clear the queue.
		void Clear();

	private:
		std::thread::id m_ownerThread;
		std::atomic<int> m_top;
		std::atomic<int> m_bottom;

		JobSystem* m_system { nullptr };

		static constexpr int MAX_PENDING = 1024;
		static constexpr int PENDING_MASK = MAX_PENDING - 1;
		Job* m_pending[MAX_PENDING];

		// Pop is always called from the owning thread.
		Job* Pop();

		// Steal is always called from another thread.
		Job* Steal();
	};

	JobSystem::JobSystem(uint threads)
	{
		DD_ASSERT(dd::IsMainThread());

		m_queues.resize(threads);
		m_queues[0] = new JobQueue(*this, std::this_thread::get_id());
		
		m_rng = Random32(0, threads - 1);

		for (uint i = 1; i < threads; ++i)
		{
			m_workers.emplace_back(&JobSystem::WorkerThread, this, i);
		}
	}

	void JobSystem::WorkerThread(uint this_index)
	{
		JobQueue* this_queue = new JobQueue(*this, std::this_thread::get_id());
		m_queues[this_index] = this_queue;

		while (!m_stop)
		{
			Job* job = this_queue->GetJob();
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

	Job* JobSystem::Allocate()
	{
		JobQueue* queue = FindQueue(std::this_thread::get_id());
		if (queue == nullptr)
		{
			DD_ASSERT(queue != nullptr, "Scheduling jobs on thread without queue!");
			return nullptr;
		}

		Job* job = queue->Allocate();
		return job;
	}

	Job* JobSystem::Create()
	{
		Job* job = Allocate();
		if (job == nullptr)
		{
			DD_ASSERT(job != nullptr);
			return nullptr;
		}

		job->m_parent = nullptr;
		job->m_pendingJobs = 1;

		return job;
	}
	
	void JobSystem::Schedule(Job* job)
	{
		if (job == nullptr)
		{
			DD_ASSERT(job != nullptr, "Scheduling a null job!");
			return;
		}

		JobQueue* queue = FindQueue(std::this_thread::get_id());
		if (queue == nullptr)
		{
			DD_ASSERT(queue != nullptr, "Scheduling jobs on thread without queue!");
			return;
		}

		queue->Push(*job);
	}

	void JobSystem::Wait(const Job* job)
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

	void JobSystem::WaitForAll(const std::vector<Job*>& jobs)
	{
		for (Job* job : jobs)
		{
			Wait(job);
		}
	}
	
	JobQueue* JobSystem::FindQueue(std::thread::id tid) const
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

	JobQueue* JobSystem::GetRandomQueue()
	{
		return m_queues[m_rng.Next()];
	}

	JobQueue::JobQueue(JobSystem& system, std::thread::id tid)
	{
		m_system = &system;
		m_ownerThread = tid;
		m_bottom = 0;
		m_top = 0;
		Clear();
	}

	void JobQueue::Clear()
	{
		std::memset(g_jobs, 0, sizeof(Job) * MAX_JOBS);
		std::memset(m_pending, 0, sizeof(Job*) * MAX_PENDING);
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
			if (queue != nullptr && queue != this)
			{
				job = queue->Steal();
			}
		}

		return job;
	}

	void JobQueue::Push(Job& job)
	{
		int bottom = m_bottom;
		m_pending[bottom & PENDING_MASK] = &job;
		m_bottom = bottom + 1;
	}

	Job* JobQueue::Pop()
	{
		DD_ASSERT(std::this_thread::get_id() == m_ownerThread);
		
		int bottom = m_bottom - 1;
		m_bottom.exchange(bottom);

		int top = m_top;

		if (bottom > top)
		{
			// empty queue
			m_bottom = top;
			return nullptr;
		}
		else
		{
			// more than one item
			Job* job = m_pending[bottom & PENDING_MASK];
			if (top != bottom)
			{
				// more than one job left
				return job;
			}

			if (!m_top.compare_exchange_strong(top, top + 1))
			{
				job = nullptr;
			}

			m_bottom = top + 1;
			return job;
		}

		return nullptr;
	}

	Job* JobQueue::Steal()
	{
		DD_ASSERT(std::this_thread::get_id() != m_ownerThread);
		DD_ASSERT(m_top < MAX_PENDING);
		
		int top = m_top;
		int bottom = m_bottom;

		if (top < bottom)
		{
			Job* job = m_pending[top & PENDING_MASK];
			if (!m_top.compare_exchange_strong(top, top + 1))
			{
				return nullptr;
			}

			return job;
		}

		return nullptr;
	}

	void Job::Run()
	{
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