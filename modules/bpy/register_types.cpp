/**************************************************************************/
/*  register_types.cpp                                                    */
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

#include "register_types.h"

#include "core/bpy_mesh.h"
#include "modifiers/bpy_modifier.h"
#include "modifiers/bpy_modifier_array.h"
#include "modifiers/bpy_modifier_bevel.h"
#include "modifiers/bpy_modifier_boolean.h"
#include "modifiers/bpy_modifier_decimate.h"
#include "modifiers/bpy_modifier_mirror.h"
#include "modifiers/bpy_modifier_solidify.h"
#include "modifiers/bpy_modifier_subdivision.h"
#include "ops/bpy_mesh_ops.h"
#include "primitives/bpy_primitives.h"
#include "io/bpy_exporter.h"
#include "io/bpy_export_dialog.h"

#ifdef TOOLS_ENABLED
#include "editor/bpy_editor_plugin.h"
#include "editor/plugins/editor_plugin.h"
#endif

#include "core/config/engine.h"
#include "core/object/class_db.h"

static BPY *bpy_singleton = nullptr;

void initialize_bpy_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		// Core
		GDREGISTER_CLASS(BPYMesh);

		// Primitives singleton
		GDREGISTER_CLASS(BPY);
		bpy_singleton = memnew(BPY);
		Engine::get_singleton()->add_singleton(Engine::Singleton("BPY", bpy_singleton));

		// Modifiers
		GDREGISTER_ABSTRACT_CLASS(BPYModifier);
		GDREGISTER_CLASS(BPYModifierSubdivision);
		GDREGISTER_CLASS(BPYModifierBoolean);
		GDREGISTER_CLASS(BPYModifierArray);
		GDREGISTER_CLASS(BPYModifierMirror);
		GDREGISTER_CLASS(BPYModifierBevel);
		GDREGISTER_CLASS(BPYModifierSolidify);
		GDREGISTER_CLASS(BPYModifierDecimate);

		// Mesh operations
		GDREGISTER_CLASS(BPYMeshOps);

		// Exporter
		GDREGISTER_CLASS(BPYExporter);
		GDREGISTER_CLASS(BPYExportDialog);
	}

#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		GDREGISTER_CLASS(BPYEditorExportDialog);
		GDREGISTER_CLASS(BPYEditorPlugin);
		EditorPlugins::add_by_type<BPYEditorPlugin>();
	}
#endif
}

void uninitialize_bpy_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	if (bpy_singleton) {
		Engine::get_singleton()->remove_singleton("BPY");
		memdelete(bpy_singleton);
		bpy_singleton = nullptr;
	}
}
