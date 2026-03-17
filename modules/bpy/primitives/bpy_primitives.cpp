/**************************************************************************/
/*  bpy_primitives.cpp                                                    */
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

#include "bpy_primitives.h"

#include "core/math/math_funcs.h"
#include "core/object/class_db.h"

void BPY::_bind_methods() {
	ClassDB::bind_method(D_METHOD("create_cube", "size"), &BPY::create_cube, DEFVAL(Vector3(1, 1, 1)));
	ClassDB::bind_method(D_METHOD("create_uv_sphere", "radius", "segments", "rings"), &BPY::create_uv_sphere, DEFVAL(1.0f), DEFVAL(32), DEFVAL(16));
	ClassDB::bind_method(D_METHOD("create_ico_sphere", "radius", "subdivisions"), &BPY::create_ico_sphere, DEFVAL(1.0f), DEFVAL(2));
	ClassDB::bind_method(D_METHOD("create_cylinder", "radius", "height", "segments", "cap_top", "cap_bottom"), &BPY::create_cylinder, DEFVAL(1.0f), DEFVAL(2.0f), DEFVAL(32), DEFVAL(true), DEFVAL(true));
	ClassDB::bind_method(D_METHOD("create_cone", "radius", "height", "segments", "cap"), &BPY::create_cone, DEFVAL(1.0f), DEFVAL(2.0f), DEFVAL(32), DEFVAL(true));
	ClassDB::bind_method(D_METHOD("create_torus", "major_radius", "minor_radius", "major_segments", "minor_segments"), &BPY::create_torus, DEFVAL(1.0f), DEFVAL(0.25f), DEFVAL(48), DEFVAL(12));
	ClassDB::bind_method(D_METHOD("create_plane", "size", "subdivisions"), &BPY::create_plane, DEFVAL(Vector2(2, 2)), DEFVAL(Vector2i(1, 1)));
	ClassDB::bind_method(D_METHOD("create_circle", "radius", "segments", "fill"), &BPY::create_circle, DEFVAL(1.0f), DEFVAL(32), DEFVAL(true));
	ClassDB::bind_method(D_METHOD("create_grid", "size", "subdivisions"), &BPY::create_grid, DEFVAL(Vector2(2, 2)), DEFVAL(Vector2i(10, 10)));
}

Ref<BPYMesh> BPY::create_cube(const Vector3 &p_size) {
	Ref<BPYMesh> mesh;
	mesh.instantiate();

	Vector3 half = p_size * 0.5f;

	// Create 8 vertices
	int v0 = mesh->add_vertex(Vector3(-half.x, -half.y, -half.z)); // 0: back-bottom-left
	int v1 = mesh->add_vertex(Vector3(half.x, -half.y, -half.z)); // 1: back-bottom-right
	int v2 = mesh->add_vertex(Vector3(half.x, half.y, -half.z)); // 2: back-top-right
	int v3 = mesh->add_vertex(Vector3(-half.x, half.y, -half.z)); // 3: back-top-left
	int v4 = mesh->add_vertex(Vector3(-half.x, -half.y, half.z)); // 4: front-bottom-left
	int v5 = mesh->add_vertex(Vector3(half.x, -half.y, half.z)); // 5: front-bottom-right
	int v6 = mesh->add_vertex(Vector3(half.x, half.y, half.z)); // 6: front-top-right
	int v7 = mesh->add_vertex(Vector3(-half.x, half.y, half.z)); // 7: front-top-left

	// Create 6 quad faces (counter-clockwise winding when viewed from outside)
	// Front face (+Z)
	int f0 = mesh->add_quad(v4, v5, v6, v7);
	// Back face (-Z)
	int f1 = mesh->add_quad(v1, v0, v3, v2);
	// Right face (+X)
	int f2 = mesh->add_quad(v5, v1, v2, v6);
	// Left face (-X)
	int f3 = mesh->add_quad(v0, v4, v7, v3);
	// Top face (+Y)
	int f4 = mesh->add_quad(v7, v6, v2, v3);
	// Bottom face (-Y)
	int f5 = mesh->add_quad(v0, v1, v5, v4);

	// Set UVs for each face
	auto set_quad_uvs = [&](int face_idx) {
		if (face_idx < 0) {
			return;
		}
		PackedInt32Array face_loops;
		for (int loop_idx : mesh->faces[face_idx].loops) {
			face_loops.push_back(loop_idx);
		}
		if (face_loops.size() >= 4) {
			mesh->set_loop_uv(face_loops[0], Vector2(0, 0));
			mesh->set_loop_uv(face_loops[1], Vector2(1, 0));
			mesh->set_loop_uv(face_loops[2], Vector2(1, 1));
			mesh->set_loop_uv(face_loops[3], Vector2(0, 1));
		}
	};

	set_quad_uvs(f0);
	set_quad_uvs(f1);
	set_quad_uvs(f2);
	set_quad_uvs(f3);
	set_quad_uvs(f4);
	set_quad_uvs(f5);

	// Set all faces to flat shading for cube
	for (int i = 0; i < mesh->get_face_count(); i++) {
		mesh->set_face_smooth(i, false);
	}

	mesh->recalculate_normals();
	return mesh;
}

Ref<BPYMesh> BPY::create_uv_sphere(float p_radius, int p_segments, int p_rings) {
	Ref<BPYMesh> mesh;
	mesh.instantiate();

	p_segments = MAX(3, p_segments);
	p_rings = MAX(2, p_rings);

	// Create vertices
	// Top pole
	int top_pole = mesh->add_vertex(Vector3(0, p_radius, 0));

	// Ring vertices
	LocalVector<LocalVector<int>> ring_verts;
	ring_verts.resize(p_rings - 1);

	for (int ring = 0; ring < p_rings - 1; ring++) {
		float phi = Math::PI * (ring + 1) / p_rings;
		float y = p_radius * Math::cos(phi);
		float ring_radius = p_radius * Math::sin(phi);

		ring_verts[ring].resize(p_segments);
		for (int seg = 0; seg < p_segments; seg++) {
			float theta = 2.0f * Math::PI * seg / p_segments;
			float x = ring_radius * Math::cos(theta);
			float z = ring_radius * Math::sin(theta);
			ring_verts[ring][seg] = mesh->add_vertex(Vector3(x, y, z));
		}
	}

	// Bottom pole
	int bottom_pole = mesh->add_vertex(Vector3(0, -p_radius, 0));

	// Create faces
	// Top cap (triangles)
	for (int seg = 0; seg < p_segments; seg++) {
		int next_seg = (seg + 1) % p_segments;
		int face_idx = mesh->add_triangle(top_pole, ring_verts[0][next_seg], ring_verts[0][seg]);

		// Set UVs
		if (face_idx >= 0) {
			float u0 = (seg + 0.5f) / p_segments;
			float u1 = (float)(next_seg) / p_segments;
			float u2 = (float)seg / p_segments;
			if (next_seg == 0) {
				u1 = 1.0f;
			}

			int base_loop = mesh->faces[face_idx].loops[0];
			mesh->set_loop_uv(base_loop, Vector2(u0, 0));
			mesh->set_loop_uv(base_loop + 1, Vector2(u1, 1.0f / p_rings));
			mesh->set_loop_uv(base_loop + 2, Vector2(u2, 1.0f / p_rings));
		}
	}

	// Middle rings (quads)
	for (int ring = 0; ring < p_rings - 2; ring++) {
		for (int seg = 0; seg < p_segments; seg++) {
			int next_seg = (seg + 1) % p_segments;

			int face_idx = mesh->add_quad(
					ring_verts[ring][seg],
					ring_verts[ring][next_seg],
					ring_verts[ring + 1][next_seg],
					ring_verts[ring + 1][seg]);

			if (face_idx >= 0) {
				float u0 = (float)seg / p_segments;
				float u1 = (float)(next_seg) / p_segments;
				if (next_seg == 0) {
					u1 = 1.0f;
				}
				float v0 = (float)(ring + 1) / p_rings;
				float v1 = (float)(ring + 2) / p_rings;

				int base_loop = mesh->faces[face_idx].loops[0];
				mesh->set_loop_uv(base_loop, Vector2(u0, v0));
				mesh->set_loop_uv(base_loop + 1, Vector2(u1, v0));
				mesh->set_loop_uv(base_loop + 2, Vector2(u1, v1));
				mesh->set_loop_uv(base_loop + 3, Vector2(u0, v1));
			}
		}
	}

	// Bottom cap (triangles)
	int last_ring = p_rings - 2;
	for (int seg = 0; seg < p_segments; seg++) {
		int next_seg = (seg + 1) % p_segments;
		int face_idx = mesh->add_triangle(ring_verts[last_ring][seg], ring_verts[last_ring][next_seg], bottom_pole);

		if (face_idx >= 0) {
			float u0 = (float)seg / p_segments;
			float u1 = (float)(next_seg) / p_segments;
			if (next_seg == 0) {
				u1 = 1.0f;
			}
			float u2 = (seg + 0.5f) / p_segments;

			int base_loop = mesh->faces[face_idx].loops[0];
			mesh->set_loop_uv(base_loop, Vector2(u0, (float)(p_rings - 1) / p_rings));
			mesh->set_loop_uv(base_loop + 1, Vector2(u1, (float)(p_rings - 1) / p_rings));
			mesh->set_loop_uv(base_loop + 2, Vector2(u2, 1.0f));
		}
	}

	mesh->recalculate_normals();
	return mesh;
}

Ref<BPYMesh> BPY::create_ico_sphere(float p_radius, int p_subdivisions) {
	Ref<BPYMesh> mesh;
	mesh.instantiate();

	p_subdivisions = CLAMP(p_subdivisions, 0, 6);

	// Golden ratio
	float t = (1.0f + Math::sqrt(5.0f)) / 2.0f;

	// Create initial icosahedron vertices
	LocalVector<Vector3> positions;
	positions.push_back(Vector3(-1, t, 0).normalized() * p_radius);
	positions.push_back(Vector3(1, t, 0).normalized() * p_radius);
	positions.push_back(Vector3(-1, -t, 0).normalized() * p_radius);
	positions.push_back(Vector3(1, -t, 0).normalized() * p_radius);
	positions.push_back(Vector3(0, -1, t).normalized() * p_radius);
	positions.push_back(Vector3(0, 1, t).normalized() * p_radius);
	positions.push_back(Vector3(0, -1, -t).normalized() * p_radius);
	positions.push_back(Vector3(0, 1, -t).normalized() * p_radius);
	positions.push_back(Vector3(t, 0, -1).normalized() * p_radius);
	positions.push_back(Vector3(t, 0, 1).normalized() * p_radius);
	positions.push_back(Vector3(-t, 0, -1).normalized() * p_radius);
	positions.push_back(Vector3(-t, 0, 1).normalized() * p_radius);

	// Initial icosahedron faces (20 triangles)
	struct Triangle {
		int v[3];
	};
	LocalVector<Triangle> triangles;

	// 5 faces around point 0
	triangles.push_back({ { 0, 11, 5 } });
	triangles.push_back({ { 0, 5, 1 } });
	triangles.push_back({ { 0, 1, 7 } });
	triangles.push_back({ { 0, 7, 10 } });
	triangles.push_back({ { 0, 10, 11 } });

	// 5 adjacent faces
	triangles.push_back({ { 1, 5, 9 } });
	triangles.push_back({ { 5, 11, 4 } });
	triangles.push_back({ { 11, 10, 2 } });
	triangles.push_back({ { 10, 7, 6 } });
	triangles.push_back({ { 7, 1, 8 } });

	// 5 faces around point 3
	triangles.push_back({ { 3, 9, 4 } });
	triangles.push_back({ { 3, 4, 2 } });
	triangles.push_back({ { 3, 2, 6 } });
	triangles.push_back({ { 3, 6, 8 } });
	triangles.push_back({ { 3, 8, 9 } });

	// 5 adjacent faces
	triangles.push_back({ { 4, 9, 5 } });
	triangles.push_back({ { 2, 4, 11 } });
	triangles.push_back({ { 6, 2, 10 } });
	triangles.push_back({ { 8, 6, 7 } });
	triangles.push_back({ { 9, 8, 1 } });

	// Subdivide
	for (int i = 0; i < p_subdivisions; i++) {
		LocalVector<Triangle> new_triangles;
		HashMap<uint64_t, int> midpoint_cache;

		auto get_midpoint = [&](int v0, int v1) -> int {
			uint64_t key = (v0 < v1) ? ((uint64_t)v0 << 32 | v1) : ((uint64_t)v1 << 32 | v0);
			if (midpoint_cache.has(key)) {
				return midpoint_cache[key];
			}
			Vector3 mid = ((positions[v0] + positions[v1]) * 0.5f).normalized() * p_radius;
			int idx = positions.size();
			positions.push_back(mid);
			midpoint_cache[key] = idx;
			return idx;
		};

		for (const Triangle &tri : triangles) {
			int a = get_midpoint(tri.v[0], tri.v[1]);
			int b = get_midpoint(tri.v[1], tri.v[2]);
			int c = get_midpoint(tri.v[2], tri.v[0]);

			new_triangles.push_back({ { tri.v[0], a, c } });
			new_triangles.push_back({ { tri.v[1], b, a } });
			new_triangles.push_back({ { tri.v[2], c, b } });
			new_triangles.push_back({ { a, b, c } });
		}

		triangles = new_triangles;
	}

	// Create mesh
	for (const Vector3 &pos : positions) {
		mesh->add_vertex(pos);
	}

	for (const Triangle &tri : triangles) {
		int face_idx = mesh->add_triangle(tri.v[0], tri.v[1], tri.v[2]);
		if (face_idx >= 0) {
			// Simple spherical UVs
			for (int i = 0; i < 3; i++) {
				const Vector3 &p = positions[tri.v[i]];
				Vector3 n = p.normalized();
				float u = 0.5f + Math::atan2(n.z, n.x) / (2.0f * Math::PI);
				float v = 0.5f - Math::asin(n.y) / Math::PI;
				mesh->set_loop_uv(mesh->faces[face_idx].loops[i], Vector2(u, v));
			}
		}
	}

	mesh->recalculate_normals();
	return mesh;
}

Ref<BPYMesh> BPY::create_cylinder(float p_radius, float p_height, int p_segments, bool p_cap_top, bool p_cap_bottom) {
	Ref<BPYMesh> mesh;
	mesh.instantiate();

	p_segments = MAX(3, p_segments);
	float half_height = p_height * 0.5f;

	// Create vertices
	LocalVector<int> top_ring;
	LocalVector<int> bottom_ring;

	for (int i = 0; i < p_segments; i++) {
		float angle = 2.0f * Math::PI * i / p_segments;
		float x = p_radius * Math::cos(angle);
		float z = p_radius * Math::sin(angle);

		top_ring.push_back(mesh->add_vertex(Vector3(x, half_height, z)));
		bottom_ring.push_back(mesh->add_vertex(Vector3(x, -half_height, z)));
	}

	// Side faces (quads)
	for (int i = 0; i < p_segments; i++) {
		int next = (i + 1) % p_segments;
		int face_idx = mesh->add_quad(bottom_ring[i], bottom_ring[next], top_ring[next], top_ring[i]);

		if (face_idx >= 0) {
			float u0 = (float)i / p_segments;
			float u1 = (float)(i + 1) / p_segments;

			int base_loop = mesh->faces[face_idx].loops[0];
			mesh->set_loop_uv(base_loop, Vector2(u0, 0));
			mesh->set_loop_uv(base_loop + 1, Vector2(u1, 0));
			mesh->set_loop_uv(base_loop + 2, Vector2(u1, 1));
			mesh->set_loop_uv(base_loop + 3, Vector2(u0, 1));
		}
	}

	// Top cap
	if (p_cap_top) {
		int center = mesh->add_vertex(Vector3(0, half_height, 0));
		for (int i = 0; i < p_segments; i++) {
			int next = (i + 1) % p_segments;
			int face_idx = mesh->add_triangle(center, top_ring[i], top_ring[next]);
			mesh->set_face_smooth(face_idx, false);

			if (face_idx >= 0) {
				float angle0 = 2.0f * Math::PI * i / p_segments;
				float angle1 = 2.0f * Math::PI * (i + 1) / p_segments;

				int base_loop = mesh->faces[face_idx].loops[0];
				mesh->set_loop_uv(base_loop, Vector2(0.5f, 0.5f));
				mesh->set_loop_uv(base_loop + 1, Vector2(0.5f + 0.5f * Math::cos(angle0), 0.5f + 0.5f * Math::sin(angle0)));
				mesh->set_loop_uv(base_loop + 2, Vector2(0.5f + 0.5f * Math::cos(angle1), 0.5f + 0.5f * Math::sin(angle1)));
			}
		}
	}

	// Bottom cap
	if (p_cap_bottom) {
		int center = mesh->add_vertex(Vector3(0, -half_height, 0));
		for (int i = 0; i < p_segments; i++) {
			int next = (i + 1) % p_segments;
			int face_idx = mesh->add_triangle(center, bottom_ring[next], bottom_ring[i]);
			mesh->set_face_smooth(face_idx, false);

			if (face_idx >= 0) {
				float angle0 = 2.0f * Math::PI * i / p_segments;
				float angle1 = 2.0f * Math::PI * (i + 1) / p_segments;

				int base_loop = mesh->faces[face_idx].loops[0];
				mesh->set_loop_uv(base_loop, Vector2(0.5f, 0.5f));
				mesh->set_loop_uv(base_loop + 1, Vector2(0.5f + 0.5f * Math::cos(angle1), 0.5f - 0.5f * Math::sin(angle1)));
				mesh->set_loop_uv(base_loop + 2, Vector2(0.5f + 0.5f * Math::cos(angle0), 0.5f - 0.5f * Math::sin(angle0)));
			}
		}
	}

	mesh->recalculate_normals();
	return mesh;
}

Ref<BPYMesh> BPY::create_cone(float p_radius, float p_height, int p_segments, bool p_cap) {
	Ref<BPYMesh> mesh;
	mesh.instantiate();

	p_segments = MAX(3, p_segments);
	float half_height = p_height * 0.5f;

	// Create apex and base ring
	int apex = mesh->add_vertex(Vector3(0, half_height, 0));

	LocalVector<int> base_ring;
	for (int i = 0; i < p_segments; i++) {
		float angle = 2.0f * Math::PI * i / p_segments;
		float x = p_radius * Math::cos(angle);
		float z = p_radius * Math::sin(angle);
		base_ring.push_back(mesh->add_vertex(Vector3(x, -half_height, z)));
	}

	// Side faces (triangles)
	for (int i = 0; i < p_segments; i++) {
		int next = (i + 1) % p_segments;
		int face_idx = mesh->add_triangle(base_ring[i], base_ring[next], apex);

		if (face_idx >= 0) {
			float u0 = (float)i / p_segments;
			float u1 = (float)(i + 1) / p_segments;
			float u_mid = (u0 + u1) * 0.5f;

			int base_loop = mesh->faces[face_idx].loops[0];
			mesh->set_loop_uv(base_loop, Vector2(u0, 0));
			mesh->set_loop_uv(base_loop + 1, Vector2(u1, 0));
			mesh->set_loop_uv(base_loop + 2, Vector2(u_mid, 1));
		}
	}

	// Base cap
	if (p_cap) {
		int center = mesh->add_vertex(Vector3(0, -half_height, 0));
		for (int i = 0; i < p_segments; i++) {
			int next = (i + 1) % p_segments;
			int face_idx = mesh->add_triangle(center, base_ring[next], base_ring[i]);
			mesh->set_face_smooth(face_idx, false);

			if (face_idx >= 0) {
				float angle0 = 2.0f * Math::PI * i / p_segments;
				float angle1 = 2.0f * Math::PI * (i + 1) / p_segments;

				int base_loop = mesh->faces[face_idx].loops[0];
				mesh->set_loop_uv(base_loop, Vector2(0.5f, 0.5f));
				mesh->set_loop_uv(base_loop + 1, Vector2(0.5f + 0.5f * Math::cos(angle1), 0.5f - 0.5f * Math::sin(angle1)));
				mesh->set_loop_uv(base_loop + 2, Vector2(0.5f + 0.5f * Math::cos(angle0), 0.5f - 0.5f * Math::sin(angle0)));
			}
		}
	}

	mesh->recalculate_normals();
	return mesh;
}

Ref<BPYMesh> BPY::create_torus(float p_major_radius, float p_minor_radius, int p_major_segments, int p_minor_segments) {
	Ref<BPYMesh> mesh;
	mesh.instantiate();

	p_major_segments = MAX(3, p_major_segments);
	p_minor_segments = MAX(3, p_minor_segments);

	// Create vertices
	LocalVector<LocalVector<int>> verts;
	verts.resize(p_major_segments);

	for (int i = 0; i < p_major_segments; i++) {
		float major_angle = 2.0f * Math::PI * i / p_major_segments;
		Vector3 center(p_major_radius * Math::cos(major_angle), 0, p_major_radius * Math::sin(major_angle));
		Vector3 radial = center.normalized();

		verts[i].resize(p_minor_segments);
		for (int j = 0; j < p_minor_segments; j++) {
			float minor_angle = 2.0f * Math::PI * j / p_minor_segments;
			Vector3 pos = center + radial * (p_minor_radius * Math::cos(minor_angle)) + Vector3(0, p_minor_radius * Math::sin(minor_angle), 0);
			verts[i][j] = mesh->add_vertex(pos);
		}
	}

	// Create faces (quads)
	for (int i = 0; i < p_major_segments; i++) {
		int next_i = (i + 1) % p_major_segments;
		for (int j = 0; j < p_minor_segments; j++) {
			int next_j = (j + 1) % p_minor_segments;

			int face_idx = mesh->add_quad(
					verts[i][j],
					verts[next_i][j],
					verts[next_i][next_j],
					verts[i][next_j]);

			if (face_idx >= 0) {
				float u0 = (float)i / p_major_segments;
				float u1 = (float)(i + 1) / p_major_segments;
				float v0 = (float)j / p_minor_segments;
				float v1 = (float)(j + 1) / p_minor_segments;

				int base_loop = mesh->faces[face_idx].loops[0];
				mesh->set_loop_uv(base_loop, Vector2(u0, v0));
				mesh->set_loop_uv(base_loop + 1, Vector2(u1, v0));
				mesh->set_loop_uv(base_loop + 2, Vector2(u1, v1));
				mesh->set_loop_uv(base_loop + 3, Vector2(u0, v1));
			}
		}
	}

	mesh->recalculate_normals();
	return mesh;
}

Ref<BPYMesh> BPY::create_plane(const Vector2 &p_size, const Vector2i &p_subdivisions) {
	Ref<BPYMesh> mesh;
	mesh.instantiate();

	Vector2i subdiv = p_subdivisions.maxi(1);
	Vector2 half = p_size * 0.5f;

	// Create vertices
	LocalVector<LocalVector<int>> verts;
	verts.resize(subdiv.y + 1);

	for (int y = 0; y <= subdiv.y; y++) {
		verts[y].resize(subdiv.x + 1);
		for (int x = 0; x <= subdiv.x; x++) {
			float px = -half.x + p_size.x * x / subdiv.x;
			float pz = -half.y + p_size.y * y / subdiv.y;
			verts[y][x] = mesh->add_vertex(Vector3(px, 0, pz));
		}
	}

	// Create faces (quads)
	for (int y = 0; y < subdiv.y; y++) {
		for (int x = 0; x < subdiv.x; x++) {
			int face_idx = mesh->add_quad(
					verts[y][x],
					verts[y][x + 1],
					verts[y + 1][x + 1],
					verts[y + 1][x]);

			if (face_idx >= 0) {
				float u0 = (float)x / subdiv.x;
				float u1 = (float)(x + 1) / subdiv.x;
				float v0 = (float)y / subdiv.y;
				float v1 = (float)(y + 1) / subdiv.y;

				int base_loop = mesh->faces[face_idx].loops[0];
				mesh->set_loop_uv(base_loop, Vector2(u0, v0));
				mesh->set_loop_uv(base_loop + 1, Vector2(u1, v0));
				mesh->set_loop_uv(base_loop + 2, Vector2(u1, v1));
				mesh->set_loop_uv(base_loop + 3, Vector2(u0, v1));

				mesh->set_face_smooth(face_idx, false);
			}
		}
	}

	mesh->recalculate_normals();
	return mesh;
}

Ref<BPYMesh> BPY::create_circle(float p_radius, int p_segments, bool p_fill) {
	Ref<BPYMesh> mesh;
	mesh.instantiate();

	p_segments = MAX(3, p_segments);

	// Create vertices
	LocalVector<int> ring;
	for (int i = 0; i < p_segments; i++) {
		float angle = 2.0f * Math::PI * i / p_segments;
		float x = p_radius * Math::cos(angle);
		float z = p_radius * Math::sin(angle);
		ring.push_back(mesh->add_vertex(Vector3(x, 0, z)));
	}

	if (p_fill) {
		// Create center vertex and fill with triangles
		int center = mesh->add_vertex(Vector3(0, 0, 0));
		for (int i = 0; i < p_segments; i++) {
			int next = (i + 1) % p_segments;
			int face_idx = mesh->add_triangle(center, ring[i], ring[next]);

			if (face_idx >= 0) {
				float angle0 = 2.0f * Math::PI * i / p_segments;
				float angle1 = 2.0f * Math::PI * (i + 1) / p_segments;

				int base_loop = mesh->faces[face_idx].loops[0];
				mesh->set_loop_uv(base_loop, Vector2(0.5f, 0.5f));
				mesh->set_loop_uv(base_loop + 1, Vector2(0.5f + 0.5f * Math::cos(angle0), 0.5f + 0.5f * Math::sin(angle0)));
				mesh->set_loop_uv(base_loop + 2, Vector2(0.5f + 0.5f * Math::cos(angle1), 0.5f + 0.5f * Math::sin(angle1)));

				mesh->set_face_smooth(face_idx, false);
			}
		}
	} else {
		// Just create edges (no faces)
		for (int i = 0; i < p_segments; i++) {
			int next = (i + 1) % p_segments;
			mesh->add_edge(ring[i], ring[next]);
		}
	}

	mesh->recalculate_normals();
	return mesh;
}

Ref<BPYMesh> BPY::create_grid(const Vector2 &p_size, const Vector2i &p_subdivisions) {
	// Grid is just a plane alias with different default subdivisions
	return create_plane(p_size, p_subdivisions);
}

BPY::BPY() {
}

BPY::~BPY() {
}
