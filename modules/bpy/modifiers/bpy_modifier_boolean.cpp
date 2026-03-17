/**************************************************************************/
/*  bpy_modifier_boolean.cpp                                              */
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

#include "bpy_modifier_boolean.h"

#include "../utils/bpy_manifold_adapter.h"

#include "core/object/class_db.h"

void BPYModifierBoolean::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_operation", "operation"), &BPYModifierBoolean::set_operation);
	ClassDB::bind_method(D_METHOD("get_operation"), &BPYModifierBoolean::get_operation);
	ClassDB::bind_method(D_METHOD("set_operand", "operand"), &BPYModifierBoolean::set_operand);
	ClassDB::bind_method(D_METHOD("get_operand"), &BPYModifierBoolean::get_operand);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "operation", PROPERTY_HINT_ENUM, "Union,Difference,Intersection"), "set_operation", "get_operation");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "operand", PROPERTY_HINT_RESOURCE_TYPE, "BPYMesh"), "set_operand", "get_operand");

	BIND_ENUM_CONSTANT(OPERATION_UNION);
	BIND_ENUM_CONSTANT(OPERATION_DIFFERENCE);
	BIND_ENUM_CONSTANT(OPERATION_INTERSECTION);
}

void BPYModifierBoolean::set_operation(Operation p_operation) {
	operation = p_operation;
}

BPYModifierBoolean::Operation BPYModifierBoolean::get_operation() const {
	return operation;
}

void BPYModifierBoolean::set_operand(const Ref<BPYMesh> &p_operand) {
	operand = p_operand;
}

Ref<BPYMesh> BPYModifierBoolean::get_operand() const {
	return operand;
}

Ref<BPYMesh> BPYModifierBoolean::apply(const Ref<BPYMesh> &p_mesh) {
	if (!enabled || p_mesh.is_null() || operand.is_null()) {
		return p_mesh;
	}

	switch (operation) {
		case OPERATION_UNION:
			return BPYManifoldAdapter::boolean_union(p_mesh, operand);
		case OPERATION_DIFFERENCE:
			return BPYManifoldAdapter::boolean_difference(p_mesh, operand);
		case OPERATION_INTERSECTION:
			return BPYManifoldAdapter::boolean_intersection(p_mesh, operand);
		default:
			return p_mesh;
	}
}

BPYModifierBoolean::BPYModifierBoolean() {
}
