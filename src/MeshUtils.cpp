//
// MeshUtils.cpp - Various utilities to make meshes of various shapes.
// Copyright (C) Sebastian Nordgren 
// September 25th 2018
//

#include "PCH.h"
#include "MeshUtils.h"

#include "Triangulator.h"

#include "ddr/Material.h"
#include "ddr/Mesh.h"
#include "ddr/Shader.h"
#include "ddr/VBO.h"

namespace dd
{
	struct GridKey
	{
		uint LOD;
		uint VertexCount;

		bool operator==(const GridKey& key) const
		{
			return LOD == other.LOD && VertexCount == other.VertexCount;
		}
	};
}

namespace std
{
	template <>
	struct hash<dd::GridKey>
	{
		size_t operator()(const dd::GridKey& key) const
		{
			return (size_t)(key.VertexCount << 16 | key.LOD);
		}
	};
}

namespace dd
{
	static dd::Service<ddr::MeshManager> s_meshManager;
	static dd::Service<ddr::ShaderManager> s_shaderManager;
	static dd::Service<ddr::MaterialManager> s_materialManager;

	static const glm::vec3 s_unitCubePositions[] =
	{
		//  X    Y    Z     
		// bottom
		glm::vec3(0.0f,	0.0f,	0.0f),
		glm::vec3(1.0f,	0.0f,	0.0f),
		glm::vec3(0.0f,	0.0f,	1.0f),
		glm::vec3(1.0f,	0.0f,	0.0f),
		glm::vec3(1.0f,	0.0f,	1.0f),
		glm::vec3(0.0f,	0.0f,	1.0f),

		// top			 
		glm::vec3(0.0f,	1.0f,	0.0f),
		glm::vec3(0.0f,	1.0f,	1.0f),
		glm::vec3(1.0f,	1.0f,	0.0f),
		glm::vec3(1.0f,	1.0f,	0.0f),
		glm::vec3(0.0f,	1.0f,	1.0f),
		glm::vec3(1.0f,	1.0f,	1.0f),

		// front
		glm::vec3(0.0f,	0.0f,	1.0f),
		glm::vec3(1.0f,	0.0f,	1.0f),
		glm::vec3(0.0f,	1.0f,	1.0f),
		glm::vec3(1.0f,	0.0f,	1.0f),
		glm::vec3(1.0f,	1.0f,	1.0f),
		glm::vec3(0.0f,	1.0f,	1.0f),

		// back
		glm::vec3(0.0f,	0.0f,	0.0f),
		glm::vec3(0.0f,	1.0f,	0.0f),
		glm::vec3(1.0f,	0.0f,	0.0f),
		glm::vec3(1.0f,	0.0f,	0.0f),
		glm::vec3(0.0f,	1.0f,	0.0f),
		glm::vec3(1.0f,	1.0f,	0.0f),

		// left
		glm::vec3(0.0f,	0.0f,	1.0f),
		glm::vec3(0.0f,	1.0f,	0.0f),
		glm::vec3(0.0f,	0.0f,	0.0f),
		glm::vec3(0.0f,	0.0f,	1.0f),
		glm::vec3(0.0f,	1.0f,	1.0f),
		glm::vec3(0.0f,	1.0f,	0.0f),

		// right
		glm::vec3(1.0f,	0.0f,	1.0f),
		glm::vec3(1.0f,	0.0f,	0.0f),
		glm::vec3(1.0f,	1.0f,	0.0f),
		glm::vec3(1.0f,	0.0f,	1.0f),
		glm::vec3(1.0f,	1.0f,	0.0f),
		glm::vec3(1.0f,	1.0f,	1.0f),
	};

	static dd::ConstBuffer<glm::vec3> s_unitCubePositionsBuffer(s_unitCubePositions, ArrayLength(s_unitCubePositions));

	static const glm::vec3 s_unitCubeNormals[] =
	{
		// bottom
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),

		// top
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),

		// front
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),

		// back
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),

		// left
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),

		// right
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f)
	};

	static const dd::ConstBuffer<glm::vec3> s_unitCubeNormalsBuffer(s_unitCubeNormals, ArrayLength(s_unitCubeNormals));

	static const glm::vec2 s_unitCubeUVs[] =
	{
		// U     V
		// bottom
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),

		// top
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),

		// front
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),

		// back
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),

		// left
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),

		// right
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
	};

	static const dd::ConstBuffer<glm::vec2> s_unitCubeUVsBuffer(s_unitCubeUVs, dd::ArrayLength(s_unitCubeUVs));

	// reference: http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
	// https://github.com/caosdoar/spheres/blob/master/src/spheres.cpp
	static const float T = (1.0f + sqrt(5.0f)) / 2.0f;

	static const std::vector<glm::vec3> s_basePosition =
	{
		glm::normalize(glm::vec3(-1,  T, 0)),
		glm::normalize(glm::vec3(1,  T, 0)),
		glm::normalize(glm::vec3(-1, -T, 0)),
		glm::normalize(glm::vec3(1, -T, 0)),

		glm::normalize(glm::vec3(0, -1,  T)),
		glm::normalize(glm::vec3(0,  1,  T)),
		glm::normalize(glm::vec3(0, -1, -T)),
		glm::normalize(glm::vec3(0,  1, -T)),

		glm::normalize(glm::vec3(T, 0, -1)),
		glm::normalize(glm::vec3(T, 0,  1)),
		glm::normalize(glm::vec3(-T, 0, -1)),
		glm::normalize(glm::vec3(-T, 0,  1))
	};

	static const std::vector<uint> s_baseIndex =
	{
		0, 11, 5,
		0, 5, 1,
		0, 1, 7,
		0, 7, 10,
		0, 10, 11,

		1, 5, 9,
		5, 11, 4,
		11, 10, 2,
		10, 7, 6,
		7, 1, 8,

		3, 9, 4,
		3, 4, 2,
		3, 2, 6,
		3, 6, 8,
		3, 8, 9,

		4, 9, 5,
		2, 4, 11,
		6, 2, 10,
		8, 6, 7,
		9, 8, 1
	};

	static std::vector<std::vector<glm::vec3>> s_icospherePositionLODs = { s_basePosition };
	static std::vector<std::vector<uint>> s_icosphereIndexLODs = { s_baseIndex };
	static std::vector<std::vector<glm::vec3>> s_icosphereNormalLODs;

	static void NormalizePositionsIcosphere(std::vector<glm::vec3>& vec)
	{
		for (glm::vec3& v : vec)
		{
			v = glm::normalize(v);
		}
	}

	static void CalculateNormals(const std::vector<glm::vec3>& positions, const std::vector<uint>& indices, std::vector<glm::vec3>& out_normals)
	{
		DD_ASSERT(out_normals.empty());
		out_normals.resize(positions.size());

		Triangulator triangulator(positions, indices);

		for (size_t i = 0; i < triangulator.Size(); ++i)
		{
			ConstTriangle tri = triangulator[i];

			glm::vec3 normal = ddm::NormalFromTriangle(tri.p0, tri.p1, tri.p2);

			out_normals[tri.i0] += normal;
			out_normals[tri.i1] += normal;
			out_normals[tri.i2] += normal;
		}

		for (size_t i = 0; i < out_normals.size(); ++i)
		{
			out_normals[i] = glm::normalize(out_normals[i]);
		}
	}

	static uint64 GetKeyIcosphere(uint a, uint b)
	{
		uint64 low = ddm::min(a, b);
		uint64 high = ddm::max(a, b);

		uint64 key = (low << 32) + high;
		return key;
	}

	static uint GetMidpointIcosphere(uint i0, uint i1, std::vector<glm::vec3>& positions, std::unordered_map<uint64, uint>& vert_cache)
	{
		uint64 key = GetKeyIcosphere(i0, i1);

		auto it = vert_cache.find(key);
		if (it != vert_cache.end())
		{
			return it->second;
		}

		glm::vec3 p0 = positions[i0];
		glm::vec3 p1 = positions[i1];

		glm::vec3 midpoint = (p0 + p1) / 2.0f;

		uint index = (uint)positions.size();
		positions.push_back(midpoint);

		vert_cache.insert(std::make_pair(key, index));

		return index;
	}

	static void SubdivideIcosphere(const std::vector<glm::vec3>& src_pos, const std::vector<uint>& src_idx,
		std::vector<glm::vec3>& dst_pos, std::vector<uint>& dst_idx)
	{
		DD_ASSERT(dst_pos.size() == 0);
		DD_ASSERT(dst_idx.size() == 0);

		dst_pos.reserve(src_pos.size() * 4);
		dst_idx.reserve(src_idx.size() * 4);

		// copy source verts to destination
		for (glm::vec3 v : src_pos)
		{
			dst_pos.push_back(v);
		}

		std::unordered_map<uint64, uint> vert_cache;

		for (size_t i = 0; i < src_idx.size(); i += 3)
		{
			uint i0 = src_idx[i];
			uint i1 = src_idx[i + 1];
			uint i2 = src_idx[i + 2];

			uint i0i1 = GetMidpointIcosphere(i0, i1, dst_pos, vert_cache);
			uint i1i2 = GetMidpointIcosphere(i1, i2, dst_pos, vert_cache);
			uint i2i0 = GetMidpointIcosphere(i2, i0, dst_pos, vert_cache);

			dst_idx.push_back(i0); dst_idx.push_back(i0i1); dst_idx.push_back(i2i0);
			dst_idx.push_back(i1); dst_idx.push_back(i1i2); dst_idx.push_back(i0i1);
			dst_idx.push_back(i2); dst_idx.push_back(i2i0); dst_idx.push_back(i1i2);
			dst_idx.push_back(i0i1); dst_idx.push_back(i1i2); dst_idx.push_back(i2i0);
		}
	}

	static void CalculateIcosphere(std::vector<glm::vec3>*& out_pos, std::vector<uint>*& out_idx, std::vector<glm::vec3>*& out_normals, int iterations)
	{
		DD_ASSERT(iterations <= 6, "Too many iterations! Danger, Will Robinson!");

		if (s_icosphereNormalLODs.empty())
		{
			std::vector<glm::vec3>& normals = s_icosphereNormalLODs.emplace_back();
			CalculateNormals(s_basePosition, s_baseIndex, normals);
		}

		if (s_icospherePositionLODs.size() > iterations)
		{
			out_pos = &s_icospherePositionLODs[iterations];
			out_idx = &s_icosphereIndexLODs[iterations];
			out_normals = &s_icosphereNormalLODs[iterations];
			return;
		}

		std::vector<glm::vec3> src_pos;
		std::vector<uint> src_idx;
		std::vector<glm::vec3> src_norm;

		size_t start = s_icospherePositionLODs.size() - 1;
		for (size_t i = start; i < iterations; ++i)
		{
			src_pos = s_icospherePositionLODs.back();
			src_idx = s_icosphereIndexLODs.back();
			src_norm = s_icosphereNormalLODs.back();

			std::vector<glm::vec3>& dst_pos = s_icospherePositionLODs.emplace_back();
			std::vector<uint>& dst_idx = s_icosphereIndexLODs.emplace_back();
			std::vector<glm::vec3>& dst_norm = s_icosphereNormalLODs.emplace_back();

			SubdivideIcosphere(src_pos, src_idx, dst_pos, dst_idx);
			NormalizePositionsIcosphere(dst_pos);
			CalculateNormals(dst_pos, dst_idx, dst_norm);
		}

		out_pos = &s_icospherePositionLODs[iterations];
		out_idx = &s_icosphereIndexLODs[iterations];
		out_normals = &s_icosphereNormalLODs[iterations];
	}

	void MeshUtils::MakeIcosphere(ddr::Mesh& mesh, int iterations)
	{
		std::vector<glm::vec3>* pos = nullptr;
		std::vector<uint>* idx = nullptr;
		std::vector<glm::vec3>* norm = nullptr;
		CalculateIcosphere(pos, idx, norm, iterations);

		dd::ConstBuffer<glm::vec3> positions(pos->data(), pos->size());
		mesh.SetPositions(positions);

		dd::ConstBuffer<uint> indices(idx->data(), idx->size());
		mesh.SetIndices(indices);

		dd::ConstBuffer<glm::vec3> normals(norm->data(), norm->size());
		mesh.SetNormals(normals);

		ddm::AABB bounds(glm::vec3(-1), glm::vec3(1));
		mesh.SetBoundBox(bounds);
	}

	void MeshUtils::MakeIcosphere(ddr::VBO& positions, ddr::VBO& indices, ddr::VBO& normals, int iterations)
	{
		std::vector<glm::vec3>* pos = nullptr;
		std::vector<uint>* idx = nullptr;
		std::vector<glm::vec3>* norm = nullptr;
		CalculateIcosphere(pos, idx, norm, iterations);

		positions.Create(*pos);
		indices.Create(*idx);
		normals.Create(*norm);
	}

	void MeshUtils::GetLineIndicesFromTriangles(const std::vector<uint>& src, std::vector<uint>& dest)
	{
		DD_ASSERT(dest.empty());

		dest.reserve(src.size() * 2);

		for (int i = 0; i < src.size(); i += 3)
		{
			uint i0 = src[i];
			uint i1 = src[i + 1];
			uint i2 = src[i + 2];

			dest.push_back(i0);
			dest.push_back(i1);

			dest.push_back(i1);
			dest.push_back(i2);

			dest.push_back(i2);
			dest.push_back(i0);
		}
	}

	void MeshUtils::MakeIcosphereLines(ddr::VBO& positions, ddr::VBO& indices, int iterations)
	{
		std::vector<glm::vec3>* pos = nullptr;
		std::vector<uint>* idx = nullptr;
		std::vector<glm::vec3>* norm = nullptr;
		CalculateIcosphere(pos, idx, norm, iterations);

		std::vector<uint> line_indices;
		GetLineIndicesFromTriangles(*idx, line_indices);

		positions.Create(*pos);
		indices.Create(line_indices);
	}

	void MeshUtils::MakeUnitCube(ddr::Mesh& mesh)
	{
		mesh.SetPositions(s_unitCubePositionsBuffer);
		mesh.SetNormals(s_unitCubeNormalsBuffer);
		mesh.SetUVs(s_unitCubeUVsBuffer);

		ddm::AABB bounds;
		bounds.Expand(glm::vec3(0, 0, 0));
		bounds.Expand(glm::vec3(1, 1, 1));
		mesh.SetBoundBox(bounds);
	}

	void MeshUtils::CreateDefaultMaterial()
	{
		ddr::MaterialHandle material_h = s_materialManager->Find("mesh");
		DD_ASSERT(!material_h.IsValid(), "Mesh material has already been created!");

		if (!material_h.IsValid())
		{
			ddr::ShaderHandle shader_h = s_shaderManager->Load("mesh");
			ddr::Shader* shader = shader_h.Access();
			DD_ASSERT(shader != nullptr);

			material_h = s_materialManager->Create("mesh");
			ddr::Material* material = material_h.Access();
			DD_ASSERT(material != nullptr);

			material->Shader = shader_h;

			material->State.BackfaceCulling = true;
			material->State.Blending = false;
			material->State.Depth = true;
		}
	}

	ddr::MaterialHandle MeshUtils::GetDefaultMaterial()
	{
		ddr::MaterialHandle material_h = s_materialManager->Find("mesh"); 
		DD_ASSERT(material_h.IsValid(), "Mesh material has not been created!");
		return material_h;
	}

	ddr::MeshHandle MeshUtils::CreateUnitCube()
	{
		ddr::MeshHandle cube_h = s_meshManager->Find("cube");
		DD_ASSERT(!cube_h.IsValid(), "Unit cube has already been created!");

		if (!cube_h.IsValid())
		{
			cube_h = s_meshManager->Create("cube");

			ddr::Mesh* cube_mesh = cube_h.Access();
			DD_ASSERT(cube_mesh != nullptr);

			MakeUnitCube(*cube_mesh);
		}

		return cube_h;
	}

	ddr::MeshHandle MeshUtils::CreateUnitSphere()
	{
		ddr::MeshHandle sphere_h = s_meshManager->Find("sphere");
		DD_ASSERT(!sphere_h.IsValid(), "Unit sphere has already been created!");

		if (!sphere_h.IsValid())
		{
			sphere_h = s_meshManager->Create("sphere");

			ddr::Mesh* sphere_mesh = sphere_h.Access();
			DD_ASSERT(sphere_mesh != nullptr);

			MakeIcosphere(*sphere_mesh, 2);
		}

		return sphere_h;
	}

	ddr::MeshHandle MeshUtils::CreateQuad()
	{
		ddr::MeshHandle quad_h = s_meshManager->Find("quad");
		DD_ASSERT(!quad_h.IsValid(), "Quad has already been created!");

		if (!quad_h.IsValid())
		{
			quad_h = s_meshManager->Create("quad");

			ddr::Mesh* quad_mesh = quad_h.Access();
			DD_ASSERT(quad_mesh != nullptr);

			MakeQuad(*quad_mesh);
		}

		return quad_h;
	}

	static const glm::vec3 s_quadPositions[] = {
		glm::vec3(-1.0f,0.0f,-1.0f),
		glm::vec3(-1.0f,0.0f,1.0f),
		glm::vec3(1.0f,	0.0f,-1.0f),
		glm::vec3(1.0f,	0.0f,-1.0f),
		glm::vec3(-1.0f,0.0f,1.0f),
		glm::vec3(1.0f,	0.0f,1.0f)
	};

	static const dd::ConstBuffer<glm::vec3> s_quadPositionsBuffer(s_quadPositions, ArrayLength(s_quadPositions));

	static const glm::vec3 s_quadNormals[] = {
		glm::vec3(0.0f,	1.0f,0.0f),
		glm::vec3(0.0f,	1.0f,0.0f),
		glm::vec3(0.0f,	1.0f,0.0f),
		glm::vec3(0.0f,	1.0f,0.0f),
		glm::vec3(0.0f,	1.0f,0.0f),
		glm::vec3(0.0f,	1.0f,0.0f)
	};

	static const dd::ConstBuffer<glm::vec3> s_quadNormalsBuffer(s_quadNormals, ArrayLength(s_quadNormals));

	void MeshUtils::MakeQuad(ddr::Mesh& mesh)
	{
		mesh.SetPositions(s_quadPositionsBuffer);
		mesh.SetNormals(s_quadNormalsBuffer);

		ddm::AABB bounds;
		bounds.Expand(glm::vec3(-1, 0, -1));
		bounds.Expand(glm::vec3(1, 0, 1));
		mesh.SetBoundBox(bounds);
	}

	static std::unordered_map<GridKey, std::vector<uint>*> s_gridIndices;

	const std::vector<uint>& MeshUtils::GetGridIndices(uint vertex_count, uint lod)
	{
		GridKey key;
		key.LOD = lod;
		key.VertexCount = vertex_count;

		auto it = s_gridIndices.find(key);
		if (it != s_gridIndices.end())
		{
			return *it->second;
		}

		const uint stride = 1 << lod;
		const uint lod_vertices = vertex_count / stride;
		const uint row_width = vertex_count + 1;
		const uint index_count = lod_vertices * lod_vertices * 6;

		DD_ASSERT(stride <= vertex_count);

		std::vector<uint>& indices = *new std::vector<uint>();
		indices.reserve(index_count);

		for (uint z = 0; z < vertex_count; z += stride)
		{
			for (uint x = 0; x < vertex_count + 1; x += stride)
			{
				const uint current = z * row_width + x;
				const uint next_row = (z + stride) * row_width + x;

				DD_ASSERT(next_row < row_width* row_width);

				if (x != 0)
				{
					indices.push_back(current);
					indices.push_back(next_row - stride);
					indices.push_back(next_row);
				}

				if (x != vertex_count)
				{
					indices.push_back(current);
					indices.push_back(next_row);
					indices.push_back(current + stride);
				}
			}
		}

		s_gridIndices.insert(std::make_pair(key, &indices));

		return indices;
	}
}
