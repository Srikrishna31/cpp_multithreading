load("@//:config.bzl", "package_copt")
load("@//:convenience_def.bzl", "generate_cc_binary_targets")
load(":targets.bzl", "sync_concurrent_op_binary_target_list")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "threadsafe_queue",
    srcs = ["thread_safe_queue.h"],
    copts = package_copt,
    deps = [],
)

generate_cc_binary_targets(sync_concurrent_op_binary_target_list)

cc_library(
    name = "sequential_quick_sort",
    srcs = ["//include/synchronizing_concurrent_operations:sequential_quick_sort.h"],
    hdrs = ["//include/synchronizing_concurrent_operations:sequential_quick_sort.h"],
    copts = package_copt,
    strip_include_prefix = "//include",
    deps = [],
)

cc_library(
    name = "parallel_quick_sort",
    srcs = ["//include/synchronizing_concurrent_operations:parallel_quick_sort.h"],
    hdrs = ["//include/synchronizing_concurrent_operations:parallel_quick_sort.h"],
    copts = package_copt,
    strip_include_prefix = "//include",
    deps = [],
)
