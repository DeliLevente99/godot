/**************************************************************************/
/*  bpy_modifier_bevel.cpp                                                */
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

#include "bpy_modifier_bevel.h"

#include "core/object/class_db.h"

void BPYModifierBevel::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_width", "width"), &BPYModifierBevel::set_width);
	ClassDB::bind_method(D_METHOD("get_width"), &BPYModifierBevel::get_width);
	ClassDB::bind_method(D_METHOD("set_segments", "segments"), &BPYModifierBevel::set_segments);
	ClassDB::bind_method(D_METHOD("get_segments"), &BPYModifierBevel::get_segments);
	ClassDB::bind_method(D_METHOD("set_limit_method", "method"), &BPYModifierBevel::set_limit_method);
	ClassDB::bind_method(D_METHOD("get_limit_method"), &BPYModifierBevel::get_limit_method);
	ClassDB::bind_method(D_METHOD("set_angle_limit", "angle"), &BPYModifierBevel::set_angle_limit);
	ClassDB::bind_method(D_METHOD("get_angle_limit"), &BPYModifierBevel::get_angle_limit);
	ClassDB::bind_method(D_METHOD("set_clamp_overlap", "clamp"), &BPYModifierBevel::set_clamp_overlap);
	ClassDB::bind_method(D_METHOD("get_clamp_overlap"), &BPYModifierBevel::get_clamp_overlap);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "width", PROPERTY_HINT_RANGE, "0,10,0.001"), "set_width", "get_width");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "segments", PROPERTY_HINT_RANGE, "1,10,1"), "set_segments", "get_segments");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "limit_method", PROPERTY_HINT_ENUM, "None,Angle,Weight"), "set_limit_method", "get_limit_method");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "angle_limit", PROPERTY_HINT_RANGE, "0,180,0.1,radians_as_degrees"), "set_angle_limit", "get_angle_limit");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "clamp_overlap"), "set_clamp_overlap", "get_clamp_overlap");

	BIND_ENUM_CONSTANT(LIMIT_NONE);
	BIND_ENUM_CONSTANT(LIMIT_ANGLE);
	BIND_ENUM_CONSTANT(LIMIT_WEIGHT);
}

void BPYModifierBevel::set_width(float p_width) { width = MAX(0.0f, p_width); }
float BPYModifierBevel::get_width() const { return width; }

void BPYModifierBevel::set_segments(int p_segments) { segments = CLAMP(p_segments, 1, 10); }
int BPYModifierBevel::get_segments() const { return segments; }

void BPYModifierBevel::set_limit_method(LimitMethod p_method) { limit_method = p_method; }
BPYModifierBevel::LimitMethod BPYModifierBevel::get_limit_method() const { return limit_method; }

void BPYModifierBevel::set_angle_limit(float p_angle) { angle_limit = CLAMP(p_angle, 0.0f, Math::PI); }
float BPYModifierBevel::get_angle_limit() const { return angle_limit; }

void BPYModifierBevel::set_clamp_overlap(bool p_clamp) { clamp_overlap = p_clamp; }
bool BPYModifierBevel::get_clamp_overlap() const { return clamp_overlap; }

Ref<BPYMesh> BPYModifierBevel::apply(const Ref<BPYMesh> &p_mesh) {
	if (!enabled || p_mesh.is_null() || width <= 0.0f) {
		return p_mesh;
	}

	// Simple edge bevel implementation
	Ref<BPYMesh> result;
	result.instantiate();

	// Determine which edges to bevel
	HashSet<int> bevel_edges;

	for (int edge_idx = 0; edge_idx < (int)p_mesh->edges.size(); edge_idx++) {
		const BPYEdge &edge = p_mesh->edges[edge_idx];

		bool should_bevel = false;

		switch (limit_method) {
			case LIMIT_NONE:
				should_bevel = true;
				break;

			case LIMIT_ANGLE:
				if (edge.faces.size() == 2) {
					Vector3 n1 = p_mesh->faces[edge.faces[0]].normal;
					Vector3 n2 = p_mesh->faces[edge.faces[1]].normal;
					float angle = n1.angle_to(n2);
					should_bevel = angle >= angle_limit;
				}
				break;

			case LIMIT_WEIGHT:
				should_bevel = edge.crease > 0.0f;
				break;
		}

		if (should_bevel) {
			bevel_edges.insert(edge_idx);
		}
	}

	if (bevel_edges.is_empty()) {
		return p_mesh->duplicate();
	}

	// For simplicity, implement single-segment bevel
	// Full multi-segment bevel would be much more complex

	// Copy vertices
	HashMap<int, int> vert_map;
	for (int i = 0; i < (int)p_mesh->vertices.size(); i++) {
		vert_map[i] = result->add_vertex(p_mesh->vertices[i].position);
		result->vertices[vert_map[i]].normal = p_mesh->vertices[i].normal;
		result->vertices[vert_map[i]].color = p_mesh->vertices[i].color;
	}

	// For each beveled edge, create offset vertices
	HashMap<uint64_t, LocalVector<int>> edge_bevel_verts; // edge_idx -> list of new verts along edge

	for (int edge_idx : bevel_edges) {
		const BPYEdge &edge = p_mesh->edges[edge_idx];
		int v0 = edge.vertices[0];
		int v1 = edge.vertices[1];

		Vector3 p0 = p_mesh->vertices[v0].position;
		Vector3 p1 = p_mesh->vertices[v1].position;
		Vector3 edge_dir = (p1 - p0).normalized();

		// Calculate bevel offset direction (perpendicular to edge, in average face normal direction)
		Vector3 avg_normal;
		for (int face_idx : edge.faces) {
			avg_normal += p_mesh->faces[face_idx].normal;
		}
		avg_normal.normalize();

		Vector3 offset_dir = edge_dir.cross(avg_normal).normalized();

		// Create bevel vertices
		LocalVector<int> bevel_verts;
		for (int seg = 0; seg <= segments; seg++) {
			float t = (float)seg / segments;
			Vector3 pos = p0.lerp(p1, t) + offset_dir * width;
			bevel_verts.push_back(result->add_vertex(pos));
		}

		uint64_t key = ((uint64_t)MIN(v0, v1) << 32) | MAX(v0, v1);
		edge_bevel_verts[key] = bevel_verts;
	}

	// Copy and modify faces
	for (int face_idx = 0; face_idx < (int)p_mesh->faces.size(); face_idx++) {
		const BPYFace &face = p_mesh->faces[face_idx];
		PackedInt32Array orig_verts = p_mesh->get_face_vertices(face_idx);

		// Check if this face has any beveled edges
		bool has_bevel = false;
		for (int loop_idx : face.loops) {
			if (bevel_edges.has(p_mesh->loops[loop_idx].edge)) {
				has_bevel = true;
				break;
			}
		}

		if (!has_bevel) {
			// Copy face as-is
			PackedInt32Array new_verts;
			for (int v : orig_verts) {
				new_verts.push_back(vert_map[v]);
			}
			int new_face = result->add_face(new_verts);
			if (new_face >= 0) {
				result->set_face_smooth(new_face, face.smooth);
				result->set_face_material_index(new_face, face.material_index);
			}
		} else {
			// For now, just copy the face (full bevel implementation would rebuild geometry)
			PackedInt32Array new_verts;
			for (int v : orig_verts) {
				new_verts.push_back(vert_map[v]);
			}
			int new_face = result->add_face(new_verts);
			if (new_face >= 0) {
				result->set_face_smooth(new_face, face.smooth);
				result->set_face_material_index(new_face, face.material_index);
			}
		}
	}

	// Create bevel faces for each beveled edge
	for (int edge_idx : bevel_edges) {
		const BPYEdge &edge = p_mesh->edges[edge_idx];
		int v0 = edge.vertices[0];
		int v1 = edge.vertices[1];

		uint64_t key = ((uint64_t)MIN(v0, v1) << 32) | MAX(v0, v1);
		if (!edge_bevel_verts.has(key)) {
			continue;
		}

		const LocalVector<int> &bevel_verts = edge_bevel_verts[key];

		// Create quad strip along the bevel
		for (int seg = 0; seg < segments; seg++) {
			int new_face = result->add_quad(
					vert_map[v0],
					bevel_verts[seg],
					bevel_verts[seg + 1],
					vert_map[v1]);

			if (new_face >= 0) {
				result->set_face_smooth(new_face, true);
			}
		}
	}

	result->recalculate_normals();
	return result;
}

BPYModifierBevel::BPYModifierBevel() {
}
