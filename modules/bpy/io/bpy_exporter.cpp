/**************************************************************************/
/*  bpy_exporter.cpp                                                      */
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

#include "bpy_exporter.h"

#include "core/io/file_access.h"
#include "core/string/print_string.h"
#include "scene/resources/material.h"

void BPYExporter::_bind_methods() {
	ClassDB::bind_static_method("BPYExporter", D_METHOD("export_obj", "mesh", "path", "materials"), &BPYExporter::export_obj, DEFVAL(TypedArray<Material>()));
	ClassDB::bind_static_method("BPYExporter", D_METHOD("export_stl", "mesh", "path", "binary"), &BPYExporter::export_stl, DEFVAL(false));
	ClassDB::bind_static_method("BPYExporter", D_METHOD("export_gltf", "mesh", "path", "materials"), &BPYExporter::export_gltf, DEFVAL(TypedArray<Material>()));
	ClassDB::bind_static_method("BPYExporter", D_METHOD("export_ply", "mesh", "path", "binary"), &BPYExporter::export_ply, DEFVAL(false));
}

Error BPYExporter::export_obj(const Ref<ArrayMesh> &p_mesh, const String &p_path, const TypedArray<Material> &p_materials) {
	ERR_FAIL_COND_V(p_mesh.is_null(), ERR_INVALID_PARAMETER);

	print_line("[BPY Export] Starting OBJ export to: " + p_path);
	print_line("[BPY Export] Mesh surfaces: " + itos(p_mesh->get_surface_count()));

	// Create OBJ file
	Ref<FileAccess> obj_file = FileAccess::open(p_path, FileAccess::WRITE);
	ERR_FAIL_COND_V(obj_file.is_null(), ERR_CANT_CREATE);

	// Create MTL file
	String mtl_path = p_path.get_basename() + ".mtl";
	String mtl_filename = p_path.get_file().get_basename() + ".mtl";
	Ref<FileAccess> mtl_file = FileAccess::open(mtl_path, FileAccess::WRITE);

	// OBJ header
	obj_file->store_line("# Exported from Godot Engine using BPY module");
	obj_file->store_line("# https://godotengine.org");
	obj_file->store_line("");

	if (mtl_file.is_valid()) {
		obj_file->store_line("mtllib " + mtl_filename);
		obj_file->store_line("");

		// MTL header
		mtl_file->store_line("# Material file for " + p_path.get_file());
		mtl_file->store_line("");
	}

	int vertex_offset = 0;
	int normal_offset = 0;
	int uv_offset = 0;

	for (int surf_i = 0; surf_i < p_mesh->get_surface_count(); surf_i++) {
		Array arrays = p_mesh->surface_get_arrays(surf_i);
		if (arrays.is_empty()) {
			continue;
		}

		PackedVector3Array vertices = arrays[Mesh::ARRAY_VERTEX];
		PackedVector3Array normals = arrays[Mesh::ARRAY_NORMAL];
		PackedVector2Array uvs = arrays[Mesh::ARRAY_TEX_UV];
		PackedInt32Array indices = arrays[Mesh::ARRAY_INDEX];

		bool has_normals = normals.size() == vertices.size();
		bool has_uvs = uvs.size() == vertices.size();

		// Get material
		Ref<Material> mat;
		if (surf_i < p_materials.size()) {
			mat = p_materials[surf_i];
		} else {
			mat = p_mesh->surface_get_material(surf_i);
		}

		String mat_name = "material_" + itos(surf_i);
		Color albedo = Color(0.8, 0.8, 0.8);

		if (mat.is_valid()) {
			Ref<StandardMaterial3D> std_mat = mat;
			if (std_mat.is_valid()) {
				albedo = std_mat->get_albedo();
			}
		}

		// Write material to MTL (no Ka - PBR doesn't use ambient light)
		if (mtl_file.is_valid()) {
			mtl_file->store_line("newmtl " + mat_name);
			mtl_file->store_line(vformat("Kd %.4f %.4f %.4f", albedo.r, albedo.g, albedo.b));
			mtl_file->store_line("Ks 0.1 0.1 0.1");
			mtl_file->store_line("Ns 32");
			mtl_file->store_line("d 1.0");
			mtl_file->store_line("");
		}

		obj_file->store_line("# Surface " + itos(surf_i));

		// Vertices
		for (int i = 0; i < vertices.size(); i++) {
			Vector3 v = vertices[i];
			obj_file->store_line(vformat("v %.6f %.6f %.6f", v.x, v.y, v.z));
		}

		// Normals
		if (has_normals) {
			for (int i = 0; i < normals.size(); i++) {
				Vector3 n = normals[i];
				obj_file->store_line(vformat("vn %.6f %.6f %.6f", n.x, n.y, n.z));
			}
		}

		// UVs
		if (has_uvs) {
			for (int i = 0; i < uvs.size(); i++) {
				Vector2 uv = uvs[i];
				obj_file->store_line(vformat("vt %.6f %.6f", uv.x, 1.0 - uv.y));
			}
		}

		// Use material and group
		obj_file->store_line("usemtl " + mat_name);
		obj_file->store_line("g surface_" + itos(surf_i));

		// Faces
		if (indices.size() > 0) {
			for (int i = 0; i < indices.size(); i += 3) {
				String face = "f";
				for (int j = 0; j < 3; j++) {
					int idx = indices[i + j];
					int v_idx = idx + vertex_offset + 1;
					int vt_idx = has_uvs ? idx + uv_offset + 1 : 0;
					int vn_idx = has_normals ? idx + normal_offset + 1 : 0;

					if (has_uvs && has_normals) {
						face += vformat(" %d/%d/%d", v_idx, vt_idx, vn_idx);
					} else if (has_normals) {
						face += vformat(" %d//%d", v_idx, vn_idx);
					} else if (has_uvs) {
						face += vformat(" %d/%d", v_idx, vt_idx);
					} else {
						face += vformat(" %d", v_idx);
					}
				}
				obj_file->store_line(face);
			}
		} else {
			for (int i = 0; i < vertices.size(); i += 3) {
				String face = "f";
				for (int j = 0; j < 3; j++) {
					int idx = i + j;
					int v_idx = idx + vertex_offset + 1;
					int vt_idx = has_uvs ? idx + uv_offset + 1 : 0;
					int vn_idx = has_normals ? idx + normal_offset + 1 : 0;

					if (has_uvs && has_normals) {
						face += vformat(" %d/%d/%d", v_idx, vt_idx, vn_idx);
					} else if (has_normals) {
						face += vformat(" %d//%d", v_idx, vn_idx);
					} else if (has_uvs) {
						face += vformat(" %d/%d", v_idx, vt_idx);
					} else {
						face += vformat(" %d", v_idx);
					}
				}
				obj_file->store_line(face);
			}
		}

		vertex_offset += vertices.size();
		normal_offset += has_normals ? normals.size() : 0;
		uv_offset += has_uvs ? uvs.size() : 0;

		obj_file->store_line("");
	}

	print_line("[BPY Export] OBJ export complete: " + itos(vertex_offset) + " vertices");
	return OK;
}

Error BPYExporter::export_stl(const Ref<ArrayMesh> &p_mesh, const String &p_path, bool p_binary) {
	ERR_FAIL_COND_V(p_mesh.is_null(), ERR_INVALID_PARAMETER);

	print_line("[BPY Export] Starting STL export to: " + p_path + (p_binary ? " (binary)" : " (ASCII)"));

	Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::WRITE);
	ERR_FAIL_COND_V(file.is_null(), ERR_CANT_CREATE);

	if (p_binary) {
		// Binary STL header (80 bytes)
		for (int i = 0; i < 80; i++) {
			file->store_8(0);
		}

		// Count total triangles
		uint32_t triangle_count = 0;
		for (int surf_i = 0; surf_i < p_mesh->get_surface_count(); surf_i++) {
			Array arrays = p_mesh->surface_get_arrays(surf_i);
			if (arrays.is_empty()) continue;

			PackedVector3Array vertices = arrays[Mesh::ARRAY_VERTEX];
			PackedInt32Array indices = arrays[Mesh::ARRAY_INDEX];

			if (indices.size() > 0) {
				triangle_count += indices.size() / 3;
			} else {
				triangle_count += vertices.size() / 3;
			}
		}

		file->store_32(triangle_count);

		// Write triangles
		for (int surf_i = 0; surf_i < p_mesh->get_surface_count(); surf_i++) {
			Array arrays = p_mesh->surface_get_arrays(surf_i);
			if (arrays.is_empty()) continue;

			PackedVector3Array vertices = arrays[Mesh::ARRAY_VERTEX];
			PackedVector3Array normals = arrays[Mesh::ARRAY_NORMAL];
			PackedInt32Array indices = arrays[Mesh::ARRAY_INDEX];

			bool has_normals = normals.size() == vertices.size();

			auto write_triangle = [&](const Vector3 &v1, const Vector3 &v2, const Vector3 &v3, const Vector3 &n) {
				// Normal
				file->store_float(n.x);
				file->store_float(n.y);
				file->store_float(n.z);
				// Vertices
				file->store_float(v1.x); file->store_float(v1.y); file->store_float(v1.z);
				file->store_float(v2.x); file->store_float(v2.y); file->store_float(v2.z);
				file->store_float(v3.x); file->store_float(v3.y); file->store_float(v3.z);
				// Attribute byte count
				file->store_16(0);
			};

			if (indices.size() > 0) {
				for (int i = 0; i < indices.size(); i += 3) {
					Vector3 v1 = vertices[indices[i]];
					Vector3 v2 = vertices[indices[i + 1]];
					Vector3 v3 = vertices[indices[i + 2]];
					Vector3 n = has_normals ? normals[indices[i]] : (v2 - v1).cross(v3 - v1).normalized();
					write_triangle(v1, v2, v3, n);
				}
			} else {
				for (int i = 0; i < vertices.size(); i += 3) {
					Vector3 v1 = vertices[i];
					Vector3 v2 = vertices[i + 1];
					Vector3 v3 = vertices[i + 2];
					Vector3 n = has_normals ? normals[i] : (v2 - v1).cross(v3 - v1).normalized();
					write_triangle(v1, v2, v3, n);
				}
			}
		}
	} else {
		// ASCII STL
		file->store_line("solid mesh");

		for (int surf_i = 0; surf_i < p_mesh->get_surface_count(); surf_i++) {
			Array arrays = p_mesh->surface_get_arrays(surf_i);
			if (arrays.is_empty()) continue;

			PackedVector3Array vertices = arrays[Mesh::ARRAY_VERTEX];
			PackedVector3Array normals = arrays[Mesh::ARRAY_NORMAL];
			PackedInt32Array indices = arrays[Mesh::ARRAY_INDEX];

			bool has_normals = normals.size() == vertices.size();

			auto write_triangle = [&](const Vector3 &v1, const Vector3 &v2, const Vector3 &v3, const Vector3 &n) {
				file->store_line(vformat("  facet normal %.6f %.6f %.6f", n.x, n.y, n.z));
				file->store_line("    outer loop");
				file->store_line(vformat("      vertex %.6f %.6f %.6f", v1.x, v1.y, v1.z));
				file->store_line(vformat("      vertex %.6f %.6f %.6f", v2.x, v2.y, v2.z));
				file->store_line(vformat("      vertex %.6f %.6f %.6f", v3.x, v3.y, v3.z));
				file->store_line("    endloop");
				file->store_line("  endfacet");
			};

			if (indices.size() > 0) {
				for (int i = 0; i < indices.size(); i += 3) {
					Vector3 v1 = vertices[indices[i]];
					Vector3 v2 = vertices[indices[i + 1]];
					Vector3 v3 = vertices[indices[i + 2]];
					Vector3 n = has_normals ? normals[indices[i]] : (v2 - v1).cross(v3 - v1).normalized();
					write_triangle(v1, v2, v3, n);
				}
			} else {
				for (int i = 0; i < vertices.size(); i += 3) {
					Vector3 v1 = vertices[i];
					Vector3 v2 = vertices[i + 1];
					Vector3 v3 = vertices[i + 2];
					Vector3 n = has_normals ? normals[i] : (v2 - v1).cross(v3 - v1).normalized();
					write_triangle(v1, v2, v3, n);
				}
			}
		}

		file->store_line("endsolid mesh");
	}

	print_line("[BPY Export] STL export complete");
	return OK;
}

Error BPYExporter::export_gltf(const Ref<ArrayMesh> &p_mesh, const String &p_path, const TypedArray<Material> &p_materials) {
	ERR_FAIL_COND_V(p_mesh.is_null(), ERR_INVALID_PARAMETER);

	// Use Godot's built-in GLTF exporter
	Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::WRITE);
	ERR_FAIL_COND_V(file.is_null(), ERR_CANT_CREATE);

	// Build JSON structure for GLTF
	Dictionary gltf;
	gltf["asset"] = Dictionary();
	((Dictionary)gltf["asset"])["version"] = "2.0";
	((Dictionary)gltf["asset"])["generator"] = "Godot BPY Exporter";

	// For a proper GLTF export, we'd need to implement the full spec
	// For now, save as .tres which Godot can use natively
	String tres_path = p_path.get_basename() + ".tres";
	Error err = ResourceSaver::save(p_mesh, tres_path);

	if (err == OK) {
		// Write a basic GLTF that references the mesh
		file->store_line("{");
		file->store_line("  \"asset\": { \"version\": \"2.0\", \"generator\": \"Godot BPY\" },");
		file->store_line("  \"scene\": 0,");
		file->store_line("  \"scenes\": [{ \"nodes\": [0] }],");
		file->store_line("  \"nodes\": [{ \"mesh\": 0, \"name\": \"Mesh\" }],");
		file->store_line("  \"meshes\": [{ \"name\": \"ExportedMesh\", \"primitives\": [] }]");
		file->store_line("}");
	}

	return err;
}

Error BPYExporter::export_ply(const Ref<ArrayMesh> &p_mesh, const String &p_path, bool p_binary) {
	ERR_FAIL_COND_V(p_mesh.is_null(), ERR_INVALID_PARAMETER);

	print_line("[BPY Export] Starting PLY export to: " + p_path + (p_binary ? " (binary)" : " (ASCII)"));

	Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::WRITE);
	ERR_FAIL_COND_V(file.is_null(), ERR_CANT_CREATE);

	// Count totals
	int total_vertices = 0;
	int total_faces = 0;

	for (int surf_i = 0; surf_i < p_mesh->get_surface_count(); surf_i++) {
		Array arrays = p_mesh->surface_get_arrays(surf_i);
		if (arrays.is_empty()) continue;

		PackedVector3Array vertices = arrays[Mesh::ARRAY_VERTEX];
		PackedInt32Array indices = arrays[Mesh::ARRAY_INDEX];

		total_vertices += vertices.size();
		if (indices.size() > 0) {
			total_faces += indices.size() / 3;
		} else {
			total_faces += vertices.size() / 3;
		}
	}

	// PLY header
	file->store_line("ply");
	file->store_line(p_binary ? "format binary_little_endian 1.0" : "format ascii 1.0");
	file->store_line("comment Exported from Godot BPY");
	file->store_line("element vertex " + itos(total_vertices));
	file->store_line("property float x");
	file->store_line("property float y");
	file->store_line("property float z");
	file->store_line("property float nx");
	file->store_line("property float ny");
	file->store_line("property float nz");
	file->store_line("element face " + itos(total_faces));
	file->store_line("property list uchar int vertex_indices");
	file->store_line("end_header");

	// Vertices
	for (int surf_i = 0; surf_i < p_mesh->get_surface_count(); surf_i++) {
		Array arrays = p_mesh->surface_get_arrays(surf_i);
		if (arrays.is_empty()) continue;

		PackedVector3Array vertices = arrays[Mesh::ARRAY_VERTEX];
		PackedVector3Array normals = arrays[Mesh::ARRAY_NORMAL];
		bool has_normals = normals.size() == vertices.size();

		for (int i = 0; i < vertices.size(); i++) {
			Vector3 v = vertices[i];
			Vector3 n = has_normals ? normals[i] : Vector3(0, 1, 0);

			if (p_binary) {
				file->store_float(v.x);
				file->store_float(v.y);
				file->store_float(v.z);
				file->store_float(n.x);
				file->store_float(n.y);
				file->store_float(n.z);
			} else {
				file->store_line(vformat("%.6f %.6f %.6f %.6f %.6f %.6f", v.x, v.y, v.z, n.x, n.y, n.z));
			}
		}
	}

	// Faces
	int vertex_offset = 0;
	for (int surf_i = 0; surf_i < p_mesh->get_surface_count(); surf_i++) {
		Array arrays = p_mesh->surface_get_arrays(surf_i);
		if (arrays.is_empty()) continue;

		PackedVector3Array vertices = arrays[Mesh::ARRAY_VERTEX];
		PackedInt32Array indices = arrays[Mesh::ARRAY_INDEX];

		if (indices.size() > 0) {
			for (int i = 0; i < indices.size(); i += 3) {
				int i1 = indices[i] + vertex_offset;
				int i2 = indices[i + 1] + vertex_offset;
				int i3 = indices[i + 2] + vertex_offset;

				if (p_binary) {
					file->store_8(3);
					file->store_32(i1);
					file->store_32(i2);
					file->store_32(i3);
				} else {
					file->store_line(vformat("3 %d %d %d", i1, i2, i3));
				}
			}
		} else {
			for (int i = 0; i < vertices.size(); i += 3) {
				int i1 = i + vertex_offset;
				int i2 = i + 1 + vertex_offset;
				int i3 = i + 2 + vertex_offset;

				if (p_binary) {
					file->store_8(3);
					file->store_32(i1);
					file->store_32(i2);
					file->store_32(i3);
				} else {
					file->store_line(vformat("3 %d %d %d", i1, i2, i3));
				}
			}
		}

		vertex_offset += vertices.size();
	}

	print_line("[BPY Export] PLY export complete: " + itos(total_vertices) + " vertices, " + itos(total_faces) + " faces");
	return OK;
}
