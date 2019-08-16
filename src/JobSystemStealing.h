// Work-stealing jobsystem based on: https://blog.molecular-matters.com/2015/08/24/job-system-2-0-lock-free-work-stealing-part-1-basics/

#pragma once

#include <atomic>

#include "Random.h"

namespace dd
{
	struct JobSystemStealing;

	struct Job
	{
		void Run();
		bool IsFinished() const;

	private:
		friend struct JobSystemStealing;

		void Finish();

		void(*m_function)(Job*) { nullptr };
		Job* m_parent { nullptr };
		std::atomic<int> m_pendingJobs { 0 };

		static constexpr size_t MaxSize = 64;
		static constexpr size_t PaddingBytes = MaxSize - (sizeof(m_function) + sizeof(m_parent) + sizeof(m_pendingJobs));

		byte m_argument[PaddingBytes];
	};

	struct JobQueue
	{
		JobQueue(JobSystemStealing& system, std::thread::id tid);

		Job* Allocate();
		void Clear();
		Job* GetJob();

		std::thread::id Owner() const { return m_ownerThread; }

	private:
		std::thread::id m_ownerThread;
		std::atomic<int> m_head;
		std::atomic<int> m_tail;

		JobSystemStealing* m_system { nullptr };

		static constexpr int MAX_PENDING = 1024;
		static constexpr int PENDING_MASK = MAX_PENDING - 1;
		Job* m_pending[MAX_PENDING];

		// Pop is always called from the owning thread.
		Job* Pop();

		// Schedule is always called from the owning thread.
		void Push(Job& job);

		// Steal is always called from another thread.
		Job* Steal();
	};

	struct JobSystemStealing
	{
		JobSystemStealing(uint threads);

		template <typename Fn, typename Arg>
		Job* Schedule(Fn&& fn, Arg arg)
		{
			static_assert(sizeof(Arg) < Job::Padding);

			JobQueue* queue = GetQueue(std::this_thread::get_id());
			DD_ASSERT(queue != nullptr, "Scheduling jobs on thread without queue!");

			if (queue == nullptr)
			{
				return nullptr;
			}

			Job* job = queue->Allocate();
			if (job == nullptr)
			{
				return nullptr;
			}

			job->m_parent = nullptr;
			job->m_pendingJobs = 1;
			std::memcpy(job->m_argument, sizeof(Arg), arg);

			job->Function = [](Job* job)
			{
				Arg* arg = reinterpret_cast<Arg*>(job->m_argument);
				fn(*arg);
			};

			queue->Push(*job);
			return job;
		}

		void Wait(const Job* job);

	private:
		friend struct JobQueue;

		dd::Random32 m_rng;

		bool m_stop { false };
		std::vector<std::thread> m_workers;
		std::vector<JobQueue*> m_queues;

		JobQueue* GetRandomQueue();
		JobQueue* FindQueue(std::thread::id tid) const;
		void WorkerThread(uint index);
	};
}