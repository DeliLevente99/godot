extends Node3D

func _ready():
	print("=== Simple BPY Test ===")

	# Create a simple cube
	var cube = BPY.create_cube(Vector3(2, 2, 2))
	print("Cube vertices: ", cube.get_vertex_count())
	print("Cube faces: ", cube.get_face_count())

	# Convert to ArrayMesh
	var array_mesh = cube.to_array_mesh()
	print("ArrayMesh surfaces: ", array_mesh.get_surface_count())

	if array_mesh.get_surface_count() > 0:
		var arrays = array_mesh.surface_get_arrays(0)
		var verts = arrays[Mesh.ARRAY_VERTEX]
		var normals = arrays[Mesh.ARRAY_NORMAL]
		print("ArrayMesh vertices: ", verts.size())
		print("ArrayMesh normals: ", normals.size() if normals else "null")
		if verts.size() > 0:
			print("First vertex: ", verts[0])
		if normals and normals.size() > 0:
			print("First normal: ", normals[0])

	# Create MeshInstance3D
	var mesh_instance = MeshInstance3D.new()
	mesh_instance.mesh = array_mesh

	# Try with a simple material
	var mat = StandardMaterial3D.new()
	mat.albedo_color = Color(1, 0, 0)  # Bright red
	mat.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED  # No lighting needed
	mesh_instance.material_override = mat

	add_child(mesh_instance)

	# Also try with built-in BoxMesh for comparison
	var box_instance = MeshInstance3D.new()
	box_instance.mesh = BoxMesh.new()
	box_instance.position = Vector3(3, 0, 0)
	var mat2 = StandardMaterial3D.new()
	mat2.albedo_color = Color(0, 1, 0)  # Green
	mat2.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	box_instance.material_override = mat2
	add_child(box_instance)

	# Camera
	var camera = Camera3D.new()
	camera.position = Vector3(5, 3, 5)
	add_child(camera)
	camera.look_at(Vector3(1, 0, 0))

	print("=== Test Complete ===")
	print("Red cube = BPY, Green cube = Built-in BoxMesh")
