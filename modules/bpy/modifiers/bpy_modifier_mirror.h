/**************************************************************************/
/*  bpy_modifier_mirror.h                                                 */
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

class BPYModifierMirror : public BPYModifier {
	GDCLASS(BPYModifierMirror, BPYModifier);

private:
	bool axis_x = true;
	bool axis_y = false;
	bool axis_z = false;
	float merge_threshold = 0.001f;
	bool use_mirror_merge = true;
	bool flip_uvs_u = false;
	bool flip_uvs_v = false;

protected:
	static void _bind_methods();

public:
	void set_axis_x(bool p_enabled);
	bool get_axis_x() const;
	void set_axis_y(bool p_enabled);
	bool get_axis_y() const;
	void set_axis_z(bool p_enabled);
	bool get_axis_z() const;

	void set_merge_threshold(float p_threshold);
	float get_merge_threshold() const;
	void set_use_mirror_merge(bool p_enabled);
	bool get_use_mirror_merge() const;

	void set_flip_uvs_u(bool p_flip);
	bool get_flip_uvs_u() const;
	void set_flip_uvs_v(bool p_flip);
	bool get_flip_uvs_v() const;

	virtual Ref<BPYMesh> apply(const Ref<BPYMesh> &p_mesh) override;

	BPYModifierMirror();
};
