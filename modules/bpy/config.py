def can_build(env, platform):
    return True


def configure(env):
    pass


def get_doc_classes():
    return [
        "BPY",
        "BPYMesh",
        "BPYModifier",
        "BPYModifierSubdivision",
        "BPYModifierBoolean",
        "BPYModifierArray",
        "BPYModifierMirror",
        "BPYModifierBevel",
        "BPYModifierSolidify",
        "BPYModifierDecimate",
        "BPYMeshOps",
    ]


def get_doc_path():
    return "doc_classes"
