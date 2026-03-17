/**************************************************************************/
/*  bpy_editor_plugin.h                                                   */
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

#ifndef BPY_EDITOR_PLUGIN_H
#define BPY_EDITOR_PLUGIN_H

#ifdef TOOLS_ENABLED

#include "editor/plugins/editor_plugin.h"
#include "editor/gui/editor_file_dialog.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/option_button.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/label.h"
#include "scene/resources/mesh.h"

class BPYEditorExportDialog : public ConfirmationDialog {
	GDCLASS(BPYEditorExportDialog, ConfirmationDialog);

private:
	Ref<Mesh> mesh;

	OptionButton *format_option = nullptr;
	LineEdit *filename_edit = nullptr;
	Button *browse_button = nullptr;
	EditorFileDialog *file_dialog = nullptr;
	Label *status_label = nullptr;

	void _on_format_changed(int p_index);
	void _on_browse_pressed();
	void _on_file_selected(const String &p_path);
	void _on_confirmed();

	Ref<ArrayMesh> _convert_to_array_mesh(const Ref<Mesh> &p_mesh);

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	void set_mesh(const Ref<Mesh> &p_mesh);
	void popup_export(const Ref<Mesh> &p_mesh, const String &p_name = "exported_mesh");

	BPYEditorExportDialog();
};

class BPYEditorPlugin : public EditorPlugin {
	GDCLASS(BPYEditorPlugin, EditorPlugin);

private:
	Button *toolbar_button = nullptr;
	BPYEditorExportDialog *export_dialog = nullptr;
	PopupMenu *bpy_menu = nullptr;

	void _on_toolbar_pressed();
	void _on_menu_id_pressed(int p_id);
	void _export_selected_mesh();

protected:
	static void _bind_methods();

public:
	BPYEditorPlugin();
	~BPYEditorPlugin();
};

#endif // TOOLS_ENABLED

#endif // BPY_EDITOR_PLUGIN_H
