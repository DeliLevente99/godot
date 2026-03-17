/**************************************************************************/
/*  bpy_modifier_subdivision.h                                            */
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

class BPYModifierSubdivision : public BPYModifier {
	GDCLASS(BPYModifierSubdivision, BPYModifier);

public:
	enum SubdivisionType {
		SUBDIVISION_CATMULL_CLARK,
		SUBDIVISION_SIMPLE
	};

private:
	int levels = 1;
	SubdivisionType subdivision_type = SUBDIVISION_CATMULL_CLARK;
	bool use_creases = true;

	Ref<BPYMesh> _subdivide_simple(const Ref<BPYMesh> &p_mesh);
	Ref<BPYMesh> _subdivide_catmull_clark(const Ref<BPYMesh> &p_mesh);

protected:
	static void _bind_methods();

public:
	void set_levels(int p_levels);
	int get_levels() const;

	void set_subdivision_type(SubdivisionType p_type);
	SubdivisionType get_subdivision_type() const;

	void set_use_creases(bool p_enabled);
	bool get_use_creases() const;

	virtual Ref<BPYMesh> apply(const Ref<BPYMesh> &p_mesh) override;

	BPYModifierSubdivision();
};

VARIANT_ENUM_CAST(BPYModifierSubdivision::SubdivisionType);
