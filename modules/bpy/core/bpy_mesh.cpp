/**************************************************************************/
/*  bpy_mesh.cpp                                                          */
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

#include "bpy_mesh.h"

#include "core/object/class_db.h"
#include "scene/resources/surface_tool.h"

void BPYMesh::_bind_methods() {
	// Vertex operations
	ClassDB::bind_method(D_METHOD("add_vertex", "position"), &BPYMesh::add_vertex);
	ClassDB::bind_method(D_METHOD("set_vertex_position", "index", "position"), &BPYMesh::set_vertex_position);
	ClassDB::bind_method(D_METHOD("get_vertex_position", "index"), &BPYMesh::get_vertex_position);
	ClassDB::bind_method(D_METHOD("set_vertex_normal", "index", "normal"), &BPYMesh::set_vertex_normal);
	ClassDB::bind_method(D_METHOD("get_vertex_normal", "index"), &BPYMesh::get_vertex_normal);
	ClassDB::bind_method(D_METHOD("set_vertex_color", "index", "color"), &BPYMesh::set_vertex_color);
	ClassDB::bind_method(D_METHOD("get_vertex_color", "index"), &BPYMesh::get_vertex_color);
	ClassDB::bind_method(D_METHOD("set_vertex_crease", "index", "crease"), &BPYMesh::set_vertex_crease);
	ClassDB::bind_method(D_METHOD("get_vertex_crease", "index"), &BPYMesh::get_vertex_crease);
	ClassDB::bind_method(D_METHOD("get_vertex_count"), &BPYMesh::get_vertex_count);
	ClassDB::bind_method(D_METHOD("get_vertex_edges", "index"), &BPYMesh::get_vertex_edges);
	ClassDB::bind_method(D_METHOD("get_vertex_faces", "index"), &BPYMesh::get_vertex_faces);

	// Edge operations
	ClassDB::bind_method(D_METHOD("add_edge", "v0", "v1"), &BPYMesh::add_edge);
	ClassDB::bind_method(D_METHOD("find_edge", "v0", "v1"), &BPYMesh::find_edge);
	ClassDB::bind_method(D_METHOD("get_edge_vertex", "edge_index", "which"), &BPYMesh::get_edge_vertex);
	ClassDB::bind_method(D_METHOD("set_edge_crease", "index", "crease"), &BPYMesh::set_edge_crease);
	ClassDB::bind_method(D_METHOD("get_edge_crease", "index"), &BPYMesh::get_edge_crease);
	ClassDB::bind_method(D_METHOD("set_edge_sharp", "index", "sharp"), &BPYMesh::set_edge_sharp);
	ClassDB::bind_method(D_METHOD("get_edge_sharp", "index"), &BPYMesh::get_edge_sharp);
	ClassDB::bind_method(D_METHOD("set_edge_seam", "index", "seam"), &BPYMesh::set_edge_seam);
	ClassDB::bind_method(D_METHOD("get_edge_seam", "index"), &BPYMesh::get_edge_seam);
	ClassDB::bind_method(D_METHOD("get_edge_count"), &BPYMesh::get_edge_count);
	ClassDB::bind_method(D_METHOD("get_edge_faces", "index"), &BPYMesh::get_edge_faces);

	// Face operations
	ClassDB::bind_method(D_METHOD("add_face", "vertex_indices"), &BPYMesh::add_face);
	ClassDB::bind_method(D_METHOD("add_triangle", "v0", "v1", "v2"), &BPYMesh::add_triangle);
	ClassDB::bind_method(D_METHOD("add_quad", "v0", "v1", "v2", "v3"), &BPYMesh::add_quad);
	ClassDB::bind_method(D_METHOD("get_face_vertices", "index"), &BPYMesh::get_face_vertices);
	ClassDB::bind_method(D_METHOD("set_face_smooth", "index", "smooth"), &BPYMesh::set_face_smooth);
	ClassDB::bind_method(D_METHOD("get_face_smooth", "index"), &BPYMesh::get_face_smooth);
	ClassDB::bind_method(D_METHOD("set_face_material_index", "index", "material"), &BPYMesh::set_face_material_index);
	ClassDB::bind_method(D_METHOD("get_face_material_index", "index"), &BPYMesh::get_face_material_index);
	ClassDB::bind_method(D_METHOD("get_face_normal", "index"), &BPYMesh::get_face_normal);
	ClassDB::bind_method(D_METHOD("get_face_center", "index"), &BPYMesh::get_face_center);
	ClassDB::bind_method(D_METHOD("get_face_count"), &BPYMesh::get_face_count);

	// Loop operations
	ClassDB::bind_method(D_METHOD("set_loop_uv", "index", "uv"), &BPYMesh::set_loop_uv);
	ClassDB::bind_method(D_METHOD("get_loop_uv", "index"), &BPYMesh::get_loop_uv);
	ClassDB::bind_method(D_METHOD("set_loop_normal", "index", "normal"), &BPYMesh::set_loop_normal);
	ClassDB::bind_method(D_METHOD("get_loop_normal", "index"), &BPYMesh::get_loop_normal);
	ClassDB::bind_method(D_METHOD("set_loop_color", "index", "color"), &BPYMesh::set_loop_color);
	ClassDB::bind_method(D_METHOD("get_loop_color", "index"), &BPYMesh::get_loop_color);
	ClassDB::bind_method(D_METHOD("get_loop_count"), &BPYMesh::get_loop_count);
	ClassDB::bind_method(D_METHOD("get_loop_vertex", "index"), &BPYMesh::get_loop_vertex);
	ClassDB::bind_method(D_METHOD("get_loop_face", "index"), &BPYMesh::get_loop_face);

	// Mesh-wide operations
	ClassDB::bind_method(D_METHOD("clear"), &BPYMesh::clear);
	ClassDB::bind_method(D_METHOD("recalculate_normals"), &BPYMesh::recalculate_normals);
	ClassDB::bind_method(D_METHOD("flip_normals"), &BPYMesh::flip_normals);
	ClassDB::bind_method(D_METHOD("transform", "transform"), &BPYMesh::transform);
	ClassDB::bind_method(D_METHOD("duplicate"), &BPYMesh::duplicate);

	// Conversion
	ClassDB::bind_method(D_METHOD("to_array_mesh"), &BPYMesh::to_array_mesh);
	ClassDB::bind_method(D_METHOD("from_array_mesh", "mesh", "surface"), &BPYMesh::from_array_mesh, DEFVAL(0));

	// Utility
	ClassDB::bind_method(D_METHOD("get_aabb"), &BPYMesh::get_aabb);
	ClassDB::bind_method(D_METHOD("merge_vertices", "threshold"), &BPYMesh::merge_vertices, DEFVAL(0.0001f));
	ClassDB::bind_method(D_METHOD("triangulate"), &BPYMesh::triangulate);
}

// Vertex operations

int BPYMesh::add_vertex(const Vector3 &p_position) {
	BPYVertex v;
	v.position = p_position;
	v.index = vertices.size();
	vertices.push_back(v);
	return v.index;
}

void BPYMesh::set_vertex_position(int p_index, const Vector3 &p_position) {
	ERR_FAIL_INDEX(p_index, (int)vertices.size());
	vertices[p_index].position = p_position;
}

Vector3 BPYMesh::get_vertex_position(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, (int)vertices.size(), Vector3());
	return vertices[p_index].position;
}

void BPYMesh::set_vertex_normal(int p_index, const Vector3 &p_normal) {
	ERR_FAIL_INDEX(p_index, (int)vertices.size());
	vertices[p_index].normal = p_normal;
}

Vector3 BPYMesh::get_vertex_normal(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, (int)vertices.size(), Vector3());
	return vertices[p_index].normal;
}

void BPYMesh::set_vertex_color(int p_index, const Color &p_color) {
	ERR_FAIL_INDEX(p_index, (int)vertices.size());
	vertices[p_index].color = p_color;
}

Color BPYMesh::get_vertex_color(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, (int)vertices.size(), Color());
	return vertices[p_index].color;
}

void BPYMesh::set_vertex_crease(int p_index, float p_crease) {
	ERR_FAIL_INDEX(p_index, (int)vertices.size());
	vertices[p_index].crease = CLAMP(p_crease, 0.0f, 1.0f);
}

float BPYMesh::get_vertex_crease(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, (int)vertices.size(), 0.0f);
	return vertices[p_index].crease;
}

int BPYMesh::get_vertex_count() const {
	return vertices.size();
}

PackedInt32Array BPYMesh::get_vertex_edges(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, (int)vertices.size(), PackedInt32Array());
	PackedInt32Array result;
	for (int e : vertices[p_index].edges) {
		result.push_back(e);
	}
	return result;
}

PackedInt32Array BPYMesh::get_vertex_faces(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, (int)vertices.size(), PackedInt32Array());
	PackedInt32Array result;
	HashSet<int> face_set;
	for (int loop_idx : vertices[p_index].loops) {
		int face_idx = loops[loop_idx].face;
		if (!face_set.has(face_idx)) {
			face_set.insert(face_idx);
			result.push_back(face_idx);
		}
	}
	return result;
}

// Edge operations

int BPYMesh::add_edge(int p_v0, int p_v1) {
	ERR_FAIL_INDEX_V(p_v0, (int)vertices.size(), -1);
	ERR_FAIL_INDEX_V(p_v1, (int)vertices.size(), -1);
	ERR_FAIL_COND_V(p_v0 == p_v1, -1);

	uint64_t key = _edge_key(p_v0, p_v1);
	if (edge_map.has(key)) {
		return edge_map[key];
	}

	BPYEdge e(p_v0, p_v1);
	e.index = edges.size();
	edges.push_back(e);

	vertices[p_v0].edges.push_back(e.index);
	vertices[p_v1].edges.push_back(e.index);

	edge_map[key] = e.index;
	return e.index;
}

int BPYMesh::find_edge(int p_v0, int p_v1) const {
	uint64_t key = _edge_key(p_v0, p_v1);
	if (edge_map.has(key)) {
		return edge_map[key];
	}
	return -1;
}

int BPYMesh::get_edge_vertex(int p_edge_index, int p_which) const {
	ERR_FAIL_INDEX_V(p_edge_index, (int)edges.size(), -1);
	ERR_FAIL_INDEX_V(p_which, 2, -1);
	return edges[p_edge_index].vertices[p_which];
}

void BPYMesh::set_edge_crease(int p_index, float p_crease) {
	ERR_FAIL_INDEX(p_index, (int)edges.size());
	edges[p_index].crease = CLAMP(p_crease, 0.0f, 1.0f);
}

float BPYMesh::get_edge_crease(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, (int)edges.size(), 0.0f);
	return edges[p_index].crease;
}

void BPYMesh::set_edge_sharp(int p_index, bool p_sharp) {
	ERR_FAIL_INDEX(p_index, (int)edges.size());
	edges[p_index].sharp = p_sharp;
}

bool BPYMesh::get_edge_sharp(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, (int)edges.size(), false);
	return edges[p_index].sharp;
}

void BPYMesh::set_edge_seam(int p_index, bool p_seam) {
	ERR_FAIL_INDEX(p_index, (int)edges.size());
	edges[p_index].seam = p_seam;
}

bool BPYMesh::get_edge_seam(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, (int)edges.size(), false);
	return edges[p_index].seam;
}

int BPYMesh::get_edge_count() const {
	return edges.size();
}

PackedInt32Array BPYMesh::get_edge_faces(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, (int)edges.size(), PackedInt32Array());
	PackedInt32Array result;
	for (int f : edges[p_index].faces) {
		result.push_back(f);
	}
	return result;
}

// Face operations

int BPYMesh::add_face(const PackedInt32Array &p_vertex_indices) {
	ERR_FAIL_COND_V(p_vertex_indices.size() < 3, -1);

	// Validate indices
	for (int i = 0; i < p_vertex_indices.size(); i++) {
		ERR_FAIL_INDEX_V(p_vertex_indices[i], (int)vertices.size(), -1);
	}

	BPYFace face;
	face.index = faces.size();

	int n = p_vertex_indices.size();
	int first_loop = loops.size();

	// Create loops for each vertex
	for (int i = 0; i < n; i++) {
		int v_curr = p_vertex_indices[i];
		int v_next = p_vertex_indices[(i + 1) % n];

		// Ensure edge exists
		int edge_idx = add_edge(v_curr, v_next);

		BPYLoop loop(v_curr, edge_idx, face.index);
		loop.index = loops.size();
		loops.push_back(loop);
		face.loops.push_back(loop.index);

		// Update vertex->loop connection
		vertices[v_curr].loops.push_back(loop.index);

		// Update edge->face connection
		if (edges[edge_idx].faces.find(face.index) == -1) {
			edges[edge_idx].faces.push_back(face.index);
		}
	}

	// Link loops within face
	for (int i = 0; i < n; i++) {
		int loop_idx = first_loop + i;
		loops[loop_idx].next = first_loop + ((i + 1) % n);
		loops[loop_idx].prev = first_loop + ((i - 1 + n) % n);
	}

	// Calculate face normal
	face.calc_normal(vertices, loops);

	// Set default loop normals to face normal
	for (int loop_idx : face.loops) {
		loops[loop_idx].normal = face.normal;
	}

	faces.push_back(face);
	return face.index;
}

int BPYMesh::add_triangle(int p_v0, int p_v1, int p_v2) {
	PackedInt32Array verts;
	verts.push_back(p_v0);
	verts.push_back(p_v1);
	verts.push_back(p_v2);
	return add_face(verts);
}

int BPYMesh::add_quad(int p_v0, int p_v1, int p_v2, int p_v3) {
	PackedInt32Array verts;
	verts.push_back(p_v0);
	verts.push_back(p_v1);
	verts.push_back(p_v2);
	verts.push_back(p_v3);
	return add_face(verts);
}

PackedInt32Array BPYMesh::get_face_vertices(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, (int)faces.size(), PackedInt32Array());
	PackedInt32Array result;
	for (int loop_idx : faces[p_index].loops) {
		result.push_back(loops[loop_idx].vertex);
	}
	return result;
}

void BPYMesh::set_face_smooth(int p_index, bool p_smooth) {
	ERR_FAIL_INDEX(p_index, (int)faces.size());
	faces[p_index].smooth = p_smooth;
}

bool BPYMesh::get_face_smooth(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, (int)faces.size(), false);
	return faces[p_index].smooth;
}

void BPYMesh::set_face_material_index(int p_index, int p_material) {
	ERR_FAIL_INDEX(p_index, (int)faces.size());
	faces[p_index].material_index = p_material;
}

int BPYMesh::get_face_material_index(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, (int)faces.size(), 0);
	return faces[p_index].material_index;
}

Vector3 BPYMesh::get_face_normal(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, (int)faces.size(), Vector3());
	return faces[p_index].normal;
}

Vector3 BPYMesh::get_face_center(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, (int)faces.size(), Vector3());
	Vector3 center;
	const BPYFace &face = faces[p_index];
	for (int loop_idx : face.loops) {
		center += vertices[loops[loop_idx].vertex].position;
	}
	return center / (float)face.loops.size();
}

int BPYMesh::get_face_count() const {
	return faces.size();
}

// Loop operations

void BPYMesh::set_loop_uv(int p_index, const Vector2 &p_uv) {
	ERR_FAIL_INDEX(p_index, (int)loops.size());
	loops[p_index].uv = p_uv;
}

Vector2 BPYMesh::get_loop_uv(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, (int)loops.size(), Vector2());
	return loops[p_index].uv;
}

void BPYMesh::set_loop_normal(int p_index, const Vector3 &p_normal) {
	ERR_FAIL_INDEX(p_index, (int)loops.size());
	loops[p_index].normal = p_normal;
}

Vector3 BPYMesh::get_loop_normal(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, (int)loops.size(), Vector3());
	return loops[p_index].normal;
}

void BPYMesh::set_loop_color(int p_index, const Color &p_color) {
	ERR_FAIL_INDEX(p_index, (int)loops.size());
	loops[p_index].color = p_color;
}

Color BPYMesh::get_loop_color(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, (int)loops.size(), Color());
	return loops[p_index].color;
}

int BPYMesh::get_loop_count() const {
	return loops.size();
}

int BPYMesh::get_loop_vertex(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, (int)loops.size(), -1);
	return loops[p_index].vertex;
}

int BPYMesh::get_loop_face(int p_index) const {
	ERR_FAIL_INDEX_V(p_index, (int)loops.size(), -1);
	return loops[p_index].face;
}

// Mesh-wide operations

void BPYMesh::clear() {
	vertices.clear();
	edges.clear();
	loops.clear();
	faces.clear();
	edge_map.clear();
}

void BPYMesh::recalculate_normals() {
	// Reset vertex normals
	for (BPYVertex &v : vertices) {
		v.normal = Vector3();
	}

	// Recalculate face normals and accumulate to vertices
	for (BPYFace &face : faces) {
		face.calc_normal(vertices, loops);

		for (int loop_idx : face.loops) {
			int v_idx = loops[loop_idx].vertex;
			vertices[v_idx].normal += face.normal;
		}
	}

	// Normalize vertex normals
	for (BPYVertex &v : vertices) {
		if (v.normal.length_squared() > 0.0001f) {
			v.normal.normalize();
		}
	}

	// Update loop normals based on smooth flag
	for (BPYFace &face : faces) {
		for (int loop_idx : face.loops) {
			if (face.smooth) {
				loops[loop_idx].normal = vertices[loops[loop_idx].vertex].normal;
			} else {
				loops[loop_idx].normal = face.normal;
			}
		}
	}
}

void BPYMesh::flip_normals() {
	for (BPYFace &face : faces) {
		face.normal = -face.normal;
		// Reverse loop order
		face.loops.reverse();
		// Update loop links
		for (int i = 0; i < (int)face.loops.size(); i++) {
			int loop_idx = face.loops[i];
			loops[loop_idx].next = face.loops[(i + 1) % face.loops.size()];
			loops[loop_idx].prev = face.loops[(i - 1 + face.loops.size()) % face.loops.size()];
			loops[loop_idx].normal = -loops[loop_idx].normal;
		}
	}

	for (BPYVertex &v : vertices) {
		v.normal = -v.normal;
	}
}

void BPYMesh::transform(const Transform3D &p_transform) {
	Basis normal_basis = p_transform.basis.inverse().transposed();

	for (BPYVertex &v : vertices) {
		v.position = p_transform.xform(v.position);
		v.normal = normal_basis.xform(v.normal).normalized();
	}

	for (BPYLoop &l : loops) {
		l.normal = normal_basis.xform(l.normal).normalized();
	}

	for (BPYFace &face : faces) {
		face.normal = normal_basis.xform(face.normal).normalized();
	}
}

Ref<BPYMesh> BPYMesh::duplicate() const {
	Ref<BPYMesh> copy;
	copy.instantiate();

	copy->vertices = vertices;
	copy->edges = edges;
	copy->loops = loops;
	copy->faces = faces;
	copy->edge_map = edge_map;

	return copy;
}

// Conversion

Ref<ArrayMesh> BPYMesh::to_array_mesh() const {
	Ref<ArrayMesh> array_mesh;
	array_mesh.instantiate();

	if (faces.is_empty()) {
		return array_mesh;
	}

	// Group faces by material
	HashMap<int, LocalVector<int>> faces_by_material;
	for (int i = 0; i < (int)faces.size(); i++) {
		faces_by_material[faces[i].material_index].push_back(i);
	}

	for (const KeyValue<int, LocalVector<int>> &E : faces_by_material) {
		Ref<SurfaceTool> st;
		st.instantiate();
		st->begin(Mesh::PRIMITIVE_TRIANGLES);

		for (int face_idx : E.value) {
			const BPYFace &face = faces[face_idx];

			// Triangulate face (fan triangulation)
			for (int i = 1; i < (int)face.loops.size() - 1; i++) {
				int loop_indices[3] = {
					(int)face.loops[0],
					(int)face.loops[i],
					(int)face.loops[i + 1]
				};

				for (int j = 0; j < 3; j++) {
					const BPYLoop &loop = loops[loop_indices[j]];
					const BPYVertex &vert = vertices[loop.vertex];

					st->set_normal(loop.normal);
					st->set_uv(loop.uv);
					st->set_color(loop.color);
					st->add_vertex(vert.position);
				}
			}
		}

		st->generate_tangents();
		st->index();
		array_mesh = st->commit(array_mesh);
	}

	return array_mesh;
}

void BPYMesh::from_array_mesh(const Ref<ArrayMesh> &p_mesh, int p_surface) {
	clear();
	ERR_FAIL_COND(p_mesh.is_null());
	ERR_FAIL_INDEX(p_surface, p_mesh->get_surface_count());

	Array arrays = p_mesh->surface_get_arrays(p_surface);
	ERR_FAIL_COND(arrays.is_empty());

	PackedVector3Array positions = arrays[Mesh::ARRAY_VERTEX];
	PackedVector3Array normals = arrays[Mesh::ARRAY_NORMAL];
	PackedVector2Array uvs = arrays[Mesh::ARRAY_TEX_UV];
	PackedColorArray colors = arrays[Mesh::ARRAY_COLOR];
	PackedInt32Array indices = arrays[Mesh::ARRAY_INDEX];

	bool has_normals = normals.size() == positions.size();
	bool has_uvs = uvs.size() == positions.size();
	bool has_colors = colors.size() == positions.size();
	bool has_indices = indices.size() > 0;

	// Merge duplicate vertices
	HashMap<Vector3, int> vertex_map;
	LocalVector<int> index_remap;
	index_remap.resize(positions.size());

	for (int i = 0; i < positions.size(); i++) {
		const Vector3 &pos = positions[i];
		if (vertex_map.has(pos)) {
			index_remap[i] = vertex_map[pos];
		} else {
			int new_idx = add_vertex(pos);
			vertex_map[pos] = new_idx;
			index_remap[i] = new_idx;

			if (has_normals) {
				vertices[new_idx].normal = normals[i];
			}
			if (has_colors) {
				vertices[new_idx].color = colors[i];
			}
		}
	}

	// Create faces
	if (has_indices) {
		for (int i = 0; i < indices.size(); i += 3) {
			int v0 = index_remap[indices[i]];
			int v1 = index_remap[indices[i + 1]];
			int v2 = index_remap[indices[i + 2]];

			int face_idx = add_triangle(v0, v1, v2);
			if (face_idx >= 0) {
				// Set UVs on loops
				if (has_uvs) {
					int base_loop = faces[face_idx].loops[0];
					loops[base_loop].uv = uvs[indices[i]];
					loops[base_loop + 1].uv = uvs[indices[i + 1]];
					loops[base_loop + 2].uv = uvs[indices[i + 2]];
				}
				if (has_normals) {
					int base_loop = faces[face_idx].loops[0];
					loops[base_loop].normal = normals[indices[i]];
					loops[base_loop + 1].normal = normals[indices[i + 1]];
					loops[base_loop + 2].normal = normals[indices[i + 2]];
				}
			}
		}
	} else {
		for (int i = 0; i < positions.size(); i += 3) {
			int v0 = index_remap[i];
			int v1 = index_remap[i + 1];
			int v2 = index_remap[i + 2];

			int face_idx = add_triangle(v0, v1, v2);
			if (face_idx >= 0 && has_uvs) {
				int base_loop = faces[face_idx].loops[0];
				loops[base_loop].uv = uvs[i];
				loops[base_loop + 1].uv = uvs[i + 1];
				loops[base_loop + 2].uv = uvs[i + 2];
			}
		}
	}
}

// Utility

AABB BPYMesh::get_aabb() const {
	if (vertices.is_empty()) {
		return AABB();
	}

	AABB aabb;
	aabb.position = vertices[0].position;
	for (int i = 1; i < (int)vertices.size(); i++) {
		aabb.expand_to(vertices[i].position);
	}
	return aabb;
}

void BPYMesh::merge_vertices(float p_threshold) {
	if (vertices.size() < 2) {
		return;
	}

	float threshold_sq = p_threshold * p_threshold;
	LocalVector<int> remap;
	remap.resize(vertices.size());

	for (int i = 0; i < (int)vertices.size(); i++) {
		remap[i] = i;
	}

	// Find vertices to merge
	for (int i = 0; i < (int)vertices.size(); i++) {
		if (remap[i] != i) {
			continue; // Already remapped
		}
		for (int j = i + 1; j < (int)vertices.size(); j++) {
			if (remap[j] != j) {
				continue;
			}
			if (vertices[i].position.distance_squared_to(vertices[j].position) < threshold_sq) {
				remap[j] = i;
			}
		}
	}

	// Update loop vertex references
	for (BPYLoop &loop : loops) {
		loop.vertex = remap[loop.vertex];
	}

	// Update edge vertex references and rebuild edge map
	edge_map.clear();
	for (BPYEdge &edge : edges) {
		edge.vertices[0] = remap[edge.vertices[0]];
		edge.vertices[1] = remap[edge.vertices[1]];
		uint64_t key = _edge_key(edge.vertices[0], edge.vertices[1]);
		edge_map[key] = edge.index;
	}

	// Rebuild vertex connections
	for (BPYVertex &v : vertices) {
		v.edges.clear();
		v.loops.clear();
	}
	_update_topology();
}

void BPYMesh::triangulate() {
	LocalVector<BPYFace> new_faces;
	LocalVector<BPYLoop> new_loops;

	for (const BPYFace &face : faces) {
		if (face.loops.size() <= 3) {
			// Already a triangle, just copy
			BPYFace new_face = face;
			new_face.index = new_faces.size();
			new_face.loops.clear();

			for (int loop_idx : face.loops) {
				BPYLoop new_loop = loops[loop_idx];
				new_loop.index = new_loops.size();
				new_loop.face = new_face.index;
				new_face.loops.push_back(new_loop.index);
				new_loops.push_back(new_loop);
			}

			// Update loop links
			for (int i = 0; i < (int)new_face.loops.size(); i++) {
				int idx = new_face.loops[i];
				new_loops[idx].next = new_face.loops[(i + 1) % new_face.loops.size()];
				new_loops[idx].prev = new_face.loops[(i - 1 + new_face.loops.size()) % new_face.loops.size()];
			}

			new_faces.push_back(new_face);
		} else {
			// Fan triangulation
			for (int i = 1; i < (int)face.loops.size() - 1; i++) {
				BPYFace new_face;
				new_face.index = new_faces.size();
				new_face.normal = face.normal;
				new_face.smooth = face.smooth;
				new_face.material_index = face.material_index;

				int src_loops[3] = { (int)face.loops[0], (int)face.loops[i], (int)face.loops[i + 1] };

				for (int j = 0; j < 3; j++) {
					BPYLoop new_loop = loops[src_loops[j]];
					new_loop.index = new_loops.size();
					new_loop.face = new_face.index;
					new_face.loops.push_back(new_loop.index);
					new_loops.push_back(new_loop);
				}

				// Update loop links
				for (int j = 0; j < 3; j++) {
					int idx = new_face.loops[j];
					new_loops[idx].next = new_face.loops[(j + 1) % 3];
					new_loops[idx].prev = new_face.loops[(j + 2) % 3];
				}

				new_faces.push_back(new_face);
			}
		}
	}

	faces = new_faces;
	loops = new_loops;

	// Rebuild vertex and edge connections
	for (BPYVertex &v : vertices) {
		v.loops.clear();
	}
	for (BPYEdge &e : edges) {
		e.faces.clear();
		e.loops.clear();
	}

	_update_topology();
}

void BPYMesh::_update_topology() {
	// Rebuild vertex->loop connections
	for (int i = 0; i < (int)loops.size(); i++) {
		int v_idx = loops[i].vertex;
		if (v_idx >= 0 && v_idx < (int)vertices.size()) {
			vertices[v_idx].loops.push_back(i);
		}
	}

	// Rebuild edge->face connections
	for (int face_idx = 0; face_idx < (int)faces.size(); face_idx++) {
		for (int loop_idx : faces[face_idx].loops) {
			int edge_idx = loops[loop_idx].edge;
			if (edge_idx >= 0 && edge_idx < (int)edges.size()) {
				if (edges[edge_idx].faces.find(face_idx) == -1) {
					edges[edge_idx].faces.push_back(face_idx);
				}
			}
		}
	}
}

void BPYMesh::_ensure_edge(int p_v0, int p_v1, int p_loop_index) {
	int edge_idx = find_edge(p_v0, p_v1);
	if (edge_idx < 0) {
		edge_idx = add_edge(p_v0, p_v1);
	}
	if (p_loop_index >= 0 && p_loop_index < (int)loops.size()) {
		loops[p_loop_index].edge = edge_idx;
		edges[edge_idx].loops.push_back(p_loop_index);
	}
}

BPYMesh::BPYMesh() {
}

BPYMesh::~BPYMesh() {
}
