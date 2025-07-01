load("@//:config.bzl", "package_copt")

_EXPECTED_ENDINGS = ["cpp", "cc", "c", "cxx", "h", "hpp", "hxx"]

# https://stackoverflow.com/questions/48510907/how-to-apply-a-macro-to-an-array-of-names-of-targets-in-bazel
# A convenience rule to define a cc_binary target for each file/name contained in the list.
def generate_cc_binary_targets(files):
    for f in files:
        # https://stackoverflow.com/questions/64201175/name-cc-library-is-not-defined
        # Inside a .bzl file the builtin definitions need to be qualified as native.cc_binary
        native.cc_binary(
            name = _get_target_name(f),
            srcs = [f],
            copts = package_copt,
            deps = [],
        )

def generate_alias_targets(files, path_to_add):
    for f in files:
        name = _get_target_name(f)
        native.alias(
            name = name,
            actual = path_to_add + ":" + name,
        )

def _get_target_name(file_name):
    file_parts = file_name.split(".")
    if file_parts[1] in _EXPECTED_ENDINGS:
        return file_parts[0]
    else:
        fail("Only C/C++ source files supported for generating targets from filenames")
