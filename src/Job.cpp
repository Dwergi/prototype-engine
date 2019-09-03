//
// Job.cpp
// Copyright (C) Sebastian Nordgren 
// August 27th 2019
// Based on: https://blog.molecular-matters.com/2015/08/24/job-system-2-0-lock-free-work-stealing-part-1-basics/
// Also influenced by https://manu343726.github.io/2017-03-13-lock-free-job-stealing-task-system-with-modern-c/
//

#include "PCH.h"
#include "Job.h"

#include "JobSystem.h"

namespace dd
{
	static dd::Service<dd::JobSystem> s_jobsystem;

	void Job::Run()
	{
		DD_ASSERT(m_pendingJobs > 0);
		
		if (m_function != nullptr)
		{
			m_function(this);
		}

		Finish();
	}

	void Job::Clear()
	{
		static_assert(sizeof(Job) == MaxSize);
		DD_ASSERT(m_pendingJobs == 0);

		std::memset(this, 0, sizeof(Job));
	}

	void Job::Finish()
	{
		int pending = --m_pendingJobs;
		DD_ASSERT(pending >= 0);

		if (m_parent != nullptr && pending == 0)
		{
			m_parent->Finish();
		}
	}

	bool Job::IsFinished() const
	{
		return m_pendingJobs == 0;
	}

	void Job::SetParent(Job* parent)
	{
		DD_ASSERT(m_parent == nullptr);
		DD_ASSERT(parent != nullptr);
		DD_ASSERT(parent->m_pendingJobs > 0);

		parent->m_pendingJobs++;
		m_parent = parent;
	}
}