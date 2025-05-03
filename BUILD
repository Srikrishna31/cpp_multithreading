load("@//:config.bzl", "package_copt")

cc_test(
    name = "test_timer",
    srcs = ["test/test_timer.cpp"],
    copts = package_copt,
    tags = ["unit"],
    visibility = ["//visibility:private"],
    deps = [
        "//src:timer",
        "@gtest",
    ],
)
