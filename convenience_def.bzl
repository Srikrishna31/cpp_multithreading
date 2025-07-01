load("@//:config.bzl", "package_copt")

# https://stackoverflow.com/questions/48510907/how-to-apply-a-macro-to-an-array-of-names-of-targets-in-bazel
# A convenience rule to define a cc_binary target for each file/name contained in the list.
def generate_cc_binary_targets(files):
    for f in files:
        # https://stackoverflow.com/questions/64201175/name-cc-library-is-not-defined
        # Inside a .bzl file the builtin definitions need to be qualified as native.cc_binary
        native.cc_binary(
            name = f.replace(".cpp", ""),
            srcs = [f],
            copts = package_copt,
            deps = [],
        )

def generate_alias_targets(files, path_to_add):
    for f in files:
        native.alias(
            name = f.replace(".cpp", ""),
            actual = path_to_add + ":" + f.replace(".cpp", ""),
        )
