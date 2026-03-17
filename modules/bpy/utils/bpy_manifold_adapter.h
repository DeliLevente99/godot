/**************************************************************************/
/*  bpy_manifold_adapter.h                                                */
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

#include "manifold/manifold.h"

class BPYManifoldAdapter {
public:
	// Property indices for Manifold vertex properties
	enum ManifoldProperty {
		MANIFOLD_PROPERTY_POSITION_X = 0,
		MANIFOLD_PROPERTY_POSITION_Y = 1,
		MANIFOLD_PROPERTY_POSITION_Z = 2,
		MANIFOLD_PROPERTY_NORMAL_X = 3,
		MANIFOLD_PROPERTY_NORMAL_Y = 4,
		MANIFOLD_PROPERTY_NORMAL_Z = 5,
		MANIFOLD_PROPERTY_UV_X = 6,
		MANIFOLD_PROPERTY_UV_Y = 7,
		MANIFOLD_PROPERTY_MAX = 8
	};

	static manifold::Manifold bpy_mesh_to_manifold(const Ref<BPYMesh> &p_mesh);
	static Ref<BPYMesh> manifold_to_bpy_mesh(const manifold::Manifold &p_manifold);

	// Boolean operations
	static Ref<BPYMesh> boolean_union(const Ref<BPYMesh> &p_a, const Ref<BPYMesh> &p_b);
	static Ref<BPYMesh> boolean_difference(const Ref<BPYMesh> &p_a, const Ref<BPYMesh> &p_b);
	static Ref<BPYMesh> boolean_intersection(const Ref<BPYMesh> &p_a, const Ref<BPYMesh> &p_b);
};
