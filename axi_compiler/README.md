# Axi (Axi)
**The Native Systems Language for the AI Era.**

[![License: AGPL v3](https://img.shields.io/badge/License-AGPL_v3-blue.svg)](https://www.gnu.org/licenses/agpl-3.0)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()
[![Platform: Windows | Linux | macOS](https://img.shields.io/badge/platform-cross--platform-lightgrey)]()

Axi (`.axi`) is a high-performance, strictly-typed systems programming language designed from the ground up to replace Python and C++ in AI orchestration, tensor math, and bare-metal hardware control.

Built on a Zero-GC deterministic memory model, Axi compiles directly to C and machine code, allowing you to interface with AI backends (like `llama.cpp`) natively—without the overhead of Python.

## Why Axi?

1. **Python-Free AI Orchestration:** Stop relying on Python wrappers. Axi allows you to hook directly into C/C++ FFI layers. Load, quantize, and execute AI models natively at C-speeds.
2. **The TOON 4D DAG:** Axi ships with a native, built-in version control system (DVCS). Instead of Git, Axi uses a Directed Acyclic Graph (DAG) serialized in **TOON** (Token-Oriented Object Notation). TOON is a highly-compressed, AST-friendly format designed specifically to reduce LLM context windows.
3. **Zero-GC & AVX-512 Ready:** Absolute control over memory. Axi is designed to eventually support bare-metal hardware acceleration for heavy tensor workloads.


## 🚀 Quickstart

1. Download the latest `axi_compiler_v3.1_public.exe` from the **Releases** tab.
2. Write your first script (`hello.axi`):
```axi
HOW TO start():
    WRITE "Hello from Axi!" TO SCREEN

start -> start
```
3. Compile and run natively:
```bash
axi compile hello.axi -o hello.exe
./hello.exe
```

---


## Example: Native Model Quantization
Here is how Axi handles AI model quantization natively, bypassing Python entirely.

```rust
import core::dag;
import ffi::llama_cpp;

pub fn main(args: string[]) {
    // 1. Hook directly into the C++ backend
    let quantize_params = llama_cpp::llama_model_quantize_default_params();
    quantize_params.ftype = llama_cpp::LLAMA_FTYPE_MOSTLY_Q4_K_M;

    // 2. Quantize the model
    llama_cpp::llama_model_quantize("model.bin", "model.gguf", quantize_params);
    
    // 3. Commit the metadata directly to your TOON DAG
    dag::commit("model_registry", "{\n  \"format\": \"GGUF_Q4\"\n}", []);
}
```

## Getting Started

Axi comes with a globally bound CLI to manage your projects and your TOON DAG.

```bash
# Initialize a new Axi project and local DAG ledger
axi init

# Tell the DAG which directories to track
axi track C:\MyProject\Backend

# Snapshot your uncommitted AST deltas to the TOON ledger
axi wrap
```

## Architecture
This repository contains the open-source compiler, standard library (`core::`), and the CLI wrapper. It is physically separated from proprietary implementations to ensure a clean, agnostic environment for external developers.

**Join the movement. Let's build the future of AI infrastructure natively.**
