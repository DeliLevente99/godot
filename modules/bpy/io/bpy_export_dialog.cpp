/**************************************************************************/
/*  bpy_export_dialog.cpp                                                 */
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

#include "bpy_export_dialog.h"
#include "bpy_exporter.h"

#include "core/io/resource_saver.h"
#include "core/object/callable_mp.h"
#include "scene/gui/separator.h"

void BPYExportDialog::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_mesh", "mesh"), &BPYExportDialog::set_mesh);
	ClassDB::bind_method(D_METHOD("get_mesh"), &BPYExportDialog::get_mesh);
	ClassDB::bind_method(D_METHOD("set_materials", "materials"), &BPYExportDialog::set_materials);
	ClassDB::bind_method(D_METHOD("get_materials"), &BPYExportDialog::get_materials);
	ClassDB::bind_method(D_METHOD("popup_export", "mesh", "materials"), &BPYExportDialog::popup_export, DEFVAL(TypedArray<Material>()));

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "mesh", PROPERTY_HINT_RESOURCE_TYPE, "ArrayMesh"), "set_mesh", "get_mesh");

	BIND_ENUM_CONSTANT(FORMAT_OBJ);
	BIND_ENUM_CONSTANT(FORMAT_STL);
	BIND_ENUM_CONSTANT(FORMAT_STL_BINARY);
	BIND_ENUM_CONSTANT(FORMAT_PLY);
	BIND_ENUM_CONSTANT(FORMAT_PLY_BINARY);
	BIND_ENUM_CONSTANT(FORMAT_GLTF);
	BIND_ENUM_CONSTANT(FORMAT_TRES);
}

void BPYExportDialog::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			connect("confirmed", callable_mp(this, &BPYExportDialog::_on_confirmed));
		} break;
	}
}

BPYExportDialog::BPYExportDialog() {
	set_title("Export Mesh");
	set_ok_button_text("Export");
	set_min_size(Size2(400, 200));

	vbox = memnew(VBoxContainer);
	add_child(vbox);

	// Format selection
	HBoxContainer *format_hbox = memnew(HBoxContainer);
	vbox->add_child(format_hbox);

	Label *format_label = memnew(Label);
	format_label->set_text("Format:");
	format_label->set_custom_minimum_size(Size2(80, 0));
	format_hbox->add_child(format_label);

	format_option = memnew(OptionButton);
	format_option->add_item("OBJ + MTL (Blender)", FORMAT_OBJ);
	format_option->add_item("STL ASCII (3D Print)", FORMAT_STL);
	format_option->add_item("STL Binary (3D Print)", FORMAT_STL_BINARY);
	format_option->add_item("PLY ASCII (MeshLab)", FORMAT_PLY);
	format_option->add_item("PLY Binary (MeshLab)", FORMAT_PLY_BINARY);
	format_option->add_item("GLTF (Unity/Unreal)", FORMAT_GLTF);
	format_option->add_item("TRES (Godot)", FORMAT_TRES);
	format_option->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	format_option->connect("item_selected", callable_mp(this, &BPYExportDialog::_on_format_changed));
	format_hbox->add_child(format_option);

	// Separator
	vbox->add_child(memnew(HSeparator));

	// Filename
	HBoxContainer *file_hbox = memnew(HBoxContainer);
	vbox->add_child(file_hbox);

	Label *file_label = memnew(Label);
	file_label->set_text("File:");
	file_label->set_custom_minimum_size(Size2(80, 0));
	file_hbox->add_child(file_label);

	filename_edit = memnew(LineEdit);
	filename_edit->set_text("res://exported_mesh.obj");
	filename_edit->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	file_hbox->add_child(filename_edit);

	browse_button = memnew(Button);
	browse_button->set_text("...");
	browse_button->connect("pressed", callable_mp(this, &BPYExportDialog::_on_browse_pressed));
	file_hbox->add_child(browse_button);

	// Status label
	status_label = memnew(Label);
	status_label->set_text("");
	status_label->add_theme_color_override("font_color", Color(0.5, 1.0, 0.5));
	vbox->add_child(status_label);

	// File dialog
	file_dialog = memnew(FileDialog);
	file_dialog->set_file_mode(FileDialog::FILE_MODE_SAVE_FILE);
	file_dialog->set_access(FileDialog::ACCESS_RESOURCES);
	file_dialog->add_filter("*.obj", "OBJ Files");
	file_dialog->connect("file_selected", callable_mp(this, &BPYExportDialog::_on_file_selected));
	add_child(file_dialog);
}

void BPYExportDialog::_on_format_changed(int p_index) {
	ExportFormat format = (ExportFormat)format_option->get_item_id(p_index);
	String ext = get_extension_for_format(format);

	String current = filename_edit->get_text();
	String base = current.get_basename();
	filename_edit->set_text(base + "." + ext);

	// Update file dialog filters
	file_dialog->clear_filters();
	switch (format) {
		case FORMAT_OBJ:
			file_dialog->add_filter("*.obj", "OBJ Files");
			break;
		case FORMAT_STL:
		case FORMAT_STL_BINARY:
			file_dialog->add_filter("*.stl", "STL Files");
			break;
		case FORMAT_PLY:
		case FORMAT_PLY_BINARY:
			file_dialog->add_filter("*.ply", "PLY Files");
			break;
		case FORMAT_GLTF:
			file_dialog->add_filter("*.gltf", "GLTF Files");
			break;
		case FORMAT_TRES:
			file_dialog->add_filter("*.tres", "Godot Resource Files");
			break;
	}
}

void BPYExportDialog::_on_browse_pressed() {
	file_dialog->popup_centered_ratio(0.7);
}

void BPYExportDialog::_on_file_selected(const String &p_path) {
	filename_edit->set_text(p_path);
}

void BPYExportDialog::_on_confirmed() {
	_do_export();
}

void BPYExportDialog::_do_export() {
	if (mesh.is_null()) {
		status_label->add_theme_color_override("font_color", Color(1.0, 0.5, 0.5));
		status_label->set_text("Error: No mesh set!");
		return;
	}

	String path = filename_edit->get_text();
	ExportFormat format = (ExportFormat)format_option->get_selected_id();
	Error err = OK;

	switch (format) {
		case FORMAT_OBJ:
			err = BPYExporter::export_obj(mesh, path, materials);
			break;
		case FORMAT_STL:
			err = BPYExporter::export_stl(mesh, path, false);
			break;
		case FORMAT_STL_BINARY:
			err = BPYExporter::export_stl(mesh, path, true);
			break;
		case FORMAT_PLY:
			err = BPYExporter::export_ply(mesh, path, false);
			break;
		case FORMAT_PLY_BINARY:
			err = BPYExporter::export_ply(mesh, path, true);
			break;
		case FORMAT_GLTF:
			err = BPYExporter::export_gltf(mesh, path, materials);
			break;
		case FORMAT_TRES:
			err = ResourceSaver::save(mesh, path);
			break;
	}

	if (err == OK) {
		status_label->add_theme_color_override("font_color", Color(0.5, 1.0, 0.5));
		status_label->set_text("Exported: " + path);
	} else {
		status_label->add_theme_color_override("font_color", Color(1.0, 0.5, 0.5));
		status_label->set_text("Export failed! Error: " + itos(err));
	}
}

String BPYExportDialog::get_extension_for_format(ExportFormat p_format) const {
	switch (p_format) {
		case FORMAT_OBJ: return "obj";
		case FORMAT_STL:
		case FORMAT_STL_BINARY: return "stl";
		case FORMAT_PLY:
		case FORMAT_PLY_BINARY: return "ply";
		case FORMAT_GLTF: return "gltf";
		case FORMAT_TRES: return "tres";
		default: return "obj";
	}
}

void BPYExportDialog::set_mesh(const Ref<ArrayMesh> &p_mesh) {
	mesh = p_mesh;
}

Ref<ArrayMesh> BPYExportDialog::get_mesh() const {
	return mesh;
}

void BPYExportDialog::set_materials(const TypedArray<Material> &p_materials) {
	materials = p_materials;
}

TypedArray<Material> BPYExportDialog::get_materials() const {
	return materials;
}

void BPYExportDialog::popup_export(const Ref<ArrayMesh> &p_mesh, const TypedArray<Material> &p_materials) {
	mesh = p_mesh;
	materials = p_materials;
	status_label->set_text("");
	popup_centered();
}
