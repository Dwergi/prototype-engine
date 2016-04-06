//
// JobSystem.h - A manager for creating job threads which will process tasks in parallel over a number of threads.
// Copyright (C) Sebastian Nordgren 
// October 29th 2015
//

#pragma once

#include <mutex>

namespace std
{
	class thread;
}

namespace dd
{
	class Function;

	//
	// Wrapper for storing arguments as Variables and creating them with less effort.
	//
	class FunctionArgs
	{
	private:
		struct LocalArg
		{
			const TypeInfo* Type;
			uint Offset;
		};

		// this is some scratch space to allow us to capture the variables we're given
		Array<byte, 256> Storage;
		Vector<LocalArg> LocalArgs;
		Vector<Variable> Variables;

		template <typename T>
		void UnpackSingle( LocalArg& arg, T data )
		{
			T* loc = (T*) (Storage.Data() + Storage.Size());
			*loc = data;

			arg.Offset = Storage.Size();
			arg.Type = GET_TYPE( T );

			Storage.SetSize( Storage.Size() + sizeof( T ) );
		}

		template <typename... Args, std::size_t... Index>
		void UnpackArgs( std::tuple<Args...> args, std::index_sequence<Index...> )
		{
			LocalArgs.Resize( sizeof...(Args) );

			int dummy[] =
			{ 
				0,(
					(void) UnpackSingle( LocalArgs[Index], std::forward<Args>( std::get<Index>( args ) ) ), 
				0)... 
			};
		}

	public:

		Variable Context;

		FunctionArgs()
		{
		}

		FunctionArgs( const FunctionArgs& other )
			: Storage( other.Storage ),
			LocalArgs( other.LocalArgs ),
			Variables( other.Variables ),
			Context( other.Context )
		{
		}

		FunctionArgs( FunctionArgs&& other )
			: Storage( other.Storage ),
			LocalArgs( other.LocalArgs ),
			Variables( other.Variables ),
			Context( other.Context )
		{
		}

		FunctionArgs& operator=( const FunctionArgs& other )
		{
			Storage = other.Storage;
			LocalArgs = other.LocalArgs;
			Variables = other.Variables;
			Context = other.Context;
			
			return *this;
		}

		Vector<Variable> GetArguments()
		{
			if( Variables.Size() > 0 )
				return Variables;

			Vector<Variable> variables;
			for( LocalArg& arg : LocalArgs )
			{
				variables.Add( Variable( arg.Type, (void*) (Storage.Data() + arg.Offset) ) );
			}

			return variables;
		}

		uint ArgCount() const
		{
			return std::max( Variables.Size(), LocalArgs.Size() );
		}
		
		template <typename... Args>
		static FunctionArgs Create( Args... args )
		{
			FunctionArgs ret;
			
			ret.UnpackArgs( std::make_tuple( args... ), std::make_index_sequence<sizeof...(Args)>() );

			return ret;
		}

		template <typename Context, typename... Args>
		static FunctionArgs CreateMethod( Context ctx, Args... args )
		{
			FunctionArgs ret;
			ret.Context = Variable( ctx );

			ret.UnpackArgs( std::make_tuple( args... ), std::make_index_sequence<sizeof...(Args)>() );

			return ret;
		}

		void AddArgument( const Variable& var )
		{
			DD_ASSERT( LocalArgs.Size() == 0 );

			Variables.Add( var );
		}
	};

	class JobHandle;

	class JobSystem
	{
	public:

		explicit JobSystem( uint thread_count );
		~JobSystem();

		JobSystem() = delete;
		JobSystem( const JobSystem& ) = delete;
		JobSystem& operator=( const JobSystem& ) = delete;

		//
		// Schedule a function to be run at some point.
		// Category allows jobs to be grouped and waited on as logical units.
		//
		void Schedule( const Function& fn, const char* category );
		void Schedule( Function&& fn, const char* category );
		void Schedule( const Function& fn, const FunctionArgs& args, const char* category );
		void Schedule( Function&& fn, FunctionArgs&& args, const char* category );

		void WaitForCategory( const char* category, uint timeout_ms = 0 );

		BASIC_TYPE( JobSystem )

	private:

		static const uint MAX_THREADS = 8;

		enum class JobStatus
		{
			Pending,
			Assigned,
			Running,
			Waiting,
			DoneWaiting,
			Done,
			Invalid
		};

		//
		// A job structure.
		//
		struct Job
		{
			Job();
			Job( const Job& other );
			Job( const Function& fn, const FunctionArgs& args, const char* category );
			Job( Function&& fn, FunctionArgs&& args, const char* category );

			Function Func;
			FunctionArgs Args;
			String128 Category;
			String128 WaitingOn;
			JobStatus Status;
		};

		friend class JobThread;
		friend class JobHandle;

		std::thread m_threads[MAX_THREADS];
		Vector<JobThread*> m_workers;
		Vector<Job> m_jobs;

		std::mutex m_jobsMutex;

		void CreateWorkers( uint thread_count );

		bool GetPendingJob( JobHandle& out_job );

		JobThread* FindCurrentWorker() const;

		bool HasPendingJobs( const char* category );
		void ClearDoneJobs();
		void UpdateWaitingJobs();
	};

	//
	// A handle to refer to a single pending job.
	//
	class JobHandle
	{
	public:
		JobHandle();
		JobHandle( JobSystem& system, JobSystem::Job& job );

		JobSystem::Job* GetJob() const;
		
	private:
		JobSystem* m_system;
		uint64 m_hash;
	};
}