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
		DD_ASSERT_ERROR(!m_vao.IsValid());
	}

	void Mesh::SetPositions(const dd::ConstBuffer<glm::vec3>& positions)
	{
		m_vboPosition.SetData(positions);

		m_dirty.Set(MeshPart::Position);
	}

	void Mesh::SetNormals(const dd::ConstBuffer<glm::vec3>& normals)
	{
		m_vboNormal.SetData(normals);

		m_dirty.Set(MeshPart::Normal);
	}

	void Mesh::SetIndices(const dd::ConstBuffer<uint>& indices)
	{
		m_vboIndex.SetData(indices);

		m_dirty.Set(MeshPart::Index);
	}

	void Mesh::SetUVs(const dd::ConstBuffer<glm::vec2>& uvs)
	{
		m_vboUV.SetData(uvs);

		m_dirty.Set(MeshPart::UV);
	}

	void Mesh::Create()
	{
		DD_ASSERT(!m_vao.IsValid());

		m_vao.Create();
	}

	void Mesh::Destroy()
	{
		m_vboPosition.Destroy();
		m_vboIndex.Destroy();
		m_vboNormal.Destroy();
		m_vboUV.Destroy();

		m_vao.Destroy();
	}

	void Mesh::Update(dd::JobSystem& jobsystem)
	{
		DD_ASSERT(m_vao.IsValid());

		m_vao.Bind();

		if (m_dirty.Has(MeshPart::Position))
		{
			if (!m_vboPosition.IsValid())
			{
				m_vboPosition.Create(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
			}

			m_vboPosition.Bind();
			m_vboPosition.CommitData();
			m_vboPosition.Unbind();
		}

		if (m_dirty.Has(MeshPart::Normal))
		{
			if (!m_vboNormal.IsValid())
			{
				m_vboNormal.Create(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
			}

			m_vboNormal.Bind();
			m_vboNormal.CommitData();
			m_vboNormal.Unbind();
		}

		if (m_dirty.Has(MeshPart::Index))
		{
			if (!m_vboIndex.IsValid())
			{
				m_vboIndex.Create(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
				m_vboIndex.Bind();
			}

			m_vboIndex.CommitData();
		}

		if (m_dirty.Has(MeshPart::UV))
		{
			if (!m_vboUV.IsValid())
			{
				m_vboUV.Create(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
			}

			m_vboUV.Bind();
			m_vboUV.CommitData();
			m_vboUV.Unbind();
		}

		m_vao.Unbind();

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

	void Mesh::BindToShader(Shader& shader)
	{
		DD_ASSERT(shader.InUse());

		if (m_vboPosition.IsValid())
		{
			m_vboPosition.Bind();
			shader.BindPositions();
			m_vboPosition.Unbind();
		}

		if (m_vboNormal.IsValid())
		{
			m_vboNormal.Bind();
			shader.BindNormals();
			m_vboNormal.Unbind();
		}

		if (m_vboUV.IsValid())
		{
			m_vboUV.Bind();
			shader.BindUVs();
			m_vboUV.Unbind();
		}
	}

	void Mesh::RebuildBVH()
	{
		DD_ASSERT(m_rebuildingBVH);

		m_bvh = new dd::BVHTree();

		ddm::AABB total_bounds;

		const dd::ConstBuffer<glm::vec3>& positions = GetPositions();
		const dd::ConstBuffer<uint>& indices = GetIndices();

		dd::ConstTriangulator triangulator(positions, indices);
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