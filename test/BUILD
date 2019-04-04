load("@UnityPlugin//Unity_rules:unity.bzl", "unity_test", "mock")

cexception_copts = [
    "-DCEXCEPTION_T=uint8_t",
    "-DCEXCEPTION_NONE=0x00"
]

unity_test(
    file_name = "PeripheralInterface_Test.c",
    copts = cexception_copts,
    deps = [
        "//:PeripheralInterface",
        ":MockMutex",
    ],
)

unity_test(
    file_name = "PeripheralSPIImpl_Test.c",
    copts = cexception_copts,
    deps = [
        "//:PeripheralInterface",
    ]
)

mock(
    name = "MockMutex",
    srcs = ["@Util//:Util/Mutex.h"],
    deps = ["@Util//:Mutex"],
)