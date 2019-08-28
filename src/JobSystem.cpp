#include "PCH.h"
#include "JobSystem.h"

#include "DDAssertHelpers.h"
#include "JobQueue.h"

#include <processthreadsapi.h>

namespace dd
{
	static constexpr bool USE_THREADS = true;
	static constexpr uint MAX_THREADS = 0;

	JobSystem::JobSystem(uint threads)
	{
		DD_ASSERT(dd::IsMainThread());

		if constexpr (!USE_THREADS)
		{
			threads = 1;
		}
		else
		{
			if (MAX_THREADS != 0 && MAX_THREADS < threads)
			{
				threads = MAX_THREADS;
			}
			m_rng = Random32(0, threads - 1);
		}

		m_queues.resize(threads);
		m_queues[0] = new JobQueue(*this, std::this_thread::get_id(), 0);

		for (uint i = 1; i < threads; ++i)
		{
			m_workers.emplace_back(&JobSystem::WorkerThread, this, i);
		}
	}

	void JobSystem::WorkerThread(uint this_index)
	{
		JobQueue* this_queue = new JobQueue(*this, std::this_thread::get_id(), this_index);
		m_queues[this_index] = this_queue;

#ifdef WIN32
		wchar_t name[64];
		swprintf(name, 64, L"Job Worker %d", this_index);
		SetThreadDescription(GetCurrentThread(), name);
#endif

		while (!m_stop)
		{
			if (this_queue->ShouldClear())
			{
				this_queue->Clear();
			}

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
		return CreateChild(nullptr);
	}

	bool JobSystem::IsOwnParent(Job* job) const
	{
		if (job == nullptr)
		{
			return false;
		}

		dd::Array<Job*, 8> jobs;
		jobs.Add(job);

		Job* parent = job->m_parent;
		while (parent != nullptr)
		{
			if (jobs.Contains(parent))
			{
				return true;
			}

			jobs.Add(parent);
			parent = parent->m_parent;
		}

		return false;
	}

	Job* JobSystem::CreateChild(Job* parent)
	{
		DD_ASSERT_SLOW(!IsOwnParent(parent));

		Job* job = Allocate();
		DD_ASSERT(job != nullptr);

		job->m_pendingJobs = 1;
		if (parent != nullptr)
		{
			DD_ASSERT(parent->m_pendingJobs > 0);

			job->SetParent(parent);
		}

		return job;
	}
	
	void JobSystem::Schedule(Job* job)
	{
		DD_ASSERT(job != nullptr, "Scheduling a null job!");
		DD_ASSERT(job->m_pendingJobs >= 1);

		JobQueue* queue = FindQueue(std::this_thread::get_id());
		DD_ASSERT(queue != nullptr, "Scheduling jobs on thread without queue!");

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
		if constexpr (!USE_THREADS)
		{
			return m_queues[0];
		}
		else
		{
			return m_queues[m_rng.Next()];
		}
	}

	void JobSystem::Clear()
	{
		DD_ASSERT(dd::IsMainThread());

		m_queues[0]->Clear();

		for (JobQueue* queue : m_queues)
		{
			queue->ScheduleClear();
		}
	}
}