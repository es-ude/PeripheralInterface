load("//:packaging.bzl", "package_libraries")

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
    visibility = ["//visibility:public"],

    deps = [
        "@EmbeddedUtilities//:BitManipulation",
        "@EmbeddedUtilities//:Mutex",
    ],
)

package_libraries(
    name = "pkg",
    hdrs = [":PublicHdrs"],
    archives = [":PeripheralInterface"],
    build_file = "BUILD.peripheralInterface",
)
