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

#########################################
### Generate zip file for publishing  ###
#########################################

genzip_cmd = "zip -j $(OUTS) $(locations :PeripheralInterface); zip $(OUTS) $(locations :PublicHdrs)"

LibAndHeaderForPublishing = [
    ":PeripheralInterface",
    ":PublicHdrs",
]

genrule(
    name = "PeripheralInterfaceZip",
    srcs = LibAndHeaderForPublishing,
    outs = ["PeripheralInterface.zip"],
    cmd = genzip_cmd,
)
