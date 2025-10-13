# C++ Mulithreading
A repository that contains code snippets from the book: C++ Concurrency in Action.

# TODO
1. Create a bazel architecture diagram. 
2. Explain the functions in convenience_def.bzl, targets.bzl in each bazel package.
3. Define a workflow for adding a new bazel package.
4. Define a workflow for adding a new file to an existing bazel package.


# C++ Multithreading

A comprehensive collection of C++ multithreading examples and utilities, demonstrating modern C++ concurrency features including atomic operations, memory ordering, synchronization primitives, and concurrent data structures.

## 🚀 Features

- **Atomic Operations**: Examples of various memory ordering models
    - Sequential consistency
    - Relaxed ordering
    - Acquire-release semantics
    - Memory fences
    - Release sequences
    - Transitive synchronization

- **Synchronization Primitives**
    - Custom spinlock implementation
    - Atomic-based synchronization mechanisms
    - Timer utilities

- **Build System**
    - Bazel-based build system with Bzlmod support
    - Custom symbol checker for ODR (One Definition Rule) violation detection
    - Python-based build tooling with libclang integration

- **Cross-Platform Support**: Works on Linux, macOS, and Windows

## 📋 Prerequisites

### Linux / macOS

- **Bazel**: Version specified in `.bazelversion` (or use Bazelisk)
- **C++ Compiler**: C++17 or later (GCC 7+, Clang 5+)
- **Python**: 3.12 (for build tools)
- **libclang**: For symbol checking (installed via pip)

### Windows

- **Bazel**: Version specified in `.bazelversion` (or use Bazelisk)
- **C++ Compiler**: One of the following:
    - **MSVC** (Visual Studio 2017 or later)
    - **Clang** (LLVM for Windows)
    - **MinGW-w64** (GCC for Windows)
- **Python**: 3.12 (for build tools)
- **libclang**: For symbol checking (installed via pip)

## 🛠️ Installation

### Linux

#### 1. Install Bazelisk

bash wget https://github.com/bazelbuild/bazelisk/releases/latest/download/bazelisk-linux-amd64 -O bazelisk chmod +x bazelisk sudo

#### 2. Install Python Dependencies

bash pip install -r requirements.txt

### macOS

#### 1. Install Bazelisk

bash brew install bazelisk

#### 2. Install Python Dependencies

bash pip3 install -r requirements.txt

### Windows

#### 1. Install Bazelisk

**Option A: Using Chocolatey**

powershell choco install bazelisk

**Option B: Manual Installation**

powershell

### Download Bazelisk
ProgressPreference = 'SilentlyContinue' Invoke-WebRequest -Uri "https://github.com/bazelbuild/bazelisk/releases/latest/download/bazelisk-windows-amd64.exe" -OutFile "env:USERPROFILE\bazelisk.exe"

### Add to PATH (run as Administrator or add manually to user PATH)

userPath = [Environment]::GetEnvironmentVariable("Path", "User") [Environment]::SetEnvironmentVariable("Path", "userPath;$env:USERPROFILE", "User")

### Rename to bazel.exe for convenience

Rename-Item "env:USERPROFILE\bazelisk.exe" "env:USERPROFILE\bazel.exe"

#### 2. Install Python and Dependencies

powershell

### Install Python 3.12 from python.org or via winget

winget install Python.Python.3.12

### Install dependencies

```shell
pip install -r requirements.txt
```
#### 3. Install Visual Studio Build Tools (Recommended)

powershell

### Download and install Visual Studio Build Tools 2022

```powershell
winget install Microsoft.VisualStudio.2022.BuildTools
```

### Or download from: https://visualstudio.microsoft.com/downloads/
### Make sure to install "Desktop development with C++" workload

#### 4. Configure Bazel for Windows (Optional)

Create or modify `.bazelrc` to specify compiler:


#### For MSVC
```bazel
build:windows --compiler=msvc-cl build:windows --cxxopt=/std:c++17
```

#### For Clang on Windows
```bazel
build:windows_clang --compiler=clang-cl build:windows_clang --cxxopt=-std=c++17
```

## 🏗️ Building

#### Build All Targets
```
bazel build //...
```
#### Build Specific Examples

### Build atomic examples
```
bazel build //src/atomic:all
```

### Build synchronization examples

```
bazel build //src/synchronizing_concurrent_operations:all
```

#### Using Specific Configuration

##### Build with MSVC

```
bazel build --config=windows //...
```

#### Build with Clang
```
bazel build --config=windows_clang //...
```

## 🧪 Running Examples

#### Atomic Operations Examples

##### Sequential consistency
```
bazel run //src/atomic:sequential_consistency
```

##### Relaxed ordering

```
bazel run //src/atomic:relaxed_ordering
```

##### Acquire-release semantics
```
bazel run //src/atomic:relaxed_ordering_acquire_release
```

##### Spinlock demonstration
```bazel run //src/atomic:spinlock```

##### Memory fences
```bazel run //src/atomic:fences```

#### Synchronization Examples
```
bash bazel run //src/synchronizing_concurrent_operations:timer
```

## 📂 Project Structure

```
cpp_multithreading/
├── include/              # Public headers
│   ├── atomic/          # Atomic utilities (spinlock, etc.)
│   ├── message_queue/   # Message queue implementations
│   └── timer.h          # Timer utilities
├── src/                 # Source files
│   ├── atomic/          # Atomic operation examples
│   ├── synchronizing_concurrent_operations/  # Sync examples
│   ├── modern_cpp_concurrency_udemy/        # Course materials
│   └── odr_check/       # ODR violation checking
├── test/                # Test files
├── BUILD                # Root BUILD file
├── MODULE.bazel         # Bazel module definition
├── .bazelrc             # Bazel configuration
├── .bazelversion        # Bazel version specification
├── symbol_checker.bzl   # Custom Bazel rules for ODR checking
├── symbol_checker.py    # ODR violation detector
└── requirements.txt     # Python dependencies
```

# 🏛️ Bazel Architecture
This project uses a sophisticated Bazel build architecture leveraging modern Bzlmod, custom rules, and aspects for 
advanced build capabilities.

## Overview
```
┌─────────────────────────────────────────────────────────────┐
│                    MODULE.bazel (Bzlmod)                    │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │
│  │  GoogleTest  │  │ rules_python │  │  platforms   │       │
│  └──────────────┘  └──────────────┘  └──────────────┘       │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                      Root BUILD file                        │
│  - Package-level configurations                             │
│  - Exported files (spinlock.h)                              │
│  - Global visibility rules                                  │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼ 
                            
┌─────────────────────────────────────────────────────────────┐
│                   Custom Bazel Rules                        │
│  ┌──────────────────────────────────────────────────────┐   │
│  │  symbol_checker.bzl                                  │   │
│  │  ├─ source_aspect: Collects compilation context      │   │
│  │  ├─ check_symbols: Validates ODR compliance          │   │
│  │  └─ Custom providers: SourceInfo                     │   │
│  └──────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────┐   │
│  │  convenience_def.bzl                                 │   │
│  │  └─ generate_cc_binary_targets: Macro for binaries   │   │
│  └──────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────┐   │
│  │  config.bzl                                          │   │
│  │  └─ package_copt: Compiler options                   │   │
│  └──────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│              Package-level BUILD files                      │
│  ┌──────────────────────┐  ┌──────────────────────┐         │
│  │  src/atomic/BUILD    │  │  include/BUILD       │         │
│  │  - cc_binary targets │  │  - cc_library targets│         │
│  │  - cc_library        │  │  - Header exports    │         │
│  │  - Uses targets.bzl  │  │                      │         │
│  └──────────────────────┘  └──────────────────────┘         │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                   Build Execution Flow                      │
│  1. Parse MODULE.bazel → Resolve dependencies               │
│  2. Load BUILD files → Expand macros                        │
│  3. Apply aspects → Collect metadata                        │
│  4. Execute actions → Compile, link, validate               │
│  5. Run symbol_checker → Detect ODR violations              │
└─────────────────────────────────────────────────────────────┘
```

### Key Components
#### 1. (Bzlmod - Modern Dependency Management) **MODULE.bazel**

```python
module(name = "cpp_multithreading", version = "0.9")

# External dependencies
bazel_dep(name = "googletest", version = "1.16.0", repo_name = "gtest")
bazel_dep(name = "rules_python", version = "0.40.0")
bazel_dep(name = "platforms", version = "0.0.11")

# Python toolchain
python = use_extension("@rules_python//python/extensions:python.bzl", "python")
python.toolchain(python_version = "3.12")

# Python dependencies via pip
pip = use_extension("@rules_python//python/extensions:pip.bzl", "pip")
pip.parse(
    hub_name = "pip",
    python_version = "3.12",
    requirements_lock = "//:requirements.txt",
)
```

**Benefits**:
- Hermetic builds with locked dependencies
- No WORKSPACE file complexity
- Version resolution handled automatically
- Better caching and reproducibility

#### 2. **Custom Aspects** () `symbol_checker.bzl`
Aspects allow traversal of the dependency graph to collect information:
```python
source_aspect = aspect(
    implementation = _collect_sources_aspect,
    attr_aspects = ["deps"],
    required_providers = [CcInfo],
    provides = [SourceInfo],
)
```

What it does:
* Traverses the dependency graph of C++ targets
* Collects source files, headers, include paths
* Extracts compiler arguments and defines
* Provides compilation context to downstream rules

Flow:

```
Target → source_aspect → CcInfo → SourceInfo Provider
                       ↓
              Collect: srcs, includes, copts, defines
```

#### 3. **Custom Rules** () `check_symbols`

```python
check_symbols = rule(
    implementation = _symbol_checker_impl,
    attrs = {
        "target": attr.label(aspects = [source_aspect]),
        "_generator": attr.label(default = Label("symbol_checker_binary")),
    },
    outputs = {"validation": "%{name}_validation.txt"},
)
```

##### Execution Flow:
1. Aspect Phase: Collect compilation info from target
2. Manifest Generation: Create JSON with all source/header paths
3. Workspace Path Resolution: Determine absolute paths
4. Python Script Execution: Run [symbol_checker.py](symbol_checker.py) with libclang
5. Validation Output: Generate report of ODR violations

##### Example Usage:

```bazel
check_symbols(
    name = "odr_check_symbols",
    target = "//src/atomic:spinlock",
)
```

### 4. Convenience Macros ([convenience_def.bzl](convenience_def.bzl))

```python
def generate_cc_binary_targets(target_list):
    """Generate multiple cc_binary targets from a list"""
    for target in target_list:
        cc_binary(
            name = target,
            srcs = [target + ".cpp"],
            copts = package_copt,
            # ... other attributes
        )
```

**Used in** : [src/atomic/targets.bzl](src/atomic/targets.bzl)
```python
atomic_binary_target_list = [
    "sequential_consistency",
    "relaxed_ordering",
    "spinlock",
    # ... more targets
]
```

#### 5. Configuration Management ([.bazelrc](.bazelrc))
Hierarchical configuration system:
```
# Global settings
build --cxxopt=-std=c++17
build --compilation_mode=opt

# Platform-specific configs
build:linux --cxxopt=-pthread
build:windows --compiler=msvc-cl
build:macos --cxxopt=-stdlib=libc++

# Feature configs
build:debug --compilation_mode=dbg
build:verbose --subcommands
```

**Usage**:

```
bazel build --config=windows --config=debug //...
```

#### 6. **Build Target Organization**

```
//src/atomic/BUILD
├── cc_library(name = "spinlock")
│   ├── srcs: ["spinlock.cpp"]
│   ├── hdrs: ["//include/atomic:spinlock.h"]
│   └── copts: package_copt
│
├── cc_binary(name = "sequential_consistency")
│   ├── srcs: ["sequential_consistency.cpp"]
│   └── deps: [":spinlock"]
│
└── Generated via generate_cc_binary_targets()
    from atomic_binary_target_list
```

#### 7. **Symbol Checker Architecture**

```
┌──────────────────────────────────────────────────────────┐
│                    check_symbols Rule                    │
└──────────────────────────────────────────────────────────┘
                         │
                         ▼
┌──────────────────────────────────────────────────────────┐
│              source_aspect (Aspect)                      │
│  Collects: srcs, includes, copts, defines from CcInfo    │
└──────────────────────────────────────────────────────────┘
                         │
                         ▼
┌──────────────────────────────────────────────────────────┐
│             Generate Manifest JSON                       │
│  {                                                       │
│    "srcs": [...],                                        │
│    "includes": [...],                                    │
│    "args": [...],                                        │
│    "defines": [...]                                      │
│  }                                                       │
└──────────────────────────────────────────────────────────┘
                         │
                         ▼
┌──────────────────────────────────────────────────────────┐
│          Execute symbol_checker.py                       │
│  1. Parse manifest                                       │
│  2. Use libclang to parse C++ files                      │
│  3. Extract symbol definitions                           │
│  4. Detect duplicates across TUs                         │
│  5. Generate validation report                           │
└──────────────────────────────────────────────────────────┘
                         │
                         ▼
┌──────────────────────────────────────────────────────────┐
│            Validation Output                             │
│  - Success: "No ODR violations found"                    │
│  - Failure: List of duplicate symbols with locations     │
└──────────────────────────────────────────────────────────┘
```

### Build Graph Example
For target `//src/atomic:spinlock`:

```
check_symbols(name = "odr_check_symbols")
    │
    ├─→ source_aspect applied to "//src/atomic:spinlock"
    │       │
    │       ├─→ Collect srcs: ["spinlock.cpp"]
    │       ├─→ Collect hdrs: ["//include/atomic:spinlock.h"]
    │       ├─→ Collect copts: ["-std=c++17", ...]
    │       └─→ Provide SourceInfo
    │
    ├─→ Generate manifest JSON
    │
    ├─→ Extract workspace path
    │
    └─→ Run symbol_checker_binary
            │
            ├─→ Input: manifest.json, workspace_path.txt
            ├─→ Parse C++ with libclang
            ├─→ Detect ODR violations
            └─→ Output: validation.txt
```

#### Hermeticity
* All dependencies locked in MODULE.bazel.lock
* Python version pinned to 3.12
* Reproducible builds across machines

#### Incremental Builds
* Action caching based on input hashes
* Only rebuilds affected targets
* Remote caching support (optional)

#### Cross-Platform Support
* Platform-specific toolchains auto-selected
* Unified build commands across OS
* Conditional compilation via select()

#### Extensibility
* Easy to add new custom rules
* Aspect system for metadata collection
* Macro system for code generation

### Build Performance Tips

```
# Use remote cache
bazel build --remote_cache=grpc://cache-server:9092 //...

# Parallel execution
bazel build --jobs=12 //...

# Build only changed targets
bazel build --keep_going //...

# Profile build
bazel build --profile=profile.json //...
bazel analyze-profile profile.json
```

### Directory Layout Philosophy
```
# Use remote cache
bazel build --remote_cache=grpc://cache-server:9092 //...

# Parallel execution
bazel build --jobs=12 //...

# Build only changed targets
bazel build --keep_going //...

# Profile build
bazel build --profile=profile.json //...
bazel analyze-profile profile.json
```

### Directory Layout Philosophy

```
/include/     → Public API headers (installed)
/src/         → Implementation files (not installed)
/test/        → Test files
/bazel-*      → Build outputs (gitignored)
```

This separation allows:
- Clean public API surface
- Internal implementation details hidden
- Easy header installation for external use

## 🔍 Symbol Checker (ODR Violation Detection)
This project includes a custom Bazel rule for detecting One Definition Rule violations across compilation units.
### Usage
```python
# In your BUILD file
load("//:symbol_checker.bzl", "check_symbols")

check_symbols(
    name = "odr_check_symbols",
    target = "//src/atomic:your_target",
)
```

The symbol checker:
- Parses C++ source files using libclang
- Collects symbol definitions across translation units
- Detects duplicate definitions that violate ODR
- Outputs validation reports

## 📚 Topics Covered
### Memory Ordering Models
1. **Sequential Consistency**: Default, strongest ordering guarantees
2. **Relaxed Ordering**: No synchronization, only atomicity
3. **Acquire-Release**: Synchronization between threads
4. **Consume**: Data-dependency ordering (deprecated in C++17)
5. **Release Sequences**: Chain of read-modify-write operations

### Synchronization Primitives
- Custom spinlock implementation using `std::atomic_flag`
- Atomic boolean synchronization
- Timer utilities for performance measurement

### Advanced Concepts
- Transitive synchronization
- Impose ordering in acquire-release
- Multi-threaded relaxed ordering
- Memory fences and barriers

## 🧰 Bazel Configuration
### Custom Build Rules
- [check_symbols](symbol_checker.bzl): ODR violation detection 
- [source_aspect](symbol_checker.bzl): Compilation context collection

### Configuration Files
- [.bazelrc](.bazelrc): Build configuration options 
- [.bazelversion](.bazelversion): Specifies Bazel version 
- [MODULE.bazel](MODULE.bazel): External dependencies (googletest, rules_python) 

## 🐛 Troubleshooting
### Windows-Specific Issues
#### Issue: "MSVC not found"
**Solution**: Install Visual Studio Build Tools 2022 with C++ workload:
```powershell
winget install Microsoft.VisualStudio.2022.BuildTools
```
#### Issue: Python script fails with "No module named 'clang'"
**Solution**: Install libclang:
```shell
pip install libclang
```

#### Issue: Long path errors
**Solution**: Enable long paths in Windows:

```powershell
# Run as Administrator
New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem" -Name "LongPathsEnabled" -Value 1 -PropertyType DWORD -Force 
```

Or add to : `.bazelrc`
```startup --output_user_root=C:/tmp/bazel```

### Linux/macOS Issues
#### Issue: "Permission denied" when running bazelisk
**Solution**:
```shell
chmod +x bazelisk
```

#### Issue: libclang not found
**Solution**:

```shell
# Ubuntu/Debian
sudo apt-get install libclang-dev

# macOS
brew install llvm

# Or via pip
pip install libclang
```

### Bazel-Specific Issues
#### Issue: "Module not found" errors
**Solution**: Clean and refetch modules:
```shell
bazel clean --expunge
bazel sync --only=pip
```

#### Issue: Aspect not applying
**Solution**: Check that target provides `CcInfo`:
```python
# Target must be cc_library or cc_binary
cc_library(name = "my_lib", ...)  # ✓ Works
filegroup(name = "my_files", ...) # ✗ Doesn't work
```

#### Issue: Symbol checker fails silently
**Solution**: Run with verbose output:

```shell
 bazel build --subcommands --verbose_failures //:odr_check_symbols
```

## 🤝 Contributing
Contributions are welcome! Please ensure:
1. Code follows C++17 standards
2. All examples are well-documented
3. Bazel builds pass on Linux, macOS, and Windows
4. Test on multiple platforms if possible
5. Update BUILD files for new targets
6. Add new targets to appropriate lists `targets.bzl`

## 📝 License
See [LICENSE](LICENSE) file for details.
## 🎓 Learning Resources
This project includes examples from various sources including:
- Modern C++ Concurrency course materials (Udemy)
- C++ Concurrency in Action by Anthony Williams
- ISO C++ standards documentation
- cppreference.com for C++ memory model
- Bazel documentation for build system architecture

## ⚠️ Platform-Specific Notes
### Windows
- Atomic operations on Windows use the Windows Memory Model
- Some examples may show different performance characteristics on Windows vs Linux
- MSVC may have different optimization behavior compared to GCC/Clang

### Linux
- Best performance typically achieved with recent kernel versions (5.x+)
- CPU-specific optimizations may be available (use `-march=native` if needed)

### macOS
- M1/M2 (ARM) chips may show different behavior due to architecture differences
- Performance characteristics differ from x86_64

## 📧 Contact
For questions or issues, please open a GitHub issue.
**Happy Concurrent Programming!** 🚀