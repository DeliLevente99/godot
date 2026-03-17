/**************************************************************************/
/*  bpy_modifier_solidify.h                                               */
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

class BPYModifierSolidify : public BPYModifier {
	GDCLASS(BPYModifierSolidify, BPYModifier);

private:
	float thickness = 0.1f;
	float offset = -1.0f; // -1 = inward, 0 = centered, 1 = outward
	bool use_even_offset = false;
	bool use_rim = true;
	int rim_material_index = 0;

protected:
	static void _bind_methods();

public:
	void set_thickness(float p_thickness);
	float get_thickness() const;

	void set_offset(float p_offset);
	float get_offset() const;

	void set_use_even_offset(bool p_enabled);
	bool get_use_even_offset() const;

	void set_use_rim(bool p_enabled);
	bool get_use_rim() const;

	void set_rim_material_index(int p_index);
	int get_rim_material_index() const;

	virtual Ref<BPYMesh> apply(const Ref<BPYMesh> &p_mesh) override;

	BPYModifierSolidify();
};
