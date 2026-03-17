extends Node3D

func _ready():
	print("=== Visual BPY Test ===")

	# Create BPY cube
	var cube = BPY.create_cube(Vector3(2, 2, 2))
	print("BPY cube vertices: ", cube.get_vertex_count())
	print("BPY cube faces: ", cube.get_face_count())

	var array_mesh = cube.to_array_mesh()
	print("ArrayMesh surfaces: ", array_mesh.get_surface_count())

	# Debug: check if mesh has any surfaces
	if array_mesh.get_surface_count() == 0:
		print("ERROR: ArrayMesh has no surfaces!")
		return

	# Debug: check AABB
	var aabb = array_mesh.get_aabb()
	print("ArrayMesh AABB: ", aabb)

	# Create mesh instance for BPY cube (RED, on the left)
	var bpy_instance = MeshInstance3D.new()
	bpy_instance.mesh = array_mesh
	bpy_instance.position = Vector3(-2, 0, 0)

	var mat_red = StandardMaterial3D.new()
	mat_red.albedo_color = Color(1, 0, 0)
	mat_red.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	mat_red.cull_mode = BaseMaterial3D.CULL_DISABLED
	bpy_instance.material_override = mat_red

	add_child(bpy_instance)
	print("Added BPY cube at position: ", bpy_instance.position)

	# Create built-in BoxMesh for comparison (GREEN, on the right)
	var box_instance = MeshInstance3D.new()
	var box_mesh = BoxMesh.new()
	box_mesh.size = Vector3(2, 2, 2)
	box_instance.mesh = box_mesh
	box_instance.position = Vector3(2, 0, 0)

	var mat_green = StandardMaterial3D.new()
	mat_green.albedo_color = Color(0, 1, 0)
	mat_green.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	mat_green.cull_mode = BaseMaterial3D.CULL_DISABLED
	box_instance.material_override = mat_green

	add_child(box_instance)
	print("Added BoxMesh at position: ", box_instance.position)

	# Camera
	var camera = Camera3D.new()
	camera.position = Vector3(0, 3, 8)
	add_child(camera)
	camera.look_at(Vector3(0, 0, 0))
	print("Camera at: ", camera.position, " looking at origin")

	# Blue background
	var env = Environment.new()
	env.background_mode = Environment.BG_COLOR
	env.background_color = Color(0.2, 0.4, 0.8)
	var world_env = WorldEnvironment.new()
	world_env.environment = env
	add_child(world_env)

	print("=== Scene Ready ===")
	print("You should see: RED cube on left (BPY), GREEN cube on right (BoxMesh)")
