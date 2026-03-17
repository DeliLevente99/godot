extends Node3D

func _ready():
	print("=== Debug BPY Mesh Test ===")

	# Create a simple cube
	var cube = BPY.create_cube(Vector3(2, 2, 2))
	print("Created cube")
	print("  Vertex count: ", cube.get_vertex_count())
	print("  Face count: ", cube.get_face_count())
	print("  Loop count: ", cube.get_loop_count())

	# Print first few vertices
	for i in range(min(4, cube.get_vertex_count())):
		print("  Vertex ", i, ": ", cube.get_vertex_position(i))

	# Print first few faces
	for i in range(min(2, cube.get_face_count())):
		print("  Face ", i, " vertices: ", cube.get_face_vertices(i))
		print("  Face ", i, " normal: ", cube.get_face_normal(i))

	# Convert to ArrayMesh
	var array_mesh = cube.to_array_mesh()
	print("Converted to ArrayMesh")
	print("  Surface count: ", array_mesh.get_surface_count())

	if array_mesh.get_surface_count() > 0:
		var arrays = array_mesh.surface_get_arrays(0)
		print("  Array size: ", arrays.size())

		var verts = arrays[Mesh.ARRAY_VERTEX]
		var normals = arrays[Mesh.ARRAY_NORMAL]
		var indices = arrays[Mesh.ARRAY_INDEX]

		print("  Vertices array size: ", verts.size() if verts else "null")
		print("  Normals array size: ", normals.size() if normals else "null")
		print("  Indices array size: ", indices.size() if indices else "null")

		if verts and verts.size() > 0:
			print("  First 4 vertices:")
			for i in range(min(4, verts.size())):
				print("    [", i, "] pos=", verts[i], " normal=", normals[i] if normals else "null")

		if indices and indices.size() > 0:
			print("  First 6 indices: ", indices.slice(0, min(6, indices.size())))

	print("=== Done ===")
