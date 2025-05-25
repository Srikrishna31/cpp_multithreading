load("@//:config.bzl", "package_copt")
load("@//:convenience_def.bzl", "generate_alias_targets")
load("@//src/atomic:targets.bzl", "atomic_binary_target_list")
load("@//src/synchronizing_concurrent_operations:targets.bzl", "sync_concurrent_op_binary_target_list")

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

generate_alias_targets(
    atomic_binary_target_list,
    "//src/atomic",
)

generate_alias_targets(
    sync_concurrent_op_binary_target_list,
    "//src/synchronizing_concurrent_operations",
)

alias(
    name = "spinlock",
    actual = "//src/atomic:spinlock",
)

alias(
    name = "message_queue",
    actual = "//include/message_queue:message_queue",
)

alias(
    name = "parallel_quick_sort",
    actual = "//src/synchronizing_concurrent_operations:parallel_quick_sort",
)

alias(
    name = "sequential_quick_sort",
    actual = "//src/synchronizing_concurrent_operations:sequential_quick_sort",
)

alias(
    name = "threadsafe_queue",
    actual = "//src/synchronizing_concurrent_operations:threadsafe_queue",
)

alias(
    name = "atm_example",
    actual = "//src/atm_example:atm_example",
)

alias(
    name = "thread_fizzbuzz",
    actual = "//src/modern_cpp_concurrency_udemy:thread_fizzbuzz",
)

alias(
    name = "data_race",
    actual = "//src/modern_cpp_concurrency_udemy:data_race",
)

alias(
    name = "deadlock",
    actual = "//src/modern_cpp_concurrency_udemy:deadlock",
)

alias(
    name = "thread_local_example",
    actual = "//src/modern_cpp_concurrency_udemy:thread_local_example",
)

alias(
    name = "lazy_initialization",
    actual = "//src/modern_cpp_concurrency_udemy:lazy_initalization",
)

alias(
    name = "dining_philosophers_problem",
    actual = "//src/modern_cpp_concurrency_udemy:dining_philosophers_problem",
)
