/**************************************************************************/
/*  bpy_modifier_mirror.cpp                                               */
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

#include "bpy_modifier_mirror.h"

#include "core/object/class_db.h"

void BPYModifierMirror::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_axis_x", "enabled"), &BPYModifierMirror::set_axis_x);
	ClassDB::bind_method(D_METHOD("get_axis_x"), &BPYModifierMirror::get_axis_x);
	ClassDB::bind_method(D_METHOD("set_axis_y", "enabled"), &BPYModifierMirror::set_axis_y);
	ClassDB::bind_method(D_METHOD("get_axis_y"), &BPYModifierMirror::get_axis_y);
	ClassDB::bind_method(D_METHOD("set_axis_z", "enabled"), &BPYModifierMirror::set_axis_z);
	ClassDB::bind_method(D_METHOD("get_axis_z"), &BPYModifierMirror::get_axis_z);
	ClassDB::bind_method(D_METHOD("set_merge_threshold", "threshold"), &BPYModifierMirror::set_merge_threshold);
	ClassDB::bind_method(D_METHOD("get_merge_threshold"), &BPYModifierMirror::get_merge_threshold);
	ClassDB::bind_method(D_METHOD("set_use_mirror_merge", "enabled"), &BPYModifierMirror::set_use_mirror_merge);
	ClassDB::bind_method(D_METHOD("get_use_mirror_merge"), &BPYModifierMirror::get_use_mirror_merge);
	ClassDB::bind_method(D_METHOD("set_flip_uvs_u", "flip"), &BPYModifierMirror::set_flip_uvs_u);
	ClassDB::bind_method(D_METHOD("get_flip_uvs_u"), &BPYModifierMirror::get_flip_uvs_u);
	ClassDB::bind_method(D_METHOD("set_flip_uvs_v", "flip"), &BPYModifierMirror::set_flip_uvs_v);
	ClassDB::bind_method(D_METHOD("get_flip_uvs_v"), &BPYModifierMirror::get_flip_uvs_v);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "axis_x"), "set_axis_x", "get_axis_x");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "axis_y"), "set_axis_y", "get_axis_y");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "axis_z"), "set_axis_z", "get_axis_z");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "merge_threshold", PROPERTY_HINT_RANGE, "0,1,0.0001"), "set_merge_threshold", "get_merge_threshold");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_mirror_merge"), "set_use_mirror_merge", "get_use_mirror_merge");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "flip_uvs_u"), "set_flip_uvs_u", "get_flip_uvs_u");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "flip_uvs_v"), "set_flip_uvs_v", "get_flip_uvs_v");
}

void BPYModifierMirror::set_axis_x(bool p_enabled) { axis_x = p_enabled; }
bool BPYModifierMirror::get_axis_x() const { return axis_x; }
void BPYModifierMirror::set_axis_y(bool p_enabled) { axis_y = p_enabled; }
bool BPYModifierMirror::get_axis_y() const { return axis_y; }
void BPYModifierMirror::set_axis_z(bool p_enabled) { axis_z = p_enabled; }
bool BPYModifierMirror::get_axis_z() const { return axis_z; }

void BPYModifierMirror::set_merge_threshold(float p_threshold) { merge_threshold = MAX(0.0f, p_threshold); }
float BPYModifierMirror::get_merge_threshold() const { return merge_threshold; }
void BPYModifierMirror::set_use_mirror_merge(bool p_enabled) { use_mirror_merge = p_enabled; }
bool BPYModifierMirror::get_use_mirror_merge() const { return use_mirror_merge; }
void BPYModifierMirror::set_flip_uvs_u(bool p_flip) { flip_uvs_u = p_flip; }
bool BPYModifierMirror::get_flip_uvs_u() const { return flip_uvs_u; }
void BPYModifierMirror::set_flip_uvs_v(bool p_flip) { flip_uvs_v = p_flip; }
bool BPYModifierMirror::get_flip_uvs_v() const { return flip_uvs_v; }

Ref<BPYMesh> BPYModifierMirror::apply(const Ref<BPYMesh> &p_mesh) {
	if (!enabled || p_mesh.is_null()) {
		return p_mesh;
	}

	if (!axis_x && !axis_y && !axis_z) {
		return p_mesh->duplicate();
	}

	Ref<BPYMesh> result = p_mesh->duplicate();

	// Apply mirror for each enabled axis
	auto mirror_axis = [&](int axis) {
		int orig_vert_count = result->vertices.size();
		int orig_face_count = result->faces.size();

		// Map from original vertex to mirrored vertex
		LocalVector<int> vert_map;
		vert_map.resize(orig_vert_count);

		// Create mirrored vertices
		for (int i = 0; i < orig_vert_count; i++) {
			Vector3 pos = result->vertices[i].position;

			// Check if vertex is on the mirror plane
			bool on_plane = Math::abs(pos[axis]) < merge_threshold && use_mirror_merge;

			if (on_plane) {
				// Vertex is on plane, reuse it
				pos[axis] = 0; // Snap to plane
				result->vertices[i].position = pos;
				vert_map[i] = i;
			} else {
				// Create mirrored vertex
				pos[axis] = -pos[axis];
				vert_map[i] = result->add_vertex(pos);

				// Copy vertex data
				result->vertices[vert_map[i]].normal = result->vertices[i].normal;
				result->vertices[vert_map[i]].normal[axis] = -result->vertices[vert_map[i]].normal[axis];
				result->vertices[vert_map[i]].color = result->vertices[i].color;
				result->vertices[vert_map[i]].crease = result->vertices[i].crease;
			}
		}

		// Create mirrored faces (with reversed winding)
		for (int face_idx = 0; face_idx < orig_face_count; face_idx++) {
			PackedInt32Array orig_verts = result->get_face_vertices(face_idx);
			PackedInt32Array mirrored_verts;

			// Reverse winding order
			for (int i = orig_verts.size() - 1; i >= 0; i--) {
				mirrored_verts.push_back(vert_map[orig_verts[i]]);
			}

			int new_face = result->add_face(mirrored_verts);
			if (new_face >= 0) {
				result->set_face_smooth(new_face, result->get_face_smooth(face_idx));
				result->set_face_material_index(new_face, result->get_face_material_index(face_idx));

				// Copy and optionally flip UVs
				const BPYFace &orig_face = result->faces[face_idx];
				BPYFace &new_face_ref = result->faces[new_face];

				for (int i = 0; i < (int)new_face_ref.loops.size(); i++) {
					int orig_loop_idx = orig_face.loops[orig_face.loops.size() - 1 - i];
					int new_loop_idx = new_face_ref.loops[i];

					Vector2 uv = result->get_loop_uv(orig_loop_idx);
					if (flip_uvs_u) {
						uv.x = 1.0f - uv.x;
					}
					if (flip_uvs_v) {
						uv.y = 1.0f - uv.y;
					}
					result->set_loop_uv(new_loop_idx, uv);

					Vector3 normal = result->get_loop_normal(orig_loop_idx);
					normal[axis] = -normal[axis];
					result->set_loop_normal(new_loop_idx, normal);
				}
			}
		}
	};

	if (axis_x) {
		mirror_axis(0);
	}
	if (axis_y) {
		mirror_axis(1);
	}
	if (axis_z) {
		mirror_axis(2);
	}

	return result;
}

BPYModifierMirror::BPYModifierMirror() {
}
