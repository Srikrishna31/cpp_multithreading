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

cc_library(
    name = "spinlock",
    srcs = [
        "src/atomic/spinlock.cpp",
    ],
    hdrs = [
        "include/atomic/spinlock.h",
    ],
    copts = package_copt,
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "sync_with_atomic_bool",
    srcs = ["src/atomic/synchronize_with_atomic_bool.cpp"],
    deps = [],
)

cc_binary(
    name = "sequential_consistency",
    srcs = ["src/atomic/sequential_consistency.cpp"],
    deps = [],
)

cc_binary(
    name = "relaxed_ordering",
    srcs = ["src/atomic/relaxed_ordering.cpp"],
    deps = [],
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
