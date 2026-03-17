extends Node

func _ready():
	print("=== Testing merge_meshes ===")

	# Create two cubes
	var cube1 = BPY.create_cube(Vector3(1, 1, 1))
	var cube2 = BPY.create_cube(Vector3(1, 1, 1))

	# Move cube2 to the right
	for i in range(cube2.get_vertex_count()):
		var pos = cube2.get_vertex_position(i)
		cube2.set_vertex_position(i, pos + Vector3(2, 0, 0))

	print("Cube1 vertices: ", cube1.get_vertex_count())
	print("Cube2 vertices: ", cube2.get_vertex_count())

	# Merge them
	var merged = BPYMeshOps.merge_meshes(cube1, cube2)
	print("Merged vertices: ", merged.get_vertex_count())
	print("Merged faces: ", merged.get_face_count())

	print("=== merge_meshes works! ===")

	get_tree().quit()
