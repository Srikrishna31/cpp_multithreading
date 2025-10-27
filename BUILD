load("@//:config.bzl", "package_copt")
load("@//:convenience_def.bzl", "generate_alias_targets")
load("@//src/atomic:targets.bzl", "atomic_binary_target_list")
load("@//src/modern_cpp_concurrency_udemy:targets.bzl", "modern_cpp_concurrency_udemy_binary_target_list")
load("@//src/synchronizing_concurrent_operations:targets.bzl", "sync_concurrent_op_binary_target_list")
load(":symbol_checker.bzl", "check_symbols")

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
    name = "odr_check",
    actual = "//src/odr_check:odr_check",
)

generate_alias_targets(modern_cpp_concurrency_udemy_binary_target_list, "//src/modern_cpp_concurrency_udemy")

py_binary(
    name = "symbol_checker_binary",
    srcs = ["symbol_checker.py"],
    main = "symbol_checker.py",
    deps = [
        "@pip//libclang",
    ],
)

check_symbols(
    name = "odr_check_symbols",
    target = ":odr_check",
)

alias (
    name = "odr",
    actual = "//src/odr",
)
