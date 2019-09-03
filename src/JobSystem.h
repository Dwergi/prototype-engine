//
// JobSystem.h
// Copyright (C) Sebastian Nordgren 
// August 24th 2019
// Work-stealing jobsystem based on: https://blog.molecular-matters.com/2015/08/24/job-system-2-0-lock-free-work-stealing-part-1-basics/
// Also influenced by https://manu343726.github.io/2017-03-13-lock-free-job-stealing-task-system-with-modern-c/
//

#pragma once

#include "Random.h"
#include "Job.h"

namespace dd
{
	struct JobSystem
	{
		JobSystem(uint threads);

		template <typename TClass, typename... TArgs>
		Job* CreateMethod(TClass* this_ptr, void (TClass::* fn)(TArgs...), TArgs... arg);
		template <typename... TArgs>
		Job* Create(void (*fn)(TArgs...), TArgs... arg);
		Job* Create(const char* id);

		template <typename TClass, typename... TArgs>
		Job* CreateMethodChild(Job* parent, TClass* this_ptr, void (TClass::* fn)(TArgs...), TArgs... args);
		template <typename... TArgs>
		Job* CreateChild(Job* parent, void (*fn)(TArgs...), TArgs... args);
		Job* CreateChild(Job* parent, const char* id);

		void Schedule(Job* job);

		// Do some work while waiting for the given job.
		// Returns immediately if the job is finished.
		void Wait(const Job* job);

		// Do some work while waiting for the given jobs.
		// Returns immediately if all jobs are finished.
		void WaitForAll(const std::vector<Job*>& jobs);

		// Unconditionally work on one job, then return.
		void WorkOne();

		void Clear();

		uint Threads() const { return (uint) m_workers.size() + 1; }

	private:
		friend struct JobQueue;

		dd::Random32 m_rng;

		bool m_stop { false };
		std::vector<std::thread> m_workers;
		std::vector<JobQueue*> m_queues;

		JobQueue* GetRandomQueue();
		JobQueue* FindQueue(std::thread::id tid) const;
		void WorkerThread(uint index);
		
		Job* Allocate();
		bool IsOwnParent(Job* job) const;
	};
}

#include "JobSystem.inl"