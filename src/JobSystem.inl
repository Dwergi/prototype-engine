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
	Job* JobSystem::CreateMethodChild(Job* parent, TClass* this_ptr, void (TClass::* method)(TArgs...), TArgs... args)
	{
		std::tuple<TArgs...> args_tuple = std::make_tuple(args...);
		static_assert((sizeof(TClass*) + sizeof(method) + sizeof(args_tuple)) < Job::PaddingBytes);
		
		Job* job = Allocate();
		if (job == nullptr)
		{
			DD_ASSERT(job != nullptr);
			return nullptr;
		}

		job->m_parent = parent;
		if (job->m_parent != nullptr)
		{
			job->m_parent->m_pendingJobs++;
		}

		job->m_pendingJobs = 1;
		job->m_function = &Job::CallMethod<TClass, TArgs...>;

		size_t offset = 0;
#ifdef DD_USE_JOB_THIS_PTR
		job->m_this = this_ptr;
#else
		offset = job->SetArgument(offset, this_ptr);
#endif

		offset = job->SetArgument(offset, method);
		offset = job->SetArgument(offset, args_tuple);
		return job;
	}

	template <typename... TArgs>
	Job* JobSystem::CreateChild(Job* parent, void (*fn)(TArgs...), TArgs... args)
	{
		std::tuple<TArgs...> args_tuple = std::make_tuple(args...);
		static_assert(sizeof(fn) + sizeof(args_tuple) < Job::PaddingBytes);

		Job* job = Allocate();
		if (job == nullptr)
		{
			DD_ASSERT(job != nullptr);
			return nullptr;
		}

		job->m_parent = parent;
		if (job->m_parent != nullptr)
		{
			job->m_parent->m_pendingJobs++;
		}

		job->m_pendingJobs = 1;
		job->m_function = &Job::CallFunction<TArgs...>;

		size_t offset = 0;
		offset = job->SetArgument(offset, fn);
		offset = job->SetArgument(offset, arg);
		return job;
	}

	template <typename TClass, typename... TArgs>
	void Job::CallMethod(Job* job)
	{
		using TMethod = void (TClass::*)(TArgs...);

		size_t offset = 0;

		TClass* this_ptr = nullptr;
#ifdef DD_USE_JOB_THIS_PTR
		this_ptr = static_cast<TClass*>(job->m_this);
#else
		offset = job->GetArgument(offset, this_ptr);
#endif
		TMethod method = nullptr;
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

		void (*fn)(TArgs...) = nullptr;
		offset = job->GetArgument(offset, fn);
		TArg arg;
		offset = job->GetArgument(offset, arg);

		std::tuple<TArgs...> args_tuple;
		offset = job->GetArgument(offset, args_tuple);

		std::apply(fn, args_tuple);
	}
}