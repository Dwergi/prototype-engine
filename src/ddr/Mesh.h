//
// Mesh.h - A mesh wrapper.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#pragma once

#include "HandleManager.h"

#include "ddm/AABB.h"

#include "ddr/VAO.h"
#include "ddr/VBO.h"

namespace dd
{
	struct BVHTree;
	struct JobSystem;
}

namespace ddr
{
	struct ICamera;
	struct Shader;
	struct UniformStorage;

	enum class MeshPart
	{
		Position = 1 << 0,
		Normal	 = 1 << 1,
		Index	 = 1 << 2,
		UV		 = 1 << 3,
		Colour	 = 1 << 4
	};

	//
	// A mesh asset.
	//
	struct Mesh : dd::HandleTarget
	{
		//
		// Retrieve the axis-aligned bounds of this mesh.
		//
		const ddm::AABB& GetBoundBox() const { return m_bounds; }

		//
		// Set the bounds of this mesh.
		//
		void SetBoundBox(const ddm::AABB& bounds) { m_bounds = bounds; m_hasBounds = true; }

		//
		// Set the positions that the mesh will use.
		// The mesh does *NOT* take ownership of this.
		// The pointer in the buffer must remain valid for the lifetime of this mesh.
		//
		void SetPositions(const dd::ConstBuffer<glm::vec3>& positions);

		//
		// Get the currently set buffer for positions.
		// Not guaranteed to be valid.
		//
		dd::ConstBuffer<glm::vec3> GetPositions() const { return m_positions; }

		//
		// Set the index buffer that the mesh will use.
		// The mesh does *NOT* take ownership of this.
		//
		void SetIndices(const dd::ConstBuffer<uint>& indices);

		//
		// Get the currently set buffer for indices.
		// Not guaranteed to be valid. If the mesh does not use indices, then this will never be valid.
		//
		dd::ConstBuffer<uint> GetIndices() const { return m_indices; }

		//
		// Set the normal buffer that the mesh will use.
		// The mesh does *NOT* take ownership of this.
		//
		void SetNormals(const dd::ConstBuffer<glm::vec3>& normals);

		//
		// Set the UV buffer that the mesh will use.
		// The mesh does *NOT* take ownership of this.
		//
		void SetUVs(const dd::ConstBuffer<glm::vec2>& uvs);

		//
		// Bind this mesh's attributes to the given shader.
		//
		void BindToShader(Shader& shader);

		//
		// Create the mesh. Must be called on the render thread.
		//
		void Create();

		//
		// Send updated mesh data to the GPU from the same place it currently is.
		//
		void Update(dd::JobSystem& jobsystem);

		//
		// Destroy the mesh. Must be called on the render thread.
		//
		void Destroy();

		//
		// Enable/disable the BVH calculation.
		//
		void UseBVH(bool enabled) { m_enableBVH = enabled; }

		//
		// Set the mesh to be dirty.
		//
		void SetDirty(MeshPart part) { m_dirty.Set(part); }

		//
		// Get the BVH of this mesh.
		//
		const dd::BVHTree* GetBVH() const { return m_bvh; }

		//
		// Get the VAO of this mesh.
		// 
		ddr::VAO& VAO() { return m_vao; }

		void BindToShader(Shader& shader);

		//
		// Do not create directly, use MeshManager.
		//
		Mesh();
		~Mesh();

		Mesh& operator=(const Mesh&) = delete;
		Mesh& operator=(Mesh&&) = delete;
		Mesh(const Mesh&) = delete;
		Mesh(Mesh&&) = delete;

		static const char* TypeName() { return "Mesh"; }

	private:

		dd::EnumFlags<MeshPart> m_dirty;

		VBO m_vboPosition;
		dd::ConstBuffer<glm::vec3> m_positions;
		VBO m_vboNormal;
		dd::ConstBuffer<glm::vec3> m_normals;
		VBO m_vboIndex;
		dd::ConstBuffer<uint> m_indices;
		VBO m_vboUV;
		dd::ConstBuffer<glm::vec2> m_uvs;

		ddr::VAO m_vao;

		bool m_hasBounds { false };
		ddm::AABB m_bounds;

		bool m_enableBVH { true };
		std::atomic<bool> m_rebuilding { false };
		dd::BVHTree* m_bvh { nullptr };

		void RebuildBVH();
	};

	using MeshHandle = dd::Handle<ddr::Mesh>;

	struct MeshManager : dd::HandleManager<ddr::Mesh>
	{
	private:

		virtual void OnCreate(Mesh& mesh) const override;
		virtual void OnDestroy(Mesh& mesh) const override;
	};
}
