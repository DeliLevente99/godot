/**************************************************************************/
/*  bpy_mesh_ops.cpp                                                      */
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

#include "bpy_mesh_ops.h"

#include "../utils/bpy_manifold_adapter.h"

#include "core/object/class_db.h"

void BPYMeshOps::_bind_methods() {
	// Face operations
	ClassDB::bind_static_method("BPYMeshOps", D_METHOD("extrude_faces", "mesh", "faces", "direction", "distance"), &BPYMeshOps::extrude_faces, DEFVAL(1.0f));
	ClassDB::bind_static_method("BPYMeshOps", D_METHOD("inset_faces", "mesh", "faces", "thickness", "depth"), &BPYMeshOps::inset_faces, DEFVAL(0.0f));
	ClassDB::bind_static_method("BPYMeshOps", D_METHOD("dissolve_faces", "mesh", "faces"), &BPYMeshOps::dissolve_faces);
	ClassDB::bind_static_method("BPYMeshOps", D_METHOD("poke_faces", "mesh", "faces"), &BPYMeshOps::poke_faces);

	// Edge operations
	ClassDB::bind_static_method("BPYMeshOps", D_METHOD("bevel_edges", "mesh", "edges", "width", "segments"), &BPYMeshOps::bevel_edges, DEFVAL(1));
	ClassDB::bind_static_method("BPYMeshOps", D_METHOD("loop_cut", "mesh", "edge", "cuts"), &BPYMeshOps::loop_cut);
	ClassDB::bind_static_method("BPYMeshOps", D_METHOD("dissolve_edges", "mesh", "edges"), &BPYMeshOps::dissolve_edges);
	ClassDB::bind_static_method("BPYMeshOps", D_METHOD("subdivide_edges", "mesh", "edges", "cuts"), &BPYMeshOps::subdivide_edges);

	// Vertex operations
	ClassDB::bind_static_method("BPYMeshOps", D_METHOD("merge_vertices", "mesh", "vertices", "type"), &BPYMeshOps::merge_vertices, DEFVAL(MERGE_CENTER));
	ClassDB::bind_static_method("BPYMeshOps", D_METHOD("dissolve_vertices", "mesh", "vertices"), &BPYMeshOps::dissolve_vertices);
	ClassDB::bind_static_method("BPYMeshOps", D_METHOD("smooth_vertices", "mesh", "vertices", "factor", "iterations"), &BPYMeshOps::smooth_vertices, DEFVAL(0.5f), DEFVAL(1));

	// Boolean operations
	ClassDB::bind_static_method("BPYMeshOps", D_METHOD("boolean_union", "a", "b"), &BPYMeshOps::boolean_union);
	ClassDB::bind_static_method("BPYMeshOps", D_METHOD("boolean_difference", "a", "b"), &BPYMeshOps::boolean_difference);
	ClassDB::bind_static_method("BPYMeshOps", D_METHOD("boolean_intersection", "a", "b"), &BPYMeshOps::boolean_intersection);

	// Mesh joining
	ClassDB::bind_static_method("BPYMeshOps", D_METHOD("merge_meshes", "a", "b"), &BPYMeshOps::merge_meshes);
	ClassDB::bind_static_method("BPYMeshOps", D_METHOD("join_meshes", "meshes"), &BPYMeshOps::join_meshes);

	// Selection helpers
	ClassDB::bind_static_method("BPYMeshOps", D_METHOD("select_all_faces", "mesh"), &BPYMeshOps::select_all_faces);
	ClassDB::bind_static_method("BPYMeshOps", D_METHOD("select_all_edges", "mesh"), &BPYMeshOps::select_all_edges);
	ClassDB::bind_static_method("BPYMeshOps", D_METHOD("select_all_vertices", "mesh"), &BPYMeshOps::select_all_vertices);
	ClassDB::bind_static_method("BPYMeshOps", D_METHOD("select_linked_faces", "mesh", "face"), &BPYMeshOps::select_linked_faces);

	BIND_ENUM_CONSTANT(MERGE_CENTER);
	BIND_ENUM_CONSTANT(MERGE_FIRST);
	BIND_ENUM_CONSTANT(MERGE_LAST);
	BIND_ENUM_CONSTANT(MERGE_COLLAPSE);
}

void BPYMeshOps::extrude_faces(Ref<BPYMesh> p_mesh, const PackedInt32Array &p_faces, const Vector3 &p_direction, float p_distance) {
	ERR_FAIL_COND(p_mesh.is_null());
	ERR_FAIL_COND(p_faces.is_empty());

	Vector3 offset = p_direction.normalized() * p_distance;

	// Collect boundary edges (edges that belong to only one selected face)
	HashSet<int> selected_faces;
	for (int f : p_faces) {
		if (f >= 0 && f < p_mesh->get_face_count()) {
			selected_faces.insert(f);
		}
	}

	HashMap<int, int> boundary_edge_face; // edge -> face that owns it

	for (int face_idx : selected_faces) {
		for (int loop_idx : p_mesh->faces[face_idx].loops) {
			int edge_idx = p_mesh->loops[loop_idx].edge;
			if (boundary_edge_face.has(edge_idx)) {
				// Edge is shared, remove from boundary
				boundary_edge_face.erase(edge_idx);
			} else {
				boundary_edge_face[edge_idx] = face_idx;
			}
		}
	}

	// Create extruded vertices
	HashMap<int, int> extruded_verts; // old_vert -> new_vert

	for (int face_idx : selected_faces) {
		PackedInt32Array verts = p_mesh->get_face_vertices(face_idx);
		for (int v : verts) {
			if (!extruded_verts.has(v)) {
				Vector3 new_pos = p_mesh->vertices[v].position + offset;
				int new_vert = p_mesh->add_vertex(new_pos);
				p_mesh->vertices[new_vert].normal = p_mesh->vertices[v].normal;
				p_mesh->vertices[new_vert].color = p_mesh->vertices[v].color;
				extruded_verts[v] = new_vert;
			}
		}
	}

	// Update selected faces to use extruded vertices
	for (int face_idx : selected_faces) {
		for (int loop_idx : p_mesh->faces[face_idx].loops) {
			int old_vert = p_mesh->loops[loop_idx].vertex;
			p_mesh->loops[loop_idx].vertex = extruded_verts[old_vert];
		}
	}

	// Create side faces for boundary edges
	for (const KeyValue<int, int> &E : boundary_edge_face) {
		int edge_idx = E.key;
		const BPYEdge &edge = p_mesh->edges[edge_idx];

		int v0 = edge.vertices[0];
		int v1 = edge.vertices[1];

		int new_v0 = extruded_verts[v0];
		int new_v1 = extruded_verts[v1];

		// Create quad connecting old and new edges
		p_mesh->add_quad(v0, v1, new_v1, new_v0);
	}

	p_mesh->recalculate_normals();
}

void BPYMeshOps::inset_faces(Ref<BPYMesh> p_mesh, const PackedInt32Array &p_faces, float p_thickness, float p_depth) {
	ERR_FAIL_COND(p_mesh.is_null());
	ERR_FAIL_COND(p_faces.is_empty());

	for (int face_idx : p_faces) {
		if (face_idx < 0 || face_idx >= p_mesh->get_face_count()) {
			continue;
		}

		const BPYFace &face = p_mesh->faces[face_idx];
		Vector3 center = p_mesh->get_face_center(face_idx);
		Vector3 normal = face.normal;

		// Create inset vertices
		LocalVector<int> inset_verts;
		PackedInt32Array orig_verts = p_mesh->get_face_vertices(face_idx);

		for (int v : orig_verts) {
			Vector3 pos = p_mesh->vertices[v].position;
			Vector3 to_center = (center - pos).normalized();
			Vector3 inset_pos = pos + to_center * p_thickness - normal * p_depth;
			inset_verts.push_back(p_mesh->add_vertex(inset_pos));
		}

		// Create inset face
		PackedInt32Array inset_face_verts;
		for (int v : inset_verts) {
			inset_face_verts.push_back(v);
		}
		int inset_face = p_mesh->add_face(inset_face_verts);
		if (inset_face >= 0) {
			p_mesh->set_face_smooth(inset_face, face.smooth);
			p_mesh->set_face_material_index(inset_face, face.material_index);
		}

		// Create connecting quads between original and inset
		int n = orig_verts.size();
		for (int i = 0; i < n; i++) {
			int next = (i + 1) % n;
			p_mesh->add_quad(
					orig_verts[i],
					orig_verts[next],
					inset_verts[next],
					inset_verts[i]);
		}
	}

	// Remove original faces (mark for removal by setting invalid loops)
	// For simplicity, we leave the original faces - in a full implementation
	// we would remove them

	p_mesh->recalculate_normals();
}

void BPYMeshOps::dissolve_faces(Ref<BPYMesh> p_mesh, const PackedInt32Array &p_faces) {
	ERR_FAIL_COND(p_mesh.is_null());
	// TODO: Implement face dissolve (merge adjacent faces)
}

void BPYMeshOps::poke_faces(Ref<BPYMesh> p_mesh, const PackedInt32Array &p_faces) {
	ERR_FAIL_COND(p_mesh.is_null());

	for (int face_idx : p_faces) {
		if (face_idx < 0 || face_idx >= p_mesh->get_face_count()) {
			continue;
		}

		const BPYFace &face = p_mesh->faces[face_idx];
		Vector3 center = p_mesh->get_face_center(face_idx);

		// Create center vertex
		int center_vert = p_mesh->add_vertex(center);

		// Create triangles from center to each edge
		PackedInt32Array verts = p_mesh->get_face_vertices(face_idx);
		int n = verts.size();

		for (int i = 0; i < n; i++) {
			int next = (i + 1) % n;
			p_mesh->add_triangle(verts[i], verts[next], center_vert);
		}
	}

	p_mesh->recalculate_normals();
}

void BPYMeshOps::bevel_edges(Ref<BPYMesh> p_mesh, const PackedInt32Array &p_edges, float p_width, int p_segments) {
	ERR_FAIL_COND(p_mesh.is_null());
	// TODO: Implement edge bevel
}

PackedInt32Array BPYMeshOps::loop_cut(Ref<BPYMesh> p_mesh, int p_edge, int p_cuts) {
	PackedInt32Array new_edges;
	ERR_FAIL_COND_V(p_mesh.is_null(), new_edges);
	ERR_FAIL_INDEX_V(p_edge, p_mesh->get_edge_count(), new_edges);

	// TODO: Implement loop cut (find edge loop, cut all edges)
	return new_edges;
}

void BPYMeshOps::dissolve_edges(Ref<BPYMesh> p_mesh, const PackedInt32Array &p_edges) {
	ERR_FAIL_COND(p_mesh.is_null());
	// TODO: Implement edge dissolve
}

void BPYMeshOps::subdivide_edges(Ref<BPYMesh> p_mesh, const PackedInt32Array &p_edges, int p_cuts) {
	ERR_FAIL_COND(p_mesh.is_null());

	for (int edge_idx : p_edges) {
		if (edge_idx < 0 || edge_idx >= p_mesh->get_edge_count()) {
			continue;
		}

		const BPYEdge &edge = p_mesh->edges[edge_idx];
		int v0 = edge.vertices[0];
		int v1 = edge.vertices[1];

		Vector3 p0 = p_mesh->vertices[v0].position;
		Vector3 p1 = p_mesh->vertices[v1].position;

		// Create intermediate vertices
		for (int i = 1; i <= p_cuts; i++) {
			float t = (float)i / (p_cuts + 1);
			Vector3 pos = p0.lerp(p1, t);
			p_mesh->add_vertex(pos);
		}
	}

	// Note: This only creates vertices, not the connecting edges/faces
	// Full implementation would rebuild affected faces
}

void BPYMeshOps::merge_vertices(Ref<BPYMesh> p_mesh, const PackedInt32Array &p_vertices, MergeType p_type) {
	ERR_FAIL_COND(p_mesh.is_null());
	ERR_FAIL_COND(p_vertices.size() < 2);

	Vector3 target_pos;

	switch (p_type) {
		case MERGE_CENTER: {
			for (int v : p_vertices) {
				if (v >= 0 && v < p_mesh->get_vertex_count()) {
					target_pos += p_mesh->vertices[v].position;
				}
			}
			target_pos /= (float)p_vertices.size();
		} break;

		case MERGE_FIRST: {
			if (p_vertices[0] >= 0 && p_vertices[0] < p_mesh->get_vertex_count()) {
				target_pos = p_mesh->vertices[p_vertices[0]].position;
			}
		} break;

		case MERGE_LAST: {
			int last = p_vertices[p_vertices.size() - 1];
			if (last >= 0 && last < p_mesh->get_vertex_count()) {
				target_pos = p_mesh->vertices[last].position;
			}
		} break;

		case MERGE_COLLAPSE: {
			// Same as center for now
			for (int v : p_vertices) {
				if (v >= 0 && v < p_mesh->get_vertex_count()) {
					target_pos += p_mesh->vertices[v].position;
				}
			}
			target_pos /= (float)p_vertices.size();
		} break;
	}

	// Move all vertices to target position, then merge
	for (int v : p_vertices) {
		if (v >= 0 && v < p_mesh->get_vertex_count()) {
			p_mesh->vertices[v].position = target_pos;
		}
	}

	p_mesh->merge_vertices(0.0001f);
}

void BPYMeshOps::dissolve_vertices(Ref<BPYMesh> p_mesh, const PackedInt32Array &p_vertices) {
	ERR_FAIL_COND(p_mesh.is_null());
	// TODO: Implement vertex dissolve
}

void BPYMeshOps::smooth_vertices(Ref<BPYMesh> p_mesh, const PackedInt32Array &p_vertices, float p_factor, int p_iterations) {
	ERR_FAIL_COND(p_mesh.is_null());

	HashSet<int> smooth_verts;
	for (int v : p_vertices) {
		if (v >= 0 && v < p_mesh->get_vertex_count()) {
			smooth_verts.insert(v);
		}
	}

	for (int iter = 0; iter < p_iterations; iter++) {
		LocalVector<Vector3> new_positions;
		new_positions.resize(p_mesh->vertices.size());

		for (int i = 0; i < (int)p_mesh->vertices.size(); i++) {
			new_positions[i] = p_mesh->vertices[i].position;
		}

		for (int v : smooth_verts) {
			// Average with connected vertices
			Vector3 avg;
			int count = 0;

			for (int edge_idx : p_mesh->vertices[v].edges) {
				int other = p_mesh->edges[edge_idx].other_vertex(v);
				avg += p_mesh->vertices[other].position;
				count++;
			}

			if (count > 0) {
				avg /= (float)count;
				new_positions[v] = p_mesh->vertices[v].position.lerp(avg, p_factor);
			}
		}

		for (int v : smooth_verts) {
			p_mesh->vertices[v].position = new_positions[v];
		}
	}

	p_mesh->recalculate_normals();
}

Ref<BPYMesh> BPYMeshOps::boolean_union(const Ref<BPYMesh> &p_a, const Ref<BPYMesh> &p_b) {
	return BPYManifoldAdapter::boolean_union(p_a, p_b);
}

Ref<BPYMesh> BPYMeshOps::boolean_difference(const Ref<BPYMesh> &p_a, const Ref<BPYMesh> &p_b) {
	return BPYManifoldAdapter::boolean_difference(p_a, p_b);
}

Ref<BPYMesh> BPYMeshOps::boolean_intersection(const Ref<BPYMesh> &p_a, const Ref<BPYMesh> &p_b) {
	return BPYManifoldAdapter::boolean_intersection(p_a, p_b);
}

Ref<BPYMesh> BPYMeshOps::merge_meshes(const Ref<BPYMesh> &p_a, const Ref<BPYMesh> &p_b) {
	Ref<BPYMesh> result;
	result.instantiate();

	if (p_a.is_null() && p_b.is_null()) {
		return result;
	}

	// Helper lambda to copy a mesh into result
	auto copy_mesh = [&](const Ref<BPYMesh> &mesh) {
		if (mesh.is_null()) {
			return;
		}

		int vert_offset = result->vertices.size();

		// Copy vertices
		for (int i = 0; i < (int)mesh->vertices.size(); i++) {
			int new_vert = result->add_vertex(mesh->vertices[i].position);
			result->vertices[new_vert].normal = mesh->vertices[i].normal;
			result->vertices[new_vert].color = mesh->vertices[i].color;
			result->vertices[new_vert].crease = mesh->vertices[i].crease;
		}

		// Copy faces
		for (int face_idx = 0; face_idx < (int)mesh->faces.size(); face_idx++) {
			PackedInt32Array verts = mesh->get_face_vertices(face_idx);
			PackedInt32Array new_verts;

			for (int v : verts) {
				new_verts.push_back(v + vert_offset);
			}

			int new_face = result->add_face(new_verts);
			if (new_face >= 0) {
				result->set_face_smooth(new_face, mesh->get_face_smooth(face_idx));
				result->set_face_material_index(new_face, mesh->get_face_material_index(face_idx));

				// Copy loop UVs
				const BPYFace &orig_face = mesh->faces[face_idx];
				for (int i = 0; i < (int)result->faces[new_face].loops.size(); i++) {
					int orig_loop = orig_face.loops[i];
					int new_loop = result->faces[new_face].loops[i];
					result->set_loop_uv(new_loop, mesh->get_loop_uv(orig_loop));
				}
			}
		}
	};

	copy_mesh(p_a);
	copy_mesh(p_b);

	return result;
}

Ref<BPYMesh> BPYMeshOps::join_meshes(const TypedArray<BPYMesh> &p_meshes) {
	Ref<BPYMesh> result;
	result.instantiate();

	for (int mesh_idx = 0; mesh_idx < p_meshes.size(); mesh_idx++) {
		Ref<BPYMesh> mesh = p_meshes[mesh_idx];
		if (mesh.is_null()) {
			continue;
		}

		int vert_offset = result->vertices.size();

		// Copy vertices
		for (int i = 0; i < (int)mesh->vertices.size(); i++) {
			int new_vert = result->add_vertex(mesh->vertices[i].position);
			result->vertices[new_vert].normal = mesh->vertices[i].normal;
			result->vertices[new_vert].color = mesh->vertices[i].color;
			result->vertices[new_vert].crease = mesh->vertices[i].crease;
		}

		// Copy faces
		for (int face_idx = 0; face_idx < (int)mesh->faces.size(); face_idx++) {
			PackedInt32Array verts = mesh->get_face_vertices(face_idx);
			PackedInt32Array new_verts;

			for (int v : verts) {
				new_verts.push_back(v + vert_offset);
			}

			int new_face = result->add_face(new_verts);
			if (new_face >= 0) {
				result->set_face_smooth(new_face, mesh->get_face_smooth(face_idx));
				result->set_face_material_index(new_face, mesh->get_face_material_index(face_idx));

				// Copy loop UVs
				const BPYFace &orig_face = mesh->faces[face_idx];
				for (int i = 0; i < (int)result->faces[new_face].loops.size(); i++) {
					int orig_loop = orig_face.loops[i];
					int new_loop = result->faces[new_face].loops[i];
					result->set_loop_uv(new_loop, mesh->get_loop_uv(orig_loop));
				}
			}
		}
	}

	return result;
}

void BPYMeshOps::separate_by_material(const Ref<BPYMesh> &p_mesh, TypedArray<BPYMesh> &r_meshes) {
	ERR_FAIL_COND(p_mesh.is_null());

	// Group faces by material
	HashMap<int, LocalVector<int>> faces_by_material;
	for (int i = 0; i < (int)p_mesh->faces.size(); i++) {
		faces_by_material[p_mesh->faces[i].material_index].push_back(i);
	}

	// Create separate meshes
	for (const KeyValue<int, LocalVector<int>> &E : faces_by_material) {
		Ref<BPYMesh> new_mesh;
		new_mesh.instantiate();

		HashMap<int, int> vert_map;

		for (int face_idx : E.value) {
			PackedInt32Array verts = p_mesh->get_face_vertices(face_idx);
			PackedInt32Array new_verts;

			for (int v : verts) {
				if (!vert_map.has(v)) {
					int new_v = new_mesh->add_vertex(p_mesh->vertices[v].position);
					new_mesh->vertices[new_v].normal = p_mesh->vertices[v].normal;
					new_mesh->vertices[new_v].color = p_mesh->vertices[v].color;
					vert_map[v] = new_v;
				}
				new_verts.push_back(vert_map[v]);
			}

			int new_face = new_mesh->add_face(new_verts);
			if (new_face >= 0) {
				new_mesh->set_face_smooth(new_face, p_mesh->get_face_smooth(face_idx));
				new_mesh->set_face_material_index(new_face, 0); // Reset to 0

				// Copy UVs
				const BPYFace &orig_face = p_mesh->faces[face_idx];
				for (int i = 0; i < (int)new_mesh->faces[new_face].loops.size(); i++) {
					int orig_loop = orig_face.loops[i];
					int new_loop = new_mesh->faces[new_face].loops[i];
					new_mesh->set_loop_uv(new_loop, p_mesh->get_loop_uv(orig_loop));
				}
			}
		}

		r_meshes.push_back(new_mesh);
	}
}

PackedInt32Array BPYMeshOps::select_all_faces(const Ref<BPYMesh> &p_mesh) {
	PackedInt32Array result;
	ERR_FAIL_COND_V(p_mesh.is_null(), result);

	for (int i = 0; i < p_mesh->get_face_count(); i++) {
		result.push_back(i);
	}
	return result;
}

PackedInt32Array BPYMeshOps::select_all_edges(const Ref<BPYMesh> &p_mesh) {
	PackedInt32Array result;
	ERR_FAIL_COND_V(p_mesh.is_null(), result);

	for (int i = 0; i < p_mesh->get_edge_count(); i++) {
		result.push_back(i);
	}
	return result;
}

PackedInt32Array BPYMeshOps::select_all_vertices(const Ref<BPYMesh> &p_mesh) {
	PackedInt32Array result;
	ERR_FAIL_COND_V(p_mesh.is_null(), result);

	for (int i = 0; i < p_mesh->get_vertex_count(); i++) {
		result.push_back(i);
	}
	return result;
}

PackedInt32Array BPYMeshOps::select_linked_faces(const Ref<BPYMesh> &p_mesh, int p_face) {
	PackedInt32Array result;
	ERR_FAIL_COND_V(p_mesh.is_null(), result);
	ERR_FAIL_INDEX_V(p_face, p_mesh->get_face_count(), result);

	HashSet<int> visited;
	LocalVector<int> queue;
	queue.push_back(p_face);

	while (!queue.is_empty()) {
		int face_idx = queue[queue.size() - 1];
		queue.remove_at(queue.size() - 1);

		if (visited.has(face_idx)) {
			continue;
		}
		visited.insert(face_idx);
		result.push_back(face_idx);

		// Find adjacent faces via shared edges
		for (int loop_idx : p_mesh->faces[face_idx].loops) {
			int edge_idx = p_mesh->loops[loop_idx].edge;
			for (int adj_face : p_mesh->edges[edge_idx].faces) {
				if (!visited.has(adj_face)) {
					queue.push_back(adj_face);
				}
			}
		}
	}

	return result;
}

BPYMeshOps::BPYMeshOps() {
}
