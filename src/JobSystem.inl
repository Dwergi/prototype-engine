//
// JobSystem.inl
// Copyright (C) Sebastian Nordgren 
// August 24th 2019
// Based on: https://blog.molecular-matters.com/2015/08/24/job-system-2-0-lock-free-work-stealing-part-1-basics/
// Also influenced by https://manu343726.github.io/2017-03-13-lock-free-job-stealing-task-system-with-modern-c/
//

namespace dd
{
	template <typename TClass, typename... TArgs>
	Job* JobSystem::CreateMethod(TClass* this_ptr, void (TClass::* fn)(TArgs...), TArgs... args)
	{
		return CreateMethodChild(nullptr, this_ptr, fn, args...);
	}

	template <typename... TArgs>
	Job* JobSystem::Create(void (*fn)(TArgs...), TArgs... args)
	{
		return CreateChild(nullptr, fn, args...);
	}

	template <typename TClass, typename... TArgs>
	Job* JobSystem::CreateMethodChild(Job* parent, TClass* this_ptr, void (TClass::* fn)(TArgs...), TArgs... args)
	{
		std::tuple<TArgs...> args_tuple = std::make_tuple(args...);
		static_assert((sizeof(TClass*) + sizeof(fn) + sizeof(args_tuple)) < Job::PaddingBytes);
		
		Job* job = CreateChild(parent);
		job->m_function = &Job::CallMethod<TClass, TArgs...>;

		size_t offset = 0;
		offset = job->SetArgument(offset, this_ptr);
		offset = job->SetArgument(offset, fn);
		offset = job->SetArgument(offset, args_tuple);

		return job;
	}

	template <typename... TArgs>
	Job* JobSystem::CreateChild(Job* parent, void (*fn)(TArgs...), TArgs... args)
	{
		std::tuple<TArgs...> args_tuple = std::make_tuple(args...);
		static_assert(sizeof(fn) + sizeof(args_tuple) < Job::PaddingBytes);

		Job* job = CreateChild(parent);
		job->m_function = &Job::CallFunction<TArgs...>;

		size_t offset = 0;
		offset = job->SetArgument(offset, fn);
		offset = job->SetArgument(offset, arg);

		return job;
	}
}