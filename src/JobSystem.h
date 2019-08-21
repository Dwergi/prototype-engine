// Work-stealing jobsystem based on: https://blog.molecular-matters.com/2015/08/24/job-system-2-0-lock-free-work-stealing-part-1-basics/

#pragma once

#include <atomic>

#include "Random.h"

namespace dd
{
	struct JobSystem;

	struct Job
	{
		void Run();
		bool IsFinished() const;

	private:
		friend struct JobSystem;

		void Finish();

		void (*m_function)(Job*);
		Job* m_parent { nullptr };
		std::atomic<int> m_pendingJobs { 0 };

		static constexpr size_t MaxSize = 128;
		static constexpr size_t PayloadSize = sizeof(m_function) + sizeof(m_parent) + sizeof(m_pendingJobs);
		static constexpr size_t PaddingBytes = MaxSize - PayloadSize;

		byte m_argument[PaddingBytes] { 0 };

		template <typename T>
		size_t SetArgument(size_t offset, T value)
		{
			std::memcpy(m_argument + offset, &value, sizeof(T));
			return offset + sizeof(T);
		}

		template <typename T>
		size_t GetArgument(size_t offset, T& value) const
		{
			value = *reinterpret_cast<const T*>(m_argument + offset);
			return offset + sizeof(T);
		}

		template <typename TClass, typename... TArgs>
		static void CallMethod(Job* job);
		template <typename... TArgs>
		static void CallFunction(Job* job);
	};

	struct JobSystem
	{
		JobSystem(uint threads);

		template <typename TClass, typename... TArgs>
		Job* CreateMethod(TClass* this_ptr, void (TClass::* fn)(TArgs...), TArgs... arg);
		template <typename... TArgs>
		Job* Create(void (*fn)(TArgs...), TArgs... arg);
		Job* Create();

		template <typename TClass, typename... TArgs>
		Job* CreateMethodChild(Job* parent, TClass* this_ptr, void (TClass::* fn)(TArgs...), TArgs... args);
		template <typename... TArgs>
		Job* CreateChild(Job* parent, void (*fn)(TArgs...), TArgs... args);

		void Schedule(Job* job);
		void Wait(const Job* job);
		void WaitForAll(const std::vector<Job*>& jobs);

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
	};
}

#include "JobSystem.inl"