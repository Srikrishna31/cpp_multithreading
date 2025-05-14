load("@//:config.bzl", "package_copt")

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
    ],
    copts = package_copt,
    deps = [
        "//include/message_queue",
    ],
)
