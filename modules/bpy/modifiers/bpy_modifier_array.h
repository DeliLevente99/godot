/**************************************************************************/
/*  bpy_modifier_array.h                                                  */
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

#include "bpy_modifier.h"

class BPYModifierArray : public BPYModifier {
	GDCLASS(BPYModifierArray, BPYModifier);

private:
	int count = 2;
	Vector3 relative_offset = Vector3(1, 0, 0);
	Vector3 constant_offset = Vector3(0, 0, 0);
	bool use_relative_offset = true;
	bool use_constant_offset = false;
	bool use_merge_vertices = false;
	float merge_threshold = 0.001f;

protected:
	static void _bind_methods();

public:
	void set_count(int p_count);
	int get_count() const;

	void set_relative_offset(const Vector3 &p_offset);
	Vector3 get_relative_offset() const;

	void set_constant_offset(const Vector3 &p_offset);
	Vector3 get_constant_offset() const;

	void set_use_relative_offset(bool p_enabled);
	bool get_use_relative_offset() const;

	void set_use_constant_offset(bool p_enabled);
	bool get_use_constant_offset() const;

	void set_use_merge_vertices(bool p_enabled);
	bool get_use_merge_vertices() const;

	void set_merge_threshold(float p_threshold);
	float get_merge_threshold() const;

	virtual Ref<BPYMesh> apply(const Ref<BPYMesh> &p_mesh) override;

	BPYModifierArray();
};
