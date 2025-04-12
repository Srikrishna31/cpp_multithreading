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
    copts = package_copt,
    deps = [],
)

cc_binary(
    name = "sequential_consistency",
    srcs = ["src/atomic/sequential_consistency.cpp"],
    copts = package_copt,
    deps = [],
)

cc_binary(
    name = "relaxed_ordering",
    srcs = ["src/atomic/relaxed_ordering.cpp"],
    copts = package_copt,
    deps = [],
)

cc_binary(
    name = "relaxed_ordering_multiple_threads",
    srcs = ["src/atomic/relaxed_ordering_multi_threaded.cpp"],
    copts = package_copt,
    deps = [],
)

cc_binary(
    name = "relaxed_ordering_acquire_release",
    srcs = ["src/atomic/relaxed_ordering_acquire_release.cpp"],
    copts = package_copt,
    deps = [],
)

cc_binary(
    name = "relaxed_ordering_acquire_release_impose_ordering",
    srcs = ["src/atomic/relaxed_ordering_acquire_release_impose_ordering.cpp"],
    copts = package_copt,
    deps = [],
)

cc_binary(
    name = "relaxed_orderign_acquire_release_transitive_synchronization",
    srcs = ["src/atomic/relaxed_ordering_acquire_release_transitive_synchronization.cpp"],
    copts = package_copt,
    deps = [],
)

cc_binary(
    name = "relaxed_ordering_consume",
    srcs = ["src/atomic/relaxed_ordering_consume.cpp"],
    copts = package_copt,
    deps = [],
)

cc_binary(
    name = "release_sequence_chain",
    srcs = ["src/atomic/release_sequence_chain.cpp"],
    copts = package_copt,
    deps = [],
)

cc_binary(
    name = "fences",
    srcs = ["src/atomic/fences.cpp"],
    copts = package_copt,
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
