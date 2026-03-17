# BPY Module - Blender-style Mesh Tools for Godot

A Godot Engine module providing Blender-like procedural mesh generation and export capabilities.

## Engine Changes

This module adds the following to the Godot Engine:

### New Files

```
modules/bpy/
├── config.py                     # Module configuration
├── SCsub                         # Build script
├── register_types.h/cpp          # Module registration
├── core/
│   ├── bpy_mesh.h/cpp            # BPYMesh class (BMesh-style mesh)
│   └── bpy_elements.h            # Vertex/Edge/Face structures
├── primitives/
│   └── bpy_primitives.h/cpp      # BPY singleton (create_cube, etc.)
├── modifiers/
│   ├── bpy_modifier.h/cpp        # Base modifier class
│   ├── bpy_modifier_array.h/cpp
│   ├── bpy_modifier_bevel.h/cpp
│   ├── bpy_modifier_boolean.h/cpp
│   ├── bpy_modifier_decimate.h/cpp
│   ├── bpy_modifier_mirror.h/cpp
│   ├── bpy_modifier_solidify.h/cpp
│   └── bpy_modifier_subdivision.h/cpp
├── ops/
│   └── bpy_mesh_ops.h/cpp        # Mesh operations (merge, etc.)
├── utils/
│   └── bpy_manifold_adapter.h/cpp # Manifold library integration
├── io/
│   ├── bpy_exporter.h/cpp        # Export functions (OBJ, STL, PLY)
│   └── bpy_export_dialog.h/cpp   # Runtime export dialog
└── editor/
    └── bpy_editor_plugin.h/cpp   # Editor plugin (menu integration)
```

### New Classes

| Class | Type | Description |
|-------|------|-------------|
| `BPY` | Singleton | Primitive creation (cube, sphere, etc.) |
| `BPYMesh` | RefCounted | BMesh-style mesh representation |
| `BPYModifier` | RefCounted | Abstract modifier base class |
| `BPYModifierSubdivision` | RefCounted | Catmull-Clark subdivision |
| `BPYModifierBoolean` | RefCounted | Boolean operations (union, difference, intersection) |
| `BPYModifierArray` | RefCounted | Array modifier |
| `BPYModifierMirror` | RefCounted | Mirror modifier |
| `BPYModifierBevel` | RefCounted | Bevel modifier |
| `BPYModifierSolidify` | RefCounted | Solidify modifier |
| `BPYModifierDecimate` | RefCounted | Mesh decimation |
| `BPYMeshOps` | Static | Mesh operations |
| `BPYExporter` | Static | File export (OBJ, STL, PLY) |
| `BPYExportDialog` | Control | Runtime export dialog |
| `BPYEditorPlugin` | EditorPlugin | Editor menu integration |
| `BPYEditorExportDialog` | ConfirmationDialog | Editor export dialog |

### Dependencies Used

- `thirdparty/manifold/` - Boolean operations (already in Godot for CSG)
- `thirdparty/meshoptimizer/` - Mesh decimation (already in Godot)

### Editor Integration

- **Project → BPY Mesh Tools** menu added
  - Export Selected Mesh...
  - Create Cube/Sphere/Cylinder/Cone/Torus

---

## Installation

The module is integrated into the Godot source code. Build Godot with:

```bash
scons platform=windows target=editor
```

---

## 1. Creating Primitives (GDScript)

```gdscript
extends Node3D

func _ready():
    # Cube
    var cube = BPY.create_cube(Vector3(2, 2, 2))

    # Sphere
    var sphere = BPY.create_uv_sphere(1.0, 32, 16)

    # Cylinder
    var cylinder = BPY.create_cylinder(0.5, 2.0, 32, true, true)

    # Cone
    var cone = BPY.create_cone(1.0, 2.0, 32, true)

    # Torus
    var torus = BPY.create_torus(1.0, 0.3, 32, 16)

    # Plane
    var plane = BPY.create_plane(Vector2(4, 4), Vector2i(4, 4))

    # Display
    var mi = MeshInstance3D.new()
    mi.mesh = cube.to_array_mesh()
    add_child(mi)
```

---

## 2. Using Modifiers

### Subdivision

```gdscript
var mesh = BPY.create_cube(Vector3.ONE)

var subdiv = BPYModifierSubdivision.new()
subdiv.levels = 2
subdiv.use_creases = true
mesh = subdiv.apply(mesh)
```

### Boolean Operations

```gdscript
var cube = BPY.create_cube(Vector3(2, 2, 2))
var sphere = BPY.create_uv_sphere(1.2, 32, 16)

var boolean = BPYModifierBoolean.new()
boolean.operation = BPYModifierBoolean.OPERATION_DIFFERENCE
boolean.operand = sphere
var result = boolean.apply(cube)
```

### Array

```gdscript
var mesh = BPY.create_cube(Vector3.ONE)

var array_mod = BPYModifierArray.new()
array_mod.count = 5
array_mod.use_constant_offset = true
array_mod.constant_offset = Vector3(1.5, 0, 0)
mesh = array_mod.apply(mesh)
```

### Mirror

```gdscript
var mesh = BPY.create_cube(Vector3.ONE)

var mirror = BPYModifierMirror.new()
mirror.axis_x = true
mirror.merge_threshold = 0.001
mesh = mirror.apply(mesh)
```

### Bevel

```gdscript
var mesh = BPY.create_cube(Vector3.ONE)

var bevel = BPYModifierBevel.new()
bevel.width = 0.1
bevel.segments = 3
mesh = bevel.apply(mesh)
```

### Solidify

```gdscript
var mesh = BPY.create_plane(Vector2(2, 2), Vector2i(1, 1))

var solidify = BPYModifierSolidify.new()
solidify.thickness = 0.2
mesh = solidify.apply(mesh)
```

### Decimate

```gdscript
var mesh = BPY.create_uv_sphere(1.0, 64, 32)

var decimate = BPYModifierDecimate.new()
decimate.ratio = 0.5  # Reduce to 50%
mesh = decimate.apply(mesh)
```

---

## 3. Mesh Operations

```gdscript
# Merge two meshes
var mesh1 = BPY.create_cube(Vector3.ONE)
var mesh2 = BPY.create_uv_sphere(0.5, 16, 8)
var merged = BPYMeshOps.merge_meshes(mesh1, mesh2)

# Mesh information
print("Vertices: ", mesh.get_vertex_count())
print("Edges: ", mesh.get_edge_count())
print("Faces: ", mesh.get_face_count())
```

---

## 4. Exporting (GDScript)

```gdscript
var mesh = BPY.create_cube(Vector3(2, 2, 2))
var array_mesh = mesh.to_array_mesh()

# OBJ + MTL
BPYExporter.export_obj(array_mesh, "res://model.obj")

# STL (for 3D printing)
BPYExporter.export_stl(array_mesh, "res://model.stl", false)  # ASCII
BPYExporter.export_stl(array_mesh, "res://model.stl", true)   # Binary

# PLY
BPYExporter.export_ply(array_mesh, "res://model.ply", false)  # ASCII
BPYExporter.export_ply(array_mesh, "res://model.ply", true)   # Binary
```

---

## 5. Editor Export (GUI)

In the editor:

1. **Select a MeshInstance3D** in the Scene panel
2. **Project → BPY Mesh Tools → Export Selected Mesh...**
3. Choose the format:
   - OBJ + MTL (Blender, Maya)
   - STL ASCII/Binary (3D printing)
   - PLY ASCII/Binary (MeshLab)
   - Godot Resource (.tres)
4. Enter the filename (defaults to node name)
5. Click **Export**

---

## 6. Full Example: Procedural Table

```gdscript
extends Node3D

func _ready():
    var table = create_table()

    var mi = MeshInstance3D.new()
    mi.mesh = table.to_array_mesh()
    add_child(mi)

    # Export
    BPYExporter.export_obj(mi.mesh, "res://table.obj")

func create_table() -> BPYMesh:
    # Table top
    var result = BPY.create_cube(Vector3(2, 0.1, 1))
    result.translate(Vector3(0, 0.8, 0))

    # 4 legs
    var leg_positions = [
        Vector3(-0.8, 0.375, -0.4),
        Vector3(0.8, 0.375, -0.4),
        Vector3(-0.8, 0.375, 0.4),
        Vector3(0.8, 0.375, 0.4)
    ]

    for pos in leg_positions:
        var leg = BPY.create_cube(Vector3(0.1, 0.75, 0.1))
        leg.translate(pos)
        result = BPYMeshOps.merge_meshes(result, leg)

    return result
```

---

## 7. Console Output

During export, the following logs appear in the Output panel:

```
[BPY Editor] Looking for selected mesh...
[BPY Editor] Selected nodes: 1
[BPY Editor] Found MeshInstance3D: MyCube
[BPY Editor] Mesh type: BoxMesh
[BPY Editor] Converting BoxMesh to ArrayMesh...
[BPY Export] Starting OBJ export to: res://MyCube.obj
[BPY Export] Mesh surfaces: 1
[BPY Export] OBJ export complete: 24 vertices
[BPY Editor] Export successful!
```

---

## API Reference

### BPY (Singleton)

| Method | Description |
|--------|-------------|
| `create_cube(size: Vector3)` | Create a cube |
| `create_uv_sphere(radius, segments, rings)` | Create a UV sphere |
| `create_cylinder(radius, height, segments, cap_top, cap_bottom)` | Create a cylinder |
| `create_cone(radius, height, segments, cap)` | Create a cone |
| `create_torus(major_radius, minor_radius, major_segments, minor_segments)` | Create a torus |
| `create_plane(size: Vector2, subdivisions: Vector2i)` | Create a plane |

### BPYMesh

| Method | Description |
|--------|-------------|
| `to_array_mesh()` | Convert to Godot ArrayMesh |
| `get_vertex_count()` | Get vertex count |
| `get_edge_count()` | Get edge count |
| `get_face_count()` | Get face count |
| `add_vertex(position)` | Add a vertex |
| `add_face(vertex_indices)` | Add a face |

### BPYExporter

| Method | Description |
|--------|-------------|
| `export_obj(mesh, path)` | Export as OBJ + MTL |
| `export_stl(mesh, path, binary)` | Export as STL |
| `export_ply(mesh, path, binary)` | Export as PLY |

---

## Supported Formats

| Format | Extension | Use Case |
|--------|-----------|----------|
| Wavefront OBJ | .obj + .mtl | Blender, Maya, 3ds Max |
| STL | .stl | 3D printing |
| PLY | .ply | MeshLab, scientific visualization |
| Godot Resource | .tres | Godot Engine |
