/**************************************************************************/
/*  bpy_mesh.h                                                            */
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

#include "bpy_elements.h"

#include "core/io/resource.h"
#include "core/templates/hash_map.h"
#include "scene/resources/mesh.h"

class BPYMesh : public RefCounted {
	GDCLASS(BPYMesh, RefCounted);

public:
	// Mesh elements
	LocalVector<BPYVertex> vertices;
	LocalVector<BPYEdge> edges;
	LocalVector<BPYLoop> loops;
	LocalVector<BPYFace> faces;

private:
	// Edge lookup by vertex pair
	HashMap<uint64_t, int> edge_map;

	static uint64_t _edge_key(int v0, int v1) {
		if (v0 > v1) {
			SWAP(v0, v1);
		}
		return ((uint64_t)v0 << 32) | (uint64_t)v1;
	}

protected:
	static void _bind_methods();

public:
	// Vertex operations
	int add_vertex(const Vector3 &p_position);
	void set_vertex_position(int p_index, const Vector3 &p_position);
	Vector3 get_vertex_position(int p_index) const;
	void set_vertex_normal(int p_index, const Vector3 &p_normal);
	Vector3 get_vertex_normal(int p_index) const;
	void set_vertex_color(int p_index, const Color &p_color);
	Color get_vertex_color(int p_index) const;
	void set_vertex_crease(int p_index, float p_crease);
	float get_vertex_crease(int p_index) const;
	int get_vertex_count() const;
	PackedInt32Array get_vertex_edges(int p_index) const;
	PackedInt32Array get_vertex_faces(int p_index) const;

	// Edge operations
	int add_edge(int p_v0, int p_v1);
	int find_edge(int p_v0, int p_v1) const;
	int get_edge_vertex(int p_edge_index, int p_which) const;
	void set_edge_crease(int p_index, float p_crease);
	float get_edge_crease(int p_index) const;
	void set_edge_sharp(int p_index, bool p_sharp);
	bool get_edge_sharp(int p_index) const;
	void set_edge_seam(int p_index, bool p_seam);
	bool get_edge_seam(int p_index) const;
	int get_edge_count() const;
	PackedInt32Array get_edge_faces(int p_index) const;

	// Face operations
	int add_face(const PackedInt32Array &p_vertex_indices);
	int add_triangle(int p_v0, int p_v1, int p_v2);
	int add_quad(int p_v0, int p_v1, int p_v2, int p_v3);
	PackedInt32Array get_face_vertices(int p_index) const;
	void set_face_smooth(int p_index, bool p_smooth);
	bool get_face_smooth(int p_index) const;
	void set_face_material_index(int p_index, int p_material);
	int get_face_material_index(int p_index) const;
	Vector3 get_face_normal(int p_index) const;
	Vector3 get_face_center(int p_index) const;
	int get_face_count() const;

	// Loop operations
	void set_loop_uv(int p_index, const Vector2 &p_uv);
	Vector2 get_loop_uv(int p_index) const;
	void set_loop_normal(int p_index, const Vector3 &p_normal);
	Vector3 get_loop_normal(int p_index) const;
	void set_loop_color(int p_index, const Color &p_color);
	Color get_loop_color(int p_index) const;
	int get_loop_count() const;
	int get_loop_vertex(int p_index) const;
	int get_loop_face(int p_index) const;

	// Mesh-wide operations
	void clear();
	void recalculate_normals();
	void flip_normals();
	void transform(const Transform3D &p_transform);
	Ref<BPYMesh> duplicate() const;

	// Conversion
	Ref<ArrayMesh> to_array_mesh() const;
	void from_array_mesh(const Ref<ArrayMesh> &p_mesh, int p_surface = 0);

	// Utility
	AABB get_aabb() const;
	void merge_vertices(float p_threshold = 0.0001f);
	void triangulate();

	// Internal helpers
	void _update_topology();
	void _ensure_edge(int p_v0, int p_v1, int p_loop_index);

	BPYMesh();
	~BPYMesh();
};
