load("@bazel_tools//tools/build_defs/pkg:pkg.bzl", "pkg_tar")

BUILD_STRING = """
cc_libary(
    name = {name},
    hdrs = {hdrs},
    srcs = {archives},
    deps = {deps},
    visibility = ["//visibility:public"],
)

cc_libray(
    name = {name}HdrsOnly,
    hdrs = {hdrs},
    deps = {hdrs_only_deps},
    visibility = ["//visibility:public"],
)
"""

def package_libraries(name, build_file = None, hdrs = [], archives = [], deps = []):
    pkg_tar(
        name = "pkg" + name + "Hdrs",
        srcs = hdrs,
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

#def generate_build_file_for_artifactory(name, ):
#    genrule(
#        name = "GeneratePkg" + name + "BUILDFile",
#        cmd = "echo '"+ BUILD_STRING.format(name = name, hdrs = hdrs) + "' > $@",
#        srcs = [],
#        outs = ["BUILD." + name + ".tpl"],
#    )