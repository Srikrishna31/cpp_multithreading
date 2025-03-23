package_copt = select({
    "@platforms//os:windows": ["/std:c++20"],
    "//conditions:default": ["-std=c++20"],
})
