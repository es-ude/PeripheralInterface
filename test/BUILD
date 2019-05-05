load("@EmbeddedSystemsBuildScripts//:unity.bzl", "unity_test", "mock")

unity_test(
    file_name = "PeripheralInterface_Test.c",
    deps = [
        "//:PeripheralInterface",
        ":MockMutex",
    ],
)

unity_test(
    file_name = "PeripheralSPIImpl_Test.c",
    deps = [
        "//:PeripheralInterface",
    ]
)

mock(
    name = "MockMutex",
    srcs = ["@EmbeddedUtilities//:Util/Mutex.h"],
    deps = ["@EmbeddedUtilities//:Mutex"],
)