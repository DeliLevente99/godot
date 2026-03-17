/**************************************************************************/
/*  bpy_modifier_decimate.cpp                                             */
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

#include "bpy_modifier_decimate.h"

#include "core/object/class_db.h"
#include "scene/resources/surface_tool.h"

// Using meshoptimizer via SurfaceTool for decimation

void BPYModifierDecimate::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_mode", "mode"), &BPYModifierDecimate::set_mode);
	ClassDB::bind_method(D_METHOD("get_mode"), &BPYModifierDecimate::get_mode);
	ClassDB::bind_method(D_METHOD("set_ratio", "ratio"), &BPYModifierDecimate::set_ratio);
	ClassDB::bind_method(D_METHOD("get_ratio"), &BPYModifierDecimate::get_ratio);
	ClassDB::bind_method(D_METHOD("set_angle_limit", "angle"), &BPYModifierDecimate::set_angle_limit);
	ClassDB::bind_method(D_METHOD("get_angle_limit"), &BPYModifierDecimate::get_angle_limit);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "mode", PROPERTY_HINT_ENUM, "Collapse,Planar"), "set_mode", "get_mode");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "ratio", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_ratio", "get_ratio");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "angle_limit", PROPERTY_HINT_RANGE, "0,180,0.1,radians_as_degrees"), "set_angle_limit", "get_angle_limit");

	BIND_ENUM_CONSTANT(MODE_COLLAPSE);
	BIND_ENUM_CONSTANT(MODE_PLANAR);
}

void BPYModifierDecimate::set_mode(DecimateMode p_mode) { mode = p_mode; }
BPYModifierDecimate::DecimateMode BPYModifierDecimate::get_mode() const { return mode; }

void BPYModifierDecimate::set_ratio(float p_ratio) { ratio = CLAMP(p_ratio, 0.0f, 1.0f); }
float BPYModifierDecimate::get_ratio() const { return ratio; }

void BPYModifierDecimate::set_angle_limit(float p_angle) { angle_limit = CLAMP(p_angle, 0.0f, Math::PI); }
float BPYModifierDecimate::get_angle_limit() const { return angle_limit; }

Ref<BPYMesh> BPYModifierDecimate::apply(const Ref<BPYMesh> &p_mesh) {
	if (!enabled || p_mesh.is_null()) {
		return p_mesh;
	}

	if (ratio >= 1.0f) {
		return p_mesh->duplicate();
	}

	// Convert to ArrayMesh, use SurfaceTool for decimation, convert back
	Ref<ArrayMesh> array_mesh = p_mesh->to_array_mesh();
	if (array_mesh.is_null() || array_mesh->get_surface_count() == 0) {
		return p_mesh->duplicate();
	}

	// Use SurfaceTool's generate_lod for decimation
	Ref<SurfaceTool> st;
	st.instantiate();
	st->create_from(array_mesh, 0);

	// Calculate target index count
	Array arrays = array_mesh->surface_get_arrays(0);
	PackedInt32Array indices = arrays[Mesh::ARRAY_INDEX];
	int orig_count = indices.size();
	if (orig_count == 0) {
		PackedVector3Array verts = arrays[Mesh::ARRAY_VERTEX];
		orig_count = verts.size();
	}

	int target_count = MAX(3, (int)(orig_count * ratio));

	// Generate LOD
	float threshold = 1.0f - ratio;
	Vector<int> lod_indices = st->generate_lod(threshold, target_count);

	if (lod_indices.is_empty()) {
		return p_mesh->duplicate();
	}

	// Create new mesh from LOD indices
	Ref<BPYMesh> result;
	result.instantiate();

	PackedVector3Array verts = arrays[Mesh::ARRAY_VERTEX];
	PackedVector3Array normals = arrays[Mesh::ARRAY_NORMAL];
	PackedVector2Array uvs = arrays[Mesh::ARRAY_TEX_UV];

	bool has_normals = normals.size() == verts.size();
	bool has_uvs = uvs.size() == verts.size();

	// Create vertices with deduplication
	HashMap<Vector3, int> vert_map;
	LocalVector<int> index_remap;
	index_remap.resize(verts.size());

	for (int i = 0; i < verts.size(); i++) {
		if (vert_map.has(verts[i])) {
			index_remap[i] = vert_map[verts[i]];
		} else {
			int new_idx = result->add_vertex(verts[i]);
			vert_map[verts[i]] = new_idx;
			index_remap[i] = new_idx;

			if (has_normals) {
				result->vertices[new_idx].normal = normals[i];
			}
		}
	}

	// Create faces from LOD indices
	for (int i = 0; i < lod_indices.size(); i += 3) {
		int v0 = index_remap[lod_indices[i]];
		int v1 = index_remap[lod_indices[i + 1]];
		int v2 = index_remap[lod_indices[i + 2]];

		// Skip degenerate triangles
		if (v0 == v1 || v1 == v2 || v2 == v0) {
			continue;
		}

		int face_idx = result->add_triangle(v0, v1, v2);
		if (face_idx >= 0 && has_uvs) {
			int base_loop = result->faces[face_idx].loops[0];
			result->set_loop_uv(base_loop, uvs[lod_indices[i]]);
			result->set_loop_uv(base_loop + 1, uvs[lod_indices[i + 1]]);
			result->set_loop_uv(base_loop + 2, uvs[lod_indices[i + 2]]);
		}
	}

	result->recalculate_normals();
	return result;
}

BPYModifierDecimate::BPYModifierDecimate() {
}
