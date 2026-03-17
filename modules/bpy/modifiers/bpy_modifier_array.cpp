/**************************************************************************/
/*  bpy_modifier_array.cpp                                                */
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

#include "bpy_modifier_array.h"

#include "core/object/class_db.h"

void BPYModifierArray::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_count", "count"), &BPYModifierArray::set_count);
	ClassDB::bind_method(D_METHOD("get_count"), &BPYModifierArray::get_count);
	ClassDB::bind_method(D_METHOD("set_relative_offset", "offset"), &BPYModifierArray::set_relative_offset);
	ClassDB::bind_method(D_METHOD("get_relative_offset"), &BPYModifierArray::get_relative_offset);
	ClassDB::bind_method(D_METHOD("set_constant_offset", "offset"), &BPYModifierArray::set_constant_offset);
	ClassDB::bind_method(D_METHOD("get_constant_offset"), &BPYModifierArray::get_constant_offset);
	ClassDB::bind_method(D_METHOD("set_use_relative_offset", "enabled"), &BPYModifierArray::set_use_relative_offset);
	ClassDB::bind_method(D_METHOD("get_use_relative_offset"), &BPYModifierArray::get_use_relative_offset);
	ClassDB::bind_method(D_METHOD("set_use_constant_offset", "enabled"), &BPYModifierArray::set_use_constant_offset);
	ClassDB::bind_method(D_METHOD("get_use_constant_offset"), &BPYModifierArray::get_use_constant_offset);
	ClassDB::bind_method(D_METHOD("set_use_merge_vertices", "enabled"), &BPYModifierArray::set_use_merge_vertices);
	ClassDB::bind_method(D_METHOD("get_use_merge_vertices"), &BPYModifierArray::get_use_merge_vertices);
	ClassDB::bind_method(D_METHOD("set_merge_threshold", "threshold"), &BPYModifierArray::set_merge_threshold);
	ClassDB::bind_method(D_METHOD("get_merge_threshold"), &BPYModifierArray::get_merge_threshold);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "count", PROPERTY_HINT_RANGE, "1,1000,1"), "set_count", "get_count");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "relative_offset"), "set_relative_offset", "get_relative_offset");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "constant_offset"), "set_constant_offset", "get_constant_offset");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_relative_offset"), "set_use_relative_offset", "get_use_relative_offset");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_constant_offset"), "set_use_constant_offset", "get_use_constant_offset");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_merge_vertices"), "set_use_merge_vertices", "get_use_merge_vertices");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "merge_threshold", PROPERTY_HINT_RANGE, "0,1,0.0001"), "set_merge_threshold", "get_merge_threshold");
}

void BPYModifierArray::set_count(int p_count) { count = MAX(1, p_count); }
int BPYModifierArray::get_count() const { return count; }

void BPYModifierArray::set_relative_offset(const Vector3 &p_offset) { relative_offset = p_offset; }
Vector3 BPYModifierArray::get_relative_offset() const { return relative_offset; }

void BPYModifierArray::set_constant_offset(const Vector3 &p_offset) { constant_offset = p_offset; }
Vector3 BPYModifierArray::get_constant_offset() const { return constant_offset; }

void BPYModifierArray::set_use_relative_offset(bool p_enabled) { use_relative_offset = p_enabled; }
bool BPYModifierArray::get_use_relative_offset() const { return use_relative_offset; }

void BPYModifierArray::set_use_constant_offset(bool p_enabled) { use_constant_offset = p_enabled; }
bool BPYModifierArray::get_use_constant_offset() const { return use_constant_offset; }

void BPYModifierArray::set_use_merge_vertices(bool p_enabled) { use_merge_vertices = p_enabled; }
bool BPYModifierArray::get_use_merge_vertices() const { return use_merge_vertices; }

void BPYModifierArray::set_merge_threshold(float p_threshold) { merge_threshold = MAX(0.0f, p_threshold); }
float BPYModifierArray::get_merge_threshold() const { return merge_threshold; }

Ref<BPYMesh> BPYModifierArray::apply(const Ref<BPYMesh> &p_mesh) {
	if (!enabled || p_mesh.is_null() || count <= 1) {
		return p_mesh;
	}

	Ref<BPYMesh> result;
	result.instantiate();

	// Calculate bounding box for relative offset
	AABB aabb = p_mesh->get_aabb();
	Vector3 size = aabb.size;

	// Calculate total offset per copy
	Vector3 offset;
	if (use_relative_offset) {
		offset += size * relative_offset;
	}
	if (use_constant_offset) {
		offset += constant_offset;
	}

	// Create copies
	for (int copy = 0; copy < count; copy++) {
		Vector3 translation = offset * (float)copy;
		int vert_offset = result->vertices.size();

		// Copy vertices with translation
		for (int i = 0; i < (int)p_mesh->vertices.size(); i++) {
			Vector3 pos = p_mesh->vertices[i].position + translation;
			int new_vert = result->add_vertex(pos);
			result->vertices[new_vert].normal = p_mesh->vertices[i].normal;
			result->vertices[new_vert].color = p_mesh->vertices[i].color;
			result->vertices[new_vert].crease = p_mesh->vertices[i].crease;
		}

		// Copy faces
		for (int face_idx = 0; face_idx < (int)p_mesh->faces.size(); face_idx++) {
			PackedInt32Array orig_verts = p_mesh->get_face_vertices(face_idx);
			PackedInt32Array new_verts;

			for (int i = 0; i < orig_verts.size(); i++) {
				new_verts.push_back(orig_verts[i] + vert_offset);
			}

			int new_face = result->add_face(new_verts);
			if (new_face >= 0) {
				result->set_face_smooth(new_face, p_mesh->get_face_smooth(face_idx));
				result->set_face_material_index(new_face, p_mesh->get_face_material_index(face_idx));

				// Copy loop data
				const BPYFace &orig_face = p_mesh->faces[face_idx];
				BPYFace &new_face_ref = result->faces[new_face];

				for (int i = 0; i < (int)new_face_ref.loops.size(); i++) {
					int orig_loop_idx = orig_face.loops[i];
					int new_loop_idx = new_face_ref.loops[i];

					result->set_loop_uv(new_loop_idx, p_mesh->get_loop_uv(orig_loop_idx));
					result->set_loop_normal(new_loop_idx, p_mesh->get_loop_normal(orig_loop_idx));
					result->set_loop_color(new_loop_idx, p_mesh->get_loop_color(orig_loop_idx));
				}
			}
		}
	}

	if (use_merge_vertices && merge_threshold > 0) {
		result->merge_vertices(merge_threshold);
	}

	return result;
}

BPYModifierArray::BPYModifierArray() {
}
