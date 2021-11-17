#include "PCH.h"
#include "JobSystem.h"

#include "DDAssertHelpers.h"

namespace dd
{
	static constexpr bool USE_THREADS = false;

	static constexpr int MAX_JOBS = 4096;
	static constexpr int JOBS_MASK = MAX_JOBS - 1;
	
	thread_local Job t_jobs[MAX_JOBS];
	thread_local int t_currentJob = 0;
	thread_local bool t_isWaiting = false;

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
		std::atomic<int> m_top;
		std::atomic<int> m_bottom;

		static constexpr int MAX_PENDING = 2048;
		static constexpr int PENDING_MASK = MAX_PENDING - 1;
		Job* m_pending[MAX_PENDING];

		std::thread::id m_ownerThread;
		JobSystem* m_system { nullptr };

		// Pop is always called from the owning thread.
		Job* Pop();

		// Steal is always called from another thread.
		Job* Steal();
	};

	JobSystem::JobSystem(uint threads)
	{
		DD_ASSERT(dd::IsMainThread());

		if constexpr (!USE_THREADS)
		{
			threads = 1;
		}
		else
		{
			m_rng = Random32(0, threads - 1);
		}

		m_queues.resize(threads);
		m_queues[0] = new JobQueue(*this, std::this_thread::get_id());

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
		// if we're already waiting on this thread, then we can end up in a situation where we do work that this thread is already waiting for, but never complete
		if (t_isWaiting)
		{
			return;
		}

		t_isWaiting = true;

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

		t_isWaiting = false;
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
		if constexpr (!USE_THREADS)
		{
			return m_queues[0];
		}
		else
		{
			return m_queues[m_rng.Next()];
		}
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
		std::memset(t_jobs, 0, sizeof(Job) * MAX_JOBS);
		std::memset(m_pending, 0, sizeof(Job*) * MAX_PENDING);
		t_currentJob = 0;
	}

	Job* JobQueue::Allocate()
	{
		DD_ASSERT(std::this_thread::get_id() == m_ownerThread);

		Job* job = &t_jobs[t_currentJob];
		std::memset(job, 0, sizeof(Job));
		t_currentJob = (t_currentJob + 1) & JOBS_MASK;
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
		int bottom = m_bottom.load(std::memory_order_relaxed);
		int top = m_top.load(std::memory_order_acquire);

		const int size = bottom - top;
		if (size >= MAX_PENDING)
		{
			DD_ASSERT(size >= MAX_PENDING, "Job queue is full!");
			return;
		}

		m_pending[bottom & PENDING_MASK] = &job;

		std::atomic_thread_fence(std::memory_order_release);

		m_bottom.store(bottom + 1, std::memory_order_relaxed);
	}

	Job* JobQueue::Pop()
	{
		DD_ASSERT(std::this_thread::get_id() == m_ownerThread);
		
		int bottom = m_bottom.load(std::memory_order_relaxed) - 1;
		m_bottom.store(bottom, std::memory_order_relaxed);

		std::atomic_thread_fence(std::memory_order_seq_cst);

		int top = m_top.load(std::memory_order_relaxed);

		Job* job = nullptr;
		if (top <= bottom)
		{
			// more than one item
			job = m_pending[bottom & PENDING_MASK];

			if (top == bottom)
			{
				// last item just got stolen
				if (!m_top.compare_exchange_strong(top, top + 1,
						std::memory_order_seq_cst,
						std::memory_order_relaxed))
				{
					// failed race against steal operation
					job = nullptr;
				}

				m_bottom.store(bottom + 1, std::memory_order_relaxed);
			}
		}
		else
		{
			// empty queue
			m_bottom.store(bottom + 1, std::memory_order_relaxed);
		}

		return job;
	}

	Job* JobQueue::Steal()
	{
		DD_ASSERT(std::this_thread::get_id() != m_ownerThread);
		
		int top = m_top.load(std::memory_order_acquire);
		std::atomic_thread_fence(std::memory_order_seq_cst); 
		int bottom = m_bottom.load(std::memory_order_acquire);

		Job* job = nullptr;

		if (top < bottom)
		{
			job = m_pending[top & PENDING_MASK];

			if (!m_top.compare_exchange_strong(top, top + 1,
				std::memory_order_seq_cst,
				std::memory_order_relaxed))
			{
				return nullptr;
			}
		}

		return job;
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
		const int pending = --m_pendingJobs;
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