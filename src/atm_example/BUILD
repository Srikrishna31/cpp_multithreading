load("@//:config.bzl", "package_copt")
load("@rules_cc//cc:defs.bzl", "cc_binary")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "atm_example",
    srcs = [
        "atm.cpp",
        "atm.h",
        "bank_machine.cpp",
        "bank_machine.h",
        "interface_machine.cpp",
        "interface_machine.h",
        "main.cpp",
        "messages.h",
    ],
    copts = package_copt,
    deps = [
        "//include/message_queue",
    ],
)
