load("@bazel_tools//tools/build_defs/pkg:pkg.bzl", "pkg_tar", "pkg_deb")

filegroup(
    name = "PublicHdrs",
    srcs = glob(["PeripheralInterface/**/*.h"]),
)

exports_files(
    srcs = glob(["PeripheralInterface/**/*.h"]),
    visibility = ["//visibility:public"],
)

cc_library(
    name = "PeripheralInterface",
    srcs = [
        "src/PeripheralInterface.c",
        "src/PeripheralSPIImpl.c",
        "src/PeripheralSPIImplIntern.h",
        "src/SpiPinNumbers.h",
        "src/Usart.c",
        "src/UsartIntern.h",
    ],
    hdrs = ["PublicHdrs"],
    features = ["c99"],
    visibility = ["//visibility:public"],
    deps = [
        "@EmbeddedUtilities//:BitManipulation",
        "@EmbeddedUtilities//:Debug",
        "@EmbeddedUtilities//:Mutex",
    ],
)

pkg_tar(
    name = "pkgPeripheralInterface",
    srcs = [":PeripheralInterface"],
    extension = "tar.gz",
    mode = "0644"
)

pkg_tar(
    name = "pkgTemplate",
    srcs = ["BUILD.peripheralInterface"],
    extension = "tar.gz",
    mode = "0644",
    remap_paths = {
        "BUILD.peripheralInterface": "BUILD"
    }
)

pkg_tar(
    name = "pkgPublicHdrs",
    srcs = [":PublicHdrs"],
    strip_prefix = ".",
    extension = "tar.gz",
    mode = "0644"
)

pkg_tar(
    name = "pkg",
    deps = ["pkgPeripheralInterface", "pkgPublicHdrs", "pkgTemplate"],
    extension = "tar.gz",
    mode = "0644"
)