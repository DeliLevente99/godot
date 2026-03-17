/**************************************************************************/
/*  bpy_export_dialog.h                                                   */
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

#ifndef BPY_EXPORT_DIALOG_H
#define BPY_EXPORT_DIALOG_H

#include "scene/gui/dialogs.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/label.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/option_button.h"
#include "scene/gui/file_dialog.h"
#include "scene/resources/mesh.h"

class BPYExportDialog : public ConfirmationDialog {
	GDCLASS(BPYExportDialog, ConfirmationDialog);

public:
	enum ExportFormat {
		FORMAT_OBJ,
		FORMAT_STL,
		FORMAT_STL_BINARY,
		FORMAT_PLY,
		FORMAT_PLY_BINARY,
		FORMAT_GLTF,
		FORMAT_TRES,
	};

private:
	Ref<ArrayMesh> mesh;
	TypedArray<Material> materials;

	VBoxContainer *vbox = nullptr;
	OptionButton *format_option = nullptr;
	LineEdit *filename_edit = nullptr;
	Button *browse_button = nullptr;
	FileDialog *file_dialog = nullptr;
	Label *status_label = nullptr;

	void _on_format_changed(int p_index);
	void _on_browse_pressed();
	void _on_file_selected(const String &p_path);
	void _on_confirmed();
	void _do_export();

	String get_extension_for_format(ExportFormat p_format) const;

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	void set_mesh(const Ref<ArrayMesh> &p_mesh);
	Ref<ArrayMesh> get_mesh() const;

	void set_materials(const TypedArray<Material> &p_materials);
	TypedArray<Material> get_materials() const;

	void popup_export(const Ref<ArrayMesh> &p_mesh, const TypedArray<Material> &p_materials = TypedArray<Material>());

	BPYExportDialog();
};

VARIANT_ENUM_CAST(BPYExportDialog::ExportFormat);

#endif // BPY_EXPORT_DIALOG_H
