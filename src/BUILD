load("@//:config.bzl", "package_copt")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "timer",
    srcs = ["timer.cpp"],
    hdrs = ["//include:timer.h"],
    copts = package_copt,
    visibility = ["//visibility:public"],
    deps = [],
)

cc_binary(
    name="getlastof",
    srcs=["get_last_of.cpp"],
    copts=package_copt,
    visibility = ["//visibility:public"]
)
cc_library(
    name = "result",
    hdrs = ["result.h"],
    copts = package_copt,
    visibility = ["//visibility:public"],
    deps = [],
)
