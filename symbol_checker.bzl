"""
Symbol Checker for Bazel C/C++ Targets

This module provides custom Bazel rules and aspects for detecting One Definition Rule (ODR)
violations in C/C++ projects. It uses libclang via Python to parse source files and identify
duplicate symbol definitions across translation units.

Key Components:
    - SourceInfo: Provider for compilation metadata
    - source_aspect: Aspect to collect compilation information
    - check_symbols: Rule to validate symbol definitions

Usage:
    load("//:symbol_checker.bzl", "check_symbols")

    check_symbols(
        name = "odr_check",
        target = "//src/atomic:spinlock",
    )
"""

VALIDATOR = "symbol_checker_binary"

SourceInfo = provider(
    doc = """A struct holding the compilation information for a target.

    This provider encapsulates all the information needed to compile and analyze
    a C/C++ target, including source files, headers, compiler arguments, and defines.
    It is populated by the source_aspect and consumed by the check_symbols rule.
    """,
    fields = {
         "trgt": "Fully qualified target label (e.g., '@//package:name')",
         "includes": "List of header files (File objects) from compilation context",
         "srcs": "List of source files (File objects) to be compiled",
         "framework_includes": "List of include path strings (system, quote, framework)",
         "compiler_args": "List of compiler options (copts) from the target",
         "defines": "List of preprocessor defines from the target",
    },
)

def _collect_sources_aspect(target, ctx):
    """Implementation function for the source_aspect.

    This aspect traverses the dependency graph of C/C++ targets and collects
    compilation information including sources, headers, include paths, compiler
    arguments, and preprocessor defines.

    Args:
        target: The target being analyzed (must provide CcInfo)
        ctx: The rule context for accessing attributes and providers

    Returns:
        A list containing a single SourceInfo provider with collected compilation data.
        Returns empty list if the target has no sources.
    """
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
    doc = """Aspect to collect compilation information from C/C++ targets.

    This aspect is applied to cc_library and cc_binary targets to collect
    all compilation-related information including sources, headers, include
    paths, compiler arguments, and defines. It traverses the dependency graph
    via the 'deps' attribute.

    The collected information is packaged into a SourceInfo provider that
    can be consumed by the check_symbols rule.
    """,
)

def _symbol_checker_impl(ctx):
    """Implementation function for the check_symbols rule.

        This rule orchestrates the symbol checking process:
        1. Extracts compilation info from the target (via source_aspect)
        2. Generates a JSON manifest with all compilation details
        3. Determines the workspace absolute path
        4. Runs the Python symbol checker script
        5. Produces a validation report

        Args:
            ctx: The rule context

        Returns:
            DefaultInfo provider with the validation output file

        Outputs:
            - <target>_symbol_info.json: Manifest with compilation details
            - <target>_workspace_path.txt: Absolute workspace path
            - <name>_validation.txt: Symbol validation report
    """
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

check_symbols = rule(
    implementation = _symbol_checker_impl,
    attrs = {
        "target": attr.label(
            doc = """Target for which symbols are to be checked.

            This should be a cc_library or cc_binary target. The source_aspect
            will be applied to this target to collect compilation information.
            """,
            mandatory = True,
            allow_files = False,
            aspects = [source_aspect],
        ),
        "_generator": attr.label(
            default = Label(VALIDATOR),
            allow_files = True,
            executable = True,
            cfg = "exec",
            doc = """The symbol checker script (symbol_checker.py).

            This is the Python script that performs the actual symbol analysis
            using libclang. It's built as symbol_checker_binary.
            """,        ),
    },
    outputs = {"validation": "%{name}_validation.txt"},
    doc = """A rule to check for symbol conflicts in C/C++ targets.

    This rule detects One Definition Rule (ODR) violations by analyzing
    symbol definitions across translation units using libclang.

    Example:
        check_symbols(
            name = "odr_check_spinlock",
            target = "//src/atomic:spinlock",
        )

    The rule will:
        1. Collect compilation info from the target (via source_aspect)
        2. Generate a manifest JSON file
        3. Run symbol_checker.py to analyze symbols
        4. Produce a validation report

    Output:
        <name>_validation.txt: Report listing any ODR violations found
    """,
)

def _build_arguments(info_path, output_path, workspace_path_file):
    return [
        "--manifest_file",
        info_path,
        "--validation_file",
        output_path,
        "--workspace_path",
        workspace_path_file,
    ]
