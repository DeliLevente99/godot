/**************************************************************************/
/*  bpy_mesh_ops.h                                                        */
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

#pragma once

#include "../core/bpy_mesh.h"

class BPYMeshOps : public RefCounted {
	GDCLASS(BPYMeshOps, RefCounted);

public:
	enum MergeType {
		MERGE_CENTER,
		MERGE_FIRST,
		MERGE_LAST,
		MERGE_COLLAPSE
	};

protected:
	static void _bind_methods();

public:
	// Face operations
	static void extrude_faces(Ref<BPYMesh> p_mesh, const PackedInt32Array &p_faces, const Vector3 &p_direction, float p_distance = 1.0f);
	static void inset_faces(Ref<BPYMesh> p_mesh, const PackedInt32Array &p_faces, float p_thickness, float p_depth = 0.0f);
	static void dissolve_faces(Ref<BPYMesh> p_mesh, const PackedInt32Array &p_faces);
	static void poke_faces(Ref<BPYMesh> p_mesh, const PackedInt32Array &p_faces);

	// Edge operations
	static void bevel_edges(Ref<BPYMesh> p_mesh, const PackedInt32Array &p_edges, float p_width, int p_segments = 1);
	static PackedInt32Array loop_cut(Ref<BPYMesh> p_mesh, int p_edge, int p_cuts);
	static void dissolve_edges(Ref<BPYMesh> p_mesh, const PackedInt32Array &p_edges);
	static void subdivide_edges(Ref<BPYMesh> p_mesh, const PackedInt32Array &p_edges, int p_cuts);

	// Vertex operations
	static void merge_vertices(Ref<BPYMesh> p_mesh, const PackedInt32Array &p_vertices, MergeType p_type = MERGE_CENTER);
	static void dissolve_vertices(Ref<BPYMesh> p_mesh, const PackedInt32Array &p_vertices);
	static void smooth_vertices(Ref<BPYMesh> p_mesh, const PackedInt32Array &p_vertices, float p_factor = 0.5f, int p_iterations = 1);

	// Boolean operations (static wrappers)
	static Ref<BPYMesh> boolean_union(const Ref<BPYMesh> &p_a, const Ref<BPYMesh> &p_b);
	static Ref<BPYMesh> boolean_difference(const Ref<BPYMesh> &p_a, const Ref<BPYMesh> &p_b);
	static Ref<BPYMesh> boolean_intersection(const Ref<BPYMesh> &p_a, const Ref<BPYMesh> &p_b);

	// Mesh joining
	static Ref<BPYMesh> merge_meshes(const Ref<BPYMesh> &p_a, const Ref<BPYMesh> &p_b);
	static Ref<BPYMesh> join_meshes(const TypedArray<BPYMesh> &p_meshes);
	static void separate_by_material(const Ref<BPYMesh> &p_mesh, TypedArray<BPYMesh> &r_meshes);

	// Selection helpers
	static PackedInt32Array select_all_faces(const Ref<BPYMesh> &p_mesh);
	static PackedInt32Array select_all_edges(const Ref<BPYMesh> &p_mesh);
	static PackedInt32Array select_all_vertices(const Ref<BPYMesh> &p_mesh);
	static PackedInt32Array select_linked_faces(const Ref<BPYMesh> &p_mesh, int p_face);

	BPYMeshOps();
};

VARIANT_ENUM_CAST(BPYMeshOps::MergeType);
