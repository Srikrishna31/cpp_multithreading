package(default_visibility = ["//visibility:public"])

cc_library(
name = "odr_lib",
srcs = ["odr1.cpp", "odr2.cpp"],
hdrs = ["odr.h"],
)

cc_binary(
name = "odr",
srcs = ["main.cpp"],
deps = [":odr_lib"],

)
