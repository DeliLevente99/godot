/**************************************************************************/
/*  bpy_manifold_adapter.cpp                                              */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "bpy_manifold_adapter.h"

manifold::Manifold BPYManifoldAdapter::bpy_mesh_to_manifold(const Ref<BPYMesh> &p_mesh) {
	ERR_FAIL_COND_V(p_mesh.is_null(), manifold::Manifold());

	// Triangulate mesh first
	Ref<BPYMesh> tri_mesh = p_mesh->duplicate();
	tri_mesh->triangulate();

	manifold::MeshGL64 mesh_gl;
	mesh_gl.numProp = MANIFOLD_PROPERTY_MAX;

	// Reserve space
	int num_triangles = tri_mesh->faces.size();
	mesh_gl.vertProperties.reserve(num_triangles * 3 * MANIFOLD_PROPERTY_MAX);
	mesh_gl.triVerts.reserve(num_triangles * 3);

	// Build vertex properties for each triangle vertex
	// Note: We need to swap winding order (Godot uses CCW, Manifold uses CW)
	constexpr int order[3] = { 0, 2, 1 };

	for (int face_idx = 0; face_idx < (int)tri_mesh->faces.size(); face_idx++) {
		const BPYFace &face = tri_mesh->faces[face_idx];
		ERR_CONTINUE(face.loops.size() != 3);

		for (int i = 0; i < 3; i++) {
			int loop_idx = face.loops[order[i]];
			const BPYLoop &loop = tri_mesh->loops[loop_idx];
			const BPYVertex &vert = tri_mesh->vertices[loop.vertex];

			mesh_gl.triVerts.push_back(mesh_gl.vertProperties.size() / MANIFOLD_PROPERTY_MAX);

			mesh_gl.vertProperties.push_back(vert.position.x);
			mesh_gl.vertProperties.push_back(vert.position.y);
			mesh_gl.vertProperties.push_back(vert.position.z);
			mesh_gl.vertProperties.push_back(loop.normal.x);
			mesh_gl.vertProperties.push_back(loop.normal.y);
			mesh_gl.vertProperties.push_back(loop.normal.z);
			mesh_gl.vertProperties.push_back(loop.uv.x);
			mesh_gl.vertProperties.push_back(loop.uv.y);
		}
	}

	mesh_gl.runIndex.push_back(0);
	mesh_gl.runIndex.push_back(mesh_gl.triVerts.size());
	mesh_gl.tolerance = 2 * FLT_EPSILON;
	mesh_gl.Merge();

	return manifold::Manifold(mesh_gl);
}

Ref<BPYMesh> BPYManifoldAdapter::manifold_to_bpy_mesh(const manifold::Manifold &p_manifold) {
	Ref<BPYMesh> mesh;
	mesh.instantiate();

	manifold::MeshGL64 mesh_gl = p_manifold.GetMeshGL64();

	if (mesh_gl.triVerts.empty()) {
		return mesh;
	}

	// Swap winding order back
	constexpr int order[3] = { 0, 2, 1 };

	// Create vertices and faces from the manifold mesh
	HashMap<Vector3, int> vertex_map;

	for (size_t tri_idx = 0; tri_idx < mesh_gl.triVerts.size(); tri_idx += 3) {
		int vert_indices[3];
		Vector3 positions[3];
		Vector3 normals[3];
		Vector2 uvs[3];

		for (int i = 0; i < 3; i++) {
			size_t prop_idx = mesh_gl.triVerts[tri_idx + order[i]];
			size_t base = prop_idx * mesh_gl.numProp;

			positions[i] = Vector3(
					mesh_gl.vertProperties[base + MANIFOLD_PROPERTY_POSITION_X],
					mesh_gl.vertProperties[base + MANIFOLD_PROPERTY_POSITION_Y],
					mesh_gl.vertProperties[base + MANIFOLD_PROPERTY_POSITION_Z]);

			if (mesh_gl.numProp > MANIFOLD_PROPERTY_NORMAL_Z) {
				normals[i] = Vector3(
						mesh_gl.vertProperties[base + MANIFOLD_PROPERTY_NORMAL_X],
						mesh_gl.vertProperties[base + MANIFOLD_PROPERTY_NORMAL_Y],
						mesh_gl.vertProperties[base + MANIFOLD_PROPERTY_NORMAL_Z]);
			}

			if (mesh_gl.numProp > MANIFOLD_PROPERTY_UV_Y) {
				uvs[i] = Vector2(
						mesh_gl.vertProperties[base + MANIFOLD_PROPERTY_UV_X],
						mesh_gl.vertProperties[base + MANIFOLD_PROPERTY_UV_Y]);
			}

			// Find or create vertex
			if (vertex_map.has(positions[i])) {
				vert_indices[i] = vertex_map[positions[i]];
			} else {
				vert_indices[i] = mesh->add_vertex(positions[i]);
				vertex_map[positions[i]] = vert_indices[i];
			}
		}

		// Skip degenerate triangles
		if (vert_indices[0] == vert_indices[1] ||
				vert_indices[1] == vert_indices[2] ||
				vert_indices[2] == vert_indices[0]) {
			continue;
		}

		int face_idx = mesh->add_triangle(vert_indices[0], vert_indices[1], vert_indices[2]);
		if (face_idx >= 0) {
			// Set loop UVs and normals
			for (int i = 0; i < 3; i++) {
				int loop_idx = mesh->faces[face_idx].loops[i];
				mesh->set_loop_uv(loop_idx, uvs[i]);
				mesh->set_loop_normal(loop_idx, normals[i]);
			}
		}
	}

	return mesh;
}

Ref<BPYMesh> BPYManifoldAdapter::boolean_union(const Ref<BPYMesh> &p_a, const Ref<BPYMesh> &p_b) {
	ERR_FAIL_COND_V(p_a.is_null() || p_b.is_null(), Ref<BPYMesh>());

	manifold::Manifold a = bpy_mesh_to_manifold(p_a);
	manifold::Manifold b = bpy_mesh_to_manifold(p_b);

	manifold::Manifold result = a + b;
	return manifold_to_bpy_mesh(result);
}

Ref<BPYMesh> BPYManifoldAdapter::boolean_difference(const Ref<BPYMesh> &p_a, const Ref<BPYMesh> &p_b) {
	ERR_FAIL_COND_V(p_a.is_null() || p_b.is_null(), Ref<BPYMesh>());

	manifold::Manifold a = bpy_mesh_to_manifold(p_a);
	manifold::Manifold b = bpy_mesh_to_manifold(p_b);

	manifold::Manifold result = a - b;
	return manifold_to_bpy_mesh(result);
}

Ref<BPYMesh> BPYManifoldAdapter::boolean_intersection(const Ref<BPYMesh> &p_a, const Ref<BPYMesh> &p_b) {
	ERR_FAIL_COND_V(p_a.is_null() || p_b.is_null(), Ref<BPYMesh>());

	manifold::Manifold a = bpy_mesh_to_manifold(p_a);
	manifold::Manifold b = bpy_mesh_to_manifold(p_b);

	manifold::Manifold result = a ^ b;
	return manifold_to_bpy_mesh(result);
}
