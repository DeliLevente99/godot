extends Node3D

var time := 0.0
var fish_root: Node3D
var tail_pivot: Node3D
var body_segments: Array[MeshInstance3D] = []

func _ready():
	create_fish()

func create_fish():
	# Realistic fish colors
	var main_color = Color(0.15, 0.35, 0.55)      # Deep blue
	var belly_color = Color(0.75, 0.8, 0.85)      # Silver belly
	var fin_color = Color(0.1, 0.25, 0.4, 0.9)    # Darker fins
	var accent_color = Color(0.3, 0.5, 0.7)       # Light blue accent

	fish_root = Node3D.new()
	add_child(fish_root)

	# === BODY - Multiple segments for realistic shape ===

	# Main body (center, largest)
	var body_main = BPY.create_uv_sphere(0.4, 20, 14)
	var body_main_mesh = MeshInstance3D.new()
	body_main_mesh.mesh = body_main.to_array_mesh()
	body_main_mesh.scale = Vector3(1.6, 0.9, 0.7)
	body_main_mesh.position = Vector3(0, 0, 0)
	apply_fish_material(body_main_mesh, main_color, belly_color)
	fish_root.add_child(body_main_mesh)
	body_segments.append(body_main_mesh)

	# Front body (tapering towards head)
	var body_front = BPY.create_uv_sphere(0.35, 16, 12)
	var body_front_mesh = MeshInstance3D.new()
	body_front_mesh.mesh = body_front.to_array_mesh()
	body_front_mesh.scale = Vector3(1.2, 0.75, 0.6)
	body_front_mesh.position = Vector3(0.5, 0.02, 0)
	apply_fish_material(body_front_mesh, main_color, belly_color)
	fish_root.add_child(body_front_mesh)
	body_segments.append(body_front_mesh)

	# Head
	var head = BPY.create_uv_sphere(0.28, 16, 12)
	var head_mesh = MeshInstance3D.new()
	head_mesh.mesh = head.to_array_mesh()
	head_mesh.scale = Vector3(1.1, 0.85, 0.7)
	head_mesh.position = Vector3(0.9, 0.03, 0)
	apply_fish_material(head_mesh, main_color, belly_color)
	fish_root.add_child(head_mesh)

	# Snout
	var snout = BPY.create_uv_sphere(0.12, 12, 8)
	var snout_mesh = MeshInstance3D.new()
	snout_mesh.mesh = snout.to_array_mesh()
	snout_mesh.scale = Vector3(1.3, 0.8, 0.7)
	snout_mesh.position = Vector3(1.15, 0.0, 0)
	apply_fish_material(snout_mesh, main_color, belly_color)
	fish_root.add_child(snout_mesh)

	# Back body (tapering towards tail)
	var body_back = BPY.create_uv_sphere(0.3, 14, 10)
	var body_back_mesh = MeshInstance3D.new()
	body_back_mesh.mesh = body_back.to_array_mesh()
	body_back_mesh.scale = Vector3(1.4, 0.65, 0.5)
	body_back_mesh.position = Vector3(-0.55, 0, 0)
	apply_fish_material(body_back_mesh, main_color, belly_color)
	fish_root.add_child(body_back_mesh)
	body_segments.append(body_back_mesh)

	# Tail base (narrow)
	var tail_base = BPY.create_uv_sphere(0.15, 10, 8)
	var tail_base_mesh = MeshInstance3D.new()
	tail_base_mesh.mesh = tail_base.to_array_mesh()
	tail_base_mesh.scale = Vector3(1.5, 0.6, 0.4)
	tail_base_mesh.position = Vector3(-0.95, 0, 0)
	apply_fish_material(tail_base_mesh, main_color, belly_color)
	fish_root.add_child(tail_base_mesh)

	# === TAIL ===
	tail_pivot = Node3D.new()
	tail_pivot.position = Vector3(-0.85, 0, 0)
	fish_root.add_child(tail_pivot)

	# Tail fin - forked shape (two parts)
	for i in [-1, 1]:
		var tail_lobe = BPY.create_cone(0.25, 0.5, 6, false)
		var tail_lobe_mesh = MeshInstance3D.new()
		tail_lobe_mesh.mesh = tail_lobe.to_array_mesh()
		tail_lobe_mesh.position = Vector3(-0.05, i * 0.15, 0)
		tail_lobe_mesh.rotation_degrees = Vector3(0, i * 25, -90 + i * 20)
		tail_lobe_mesh.scale = Vector3(0.7, 1.0, 0.08)
		apply_fin_material(tail_lobe_mesh, fin_color)
		tail_pivot.add_child(tail_lobe_mesh)

	# === DORSAL FIN (top) - moved forward ===
	var dorsal = BPY.create_cone(0.12, 0.4, 6, false)
	var dorsal_mesh = MeshInstance3D.new()
	dorsal_mesh.mesh = dorsal.to_array_mesh()
	dorsal_mesh.position = Vector3(0.2, 0.38, 0)
	dorsal_mesh.rotation_degrees = Vector3(0, 0, 10)
	dorsal_mesh.scale = Vector3(2.0, 1.0, 0.06)
	apply_fin_material(dorsal_mesh, fin_color)
	fish_root.add_child(dorsal_mesh)

	# Dorsal fin support (front)
	var dorsal2 = BPY.create_cone(0.08, 0.25, 5, false)
	var dorsal2_mesh = MeshInstance3D.new()
	dorsal2_mesh.mesh = dorsal2.to_array_mesh()
	dorsal2_mesh.position = Vector3(0.55, 0.32, 0)
	dorsal2_mesh.scale = Vector3(1.0, 1.0, 0.05)
	apply_fin_material(dorsal2_mesh, fin_color)
	fish_root.add_child(dorsal2_mesh)

	# === ANAL FIN (bottom back) ===
	var anal_fin = BPY.create_cone(0.08, 0.2, 5, false)
	var anal_fin_mesh = MeshInstance3D.new()
	anal_fin_mesh.mesh = anal_fin.to_array_mesh()
	anal_fin_mesh.position = Vector3(-0.4, -0.28, 0)
	anal_fin_mesh.rotation_degrees = Vector3(0, 0, 180)
	anal_fin_mesh.scale = Vector3(1.5, 1.0, 0.05)
	apply_fin_material(anal_fin_mesh, fin_color)
	fish_root.add_child(anal_fin_mesh)

	# === PECTORAL FINS (sides) ===
	for side in [-1, 1]:
		var pec_fin = BPY.create_cone(0.1, 0.3, 6, false)
		var pec_fin_mesh = MeshInstance3D.new()
		pec_fin_mesh.mesh = pec_fin.to_array_mesh()
		pec_fin_mesh.position = Vector3(0.45, -0.08, side * 0.22)
		pec_fin_mesh.rotation_degrees = Vector3(side * 20, side * 30, side * 70)
		pec_fin_mesh.scale = Vector3(0.8, 1.2, 0.05)
		apply_fin_material(pec_fin_mesh, fin_color)
		fish_root.add_child(pec_fin_mesh)

	# === PELVIC FINS (bottom front) ===
	for side in [-1, 1]:
		var pel_fin = BPY.create_cone(0.05, 0.15, 5, false)
		var pel_fin_mesh = MeshInstance3D.new()
		pel_fin_mesh.mesh = pel_fin.to_array_mesh()
		pel_fin_mesh.position = Vector3(0.2, -0.25, side * 0.1)
		pel_fin_mesh.rotation_degrees = Vector3(side * 10, side * 20, 160)
		pel_fin_mesh.scale = Vector3(0.6, 1.0, 0.04)
		apply_fin_material(pel_fin_mesh, fin_color)
		fish_root.add_child(pel_fin_mesh)

	# === EYES ===
	for side in [-1, 1]:
		# Eye socket (slight indent)
		var socket = BPY.create_uv_sphere(0.07, 10, 8)
		var socket_mesh = MeshInstance3D.new()
		socket_mesh.mesh = socket.to_array_mesh()
		socket_mesh.position = Vector3(0.95, 0.08, side * 0.17)
		var socket_mat = StandardMaterial3D.new()
		socket_mat.albedo_color = Color(0.1, 0.2, 0.3)
		socket_mat.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
		socket_mesh.set_surface_override_material(0, socket_mat)
		fish_root.add_child(socket_mesh)

		# Eye
		var eye = BPY.create_uv_sphere(0.055, 10, 8)
		var eye_mesh = MeshInstance3D.new()
		eye_mesh.mesh = eye.to_array_mesh()
		eye_mesh.position = Vector3(0.97, 0.08, side * 0.2)
		var eye_mat = StandardMaterial3D.new()
		eye_mat.albedo_color = Color(0.9, 0.85, 0.7)  # Yellowish
		eye_mat.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
		eye_mesh.set_surface_override_material(0, eye_mat)
		fish_root.add_child(eye_mesh)

		# Pupil
		var pupil = BPY.create_uv_sphere(0.03, 8, 6)
		var pupil_mesh = MeshInstance3D.new()
		pupil_mesh.mesh = pupil.to_array_mesh()
		pupil_mesh.position = Vector3(1.0, 0.08, side * 0.23)
		var pupil_mat = StandardMaterial3D.new()
		pupil_mat.albedo_color = Color(0.02, 0.02, 0.02)
		pupil_mat.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
		pupil_mesh.set_surface_override_material(0, pupil_mat)
		fish_root.add_child(pupil_mesh)

	# === GILLS ===
	for side in [-1, 1]:
		for i in range(3):
			var gill = BPY.create_cube(Vector3(0.08, 0.06, 0.01))
			var gill_mesh = MeshInstance3D.new()
			gill_mesh.mesh = gill.to_array_mesh()
			gill_mesh.position = Vector3(0.65 - i * 0.06, -0.02 - i * 0.03, side * 0.23)
			gill_mesh.rotation_degrees = Vector3(0, side * 15, -20)
			var gill_mat = StandardMaterial3D.new()
			gill_mat.albedo_color = Color(0.6, 0.2, 0.2)
			gill_mat.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
			gill_mesh.set_surface_override_material(0, gill_mat)
			fish_root.add_child(gill_mesh)

	# === MOUTH ===
	var mouth = BPY.create_uv_sphere(0.04, 8, 6)
	var mouth_mesh = MeshInstance3D.new()
	mouth_mesh.mesh = mouth.to_array_mesh()
	mouth_mesh.position = Vector3(1.25, -0.02, 0)
	mouth_mesh.scale = Vector3(0.5, 0.6, 1.2)
	var mouth_mat = StandardMaterial3D.new()
	mouth_mat.albedo_color = Color(0.4, 0.15, 0.15)
	mouth_mat.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	mouth_mesh.set_surface_override_material(0, mouth_mat)
	fish_root.add_child(mouth_mesh)

	# === LATERAL LINE (stripe) ===
	for i in range(8):
		var scale_dot = BPY.create_uv_sphere(0.015, 6, 4)
		var scale_mesh = MeshInstance3D.new()
		scale_mesh.mesh = scale_dot.to_array_mesh()
		scale_mesh.position = Vector3(0.6 - i * 0.18, 0.05, 0.28)
		scale_mesh.scale = Vector3(1.5, 0.8, 0.3)
		var scale_mat = StandardMaterial3D.new()
		scale_mat.albedo_color = accent_color
		scale_mat.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
		scale_mesh.set_surface_override_material(0, scale_mat)
		fish_root.add_child(scale_mesh)

		# Other side
		var scale_mesh2 = scale_mesh.duplicate()
		scale_mesh2.position.z = -0.28
		fish_root.add_child(scale_mesh2)

	# Camera
	var camera = Camera3D.new()
	camera.position = Vector3(0, 0.8, 2.5)
	camera.rotation_degrees = Vector3(-15, 0, 0)
	add_child(camera)

func apply_fish_material(mesh: MeshInstance3D, top_color: Color, bottom_color: Color):
	var mat = StandardMaterial3D.new()
	mat.albedo_color = top_color
	mat.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	mesh.set_surface_override_material(0, mat)

func apply_fin_material(mesh: MeshInstance3D, color: Color):
	var mat = StandardMaterial3D.new()
	mat.albedo_color = color
	mat.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	mat.cull_mode = BaseMaterial3D.CULL_DISABLED
	mat.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
	mesh.set_surface_override_material(0, mat)

func _process(delta):
	time += delta
	var swim_speed = 3.0

	# Realistic swimming - sinusoidal body wave
	if tail_pivot:
		tail_pivot.rotation_degrees.y = sin(time * swim_speed) * 25

	# Subtle body movement
	for i in range(body_segments.size()):
		var phase = i * 0.3
		body_segments[i].rotation_degrees.y = sin(time * swim_speed + phase) * (2 + i * 1.5)

	# Whole fish gentle movement
	if fish_root:
		fish_root.position.y = sin(time * swim_speed * 0.4) * 0.03
		fish_root.rotation_degrees.x = sin(time * swim_speed * 0.3) * 2
