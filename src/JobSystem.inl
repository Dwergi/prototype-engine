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
		Job* job = CreateChild(parent, nullptr);
		job->SetMethod(this_ptr, fn, args...);

		return job;
	}

	template <typename... TArgs>
	Job* JobSystem::CreateChild(Job* parent, void (*fn)(TArgs...), TArgs... args)
	{
		Job* job = CreateChild(parent, nullptr);
		job->SetFunction(fn, args...);

		return job;
	}
}