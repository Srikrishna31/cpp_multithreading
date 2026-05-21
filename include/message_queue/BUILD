load("@//:config.bzl", "package_copt")
load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "message_queue",
    srcs = [],
    hdrs = [
        "dispatcher.h",
        "message_queue.h",
        "receiver.h",
        "sender.h",
        "template_dispatcher.h",
    ],
    copts = package_copt,
    visibility = ["//visibility:public"],
)
