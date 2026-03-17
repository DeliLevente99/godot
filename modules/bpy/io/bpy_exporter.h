/**************************************************************************/
/*  bpy_exporter.h                                                        */
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

#ifndef BPY_EXPORTER_H
#define BPY_EXPORTER_H

#include "core/object/class_db.h"
#include "core/object/object.h"
#include "scene/resources/mesh.h"

class BPYExporter : public Object {
	GDCLASS(BPYExporter, Object);

protected:
	static void _bind_methods();

public:
	// Export mesh to OBJ format (with MTL for materials)
	static Error export_obj(const Ref<ArrayMesh> &p_mesh, const String &p_path, const TypedArray<Material> &p_materials = TypedArray<Material>());

	// Export mesh to STL format (for 3D printing)
	static Error export_stl(const Ref<ArrayMesh> &p_mesh, const String &p_path, bool p_binary = false);

	// Export mesh to GLTF format
	static Error export_gltf(const Ref<ArrayMesh> &p_mesh, const String &p_path, const TypedArray<Material> &p_materials = TypedArray<Material>());

	// Export mesh to PLY format
	static Error export_ply(const Ref<ArrayMesh> &p_mesh, const String &p_path, bool p_binary = false);

	BPYExporter() {}
};

#endif // BPY_EXPORTER_H
