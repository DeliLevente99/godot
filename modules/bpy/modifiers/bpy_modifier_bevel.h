/**************************************************************************/
/*  bpy_modifier_bevel.h                                                  */
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

class BPYModifierBevel : public BPYModifier {
	GDCLASS(BPYModifierBevel, BPYModifier);

public:
	enum LimitMethod {
		LIMIT_NONE,
		LIMIT_ANGLE,
		LIMIT_WEIGHT
	};

private:
	float width = 0.1f;
	int segments = 1;
	LimitMethod limit_method = LIMIT_NONE;
	float angle_limit = Math::deg_to_rad(30.0f);
	bool clamp_overlap = true;

protected:
	static void _bind_methods();

public:
	void set_width(float p_width);
	float get_width() const;

	void set_segments(int p_segments);
	int get_segments() const;

	void set_limit_method(LimitMethod p_method);
	LimitMethod get_limit_method() const;

	void set_angle_limit(float p_angle);
	float get_angle_limit() const;

	void set_clamp_overlap(bool p_clamp);
	bool get_clamp_overlap() const;

	virtual Ref<BPYMesh> apply(const Ref<BPYMesh> &p_mesh) override;

	BPYModifierBevel();
};

VARIANT_ENUM_CAST(BPYModifierBevel::LimitMethod);
