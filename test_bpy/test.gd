extends Node

func _ready():
	print("=== BPY Module Test ===")

	# Test cube creation
	print("\n1. Creating cube...")
	var cube = BPY.create_cube(Vector3(2, 2, 2))
	print("   Cube vertices: ", cube.get_vertex_count())
	print("   Cube edges: ", cube.get_edge_count())
	print("   Cube faces: ", cube.get_face_count())

	# Test sphere creation
	print("\n2. Creating UV sphere...")
	var sphere = BPY.create_uv_sphere(1.0, 16, 8)
	print("   Sphere vertices: ", sphere.get_vertex_count())
	print("   Sphere faces: ", sphere.get_face_count())

	# Test cylinder creation
	print("\n3. Creating cylinder...")
	var cylinder = BPY.create_cylinder(1.0, 2.0, 16, true, true)
	print("   Cylinder vertices: ", cylinder.get_vertex_count())
	print("   Cylinder faces: ", cylinder.get_face_count())

	# Test torus creation
	print("\n4. Creating torus...")
	var torus = BPY.create_torus(1.0, 0.25, 24, 8)
	print("   Torus vertices: ", torus.get_vertex_count())
	print("   Torus faces: ", torus.get_face_count())

	# Test conversion to ArrayMesh
	print("\n5. Converting to ArrayMesh...")
	var array_mesh = cube.to_array_mesh()
	print("   ArrayMesh surfaces: ", array_mesh.get_surface_count())

	# Test subdivision modifier
	print("\n6. Testing Subdivision modifier...")
	var subdiv = BPYModifierSubdivision.new()
	subdiv.levels = 1
	var subdivided = subdiv.apply(cube)
	print("   Original cube faces: ", cube.get_face_count())
	print("   Subdivided faces: ", subdivided.get_face_count())

	# Test array modifier
	print("\n7. Testing Array modifier...")
	var array_mod = BPYModifierArray.new()
	array_mod.count = 3
	array_mod.use_constant_offset = true
	array_mod.constant_offset = Vector3(2.5, 0, 0)
	var arrayed = array_mod.apply(cube)
	print("   Array (3x) faces: ", arrayed.get_face_count())

	# Test mirror modifier
	print("\n8. Testing Mirror modifier...")
	var mirror = BPYModifierMirror.new()
	mirror.axis_x = true
	var mirrored = mirror.apply(cube)
	print("   Mirrored faces: ", mirrored.get_face_count())

	print("\n=== BPY Module Test Complete ===")

	# Quit after test
	get_tree().quit()
