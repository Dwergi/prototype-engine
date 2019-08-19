namespace dd
{
	template <typename TClass, typename TArg>
	Job* JobSystem::CreateMethod(TClass* this_ptr, void (TClass::* fn)(TArg), const TArg& arg)
	{
		return CreateMethodChild(nullptr, this_ptr, fn, arg);
	}

	template <typename TClass>
	Job* JobSystem::CreateMethod(TClass* this_ptr, void (TClass::* fn)(void))
	{
		return CreateMethodChild(nullptr, this_ptr, fn);
	}

	template <typename TArg>
	Job* JobSystem::Create(void (*fn)(TArg), const TArg& arg)
	{
		return CreateChild(nullptr, fn, arg);
	}

	template <typename TClass, typename TArg>
	Job* JobSystem::CreateMethodChild(Job* parent, TClass* this_ptr, void (TClass::* fn)(TArg), const TArg& arg)
	{
		static_assert((sizeof(TClass*) + sizeof(fn) + sizeof(TArg)) < Job::PaddingBytes);
		
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
		job->m_function = &Job::CallMethodArg<TClass, TArg>;

		size_t offset = 0;
		offset = job->SetArgument(offset, this_ptr);
		offset = job->SetArgument(offset, fn);
		offset = job->SetArgument(offset, arg);

		Push(*job);
		return job;
	}

	template <typename TClass>
	Job* JobSystem::CreateMethodChild(Job* parent, TClass* this_ptr, void (TClass::* fn)(void))
	{
		static_assert((sizeof(TClass*) + sizeof(fn)) < Job::PaddingBytes);

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
		job->m_function = &Job::CallMethod<TClass>;

		size_t offset = 0;
		offset = job->SetArgument(offset, this_ptr);
		offset = job->SetArgument(offset, fn);

		Push(*job);
		return job;
	}

	template <typename TArg>
	Job* JobSystem::CreateChild(Job* parent, void (*fn)(TArg), const TArg& arg)
	{
		static_assert(sizeof(fn) + sizeof(args) < Job::PaddingBytes);

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
		job->m_function = &Job::CallArg<TArg>;

		size_t offset = 0;
		offset = job->SetArgument(offset, fn);
		offset = job->SetArgument(offset, arg);

		Push(*job);

		return job;
	}

	template <typename TClass, typename TArg>
	void Job::CallMethodArg(Job* job)
	{
		using TMethod = void (TClass::*)(TArg);

		size_t offset = 0;

		TClass* this_ptr;
		offset = job->GetArgument(offset, this_ptr);
		TMethod method;
		offset = job->GetArgument(offset, method);
		TArg arg;
		offset = job->GetArgument(offset, arg);

		std::invoke(method, this_ptr, arg);
	}

	template <typename TClass>
	void Job::CallMethod(Job* job)
	{
		using TMethod = void (TClass::*)();

		size_t offset = 0;

		TClass* this_ptr;
		offset = job->GetArgument(offset, this_ptr);
		TMethod method;
		offset = job->GetArgument(offset, method);

		std::invoke(method, this_ptr);
	}

	template <typename TArg>
	void Job::CallArg(Job* job)
	{
		size_t offset = 0;

		void (*fn)(TArg);
		offset = job->GetArgument(offset, fn);
		TArg arg;
		offset = job->GetArgument(offset, arg);

		fn(arg);
	}
}