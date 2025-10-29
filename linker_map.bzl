load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "use_cpp_toolchain")
load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")

MapFileInfo = provider(
    doc = "A simple structure which wraps cc_binary and cc_library targets, and also provides the map file path as output",
    fields = {
        "default_info": "The default output coming out of the target",
        "map_file": "The generated map file",
    },
)

def _collect_files(linker_inputs):
    res = []
    for input in linker_inputs:
        for lib in input.libraries:
            res.extend([obj.path for obj in lib.objects])
            res.extend([lib.path for lib in lib.libraries])
            res.extend([dlib.path for dlib in lib.dynamic_libraries])
            res.extend([slib.path for slib in lib.static_libraries])

        # res.extend(input.objects)
        # res.extend(input.libraries)
        # res.extend(input.dynamic_libraries)
        # res.extend(input.static_libraries)
    return res

def _map_file_aspect_impl(target, ctx):
    if CcInfo not in target:
        print("Returning from aspect empty handed")
        return []

    cc_info = target[CcInfo]
    linker_inputs = cc_info.linking_context.linker_inputs.to_list()

    map_file = ctx.actions.declare_file(ctx.label.name + ".map")

    toolchain = ctx.toolchains["@bazel_tools//tools/cpp:toolchain_type"]
    linker_path = toolchain.cc.ld_executable

    print("Generating linker map file for {}".format(map_file.path))
    print("Input files: {}".format([f.path for f in linker_inputs]))
    print("Collecting files: {}".format(_collect_files(linker_inputs)))

    ctx.actions.run_shell(
        outputs = [map_file],
        inputs = depset(linker_inputs),
        command = "{} -shared -o /dev/null -Map={} {}".format(
            linker_path,
            map_file.path,
            " ".join([f.path for f in linker_inputs])
            # " ".join([_collect_files(linker_inputs)]
        ),
        mnemonic = "GenerateLinkerMap",
        progress_message = "Generating linker map file for {}".format(target.label),
    )
    return [DefaultInfo(files=depset([map_file]))]

# To run the aspect, use the following command:
# bazel build odr --aspects=//:linker_map.bzl%map_file_aspect --verbose_failures
map_file_aspect = aspect(
    implementation = _map_file_aspect_impl,
    attr_aspects = ["deps"],
    required_providers = [CcInfo],
    toolchains = ["@bazel_tools//tools/cpp:toolchain_type"],
)
