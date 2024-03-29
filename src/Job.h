//
// Job.h
// Copyright (C) Sebastian Nordgren 
// August 27th 2019
// Based on: https://blog.molecular-matters.com/2015/08/24/job-system-2-0-lock-free-work-stealing-part-1-basics/
// Also influenced by https://manu343726.github.io/2017-03-13-lock-free-job-stealing-task-system-with-modern-c/
//

#pragma once

#include <atomic>

namespace dd
{
	struct JobSystem;

	struct Job
	{
		void Run();
		bool IsFinished() const;
		void Clear();
		void SetParent(Job* job);

		template <typename TClass, typename... TArgs>
		void SetMethod(TClass* this_ptr, void (TClass::* fn)(TArgs...), TArgs... args);

		template <typename... TArgs>
		void SetFunction(void (*fn)(TArgs...), TArgs... args);

	private:
		friend struct JobSystem;

		void Finish();

		void (*m_function)(Job*) { nullptr };
		Job* m_parent { nullptr };
		std::atomic<int> m_pendingJobs { 0 };

		static constexpr size_t MaxSize = 64;
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

	template <typename TClass, typename... TArgs>
	void Job::CallMethod(Job* job)
	{
		using TMethod = void (TClass::*)(TArgs...);

		size_t offset = 0;

		TClass* this_ptr;
		offset = job->GetArgument(offset, this_ptr);

		TMethod method;
		offset = job->GetArgument(offset, method);

		std::tuple<TArgs...> args_tuple;
		offset = job->GetArgument(offset, args_tuple);

		auto wrapper = [this_ptr, method](TArgs... args)
		{
			std::invoke(method, this_ptr, std::forward<TArgs>(args)...);
		};
		std::apply(wrapper, args_tuple);
	}

	template <typename... TArgs>
	void Job::CallFunction(Job* job)
	{
		size_t offset = 0;

		void (*fn)(TArgs...);
		offset = job->GetArgument(offset, fn);

		std::tuple<TArgs...> args_tuple;
		offset = job->GetArgument(offset, args_tuple);

		std::apply(fn, args_tuple);
	}

	template <typename TClass, typename... TArgs>
	void Job::SetMethod(TClass* this_ptr, void (TClass::* fn)(TArgs...), TArgs... args)
	{
		std::tuple<TArgs...> args_tuple = std::make_tuple(args...);
		static_assert((sizeof(TClass*) + sizeof(fn) + sizeof(args_tuple)) <= Job::PaddingBytes);

		m_function = &Job::CallMethod<TClass, TArgs...>;

		size_t offset = 0;
		offset = SetArgument(offset, this_ptr);
		offset = SetArgument(offset, fn);
		offset = SetArgument(offset, args_tuple);
	}

	template <typename... TArgs>
	void Job::SetFunction(void (*fn)(TArgs...), TArgs... args)
	{
		std::tuple<TArgs...> args_tuple = std::make_tuple(args...);
		static_assert(sizeof(fn) + sizeof(args_tuple) <= Job::PaddingBytes);

		m_function = &Job::CallFunction<TArgs...>;

		size_t offset = 0;
		offset = SetArgument(offset, fn);
		offset = SetArgument(offset, args_tuple);
	}
}