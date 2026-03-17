/**************************************************************************/
/*  bpy_editor_plugin.cpp                                                 */
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

#ifdef TOOLS_ENABLED

#include "bpy_editor_plugin.h"
#include "../io/bpy_exporter.h"

#include "core/object/callable_mp.h"
#include "core/string/print_string.h"
#include "editor/editor_interface.h"
#include "editor/editor_node.h"
#include "editor/gui/editor_file_dialog.h"
#include "scene/3d/mesh_instance_3d.h"
#include "scene/gui/separator.h"

// =====================================================
// BPYEditorExportDialog
// =====================================================

void BPYEditorExportDialog::_bind_methods() {
}

void BPYEditorExportDialog::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			connect("confirmed", callable_mp(this, &BPYEditorExportDialog::_on_confirmed));
		} break;
	}
}

BPYEditorExportDialog::BPYEditorExportDialog() {
	set_title("BPY - Export Mesh");
	set_ok_button_text("Export");
	set_min_size(Size2(450, 180));

	VBoxContainer *vbox = memnew(VBoxContainer);
	add_child(vbox);

	// Format selection
	HBoxContainer *format_hbox = memnew(HBoxContainer);
	vbox->add_child(format_hbox);

	Label *format_label = memnew(Label);
	format_label->set_text("Format:");
	format_label->set_custom_minimum_size(Size2(100, 0));
	format_hbox->add_child(format_label);

	format_option = memnew(OptionButton);
	format_option->add_item("OBJ + MTL (Blender, Maya)", 0);
	format_option->add_item("STL ASCII (3D Printing)", 1);
	format_option->add_item("STL Binary (3D Printing)", 2);
	format_option->add_item("PLY ASCII (MeshLab)", 3);
	format_option->add_item("PLY Binary (MeshLab)", 4);
	format_option->add_item("Godot Resource (.tres)", 5);
	format_option->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	format_option->connect("item_selected", callable_mp(this, &BPYEditorExportDialog::_on_format_changed));
	format_hbox->add_child(format_option);

	vbox->add_child(memnew(HSeparator));

	// Filename
	HBoxContainer *file_hbox = memnew(HBoxContainer);
	vbox->add_child(file_hbox);

	Label *file_label = memnew(Label);
	file_label->set_text("Save Path:");
	file_label->set_custom_minimum_size(Size2(100, 0));
	file_hbox->add_child(file_label);

	filename_edit = memnew(LineEdit);
	filename_edit->set_text("res://exported_mesh.obj");
	filename_edit->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	file_hbox->add_child(filename_edit);

	browse_button = memnew(Button);
	browse_button->set_text("Browse...");
	browse_button->connect("pressed", callable_mp(this, &BPYEditorExportDialog::_on_browse_pressed));
	file_hbox->add_child(browse_button);

	// Status
	status_label = memnew(Label);
	status_label->set_text("");
	vbox->add_child(status_label);

	// File dialog
	file_dialog = memnew(EditorFileDialog);
	file_dialog->set_file_mode(EditorFileDialog::FILE_MODE_SAVE_FILE);
	file_dialog->set_access(EditorFileDialog::ACCESS_RESOURCES);
	file_dialog->add_filter("*.obj", "OBJ Files");
	file_dialog->connect("file_selected", callable_mp(this, &BPYEditorExportDialog::_on_file_selected));
	add_child(file_dialog);
}

void BPYEditorExportDialog::_on_format_changed(int p_index) {
	String current = filename_edit->get_text();
	String base = current.get_basename();

	file_dialog->clear_filters();

	switch (p_index) {
		case 0: // OBJ
			filename_edit->set_text(base + ".obj");
			file_dialog->add_filter("*.obj", "OBJ Files");
			break;
		case 1: // STL ASCII
		case 2: // STL Binary
			filename_edit->set_text(base + ".stl");
			file_dialog->add_filter("*.stl", "STL Files");
			break;
		case 3: // PLY ASCII
		case 4: // PLY Binary
			filename_edit->set_text(base + ".ply");
			file_dialog->add_filter("*.ply", "PLY Files");
			break;
		case 5: // TRES
			filename_edit->set_text(base + ".tres");
			file_dialog->add_filter("*.tres", "Godot Resource");
			break;
	}
}

void BPYEditorExportDialog::_on_browse_pressed() {
	file_dialog->popup_file_dialog();
}

void BPYEditorExportDialog::_on_file_selected(const String &p_path) {
	filename_edit->set_text(p_path);
}

void BPYEditorExportDialog::_on_confirmed() {
	if (mesh.is_null()) {
		status_label->add_theme_color_override("font_color", Color(1.0, 0.4, 0.4));
		status_label->set_text("Error: No mesh selected!");
		print_line("[BPY Editor] Export failed: No mesh selected");
		return;
	}

	String path = filename_edit->get_text();
	int format = format_option->get_selected();
	Error err = OK;

	String format_name;
	switch (format) {
		case 0: // OBJ
			format_name = "OBJ";
			break;
		case 1: // STL ASCII
			format_name = "STL ASCII";
			break;
		case 2: // STL Binary
			format_name = "STL Binary";
			break;
		case 3: // PLY ASCII
			format_name = "PLY ASCII";
			break;
		case 4: // PLY Binary
			format_name = "PLY Binary";
			break;
		case 5: // TRES
			format_name = "Godot Resource";
			break;
	}

	print_line("[BPY Editor] Exporting mesh as " + format_name + " to: " + path);

	// Convert Mesh to ArrayMesh if needed
	Ref<ArrayMesh> array_mesh = mesh;
	if (array_mesh.is_null()) {
		print_line("[BPY Editor] Converting Mesh to ArrayMesh...");
		array_mesh.instantiate();
		for (int s = 0; s < mesh->get_surface_count(); s++) {
			Array arrays = mesh->surface_get_arrays(s);
			array_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
			array_mesh->surface_set_material(s, mesh->surface_get_material(s));
		}
		print_line("[BPY Editor] Converted " + itos(mesh->get_surface_count()) + " surfaces");
	}

	switch (format) {
		case 0: // OBJ
			err = BPYExporter::export_obj(array_mesh, path);
			break;
		case 1: // STL ASCII
			err = BPYExporter::export_stl(array_mesh, path, false);
			break;
		case 2: // STL Binary
			err = BPYExporter::export_stl(array_mesh, path, true);
			break;
		case 3: // PLY ASCII
			err = BPYExporter::export_ply(array_mesh, path, false);
			break;
		case 4: // PLY Binary
			err = BPYExporter::export_ply(array_mesh, path, true);
			break;
		case 5: // TRES
			err = ResourceSaver::save(array_mesh, path);
			if (err == OK) {
				print_line("[BPY Editor] Saved Godot Resource to: " + path);
			}
			break;
	}

	if (err == OK) {
		status_label->add_theme_color_override("font_color", Color(0.4, 1.0, 0.4));
		status_label->set_text("Success! Exported to: " + path);
		print_line("[BPY Editor] Export successful!");
	} else {
		status_label->add_theme_color_override("font_color", Color(1.0, 0.4, 0.4));
		status_label->set_text("Export failed! Error code: " + itos(err));
		print_line("[BPY Editor] Export failed with error code: " + itos(err));
	}
}

void BPYEditorExportDialog::set_mesh(const Ref<Mesh> &p_mesh) {
	mesh = p_mesh;
}

void BPYEditorExportDialog::popup_export(const Ref<Mesh> &p_mesh, const String &p_name) {
	mesh = p_mesh;
	status_label->set_text("");

	// Set filename based on node name
	String ext = ".obj";
	int format = format_option->get_selected();
	switch (format) {
		case 0: ext = ".obj"; break;
		case 1: case 2: ext = ".stl"; break;
		case 3: case 4: ext = ".ply"; break;
		case 5: ext = ".tres"; break;
	}
	filename_edit->set_text("res://" + p_name + ext);

	popup_centered();
}

// =====================================================
// BPYEditorPlugin
// =====================================================

void BPYEditorPlugin::_bind_methods() {
}

BPYEditorPlugin::BPYEditorPlugin() {
	// Create export dialog
	export_dialog = memnew(BPYEditorExportDialog);
	EditorNode::get_singleton()->get_gui_base()->add_child(export_dialog);

	// Add menu to editor
	bpy_menu = memnew(PopupMenu);
	bpy_menu->add_item("Export Selected Mesh...", 0);
	bpy_menu->add_separator();
	bpy_menu->add_item("Create Cube", 10);
	bpy_menu->add_item("Create Sphere", 11);
	bpy_menu->add_item("Create Cylinder", 12);
	bpy_menu->add_item("Create Cone", 13);
	bpy_menu->add_item("Create Torus", 14);
	bpy_menu->connect("id_pressed", callable_mp(this, &BPYEditorPlugin::_on_menu_id_pressed));

	add_tool_submenu_item("BPY Mesh Tools", bpy_menu);
}

BPYEditorPlugin::~BPYEditorPlugin() {
	remove_tool_menu_item("BPY Mesh Tools");
}

void BPYEditorPlugin::_on_menu_id_pressed(int p_id) {
	switch (p_id) {
		case 0: // Export Selected Mesh
			_export_selected_mesh();
			break;
		case 10: // Create Cube
		case 11: // Create Sphere
		case 12: // Create Cylinder
		case 13: // Create Cone
		case 14: // Create Torus
			// TODO: Implement primitive creation in editor
			break;
	}
}

void BPYEditorPlugin::_export_selected_mesh() {
	print_line("[BPY Editor] Looking for selected mesh...");

	EditorSelection *selection = EditorInterface::get_singleton()->get_selection();
	TypedArray<Node> selected = selection->get_selected_nodes();

	print_line("[BPY Editor] Selected nodes: " + itos(selected.size()));

	if (selected.is_empty()) {
		print_line("[BPY Editor] No nodes selected!");
		export_dialog->set_mesh(Ref<Mesh>());
		export_dialog->popup_centered();
		return;
	}

	// Find first MeshInstance3D in selection
	for (int i = 0; i < selected.size(); i++) {
		Node *node = Object::cast_to<Node>(selected[i]);
		if (node) {
			print_line("[BPY Editor] Checking node: " + node->get_name() + " (type: " + node->get_class() + ")");
		}

		MeshInstance3D *mi = Object::cast_to<MeshInstance3D>(node);
		if (mi) {
			print_line("[BPY Editor] Found MeshInstance3D: " + mi->get_name());

			if (mi->get_mesh().is_valid()) {
				Ref<Mesh> mesh = mi->get_mesh();
				print_line("[BPY Editor] Mesh type: " + mesh->get_class());
				print_line("[BPY Editor] Mesh surfaces: " + itos(mesh->get_surface_count()));

				String node_name = mi->get_name();
				print_line("[BPY Editor] Using node name: " + node_name);

				// Try to cast directly to ArrayMesh
				Ref<ArrayMesh> array_mesh = mesh;
				if (array_mesh.is_valid()) {
					print_line("[BPY Editor] Using ArrayMesh directly");
					export_dialog->popup_export(array_mesh, node_name);
					return;
				}

				// Convert to ArrayMesh if needed (e.g., BoxMesh, SphereMesh, etc.)
				print_line("[BPY Editor] Converting " + mesh->get_class() + " to ArrayMesh...");
				Ref<ArrayMesh> converted;
				converted.instantiate();
				for (int s = 0; s < mesh->get_surface_count(); s++) {
					Array arrays = mesh->surface_get_arrays(s);
					if (arrays.size() > 0) {
						converted->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
						converted->surface_set_material(s, mesh->surface_get_material(s));
					}
				}
				print_line("[BPY Editor] Converted to ArrayMesh with " + itos(converted->get_surface_count()) + " surfaces");
				export_dialog->popup_export(converted, node_name);
				return;
			}
		}
	}

	// No mesh found
	print_line("[BPY Editor] No MeshInstance3D with valid mesh found in selection!");
	export_dialog->set_mesh(Ref<Mesh>());
	export_dialog->popup_centered();
}

void BPYEditorPlugin::_on_toolbar_pressed() {
	_export_selected_mesh();
}

#endif // TOOLS_ENABLED
