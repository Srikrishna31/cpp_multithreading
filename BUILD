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

alias(
    name = "spinlock",
    actual = "//src/atomic:spinlock",
)

alias(
    name = "fences",
    actual = "//src/atomic:fences",
)

alias(
    name = "relaxed_orderign_acquire_release_transitive_synchronization",
    actual = "//src/atomic:relaxed_orderign_acquire_release_transitive_synchronization",
)

alias(
    name = "relaxed_ordering",
    actual = "//src/atomic:relaxed_ordering",
)

alias(
    name = "relaxed_ordering_acquire_release",
    actual = "//src/atomic:relaxed_ordering_acquire_release",
)

alias(
    name = "relaxed_ordering_acquire_release_impose_ordering",
    actual = "//src/atomic:relaxed_ordering_acquire_release_impose_ordering",
)

alias(
    name = "relaxed_ordering_consume",
    actual = "//src/atomic:relaxed_ordering_consume",
)

alias(
    name = "relaxed_ordering_multiple_threads",
    actual = "//src/atomic:relaxed_ordering_multiple_threads",
)

alias(
    name = "release_sequence_chain",
    actual = "//src/atomic:release_sequence_chain",
)

alias(
    name = "sequential_consistency",
    actual = "//src/atomic:sequential_consistency",
)

alias(
    name = "sync_with_atomic_bool",
    actual = "//src/atomic:sync_with_atomic_bool",
)

alias(
    name = "message_queue",
    actual = "//include/message_queue:message_queue",
)

alias(
    name = "future_example",
    actual = "//src/synchronizing_concurrent_operations:future_example",
)

alias(
    name = "packaged_task_example",
    actual = "//src/synchronizing_concurrent_operations:packaged_task_example",
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
    name = "shared_future_example",
    actual = "//src/synchronizing_concurrent_operations:shared_future_example",
)

alias(
    name = "test_quick_sort",
    actual = "//src/synchronizing_concurrent_operations:test_quick_sort",
)

alias(
    name = "threadsafe_queue",
    actual = "//src/synchronizing_concurrent_operations:threadsafe_queue",
)

alias(
    name = "waiting_timeout_cv",
    actual = "//src/synchronizing_concurrent_operations:waiting_timeout_cv",
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
