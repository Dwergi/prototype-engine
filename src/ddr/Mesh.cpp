//
// Mesh.cpp - A mesh wrapper.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#include "PCH.h"
#include "Mesh.h"

#include "BVHTree.h"
#include "ICamera.h"
#include "JobSystem.h"
#include "Material.h"
#include "OpenGL.h"
#include "Shader.h"
#include "Triangulator.h"
#include "Uniforms.h"
#include "RenderManager.h"

DD_POD_CPP(ddr::MeshHandle);

namespace ddr
{
	Mesh::Mesh()
	{
		DD_PROFILE_SCOPED(Mesh_Create);
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::SetPositions(const dd::ConstBuffer<glm::vec3>& positions)
	{
		DD_ASSERT(positions.Size() > 0);

		m_positions.ReleaseConst();
		m_positions = positions;
		m_dirty.Set(MeshPart::Position);
	}

	void Mesh::SetNormals(const dd::ConstBuffer<glm::vec3>& normals)
	{
		DD_ASSERT(normals.Size() > 0);

		m_normals.ReleaseConst();
		m_normals = normals;
		m_dirty.Set(MeshPart::Normal);
	}

	void Mesh::SetIndices(const dd::ConstBuffer<uint>& indices)
	{
		DD_ASSERT(indices.Size() > 0);

		m_indices.ReleaseConst();
		m_indices = indices;
		m_dirty.Set(MeshPart::Index);
	}

	void Mesh::SetUVs(const dd::ConstBuffer<glm::vec2>& uvs)
	{
		DD_ASSERT(uvs.Size() > 0);

		m_uvs.ReleaseConst();
		m_uvs = uvs;
		m_dirty.Set(MeshPart::UV);
	}

	void Mesh::Create()
	{
	}

	void Mesh::Destroy()
	{
		m_vboPosition.Destroy();
		m_vboIndex.Destroy();
		m_vboNormal.Destroy();
		m_vboUV.Destroy();
	}

	void Mesh::Update(dd::JobSystem& jobsystem)
	{
		if (m_dirty.Has(MeshPart::Position))
		{
			if (!m_vboPosition.IsValid())
			{
				m_vboPosition.Create("position");
			}

			m_vboPosition.SetData(m_positions);
		}

		if (m_dirty.Has(MeshPart::Normal))
		{
			if (!m_vboNormal.IsValid())
			{
				m_vboNormal.Create("normal");
			}

			m_vboNormal.SetData(m_normals);
		}

		if (m_dirty.Has(MeshPart::Index))
		{
			if (!m_vboIndex.IsValid())
			{
				m_vboIndex.Create("index");
			}

			m_vboIndex.SetData(m_indices);
		}

		if (m_dirty.Has(MeshPart::UV))
		{
			if (!m_vboUV.IsValid())
			{
				m_vboUV.Create("uv");
			}

			m_vboUV.SetData(m_uvs);
		}

		// rebuild BVH only if positions or indices have changed
		if (m_enableBVH &&
			(m_dirty.Has(MeshPart::Position) || m_dirty.Has(MeshPart::Index)))
		{
			if (m_rebuildingBVH.exchange(true) == false)
			{
				if (m_bvh != nullptr)
				{
					delete m_bvh;
				}

				dd::Job* job = jobsystem.CreateMethod(this, &Mesh::RebuildBVH);
				jobsystem.Schedule(job);
			}
		}

		m_dirty.Clear();
	}

	void Mesh::Bind(VAO& vao, Shader& shader)
	{
		if (m_vboIndex.IsValid())
		{
			vao.BindIndices(m_vboIndex);
		}

		if (m_vboPosition.IsValid())
		{
			vao.BindVBO(m_vboPosition, 0, sizeof(m_positions[0]));
			shader.BindPositions(vao, m_vboPosition);
		}

		if (m_vboNormal.IsValid())
		{
			vao.BindVBO(m_vboNormal, 0, sizeof(m_normals[0]));
			shader.BindNormals(vao, m_vboNormal);
		}

		if (m_vboUV.IsValid())
		{
			vao.BindVBO(m_vboUV, 0, sizeof(m_uvs[0]));
			shader.BindUVs(vao, m_vboUV);
		}
	}

	void Mesh::RebuildBVH()
	{
		DD_ASSERT(m_rebuildingBVH);

		m_bvh = new dd::BVHTree();

		ddm::AABB total_bounds;

		const dd::ConstBuffer<glm::vec3>& positions = GetPositions();
		const dd::ConstBuffer<uint>& indices = GetIndices();

		dd::Triangulator triangulator(positions, indices);
		for (size_t i = 0; i < triangulator.Size(); ++i)
		{
			dd::ConstTriangle tri = triangulator[i];

			ddm::AABB bounds;
			bounds.Expand(tri.p0);
			bounds.Expand(tri.p1);
			bounds.Expand(tri.p2);

			total_bounds.Expand(bounds);

			m_bvh->Add(bounds);
		}

		m_bvh->Build();

		if (!m_hasBounds)
		{
			m_bounds = total_bounds;
		}

		m_rebuildingBVH = false;
	}

	void MeshManager::OnCreate(Mesh& mesh) const
	{
		mesh.Create();
	}

	void MeshManager::OnDestroy(Mesh& mesh) const
	{
		mesh.Destroy();
	}
}
