//
// JobThread.h - A worker thread that requests tasks and executes them continuously.
// Copyright (C) Sebastian Nordgren 
// October 29th 2015
//

#pragma once

namespace dd
{
	class JobSystem;

	class JobThread
	{
	public:

		JobThread( JobSystem& m_owner );
		~JobThread();

		void Kill();

	private:

		bool m_killed;
		JobSystem& m_owner;

		void Run();
		void Execute( const Function& fn ) const;

		friend class JobSystem;
	};
}
