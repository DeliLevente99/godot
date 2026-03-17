extends Node3D

var time: float = 0.0
var light_node: OmniLight3D

func _ready():
	print("=== Creating Lamp ===")

	var metal_dark = Color(0.25, 0.22, 0.2)
	var metal_gold = Color(0.85, 0.7, 0.4)
	var shade_color = Color(0.95, 0.9, 0.8)
	var cord_color = Color(0.15, 0.12, 0.1)

	# === BASE - heavy, decorative ===
	var base_bottom = BPY.create_cylinder(0.25, 0.05, 16, true, true)
	move_mesh(base_bottom, Vector3(0, 0.025, 0))

	var base_mid = BPY.create_cylinder(0.2, 0.08, 16, true, true)
	move_mesh(base_mid, Vector3(0, 0.09, 0))

	var base_top = BPY.create_cylinder(0.15, 0.03, 16, true, true)
	move_mesh(base_top, Vector3(0, 0.145, 0))

	var base = BPYMeshOps.merge_meshes(base_bottom, base_mid)
	base = BPYMeshOps.merge_meshes(base, base_top)

	# === STEM - tall pole ===
	var stem_lower = BPY.create_cylinder(0.04, 0.5, 12, true, true)
	move_mesh(stem_lower, Vector3(0, 0.41, 0))

	# Decorative ball
	var deco_ball = BPY.create_uv_sphere(0.06, 12, 8)
	move_mesh(deco_ball, Vector3(0, 0.7, 0))

	var stem_upper = BPY.create_cylinder(0.035, 0.4, 12, true, true)
	move_mesh(stem_upper, Vector3(0, 0.96, 0))

	var stem = BPYMeshOps.merge_meshes(stem_lower, deco_ball)
	stem = BPYMeshOps.merge_meshes(stem, stem_upper)

	# === LAMP HOLDER ===
	var holder = BPY.create_cylinder(0.05, 0.06, 12, true, true)
	move_mesh(holder, Vector3(0, 1.19, 0))

	# === SHADE - classic lamp shade ===
	var shade = BPY.create_cone(0.3, 0.25, 24, false)
	# Flip and hollow it out
	for i in range(shade.get_vertex_count()):
		var pos = shade.get_vertex_position(i)
		pos.y = -pos.y  # Flip
		# Make it more like a truncated cone (lampshade shape)
		var t = (pos.y + 0.125) / 0.25
		var scale_factor = 0.6 + t * 0.4
		pos.x *= scale_factor * 1.3
		pos.z *= scale_factor * 1.3
		shade.set_vertex_position(i, pos)
	move_mesh(shade, Vector3(0, 1.35, 0))

	# Shade top rim
	var shade_rim_top = BPY.create_torus(0.15, 0.015, 24, 8)
	move_mesh(shade_rim_top, Vector3(0, 1.22, 0))

	# Shade bottom rim
	var shade_rim_bottom = BPY.create_torus(0.32, 0.015, 24, 8)
	move_mesh(shade_rim_bottom, Vector3(0, 1.48, 0))

	# === POWER CORD ===
	var cord = BPY.create_cylinder(0.015, 0.4, 8, true, true)
	rotate_mesh_x(cord, 80)
	move_mesh(cord, Vector3(-0.15, 0.05, 0.15))

	# === BUILD SCENE ===
	add_child(create_mesh_instance(base, metal_dark))
	add_child(create_mesh_instance(stem, metal_gold))
	add_child(create_mesh_instance(holder, metal_gold))
	add_child(create_mesh_instance(shade, shade_color))
	add_child(create_mesh_instance(shade_rim_top, metal_gold))
	add_child(create_mesh_instance(shade_rim_bottom, metal_gold))
	add_child(create_mesh_instance(cord, cord_color))

	# === ACTUAL LIGHT ===
	light_node = OmniLight3D.new()
	light_node.position = Vector3(0, 1.3, 0)
	light_node.light_color = Color(1.0, 0.95, 0.8)
	light_node.light_energy = 2.0
	light_node.omni_range = 5.0
	add_child(light_node)

	# === CAMERA & ENVIRONMENT ===
	var camera = Camera3D.new()
	camera.position = Vector3(2, 1.5, 2)
	add_child(camera)
	camera.look_at(Vector3(0, 0.8, 0))

	var env = Environment.new()
	env.background_mode = Environment.BG_COLOR
	env.background_color = Color(0.15, 0.12, 0.18)
	env.ambient_light_color = Color(0.3, 0.3, 0.4)
	env.ambient_light_energy = 0.3
	var world_env = WorldEnvironment.new()
	world_env.environment = env
	add_child(world_env)

	# Export menu
	create_export_menu()

	print("=== Lamp Complete! ===")

func _process(delta):
	time += delta

	# Light flicker effect (subtle)
	var flicker = 1.0 + sin(time * 15) * 0.02 + sin(time * 23) * 0.01
	light_node.light_energy = 2.0 * flicker

func rotate_mesh_x(mesh: BPYMesh, degrees: float):
	var rad = deg_to_rad(degrees)
	var c = cos(rad)
	var s = sin(rad)
	for i in range(mesh.get_vertex_count()):
		var pos = mesh.get_vertex_position(i)
		mesh.set_vertex_position(i, Vector3(pos.x, pos.y * c - pos.z * s, pos.y * s + pos.z * c))

var mesh_instances_cache: Array = []

func create_export_menu():
	find_mesh_instances(self, mesh_instances_cache)

	var canvas = CanvasLayer.new()
	add_child(canvas)

	var btn = Button.new()
	btn.text = "Export Model"
	btn.position = Vector2(20, 20)
	btn.pressed.connect(_on_export_button)
	canvas.add_child(btn)

	var dialog = BPYExportDialog.new()
	dialog.name = "ExportDialog"
	add_child(dialog)

func _on_export_button():
	var mesh = get_combined_mesh()
	var dialog = get_node("ExportDialog") as BPYExportDialog
	dialog.popup_export(mesh)

func get_combined_mesh() -> ArrayMesh:
	var combined = ArrayMesh.new()
	for mi in mesh_instances_cache:
		if mi.mesh == null:
			continue
		var mesh: Mesh = mi.mesh
		var transform = mi.global_transform
		for surf_i in range(mesh.get_surface_count()):
			var arrays = mesh.surface_get_arrays(surf_i)
			if arrays.is_empty():
				continue
			var verts: PackedVector3Array = arrays[Mesh.ARRAY_VERTEX]
			var normals = arrays[Mesh.ARRAY_NORMAL]
			var new_verts = PackedVector3Array()
			var new_normals = PackedVector3Array()
			for i in range(verts.size()):
				new_verts.append(transform * verts[i])
				if normals and normals.size() > i:
					new_normals.append(transform.basis * normals[i])
			arrays[Mesh.ARRAY_VERTEX] = new_verts
			if new_normals.size() > 0:
				arrays[Mesh.ARRAY_NORMAL] = new_normals
			combined.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arrays)
			var mat = mi.material_override
			if mat:
				combined.surface_set_material(combined.get_surface_count() - 1, mat)
	return combined

func find_mesh_instances(node: Node, result: Array):
	if node is MeshInstance3D:
		result.append(node)
	for child in node.get_children():
		find_mesh_instances(child, result)

func move_mesh(mesh: BPYMesh, offset: Vector3):
	for i in range(mesh.get_vertex_count()):
		mesh.set_vertex_position(i, mesh.get_vertex_position(i) + offset)

func scale_mesh(mesh: BPYMesh, scale: Vector3):
	for i in range(mesh.get_vertex_count()):
		var pos = mesh.get_vertex_position(i)
		mesh.set_vertex_position(i, pos * scale)

func create_mesh_instance(bpy_mesh: BPYMesh, color: Color) -> MeshInstance3D:
	var instance = MeshInstance3D.new()
	bpy_mesh.recalculate_normals()
	instance.mesh = bpy_mesh.to_array_mesh()

	var material = StandardMaterial3D.new()
	material.albedo_color = color
	material.cull_mode = BaseMaterial3D.CULL_DISABLED
	material.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	instance.material_override = material

	return instance
