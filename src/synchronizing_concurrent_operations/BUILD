load("@//:config.bzl", "package_copt")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "threadsafe_queue",
    srcs = ["thread_safe_queue.h"],
    copts = package_copt,
    deps = [],
)

cc_binary(
    name = "future_example",
    srcs = ["future_example.cpp"],
    copts = package_copt,
    deps = [],
)

cc_binary(
    name = "packaged_task_example",
    srcs = ["packaged_task_example.cpp"],
    copts = package_copt,
    deps = [],
)

cc_binary(
    name = "shared_future_example",
    srcs = ["shared_future_example.cpp"],
    copts = package_copt,
    deps = [],
)

cc_binary(
    name = "waiting_timeout_cv",
    srcs = ["waiting_timeout_cv.cpp"],
    copts = package_copt,
    deps = [],
)

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

cc_binary(
    name = "test_quick_sort",
    srcs = ["test_quick_sort.cpp"],
    deps = [
        ":parallel_quick_sort",
        ":sequential_quick_sort",
    ],
)
