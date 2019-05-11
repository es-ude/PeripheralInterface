load("@bazel_tools//tools/build_defs/pkg:pkg.bzl", "pkg_tar")

def package_libraries(name, build_file, hdrs = [], archives = []):
    pkg_tar(
        name = "pkg" + name + "Hdrs",
        srcs = archives,
        strip_prefix = ".",
        extension = "tar.gz",
        mode = "0644"
    )

    pkg_tar(
        name = "pkg" + name + "BuildFile",
        srcs = [build_file],
        extension = "tar.gz",
        mode = "0644",
        remap_paths = {
            build_file: "BUILD"
        }
    )

    pkg_tar(
        name = "pkg" + name + "Archives",
        srcs = archives,
        extension = "tar.gz",
        mode = "0644"
    )

    pkg_tar(
        name = name,
        deps = [":pkg" + name + suffix for suffix in ["BuildFile", "Archives", "Hdrs"]],
        extension = "tar.gz",
        mode = "0644"
    )