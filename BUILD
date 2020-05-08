load("@bazel_tools//tools/build_defs/pkg:pkg.bzl", "pkg_tar")
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
    linkstatic = True,
    visibility = ["//visibility:public"],
    deps = [
        ":LufaUsart",
        "@EmbeddedUtilities//:BitManipulation",
        "@EmbeddedUtilities//:Mutex",
    ],
)

cc_library(
    name = "PeripheralInterfaceHdrsOnly",
    srcs = [
        ":PublicHdrs",
    ],
    linkstatic = True,
    visibility = ["//visibility:public"],
    deps = [
        "@EmbeddedUtilities//:MutexHdrsOnly",
        "@LUFA//:VirtualSerial",
    ],
)

cc_library(
    name = "LufaUsart",
    srcs = [
        "src/LufaUsartImpl.c",
    ],
    hdrs = [
        "PeripheralInterface/LufaUsartImpl.h",
    ],
    copts = cpu_frequency_flag() + [
        "-isystem external/LUFA",
         "-Iexternal/LUFA/Demos/Device/ClassDriver/VirtualSerial/Config"
    ],
    deps = [
        "@LUFA//:VirtualSerial",
    ],
)
