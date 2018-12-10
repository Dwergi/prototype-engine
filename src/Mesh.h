//
// Mesh.h - A mesh wrapper.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#pragma once

#include "AABB.h"
#include "HandleManager.h"
#include "Material.h"
#include "VAO.h"
#include "VBO.h"

#include <atomic>
#include <bitset>
#include <memory>
#include <unordered_map>

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

	enum class BufferType
	{
		Position,
		Normal,
		Index,
		UV,
		Colour,
		Count
	};

	//
	// A mesh asset.
	//
	struct Mesh : dd::HandleTarget
	{
		//
		// Render this mesh with the given shader.
		//
		void Render( Shader& shader );

		//
		// Retrieve the axis-aligned bounds of this mesh.
		//
		const ddm::AABB& GetBoundBox() const { return m_bounds; }

		//
		// Set the bounds of this mesh.
		//
		void SetBoundBox( const ddm::AABB& bounds ) { m_bounds = bounds; }

		//
		// Set the positions that the mesh will use.
		// The mesh does *NOT* take ownership of this.
		// The pointer in the buffer must remain valid for the lifetime of this mesh.
		//
		void SetPositions( const dd::ConstBuffer<glm::vec3>& positions );

		//
		// Access the currently set buffer for positions.
		// Not guaranteed to be valid.
		//
		dd::Buffer<glm::vec3> AccessPositions() const { return dd::Buffer<glm::vec3>( m_vboPosition.GetData() ); }

		//
		// Get the currently set buffer for positions.
		// Not guaranteed to be valid.
		//
		dd::ConstBuffer<glm::vec3> GetPositions() const { return dd::ConstBuffer<glm::vec3>( m_vboPosition.GetData() ); }

		//
		// Set the index buffer that the mesh will use.
		// The mesh does *NOT* take ownership of this.
		//
		void SetIndices( const dd::ConstBuffer<uint>& indices );

		//
		// Get the currently set buffer for indices.
		// Not guaranteed to be valid. If the mesh does not use indices, then this will never be valid.
		//
		dd::ConstBuffer<uint> GetIndices() const { return dd::ConstBuffer<uint>( m_vboIndex.GetData() ); }

		//
		// Access the currently set buffer for indices.
		// Not guaranteed to be valid. If the mesh does not use indices, then this will never be valid.
		//
		dd::Buffer<uint> AccessIndices() const { return dd::Buffer<uint>( m_vboIndex.GetData() ); }

		//
		// Set the normal buffer that the mesh will use.
		// The mesh does *NOT* take ownership of this.
		//
		void SetNormals( const dd::ConstBuffer<glm::vec3>& normals );

		//
		// Set the vertex colour buffer that the mesh will use.
		// The mesh does *NOT* take ownership of this.
		//
		void SetVertexColours( const dd::ConstBuffer<glm::vec4>& colours );

		//
		// Set the UV buffer that the mesh will use.
		// The mesh does *NOT* take ownership of this.
		//
		void SetUVs( const dd::ConstBuffer<glm::vec2>& uvs );

		//
		// Set the material that this mesh uses.
		//
		void SetMaterial( MaterialHandle material );

		//
		// Get the material this mesh uses.
		//
		MaterialHandle GetMaterial() const { return m_material; }

		//
		// Create the mesh. Must be called on the render thread.
		//
		void Create();

		//
		// Send updated mesh data to the GPU from the same place it currently is.
		//
		void Update( dd::JobSystem& jobsystem );

		//
		// Destroy the mesh. Must be called on the render thread.
		//
		void Destroy();

		//
		// Enable/disable the BVH calculation.
		//
		void UseBVH( bool enabled ) { m_enableBVH = enabled; }

		//
		// Set the mesh to be dirty.
		//
		void SetDirty( BufferType type ) { m_dirty.set( (size_t) type, true ); }

		//
		// Get the BVH of this mesh.
		//
		const dd::BVHTree* GetBVH() const { return m_bvh; }

		Mesh();
		~Mesh();

		Mesh& operator=( const Mesh& ) = delete;
		Mesh& operator=( Mesh&& ) = delete;
		Mesh( const Mesh& ) = delete;
		Mesh( Mesh&& ) = delete;

	private:

		static const size_t DirtyBitCount = (size_t) BufferType::Count;
		std::bitset<DirtyBitCount> m_dirty;
		
		VBO m_vboPosition;
		VBO m_vboNormal;
		VBO m_vboIndex;
		VBO m_vboUV;
		VBO m_vboVertexColour;

		MaterialHandle m_material;
		
		VAO m_vao;

		ddm::AABB m_bounds;

		bool m_enableBVH { true };

		std::atomic<bool> m_rebuilding { false };
		dd::BVHTree* m_bvh { nullptr };

		void BindToShader( Shader& shader );

		void RebuildBVH();
	};

	using MeshHandle = dd::Handle<ddr::Mesh>;

	struct MeshManager : dd::HandleManager<ddr::Mesh>
	{
	private:

		virtual void OnCreate( Mesh& mesh ) const override;
		virtual void OnDestroy( Mesh& mesh ) const override;
	};
}