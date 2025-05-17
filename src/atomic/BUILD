load("@//:config.bzl", "package_copt")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "spinlock",
    srcs = [
        "spinlock.cpp",
    ],
    hdrs = [
        "//include/atomic:spinlock.h",
    ],
    copts = package_copt,
    strip_include_prefix = "//include/atomic",
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "sync_with_atomic_bool",
    srcs = ["synchronize_with_atomic_bool.cpp"],
    copts = package_copt,
    deps = [],
)

cc_binary(
    name = "sequential_consistency",
    srcs = ["sequential_consistency.cpp"],
    copts = package_copt,
    deps = [],
)

cc_binary(
    name = "relaxed_ordering",
    srcs = ["relaxed_ordering.cpp"],
    copts = package_copt,
    deps = [],
)

cc_binary(
    name = "relaxed_ordering_multiple_threads",
    srcs = ["relaxed_ordering_multi_threaded.cpp"],
    copts = package_copt,
    deps = [],
)

cc_binary(
    name = "relaxed_ordering_acquire_release",
    srcs = ["relaxed_ordering_acquire_release.cpp"],
    copts = package_copt,
    deps = [],
)

cc_binary(
    name = "relaxed_ordering_acquire_release_impose_ordering",
    srcs = ["relaxed_ordering_acquire_release_impose_ordering.cpp"],
    copts = package_copt,
    deps = [],
)

cc_binary(
    name = "relaxed_orderign_acquire_release_transitive_synchronization",
    srcs = ["relaxed_ordering_acquire_release_transitive_synchronization.cpp"],
    copts = package_copt,
    deps = [],
)

cc_binary(
    name = "relaxed_ordering_consume",
    srcs = ["relaxed_ordering_consume.cpp"],
    copts = package_copt,
    deps = [],
)

cc_binary(
    name = "release_sequence_chain",
    srcs = ["release_sequence_chain.cpp"],
    copts = package_copt,
    deps = [],
)

cc_binary(
    name = "fences",
    srcs = ["fences.cpp"],
    copts = package_copt,
    deps = [],
)
