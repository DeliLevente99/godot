/**************************************************************************/
/*  bpy_modifier_solidify.cpp                                             */
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

#include "bpy_modifier_solidify.h"

#include "core/object/class_db.h"

void BPYModifierSolidify::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_thickness", "thickness"), &BPYModifierSolidify::set_thickness);
	ClassDB::bind_method(D_METHOD("get_thickness"), &BPYModifierSolidify::get_thickness);
	ClassDB::bind_method(D_METHOD("set_offset", "offset"), &BPYModifierSolidify::set_offset);
	ClassDB::bind_method(D_METHOD("get_offset"), &BPYModifierSolidify::get_offset);
	ClassDB::bind_method(D_METHOD("set_use_even_offset", "enabled"), &BPYModifierSolidify::set_use_even_offset);
	ClassDB::bind_method(D_METHOD("get_use_even_offset"), &BPYModifierSolidify::get_use_even_offset);
	ClassDB::bind_method(D_METHOD("set_use_rim", "enabled"), &BPYModifierSolidify::set_use_rim);
	ClassDB::bind_method(D_METHOD("get_use_rim"), &BPYModifierSolidify::get_use_rim);
	ClassDB::bind_method(D_METHOD("set_rim_material_index", "index"), &BPYModifierSolidify::set_rim_material_index);
	ClassDB::bind_method(D_METHOD("get_rim_material_index"), &BPYModifierSolidify::get_rim_material_index);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "thickness", PROPERTY_HINT_RANGE, "-10,10,0.001"), "set_thickness", "get_thickness");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "offset", PROPERTY_HINT_RANGE, "-1,1,0.01"), "set_offset", "get_offset");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_even_offset"), "set_use_even_offset", "get_use_even_offset");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_rim"), "set_use_rim", "get_use_rim");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "rim_material_index", PROPERTY_HINT_RANGE, "0,100,1"), "set_rim_material_index", "get_rim_material_index");
}

void BPYModifierSolidify::set_thickness(float p_thickness) { thickness = p_thickness; }
float BPYModifierSolidify::get_thickness() const { return thickness; }

void BPYModifierSolidify::set_offset(float p_offset) { offset = CLAMP(p_offset, -1.0f, 1.0f); }
float BPYModifierSolidify::get_offset() const { return offset; }

void BPYModifierSolidify::set_use_even_offset(bool p_enabled) { use_even_offset = p_enabled; }
bool BPYModifierSolidify::get_use_even_offset() const { return use_even_offset; }

void BPYModifierSolidify::set_use_rim(bool p_enabled) { use_rim = p_enabled; }
bool BPYModifierSolidify::get_use_rim() const { return use_rim; }

void BPYModifierSolidify::set_rim_material_index(int p_index) { rim_material_index = MAX(0, p_index); }
int BPYModifierSolidify::get_rim_material_index() const { return rim_material_index; }

Ref<BPYMesh> BPYModifierSolidify::apply(const Ref<BPYMesh> &p_mesh) {
	if (!enabled || p_mesh.is_null() || Math::is_zero_approx(thickness)) {
		return p_mesh;
	}

	Ref<BPYMesh> result;
	result.instantiate();

	// Calculate offset distances
	float outer_offset = thickness * (1.0f + offset) * 0.5f;
	float inner_offset = thickness * (1.0f - offset) * 0.5f;

	int orig_vert_count = p_mesh->vertices.size();
	int orig_face_count = p_mesh->faces.size();

	// Calculate vertex normals for offset direction
	LocalVector<Vector3> vert_normals;
	vert_normals.resize(orig_vert_count);

	for (int i = 0; i < orig_vert_count; i++) {
		vert_normals[i] = Vector3();
	}

	for (int face_idx = 0; face_idx < orig_face_count; face_idx++) {
		Vector3 face_normal = p_mesh->faces[face_idx].normal;
		for (int loop_idx : p_mesh->faces[face_idx].loops) {
			int v_idx = p_mesh->loops[loop_idx].vertex;
			vert_normals[v_idx] += face_normal;
		}
	}

	for (int i = 0; i < orig_vert_count; i++) {
		if (vert_normals[i].length_squared() > 0.0001f) {
			vert_normals[i].normalize();
		}
	}

	// Create outer vertices (original + outer offset)
	LocalVector<int> outer_vert_map;
	outer_vert_map.resize(orig_vert_count);

	for (int i = 0; i < orig_vert_count; i++) {
		Vector3 pos = p_mesh->vertices[i].position + vert_normals[i] * outer_offset;
		outer_vert_map[i] = result->add_vertex(pos);
		result->vertices[outer_vert_map[i]].normal = p_mesh->vertices[i].normal;
		result->vertices[outer_vert_map[i]].color = p_mesh->vertices[i].color;
	}

	// Create inner vertices (original - inner offset)
	LocalVector<int> inner_vert_map;
	inner_vert_map.resize(orig_vert_count);

	for (int i = 0; i < orig_vert_count; i++) {
		Vector3 pos = p_mesh->vertices[i].position - vert_normals[i] * inner_offset;
		inner_vert_map[i] = result->add_vertex(pos);
		result->vertices[inner_vert_map[i]].normal = -p_mesh->vertices[i].normal;
		result->vertices[inner_vert_map[i]].color = p_mesh->vertices[i].color;
	}

	// Create outer faces (same winding)
	for (int face_idx = 0; face_idx < orig_face_count; face_idx++) {
		PackedInt32Array orig_verts = p_mesh->get_face_vertices(face_idx);
		PackedInt32Array new_verts;

		for (int i = 0; i < orig_verts.size(); i++) {
			new_verts.push_back(outer_vert_map[orig_verts[i]]);
		}

		int new_face = result->add_face(new_verts);
		if (new_face >= 0) {
			result->set_face_smooth(new_face, p_mesh->get_face_smooth(face_idx));
			result->set_face_material_index(new_face, p_mesh->get_face_material_index(face_idx));

			// Copy UVs
			const BPYFace &orig_face = p_mesh->faces[face_idx];
			for (int i = 0; i < (int)result->faces[new_face].loops.size(); i++) {
				int orig_loop = orig_face.loops[i];
				int new_loop = result->faces[new_face].loops[i];
				result->set_loop_uv(new_loop, p_mesh->get_loop_uv(orig_loop));
			}
		}
	}

	// Create inner faces (reversed winding)
	for (int face_idx = 0; face_idx < orig_face_count; face_idx++) {
		PackedInt32Array orig_verts = p_mesh->get_face_vertices(face_idx);
		PackedInt32Array new_verts;

		// Reverse winding
		for (int i = orig_verts.size() - 1; i >= 0; i--) {
			new_verts.push_back(inner_vert_map[orig_verts[i]]);
		}

		int new_face = result->add_face(new_verts);
		if (new_face >= 0) {
			result->set_face_smooth(new_face, p_mesh->get_face_smooth(face_idx));
			result->set_face_material_index(new_face, p_mesh->get_face_material_index(face_idx));

			// Copy UVs (reversed)
			const BPYFace &orig_face = p_mesh->faces[face_idx];
			for (int i = 0; i < (int)result->faces[new_face].loops.size(); i++) {
				int orig_loop = orig_face.loops[orig_face.loops.size() - 1 - i];
				int new_loop = result->faces[new_face].loops[i];
				result->set_loop_uv(new_loop, p_mesh->get_loop_uv(orig_loop));
			}
		}
	}

	// Create rim faces for boundary edges
	if (use_rim) {
		// Find boundary edges (edges with only one face)
		for (int edge_idx = 0; edge_idx < (int)p_mesh->edges.size(); edge_idx++) {
			if (p_mesh->edges[edge_idx].faces.size() != 1) {
				continue;
			}

			int v0 = p_mesh->edges[edge_idx].vertices[0];
			int v1 = p_mesh->edges[edge_idx].vertices[1];

			// Create quad connecting outer and inner shells
			// Winding depends on which side of the face the edge is on
			int rim_face = result->add_quad(
					outer_vert_map[v0],
					outer_vert_map[v1],
					inner_vert_map[v1],
					inner_vert_map[v0]);

			if (rim_face >= 0) {
				result->set_face_smooth(rim_face, false);
				result->set_face_material_index(rim_face, rim_material_index);
			}
		}
	}

	result->recalculate_normals();
	return result;
}

BPYModifierSolidify::BPYModifierSolidify() {
}
