load("@//:config.bzl", "package_copt")
load("@//:convenience_def.bzl", "generate_cc_binary_targets")
load(":targets.bzl", "atomic_binary_target_list")

package(default_visibility = ["//visibility:public"])

generate_cc_binary_targets(atomic_binary_target_list)

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
)
