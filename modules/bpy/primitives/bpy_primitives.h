/**************************************************************************/
/*  bpy_primitives.h                                                      */
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

#include "core/object/object.h"

class BPY : public Object {
	GDCLASS(BPY, Object);

protected:
	static void _bind_methods();

public:
	// Basic primitives
	Ref<BPYMesh> create_cube(const Vector3 &p_size = Vector3(1, 1, 1));
	Ref<BPYMesh> create_uv_sphere(float p_radius = 1.0f, int p_segments = 32, int p_rings = 16);
	Ref<BPYMesh> create_ico_sphere(float p_radius = 1.0f, int p_subdivisions = 2);
	Ref<BPYMesh> create_cylinder(float p_radius = 1.0f, float p_height = 2.0f, int p_segments = 32, bool p_cap_top = true, bool p_cap_bottom = true);
	Ref<BPYMesh> create_cone(float p_radius = 1.0f, float p_height = 2.0f, int p_segments = 32, bool p_cap = true);
	Ref<BPYMesh> create_torus(float p_major_radius = 1.0f, float p_minor_radius = 0.25f, int p_major_segments = 48, int p_minor_segments = 12);
	Ref<BPYMesh> create_plane(const Vector2 &p_size = Vector2(2, 2), const Vector2i &p_subdivisions = Vector2i(1, 1));
	Ref<BPYMesh> create_circle(float p_radius = 1.0f, int p_segments = 32, bool p_fill = true);
	Ref<BPYMesh> create_grid(const Vector2 &p_size = Vector2(2, 2), const Vector2i &p_subdivisions = Vector2i(10, 10));

	BPY();
	~BPY();
};
