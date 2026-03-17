/**************************************************************************/
/*  bpy_elements.h                                                        */
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

#include "core/math/color.h"
#include "core/math/vector2.h"
#include "core/math/vector3.h"
#include "core/templates/local_vector.h"

// BMesh-style mesh element structures for procedural mesh generation

struct BPYVertex {
	Vector3 position;
	Vector3 normal;
	Color color = Color(1, 1, 1, 1);

	// Topology connections
	LocalVector<int> edges; // Indices of connected edges
	LocalVector<int> loops; // Indices of loops using this vertex

	// Additional data
	float crease = 0.0f; // For subdivision creasing
	bool selected = false;
	int index = -1; // Index in the vertex array

	BPYVertex() = default;
	BPYVertex(const Vector3 &p_pos) :
			position(p_pos) {}
};

struct BPYEdge {
	int vertices[2] = { -1, -1 }; // Vertex indices

	// Topology connections
	LocalVector<int> loops; // Indices of loops using this edge
	LocalVector<int> faces; // Indices of faces using this edge

	// Additional data
	float crease = 0.0f; // For subdivision creasing
	bool seam = false; // UV seam
	bool sharp = false; // Sharp edge for smooth shading
	bool selected = false;
	int index = -1;

	BPYEdge() = default;
	BPYEdge(int v0, int v1) {
		vertices[0] = v0;
		vertices[1] = v1;
	}

	int other_vertex(int v) const {
		return (vertices[0] == v) ? vertices[1] : vertices[0];
	}

	bool has_vertex(int v) const {
		return vertices[0] == v || vertices[1] == v;
	}
};

struct BPYLoop {
	int vertex = -1; // Vertex index
	int edge = -1; // Edge index (edge going to next vertex in face)
	int face = -1; // Face index

	// Loop chain (within face)
	int next = -1; // Next loop in face
	int prev = -1; // Previous loop in face

	// Per-loop data (for split normals, UVs, etc.)
	Vector3 normal;
	Vector2 uv;
	Color color = Color(1, 1, 1, 1);

	int index = -1;

	BPYLoop() = default;
	BPYLoop(int p_vertex, int p_edge, int p_face) :
			vertex(p_vertex), edge(p_edge), face(p_face) {}
};

struct BPYFace {
	LocalVector<int> loops; // Loop indices (defines the face polygon)

	Vector3 normal;
	int material_index = 0;
	bool smooth = true;
	bool selected = false;
	int index = -1;

	BPYFace() = default;

	int vertex_count() const { return loops.size(); }

	void calc_normal(const LocalVector<BPYVertex> &vertices, const LocalVector<BPYLoop> &all_loops) {
		if (loops.size() < 3) {
			normal = Vector3(0, 1, 0);
			return;
		}

		// Newell's method for polygon normal
		normal = Vector3();
		int n = loops.size();
		for (int i = 0; i < n; i++) {
			const Vector3 &v_curr = vertices[all_loops[loops[i]].vertex].position;
			const Vector3 &v_next = vertices[all_loops[loops[(i + 1) % n]].vertex].position;
			normal.x += (v_curr.y - v_next.y) * (v_curr.z + v_next.z);
			normal.y += (v_curr.z - v_next.z) * (v_curr.x + v_next.x);
			normal.z += (v_curr.x - v_next.x) * (v_curr.y + v_next.y);
		}
		normal.normalize();
	}
};
