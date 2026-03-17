extends Node3D
## BPY Module Test Script
## Run this scene to test all BPY features

func _ready():
	print("=" .repeat(60))
	print("BPY MODULE TEST SUITE")
	print("=" .repeat(60))

	test_primitives()
	test_modifiers()
	test_mesh_operations()
	test_export()

	print("=" .repeat(60))
	print("ALL TESTS COMPLETE")
	print("=" .repeat(60))

## TEST 1: Primitives
func test_primitives():
	print("\n[TEST] Primitives")
	print("-" .repeat(40))

	# Cube
	var cube = BPY.create_cube(Vector3(2, 2, 2))
	print("  [OK] Cube: %d vertices, %d faces" % [cube.get_vertex_count(), cube.get_face_count()])

	# Sphere
	var sphere = BPY.create_uv_sphere(1.0, 16, 8)
	print("  [OK] Sphere: %d vertices, %d faces" % [sphere.get_vertex_count(), sphere.get_face_count()])

	# Cylinder
	var cylinder = BPY.create_cylinder(0.5, 2.0, 16, true, true)
	print("  [OK] Cylinder: %d vertices, %d faces" % [cylinder.get_vertex_count(), cylinder.get_face_count()])

	# Cone
	var cone = BPY.create_cone(1.0, 2.0, 16, true)
	print("  [OK] Cone: %d vertices, %d faces" % [cone.get_vertex_count(), cone.get_face_count()])

	# Torus
	var torus = BPY.create_torus(1.0, 0.3, 16, 8)
	print("  [OK] Torus: %d vertices, %d faces" % [torus.get_vertex_count(), torus.get_face_count()])

	# Plane
	var plane = BPY.create_plane(Vector2(2, 2), Vector2i(2, 2))
	print("  [OK] Plane: %d vertices, %d faces" % [plane.get_vertex_count(), plane.get_face_count()])

## TEST 2: Modifiers
func test_modifiers():
	print("\n[TEST] Modifiers")
	print("-" .repeat(40))

	# Subdivision
	var mesh = BPY.create_cube(Vector3.ONE)
	var subdiv = BPYModifierSubdivision.new()
	subdiv.levels = 1
	var subdivided = subdiv.apply(mesh)
	print("  [OK] Subdivision: %d -> %d faces" % [mesh.get_face_count(), subdivided.get_face_count()])

	# Array
	mesh = BPY.create_cube(Vector3.ONE)
	var array_mod = BPYModifierArray.new()
	array_mod.count = 3
	array_mod.use_constant_offset = true
	array_mod.constant_offset = Vector3(1.5, 0, 0)
	var arrayed = array_mod.apply(mesh)
	print("  [OK] Array: %d -> %d faces (count=3)" % [mesh.get_face_count(), arrayed.get_face_count()])

	# Mirror
	mesh = BPY.create_cube(Vector3.ONE)
	var mirror = BPYModifierMirror.new()
	mirror.axis_x = true
	var mirrored = mirror.apply(mesh)
	print("  [OK] Mirror: %d -> %d faces" % [mesh.get_face_count(), mirrored.get_face_count()])

	# Solidify
	var plane_mesh = BPY.create_plane(Vector2(2, 2), Vector2i(1, 1))
	var solidify = BPYModifierSolidify.new()
	solidify.thickness = 0.2
	var solidified = solidify.apply(plane_mesh)
	print("  [OK] Solidify: %d -> %d faces" % [plane_mesh.get_face_count(), solidified.get_face_count()])

	# Bevel
	mesh = BPY.create_cube(Vector3.ONE)
	var bevel = BPYModifierBevel.new()
	bevel.width = 0.1
	bevel.segments = 2
	var beveled = bevel.apply(mesh)
	print("  [OK] Bevel: %d -> %d faces" % [mesh.get_face_count(), beveled.get_face_count()])

	# Decimate
	var sphere = BPY.create_uv_sphere(1.0, 32, 16)
	var decimate = BPYModifierDecimate.new()
	decimate.ratio = 0.5
	var decimated = decimate.apply(sphere)
	print("  [OK] Decimate: %d -> %d faces (50%%)" % [sphere.get_face_count(), decimated.get_face_count()])

	# Boolean
	var cube1 = BPY.create_cube(Vector3(2, 2, 2))
	var cube2 = BPY.create_cube(Vector3(1.5, 1.5, 3))
	var boolean = BPYModifierBoolean.new()
	boolean.operation = BPYModifierBoolean.OPERATION_DIFFERENCE
	boolean.operand = cube2
	var bool_result = boolean.apply(cube1)
	print("  [OK] Boolean Difference: %d faces" % [bool_result.get_face_count()])

## TEST 3: Mesh Operations
func test_mesh_operations():
	print("\n[TEST] Mesh Operations")
	print("-" .repeat(40))

	# Merge meshes
	var mesh1 = BPY.create_cube(Vector3.ONE)
	var mesh2 = BPY.create_uv_sphere(0.5, 8, 4)
	var merged = BPYMeshOps.merge_meshes(mesh1, mesh2)
	print("  [OK] Merge: %d + %d = %d faces" % [mesh1.get_face_count(), mesh2.get_face_count(), merged.get_face_count()])

	# To ArrayMesh
	var cube = BPY.create_cube(Vector3.ONE)
	var array_mesh = cube.to_array_mesh()
	print("  [OK] To ArrayMesh: %d surfaces" % [array_mesh.get_surface_count()])

## TEST 4: Export
func test_export():
	print("\n[TEST] Export")
	print("-" .repeat(40))

	var mesh = BPY.create_cube(Vector3(2, 2, 2))
	var array_mesh = mesh.to_array_mesh()

	# OBJ
	var err = BPYExporter.export_obj(array_mesh, "res://test_output.obj")
	print("  [%s] OBJ Export" % ["OK" if err == OK else "FAIL"])

	# STL ASCII
	err = BPYExporter.export_stl(array_mesh, "res://test_output_ascii.stl", false)
	print("  [%s] STL ASCII Export" % ["OK" if err == OK else "FAIL"])

	# STL Binary
	err = BPYExporter.export_stl(array_mesh, "res://test_output_binary.stl", true)
	print("  [%s] STL Binary Export" % ["OK" if err == OK else "FAIL"])

	# PLY ASCII
	err = BPYExporter.export_ply(array_mesh, "res://test_output_ascii.ply", false)
	print("  [%s] PLY ASCII Export" % ["OK" if err == OK else "FAIL"])

	# PLY Binary
	err = BPYExporter.export_ply(array_mesh, "res://test_output_binary.ply", true)
	print("  [%s] PLY Binary Export" % ["OK" if err == OK else "FAIL"])
