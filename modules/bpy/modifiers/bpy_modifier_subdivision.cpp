/**************************************************************************/
/*  bpy_modifier_subdivision.cpp                                          */
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

#include "bpy_modifier_subdivision.h"

#include "core/object/class_db.h"

void BPYModifierSubdivision::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_levels", "levels"), &BPYModifierSubdivision::set_levels);
	ClassDB::bind_method(D_METHOD("get_levels"), &BPYModifierSubdivision::get_levels);
	ClassDB::bind_method(D_METHOD("set_subdivision_type", "type"), &BPYModifierSubdivision::set_subdivision_type);
	ClassDB::bind_method(D_METHOD("get_subdivision_type"), &BPYModifierSubdivision::get_subdivision_type);
	ClassDB::bind_method(D_METHOD("set_use_creases", "enabled"), &BPYModifierSubdivision::set_use_creases);
	ClassDB::bind_method(D_METHOD("get_use_creases"), &BPYModifierSubdivision::get_use_creases);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "levels", PROPERTY_HINT_RANGE, "0,6,1"), "set_levels", "get_levels");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "subdivision_type", PROPERTY_HINT_ENUM, "Catmull-Clark,Simple"), "set_subdivision_type", "get_subdivision_type");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_creases"), "set_use_creases", "get_use_creases");

	BIND_ENUM_CONSTANT(SUBDIVISION_CATMULL_CLARK);
	BIND_ENUM_CONSTANT(SUBDIVISION_SIMPLE);
}

void BPYModifierSubdivision::set_levels(int p_levels) { levels = CLAMP(p_levels, 0, 6); }
int BPYModifierSubdivision::get_levels() const { return levels; }

void BPYModifierSubdivision::set_subdivision_type(SubdivisionType p_type) { subdivision_type = p_type; }
BPYModifierSubdivision::SubdivisionType BPYModifierSubdivision::get_subdivision_type() const { return subdivision_type; }

void BPYModifierSubdivision::set_use_creases(bool p_enabled) { use_creases = p_enabled; }
bool BPYModifierSubdivision::get_use_creases() const { return use_creases; }

Ref<BPYMesh> BPYModifierSubdivision::_subdivide_simple(const Ref<BPYMesh> &p_mesh) {
	Ref<BPYMesh> result;
	result.instantiate();

	// Copy original vertices
	HashMap<int, int> vert_map;
	for (int i = 0; i < (int)p_mesh->vertices.size(); i++) {
		vert_map[i] = result->add_vertex(p_mesh->vertices[i].position);
		result->vertices[vert_map[i]].crease = p_mesh->vertices[i].crease;
	}

	// Create edge midpoints
	HashMap<uint64_t, int> edge_vert_map;
	for (int i = 0; i < (int)p_mesh->edges.size(); i++) {
		int v0 = p_mesh->edges[i].vertices[0];
		int v1 = p_mesh->edges[i].vertices[1];

		Vector3 mid = (p_mesh->vertices[v0].position + p_mesh->vertices[v1].position) * 0.5f;
		uint64_t key = ((uint64_t)MIN(v0, v1) << 32) | MAX(v0, v1);
		edge_vert_map[key] = result->add_vertex(mid);
		result->vertices[edge_vert_map[key]].crease = (p_mesh->vertices[v0].crease + p_mesh->vertices[v1].crease) * 0.5f;
	}

	// Subdivide each face
	for (int face_idx = 0; face_idx < (int)p_mesh->faces.size(); face_idx++) {
		const BPYFace &face = p_mesh->faces[face_idx];
		PackedInt32Array orig_verts = p_mesh->get_face_vertices(face_idx);
		int n = orig_verts.size();

		// Create face center vertex
		Vector3 center;
		for (int i = 0; i < n; i++) {
			center += p_mesh->vertices[orig_verts[i]].position;
		}
		center /= (float)n;
		int center_vert = result->add_vertex(center);

		// Create sub-faces
		for (int i = 0; i < n; i++) {
			int v_curr = orig_verts[i];
			int v_next = orig_verts[(i + 1) % n];
			int v_prev = orig_verts[(i - 1 + n) % n];

			uint64_t edge_key_prev = ((uint64_t)MIN(v_prev, v_curr) << 32) | MAX(v_prev, v_curr);
			uint64_t edge_key_next = ((uint64_t)MIN(v_curr, v_next) << 32) | MAX(v_curr, v_next);

			int edge_prev = edge_vert_map[edge_key_prev];
			int edge_next = edge_vert_map[edge_key_next];

			int new_face = result->add_quad(
					vert_map[v_curr],
					edge_next,
					center_vert,
					edge_prev);

			if (new_face >= 0) {
				result->set_face_smooth(new_face, face.smooth);
				result->set_face_material_index(new_face, face.material_index);
			}
		}
	}

	result->recalculate_normals();
	return result;
}

Ref<BPYMesh> BPYModifierSubdivision::_subdivide_catmull_clark(const Ref<BPYMesh> &p_mesh) {
	Ref<BPYMesh> result;
	result.instantiate();

	int num_verts = p_mesh->vertices.size();
	int num_edges = p_mesh->edges.size();
	int num_faces = p_mesh->faces.size();

	// Step 1: Create face points (average of face vertices)
	LocalVector<int> face_points;
	face_points.resize(num_faces);

	for (int face_idx = 0; face_idx < num_faces; face_idx++) {
		Vector3 center;
		PackedInt32Array verts = p_mesh->get_face_vertices(face_idx);
		for (int v : verts) {
			center += p_mesh->vertices[v].position;
		}
		center /= (float)verts.size();
		face_points[face_idx] = result->add_vertex(center);
	}

	// Step 2: Create edge points
	HashMap<uint64_t, int> edge_points;

	for (int edge_idx = 0; edge_idx < num_edges; edge_idx++) {
		const BPYEdge &edge = p_mesh->edges[edge_idx];
		int v0 = edge.vertices[0];
		int v1 = edge.vertices[1];

		Vector3 edge_point;
		float crease = use_creases ? edge.crease : 0.0f;

		if (edge.faces.size() == 2 && crease < 1.0f) {
			// Smooth edge: average of edge vertices and adjacent face points
			Vector3 sum = p_mesh->vertices[v0].position + p_mesh->vertices[v1].position;
			for (int f : edge.faces) {
				sum += result->vertices[face_points[f]].position;
			}
			Vector3 smooth_point = sum / 4.0f;

			// Sharp edge: midpoint
			Vector3 sharp_point = (p_mesh->vertices[v0].position + p_mesh->vertices[v1].position) * 0.5f;

			// Blend based on crease
			edge_point = sharp_point.lerp(smooth_point, 1.0f - crease);
		} else {
			// Boundary or sharp edge: midpoint
			edge_point = (p_mesh->vertices[v0].position + p_mesh->vertices[v1].position) * 0.5f;
		}

		uint64_t key = ((uint64_t)MIN(v0, v1) << 32) | MAX(v0, v1);
		edge_points[key] = result->add_vertex(edge_point);
	}

	// Step 3: Move original vertices
	LocalVector<int> new_verts;
	new_verts.resize(num_verts);

	for (int vert_idx = 0; vert_idx < num_verts; vert_idx++) {
		const BPYVertex &vert = p_mesh->vertices[vert_idx];
		float vert_crease = use_creases ? vert.crease : 0.0f;

		// Count adjacent faces and sharp edges
		HashSet<int> adj_faces;
		LocalVector<int> sharp_edges;
		LocalVector<int> boundary_edges;

		for (int edge_idx : vert.edges) {
			const BPYEdge &edge = p_mesh->edges[edge_idx];
			float edge_crease = use_creases ? edge.crease : 0.0f;

			if (edge.faces.size() == 1) {
				boundary_edges.push_back(edge_idx);
			} else if (edge_crease >= 1.0f) {
				sharp_edges.push_back(edge_idx);
			}

			for (int f : edge.faces) {
				adj_faces.insert(f);
			}
		}

		Vector3 new_pos;

		if (boundary_edges.size() >= 2 || sharp_edges.size() >= 2 || vert_crease >= 1.0f) {
			// Corner vertex: keep position
			new_pos = vert.position;
		} else if (boundary_edges.size() == 2) {
			// Boundary vertex: average with boundary edge midpoints
			Vector3 sum = vert.position * 6.0f;
			for (int edge_idx : boundary_edges) {
				int other = p_mesh->edges[edge_idx].other_vertex(vert_idx);
				sum += p_mesh->vertices[other].position;
			}
			new_pos = sum / 8.0f;
		} else if (sharp_edges.size() == 2) {
			// Crease vertex: average along crease
			Vector3 sum = vert.position * 6.0f;
			for (int edge_idx : sharp_edges) {
				int other = p_mesh->edges[edge_idx].other_vertex(vert_idx);
				sum += p_mesh->vertices[other].position;
			}
			new_pos = sum / 8.0f;
		} else {
			// Interior smooth vertex: Catmull-Clark formula
			int n = adj_faces.size();
			if (n > 0) {
				Vector3 F; // Average of face points
				for (int f : adj_faces) {
					F += result->vertices[face_points[f]].position;
				}
				F /= (float)n;

				Vector3 R; // Average of edge midpoints
				for (int edge_idx : vert.edges) {
					const BPYEdge &edge = p_mesh->edges[edge_idx];
					int other = edge.other_vertex(vert_idx);
					R += (vert.position + p_mesh->vertices[other].position) * 0.5f;
				}
				R /= (float)vert.edges.size();

				new_pos = (F + R * 2.0f + vert.position * (float)(n - 3)) / (float)n;
			} else {
				new_pos = vert.position;
			}
		}

		new_verts[vert_idx] = result->add_vertex(new_pos);
		result->vertices[new_verts[vert_idx]].crease = vert.crease;
	}

	// Step 4: Create new faces
	for (int face_idx = 0; face_idx < num_faces; face_idx++) {
		const BPYFace &face = p_mesh->faces[face_idx];
		PackedInt32Array orig_verts = p_mesh->get_face_vertices(face_idx);
		int n = orig_verts.size();

		for (int i = 0; i < n; i++) {
			int v_curr = orig_verts[i];
			int v_next = orig_verts[(i + 1) % n];
			int v_prev = orig_verts[(i - 1 + n) % n];

			uint64_t edge_key_prev = ((uint64_t)MIN(v_prev, v_curr) << 32) | MAX(v_prev, v_curr);
			uint64_t edge_key_next = ((uint64_t)MIN(v_curr, v_next) << 32) | MAX(v_curr, v_next);

			int new_face = result->add_quad(
					new_verts[v_curr],
					edge_points[edge_key_next],
					face_points[face_idx],
					edge_points[edge_key_prev]);

			if (new_face >= 0) {
				result->set_face_smooth(new_face, true);
				result->set_face_material_index(new_face, face.material_index);
			}
		}
	}

	result->recalculate_normals();
	return result;
}

Ref<BPYMesh> BPYModifierSubdivision::apply(const Ref<BPYMesh> &p_mesh) {
	if (!enabled || p_mesh.is_null() || levels == 0) {
		return p_mesh;
	}

	Ref<BPYMesh> result = p_mesh->duplicate();

	for (int i = 0; i < levels; i++) {
		switch (subdivision_type) {
			case SUBDIVISION_SIMPLE:
				result = _subdivide_simple(result);
				break;
			case SUBDIVISION_CATMULL_CLARK:
				result = _subdivide_catmull_clark(result);
				break;
		}
	}

	return result;
}

BPYModifierSubdivision::BPYModifierSubdivision() {
}
