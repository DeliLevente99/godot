extends Node3D

func _ready():
	print("=== Creating Car with BPY ===")

	# Main body - stretched cube
	var body = BPY.create_cube(Vector3(4, 1, 2))

	# Cabin - smaller cube on top, moved up and back
	var cabin = BPY.create_cube(Vector3(2, 0.8, 1.8))
	move_mesh(cabin, Vector3(-0.3, 0.9, 0))

	# Merge body and cabin into one mesh
	var car_body = BPYMeshOps.merge_meshes(body, cabin)
	print("Car body vertices: ", car_body.get_vertex_count())
	print("Car body faces: ", car_body.get_face_count())

	# Create wheel (rotated cylinder)
	var wheel_base = BPY.create_cylinder(0.4, 0.3, 16, true, true)
	rotate_mesh_x(wheel_base)  # Rotate to align with Z axis

	# Clone and position 4 wheels
	var wheel_fl = wheel_base.duplicate()
	var wheel_fr = wheel_base.duplicate()
	var wheel_bl = wheel_base.duplicate()
	var wheel_br = wheel_base.duplicate()

	move_mesh(wheel_fl, Vector3(1.2, -0.3, 1.15))   # Front left
	move_mesh(wheel_fr, Vector3(1.2, -0.3, -1.15))  # Front right
	move_mesh(wheel_bl, Vector3(-1.2, -0.3, 1.15))  # Back left
	move_mesh(wheel_br, Vector3(-1.2, -0.3, -1.15)) # Back right

	# Merge all wheels into one mesh
	var wheels = BPYMeshOps.merge_meshes(wheel_fl, wheel_fr)
	wheels = BPYMeshOps.merge_meshes(wheels, wheel_bl)
	wheels = BPYMeshOps.merge_meshes(wheels, wheel_br)
	print("Wheels vertices: ", wheels.get_vertex_count())

	# Create headlights (small cylinders, rotated)
	var headlight_base = BPY.create_cylinder(0.15, 0.1, 12, true, true)
	rotate_mesh_z(headlight_base)  # Rotate to point forward

	var headlight_l = headlight_base.duplicate()
	var headlight_r = headlight_base.duplicate()
	move_mesh(headlight_l, Vector3(2.0, 0.1, 0.6))
	move_mesh(headlight_r, Vector3(2.0, 0.1, -0.6))
	var headlights = BPYMeshOps.merge_meshes(headlight_l, headlight_r)

	# Create taillights
	var taillight_base = BPY.create_cube(Vector3(0.1, 0.2, 0.3))
	var taillight_l = taillight_base.duplicate()
	var taillight_r = taillight_base.duplicate()
	move_mesh(taillight_l, Vector3(-2.0, 0.1, 0.6))
	move_mesh(taillight_r, Vector3(-2.0, 0.1, -0.6))
	var taillights = BPYMeshOps.merge_meshes(taillight_l, taillight_r)

	# Create windshield (thin angled cube)
	var windshield = BPY.create_cube(Vector3(0.05, 0.6, 1.6))
	# Angle the windshield
	for i in range(windshield.get_vertex_count()):
		var pos = windshield.get_vertex_position(i)
		if pos.y > 0:
			pos.x += 0.3
		windshield.set_vertex_position(i, pos)
	move_mesh(windshield, Vector3(0.7, 1.0, 0))

	# Add meshes to scene
	print("Adding car parts...")
	add_child(create_mesh_instance(car_body, Color(0.8, 0.1, 0.1)))  # Red body
	add_child(create_mesh_instance(wheels, Color(0.15, 0.15, 0.15)))  # Black wheels
	add_child(create_mesh_instance(headlights, Color(1.0, 1.0, 0.9)))  # Yellow-white headlights
	add_child(create_mesh_instance(taillights, Color(1.0, 0.0, 0.0)))  # Red taillights
	add_child(create_mesh_instance(windshield, Color(0.3, 0.5, 0.8, 0.5)))  # Blue glass

	# Add a camera
	var camera = Camera3D.new()
	camera.position = Vector3(6, 4, 6)
	add_child(camera)
	camera.look_at(Vector3.ZERO)

	# Add directional light
	var light = DirectionalLight3D.new()
	light.rotation_degrees = Vector3(-45, 45, 0)
	add_child(light)

	# Add ambient light
	var env = Environment.new()
	env.background_mode = Environment.BG_COLOR
	env.background_color = Color(0.4, 0.6, 0.8)
	env.ambient_light_color = Color(1, 1, 1)
	env.ambient_light_energy = 0.3
	var world_env = WorldEnvironment.new()
	world_env.environment = env
	add_child(world_env)

	print("=== Car Created with merge_meshes! ===")

func move_mesh(mesh: BPYMesh, offset: Vector3):
	for i in range(mesh.get_vertex_count()):
		var pos = mesh.get_vertex_position(i)
		mesh.set_vertex_position(i, pos + offset)

func rotate_mesh_x(mesh: BPYMesh):
	for i in range(mesh.get_vertex_count()):
		var pos = mesh.get_vertex_position(i)
		mesh.set_vertex_position(i, Vector3(pos.x, -pos.z, pos.y))

func rotate_mesh_z(mesh: BPYMesh):
	for i in range(mesh.get_vertex_count()):
		var pos = mesh.get_vertex_position(i)
		mesh.set_vertex_position(i, Vector3(pos.y, -pos.x, pos.z))

func create_mesh_instance(bpy_mesh: BPYMesh, color: Color) -> MeshInstance3D:
	var instance = MeshInstance3D.new()
	instance.mesh = bpy_mesh.to_array_mesh()

	var material = StandardMaterial3D.new()
	material.albedo_color = color
	if color.a < 1.0:
		material.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
	instance.material_override = material

	return instance
