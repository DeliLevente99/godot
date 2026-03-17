/**************************************************************************/
/*  bpy_modifier_boolean.h                                                */
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

class BPYModifierBoolean : public BPYModifier {
	GDCLASS(BPYModifierBoolean, BPYModifier);

public:
	enum Operation {
		OPERATION_UNION,
		OPERATION_DIFFERENCE,
		OPERATION_INTERSECTION
	};

private:
	Operation operation = OPERATION_DIFFERENCE;
	Ref<BPYMesh> operand;

protected:
	static void _bind_methods();

public:
	void set_operation(Operation p_operation);
	Operation get_operation() const;

	void set_operand(const Ref<BPYMesh> &p_operand);
	Ref<BPYMesh> get_operand() const;

	virtual Ref<BPYMesh> apply(const Ref<BPYMesh> &p_mesh) override;

	BPYModifierBoolean();
};

VARIANT_ENUM_CAST(BPYModifierBoolean::Operation);
