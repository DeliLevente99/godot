extends Node3D

func _ready():
	print("=== Detailed Mesh Comparison ===")

	# Create BPY cube
	var bpy_cube = BPY.create_cube(Vector3(2, 2, 2))
	var bpy_mesh = bpy_cube.to_array_mesh()

	# Create built-in BoxMesh and convert to ArrayMesh for comparison
	var box = BoxMesh.new()
	box.size = Vector3(2, 2, 2)

	# BoxMesh is a PrimitiveMesh, need to get arrays differently
	var box_arrays = box.get_mesh_arrays()

	print("\n=== BPY ArrayMesh ===")
	print_mesh_info(bpy_mesh)

	print("\n=== BoxMesh Arrays ===")
	print_arrays_info(box_arrays)

	# Create scene with both meshes
	var bpy_instance = MeshInstance3D.new()
	bpy_instance.mesh = bpy_mesh
	bpy_instance.position = Vector3(-2, 0, 0)
	var mat_red = StandardMaterial3D.new()
	mat_red.albedo_color = Color(1, 0, 0)
	mat_red.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	bpy_instance.material_override = mat_red
	add_child(bpy_instance)

	var box_instance = MeshInstance3D.new()
	box_instance.mesh = box
	box_instance.position = Vector3(2, 0, 0)
	var mat_green = StandardMaterial3D.new()
	mat_green.albedo_color = Color(0, 1, 0)
	mat_green.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	box_instance.material_override = mat_green
	add_child(box_instance)

	# Camera
	var camera = Camera3D.new()
	camera.position = Vector3(0, 3, 8)
	add_child(camera)
	camera.look_at(Vector3(0, 0, 0))

	print("\n=== Scene Ready ===")

func print_mesh_info(mesh: ArrayMesh):
	print("Surface count: ", mesh.get_surface_count())
	if mesh.get_surface_count() > 0:
		var arrays = mesh.surface_get_arrays(0)
		print_arrays_info(arrays)

func print_arrays_info(arrays: Array):
	var verts = arrays[Mesh.ARRAY_VERTEX] if arrays.size() > Mesh.ARRAY_VERTEX else null
	var normals = arrays[Mesh.ARRAY_NORMAL] if arrays.size() > Mesh.ARRAY_NORMAL else null
	var tangents = arrays[Mesh.ARRAY_TANGENT] if arrays.size() > Mesh.ARRAY_TANGENT else null
	var uvs = arrays[Mesh.ARRAY_TEX_UV] if arrays.size() > Mesh.ARRAY_TEX_UV else null
	var indices = arrays[Mesh.ARRAY_INDEX] if arrays.size() > Mesh.ARRAY_INDEX else null

	print("  Vertices: ", verts.size() if verts else "null")
	print("  Normals: ", normals.size() if normals else "null")
	print("  Tangents: ", tangents.size() if tangents else "null")
	print("  UVs: ", uvs.size() if uvs else "null")
	print("  Indices: ", indices.size() if indices else "null")

	if verts and verts.size() > 0:
		# Print first triangle
		print("  First triangle:")
		for i in range(min(3, verts.size())):
			var v = verts[i]
			var n = normals[i] if normals and normals.size() > i else null
			var uv = uvs[i] if uvs and uvs.size() > i else null
			print("    [", i, "] v=", v, " n=", n, " uv=", uv)
