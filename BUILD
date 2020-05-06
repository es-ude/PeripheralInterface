load("@bazel_tools//tools/build_defs/pkg:pkg.bzl", "pkg_tar")
load("@AvrToolchain//:helpers.bzl", "default_embedded_binaries", "default_embedded_binary")
load("@AvrToolchain//platforms/cpu_frequency:cpu_frequency.bzl", "cpu_frequency_flag")

filegroup(
    name = "PublicHdrs",
    srcs = glob(["PeripheralInterface/**/*.h"]),
)

filegroup(
    name = "Srcs",
    srcs = [
                "src/PeripheralInterface.c",
                "src/PeripheralSPIImpl.c",
                "src/PeripheralSPIImplIntern.h",
                "src/SpiPinNumbers.h",
                "src/Usart.c",
                "src/UsartIntern.h",
                "src/LufaUsartImpl.c"
    ],
)

exports_files(
    srcs = glob(["PeripheralInterface/**/*.h"]),
    visibility = ["//visibility:public"],
)

cc_library(
    name = "PeripheralInterface",
    srcs = [":Srcs"],
    hdrs = [":PublicHdrs"],
    visibility = ["//visibility:public"],
    linkstatic = True,
    deps = [
        "@EmbeddedUtilities//:BitManipulation",
        "@EmbeddedUtilities//:Mutex",
        "@LUFA//:VirtualSerial"
    ],
)

cc_library(
    name = "PeripheralInterfaceHdrsOnly",
    srcs = [
        ":PublicHdrs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@EmbeddedUtilities//:MutexHdrsOnly",
        "@LUFA//:VirtualSerial",
    ],
    linkstatic = True,
)

pkg_tar(
    name = "pkg",
    srcs = [
        ":PublicHdrs",
        ":Srcs",
        "BUILD",
    ],
    extension = "tar.gz",
    mode = "0644",
    strip_prefix = ".",
)
