//
// ParallelFor.h
// Copyright (C) Sebastian Nordgren 
// August 28th 2019
//

#pragma once

namespace dd
{
	struct CountSplitter
	{
		explicit CountSplitter(unsigned int count)
			: Count(count)
		{
		}

		template <typename T>
		inline bool Split(unsigned int count) const
		{
			return (count > Count);
		}

	private:
		unsigned int Count;
	};

	struct DataSizeSplitter
	{
		explicit DataSizeSplitter(uint size)
			: m_size(size)
		{
		}

		template <typename T>
		inline bool Split(unsigned int count) const
		{
			return (count * sizeof(T) > m_size);
		}

	private:
		uint m_size;
	};

	template <typename TData, typename TSplitter>
	struct ParallelForData
	{
		ParallelForData(TData* data, unsigned int count, void (*function)(TData*, unsigned int), const TSplitter& splitter) :
			m_data(data),
			Count(count),
			Function(function),
			Splitter(splitter)
		{
		}

		TData* m_data { nullptr };
		unsigned int Count { 0 };
		void (*Function)(DataType*, unsigned int) { nullptr };
		TSplitter Splitter;
	};

	template <typename T, typename S>
	Job* ParallelFor(T* data, unsigned int count, void (*function)(T*, uint), const S& splitter)
	{
		Service<JobSystem> jobsystem;

		const ParallelForData<T, S> jobData(data, count, function, splitter);

		Job* job = jobSystem::CreateJob(&jobs::parallel_for_job<JobData>, jobData);

		return job;
	}

	template <typename JobData>
	void ParallelForJob(Job* job, JobData job_data)
	{
		Service<JobSystem> jobsystem;

		const JobData* data = static_cast<const JobData*>(jobData);
		const JobData::TSplitter& splitter = data->splitter;

		if (splitter.Split<JobData::TData>(data->Count))
		{
			// split in two
			const unsigned int left_count = data->Count / 2;
			const JobData left_data(data->m_data, left_count, data->Function, splitter);
			Job* left = jobsystem->CreateChild(job, &ParallelForJob<JobData>, left_data);
			jobsystem->Run(left);

			const unsigned int right_count = data->Count - left_count;
			const JobData right_data(data->m_data + left_count, right_count, data->Function, splitter);
			Job* right = jobsystem->CreateChild(job, &ParallelForJob<JobData>, right_data);
			jobSystem->Run(right);
		}
		else
		{
			// execute the function on the range of data
			(data->Function)(data->m_data, data->Count);
		}
	}
}