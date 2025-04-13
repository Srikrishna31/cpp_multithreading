load("@//:config.bzl", "package_copt")

cc_library(
    name = "timer",
    srcs = [
        "src/timer.cpp",
    ],
    hdrs = [
        "include/timer.h",
    ],
    copts = package_copt,
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
)

cc_test(
    name = "test_timer",
    srcs = ["test/test_timer.cpp"],
    copts = package_copt,
    tags = ["unit"],
    visibility = ["//visibility:private"],
    deps = [
        ":timer",
        "@gtest",
    ],
)
