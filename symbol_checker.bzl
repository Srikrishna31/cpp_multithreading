VALIDATOR = "symbol_checker"

SourceInfo = provider(
    doc = "A struct holding the compilation information for a target.",
    fields = ["trgt", "includes", "srcs", "framework_includes", "compiler_args", "defines"],
)

def _collect_sources_aspect(target, ctx):
    src_list = []
    compiler_args = []
    defines = []
    headers = []
    other_incs = []

    if hasattr(ctx.rule.attr, "srcs"):
        for src in ctx.rule.attr.srcs:
            if hasattr(src, "files"):
                src_list.extend(src.files.to_list())
            else:
                src_list.append(src.path)

        if CcInfo in target:
            cc_info = target[CcInfo]
            c_ctx = cc_info.compilation_context

            # Collect INCLUDE PATHS as other_incs (strings)
            other_incs.extend(c_ctx.includes.to_list())
            other_incs.extend(c_ctx.system_includes.to_list())
            other_incs.extend(c_ctx.quote_includes.to_list())

            # Collect HEADERS FILES as File objects (depset)
            headers = c_ctx.direct_public_headers + c_ctx.direct_private_headers + c_ctx.direct_textual_headers

            # Collect defines
            defines = c_ctx.defines.to_list() + c_ctx.local_defines.to_list()

        if hasattr(ctx.rule.attr, "copts"):
            compiler_args.extend(ctx.rule.attr.copts)

        return [
            SourceInfo(
                trgt = "@" + target.label.workspace_name + "//" + target.label.package + ":" + target.label.name,
                includes = headers,
                srcs = src_list,
                framework_includes = other_incs,
                compiler_args = compiler_args,
                defines = defines,
            ),
        ]

source_aspect = aspect(
    implementation = _collect_sources_aspect,
    attr_aspects = ["deps"],
    required_providers = [CcInfo],
    provides = [SourceInfo],
)

def _symbol_checker_impl(ctx):
    context = ctx.attr.target[SourceInfo]
    manifest = {
        "args": context.compiler_args,
        "defines": context.defines,
        "includes": [f.path for f in context.includes],
        "framework_includes": context.framework_includes,
        "srcs": [f.path for f in context.srcs],
        "tgt": context.trgt,
    }

    info = ctx.actions.declare_file("{}_symbol_info.json".format(ctx.attr.target.label.name))
    ctx.actions.write(output = info, content = json.indent(json.encode(manifest)))

    path_file = ctx.actions.declare_file("{}_workspace_path.txt".format(ctx.attr.target.label.name))
    src = context.srcs[0]
    ctx.actions.run_shell(
        outputs = [path_file],
        inputs = [src],
        mnemonic = "WriteWorkspacePath",
        progress_message = "Writing workspace path to file",
        command = """
        full_path="$(readlink -f -- "{src_full}")"
        echo "${{full_path%/{src_short}}}" >> {out_full}
        """.format(
            src_full = src.path,
            src_short = src.short_path,
            out_full = path_file.path,
        ),
        execution_requirements = {
            "local": "1",
            "no-remote": "1",
            "no-sandbox": "1",
        },
    )

    outputs = [ctx.actions.declare_file("{}_validation.txt".format(ctx.attr.name))]

    ctx.actions.run(
        inputs = context.includes + context.srcs + [info, path_file],
        outputs = outputs,
        mnemonic = "SymbolChecker",
        arguments = _build_arguments(info.path, outputs[0].path, path_file.path),
        executable = ctx.executable._generator,
    )

    return [
        DefaultInfo(
            files = depset(outputs),
        ),
    ]

#    all_sources = []
#    for dep in ctx.attr.deps:
#        if SourceInfo in dep:
#            all_sources.extend(dep[SourceInfo])
#
#    # Collect sources from the target itself
#    if SourceInfo in ctx.rule:
#        all_sources.extend(ctx.rule[SourceInfo])
#
#    # Now we have a list of SourceInfo structs in all_sources
#    # We can process them to find duplicates
#
#    symbol_map = {}
#    duplicates = {}
#
#    for source_info in all_sources:
#        trgt = source_info.trgt
#        includes = source_info.includes
#        srcs = source_info.srcs
#        framework_includes = source_info.framework_includes
#        compiler_args = source_info.compiler_args
#        defines = source_info.defines
#
#        for src in srcs:
#            symbol = src.split("/")[-1]  # Get the file name from the path
#            if symbol not in symbol_map:
#                symbol_map[symbol] = (trgt, src)
#            else:
#                if symbol not in duplicates:
#                    duplicates[symbol] = [symbol_map[symbol]]
#                duplicates[symbol].append((trgt, src))
#
#    if duplicates:
#        error_messages = []
#        for symbol, occurrences in duplicates.items():
#            message = "Symbol '{}' is defined in multiple targets:\n".format(symbol)
#            for trgt, src in occurrences:
#                message += "  - Target: {}, Source: {}\n".format(trgt, src)
#            error_messages.append(message)
#        fail("\n".join(error_messages))

check_symbols = rule(
    implementation = _symbol_checker_impl,
    attrs = {
        "target": attr.label(
            doc = "Target for which symbols are to be checked.",
            mandatory = True,
            allow_files = False,
            aspects = [source_aspect],
        ),
        "_generator": attr.label(
            default = Label(VALIDATOR),
            allow_files = True,
            executable = True,
            cfg = "exec",
            doc = "The symbol checker script.",
        ),
    },
    outputs = {"validation": "%{name}_validation.txt"},
    doc = "A rule to check for symbol conflicts in C/C++ targets.",
)

def _build_arguments(info_path, output_path, workspace_path_file):
    return [
        "--manifest_file",
        info_path,
        "--validation_file",
        output_path,
        "--workspace_path_file",
        workspace_path_file,
    ]
