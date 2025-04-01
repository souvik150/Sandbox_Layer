# WASI Sandboxed Layer Sample Project

This project demonstrates a sample implementation of a sandbox layer for WASI file I/O operations in C++. The goal is to ensure that file reads and writes are performed only within a specified sandbox directory. The project replaces direct system calls (such as using `std::ifstream`/`std::ofstream` or `std::filesystem::canonical`) with a secure wrapper that resolves file paths using a “lexically normal” resolution strategy. This guarantees that even when writing new files (which may not yet exist), the operations remain confined to a trusted filesystem root.

## Overview

The WASI proposal guarantees security and isolation by limiting access to the host filesystem. However, if guest WebAssembly code invokes host functions for file access without going through the WASI APIs, the sandbox can be bypassed. This project creates a secure layer that intercepts such operations and checks that the target file is within the sandbox directory. In our implementation:

- The sandbox uses `std::filesystem::absolute()` and `lexically_normal()` to compute paths without requiring that the file exist.
- If a file is outside the sandbox or the check fails, an error code (77, meaning "NotAllowed") is returned.
- Both read and write operations are wrapped to enforce this policy.

## Folder Structure

```
wasi_sandbox/
├── CMakeLists.txt
├── README.md
├── include/
│   └── sandbox.h
├── src/
│   ├── sandbox.cpp
│   ├── main.cpp
│   └── wasi_layer.cpp  # Contains file I/O wrappers using the sandbox
└── sandbox_dir/        # The designated sandbox root directory (created or provided)
```

- **include/sandbox.h**: Declares the `Sandbox` class interface.
- **src/sandbox.cpp**: Implements the sandbox functions (path resolution, checking allowed paths).
- **src/wasi_layer.cpp**: Contains the WASI file I/O functions (read/write) that call into the sandbox layer.
- **src/main.cpp**: A simple test driver that initializes the sandbox, performs file I/O via the WASI layer, and prints results.
- **CMakeLists.txt**: Build script using CMake.

## Build Instructions

This project uses CMake. To build the project, run the following commands in a terminal:

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

The built executable (for example, `wasi_sandbox`) will be placed in the build directory.

## Running the Sandbox

Before running, ensure that you have a valid sandbox directory. For example, you may use the provided `sandbox_dir/` folder. Then run:

```bash
./wasi_sandbox
```

The sample project will:

- Initialize the sandbox with the root set to `sandbox_dir/`
- Attempt to read and write files using the secure WASI layer
- Report any errors (e.g. if a file is attempted to be written outside the sandbox)

An error such as:

```
Sandbox root: "/home/user/wasi_sandbox/sandbox_dir"
Error writing file. Errno: 77
```

indicates that the file path was rejected because it did not lie within the sandbox.

## How the Sandboxed WASI Layer Works

### Sandbox Path Resolution

The `Sandbox` class (in `sandbox.h` and `sandbox.cpp`) constructs a canonical sandbox root. When checking an input path, it uses:

```cpp
auto normPath = std::filesystem::absolute(path).lexically_normal();
```

This avoids the pitfalls of `std::filesystem::canonical()`, which requires the file to exist. The check then confirms that `normPath` begins with the sandbox root.

### File I/O Wrappers

The WASI layer functions (in `wasi_layer.cpp`) wrap typical file I/O operations. Instead of calling system APIs directly, they call the sandbox’s `resolve()` method to obtain an allowed path, then perform the read or write.

#### Writing a File

When writing, the function first calls `Sandbox::resolve(relPath)` to generate an absolute path and then checks with `Sandbox::isAllowed()`. If the file does not exist yet, the `lexically_normal` path is used to allow file creation within the sandbox.

#### Reading a File

Similarly, file reads first resolve the file path and check that it lies inside the sandbox before opening.

### Error Handling

If a file operation fails the sandbox check, the function returns an error code (`77`, “NotAllowed”), ensuring that the host filesystem remains protected from unauthorized access.