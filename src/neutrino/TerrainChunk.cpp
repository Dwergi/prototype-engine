//
// TerrainChunk.h - A chunk of procedural terrain.
// Copyright (C) Sebastian Nordgren 
// April 15th 2016
//

#include "PCH.h"
#include "neutrino/TerrainChunk.h"

#include "ICamera.h"
#include "JobSystem.h"
#include "MeshUtils.h"

#include "ddr/Mesh.h"

#include "neutrino/TerrainParameters.h"

#include "glm/gtc/noise.hpp"

#include "stb/stb_image_write.h"

#include "fmt/format.h"

namespace neut
{
	static dd::Service<dd::JobSystem> s_jobsystem;
	static dd::Service<ddr::MeshManager> s_meshManager;

	static dd::FSMPrototype<TerrainChunkStates> s_fsmPrototype;

	static std::vector<uint> s_indices[neut::TerrainParameters::LODs];

	TerrainChunk::TerrainChunk(const neut::TerrainParameters& params, glm::vec2 position) :
		m_terrainParams(params),
		m_state(s_fsmPrototype),
		m_position(position)
	{
		m_vertices.resize(TotalVertexCount);
	}

	TerrainChunk::~TerrainChunk()
	{
		m_mesh.Destroy();
	}

	void TerrainChunk::InitializeShared()
	{
		DD_ASSERT(s_indices[0].empty(), "Multiple calls of InitializeShared!");

		for (int lod = 0; lod < neut::TerrainParameters::LODs; ++lod)
		{
			const uint stride = 1 << lod;
			const uint lod_vertices = MaxVertices / stride;
			const uint row_width = MaxVertices + 1;
			const uint mesh_index_count = lod_vertices * lod_vertices * 6;

			DD_ASSERT(stride <= MaxVertices);

			const uint flap_index_count = lod_vertices * 6 * 4;

			std::vector<uint>& indices = s_indices[lod];
			indices.reserve(mesh_index_count + flap_index_count);

			// fetch grid indices
			const std::vector<uint>& grid_indices = dd::MeshUtils::GetGridIndices(MaxVertices, lod);
			indices.insert(indices.end(), grid_indices.begin(), grid_indices.end());

			DD_ASSERT(indices.size() == mesh_index_count);

			// add flap indices
			int flap_vertex_start = MeshVertexCount;

			// (top, x = varying, z = 0)
			for (uint x = 0; x < MaxVertices; x += stride)
			{
				indices.push_back(x);
				indices.push_back(flap_vertex_start + x + stride);
				indices.push_back(flap_vertex_start + x);

				indices.push_back(x);
				indices.push_back(x + stride);
				indices.push_back(flap_vertex_start + x + stride);
			}

			flap_vertex_start += row_width;

			// (right, x = chunk size, z = varying)
			for (uint z = 0; z < MaxVertices; z += stride)
			{
				indices.push_back((z + stride) * row_width + MaxVertices);
				indices.push_back(flap_vertex_start + z + stride);
				indices.push_back(flap_vertex_start + z);

				indices.push_back(flap_vertex_start + z);
				indices.push_back(z * row_width + MaxVertices);
				indices.push_back((z + stride) * row_width + MaxVertices);
			}

			flap_vertex_start += row_width;

			// (bottom, x = varying, z = chunk size)
			for (uint x = 0; x < MaxVertices; x += stride)
			{
				const uint last_row = MeshVertexCount - row_width;

				indices.push_back(last_row + x + stride);
				indices.push_back(flap_vertex_start + x);
				indices.push_back(flap_vertex_start + x + stride);

				indices.push_back(last_row + x);
				indices.push_back(flap_vertex_start + x);
				indices.push_back(last_row + x + stride);
			}

			flap_vertex_start += row_width;

			// (left, x = 0, z = varying)
			for (uint z = 0; z < MaxVertices; z += stride)
			{
				indices.push_back((z + stride) * row_width);
				indices.push_back(flap_vertex_start + z);
				indices.push_back(flap_vertex_start + z + stride);

				indices.push_back(z * row_width);
				indices.push_back(flap_vertex_start + z);
				indices.push_back((z + stride) * row_width);
			}

			DD_ASSERT(indices.size() == mesh_index_count + flap_index_count);

			for (uint i : indices)
			{
				DD_ASSERT(i < TotalVertexCount);
			}
		}

		// initialize FSM
		s_fsmPrototype.AddState(TerrainChunkStates::InitializePending);
		s_fsmPrototype.SetInitialState(TerrainChunkStates::InitializePending);

		s_fsmPrototype.AddState(TerrainChunkStates::InitializeDone);
		s_fsmPrototype.AddTransition(TerrainChunkStates::InitializePending, TerrainChunkStates::InitializeDone);

		s_fsmPrototype.AddState(TerrainChunkStates::UpdatePending);
		s_fsmPrototype.AddTransition(TerrainChunkStates::InitializeDone, TerrainChunkStates::UpdatePending);

		s_fsmPrototype.AddState(TerrainChunkStates::UpdateDone);
		s_fsmPrototype.AddTransition(TerrainChunkStates::UpdatePending, TerrainChunkStates::UpdateDone);

		s_fsmPrototype.AddState(TerrainChunkStates::RenderUpdatePending);
		s_fsmPrototype.AddTransition(TerrainChunkStates::UpdateDone, TerrainChunkStates::RenderUpdatePending);

		s_fsmPrototype.AddState(TerrainChunkStates::RenderUpdateDone);
		s_fsmPrototype.AddTransition(TerrainChunkStates::RenderUpdatePending, TerrainChunkStates::RenderUpdateDone);

		s_fsmPrototype.AddState(TerrainChunkStates::Ready);
		s_fsmPrototype.AddTransition(TerrainChunkStates::RenderUpdateDone, TerrainChunkStates::Ready);

		s_fsmPrototype.AddTransition(TerrainChunkStates::UpdateDone, TerrainChunkStates::UpdatePending);
		s_fsmPrototype.AddTransition(TerrainChunkStates::Ready, TerrainChunkStates::UpdatePending);
	}


	void TerrainChunk::Initialize()
	{
		DD_PROFILE_SCOPED(TerrainChunk_Initialize);
		DD_ASSERT(m_state == TerrainChunkStates::InitializePending);

		const int row_width = MaxVertices + 1;
		const float vertex_distance = m_terrainParams.ChunkSize / MaxVertices;

		for (int z = 0; z < row_width; ++z)
		{
			for (int x = 0; x < row_width; ++x)
			{
				const int current = z * row_width + x;
				DD_ASSERT(current < MeshVertexCount);

				// height is y
				m_vertices[current] = glm::vec3(x * vertex_distance, 0, z * vertex_distance);
			}
		}

		int flap_vertex_start = MeshVertexCount;
		for (int x = 0; x < row_width; ++x)
		{
			m_vertices[flap_vertex_start + x] = glm::vec3(x * vertex_distance, 0, 0);
		}

		flap_vertex_start += row_width;
		for (int z = 0; z < row_width; ++z)
		{
			m_vertices[flap_vertex_start + z] = glm::vec3(m_terrainParams.ChunkSize, 0, z * vertex_distance);
		}

		flap_vertex_start += row_width;
		for (int x = 0; x < row_width; ++x)
		{
			m_vertices[flap_vertex_start + x] = glm::vec3(x * vertex_distance, 0, m_terrainParams.ChunkSize);
		}

		flap_vertex_start += row_width;
		for (int z = 0; z < row_width; ++z)
		{
			m_vertices[flap_vertex_start + z] = glm::vec3(0, 0, z * vertex_distance);
		}

		m_state.TransitionTo(TerrainChunkStates::InitializeDone);
	}

	void TerrainChunk::SwitchLOD(int lod)
	{
		if (m_lod != lod)
		{
			m_lod = lod;

			m_dirty = true;
		}
	}

	void TerrainChunk::SetNoiseOffset(glm::vec2 noise_offset)
	{
		if (m_offset != noise_offset)
		{
			m_offset = noise_offset;

			m_dirty = true;
		}
	}

	void TerrainChunk::Update(dd::Job* parent_job)
	{
		if (m_state == TerrainChunkStates::InitializePending)
		{
			dd::Job* job = s_jobsystem->CreateMethodChild(parent_job, this, &TerrainChunk::Initialize);
			s_jobsystem->Schedule(job);
		}

		if (m_state == TerrainChunkStates::InitializeDone)
		{
			m_state.TransitionTo(TerrainChunkStates::UpdatePending);
		}

		if (m_state == TerrainChunkStates::RenderUpdateDone)
		{
			m_state.TransitionTo(TerrainChunkStates::Ready);
		}

		if (m_dirty)
		{
			if (m_state == TerrainChunkStates::UpdateDone || m_state == TerrainChunkStates::Ready)
			{
				m_state.TransitionTo(TerrainChunkStates::UpdatePending);
				m_dirty = false;
			}
		}

		if (m_state == TerrainChunkStates::UpdatePending)
		{
			if (m_updating.exchange(true) == false)
			{
				dd::Job* job = s_jobsystem->CreateMethodChild(parent_job, this, &TerrainChunk::UpdateVertices);
				s_jobsystem->Schedule(job);
			}
		}

		if (m_state == TerrainChunkStates::UpdateDone)
		{
			m_state.TransitionTo(TerrainChunkStates::RenderUpdatePending);
		}
	}

	void TerrainChunk::RenderUpdate()
	{
		if (m_state == TerrainChunkStates::RenderUpdatePending)
		{
			if (!m_mesh.IsValid())
			{
				CreateMesh(m_position);
			}
			else
			{
				ddr::Mesh* mesh = m_mesh.Access();
				mesh->SetPositions(dd::ConstBuffer<glm::vec3>(m_vertices));
				mesh->SetIndices(dd::ConstBuffer<uint>(s_indices[m_lod]));
				mesh->SetBoundBox(m_bounds);
			}

			m_state.TransitionTo(TerrainChunkStates::RenderUpdateDone);
		}
	}

	thread_local std::vector<float> sm_noise;

	void TerrainChunk::UpdateVertices()
	{
		DD_ASSERT(m_updating);
		DD_ASSERT(m_state == TerrainChunkStates::UpdatePending);

		m_updating = false;

		if (m_minLod <= m_lod && m_offset == m_previousOffset)
		{
			m_state.TransitionTo(TerrainChunkStates::UpdateDone);
			return;
		}

		m_minLod = m_lod;
		m_previousOffset = m_offset;

		const glm::ivec2 dimensions(MaxVertices + 1);

		const glm::ivec2 stride(1 << m_lod);
		DD_ASSERT(stride.x < MaxVertices);

		const glm::vec2 increment(m_terrainParams.ChunkSize / MaxVertices);

		ddm::GenerateNoise(sm_noise, dimensions, stride, m_position + m_offset, increment, m_terrainParams.Noise);

		float max_height = std::numeric_limits<float>::min();
		float min_height = std::numeric_limits<float>::max();

		for (int y = 0; y < dimensions.y; y += stride.y)
		{
			for (int x = 0; x < dimensions.x; x += stride.x)
			{
				const int current = y * dimensions.y + x;
				DD_ASSERT(current < MeshVertexCount);

				m_vertices[current].y = sm_noise[current] * m_terrainParams.HeightRange;

				min_height = ddm::min(min_height, m_vertices[current].y);
				max_height = ddm::max(max_height, m_vertices[current].y);
			}
		}

		m_bounds.Min = glm::vec3(0, min_height, 0);
		m_bounds.Max = glm::vec3(m_terrainParams.ChunkSize, max_height, m_terrainParams.ChunkSize);

		m_state.TransitionTo(TerrainChunkStates::UpdateDone);
	}

	void TerrainChunk::CreateMesh(glm::vec2 pos)
	{
		DD_PROFILE_SCOPED(TerrainChunk_CreateMesh);

		std::string name = fmt::format("terrain_{}x{}", (int) pos.x, (int) pos.y);

		m_mesh = s_meshManager->Create(name.c_str());

		ddr::Mesh* mesh = m_mesh.Access();
		mesh->SetPositions(dd::ConstBuffer<glm::vec3>(m_vertices));
		mesh->SetIndices(dd::ConstBuffer<uint>(s_indices[m_lod]));
		mesh->SetBoundBox(m_bounds);
	}

	void TerrainChunk::WriteHeightImage(const char* filename) const
	{
		const int stride = 1 << m_lod;
		const int terrain_size = MaxVertices + 1;
		const int image_size = (MaxVertices >> m_lod) + 1;

		std::vector<byte> pixels;
		pixels.resize(image_size * image_size);

		const float range = m_bounds.Extents().y;
		const float min = m_bounds.Min.y;
		const float max = m_bounds.Max.y;

		for (int y = 0; y < image_size; ++y)
		{
			for (int x = 0; x < image_size; ++x)
			{
				float value = m_vertices[y * stride * terrain_size + x * stride].y;
				float greyscale = (value - min) / range;

				pixels[y * image_size + x] = (byte) (greyscale * 255);
			}
		}

		stbi_write_tga(filename, image_size, image_size, 1, pixels.data());
	}

	static byte NormalToColour(float f)
	{
		return (byte) (((f + 1.0f) / 2.0f) * 255.f);
	}
}